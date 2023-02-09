//************************************************************************
//************************************************************************
//**                                                                  	**
//**        (C)Copyright 2016, American Megatrends, Inc.            	**
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
PldmImportLib.c

Responsible for getting Pldm table and importing it

*/

//************************************************************************
//					Include Section
//************************************************************************


//#include "stdlib.h"
#include "AmiDxeLib.h"
#include <Protocol/SimpleFileSystem.h>
#include "PldmSceStruct.h"
#include "Protocol/AmiDigitalSignature.h"
#include "Token.h"
#include "Framework/Hob.h"
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>
#include <Protocol/FlashProtocol.h>
#include <Protocol/FirmwareVolumeBlock.h>
#include <Library/MemoryAllocationLib.h>

#define Align4(Value) (((Value)+3) & ~3)
#define Align8(Value) (((Value)+7) & ~7)

UINT8	*gPldmHash = NULL;
CHAR16 	*gPldmFileName = NULL ;
UINTN 	gHashSize = 0;
BOOLEAN gRunTimeFlash = 0 ;
UINT8   *gPldmNvram = NULL;
UINTN   gPldmNvramSize = 0;
UINTN   gDataSize = 0;
UINT8   *gPldmRecData = NULL;
EFI_GUID        gFlashDataFile  =  EFI_BCP_FV_DATA_GUID;
FLASH_PROTOCOL  *gPLDMFlash = NULL;
BOOLEAN         gUseNewImage = FALSE;
BOOLEAN         gIsPreserveAborted = FALSE;
extern EFI_GUID gNvramPreserveStatusGuid;
/**
    Find the RAW section

    @param Section
    @param End

    @return Pointer to Raw Section if found

**/
VOID*
FindRawSection (
    IN VOID *Section,
    IN VOID *End
)
{
    EFI_COMMON_SECTION_HEADER   *p = Section;
    while((UINTN)p < (UINTN)End)    //Use signed because 0 = 0x100000000
    {
        if (p->Type == EFI_SECTION_RAW) return (p+1);

        if (Align4(SECTION_SIZE(p)) == 0x00) {
            return 0;               // Section size = 0 indicates data is corrupted
        }

        p = (EFI_COMMON_SECTION_HEADER*)((UINT8*)p+Align4(SECTION_SIZE(p)));
    }
    return 0;
}


/**
    Find file in the FV with the input GUID

    @param Guid
    @param File
    @param EndOfFiles

    @return Pointer to File if found

**/
VOID*
FindFile (
    IN EFI_GUID *Guid,
    IN VOID     *File,
    IN VOID     *EndOfFiles
)
{
    EFI_FFS_FILE_HEADER *p = File;

    while(((UINTN)p < (UINTN)EndOfFiles) &&
          ((UINTN)(p+sizeof(EFI_FFS_FILE_HEADER)) < (UINTN)EndOfFiles)) // Use signed because 0 = 0x100000000
    {
        if (CompareGuid(Guid,&p->Name)) {
            // Found File.
            return FindRawSection(
                        p+1,
                        (UINT8*)p + (*(UINT32*)p->Size & 0xffffff) - sizeof(*p)
                        );
        }

        p = (EFI_FFS_FILE_HEADER*)((UINT8*)p + Align8((*(UINT32*)p->Size & 0xffffff)));
        if((*(UINT32 *)p == 0xFFFFFFFF))    //If Next file is not there, then break.
            break;
    }

    return NULL;
}


/**
   	Function to get Volume name for a specific file system

	@param EFI_HANDLE
  	
	@retval	CHAR16*
*/ 
CHAR16 * GetVolumeNameForFileSystemHandle( EFI_HANDLE FsHandle)
{
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFileSystem = (EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *)NULL;
    EFI_FILE_PROTOCOL *FileSystem = (EFI_FILE_PROTOCOL *)NULL;
    CHAR16 *VolName = (CHAR16 *)NULL;
    UINTN      FileSystemInfoSize = (UINTN)sizeof(EFI_FILE_SYSTEM_INFO) + 1024;
    EFI_FILE_SYSTEM_INFO    *FileSystemInfo = (EFI_FILE_SYSTEM_INFO*)NULL;
    EFI_GUID FileSystemInfoType = EFI_FILE_SYSTEM_INFO_ID;
    
    if(FsHandle == NULL)
        return NULL;
    Status = gBS->HandleProtocol(FsHandle, &gEfiSimpleFileSystemProtocolGuid,(void**) &SimpleFileSystem);

    if(!EFI_ERROR(Status))
    {
        Status = SimpleFileSystem->OpenVolume(SimpleFileSystem, &FileSystem);
    }
    
    if(Status == EFI_SUCCESS && FileSystem)
    {
       Status = gBS->AllocatePool(EfiBootServicesData,FileSystemInfoSize,(void **)&FileSystemInfo);
       if(Status == EFI_SUCCESS)
       {
           Status = FileSystem->GetInfo(FileSystem, &FileSystemInfoType, &FileSystemInfoSize, FileSystemInfo);
           if(Status == EFI_BUFFER_TOO_SMALL)
           {
               if (FileSystemInfo != NULL)
               {
                   gBS->FreePool(FileSystemInfo);
                   FileSystemInfo = (EFI_FILE_SYSTEM_INFO *)NULL;
               }
               Status = gBS->AllocatePool(EfiBootServicesData,FileSystemInfoSize,(void **)&FileSystemInfo); 
               if ( Status != EFI_SUCCESS )
                   return NULL;
               Status = FileSystem->GetInfo(FileSystem, &FileSystemInfoType, &FileSystemInfoSize, FileSystemInfo);
               if ( Status != EFI_SUCCESS ) 
               {
                   return NULL;
               }
           }
           if (!EFI_ERROR(Status))
           {
                if(FileSystemInfo->VolumeLabel)
                {
                    Status = gBS->AllocatePool(EfiBootServicesCode,(Wcslen(FileSystemInfo->VolumeLabel)+1)*2, (VOID**)&VolName);
                    if ( Status != EFI_SUCCESS ) 
                    {
                    return NULL;
                    }
                    Wcscpy(VolName,FileSystemInfo->VolumeLabel);
                }
                else
                   return NULL;
           }
       }
    }
    if(FileSystemInfo)
        gBS->FreePool(FileSystemInfo);
    
    return VolName;
}

/**
   	Function to open a particular file Handle

	@param EFI_HANDLE
  	@param CHAR16 *
  	
	@retval	EFI_FILE_PROTOCOL *
*/ 

EFI_FILE_PROTOCOL *Opening_File_Handle(EFI_HANDLE FSHandle , CHAR16 *FileName)
{
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFileSystem = NULL;
	EFI_FILE	 *mRoot = NULL;
	EFI_FILE_PROTOCOL  *FileHandle = NULL;
	EFI_STATUS  Status = EFI_SUCCESS;
	
	Status = gBS->HandleProtocol ( FSHandle, &gEfiSimpleFileSystemProtocolGuid,(VOID*)&SimpleFileSystem);

	if(Status == EFI_SUCCESS)
	{
		Status = SimpleFileSystem->OpenVolume (SimpleFileSystem,&mRoot);
		if(Status == EFI_SUCCESS)
		{
			Status = mRoot->Open (mRoot,&FileHandle,FileName,EFI_FILE_MODE_READ,0);
		}
	}
		
	if (EFI_ERROR(Status)) 
	{
		return NULL;
	}
	
	return FileHandle;
}

