//************************************************************************
//************************************************************************
//**                                                                  	**
//**        (C)Copyright 2015, American Megatrends, Inc.            	**
//**                                                                  	**
//**                       All Rights Reserved.                       	**
//**                                                                  	**
//**  5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093  			**
//**                                                                  	**
//**                       Phone: (770)-246-8600                      	**
//**                                                                  	**
//************************************************************************
//************************************************************************

/** @file 
EsrtConfigPreserve.c

Responsible for preserving setup configuration in ESRT flash update

*/

//************************************************************************
//					Include Section
//************************************************************************

//#include <stdlib.h>
#include "AmiDxeLib.h"
#include "PldmSceStruct.h"
#include "Token.h"
#include "Capsule.h"
#include <Library/BaseMemoryLib.h>
#include "PldmApi.h"
#include <Library/MemoryAllocationLib.h>
#include "Protocol/AmiDigitalSignature.h"
#include <Protocol/SimpleFileSystem.h>
#include "AfuCommunication.h"
#include <Protocol/FlashProtocol.h>

FLASH_PROTOCOL *gConfigmFlash = NULL;

extern EFI_GUID gEfiCapsuleVendorGuid;
//static 
extern EFI_STATUS Preserve_flash_Update( IN void *,CHAR16 *, IN UINT32, EFI_HANDLE FileSystemHandle ) ;
extern EFI_STATUS UpdateCapsuleToPush(IN void *, IN UINT32 ) ;
extern void PreservePldmVariables (void) ;
extern CHAR16   *gPldmFileName ;
extern UINT8    *gPldmHash;
extern UINTN    gHashSize;
extern void        *gRomDataLocation;
void        *gFvLocation;
extern UINTN

EFIAPI

Print (

IN CONST CHAR16 *Format,

...

);

extern void free(void *Ptr);

