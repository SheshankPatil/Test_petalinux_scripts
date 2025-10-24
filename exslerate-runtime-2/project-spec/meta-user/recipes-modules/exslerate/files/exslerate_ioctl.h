
#ifndef _EXSLERATE_IOCTL_H_
#define _EXSLERATE_IOCTL_H_

#include <drm/drm.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#ifdef __KERNEL__
#include <linux/stddef.h>
#else
#include <stddef.h>
#endif

/* IOCTL numbers */
#define DRM_EXSL_SUBMIT 0x00
#define DRM_EXSL_CREATE_BO 0x01
#define DRM_EXSL_GEM_MMAP 0x02
#define DRM_EXSL_GEM_DESTROY 0x03
#define DRM_EXSL_WRITE_CONFIG 0x04
#define DRM_EXSL_READ_STATUS 0x05
#define DRM_EXSL_PROGRAM_CORE 0x06

#define EXSL_INVALID_BO_HANDLE (~0U)

struct exsl_dev_info {
  int reg_bar;
  int mbox_bar;
  int sram_bar;
  int psp_bar;
  int smu_bar;
  int device_type;
  int first_col;
  __u32 dev_mem_buf_shift;
  __u64 dev_mem_base;
  size_t dev_mem_size;
  char *vbnv;
  const struct exsl_dev_priv *dev_priv;
  const struct exsl_dev_ops *ops;
};

/* Submit arguments */
struct exsl_submit_args {
  __u64 ext;
  __u64 ext_flags;
  __u32 hwctx;
#define EXSL_CMD_SUBMIT_EXEC_BUF 0
#define EXSL_CMD_SUBMIT_DEPENDENCY 1
#define EXSL_CMD_SUBMIT_SIGNAL 2
  __u32 type;
  __u64 cmd_handles;
  __u64 args;
  __u32 cmd_count;
  __u32 arg_count;
  __u64 seq;
};

/* GEM operations */
struct exsl_drm_create_bo {
  __u64 flags;
  __u64 vaddr;
  __u64 size;
#define EXSL_BO_INVALID 0  /* Invalid BO type */
#define EXSL_BO_SHARE 1    /* Regular BO shared between user and device */
#define EXSL_BO_DEV_HEAP 2 /* Shared host memory to device as heap memory */
#define EXSL_BO_DEV 3      /* Allocated from BO_DEV_HEAP */
#define EXSL_BO_CMD 4      /* User and driver accessible BO */
#define EXSL_BO_DMA 5      /* DRM GEM DMA BO */
  __u64 type;
  __u32 handle;
};

struct exsl_gem_create_args {
  __u32 handle;
  __u32 flags;
  __u64 size;
};

struct exsl_gem_map_offset_args {
  __u32 handle;
  __u32 flags;
  __u32 pad;        /* Reserved for alignment/future use */
  __u64 map_offset; /* mmap offset */
  __u64 vaddr;      /* Userspace Virt address (0 if unmapped) */
  __u64 dev_addr;   /* Device address */
};

struct exsl_gem_destroy_args {
  __u32 handle;
};

struct exsl_mem_handle {
  __u32 handle;
  __u32 flags;
  __u64 offset;
};

/* Conv core configuration structure */
struct exsl_write_config_args {
  /* Basic convolution parameters */
  __u32 PADDING, FILT_H, CHANNEL_SETS, filter_sets;
  __u32 stride, OACT_W, OACT_H, LINE_STRIDE;
  __u32 FILT_SIZE, SURF_STRIDE, TOTAL_FIL_SETS;
  __u32 IACT_H, IACT_W, ifMaxRam, ifOffset, IACT_BASE_ADDR, IACT_MAX_STRIPE,
      IACT_MAX_VALUE;
  __u32 flMaxRam, flOffset, flMaxStripe, flMaxValue;
  __u32 lifeTimeOffset;
  __u32 scalingFactor1, scalingFactor2, pooling_type;
  __u64 flBaseAddr, ifBaseAddr, lifetimeBaseAddr, biasBaseAddr, OutputBaseAddr;
  __u32 options;

  /* Tile configuration */
  __u32 tile_width_offset, tile_height_offset;
  __u32 tile_width, tile_height;
  __u32 strideCX, strideCY, featureLineStride;

  /* Missing burst length fields */
  __u32 ifBurstLen;       /* Input feature burst length */
  __u32 flBurstLen;       /* Filter burst length */
  __u32 lifetimeBurstLen; /* Lifetime burst length */

  /* Missing tile shifter fields */
  __u32 outTileHeight;           /* Output tile height */
  __u32 outTileWidth;            /* Output tile width */
  __u32 tileSizeShifter;         /* Tile size shifter */
  __u32 tileWidthShifter;        /* Tile width shifter */
  __u32 elemPerInputTileShifter; /* Elements per input tile shifter */
  __u32 elemPerOutputTile;       /* Elements per output tile */
  __u32 tileHeightShifter;       /* Tile height shifter */
  __u32 paddingW;                /* Padding width */

