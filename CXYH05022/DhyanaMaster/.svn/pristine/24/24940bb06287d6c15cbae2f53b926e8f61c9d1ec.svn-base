/* $NoKeywords:$ */

/**
 * @file
 *
 * IOHC SW SMI Dispatcher Driver
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

#ifndef _IOHC_SMM_SW_SMI_DISPATCHER_H_
#define _IOHC_SMM_SW_SMI_DISPATCHER_H_

#include <Protocol/FchSmmApuRasDispatch.h>
#include <AmdSoc.h>
#include <Library/BaseCoreLogicalIdLib.h>
#include <Protocol/FchSmmPeriodicalDispatch2.h>
#include <AmdCpmBase.h>
#include "Hpmm.h"

#define NB_RAS_CONTROL      0x13B2015C
#define SW_SMI_CNTL         0x13B1010C
#define SMN_INDEX_2         0xB8
#define SMN_DATA_2          0xBC
#define SMM_CFG_INDEX_PORT  0xCF8
#define SMM_CFG_DATA_PORT   0xCFC
#define HP_CORE_TIMER_STOP  0
#define HP_CORE_TIMER_RUN   1

typedef struct {
  UINTN    SwSmiCpuIndex;                   ///< 0-based CPU ID
  UINT8    CommandPort;                     ///< SMI Command port
  UINT8    DataPort;                        ///< SMI Data Port
} IOHC_SMM_SW_CONTEXT;

typedef struct {
  UINT32    SmiStatusBit;                  ///< Status Bit
  UINT8     DieNum;                        ///< Die Number
} IOHC_SMM_COMMUNICATION_BUFFER;

typedef EFI_STATUS (EFIAPI *IOHC_SMM_CHILD_DISPATCHER_HANDLER)(
  IN      EFI_HANDLE     DispatchHandle,
  IN OUT  VOID           *CommunicationBuffer OPTIONAL,
  IN OUT  UINTN          *SourceSize OPTIONAL
  );

///
/// IOHC SMM Dispatcher Structure
///
typedef struct {
  UINT32                               SmiStatusBit;    ///< Status Bit
  IOHC_SMM_CHILD_DISPATCHER_HANDLER    SmiDispatcher;   ///< Dispatcher Address
} IOHC_SMM_DISPATCHER_TABLE;

typedef struct {
  UINTN    IohcSwSmiValue;                  ///< SW SMI value
  UINT8    Order;                           ///< Priority 0-Highest (reserved), 0xFF-Lowest (reserved)
} IOHC_SMM_SW_REGISTER_CONTEXT;

typedef
EFI_STATUS
(EFIAPI *IOHC_SMM_SW_HANDLER_ENTRY_POINT)(
  IN       EFI_HANDLE                          DispatchHandle,
  IN CONST IOHC_SMM_SW_REGISTER_CONTEXT        *DispatchContext,
  IN OUT   IOHC_SMM_SW_CONTEXT                 *SwContext,
  IN OUT   UINTN                               *SizeOfSwContext
  );

typedef struct {
  EFI_STATUS                               InitStatus;
  CPU_GEN                                  CpuGen;
  SOC_LOGICAL_ID                           LogicalId;
  FCH_SMM_PERIODICAL_DISPATCH2_PROTOCOL    *AmdNbioPeriodicalDispatch;
  EFI_HANDLE                               HpPeriodicSmiHandle;
  UINT32                                   CoreTimerState;
  UINT32                                   HpmmsState;
  HPMM_T                                   Hpmm[MAX_DIE_COUNT][MAX_PORT_COUNT_PER_DIE];
} HP_DRIVER_PRIVATE_DATA;

VOID
SaveSmnIndexDataRegisters (
  IN VOID
  );

VOID
RestoreSmnIndexDataRegisters (
  IN VOID
  );

EFI_STATUS
EFIAPI
IohcSmmNVMeEqDispatchHandler (
  IN       EFI_HANDLE   SmmImageHandle,
  IN OUT   VOID         *CommunicationBuffer OPTIONAL,
  IN OUT   UINTN        *SourceSize OPTIONAL
  );

EFI_STATUS
EFIAPI
IohcSwSmiDispatchHandler (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       FCH_SMM_APURAS_REGISTER_CONTEXT   *MiscRegisterContext
  );

VOID
EqSmnRead (
  IN       UINT32              DieNum,
  IN       UINT32              Address,
  IN       UINT32              *Value
  );

VOID
EqSmnWrite (
  IN       UINT32              DieNum,
  IN       UINT32              Address,
  IN       UINT32              *Value
  );

VOID
EqSmnRW (
  IN       UINT32              DieNum,
  IN       UINT32              Address,
  IN       UINT32              AndMask,
  IN       UINT32              OrMask
  );

EFI_STATUS
HpPrivateDataInit (
  IN       VOID
  );

VOID
StartOneHpmm (
  UINT8  Die,
  UINT8  Port
  );

EFI_STATUS
HpPeriodicSmiManager (
  IN VOID
  );

VOID
NotifySmu (
  IN       UINT32              DieNum,
  IN       UINT32              PortNum
  );

EFI_STATUS
HpGetEqTable (
  HYGON_CPM_EQ_CONFIG_TABLE  **HygonEqTable
  );

VOID
HpDumpEqCfgTable (
  IN  HYGON_CPM_EQ_CONFIG_TABLE  *EqCfgTable
  );

EFI_STATUS
HpmmsInit (
  HP_DRIVER_PRIVATE_DATA *HpPrivateData
  );

BOOLEAN
BusyHpmmExist (
  HP_DRIVER_PRIVATE_DATA *HpPrivateData
  );

EFI_STATUS
HpmmsTaskMaster (
  HP_DRIVER_PRIVATE_DATA *HpPrivateData
  );

#endif
