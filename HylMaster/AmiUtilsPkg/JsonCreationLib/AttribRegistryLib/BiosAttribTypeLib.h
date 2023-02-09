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

/** @file BiosAttributeBin.h
    Contains declarations for control types and has function declarations

**/

#ifndef _BIOS_ATTRIBUTEBIN_H_
#define	_BIOS_ATTRIBUTEBIN_H_

#include <Uefi/UefiBaseType.h>
#include <Token.h>
#include <JsonUtilityLib.h>

#define MAX_SUPPORTED_CONTROL 20
#if REMOTE_CONFIG_TSE_DEPENDENCY
#define Uefi21HiiGetStringLanguage HiiGetStringLanguage
#endif

typedef EFI_STATUS (*GET_FIRMWARE_ATTRIB_CALLBACK) ( OUT CONST CHAR16**,OUT UINTN *Size);
typedef EFI_STATUS (*GET_PASSWORD_ENCRYPTED_DATA) ( CHAR16 *MapId, CHAR16 *NewPwdData, CHAR16 *OldPwdData, VOID **EncryptedPwdBuffer, UINTN *PwdSize);
typedef EFI_STATUS (*RESET_BIOS_CONFIG) (UINT8 Operation);
typedef EFI_STATUS (*CHECK_FOR_SYSTEM_PASSWORD) (UINT32 *PasswordInstalled);
typedef EFI_STATUS (*VALIDATE_SYSTEM_PASSWORD) (CHAR16 *PasswordEntered, UINT32 *PasswordInstalled, BOOLEAN *PasswordMatched);
typedef EFI_STATUS (*GET_LANGUAGE_LIST) (OUT CHAR16 ***LangList, OUT UINTN *LangCount, IN UINTN InputOperation);
typedef EFI_STATUS (*SET_LANGUAGE) (IN CHAR16 *Language);

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

typedef struct
{
    CHAR16       MapId[128];       //MapString name 
} MAPSTRING_NAME;

typedef struct _FIRWARE_CONFIG_ATTRIB_PROTOCOL 
{
	GET_FIRMWARE_ATTRIB_CALLBACK 	GetFirmwareCallback;
	GET_PASSWORD_ENCRYPTED_DATA 	GetPasswordEncryptedData;
	RESET_BIOS_CONFIG				ResetBiosConfig;
	CHECK_FOR_SYSTEM_PASSWORD		CheckForSystemPassword;
	VALIDATE_SYSTEM_PASSWORD		ValidateSystemPassword;
}FIRWARE_CONFIG_ATTRIB_PROTOCOL;

//Protocol to set and get the multilanguage ids
typedef struct _MULTI_LANGUAGE_PROVISION_PROTOCOL
{
    GET_LANGUAGE_LIST               GetLanguageList;
    SET_LANGUAGE                    SetLanguage;
    void                            *LanguagePtr;    //For future use
} MULTI_LANGUAGE_PROVISION_PROTOCOL;

extern EFI_GUID gFirmwareConfigAttribGuid;

EFI_STATUS  GetLanguageList (OUT CHAR16 ***LangList, OUT UINTN *LangCount, IN UINTN InputOperation);
EFI_STATUS  SetLanguage (IN CHAR16 *Language);
BOOLEAN     AllowMappingStringOnly ();
BOOLEAN     AllowDuplicateMapID ();
CHAR16      *Uefi21HiiGetStringLanguage(EFI_HII_HANDLE handle, UINT16 token, CHAR16 *lang);
EFI_STATUS  ConstructMappingIdList ();

#endif // _BIOS_ATTRIBUTEBIN_H_

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
