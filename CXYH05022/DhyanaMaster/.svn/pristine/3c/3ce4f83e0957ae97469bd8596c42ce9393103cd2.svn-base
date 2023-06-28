/* $NoKeywords:$ */
/**
 * @file
 *
 * Generate SMBIOS type 20
 *
 * Contains code that generate SMBIOS type 20
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Universal
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
#include "AmdSmbios.h"
#include "AMD.h"
#include "Porting.h"
#include "Filecode.h"
#include "Library/IdsLib.h"
#include "Library/BaseLib.h"
#include <MemDmi.h>
#include "Library/UefiBootServicesTableLib.h"
#include "Library/MemoryAllocationLib.h"
#include "Protocol/Smbios.h"
#include "Protocol/AmdSmbiosServicesProtocol.h"
#include "PiDxe.h"

#define FILECODE UNIVERSAL_SMBIOS_AMDSMBIOSTYPE20_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
/**
 *---------------------------------------------------------------------------------------
 *
 *  Generate SMBIOS type 20
 *
 *  Parameters:
 *    @param[in]       Smbios                       Pointer to EfiSmbiosProtocol
 *    @param[in]       MemDmiInfo                   Pointer to Memory DMI information
 *    @param[in]       Socket                       Socket number
 *    @param[in]       Channel                      Channel number
 *    @param[in]       Dimm                         Dimm number
 *    @param[in]       MemoryDeviceHandle           Handle of the device where the address range is mapped to
 *    @param[in]       MemoryArrayHandle            Handle of the array where the address range is mapped to
 *    @param[in]       StdHeader                    Handle to config for library and services
 *
 *    @retval          EFI_SUCCESS                  The Type 20 entry is added successfully.
 *
 *---------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
AmdAddSmbiosType20 (
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
  IN       DMI_INFO             *MemDmiInfo,
  IN       UINT8                 Socket,
  IN       UINT8                 Channel,
  IN       UINT8                 Dimm,
  IN       EFI_SMBIOS_HANDLE     MemoryDeviceHandle,
  IN       EFI_SMBIOS_HANDLE     *MemoryArrayHandle,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  UINTN                             StructureSize;
  UINTN                             TotalSize;
  EFI_STATUS                        Status;
  EFI_STATUS                        CalledStatus;
  AMD_SMBIOS_TABLE_TYPE20           *SmbiosTableType20;
  UINT32                             MmioHoleSizeBelow4G;
  
  UINT64                            MemStartAddr;
  EFI_STATUS                        Status1 = EFI_SUCCESS;
  EFI_STATUS                        Status2 = EFI_SUCCESS;

  Status = EFI_SUCCESS;
  MmioHoleSizeBelow4G = MemDmiInfo->T19[0].EndingAddr - MemDmiInfo->T19[0].StartingAddr + 1;
  // Generate DMI type 20 --- Start

  if (MemDmiInfo->T20[Socket][Channel][Dimm].EndingAddr != 0) {
  // Calculate size of DMI type 20
    StructureSize = sizeof (AMD_SMBIOS_TABLE_TYPE20);
    TotalSize = StructureSize + 2; // Additional null (00h), End of strings

    // Allocate zero pool
    SmbiosTableType20 = NULL;
    SmbiosTableType20 = AllocateZeroPool (TotalSize);
    if (SmbiosTableType20 == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    
    if (MemDmiInfo->T20[Socket][Channel][Dimm].StartingAddr != 0xFFFFFFFF){
        MemStartAddr = MemDmiInfo->T20[Socket][Channel][Dimm].StartingAddr;
    } else {
        MemStartAddr = MemDmiInfo->T20[Socket][Channel][Dimm].ExtStartingAddr;
    }

    //assume that size of one dimm >= 4GB and total memory is less than (4TB - MmioHoleSizeBelow4G)
    if (MemStartAddr >= 0x400000) {
        //if dimm base address is above 4G, adjust starting and ending address by adding mmio size
        SmbiosTableType20->Hdr.Handle                        = AMD_SMBIOS_HANDLE_PI_RESERVED;
        SmbiosTableType20->Hdr.Type                          = AMD_EFI_SMBIOS_TYPE_MEMORY_DEVICE_MAPPED_ADDRESS;
        SmbiosTableType20->Hdr.Length                        = (UINT8) StructureSize;
        SmbiosTableType20->MemoryDeviceHandle                = MemoryDeviceHandle;
        SmbiosTableType20->MemoryArrayMappedAddressHandle    = MemoryArrayHandle[1]; //memroy map address block above 4G
        SmbiosTableType20->PartitionRowPosition              = MemDmiInfo->T20[Socket][Channel][Dimm].PartitionRowPosition;
        SmbiosTableType20->InterleavePosition                = MemDmiInfo->T20[Socket][Channel][Dimm].InterleavePosition;
        SmbiosTableType20->InterleavedDataDepth              = MemDmiInfo->T20[Socket][Channel][Dimm].InterleavedDataDepth;
        
        if (MemDmiInfo->T20[Socket][Channel][Dimm].StartingAddr != 0xFFFFFFFF) {
            SmbiosTableType20->StartingAddress                   = MemDmiInfo->T20[Socket][Channel][Dimm].StartingAddr + MmioHoleSizeBelow4G;
            SmbiosTableType20->EndingAddress                     = MemDmiInfo->T20[Socket][Channel][Dimm].EndingAddr + MmioHoleSizeBelow4G;
            SmbiosTableType20->ExtendedStartingAddress           = MemDmiInfo->T20[Socket][Channel][Dimm].ExtStartingAddr;
            SmbiosTableType20->ExtendedEndingAddress             = MemDmiInfo->T20[Socket][Channel][Dimm].ExtEndingAddr;
        } else {
            SmbiosTableType20->StartingAddress                   = MemDmiInfo->T20[Socket][Channel][Dimm].StartingAddr;
            SmbiosTableType20->EndingAddress                     = MemDmiInfo->T20[Socket][Channel][Dimm].EndingAddr;
            SmbiosTableType20->ExtendedStartingAddress           = MemDmiInfo->T20[Socket][Channel][Dimm].ExtStartingAddr + MmioHoleSizeBelow4G;
            SmbiosTableType20->ExtendedEndingAddress             = MemDmiInfo->T20[Socket][Channel][Dimm].ExtEndingAddr + MmioHoleSizeBelow4G;
        }
        
        // Add DMI type 20
        CalledStatus = Smbios->Add (Smbios, NULL, &SmbiosTableType20->Hdr.Handle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosTableType20);
        Status = (CalledStatus > Status) ? CalledStatus : Status;
        // Free pool
        FreePool (SmbiosTableType20);
        return Status;
    }
    else {
        //If memory address covers mmio hole below 4g, we split it into two ranges:
        //MemDmiInfo->T20[Socket][Channel][Dimm].StartingAddr ~ MemDmiInfo->T19[0].EndingAddr and
        //4G ~ SmbiosTableType20->EndingAddress + MmioHoleSizeBelow4G
        SmbiosTableType20->Hdr.Handle                        = AMD_SMBIOS_HANDLE_PI_RESERVED;
        SmbiosTableType20->Hdr.Type                          = AMD_EFI_SMBIOS_TYPE_MEMORY_DEVICE_MAPPED_ADDRESS;
        SmbiosTableType20->Hdr.Length                        = (UINT8) StructureSize;
        SmbiosTableType20->MemoryDeviceHandle                = MemoryDeviceHandle;
        SmbiosTableType20->MemoryArrayMappedAddressHandle    = MemoryArrayHandle[0];
        SmbiosTableType20->PartitionRowPosition              = MemDmiInfo->T20[Socket][Channel][Dimm].PartitionRowPosition;
        SmbiosTableType20->InterleavePosition                = MemDmiInfo->T20[Socket][Channel][Dimm].InterleavePosition;
        SmbiosTableType20->InterleavedDataDepth              = MemDmiInfo->T20[Socket][Channel][Dimm].InterleavedDataDepth;
        SmbiosTableType20->StartingAddress                   = MemDmiInfo->T20[Socket][Channel][Dimm].StartingAddr;
        SmbiosTableType20->EndingAddress                     = MemDmiInfo->T19[0].EndingAddr;
        SmbiosTableType20->ExtendedStartingAddress           = MemDmiInfo->T20[Socket][Channel][Dimm].ExtStartingAddr;
        SmbiosTableType20->ExtendedEndingAddress             = MemDmiInfo->T20[Socket][Channel][Dimm].ExtEndingAddr;
    
        // Add DMI type 20
        CalledStatus = Smbios->Add (Smbios, NULL, &SmbiosTableType20->Hdr.Handle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosTableType20);
        Status1 = (CalledStatus > EFI_SUCCESS) ? CalledStatus : EFI_SUCCESS;
        
        SmbiosTableType20->Hdr.Handle                        = AMD_SMBIOS_HANDLE_PI_RESERVED;
        SmbiosTableType20->Hdr.Type                          = AMD_EFI_SMBIOS_TYPE_MEMORY_DEVICE_MAPPED_ADDRESS;
        SmbiosTableType20->Hdr.Length                        = (UINT8) StructureSize;
        SmbiosTableType20->MemoryDeviceHandle                = MemoryDeviceHandle;
        SmbiosTableType20->MemoryArrayMappedAddressHandle    = MemoryArrayHandle[1];
        SmbiosTableType20->PartitionRowPosition              = MemDmiInfo->T20[Socket][Channel][Dimm].PartitionRowPosition;
        SmbiosTableType20->InterleavePosition                = MemDmiInfo->T20[Socket][Channel][Dimm].InterleavePosition;
        SmbiosTableType20->InterleavedDataDepth              = MemDmiInfo->T20[Socket][Channel][Dimm].InterleavedDataDepth;
        SmbiosTableType20->StartingAddress                   = MemDmiInfo->T19[1].StartingAddr;
        SmbiosTableType20->EndingAddress                     = MemDmiInfo->T20[Socket][Channel][Dimm].EndingAddr + MmioHoleSizeBelow4G;
        SmbiosTableType20->ExtendedStartingAddress           = MemDmiInfo->T20[Socket][Channel][Dimm].ExtStartingAddr;
        SmbiosTableType20->ExtendedEndingAddress             = MemDmiInfo->T20[Socket][Channel][Dimm].ExtEndingAddr;
    
        // Add DMI type 20
        CalledStatus = Smbios->Add (Smbios, NULL, &SmbiosTableType20->Hdr.Handle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosTableType20);
        Status2 = (CalledStatus > EFI_SUCCESS) ? CalledStatus : EFI_SUCCESS;
        
        // Free pool
        FreePool (SmbiosTableType20);

        return (Status1 != EFI_SUCCESS) ? Status1 : ((Status2 != EFI_SUCCESS) ? Status2 : EFI_SUCCESS);
    } 
  } else {
    return EFI_SUCCESS;
  }
}



/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
