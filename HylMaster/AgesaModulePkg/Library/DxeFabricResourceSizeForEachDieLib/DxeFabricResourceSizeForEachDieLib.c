/* $NoKeywords:$ */
/**
 * @file
 *
 * Get MMIO size for each Die
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Library
 * @e \$Revision$   @e \$Date$
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
 */


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "Uefi.h"
#include "PiDxe.h"
#include "Filecode.h"
#include "AGESA.h"
#include <Library/AmdBaseLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/FabricResourceManagerServicesProtocol.h>

#define FILECODE LIBRARY_DXEFABRICRESOURCESIZEFOREACHDIELIB_DXEFABRICRESOURCESIZEFOREACHDIELIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * FabricGetResourceSizeForEachDie
 *
 * Try to get user's request about MMIO/IO size for each Die from NV variable
 *
 * @param[in, out]    MmioSizeForEachDie        User's request about Mmio size for each Die
 *
 * @retval            EFI_SUCCESS               Get user's request from NV variable successfully
 */
EFI_STATUS
FabricGetResourceSizeForEachDie (
  IN       FABRIC_RESOURCE_FOR_EACH_DIE *ResourceSizeForEachDie
  )
{
  UINTN                             VariableSize;
  EFI_STATUS                        Status;

  VariableSize = sizeof (FABRIC_RESOURCE_FOR_EACH_DIE);
  Status = gRT->GetVariable (
                  L"ResourceSizeForEachDie",
                  &gAmdResourceSizeForEachDieGuid,
                  NULL,
                  &VariableSize,
                  (VOID *) ResourceSizeForEachDie
                  );

  return Status;
}

/*---------------------------------------------------------------------------------------*/
/**
 * FabricSetResourceSizeForEachDie
 *
 * Save user's request about MMIO/IO size for each Die to NV variable
 *
 * @param[in]    CurrentUsed        Current used Mmio size for each Die
 * @param[in]    UserRequest        User's request about Mmio size for each Die
 *
 * @retval       EFI_SUCCESS        Save user's request to NV variable successfully
 */
EFI_STATUS
FabricSetResourceSizeForEachDie (
  IN       FABRIC_RESOURCE_FOR_EACH_DIE *CurrentUsed,
  IN       FABRIC_RESOURCE_FOR_EACH_DIE *UserRequest
  )
{
  UINT8  i;
  UINT8  j;
  UINTN  VariableSize;
  EFI_STATUS Status;
  FABRIC_RESOURCE_FOR_EACH_DIE CurrentRequest;

  // Calculate total size for each DIE
  for (i = 0; i < MAX_SOCKETS_SUPPORTED; i++) {
    for (j = 0; j < MAX_DIES_PER_SOCKET; j++) {
      CurrentRequest.NonPrefetchableMmioSizeAbove4G[i][j].Size = CurrentUsed->NonPrefetchableMmioSizeAbove4G[i][j].Size + UserRequest->NonPrefetchableMmioSizeAbove4G[i][j].Size;
      CurrentRequest.NonPrefetchableMmioSizeBelow4G[i][j].Size = CurrentUsed->NonPrefetchableMmioSizeBelow4G[i][j].Size + UserRequest->NonPrefetchableMmioSizeBelow4G[i][j].Size;
      CurrentRequest.PrefetchableMmioSizeAbove4G[i][j].Size = CurrentUsed->PrefetchableMmioSizeAbove4G[i][j].Size + UserRequest->PrefetchableMmioSizeAbove4G[i][j].Size;
      CurrentRequest.PrefetchableMmioSizeBelow4G[i][j].Size = CurrentUsed->PrefetchableMmioSizeBelow4G[i][j].Size + UserRequest->PrefetchableMmioSizeBelow4G[i][j].Size;

      CurrentRequest.NonPrefetchableMmioSizeAbove4G[i][j].Alignment = UserRequest->NonPrefetchableMmioSizeAbove4G[i][j].Alignment;
      CurrentRequest.NonPrefetchableMmioSizeBelow4G[i][j].Alignment = UserRequest->NonPrefetchableMmioSizeBelow4G[i][j].Alignment;
      CurrentRequest.PrefetchableMmioSizeAbove4G[i][j].Alignment = UserRequest->PrefetchableMmioSizeAbove4G[i][j].Alignment;
      CurrentRequest.PrefetchableMmioSizeBelow4G[i][j].Alignment = UserRequest->PrefetchableMmioSizeBelow4G[i][j].Alignment;

      CurrentRequest.IO[i][j].Size = CurrentUsed->IO[i][j].Size + UserRequest->IO[i][j].Size;
    }
  }

  // Die0's 2nd MMIO
  CurrentRequest.Die0SecondNonPrefetchableMmioSizeBelow4G.Alignment = UserRequest->Die0SecondNonPrefetchableMmioSizeBelow4G.Alignment;
  CurrentRequest.Die0SecondNonPrefetchableMmioSizeBelow4G.Size = CurrentUsed->Die0SecondNonPrefetchableMmioSizeBelow4G.Size + UserRequest->Die0SecondNonPrefetchableMmioSizeBelow4G.Size;
  CurrentRequest.Die0SecondPrefetchableMmioSizeBelow4G.Alignment = UserRequest->Die0SecondPrefetchableMmioSizeBelow4G.Alignment;
  CurrentRequest.Die0SecondPrefetchableMmioSizeBelow4G.Size = CurrentUsed->Die0SecondPrefetchableMmioSizeBelow4G.Size + UserRequest->Die0SecondPrefetchableMmioSizeBelow4G.Size;

  // Save user's request to NV
  VariableSize = sizeof (FABRIC_RESOURCE_FOR_EACH_DIE);

  Status = gRT->SetVariable (
                  L"ResourceSizeForEachDie",
                  &gAmdResourceSizeForEachDieGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  VariableSize,
                  (VOID *) &CurrentRequest
                  );
  IDS_HDT_CONSOLE (CPU_TRACE, "  Set DF Resource size per DIE to NV variable, Status: %x\n", Status);

  return Status;
}

