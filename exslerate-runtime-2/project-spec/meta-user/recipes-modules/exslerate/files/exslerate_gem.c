/* exslerate_gem.c - ExSLerate GEM buffer management */
#include <drm/drm.h>
#include <drm/drm_device.h>
#include <drm/drm_drv.h>
#include <drm/drm_file.h>
#include <drm/drm_gem.h>
#include <drm/drm_gem_cma_helper.h>
#include <drm/drm_ioctl.h>
#include <linux/dma-buf.h>
#include <linux/dma-mapping.h>

#include "exslerate_drv.h"
#include "exslerate_gem.h"
#include "exslerate_ioctl.h"

#define EXSLERATE_BO_SHARE 1
#define EXSLERATE_BO_CMD 2
#define EXSLERATE_BO_DMA 3
#define EXSLERATE_BO_DEV 4

#define EXSLERATE_INVALID_ADDR (0)

static void exslerate_gem_free_object(struct drm_gem_object *gobj) {
  struct exslerate_gem_obj *abo = to_exsl_obj(gobj);

  if (abo->mem.pages) {
    u32 i;
    for (i = 0; i < abo->mem.nr_pages; i++) {
      if (abo->mem.pages[i])
        put_page(abo->mem.pages[i]);
    }
    kvfree(abo->mem.pages);
  }

  mutex_destroy(&abo->lock);
  drm_gem_cma_free_object(gobj);
}

static void exslerate_gem_print_info(struct drm_printer *p, unsigned int indent,
                                     const struct drm_gem_object *gobj) {
  drm_gem_cma_print_info(p, indent, gobj);
}

static const struct drm_gem_object_funcs exslerate_gem_cma_funcs = {
    .free = exslerate_gem_free_object,
    .print_info = exslerate_gem_print_info,
    .vm_ops = &drm_gem_cma_vm_ops,
};

struct drm_gem_object *exslerate_gem_create_object_cb(struct drm_device *dev,
                                                      size_t size) {
  struct exslerate_gem_obj *abo;

  abo = kzalloc(sizeof(*abo), GFP_KERNEL);
  if (!abo)
    return ERR_PTR(-ENOMEM);

  to_gobj(abo)->funcs = &exslerate_gem_cma_funcs;
  abo->type = EXSLERATE_BO_SHARE;
  mutex_init(&abo->lock);

  drm_gem_object_init(dev, to_gobj(abo), size);

  abo->mem.userptr = EXSLERATE_INVALID_ADDR;
  abo->mem.dev_addr = EXSLERATE_INVALID_ADDR;
  abo->mem.size = size;

  return to_gobj(abo);
}

static struct exslerate_gem_obj *
exslerate_drm_create_cma_bo(struct drm_device *dev,
                            struct exsl_drm_create_bo *args,
                            struct drm_file *file) {
  struct drm_gem_cma_object *cma;
  struct exslerate_gem_obj *abo;
  size_t size = args->size;

  /* Round up small sizes to ensure CMA allocation */
  if (size <= PAGE_SIZE)
    size = round_up(size, 2 * PAGE_SIZE);

  cma = drm_gem_cma_create(dev, size);
  if (IS_ERR(cma))
    return ERR_CAST(cma);

  abo = to_exsl_obj(&cma->base);
  abo->mem.dev_addr =
      cma->paddr; // This is correct - storing physical address in dev_addr
  abo->mem.kva = cma->vaddr; // Kernel virtual address
  abo->type = args->type;

  // Fixed printk format - use %p for void* and proper field names
  printk(KERN_ERR "cma->vaddr = %p, cma->paddr = 0x%llx, size = 0x%zx\n",
         cma->vaddr, (unsigned long long)cma->paddr, size);

  return abo;
}

static int32_t exsl_submit(struct drm_device *drm, void *arg,
                           struct drm_file *file) {
  struct exsl_submit_args *args = arg;

  DRM_INFO("Submit request: type=%u, cmd_count=%u\n", args->type,
           args->cmd_count);

  if (args->cmd_count == 0 || args->cmd_count > 16) {
    DRM_ERROR("Invalid number of handles: %u\n", args->cmd_count);
    return -EINVAL;
  }

  if (!args->cmd_handles) {
    DRM_ERROR("Invalid handle list pointer\n");
    return -EINVAL;
  }

  /* TODO: actual submission */
  return 0;
}

static int32_t exsl_create_bo(struct drm_device *dev, void *data,
                              struct drm_file *file) {
  struct exsl_drm_create_bo *args = data;
  struct exslerate_gem_obj *abo;
  int ret;

  if (args->flags || !args->size) {
    DRM_ERROR("Invalid BO args: flags=0x%llx, size=%llu\n", args->flags,
              args->size);
    return -EINVAL;
  }

  switch (args->type) {
  case EXSL_BO_SHARE:
  case EXSL_BO_CMD:
  case EXSL_BO_DMA:
  case EXSL_BO_DEV:
    abo = exslerate_drm_create_cma_bo(dev, args, file);
    break;
  default:
    return -EINVAL;
  }

  if (IS_ERR(abo))
    return PTR_ERR(abo);

  ret = drm_gem_handle_create(file, to_gobj(abo), &args->handle);
  if (ret) {
    DRM_ERROR("Failed to create handle: %d\n", ret);
    goto put_obj;
  }

  DRM_DEBUG("Created BO handle %d type %llu size 0x%zx dev_addr 0x%llx\n",
            args->handle, args->type, abo->mem.size, abo->mem.dev_addr);

put_obj:
  drm_gem_object_put(to_gobj(abo));
  return ret;
}

