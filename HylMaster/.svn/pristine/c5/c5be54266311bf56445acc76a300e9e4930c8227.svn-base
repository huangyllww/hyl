//*****************************************************************
//*****************************************************************
//**                                                             **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.    **
//**                                                             **
//**                       All Rights Reserved.                  **
//**                                                             **
//**    5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093 **
//**                                                             **
//**                       Phone (770)-246-8600                  **
//**                                                             **
//*****************************************************************
//*****************************************************************

/** @file FwConfigFileOperation.c
    Contains implementation of file operations

**/

//---------------------------------------------------------------------------

#include <Include/AmiDxeLib.h>
#include <Library/BaseLib.h>
#include <Token.h>
#include <Protocol/SimpleFileSystem.h>
#include <Library/MemoryAllocationLib.h>
#include <JsonUtilityLib.h>
#include <Library/UefiLib.h>

//---------------------------------------------------------------------------

VOID MemFreePointer( VOID **ptr );
void WriteChar8ToFile (EFI_FILE_PROTOCOL *FilePtr,UINTN BufSize, CHAR8 *tempBuffer);
CHAR8* ConvertChar16toChar8(CHAR16 *InputString16);

/**
    Convert CHAR16 String to CHAR8 string

    @param CHAR16 *InputString16

    @retval CHAR8*
**/
CHAR8* ConvertChar16toChar8(CHAR16 *InputString16)
{
	CHAR8 *OutString8 = (CHAR8 *)NULL;
	UINTN Strlen16 = 0;
	UINTN Index = 0;
	
    if ((NULL == InputString16) || (*InputString16 == L'\0'))
    {
        return (CHAR8*)NULL;
    }

	Strlen16 = StrLen(InputString16);
	OutString8 = AllocateZeroPool((Strlen16+1) * sizeof(CHAR8));

	while(InputString16[Index] != L'\0')
	{
		OutString8[Index] = (CHAR8)InputString16[Index];
		Index++;
	}

	OutString8[Index] = '\0';
	return OutString8;
}

/**
    Create Json file in local directory

    @param CHAR16 *InputString16

    @retval EFI_FILE_PROTOCOL
**/

EFI_FILE_PROTOCOL *CreateJsonFile(CHAR16 *FileName)
{
	EFI_FILE_PROTOCOL 	*FileProtocol = NULL;
	EFI_FILE_PROTOCOL 	*Root;
	EFI_STATUS 			Status = EFI_SUCCESS;
	UINTN 				FSHandleCount = 0;
	UINTN 				Index = 0;
	EFI_HANDLE 			*FSBuffer;
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSysProtocol =  NULL;
	EFI_GUID 		SimpleFileSystemGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
	
	Status = pBS->LocateHandleBuffer(ByProtocol, &SimpleFileSystemGuid, NULL, &FSHandleCount, &FSBuffer);
	if ( !EFI_ERROR(Status))
	{
		for ( Index = 0; Index < FSHandleCount; Index++ )
		{
		    Status = pBS->HandleProtocol( FSBuffer[Index], &SimpleFileSystemGuid, &FileSysProtocol );
		    if ( EFI_ERROR( Status ) )
		   	 continue;
		    Status = FileSysProtocol->OpenVolume(FileSysProtocol,&Root);
		    if ( EFI_ERROR( Status ) )
		   	 continue;
		    //check whether the file with same name exists
		    Status = Root->Open(Root,&FileProtocol,FileName, EFI_FILE_MODE_WRITE | EFI_FILE_MODE_READ, 0);
		    if ( !EFI_ERROR( Status ) )
		    {
		   	 //if exists, delete it and create again
		    	Root->Delete(FileProtocol);
		    }
		    Status = Root->Open(Root,&FileProtocol,FileName, EFI_FILE_MODE_CREATE| EFI_FILE_MODE_WRITE | EFI_FILE_MODE_READ, 0);
		    if ( EFI_ERROR( Status ) )
		   	 continue;
		    break;
		}
	}
		return FileProtocol;
}

/**
    write CHAR8 string to file

    @param EFI_FILE_PROTOCOL *FilePtr,UINTN BufSize, CHAR8 *tempBuffer

    @retval void
**/

void WriteChar8ToFile (EFI_FILE_PROTOCOL *FilePtr,UINTN BufSize, CHAR8 *tempBuffer)
{
    UINTN Size = 0;
    

    Size = BufSize;
    if (FilePtr)
    {
        FilePtr->Write (FilePtr,&Size,tempBuffer);
    }
}