/**
    Check whether if FV has PLDM data. If found erase it

    @return VOID

**/
VOID IsFVClean()
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
    
    BcpFvSize = FVBCPVolumeSize();	// To get total BCP volume
    DEBUG((DEBUG_ERROR,"ConfigPreserveInterface:IsFVClean: BcpFvSize %x \n", BcpFvSize));
    
    BcpFlashBlockSize = FVFlashBlockSize();	// BCP flash block size
    
    ReadData = (UINT8 *)AllocatePool(BcpFvSize) ;

    FvData = (void *)AllocatePool(BcpFlashBlockSize) ;
    
    if( (ReadData != NULL) && (FvData != NULL) )
    {
       DEBUG((DEBUG_ERROR,"ConfigPreserveInterface:In IsFVClean: (WriteToFlash) entered\n"));
        Status = gConfigmFlash->DeviceWriteEnable();
        if (EFI_ERROR(Status))
        {
            if(FvData)
            {
                FreePool(FvData) ;
                FvData = NULL;
            }
            if(ReadData)
            {
                FreePool(ReadData) ;
                ReadData = NULL;
            }
            return ;
        }
        
       DEBUG((DEBUG_ERROR,"In ConfigPreserveInterface:IsFVClean: BcpFlashBlockSize: %d\n", BcpFlashBlockSize));
        for(Blocks = 0; Blocks < BcpFvSize; )	// Read FV from BCP offset
        {
           DEBUG((DEBUG_ERROR,"In ConfigPreserveInterface: ReadAddress: %x, DataAddress: %x, Blocks: %x\n", ((UINT8 *)gFvLocation + Blocks), (ReadData + Blocks), Blocks));
            Status = gConfigmFlash->Read((VOID *)((UINT8 *)gFvLocation + Blocks), BcpFlashBlockSize, (VOID *)(FvData));
            MemCpy((VOID *)(ReadData + Blocks), FvData, BcpFlashBlockSize);
            Blocks += BcpFlashBlockSize;
        }
        
        TempAddr = ReadData + BCP_FFS_DATA_START_OFFSET;
        ScePldmFVData = (SCE_PLDM_DATA_HEADER *)TempAddr;
        DEBUG((DEBUG_ERROR,"ConfigPreserveInterface:IsFVClean: ScePldmFVData->Signature %x \n", ScePldmFVData->Signature));
    
        if( ! MemCmp((((UINT8 *)TempAddr)+4), (UINT8 *)PldmSignature, 4) )	// looking for PLDM signature match
        {
            for(Blocks = 0; Blocks < BcpFvSize; )	// Erasing FV
            {
               DEBUG((DEBUG_ERROR,"In ConfigPreserveInterface: EraseAddress: %x, Blocks: %x\n", ((UINT8 *)gFvLocation + Blocks), Blocks));
                Status = gConfigmFlash->Erase((VOID *)((UINT8 *)gFvLocation + Blocks), BcpFlashBlockSize);
                DEBUG((DEBUG_ERROR,"ConfigPreserveInterface: Erase Status %x \n", Status));
                Blocks += BcpFlashBlockSize;
            }
            
            FlashDataSize = FVFlashDataSize();
            DEBUG((DEBUG_ERROR,"ConfigPreserveInterface:: FlashDataSize %x \n", FlashDataSize));
            MemSet(ReadData + BCP_FFS_DATA_START_OFFSET, FlashDataSize, 0xFF);
           DEBUG((DEBUG_ERROR,"AfuBcpPre:In IsFVClean: (WriteToFlash) write begin\n"));
            for(Blocks = 0; Blocks < BcpFvSize; )	// Writing FV
            {
               DEBUG((DEBUG_ERROR,"In ConfigPreserveInterface:: WriteAddress: %x, DataAddress: %x, Blocks: %x\n", ((UINT8 *)gFvLocation + Blocks), (ReadData + Blocks), Blocks));
                Status = gConfigmFlash->Write((VOID *)((UINT8 *)gFvLocation + Blocks), BcpFlashBlockSize, (VOID *)(ReadData + Blocks));
                Blocks += BcpFlashBlockSize;
            }
           DEBUG((DEBUG_ERROR,"ConfigPreserveInterface::In IsFVClean: (WriteToFlash write) %r\n", Status));
        }
        gConfigmFlash->DeviceWriteDisable();

        if(FvData)
        {
            FreePool(FvData) ;
            FvData = NULL;
        }
        if(ReadData)
        {
            FreePool(ReadData) ;
            ReadData = NULL;
        }
    }
    
   DEBUG((DEBUG_ERROR,"ConfigPreserveInterface::In IsFVClean: (WriteToFlash) exit %r\n", Status));
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
WriteToFlash(
    IN VOID    *Address,
    IN VOID    *Data,
    IN UINTN   Size
)
{
    EFI_STATUS  Status;
    UINTN FlashDataSize = 0;

    FlashDataSize = FVFlashDataSize();
    if(Size > FlashDataSize)
    {
       DEBUG((DEBUG_ERROR,"In ConfigPreserveInterface:: (WriteToFlash) Error PldmSize: %d, FlashDataSize: %d\n", Size, FlashDataSize));
        return EFI_INVALID_PARAMETER;
    }
    
   DEBUG((DEBUG_ERROR,"ConfigPreserveInterface::In BiosConfigPreserveInSmm: (WriteToFlash) entered\n"));
    Status = gConfigmFlash->DeviceWriteEnable();
    if (EFI_ERROR(Status)) return Status;

   DEBUG((DEBUG_ERROR,"ConfigPreserveInterface::In BiosConfigPreserveInSmm: (WriteToFlash) write begin %r\n", Status));
    Status = gConfigmFlash->Write(Address, Size, Data);	// Writing FV with expected DATA
   DEBUG((DEBUG_ERROR,"ConfigPreserveInterface::In BiosConfigPreserveInSmm: (WriteToFlash write) %r\n", Status));

    gConfigmFlash->DeviceWriteDisable();

   DEBUG((DEBUG_ERROR,"ConfigPreserveInterface::In BiosConfigPreserveInSmm: (WriteToFlash) exit %r\n", Status));
    return Status;
}

/**
    Function to convert char16 to char8

    @param CHAR8 *
    
    @retval CHAR16 *
*/ 
CHAR8 *PldmStrDup16to8 (CHAR16 *string)
{
    CHAR8      *text;
    UINTN       i = 0;
    EFI_STATUS  Status = EFI_SUCCESS;
    
    Status = gBS->AllocatePool(EfiBootServicesData, ((1 + StrLen (string)) * sizeof(CHAR8)),(void **)&text);
    
    if (EFI_ERROR (Status))
    {
        return NULL;
    }
    if ( text != NULL )
    {
        i=0;
        while(text[i] = (CHAR8)string[i])
            i++;
    }
    return text;
}

 /**
     Function to create PLDm table and store it to a buffer

     @param void **	
     @param UINT32 *
     
     @retval EFI_STATUS
 */ 
