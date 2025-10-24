#ifndef _EXSLERATE_DRV_H_
#define _EXSLERATE_DRV_H_

#include <drm/drm_drv.h>
#include <drm/drm_print.h>
#include <linux/cdev.h>
#include <linux/clk.h>
#include <linux/completion.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/kref.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>
#include <linux/types.h>

#include "exslerate_ioctl.h"

#define DRIVER_NAME "exslerate"
#define WDMA_OFFSET 0x30040000
#define DDR_INIT 0x30000000

/* Task information for ExSLerate operations */
struct exslerate_task {
  struct kref ref;
  uint32_t num_addresses;
  struct exslerate_device *exsl_dev;
  struct exslerate_mem_handle *address_list;
  struct drm_file *file;
};

/* Memory handle for DMA operations */
struct exslerate_mem_handle {
  uint32_t handle;
  uint64_t offset;
};

struct exslerate_device {
  struct platform_device *pdev;
  void __iomem *base;
  void __iomem *rdma_base;
  void __iomem *wdma_base;
  struct clk *axi_clk;
  struct drm_device *drm;
  struct exslerate_task *task;
  struct exsl_write_config_args conv_config;
  uint32_t core_enabled;
  spinlock_t status_lock;
  struct cdev cdev;
  dev_t devt;
  struct class *class;
  struct device *dev;
};

/* Common helper functions */
static inline void reg_write(struct exslerate_device *dev, uint32_t offset,
                             uint32_t value) {
  writel(value, dev->base + offset);
}

static inline uint32_t reg_read(struct exslerate_device *dev, uint32_t offset) {
  uint32_t value = readl(dev->base + offset);
  return value;
}

/* Engine function declarations */
int program_conv_core(struct exslerate_device *dev);
int program_gemm_core(struct exslerate_device *dev);

#endif /* _EXSLERATE_DRV_H_ */