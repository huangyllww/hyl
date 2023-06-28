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
#include <AMD.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/IdsLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmConfiguration.h>
#include <Protocol/SmmCpuIo2.h>
#include "IohcSmmSwSmiDispatcher.h"
#include "PcieHpCommon.h"
#include <Library/SmnAccessLib.h>
#include <Library/PciLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include <Protocol/FabricTopologyServices.h>
#include <GnbDxio.h>
#include <Library/NbioSmuV9Lib.h>
#include <Library/TimerLib.h>
#include <Protocol/AmdCpmTableProtocol/AmdCpmTableProtocol.h>

#define SW_SMI_EN             BIT16
#define HOTPLUG_FLAG          BIT0
#define SMN_INDEX_EXT_3_BIOS  0x00C0
#define SMN_INDEX_3_BIOS      0x00C4
#define SMN_DATA_3_BIOS       0x00C8

UINTN                          mNumberOfSockets = 0;
UINTN                          mTotalDieNumber = 0;
HP_DRIVER_PRIVATE_DATA         mHpData;
IOHC_SMM_COMMUNICATION_BUFFER  *mIohcCommunicationBufferPtr;

IOHC_SMM_DISPATCHER_TABLE  mIohcDispatcherTable[] = {
  {
    HOTPLUG_FLAG, // dedicate for nvme/pcie hotplug, it should never be changed
    IohcSmmNVMeEqDispatchHandler
  },
};

UINT8  mNumOfDispatcherTableEntry = sizeof (mIohcDispatcherTable) / sizeof (IOHC_SMM_DISPATCHER_TABLE);

/*----------------------------------------------------------------------------------------*/

/**
 * Entry point of the IOHC SW SMI SMM dispatcher driver
 *
 * @param[in]     ImageHandle    Pointer to the firmware file system header
 * @param[in]     SystemTable    Pointer to System table
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
IohcSmmSwSmiDispatcherEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                             Status;
  UINT32                                 Value;
  UINT8                                  Die;
  EFI_HANDLE                             ApuRasHandle = NULL;
  FCH_SMM_APURAS_REGISTER_CONTEXT        IohcSwSmiRegisterContext;
  FCH_SMM_APURAS_DISPATCH_PROTOCOL       *IohcSwSmiDispatch;
  AMD_FABRIC_TOPOLOGY_SERVICES_PROTOCOL  *FabricTopology;

  Status = gBS->LocateProtocol (
                  &gAmdFabricTopologyServicesProtocolGuid,
                  NULL,
                  &FabricTopology
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (FabricTopology->GetSystemInfo (FabricTopology, &mNumberOfSockets, &mTotalDieNumber, NULL) != EFI_SUCCESS) {
    return EFI_DEVICE_ERROR;
  }

  DEBUG ((EFI_D_INFO, "IOHC SM SW mNumberOfSockets = %d, mTotalDieNumber = %d\n", mNumberOfSockets, mTotalDieNumber));

  //
  // Clear all SMI status bit
  //
  // clear SW_SMI_CNTL. Writing 1 to bit[x] inverts the value of bit[x].
  for (Die = 0; Die < mTotalDieNumber; Die++) {
    EqSmnRead (Die, SW_SMI_CNTL, &Value);
    EqSmnWrite (Die, SW_SMI_CNTL, &Value);
    EqSmnRW (Die, NB_RAS_CONTROL, ~(UINT32)SW_SMI_EN, SW_SMI_EN);
    NotifySmu (Die, 0);
  }

  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (IOHC_SMM_COMMUNICATION_BUFFER),
                    &mIohcCommunicationBufferPtr
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gSmst->SmmLocateProtocol (
                    &gFchSmmApuRasDispatchProtocolGuid,
                    NULL,
                    &IohcSwSmiDispatch
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  HpPrivateDataInit ();

  // Only register handler for master die
  IohcSwSmiRegisterContext.Bus   = 0;
  IohcSwSmiRegisterContext.Die   = 0;
  IohcSwSmiRegisterContext.Order = 0x1;

  Status = IohcSwSmiDispatch->Register (
                                IohcSwSmiDispatch,
                                IohcSwSmiDispatchHandler,
                                &IohcSwSmiRegisterContext,
                                &ApuRasHandle
                                );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return Status;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Initialize Hotplug Private Data
 *
 * @param[in]     VOID
 *
 * @retval        EFI_SUCCESS    Initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
HpPrivateDataInit (
  IN       VOID
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;

  mHpData.CpuGen = GetModelId ();
  BaseGetLogicalIdOnExecutingCore (&mHpData.LogicalId);
  mHpData.HpmmsState     = HPMMS_STATE_IDLE;
  mHpData.CoreTimerState = HP_CORE_TIMER_STOP;
  HpmmsInit (&mHpData);

  Status = gSmst->SmmLocateProtocol (
                    &gFchSmmPeriodicalDispatch2ProtocolGuid,
                    NULL,
                    &mHpData.AmdNbioPeriodicalDispatch
                    );

  ASSERT_EFI_ERROR (Status);
  mHpData.InitStatus = Status;

  return Status;
}

/*----------------------------------------------------------------------------------------*/