/**
   	Function to search the particular file and return the file path

	@param EFI_FILE_PROTOCOL *
  	@param CHAR16 *
  	@param CHAR16 *
  	
	@retval	CHAR16 *
*/ 
CHAR16 *GetFilePath(EFI_FILE_PROTOCOL *SourceRoot, CHAR16 *SourceFilePath, CHAR16* FileNameToSearch)
{
	EFI_STATUS Status;
	EFI_FILE_INFO *FileInfo = NULL;
	EFI_FILE_PROTOCOL *NewFileHandle = NULL;
	UINTN Length1=0,Length2=0;
	UINTN Length=128;
	CHAR16 Slash[2] = L"\\";
			 
	if(SourceFilePath == NULL) // need to shift
	{
		Status=pBS->AllocatePool(EfiBootServicesData,1024,&SourceFilePath);
		MemSet(SourceFilePath,1024,0);
		StrCpy(SourceFilePath,Slash); 
	}
		
	 Status = SourceRoot->Open (SourceRoot,&NewFileHandle,SourceFilePath,EFI_FILE_MODE_READ,0);
	 if(Status == EFI_SUCCESS)
	 {
         while(1)
		 {
    		 Status=pBS->AllocatePool(EfiBootServicesData,Length,&FileInfo);
    		 if(Status != EFI_SUCCESS)
    			 break;
    		 
			 Status=SourceRoot->Read(NewFileHandle,&Length,FileInfo);
             // Reallocate enough buffer if the buffer is small
			 if((Status != 0) && (Status != EFI_BUFFER_TOO_SMALL))  // for checking the file read failure like Out_Of_Resources
			 {
				 Length = 0;
			 }
             if(Status == EFI_BUFFER_TOO_SMALL)
			 {
                 if(FileInfo)
                     pBS->FreePool(FileInfo);
                 Status=pBS->AllocatePool(EfiBootServicesData,Length,&FileInfo);
                 if(Status == EFI_SUCCESS)
                 {
                     Status=SourceRoot->Read(NewFileHandle,&Length,FileInfo);
        			 if((Status != 0))
        			 {
        				 Length = 0;
        			 }
                 }
                 else
                	 Length = 0;
             }
            	 
             if(Length == 0) // All files parsed
			 {
                 if(FileInfo)
                     pBS->FreePool(FileInfo);
				 break;
			 }
			 
			 if(Status == EFI_SUCCESS) //Valid file read
			 {	 
				if((Wcscmp(FileInfo->FileName , L".") && Wcscmp(FileInfo->FileName , L"..")) == 0)         
					continue;
				 				 
				 if(FileInfo->Attribute & EFI_FILE_DIRECTORY )              // if it is a directory
				 {
					StrCat(SourceFilePath, FileInfo->FileName) ;
					StrCat(SourceFilePath,Slash);           // concatenating the slash(\) and file name with the source path
														
					GetFilePath(SourceRoot,SourceFilePath,FileNameToSearch);     // calling recursively inside the directories
					if( NULL != StrStr(SourceFilePath, FileNameToSearch) )
						return SourceFilePath ;
					Length1 = Wcslen(FileInfo->FileName);					
					Length2 = Wcslen(SourceFilePath);	

					SourceFilePath[Length2-Length1-1] = L'\0';        // again removing the last filename from the source filepath
				 }
				 else
				 {	
					 if( (FileInfo->FileName != NULL) && (FileNameToSearch != NULL) )
						 if( StrCmp( FileInfo->FileName, FileNameToSearch ) == 0 )
						 {
							 return StrCat(SourceFilePath, FileInfo->FileName) ;
						 }
					 
				 }
			 }
	         if(FileInfo)			 
	             pBS->FreePool(FileInfo);
		}	 
	 }
	 return SourceFilePath;
}

/**
   	Function to search the specific file

	@param EFI_HANDLE
  	@param CHAR16 *
  	
	@retval	CHAR16 * 
*/ 
CHAR16 *SearchingFile(EFI_HANDLE SourceFileSystemHandle, CHAR16* FileName)
{
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SourceInterface;
	EFI_FILE_PROTOCOL  *SourceRoot=NULL;
	EFI_STATUS Status;
	CHAR16 *FullFilePath = (CHAR16 *)NULL ;
	CHAR16 Slash[2] = L"\\";

	 Status= pBS->HandleProtocol(SourceFileSystemHandle,&gEfiSimpleFileSystemProtocolGuid,(VOID **)&SourceInterface);
	 if(Status == EFI_SUCCESS)
	 {
		 Status=SourceInterface->OpenVolume(SourceInterface,&SourceRoot);
		 if(Status == EFI_SUCCESS)
		 {
			 FullFilePath = GetFilePath(SourceRoot, 0, FileName) ;
			 if( 0 == StrCmp(FullFilePath, Slash) )
			 {
				 StrCpy(FullFilePath, L"\0") ;
				 if(NULL != FullFilePath)
				 {
					gBS->FreePool(FullFilePath);
					FullFilePath = (CHAR16 *)NULL;
				 }
			 }
		 }
	 }
	 return FullFilePath;	 
}

/**
    Function to read Pldm file from FV and imports it

    @param EFI_HANDLE
    
    @retval EFI_STATUS
*/
EFI_STATUS Apply_Pldm_Offset_FV( VOID )
{
    EFI_STATUS Status = EFI_SUCCESS ;
    
	Status = CompareFVHashAndImportPldm ();
	DEBUG((DEBUG_ERROR,"In PLDMImportLib: (CompareFVHashAndImportPldm success) %r\n", Status));
    
    return Status ;
}

/**
   	Function to read Pldm file and imports it

	@param EFI_HANDLE
  	
	@retval	EFI_STATUS
*/ 
EFI_STATUS Apply_Pldm_Offset (EFI_HANDLE FSHandle)
{
	EFI_STATUS 	Status = EFI_SUCCESS;  
	UINTN 		FileInfoSize = 0;
	UINT64 		FileSize = 0 ;
	UINT8 		*Buffer = 0 ;
	EFI_FILE_PROTOCOL  *SourceRoot=NULL;
	EFI_FILE_PROTOCOL *NewFileHandle = NULL;
	EFI_FILE_INFO *FileInfo = NULL;
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFileSystem = (EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *)NULL;
	
	Status = gBS->HandleProtocol ( FSHandle, &gEfiSimpleFileSystemProtocolGuid,(VOID*)&SimpleFileSystem);
	
	if (!EFI_ERROR (Status))
	{
		//Test_File_Handle(TempHandleBuffer[Index], Pldm) ;
		Status=SimpleFileSystem->OpenVolume(SimpleFileSystem,&SourceRoot);
		if (!EFI_ERROR (Status))
		{
			Status = SourceRoot->Open (SourceRoot,&NewFileHandle,gPldmFileName,EFI_FILE_MODE_READ,0);
			if (!EFI_ERROR (Status))
			{
				FileInfoSize =  (UINTN)(sizeof(EFI_FILE_INFO) + 128);
				gBS->AllocatePool(EfiBootServicesData,FileInfoSize,(VOID **)&FileInfo);
				Status = NewFileHandle->GetInfo(NewFileHandle, &gEfiFileInfoGuid, &FileInfoSize, FileInfo);
				if(EFI_BUFFER_TOO_SMALL == Status)
				{
					gBS->AllocatePool(EfiBootServicesData,FileInfoSize,(VOID **)&FileInfo);
					Status = NewFileHandle->GetInfo(NewFileHandle, &gEfiFileInfoGuid, &FileInfoSize, FileInfo);
				}
				
				if (!EFI_ERROR (Status))
				{
					FileSize = (UINTN)(FileInfo->FileSize);
					gBS->AllocatePool(EfiBootServicesData, (UINTN)FileSize, &Buffer);
					Status = NewFileHandle->Read(NewFileHandle, (UINTN *)&FileSize, Buffer);
					if(EFI_BUFFER_TOO_SMALL == Status)
					{
						gBS->AllocatePool(EfiBootServicesData, (UINTN)FileSize, &Buffer);
						Status = NewFileHandle->Read(NewFileHandle, (UINTN *)&FileSize, Buffer);
					}
					if (!EFI_ERROR (Status))
					{
						DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Apply_Pldm_Offset: Buffer is %x FileSize is %x\n", Buffer, FileSize));
						Status = CompareHashAndImportPldm (Buffer, (UINT32)FileSize);
					}
				}
			}
		}
	}
	return Status ;
}

