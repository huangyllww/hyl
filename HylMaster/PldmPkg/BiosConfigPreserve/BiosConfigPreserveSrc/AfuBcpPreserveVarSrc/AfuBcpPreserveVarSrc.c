//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************

/** @file 
AfuVariablesPreserve.c

AfuVariablesPreserve works together with SmiFlash to preserve the variables


*/

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------

#include <Efi.h>
#include <Token.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include "PldmSceStruct.h"
#include "AfuCommunication.h"
#include <Guid/CapsuleVendor.h>
#include <Protocol/FlashProtocol.h>
#include <Library/SmmServicesTableLib.h>

FLASH_PROTOCOL                  *gmFlash = NULL;
BOOLEAN gNotFirstTime = FALSE;
VOID        *gFVPreLocation = NULL;

/**
    Check whether if FV has PLDM data. If found erase it

    @return VOID

**/
VOID IsPreFVClean()
{
    EFI_STATUS  Status;
	UINT8 *ReadData = NULL;
    SCE_PLDM_DATA_HEADER *ScePldmFVData = NULL;
    UINT8 *PldmSignature = "PLDM" ;
    UINT8 *TempAddr = NULL;
    UINTN FlashDataSize = 0;
    UINTN BcpFlashBlockSize = 0;
    UINTN Blocks = 0;
    VOID *FvData = NULL;
    UINTN BcpFvSize = 0;
    
    BcpFvSize = FVBCPVolumeSize();
    DEBUG((DEBUG_ERROR,"IsFVClean: BcpFvSize %x \n", BcpFvSize));
    
    BcpFlashBlockSize = FVFlashBlockSize();
    
    if(gSmst == NULL)
    {
        DEBUG((DEBUG_ERROR,"IsFVClean: gSmst is NULL \n"));
        return;
    }
    
    Status = gSmst->SmmAllocatePool (EfiRuntimeServicesData, BcpFvSize, (void**)&ReadData);
    if (EFI_ERROR(Status)) return ;
    Status = gSmst->SmmAllocatePool (EfiRuntimeServicesData, BcpFlashBlockSize, (void**)&FvData);
    if (EFI_ERROR(Status))
    {
        if(ReadData)
        {
            Status = gSmst->SmmFreePool(ReadData);
            ReadData = NULL;
        }
        return ;
    }
    
    DEBUG((DEBUG_ERROR,"AfuBcpPre:In IsFVClean: (WriteToFlash) entered\n"));
    Status = gmFlash->DeviceWriteEnable();
    if (EFI_ERROR(Status))
    {
        if(FvData)
        {
            Status = gSmst->SmmFreePool(FvData);
            FvData = NULL;
        }
        if(ReadData)
        {
            Status = gSmst->SmmFreePool(ReadData);
            ReadData = NULL;
        }
        return ;
    }

    DEBUG((DEBUG_ERROR,"In IsFVClean: BcpFlashBlockSize: %d\n", BcpFlashBlockSize));
    for(Blocks = 0; Blocks < BcpFvSize; )
    {
        DEBUG((DEBUG_ERROR,"In IsFVClean: ReadAddress: %x, DataAddress: %x, Blocks: %x\n", ((UINT8 *)gFVPreLocation + Blocks), (ReadData + Blocks), Blocks));
        Status = gmFlash->Read((VOID *)((UINT8 *)gFVPreLocation + Blocks), BcpFlashBlockSize, (VOID *)(FvData));
        MemCpy((VOID *)(ReadData + Blocks), FvData, BcpFlashBlockSize);
        Blocks += BcpFlashBlockSize;
    }
    
    TempAddr = ReadData + BCP_FFS_DATA_START_OFFSET;
    ScePldmFVData = (SCE_PLDM_DATA_HEADER *)TempAddr;
    DEBUG((DEBUG_ERROR,"AfuBcpPre:IsFVClean: ScePldmFVData->Signature %x \n", ScePldmFVData->Signature));

    if( ! MemCmp((((UINT8 *)TempAddr)+4), (UINT8 *)PldmSignature, 4) )
    {
        for(Blocks = 0; Blocks < BcpFvSize; )
        {
            DEBUG((DEBUG_ERROR,"In IsFVClean: EraseAddress: %x, Blocks: %x\n", ((UINT8 *)gFVPreLocation + Blocks), Blocks));
            Status = gmFlash->Erase((VOID *)((UINT8 *)gFVPreLocation + Blocks), BcpFlashBlockSize);
            DEBUG((DEBUG_ERROR,"IsFVClean: Erase Status %x \n", Status));
            Blocks += BcpFlashBlockSize;
        }
		
        FlashDataSize = FVFlashDataSize();
        DEBUG((DEBUG_ERROR,"IsFVClean: FlashDataSize %x \n", FlashDataSize));
		MemSet(ReadData + BCP_FFS_DATA_START_OFFSET, FlashDataSize, 0xFF);
		DEBUG((DEBUG_ERROR,"AfuBcpPre:In IsFVClean: (WriteToFlash) write begin\n"));
        for(Blocks = 0; Blocks < BcpFvSize; )
        {
            DEBUG((DEBUG_ERROR,"In IsFVClean: WriteAddress: %x, DataAddress: %x, Blocks: %x\n", ((UINT8 *)gFVPreLocation + Blocks), (ReadData + Blocks), Blocks));
            Status = gmFlash->Write((VOID *)((UINT8 *)gFVPreLocation + Blocks), BcpFlashBlockSize, (VOID *)(ReadData + Blocks));
            Blocks += BcpFlashBlockSize;
        }
		DEBUG((DEBUG_ERROR,"AfuBcpPre:In IsFVClean: (WriteToFlash write) %r\n", Status));
    }
    gmFlash->DeviceWriteDisable();

    if(FvData)
    {
        Status = gSmst->SmmFreePool(FvData);
        FvData = NULL;
    }
    if(ReadData)
    {
        Status = gSmst->SmmFreePool(ReadData);
        ReadData = NULL;
    }
    
    DEBUG((DEBUG_ERROR,"AfuBcpPre:In IsFVClean: (WriteToFlash) exit %r\n", Status));
    return ;
}