void Dump_ContentToFile(BOOLEAN IsUnicodeChar,CONST CHAR16 *Filenamme,CONST VOID *String,UINTN Size)
{

	CHAR8 *String8 = NULL;
	EFI_FILE_PROTOCOL *JsonFilePtr = (EFI_FILE_PROTOCOL*)NULL;
	if((NULL != String) &&(Filenamme))
	{
		JsonFilePtr = CreateJsonFile (Filenamme);
		if(NULL != JsonFilePtr)
		{
			if(TRUE == IsUnicodeChar)
				String8 = ConvertChar16toChar8(String);
			else
				String8 = String;
		
			if(NULL != String8)
			{
				WriteChar8ToFile(JsonFilePtr,Size,String8);
//				if(TRUE == IsUnicodeChar)
//					MemFreePointer(&String8);
			}		
			JsonFilePtr->Close(JsonFilePtr);
			JsonFilePtr = NULL;
		}
	}

}
//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:   GetFileSize()
//
// Description: Gets the size of the File
//
// Parameter:   CHAR16 *, UINT64 *
//
// Return Value: INTN
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>
INTN GetFileSize( EFI_FILE_HANDLE   File, UINT64 *fsize )
{
    EFI_STATUS  Status = (EFI_STATUS) - 1;
    EFI_FILE_INFO *FileInfo;
    UINTN               FileInfoSize;
    EFI_GUID        infotype = EFI_FILE_INFO_ID;

    FileInfoSize = sizeof(EFI_FILE_INFO) + 1024;
    *fsize = 0;

    if ( (FileInfo = AllocatePool (FileInfoSize)) == NULL )
    {
        return 1;
    }

    Status = File->GetInfo(File, &infotype, &FileInfoSize, FileInfo);

    if (!EFI_ERROR(Status))
    {
        *fsize = FileInfo->FileSize;
    }

    FreePool(FileInfo);

    if (Status)
    {
        return EFI_INVALID_PARAMETER;
    }

    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:   ReadFileData()
//
// Description: Reads the data for the given file handle.
//
// Parameter:   EFI_FILE_HANDLE ,INTN ,VOID *
//
// Return Value: INTN
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>
INTN ReadFileData(EFI_FILE_HANDLE File, INTN BufferSize, CHAR8 **FileBuffer)
{
    UINTN size = BufferSize;
    EFI_STATUS      Status;
    VOID *Buffer;

    Buffer = (VOID *) AllocateZeroPool(BufferSize + 1);

    if (Buffer == NULL)
    {
        return 0;
    }

    if ( File )
    {
        Status = File->Read(File, &size, Buffer);

        if ( Status != EFI_SUCCESS )
        {
            size = 0;
        }
    }
    else
    {
        size = 0;
    }

    *FileBuffer = Buffer;
    return (INTN)size;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:   Get_ContentFromFile()
//
// Description: Opens and gets the content from the file
//
// Parameter:   CHAR16 *FileName, CHAR8 **ImageData, UINT32 *ImageSize
//
// Return Value: EFI_STATUS
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS Get_ContentFromFile(CHAR16 *FileName, CHAR8 **FileData, UINT32 *FileSize)
{
    EFI_FILE_HANDLE fileHandle = NULL;
    CHAR16 strpath[256];
    UINT64 ReadMode = EFI_FILE_MODE_READ;
    EFI_GUID    fsprotcol = SIMPLE_FILE_SYSTEM_PROTOCOL;
    EFI_GUID    FileSystemProtocol = SIMPLE_FILE_SYSTEM_PROTOCOL;
    EFI_FILE_IO_INTERFACE *Volume;
    UINTN HandleCount, i = 0;
    EFI_HANDLE *Handle;
    EFI_HANDLE fsHandle;
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_FILE_HANDLE File = NULL;
    EFI_FILE_HANDLE dirHandle = (EFI_FILE_HANDLE)NULL;
    CHAR16 StringTablePath[256] = {0};

    Status = pBS->LocateHandleBuffer(ByProtocol, &fsprotcol, NULL, &HandleCount, &Handle);

    if (EFI_ERROR(Status))
    {
        return Status;
    }

    Swprintf(strpath, (CHAR16 *)L"\\%s", (CHAR16 *)L"https");

    for (i = 0; i < HandleCount; i++)
    {
        fsHandle = Handle[i];
        Status = pBS->HandleProtocol (fsHandle, &FileSystemProtocol, (void**)&Volume);

        if (!EFI_ERROR(Status))
        {
            Status = Volume->OpenVolume(Volume, &File);

            if (File != NULL)
            {
                Status = File->Open( File, &fileHandle, strpath, ReadMode, 0 );

                if (Status != EFI_SUCCESS)
                {
                    continue;
                }
                else
                {
                    dirHandle = File;
                    break;
                }
            }
        }
    }
    
    if(dirHandle != NULL)
    {
        Swprintf(StringTablePath, L"\\%s", FileName);
        
        Status = dirHandle->Open( dirHandle, &fileHandle, StringTablePath, ReadMode, 0 );
    
        if (Status == EFI_SUCCESS)
        {
            INTN SizeReturned = 0;
            UINT64 FileBufferSize = 0;
            CHAR8 *FileBuffer = (CHAR8 *)NULL;
            GetFileSize(fileHandle, &FileBufferSize);
    
            FileBuffer = (CHAR8 *)AllocateZeroPool((UINTN)FileBufferSize + 1);
    
            if (FileBuffer == NULL)
            {
                return 0;
            }
    
            SizeReturned = ReadFileData(fileHandle, (INTN)FileBufferSize, &FileBuffer);
    
            *FileData = FileBuffer;
            *FileSize = (UINT32)FileBufferSize;
    
            if (fileHandle)
            {
                fileHandle->Close(fileHandle);
            }
        }
        else
        {
            Status = EFI_NOT_FOUND;
        }
        
        if (dirHandle)
        {
            dirHandle->Close( dirHandle );
            dirHandle = (EFI_FILE_HANDLE)NULL;
        }
    }
    else
    {
        Status = EFI_NOT_FOUND;
    }
    
    return Status;
}

//*****************************************************************
//*****************************************************************
//**                                                             **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.    **
//**                                                             **
//**                       All Rights Reserved.                  **
//**                                                             **
//**    5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093 **
//**                                                             **
//**                       Phone (770)-246-8600                  **
//**                                                             **
//*****************************************************************
//*****************************************************************
