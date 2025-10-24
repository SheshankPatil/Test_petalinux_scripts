#include "exslerate_drv.h"

#include "exslerate_gem.h"
#include "exslerate_ioctl.h"
#include <drm/drm_drv.h>
#include <drm/drm_print.h>
#include <linux/clk.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_reserved_mem.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

/* Device tree matching table */
static const struct of_device_id exslerate_of_match[] = {
    {.compatible = "xlnx,top-1.0"},
    {},
};
// static int32_t exslerate_probe(struct platform_device *pdev) {
//   int32_t err = 0;
//   struct resource *res;
//   struct exslerate_device *exsl_dev;
//   struct device *dev = &pdev->dev;

//   dev_info(&pdev->dev, "Probing ExSLerate device\n");

//   if (!pdev->dev.of_node) {
//       dev_err(&pdev->dev, "Missing device tree node\n");
//       return -EINVAL;
//   }

//   if (!of_match_device(exslerate_of_match, &pdev->dev)) {
//       dev_err(&pdev->dev, "Missing DT entry!\n");
//       return -EINVAL;
//   }

//   exsl_dev = devm_kzalloc(dev, sizeof(*exsl_dev), GFP_KERNEL);
//   if (!exsl_dev) return -ENOMEM;

//   platform_set_drvdata(pdev, exsl_dev);
//   exsl_dev->pdev = pdev;

//   /* Initialize spinlock */
//   spin_lock_init(&exsl_dev->status_lock);

//   /* Get memory resource */
//   res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
//   if (!res) {
//       dev_err(&pdev->dev, "No memory resource found\n");
//       return -ENODEV;
//   }

//   exsl_dev->base = devm_ioremap_resource(&pdev->dev, res);
//   if (IS_ERR(exsl_dev->base)) {
//       dev_err(&pdev->dev, "Failed to map memory resource\n");
//       return PTR_ERR(exsl_dev->base);
//   }

//   /* Initialize reserved memory for DMA (key addition) */
//   err = of_reserved_mem_device_init(&pdev->dev);
//   if (err) {
//       dev_err(&pdev->dev, "Failed to initialize reserved memory: %d\n", err);
//       return err;  /* Fail probe if reserved memory init fails */
//   }
//   dev_info(&pdev->dev, "Reserved memory initialized for DMA allocations\n");

//   /* Get clock resource (optional) */
//   exsl_dev->axi_clk = devm_clk_get(&pdev->dev, "axi_aclk");
//   if (IS_ERR(exsl_dev->axi_clk)) {
//       dev_warn(&pdev->dev, "Failed to get AXI clock, continuing without clock
//       management\n"); exsl_dev->axi_clk = NULL;
//   } else {
//       err = clk_prepare_enable(exsl_dev->axi_clk);
//       if (err) {
//           dev_err(&pdev->dev, "Failed to enable AXI clock\n");
//           goto err_release_mem;
//       }
//   }

//   /* Initialize device parameters with defaults */
//   memset(&exsl_dev->conv_config, 0, sizeof(exsl_dev->conv_config));
//   exsl_dev->core_enabled = 0;

//   /* Register DRM device */
//   err = exslerate_drm_probe(exsl_dev);
//   if (err) {
//       dev_err(&pdev->dev, "Failed to register DRM device\n");
//       goto err_clk_disable;
//   }

//   dev_info(&pdev->dev, "ExSLerate driver initialized successfully\n");
//   return 0;

// err_clk_disable:
//   if (exsl_dev->axi_clk) clk_disable_unprepare(exsl_dev->axi_clk);
// err_release_mem:
//   of_reserved_mem_device_release(&pdev->dev);  /* Clean up reserved memory */
//   return err;
// }

MODULE_DEVICE_TABLE(of, exslerate_of_match);

