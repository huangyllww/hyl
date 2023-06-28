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
#include "Hpmm.h"
#include "PcieHpCommon.h"
#include "IohcSmmSwSmiDispatcher.h"
#include <Library/DebugLib.h>

/*----------------------------------------------------------------------------------------*/

/**
 * HpmmSetTimer - Set Timer Of Hotplug Management Machine
 *
 * @param[in] Hpmm        -  Pointer To HPMM_T
 * @param[in] Timeout     -  Timeout
 * @retval    EFI_SUCCESS
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
HpmmSetTimer (
  HPMM_T  *Hpmm,
  UINTN   Timeout
  )
{
  if (Hpmm->Timer.Status == HPMM_TIMER_RUN) {
    return EFI_UNSUPPORTED;
  }

  Hpmm->Timer.Status  = HPMM_TIMER_RUN;
  Hpmm->Timer.Counter = 0;
  Hpmm->Timer.Timeout = Timeout;

  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/**
 * HpmmStopTimer - Stop Timer Of Hotplug Management Machine
 *
 * @param[in] Hpmm        -  Pointer To HPMM_T
 *
/*----------------------------------------------------------------------------------------*/
VOID
HpmmStopTimer (
  HPMM_T     *Hpmm
  )
{
  Hpmm->Timer.Status  = HPMM_TIMER_STOP;
  Hpmm->Timer.Counter = 0;
}

/*----------------------------------------------------------------------------------------*/

/**
 * HpmmCheckTimer - Check Timer Of Hotplug Management Machine
 *
 * @param[in] Hpmm        -  Pointer To HPMM_T
 * @retval
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
HpmmCheckTimer (
  HPMM_T     *Hpmm
  )
{
  if ((Hpmm->Timer.Status == HPMM_TIMER_STOP) ||
      (Hpmm->Timer.Counter >= Hpmm->Timer.Timeout)) {
    HpmmStopTimer (Hpmm);
    return EFI_SUCCESS;
  }

  return EFI_NOT_READY;
}

/*----------------------------------------------------------------------------------------*/

