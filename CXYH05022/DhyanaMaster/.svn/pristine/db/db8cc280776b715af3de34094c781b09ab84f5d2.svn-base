//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
/** @file PeiRamBootDxe.c
    PEI RAM BOOT DXE driver.
**/
//----------------------------------------------------------------------------
// Includes
// Statements that include other files
#include <AmiDxeLib.h>
#include <PeiRamBoot.h>
#include <Protocol/FirmwareVolumeBlock.h>
#include <CoreFwVolBlock.h>
#include <Token.h>

//----------------------------------------------------------------------------
// Function Externs

//----------------------------------------------------------------------------
// Local prototypes
#ifndef _CR
#define _CR(Record, TYPE, Field)  ((TYPE *) ((CHAR8 *) (Record) - (CHAR8 *) &(((TYPE *) 0)->Field)))
#endif
#ifndef CR
#define CR(Record, TYPE, Field, Signature)  _CR (Record, TYPE, Field)
#endif
#ifndef EFI_FVB_MEMORY_MAPPED
#define EFI_FVB_MEMORY_MAPPED EFI_FVB2_MEMORY_MAPPED
#endif

static EFI_GUID gHobRomImageGuid = ROM_IMAGE_MEMORY_HOB_GUID;
static EFI_GUID gRomImageAddressGuid = ROM_IMAGE_ADDRESS_GUID;

//----------------------------------------------------------------------------
// Local Variables
//----------------------------------------------------------------------------
// Function Definitions
/**
 * DXE Entry Point for PeiRamBoot Driver.
 *
 * @param ImageHandle 
 * @param SystemTable 
 * @retval EFI_SUCCESS Success
**/
EFI_STATUS
PeiRamBootDxeInit (
    IN EFI_HANDLE             ImageHandle,
    IN EFI_SYSTEM_TABLE       *SystemTable
)
{
    EFI_GUID                GuidHob = HOB_LIST_GUID;
    EFI_STATUS              Status;
    UINTN                   NumHandles;
    UINT8                   i, j;
    VOID                    *p =  NULL;
    HOB_ROM_IMAGE           *HobRomImage;
    EFI_HANDLE              *HandleBuffer;
    EFI_FW_VOL_BLOCK_DEVICE *FvbDevice;
    EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL  *FvBlock;

    InitAmiLib(ImageHandle, SystemTable);

    // Get RamBoot Informatoin from HOB.    
    HobRomImage = (HOB_ROM_IMAGE*)GetEfiConfigurationTable (pST, &GuidHob);
    Status = FindNextHobByGuid (&gHobRomImageGuid, (VOID**)&HobRomImage);
    if (EFI_ERROR(Status)) return EFI_UNSUPPORTED;

    // Redirect the FV address to RAM and Free memory.
    for (p = GetEfiConfigurationTable (pST, &GuidHob);
            !(FindNextHobByType(EFI_HOB_TYPE_FV,&p)); ) {
        EFI_HOB_FIRMWARE_VOLUME *FvHob = (EFI_HOB_FIRMWARE_VOLUME*)p;
        for (i = 0; i < HobRomImage->NumOfFv; i++) {
            if ((FvHob->BaseAddress == HobRomImage->FvInfo[i].MemAddress) && \
                    (FvHob->Length == HobRomImage->FvInfo[i].FvLength)) {
                FvHob->BaseAddress = HobRomImage->FvInfo[i].FvAddress;
                // Do not free memory, it causes an ASSERT error if using Efi
                // Firmware Volume 2 Protocol.
                // pBS->FreePages (HobRomImage->FvInfo[i].MemAddress, \
                //                        HobRomImage->FvInfo[i].NumOfPages);
                break;
            }
        }
    }

    // Restore the Base Address of Firmware Volume Block Protocol from RAM
    // to ROM.
    Status = pBS->LocateHandleBuffer ( ByProtocol, \
                                       &gEfiFirmwareVolumeBlockProtocolGuid, \
                                       NULL, \
                                       &NumHandles, \
                                       &HandleBuffer    );
    if (EFI_ERROR(Status)) return Status;

    for (i = 0; i < NumHandles; ++i) {
        Status = pBS->HandleProtocol ( HandleBuffer[i], \
                                       &gEfiFirmwareVolumeBlockProtocolGuid, \
                                       &FvBlock );
        if (EFI_ERROR(Status)) continue;
        FvbDevice = FVB_DEVICE_FROM_THIS (FvBlock);
        if (!(FvbDevice->FvbAttributes & EFI_FVB_MEMORY_MAPPED)) continue;
        for (j = 0; j < HobRomImage->NumOfFv; j++) {
            if ((FvbDevice->BaseAddress == HobRomImage->FvInfo[j].MemAddress) && \
                    (FvbDevice->NumBlocks == \
                     (HobRomImage->FvInfo[j].FvLength / FLASH_BLOCK_SIZE))) {
                FvbDevice->BaseAddress = HobRomImage->FvInfo[j].FvAddress;
            }
        }
    }

    return EFI_SUCCESS;
}

/**
 * DXE Entry Point for PeiRamBoot Driver.
 *
 * @param ImageHandle 
 * @param SystemTable 
 * @retval EFI_SUCCESS Success
**/
EFI_STATUS
PeiRamBootDxeEntry (
    IN EFI_HANDLE             ImageHandle,
    IN EFI_SYSTEM_TABLE       *SystemTable
)
{
    InitAmiLib(ImageHandle,SystemTable);
    return InitSmmHandlerEx(ImageHandle, SystemTable, NULL, PeiRamBootDxeInit);
}
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