static int32_t exsl_gem_destroy(struct drm_device *drm, void *data,
                                struct drm_file *file) {
  struct exsl_gem_destroy_args *args = data;
  return drm_gem_handle_delete(file, args->handle);
}

static int32_t exsl_gem_map_offset(struct drm_device *drm, void *data,
                                   struct drm_file *file) {
  struct exsl_gem_map_offset_args *args = data;
  struct drm_gem_object *gobj;
  struct exslerate_gem_obj *abo;
  int ret;

  gobj = drm_gem_object_lookup(file, args->handle);
  if (!gobj) {
    DRM_ERROR("Failed to lookup GEM object %d\n", args->handle);
    return -ENOENT;
  }

  abo = to_exsl_obj(gobj);

  /* Create a mappable offset if it doesn't exist yet */
  ret = drm_gem_create_mmap_offset(gobj);
  if (ret) {
    DRM_ERROR("Failed to create mmap offset for handle %d\n", args->handle);
    drm_gem_object_put(gobj);
    return ret;
  }

  /* Get the correct addresses */
  args->map_offset = drm_vma_node_offset_addr(&gobj->vma_node);
  args->dev_addr = abo->mem.dev_addr;
  args->vaddr = (u64)abo->mem.kva;

  DRM_DEBUG("GEM handle %d: map_offset=0x%llx, dev_addr=0x%llx, vaddr=0x%llx\n",
            args->handle, args->map_offset, args->dev_addr, args->vaddr);

  drm_gem_object_put(gobj);
  return 0;
}

static int32_t exsl_write_config(struct drm_device *drm, void *data,
                                 struct drm_file *file) {
  struct exslerate_device *exsl_dev = drm->dev_private;
  struct exsl_write_config_args *args = data;

  memcpy(&exsl_dev->conv_config, args, sizeof(*args));
  DRM_DEBUG("Conv config written\n");

  return 0;
}

static int32_t exsl_read_status(struct drm_device *drm, void *data,
                                struct drm_file *file) {
  struct exslerate_device *exsl_dev = drm->dev_private;
  struct exsl_read_status_args *args = data;

  args->status_value = reg_read(exsl_dev, 0xD8);
  return 0;
}

static int32_t exsl_program_core(struct drm_device *drm, void *data,
                                 struct drm_file *file) {
  struct exslerate_device *exsl_dev = drm->dev_private;
  struct exsl_program_core_args *args = data;

  switch (args->core_type) {
  case EXSL_CONV_CORE:
    return program_conv_core(exsl_dev);
  case EXSL_GEMM_CORE:
    return program_gemm_core(exsl_dev);
  default:
    return -EINVAL;
  }
}

DEFINE_DRM_GEM_FOPS(exslerate_drm_fops);

static const struct drm_ioctl_desc exslerate_drm_ioctls[] = {
    DRM_IOCTL_DEF_DRV(EXSL_SUBMIT, exsl_submit, DRM_RENDER_ALLOW),
    DRM_IOCTL_DEF_DRV(EXSL_CREATE_BO, exsl_create_bo, DRM_RENDER_ALLOW),
    DRM_IOCTL_DEF_DRV(EXSL_GEM_MMAP, exsl_gem_map_offset, DRM_RENDER_ALLOW),
    DRM_IOCTL_DEF_DRV(EXSL_GEM_DESTROY, exsl_gem_destroy, DRM_RENDER_ALLOW),
    DRM_IOCTL_DEF_DRV(EXSL_WRITE_CONFIG, exsl_write_config, DRM_RENDER_ALLOW),
    DRM_IOCTL_DEF_DRV(EXSL_READ_STATUS, exsl_read_status, DRM_RENDER_ALLOW),
    DRM_IOCTL_DEF_DRV(EXSL_PROGRAM_CORE, exsl_program_core, DRM_RENDER_ALLOW),
};

static struct drm_driver exslerate_drm_driver = {
    .driver_features = DRIVER_GEM | DRIVER_RENDER,
    .gem_create_object = exslerate_gem_create_object_cb,
    .ioctls = exslerate_drm_ioctls,
    .num_ioctls = ARRAY_SIZE(exslerate_drm_ioctls),
    .fops = &exslerate_drm_fops,
    .name = "exslerate",
    .desc = "ExSLerate AI Chip Driver",
    .date = "20250627",
    .major = 1,
    .minor = 0,
    .patchlevel = 0,
};

int32_t exslerate_drm_probe(struct exslerate_device *exsl_dev) {
  struct drm_device *drm;
  int32_t err;

  drm = drm_dev_alloc(&exslerate_drm_driver, &exsl_dev->pdev->dev);
  if (IS_ERR(drm))
    return PTR_ERR(drm);

  exsl_dev->drm = drm;
  drm->dev_private = exsl_dev;

  err = drm_dev_register(drm, 0);
  if (err < 0) {
    drm_dev_put(drm);
    return err;
  }

  DRM_INFO("ExSLerate DRM registered\n");
  return 0;
}
EXPORT_SYMBOL(exslerate_drm_probe);

void exslerate_drm_remove(struct exslerate_device *exsl_dev) {
  if (exsl_dev->drm) {
    drm_dev_unregister(exsl_dev->drm);
    drm_dev_put(exsl_dev->drm);
    exsl_dev->drm = NULL;
  }
}
EXPORT_SYMBOL(exslerate_drm_remove);