/**
 * IOHC SMM dispatcher handler
 *
 *
 * @param[in]         DispatchHandle        Dispatcher Handle
 * @param[in]         RegisterContext       FCH APU RAS Register Context
 * @retval            EFI_SUCCESS           SMI handled by dispatcher
 * @retval            EFI_UNSUPPORTED       SMI not supported by dispcther
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
IohcSwSmiDispatchHandler (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       FCH_SMM_APURAS_REGISTER_CONTEXT   *MiscRegisterContext
  )
{
  UINT8       DispatcherIndex;
  UINT32      SwSmiRegData;
  UINT8       DieIndex;
  VOID        *CommBuffer;
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT32      IohcGlobalHi;
  

  for (DieIndex = 0; DieIndex < mTotalDieNumber; DieIndex++) {
    IohcGlobalHi = HpIohcGlobalStatus (DieIndex);
    if ((IohcGlobalHi & 0xFFFF) != 0) {
      HpClearPcieHotplugErrors (DieIndex, IohcGlobalHi);
    }

    EqSmnRead (DieIndex, SW_SMI_CNTL, &SwSmiRegData);

    for (DispatcherIndex = 0; DispatcherIndex < mNumOfDispatcherTableEntry; DispatcherIndex++ ) {
      if (((SwSmiRegData & mIohcDispatcherTable[DispatcherIndex].SmiStatusBit) != 0) &&
          (mIohcDispatcherTable[DispatcherIndex].SmiDispatcher != NULL)) {
        mIohcCommunicationBufferPtr->SmiStatusBit = mIohcDispatcherTable[DispatcherIndex].SmiStatusBit;
        mIohcCommunicationBufferPtr->DieNum = DieIndex;
        CommBuffer = (VOID *)mIohcCommunicationBufferPtr;
        // 1. callback function
        Status = mIohcDispatcherTable[DispatcherIndex].SmiDispatcher (DispatchHandle, CommBuffer, NULL);

        // 2. Clear Smi Status bit. Writing 1 to bit[x] inverts the value of bit[x].
        EqSmnWrite (DieIndex, SW_SMI_CNTL, &(mIohcCommunicationBufferPtr->SmiStatusBit));
      }
    }
  }

  return Status;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Hotplug Periodical Smi handler
 *
 *
 * @param[in]         DispatchHandle        Dispatcher Handle
 * @param[in]         RegisterContext       FCH SMM Periodical Register Context
 * @param[in][out]    PeriodicTimerContext  FCH SMM Periodical Timer Context
 * @param[in][out]    SizeOfContext         Size of Context
 * @retval            EFI_SUCCESS
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
HpPeriodicSmiCallback (
  IN       EFI_HANDLE                                DispatchHandle,
  IN       CONST FCH_SMM_PERIODICAL_REGISTER_CONTEXT *RegisterContext,
  IN OUT   EFI_SMM_PERIODIC_TIMER_CONTEXT            *PeriodicTimerContext,
  IN OUT   UINTN                                     *SizeOfContext
  )
{
  if (BusyHpmmExist (&mHpData)) {
    HpmmsTaskMaster (&mHpData);
  } else {
    mHpData.CoreTimerState = HP_CORE_TIMER_STOP;
    mHpData.HpmmsState     = HPMMS_STATE_IDLE;
    mHpData.AmdNbioPeriodicalDispatch->UnRegister (
                                         mHpData.AmdNbioPeriodicalDispatch,
                                         mHpData.HpPeriodicSmiHandle
                                         );
  }

  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Start Hotplug Process For Dedicate Pcie Port
 *
 *
 * @param[in]         Die                   Die Number
 * @param[in]         Port                  Port Number
 * @retval
 */