/*---------------------------------------------------------------------------------------*/
/**
 * FabricClearResourceSizeForEachDie
 *
 * Clear user's request about MMIO/IO size for each Die to NV variable
 *
 *
 * @retval       EFI_SUCCESS        Clear NV variable
 */
EFI_STATUS
FabricClearResourceSizeForEachDie (
  )
{
  UINT8  Ignore;

  gRT->SetVariable (
                  L"ResourceSizeForEachDie",
                  &gAmdResourceSizeForEachDieGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  0,
                  (VOID *) &Ignore
                  );
  IDS_HDT_CONSOLE (CPU_TRACE, "  Clear DF Resource size per DIE to NV variable\n");

  return EFI_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/
/**
 * FabricGetResourceDistribution
 *
 * Get the information about which Die's MMIO should be put above Pcie Cfg
 *
 * @param[in, out]    ResourceDistribution       If it's above Pcie Cfg
 *
 */
EFI_STATUS
FabricGetResourceDistribution (
  IN       BOOLEAN   *ResourceDistribution
  )
{
  UINTN                             VariableSize;
  EFI_STATUS                        Status;

  VariableSize = sizeof (BOOLEAN) * MAX_SOCKETS_SUPPORTED * MAX_DIES_PER_SOCKET;
  Status = gRT->GetVariable (
                  L"ResourceDistribution",
                  &gAmdResourceDistributionGuid,
                  NULL,
                  &VariableSize,
                  (VOID *) ResourceDistribution
                  );

  return Status;
}

/*---------------------------------------------------------------------------------------*/
/**
 * FabricSetResourceDistribution
 *
 * Set the information about which Die's MMIO should be put above Pcie Cfg
 *
 * @param[in, out]    ResourceDistribution       If it's above Pcie Cfg
 *
 */
EFI_STATUS
FabricSetResourceDistribution (
  IN       BOOLEAN   *ResourceDistribution
  )
{
  UINTN                             VariableSize;
  EFI_STATUS                        Status;

  VariableSize = sizeof (BOOLEAN) * MAX_SOCKETS_SUPPORTED * MAX_DIES_PER_SOCKET;
  Status = gRT->SetVariable (
                  L"ResourceDistribution",
                  &gAmdResourceDistributionGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  VariableSize,
                  (VOID *) ResourceDistribution
                  );

  return Status;
}




