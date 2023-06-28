#ifndef _UEFI_2_0_STRING_MANAGEMENT_H
#define _UEFI_2_0_STRING_MANAGEMENT_H

typedef struct _LANGUAGE_DATA
{
	wchar_t	*LangCode;
	wchar_t	*LangString;
}LANGUAGE_DATA;

extern LANGUAGE_DATA *gLanguages;
extern UINT32 gLangCount;

UINT32 GetUefi20HiiLanguages( UINT8 *IFRDataTable, VOID* HiiPackage, LANGUAGE_DATA **Language );
wchar_t *UEFI20HiiGetString( UINT8 *DataTable, VOID* HIIPackage, UINT16 Token );
BOOLEAN IsUQISupported();

#endif /* #ifndef _UEFI_2_0_STRING_MANAGEMENT_H */
