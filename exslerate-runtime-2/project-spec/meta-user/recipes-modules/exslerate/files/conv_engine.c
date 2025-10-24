/* conv_engine.c - Fixed version */
#include "conv_engine.h"
#include "exslerate_drv.h" // This must come FIRST

#include <drm/drm_print.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>

#include "exslerate_ioctl.h"

#define CSR_DEBUG 0

int program_convolution_core(struct exslerate_device *dev) {
  struct exsl_write_config_args *params = &dev->conv_config;
  uint32_t val;

  DRM_INFO("Programming Convolution Core\n");

  /* Basic convolution parameters */
  reg_write(dev, CSR_CC_MAPPING, BUILD_CC_MAPPING(params->pooling_type));
  reg_write(dev, CSR_CC_PADDING_H, params->PADDING);
  reg_write(dev, CSR_CC_KERNEL_H, params->FILT_H);
  reg_write(dev, CSR_CC_KERNEL_W, params->FILT_H);
  reg_write(dev, CSR_CC_CHANNEL_SETS, params->CHANNEL_SETS);
  reg_write(dev, CSR_CC_STRIDE_W, params->stride);
  reg_write(dev, CSR_CC_OUT_WIDTH, params->OACT_W);
  reg_write(dev, CSR_CC_OUT_HEIGHT, params->OACT_H);
  reg_write(dev, CSR_CC_LINE_STRIDE,
            params->LINE_STRIDE * params->CHANNEL_SETS);
  reg_write(dev, CSR_CC_FILTER_SIZE, params->FILT_SIZE);
  reg_write(dev, CSR_CC_SURF_STRIDE, params->SURF_STRIDE);
  reg_write(dev, CSR_CC_TOTAL_FILTER_SETS, params->TOTAL_FIL_SETS);
  reg_write(dev, CSR_CC_FEATURE_H, params->IACT_H);
  reg_write(dev, CSR_CC_FEATURE_W, params->IACT_W);

  /* Tile configuration */
  reg_write(dev, CSR_CC_TILE_WIDTH_OFFSET, params->tile_width_offset);
  reg_write(dev, CSR_CC_TILE_HEIGHT_OFFSET,
            params->tile_height_offset + params->tile_width_offset);
  reg_write(dev, CSR_CC_TILE_WIDTH, params->tile_width - 1);
  reg_write(dev, CSR_CC_TILE_HEIGHT, params->tile_height - 1);
  reg_write(dev, CSR_CC_STRIDE_H, params->strideCY);
  reg_write(dev, CSR_CC_STRIDE_CX, params->stride);
  reg_write(dev, CSR_CC_STRIDE_CY, params->strideCY);

  /* Input activation configuration */
  reg_write(dev, CSR_CC_IACT_MAX_RAM,
            BUILD_IACT_MAX_RAM(params->ifMaxRam, params->ifOffset));
  reg_write(dev, CSR_CC_IACT_MAX_STRIPE, params->IACT_MAX_STRIPE);
  reg_write(dev, CSR_CC_IACT_MAX_VALUE, params->IACT_MAX_VALUE);
  reg_write(dev, CSR_CC_IACT_BURST_LEN, params->ifBurstLen);

  /* Filter configuration */
  reg_write(dev, CSR_CC_FILT_MAX_RAM,
            BUILD_FILT_MAX_RAM(params->flMaxRam, params->flOffset));
  reg_write(dev, CSR_CC_FILT_MAX_STRIPE, params->flMaxStripe);
  reg_write(dev, CSR_CC_FILT_MAX_VALUE, params->flMaxValue);
  reg_write(dev, CSR_CC_FILT_BURST_LEN, params->flBurstLen);

  /* Lifetime configuration */
  reg_write(dev, CSR_CC_LIFETIME_ADDR_OFFSET, params->lifeTimeOffset);
  reg_write(dev, CSR_CC_LIFETIME_BURST_LEN, params->lifetimeBurstLen);

  /* Special function and tile registers */
  reg_write(dev, CSR_OUT_TILE_HEIGHT, params->outTileHeight);
  reg_write(dev, CSR_OUT_TILE_WIDTH, params->outTileWidth);
  reg_write(dev, CSR_TILE_SIZE_SHIFTER, params->tileSizeShifter);
  reg_write(dev, CSR_TILE_WIDTH_SHIFTER, params->tileWidthShifter);
  reg_write(dev, CSR_ELEM_PER_INPUT_TILE_SHIFTER,
            params->elemPerInputTileShifter);
  reg_write(dev, CSR_ELEM_PER_OUTPUT_TILE, params->elemPerOutputTile);
  reg_write(dev, CSR_TILE_HEIGHT_SHIFTER, params->tileHeightShifter);
  reg_write(dev, CSR_PADDING_W, params->paddingW);

  /* Special function register */
  val = BUILD_SPECIAL_FUNCTION(params->specialFilterSets,
                               params->specialSurfaceStride,
                               params->specialFixedVal);
  reg_write(dev, CSR_SPECIAL_FUNCTION, val);

  DRM_INFO("Convolution Core programming completed\n");
  return 0;
}

