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
/******************************************************************************
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
****************************************************************************/
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include "PcieHpCommon.h"
#include "IohcSmmSwSmiDispatcher.h"
#include "PcieHotplugRegDefLocal.h"

enum {
  LANE_ANA_RX_VERG_CTRL2_INDEX = 0,
  LANE_DIG_ANA_RX_AFE_CTLE_INDEX,
  LANE_ANA_RX_AFE_OVER_INDEX,
  LANE_ANA_RX_AFE_OVER_RTRIM_INDEX,
  LANE_ANA_RX_AFE_OVER_ICTRL_INDEX,
  LANE_ANA_RX_CAL_MUXB_INDEX,
  LANE_ANA_RX_CAL_MUXA_INDEX,
  LANE_DIG_RX_CDR_CTL_3_INDEX,
  LANE_DIG_RX_CDR_CTL_4_INDEX
};

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
UINT16  PhyParameterInitTable_Hygon_Cx[9] = {
  // VERG_CTRL2,AFE_CTLE,AFE_OVER_RTRIM,AFE_OVER_ICTRL,CAL_MUXB,CAL_MUXA,CDR_CTL_3_3_5,CTL_4_0_2
  0xa8, 0x98, 0x0, 0x0, 0x0, 0x0, 0x0, 0x00d6, 0x2cdb
};

UINT16  PhyParameterInitTable_Hygon_Dx[9] = {
  // VERG_CTRL2,AFE_CTLE,AFE_OVER_RTRIM,AFE_OVER_ICTRL,CAL_MUXB,CAL_MUXA,CDR_CTL_3_3_5,CTL_4_0_2
  0xa8, 0x98, 0x0, 0x0, 0x0, 0x0, 0x0, 0x00d6, 0x2cdb
};

UINT16  PhyParameterTable_Hygon_Cx[3][9] = {
  // VERG_CTRL2,AFE_CTLE,AFE_OVER_RTRIM,AFE_OVER_ICTRL,CAL_MUXB,CAL_MUXA,CDR_CTL_3_3_5,CTL_4_0_2
  { 0,    0,    0,    0,    0,    0,    0,    0,   0   },// DM1
  { 0xac, 0x18, 0xa6, 0x7d, 0x07, 0x48, 0x0d, 0x1, 0x1 },// SL1
  { 0xac, 0x18, 0xa2, 0xff, 0x07, 0x48, 0x1d, 0x1, 0x1 },// SL1R2
};

UINT16  PhyParameterTable_Hygon_Dx[2][9] = {
  // VERG_CTRL2,AFE_CTLE,AFE_OVER_RTRIM,AFE_OVER_ICTRL,CAL_MUXB,CAL_MUXA,CDR_CTL_3_3_5,CTL_4_0_2
  { 0, 0,    0, 0, 0, 0, 0, 0x1, 0x1 },// DM1
  { 0, 0x3c, 0, 0, 0, 0, 0, 0x1, 0x1 },// SL1
};

/**
 * Read PHY register
 * @param[in]  Die            Die number
 * @param[in]  DxioType       0=GOP TYPE A /1=GOP TYPE B /2=PCIE TYPE A /3=PCIE TYPE B /4=SATA
 * @param[in]  Lane           lane number
 * @param[in]  RegAddress     PHY register address
 * @param[in]  Value           Read value
 * @retval
 */
