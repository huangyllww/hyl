/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPM Initialization
 *
 * Contains CPM code to perform CPM initialization in SMM
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CPM
 * @e sub-project   Kernel
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
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
 */

#include <AmdCpmSmm.h>
#include <AmdCpmBaseIo.h>
#include <AmdCpmCpu.h>
#include <AmdCpmFch.h>
#include <AmdCpmTable.h>

CPM_DEFINE_gBS;
CPM_DEFINE_gSmst;

VOID
EFIAPI
AmdCpmSmmRegisterKernel (
  IN       VOID                    *This
  );


/*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the AMD CPM Init SMM driver
 *
 * This function installs AmdCpmTableSmmProtocol and registers CPM common functions
 * at AmdCpmInitSmm.
 *
 * @param[in]     ImageHandle    Pointer to the firmware file system header
 * @param[in]     SystemTable    Pointer to System table
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
EFI_STATUS
AmdCpmInitSmmEntryPoint (
  IN       EFI_HANDLE              ImageHandle,
  IN       EFI_SYSTEM_TABLE        *SystemTable
  )
{
  EFI_STATUS                      Status;
  EFI_HANDLE                      Handle;

  AMD_CPM_TABLE_PROTOCOL          *CpmTableProtocolPtr;
  AMD_CPM_TABLE_PROTOCOL          *CpmTableSmmProtocolPtr;

  If_CpmInSmm (ImageHandle, SystemTable, gBS, gSmst, Status) {

    //
    // Initialize global variables
    //
    Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (AMD_CPM_TABLE_PROTOCOL),
                    (VOID**)&CpmTableSmmProtocolPtr
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = gBS->LocateProtocol (
                    &gAmdCpmTableProtocolGuid,
                    NULL,
                    (VOID**)&CpmTableProtocolPtr
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    CpmTableSmmProtocolPtr->Revision      = CpmTableProtocolPtr->Revision;
    CpmTableSmmProtocolPtr->MainTablePtr  = CpmTableProtocolPtr->MainTablePtr;

    AmdCpmSmmRegisterKernel (CpmTableSmmProtocolPtr);
    CpmRegisterBaseIo (CpmTableSmmProtocolPtr);
    CpmRegisterCpu (CpmTableSmmProtocolPtr);
    CpmRegisterFch (CpmTableSmmProtocolPtr);

    Handle = NULL;
    // SGEZT#23339:Import Hygon Patch For U2 Hotplug Issue+>
    Status = gSmst->SmmInstallProtocolInterface (
                    &Handle,
                    &gAmdCpmTableSmmProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    CpmTableSmmProtocolPtr
                    );
    // SGEZT#23339:Import Hygon Patch For U2 Hotplug Issue ->
  }

  return Status;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Register Common Kenerl functions at the AmdCpmInitSmm entry point.
 *
 * This function registers CPM common kernel functions in AmdCpmTableSmmProtocol
 * at AmdCpmInitSmm.
 *
 *  @param[in]   This                Pointer to Protocol.
 *
 */
VOID
EFIAPI
AmdCpmSmmRegisterKernel (
  IN       VOID                    *This
  )
{
  AMD_CPM_TABLE_PROTOCOL  *CpmTableSmmProtocolPtr         = This;
  CpmTableSmmProtocolPtr->CommonFunction.GetTablePtr      = CpmGetTablePtr;
  CpmTableSmmProtocolPtr->CommonFunction.GetTablePtr2     = CpmGetTablePtr2;
  CpmTableSmmProtocolPtr->CommonFunction.RelocateTable    = CpmRelocateTableList;
  CpmTableSmmProtocolPtr->CommonFunction.CopyMem          = CpmCopyMem;
  CpmTableSmmProtocolPtr->CommonFunction.ReadSmbus        = NULL;
  CpmTableSmmProtocolPtr->CommonFunction.WriteSmbus       = NULL;
  CpmTableSmmProtocolPtr->CommonFunction.ReadSmbusBlock   = NULL;
  CpmTableSmmProtocolPtr->CommonFunction.WriteSmbusBlock  = NULL;
  return;
}
