/* $NoKeywords:$ */
/**
 * @file
 *
 * SMU services
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
#ifndef _NBIOSMUV9LIB_H_
#define _NBIOSMUV9LIB_H_

#include <NbioRegisterTypes.h>

#define BIOSSMC_Result_OK             0x1                 // The operations was successful.

#define SMC_MSG_TestMessage                0x1
#define SMC_MSG_GetSmuVersion              0x2
#define SMC_MSG_GetBiosIfVersion           0x3
#define SMC_MSG_DfPstateChng               0x4 // DO NOT CHANGE, USED IN DV
#define SMC_MSG_CpuPstateChng              0x5
#define SMC_MSG_ConfigureWaflLinks         0x6
#define SMC_MSG_QueryReadyDramTraining     0x7
#define SMC_MSG_SetSharedDramAddress       0x8
#define SMC_MSG_EnableSmuFeatures          0x9
#define SMC_MSG_PmLogSetDramAddrHigh       0xB
#define SMC_MSG_PmLogSetDramAddrLow        0xC
#define SMC_MSG_DramLogSetDramAddrHigh     0xD
#define SMC_MSG_DramLogSetDramAddrLow      0xE
#define SMC_MSG_DXIO_SERVICE               0xF
#define SMC_MSG_ReadCoreCacWeightRegister  0x10
#define SMC_MSG_DfPerfMonCfg               0x11
#define SMC_MSG_GetThrottlerThresholds     0x12
#define SMC_MSG_S3Entry                    0x13
#define SMC_MSG_EnableHotPlug              0x14
#define SMC_MSG_DisableHotPlug             0x15
#define SMC_MSG_GPIOService                0x16
#define SMC_MSG_SetI2CBusSwitchBaseAddress 0x17
#define SMC_MSG_ConfigureNTBUsageMode      0x18
#define SMC_MSG_EnableNTBOptions           0x19
#define SMC_MSG_DisableNTBOptions          0x1A
#define SMC_MSG_EnableLnkWdthMgt           0x1B
#define SMC_MSG_DisableLnkWdthMgt          0x1C
#define SMC_MSG_SetBiosDramAddrHigh        0x1D
#define SMC_MSG_SetBiosDramAddrLow         0x1E
#define SMC_MSG_SetToolsDramAddrHigh       0x1F
#define SMC_MSG_SetToolsDramAddrLow        0x20
#define SMC_MSG_TransferTableSmu2Dram      0x21
#define SMC_MSG_TransferTableDram2Smu      0x22
#define SMC_MSG_AcBtcStartCal              0x23
#define SMC_MSG_AcBtcStopCal               0x24
#define SMC_MSG_AcBtcEndCal                0x25
#define SMC_MSG_DcBtc                      0x26
#define SMC_MSG_BtcRestoreOnS3Resume       0x27
#define BIOSSMC_MSG_GetNameString          0x28
#define SMC_MSG_CheckTdcCompatibility      0x29
#define SMC_MSG_CheckEdcCompatibility      0x2A
#define SMC_MSG_DramLogSetDramSize         0x2B
#define SMC_MSG_SetPerfmonDramAddressHigh  0x2D
#define SMC_MSG_SetPerfmonDramAddressLow   0x2E
#define SMC_MSG_OptimizeCakeDataLatency    0x33
#define SMC_MSG_IommuInterruptRemap        0x34
#define SMC_MSG_DisablePSI                 0x35
#define SMC_MSG_TDP_TJMAX                  0x36
#define SMC_MSG_OC_DISABLE                 0x37
#define SMC_MSG_OC_VOLTAGEMAX              0x38     //mV
#define SMC_MSG_OC_FREQMAX                 0x39     //Mhz
#define SMC_MSG_TCTL_OFFSET                0x3A
#define SMC_MSG_PCIeInterruptRemap         0x3B
#define SMC_MSG_VMIN_Limiting              0x41
#define BIOSSMC_MSG_GetCpuSerialNumber     0x47
//
#define SMC_MSG_PCIeAERlogRemap            0x3D
#define SMC_MSG_SetHotplugPollInterval     0x3E
#define SMC_MSG_SetHotplugDebugFlags       0x45
#define SMC_MSG_EnableGpio89Polling        0x46

#define SMC_MSG_SET_LCLK				   0x48	    //SetLclk
#define BIOSSMC_MSG_PcieEQParamsOptionFinished  0x49
#define SMC_Message_Count                  0x4a

#define SMC_DXIO_MSG_XGMI_READAPTATION     0x34

// Address
#define MP1_C2PMSG_MESSAGE_ADDRESS                         0x3B10528ul

// Type
#define MP1_C2PMSG_MESSAGE_TYPE                            TYPE_SMN

#define MP1_C2PMSG_MESSAGE_CONTENT_OFFSET                  0
#define MP1_C2PMSG_MESSAGE_CONTENT_WIDTH                   32
#define MP1_C2PMSG_MESSAGE_CONTENT_MASK                    0xffffffffUL

/// MP1_C2PMSG_MESSAGE
typedef union {
  struct {                                                             ///<
    UINT32                                                 CONTENT:32; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} MP1_C2PMSG_MESSAGE_STRUCT;

// Address
#define MP1_C2PMSG_RESPONSE_ADDRESS                        0x3B10564ul

// Type
#define MP1_C2PMSG_RESPONSE_TYPE                           TYPE_SMN

#define MP1_C2PMSG_RESPONSE_CONTENT_OFFSET                 0
#define MP1_C2PMSG_RESPONSE_CONTENT_WIDTH                  32
#define MP1_C2PMSG_RESPONSE_CONTENT_MASK                   0xffffffffUL

/// MP1_C2PMSG_RESPONSE
typedef union {
  struct {                                                             ///<
    UINT32                                                 CONTENT:32; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} MP1_C2PMSG_RESPONSE_STRUCT;


// Address
#define MP1_C2PMSG_ARGUMENT_0_ADDRESS                      0x3B10598ul

// Type
#define MP1_C2PMSG_ARGUMENT_0_TYPE                         TYPE_SMN

#define MP1_C2PMSG_ARGUMENT_0_CONTENT_OFFSET               0
#define MP1_C2PMSG_ARGUMENT_0_CONTENT_WIDTH                32
#define MP1_C2PMSG_ARGUMENT_0_CONTENT_MASK                 0xffffffffUL

/// MP1_C2PMSG_ARGUMENT_0
typedef union {
  struct {                                                             ///<
    UINT32                                                 CONTENT:32; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} MP1_C2PMSG_ARGUMENT_0_STRUCT;


// Address
#define MP1_C2PMSG_ARGUMENT_1_ADDRESS                      0x3B1059cul

// Type
#define MP1_C2PMSG_ARGUMENT_1_TYPE                         TYPE_SMN

#define MP1_C2PMSG_ARGUMENT_1_CONTENT_OFFSET               0
#define MP1_C2PMSG_ARGUMENT_1_CONTENT_WIDTH                32
#define MP1_C2PMSG_ARGUMENT_1_CONTENT_MASK                 0xffffffffUL

/// MP1_C2PMSG_ARGUMENT_1
typedef union {
  struct {                                                             ///<
    UINT32                                                 CONTENT:32; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} MP1_C2PMSG_ARGUMENT_1_STRUCT;


// Address
#define MP1_C2PMSG_ARGUMENT_2_ADDRESS                      0x3B105a0UL

// Type
#define MP1_C2PMSG_ARGUMENT_2_TYPE                         TYPE_SMN

#define MP1_C2PMSG_ARGUMENT_2_CONTENT_OFFSET               0
#define MP1_C2PMSG_ARGUMENT_2_CONTENT_WIDTH                32
#define MP1_C2PMSG_ARGUMENT_2_CONTENT_MASK                 0xffffffff

/// MP1_C2PMSG_ARGUMENT_2
typedef union {
  struct {                                                             ///<
    UINT32                                                 CONTENT:32; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} MP1_C2PMSG_ARGUMENT_2_STRUCT;


// Address
#define MP1_C2PMSG_ARGUMENT_3_ADDRESS                      0x3B105a4UL

// Type
#define MP1_C2PMSG_ARGUMENT_3_TYPE                         TYPE_SMN

#define MP1_C2PMSG_ARGUMENT_3_CONTENT_OFFSET               0
#define MP1_C2PMSG_ARGUMENT_3_CONTENT_WIDTH                32
#define MP1_C2PMSG_ARGUMENT_3_CONTENT_MASK                 0xffffffffUL

/// MP1_C2PMSG_ARGUMENT_3
typedef union {
  struct {                                                             ///<
    UINT32                                                 CONTENT:32; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} MP1_C2PMSG_ARGUMENT_3_STRUCT;


// Address
#define MP1_C2PMSG_ARGUMENT_4_ADDRESS                      0x3B105a8ul

// Type
#define MP1_C2PMSG_ARGUMENT_4_TYPE                         TYPE_SMN

#define MP1_C2PMSG_ARGUMENT_4_CONTENT_OFFSET               0
#define MP1_C2PMSG_ARGUMENT_4_CONTENT_WIDTH                32
#define MP1_C2PMSG_ARGUMENT_4_CONTENT_MASK                 0xffffffffUL

/// MP1_C2PMSG_ARGUMENT_4
typedef union {
  struct {                                                             ///<
    UINT32                                                 CONTENT:32; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} MP1_C2PMSG_ARGUMENT_4_STRUCT;


// Address
#define MP1_C2PMSG_ARGUMENT_5_ADDRESS                      0x3B105acul

// Type
#define MP1_C2PMSG_ARGUMENT_5_TYPE                         TYPE_SMN

#define MP1_C2PMSG_ARGUMENT_5_CONTENT_OFFSET               0
#define MP1_C2PMSG_ARGUMENT_5_CONTENT_WIDTH                32
#define MP1_C2PMSG_ARGUMENT_5_CONTENT_MASK                 0xffffffffUL

/// MP1_C2PMSG_ARGUMENT_5
typedef union {
  struct {                                                             ///<
    UINT32                                                 CONTENT:32; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} MP1_C2PMSG_ARGUMENT_5_STRUCT;


/// SMU hot plug controller flags

#define HP_DISABLE_SIDEBAND            0x01
#define HP_DISABLE_L1_WA               0x02
#define HP_DISABLE_BRIDGE_DIS          0x04
#define HP_DISABLE_IRQ_POLL            0x08
#define HP_DISABLE_IRQ_SET_BRIDGE_DIS  0x10

/// SMU V9 basic lib

VOID
NbioSmuServiceCommonInitArgumentsV9 (
  IN OUT   UINT32                   *SmuArg
  );

UINT32
NbioSmuServiceRequestV9 (
  IN       PCI_ADDR                 NbioPciAddress,
  IN       UINT32                   RequestId,
  IN OUT   UINT32                   *RequestArgument,
  IN       UINT32                   AccessFlags
  );

AGESA_STATUS
NbioSmuFirmwareTestV9 (
  IN       GNB_HANDLE               *NbioHandle
  );

AGESA_STATUS
NbioDxioServiceRequestV9 (
  IN       GNB_HANDLE               *NbioHandle,
  IN       UINT32                   RequestId,
  IN OUT   UINT32                   *RequestArgument,
  IN       UINT32                   AccessFlags
  );

VOID
SmuNotifyS3Entry (
  );

#endif