/*----------------------------------------------------------------------------------------*/
VOID
StartOneHpmm (
  UINT8  Die,
  UINT8  Port
  )
{
  HpmmInit (&mHpData.Hpmm[Die][Port]);
  HpmmRun (&mHpData.Hpmm[Die][Port]);
}

/*----------------------------------------------------------------------------------------*/

/**
 * Register Hotplug Periodic Smi Handler
 *
 * @retval
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
HpPeriodicSmiManager (
  IN                  VOID
  )
{
  EFI_STATUS                           Status = EFI_SUCCESS;
  FCH_SMM_PERIODICAL_REGISTER_CONTEXT  PeriodicalRegisterContext;

  if (mHpData.CoreTimerState != HP_CORE_TIMER_STOP) {
    return EFI_SUCCESS;
  }

  mHpData.CoreTimerState = HP_CORE_TIMER_RUN;
  PeriodicalRegisterContext.SmiTickInterval = LONG_TIMER_SMI_INTERVAL;
  PeriodicalRegisterContext.Period   = 10 * LONG_TIMER_SMI_INTERVAL; // 10ms Period
  PeriodicalRegisterContext.StartNow = 1;

  Status = mHpData.AmdNbioPeriodicalDispatch->Register (
                                                mHpData.AmdNbioPeriodicalDispatch,
                                                HpPeriodicSmiCallback,
                                                &PeriodicalRegisterContext,
                                                &mHpData.HpPeriodicSmiHandle
                                                );
  return Status;
}

/*----------------------------------------------------------------------------------------*/

/**
 * NVMe EQ Dispatch Handler
 *
 *
 * @param[in]         DispatchHandle      Dispatcher Handle
 * @param[in, out]    CommBuffer          Communication Buffer
 * @param[in, out]    CommBufferSize      Buffer size
 *
 * @retval            EFI_SUCCESS         Always Return Success
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
IohcSmmNVMeEqDispatchHandler (
  IN       EFI_HANDLE   DispatchHandle,
  IN OUT   VOID         *CommBuffer OPTIONAL,
  IN OUT   UINTN        *CommBufferSize OPTIONAL
  )
{
  UINT8   CurrentDieNum = ((IOHC_SMM_COMMUNICATION_BUFFER *)CommBuffer)->DieNum;
  UINT32  Port;

  EqSmnRead (CurrentDieNum, MP1_C2PMSG_ARGUMENT_5_ADDRESS, &Port);
  // PCIeHotPlugConfig (CurrentDieNum, Port);
  StartOneHpmm (CurrentDieNum, (UINT8)Port);
  HpPeriodicSmiManager ();

  // Allways return success, because even if it fails, system should not hang
  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/**
 * EQ Processing Completion Notification.
 * Message box register0 stores port number which has been set
 *
 * @param[in]         DieNum    Notified Die Number
 * @param[in]         PortNum   Processed Port Number
 * @retval
 */
/*----------------------------------------------------------------------------------------*/
VOID
NotifySmu (
  IN       UINT32              DieNum,
  IN       UINT32              PortNum
  )
{
  PCI_ADDR                             PciAddr;
  UINTN                                Socket;
  UINTN                                Die;
  UINT32                               SmuArg[6];
  EFI_STATUS                           Status;
  
  PciAddr.AddressValue = 0;
  Socket = DieNum / FabricTopologyGetNumberOfDiesOnSocket(0);
  Die = DieNum % FabricTopologyGetNumberOfDiesOnSocket(0);
  
  PciAddr.Address.Bus = (UINT8)FabricTopologyGetHostBridgeBusBase (Socket, Die, 0);
  
  NbioSmuServiceCommonInitArgumentsV9 (SmuArg);
  SmuArg[0] = PortNum;
  
  Status = NbioSmuServiceRequestV9(PciAddr, BIOSSMC_MSG_PcieEQParamsOptionFinished, SmuArg, 0);
}

/*----------------------------------------------------------------------------------------*/

