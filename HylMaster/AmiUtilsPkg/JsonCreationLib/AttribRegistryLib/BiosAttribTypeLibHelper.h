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

/** @file BiosAttribCreationSrc.h
    Contains structure declarations related bios attributes and values

**/

#ifndef _BIOS_ATTRIB_CREATION_SRC_H_
#define	_BIOS_ATTRIB_CREATION_SRC_H_

#include <JsonUtilityLib.h>
#include "variable.h"
#include <ConditionalExpression.h>
#include <ConditionalExpressionApi.h>
#include "Inc/setupdata.h"

typedef struct _SUPPORTEDSYSTEMS
{
	CHAR16 *_ProductName;
	CHAR16 *_SystemId;
	CHAR16 *_FirmwareVersion;
}SUPPORTEDSYSTEMS;

//Structure changed to hold multilanguage strings
typedef struct _VALUE
{
    MULTI_LANG_STRINGS  *_ValueDisplayName;
	CHAR16              *_ValueName;
	//UINTN				EqvValue;
}VALUE;

typedef enum {
    CurrentValue,
    DefaultValue,         //unused
    ReadOnly,             //unused
    WriteOnly,            //unused
    GrayOut,
    Hidden,
    LowerBound,           //unused
    UpperBound,           //unused
    MinLength,            //unused
    MaxLength,            //unused
    ScalarIncrement       //unused
} MAP_PROPERTY;

typedef enum {
    EQU,
    NEQ,
    GTR,
    GEQ,
    LSS,
    LEQ
} MAP_CONDITION;

typedef enum {
    AND,
    OR
} MAP_TERMS;

typedef enum {
    Map
} DEPENDENCY_TYPE;

typedef struct _DEPENDENCIES
{
//    CHAR16              MapFromAttribute [20];
    MAP_TERMS           MapTerms;
    MAP_PROPERTY        MapFromProperty;
    MAP_CONDITION       MapCondition;
    UINTN               MapFromValue;
    MAP_PROPERTY        MapToProperty;
    UINT16              ControlKey;
    struct _DEPENDENCIES *Next;
    struct _DEPENDENCIES *Prev;
    struct _DEPENDENCIES *Child;
}DEPENDENCIES;

//Structure changed to hold multilanguage strings for _DisplayName and _HelpText
typedef struct _ATTRIBUTES
{
	
	CHAR16 		        * _DefaultValue;
	MULTI_LANG_STRINGS  * _DisplayName;
	MULTI_LANG_STRINGS  * _HelpText;
	CHAR16 		        * _AttributeName;
	BOOLEAN		        _ReadOnly;
	CONTROL_TYPE        _Type;
	VALUE		        * _Value;
	UINTN               _ValueCount;
	CHAR16 		        * _WarningText;
	CHAR16		        *_UpperBoundStr;
	CHAR16		        *_LowerBoundStr;
	CHAR16		        *_ScalarIncrementStr;
	CHAR16		        *_MinLengthStr;
	CHAR16		        *_MaxLengthStr;
    UINT16              ControlKey;
    EFI_GUID            PageGuid;
    void                *AttributePtr;
    struct _ATTRIBUTES  *Next;
    DEPENDENCIES        *Dependencies;     //Array of dependencies for that control
    //<Yaosr-20210312 Improve redfish: diplay bios options according to the BIOS setup hierarchy on the BMC web. +>
    CHAR16              * _MenuName;
    CHAR16              * _MenuPath;
    CHAR16              *_DisplayOrder;
    UINTN               _ControlDestPageID;
    BOOLEAN             _GrayOut;
    BOOLEAN             _Hidden;
    //<Yaosr-20210312 Improve redfish: diplay bios options according to the BIOS setup hierarchy on the BMC web. ->

} ATTRIBUTES;

typedef enum _JSON_DATA_STR_INDEX
{
	SUPPORTED_SYSTEM_INDEX = 0,
	REGISTRY_ENTRIES_INDEX = 4,
	ATTRIBUTES_INDEX = 5,	
}JSON_DATA_STR_INDEX;

typedef struct
{
    CHAR16                  MappingId [128];
    UINT16                  ControlKey;
    ATTRIBUTES              *AttributePtr;
} MAPPING_ID_DETAILS;

//typedef struct
//{
//    UINTN               MappingIdCount;
//    UINTN               MappingIdReservedCount;
//    MAPPING_ID_DETAILS  *MappingIdDetails;
//} MAPPING_ID_LIST;

typedef struct
{
    EFI_GUID                GuidValue;
    UINTN                   MappingIdCount;
    UINTN                   MappingIdReservedCount;
    MAPPING_ID_DETAILS      **MappingIdDetails;
} FORMSET_MAPPING_DETAILS;

typedef struct
{
    UINTN                   FormsetMappingCount;
    UINTN                   FormsetMappingReservedCount;
    FORMSET_MAPPING_DETAILS **FormsetMappingDetails;
} FORMSET_MAPPING_LIST;

