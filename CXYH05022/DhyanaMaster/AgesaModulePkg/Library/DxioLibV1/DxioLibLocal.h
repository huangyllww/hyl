/* $NoKeywords:$ */
/**
 * @file
 *
 * DXIO Ancillary Data Definitions
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
 *
 */
/*
*****************************************************************************
*
 * 
 * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
 * 
 * HYGON is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with HYGON.  This header does *NOT* give you permission to use the Materials
 * or any rights under HYGON's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 * 
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by HYGON shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 * 
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 * 
 * HYGON does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by HYGON, or
 * result from use of the Materials or any related information.
 * 
 * You agree that you will not reverse engineer or decompile the Materials.
 * 
 * NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, HYGON retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 * 
 * AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
 * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 * the right to make the modified version available for use with HYGON's PRODUCT.
* ***************************************************************************
*
*/

#ifndef _DXIOLIBLOCAL_H_
#define _DXIOLIBLOCAL_H_

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

#define   ZP_PLATFORM_TYPE  0
#define   MAX_INSTANCE_ID   8




// Register Name PCIE_LC_SPEED_CNTL

// Address
#define PCIE0_LC_SPEED_CNTL_ADDRESS                                          0x11140290
#define PCIE1_LC_SPEED_CNTL_ADDRESS                                          0x11240290