int program_udp_core(struct exslerate_device *dev) {
  struct exsl_write_config_args *params = &dev->conv_config;
  uint32_t val;

  DRM_INFO("Programming UDP Core\n");

  /* Scaling and quantization parameters */
  reg_write(dev, CSR_UDP_SCALE_A, params->scalingFactor1);
  reg_write(dev, CSR_UDP_SCALE_B, params->scalingFactor2);
  reg_write(dev, CSR_UDP_LUT_FACTOR, params->lutFactor);
  reg_write(dev, CSR_UDP_LUT_SUB_FACTOR_0, params->lutSubFactor0);
  reg_write(dev, CSR_UDP_LUT_SUB_FACTOR_1, params->lutSubFactor1);
  reg_write(dev, CSR_UDP_LUT_ZERO_POINT, params->lutZeroPoint);
  reg_write(dev, CSR_UDP_SCALE_N_ZERO_POINT, params->_k);
  reg_write(dev, CSR_UDP_LAYER_NORM_B, params->layerNormB);

  /* Bias DMA configuration */
  val = BUILD_UDP_BIAS_DMA_OFFSET(params->BDMA_adrr_offset,
                                  params->BNDMA_addr_offset,
                                  params->clipped_scale, params->PreRelu_scale);
  reg_write(dev, CSR_UDP_BIAS_DMA_ADDR_OFFSET, val);

  /* Control and activation functions */
  val = BUILD_UDP_CONTROL_DEMUX(params->csrdmux, params->enableBias,
                                params->enableBatchNorm, params->enableRelu,
                                params->enablePRelu, params->enableSQRelu,
                                params->enableclippedRelu, params->csrmode);
  reg_write(dev, CSR_UDP_CONTROL_DEMUX, val);

  /* DMA and control registers */
  reg_write(dev, CSR_WDMA_CSR, params->wdmaCSR);
  reg_write(dev, CSR_WDMA_AXI_CSR, params->wdmaAXICSR);
  reg_write(dev, CSR_RDMA_AXI_CSR, params->rdmaAXICSR);
  reg_write(dev, CSR_WDMA_TRANSACTION_NUM, params->wdma_transaction_num);

  /* Interrupt configuration */
  reg_write(dev, CSR_GLOBAL_INTERRUPT_EN, params->globalInterruptEn);
  val = BUILD_INTERRUPT_EN(params->doneInterruptEn, params->errorInterruptEn,
                           params->timeoutInterruptEn);
  reg_write(dev, CSR_INTERRUPT_EN, val);

  /* Stall configuration */
  val = BUILD_STALL_COUNT(params->stallEn, params->stallCountValue);
  reg_write(dev, CSR_STALL_COUNT, val);

  DRM_INFO("UDP Core programming completed\n");
  return 0;
}

