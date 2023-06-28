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

#ifndef __PCIE_HP_COMMON_H__
#define __PCIE_HP_COMMON_H__

#include <Base.h>
#include <Uefi.h>
#include <Library/DebugLib.h>
#include <AmdCpmBase.h>

#define HP_TASK_SPEED_CHANGE                          BIT0
#define HP_TASK_REDO_EQ                               BIT1
#define HP_TASK_BYPASS_RX_RECONFIG                    BIT2
#define HP_TASK_DO_DELAY                              BIT3
#define HP_TASK_WAIT_TIMEOUT                          BIT31
#define WAIT_DL_ACTIVE_MAX_TRY_COUNT_WITH_10MS_DELAY  10

typedef enum {
  cfg_eq_card_add,
  cfg_eq_card_remove
} HP_CFG_EQ_PHASE;

EFI_STATUS
HpPhyRegRead (
  UINT8                     Die,
  UINT8                     DxioType,
  UINT8                     Lane,
  UINT32                    RegAddress,
  UINT16                    *Value
  );

EFI_STATUS
HpPhyRegWrite (
  UINT8                     Die,
  UINT8                     DxioType,
  UINT8                     Lane,
  UINT32                    RegAddress,
  UINT16                    Value
  );

VOID
HpPcieRetrainingWithOutEq (
  IN     UINT8                 Die,
  IN     UINT8                 Port
  );

BOOLEAN
HpPerformEqualizationSetting (
  IN     UINT8                 Die,
  IN     UINT8                 Port,
  IN     BOOLEAN               PerformEq
  );

VOID
HpPcieRetraining (
  IN     UINT8                 Die,
  IN     UINT8                 Port,
  IN     BOOLEAN               ForceEq
  );

BOOLEAN
HpPcieCheckL0 (
  IN     UINT8                     Die,
  IN     UINT8                     Port
  );

BOOLEAN
HpPcieWaitToL0 (
  IN     UINT8                     Die,
  IN     UINT8                     Port,
  IN     UINT32                    MaxDelayMs
  );

BOOLEAN
HpPcieRetryToL0 (
  IN     UINT8                     Die,
  IN     UINT8                     Port,
  IN     UINT32                    RetryCount,
  IN     UINT32                    MaxDelayMsPerTry
  );

VOID
HpPcieEnableAllErrorReport (
  IN     UINT8                     Die,
  IN     UINT8                     Port,
  IN     BOOLEAN                   Enable
  );

VOID
HpPcieClearAllErrors (
  IN     UINT8                     Die,
  IN     UINT8                     Port
  );

VOID
HpPcieSpeedChange (
  IN     UINT8                     Die,
  IN     UINT8                     Port,
  IN     UINT8                     NewSpeed,
  OUT    UINT8                     *OldSpeed
  );

UINT32
HpPcieCurrentLinkSpeed (
  IN     UINT8                     Die,
  IN     UINT8                     Port
  );

UINT32
HpPcieCurrentLinkWidth (
  IN     UINT8                     Die,
  IN     UINT8                     Port
  );

UINT32
HpPcieCapLinkSpeed (
  IN     UINT8                     Die,
  IN     UINT8                     Port
  );

UINT32
HpPcieCapLinkWidth (
  IN     UINT8                     Die,
  IN     UINT8                     Port
  );

UINT8
HpPcieTargetMaxSupportedSpeed (
  IN     UINT8                     Die,
  IN     UINT8                     Port
  );

UINT32
HpGetAerCeMask(
  IN     UINT8                     Die,
  IN     UINT8                     Port
  );

VOID
HpSetAerCeMask(
  IN     UINT8                     Die,
  IN     UINT8                     Port,
  IN     UINT32                    Value
  );

EFI_STATUS
HpPcieGetLaneRange (
  IN     UINT8                     Die,
  IN     UINT8                     Port,
  OUT    UINT8                     *StartLane,
  OUT    UINT8                     *EndLane
  );

VOID
HpDxioPhyReCfgHygonCx (
  IN     UINT8                     Die,
  IN     UINT8                     Port,
  IN     UINT8                     StartLane,
  IN     UINT8                     EndLane,
  OUT    UINT32                     *Task
  );

VOID
HpDxioPhyCfgInitHygonCx (
  IN     UINT8                     Die,
  IN     UINT8                     Port,
  IN     UINT8                     StartLane,
  IN     UINT8                     EndLane
  );

VOID
HpDxioPhyReCfgHygonDx (
  IN     UINT8                     Die,
  IN     UINT8                     Port,
  IN     UINT8                     StartLane,
  IN     UINT8                     EndLane,
  OUT    UINT32                    *Task
  );

VOID
HpDxioPhyCfgInitHygonDx (
  IN     UINT8                     Die,
  IN     UINT8                     Port,
  IN     UINT8                     StartLane,
  IN     UINT8                     EndLane
  );

BOOLEAN
IsPcieCardAdd (
  IN     UINT8                     Die,
  IN     UINT8                     Port
  );

UINT32
HpGetTargetVidDid (
  IN     UINT8                     Die,
  IN     UINT8                     Port
  );

UINT32
HpGetTargetSubSystemVidDid (
  IN     UINT8                     Die,
  IN     UINT8                     Port
  );

EFI_STATUS
HpDoPcieEqCfg (
  IN     UINT8                     Die,
  IN     UINT8                     Port,
  IN     HYGON_EQ_CFG              *EqCfg,
  IN     HP_CFG_EQ_PHASE           Phase,
  OUT    UINT32                    *Task
  );

EFI_STATUS
HpPcieEqConfiguration (
  IN  UINT8                      Die,
  IN  UINT8                      Port,
  IN  UINT8                      StartLane,
  IN  UINT8                      EndLane,
  IN  HYGON_CPM_EQ_CONFIG_TABLE  *EqCfgTable,
  IN  HP_CFG_EQ_PHASE            Phase,
  IN  UINT32                     *Task
  );

VOID
HpSetSpcModeGen3 (
  IN  UINT8      Die,
  IN  UINT8      Port,
  IN  UINT32     Value
  );

VOID
HpEnableGen4Strip (
  IN  UINT8      Die,
  IN  UINT8      Port,
  IN  BOOLEAN    Enable
  );

VOID
PcieCommonConfig (
  IN  UINT8      Die,
  IN  UINT8      Port,
  IN OUT UINT32     *Task
  );

VOID
PcieCommonConfigRestore (
  IN  UINT8     Die,
  IN  UINT8     Port
  );

VOID
PCIeHotPlugConfig (
  IN     UINT8                     Die,
  IN     UINT8                     Port
  );

UINT32
HpIohcGlobalStatus (
  UINT8   Die
  );

VOID
HpClearPcieHotplugErrors (
  UINT8  Die,
  UINT32 IohcGlobalStatusHi
  );

#endif