/**
 * HpmmTimerTickTock - Check Timer Of Hotplug Management Machine
 *
 * @param[in] Hpmm        -  Pointer To HPMM_T
 * @retval
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
HpmmTimerTickTock (
  HPMM_T     *Hpmm
  )
{
  if (Hpmm->Timer.Status == HPMM_TIMER_RUN) {
    Hpmm->Timer.Counter++;
    DEBUG((EFI_D_ERROR, "%d Tick...\n", Hpmm->Timer.Counter));
  }

  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/**
 * HpmmGoToState - Translate State Of Hotplug Management Machine
 *
 * @param[in] Hpmm        -   Pointer To HPMM_T
 * @param[in] State       -   State Of HPMM
 * @retval
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
HpmmGoToState (
  HPMM_T     *Hpmm,
  HPMM_STATE State
  )
{
  Hpmm->PreviousState = Hpmm->CurrentState;
  Hpmm->CurrentState  = State;
  DEBUG((EFI_D_ERROR, "D%02d-P%02d-HpmmState: %02d -> %02d\n", Hpmm->Die, Hpmm->Port, Hpmm->PreviousState, Hpmm->CurrentState));
  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/**
 * HpmmSpeedChange - Change Speed
 *
 * @param[in] Hpmm        -   Pointer To HPMM_T
 * @retval
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
HpmmSpeedChange (
  HPMM_T     *Hpmm
  )
{
  EFI_STATUS  Status;

  if ((Hpmm->HpmmPendingTask & HP_TASK_SPEED_CHANGE) != 0) {
    HpPcieSpeedChange (Hpmm->Die, Hpmm->Port, 1, NULL);
    Status = HpmmSetTimer (Hpmm, HPMM_SPEED_CHANGE_TIMEOUT);
    Hpmm->HpmmDoingTask   |= HP_TASK_WAIT_TIMEOUT;
    Hpmm->HpmmDoingTask   |= HP_TASK_SPEED_CHANGE;
    Hpmm->HpmmPendingTask &= ~HP_TASK_SPEED_CHANGE;
  }

  Status = HpmmCheckTimer (Hpmm);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  HpPerformEqualizationSetting (Hpmm->Die, Hpmm->Port, TRUE);
  Hpmm->HpmmDoingTask &= ~HP_TASK_WAIT_TIMEOUT;
  HpPcieSpeedChange (Hpmm->Die, Hpmm->Port, (UINT8)Hpmm->InitLinkSpeed, NULL);
  Hpmm->HpmmDoingTask &= ~HP_TASK_SPEED_CHANGE;

  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/**
 * HpmmRedoEq - Set EQ
 *
 * @param[in] Hpmm        -   Pointer To HPMM_T
 * @retval
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
HpmmRedoEq (
  HPMM_T     *Hpmm
  )
{
  EFI_STATUS  Status;

  if ((Hpmm->HpmmPendingTask & HP_TASK_REDO_EQ) != 0) {
    HpPerformEqualizationSetting (Hpmm->Die, Hpmm->Port, TRUE);
    HpPcieRetrainingWithOutEq (Hpmm->Die, Hpmm->Port);
    Status = HpmmSetTimer (Hpmm, HPMM_REDO_EQ_TIMEOUT);
    Hpmm->HpmmDoingTask   |= HP_TASK_WAIT_TIMEOUT;
    Hpmm->HpmmDoingTask   |= HP_TASK_REDO_EQ;
    Hpmm->HpmmPendingTask &= ~HP_TASK_REDO_EQ;
  }

  Status = HpmmCheckTimer (Hpmm);
  if (EFI_ERROR (Status)) {
    if (HpPcieCheckL0 (Hpmm->Die, Hpmm->Port)) {
      HpmmStopTimer (Hpmm);
      Hpmm->HpmmDoingTask &= ~HP_TASK_WAIT_TIMEOUT;
      Hpmm->HpmmDoingTask &= ~HP_TASK_REDO_EQ;
      return EFI_SUCCESS;
    }

    return Status;
  }

  Hpmm->HpmmDoingTask &= ~HP_TASK_WAIT_TIMEOUT;
  Hpmm->HpmmDoingTask &= ~HP_TASK_REDO_EQ;

  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/**
 * HpmmDoDelay
 *
 * @param[in] Hpmm        -   Pointer To HPMM_T
 * @retval
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
HpmmDoDelay (
  HPMM_T     *Hpmm,
  UINT32     DelayTicks
  )
{
  if ((Hpmm->HpmmDoingTask & HP_TASK_DO_DELAY) == 0) {
    HpmmSetTimer (Hpmm, DelayTicks);
    Hpmm->HpmmDoingTask |= HP_TASK_DO_DELAY;
    HpmmGoToState (Hpmm, hpmm_delay);
    return EFI_NOT_READY;
  }

  Hpmm->HpmmDoingTask &= ~HP_TASK_DO_DELAY;

  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/**
 * HpmmNextStep
 *
 * @param[in] Hpmm        -   Pointer To HPMM_T
 * @retval
/*----------------------------------------------------------------------------------------*/
HPMM_STATE
HpmmNextStep (
  HPMM_T     *Hpmm
  )
{
  EFI_STATUS                 Status;
  UINT8                      Die;
  UINT8                      Port;
  UINT32                     Task;
  HYGON_CPM_EQ_CONFIG_TABLE  *HygonEqTable;
  UINT32                     CpuStepMirrorAddr = 0x5a86c;
  UINT32                     Data;
  UINT8                      CpuGen;
  UINT8                      Stepping;

  Die  = Hpmm->Die;
  Port = Hpmm->Port;

  switch (Hpmm->CurrentState) {
    case hpmm_init:
      HpPcieGetLaneRange (Die, Port, &Hpmm->StartLane, &Hpmm->EndLane);
      HpmmGoToState (Hpmm, hpmm_card_check);
      break;

    case hpmm_card_check:
      if (IsPcieCardAdd (Die, Port)) {
        HpmmGoToState (Hpmm, hpmm_card_add);
      } else {
        HpmmGoToState (Hpmm, hpmm_card_remove);
      }

      break;

    case hpmm_card_add:
      Hpmm->InitLinkSpeed = HpPcieCurrentLinkSpeed (Hpmm->Die, Hpmm->Port);
      Hpmm->AerCeMask = HpGetAerCeMask (Hpmm->Die, Hpmm->Port);
      HpmmGoToState (Hpmm, hpmm_card_add_common_cfg);
      break;

    case hpmm_card_add_common_cfg:
      EqSmnRead (Die, CpuStepMirrorAddr, &Data);
      CpuGen = (UINT8) ((Data & 0xF0) >> 4);
      Stepping = (UINT8) (Data & 0xF);
      if (((CpuGen == 2) && (Stepping < 2)) ||
          ((Hpmm->LogicalId.Revision & HYGON_REV_F18_DN_Dx) && (Hpmm->LogicalId.Revision < HYGON_REV_F18_DN_D1))) {
        PcieCommonConfig (Die, Port, &Task);
        Hpmm->ForceLinkSpeed   = 3;
        Hpmm->HpmmPendingTask |= Task;
        Hpmm->HpmmPendingTask |= HP_TASK_BYPASS_RX_RECONFIG;
      }

      HpmmGoToState (Hpmm, hpmm_card_add_eq_configuration);
      break;

    case hpmm_card_add_eq_configuration:
      Status = HpGetEqTable (&HygonEqTable);
      if (!EFI_ERROR (Status)) {
        // HpDumpEqCfgTable (HygonEqTable);
        HpPcieEqConfiguration (Die, Port, Hpmm->StartLane, Hpmm->EndLane, HygonEqTable, cfg_eq_card_add, &Task);
        Hpmm->HpmmPendingTask |= Task;
      }

      HpmmGoToState (Hpmm, hpmm_card_add_rx_reconfig);
      break;

    case hpmm_card_add_rx_reconfig:
      if ((Hpmm->HpmmPendingTask & HP_TASK_BYPASS_RX_RECONFIG) == 0) {
        if(Hpmm->LogicalId.Revision & HYGON_REV_F18_DN_Cx) {
          HpDxioPhyReCfgHygonCx (Die, Port, Hpmm->StartLane, Hpmm->EndLane, &Task);
          Hpmm->HpmmPendingTask |= Task;      
        } else if (Hpmm->LogicalId.Revision & HYGON_REV_F18_DN_Dx) {
          HpDxioPhyReCfgHygonDx (Die, Port, Hpmm->StartLane, Hpmm->EndLane, &Task);
          Hpmm->HpmmPendingTask |= Task;
        }
      }
      HpmmGoToState (Hpmm, hpmm_card_add_deal_with_pending_task);
      break;

    case hpmm_card_add_deal_with_pending_task:
      if ((Hpmm->HpmmPendingTask & HP_TASK_SPEED_CHANGE) != 0) {
        HpmmGoToState (Hpmm, hpmm_speed_change);
      } else if ((Hpmm->HpmmPendingTask & HP_TASK_REDO_EQ) != 0) {
        HpmmGoToState (Hpmm, hpmm_redo_eq);
      } else {
        HpPcieEnableAllErrorReport (Die, Port, TRUE);
        HpmmGoToState (Hpmm, hpmm_done);
      }

      break;

    case hpmm_speed_change:
      if (((Hpmm->HpmmPendingTask & HP_TASK_SPEED_CHANGE) != 0) ||
          ((Hpmm->HpmmDoingTask & HP_TASK_SPEED_CHANGE) != 0)) {
        Status = HpmmSpeedChange (Hpmm);
        if (EFI_ERROR (Status)) {
          break;
        }
      }

      Status = HpmmDoDelay (Hpmm, HPMM_SPEED_CHANGE_TIMEOUT);
      if (EFI_ERROR (Status)) {
        break;
      }

      HpmmGoToState (Hpmm, hpmm_card_add_deal_with_pending_task);
      break;

    case hpmm_redo_eq:
      Status = HpmmRedoEq (Hpmm);
      if (EFI_ERROR (Status)) {
        break;
      }

      HpmmGoToState (Hpmm, hpmm_card_add_deal_with_pending_task);
      break;

    case hpmm_delay:
      if (Hpmm->Timer.Status == HPMM_TIMER_RUN) {
        Hpmm->HpmmDoingTask |= HP_TASK_WAIT_TIMEOUT;
        Status = HpmmCheckTimer (Hpmm);
        if (EFI_ERROR (Status)) {
          break;
        } else {
          Hpmm->HpmmDoingTask &= ~HP_TASK_WAIT_TIMEOUT;
        }
      }

      HpmmGoToState (Hpmm, Hpmm->PreviousState);
      break;

    case hpmm_card_remove:
      HpPcieClearAllErrors (Die, Port);
      HpPcieEnableAllErrorReport (Die, Port, FALSE);
      HpmmGoToState (Hpmm, hpmm_card_remove_resore);
      break;

    case hpmm_card_remove_resore:
#if 0
      if (Hpmm->LogicalId.Revision & (HYGON_REV_F18_DN_Cx)) {
        HpDxioPhyCfgInitHygonCx (Die, Port, Hpmm->StartLane, Hpmm->EndLane);
      } else if (Hpmm->LogicalId.Revision & HYGON_REV_F18_DN_Dx) {
        HpDxioPhyCfgInitHygonDx (Die, Port, Hpmm->StartLane, Hpmm->EndLane);
      }
#endif
      PcieCommonConfigRestore (Die, Port);
      Status = HpGetEqTable (&HygonEqTable);
      if (!EFI_ERROR (Status)) {
        // HpDumpEqCfgTable (HygonEqTable);
        HpPcieEqConfiguration (Die, Port, Hpmm->StartLane, Hpmm->EndLane, HygonEqTable, cfg_eq_card_remove, &Task);
      }
      HpmmGoToState (Hpmm, hpmm_done);
      break;

    case hpmm_done:
      NotifySmu (Die, Port);
      HpmmGoToState (Hpmm, hpmm_idle);
      break;
    case hpmm_idle:
      break;
      
    case hpmm_error:
      break;
  }

  return Hpmm->CurrentState;
}

