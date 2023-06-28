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

/** @file SetupDataXmlCreation.h
    Contains structure declarations related bios pages information

**/

#ifndef _SETUPDATA_XML_CREATION_H_
#define	_SETUPDATA_XML_CREATION_H_

#include <Include/AmiDxeLib.h>
#include "variable.h"
#include <Protocol/HiiString.h>
#include <HiiLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <TseCommon.h>
#include <Inc/boot.h>
#include "setupdata.h"
#include <Protocol/HiiString.h>
#include <Library/BaseLib.h>
#include <Token.h>
#include <Include/AMIVfr.h>
#include "Library/MemoryAllocationLib.h"
#include <Token.h>
#include <ConditionalExpressionApi.h>

#define MAX_SUPPORTED_CONTROL 20

typedef enum _CONTROL_TYPE
{
	_CONTROL_TYPE_NULL,
	_CONTROL_TYPE_SUBMENU,
	_CONTROL_TYPE_LABEL,
	_CONTROL_TYPE_TEXT,
	_CONTROL_TYPE_DATE,
	_CONTROL_TYPE_TIME,
	_CONTROL_TYPE_POPUPSEL,
	_CONTROL_TYPE_EMPTY,
	_CONTROL_TYPE_MEMO,
	_CONTROL_TYPE_MSGBOX,
	_CONTROL_TYPE_CHECKBOX, 
	_CONTROL_TYPE_NUMERIC,
	_CONTROL_TYPE_EDIT,
	_CONTROL_TYPE_PASSWORD,
	_CONTROL_TYPE_MENU,
	_CONTROL_TYPE_ORDERED_LIST,
	_CONTROL_TYPE_POPUPEDIT,
	_CONTROL_TYPE_VARSTORE_SELECT,
	_CONTROL_TYPE_VARSTORE_SELECT_PAIR,
	_CONTROL_TYPE_POPUP_STRING,
	_CONTROL_TYPE_VARSTORE,
	_INCONSISTENT_IF,
	_NO_SUBMIT_IF,
	_DISABLE_IF,
	_CONTROL_TYPE_VARSTORE_NAME_VALUE,
	_CONTROL_TYPE_VARSTORE_EFI,
	_CONTROL_TYPE_VARSTORE_DEVICE,
	_CONTROL_TYPE_ACTION,
	_CONTROL_TYPE_RESET,
	_CONTROL_TYPE_RULE,
	_CONTROL_TYPE_REF2,
	_CONTROL_TYPE_SLIDE,
	_WARN_IF,
}CONTROL_TYPE;

typedef struct _VALUE
{
	CHAR16 *_ValueDisplayName;
	CHAR16 *_ValueName;
}VALUE;

typedef struct _ATTRIBUTES
{
	
	CHAR16 		* _DefaultValue;
	CHAR16 		* _DisplayName;
	CHAR16 		* _HelpText;
	CHAR16 		* _AttributeName;
	BOOLEAN		  _ReadOnly;
	CONTROL_TYPE  _Type;
	VALUE		* _Value;
	UINTN         _ValueCount;
	CHAR16 		* _WarningText;
	CHAR16		*_UpperBoundStr;
	CHAR16		*_LowerBoundStr;
	CHAR16		*_ScalarIncrementStr;
	CHAR16		*_MinLengthStr;
	CHAR16		*_MaxLengthStr;
}ATTRIBUTES;

typedef struct
{
    CHAR16       MapId[128];       //MapString name 
} MAPSTRING_NAME;

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

void 		XmlFreeAttributeData(ATTRIBUTES      *Attributes);
EFI_STATUS CreateSetupData (void **Buffer, UINTN *BuffSize, UINT8 SplCtrlIndex);
CHAR16* 	XmlGetActualHiiString(EFI_HII_HANDLE handle, UINT16 token);
BOOLEAN 	IsXmlMapIdSupported(CHAR16 *MapString);
UINT16 		_SpecialGetValue( CONTROL_INFO *controlinfo, GUID_INFO **guidInfo );
UINT32 		XmlGetControlGuidType(CONTROL_INFO *In_ControlInfo);
void 		XmlGet_BiosAttributes();
void 		XmlInit_AttributeData();
BOOLEAN 	XmlIsStringControl(CONTROL_TYPE  ControlType);
CHAR16 		*SkipEscCode (CHAR16 *String);
void    	WriteToBuffer (CHAR16 *tempBuffer);
CHAR16  	*GetGUIDAsString(EFI_GUID *Guid);
CHAR16  	*CheckSpecialChar(CHAR16 *text);
void    	WriteControlInfoToBuffer(CONTROL_INFO* CtrlInfo, ATTRIBUTES  *Attributes, EFI_GUID RefGuid);
VOID 		LoadSetupDefaultsHook( NVRAM_VARIABLE *defaults, UINTN data );
BOOLEAN 	_SaveValues (UINTN data);
CHAR16* 	XmlConvVfrNumToString(UINT64 Num,UINTN ControlDataWidth,UINT8 Base);
BOOLEAN 	XmlAllowDuplicateMapID ();
BOOLEAN 	XmlAllowMappingStringOnly ();
VOID 		XmlCreationInitializeTseMethods();
EFI_STATUS 	GetPageGuidFromPgIndex(UINT32 PgIndex, EFI_GUID *FormGuid);
VOID 		MemFreePointer( VOID **ptr );
EFI_STATUS 	HiiFixupData();
VOID 		ProcessUIInitHook(VOID);
UINT16 		UefiGetQuestionOffset(VOID *IfrPtr);
UINT16		UefiGetWidth(VOID *IfrPtr);
UINT8 		UefiGetBaseValue(VOID *IfrPtr);
UINT64 		UefiGetStepValue(VOID *IfrPtr);
UINT64 		UefiGetMinValue(VOID *IfrPtr);
UINT64 		UefiGetMaxValue(VOID *IfrPtr);
EFI_STATUS 	UefiGetOneOfOptions(CONTROL_INFO *CtrlInfo, VOID **Handle, UINT16 **OptionPtrTokens,
                               UINT64 **ValuePtrTokens, UINT16 *ItemCount,
                               UINT16 *Interactive,UINT16 *CallBackKey );
BOOLEAN 	IsXmlControlTypeSupported(UINT8 In_ControlType);
CHAR16 *	GetCurrentValue(CONTROL_INFO *In_ControlInfo);
CHAR8 * 	GetMapLanguageName ();
BOOLEAN   	IsSupportDefaultForStringControl (VOID);

CHAR16 *Uefi21HiiGetStringLanguage(EFI_HII_HANDLE handle, UINT16 token, CHAR16 *lang);
#if REMOTE_CONFIG_TSE_DEPENDENCY == 0
EFI_STATUS  Uefi21HiiInitializeProtocol(VOID);
EFI_STATUS  InitApplicationData ();
#endif

//---------------------------------------------------------------------------

extern UINT16   	UefiGetSpecialEqIDValue(CONTROL_INFO *ControlInfo, GUID_INFO **GuidInfo);
extern CHAR16 		*StrDup(CHAR16 *String);
extern  EFI_STATUS  GetPageGuidFromPgIndex (UINT32 PgIndex, EFI_GUID *FormGuid);
extern UINT16       UefiGetTextTwoField(VOID *IfrPtr) ;
EFI_STATUS          HiiExit ();
CHAR16              *GetMultiLanguageXmlString (VOID *Handle, UINT16 StringId, CHAR16 *LeftString, CHAR16 *DefaultString);
#endif /* _SETUPDATA_XML_CREATION_H_ */

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
