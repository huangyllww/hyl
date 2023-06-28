//*****************************************************************
//*****************************************************************
//**                                                             **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.    **
//**                                                             **
//**                       All Rights Reserved.                  **
//**                                                             **
//**    5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093 **
//**                                                             **
//**                       Phone (770)-246-8600                  **
//**                                                             **
//*****************************************************************
//*****************************************************************

/** @file FwConfigJsonLib.h
    Contains structure declarations for Jsondata types and has function declarations

**/

#ifndef _FW_CONFIG_JSONLIB_H_
#define	_FW_CONFIG_JSONLIB_H_

#include <AmiLib.h>

typedef struct _VECTOR
{
	CHAR16 *_String;
	CHAR8 *_String8;
	UINTN	_SizeUsed;
	UINTN	_SizeFree;
	UINTN	_ReserveSize;
}VECTOR;

typedef enum _JSON_DATA_FRMT_TYPE
{
	JSON_OBJECT_OPEN ,
	JSON_OBJECT_CLOSE ,
	JSON_ARRAY_OPEN ,
	JSON_ARRAY_CLOSE,
	JSON_DATA,
	JSON_DATA_INTEGER,
	JSON_DATA_BOOLEAN,
	JSON_NOT_SET,
	
}JSON_DATA_FRMT_TYPE;
typedef struct _SYSTEM_BUILD_INFO
{
	UINT8 	ProjectMajorVersion;
	UINT8   ProjectMinorVersion; 
	UINT8   ProjectBuildVersion; 
	CHAR8   ProjectTag[20];
	CHAR8   BiosTag[20]; //combining project major version ,project minor version and project tag
	BOOLEAN IsLoaded;
}SYSTEM_BUILD_INFO;

//This structure used to store language id and its VECTOR
typedef struct _MULTILANG_VECTOR
{
    CHAR16  Language [20];
    VECTOR  *LanguageVector;
} MULTILANG_VECTOR;

//This structure used to store language id and its string
typedef struct _MULTI_LANG_STRINGS
{
    CHAR16 *String;
    CHAR16 StringLang [20];
}MULTI_LANG_STRINGS;

EFI_STATUS 	ReAllocString16(VECTOR *SrcVector,UINTN RequireSize);
EFI_STATUS 	ReAllocString8(VECTOR *SrcVector,UINTN RequireSize);
CHAR8* 		GetJsonDataStr8(JSON_DATA_FRMT_TYPE Style,UINTN Lebel,CHAR8 *LeftNameStr,CHAR8 *RightValueStr);
CHAR16* 	GetJsonDataStr16(JSON_DATA_FRMT_TYPE Style,UINTN Lebel,CHAR16 *LeftNameStr,CHAR16 *RightValueStr);
VOID* 		GetJsonDataStr(BOOLEAN IsUniCode,JSON_DATA_FRMT_TYPE Style,UINTN Lebel,VOID *LeftNameStr,VOID *RightValueStr);
VOID        GetJsonDataAndUpdate (BOOLEAN IsUniCode, JSON_DATA_FRMT_TYPE Style, UINTN Lebel, VOID *LeftNameStr, MULTI_LANG_STRINGS *RightValueStr);
VOID        GetEngJsonDataAndUpdate (BOOLEAN IsUniCode, JSON_DATA_FRMT_TYPE Style, UINTN Lebel, VOID *LeftNameStr, MULTI_LANG_STRINGS *RightValueStr);

EFI_STATUS 	AppendString16(VECTOR *SrcVector,CHAR16 *AppendStr);
EFI_STATUS 	AppendString8(VECTOR *SrcVector,CHAR8 *AppendStr);
EFI_STATUS 	AppendString(BOOLEAN IsUnicode,VECTOR *SrcVector,VOID *AppendStr);

CHAR8* 		RemoteConvertChar16toChar8(CHAR16 *InputString16);
CHAR16*		DupString(CHAR16 *Src);
void 		ReplaceString(CHAR16 *Src,CHAR16 *FindStr,CHAR16 *ReplaceStr);
UINTN 		Rbatoi_base (CHAR8 *string, UINT8 base);

CHAR16* ConvVfrNumToString(UINT64 Num,UINTN ControlDataWidth,UINT8 Base);
EFI_STATUS GetSystemBuildInfo(SYSTEM_BUILD_INFO *SysBuildInfo);
CHAR8* Get_BiosTag();
CHAR8* Get_ProjectTag();
UINT8 Get_ProjectMajorVersion();
UINT8 Get_ProjectMinorVersion();
UINT8 Get_ProjectBuildVersion();
#endif /* _FW_CONFIG_JSONLIB_H_ */

//*****************************************************************
//*****************************************************************
//**                                                             **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.    **
//**                                                             **
//**                       All Rights Reserved.                  **
//**                                                             **
//**    5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093 **
//**                                                             **
//**                       Phone (770)-246-8600                  **
//**                                                             **
//*****************************************************************
//*****************************************************************
