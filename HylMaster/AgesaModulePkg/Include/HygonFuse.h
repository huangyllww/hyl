#ifndef _FUSE_H_
#define _FUSE_H_

#include <Base.h>

#define CCX_BIST_EN                     0x5d0CC
#define BIST_EN_BITS                    4
#define CCX_COLD_RESET_MBIST_EN         0x5D0DC
#define COLD_RESET_MBIST_EN_BITS        25

//HG1, HG2, HG3 Fuse
#define OPNFUSEPKG_SP3R2                0x5D258
#define OPEN_FUSE_BIT                   BIT30
#define MP0_RUN_MBIST                   0x5D258
#define RUN_MBIST_FUSE_BITS             6
#define MP0_CORE_DISABLE                0x5D25C
#define MP1_MAX_TDP                     0x5D378
#define MP1_MAX_TDP_START_BITS          1
#define MP1_FMAX                        0x5D380
#define MP1_FMAX_START_BITS             6

//HG4 Fuse
#define	HG4_OPNFUSEPKG_SP3R2            0x5D274
#define HG4_OPEN_FUSE_BIT               BIT30
#define	HG4_MP0_RUN_MBIST               0x5D274
#define HG4_RUN_MBIST_FUSE_BITS         6
#define	HG4_MP0_CORE_DISABLE            0x5D278
#define HG4_MP1_MAX_TDP                 0x5D3A0
#define	HG4_MP1_MAX_TDP_START_BITS      4
#define HG4_MP1_FMAX                    0x5D3A8
#define HG4_MP1_FMAX_START_BITS         9

#endif //_FUSE_H_
