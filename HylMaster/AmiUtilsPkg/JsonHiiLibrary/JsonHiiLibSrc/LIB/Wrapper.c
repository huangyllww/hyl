//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2013, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**    5555 Oakbrook Pkwy, Norcross, Suite 200, Georgia 30093   **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

//**********************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//**********************************************************************
// Revision History
// ----------------
// $Log: $
//
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  		Wrapper.c
//
// Description:	 Defines Wrapper functions used by PLDM Library
//
//<AMI_FHDR_END>

#include "Efi.h"
#include "string.h"
#include "wchar.h"
#include "stdlib.h"
#include  <errno.h>
#include <assert.h>
#include "Utilities.h"
#include "Wrapper.h"
#include "Protocol/SimpleFileSystem.h"
#include "Guid/FileInfo.h"
#include "os_hook.h"
#include "Protocol/LoadedImage.h"
#include "Library/MemoryAllocationLib.h"

//extern wchar_t* UnicodeRTrim(wchar_t *StrKey);
extern EFI_BOOT_SERVICES       *gBS;

EFI_STATUS
WaitForSingleEvent (
    IN EFI_EVENT        Event,
    IN UINT64           Timeout OPTIONAL
    )
{
    EFI_STATUS          Status;
    UINTN               Index;
    EFI_EVENT           TimerEvent;
    EFI_EVENT           WaitList[2];

    if (Timeout) {
        //
        // Create a timer event
        //

        Status = gBS->CreateEvent (EVT_TIMER, 0, NULL, NULL, &TimerEvent);
        if (!EFI_ERROR(Status)) {

            //
            // Set the timer event
            //

        	gBS->SetTimer (TimerEvent, TimerRelative, Timeout);

            //
            // Wait for the original event or the timer
            //

            WaitList[0] = Event;
            WaitList[1] = TimerEvent;
            Status = gBS->WaitForEvent (2, WaitList, &Index);
            gBS->CloseEvent (TimerEvent);

            //
            // If the timer expired, change the return to timed out
            //

            if (!EFI_ERROR(Status)  &&  Index == 1) {
                Status = EFI_TIMEOUT;
            }
        }

    } else {

        //
        // No timeout... just wait on the event
        //

        Status = gBS->WaitForEvent (1, &Event, &Index);
        assert (!EFI_ERROR(Status));
        assert (Index == 0);
    }

    return Status;
}

/*#pragma function(memcpy)
VOID *memcpy(VOID* pDestination, const VOID* pSource, size_t Count)
{
	gBS->CopyMem (pDestination, (void *)pSource, Count);
	return 0;
}

#pragma function(memset)
VOID *memset( VOID *buffer, int value, size_t size )
{
	gBS->SetMem( buffer, size, (UINT8)value );
	return 0;
}*/

VOID GetArgumentsFromImage(EFI_HANDLE ImageHandle, CHAR16 ***pArgv, UINTN *pArgc)
{
    EFI_STATUS	status;
    EFI_LOADED_IMAGE	*image;
    EFI_GUID	loadedImageProtocol = LOADED_IMAGE_PROTOCOL;
    CHAR16	*cmdLine;
//    CHAR16	*cmdLineString;
    UINTN	argc = 0, count;

    status = gBS->HandleProtocol( ImageHandle, &loadedImageProtocol, &image );

    if ( ! EFI_ERROR(status) )
    {
		UINTN	i;

		if ( image->LoadOptions == NULL )
		{
			*pArgv = AllocatePool (2 * sizeof(CHAR16 *));
		    if ( *pArgv != NULL )
		    {
				(*pArgv)[0] = (CHAR16 *)"AMISCE";
				(*pArgv)[1] = NULL;
		    }
		    *pArgc = 1;
		    return;
		}

		cmdLine = (CHAR16 *)image->LoadOptions;
//		cmdLineString = UnicodeRTrim(cmdLine);
//		count = strlen ((const char *)cmdLine);
		count = wcslen(cmdLine);
		//count = image->LoadOptionsSize / sizeof(CHAR16);
		for ( i = 0; i < count; i++ )
		{
		    if ( cmdLine[i] == L' ' )
		    {
				cmdLine[i] = L'\0';
				argc++;
		    }
		}

		*pArgv = AllocatePool ((argc + 1) * sizeof (CHAR16 *));
		if ( *pArgv != NULL )
		{
		    for ( i = 0; i < argc; i++ )
		    {
				(*pArgv)[i] = cmdLine;
				cmdLine += wcslen (cmdLine) + 1;
		    }

		    (*pArgv)[i] = NULL;
		}
    }

    *pArgc = argc;

//	FreePool( cmdLine );
}

int
ReadLinefromBuffer(char *buf, int n, CHAR8 *fp)//, INTN Size)
{
	int i;
	int c;
	int done = 0;
	if (buf == 0 || n <= 0 || fp == 0)
		return 0;
	for (i = 0; !done && i < n - 1; i++) {
		c = fp[i];
		if (c == EOF) {
		done = 1;
		i--;
		} else {
		buf[i] = (char)c;
		if (c == '\n')
			done = 1;
		}
	}
	buf[i] = '\0';
	if (i == 0)
		return 0;
	else
		return i;
}

