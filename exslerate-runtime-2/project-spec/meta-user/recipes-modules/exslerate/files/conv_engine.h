#ifndef _CONV_ENGINE_H_
#define _CONV_ENGINE_H_

#include "exslerate_drv.h"
#include <linux/types.h>

/* CSR Register Offsets */
#define CSR_CC_MAPPING 0x00000000            /* CC Config/Pooling */
#define CSR_CC_PADDING_H 0x00000004          /* Padding Height */
#define CSR_CC_KERNEL_H 0x00000008           /* Kernel Height */
#define CSR_CC_KERNEL_W 0x0000000C           /* Kernel Width */
#define CSR_CC_CHANNEL_SETS 0x00000010       /* Channel Sets */
#define CSR_CC_STRIDE_W 0x00000014           /* Stride Width */
#define CSR_CC_OUT_WIDTH 0x00000018          /* Output Width */
#define CSR_CC_OUT_HEIGHT 0x0000001C         /* Output Height */
#define CSR_CC_LINE_STRIDE 0x00000020        /* Line Stride */
#define CSR_CC_FILTER_SIZE 0x00000024        /* Filter Size */
#define CSR_CC_SURF_STRIDE 0x00000028        /* Surface Stride */
#define CSR_CC_TOTAL_FILTER_SETS 0x0000002C  /* Total Filter Sets */
#define CSR_CC_FEATURE_H 0x00000030          /* Feature Height */
#define CSR_CC_FEATURE_W 0x00000034          /* Feature Width */
#define CSR_CC_TILE_WIDTH_OFFSET 0x00000038  /* Tile Width Offset */
#define CSR_CC_TILE_HEIGHT_OFFSET 0x0000003C /* Tile Height Offset */
#define CSR_CC_TILE_WIDTH 0x00000040         /* Tile Width */
#define CSR_CC_TILE_HEIGHT 0x00000044        /* Tile Height */
#define CSR_CC_STRIDE_H 0x00000048           /* Stride Height */
#define CSR_CC_STRIDE_CX 0x0000004C          /* Stride CX */
#define CSR_CC_STRIDE_CY 0x00000050          /* Stride CY */
#define CSR_CC_IACT_MAX_RAM 0x00000054       /* Input Activation Max RAM */
#define CSR_CC_IACT_BASE_ADDR_LOW                                              \
  0x00000058 /* Input Activation Base Address Low */
#define CSR_CC_IACT_MAX_STRIPE 0x0000005C    /* Input Activation Max Stripe */
#define CSR_CC_IACT_MAX_VALUE 0x00000060     /* Input Activation Max Value */
#define CSR_CC_IACT_BURST_LEN 0x00000064     /* Input Activation Burst Length */
#define CSR_CC_FILT_MAX_RAM 0x00000068       /* Filter Max RAM */
#define CSR_CC_FILT_BASE_ADDR_LOW 0x0000006C /* Filter Base Address Low */
#define CSR_CC_FILT_MAX_STRIPE 0x00000070    /* Filter Max Stripe */
#define CSR_CC_FILT_MAX_VALUE 0x00000074     /* Filter Max Value */
#define CSR_CC_FILT_BURST_LEN 0x00000078     /* Filter Burst Length */
#define CSR_CC_LIFETIME_ADDR_OFFSET 0x0000007C /* Lifetime Address Offset */
#define CSR_CC_LIFETIME_BASE_ADDR_LOW                                          \
  0x00000080                                 /* Lifetime Base Address Low      \
                                              */
#define CSR_CC_LIFETIME_BURST_LEN 0x00000084 /* Lifetime Burst Length */

/* SDP (UDP) CSR Offsets */
#define CSR_UDP_SCALE_A 0x00000088              /* Scale A */
#define CSR_UDP_SCALE_B 0x0000008C              /* Scale B */
#define CSR_UDP_LUT_FACTOR 0x00000090           /* LUT Factor */
#define CSR_UDP_LUT_SUB_FACTOR_0 0x00000094     /* LUT Sub Factor 0 */
#define CSR_UDP_LUT_SUB_FACTOR_1 0x00000098     /* LUT Sub Factor 1 */
#define CSR_UDP_LUT_ZERO_POINT 0x0000009C       /* LUT Zero Point */
#define CSR_UDP_SCALE_N_ZERO_POINT 0x000000A0   /* Scale N Zero Point */
#define CSR_UDP_LAYER_NORM_B 0x000000A4         /* Layer Norm B */
#define CSR_UDP_BIAS_DMA_ADDR_OFFSET 0x000000A8 /* Bias DMA Address Offset */
#define CSR_UDP_LUT_BASE_ADDR 0x000000AC        /* LUT Base Address */
#define CSR_UDP_BIAS_BASE_ADDR 0x000000B0       /* Bias Base Address */
#define CSR_UDP_BN_BIAS_BASE_ADDR_LOW                                          \
  0x000000B4 /* BN Bias Base Address Low                                       \
              */