/*----------------------------------------------------------------------------------------*/

/**
 * HpmmRun
 *
 * @param[in] Hpmm        -   Pointer To HPMM_T
 * @retval
/*----------------------------------------------------------------------------------------*/
HPMM_STATE
HpmmRun (
  HPMM_T     *Hpmm
  )
{
  HPMM_STATE  State;

  if ((Hpmm->CurrentState == hpmm_not_ready) || (Hpmm->CurrentState >= hpmm_idle)) {
    return Hpmm->CurrentState;
  }

  do {
    State = HpmmNextStep (Hpmm);
  } while ((State < hpmm_idle) && ((Hpmm->HpmmDoingTask & HP_TASK_WAIT_TIMEOUT) == 0));

  return Hpmm->CurrentState;
}

/*----------------------------------------------------------------------------------------*/

/**
 * HpmmInit
 *
 * @param[in] Hpmm        -   Pointer To HPMM_T
 * @retval
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
HpmmInit (
  HPMM_T *Hpmm
  )
{
  if ((hpmm_not_ready < Hpmm->CurrentState) && (Hpmm->CurrentState < hpmm_idle)) {
    return EFI_NOT_READY;
  }

  Hpmm->CurrentState  = hpmm_init;
  Hpmm->PreviousState = hpmm_init;
  Hpmm->StartLane     = 0;
  Hpmm->EndLane = 0;
  Hpmm->HpmmPendingTask = 0;
  Hpmm->HpmmDoingTask   = 0;
  Hpmm->InitLinkSpeed   = 0;
  Hpmm->ForceLinkSpeed  = 0;
  Hpmm->Timer.Counter   = 0;
  Hpmm->Timer.Timeout   = 0;
  Hpmm->Timer.Status    = HPMM_TIMER_STOP;

  return EFI_SUCCESS;
}