#define	GUID_KEY_LANG_CODES		0x0005
#define	GUID_KEY_BOOT_NOW		0x0001
#define GUID_KEY_BBS_ORDER		0x0007
#define	GUID_KEY_DRIVER_OPTION	0x0012	
#define SYSTEM_PASSWORD        	0x600
#define NON_SYSTEM_PASSWORD     0x601
#define BOOL_STR(X) (X==TRUE)?L"true":L"false"
#define IS_UNSIGNED(Type,Min,Max) ( ((Type)Min) < ((Type)Max) )
#define RESERVE_SIZE 2048
#define GUID_KEY_AMI_CALLBACK           0x0002
#define swprintf            SPrint
#define realloc             MemReallocateZeroPool
#define MEMORY_ALLOC_UNIT   2048

void		Dump_JsonHeaderInfo();
void 		WriteAttributesToFile(ATTRIBUTES 	*In_Attributes);
void 		FreeAttributeData(ATTRIBUTES      **Attributes);
EFI_STATUS 	AppendBiosConfig(VECTOR *SrcVector,CHAR16 *AppendStr);
EFI_STATUS 	AllocCHAR16buf(VECTOR *SrcVector,UINTN RequireSize);
EFI_STATUS Get_BiosSetupJsonData(OUT CONST CHAR16** Out_JsonBuffer,OUT UINTN *Out_JsonbufSize);
EFI_STATUS 	GetPasswordEncryptedData ( CHAR16 *MapId, CHAR16 *NewPwdData, CHAR16 *OldPwdData, VOID **EncryptedPwdBuffer, UINTN *PwdSize);
EFI_STATUS 	ResetBiosConfig (UINT8 Operation);
EFI_STATUS 	Fw_EncodePasswordData( CHAR16 *Password,  CONTROL_INFO    *CtrlInfo, CHAR16 **EncodedBuffer );
EFI_STATUS 	CreateSetupData (void **Buffer, UINTN *BuffSize);
BOOLEAN 	IsPasswordSupportNonCaseSensitive();
BOOLEAN 	IsPasswordEncodeEnabled(CONTROL_INFO *PwDataPtr);
VOID 		PasswordEncodeHook( CHAR16 *Password, UINTN MaxSize);
CHAR16* 	GetActualHiiString(EFI_HII_HANDLE handle, UINT16 token);
BOOLEAN 	IsMapIdSupported(CHAR16 *MapString);
UINT16 		_SpecialGetValue( CONTROL_INFO *controlinfo, GUID_INFO **guidInfo );
CHAR16 		*HiiGetStringLanguage( VOID* handle, UINT16 token, CHAR16 *lang );
UINT32 		GetControlGuidType(CONTROL_INFO *In_ControlInfo);
void 		UpdateData(CHAR16 **String);
void 		Get_BiosAttributes();
void 		Init_AttributeData();
BOOLEAN 	IsStringControl(CONTROL_TYPE  ControlType);
CHAR16 		*SkipEscCode (CHAR16 *String);
VOID 		LoadSetupDefaultsHook( NVRAM_VARIABLE *defaults, UINTN data );
BOOLEAN 	_SaveValues (UINTN data);
extern UINT16   	UefiGetSpecialEqIDValue(CONTROL_INFO *ControlInfo, GUID_INFO **GuidInfo);
extern CHAR16 		*StrDup(CHAR16 *String);
extern  EFI_STATUS  GetPageGuidFromPgIndex (UINT32 PgIndex, EFI_GUID *FormGuid);
EFI_STATUS          InitApplicationData ();
BOOLEAN             CheckForSpaceInMappingId (CHAR16 *AttributeName);
EFI_STATUS 			RemoveSpecialChars (CHAR16 **String);
UINT32 				PasswordAuthenticate( CHAR16 *Password );
UINT32 				PasswordCheckInstalled( VOID );
EFI_STATUS			CheckForSystemPassword (UINT32 *PasswordInstalled);
EFI_STATUS 			ValidateSystemPassword (CHAR16 *PasswordEntered, UINT32 *PasswordInstalled, BOOLEAN 	*PasswordMatched);
VOID                FreeMultiLangStrings (MULTI_LANG_STRINGS *MultiLangStrings);
MULTI_LANG_STRINGS  *FormMultiLangStrings (EFI_HII_HANDLE Handle, UINT16 StrToken, CHAR16 *String, UINTN BitMasking);
UINTN               FindEnglishString (MULTI_LANG_STRINGS *MultiLangStrings);
VARIABLE_INFO       *VarGetVariableInfoIndex( UINT32 index );
EFI_STATUS          AddControlDependencies (CONTROL_INFO *controlInfo, DEPENDENCIES  **AttribDependencies, CHAR16 *AttributeName, BOOLEAN *ReadOnly, BOOLEAN *IsHide);//<Yaosr-20210312 Improve redfish: diplay bios options according to the BIOS setup hierarchy on the BMC web.>
EFI_STATUS          WriteDependencyToFile (ATTRIBUTES *AttributeList);
EFI_STATUS          WriteMenusToFile (ATTRIBUTES *AttributeList);//<Yaosr-20210312 Improve redfish: diplay bios options according to the BIOS setup hierarchy on the BMC web.>
CHAR16              *StrDup8to16( CHAR8 *string );

#endif /* _BIOS_ATTRIB_CREATION_SRC_H_ */

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