EFI_STATUS
HpPhyRegRead (
  UINT8                     Die,
  UINT8                     DxioType,
  UINT8                     Lane,
  UINT32                    RegAddress,
  UINT16                    *Value
  )
{
  UINT32  Data32;
  UINT32  SmnAddr;
  UINT8   Phy;
  UINT8   PhyLane;

  if(Lane > 3) {
    Phy     = Lane/4+1;
    PhyLane = Lane%4;
  } else {
    Phy     = Lane/2;
    PhyLane = Lane%2;
  }

  SmnAddr = 0x12000000+(DxioType<<20)+((Phy*2)<<16)+(RegAddress+(PhyLane<<8))*2;
  EqSmnRead (Die, SmnAddr&0xFFFFFFFC, &Data32);
  if ((SmnAddr & 3) == 2) {
    *Value = (UINT16)(Data32 >> 16);
  } else if ((SmnAddr & 3) == 0) {
    *Value = (UINT16)Data32;
  } else {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Write PHY register
 * @param[in]  Die            Die number
 * @param[in]  DxioType       0=GOP TYPE A /1=GOP TYPE B /2=PCIE TYPE A /3=PCIE TYPE B /4=SATA
 * @param[in]  Lane           lane number
 * @param[in]  RegAddress     PHY register address
 * @param[in]  Value           Write value
 * @retval
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
HpPhyRegWrite (
  UINT8                     Die,
  UINT8                     DxioType,
  UINT8                     Lane,
  UINT32                    RegAddress,
  UINT16                    Value
  )
{
  UINT32  Data32;
  UINT32  SmnAddr;
  UINT8   Phy;
  UINT8   PhyLane;

  if(Lane > 3) {
    Phy     = Lane / 4 + 1;
    PhyLane = Lane % 4;
  } else {
    Phy     = Lane / 2;
    PhyLane = Lane % 2;
  }

  SmnAddr = 0x12000000 + (DxioType << 20) + ((Phy*2) << 16) + (RegAddress + (PhyLane <<8)) * 2;
  EqSmnRead (Die, SmnAddr & 0xFFFFFFFC, &Data32);
  if((SmnAddr&3) == 2) {
    Data32 &= 0xFFFF;
    Data32 |= (Value<<16);
  } else if ((SmnAddr&3) == 0) {
    Data32 &= 0xFFFF0000;
    Data32 |= Value;
  } else {
    return EFI_INVALID_PARAMETER;
  }

  EqSmnWrite (Die, SmnAddr & 0xFFFFFFFC, &Data32);

  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Retrain Pcie Without EQ
 * @param[in]  Die            Die Number
 * @param[in]  Port           Port Number
 *
 * @retval
 */
/*----------------------------------------------------------------------------------------*/
VOID
HpPcieRetrainingWithOutEq (
  IN     UINT8                 Die,
  IN     UINT8                 Port
  )
{
  PCIE_LINK_CNTL_STRUCT  LinkCtl;
  UINT32                 PcieCore;
  UINT32                 MappingPortID;

  PcieCore = Port / 8;
  MappingPortID = Port % 8;

  // force retrain link
  EqSmnRead (Die, PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CNTL_OFFSET, &LinkCtl.Value);
  LinkCtl.Field.RETRAIN_LINK = 1;
  EqSmnWrite (Die, PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CNTL_OFFSET, &LinkCtl.Value);
}

/*----------------------------------------------------------------------------------------*/

/**
 * Retrain Pcie Without EQ
 * @param[in]  Die            Die Number
 * @param[in]  Port           Port Number
 *
 * @retval
 */
/*----------------------------------------------------------------------------------------*/
BOOLEAN
HpPerformEqualizationSetting (
  IN     UINT8                  Die,
  IN     UINT8                  Port,
  IN     BOOLEAN                PerformEq
  )
{
  PCIE_LINK_CNTL3_STRUCT  LinkCtl3;
  UINT8                   PcieCore;
  UINT32                  MappingPortID;
  UINT32                  DoEq;
  BOOLEAN                 OldPerformEqStatus;

  PcieCore = Port / 8;
  MappingPortID = Port % 8;

  if (PerformEq) {
    DoEq = 1;
  } else {
    DoEq = 0;
  }

  EqSmnRead (Die, PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CNTL3_OFFSET, &LinkCtl3.Value);
  OldPerformEqStatus = (LinkCtl3.Field.PERFORM_EQUALIZATION == 1) ? TRUE : FALSE;
  LinkCtl3.Field.PERFORM_EQUALIZATION = DoEq;
  EqSmnWrite (Die, PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CNTL3_OFFSET, &LinkCtl3.Value);

  return OldPerformEqStatus;
}

/*----------------------------------------------------------------------------------------*/

/**
 * PCIe Retraining
 *
 * @param[in]  Die            Die number
 * @param[in]  Port           PCIe Port number
 * @param[in]  ForceEq        Always Do EQ
 * @retval    void
 */
/*----------------------------------------------------------------------------------------*/
VOID
HpPcieRetraining (
  IN     UINT8                  Die,
  IN     UINT8                  Port,
  IN     BOOLEAN                ForceEq
  )
{
  PCIE_LINK_CNTL_STRUCT  LinkCtl;
  UINT8                  PcieCore;
  UINT32                 MappingPortID;
  BOOLEAN                OldPerformEqStatus;

  PcieCore = Port / 8;
  MappingPortID = Port % 8;

  OldPerformEqStatus = HpPerformEqualizationSetting (Die, Port, ForceEq);

  // force retrain link
  EqSmnRead (Die, PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CNTL_OFFSET, &LinkCtl.Value);
  LinkCtl.Field.RETRAIN_LINK = 1;
  EqSmnWrite (Die, PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CNTL_OFFSET, &LinkCtl.Value);

  if (ForceEq) {
    // delay 50ms
    MicroSecondDelay (50*1000);
  }

  HpPerformEqualizationSetting (Die, Port, OldPerformEqStatus);
}

/*----------------------------------------------------------------------------------------*/

/**
 * PCIe Wait to L0
 *
 * @param[in]  Die            Die number
 * @param[in]  Port           PCIe Port number
 *
 * @retval    TURE  PCIe Check L0 success
 *            FALSE PCIe Check L0 fail
 */
/*----------------------------------------------------------------------------------------*/
BOOLEAN
HpPcieCheckL0 (
  IN     UINT8                     Die,
  IN     UINT8                     Port
  )
{
  UINT8                  PcieCore;
  UINT32                 MappingPortID;
  PCIE_LC_STATE0_STRUCT  LcState0;

  PcieCore = Port / 8;
  MappingPortID = Port % 8;

  EqSmnRead (Die, PCIE0_LC_STATE0_ADDRESS + (PcieCore << 20) + (MappingPortID << 12), &LcState0.Value);

  return ((UINT8)LcState0.Field.LC_CURRENT_STATE == 0x10) ? TRUE : FALSE;
}

/*----------------------------------------------------------------------------------------*/

/**
 * PCIe Wait to L0
 *
 * @param[in]  Die            Die number
 * @param[in]  Port           PCIe Port number
 * @param[in]  MaxDelayMs     Max Delay Times to L0(ms)
 *
 * @retval    TURE  PCIe wait L0 success
 *            FALSE PCIe wait L0 fail
 */
/*----------------------------------------------------------------------------------------*/
BOOLEAN
HpPcieWaitToL0 (
  IN     UINT8                      Die,
  IN     UINT8                      Port,
  IN     UINT32                     MaxDelayMs
  )
{
  UINT32                 i;
  UINT8                  PcieCore;
  UINT32                 MappingPortID;
  PCIE_LC_STATE0_STRUCT  LcState0;
  BOOLEAN                WaitStatus = FALSE;

  PcieCore = Port / 8;
  MappingPortID = Port % 8;

  for (i = 0; i < MaxDelayMs; i++) {
    MicroSecondDelay (1000);
    EqSmnRead (Die, PCIE0_LC_STATE0_ADDRESS + (PcieCore << 20) + (MappingPortID << 12), &LcState0.Value);
    if ((UINT8)LcState0.Field.LC_CURRENT_STATE == 0x10) {
      WaitStatus = TRUE;
      break;
    }
  }

  if (WaitStatus) {
    // DEBUG ((EFI_D_INFO, "Wait to L0 Success, Wait %d ms\n", i));
  } else {
    // DEBUG ((EFI_D_INFO, "Wait to L0 Fail, Time out\n", i));
  }

  return WaitStatus;
}

/*----------------------------------------------------------------------------------------*/

/**
 * PCIe Retry to L0
 *
 * @param[in]  Die                  Die number
 * @param[in]  Port                 PCIe Port number
 * @param[in]  RetryCount           Retry Count
 * @param[in]  MaxDelayMsPerTry     Max Delay Times to L0(ms) PreTry
 *
 * @retval    TURE  PCIe Retry to L0 success
 *            FALSE PCIe Retry to L0 fail
 */
/*----------------------------------------------------------------------------------------*/
BOOLEAN
HpPcieRetryToL0 (
  IN     UINT8                      Die,
  IN     UINT8                      Port,
  IN     UINT32                     RetryCount,
  IN     UINT32                     MaxDelayMsPerTry
  )
{
  UINT32   i;
  UINT32   PcieCore;
  UINT32   MappingPortID;
  BOOLEAN  RetryStatus = FALSE;

  PcieCore = Port / 8;
  MappingPortID = Port % 8;

  for (i = 0; i < RetryCount; i++) {
    // Reqo EQ
    HpPcieRetraining (Die, Port, TRUE);
    // wait to L0
    if (HpPcieWaitToL0 (Die, Port, MaxDelayMsPerTry)) {
      RetryStatus = TRUE;
    }
  }

  if (RetryStatus) {
    // DEBUG ((EFI_D_INFO, "Retry to L0 Success, Retry count = %d\n", i));
  } else {
    // DEBUG ((EFI_D_INFO, "Retry to L0 Fail, Retry count = %d\n", RetryCount));
  }

  return RetryStatus;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Pcie Enable All Error Report
 *
 * @param[in]  Die                  Die number
 * @param[in]  Port                 PCIe Port number
 * @param[in]  Enable               Enable or Disable
 *
 * @retval    VOID
 */
/*----------------------------------------------------------------------------------------*/
VOID
HpPcieEnableAllErrorReport (
  IN     UINT8                      Die,
  IN     UINT8                      Port,
  IN     BOOLEAN                    Enable
  )
{
  UINT32  PcieCore;
  UINT32  MappingPortID;
  UINT32  Data32;
  UINT32  Cntl;

  PcieCore = Port / 8;
  MappingPortID = Port % 8;
  Cntl = Enable ? 0 : 1;

  EqSmnRead (Die, HP_SMN_0x111401a8_ADDRESS + (PcieCore << 20) + (MappingPortID << 12), &Data32);
  Data32 &= ~BIT0;
  Data32 |= Cntl;
  EqSmnWrite (Die, HP_SMN_0x111401a8_ADDRESS + (PcieCore << 20) + (MappingPortID << 12), &Data32);
}

/*----------------------------------------------------------------------------------------*/

/**
 * PCIe Clear All errors
 *
 * @param[in]  Die                  Die number
 * @param[in]  Port                 PCIe Port number
 *
 * @retval    VOID
 */
/*----------------------------------------------------------------------------------------*/
VOID
HpPcieClearAllErrors (
  IN     UINT8                     Die,
  IN     UINT8                     Port
  )
{
  UINT32  PcieCore;
  UINT32  MappingPortID;
  UINT32  Data32;

  PcieCore = Port / 8;
  MappingPortID = Port % 8;

  // 1. clear CE/UCE/Root Errors
  EqSmnRead (Die, PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_UNCORR_ERR_STATUS_OFFSET, &Data32);
  EqSmnWrite (Die, PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_UNCORR_ERR_STATUS_OFFSET, &Data32);
  EqSmnRead (Die, PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_UNCORR_ERR_STATUS_OFFSET, &Data32);

  EqSmnRead (Die, PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_CORR_ERR_STATUS_OFFSET, &Data32);
  EqSmnWrite (Die, PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_CORR_ERR_STATUS_OFFSET, &Data32);
  EqSmnRead (Die, PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_CORR_ERR_STATUS_OFFSET, &Data32);

  EqSmnRead (Die, PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_ROOT_ERR_STATUS_OFFSET, &Data32);
  EqSmnWrite (Die, PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_ROOT_ERR_STATUS_OFFSET, &Data32);
  EqSmnRead (Die, PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_ROOT_ERR_STATUS_OFFSET, &Data32);

  // 2. clear device status errors
  EqSmnRead (Die, PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_DEVICE_CNTL_AND_STATUS_OFFSET, &Data32);
  EqSmnWrite (Die, PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_DEVICE_CNTL_AND_STATUS_OFFSET, &Data32);
  EqSmnRead (Die, PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_DEVICE_CNTL_AND_STATUS_OFFSET, &Data32);

  // 3. clear lane errors
  EqSmnRead (Die, PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_LANE_ERR_STATUS_OFFSET, &Data32);
  EqSmnWrite (Die, PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_LANE_ERR_STATUS_OFFSET, &Data32);
  EqSmnRead (Die, PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_LANE_ERR_STATUS_OFFSET, &Data32);

  return;
}

/*----------------------------------------------------------------------------------------*/

/**
 * PCIe Link Speed change
 *
 * @param[in]  Die            Die number
 * @param[in]  Port           PCIe Port number
 * @param[in]  NewSpeed       Link Speed change value
 * @param[in]  OldSpeed       Old Link Speed value
 *
 * @retval    VOID
 */
/*----------------------------------------------------------------------------------------*/
VOID
HpPcieSpeedChange (
  IN     UINT8                      Die,
  IN     UINT8                      Port,
  IN     UINT8                      NewSpeed,
  OUT    UINT8                      *OldSpeed
  )
{
  PCIE_LINK_CNTL2_STRUCT   LinkCtl2;
  UINT32                   PcieCore;
  UINT32                   MappingPortID;
  PCIE_LINK_CNTL_STRUCT    LinkCtl;
  PCIE_LINK_STATUS_STRUCT  LinkStatus;
  UINT32                   SmnAddress;

  PcieCore = Port / 8;
  MappingPortID = Port % 8;

  // read current link speed
  if (OldSpeed != NULL) {
    SmnAddress = PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_LINK_STATUS_OFFSET;
    EqSmnRead (Die, SmnAddress, &LinkStatus.Value);
    *OldSpeed = (UINT8)LinkStatus.Field.CURRENT_LINK_SPEED;
  }

  // speed change to NewSpeed
  SmnAddress = PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CNTL2_OFFSET;
  EqSmnRead (Die, SmnAddress, &LinkCtl2.Value);
  LinkCtl2.Field.TARGET_LINK_SPEED = NewSpeed;
  EqSmnWrite (Die, SmnAddress, &LinkCtl2.Value);

  // retraining link
  SmnAddress = PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CNTL_OFFSET;
  EqSmnRead (Die, SmnAddress, &LinkCtl.Value);
  LinkCtl.Field.RETRAIN_LINK = 1;
  EqSmnWrite (Die, SmnAddress, &LinkCtl.Value);
}

/*----------------------------------------------------------------------------------------*/

/**
 * Get PCIe Current Link Speed
 *
 * @param[in]  Die            Die number
 * @param[in]  Port           PCIe Port number
 *
 * @retval    Link Speed
 */
/*----------------------------------------------------------------------------------------*/
UINT32
HpPcieCurrentLinkSpeed (
  IN     UINT8                     Die,
  IN     UINT8                     Port
  )
{
  UINT32                   PcieCore;
  UINT32                   MappingPortID;
  PCIE_LINK_STATUS_STRUCT  LinkStatus;

  PcieCore = Port / 8;
  MappingPortID = Port % 8;

  EqSmnRead (Die, PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_LINK_STATUS_OFFSET, &LinkStatus.Value);

  return LinkStatus.Field.CURRENT_LINK_SPEED;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Get PCIe Current Link Width
 *
 * @param[in]  Die            Die number
 * @param[in]  Port           PCIe Port number
 *
 * @retval    Link Width
 */
/*----------------------------------------------------------------------------------------*/
UINT32
HpPcieCurrentLinkWidth (
  IN     UINT8                     Die,
  IN     UINT8                     Port
  )
{
  UINT32                   PcieCore;
  UINT32                   MappingPortID;
  PCIE_LINK_STATUS_STRUCT  LinkStatus;

  PcieCore = Port / 8;
  MappingPortID = Port % 8;

  EqSmnRead (Die, PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_LINK_STATUS_OFFSET, &LinkStatus.Value);

  return LinkStatus.Field.NEGOTIATED_LINK_WIDTH;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Get PCIe Current Link Speed Capability
 *
 * @param[in]  Die            Die number
 * @param[in]  Port           PCIe Port number
 *
 * @retval    Link Speed Capability
 */
/*----------------------------------------------------------------------------------------*/
UINT32
HpPcieCapLinkSpeed (
  IN     UINT8                     Die,
  IN     UINT8                     Port
  )
{
  UINT8                 PcieCore;
  UINT8                 MappingPortID;
  PCIE_LINK_CAP_STRUCT  LinkCap;

  PcieCore = Port / 8;
  MappingPortID = Port % 8;

  EqSmnRead (Die, PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CAP_OFFSET, &LinkCap.Value);

  return LinkCap.Field.LINK_SPEED;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Get PCIe Current Link Width Capability
 *
 * @param[in]  Die            Die number
 * @param[in]  Port           PCIe Port number
 *
 * @retval    Link Width Capability
 */
/*----------------------------------------------------------------------------------------*/
UINT32
HpPcieCapLinkWidth (
  IN     UINT8                     Die,
  IN     UINT8                     Port
  )
{
  UINT32                PcieCore;
  UINT32                MappingPortID;
  PCIE_LINK_CAP_STRUCT  LinkCap;

  PcieCore = Port / 8;
  MappingPortID = Port % 8;

  EqSmnRead (Die, PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CAP_OFFSET, &LinkCap.Value);

  return LinkCap.Field.LINK_WIDTH;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Get Max Supported Speed of Target Device
 *
 * @param[in]  Die            Die number
 * @param[in]  Port           PCIe Port number
 *
 * @retval    Max Supported Speed
 */
/*----------------------------------------------------------------------------------------*/
UINT8
HpPcieTargetMaxSupportedSpeed (
  IN     UINT8                     Die,
  IN     UINT8                     Port
  )
{
  PCIE_LC_CNTL_10            LcCntl10;
  PCIE_LC_SPEED_CNTL_STRUCT  LcSpeed;

  // check target supported speed
  if (Port > 7) {
    EqSmnRead (Die, PCIE1_LC_CNTL_10_ADDRESS + ((Port - 8) << 12), &LcCntl10.Value);
  } else {
    EqSmnRead (Die, PCIE0_LC_CNTL_10_ADDRESS + (Port << 12), &LcCntl10.Value);
  }

  if (LcCntl10.Field.LC_OTHER_SIDE_SUPPORTS_GEN4 == 1) {
    return 4;
  }

  if (Port > 7) {
    EqSmnRead (Die, PCIE1_LC_SPEED_CNTL_ADDRESS + ((Port - 8) << 12), &LcSpeed.Value);
  } else {
    EqSmnRead (Die, PCIE0_LC_SPEED_CNTL_ADDRESS + (Port << 12), &LcSpeed.Value);
  }

  if (LcSpeed.Field.LC_OTHER_SIDE_SUPPORTS_GEN3 == 1) {
    return 3;
  }

  if (Port > 7) {
    EqSmnRead (Die, PCIE1_LC_SPEED_CNTL_ADDRESS + ((Port - 8) << 12), &LcSpeed.Value);
  } else {
    EqSmnRead (Die, PCIE0_LC_SPEED_CNTL_ADDRESS + (Port << 12), &LcSpeed.Value);
  }

  if (LcSpeed.Field.LC_OTHER_SIDE_SUPPORTS_GEN2 == 1) {
    return 2;
  }

  return 1;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Get CE AER MASK
 *
 * @param[in]  Die            Die number
 * @param[in]  Port           PCIe Port number
 *
 * @retval    AER MASK
 */
/*----------------------------------------------------------------------------------------*/
UINT32
HpGetAerCeMask(
  IN     UINT8                     Die,
  IN     UINT8                     Port
  )
{
  UINT32  PcieCore;
  UINT32  MappingPortID;
  UINT32  SmnAddress;
  UINT32  Data32;

  PcieCore = Port / 8;
  MappingPortID = Port % 8;
  
  SmnAddress  =  (PcieCore == 0) ? HP_PCIE0_CE_MASK_ADDRESS : HP_PCIE1_CE_MASK_ADDRESS;
  SmnAddress += (MappingPortID << 12);
  EqSmnRead (Die, SmnAddress, &Data32);

  return Data32;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Set CE AER MASK
 *
 * @param[in]  Die            Die number
 * @param[in]  Port           PCIe Port number
 * @param[in]  Value          MASK Value
 *
 * @retval    VOID
 */
/*----------------------------------------------------------------------------------------*/
VOID
HpSetAerCeMask(
  IN     UINT8                      Die,
  IN     UINT8                      Port,
  IN     UINT32                     Value
  )
{
  UINT32  PcieCore;
  UINT32  MappingPortID;
  UINT32  SmnAddress;
  UINT32  Data32;

  PcieCore = Port / 8;
  MappingPortID = Port % 8;
  Data32 = Value;
  
  SmnAddress  =  (PcieCore == 0) ? HP_PCIE0_CE_MASK_ADDRESS : HP_PCIE1_CE_MASK_ADDRESS;
  SmnAddress += (MappingPortID << 12);
  EqSmnWrite (Die, SmnAddress, &Data32);

  return;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Get PCIe Port Lane Range
 *
 * @param[in]  Die            Die number
 * @param[in]  Port           PCIe Port number
 * @param[in]  StartLane      Start Of Lane Number
 * @param[in]  EndLane        End Of Lane Number
 *
 * @retval    VOID
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
HpPcieGetLaneRange (
  IN     UINT8                     Die,
  IN     UINT8                     Port,
  OUT    UINT8                     *StartLane,
  OUT    UINT8                     *EndLane
  )
{
  UINT32  PcieCore;
  UINT32  MappingPortID;
  UINT32  Data32;

  PcieCore = Port / 8;
  MappingPortID = Port % 8;

  // set lane order cntl
  EqSmnRW (Die, PCIE_CORE_REGS_SMN_BASE + (PcieCore << 20) + PCIE_LC_PORT_ORDER_CNTL, 0xFFFFFFFF, BIT0);
  EqSmnRead (Die, PCIE_CORE_REGS_SMN_BASE + (PcieCore << 20) + PCIE_LC_PORT_ORDER_CNTL, &Data32);

  if (!(Data32 & BIT0)) {
    return EFI_UNSUPPORTED;
  }

  // get start/end lane number
  EqSmnRead (Die, PCIE_PORT_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + PCIE_LC_PORT_ORDER, &Data32);
  *StartLane = (UINT8)Data32;
  *EndLane   = *StartLane + (UINT8)HpPcieCapLinkWidth (Die, Port) - 1;

  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/**
 * PHY register reconfigure
 * @param[in]  Die            Die Number
 * @param[in]  Port           PCIe Port Number
 * @param[in]  StartLane      Start Of Lane Number
 * @param[in]  EndLane        End Of Lane Number
 * @param[in]  Task           Pointer to Task
 *
 * @retval
 */
/*----------------------------------------------------------------------------------------*/
VOID
HpDxioPhyReCfgHygonCx (
  IN     UINT8                      Die,
  IN     UINT8                      Port,
  IN     UINT8                      StartLane,
  IN     UINT8                      EndLane,
  OUT    UINT32                     *Task
  )
{
  UINT16    Data16;
  UINT8     DxioType;
  UINT8     Lane;
  UINT8     PkgIndex;
  PKG_TYPE  CpuPkg;

  *Task = 0;

  // get CPU Generation and Package info
  CpuPkg = GetPkgType ();
  if(CpuPkg == PKG_DM1) {
    // DEBUG ((EFI_D_INFO, "Skip DM1 Pkg\n"));
    goto EXIT;
  }

  PkgIndex = 0;
  switch (CpuPkg) {
    case PKG_DM1:
      PkgIndex = 0;
      break;

    case PKG_SL1:
      PkgIndex = 1;
      break;

    case PKG_SL1R2:
      PkgIndex = 2;
      break;

    default:
      break;
  }

  DxioType = (Port < 8) ? 2 : 3;

  if ((HpPcieCapLinkSpeed (Die, Port) < 4) || (HpPcieTargetMaxSupportedSpeed (Die, Port) < 4)) {
    goto EXIT;
  }

  // Set HYGON_CX Gen4 PHY Parameters
  for (Lane = StartLane; Lane <= EndLane; Lane++) {
    HpPhyRegWrite (Die, DxioType, Lane, LANE_ANA_RX_VERG_CTRL2, PhyParameterTable_Hygon_Cx[PkgIndex][LANE_ANA_RX_VERG_CTRL2_INDEX]);
    HpPhyRegWrite (Die, DxioType, Lane, LANE_DIG_ANA_RX_AFE_CTLE, PhyParameterTable_Hygon_Cx[PkgIndex][LANE_DIG_ANA_RX_AFE_CTLE_INDEX]);
    HpPhyRegWrite (Die, DxioType, Lane, LANE_ANA_RX_AFE_OVER, PhyParameterTable_Hygon_Cx[PkgIndex][LANE_ANA_RX_AFE_OVER_INDEX]);
    HpPhyRegWrite (Die, DxioType, Lane, LANE_ANA_RX_AFE_OVER_RTRIM, PhyParameterTable_Hygon_Cx[PkgIndex][LANE_ANA_RX_AFE_OVER_RTRIM_INDEX]);
    HpPhyRegWrite (Die, DxioType, Lane, LANE_ANA_RX_AFE_OVER_ICTRL, PhyParameterTable_Hygon_Cx[PkgIndex][LANE_ANA_RX_AFE_OVER_ICTRL_INDEX]);
    HpPhyRegWrite (Die, DxioType, Lane, LANE_ANA_RX_CAL_MUXB, PhyParameterTable_Hygon_Cx[PkgIndex][LANE_ANA_RX_CAL_MUXB_INDEX]);
    HpPhyRegWrite (Die, DxioType, Lane, LANE_ANA_RX_CAL_MUXA, PhyParameterTable_Hygon_Cx[PkgIndex][LANE_ANA_RX_CAL_MUXA_INDEX]);
    HpPhyRegRead (Die, DxioType, Lane, LANE_DIG_RX_CDR_CTL_3, &Data16);
    Data16 &= 0xffc7;
    Data16 |= (PhyParameterTable_Hygon_Cx[PkgIndex][LANE_DIG_RX_CDR_CTL_3_INDEX] << 3);
    HpPhyRegWrite (Die, DxioType, Lane, LANE_DIG_RX_CDR_CTL_3, Data16);
    HpPhyRegRead (Die, DxioType, Lane, LANE_DIG_RX_CDR_CTL_4, &Data16);
    Data16 &= 0xfff8;
    Data16 |= PhyParameterTable_Hygon_Cx[PkgIndex][LANE_DIG_RX_CDR_CTL_4_INDEX];
    HpPhyRegWrite (Die, DxioType, Lane, LANE_DIG_RX_CDR_CTL_4, Data16);
  }

  *Task |= HP_TASK_REDO_EQ;
EXIT:
  return;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Initialize PHY Register
 * @param[in]  Die            Die Number
 * @param[in]  Port           PCIe Port Number
 * @param[in]  StartLane      Start Of Lane Number
 * @param[in]  EndLane        End Of Lane Number
 * @retval
 */
/*----------------------------------------------------------------------------------------*/
VOID
HpDxioPhyCfgInitHygonCx (
  IN     UINT8                     Die,
  IN     UINT8                     Port,
  IN     UINT8                     StartLane,
  IN     UINT8                     EndLane
  )
{
  UINT8  DxioType;
  UINT8  Lane;

  DxioType = (Port < 8) ? 2 : 3;
  // Set HYGON_CX Gen4 PHY Parameters
  for (Lane = StartLane; Lane <= EndLane; Lane++) {
    HpPhyRegWrite (Die, DxioType, Lane, LANE_ANA_RX_VERG_CTRL2, PhyParameterInitTable_Hygon_Cx[LANE_ANA_RX_VERG_CTRL2_INDEX]);
    HpPhyRegWrite (Die, DxioType, Lane, LANE_DIG_ANA_RX_AFE_CTLE, PhyParameterInitTable_Hygon_Cx[LANE_DIG_ANA_RX_AFE_CTLE_INDEX]);
    HpPhyRegWrite (Die, DxioType, Lane, LANE_ANA_RX_AFE_OVER, PhyParameterInitTable_Hygon_Cx[LANE_ANA_RX_AFE_OVER_INDEX]);
    HpPhyRegWrite (Die, DxioType, Lane, LANE_ANA_RX_AFE_OVER_RTRIM, PhyParameterInitTable_Hygon_Cx[LANE_ANA_RX_AFE_OVER_RTRIM_INDEX]);
    HpPhyRegWrite (Die, DxioType, Lane, LANE_ANA_RX_AFE_OVER_ICTRL, PhyParameterInitTable_Hygon_Cx[LANE_ANA_RX_AFE_OVER_ICTRL_INDEX]);
    HpPhyRegWrite (Die, DxioType, Lane, LANE_ANA_RX_CAL_MUXB, PhyParameterInitTable_Hygon_Cx[LANE_ANA_RX_CAL_MUXB_INDEX]);
    HpPhyRegWrite (Die, DxioType, Lane, LANE_ANA_RX_CAL_MUXA, PhyParameterInitTable_Hygon_Cx[LANE_ANA_RX_CAL_MUXA_INDEX]);
    HpPhyRegWrite (Die, DxioType, Lane, LANE_DIG_RX_CDR_CTL_3, PhyParameterInitTable_Hygon_Cx[LANE_DIG_RX_CDR_CTL_3_INDEX]);
    HpPhyRegWrite (Die, DxioType, Lane, LANE_DIG_RX_CDR_CTL_4, PhyParameterInitTable_Hygon_Cx[LANE_DIG_RX_CDR_CTL_4_INDEX]);
  }

  return;
}

/*----------------------------------------------------------------------------------------*/

/**
 * PHY register reconfigure for HYGON_DX
 * @param[in]  Die            Die Number
 * @param[in]  Port           PCIe Port Number
 * @param[in]  StartLane      Start Of Lane Number
 * @param[in]  EndLane        End Of Lane Number
 * @param[in]  Task           Pointer to Task
 * @retval
 */
/*----------------------------------------------------------------------------------------*/
VOID
HpDxioPhyReCfgHygonDx (
  IN     UINT8                      Die,
  IN     UINT8                      Port,
  IN     UINT8                      StartLane,
  IN     UINT8                      EndLane,
  OUT    UINT32                     *Task
  )
{
  UINT16           Data16;
  UINT8            DxioType;
  UINT8            Lane;
  UINT8            PkgIndex;
  PKG_TYPE         CpuPkg;
  PCIE_LC_CNTL_10  LcCntl10;

  *Task = 0;
  // get CPU Generation and Package info
  CpuPkg   = GetPkgType ();
  PkgIndex = 0;
  switch (CpuPkg) {
    case PKG_DM1:
      PkgIndex = 0;
      break;

    case PKG_SL1:
      PkgIndex = 1;
      break;

    case PKG_SL1R2:
      PkgIndex = 2;
      break;

    default:
      break;
  }

  DxioType = (Port < 8) ? 2 : 3;

  // check target supported speed
  if (DxioType > 7) {
    EqSmnRead (Die, PCIE1_LC_CNTL_10_ADDRESS + ((Port - 8) << 12), &LcCntl10.Value);
  } else {
    EqSmnRead (Die, PCIE0_LC_CNTL_10_ADDRESS + (Port << 12), &LcCntl10.Value);
  }

  // Check target if support gen4
  if (LcCntl10.Field.LC_OTHER_SIDE_SUPPORTS_GEN4 == 0) {
    goto EXIT;
  }

  // HYGON_DX Set Ctl3/Ctl4 config for Gen4
  if ((CpuPkg == PKG_DM1) || (CpuPkg == PKG_SL1)) {
    for (Lane = StartLane; Lane <= EndLane; Lane++) {
      HpPhyRegRead (Die, DxioType, Lane, LANE_DIG_RX_CDR_CTL_3, &Data16);
      Data16 &= 0xffc7;
      Data16 |= (PhyParameterTable_Hygon_Dx[PkgIndex][LANE_DIG_RX_CDR_CTL_3_INDEX] << 3);
      HpPhyRegWrite (Die, DxioType, Lane, LANE_DIG_RX_CDR_CTL_3, Data16);
      HpPhyRegRead (Die, DxioType, Lane, LANE_DIG_RX_CDR_CTL_4, &Data16);
      Data16 &= 0xfff8;
      Data16 |= PhyParameterTable_Hygon_Dx[PkgIndex][LANE_DIG_RX_CDR_CTL_4_INDEX];
      HpPhyRegWrite (Die, DxioType, Lane, LANE_DIG_RX_CDR_CTL_4, Data16);
    }
  }

EXIT:
  return;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Initialize PHY Register
 * @param[in]  Die            Die Number
 * @param[in]  Port           PCIe Port Number
 * @param[in]  StartLane      Start Of Lane Number
 * @param[in]  EndLane        End Of Lane Number
 * @retval
 */
/*----------------------------------------------------------------------------------------*/
VOID
HpDxioPhyCfgInitHygonDx (
  IN     UINT8                     Die,
  IN     UINT8                     Port,
  IN     UINT8                     StartLane,
  IN     UINT8                     EndLane
  )
{
  UINT8  DxioType;
  UINT8  Lane;

  DxioType = (Port < 8) ? 2 : 3;
  // Set HygonDx Gen4 PHY Parameters
  for (Lane = StartLane; Lane <= EndLane; Lane++) {
    HpPhyRegWrite (Die, DxioType, Lane, LANE_DIG_RX_CDR_CTL_3, PhyParameterInitTable_Hygon_Dx[LANE_DIG_RX_CDR_CTL_3_INDEX]);
    HpPhyRegWrite (Die, DxioType, Lane, LANE_DIG_RX_CDR_CTL_4, PhyParameterInitTable_Hygon_Dx[LANE_DIG_RX_CDR_CTL_4_INDEX]);
  }

  return;
}


/*----------------------------------------------------------------------------------------*/

/**
 * Check If Pcie Card Exist
 * @param[in]  Die            Die Number
 * @param[in]  Port           PCIe Port Number
 * @retval     True     Card Add
 *             False   Card Remove
 */
/*----------------------------------------------------------------------------------------*/
BOOLEAN
IsPcieCardAdd (
  IN     UINT8                     Die,
  IN     UINT8                     Port
  )
{
  UINT32                   PcieCore;
  UINT32                   MappingPortID;
  PCIE_LINK_STATUS_STRUCT  LinkStatus;
  UINT32                   TryCount = 0;

  PcieCore = Port / 8;
  MappingPortID = Port % 8;

  do {
    EqSmnRead (Die, PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_LINK_STATUS_OFFSET, &LinkStatus.Value);
    MicroSecondDelay (10000);
    TryCount++;
  } while ((LinkStatus.Field.DL_ACTIVE != 1) && (TryCount < WAIT_DL_ACTIVE_MAX_TRY_COUNT_WITH_10MS_DELAY));

  return (TryCount < WAIT_DL_ACTIVE_MAX_TRY_COUNT_WITH_10MS_DELAY) ? TRUE : FALSE;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Get Downstream PCIe Device vendor ID and Device ID
 *
 * @param[in]  Die      Die Number
 * @param[in]  Port     PCIe Port Number
 *
 * @retval    Downstream vendor ID and Device ID
 */
/*----------------------------------------------------------------------------------------*/
UINT32
HpGetTargetVidDid (
  IN     UINT8                     Die,
  IN     UINT8                     Port
  )
{
  UINT32  RpSubBusCfgAddress, TargetPciVidDidAddr;
  UINT32  VidDid;
  UINT32  SubBus;
  UINT32  MmCfg;
  UINT32  PcieCore;
  UINT32  MappingPortID;

  PcieCore = Port / 8;
  MappingPortID = Port % 8;

  RpSubBusCfgAddress  = (PcieCore == 0) ? SMN_PCIE0_SUB_BUS_NUMBER_LATENCY : SMN_PCIE1_SUB_BUS_NUMBER_LATENCY;
  RpSubBusCfgAddress += (MappingPortID * 0x1000);
  // Get Sub bus number
  EqSmnRead (Die, RpSubBusCfgAddress, &SubBus);
  SubBus = (SubBus >> 16) & 0xFF;

  // read target viddid
  MmCfg = (UINT32)(AsmReadMsr64 (0xC0010058) & 0xfff00000);
  TargetPciVidDidAddr = PCIE_VIDDID_ADDR (MmCfg, SubBus, 0, 0);
  VidDid = MmioRead32 (TargetPciVidDidAddr);

  return VidDid;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Get Downstream PCIe Device subsystem vendor ID and Device ID
 *
 * @param[in]  Die      Die Number
 * @param[in]  Port     PCIe Port Number
 *
 * @retval    Downstream subsystem vendor ID and Device ID
 */
/*----------------------------------------------------------------------------------------*/
UINT32
HpGetTargetSubSystemVidDid (
  IN     UINT8                     Die,
  IN     UINT8                     Port
  )
{
  UINT32  RpSubBusCfgAddress, TargetPciSubSystemVidDidAddr;
  UINT32  SubSystemVidDid;
  UINT32  SubBus;
  UINT32  MmCfg;
  UINT32  PcieCore;
  UINT32  MappingPortID;

  PcieCore = Port / 8;
  MappingPortID = Port % 8;

  RpSubBusCfgAddress  = (PcieCore == 0) ? SMN_PCIE0_SUB_BUS_NUMBER_LATENCY : SMN_PCIE1_SUB_BUS_NUMBER_LATENCY;
  RpSubBusCfgAddress += (MappingPortID * 0x1000);
  // Get Sub bus number
  EqSmnRead (Die, RpSubBusCfgAddress, &SubBus);
  SubBus = (SubBus >> 16) & 0xFF;
  
  // read target viddid
  MmCfg = (UINT32)(AsmReadMsr64 (0xC0010058) & 0xfff00000);
  TargetPciSubSystemVidDidAddr = PCIE_SUBSYSTEM_VIDDID_ADDR (MmCfg, SubBus, 0, 0);
  SubSystemVidDid = MmioRead32 (TargetPciSubSystemVidDidAddr);

  return SubSystemVidDid;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Configure EQ for current PCIe Engine
 *
 * @param[in]  Die             Die Number
 * @param[in]  Port            PCIe Port Number
 * @param[in]  EqCfg           Pointer to EQ Configuration
 * @param[in]  Phase           Configure EQ Phase
 * @param[in]  Task            Pointer to Task
 *
 * @retval    EFI_STATUS
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
HpDoPcieEqCfg (
  IN     UINT8                      Die,
  IN     UINT8                      Port,
  IN     HYGON_EQ_CFG               *EqCfg,
  IN     HP_CFG_EQ_PHASE            Phase,
  OUT    UINT32                     *Task
  )
{
  UINT32         PcieCore;
  UINT32         MappingPortID;
  UINT32         SmnAddress;
  PCIE_LC_CNTL4  PcieLcCntl4;
  PCIE_LC_CNTL8  PcieLcCntl8;
  UINT16         Cdr_Data;
  UINT8          DxioType;

  if (Task == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Task    = 0;
  PcieCore = Port / 8;
  MappingPortID = Port % 8;
  DxioType = (Port < 8) ? 2 : 3;
  DEBUG((EFI_D_ERROR, "Eq Cfg Phase=%02d Type=%02d\n", Phase, EqCfg->Type));
  if (Phase == cfg_eq_card_remove) {
    switch (EqCfg->Type) {
      case pcie_gen3_ttx_force_otherside_preset:
        SmnAddress = PCIE_PORT_REGS_SMN_BASE + (PcieCore << 20) + PCIE_LC_CNTL4_REG_OFFSET + (MappingPortID << 12);
        EqSmnRead (Die, SmnAddress, &PcieLcCntl4.Value);
        PcieLcCntl4.Reg.LC_FORCE_PRESET_IN_EQ_REQ_PHASE = 0;
        EqSmnWrite (Die, SmnAddress, &PcieLcCntl4.Value);
        break;
        
      case pcie_gen4_ttx_force_otherside_preset:
        SmnAddress = PCIE_PORT_REGS_SMN_BASE + (PcieCore << 20) + PCIE_LC_CNTL8_REG_OFFSET + (MappingPortID << 12);
        EqSmnRead (Die, SmnAddress, &PcieLcCntl8.Value);
        PcieLcCntl8.Reg.LC_FORCE_PRESET_IN_EQ_REQ_PHASE_16GT = 0;
        break;
    
      default:
        return EFI_UNSUPPORTED;
    }
  }
  
  if (Phase == cfg_eq_card_add) {  
    switch (EqCfg->Type) {
      case pcie_gen3_ttx_force_otherside_preset:
        *Task |= HP_TASK_SPEED_CHANGE;
        SmnAddress = PCIE_PORT_REGS_SMN_BASE + (PcieCore << 20) + PCIE_LC_CNTL4_REG_OFFSET + (MappingPortID << 12);
        EqSmnRead (Die, SmnAddress, &PcieLcCntl4.Value);
        PcieLcCntl4.Reg.LC_FORCE_PRESET_IN_EQ_REQ_PHASE = 1;
        PcieLcCntl4.Reg.LC_FORCE_PRESET_VALUE = EqCfg->CfgValue[0];
        PcieLcCntl4.Reg.LC_EQ_SEARCH_MODE     = 3;
        EqSmnWrite (Die, SmnAddress, &PcieLcCntl4.Value);
   #if 0
          // clear PERFORM_EQUALIZATION and LINK_EQUALIZATION_REQ_INT_EN
          SmnAddress = PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CNTL3_OFFSET;
          EqSmnRead (Die, SmnAddress, &LinkCtl3.Value);
          LinkCtl3.Field.PERFORM_EQUALIZATION = 0;
          LinkCtl3.Field.LINK_EQUALIZATION_REQ_INT_EN = 0;
          EqSmnWrite (Die, SmnAddress, &LinkCtl3.Value);
          // speed change to Gen2
          HpPcieSpeedChange (Die, Port, 2, &OldLinkSpeed);
          // delay 50ms
          MicroSecondDelay (20*1000);
          // set PERFORM_EQUALIZATION to 1
          SmnAddress = PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CNTL3_OFFSET;
          EqSmnRead (Die, SmnAddress, &LinkCtl3.Value);
          LinkCtl3.Field.PERFORM_EQUALIZATION = 1;
          EqSmnWrite (Die, SmnAddress, &LinkCtl3.Value);
          // speed change to Gen3
          HpPcieSpeedChange (Die, Port, 3, NULL);
          // speed change to old speed
          HpPcieSpeedChange (Die, Port, OldLinkSpeed, NULL);
          // set PERFORM_EQUALIZATION to 0
          EqSmnRead (Die, SmnAddress, &LinkCtl3.Value);
          LinkCtl3.Field.PERFORM_EQUALIZATION = 0;
          EqSmnWrite (Die, SmnAddress, &LinkCtl3.Value);
   #endif
        break;
        
      case pcie_gen4_ttx_force_otherside_preset:
        *Task |= HP_TASK_REDO_EQ;
        SmnAddress = PCIE_PORT_REGS_SMN_BASE + (PcieCore << 20) + PCIE_LC_CNTL8_REG_OFFSET + (MappingPortID << 12);
        EqSmnRead (Die, SmnAddress, &PcieLcCntl8.Value);
        PcieLcCntl8.Reg.LC_FORCE_PRESET_IN_EQ_REQ_PHASE_16GT = 1;
        PcieLcCntl8.Reg.LC_FORCE_PRESET_VALUE_16GT = EqCfg->CfgValue[0];
        EqSmnWrite (Die, SmnAddress, &PcieLcCntl8.Value);
        break;
  
      case pcie_rxX_SSC_OFF_PHUG1:
        if(EqCfg->CpuGen == CPU_HYGON_DX) {
          if (EqCfg->PerPort) {
            HpPhyRegRead (Die, DxioType, EqCfg->StartLane, LANE_DIG_RX_CDR_CTL_3, &Cdr_Data);
            Cdr_Data &= 0xffc7;
            Cdr_Data |= (EqCfg->CfgValue[0] << 3);
            HpPhyRegWrite (Die, DxioType, EqCfg->StartLane, LANE_DIG_RX_CDR_CTL_3, Cdr_Data);
          }
        }
        break;
  
      case pcie_rxX_SSC_OFF_FRUG1:
        if(EqCfg->CpuGen == CPU_HYGON_DX) {
          if (EqCfg->PerPort) {
            HpPhyRegRead (Die, DxioType, EqCfg->StartLane, LANE_DIG_RX_CDR_CTL_4, &Cdr_Data);
            Cdr_Data &= 0xfff8;
            Cdr_Data |= EqCfg->CfgValue[0];
            HpPhyRegWrite (Die, DxioType, EqCfg->StartLane, LANE_DIG_RX_CDR_CTL_4, Cdr_Data);
          }
        }
        break;
  
      default:
        return EFI_UNSUPPORTED;
    }
  }

  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Configure EQ for current PCIe Engine
 *
 * @param[in]  Die             Die Number
 * @param[in]  Port            PCIe Port Number
 * @param[in]  StartLane       Start Of Lane Number
 * @param[in]  EndLane         End Of Lane Number
 * @param[in]  EqCfgTable      Pointer to HYGON_CPM_EQ_CONFIG_TABLE
 * @param[in]  Phase           Configure EQ Phase
 * @param[in]  Task            Task
 *
 * @retval    EFI_STATUS
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
HpPcieEqConfiguration (
  IN  UINT8                      Die,
  IN  UINT8                      Port,
  IN  UINT8                      StartLane,
  IN  UINT8                      EndLane,
  IN  HYGON_CPM_EQ_CONFIG_TABLE  *EqCfgTable,
  IN  HP_CFG_EQ_PHASE            Phase,
  IN  UINT32                     *Task
  )
{
  CPU_GEN  CpuGen;
  UINT32   VidDid, SubSystemVidDid;
  UINT32   i;

  CpuGen = GetModelId ();
  DEBUG ((EFI_D_INFO, "Pcie Eq SL=%d EL=%d \n", StartLane, EndLane));
  for (i = 0; (EqCfgTable->EqCfgList[i].Flag & EQ_TABLE_END) != EQ_TABLE_END; i++) {
    if (((EqCfgTable->EqCfgList[i].Type & EQ_TYPE_MASK) != pcie_eq_type) ||
        (CpuGen != EqCfgTable->EqCfgList[i].CpuGen) ||
        (Die != EqCfgTable->EqCfgList[i].Die)) {
      continue;
    }

    if ((StartLane == EqCfgTable->EqCfgList[i].StartLane) && (EndLane == EqCfgTable->EqCfgList[i].EndLane)) {
      if (EqCfgTable->EqCfgList[i].Flag & FLAG_SPECIFIED) {
        VidDid = HpGetTargetVidDid (Die, Port);
        if ((VidDid == 0xffffffff) || (VidDid != EqCfgTable->EqCfgList[i].VidDid)) {
          continue;
        }

        if (EqCfgTable->EqCfgList[i].SubSysVidDid != 0xffffffff) {
          SubSystemVidDid = HpGetTargetSubSystemVidDid (Die, Port);
          if (SubSystemVidDid != EqCfgTable->EqCfgList[i].SubSysVidDid) {
            continue;
          }
        }
      }

      HpDoPcieEqCfg (Die, Port, &EqCfgTable->EqCfgList[i], Phase, Task);
    }
  }

  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Set Spc Mode
 *
 * @param[in]  Die             Die Number
 * @param[in]  Port            PCIe Port Number
 * @param[in]  Value
 *
 * @retval
 */
/*----------------------------------------------------------------------------------------*/
VOID
HpSetSpcModeGen3 (
  IN  UINT8      Die,
  IN  UINT8      Port,
  IN  UINT32     Value
  )
{
  UINT32  PcieCore;
  UINT32  MappingPortID;
  UINT32  SmnAddress;

  if (Value > 1) {
    return;
  }

  PcieCore = Port / 8;
  MappingPortID = Port % 8;

  SmnAddress  =  (PcieCore == 0) ? HP_SMN_0x111402ec_ADDRESS : HP_SMN_0x112402ec_ADDRESS;
  SmnAddress += (MappingPortID << 12);
  EqSmnRW (Die, SmnAddress, ~((UINT32)BIT4), (Value<<4));
}

/*----------------------------------------------------------------------------------------*/

/**
 * Enable Gen4 Strip
 *
 * @param[in]  Die             Die Number
 * @param[in]  Port            PCIe Port Number
 * @param[in]  Enable          Enable or Disable
 *
 * @retval
 */
/*----------------------------------------------------------------------------------------*/
VOID
HpEnableGen4Strip (
  IN  UINT8      Die,
  IN  UINT8      Port,
  IN  BOOLEAN    Enable
  )
{
  UINT32  PcieCore;
  UINT32  MappingPortID;
  UINT32  SmnAddress;
  UINT32  Data32;

  PcieCore = Port / 8;
  MappingPortID = Port % 8;

  SmnAddress  =  (PcieCore == 0) ? PCIE0_LC_SPEED_CNTL_ADDRESS : PCIE1_LC_SPEED_CNTL_ADDRESS;
  SmnAddress += (MappingPortID << 12);
  Data32      = Enable ? 1 : 0;
  EqSmnRW (Die, SmnAddress, ~((UINT32)BIT2), (Data32<<2));
}

/*----------------------------------------------------------------------------------------*/

/**
 * Do Some Pcie Common Configurations
 *
 * @param[in]  Die             Die Number
 * @param[in]  Port            PCIe Port Number
 * @param[in]  Task            Pointer to Task
 *
 * @retval
 */
/*----------------------------------------------------------------------------------------*/
VOID
PcieCommonConfig (
  IN  UINT8      Die,
  IN  UINT8      Port,
  OUT UINT32     *Task
  )
{
  UINT32  PcieCore;
  UINT32  MappingPortID;
  UINT32  SmnAddress;
  UINT32  TargetMaxSupportedSpeed;
  UINT32  LinkSpeed;
  UINT32  LinkWidth;

  PcieCore = Port / 8;
  MappingPortID = Port % 8;
  *Task = 0;
  // read current link speed
  LinkSpeed = HpPcieCurrentLinkSpeed (Die, Port);

  // read current link width
  LinkWidth = HpPcieCurrentLinkWidth (Die, Port);

  // check link width
  if (LinkWidth == 4) {
    // set PCIE_LC_CNTL6 bit4 LC_SPC_MODE_8GT = 0
    HpSetSpcModeGen3 (Die, Port, 0);
    // set gen4 strip to 0
    HpEnableGen4Strip (Die, Port, FALSE);

    if (LinkSpeed == 3) {
      *Task |= HP_TASK_SPEED_CHANGE;
 #if 0
        // speed change to Gen1
        HpPcieSpeedChange (Die, Port, 1, NULL);

        // delay 20ms
        MicroSecondDelay (20000);

        // speed change to Gen3
        HpPcieSpeedChange (Die, Port, 3, NULL);
 #endif
    }

    if (LinkSpeed == 4) {
      // retrain link
      HpPcieRetraining (Die, Port, FALSE);
    }
  }

  if((LinkWidth == 4) || (LinkWidth == 2) || (LinkWidth == 1)) {
    TargetMaxSupportedSpeed = HpPcieTargetMaxSupportedSpeed (Die, Port);

    if(TargetMaxSupportedSpeed >= 3) {
      // mask bad TLP
      SmnAddress  =  (PcieCore == 0) ? HP_PCIE0_CE_MASK_ADDRESS : HP_PCIE1_CE_MASK_ADDRESS;
      SmnAddress += (MappingPortID << 12);
      EqSmnRW (Die, SmnAddress, ~((UINT32)(BIT6 | BIT0)), (UINT32)(BIT6 | BIT0));

      SmnAddress =  (PcieCore == 0) ? HP_SMN_0x11180008_ADDRESS : HP_SMN_0x11280008_ADDRESS;
      EqSmnRW (Die, SmnAddress, ~((UINT32)BIT6), (UINT32)BIT6);
    }
  }
}

/*----------------------------------------------------------------------------------------*/

/**
 * Do Some Pcie Common Configurations Restore
 *
 * @param[in]  Die             Die Number
 * @param[in]  Port            PCIe Port Number
 * @param[in]  Task            Pointer to Task
 *
 * @retval
 */
/*----------------------------------------------------------------------------------------*/
VOID
PcieCommonConfigRestore (
  IN  UINT8     Die,
  IN  UINT8     Port
  )
{
  UINT32  PcieCore;
  UINT32  MappingPortID;
  UINT32  SmnAddress;

  PcieCore = Port / 8;
  MappingPortID = Port % 8;
  
  // set PCIE_LC_CNTL6 bit4 LC_SPC_MODE_8GT to 1
  HpSetSpcModeGen3 (Die, Port, 1);

  // set gen4 strip to 1
  HpEnableGen4Strip (Die, Port, TRUE);
  
  // Set 0x11180008 Bit6 to 0
  SmnAddress =  (PcieCore == 0) ? HP_SMN_0x11180008_ADDRESS : HP_SMN_0x11280008_ADDRESS;
  EqSmnRW (Die, SmnAddress, ~((UINT32)BIT6), 0);
}
/*----------------------------------------------------------------------------------------*/

/**
 * PCIe Hotplug Configurations
 *
 * @param[in]     Die     Die number
 * @param[in]     Port    PCIe port number
 *
 * @retval        VOID
 */
/*----------------------------------------------------------------------------------------*/
VOID
PCIeHotPlugConfig (
  IN     UINT8                     Die,
  IN     UINT8                     Port
  )
{
  EFI_STATUS                 Status;
  UINT8                      StartLane = 0;
  UINT8                      EndLane = 0;
  SOC_LOGICAL_ID             LogicalId;
  UINT32                     Task;
  UINT32                     RequestTask;
  HYGON_CPM_EQ_CONFIG_TABLE  *HygonEqTable;

  HpPcieGetLaneRange (Die, Port, &StartLane, &EndLane);
  BaseGetLogicalIdOnExecutingCore (&LogicalId);

  if (IsPcieCardAdd (Die, Port)) {
    // card add
    Status = HpGetEqTable (&HygonEqTable);
    if (!EFI_ERROR (Status)) {
      // HpDumpEqCfgTable (HygonEqTable);
      HpPcieEqConfiguration (Die, Port, StartLane, EndLane, HygonEqTable, cfg_eq_card_add, &RequestTask);
      Task |= RequestTask;
    }

    // Check if HYGON_CX Platform
    if(LogicalId.Revision & (HYGON_REV_F18_DN_Cx)) {
      HpDxioPhyReCfgHygonCx (Die, Port, StartLane, EndLane, &RequestTask);
      Task |= RequestTask;
    }

 #if 0
      // Check if HYGON_DX Platform
      if(LogicalId.Revision & (HYGON_REV_F18_DN_Dx)) {
        HpDxioPhyReCfgHygonDx (Die, Port, StartLane, EndLane, &RequestTask);
        Task |= RequestTask;
      }

 #endif
    if ((Task & HP_TASK_REDO_EQ) != 0) {
      HpPcieRetraining (Die, Port, TRUE);
    }

    HpPcieWaitToL0 (Die, Port, 200);

    if(LogicalId.Revision < HYGON_REV_F18_DN_C2) {
      // PcieCommonConfig (Die, Port);
    }

    HpPcieEnableAllErrorReport (Die, Port, TRUE);

    HpPcieClearAllErrors (Die, Port);
  } else {
    // card remove
    HpPcieEnableAllErrorReport (Die, Port, FALSE);
    Status = HpGetEqTable (&HygonEqTable);
    if (!EFI_ERROR (Status)) {
      // HpDumpEqCfgTable (HygonEqTable);
      HpPcieEqConfiguration (Die, Port, StartLane, EndLane, HygonEqTable, cfg_eq_card_remove, &RequestTask);
    }
    if (LogicalId.Revision & (HYGON_REV_F18_DN_Cx)) {
      HpDxioPhyCfgInitHygonCx (Die, Port, StartLane, EndLane);
      HpPcieClearAllErrors (Die, Port);
    }
  }
}

/*----------------------------------------------------------------------------------------*/

/**
 * Get IOHC Global Status
 *
 * @param[in]     Die     Die number
 *
 * @retval        VOID
 */
/*----------------------------------------------------------------------------------------*/
UINT32
HpIohcGlobalStatus (
  UINT8 Die
  )
{
  UINT32  Value;

  EqSmnRead (Die, HP_SMN_IOHC_GLOBAL_HI_STATUS_ADDRESS, &Value);

  return Value;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Check Hotplug Event
 *
 * @param[in]     Die     Die number
 * @param[in]     Port    PCIe port number
 *
 * @retval        VOID
 */
/*----------------------------------------------------------------------------------------*/
BOOLEAN
HpCheckHotplugEvent (
  UINT8 Die,
  UINT8 Port
  )
{
  UINT32                   TimeOut = 200;
  UINT32                   PcieCore;
  UINT32                   MappingPortID;
  PCIE_SLOT_STATUS_STRUCT  SlotStatus;

  PcieCore = Port / 8;
  MappingPortID = Port % 8;

  do {
    EqSmnRead (Die, PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + HP_RC_PCIE_SLOT_STATUS_REG_OFFSET, &SlotStatus.Value);//SGEZB#24403:Inject PCIe Error Fail
    MicroSecondDelay (1000);
    TimeOut--;
  } while ((TimeOut != 0) && (SlotStatus.Field.PRESENCE_DETECT_CHANGED == 0));//SGEZB#24403:Inject PCIe Error Fail
  
  return (TimeOut != 0) ? TRUE : FALSE;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Clear All Error From Hotplug Port
 *
 * @param[in]     Die     Die number
 * @param[in]     Port    PCIe port number
 *
 * @retval        VOID
 */
/*----------------------------------------------------------------------------------------*/
VOID
HpClearPcieHotplugErrors (
  UINT8  Die,
  UINT32 IohcGlobalStatusHi
  )
{
  UINT8  i;

  for (i = 0; i < 16; i++) {
    if ((IohcGlobalStatusHi & (BIT0 << i)) != 0) {
      if (HpCheckHotplugEvent (Die, i)) {
        HpPcieClearAllErrors (Die, i);
      }
    }
  }
}