EFI_STATUS CreateBsNvramVariable(CHAR16 *VariableName,EFI_GUID *NvramPreserveGuid)
{
	EFI_STATUS Status = EFI_SUCCESS;
	UINT32 PreserveVal = 0;	
	UINT32 		Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE; 
	
	Status = pRS->SetVariable (VariableName, NvramPreserveGuid, Attributes, sizeof(UINT32), &PreserveVal);
	
	return Status;
}

/**
   	Function to compare the hash and import the Pldm table

	@param Buffer     	Pldm buffer
  	@param BufferSize	*Pldm buffer size 
  	
	@retval	EFI_STATUS
*/ 
EFI_STATUS CompareHashAndImportPldm (UINT8 *Buffer, UINT32 BufferSize)
{
	EFI_STATUS 	Status = EFI_SUCCESS;
	UINT8 		*HIIDBData = {0};
	UINT32 		CRC32 = 0;
	UINTN 		DBSize = 0;
	EFI_GUID 	AmiDigitalSignProtocolGuid = AMI_DIGITAL_SIGNATURE_PROTOCOL_GUID;
	UINT8	 	HashDB [33] = {0};
	AMI_DIGITAL_SIGNATURE_PROTOCOL *AmiSig = NULL;
	SCE_PLDM_DATA_HEADER *ScePldmData = NULL;
	
	DEBUG((DEBUG_ERROR,"BiosConfigPreserve:CompareHashAndImportPldm: In CompareHashAndImportPldm\n"));
	
	////////////Hashing validation starts//////////
	Status = pBS->LocateProtocol (&AmiDigitalSignProtocolGuid, NULL, &AmiSig);
	DEBUG((DEBUG_ERROR,"BiosConfigPreserve:CompareHashAndImportPldm: Status for AmiDigitalSignProtocolGuid is %r\n", Status));
	if (EFI_ERROR (Status))
	{
		return Status;
	}

    Status = AmiSig->Hash (AmiSig, &gEfiHashAlgorithmSha256Guid, 1, &Buffer,(const UINTN*)&BufferSize, HashDB);
    DEBUG((DEBUG_ERROR,"BiosConfigPreserve:CompareHashAndImportPldm: Status for Hash is %r   %d\n", Status,gHashSize));

	if (EFI_ERROR (Status))
	{
		return Status;
	}

	if (MemCmp (gPldmHash, HashDB, gHashSize))
	{
		return EFI_SECURITY_VIOLATION;
	}
	DEBUG((DEBUG_ERROR,"BiosConfigPreserve:CompareHashAndImportPldm: Hash matches\n"));
	////////////Hashing validation ends//////////
					            	
	Status = AMI_RetrieveHIIData (&HIIDBData, &DBSize, &CRC32);
	DEBUG((DEBUG_ERROR,"BiosConfigPreserve:CompareHashAndImportPldm: Status for AMI_RetrieveHIIData is %r\n", Status));
	if (!EFI_ERROR (Status))
	{
		ScePldmData = (SCE_PLDM_DATA_HEADER *)Buffer;
		Status = ImportMappedPldmValues (
						HIIDBData,
						(UINT32)DBSize,
						(Buffer + (ScePldmData->ValueOffset)),
						ScePldmData->ValueSize,
						(Buffer + (ScePldmData->AttribOffset)),
						ScePldmData->AttribSize,
						(Buffer + (ScePldmData->MappingString)),
						ScePldmData->MapStringSize,
						(Buffer + (ScePldmData->StringOffset)),
						ScePldmData->StringSize
						);
		DEBUG((DEBUG_ERROR,"BiosConfigPreserve:CompareHashAndImportPldm: Status for ImportMappedPldmValues is %r\n", Status));
		if(Status == EFI_SUCCESS)
		{
			CreateBsNvramVariable(L"ConfigPreserveHappened",&gNvramPreserveStatusGuid);
			gRunTimeFlash = 1 ;
		}
	}
	return Status;
}