#define CSR_UDP_BN_WEIGHT_BASE_ADDR_LOW                                        \
  0x000000B8                             /* BN Weight Base Address Low */
#define CSR_UDP_CONTROL_DEMUX 0x000000BC /* Control Demux */

/* DMA and AXI CSR Offsets */
#define CSR_AXI_OUTPUT_BASE_ADDR_LOW 0x000000C0 /* Output Base Address */
#define CSR_WDMA_CSR 0x000000C4                 /* WDMA Control */
#define CSR_WDMA_AXI_CSR 0x000000C8             /* WDMA AXI Control */
#define CSR_RDMA_AXI_CSR 0x000000CC             /* RDMA AXI Control */
#define CSR_CONV_CORE_EN 0x000000D0             /* Conv Core Enable */
#define CSR_WDMA_TRANSACTION_NUM 0x000000D4     /* WDMA Transaction Number */

/* High Address Registers */
#define CSR_CC_IACT_BASE_ADDR_HIGH                                             \
  0x000000DC /* Input Activation Base Address High */
#define CSR_CC_FILT_BASE_ADDR_HIGH 0x000000E0 /* Filter Base Address High */
#define CSR_CC_LIFETIME_BASE_ADDR_HIGH                                         \
  0x000000E4 /* Lifetime Base Address High */
#define CSR_UDP_BN_BIAS_BASE_ADDR_HIGH                                         \
  0x000000F0 /* BN Bias Base Address High */
#define CSR_UDP_BN_WEIGHT_BASE_ADDR_HIGH                                       \
  0x000000F4 /* BN Weight Base Address High */
#define CSR_AXI_OUTPUT_BASE_ADDR_HIGH                                          \
  0x000000F8 /* Output Base Address High                                       \
              */

/* Additional CSR Offsets */
#define CSR_OUT_TILE_HEIGHT 0x00000100    /* Output Tile Height */
#define CSR_OUT_TILE_WIDTH 0x00000104     /* Output Tile Width */
#define CSR_TILE_SIZE_SHIFTER 0x00000108  /* Tile Size Shifter */
#define CSR_TILE_WIDTH_SHIFTER 0x0000010C /* Tile Width Shifter */
#define CSR_ELEM_PER_INPUT_TILE_SHIFTER                                        \
  0x00000110 /* Elements per Input Tile Shifter */
#define CSR_ELEM_PER_OUTPUT_TILE 0x00000114 /* Elements per Output Tile */
#define CSR_TILE_HEIGHT_SHIFTER 0x00000118  /* Tile Height Shifter */
#define CSR_PADDING_W 0x0000011C            /* Padding Width */

/* Interrupt and Control CSRs */
#define CSR_STALL_COUNT 0x00000128         /* Stall Count */
#define CSR_GLOBAL_INTERRUPT_EN 0x0000012C /* Global Interrupt Enable */
#define CSR_INTERRUPT_EN 0x00000130        /* Interrupt Enable */

/* Special Function CSR */
#define CSR_SPECIAL_FUNCTION 0x000000FC /* Special Function Register */
/* Basic bit manipulation macros */
#define GEN_MASK(width) ((1U << (width)) - 1)
#define SET_BITS(val, shift, width) (((val) & GEN_MASK(width)) << (shift))
#define GET_BITS(reg, shift, width) (((reg) >> (shift)) & GEN_MASK(width))

/* CC_MAPPING register bit fields */
#define CC_MAPPING_DATA_MAP_SHIFT 1
#define CC_MAPPING_POOLING_SHIFT 3
#define SET_CC_POOLING(val) SET_BITS(val, CC_MAPPING_POOLING_SHIFT, 2)

