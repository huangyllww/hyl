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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "Uefi.h"
#include "PiPei.h"
#include "Filecode.h"
#include "AGESA.h"
#include "Library/AmdBaseLib.h"
#include <Library/PeiServicesTablePointerLib.h>
#include <Protocol/FabricResourceManagerServicesProtocol.h>
#include <Ppi/ReadOnlyVariable2.h>

#define FILECODE LIBRARY_PEIFABRICRESOURCESIZEFOREACHDIELIB_PEIFABRICRESOURCESIZEFOREACHDIELIB_FILECODE

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
  EFI_PEI_SERVICES                **PeiServices;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *ReadVariable2;

  PeiServices = (EFI_PEI_SERVICES**) GetPeiServicesTablePointer();
  Status = (**PeiServices).LocatePpi (PeiServices, &gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, &ReadVariable2);
  ASSERT (Status == EFI_SUCCESS);

  VariableSize = sizeof (FABRIC_RESOURCE_FOR_EACH_DIE);
  Status = ReadVariable2->GetVariable (
                      ReadVariable2,
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
 * @retval       EFI_UNSUPPORTED        Don't support this function during PEI
 */
EFI_STATUS
FabricSetResourceSizeForEachDie (
  IN       FABRIC_RESOURCE_FOR_EACH_DIE *CurrentUsed,
  IN       FABRIC_RESOURCE_FOR_EACH_DIE *UserRequest
  )
{
  return EFI_UNSUPPORTED;
}

/*---------------------------------------------------------------------------------------*/
/**
 * FabricClearResourceSizeForEachDie
 *
 * Clear user's request about MMIO/IO size for each Die to NV variable
 *
 *
 * @retval       EFI_UNSUPPORTED        Don't support this function during PEI
 */
EFI_STATUS
FabricClearResourceSizeForEachDie (
  )
{
  return EFI_UNSUPPORTED;
}

/*---------------------------------------------------------------------------------------*/
/**
 * FabricGetResourceDistribution
 *
 * Get the information about which Die's MMIO should be put above Pcie Cfg
 *
 * @param[in, out]    ResourceDistribution       If it's above Pcie Cfg
 *
 * @retval            EFI_UNSUPPORTED            Don't support this function during PEI
 */
EFI_STATUS
FabricGetResourceDistribution (
  IN       BOOLEAN   *ResourceDistribution
  )
{
  UINTN                             VariableSize;
  EFI_STATUS                        Status;
  EFI_PEI_SERVICES                **PeiServices;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *ReadVariable2;

  PeiServices = (EFI_PEI_SERVICES**) GetPeiServicesTablePointer();
  Status = (**PeiServices).LocatePpi (PeiServices, &gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, &ReadVariable2);
  ASSERT (Status == EFI_SUCCESS); 

  VariableSize = sizeof (BOOLEAN) * MAX_SOCKETS_SUPPORTED * MAX_DIES_PER_SOCKET;
  Status = ReadVariable2->GetVariable (
                  ReadVariable2,
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
 * @retval            EFI_UNSUPPORTED            Don't support this function during PEI
 */
EFI_STATUS
FabricSetResourceDistribution (
  IN       BOOLEAN   *ResourceDistribution
  )
{
  return EFI_UNSUPPORTED;
}




