/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CCX MP Services DXE Library
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision: 312770 $   @e \$Date: 2015-02-11 13:24:42 +0800 (Wed, 11 Feb 2015) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 **/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <PiDxe.h>
#include <Filecode.h>
#include <Library/AmdBaseLib.h>
#include <Library/CcxMpServicesLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/CcxRolesLib.h>
#include <Protocol/AmdCoreTopologyProtocol.h>
#include <Protocol/FabricTopologyServices.h>


UINTN                      NumberOfThreadsPerCore  = 0;
UINTN                      NumberOfCoresPerComplex = 0;
UINTN                      NumberOfComplexesPerDie = 0;
UINTN                      NumberOfDiesPerSocket   = 0;
UINTN                      NumberOfSockets         = 0;


/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE  LIBRARY_CCXMPSERVICESSMMLIB_CCXMPSERVICESSMMLIB_FILECODE


/*---------------------------------------------------------------------------------------*/
/**
 * Run function on selected APs
 *
 *
 * @param[in]  CoreSelector       core selector
 * @param[in]  RunThisFunction    function
 * @param[in]  Argument           argument
 * @param[in]  BlockingMode       BLOCKING_MODE     - execute in blocking mode. BSP waits until this AP finishes
 *                                NON_BLOCKING_MODE - execute in non-blocking mode. BPS won't wait for AP
 *                                WARNING: LIB instance for SMM won't support NonBlockingMode
 *
 * @retval     EFI_STATUS
 *
 */
EFI_STATUS
CcxRunFunctionOnAps (
  IN       CCX_MP_SERVICES_CORE_SELECTOR  CoreSelector,
  IN       EFI_AP_PROCEDURE               RunThisFunction,
  IN       VOID                          *Argument,
  IN       CCX_MP_SERVICES_BLOCKING_MODE  BlockingMode
  )
{
  UINTN       StartThisAp;
  UINTN       StartThisApFrom;
  UINTN       StartThisApTo;
  UINTN       StartThisApSteps;
  EFI_STATUS  Status;
  EFI_STATUS  CalledStatus;

  if (!CcxIsBsp (NULL)) {
    ASSERT (FALSE);
    return EFI_UNSUPPORTED;
  }

  if ((NumberOfSockets == 0) || (NumberOfDiesPerSocket == 0) || (NumberOfComplexesPerDie == 0) || (NumberOfCoresPerComplex == 0) || (NumberOfThreadsPerCore == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  // Get StartThisApFrom, StartThisApTo, StartThisApSteps
  StartThisApTo = gSmst->NumberOfCpus;
  ASSERT (StartThisApTo == (NumberOfThreadsPerCore * NumberOfCoresPerComplex * NumberOfComplexesPerDie * NumberOfDiesPerSocket * NumberOfSockets));

  switch (CoreSelector) {
  case ALL_THREADS:
    StartThisApFrom = 1;
    StartThisApSteps = 1;
    break;
  case ALL_CORE_PRIMARY:
    StartThisApFrom = 0 + NumberOfThreadsPerCore;
    StartThisApSteps = StartThisApFrom;
    break;
  case ALL_COMPLEX_PRIMARY:
    StartThisApFrom = 0 + (NumberOfThreadsPerCore * NumberOfCoresPerComplex);
    StartThisApSteps = StartThisApFrom;
    break;
  case ALL_DIE_PRIMARY:
    StartThisApFrom = 0 + (NumberOfThreadsPerCore * NumberOfCoresPerComplex * NumberOfComplexesPerDie);
    StartThisApSteps = StartThisApFrom;
    break;
  case ALL_SOCKET_PRIMARY:
    StartThisApFrom = 0 + (NumberOfThreadsPerCore * NumberOfCoresPerComplex * NumberOfComplexesPerDie * NumberOfDiesPerSocket);
    StartThisApSteps = StartThisApFrom;
    break;
  default:
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  // Run function on APs
  Status = EFI_SUCCESS;
  for (StartThisAp = StartThisApFrom; StartThisAp < StartThisApTo; StartThisAp += StartThisApSteps) {
    CalledStatus = gSmst->SmmStartupThisAp (RunThisFunction, StartThisAp, Argument);
    Status = (CalledStatus > Status) ? CalledStatus : Status;
  }

  return Status;
}


/*---------------------------------------------------------------------------------------*/
/**
 * CcxMpServicesSmmLibConstructor
 *
 * initial the global variable
 *
 */
EFI_STATUS
EFIAPI
CcxMpServicesSmmLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  UINTN       TotalNumberOfDie;
  EFI_STATUS  Status;
  EFI_STATUS  CalledStatus;
  AMD_CORE_TOPOLOGY_SERVICES_PROTOCOL   *CoreTopology;
  AMD_FABRIC_TOPOLOGY_SERVICES_PROTOCOL *FabricTopology;

  Status = EFI_SUCCESS;
  // Get system topology information
  if ((NumberOfSockets == 0) || (NumberOfDiesPerSocket == 0) || (NumberOfComplexesPerDie == 0) || (NumberOfCoresPerComplex == 0) || (NumberOfThreadsPerCore == 0)) {
    CalledStatus = gBS->LocateProtocol (&gAmdFabricTopologyServicesProtocolGuid, NULL, &FabricTopology);
    Status = (CalledStatus > Status) ? CalledStatus : Status;
    CalledStatus = gBS->LocateProtocol (&gAmdCoreTopologyServicesProtocolGuid, NULL, &CoreTopology);
    Status = (CalledStatus > Status) ? CalledStatus : Status;

    if (Status == EFI_SUCCESS) {
      FabricTopology->GetSystemInfo (FabricTopology, &NumberOfSockets, &TotalNumberOfDie, NULL);
      NumberOfDiesPerSocket = TotalNumberOfDie / NumberOfSockets;

      CoreTopology->GetCoreTopologyOnDie (CoreTopology, 0, 0, &NumberOfComplexesPerDie, &NumberOfCoresPerComplex, &NumberOfThreadsPerCore);
    }
  }
  return EFI_SUCCESS;
}