/**
 * EqSmnRead - Read SMN register
 *
 * @param[in] DieNum          - Die number
 * @param[in] Address         - Register SMN address
 * @param[in,out] Value       - Pointer to register value
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
EqSmnRead (
  IN       UINT32              DieNum,
  IN       UINT32              Address,
  IN       UINT32              *Value
  )
{
  UINTN    Socket;
  UINTN    Die;
  UINT32   IohcBus;
  
  Socket = DieNum / FabricTopologyGetNumberOfDiesOnSocket(0);
  Die = DieNum % FabricTopologyGetNumberOfDiesOnSocket(0);
  
  IohcBus = (UINT32)FabricTopologyGetHostBridgeBusBase (Socket, Die, 0);
  
  SmnRegisterRead (IohcBus, Address, Value);
}

/*----------------------------------------------------------------------------------------*/

/**
 * EqSmnWrite - Write SMN register
 *
 * @param[in] DieNum          - Die number
 * @param[in] Address         - Register SMN address
 * @param[in,out] Value       - Pointer to register value
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
EqSmnWrite (
  IN       UINT32              DieNum,
  IN       UINT32              Address,
  IN       UINT32              *Value
  )
{
  UINTN    Socket;
  UINTN    Die;
  UINT32   IohcBus;
  
  Socket = DieNum / FabricTopologyGetNumberOfDiesOnSocket(0);
  Die = DieNum % FabricTopologyGetNumberOfDiesOnSocket(0);
  
  IohcBus = (UINT32)FabricTopologyGetHostBridgeBusBase (Socket, Die, 0);
  
  SmnRegisterWrite (IohcBus, Address, Value, 0);
}

/*----------------------------------------------------------------------------------------*/

/**
 * EqSmnRW - Read and Write SMN register
 *
 * @param[in] DieNum          - Die number
 * @param[in] Address         - Register SMN address
 * @param[in] AndMask         - And Mask
 * @param[in] OrMask          - Or Mask
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
EqSmnRW (
  IN       UINT32              DieNum,
  IN       UINT32              Address,
  IN       UINT32              AndMask,
  IN       UINT32              OrMask
  )
{
  UINT32  RegValue;

  EqSmnRead (DieNum, Address, &RegValue);
  RegValue &= AndMask;
  RegValue |= OrMask;
  EqSmnWrite (DieNum, Address, &RegValue);
}

/*----------------------------------------------------------------------------------------*/

/**
 * HpGetEqTable - Get Eq Configuration Table
 *
 * @param[in] DieNum          - Die number
 * @param[in] Address         - Register SMN address
 * @param[in] AndMask         - And Mask
 * @param[in] OrMask          - Or Mask
 *
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
HpGetEqTable (
  HYGON_CPM_EQ_CONFIG_TABLE  **HygonEqTable
  )
{
  EFI_STATUS              Status;
  AMD_CPM_TABLE_PROTOCOL  *SmmCpmProtocol;

  Status = gSmst->SmmLocateProtocol (&gAmdCpmTableSmmProtocolGuid, NULL, &SmmCpmProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  *HygonEqTable = SmmCpmProtocol->CommonFunction.GetTablePtr (SmmCpmProtocol, CPM_SIGNATURE_EQ_CONFIG);

  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/**
 * HpDumpEqCfgTable - Dump Configuration Table
 *
 * @param[in] EqCfgTable          - Eq Configuration Table Pointer
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
HpDumpEqCfgTable (
  IN  HYGON_CPM_EQ_CONFIG_TABLE  *EqCfgTable
  )
{
  UINT32  i, j;

  for (i = 0; (EqCfgTable->EqCfgList[i].Flag & EQ_TABLE_END) != EQ_TABLE_END; i++) {
    if ((EqCfgTable->EqCfgList[i].Type & EQ_TYPE_MASK) == pcie_eq_type) {
      DEBUG ((EFI_D_ERROR, "PCIE_EQ { 0x%x, ", EqCfgTable->EqCfgList[i].Flag));
      DEBUG ((EFI_D_ERROR, "0x%x, ", EqCfgTable->EqCfgList[i].Type));
      DEBUG ((EFI_D_ERROR, "0x%x, ", EqCfgTable->EqCfgList[i].CpuGen));
      DEBUG ((EFI_D_ERROR, "0x%x, ", EqCfgTable->EqCfgList[i].Die));
      DEBUG ((EFI_D_ERROR, "0x%x, ", EqCfgTable->EqCfgList[i].VidDid));
      DEBUG ((EFI_D_ERROR, "0x%x, ", EqCfgTable->EqCfgList[i].SubSysVidDid));
      DEBUG ((EFI_D_ERROR, "0x%x, ", EqCfgTable->EqCfgList[i].StartLane));
      DEBUG ((EFI_D_ERROR, "0x%x, ", EqCfgTable->EqCfgList[i].EndLane));
      DEBUG ((EFI_D_ERROR, "0x%x, ", EqCfgTable->EqCfgList[i].PerPort));
      DEBUG ((EFI_D_ERROR, "{ "));
      for (j = 0; j < 16; j++) {
        DEBUG ((EFI_D_ERROR, "0x%x, ", EqCfgTable->EqCfgList[i].CfgValue[j]));
      }

      DEBUG ((EFI_D_ERROR, "}}\n"));
    }
  }
}

/*----------------------------------------------------------------------------------------*/