/**
    Function to compare the hash from FV and import the Pldm table

    @param Buffer       Pldm buffer
    @param BufferSize   *Pldm buffer size 
    
    @retval EFI_STATUS
*/ 
EFI_STATUS CompareFVHashAndImportPldm ()
{
    EFI_STATUS  Status = EFI_SUCCESS;
    UINT8       *HIIDBData = {0};
    UINT32      CRC32 = 0;
    UINTN       DBSize = 0;
    EFI_GUID    AmiDigitalSignProtocolGuid = AMI_DIGITAL_SIGNATURE_PROTOCOL_GUID;
    UINT8       HashDB [50];
    AMI_DIGITAL_SIGNATURE_PROTOCOL *AmiSig = NULL;
    SCE_PLDM_DATA_HEADER *ScePldmData = NULL;
    SCE_PLDM_DATA_HEADER *ScePldmFVData = NULL;
    UINT32 FVDataSize = 0 ;
    UINT8 *PldmSignature = "PLDM" ;
    
    DEBUG((DEBUG_ERROR,"BiosConfigPreserve:CompareFVHashAndImportPldm: In CompareFVHashAndImportPldm\n"));
    
    ////////////Hashing validation starts//////////
    Status = pBS->LocateProtocol (&AmiDigitalSignProtocolGuid, NULL, &AmiSig);
    DEBUG((DEBUG_ERROR,"BiosConfigPreserve:CompareFVHashAndImportPldm: Status for AmiDigitalSignProtocolGuid is %r\n", Status));
    if (EFI_ERROR (Status))
    {
        return Status;
    }
    DEBUG((DEBUG_ERROR,"BiosConfigPreserve:CompareFVHashAndImportPldm: FVDataBuffer is %x\n", gPldmRecData));
    ScePldmFVData = (SCE_PLDM_DATA_HEADER *)gPldmRecData;
    DEBUG((DEBUG_ERROR,"BiosConfigPreserve:CompareFVHashAndImportPldm: ScePldmFVData->Signature %x \n", ScePldmFVData->Signature));
    
    if( MemCmp((((UINT8 *)gPldmRecData)+4), (UINT8 *)PldmSignature, 4) )
    {
    	DEBUG((DEBUG_ERROR,"BiosConfigPreserve:CompareFVHashAndImportPldm: Signature not found\n"));
    	return EFI_NOT_FOUND;
    }
    
    FVDataSize = ScePldmFVData->MappingString + ScePldmFVData->MapStringSize;
    DEBUG((DEBUG_ERROR,"BiosConfigPreserve:CompareFVHashAndImportPldm: FVDataSize is %d %x \n", FVDataSize, FVDataSize));
    
    Status = AmiSig->Hash (AmiSig, &gEfiHashAlgorithmSha256Guid, 1, &gPldmRecData,(const UINTN*)&FVDataSize, HashDB);
    DEBUG((DEBUG_ERROR,"BiosConfigPreserve:CompareFVHashAndImportPldm: Status for Hash is %r\n", Status));
    if (EFI_ERROR (Status))
    {
        return Status;
    }

    if (MemCmp (gPldmHash, HashDB, gHashSize))
    {
        return EFI_SECURITY_VIOLATION;
    }
    DEBUG((DEBUG_ERROR,"BiosConfigPreserve:CompareFVHashAndImportPldm: FV Hash matches\n"));
    ////////////Hashing validation ends//////////
                                    
    Status = AMI_RetrieveHIIData (&HIIDBData, &DBSize, &CRC32);
    DEBUG((DEBUG_ERROR,"BiosConfigPreserve:CompareFVHashAndImportPldm: Status for AMI_RetrieveHIIData is %r\n", Status));
    if (!EFI_ERROR (Status))
    {
        ScePldmData = (SCE_PLDM_DATA_HEADER *)gPldmRecData;
        Status = ImportMappedPldmValues (
                        HIIDBData,
                        (UINT32)DBSize,
                        (gPldmRecData + (ScePldmData->ValueOffset)),
                        ScePldmData->ValueSize,
                        (gPldmRecData + (ScePldmData->AttribOffset)),
                        ScePldmData->AttribSize,
                        (gPldmRecData + (ScePldmData->MappingString)),
                        ScePldmData->MapStringSize,
                        (gPldmRecData + (ScePldmData->StringOffset)),
                        ScePldmData->StringSize
                        );
        DEBUG((DEBUG_ERROR,"BiosConfigPreserve:CompareFVHashAndImportPldm: Status for ImportMappedFVPldmValues is %r\n", Status));
        if(Status == EFI_SUCCESS)
        {
        	CreateBsNvramVariable(L"ConfigPreserveHappened",&gNvramPreserveStatusGuid);
            gRunTimeFlash = 1 ;
        }
    }
    return Status;
}           

/**
   	Function to calculate string length

	@param CHAR8 *
  	
	@retval	UINTN
*/ 
UINTN StrLen8(CHAR8 *string)
{
    UINTN i=0;
    while(string[i])
        i++;

    return i;
}

/**
   	Function to convert char8 to char16

	@param CHAR8 *
  	
	@retval	CHAR16 *
*/ 
CHAR16 *PldmStrDup8to16 (CHAR8 *string)
{
	CHAR16 		*text;
	UINTN 		i = 0;
	EFI_STATUS 	Status = EFI_SUCCESS;
	
	Status = gBS->AllocatePool(EfiBootServicesData, ((1 + StrLen8 (string)) * sizeof(CHAR16)),(void **)&text);
	
	if (EFI_ERROR (Status))
	{
		return NULL;
	}
	if ( text != NULL )
	{
		i=0;
		while(text[i] = (CHAR16)string[i])
			i++;
	}
	return text;
}

/**
   	Function to get PLDM capsule hob and to import it
   	
	@retval	EFI_STATUS
   	
*/ 
EFI_STATUS CapsulePldmPreserve (void)
{
	EFI_GUID 	HobListGuid = HOB_LIST_GUID;
	EFI_GUID    PldmCapsuleGuid = EFI_PLDM_CAPSULE_GUID;
	EFI_STATUS  Status = EFI_SUCCESS;
	EFI_HOB_CAPSULE_VOLUME *HobList = NULL;
	EFI_CAPSULE_HEADER     *CapsuleHeader = NULL;
	
	HobList = GetEfiConfigurationTable (gST, &HobListGuid);
	DEBUG((DEBUG_ERROR,"BiosConfigPreserve:CapsulePldmPreserve: HobList is %x\n", HobList));
	if (HobList == NULL)
	{
		return EFI_NOT_FOUND;
	}
	while (!EFI_ERROR(FindNextHobByType(EFI_HOB_TYPE_UEFI_CAPSULE, &HobList)))
	{
		DEBUG((DEBUG_ERROR, "BiosConfigPreserve:CapsulePldmPreserve: HobList->BaseAddress %x\n", HobList->BaseAddress));
		DEBUG((DEBUG_ERROR, "BiosConfigPreserve:CapsulePldmPreserve: HobList->Length %x\n", HobList->Length));
		
		CapsuleHeader = (EFI_CAPSULE_HEADER*) HobList->BaseAddress;
		
		DEBUG((DEBUG_ERROR, "BiosConfigPreserve:CapsulePldmPreserve: CapsuleHeader->CapsuleGuid %g\nCapsuleHeader->CapsuleImageSize %x\nCapsuleHeader->HeaderSize %x\nCapsuleHeader->Flags %x\n",
				CapsuleHeader->CapsuleGuid, CapsuleHeader->CapsuleImageSize, CapsuleHeader->HeaderSize, CapsuleHeader->Flags));
		
	    if (CompareGuid (&CapsuleHeader->CapsuleGuid, &PldmCapsuleGuid))
	    {
	    	DEBUG((DEBUG_ERROR, "BiosConfigPreserve:CapsulePldmPreserve: (((UINT8 *)HobList->BaseAddress) +  CapsuleHeader->HeaderSize) %x\n", (((UINT8 *)HobList->BaseAddress) +  CapsuleHeader->HeaderSize)));
	    	DEBUG((DEBUG_ERROR, "BiosConfigPreserve:CapsulePldmPreserve: (HobList->Length - CapsuleHeader->HeaderSize) %x\n", (HobList->Length - CapsuleHeader->HeaderSize)));
	    	
	    	if (FALSE == gUseNewImage)
            {
                //Chaging capsule preserve to runtime preserve
                ChangeUpdateModePreservingToRuntime (CAPSULE, (((UINT8 *)HobList->BaseAddress) +  CapsuleHeader->HeaderSize), (UINT32)(HobList->Length - CapsuleHeader->HeaderSize));
                gIsPreserveAborted = TRUE;
                return EFI_SUCCESS;
            }
	    	Status = CompareHashAndImportPldm ((((UINT8 *)HobList->BaseAddress) +  CapsuleHeader->HeaderSize), (UINT32)(HobList->Length - CapsuleHeader->HeaderSize));
	    	DEBUG((DEBUG_ERROR, "BiosConfigPreserve:CapsulePldmPreserve: Status for CompareHashAndImportPldm is %r\n", Status));
	    	if (!EFI_ERROR (Status))
	    	{
	    		return Status;
	    	}
	    }
	}
	return EFI_NOT_FOUND;
}