EFI_STATUS CreatePldmForConfigPreserving( OUT void **Buffer, IN OUT UINT32 *BufferSize )
{
	EFI_STATUS Status = EFI_SUCCESS ;
	UINT32 		Version = 0x1;
	UINT32 		Signature = 'MDLP';
	UINT8 		*HIIDBData = {0};
	UINT32 		CRC32 = 0;
	UINTN 		DBSize = 0;
	BOOLEAN 	SaveVariable = 0;
	UINT8		*StringTable = NULL;
	UINT32 		StringSize = 0;
	UINT8 		*AttributeTable = NULL;
	UINT32 		AttributeSize = 0;
	UINT8 		*ValueTable = NULL;
	UINT32 		ValueSize = 0;
	UINT8 		*MapStringTable = NULL;
	UINT32 		MapStringSize = 0;
	UINT32 		PldmBufferSize = 0; 
	SCE_PLDM_DATA_HEADER 	*PldmBuffer = NULL;
	UINT8		NvramFlashFlag = 1;
    AMI_DIGITAL_SIGNATURE_PROTOCOL *AmiSig = NULL;
    UINT8       *HashDB = NULL;
    UINT32      HashDBSize = 32;
    EFI_GUID    PldmHashGuid = EFI_PLDM_HASH_GUID;
	EFI_GUID	NvramFlashGuid = AFU_BCP_NVRAM_FLASH_GUID;
	UINT32      Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS;
	
	Status = AMI_RetrieveHIIData (&HIIDBData, &DBSize, &CRC32);
	DEBUG((DEBUG_ERROR,"ConfigPreserveInterface: Status for AMI_RetrieveHIIData is %r\n", Status));
	
	if (EFI_ERROR (Status))
	{
		return Status ;
	}
	
	Status = CreatePldmTables (HIIDBData, (UINT32)DBSize, &StringTable, &StringSize, &AttributeTable, &AttributeSize, &ValueTable, &ValueSize, &MapStringTable, &MapStringSize, SaveVariable);
	DEBUG((DEBUG_ERROR,"ConfigPreserveInterface: Status for CreatePldmTables is %r\n", Status));
	
	if (EFI_ERROR (Status))
	{
		return Status ;
	}
	
	PldmBufferSize = sizeof (SCE_PLDM_DATA_HEADER) + StringSize + AttributeSize + ValueSize + MapStringSize;
	PldmBuffer = (SCE_PLDM_DATA_HEADER *)AllocatePool(PldmBufferSize) ;

	if((Status == EFI_SUCCESS) || (NULL != PldmBuffer))
	{	
        MemSet((void*)PldmBuffer, PldmBufferSize, 0 );
        PldmBuffer->Version = Version;
        PldmBuffer->Signature = Signature;
        PldmBuffer->StringOffset = sizeof (SCE_PLDM_DATA_HEADER);
        PldmBuffer->StringSize = StringSize;
        PldmBuffer->AttribOffset = PldmBuffer->StringOffset + PldmBuffer->StringSize ;
        PldmBuffer->AttribSize = AttributeSize;
        PldmBuffer->ValueOffset = PldmBuffer->AttribOffset + PldmBuffer->AttribSize; 
        PldmBuffer->ValueSize = ValueSize;
        PldmBuffer->MappingString = PldmBuffer->ValueOffset + PldmBuffer->ValueSize;
        PldmBuffer->MapStringSize = MapStringSize;
        
        if( (NULL == StringTable) || (NULL == AttributeTable) || (NULL == ValueTable) || (NULL == MapStringTable) )
        {
            return EFI_INVALID_PARAMETER;
        }
        else
        {
            MemCpy ((UINT8 *)PldmBuffer + (PldmBuffer->StringOffset) , StringTable, StringSize);
            MemCpy ((UINT8 *)PldmBuffer + (PldmBuffer->AttribOffset), AttributeTable, AttributeSize);
            MemCpy ((UINT8 *)PldmBuffer + (PldmBuffer->ValueOffset), ValueTable, ValueSize);
            MemCpy ((UINT8 *)PldmBuffer + (PldmBuffer->MappingString), MapStringTable, MapStringSize); 
        }
            
        
        if(*Buffer == NULL)
        {
            *Buffer = AllocatePool(PldmBufferSize) ;
        }
        if(Status == EFI_SUCCESS)
        {
            MemCpy (*Buffer , PldmBuffer, PldmBufferSize);
        }
        if(PldmBufferSize)
            *BufferSize = PldmBufferSize ;
        
        //Free the PLDM Tables memory
        if(AttributeTable)
            free(AttributeTable);
        if(StringTable)
            free(StringTable);
        if(MapStringTable)
            free(MapStringTable);
        if(ValueTable)
            free(ValueTable);
        
        Status = pBS->LocateProtocol(&gAmiDigitalSignatureProtocolGuid, NULL, &AmiSig);
        DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Status for locating gAmiDigitalSignatureProtocolGuid is %r \n", Status));
    
        if (EFI_ERROR (Status))
        {
            return Status ;
        }
        HashDB = (UINT8 *)AllocatePool(50) ;
        DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Status for AllocatePool HashDB is %r \n", Status));
    
		if(  (Status == EFI_SUCCESS) && (NULL != HashDB) )
        {		
			Status = AmiSig->Hash (AmiSig, &gEfiHashAlgorithmSha256Guid, 1, (const UINT8 **)&PldmBuffer, (const UINTN*)&PldmBufferSize, HashDB);
			DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Status for Hash is %r \n", Status));
		
			if (EFI_ERROR (Status))
			{
				return Status ;
			}
			Status = pRS->SetVariable(
						L"PldmHash",
						&PldmHashGuid,
						EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS,
						HashDBSize,
						HashDB
						);
			DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Status for PldmHash SetVariable %r \n", Status));
			
			Status = pRS->SetVariable(
						L"NvramFlashed",
						&NvramFlashGuid,
						EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS,
						sizeof (UINT8),
						&NvramFlashFlag
						);
			
			if(PldmBuffer)
				FreePool(PldmBuffer);
		}
	}
	
	return Status ;
}