/**
  Returns the length of a Null-terminated Unicode string.

  This function returns the number of Unicode characters in the Null-terminated
  Unicode string specified by String.

  If String is NULL, then ASSERT().
  If PcdMaximumUnicodeStringLength is not zero, and String contains more than
  PcdMaximumUnicodeStringLength Unicode characters, not including the
  Null-terminator, then ASSERT().

  @param  String  A pointer to a Null-terminated Unicode string.

  @return The length of String.

**/
UINTN
EFIAPI
SCE_StrLen (
  IN      CONST CHAR16              *String
  )
{
  UINTN                             Length;

  assert (String != NULL);

  for (Length = 0; *String != L'\0'; String++, Length++) {
    //
    // If PcdMaximumUnicodeStringLength is not zero,
    // length should not more than PcdMaximumUnicodeStringLength
    //
//    if (PcdGet32 (PcdMaximumUnicodeStringLength) != 0) {
   //   ASSERT (Length < PcdGet32 (PcdMaximumUnicodeStringLength));
    //}
  }
  return Length;
}

char *
SCE_fgets(char *buf, int n, FILE *fp)
{
	int i;
	int c;
	int done = 0;
	if (buf == 0 || n <= 0 || fp == 0)
		return 0;
	for (i = 0; !done && i < n - 1; i++) {
		c = fgetc(fp);
		if (c == EOF) {
		done = 1;
		i--;
		} else {
		buf[i] = (char)c;
		if (c == '\n')
			done = 1;
		}
	}
	buf[i] = '\0';
	if (i == 0)
		return 0;
	else
		return buf;
}

/*char *
SCE_fgets(char *buf, int n, FILE *fp)
{
register int c;
       register char *cs;

       cs = buf;
       while (--n > 0 && (c = getc(fp)) != EOF)
           if ((*cs++ = c) == '\n')
               break;
       *cs = '\0';
       return (c == EOF && cs == buf) ? NULL : buf;
}*/

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	AMI_SCE_GetFileSystemHandle()
//
// Description:	Entry point for AMI_SCE_GetFileSystemHandle Function.
//
// Parameter:	INTN
//
// Return Value: EFI_HANDLE
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>

//#define SIMPLE_FILE_SYSTEM_PROTOCOL \
//    { 0x964e5b22, 0x6459, 0x11d2, 0x8e, 0x39, 0x0, 0xa0, 0xc9, 0x69, 0x72, 0x3b }