/**
   	Function to locate various file system and imports PLDM in recovery mode
   	
*/ 
void RecoveryPldmPreserve (void)
{
    EFI_STATUS 	Status = EFI_SUCCESS;
    UINTN   	NoHandles = 0;
    UINT32		Index = 0;
    EFI_HANDLE  *pHandleBuffer = NULL;
//    CHAR16 		*VolumeName = (CHAR16 *)NULL;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *Interface = (EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *)NULL;
    
    DEBUG((DEBUG_ERROR,"BiosConfigPreserve: In RecoveryPldmPreserve\n"));
    DEBUG((DEBUG_ERROR,"BiosConfigPreserve: FVDataAccessType is %d\n", FVDataAccessType()));
    if (!FVDataAccessType())
    {
        Status = gBS->LocateHandleBuffer (ByProtocol,&gEfiSimpleFileSystemProtocolGuid,NULL,&NoHandles,&pHandleBuffer);
		if(EFI_ERROR(Status))
		{
			return;
		}
	
		for (Index = 0; Index < NoHandles; Index++)
		{
			// get the protocol device instances..
			Status = gBS->HandleProtocol (pHandleBuffer[Index], &gEfiSimpleFileSystemProtocolGuid, &Interface);
			if(EFI_ERROR(Status))
			{
				continue;
				}
		
				Status = Apply_Pldm_Offset (pHandleBuffer[Index]) ;
		
			DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Status for Apply_Pldm_Offset is %r Index %x\n", Status, Index));
			if (!EFI_ERROR (Status))
			{
				break;
			}
/*			//Once AFU provides label name then uncomment this
			if(VolumeName)
			{
				gBS->FreePool(VolumeName);
				VolumeName = (CHAR16 *)NULL;
			}
			VolumeName = GetVolumeNameForFileSystemHandle(pHandleBuffer[Index]);
*/		
		}
		
		if(pHandleBuffer)
				gBS->FreePool(pHandleBuffer);
    }
    else
    {
        DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Apply from FV\n"));
        Status = Apply_Pldm_Offset_FV () ;
    }

    
/*									//Once AFU provides label name then uncomment this			
    if(VolumeName)
        gBS->FreePool(VolumeName);
*/
}

