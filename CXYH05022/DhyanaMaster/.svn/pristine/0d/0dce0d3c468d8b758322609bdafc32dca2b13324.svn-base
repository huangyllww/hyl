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

#ifndef __HPMM_H__
#define __HPMM_H__
#include <Base.h>
#include <Uefi.h>
#include <AmdSoc.h>
#include <Library/BaseCoreLogicalIdLib.h>

#define MAX_DIE_COUNT              8
#define MAX_PORT_COUNT_PER_DIE     16
#define HPMM_TIMER_RUN             0
#define HPMM_TIMER_STOP            1
#define HPMMS_STATE_IDLE           0
#define HPMMS_STATE_WORK           1
#define HPMM_REDO_EQ_TIMEOUT       5 // 50ms timeout
#define HPMM_SPEED_CHANGE_TIMEOUT  3 // 30ms timeout

typedef enum {
  hpmm_not_ready,
  hpmm_init,
  hpmm_card_check,
  hpmm_card_add,
  hpmm_card_add_common_cfg,
  hpmm_card_add_eq_configuration,
  hpmm_card_add_rx_reconfig,
  hpmm_card_add_deal_with_pending_task,
  hpmm_speed_change,
  hpmm_redo_eq,
  hpmm_delay,
  hpmm_card_remove,
  hpmm_card_remove_resore,
  hpmm_done,
  hpmm_idle,
  hpmm_error
} HPMM_STATE;

typedef struct {
  UINT8    Status;
  UINTN    Counter;
  UINTN    Timeout;
} HPMM_TIMER;

typedef struct {
  SOC_LOGICAL_ID    LogicalId;
  UINT8             Die;
  UINT8             Port;
  UINT8             StartLane;
  UINT8             EndLane;
  HPMM_STATE        CurrentState;
  HPMM_STATE        PreviousState;
  UINT32            HpmmPendingTask;
  UINT32            HpmmDoingTask;
  UINT32            CardAddEver;
  UINT32            InitLinkSpeed;
  UINT32            ForceLinkSpeed;
  UINT32            AerCeMask;
  HPMM_TIMER        Timer;
} HPMM_T;

HPMM_STATE
HpmmRun (
  HPMM_T *Hpmm
  );

EFI_STATUS
HpmmInit (
  HPMM_T *Hpmm
  );

EFI_STATUS
HpmmTimerTickTock (
  HPMM_T *Hpmm
  );

#endif
