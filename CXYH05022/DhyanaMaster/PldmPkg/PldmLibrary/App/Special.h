#ifndef _SPECIAL_H_
#define _SPECIAL_H_

#define _EFI 1

#if defined(_LNX) || defined(_BSD) || defined(_EFI)
#include "os_hook.h"
#endif

#define FORMSET_DETAILS \
  L"\n// FORM SET\n" \
  L"// GUID\t= %"_S L"\n" \
  L"// Title\t= %"_S L"\n" \
  L"// Help\t= %"_S L"\n" \
  L"// Class\t= %02X\n" \
  L"// SubClass\t= %02X\n"

#define FORM_DETAILS \
  L"\n// FORM\n" \
  L"// FORM ID\t= %02X\n" \
  L"// Title\t= %"_S L"\n"

#define CONTROL_TEMPLATE \
  L"\nSetup Question\t= %"_S L"\n" \
  L"Token\t=%02X\t// Do NOT change this line\n" \
  L"Offset\t=%02X\n" \
  L"Width\t=%02X\n" \

#define CONTROL_TEMPLATE_MAP_STRING \
  L"\nSetup Question\t= %"_S L"\n" \
  L"Map String\t= %"_S L"\n" \
  L"Token\t=%02X\t// Do NOT change this line\n" \
  L"Offset\t=%02X\n" \
  L"Width\t=%02X\n" \


#define STD_DEFAULTS_STR    L"BIOS Default\t="
//CHAR16 *STD_DEFAULTS_STR     = L"BIOS Default\t=";

#define MFG_DEFAULTS_STR    L"MFG Default\t="
#define CONTROL_VALUE_STR   L"Value\t="
#define CONTROL_OPTION_STR  L"Options\t="
#define CONTROL_ORDER_STR   L"ListOrder\t="

#define DEFAULT_FLAG  0x01
#define MFG_FLAG      0x02

#define MAX_LENGTH    1024
#define BOOT_STRING_LENGTH 10

#define UEFI_21_VERSION		0x021
#define UEFI_20_VERSION		0x020
#define SUBTITLE_TEMPLATE L"\n// SUBTITLE\t= %"_S L"\n"

typedef struct _TokenVal
{
	DWORD	dLineNumber;	// Line number being parsed.  Used to report syntax errors.
	WORD	wToken;		// Token found in Script file.
	UINT64	bValue;		// Value found in Script file.
	// start Jason Messer 051700 - Fixed Token Support >>
	UINT64	bBiosDefault;	// BIOS Default found in Script file.
	UINT64	bMfgDefault;	// MFG Default found in Script file.
	// end Jason Messer 051700 - Fixed Token Support <<
	char  *cSetupPrompt; // Setup Question Text Prompt
	WORD* bOrder;
	UINT32 VariableStorage; // To store variable info of the matching control
	//Fix for EIP 43204
	UINT16 DataOffset; //	To store offset info of the matching control
	UINT16 DataWidth; //To store DataWidth of the matching control
	char	*cStringValue; //To store string value for string opcode
	UINT16 cStringLen; //To store the acutal string length of cStrngValue in Unicode
	bool IsPrintableChar; //To flag false for non-printable cStringValue, true for printable cStringValue
	UINT8 *cStringValue_hex; //To store non-printable unicode string value for string opcode
}TokenVal;

typedef struct _ParseStatus
{
	UINT32  uSetupQuestion  	:1;	// This bit will be set when a Setup Question is found in the Script file.
	// start Jason Messer 051700 - Fixed Token Support >>
	UINT32  uFixedToken     	:1;  	//  This bit will be set when a Fixed Token is found in the Script file.
	// end Jason Messer 051700 - Fixed Token Support  <<
	UINT32	uToken		:1;  	// This bit will be set when a Token is found in the Script file.
	UINT32	uOptimalSetting 	:1;  	// This bit will be set when a Optimal Setting value is found in the Script file.
	UINT32	uFailSafeSetting	:1;  	// This bit will be set when a FailSave value is found in the Script file.
	UINT32	uOptions		:1;  	// This bit will be set when a Option List is found in the Script file.
	UINT32	uValue		:1;  	// This bit will be set when a Value for external function is found in the Script file.
	UINT32	uAsterisk		:1;  	// This bit will be set when a Current Setting (Asterisk) is found in a Setup Question from the Script file.
	UINT32  uFail   :1;  	// This will be set if an error is found while parsing the file.
	UINT32	uBIOS_ID		:1;  	// This bit is set when the BIOS ID is found in the Script file.
	UINT32  uOrder  :1; // This bit will be set when a Order List is found in the Script file.
	UINT32  uParsed :1; // This bit will be set if the corresponding setup question is updated
	//Fix for EIP 43204
	UINT32  uDataWidth :1; //This bit will be set when a DataWidth is found in the Script file.
	UINT32  uDataOffset :1; //This bit will be set when a Offset is found in the Script file.
}ParseStatus;

typedef struct _SETUP_QUESTION_LIST SETUP_QUESTION_LIST;
struct _SETUP_QUESTION_LIST
{
	TokenVal            spScriptVal;
	ParseStatus         spStatus;
	SETUP_QUESTION_LIST *Next;
};

BOOLEAN ImportLanguageControlToNVRAM( SETUP_QUESTION_LIST *SetupQuestion, UINT8 *DataTable, UINT8 *HIIDb, CONTROL_INFO *Control );
BOOLEAN ImportBootOrderContrlToNVRAM( SETUP_QUESTION_LIST *SetupQuestion, UINT8 *DataTable, UINT8 *HIIDb, CONTROL_INFO *Control );
BOOLEAN ImportLegacyDevOrderControlToNVRAM( SETUP_QUESTION_LIST *SetupQuestion, UINT8 *DataTable, UINT8 *HIIDb, CONTROL_INFO *Control );
UINT32  GetHiiLanguages( UINT8 *IFRDataTable, VOID* HiiPackage, LANGUAGE_DATA **Language );
UINT16 GetLangDataWidth();
VOID FreeBootOptions();

#endif /* #ifndef _SPECIAL_H_ */