/**
   	Function to preserve the Pldm related variables. This function is child of OemBeforeFlashUpdateList.
   	
*/ 
void PreservePldmVariables (void)
{
	EFI_STATUS Status = EFI_SUCCESS ;
	EFI_STATUS FVStatus = EFI_SUCCESS ;
    UINT8 		*ImageData = NULL;
    UINTN 		ImageSize = 0;
    EFI_GUID 	PldmFilePathGuid = EFI_PLDM_FILE_PATH_GUID;
	EFI_GUID 	PldmHashGuid = EFI_PLDM_HASH_GUID;
	CHAR8		*FileName8 = NULL;
	EFI_BOOT_MODE BootMode = 0;
    EFI_GUID                            gEfiFirmwareVolumeBlockProtocolGuid = EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL_GUID;
    EFI_HANDLE                          *HandleBuffer;
    UINTN                               NumHandles;
    EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL  *FvBlock;
    EFI_PHYSICAL_ADDRESS                Address;
    UINTN                               i;
    ROM_INFO    gRomInfoImportData;
    void        *gRomDataImportLocation;
    void                                *FirstFile;
    void                                *EndOfFiles;
    void                                *File;
    BOOLEAN                             Flag = 0 ;
    UINT8 *PldmSignature = "PLDM" ;
    SCE_PLDM_DATA_HEADER *ScePldmFVData = NULL;
    UINTN FlashVolumeSize = 0;
	UINT8 *ReadData = NULL;

	BootMode = GetBootModeHob ();
	DEBUG((DEBUG_ERROR,"PreservePldmVariables: BiosConfigPreserve: BootMode is %x\n", BootMode));
	
	gHashSize = 0;
	
	if (!FVDataAccessType())
	{
		//For recovery also boot mode coming as flash update so removing the check
		//For capsule is not needed. So this case will be handled there
		//There won't be any filename for capsule mode
		//	if (BOOT_ON_FLASH_UPDATE != BootMode)			
		//{
		FileName8 = VarGetNvramName (L"PldmFileName", &PldmFilePathGuid, NULL, &gHashSize);
	    DEBUG((DEBUG_ERROR,"PreservePldmVariables: FileName8 is %a gHashSize is %x\n", FileName8, gHashSize));
		if (NULL != FileName8)
		{
			gPldmFileName = PldmStrDup8to16 (FileName8);
			DEBUG((DEBUG_ERROR,"PreservePldmVariables: gPldmFileName is %S\n", gPldmFileName));
			if (NULL == gPldmFileName)
			{
				return;
			}
			if (FileName8)
				gBS->FreePool (FileName8);
		}
    	gHashSize = 0;
    	gPldmHash = VarGetNvramName (L"PldmHash", &PldmHashGuid, NULL, &gHashSize);
    	DEBUG((DEBUG_ERROR,"PreservePldmVariables: gPldmHash address is %x gHashSize is %x\n", gPldmHash, gHashSize));
    	
    	gPldmNvramSize = 0;
    	gPldmNvram = NULL;
    	gPldmNvram = VarGetNvramName (L"PldmNvramData", &PldmFilePathGuid, NULL, &gPldmNvramSize);
        DEBUG((DEBUG_ERROR,"PreservePldmVariables: gPldmNvram address is %x gPldmNvramSize is %x\n", gPldmNvram, gPldmNvramSize));
	}
    else
	{
    	gHashSize = 0;
    	gPldmHash = VarGetNvramName (L"PldmHash", &PldmHashGuid, NULL, &gHashSize);
    	DEBUG((DEBUG_ERROR,"PreservePldmVariables: gPldmHash address is %x gHashSize is %x\n", gPldmHash, gHashSize));
    	
    	if( gHashSize == 0 )
    		return ;
    	
		gPldmNvramSize = 0;
    	gPldmNvram = NULL;
    	gPldmNvram = VarGetNvramName (L"PldmNvramData", &PldmFilePathGuid, NULL, &gPldmNvramSize);
        DEBUG((DEBUG_ERROR,"PreservePldmVariables: gPldmNvram address is %x gPldmNvramSize is %x\n", gPldmNvram, gPldmNvramSize));
        if (NULL != gPldmNvram) //If data is in NVRAM then try to import it. useful for when PcdUseNewImageOnFlashUpdate is FALSE
            return;

	    DEBUG((DEBUG_ERROR,"In PreservePldmVariables: (Apply_Pldm_Offset_FV success) %r\n", Status));
	    Status = pBS->LocateHandleBuffer(ByProtocol,
	                                    &gEfiFirmwareVolumeBlockProtocolGuid,
	                                    NULL,
	                                    &NumHandles,
	                                    &HandleBuffer);
	    DEBUG((DEBUG_ERROR,"In PreservePldmVariables: (LocateHandleBuffer success) %r\n", Status));
	    if (EFI_ERROR(Status)) return ;
	    
	    for (i = 0; i < NumHandles; ++i) {
	        Status = pBS->HandleProtocol(HandleBuffer[i],
	                                    &gEfiFirmwareVolumeBlockProtocolGuid,
	                                    (void **)&FvBlock);
	        DEBUG((DEBUG_ERROR,"In PreservePldmVariables: (FvBlock success) %r\n", Status));
	        if (EFI_ERROR(Status)) continue;

	        Status = FvBlock->GetPhysicalAddress(FvBlock, &Address);
	        DEBUG((DEBUG_ERROR,"In PreservePldmVariables: (GetPhysicalAddress success) %r\n", Status));

	        if (Status == EFI_SUCCESS) {
	            FirstFile = (UINT8*)Address
	                        + ((EFI_FIRMWARE_VOLUME_HEADER*)Address)->HeaderLength;

	            EndOfFiles = (UINT8*)Address
	                        + ((EFI_FIRMWARE_VOLUME_HEADER*)Address)->FvLength;

	            File = FindFile(&gFlashDataFile, FirstFile, EndOfFiles);
	            DEBUG((DEBUG_ERROR,"In PreservePldmVariables: (FindFile success) %x\n", File));
	            if (File) {
	                gRomInfoImportData = *(ROM_INFO*)File;
	                gRomDataImportLocation = (ROM_INFO*)File + 1;
	                Flag = 1 ;
	            }
	        }
			DEBUG((DEBUG_ERROR,"In PreservePldmVariables: (gRomInfoImportData.Size) %d\n", (UINT32)gRomInfoImportData.Size));
			
			if( Flag == 1 )
				break ;
	    }
	    if( Flag == 1 )		// Runtime flash
	    {
	        ScePldmFVData = (SCE_PLDM_DATA_HEADER *)gRomDataImportLocation;
	        DEBUG((DEBUG_ERROR,"PreservePldmVariables: %x, ScePldmFVData->Signature %x \n", gRomDataImportLocation, ScePldmFVData->Signature));
	    	DEBUG((DEBUG_ERROR,"In PreservePldmVariables: PLDM FFS found\n"));
	        if( ! MemCmp((((UINT8 *)gRomDataImportLocation)+4), (UINT8 *)PldmSignature, 4) )
	        {
	            gDataSize = gRomInfoImportData.Size;
	            pBS->AllocatePool(EfiBootServicesData, gDataSize, &gPldmRecData );
	            MemCpy(gPldmRecData, gRomDataImportLocation, gDataSize);
                DEBUG((DEBUG_ERROR,"In PreservePldmVariables: MemCmp %x %d \n", gPldmRecData, gDataSize));
	        }
	        else		// recovery flash
	        {
	        	DEBUG((DEBUG_ERROR,"PreservePldmVariables: In get FV\n"));
	        	Status = pBS->LocateProtocol (&gFlashProtocolGuid, NULL, &gPLDMFlash);	// Locating FLASH protocol
	        	
	        	if ( Status == EFI_SUCCESS )
	        	{
	        	    UINTN BcpFlashBlockSize = 0;
	        	    UINTN BcpFvOffset = FVBCPVolumeOffset();
	        	    void        *gPLDMFVLocation = (void *)( FLASH_DEVICE_FV_BASE + BcpFvOffset );
	        	    UINTN Blocks = 0;

	        	    DEBUG((DEBUG_ERROR,"In PreservePldmVariables: DeviceWriteEnable: entered\n"));

	        	    Status = gPLDMFlash->DeviceWriteEnable();
	        	    if (EFI_ERROR(Status)) return ;
	        	    DEBUG((DEBUG_ERROR,"PreservePldmVariables:DeviceWriteEnable Status = %r\n",Status));
					
                    FlashVolumeSize = FVBCPVolumeSize();
	        	    BcpFlashBlockSize = FVFlashBlockSize();
	        	    DEBUG((DEBUG_ERROR,"PreservePldmVariables:FlashVolumeSize = %d, BcpFlashBlockSize: %d\n",FlashVolumeSize, BcpFlashBlockSize));
	        	    ReadData = (UINT8 *)AllocateZeroPool(FlashVolumeSize);
                    if(ReadData)
                    {
                        for(Blocks = 0; Blocks < FlashVolumeSize; )
                        {
                            DEBUG((DEBUG_ERROR,"PreservePldmVariables:ReadAdd = %x, DataAdd: %x\n",((UINT8 *)gPLDMFVLocation + Blocks), (ReadData + Blocks)));
                            Status = gPLDMFlash->Read((VOID *)((UINT8 *)gPLDMFVLocation + Blocks), BcpFlashBlockSize, (VOID *)(ReadData + Blocks)); // Reading BCP data from BCP offset
                            Blocks += BcpFlashBlockSize;
                        }
                        DEBUG((DEBUG_ERROR,"In PreservePldmVariables: Read: %r\n", Status));
                        if( Status == EFI_SUCCESS )
                        {
                            DEBUG((DEBUG_ERROR,"In PreservePldmVariables: gPLDMFVLocation: %x\n", gPLDMFVLocation));
                            
                            if( ! MemCmp((((UINT8 *)ReadData) + BCP_PLDM_SIGNATURE_OFFSET), (UINT8 *)PldmSignature, 4) )	// Checking for valid signature PLDM data
                            {
                                gDataSize = FVFlashDataSize();
                                pBS->AllocatePool(EfiBootServicesData, gDataSize, &gPldmRecData );
                                MemCpy(gPldmRecData, (ReadData + BCP_FFS_DATA_START_OFFSET), gDataSize);
                                DEBUG((DEBUG_ERROR,"In PreservePldmVariables: Recovery MemCmp %x %d \n", gPldmRecData, gDataSize));
                            }
                        }
                        if(ReadData)
                        {
                            FreePool(ReadData);
                            ReadData = NULL;
                        }
                    }
	        	    Status = gPLDMFlash->DeviceWriteDisable() ;
	        	    DEBUG((DEBUG_ERROR,"PreservePldmVariables:DeviceWriteDisable Status = %r\n",Status));
	        	}
	        }
	    }
	    
	    if( HandleBuffer )
	    {
	        pBS->FreePool(HandleBuffer);
	    }
	}
	
}

