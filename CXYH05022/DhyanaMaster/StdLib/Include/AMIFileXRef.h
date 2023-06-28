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


#ifndef _FOPENTEST_H_
#define _FOPENTEST_H_

#include "stdlib.h"
#include "wchar.h"
#include "string.h"

#include "stdio.h"
#include "Token.h"
#include "Include/Protocol/SimpleFileSystem.h"

#include "Library/UefiLib.h"
#include  <MainData.h>
#include  <errno.h>
#include  <unistd.h>
#include  <sys/types.h>
#include "Protocol/HiiDatabase.h"
#include "Library/MemoryAllocationLib.h"
#include <Protocol/LoadedImage.h>
#include <Guid/FileInfo.h>
#if MINI_LIB_SUPPORT == 0
#include <Protocol/EfiShellInterface.h>
#include <Protocol/EfiShellEnvironment2.h>
#include <Protocol/EfiShell.h>
#endif

/** Currently unused
extern EFI_SHELL_INTERFACE *mEfiShellInterface;
extern EFI_SHELL_ENVIRONMENT2        *mEfiShellEnvironment2;


#define ALL_SHELL_ASSIGNED (mEfiShellInterface != NULL && mEfiShellEnvironment2 != NULL && gEfiShellProtocol !=NULL)
#define IS_MEFISHELLINTERFACE_VALID (mEfiShellInterface !=NULL)
#define IS_MEFISHELLENVIRONMENT2_VALID (mEfiShellEnvironment2 !=NULL)
**/

#define READ_FROM_FV_ONLY		1
#define READ_FROM_FS_ONLY		2
#define PREFER_FV_THEN_FS		3
#define PREFER_FS_THEN_FV		4
#define INVALID_FILE_DESC			-1
#define INVALID		INVALID_FILE_DESC	
#define FREE_TO_USE		INVALID_FILE_DESC	

extern BOOLEAN gFWDescriptorsInitialized;

#define NULL_GUID \
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }
BOOLEAN IsShellProtocolAvailable();
BOOLEAN IsAFirmwareFile(char *FileName);
int GetFreeFD();
BOOLEAN IsAValidFWMode(char *Mode);
UINT8 AmiStrICaseCmp (char *Buffer1,char *Buffer2);
char AmiCharToUppercase(char Chr);
EFI_STATUS InitializeFWDescriptors();
int FVfopen(FILE *fp,char *file);
int
fseek_FV(FILE *fp, off_t offset, int whence);
EFI_GUID GetGUIDWithFVName(char *SearchFile);
BOOLEAN StdLibCompareGuid(IN EFI_GUID *Guid1, IN EFI_GUID *Guid2);
EFI_STATUS
ReadFromFVWithGuid ( IN EFI_GUID *FileNameGuid, OUT VOID **Image, OUT UINTN *ImageSize );
EFI_STATUS CopyFDToFWDescriptors(int FILEDes);
BOOLEAN IsAFirmwareDescriptor(int FILEDes);
EFI_STATUS ReleaseFWDescriptor(int FILEDes);

#endif	// #ifndef _FOPENTEST_H_

//**********************************************************************
//**********************************************************************
//**                                                                  						   **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.       **
//**                                                                  						   **
//**                       All Rights Reserved.                       				   **
//**                                                                  						   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093   **
//**                                                                  						   **
//**                       Phone: (770)-246-8600                      			   **
//**                                                                  						   **
//**********************************************************************
//**********************************************************************