  /* Missing special function fields */
  __u32 specialFilterSets;    /* Special filter sets */
  __u32 specialSurfaceStride; /* Special surface stride */
  __u32 specialFixedVal;      /* Special fixed value */

  /* UDP/SDP fields */
  __u32 BDMA_nof_batch;
  __u32 BNDMA_nof_batch;
  __u32 enableBias;
  __u32 enableBatchNorm;
  __u32 bn_shifter;
  __u32 enableLUT;
  __u32 _k;
  __u32 _z_lower;
  __u32 _z_upper;
  __u32 sml;
  __u32 enableSpecialFunc;
  __u32 enablePRelu;
  __u32 PreRelu_scale;
  __u32 enableRelu;
  __u32 enableSQRelu;
  __u32 enableclippedRelu;
  __u32 clipped_scale;
  __u32 enableonlyQuntization;
  __u32 csrdmux;
  __u32 csrmode;
  __u32 BDMA_adrr_offset;
  __u32 BNDMA_addr_offset;
  __u32 biasSizebytes;
  __u32 BNweightSizebytes;
  __u32 BNbiasSizebytes;
  __u32 lutSizebytes;
  __u32 quant_shifter;
  __u32 quant_zero_point;
  __u32 wdma_transaction_num;

  /* Missing UDP LUT fields */
  __u32 lutFactor;     /* LUT factor */
  __u32 lutSubFactor0; /* LUT sub factor 0 */
  __u32 lutSubFactor1; /* LUT sub factor 1 */
  __u32 lutZeroPoint;  /* LUT zero point */
  __u32 layerNormB;    /* Layer normalization B */
  __u32 lutBaseAddr;   /* LUT base address */

  /* Missing BN address fields */
  __u64 bnBiasBaseAddr;   /* Batch normalization bias base address */
  __u64 bnWeightBaseAddr; /* Batch normalization weight base address */

  /* Missing DMA control fields */
  __u32 wdmaCSR;    /* WDMA control register */
  __u32 wdmaAXICSR; /* WDMA AXI control register */
  __u32 rdmaAXICSR; /* RDMA AXI control register */

  /* Missing interrupt fields */
  __u32 globalInterruptEn;  /* Global interrupt enable */
  __u32 doneInterruptEn;    /* Done interrupt enable */
  __u32 errorInterruptEn;   /* Error interrupt enable */
  __u32 timeoutInterruptEn; /* Timeout interrupt enable */

  /* Missing stall fields */
  __u32 stallEn;         /* Stall enable */
  __u32 stallCountValue; /* Stall count value */

  /* Padding for future expansion */
  __u32 reserved[8];
};

/* Status read structure */
struct exsl_read_status_args {
  __u32 status_value;
  __u32 reserved;
};

/* Program core structure */
struct exsl_program_core_args {
  __u32 core_type; /* 0 = CONV, 1 = GEMM */
  __u32 flags;
  __u32 reserved[2];
};

/* Memory access flags */
#define EXSL_MEM_READ (1 << 0)
#define EXSL_MEM_WRITE (1 << 1)

/* Core type flags */
#define EXSL_CONV_CORE 0
#define EXSL_GEMM_CORE 1

/* Flags for gem_map operation */
#define EXSL_GEM_MAP_GET_PHYS (1 << 0)

/* IOCTL definitions */
#define DRM_IOCTL_EXSL_SUBMIT                                                  \
  DRM_IOWR(DRM_COMMAND_BASE + DRM_EXSL_SUBMIT,                                 \
           struct exsl_submit_args) // exsl_drm_exec_cmd
#define DRM_IOCTL_EXSL_CREATE_BO                                               \
  DRM_IOWR(DRM_COMMAND_BASE + DRM_EXSL_CREATE_BO,                              \
           struct exsl_drm_create_bo) // exsl_gem_create_args

#define DRM_IOCTL_EXSL_GEM_MMAP                                                \
  DRM_IOWR(DRM_COMMAND_BASE + DRM_EXSL_GEM_MMAP,                               \
           struct exsl_gem_map_offset_args)
#define DRM_IOCTL_EXSL_GEM_DESTROY                                             \
  DRM_IOWR(DRM_COMMAND_BASE + DRM_EXSL_GEM_DESTROY,                            \
           struct exsl_gem_destroy_args)
#define DRM_IOCTL_EXSL_WRITE_CONFIG                                            \
  DRM_IOW(DRM_COMMAND_BASE + DRM_EXSL_WRITE_CONFIG,                            \
          struct exsl_write_config_args)
#define DRM_IOCTL_EXSL_READ_STATUS                                             \
  DRM_IOWR(DRM_COMMAND_BASE + DRM_EXSL_READ_STATUS,                            \
           struct exsl_read_status_args)
#define DRM_IOCTL_EXSL_PROGRAM_CORE                                            \
  DRM_IOW(DRM_COMMAND_BASE + DRM_EXSL_PROGRAM_CORE,                            \
          struct exsl_program_core_args)

#endif /* _EXSLERATE_IOCTL_H_ */