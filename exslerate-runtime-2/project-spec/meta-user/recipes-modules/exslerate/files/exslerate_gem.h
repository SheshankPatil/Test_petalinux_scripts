#ifndef _EXSLERATE_GEM_H_
#define _EXSLERATE_GEM_H_

#include <drm/drm_file.h>
#include <drm/drm_gem.h>
#include <drm/drm_gem_cma_helper.h>
#include <linux/dma-buf.h>

struct exslerate_device;
struct exslerate_client;

struct exslerate_mem {
  u64 userptr;
  void *kva;
  u64 dev_addr;
  size_t size;
  struct page **pages;
  u32 nr_pages;
};

#define BO_SUBMIT_PINNED BIT(0)
#define BO_SUBMIT_LOCKED BIT(1)

struct exslerate_gem_obj {
  struct drm_gem_cma_object base;
  // struct exslerate_client		*client;
  u8 type;
  u64 flags;
  struct mutex lock;
  struct exslerate_mem mem;
};

static inline struct exslerate_gem_obj *
to_exsl_obj(struct drm_gem_object *obj) {
  return container_of(obj, struct exslerate_gem_obj, base.base);
}

#define to_gobj(obj) (&(obj)->base.base)

struct drm_gem_object *exslerate_gem_create_object_cb(struct drm_device *dev,
                                                      size_t size);
int32_t exslerate_drm_probe(struct exslerate_device *exsl_dev);
void exslerate_drm_remove(struct exslerate_device *exsl_dev);

#endif /* _EXSLERATE_GEM_H_ */