/**
   	Function to import the Pldm tables in recovery and capsule mode.  This function is child of OemAfterFlashUpdateList.
   	
*/ 
void PldmImport (void)
{
    EFI_GUID 	PldmHashGuid = EFI_PLDM_HASH_GUID;
    EFI_GUID 	PldmFilePathGuid = EFI_PLDM_FILE_PATH_GUID;
    EFI_STATUS	Status = EFI_SUCCESS;
    UINTN		Size = 1;
    UINT8		NvramFlashFlag = 0;
    EFI_GUID	NvramFlashGuid = AFU_BCP_NVRAM_FLASH_GUID;
    EFI_BOOT_MODE BootMode;
    SCE_PLDM_DATA_HEADER *ScePldmFVData = NULL;
    UINT32 FVDataSize = 0 ;

	DEBUG((DEBUG_ERROR,"BiosConfigPreserve:PldmImport: In PldmImport\n"));
		
	gUseNewImage = PcdGetBool (PcdUseNewImageOnFlashUpdate);
	DEBUG((DEBUG_ERROR,"BiosConfigPreserve:PldmImport: gUseNewImage is %d\n", gUseNewImage));
	//commented below to support PcdUseNewImageOnFlashUpdate FALSE case
	//Sometime keeping this pcd to TRUE might assert(old PEI with new DXE)
/*	
	if (FALSE == UseNewImage)
	{
		DEBUG((DEBUG_ERROR,"BiosConfigPreserve:PldmImport: Please set PcdUseNewImageOnFlashUpdate pcd to 1 to preserve BIOS configuration\n"));
		ASSERT (0);
		return;
	}
*/	
    Status = pRS->GetVariable (L"NvramFlashed",
    			&NvramFlashGuid,
                NULL,
                &Size, 
                &NvramFlashFlag);
    DEBUG((DEBUG_ERROR,"BiosConfigPreserve:PldmImport: Status for NvramFlashed variable is  %r Size is %x\n", Status, Size));
    
    if (EFI_ERROR (Status))		//If NvramFlashed variable not found then only import Pldm
    {
    	//For recovery also boot mode coming as flash update so removing the check
    	//If pldmfilename is there then it will be recovery only
    	BootMode = GetBootModeHob ();
    	DEBUG((DEBUG_ERROR,"BiosConfigPreserve:PldmImport: BootMode is %x\n", BootMode));
    	
        if ((BootMode == BOOT_ON_FLASH_UPDATE) && (NULL != gPldmHash))         //Capsule needs only hash
        {
            DEBUG((DEBUG_ERROR,"BiosConfigPreserve:PldmImport:Call CapsulePldmPreserve\n"));
            gIsPreserveAborted =  FALSE;
            Status = CapsulePldmPreserve ();
            DEBUG((DEBUG_ERROR,"BiosConfigPreserve:PldmImport: Status for CapsulePldmPreserve is %r\n", Status));
            if (TRUE == gIsPreserveAborted)
            {
                goto BCPDONE;
            }
        }
        
        if (EFI_ERROR (Status) && FVDataAccessType() && (NULL != gPldmRecData) && (NULL != gPldmHash))	//Recovery needs both Received data and hash, flash through FV
    	{
            if (TRUE == gUseNewImage)
            {
                DEBUG((DEBUG_ERROR,"BiosConfigPreserve:PldmImport: In FVDataAccessType PldmImport gUseNewImage is TRUE\n"));
                RecoveryPldmPreserve ();
            }
            else
            {
                DEBUG((DEBUG_ERROR,"BiosConfigPreserve:PldmImport: gPldmRecData %x gDataSize %x\n", gPldmRecData, gDataSize));
                ScePldmFVData = (SCE_PLDM_DATA_HEADER *)gPldmRecData;
                FVDataSize = ScePldmFVData->MappingString + ScePldmFVData->MapStringSize;
                DEBUG((DEBUG_ERROR,"BiosConfigPreserve:PldmImport: FVDataSize is dec %d FVDataSize hex %x \n", FVDataSize, FVDataSize));
                ChangeUpdateModePreservingToRuntime (CAPSULE, gPldmRecData, FVDataSize);
                goto BCPDONE;
            }
        }
    	else if (/*(BootMode == BOOT_IN_RECOVERY_MODE) && */(NULL != gPldmFileName) && (NULL != gPldmHash))  //Recovery needs both file name and hash, flash through NVRAM
    	{
    		DEBUG((DEBUG_ERROR,"BiosConfigPreserve:PldmImport:Call RecoveryPldmPreserve\n"));
            if (TRUE == gUseNewImage)
            {
                RecoveryPldmPreserve ();
            }
            else
            {
                ChangeUpdateModePreservingToRuntime (RECOVERY, NULL, 0);
                goto BCPDONE;
            }
    	}
    }
    else
    {
		Status = VarSetNvramName (L"NvramFlashed",
									&NvramFlashGuid,
									EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
									NULL,
									0);
		DEBUG((DEBUG_ERROR,"BiosConfigPreserve:PldmImport: Status for deleting NvramFlashed is %r\n", Status));
    }
    
	Status = VarSetNvramName (L"PldmFileName",
									&PldmFilePathGuid,
									EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
									NULL,
									0);
	DEBUG((DEBUG_ERROR,"BiosConfigPreserve:PldmImport: Status for deleting PldmFileName is %r\n", Status));
    
	Status = VarSetNvramName (L"PldmHash",
								&PldmHashGuid,
								EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
								NULL,
								0);
	DEBUG((DEBUG_ERROR,"BiosConfigPreserve:PldmImport: Status for deleting PldmHash is %r\n", Status));

BCPDONE:
    if (gPldmHash)
        gBS->FreePool (gPldmHash);
    
    if (gPldmFileName)
        gBS->FreePool (gPldmFileName);
}

/**
  Delets all the pldm import related details
    
  @return EFI_STATUS
  @retval EFI_SUCCESS
**/

EFI_STATUS DeletePldmDetailsAndReset ()
{
    EFI_GUID    PldmHashGuid = EFI_PLDM_HASH_GUID;
    EFI_GUID    PldmFilePathGuid = EFI_PLDM_FILE_PATH_GUID;
    EFI_STATUS  Status = EFI_SUCCESS;
    
    Status = VarSetNvramName (L"PldmFileName",
                                    &PldmFilePathGuid,
                                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                                    NULL,
                                    0);
    DEBUG((DEBUG_ERROR,"BiosConfigPreserve:DeletePldmDetailsAndReset Status for deleting PldmFileName is %r\n", Status));
    if (gPldmFileName)  
        gBS->FreePool (gPldmFileName);
    
    Status = VarSetNvramName (L"PldmHash",
                                &PldmHashGuid,
                                EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                                NULL,
                                0);
    DEBUG((DEBUG_ERROR,"BiosConfigPreserve:DeletePldmDetailsAndReset Status for deleting PldmHash is %r\n", Status));
    if (gPldmHash)
        gBS->FreePool (gPldmHash);
    
    Status = VarSetNvramName (L"PldmNvramData",
                                    &PldmFilePathGuid,
                                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                                    NULL,
                                    0);
    
    DEBUG((DEBUG_ERROR,"BiosConfigPreserve:DeletePldmDetailsAndReset Status for deleting PldmNvramData is %r\n", Status));
    if (gPldmNvram)
        gBS->FreePool (gPldmNvram);

    if( gRunTimeFlash == 1 )
    {
        gRunTimeFlash = 0 ;
        gRT->ResetSystem (EfiResetWarm, EFI_SUCCESS, 0, NULL);
    }
    return EFI_SUCCESS;
}

/**
   	Function to import the Pldm tables in runtime mode.
   	
*/ 
void RuntimePldmImport (void)
{
    EFI_GUID 	PldmHashGuid = EFI_PLDM_HASH_GUID;
    EFI_GUID 	PldmFilePathGuid = EFI_PLDM_FILE_PATH_GUID;
    EFI_STATUS	Status = EFI_SUCCESS;
    UINTN		Size = 1;
    UINT8		NvramFlashFlag = 0;
    EFI_GUID	NvramFlashGuid = AFU_BCP_NVRAM_FLASH_GUID;
	
	DEBUG((DEBUG_ERROR,"BiosConfigPreserve:RuntimePldmImport: In RuntimePldmImport\n"));
	
    Status = pRS->GetVariable (L"NvramFlashed",
    			&NvramFlashGuid,
                NULL,
                &Size, 
                &NvramFlashFlag);
    DEBUG((DEBUG_ERROR,"BiosConfigPreserve:RuntimePldmImport: Status for NvramFlashed variable is  %r Size is %x\n", Status, Size));
    
    if (EFI_ERROR (Status))		//If NvramFlashed variable not found then only import Pldm
    {
		PreservePldmVariables ();
		
		DEBUG((DEBUG_ERROR,"BiosConfigPreserve:RuntimePldmImport: In gPldmHash is %x, gPldmFileName is %x gPldmRecData is %x\n", gPldmHash, gPldmFileName, gPldmRecData));
		if ( ((NULL != gPldmRecData) && (NULL != gPldmHash) && FVDataAccessType()) || ((NULL != gPldmHash) && (NULL != gPldmFileName)) )
		{
			RecoveryPldmPreserve ();
		}
		else if ((NULL != gPldmNvram) && (0 != gPldmNvramSize) && (NULL != gPldmHash) && (0 != gHashSize))
        {
            DEBUG((DEBUG_ERROR,"BiosConfigPreserve:RuntimePldmImport calling CompareHashAndImportPldm\n"));
            CompareHashAndImportPldm (gPldmNvram, (UINT32)gPldmNvramSize);
        }
    }
    else
    {
		Status = VarSetNvramName (L"NvramFlashed",
									&NvramFlashGuid,
									EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
									NULL,
									0);
		DEBUG((DEBUG_ERROR,"BiosConfigPreserve:RuntimePldmImport: Status for deleting NvramFlashed is %r\n", Status));
    }
    DeletePldmDetailsAndReset ();
}

