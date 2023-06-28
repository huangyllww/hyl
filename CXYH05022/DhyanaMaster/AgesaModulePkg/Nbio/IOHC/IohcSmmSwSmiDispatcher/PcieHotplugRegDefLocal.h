/* $NoKeywords:$ */

/**
 * @file
 *
 * IOHC Pcie Hotplug Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      Agesa
 * @e sub-project:  IOHC
 * @e \$Revision: 000001 $   @e \$Date: 2022-05-14 10:28:05  (Sat, 14 May 2022) $
 *
 */
/*****************************************************************************
*
 *
 * Copyright 2016 - 2022 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
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

#ifndef __PCIE_HOTPLUG_REG_DEF_LOCAL_H__
#define __PCIE_HOTPLUG_REG_DEF_LOCAL_H__

/// PCIE_LINK_CAP
#define RC_PCIE_LINK_CAP_OFFSET  0x64
typedef union {
  struct {
    UINT32    LINK_SPEED                  : 4;
    UINT32    LINK_WIDTH                  : 6;
    UINT32    PM_SUPPORT                  : 2;
    UINT32    L0S_EXIT_LATENCY            : 3;
    UINT32    L1_EXIT_LATENCY             : 3;
    UINT32    CLOCK_POWER_MANAGEMENT      : 1;
    UINT32    SURPRISE_DOWN_ERR_ERPORTING : 1;
    UINT32    DL_ACTIVE_ERPORTING_CAPABLE : 1;
    UINT32    LINK_BW_NOTFICATION_CAP     : 1;
    UINT32    ASM_OPTIONALITY_COMPLIANCE  : 1;
    UINT32    RESERVER_23                 : 1;
    UINT32    PORT_NUMBER                 : 8;
  } Field;
  UINT32    Value;
} PCIE_LINK_CAP_STRUCT;

/// PCIE_LINK_STATUS
#define RC_PCIE_LINK_STATUS_OFFSET  0x68
typedef union {
  struct {
    UINT32    reserved                  : 16;                          ///<
    UINT32    CURRENT_LINK_SPEED        : 4;                           ///<
    UINT32    NEGOTIATED_LINK_WIDTH     : 6;                           ///<
    UINT32    reserved_10               : 1;                           ///<
    UINT32    LINK_TRAINING             : 1;                           ///<
    UINT32    SLOT_CLOCK_CFG            : 1;                           ///<
    UINT32    DL_ACTIVE                 : 1;                           ///<
    UINT32    LINK_BW_MANAGEMENT_STATUS : 1;                           ///<
    UINT32    LINK_AUTONOMOUS_BW_STATUS : 1;                           ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIE_LINK_STATUS_STRUCT;

/// PCIE_LINK_CNTL
#define RC_PCIE_LINK_CNTL_OFFSET  0x68
typedef union {
  struct {
    UINT32    PM_CONTROL                  : 2;
    UINT32    RESERVED_2                  : 1;
    UINT32    READ_CPL_BOUNDARY           : 1;
    UINT32    LINK_DIS                    : 1;
    UINT32    RETRAIN_LINK                : 1;
    UINT32    COMMON_CLOCK_CFG            : 1;
    UINT32    EXTENDED_SYNC               : 1;
    UINT32    CLOCK_POWER_MANAGEMENT_EN   : 1;
    UINT32    HW_AUTONOMOUS_WIDTH_DISABLE : 1;
    UINT32    LINK_BW_MANAGEMENT_INT_EN   : 1;
    UINT32    LINK_AUTONMOUS_BW_INT_EN    : 1;
    UINT32    RESERVED_12_13              : 2;
    UINT32    DRS_SIGNALING_CONTROL       : 2;
    UINT32    reserved                    : 16;
  } Field;
  UINT32    Value;
} PCIE_LINK_CNTL_STRUCT;

/// PCIE_LINK_CNTL3
#define RC_PCIE_LINK_CNTL3_OFFSET  0x274
typedef union {
  struct {
    UINT32    PERFORM_EQUALIZATION         : 1;
    UINT32    LINK_EQUALIZATION_REQ_INT_EN : 1;
    UINT32    RESERVED_2_8                 : 7;
    UINT32    ENABLE_LOWER_SKIP_OS_GEN     : 7;
    UINT32    reserved                     : 16;
  } Field;
  UINT32    Value;
} PCIE_LINK_CNTL3_STRUCT;

#define RC_PCIE_DEVICE_CNTL_AND_STATUS_OFFSET  0x60  /// RC_PCIE_DEVICE_STATUS
#define RC_PCIE_UNCORR_ERR_STATUS_OFFSET       0x154 /// RC_PCIE_UNCORR_ERR_STATUS
#define RC_PCIE_CORR_ERR_STATUS_OFFSET         0x160 /// RC_PCIE_CORR_ERR_STATUS
#define RC_PCIE_ROOT_ERR_STATUS_OFFSET         0x180 /// RC_PCIE_ROOT_ERR_STATUS
#define RC_PCIE_LANE_ERR_STATUS_OFFSET         0x278 /// RC_PCIE_LANE_ERR_STATUS

/// PCIE_LC_CNTL8
#define PCIE_PORT_REGS_SMN_BASE   0x11140000
#define PCIE_LC_CNTL8_REG_OFFSET  0x0374
typedef struct {
  UINT32    LC_EQ_SEARCH_MODE_16GT               : 2;
  UINT32    LC_BYPASS_EQ_16GT                    : 1;
  UINT32    LC_BYPASS_EQ_PRESET_16GT             : 4;
  UINT32    LC_REDO_EQ_16GT                      : 1;
  UINT32    LC_USC_EQ_NOT_PEQD_16GT              : 1;
  UINT32    LC_USC_GO_TO_EQ_16GT                 : 1;
  UINT32    LC_UNEXPECTED_COEFFS_RCVD_16GT       : 1;
  UINT32    LC_BYPASS_EQ_REQ_PHASE_16GT          : 1;
  UINT32    LC_FORCE_PRESET_IN_EQ_REQ_PHASE_16GT : 1;
  UINT32    LC_FORCE_PRESET_VALUE_16GT           : 4;
  UINT32    LC_EQTS2_PRESET_EN                   : 1;
  UINT32    LC_EQTS2_PRESET                      : 4;
  UINT32    LC_USE_EQTS2_PRESET                  : 1;
  UINT32    RESERVED_23_27                       : 5;
  UINT32    LC_8GT_EQ_REDO_EN                    : 1;
  UINT32    RESERVED_29                          : 1;
  UINT32    LC_SPC_MODE_16GT                     : 2;
} PCIE_LC_CNTL8_T;

typedef union {
  PCIE_LC_CNTL8_T    Reg;
  UINT32             Value;
} PCIE_LC_CNTL8;

#define PCIE0_LC_SPEED_CNTL_ADDRESS  0x11140290
#define PCIE1_LC_SPEED_CNTL_ADDRESS  0x11240290

/// PCIE_LC_SPEED_CNTL
typedef union {
  struct {
    UINT32    LC_GEN2_EN_STRAP                     : 1;                ///<
    UINT32    LC_GEN3_EN_STRAP                     : 1;                ///<
    UINT32    LC_TARGET_LINK_SPEED_OVERRIDE_EN     : 1;                ///<
    UINT32    LC_TARGET_LINK_SPEED_OVERRIDE        : 2;                ///<
    UINT32    LC_FORCE_EN_SW_SPEED_CHANGE          : 1;                ///<
    UINT32    LC_FORCE_DIS_SW_SPEED_CHANGE         : 1;                ///<
    UINT32    LC_FORCE_EN_HW_SPEED_CHANGE          : 1;                ///<
    UINT32    LC_FORCE_DIS_HW_SPEED_CHANGE         : 1;                ///<
    UINT32    LC_INITIATE_LINK_SPEED_CHANGE        : 1;                ///<
    UINT32    LC_SPEED_CHANGE_ATTEMPTS_ALLOWED     : 2;                ///<
    UINT32    LC_SPEED_CHANGE_ATTEMPT_FAILED       : 1;                ///<
    UINT32    LC_CURRENT_DATA_RATE                 : 2;                ///<
    UINT32    LC_DONT_CLR_TARGET_SPD_CHANGE_STATUS : 1;                ///<
    UINT32    LC_CLR_FAILED_SPD_CHANGE_CNT         : 1;                ///<
    UINT32    LC_1_OR_MORE_TS2_SPEED_ARC_EN        : 1;                ///<
    UINT32    LC_OTHER_SIDE_EVER_SENT_GEN2         : 1;                ///<
    UINT32    LC_OTHER_SIDE_SUPPORTS_GEN2          : 1;                ///<
    UINT32    LC_OTHER_SIDE_EVER_SENT_GEN3         : 1;                ///<
    UINT32    LC_OTHER_SIDE_SUPPORTS_GEN3          : 1;                ///<
    UINT32    LC_AUTO_RECOVERY_DIS                 : 1;                ///<
    UINT32    LC_SPEED_CHANGE_STATUS               : 1;                ///<
    UINT32    LC_DATA_RATE_ADVERTISED              : 2;                ///<
    UINT32    LC_CHECK_DATA_RATE                   : 1;                ///<
    UINT32    LC_MULT_UPSTREAM_AUTO_SPD_CHNG_EN    : 1;                ///<
    UINT32    LC_INIT_SPEED_NEG_IN_L0s_EN          : 1;                ///<
    UINT32    LC_INIT_SPEED_NEG_IN_L1_EN           : 1;                ///<
    UINT32    LC_DONT_CHECK_EQTS_IN_RCFG           : 1;                ///<
    UINT32    LC_DELAY_COEFF_UPDATE_DIS            : 1;                ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIE_LC_SPEED_CNTL_STRUCT;

/// PCIE_LINK_CNTL2
#define RC_PCIE_LINK_CNTL2_OFFSET  0x88

typedef union {
  struct {
    UINT32    TARGET_LINK_SPEED           : 4;                         ///<
    UINT32    ENTER_COMPLIANCE            : 1;                         ///<
    UINT32    HW_AUTONOMOUS_SPEED_DISABLE : 1;                         ///<
    UINT32    SELECTABLE_DEEMPHASIS       : 1;                         ///<
    UINT32    XMIT_MARGIN                 : 3;                         ///<
    UINT32    ENTER_MOD_COMPLIANCE        : 1;                         ///<
    UINT32    COMPLIANCE_SOS              : 1;                         ///<
    UINT32    COMPLIANCE_DEEMPHASIS       : 4;                         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIE_LINK_CNTL2_STRUCT;

/// DXIO PHY register offset
#define LANE_ANA_RX_VERG_CTRL2      0x10fa
#define LANE_DIG_ANA_RX_AFE_CTLE    0x10c0
#define LANE_ANA_RX_AFE_OVER        0x10fb
#define LANE_ANA_RX_AFE_OVER_RTRIM  0x10fc
#define LANE_ANA_RX_AFE_OVER_ICTRL  0x10fd
#define LANE_ANA_RX_CAL_MUXB        0x10f5
#define LANE_ANA_RX_CAL_MUXA        0x10f2
#define LANE_DIG_RX_CDR_CTL_3       0x104C
#define LANE_DIG_RX_CDR_CTL_4       0x104D

// Address
#define PCIE_LC_STATE0_ADDRESS   0x11140294
#define PCIE0_LC_STATE0_ADDRESS  0x11140294
#define PCIE1_LC_STATE0_ADDRESS  0x11240294

/// PCIE_LC_STATE0
typedef union {
  struct {
    UINT32    LC_CURRENT_STATE : 6;                                    ///<
    UINT32    Reserved0        : 2;                                    ///<
    UINT32    LC_PREV_STATE1   : 6;                                    ///<
    UINT32    Reserved1        : 2;                                    ///<
    UINT32    LC_PREV_STATE2   : 6;                                    ///<
    UINT32    Reserved2        : 2;                                    ///<
    UINT32    LC_PREV_STATE3   : 6;                                    ///<
    UINT32    Reserved3        : 2;                                    ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIE_LC_STATE0_STRUCT;

// PCIE_LC_CNTL4
#define PCIE_LC_CNTL4_REG_OFFSET  0x02D8
typedef struct {
  UINT32    LC_TX_ENABLE_BEHAVIOUR          : 2;
  UINT32    LC_DIS_CONTIG_END_SET_CHECK     : 1;
  UINT32    LC_DIS_ASPM_L1_IN_SPEED_CHANGE  : 1;
  UINT32    LC_BYPASS_EQ                    : 1;
  UINT32    LC_REDO_EQ                      : 1;
  UINT32    LC_EXTEND_EIEOS                 : 1;
  UINT32    LC_IGNORE_PARITY                : 1;
  UINT32    LC_EQ_SEARCH_MODE               : 2;
  UINT32    LC_DSC_CHECK_COEFFS_IN_RLOCK    : 1;
  UINT32    LC_USC_EQ_NOT_REQD              : 1;
  UINT32    LC_USC_GO_TO_EQ                 : 1;
  UINT32    LC_SET_QUIESCE                  : 1;
  UINT32    LC_QUIESCE_RCVD                 : 1;
  UINT32    LC_UNEXPECTED_COEFFS_RCVD       : 1;
  UINT32    LC_BYPASS_EQ_REQ_PHASE          : 1;
  UINT32    LC_FORCE_PRESET_IN_EQ_REQ_PHASE : 1;
  UINT32    LC_FORCE_PRESET_VALUE           : 4;
  UINT32    LC_USC_DELAY_DLLPS              : 1;
  UINT32    LC_TX_SWING                     : 1;
  UINT32    LC_EQ_WAIT_FOR_EVAL_DONE        : 1;
  UINT32    LC_8GT_SKIP_ORDER_EN            : 1;
  UINT32    LC_WAIT_FOR_MORE_TS_IN_RLOCK    : 6;
} PCIE_LC_CNTL4_T;

typedef union {
  PCIE_LC_CNTL4_T    Reg;
  UINT32             Value;
} PCIE_LC_CNTL4;

/// LC_CNTL_10
#define PCIE0_LC_CNTL_10_ADDRESS  0x1114038c
#define PCIE1_LC_CNTL_10_ADDRESS  0x1124038c

typedef union {
  struct {
    UINT32    LC_LINK_DOWN_SPD_CHG_EN                       : 1;
    UINT32    LC_CLR_DELAY_DLLP_WHEN_NO_AUTO_EQ             : 1;
    UINT32    LC_MULT_AUTO_SPD_CHG_ON_LAST_RATE             : 1;
    UINT32    LC_CLEAR_RXSTANDBY_ON_RATE_UPDATE_ONLY        : 1;
    UINT32    LC_POWERDOWN_P0_WAIT_FOR_REFCLKACK_ON_L1_EXIT : 1;
    UINT32    RESERVE_5_7                                   : 3;
    UINT32    LC_TARGET_LINK_SPEED_OVERRIDE_EN              : 1;
    UINT32    LC_TARGET_LINK_SPEED_OVERRIDE                 : 2;
    UINT32    RESERVE_11_15                                 : 5;
    UINT32    LC_CLEAR_REVERSE_ATTEMPT_IN_L0                : 1;
    UINT32    LC_LOCK_REVERSAL                              : 1;
    UINT32    RESERVE_18                                    : 1;
    UINT32    LC_WAIT_FOR_LANES_IN_CONFIG                   : 1;
    UINT32    LC_REQ_COEFFS_FOR_TXMARGIN_EN                 : 1;
    UINT32    LC_AUTO_REJECT_AFTER_TIMEOUT                  : 1;
    UINT32    LC_ESM_RATES                                  : 2;
    UINT32    LC_ESM_ENTRY_MODE                             : 1;
    UINT32    RESERVE_25_29                                 : 5;
    UINT32    LC_OTHER_SIDE_EVER_SENT_GEN4                  : 1;
    UINT32    LC_OTHER_SIDE_SUPPORTS_GEN4                   : 1;
  } Field;
  UINT32    Value;
} PCIE_LC_CNTL_10;

/// PCIE_CE_MASK
/// Address
#define HP_PCIE0_CE_MASK_ADDRESS  0x11100164
#define HP_PCIE1_CE_MASK_ADDRESS  0x11200164

/// PCIE_SLOT_CNTL
#define HP_RC_PCIE_SLOT_CNTL_REG_OFFSET    0x70
#define HP_RC_PCIE_SLOT_STATUS_REG_OFFSET  0x72 //SGEZB#24403:Inject PCIe Error Fail
typedef union {
  struct {
    UINT32    reserved                     : 16;                       ///<
    UINT32    ATTN_BUTTON_PRESSED          : 1;                        ///<
    UINT32    PWR_FAULT_DETECTED           : 1;                        ///<
    UINT32    MRL_SENSOR_CHANGED           : 1;                        ///<
    UINT32    PRESENCE_DETECT_CHANGED      : 1;                        ///<
    UINT32    COMMAND_COMPLETED            : 1;
    UINT32    MRL_SENSOR_STATE             : 1;                        ///<
    UINT32    PRESENCE_DETECT_STATE        : 1;                        ///<
    UINT32    ELECTROMECH_INTERLOCK_STATUS : 1;                        ///<
    UINT32    DL_STATE_CHANGED             : 1;                        ///<
    UINT32    reserved_1                   : 7;                        ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIE_SLOT_STATUS_STRUCT;

/// SMN_0x11180008_ADDRESS
/// Address
#define PCIE_LC_PORT_ORDER                    0x0320
#define PCIE_LC_PORT_ORDER_CNTL               0x00F8
#define HP_SMN_IOHC_GLOBAL_LO_STATUS_ADDRESS  0x13b20020
#define HP_SMN_IOHC_GLOBAL_HI_STATUS_ADDRESS  0x13b20024
#define HP_SMN_0x111401a8_ADDRESS             0x111401a8
#define HP_SMN_0x112401a8_ADDRESS             0x112401a8
#define HP_SMN_0x11180008_ADDRESS             0x11180008
#define HP_SMN_0x11280008_ADDRESS             0x11280008
#define HP_SMN_0x111402ec_ADDRESS             0x111402ec
#define HP_SMN_0x112402ec_ADDRESS             0x112402ec
#define PCIE_RC_REGS_SMN_BASE                 0x11100000
#define PCIE_PORT_REGS_SMN_BASE               0x11140000
#define PCIE_CORE_REGS_SMN_BASE               0x11180000
#define SMN_PCIE0_SUB_BUS_NUMBER_LATENCY      0x11100018
#define SMN_PCIE1_SUB_BUS_NUMBER_LATENCY      0x11200018
#define PCIE_VIDDID_ADDR(mmcfg, bus, dev, fun)            (UINT32)((mmcfg + (bus << 20) + (dev << 15) + (fun << 12)) & 0xFFFFF000)
#define PCIE_SUBSYSTEM_VIDDID_ADDR(mmcfg, bus, dev, fun)  (UINT32)(((mmcfg + (bus << 20) + (dev << 15) + (fun << 12)) & 0xFFFFF000) + 0x2C)
#define PCIE_PORT_SMN_ADDR(base, core, port, offset)      (UINT32)(base + (core << 20) + (port << 12) + offset)

#endif