/* CC_IACT_MAX_RAM register bit fields */
#define CC_IACT_RAM_SHIFT 0
#define CC_IACT_OFFSET_SHIFT 4
#define SET_IACT_RAM(val) SET_BITS(val, CC_IACT_RAM_SHIFT, 4)
#define SET_IACT_OFFSET(val) SET_BITS(val, CC_IACT_OFFSET_SHIFT, 5)

/* CC_FILT_MAX_RAM register bit fields */
#define CC_FILT_RAM_SHIFT 0
#define CC_FILT_OFFSET_SHIFT 4
#define SET_FILT_RAM(val) SET_BITS(val, CC_FILT_RAM_SHIFT, 4)
#define SET_FILT_OFFSET(val) SET_BITS(val, CC_FILT_OFFSET_SHIFT, 5)

/* UDP_BIAS_DMA_ADDR_OFFSET register bit fields */
#define UDP_BIAS_DMA_OFFSET_SHIFT 0
#define UDP_BN_DMA_OFFSET_SHIFT 5
#define UDP_CLIPPED_SCALE_SHIFT 10
#define UDP_PRELU_SCALE_SHIFT 18
#define SET_BIAS_DMA_OFFSET(val) SET_BITS(val, UDP_BIAS_DMA_OFFSET_SHIFT, 5)
#define SET_BN_DMA_OFFSET(val) SET_BITS(val, UDP_BN_DMA_OFFSET_SHIFT, 5)
#define SET_CLIPPED_SCALE(val) SET_BITS(val, UDP_CLIPPED_SCALE_SHIFT, 8)
#define SET_PRELU_SCALE(val) SET_BITS(val, UDP_PRELU_SCALE_SHIFT, 8)

/* UDP_CONTROL_DEMUX register bit fields */
#define UDP_DEMUX_SHIFT 0
#define UDP_BIAS_EN_SHIFT 2
#define UDP_BN_EN_SHIFT 3
#define UDP_TRANSFORMER_BIAS_SHIFT 4
#define UDP_LN_EN_SHIFT 5
#define UDP_RELU_EN_SHIFT 6
#define UDP_PRELU_EN_SHIFT 7
#define UDP_SR_EN_SHIFT 8
#define UDP_CR_EN_SHIFT 9
#define UDP_CSR_MODE_SHIFT 10
#define SET_UDP_DEMUX(val) SET_BITS(val, UDP_DEMUX_SHIFT, 2)
#define SET_UDP_BIAS_EN(val) SET_BITS(val, UDP_BIAS_EN_SHIFT, 1)
#define SET_UDP_BN_EN(val) SET_BITS(val, UDP_BN_EN_SHIFT, 1)
#define SET_UDP_TRANSFORMER_BIAS(val)                                          \
  SET_BITS(val, UDP_TRANSFORMER_BIAS_SHIFT, 1)
#define SET_UDP_LN_EN(val) SET_BITS(val, UDP_LN_EN_SHIFT, 1)
#define SET_UDP_RELU_EN(val) SET_BITS(val, UDP_RELU_EN_SHIFT, 1)
#define SET_UDP_PRELU_EN(val) SET_BITS(val, UDP_PRELU_EN_SHIFT, 1)
#define SET_UDP_SR_EN(val) SET_BITS(val, UDP_SR_EN_SHIFT, 1)
#define SET_UDP_CR_EN(val) SET_BITS(val, UDP_CR_EN_SHIFT, 1)
#define SET_UDP_CSR_MODE(val) SET_BITS(val, UDP_CSR_MODE_SHIFT, 3)

/* WDMA_CSR register bit fields */
#define WDMA_GO_SHIFT 0
#define WDMA_BURST_SIZE_SHIFT 1
#define WDMA_AXI_BURST_LEN_SHIFT 13
#define SET_WDMA_GO(val) SET_BITS(val, WDMA_GO_SHIFT, 1)
#define SET_WDMA_BURST_SIZE(val) SET_BITS(val, WDMA_BURST_SIZE_SHIFT, 12)
#define SET_WDMA_AXI_BURST_LEN(val) SET_BITS(val, WDMA_AXI_BURST_LEN_SHIFT, 9)

/* Special Function register bit fields */
#define SPECIAL_FUNC_FILTER_SETS_SHIFT 0
#define SPECIAL_FUNC_SURFACE_STRIDE_SHIFT 11
#define SPECIAL_FUNC_FIXED_VAL_SHIFT 24
#define SET_SPECIAL_FILTER_SETS(val)                                           \
  SET_BITS(val, SPECIAL_FUNC_FILTER_SETS_SHIFT, 11)