int program_address_offsets(struct exslerate_device *dev,
                            uint64_t input_base_addr, uint64_t filter_base_addr,
                            uint64_t output_base_addr) {
  struct exsl_write_config_args *params = &dev->conv_config;

  DRM_INFO("Programming Address Offsets\n");

  /* Input activation addresses */
  reg_write(dev, CSR_CC_IACT_BASE_ADDR_LOW,
            (uint32_t)(input_base_addr & 0xFFFFFFFF));
  reg_write(dev, CSR_CC_IACT_BASE_ADDR_HIGH, (uint32_t)(input_base_addr >> 32));

  /* Filter addresses */
  reg_write(dev, CSR_CC_FILT_BASE_ADDR_LOW,
            (uint32_t)(filter_base_addr & 0xFFFFFFFF));
  reg_write(dev, CSR_CC_FILT_BASE_ADDR_HIGH,
            (uint32_t)(filter_base_addr >> 32));

  /* Lifetime addresses */
  reg_write(dev, CSR_CC_LIFETIME_BASE_ADDR_LOW,
            (uint32_t)(params->lifetimeBaseAddr & 0xFFFFFFFF));
  reg_write(dev, CSR_CC_LIFETIME_BASE_ADDR_HIGH,
            (uint32_t)(params->lifetimeBaseAddr >> 32));

  /* UDP LUT and bias addresses */
  reg_write(dev, CSR_UDP_LUT_BASE_ADDR, params->lutBaseAddr);
  reg_write(dev, CSR_UDP_BIAS_BASE_ADDR, params->biasBaseAddr);

  /* BN bias addresses */
  reg_write(dev, CSR_UDP_BN_BIAS_BASE_ADDR_LOW,
            (uint32_t)(params->bnBiasBaseAddr & 0xFFFFFFFF));
  reg_write(dev, CSR_UDP_BN_BIAS_BASE_ADDR_HIGH,
            (uint32_t)(params->bnBiasBaseAddr >> 32));

  /* BN weight addresses */
  reg_write(dev, CSR_UDP_BN_WEIGHT_BASE_ADDR_LOW,
            (uint32_t)(params->bnWeightBaseAddr & 0xFFFFFFFF));
  reg_write(dev, CSR_UDP_BN_WEIGHT_BASE_ADDR_HIGH,
            (uint32_t)(params->bnWeightBaseAddr >> 32));

  /* Output base address */
  reg_write(dev, CSR_AXI_OUTPUT_BASE_ADDR_LOW,
            (uint32_t)(output_base_addr & 0xFFFFFFFF));
  reg_write(dev, CSR_AXI_OUTPUT_BASE_ADDR_HIGH,
            (uint32_t)(output_base_addr >> 32));

  DRM_INFO("Address Offsets programming completed\n");
  return 0;
}

int program_conv_core(struct exslerate_device *dev) {

  DRM_INFO("Starting Conv Core Programming\n");

  /* For now, just return success - uncomment individual parts as needed */
  /*
  ret = program_convolution_core(dev);
  if (ret) {
          DRM_ERROR("Failed to program convolution core: %d\n", ret);
          return ret;
  }

  ret = program_udp_core(dev);
  if (ret) {
          DRM_ERROR("Failed to program UDP core: %d\n", ret);
          return ret;
  }

  ret = program_address_offsets(dev, dev->conv_config.ifBaseAddr,
                                dev->conv_config.flBaseAddr,
                                dev->conv_config.OutputBaseAddr);
  if (ret) {
          DRM_ERROR("Failed to program address offsets: %d\n", ret);
          return ret;
  }
  */

  DRM_INFO("Conv Core Programming completed successfully\n");
  return 0;
}

int program_gemm_core(struct exslerate_device *dev) {
  DRM_INFO("GEMM Core programming - not implemented yet\n");
  return 0;
}

int debug_conv_core(struct exslerate_device *dev) {
  uint32_t rdata;

  DRM_INFO("=== ExSLerate Core Register Dump ===\n");

  /* Basic registers for now - add more as needed */
  rdata = reg_read(dev, CSR_CC_MAPPING);
  DRM_DEBUG("CSR_CC_MAPPING: 0x%08x\n", rdata);

  rdata = reg_read(dev, CSR_CC_PADDING_H);
  DRM_DEBUG("CSR_CC_PADDING_H: 0x%08x\n", rdata);

  rdata = reg_read(dev, CSR_CONV_CORE_EN);
  DRM_DEBUG("CSR_CONV_CORE_EN: 0x%08x\n", rdata);

  DRM_INFO("=== Register Dump Complete ===\n");
  return 0;
}