/**
    Function to create capsule and push it through UpdateCapsule

    @param void *
    @param UINT32
    
    @retval EFI_STATUS
*/ 

EFI_STATUS UpdateCapsuleToPush(IN void *PldmBuffer, IN UINT32 PldmBufferSize )
{
	EFI_STATUS Status = EFI_SUCCESS ;
	EFI_CAPSULE_HEADER   	*CapsuleHeader;
	EFI_GUID    PldmCapsuleGuid = EFI_PLDM_CAPSULE_GUID;
	UINTN       Size = 0;
	EFI_CAPSULE_BLOCK_DESCRIPTOR  *PldmCapsuleHdr = NULL;
	CHAR16  *FwCapsuleVarName = NULL;
	UINT32      Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS;
	
	CapsuleHeader = NULL;
	CapsuleHeader = AllocatePool( sizeof (EFI_CAPSULE_HEADER) ) ;
	if ((!EFI_ERROR (Status)) && (NULL != CapsuleHeader))
	{

		MemSet((void*)CapsuleHeader, sizeof (EFI_CAPSULE_HEADER), 0 );
		MemCpy (&(CapsuleHeader->CapsuleGuid), &PldmCapsuleGuid, sizeof (EFI_GUID));
		CapsuleHeader->HeaderSize = sizeof (EFI_CAPSULE_HEADER);
		CapsuleHeader->Flags = CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE | CAPSULE_FLAGS_PERSIST_ACROSS_RESET;
		CapsuleHeader->CapsuleImageSize = PldmBufferSize + sizeof (EFI_CAPSULE_HEADER);
		DEBUG((DEBUG_ERROR, "ConfigPreserveInterface: CapsuleHeader->HeaderSize is %x CapsuleHeader->Flags is %x CapsuleHeader->CapsuleImageSize is %x\n",
		CapsuleHeader->HeaderSize, CapsuleHeader->Flags, CapsuleHeader->CapsuleImageSize));
		
		Size = 4 * sizeof (EFI_CAPSULE_BLOCK_DESCRIPTOR) + sizeof (EFI_CAPSULE_HEADER); // (4*16)+28
		DEBUG((DEBUG_ERROR, "ConfigPreserveInterface: Size for capsule %x\n", Size));
		Status = pBS->AllocatePool (EfiReservedMemoryType, Size, (void**)&PldmCapsuleHdr);
		if ((!EFI_ERROR (Status)) && (NULL != PldmCapsuleHdr))
		{
			MemSet ((void*)PldmCapsuleHdr, Size, 0 );
			PldmCapsuleHdr [0].Length = CapsuleHeader->HeaderSize;
			PldmCapsuleHdr [0].Union.DataBlock = (EFI_PHYSICAL_ADDRESS)CapsuleHeader;
			
			PldmCapsuleHdr [1].Length = PldmBufferSize;
			PldmCapsuleHdr [1].Union.DataBlock = (EFI_PHYSICAL_ADDRESS)PldmBuffer;
			
			PldmCapsuleHdr [2].Length = 0;
			PldmCapsuleHdr [2].Union.DataBlock = 0;
			
			//Get the VapsuleVarname and Set with Variable data
			FwCapsuleVarName = BCPGetLastFwCapsuleVariable ();
			if (FwCapsuleVarName != NULL)
            {
                EFI_STATUS Status = EFI_SUCCESS;
                
                if(FVDataAccessType ())
                {
                    UINTN BcpFvOffset = 0 ;
                    
                    Status = pBS->LocateProtocol (&gFlashProtocolGuid, NULL, &gConfigmFlash);  // Locating FLASH protocol
                   DEBUG((DEBUG_ERROR,"UpdateCapsuleToPush::In gFlashProtocolGuid %r\n", Status));
                    
                    BcpFvOffset = FVBCPVolumeOffset();
                   DEBUG((DEBUG_ERROR,"UpdateCapsuleToPush::In FVBCPVolumeOffset %r\n", FVBCPVolumeOffset()));
                    gFvLocation = (void *)( FLASH_DEVICE_FV_BASE + BcpFvOffset );
                   DEBUG((DEBUG_ERROR,"UpdateCapsuleToPush: (SmmLocateProtocol success) %x, gFvLocation:%x\n", gConfigmFlash, gFvLocation));
                    
                    if( gConfigmFlash != NULL && gFvLocation != NULL )
                    {
                        IsFVClean();
                    }
                }
                
                Size = sizeof (EFI_PHYSICAL_ADDRESS);
                //Set the NVRAM
                Status = pRS->SetVariable(
                        FwCapsuleVarName,
                        &gEfiCapsuleVendorGuid,
                        Attributes,
                        Size,
                    (void *)&PldmCapsuleHdr
                    );
                DEBUG((DEBUG_ERROR,"BiosConfigPreserve: status for FwCapsuleVarName setvariable is %x,Name:%a\n", Status,FwCapsuleVarName));
            }
		}
	}
	
	return Status ;
}

