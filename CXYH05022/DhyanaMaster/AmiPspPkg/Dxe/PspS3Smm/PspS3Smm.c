//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//**********************************************************************
//
/** @file PspS3Smm.c
    Save PSP S3 data to Psp S3 Nvram

**/

#include "PspS3Smm.h"
#include "Guid/AmiPsp.h"
#include "Token.h"
#include <Library/AmdPspBaseLibV2.h>

EFI_SMM_SYSTEM_TABLE2           *Smst2 = NULL;
FLASH_PROTOCOL 					*Flash = NULL;


/**
    This function returns base address of PSP S3 NVRAM

  @param Address  The base address of PSP S3 Nvram.

  @retval EFI_SUCCESS Command is handled successfully.

**/
EFI_STATUS
GetPspS3NvBase(
  IN OUT EFI_PHYSICAL_ADDRESS   *Address
)
{
    UINT32          PspDirBase;
    UINT32          BiosDirBase;
    PSP_DIRECTORY   *PspDir;
    UINTN           i;

    if (GetDirBase (&PspDirBase, &BiosDirBase) != TRUE) return  EFI_NOT_FOUND;
    
    PspDir = (PSP_DIRECTORY *)PspDirBase;
    for (i = 0; i < PspDir->Header.TotalEntries; i++) {
        if (PspDir->PspEntry[i].Type == PSP_S3_NV_DATA) {
            *Address = PspDir->PspEntry[i].Location;
            return  EFI_SUCCESS;
        }
    }
    
    return  EFI_NOT_FOUND;
}

/**
    This function saves the Psp S3 data to Psp S3 Nvram.

  @param DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param Context         Points to an optional handler context which was specified when the
                         handler was registered.
  @param CommBuffer      A pointer to a collection of data in memory that will
                         be conveyed from a non-SMM environment into an SMM environment.
  @param CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS Command is handled successfully.

**/
EFI_STATUS
SavePspS3DataToNvram (
    IN  EFI_HANDLE      DispatchHandle,
    IN  CONST   VOID    *DispatchContext OPTIONAL,
    IN  OUT VOID        *CommBuffer      OPTIONAL,
    IN  OUT UINTN       *CommBufferSize  OPTIONAL
)
{
    EFI_STATUS      		Status = EFI_SUCCESS;
    EFI_PHYSICAL_ADDRESS    PspS3NvBase = 0;
    UINTN                   EraseSize = 0;
    UINTN                   S3DataSize = 0;
    VOID                    *S3Data = NULL;

    Status = GetPspS3NvBase(&PspS3NvBase);
    DEBUG((DEBUG_INFO, "\n PspS3NvBase = %lx\n", PspS3NvBase));
    if (Status != EFI_SUCCESS)
    {
        DEBUG((DEBUG_INFO, "WARNING: Unable to get PSP S3 nvram base\n"));
        return EFI_UNSUPPORTED;
    }

    S3Data = CommBuffer;
    S3DataSize = *CommBufferSize;
    DEBUG((DEBUG_INFO, "S3 Data size(include Hmac) = %lx\n", S3DataSize));
    // The Erase size muse be 4k alignment, otherwise an error status will return
    EraseSize = ((S3DataSize / 0x1000) * 0x1000) + 0x1000;
    Status = Flash->Erase((VOID*)PspS3NvBase, EraseSize);
    DEBUG((DEBUG_INFO, "\n Flash Erase status = %lx\n", Status));
 
    Status = Flash->Write((VOID*)PspS3NvBase, S3DataSize, S3Data);
    DEBUG((DEBUG_INFO, "\n Flash Write status = %lx\n", Status));

    return Status;
}

/**
    Called from InitSmmHandler

    @param

    @retval VOID

**/
EFI_STATUS
InSmmFunction (
    IN  EFI_HANDLE       ImageHandle,
    IN  EFI_SYSTEM_TABLE *SystemTable
 )
{
    EFI_STATUS  Status;
    EFI_HANDLE  Handle;

    Status = Smst2->SmiHandlerRegister(
                        (VOID *)SavePspS3DataToNvram,
                        &gAmiPspS3SmmCommunicationGuid,
                        &Handle
                        );
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return Status;
    }

    return EFI_SUCCESS;
}


/**
    Initializes Psp S3 SMM Drivers.

    @param

    @retval EFI_STATUS

    @note Here is the control flow of this function:
**/

EFI_STATUS
PspS3SmmInit (
    IN  EFI_HANDLE       ImageHandle,
    IN  EFI_SYSTEM_TABLE *SystemTable
 )
{
    EFI_STATUS              Status;
    EFI_SMM_BASE2_PROTOCOL  *SmmBase2;

    InitAmiSmmLib(ImageHandle, SystemTable);

    Status = pBS->LocateProtocol(&gEfiSmmBase2ProtocolGuid, NULL, (VOID**)&SmmBase2);
    if (EFI_ERROR(Status)) {
        return Status;
    }

    Status = SmmBase2->GetSmstLocation(SmmBase2, &Smst2);
    if (EFI_ERROR(Status)) {
        return EFI_UNSUPPORTED;
    }

    Status = Smst2->SmmLocateProtocol (&gFlashSmmProtocolGuid, NULL, &Flash);
    if (EFI_ERROR(Status)) return Status;

    return (InitSmmHandler (ImageHandle, SystemTable, InSmmFunction, NULL));
}

//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************