/**
    Write to the flash part starting at "Address" for a length
    of "Size".

    @param Address
    @param Data
    @param Size

    @return EFI_STATUS

**/
EFI_STATUS
PreWriteToFlash(
    IN VOID    *Address,
    IN VOID    *Data,
    IN UINTN   Size
)
{
    EFI_STATUS  Status = EFI_SUCCESS;
    UINTN FlashDataSize = 0;

    DEBUG((DEBUG_ERROR,"AfuBcpPre:In BiosConfigPreserveInSmm: (WriteToFlash) entered\n"));
    FlashDataSize = FVFlashDataSize();
    if(Size > FlashDataSize)
    {
        DEBUG((DEBUG_ERROR,"In BiosConfigPreserveInSmm: (WriteToFlash) Error PldmSize: %d, FlashDataSize: %d\n", Size, FlashDataSize));
        return EFI_INVALID_PARAMETER;
    }
    
    Status = gmFlash->DeviceWriteEnable();
    if (EFI_ERROR(Status)) return Status;

    DEBUG((DEBUG_ERROR,"AfuBcpPre:In BiosConfigPreserveInSmm: (WriteToFlash) write begin %r\n", Status));
    Status = gmFlash->Write(Address, Size, Data);
    DEBUG((DEBUG_ERROR,"AfuBcpPre:In BiosConfigPreserveInSmm: (WriteToFlash write) %r\n", Status));

    gmFlash->DeviceWriteDisable();

    DEBUG((DEBUG_ERROR,"AfuBcpPre:In BiosConfigPreserveInSmm: (WriteToFlash) exit %r\n", Status));
    return Status;
}
/**
    Function to set the NVRAM

*/
EFI_STATUS BcpAfuVarSetNvramName( CHAR16 *name, EFI_GUID *guid, UINT32 attributes, VOID *buffer, UINTN size )
{
	EFI_STATUS Status = EFI_SUCCESS;
	
	Status = pRS->SetVariable(
		name,
		guid,
		attributes,
		size,
		buffer
		);
	DEBUG((DEBUG_ERROR,"BiosConfigPreserve: status for setvariable is %r buffer is %x size is %x name is %S\n", Status, buffer, size, name));
	return Status;
}