/**
    Function to create PLDm table and write it to a file

    @param void **	
    @param CHAR16 *
    @param UINT32
    
    @retval EFI_STATUS
*/ 
EFI_STATUS Preserve_flash_Update( IN void *PldmBuffer,CHAR16 *FileName, IN UINT32 PldmBufferSize, EFI_HANDLE FileSystemHandle )
{
	EFI_STATUS Status = EFI_NOT_FOUND ;
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFileSystemInterface;
	EFI_FILE_PROTOCOL	 *mRoot = NULL;
	EFI_FILE_PROTOCOL  *FileHandle = NULL;
	EFI_FILE_PROTOCOL  *NewFileHandle = NULL;
	UINTN 				HandleCount = 0 ;
	UINTN 				HandleIndex = 0;
	EFI_HANDLE 			*HandleBuffer;
	CHAR16 *PLDMPath = (CHAR16 *)NULL ;
	UINTN PldmPathSize = 0 ;
	CHAR8       *FileName8 = NULL;
	CHAR16       *FileName16 = NULL;
	EFI_GUID    PldmFilePathGuid = EFI_PLDM_FILE_PATH_GUID;
	UINT32      Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS;
	UINTN BcpFvOffset = 0 ;
	
	if(!FVDataAccessType ())
	{
	    PLDMPath = (CHAR16 *) AllocateZeroPool (1024 * sizeof (CHAR16));
	    if(NULL == PLDMPath)
	    {
	        return EFI_OUT_OF_RESOURCES;
	    }
	    
	    //Check the file system handle provided is valid,if valid create the file in that file system
	    if(FileSystemHandle)
	    {
            Status = pBS->HandleProtocol(FileSystemHandle, &gEfiSimpleFileSystemProtocolGuid, (void**)&SimpleFileSystemInterface );
            if(EFI_SUCCESS == Status)
            {
                Status = SimpleFileSystemInterface->OpenVolume(SimpleFileSystemInterface, &mRoot);
                if(EFI_SUCCESS == Status)
                {
                    Status = mRoot->Open(mRoot, &NewFileHandle, L"PldmFile.txt",( EFI_FILE_MODE_READ|EFI_FILE_MODE_WRITE), 0);
                    if(!EFI_ERROR (Status))
                    {
                        //If file is already present, then delete the file.
                        NewFileHandle->Delete(NewFileHandle);
                        NewFileHandle = NULL;
                    }
                    
                    Status = mRoot->Open(mRoot, &NewFileHandle, L"PldmFile.txt",( EFI_FILE_MODE_READ|EFI_FILE_MODE_CREATE|EFI_FILE_MODE_WRITE), 0);
            
                    if(EFI_SUCCESS == Status)
                    {
                        Status = NewFileHandle->Write(NewFileHandle, (UINTN *)&PldmBufferSize, PldmBuffer );
                        if( PLDMPath != (CHAR16 *)NULL )
                        {
                            CHAR8 *PldmPath8 = (CHAR8 * )NULL;
                            Wcscpy( PLDMPath, (CHAR16 *)L"PldmFile.txt" ) ;
                            PldmPathSize = Wcslen(PLDMPath) ;
            
                            PldmPath8 = PldmStrDup16to8(PLDMPath);
            
                            if(*PldmPath8 != 0 )
                            VarSetNvramName (L"PldmFileName", &PldmFilePathGuid, Attributes, PldmPath8, Strlen(PldmPath8));
                            if(PldmPath8)
                            FreePool(PldmPath8);
                        }
                        if(NewFileHandle)
                            NewFileHandle->Close(NewFileHandle) ;
                    }
                    if(mRoot)
                        mRoot->Close(mRoot) ;
                }
            }
	    }
	    
	    //Create File if the FileSystemHandle failed to create the file or if it is NULL 
	    if(EFI_SUCCESS != Status)
	    {
            Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiSimpleFileSystemProtocolGuid, NULL, &HandleCount, &HandleBuffer );	
            if(EFI_SUCCESS == Status)
            {
                for(HandleIndex=0; HandleIndex < HandleCount; HandleIndex++)
                {
                    Status = pBS->HandleProtocol(HandleBuffer[HandleIndex], &gEfiSimpleFileSystemProtocolGuid, (void**)&SimpleFileSystemInterface );
                    if(EFI_SUCCESS == Status)
                    {
                        Status = SimpleFileSystemInterface->OpenVolume(SimpleFileSystemInterface, &mRoot);
                        if(EFI_SUCCESS == Status)
                        {
                            Status = mRoot->Open(mRoot, &FileHandle, FileName,( EFI_FILE_MODE_READ), 0);
                            if(EFI_SUCCESS == Status)
                            {
                                if(FileHandle)
                                    FileHandle->Close(FileHandle) ;
                                
                                Status = mRoot->Open(mRoot, &NewFileHandle, L"PldmFile.txt",( EFI_FILE_MODE_READ|EFI_FILE_MODE_WRITE), 0);
                                if(!EFI_ERROR (Status))
                                {
                                    //If file is already present, then delete the file.
                                    NewFileHandle->Delete(NewFileHandle);
                                    NewFileHandle = NULL;
                                }
                                
                                Status = mRoot->Open(mRoot, &NewFileHandle, L"PldmFile.txt",( EFI_FILE_MODE_READ|EFI_FILE_MODE_CREATE|EFI_FILE_MODE_WRITE), 0);
                    
                                if(EFI_SUCCESS == Status)
                                {
                                    Status = NewFileHandle->Write(NewFileHandle, (UINTN *)&PldmBufferSize, PldmBuffer );
                                    if( PLDMPath != (CHAR16 *)NULL )
                                    {
                                        CHAR8 *PldmPath8 = (CHAR8 * )NULL;
                                        Wcscpy( PLDMPath, (CHAR16 *)L"PldmFile.txt" ) ;
                                        PldmPathSize = Wcslen(PLDMPath) ;
                        
                                        PldmPath8 = PldmStrDup16to8(PLDMPath);
                        
                                        if(*PldmPath8 != 0 )
                                            VarSetNvramName (L"PldmFileName", &PldmFilePathGuid, Attributes, PldmPath8, Strlen(PldmPath8));
                                        if(PldmPath8)
                                            FreePool(PldmPath8);
                                    }
                                    if(NewFileHandle)
                                        NewFileHandle->Close(NewFileHandle) ;
                                }
                                if(mRoot)
                                    mRoot->Close(mRoot) ;
                            }
                        }
                    }
                }
            }
            if(HandleBuffer)
                gBS->FreePool(HandleBuffer);
	    }
		
	    if(PLDMPath)
		FreePool(PLDMPath);
	}
	else
	{
        Status = pBS->LocateProtocol (&gFlashProtocolGuid, NULL, &gConfigmFlash);  // Locating FLASH protocol
       DEBUG((DEBUG_ERROR,"ConfigPreserveInterface::In gFlashProtocolGuid %r\n", Status));
        
        BcpFvOffset = FVBCPVolumeOffset();
       DEBUG((DEBUG_ERROR,"ConfigPreserveInterface::In FVBCPVolumeOffset %r\n", FVBCPVolumeOffset()));
        gFvLocation = (void *)( FLASH_DEVICE_FV_BASE + BcpFvOffset );
       DEBUG((DEBUG_ERROR,"ConfigPreserveInterface: (SmmLocateProtocol success) %x, gFvLocation:%x\n", gConfigmFlash, gFvLocation));
		
		if( gConfigmFlash != NULL && gFvLocation != NULL )
		{
			UINTN BcpFlashBlockSize = 0;
			UINTN FlashDataSize = 0;
			
			IsFVClean();
			
			FlashDataSize = FVFlashDataSize();
			if(PldmBufferSize < FlashDataSize)
			{
			    VOID *TempAddr;
			    SCE_PLDM_DATA_HEADER *ScePldmFVData = NULL;
			    
			    ScePldmFVData = (SCE_PLDM_DATA_HEADER *)PldmBuffer;
			    BcpFlashBlockSize = FVFlashBlockSize();
			
			    DEBUG((DEBUG_ERROR,"ConfigPreserveInterface: ScePldmFVData->Signature %x \n", ScePldmFVData->Signature));
			    TempAddr = (VOID *)((UINT8 *)gFvLocation + BCP_FFS_DATA_START_OFFSET);
			   DEBUG((DEBUG_ERROR,"ConfigPreserveInterface::In WriteInFv: (TempAddr) %x\n", TempAddr));
			    if(PldmBufferSize < BcpFlashBlockSize)
			    {
                    Status = WriteToFlash(TempAddr,
                                (VOID*)PldmBuffer,
                                PldmBufferSize) ;
                            
                   DEBUG((DEBUG_ERROR,"ConfigPreserveInterface::In WriteInFv: (WriteToFlash) %r\n", Status));
			    }
			    else
			    {
					// Starting the writing process
                    UINTN SizeWritten = 0;
                    UINTN InitialSize = 0;
                    UINTN RemainingSize = 0;
                    
                    InitialSize = BcpFlashBlockSize - BCP_FFS_DATA_START_OFFSET;
                    
                   DEBUG((DEBUG_ERROR,"In ConfigPreserveInterface:: (WriteToFlash) InitialSize: %d\n", InitialSize));
                    Status = WriteToFlash(TempAddr,
                            (VOID*)PldmBuffer,
                            InitialSize);
                    SizeWritten = InitialSize;
                    RemainingSize = PldmBufferSize - SizeWritten;
                   DEBUG((DEBUG_ERROR,"In ConfigPreserveInterface:: (IniWriteToFlash) SizeWritten: %d, RemainingSize: %d\n", SizeWritten, RemainingSize));
                    for(; RemainingSize > 0; )
                    {
                        if(RemainingSize < BcpFlashBlockSize)
                        {
                           DEBUG((DEBUG_ERROR,"In ConfigPreserveInterface:: (WriteToFlash) EndPart: %d\n", RemainingSize));
                           DEBUG((DEBUG_ERROR,"ConfigPreserveInterface::In WriteInFv: (EndPart TempAddr) %x\n", (VOID*)((UINT8 *)TempAddr + SizeWritten)));
                            Status = WriteToFlash((VOID*)((UINT8 *)TempAddr + SizeWritten),
                                    (VOID*)((UINT8 *)PldmBuffer + SizeWritten),
                                    RemainingSize) ;
                            SizeWritten = SizeWritten + RemainingSize;
                           DEBUG((DEBUG_ERROR,"In ConfigPreserveInterface:: (RemWriteToFlash) %r, SizeWritten: %d\n", Status, SizeWritten));
                        }
                        else
                        {
                           DEBUG((DEBUG_ERROR,"In ConfigPreserveInterface:: (WriteToFlash) BcpFlashBlockSize: %d\n", BcpFlashBlockSize));
                           DEBUG((DEBUG_ERROR,"ConfigPreserveInterface::In WriteInFv: (FlashBlock TempAddr) %x\n", (VOID*)((UINT8 *)TempAddr + SizeWritten)));
                            Status = WriteToFlash((VOID*)((UINT8 *)TempAddr + SizeWritten),
                                    (VOID*)((UINT8 *)PldmBuffer + SizeWritten),
                                    BcpFlashBlockSize);
                            
                            SizeWritten = SizeWritten + BcpFlashBlockSize;
                           DEBUG((DEBUG_ERROR,"In ConfigPreserveInterface:: (BlockWriteToFlash) %r, SizeWritten: %d\n", Status, SizeWritten));
                        }
                        RemainingSize = PldmBufferSize - SizeWritten;
                       DEBUG((DEBUG_ERROR,"In ConfigPreserveInterface:: (WriteToFlash) RemainingSize: %d\n", RemainingSize));
                    }
			    }
			}
			else
			{
			   DEBUG((DEBUG_ERROR,"In ConfigPreserveInterface:: (Error) PldmBufferSize: %d, FlashDataSize: %d\n", PldmBufferSize, FlashDataSize));
			}
					}
	}
    
	return Status ;
}

