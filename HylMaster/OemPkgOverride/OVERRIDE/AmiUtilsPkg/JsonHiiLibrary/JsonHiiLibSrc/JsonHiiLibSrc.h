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

/** @file HiiDatabaseLib.h

**/

//---------------------------------------------------------------------------
#pragma once
#pragma pack(push, 1)

#include "UEFI20HIIParser.h"
#include "types.h"

typedef struct
{
    UINT32  Active;
    UINT16  PathLength;
    CHAR16  Name[1];
    //EFI_DEVICE_PATH_PROTOCOL  DevicePath;
    //UINT8 LoadOptions[];
}
BOOT_OPTION;
#pragma pack(pop)
#define RESERVE_SIZE 2048
#define MEMORY_ALLOC_UNIT           2048

typedef struct _DATAVECTOR
{
    CHAR16 *_String;
    CHAR8 *_String8;
    UINTN   _SizeUsed;
    UINTN   _SizeFree;
    UINTN   _ReserveSize;
}DATAVECTOR;

typedef enum _HIILIB_JSON_DATA_FRMT_TYPE
{
    HII_JSON_OBJECT_OPEN ,
    HII_JSON_OBJECT_CLOSE ,
    HII_JSON_ARRAY_OPEN ,
    HII_JSON_ARRAY_CLOSE,
    HII_JSON_DATA,
    HII_JSON_DATA_INTEGER,
    HII_JSON_DATA_BOOLEAN,
    HII_JSON_NOT_SET,
    
}HIILIB_JSON_DATA_FRMT_TYPE;

// leave a gap in numbering so that we can introduce
// extra legacy controls and strings for devices
#define CONTROL_GAP 11
#define STRING_GAP 11

extern
BOOT_OPTION **gBootOptions;
extern
UINT8 *gBootOrder;
extern
UINT64 gBootCount;

extern
EFI_STATUS _PopulateGlobalBootOptions(); //<OEMDBG-hujf1001-20201105 Fix system hang when no boot device insert +>

extern
char *gpStdBuf;
extern
char *gpMfgBuf;
extern
BOOLEAN gIsStdBuf;
extern
BOOLEAN gIsMfgBuf;
VOID _GetStdMfgDefaultsBuffer();
BOOLEAN _GetStdDefaults(CONTROL_INFO* Control);
BOOLEAN _GetMfgDefaults(CONTROL_INFO* Control);
void FreeResetControlInfo(void);

//EFI_STATUS  LoadOptimalDefaultsOfAmiSce(void);
// things not published in headers
#define RFC_3066_ENTRY_SIZE (42 + 1)
extern UINT32 UpdateLanguageCodes(UINT32 ConditionalVariable);

void HiiLib_UpdateData (CHAR8 **String);
void SaveFailedMapId (CHAR8 *tempBuffer);
VOID* HiiLib_GetJsonDataStr(HIILIB_JSON_DATA_FRMT_TYPE Style,UINTN Lebel,VOID *LeftNameStr,VOID *RightValueStr);
UINTN HiiLib_Rbatoi_base (CHAR8 *string, UINT8 base);
CHAR16 *HiiLib_SkipEscCode(CHAR16 *String);
EFI_STATUS HiiLib_RemoveSpecialChars (CHAR16 **String);

#define DEFAULT_BOOT_ORDER_VARIABLE_GUID  \
  { 0x45cf35f6, 0x0d6e, 0x4d04, 0x85, 0x6a, 0x03, 0x70, 0xa5, 0xb1, 0x6f, 0x53 }
#define DEFAULT_LEGACY_DEV_ORDER_VARIABLE_GUID  \
  { 0x3c4ead08, 0x45ae, 0x4315, 0x8d, 0x15, 0xa6, 0x0e, 0xaa, 0x8c, 0xaf, 0x69 }
#define NULL_GUID \
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }

BOOLEAN
EfiCompareGuid (
  IN EFI_GUID *Guid1,
  IN EFI_GUID *Guid2
  ) ;

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
