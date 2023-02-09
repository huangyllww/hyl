#ifndef _UEFI_2_1_STRING_MANAGEMENT_H
#define _UEFI_2_1_STRING_MANAGEMENT_H

#include "types.h"
#include "EDKStructures21.h"

typedef struct _LANGUAGE_DATA
{
	wchar_t	*LangCode;
	wchar_t	*LangString;
}LANGUAGE_DATA;

extern LANGUAGE_DATA *gLanguages;
extern UINT32 gLangCount;

UINT32 GetUefi21HiiLanguages( UINT8 *IFRDataTable, VOID* HiiPackage, LANGUAGE_DATA **Language );
wchar_t *UEFI21HiiGetString( UINT8 *DataTable, VOID* HIIPackage, UINT16 Token ,BOOLEAN MapLang);
UINT8 *GetUefi21StringOffset( UINT8** pStringPack, UINT16 Token );
BOOLEAN IsUefi21UQISupported();
UINTN GetPLDMMappingCount();
CHAR8** GetPLDMMappingLanguageID();

#endif /* #ifndef _UEFI_2_0_STRING_MANAGEMENT_H */