/**
     Function to get FV data and update it to a buffer

     @param void ** 
     @param UINT32 *
     
     @retval EFI_STATUS
 */ 
EFI_STATUS Get_FV_Data( OUT void **Buffer, IN OUT UINT32 *BufferSize )
{
    EFI_STATUS  Status = EFI_SUCCESS;
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
    DEBUG((DEBUG_ERROR,"Get_FV_Data:IsFVClean: BcpFvSize %x \n", BcpFvSize));
    
    BcpFlashBlockSize = FVFlashBlockSize();
    
    ReadData = (UINT8 *)AllocatePool(BcpFvSize) ;

    FvData = (UINT8 *)AllocatePool(BcpFlashBlockSize) ;
    
    if( (ReadData != NULL) && (FvData != NULL) )
    {
       DEBUG((DEBUG_ERROR,"Get_FV_Data:In IsFVClean: (WriteToFlash) entered\n"));
        Status = gConfigmFlash->DeviceWriteEnable();
        if (EFI_ERROR(Status))
        {
            if(FvData)
            {
                FreePool(FvData) ;
                FvData = NULL;
            }
                
            if(ReadData)
            {
                FreePool(ReadData) ;
                ReadData = NULL;
            }
            return Status;
        }
    
       DEBUG((DEBUG_ERROR,"In Get_FV_Data:IsFVClean: BcpFlashBlockSize: %d\n", BcpFlashBlockSize));
        for(Blocks = 0; Blocks < BcpFvSize; )
        {
           DEBUG((DEBUG_ERROR,"In Get_FV_Data: ReadAddress: %x, DataAddress: %x, Blocks: %x\n", ((UINT8 *)gFvLocation + Blocks), (ReadData + Blocks), Blocks));
            Status = gConfigmFlash->Read((VOID *)((UINT8 *)gFvLocation + Blocks), BcpFlashBlockSize, (VOID *)(FvData));
            MemCpy((VOID *)(ReadData + Blocks), FvData, BcpFlashBlockSize);
            Blocks += BcpFlashBlockSize;
        }
        
        TempAddr = AllocateZeroPool(BcpFvSize);
        MemCpy(TempAddr, (VOID*)(ReadData + BCP_FFS_DATA_START_OFFSET), BcpFvSize);
        //TempAddr = ReadData + BCP_FFS_DATA_START_OFFSET;
        ScePldmFVData = (SCE_PLDM_DATA_HEADER *)TempAddr;
        DEBUG((DEBUG_ERROR,"Get_FV_Data:IsFVClean: ScePldmFVData->Signature %x \n", ScePldmFVData->Signature));
    
        if( ! MemCmp((((UINT8 *)TempAddr)+4), (UINT8 *)PldmSignature, 4) )	// Searching for PLDM signature
        {
            *Buffer = TempAddr ;
            *BufferSize = ScePldmFVData->MappingString + ScePldmFVData->MapStringSize;
    
            Status = EFI_SUCCESS;
           DEBUG((DEBUG_ERROR,"Get_FV_Data:: Updating FV Data %r\n", Status));
        }
        gConfigmFlash->DeviceWriteDisable();
    
    }
	
	if(FvData)
	{
		FreePool(FvData) ;
		FvData = NULL;
	}
		
    if(ReadData)
    {
        FreePool(ReadData) ;
        ReadData = NULL;
    }
    
   DEBUG((DEBUG_ERROR,"Get_FV_Data::In IsFVClean: (WriteToFlash) exit %r\n", Status));

    return Status ;
}


//************************************************************************
//************************************************************************
//**                                                                  	**
//**        (C)Copyright 2015, American Megatrends, Inc.            	**
//**                                                                  	**
//**                       All Rights Reserved.                       	**
//**                                                                  	**
//**  5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093              **
//**                                                                  	**
//**                       Phone: (770)-246-8600                      	**
//**                                                                  	**
//************************************************************************
//************************************************************************
