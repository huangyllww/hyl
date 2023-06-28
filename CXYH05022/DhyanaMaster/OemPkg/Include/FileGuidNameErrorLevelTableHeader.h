#ifndef __FILE_GUID_NAME_ERRORLEVEL_TABLE_HEADER__H__
#define __FILE_GUID_NAME_ERRORLEVEL_TABLE_HEADER__H__

#include <Uefi/UefiBaseType.h>

typedef struct{
  EFI_GUID      Guid;
  UINT32        PrintErrorLevel;
} FILE_GUID_PRINT_ERROR_LEVEL_TABLE;

typedef struct{
  EFI_GUID      Guid;
  CHAR8         *Name;
} FILE_GUID_NAME_TABLE;

#endif