/**
 * HpmmsInit - Initialize Hotplug Management Machine State
 *
 * @param[in] HpPrivateData    -  Pointer To Hotplug Driver Private Data
 *
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
HpmmsInit (
  HP_DRIVER_PRIVATE_DATA *HpPrivateData
  )
{
  UINT8  Die;
  UINT8  Port;

  for (Die = 0; Die < MAX_DIE_COUNT; Die++) {
    for (Port = 0; Port < MAX_PORT_COUNT_PER_DIE; Port++) {
      HpPrivateData->Hpmm[Die][Port].LogicalId = HpPrivateData->LogicalId;
      HpPrivateData->Hpmm[Die][Port].Die  = Die;
      HpPrivateData->Hpmm[Die][Port].Port = Port;
      HpPrivateData->Hpmm[Die][Port].StartLane       = 0;
      HpPrivateData->Hpmm[Die][Port].EndLane         = 0;
      HpPrivateData->Hpmm[Die][Port].CurrentState    = hpmm_not_ready;
      HpPrivateData->Hpmm[Die][Port].PreviousState   = hpmm_not_ready;
      HpPrivateData->Hpmm[Die][Port].HpmmPendingTask = 0;
      HpPrivateData->Hpmm[Die][Port].HpmmDoingTask   = 0;
      HpPrivateData->Hpmm[Die][Port].CardAddEver     = 0;
      HpPrivateData->Hpmm[Die][Port].InitLinkSpeed   = 0;
      HpPrivateData->Hpmm[Die][Port].ForceLinkSpeed  = 0;
      HpPrivateData->Hpmm[Die][Port].AerCeMask       = 0;
      HpPrivateData->Hpmm[Die][Port].Timer.Counter   = 0;
      HpPrivateData->Hpmm[Die][Port].Timer.Timeout   = 0;
      HpPrivateData->Hpmm[Die][Port].Timer.Status    = HPMM_TIMER_STOP;
    }
  }

  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/**
 * HpmmsTaskMaster - Initialize Task
 *
 * @param[in] HpPrivateData    -  Pointer To Hotplug Driver Private Data
 *
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
HpmmsTaskMaster (
  HP_DRIVER_PRIVATE_DATA *HpPrivateData
  )
{
  UINT32  Die;
  UINT32  Port;

  for (Die = 0; Die < MAX_DIE_COUNT; Die++) {
    for (Port = 0; Port < MAX_PORT_COUNT_PER_DIE; Port++) {
      HpmmTimerTickTock (&HpPrivateData->Hpmm[Die][Port]);
      HpmmRun (&HpPrivateData->Hpmm[Die][Port]);
    }
  }

  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/**
 * BusyHpmmExist - Check Busy State
 *
 * @param[in] HpPrivateData    -  Pointer To Hotplug Driver Private Data
 *
 */
/*----------------------------------------------------------------------------------------*/
BOOLEAN
BusyHpmmExist (
  HP_DRIVER_PRIVATE_DATA *HpPrivateData
  )
{
  UINT8   Die;
  UINT8   Port;
  UINT32  BusyHpmmCount = 0;

  for (Die = 0; Die < MAX_DIE_COUNT; Die++) {
    for (Port = 0; Port < MAX_PORT_COUNT_PER_DIE; Port++) {
      if((HpPrivateData->Hpmm[Die][Port].CurrentState != hpmm_idle) &&
         (HpPrivateData->Hpmm[Die][Port].CurrentState != hpmm_error) &&
         (HpPrivateData->Hpmm[Die][Port].CurrentState != hpmm_not_ready)) {
        BusyHpmmCount++;
        break;
      }
    }
  }

  return (BusyHpmmCount > 0) ? TRUE : FALSE;
}