// Field Definitions
#define PCIE_LC_SPEED_CNTL_LC_GEN2_EN_STRAP_OFFSET                           0
#define PCIE_LC_SPEED_CNTL_LC_GEN2_EN_STRAP_WIDTH                            1
#define PCIE_LC_SPEED_CNTL_LC_GEN2_EN_STRAP_MASK                             0x1
#define PCIE_LC_SPEED_CNTL_LC_GEN3_EN_STRAP_OFFSET                           1
#define PCIE_LC_SPEED_CNTL_LC_GEN3_EN_STRAP_WIDTH                            1
#define PCIE_LC_SPEED_CNTL_LC_GEN3_EN_STRAP_MASK                             0x2
#define PCIE_LC_SPEED_CNTL_LC_TARGET_LINK_SPEED_OVERRIDE_EN_OFFSET           2
#define PCIE_LC_SPEED_CNTL_LC_TARGET_LINK_SPEED_OVERRIDE_EN_WIDTH            1
#define PCIE_LC_SPEED_CNTL_LC_TARGET_LINK_SPEED_OVERRIDE_EN_MASK             0x4
#define PCIE_LC_SPEED_CNTL_LC_TARGET_LINK_SPEED_OVERRIDE_OFFSET              3
#define PCIE_LC_SPEED_CNTL_LC_TARGET_LINK_SPEED_OVERRIDE_WIDTH               2
#define PCIE_LC_SPEED_CNTL_LC_TARGET_LINK_SPEED_OVERRIDE_MASK                0x18
#define PCIE_LC_SPEED_CNTL_LC_FORCE_EN_SW_SPEED_CHANGE_OFFSET                5
#define PCIE_LC_SPEED_CNTL_LC_FORCE_EN_SW_SPEED_CHANGE_WIDTH                 1
#define PCIE_LC_SPEED_CNTL_LC_FORCE_EN_SW_SPEED_CHANGE_MASK                  0x20
#define PCIE_LC_SPEED_CNTL_LC_FORCE_DIS_SW_SPEED_CHANGE_OFFSET               6
#define PCIE_LC_SPEED_CNTL_LC_FORCE_DIS_SW_SPEED_CHANGE_WIDTH                1
#define PCIE_LC_SPEED_CNTL_LC_FORCE_DIS_SW_SPEED_CHANGE_MASK                 0x40
#define PCIE_LC_SPEED_CNTL_LC_FORCE_EN_HW_SPEED_CHANGE_OFFSET                7
#define PCIE_LC_SPEED_CNTL_LC_FORCE_EN_HW_SPEED_CHANGE_WIDTH                 1
#define PCIE_LC_SPEED_CNTL_LC_FORCE_EN_HW_SPEED_CHANGE_MASK                  0x80
#define PCIE_LC_SPEED_CNTL_LC_FORCE_DIS_HW_SPEED_CHANGE_OFFSET               8
#define PCIE_LC_SPEED_CNTL_LC_FORCE_DIS_HW_SPEED_CHANGE_WIDTH                1
#define PCIE_LC_SPEED_CNTL_LC_FORCE_DIS_HW_SPEED_CHANGE_MASK                 0x100
#define PCIE_LC_SPEED_CNTL_LC_INITIATE_LINK_SPEED_CHANGE_OFFSET              9
#define PCIE_LC_SPEED_CNTL_LC_INITIATE_LINK_SPEED_CHANGE_WIDTH               1
#define PCIE_LC_SPEED_CNTL_LC_INITIATE_LINK_SPEED_CHANGE_MASK                0x200
#define PCIE_LC_SPEED_CNTL_LC_SPEED_CHANGE_ATTEMPTS_ALLOWED_OFFSET           10
#define PCIE_LC_SPEED_CNTL_LC_SPEED_CHANGE_ATTEMPTS_ALLOWED_WIDTH            2
#define PCIE_LC_SPEED_CNTL_LC_SPEED_CHANGE_ATTEMPTS_ALLOWED_MASK             0xc00
#define PCIE_LC_SPEED_CNTL_LC_SPEED_CHANGE_ATTEMPT_FAILED_OFFSET             12
#define PCIE_LC_SPEED_CNTL_LC_SPEED_CHANGE_ATTEMPT_FAILED_WIDTH              1
#define PCIE_LC_SPEED_CNTL_LC_SPEED_CHANGE_ATTEMPT_FAILED_MASK               0x1000
#define PCIE_LC_SPEED_CNTL_LC_CURRENT_DATA_RATE_OFFSET                       13
#define PCIE_LC_SPEED_CNTL_LC_CURRENT_DATA_RATE_WIDTH                        2
#define PCIE_LC_SPEED_CNTL_LC_CURRENT_DATA_RATE_MASK                         0x6000
#define PCIE_LC_SPEED_CNTL_LC_DONT_CLR_TARGET_SPD_CHANGE_STATUS_OFFSET       15
#define PCIE_LC_SPEED_CNTL_LC_DONT_CLR_TARGET_SPD_CHANGE_STATUS_WIDTH        1
#define PCIE_LC_SPEED_CNTL_LC_DONT_CLR_TARGET_SPD_CHANGE_STATUS_MASK         0x8000
#define PCIE_LC_SPEED_CNTL_LC_CLR_FAILED_SPD_CHANGE_CNT_OFFSET               16
#define PCIE_LC_SPEED_CNTL_LC_CLR_FAILED_SPD_CHANGE_CNT_WIDTH                1
#define PCIE_LC_SPEED_CNTL_LC_CLR_FAILED_SPD_CHANGE_CNT_MASK                 0x10000
#define PCIE_LC_SPEED_CNTL_LC_1_OR_MORE_TS2_SPEED_ARC_EN_OFFSET              17
#define PCIE_LC_SPEED_CNTL_LC_1_OR_MORE_TS2_SPEED_ARC_EN_WIDTH               1
#define PCIE_LC_SPEED_CNTL_LC_1_OR_MORE_TS2_SPEED_ARC_EN_MASK                0x20000
#define PCIE_LC_SPEED_CNTL_LC_OTHER_SIDE_EVER_SENT_GEN2_OFFSET               18
#define PCIE_LC_SPEED_CNTL_LC_OTHER_SIDE_EVER_SENT_GEN2_WIDTH                1
#define PCIE_LC_SPEED_CNTL_LC_OTHER_SIDE_EVER_SENT_GEN2_MASK                 0x40000
#define PCIE_LC_SPEED_CNTL_LC_OTHER_SIDE_SUPPORTS_GEN2_OFFSET                19
#define PCIE_LC_SPEED_CNTL_LC_OTHER_SIDE_SUPPORTS_GEN2_WIDTH                 1
#define PCIE_LC_SPEED_CNTL_LC_OTHER_SIDE_SUPPORTS_GEN2_MASK                  0x80000
#define PCIE_LC_SPEED_CNTL_LC_OTHER_SIDE_EVER_SENT_GEN3_OFFSET               20
#define PCIE_LC_SPEED_CNTL_LC_OTHER_SIDE_EVER_SENT_GEN3_WIDTH                1
#define PCIE_LC_SPEED_CNTL_LC_OTHER_SIDE_EVER_SENT_GEN3_MASK                 0x100000
#define PCIE_LC_SPEED_CNTL_LC_OTHER_SIDE_SUPPORTS_GEN3_OFFSET                21
#define PCIE_LC_SPEED_CNTL_LC_OTHER_SIDE_SUPPORTS_GEN3_WIDTH                 1
#define PCIE_LC_SPEED_CNTL_LC_OTHER_SIDE_SUPPORTS_GEN3_MASK                  0x200000
#define PCIE_LC_SPEED_CNTL_LC_AUTO_RECOVERY_DIS_OFFSET                       22
#define PCIE_LC_SPEED_CNTL_LC_AUTO_RECOVERY_DIS_WIDTH                        1
#define PCIE_LC_SPEED_CNTL_LC_AUTO_RECOVERY_DIS_MASK                         0x400000
#define PCIE_LC_SPEED_CNTL_LC_SPEED_CHANGE_STATUS_OFFSET                     23
#define PCIE_LC_SPEED_CNTL_LC_SPEED_CHANGE_STATUS_WIDTH                      1
#define PCIE_LC_SPEED_CNTL_LC_SPEED_CHANGE_STATUS_MASK                       0x800000
#define PCIE_LC_SPEED_CNTL_LC_DATA_RATE_ADVERTISED_OFFSET                    24
#define PCIE_LC_SPEED_CNTL_LC_DATA_RATE_ADVERTISED_WIDTH                     2
#define PCIE_LC_SPEED_CNTL_LC_DATA_RATE_ADVERTISED_MASK                      0x3000000
#define PCIE_LC_SPEED_CNTL_LC_CHECK_DATA_RATE_OFFSET                         26
#define PCIE_LC_SPEED_CNTL_LC_CHECK_DATA_RATE_WIDTH                          1
#define PCIE_LC_SPEED_CNTL_LC_CHECK_DATA_RATE_MASK                           0x4000000
#define PCIE_LC_SPEED_CNTL_LC_MULT_UPSTREAM_AUTO_SPD_CHNG_EN_OFFSET          27
#define PCIE_LC_SPEED_CNTL_LC_MULT_UPSTREAM_AUTO_SPD_CHNG_EN_WIDTH           1
#define PCIE_LC_SPEED_CNTL_LC_MULT_UPSTREAM_AUTO_SPD_CHNG_EN_MASK            0x8000000
#define PCIE_LC_SPEED_CNTL_LC_INIT_SPEED_NEG_IN_L0s_EN_OFFSET                28
#define PCIE_LC_SPEED_CNTL_LC_INIT_SPEED_NEG_IN_L0s_EN_WIDTH                 1
#define PCIE_LC_SPEED_CNTL_LC_INIT_SPEED_NEG_IN_L0s_EN_MASK                  0x10000000
#define PCIE_LC_SPEED_CNTL_LC_INIT_SPEED_NEG_IN_L1_EN_OFFSET                 29
#define PCIE_LC_SPEED_CNTL_LC_INIT_SPEED_NEG_IN_L1_EN_WIDTH                  1
#define PCIE_LC_SPEED_CNTL_LC_INIT_SPEED_NEG_IN_L1_EN_MASK                   0x20000000
#define PCIE_LC_SPEED_CNTL_LC_DONT_CHECK_EQTS_IN_RCFG_OFFSET                 30
#define PCIE_LC_SPEED_CNTL_LC_DONT_CHECK_EQTS_IN_RCFG_WIDTH                  1
#define PCIE_LC_SPEED_CNTL_LC_DONT_CHECK_EQTS_IN_RCFG_MASK                   0x40000000
#define PCIE_LC_SPEED_CNTL_LC_DELAY_COEFF_UPDATE_DIS_OFFSET                  31
#define PCIE_LC_SPEED_CNTL_LC_DELAY_COEFF_UPDATE_DIS_WIDTH                   1
#define PCIE_LC_SPEED_CNTL_LC_DELAY_COEFF_UPDATE_DIS_MASK                    0x80000000L