/**
   	Function to restore the pldmfilename and pldmhash variables
  	
*/ 
VOID RestoreBcpAfuVariables (
	VOID
)
{
	EFI_GUID	PldmFilePathGuid = EFI_PLDM_FILE_PATH_GUID;
	EFI_GUID  	PldmHashGuid = EFI_PLDM_HASH_GUID;
	UINT32 		Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS;
	UINT8   	*PldmFileName = NULL;
	UINT8   	*PldmHash = NULL;
	UINT32   	*PldmCapsule = NULL;
	UINT32   	PldmFileNameSize = 0;
	UINT32   	PldmHashSize = 0;
	UINT32   	PldmCapsuleSize = 0;
	EFI_GUID 	AfuBcpSharingProtocolGuid = AFU_BCP_SHARING_PROTOCOL_GUID;
	EFI_STATUS 	Status = EFI_SUCCESS;
	CHAR16      *FwCapsuleVarName = NULL;
	AFU_BCP_SHARING_PROTOCOL *AfuBcpSharingProtocol = NULL;
	void        *RomDataLocation;
	BOOLEAN PldmPreserveStatus = FALSE;

	DEBUG((DEBUG_ERROR,"BiosConfigPreserve: In RestoreBcpAfuVariables\n"));
	
    if(gSmst == NULL)
    {
        DEBUG((DEBUG_ERROR,"BiosConfigPreserve: gSmst is NULL \n"));
        return;
    }
    
    Status = gSmst->SmmLocateProtocol (&AfuBcpSharingProtocolGuid, NULL, &AfuBcpSharingProtocol);
    DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Status for SmmLocateProtocol locate protocol %r\n", Status));
    
    if (!EFI_ERROR (Status))
    {
        Status = AfuBcpSharingProtocol->AfuBcpGetVariable (AfuBcpSharingProtocol, L"PldmCapsule", &PldmCapsule, &PldmCapsuleSize, &FwCapsuleVarName, NULL, NULL);
        DEBUG((DEBUG_ERROR,"BiosConfigPreserve: AfuBcpGetVariable PldmCapsule is %x\n", Status));
        if (!EFI_ERROR (Status))
        {
            Status = BcpAfuVarSetNvramName (FwCapsuleVarName, &gEfiCapsuleVendorGuid, Attributes, (void *)&PldmCapsule, PldmCapsuleSize);
            if (!EFI_ERROR (Status))
            {
                DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Set Capsule Success\n"));
                PldmPreserveStatus = TRUE;
            }
        }
        else
        {
            Status = AfuBcpSharingProtocol->AfuBcpGetVariable (AfuBcpSharingProtocol, L"PldmFileName", &PldmFileName, &PldmFileNameSize, NULL, &RomDataLocation, &gFVPreLocation);
            if (!EFI_ERROR (Status))
            {
                if( !FVDataAccessType() )
                {
                    Status = BcpAfuVarSetNvramName (L"PldmFileName", &PldmFilePathGuid, Attributes, PldmFileName, PldmFileNameSize);
                    DEBUG((DEBUG_ERROR,"RestoreBcpAfuVariables:BiosConfigPreserve: PldmFileName is %x PldmFileNameSize is %x\n", PldmFileName, PldmFileNameSize));
                    if (!EFI_ERROR (Status))
                    {
                        DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Set Pldmdata success\n"));
                        PldmPreserveStatus = TRUE;
                    }
                    if (NULL != PldmFileName)
                    {
                        Status = gSmst->SmmFreePool (PldmFileName);
                        DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Status for freeing Pldmfilename is %r\n", Status));
                    }
                }
                else
                {
                    if(gSmst)
                    {
                        Status = gSmst->SmmLocateProtocol (&gFlashSmmProtocolGuid, NULL, &gmFlash);
                        DEBUG((DEBUG_ERROR,"RestoreBcpAfuVariables:In WriteInFV: (SmmLocateProtocol success) %x, RomDataLocation:%x\n", gmFlash, RomDataLocation));
                        if( gmFlash != NULL && RomDataLocation != NULL )
                        {
                            UINTN BcpFlashBlockSize = 0;
                            UINTN FlashDataSize = 0;
    
                            IsPreFVClean();
    
                            FlashDataSize = FVFlashDataSize();
                            if(PldmFileNameSize < FlashDataSize)
                            {
                                BcpFlashBlockSize = FVFlashBlockSize();
    
                                if(PldmFileNameSize < BcpFlashBlockSize)
                                {
                                    Status = PreWriteToFlash(RomDataLocation,
                                                            (VOID*)PldmFileName,
                                                            PldmFileNameSize) ;
                            
                                    if (!EFI_ERROR (Status))
                                    {
                                        DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Set Pldmdata success\n"));
                                        PldmPreserveStatus = TRUE;
                                    }
                                    DEBUG((DEBUG_ERROR,"RestoreBcpAfuVariables:In WriteInFv: (WriteToFlash) %r\n", Status));
                                }
                                else
                                {
                                    UINTN SizeWritten = 0;
                                    UINTN InitialSize = 0;
                                    UINTN RemainingSize = 0;
                                    
                                    PldmPreserveStatus = TRUE;
    
                                    InitialSize = BcpFlashBlockSize - BCP_FFS_DATA_START_OFFSET;
                                    
                                    DEBUG((DEBUG_ERROR,"In BiosConfigPreserveInSmm: (WriteToFlash) InitialSize: %d\n", InitialSize));
                                    Status = PreWriteToFlash(RomDataLocation,
                                                    (VOID*)PldmFileName,
                                                    InitialSize);
                                    SizeWritten = InitialSize;
                                    RemainingSize = PldmFileNameSize - SizeWritten;
                                    DEBUG((DEBUG_ERROR,"In BiosConfigPreserveInSmm: (IniWriteToFlash) SizeWritten: %d, RemainingSize: %d\n", SizeWritten, RemainingSize));
                                    for(; RemainingSize > 0; )
                                    {
                                        if(RemainingSize < BcpFlashBlockSize)
                                        {
                                            DEBUG((DEBUG_ERROR,"In BiosConfigPreserveInSmm: (WriteToFlash) EndPart: %d\n", RemainingSize));
                                            Status = PreWriteToFlash((VOID*)((UINT8 *)RomDataLocation + SizeWritten),
                                                            (VOID*)(PldmFileName + SizeWritten),
                                                            RemainingSize) ;
                                            SizeWritten = SizeWritten + RemainingSize;
                                            DEBUG((DEBUG_ERROR,"In WriteInFv: (RemWriteToFlash) %r, SizeWritten: %d\n", Status, SizeWritten));
                                        }
                                        else
                                        {
                                            DEBUG((DEBUG_ERROR,"In BiosConfigPreserveInSmm: (WriteToFlash) BcpFlashBlockSize: %d\n", BcpFlashBlockSize));
                                            Status = PreWriteToFlash((VOID*)((UINT8 *)RomDataLocation + SizeWritten),
                                                            (VOID*)(PldmFileName + SizeWritten),
                                                            BcpFlashBlockSize);
                                            
                                            SizeWritten = SizeWritten + BcpFlashBlockSize;
                                            DEBUG((DEBUG_ERROR,"In WriteInFv: (BlockWriteToFlash) %r, SizeWritten: %d\n", Status, SizeWritten));
                                        }
                                        RemainingSize = PldmFileNameSize - SizeWritten;
                                        DEBUG((DEBUG_ERROR,"In BiosConfigPreserveInSmm: (WriteToFlash) RemainingSize: %d\n", RemainingSize));
                                    }
                                }
                            }
                            else
                            {
                                DEBUG((DEBUG_ERROR,"In BiosConfigPreserveInSmm: (Error) PldmFileNameSize: %d, FlashDataSize: %d\n", PldmFileNameSize, FlashDataSize));
                            }
                        }
                    }
                }
            }
        }
        if(PldmPreserveStatus == TRUE)
        {
            DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Set PldmHash\n"));
            Status = AfuBcpSharingProtocol->AfuBcpGetVariable (AfuBcpSharingProtocol, L"PldmHash", &PldmHash, &PldmHashSize,NULL, NULL, NULL);
            if (!EFI_ERROR (Status))
            {
                Status = BcpAfuVarSetNvramName (L"PldmHash", &PldmHashGuid, Attributes, PldmHash, PldmHashSize);
                DEBUG((DEBUG_ERROR,"BiosConfigPreserve: PldmHash is %x PldmHashSize is %x\n", PldmHash, PldmHashSize));
                if (NULL != PldmHash)
                {
                    Status = gSmst->SmmFreePool (PldmHash);
                    DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Status for freeing Pldmhash is %r\n", Status));
                }
            }
        }
    }
    DEBUG((DEBUG_ERROR,"BiosConfigPreserve: RestoreBcpAfuVariables Return\n"));
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