#define SET_SPECIAL_SURFACE_STRIDE(val)                                        \
  SET_BITS(val, SPECIAL_FUNC_SURFACE_STRIDE_SHIFT, 8)
#define SET_SPECIAL_FIXED_VAL(val)                                             \
  SET_BITS(val, SPECIAL_FUNC_FIXED_VAL_SHIFT, 8)

/* Stall Count register bit fields */
#define STALL_EN_SHIFT 0
#define STALL_COUNT_VALUE_SHIFT 1
#define SET_STALL_EN(val) SET_BITS(val, STALL_EN_SHIFT, 1)
#define SET_STALL_COUNT_VALUE(val) SET_BITS(val, STALL_COUNT_VALUE_SHIFT, 31)

/* Interrupt enable register bit fields */
#define INT_DONE_EN_SHIFT 0
#define INT_ERROR_EN_SHIFT 1
#define INT_TIMEOUT_EN_SHIFT 2
#define SET_INT_DONE_EN(val) SET_BITS(val, INT_DONE_EN_SHIFT, 1)
#define SET_INT_ERROR_EN(val) SET_BITS(val, INT_ERROR_EN_SHIFT, 1)
#define SET_INT_TIMEOUT_EN(val) SET_BITS(val, INT_TIMEOUT_EN_SHIFT, 1)

/* Multi-field register builders */
#define BUILD_CC_MAPPING(pooling) SET_CC_POOLING(pooling)

#define BUILD_IACT_MAX_RAM(max_ram, offset)                                    \
  (SET_IACT_RAM(max_ram) | SET_IACT_OFFSET(offset))

#define BUILD_FILT_MAX_RAM(max_ram, offset)                                    \
  (SET_FILT_RAM(max_ram) | SET_FILT_OFFSET(offset))

#define BUILD_UDP_BIAS_DMA_OFFSET(bias_offset, bn_offset, clipped_scale,       \
                                  prelu_scale)                                 \
  (SET_BIAS_DMA_OFFSET(bias_offset) | SET_BN_DMA_OFFSET(bn_offset) |           \
   SET_CLIPPED_SCALE(clipped_scale) | SET_PRELU_SCALE(prelu_scale))

#define BUILD_UDP_CONTROL_DEMUX(demux, bias_en, bn_en, relu_en, prelu_en,      \
                                sr_en, cr_en, csr_mode)                        \
  (SET_UDP_DEMUX(demux) | SET_UDP_BIAS_EN(bias_en) | SET_UDP_BN_EN(bn_en) |    \
   SET_UDP_RELU_EN(relu_en) | SET_UDP_PRELU_EN(prelu_en) |                     \
   SET_UDP_SR_EN(sr_en) | SET_UDP_CR_EN(cr_en) | SET_UDP_CSR_MODE(csr_mode))

#define BUILD_WDMA_CSR(go, burst_size, axi_burst_len)                          \
  (SET_WDMA_GO(go) | SET_WDMA_BURST_SIZE(burst_size) |                         \
   SET_WDMA_AXI_BURST_LEN(axi_burst_len))

#define BUILD_SPECIAL_FUNCTION(filter_sets, surface_stride, fixed_val)         \
  (SET_SPECIAL_FILTER_SETS(filter_sets) |                                      \
   SET_SPECIAL_SURFACE_STRIDE(surface_stride) |                                \
   SET_SPECIAL_FIXED_VAL(fixed_val))

#define BUILD_STALL_COUNT(enable, count_val)                                   \
  (SET_STALL_EN(enable) | SET_STALL_COUNT_VALUE(count_val))

#define BUILD_INTERRUPT_EN(done_en, error_en, timeout_en)                      \
  (SET_INT_DONE_EN(done_en) | SET_INT_ERROR_EN(error_en) |                     \
   SET_INT_TIMEOUT_EN(timeout_en))

/* Function declarations */
int program_convolution_core(struct exslerate_device *dev);
int program_udp_core(struct exslerate_device *dev);
int program_address_offsets(struct exslerate_device *dev,
                            uint64_t input_base_addr, uint64_t filter_base_addr,
                            uint64_t output_base_addr);

#endif /* _CONV_ENGINE_H_ */