/// PCIE_LC_SPEED_CNTL
typedef union {
  struct {
    UINT32                                        LC_GEN2_EN_STRAP:1 ; ///<
    UINT32                                        LC_GEN3_EN_STRAP:1 ; ///<
    UINT32                        LC_TARGET_LINK_SPEED_OVERRIDE_EN:1 ; ///<
    UINT32                           LC_TARGET_LINK_SPEED_OVERRIDE:2 ; ///<
    UINT32                             LC_FORCE_EN_SW_SPEED_CHANGE:1 ; ///<
    UINT32                            LC_FORCE_DIS_SW_SPEED_CHANGE:1 ; ///<
    UINT32                             LC_FORCE_EN_HW_SPEED_CHANGE:1 ; ///<
    UINT32                            LC_FORCE_DIS_HW_SPEED_CHANGE:1 ; ///<
    UINT32                           LC_INITIATE_LINK_SPEED_CHANGE:1 ; ///<
    UINT32                        LC_SPEED_CHANGE_ATTEMPTS_ALLOWED:2 ; ///<
    UINT32                          LC_SPEED_CHANGE_ATTEMPT_FAILED:1 ; ///<
    UINT32                                    LC_CURRENT_DATA_RATE:2 ; ///<
    UINT32                    LC_DONT_CLR_TARGET_SPD_CHANGE_STATUS:1 ; ///<
    UINT32                            LC_CLR_FAILED_SPD_CHANGE_CNT:1 ; ///<
    UINT32                           LC_1_OR_MORE_TS2_SPEED_ARC_EN:1 ; ///<
    UINT32                            LC_OTHER_SIDE_EVER_SENT_GEN2:1 ; ///<
    UINT32                             LC_OTHER_SIDE_SUPPORTS_GEN2:1 ; ///<
    UINT32                            LC_OTHER_SIDE_EVER_SENT_GEN3:1 ; ///<
    UINT32                             LC_OTHER_SIDE_SUPPORTS_GEN3:1 ; ///<
    UINT32                                    LC_AUTO_RECOVERY_DIS:1 ; ///<
    UINT32                                  LC_SPEED_CHANGE_STATUS:1 ; ///<
    UINT32                                 LC_DATA_RATE_ADVERTISED:2 ; ///<
    UINT32                                      LC_CHECK_DATA_RATE:1 ; ///<
    UINT32                       LC_MULT_UPSTREAM_AUTO_SPD_CHNG_EN:1 ; ///<
    UINT32                             LC_INIT_SPEED_NEG_IN_L0s_EN:1 ; ///<
    UINT32                              LC_INIT_SPEED_NEG_IN_L1_EN:1 ; ///<
    UINT32                              LC_DONT_CHECK_EQTS_IN_RCFG:1 ; ///<
    UINT32                               LC_DELAY_COEFF_UPDATE_DIS:1 ; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} PCIE_LC_SPEED_CNTL_STRUCT;

#define PCIE0_LC_CNTL_10_ADDRESS 0x1114038c
#define PCIE1_LC_CNTL_10_ADDRESS 0x1124038c

typedef union {
    struct {
        UINT32                              LC_LINK_DOWN_SPD_CHG_EN:1;   
        UINT32                    LC_CLR_DELAY_DLLP_WHEN_NO_AUTO_EQ:1;    
        UINT32                    LC_MULT_AUTO_SPD_CHG_ON_LAST_RATE:1;  
        UINT32               LC_CLEAR_RXSTANDBY_ON_RATE_UPDATE_ONLY:1;
        UINT32        LC_POWERDOWN_P0_WAIT_FOR_REFCLKACK_ON_L1_EXIT:1;
        UINT32                                          RESERVE_5_7:3;
        UINT32                     LC_TARGET_LINK_SPEED_OVERRIDE_EN:1;
        UINT32                        LC_TARGET_LINK_SPEED_OVERRIDE:2;
        UINT32                                        RESERVE_11_15:5;
        UINT32                       LC_CLEAR_REVERSE_ATTEMPT_IN_L0:1;
        UINT32                                     LC_LOCK_REVERSAL:1;
        UINT32                                           RESERVE_18:1;
        UINT32                          LC_WAIT_FOR_LANES_IN_CONFIG:1;
        UINT32                        LC_REQ_COEFFS_FOR_TXMARGIN_EN:1;
        UINT32                         LC_AUTO_REJECT_AFTER_TIMEOUT:1; 
        UINT32                                         LC_ESM_RATES:2;
        UINT32                                    LC_ESM_ENTRY_MODE:1;  
        UINT32                                        RESERVE_25_29:5;
        UINT32                         LC_OTHER_SIDE_EVER_SENT_GEN4:1;  
        UINT32                          LC_OTHER_SIDE_SUPPORTS_GEN4:1;
    }Field;
    UINT32 Value;
}PCIE_LC_CNTL_10;
      
// Address
#define PCIE_LC_STATE0_ADDRESS                                  0x11140294
#define PCIE0_LC_STATE0_ADDRESS                                 0x11140294
#define PCIE1_LC_STATE0_ADDRESS                                 0x11240294

#define PCIE_LC_STATE0_LC_CURRENT_STATE_OFFSET           0
#define PCIE_LC_STATE0_LC_CURRENT_STATE_WIDTH            6
#define PCIE_LC_STATE0_LC_CURRENT_STATE_MASK             0x3f
#define PCIE_LC_STATE0_Reserved0_OFFSET                  6
#define PCIE_LC_STATE0_Reserved0_WIDTH                   2
#define PCIE_LC_STATE0_Reserved0_MASK                    0xc0
#define PCIE_LC_STATE0_LC_PREV_STATE1_OFFSET             8
#define PCIE_LC_STATE0_LC_PREV_STATE1_WIDTH              6
#define PCIE_LC_STATE0_LC_PREV_STATE1_MASK               0x3f00
#define PCIE_LC_STATE0_Reserved1_OFFSET                  14
#define PCIE_LC_STATE0_Reserved1_WIDTH                   2
#define PCIE_LC_STATE0_Reserved1_MASK                    0xc000
#define PCIE_LC_STATE0_LC_PREV_STATE2_OFFSET             16
#define PCIE_LC_STATE0_LC_PREV_STATE2_WIDTH              6
#define PCIE_LC_STATE0_LC_PREV_STATE2_MASK               0x3f0000
#define PCIE_LC_STATE0_Reserved2_OFFSET                  22
#define PCIE_LC_STATE0_Reserved2_WIDTH                   2
#define PCIE_LC_STATE0_Reserved2_MASK                    0xc00000
#define PCIE_LC_STATE0_LC_PREV_STATE3_OFFSET             24
#define PCIE_LC_STATE0_LC_PREV_STATE3_WIDTH              6
#define PCIE_LC_STATE0_LC_PREV_STATE3_MASK               0x3f000000
#define PCIE_LC_STATE0_Reserved3_OFFSET                  30
#define PCIE_LC_STATE0_Reserved3_WIDTH                   2
#define PCIE_LC_STATE0_Reserved3_MASK                    0xc0000000


/// PCIE_LC_STATE0
typedef union {
  struct {
    UINT32                                        LC_CURRENT_STATE:6 ; ///<
    UINT32                                               Reserved0:2 ; ///<
    UINT32                                          LC_PREV_STATE1:6 ; ///<
    UINT32                                               Reserved1:2 ; ///<
    UINT32                                          LC_PREV_STATE2:6 ; ///<
    UINT32                                               Reserved2:2 ; ///<
    UINT32                                          LC_PREV_STATE3:6 ; ///<
    UINT32                                               Reserved3:2 ; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} PCIE_LC_STATE0_STRUCT;
// Address
#define PCIE0_SWRST_CONTROL_6_ADDRESS                          0x11180428
#define PCIE1_SWRST_CONTROL_6_ADDRESS                          0x11280428


#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_A_OFFSET                     0
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_A_WIDTH                      1
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_A_MASK                       0x1
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_B_OFFSET                     1
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_B_WIDTH                      1
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_B_MASK                       0x2
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_C_OFFSET                     2
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_C_WIDTH                      1
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_C_MASK                       0x4
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_D_OFFSET                     3
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_D_WIDTH                      1
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_D_MASK                       0x8
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_E_OFFSET                     4
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_E_WIDTH                      1
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_E_MASK                       0x10
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_F_OFFSET                     5
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_F_WIDTH                      1
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_F_MASK                       0x20
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_G_OFFSET                     6
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_G_WIDTH                      1
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_G_MASK                       0x40
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_H_OFFSET                     7
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_H_WIDTH                      1
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_H_MASK                       0x80
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_I_OFFSET                     8
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_I_WIDTH                      1
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_I_MASK                       0x100
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_J_OFFSET                     9
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_J_WIDTH                      1
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_J_MASK                       0x200
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_K_OFFSET                     10
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_K_WIDTH                      1
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_K_MASK                       0x400

/// PCIE_SWRST_CNTL6_STRUCT
typedef union {
  struct {
    UINT32                                         HOLD_TRAINING_A:1 ; ///<
    UINT32                                         HOLD_TRAINING_B:1 ; ///<
    UINT32                                         HOLD_TRAINING_C:1 ; ///<
    UINT32                                         HOLD_TRAINING_D:1 ; ///<
    UINT32                                         HOLD_TRAINING_E:1 ; ///<
    UINT32                                         HOLD_TRAINING_F:1 ; ///<
    UINT32                                         HOLD_TRAINING_G:1 ; ///<
    UINT32                                         HOLD_TRAINING_H:1 ; ///<
    UINT32                                         HOLD_TRAINING_I:1 ; ///<
    UINT32                                         HOLD_TRAINING_J:1 ; ///<
    UINT32                                         HOLD_TRAINING_K:1 ; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} PCIE_SWRST_CNTL6_STRUCT;

// Register Name PCIE_LC_CNTL5

// Address
#define PCIE0_LC_CNTL5_ADDRESS                                           0x111402dc
#define PCIE1_LC_CNTL5_ADDRESS                                           0x111412dc
#define PCIE2_LC_CNTL5_ADDRESS                                           0x111422dc
#define PCIE3_LC_CNTL5_ADDRESS                                           0x111432dc
#define PCIE4_LC_CNTL5_ADDRESS                                           0x111442dc
#define PCIE5_LC_CNTL5_ADDRESS                                           0x111452dc
#define PCIE6_LC_CNTL5_ADDRESS                                           0x111462dc
#define PCIE7_LC_CNTL5_ADDRESS                                           0x111472dc
#define PCIE8_LC_CNTL5_ADDRESS                                           0x112402dc
#define PCIE9_LC_CNTL5_ADDRESS                                           0x112412dc
#define PCIEA_LC_CNTL5_ADDRESS                                           0x112422dc
#define PCIEB_LC_CNTL5_ADDRESS                                           0x112432dc
#define PCIEC_LC_CNTL5_ADDRESS                                           0x112442dc
#define PCIED_LC_CNTL5_ADDRESS                                           0x112452dc
#define PCIEE_LC_CNTL5_ADDRESS                                           0x112462dc
#define PCIEF_LC_CNTL5_ADDRESS                                           0x112472dc

// Type
#define PCIE_LC_CNTL5_TYPE                                              TYPE_SMN

#define PCIE_LC_CNTL5_LC_EQ_FS_0_OFFSET                                 0
#define PCIE_LC_CNTL5_LC_EQ_FS_0_WIDTH                                  6
#define PCIE_LC_CNTL5_LC_EQ_FS_0_MASK                                   0x3f
#define PCIE_LC_CNTL5_LC_EQ_FS_8_OFFSET                                 6
#define PCIE_LC_CNTL5_LC_EQ_FS_8_WIDTH                                  6
#define PCIE_LC_CNTL5_LC_EQ_FS_8_MASK                                   0xfc0
#define PCIE_LC_CNTL5_LC_EQ_LF_0_OFFSET                                 12
#define PCIE_LC_CNTL5_LC_EQ_LF_0_WIDTH                                  6
#define PCIE_LC_CNTL5_LC_EQ_LF_0_MASK                                   0x3f000
#define PCIE_LC_CNTL5_LC_EQ_LF_8_OFFSET                                 18
#define PCIE_LC_CNTL5_LC_EQ_LF_8_WIDTH                                  6
#define PCIE_LC_CNTL5_LC_EQ_LF_8_MASK                                   0xfc0000
#define PCIE_LC_CNTL5_LC_DSC_EQ_FS_LF_INVALID_TO_PRESETS_OFFSET         24
#define PCIE_LC_CNTL5_LC_DSC_EQ_FS_LF_INVALID_TO_PRESETS_WIDTH          1
#define PCIE_LC_CNTL5_LC_DSC_EQ_FS_LF_INVALID_TO_PRESETS_MASK           0x1000000
#define PCIE_LC_CNTL5_LC_TX_SWING_OVERRIDE_OFFSET                       25
#define PCIE_LC_CNTL5_LC_TX_SWING_OVERRIDE_WIDTH                        1
#define PCIE_LC_CNTL5_LC_TX_SWING_OVERRIDE_MASK                         0x2000000
#define PCIE_LC_CNTL5_LC_ACCEPT_ALL_PRESETS_OFFSET                      26
#define PCIE_LC_CNTL5_LC_ACCEPT_ALL_PRESETS_WIDTH                       1
#define PCIE_LC_CNTL5_LC_ACCEPT_ALL_PRESETS_MASK                        0x4000000
#define PCIE_LC_CNTL5_LC_ACCEPT_ALL_PRESETS_TEST_OFFSET                 27
#define PCIE_LC_CNTL5_LC_ACCEPT_ALL_PRESETS_TEST_WIDTH                  1
#define PCIE_LC_CNTL5_LC_ACCEPT_ALL_PRESETS_TEST_MASK                   0x8000000
#define PCIE_LC_CNTL5_LC_WAIT_IN_DETECT_OFFSET                          28
#define PCIE_LC_CNTL5_LC_WAIT_IN_DETECT_WIDTH                           1
#define PCIE_LC_CNTL5_LC_WAIT_IN_DETECT_MASK                            0x10000000
#define PCIE_LC_CNTL5_LC_HOLD_TRAINING_MODE_OFFSET                      29
#define PCIE_LC_CNTL5_LC_HOLD_TRAINING_MODE_WIDTH                       3
#define PCIE_LC_CNTL5_LC_HOLD_TRAINING_MODE_MASK                        0xe0000000L

/// PCIE_LC_CNTL5_STRUCT
typedef union {
  struct {
    UINT32                                              LC_EQ_FS_0:6 ; ///<
    UINT32                                              LC_EQ_FS_8:6 ; ///<
    UINT32                                              LC_EQ_LF_0:6 ; ///<
    UINT32                                              LC_EQ_LF_8:6 ; ///<
    UINT32                      LC_DSC_EQ_FS_LF_INVALID_TO_PRESETS:1 ; ///<
    UINT32                                    LC_TX_SWING_OVERRIDE:1 ; ///<
    UINT32                                   LC_ACCEPT_ALL_PRESETS:1 ; ///<
    UINT32                              LC_ACCEPT_ALL_PRESETS_TEST:1 ; ///<
    UINT32                                       LC_WAIT_IN_DETECT:1 ; ///<
    UINT32                                   LC_HOLD_TRAINING_MODE:3 ; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} PCIE_LC_CNTL5_STRUCT;

/// PCIE_LINK_CAP
#define RC_PCIE_LINK_CAP_OFFSET    0x64  
typedef union {
  struct {
    UINT32                                              LINK_SPEED:4 ;
    UINT32                                              LINK_WIDTH:6 ;
    UINT32                                              PM_SUPPORT:2 ;
    UINT32                                        L0S_EXIT_LATENCY:3 ;
    UINT32                                         L1_EXIT_LATENCY:3 ;
    UINT32                                  CLOCK_POWER_MANAGEMENT:1 ;
    UINT32                             SURPRISE_DOWN_ERR_ERPORTING:1 ;
    UINT32                             DL_ACTIVE_ERPORTING_CAPABLE:1 ;
    UINT32                                 LINK_BW_NOTFICATION_CAP:1 ;
    UINT32                              ASM_OPTIONALITY_COMPLIANCE:1 ;
    UINT32                                             RESERVER_23:1 ;
    UINT32                                             PORT_NUMBER:8 ;
  } Field;                                                             
  UINT32 Value;                                                        
} PCIE_LINK_CAP_STRUCT;

// Address
#define PCIE0_LINK_STATUS_ADDRESS                                    0x11100068
#define PCIE1_LINK_STATUS_ADDRESS                                    0x11200068

// Type
#define PCIE_LINK_STATUS_TYPE                                        TYPE_SMN

#define PCIE_LINK_STATUS_CURRENT_LINK_SPEED_OFFSET                    16
#define PCIE_LINK_STATUS_CURRENT_LINK_SPEED_WIDTH                     4
#define PCIE_LINK_STATUS_CURRENT_LINK_SPEED_MASK                      0xf0000
#define PCIE_LINK_STATUS_NEGOTIATED_LINK_WIDTH_OFFSET                 20
#define PCIE_LINK_STATUS_NEGOTIATED_LINK_WIDTH_WIDTH                  6
#define PCIE_LINK_STATUS_NEGOTIATED_LINK_WIDTH_MASK                   0x3f00000
#define PCIE_LINK_STATUS_LINK_TRAINING_OFFSET                         27
#define PCIE_LINK_STATUS_LINK_TRAINING_WIDTH                          1
#define PCIE_LINK_STATUS_LINK_TRAINING_MASK                           0x8000000
#define PCIE_LINK_STATUS_SLOT_CLOCK_CFG_OFFSET                        28
#define PCIE_LINK_STATUS_SLOT_CLOCK_CFG_WIDTH                         1
#define PCIE_LINK_STATUS_SLOT_CLOCK_CFG_MASK                          0x10000000
#define PCIE_LINK_STATUS_DL_ACTIVE_OFFSET                             29
#define PCIE_LINK_STATUS_DL_ACTIVE_WIDTH                              1
#define PCIE_LINK_STATUS_DL_ACTIVE_MASK                               0x20000000
#define PCIE_LINK_STATUS_LINK_BW_MANAGEMENT_STATUS_OFFSET             30
#define PCIE_LINK_STATUS_LINK_BW_MANAGEMENT_STATUS_WIDTH              1
#define PCIE_LINK_STATUS_LINK_BW_MANAGEMENT_STATUS_MASK               0x40000000
#define PCIE_LINK_STATUS_LINK_AUTONOMOUS_BW_STATUS_OFFSET             31
#define PCIE_LINK_STATUS_LINK_AUTONOMOUS_BW_STATUS_WIDTH              1
#define PCIE_LINK_STATUS_LINK_AUTONOMOUS_BW_STATUS_MASK               0x80000000

/// PCIE_LINK_STATUS
#define RC_PCIE_LINK_STATUS_OFFSET 0x68 
typedef union {
  struct {
    UINT32                                                reserved:16; ///<
    UINT32                                      CURRENT_LINK_SPEED:4 ; ///<
    UINT32                                   NEGOTIATED_LINK_WIDTH:6 ; ///<
    UINT32                                             reserved_10:1 ; ///<
    UINT32                                           LINK_TRAINING:1 ; ///<
    UINT32                                          SLOT_CLOCK_CFG:1 ; ///<
    UINT32                                               DL_ACTIVE:1 ; ///<
    UINT32                               LINK_BW_MANAGEMENT_STATUS:1 ; ///<
    UINT32                               LINK_AUTONOMOUS_BW_STATUS:1 ; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} PCIE_LINK_STATUS_STRUCT;
 
/// PCIE_LINK_CNTL
#define RC_PCIE_LINK_CNTL_OFFSET    0x68  
typedef union {
  struct {
    UINT32                                              PM_CONTROL:2 ;
    UINT32                                              RESERVED_2:1 ;
    UINT32                                       READ_CPL_BOUNDARY:1 ;
    UINT32                                                LINK_DIS:1 ;
    UINT32                                            RETRAIN_LINK:1 ;
    UINT32                                        COMMON_CLOCK_CFG:1 ;
    UINT32                                           EXTENDED_SYNC:1 ;
    UINT32                               CLOCK_POWER_MANAGEMENT_EN:1 ;
    UINT32                             HW_AUTONOMOUS_WIDTH_DISABLE:1 ;
    UINT32                               LINK_BW_MANAGEMENT_INT_EN:1 ;
    UINT32                                LINK_AUTONMOUS_BW_INT_EN:1 ;
    UINT32                                          RESERVED_12_13:2 ;
    UINT32                                   DRS_SIGNALING_CONTROL:2 ;
    UINT32                                                reserved:16;
  } Field;                                                             
  UINT32 Value;                                                        
} PCIE_LINK_CNTL_STRUCT;

/// PCIE_LINK_CNTL3
#define RC_PCIE_LINK_CNTL3_OFFSET    0x274   
typedef union {
  struct {
    UINT32                                    PERFORM_EQUALIZATION:1 ;
    UINT32                            LINK_EQUALIZATION_REQ_INT_EN:1 ;
    UINT32                                            RESERVED_2_8:7 ;
    UINT32                                ENABLE_LOWER_SKIP_OS_GEN:7 ;
    UINT32                                                reserved:16;
  } Field;                                                             
  UINT32 Value;                                                        
} PCIE_LINK_CNTL3_STRUCT;

/// PCIE_LANE_EQUALIZATION_CNTL Gen3
#define PCIE_LANE_EQUALIZATION_CNTL_OFFSET    0x27C

typedef union {
  struct {
    UINT16    DOWNSTREAM_PORT_TX_PRESET         : 4;                        ///<
    UINT16    DOWNSTREAM_PORT_RX_PRESET_HINT    : 3;                        ///<
    UINT16    Reserved_0                        : 1;                        ///<
    UINT16    UPSTREAM_PORT_TX_PRESET           : 4;                        ///<
    UINT16    UPSTREAM_PORT_RX_PRESET_HINT      : 3;                        ///<
    UINT16    Reserved_1                        : 1;                        ///<
  } Field;                                                                  ///<
  UINT16    Value;                                                          ///<
} PCIE_LANE_EQ_CNTL_STRUCT;

/// PCIE_LANE_EQUALIZATION_CNTL_16GT Gen4
#define PCIE_LANE_EQUALIZATION_CNTL_16GT_OFFSET    0x440

typedef union {
  struct {
    UINT8    DOWNSTREAM_PORT_TX_PRESET          : 4;                        ///<
    UINT8    UPSTREAM_PORT_TX_PRESET            : 4;                        ///< 
  } Field;                                                                  ///<
  UINT8    Value;                                                           ///<
} PCIE_LANE_EQ_CNTL_16GT_STRUCT;

///RC_PCIE_DEVICE_STATUS
#define RC_PCIE_DEVICE_CNTL_AND_STATUS_OFFSET        0x60
///RC_PCIE_UNCORR_ERR_STATUS
#define RC_PCIE_UNCORR_ERR_STATUS_OFFSET    0x154
///RC_PCIE_CORR_ERR_STATUS
#define RC_PCIE_CORR_ERR_STATUS_OFFSET      0x160
///RC_PCIE_ROOT_ERR_STATUS
#define RC_PCIE_ROOT_ERR_STATUS_OFFSET      0x180
///RC_PCIE_LANE_ERR_STATUS
#define RC_PCIE_LANE_ERR_STATUS_OFFSET      0x278

// Address
#define PCIE0_LINK_CNTL2_ADDRESS                               0x11100088
#define PCIE1_LINK_CNTL2_ADDRESS                               0x11200088

// Type
#define PCIE_LINK_CNTL2_TYPE                                   TYPE_SMN

#define PCIE_LINK_CNTL2_TARGET_LINK_SPEED_OFFSET               0
#define PCIE_LINK_CNTL2_TARGET_LINK_SPEED_WIDTH                4
#define PCIE_LINK_CNTL2_TARGET_LINK_SPEED_MASK                 0xf
#define PCIE_LINK_CNTL2_ENTER_COMPLIANCE_OFFSET                4
#define PCIE_LINK_CNTL2_ENTER_COMPLIANCE_WIDTH                 1
#define PCIE_LINK_CNTL2_ENTER_COMPLIANCE_MASK                  0x10
#define PCIE_LINK_CNTL2_HW_AUTONOMOUS_SPEED_DISABLE_OFFSET     5
#define PCIE_LINK_CNTL2_HW_AUTONOMOUS_SPEED_DISABLE_WIDTH      1
#define PCIE_LINK_CNTL2_HW_AUTONOMOUS_SPEED_DISABLE_MASK       0x20
#define PCIE_LINK_CNTL2_SELECTABLE_DEEMPHASIS_OFFSET           6
#define PCIE_LINK_CNTL2_SELECTABLE_DEEMPHASIS_WIDTH            1
#define PCIE_LINK_CNTL2_SELECTABLE_DEEMPHASIS_MASK             0x40
#define PCIE_LINK_CNTL2_XMIT_MARGIN_OFFSET                     7
#define PCIE_LINK_CNTL2_XMIT_MARGIN_WIDTH                      3
#define PCIE_LINK_CNTL2_XMIT_MARGIN_MASK                       0x380
#define PCIE_LINK_CNTL2_ENTER_MOD_COMPLIANCE_OFFSET            10
#define PCIE_LINK_CNTL2_ENTER_MOD_COMPLIANCE_WIDTH             1
#define PCIE_LINK_CNTL2_ENTER_MOD_COMPLIANCE_MASK              0x400
#define PCIE_LINK_CNTL2_COMPLIANCE_SOS_OFFSET                  11
#define PCIE_LINK_CNTL2_COMPLIANCE_SOS_WIDTH                   1
#define PCIE_LINK_CNTL2_COMPLIANCE_SOS_MASK                    0x800
#define PCIE_LINK_CNTL2_COMPLIANCE_DEEMPHASIS_OFFSET           12
#define PCIE_LINK_CNTL2_COMPLIANCE_DEEMPHASIS_WIDTH            4
#define PCIE_LINK_CNTL2_COMPLIANCE_DEEMPHASIS_MASK             0xf000

//PCIE_LC_CNTL8
#define PCIE_PORT_REGS_SMN_BASE                                    0x11140000
#define PCIE_LC_CNTL8_REG_OFFSET                                 0x0374
typedef struct {
    UINT32    LC_EQ_SEARCH_MODE_16GT:2;
    UINT32    LC_BYPASS_EQ_16GT:1;
    UINT32    LC_BYPASS_EQ_PRESET_16GT:4;
    UINT32    LC_REDO_EQ_16GT:1;
    UINT32    LC_USC_EQ_NOT_PEQD_16GT:1;
    UINT32    LC_USC_GO_TO_EQ_16GT:1;
    UINT32    LC_UNEXPECTED_COEFFS_RCVD_16GT:1;
    UINT32    LC_BYPASS_EQ_REQ_PHASE_16GT:1;
    UINT32    LC_FORCE_PRESET_IN_EQ_REQ_PHASE_16GT:1;
    UINT32    LC_FORCE_PRESET_VALUE_16GT:4;
    UINT32    LC_EQTS2_PRESET_EN:1;
    UINT32    LC_EQTS2_PRESET:4;
    UINT32    LC_USE_EQTS2_PRESET:1;
    UINT32    RESERVED_23_27:5;
    UINT32    LC_8GT_EQ_REDO_EN:1;
    UINT32    RESERVED_29:1;
    UINT32    LC_SPC_MODE_16GT:2;
}PCIE_LC_CNTL8_T;

typedef union{
    PCIE_LC_CNTL8_T Reg;
    UINT32          Value;
}PCIE_LC_CNTL8;

/// PCIE_LINK_CNTL2
#define RC_PCIE_LINK_CNTL2_OFFSET   0x88 

typedef union {
  struct {
    UINT32                                       TARGET_LINK_SPEED:4 ; ///<
    UINT32                                        ENTER_COMPLIANCE:1 ; ///<
    UINT32                             HW_AUTONOMOUS_SPEED_DISABLE:1 ; ///<
    UINT32                                   SELECTABLE_DEEMPHASIS:1 ; ///<
    UINT32                                             XMIT_MARGIN:3 ; ///<
    UINT32                                    ENTER_MOD_COMPLIANCE:1 ; ///<
    UINT32                                          COMPLIANCE_SOS:1 ; ///<
    UINT32                                   COMPLIANCE_DEEMPHASIS:4 ; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} PCIE_LINK_CNTL2_STRUCT;

///DXIO PHY register offset
#define LANE_ANA_RX_VERG_CTRL2            0x10fa
#define LANE_DIG_ANA_RX_AFE_CTLE          0x10c0
#define LANE_ANA_RX_AFE_OVER              0x10fb
#define LANE_ANA_RX_AFE_OVER_RTRIM        0x10fc
#define LANE_ANA_RX_AFE_OVER_ICTRL        0x10fd
#define LANE_ANA_RX_CAL_MUXB              0x10f5
#define LANE_ANA_RX_CAL_MUXA              0x10f2
#define LANE_DIG_RX_CDR_CTL_3             0x104C
#define LANE_DIG_RX_CDR_CTL_4             0x104D
#define LANE_DIG_PCS_XF_TX_OVER_IN_3      0x302e

//PCIE_LC_CNTL4
#define PCIE_LC_CNTL4_REG_OFFSET                                  0x02D8
typedef struct{
    UINT32    LC_TX_ENABLE_BEHAVIOUR:2;
    UINT32    LC_DIS_CONTIG_END_SET_CHECK:1;    
    UINT32    LC_DIS_ASPM_L1_IN_SPEED_CHANGE:1; 
    UINT32    LC_BYPASS_EQ:1;
    UINT32    LC_REDO_EQ:1; 
    UINT32    LC_EXTEND_EIEOS:1;    
    UINT32    LC_IGNORE_PARITY:1;   
    UINT32    LC_EQ_SEARCH_MODE:2;  
    UINT32    LC_DSC_CHECK_COEFFS_IN_RLOCK:1;
    UINT32    LC_USC_EQ_NOT_REQD:1; 
    UINT32    LC_USC_GO_TO_EQ:1;
    UINT32    LC_SET_QUIESCE:1;
    UINT32    LC_QUIESCE_RCVD:1;    
    UINT32    LC_UNEXPECTED_COEFFS_RCVD:1;
    UINT32    LC_BYPASS_EQ_REQ_PHASE:1;
    UINT32    LC_FORCE_PRESET_IN_EQ_REQ_PHASE:1;
    UINT32    LC_FORCE_PRESET_VALUE:4;
    UINT32    LC_USC_DELAY_DLLPS:1;
    UINT32    LC_TX_SWING:1;    
    UINT32    LC_EQ_WAIT_FOR_EVAL_DONE:1;
    UINT32    LC_8GT_SKIP_ORDER_EN:1;
    UINT32    LC_WAIT_FOR_MORE_TS_IN_RLOCK:6;
}PCIE_LC_CNTL4_T;

typedef union{
    PCIE_LC_CNTL4_T Reg;
    UINT32          Value;
}PCIE_LC_CNTL4;

#define PCIE_LC_FORCE_COEFF_OFFSET                                 0x02E0
#define PCIE_LC_FORCE_COEFF2_OFFSET                                0x037C

#define PCIE_RC_REGS_SMN_BASE                                      0x11100000
#define PCIE_PORT_REGS_SMN_BASE                                    0x11140000
#define SMN_PCIE0_SUB_BUS_NUMBER_LATENCY                           0x11100018
#define SMN_PCIE1_SUB_BUS_NUMBER_LATENCY                           0x11200018
#define PCIE_VIDDID_ADDR(mmcfg, bus, dev, fun)                     (UINT32)((mmcfg + (bus << 20) + (dev << 15) + (fun << 12)) & 0xFFFFF000)
#define PCIE_SUBSYSTEM_VIDDID_ADDR(mmcfg, bus, dev, fun)           (UINT32)(((mmcfg + (bus << 20) + (dev << 15) + (fun << 12)) & 0xFFFFF000) + 0x2C)
#define PCIE_PORT_SMN_ADDR(base, core, port, offset)               (UINT32)(base + (core << 20) + (port << 12) + offset)
/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
PcieMapPortPciAddress (
  IN      PCIe_ENGINE_CONFIG     *Engine,
  IN      UINT8*                 PortDevMap
  );

VOID
PcieSetPortPciAddressMap (
  IN      GNB_HANDLE    *GnbHandle,
  IN      UINT8         *PortDevMap
  );

VOID
NbioEnableNbifDevice (
  IN        GNB_HANDLE         *GnbHandle,
  IN        UINT8              DeviceType,
  IN        UINT8              StartLane
  );

VOID
NbioDisableNbifDevice (
  IN        GNB_HANDLE         *GnbHandle,
  IN        UINT8              DeviceType,
  IN        UINT8              StartLane
  );

UINT32
DxioAncDataCalcSize (
  IN       DXIO_PORT_DESCRIPTOR    *PortPointer
  );

UINT32
DxioAncDataPopulate (
  IN       DXIO_ENGINE_DESCRIPTOR  *EnginePointer,
  IN       DXIO_PORT_DESCRIPTOR    *PortPointer,
  IN       UINT8                   *AncData
  );

VOID
DxioSetOperatingSpeed (
  IN        GNB_HANDLE                      *GnbHandle,
  IN        PCIe_ENGINE_CONFIG              *PcieEngineList,
  IN        DXIO_ENGINE_DESCRIPTOR          *DxioEnginePointer,
  IN        DXIO_PORT_DESCRIPTOR            *UserCfgPortPointer,
  IN        PEI_AMD_NBIO_SMU_SERVICES_PPI   *SmuServicesPpi
  );

#endif
