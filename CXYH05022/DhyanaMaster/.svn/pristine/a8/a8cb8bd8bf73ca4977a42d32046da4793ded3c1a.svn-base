#ifndef _EFILIB_INCLUDE_
#define _EFILIB_INCLUDE_

#include "Protocol/SimpleFileSystem.h"
//EFI_SYSTEM_TABLE         *ST;
//EFI_BOOT_SERVICES        *BS;
//EFI_RUNTIME_SERVICES     *RT;

#include "stdio.h"

EFI_STATUS
WaitForSingleEvent (
    IN EFI_EVENT        Event,
    IN UINT64           Timeout OPTIONAL
    );

UINTN
EFIAPI
SCE_StrLen (
  IN      CONST CHAR16              *String
  );

char *
SCE_fgets(char *buf, int n, FILE *fp);

//#define MemCpy memcpy
//#define MemSet memset

//VOID *memcpy(VOID* pDestination, const VOID* pSource, size_t Count);
//VOID *memset( VOID *buffer, int value, size_t size );

VOID GetArgumentsFromImage(EFI_HANDLE ImageHandle, CHAR16 ***pArgv, UINTN *pArgc);

EFI_STATUS AMI_SCE_FindExecDirPath(EFI_HANDLE ImageHandle, CHAR16 *DirBuffer, INTN MaxDirBufferLen);
EFI_HANDLE AMI_SCE_GetFileSystemHandle(INTN fsindex);

INTN AMI_SCE_FileSize( EFI_FILE_HANDLE 	File, UINT64 *fsize );
INTN AMI_SCE_FileRead(EFI_FILE_HANDLE File, INTN BufferSize, CHAR8 **FileBuffer);

int ReadLinefromBuffer(char *buf, int n, CHAR8 *fp);

#endif	//_EFILIB_INCLUDE_