/**
  Imports the pldm table based on previous boot flash updated mode.
  This will be used only when PcdUseNewImageOnFlashUpdate FALSE
    
  @param  void

  @return void
**/

void ImportUpdateModePreservingToRuntime (void)
{
/////Recovery import starts/////////
    
    PreservePldmVariables ();
    DEBUG((DEBUG_ERROR,"BiosConfigPreserve:ImportUpdateModePreservingToRuntime In gPldmHash is %x, gPldmFileName is %x\n", gPldmHash, gPldmFileName));
    if ((NULL != gPldmHash) && (NULL != gPldmFileName))
    {
        DEBUG((DEBUG_ERROR,"BiosConfigPreserve:ImportUpdateModePreservingToRuntime calling RecoveryPldmPreserve\n"));
        RecoveryPldmPreserve ();
        DeletePldmDetailsAndReset ();
    }
/////Recovery import ends/////////

/////Capsule import starts/////////    
    else
    {
        if ((NULL != gPldmNvram) && (0 != gPldmNvramSize) && (NULL != gPldmHash) && (0 != gHashSize))
        {
            DEBUG((DEBUG_ERROR,"BiosConfigPreserve:ImportUpdateModePreservingToRuntime calling CompareHashAndImportPldm\n"));
            CompareHashAndImportPldm (gPldmNvram, (UINT32)gPldmNvramSize);
            DeletePldmDetailsAndReset ();
        }
    }
/////Capsule import ends/////////
}

/**
  Sets the variable needed for changing recovery/capsule preserving to runtime preserving when PcdUseNewImageOnFlashUpdate 
  set to FALSE
    
  @param  FlashMode         Flash mode to act accordingly
  @param  Buffer            Capsule update pldm buffer to be saved into NVRAM
  @param  BufferSize        Capsule update pldm buffer size

  @return EFI_STATUS
  @retval EFI_SUCCESS for success case else other EFI_STATUS
**/

EFI_STATUS ChangeUpdateModePreservingToRuntime (FLASH_UPDATE_MODE FlashMode, UINT8 *Buffer, UINT32 BufferSize)
{
    UINT64      MaximumVariableStorageSize = 0;
    UINT64      RemainingVariableStorageSize = 0;
    UINT64      MaximumVariableSize = 0;
    EFI_GUID    PldmFilePathGuid = EFI_PLDM_FILE_PATH_GUID;
    EFI_GUID    PldmHashGuid = EFI_PLDM_HASH_GUID;
    EFI_STATUS  Status = EFI_SUCCESS;
    CHAR8       *TempPldmFileName = NULL;
    
    if (RECOVERY == FlashMode)
    {
        TempPldmFileName = StrDup16to8 (gPldmFileName);
        Status = VarSetNvramName (L"PldmFileName",
                                        &PldmFilePathGuid,
                                        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                                        TempPldmFileName,
                                        (StrLen8 (TempPldmFileName) + 1));

        if (TempPldmFileName)
            gBS->FreePool (TempPldmFileName);
        
        DEBUG((DEBUG_ERROR,"BiosConfigPreserve:ChangeUpdateModePreservingToRuntime Status for setting PldmFileName is %r\n", Status));
        
        //Deleting PldmNvramData to make sure consecutive capsule and recovery update doesn't screw up anything
        Status = VarSetNvramName (L"PldmNvramData",
                                        &PldmFilePathGuid,
                                        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                                        NULL,
                                        0);
        
    }
    else if (CAPSULE == FlashMode)
    {
     
        Status = gRT->QueryVariableInfo (
                        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                        &MaximumVariableStorageSize,
                        &RemainingVariableStorageSize,
                        &MaximumVariableSize
                        );
        if (EFI_ERROR (Status))
        {
            DEBUG((DEBUG_ERROR,"BiosConfigPreserve:ChangeUpdateModePreservingToRuntime Status for QueryVariableInfo %r\n", Status));
            DEBUG((DEBUG_ERROR,"BiosConfigPreserve:ChangeUpdateModePreservingToRuntime Can't preserve the settings\n"));
            return Status;
        }
        else if ((MaximumVariableSize < BufferSize) || (RemainingVariableStorageSize < BufferSize))
        {
            DEBUG((DEBUG_ERROR,"BiosConfigPreserve:ChangeUpdateModePreservingToRuntime Can't preserve the settings due to insufficient NVRAM size\n"));
            DEBUG((DEBUG_ERROR,"BiosConfigPreserve:ChangeUpdateModePreservingToRuntime MaximumVariableStorageSize %x, RemainingVariableStorageSize %x, MaximumVariableSize %x Pldmsize %x\n",
                    MaximumVariableSize, RemainingVariableStorageSize, MaximumVariableSize, BufferSize));
            return EFI_BUFFER_TOO_SMALL;
        }
        else
        {
            Status = VarSetNvramName (L"PldmNvramData",
                                            &PldmFilePathGuid,
                                            EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                                            Buffer,
                                            BufferSize);
            DEBUG((DEBUG_ERROR,"BiosConfigPreserve:ChangeUpdatePreserveToRuntime Status for setting PldmNvramData is %r\n", Status));
            
            //Deleting PldmFileName to make sure consecutive recovery and capsule update doesn't screw up anything
            Status = VarSetNvramName (L"PldmFileName",
                                            &PldmFilePathGuid,
                                            EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                                            NULL,
                                            0);
        }
    }
    Status = VarSetNvramName (L"PldmHash",
                                &PldmHashGuid,
                                EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                                gPldmHash,
                                gHashSize);
    DEBUG((DEBUG_ERROR,"BiosConfigPreserve:ChangeUpdatePreserveToRuntime Status for setting PldmHash is %r\n", Status));
    return EFI_SUCCESS;
}


//************************************************************************
//************************************************************************
//**                                                                  	**
//**        (C)Copyright 2016, American Megatrends, Inc.            	**
//**                                                                  	**
//**                       All Rights Reserved.                       	**
//**                                                                  	**
//**  5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093              **
//**                                                                  	**
//**                       Phone: (770)-246-8600                      	**
//**                                                                  	**
//************************************************************************
//************************************************************************