EFI_HANDLE AMI_SCE_GetFileSystemHandle(INTN fsindex)
{
	EFI_HANDLE	*Handle,Hnd;
	UINTN		HandleCount,i,Index=0;
	EFI_GUID 	fsprotcol = SIMPLE_FILE_SYSTEM_PROTOCOL;
	UINT8	       *data;
	EFI_STATUS 	Status;
	EFI_GUID DevicePathProtocol 			= DEVICE_PATH_PROTOCOL;

	Status = gBS->LocateHandleBuffer(ByProtocol, &fsprotcol, NULL, &HandleCount, &Handle);
	if (EFI_ERROR(Status)) {
		return NULL;
	}
	else
	{
		if(HandleCount)
		{
			for(i=0;i<HandleCount;i++)
			{
				Status = gBS->HandleProtocol(Handle[i], &DevicePathProtocol, (VOID*)&data);
				if (!EFI_ERROR(Status)) {
					if((INTN)Index==fsindex)
					{
						Hnd = Handle[i];
						gBS->FreePool(Handle);
						return Hnd;
					}
					Index++;
				}
			}
			gBS->FreePool(Handle);
		}
	}
	return NULL;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure	: DevicePathToStr
//
// Description	: Giving the input device path as string
//
// Parameter	: DevPath 	- Input device path
//
// Return Value: CHAR16* 	- Out put device path string
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
/*
CHAR16 *
DevicePathToStr (
    EFI_DEVICE_PATH         *DevPath
    )
{
    POOL_PRINT          Str;
    EFI_DEVICE_PATH     *DevPathNode;
    VOID                (*DumpNode)(POOL_PRINT *, VOID *);
    UINTN               Index, NewSize;

    if( DevPath == NULL )
    {
        return NULL;
    }

    ZeroMem(&Str, sizeof(Str));
    DevPath = UnpackDevicePath(DevPath);
    ASSERT (DevPath);
    DevPathNode = DevPath;
    while (!IsDevicePathEnd(DevPathNode)) {
        DumpNode = NULL;
        for (Index = 0; DevPathTable[Index].Function; Index += 1) {

            if (DevicePathType(DevPathNode) == DevPathTable[Index].Type &&
                DevicePathSubType(DevPathNode) == DevPathTable[Index].SubType) {
                DumpNode = DevPathTable[Index].Function;
                break;
            }
        }
        if (!DumpNode) {
            DumpNode = _DevPathNodeUnknown;
        }
        if (Str.len  &&  DumpNode != _DevPathEndInstance) {
            CatPrint (&Str, L"/");
        }
        DumpNode (&Str, DevPathNode);
        DevPathNode = NextDevicePathNode(DevPathNode);
    }
    FreePool (DevPath);
    NewSize = (Str.len + 1) * sizeof(CHAR16);
    Str.str = ReallocatePool (Str.str, NewSize, NewSize);
    Str.str[Str.len] = 0;
    return Str.str;
}*/

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	AMI_SCE_FindExecDirPath()
//
// Description:	Entry point for AMI_SCE_FindExecDirPath Function.
//
// Parameter:	EFI_HANDLE ,CHAR16 *,INTN
//
// Return Value: EFI_STATUS
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS AMI_SCE_FindExecDirPath(EFI_HANDLE ImageHandle, CHAR16 *DirBuffer, INTN MaxDirBufferLen)
{
    EFI_LOADED_IMAGE	    *image;
    EFI_GUID		    protocol = LOADED_IMAGE_PROTOCOL;
    INTN		    ii, jj;
    CHAR16		    dirbuf[256];//, *pDevStr;
    EFI_STATUS		    Status;
    EFI_HANDLE		    handle;
    //UINTN		    stsize = sizeof(dirbuf);

    DirBuffer[0] = 0;

    if( ImageHandle == NULL )
		return EFI_INVALID_PARAMETER;

    Status = gBS->HandleProtocol( ImageHandle, &protocol, &image );

    if ( !EFI_ERROR( Status ) )
    {
		ii = 0;
		handle = AMI_SCE_GetFileSystemHandle(ii);
	if ( handle != image->DeviceHandle )
	for ( ii = 1 ; handle ; ii++ )
	{
		handle = AMI_SCE_GetFileSystemHandle(ii);
	    if ( handle == image->DeviceHandle )
		break;
	}
	if ( handle )
	{
	    //pDevStr = DevicePathToStr( image->FilePath );
	    swprintf(dirbuf,sizeof(dirbuf), L"fs%d:\\%ls", (int)ii,L"Test.txt");
	    for ( jj = 2 ; dirbuf[jj] ; jj++ )
	    {
		if ( dirbuf[jj] == L'/' )
		{
		    dirbuf[jj] = L'\0';
		    if ( dirbuf[jj-1] != L'\\' )
			wcscat( dirbuf, L"\\" );
		    break;	    // execute in current direcory
		}
	    }

	    for ( jj = wcslen(dirbuf)-1 ; jj > 0 ; jj-- )
	    {
		if ( dirbuf[jj] == L'\\' || dirbuf[jj] == L':' )
		    break;
	    }
	    dirbuf[jj+1] = 0;

	    wcsncpy(DirBuffer, dirbuf, MaxDirBufferLen);

	    return EFI_SUCCESS;
	}
    }
    return EFI_NOT_FOUND;
}


//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	AMI_SCE_FileSize()
//
// Description:	Entry point for AMI_SCE_FileSize Function.
//
// Parameter:	CHAR16 *, UINT64 *
//
// Return Value: INTN
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>

INTN AMI_SCE_FileSize( EFI_FILE_HANDLE 	File, UINT64 *fsize )
{
    EFI_STATUS	    	Status=(EFI_STATUS)-1;
    EFI_FILE_INFO  		*FileInfo;
    UINTN	    		FileInfoSize;
    EFI_GUID			infotype = EFI_FILE_INFO_ID;

    FileInfoSize = sizeof(EFI_FILE_INFO) + 1024;
    *fsize = 0;
    if ( (FileInfo = AllocatePool (FileInfoSize)) == NULL )
		return ERROR_OUTOFMEMORY;
    //if ( File = AMI_SCE_FileOpen(FileName,EFI_FILE_MODE_READ) )
    {
		Status = File->GetInfo(File,&infotype,&FileInfoSize,FileInfo);
		if (!EFI_ERROR(Status))
	 	   	*fsize = FileInfo->FileSize;
		//File->Close(File);
    }
    FreePool(FileInfo);
    if ( Status )
	return EFI_INVALID_PARAMETER;

    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	AMI_SCE_FileRead()
//
// Description:	Reads the data for the given file handle.
//
// Parameter:	EFI_FILE_HANDLE ,INTN ,VOID *
//
// Return Value: INTN
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>

INTN AMI_SCE_FileRead(EFI_FILE_HANDLE File, INTN BufferSize, CHAR8 **FileBuffer)
{
    UINTN size = BufferSize;
    EFI_STATUS		Status;
    VOID *Buffer;
    //CHAR16 *TempBuffer;

    Buffer = (VOID *) calloc (BufferSize + 1, sizeof(VOID *));

    if (Buffer == NULL)
     return 0;

	//TempBuffer = (CHAR16 *) calloc (BufferSize + 1, sizeof(CHAR16));
    if ( File )
    {
	Status = File->Read(File,&size,Buffer);
	if ( Status != EFI_SUCCESS )
	    size = 0;
    }
    else
	size = 0;

	//mbstowcs(TempBuffer, Buffer, size);

	*FileBuffer = Buffer;
    return (INTN)size;
}

//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2013, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**    5555 Oakbrook Pkwy, Norcross, Suite 200, Georgia 30093   **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