static int32_t exslerate_remove(struct platform_device *pdev) {
  struct exslerate_device *exsl_dev = platform_get_drvdata(pdev);

  dev_info(&pdev->dev, "Removing ExSLerate driver\n");

  /* Remove DRM device first */
  exslerate_drm_remove(exsl_dev);

  /* Disable clock if it was enabled */
  if (exsl_dev->axi_clk) {
    clk_disable_unprepare(exsl_dev->axi_clk);
    dev_info(&pdev->dev, "AXI clock disabled\n");
  }

  /* Release reserved memory */
  of_reserved_mem_device_release(&pdev->dev);

  dev_info(&pdev->dev, "ExSLerate driver removed successfully\n");
  return 0;
}
static int32_t exslerate_probe(struct platform_device *pdev) {
  int32_t err = 0;
  struct resource *res;
  struct exslerate_device *exsl_dev;
  struct device *dev = &pdev->dev;

  dev_info(dev, "Probing ExSLerate device\n");

  if (!pdev->dev.of_node) {
    dev_err(dev, "Missing device tree node\n");
    return -EINVAL;
  }

  if (!of_match_device(exslerate_of_match, dev)) {
    dev_err(dev, "No matching device tree entry\n");
    return -EINVAL;
  }

  exsl_dev = devm_kzalloc(dev, sizeof(*exsl_dev), GFP_KERNEL);
  if (!exsl_dev)
    return -ENOMEM;

  platform_set_drvdata(pdev, exsl_dev);
  exsl_dev->pdev = pdev;

  /* Initialize spinlock */
  spin_lock_init(&exsl_dev->status_lock);

  /* Get memory resource */
  res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
  if (!res) {
    dev_err(dev, "No memory resource found\n");
    return -ENODEV;
  }

  exsl_dev->base = devm_ioremap_resource(dev, res);
  if (IS_ERR(exsl_dev->base)) {
    dev_err(dev, "Failed to map memory resource: %ld\n",
            PTR_ERR(exsl_dev->base));
    return PTR_ERR(exsl_dev->base);
  }

  /* Initialize reserved memory for DMA */
  err = of_reserved_mem_device_init(dev);
  if (err) {
    dev_err(dev, "Failed to initialize reserved memory: %d\n", err);
    return err; /* Fail probe if reserved memory init fails */
  }
  dev_info(dev, "Reserved memory initialized for DMA allocations\n");

  /* Get clock resource (optional) */
  exsl_dev->axi_clk = devm_clk_get(dev, "axi_aclk");
  if (IS_ERR(exsl_dev->axi_clk)) {
    dev_warn(dev,
             "Failed to get AXI clock, continuing without clock management\n");
    exsl_dev->axi_clk = NULL;
  } else {
    err = clk_prepare_enable(exsl_dev->axi_clk);
    if (err) {
      dev_err(dev, "Failed to enable AXI clock: %d\n", err);
      goto err_release_mem;
    }
  }

  /* Initialize device parameters with defaults */
  memset(&exsl_dev->conv_config, 0, sizeof(exsl_dev->conv_config));
  exsl_dev->core_enabled = 0;

  /* Register DRM device */
  err = exslerate_drm_probe(exsl_dev);
  if (err) {
    dev_err(dev, "Failed to register DRM device: %d\n", err);
    goto err_clk_disable;
  }

  dev_info(dev, "ExSLerate driver initialized successfully\n");
  return 0;

err_clk_disable:
  if (exsl_dev->axi_clk)
    clk_disable_unprepare(exsl_dev->axi_clk);
err_release_mem:
  of_reserved_mem_device_release(dev);
  return err;
}

// static int32_t exslerate_remove(struct platform_device *pdev) {
//   struct exslerate_device *exsl_dev = platform_get_drvdata(pdev);

//   /* Remove DRM device */
//   exslerate_drm_remove(exsl_dev);

//   /* Disable clock if it was enabled */
//   if (exsl_dev->axi_clk) clk_disable_unprepare(exsl_dev->axi_clk);

//   /* Release reserved memory */
//   of_reserved_mem_device_release(&pdev->dev);

//   dev_info(&pdev->dev, "ExSLerate driver removed\n");
//   return 0;
// }

static struct platform_driver exslerate_platform_driver = {
    .probe = exslerate_probe,
    .remove = exslerate_remove,
    .driver =
        {
            .owner = THIS_MODULE,
            .name = DRIVER_NAME,
            .of_match_table = of_match_ptr(exslerate_of_match),
        },
};

module_platform_driver(exslerate_platform_driver);

MODULE_DESCRIPTION("SandLogic ExSLerate AI Chip Driver");
MODULE_IMPORT_NS(DMA_BUF);
MODULE_LICENSE("Dual BSD/GPL");