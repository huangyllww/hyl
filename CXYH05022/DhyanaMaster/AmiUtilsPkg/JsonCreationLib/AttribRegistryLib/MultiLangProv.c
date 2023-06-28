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

/** @file MultiLangProv.c
    This files provides provision to handle multi language requests

**/

//---------------------------------------------------------------------------

#include <Include/AmiDxeLib.h>
#include <Token.h>
#include <BiosAttribTypeLib.h>
#include <Inc/boot.h>

//---------------------------------------------------------------------------

CHAR16  gRemoteCurrentLang [20] = L"en-US";

MULTI_LANGUAGE_PROVISION_PROTOCOL gMultiLangProvProtocol = {
        GetLanguageList,
        SetLanguage,
        NULL
};

VOID    *EfiLibAllocateZeroPool (IN  UINTN   AllocationSize);
CHAR16  *StrDup( CHAR16 *string );

/**
  Returns the language lists available in the platform
    
  @param  LangList          Buffer for returning available languages. Buffer allocated by this function and returned.
                            User responsible to free it
  @param  LangCount         Count for available languages
  @param  InputOperation    For future use

  @return EFI_STATUS
  @retval EFI_SUCCESS for now
**/

EFI_STATUS GetLanguageList (OUT CHAR16 ***LangList, OUT UINTN *LangCount, IN UINTN InputOperation)
{
    UINTN                   iIndex = 0;
    extern UINTN            gLangCount;
    extern LANGUAGE_DATA    *gLanguages;
    
    *LangCount = 0;
    *LangList = NULL;
    
    BootGetLanguages ();
    
    if (gLangCount)
    {
        *LangList = EfiLibAllocateZeroPool (gLangCount);
        if (NULL != *LangList)
        {
            for (iIndex = 0; iIndex < gLangCount; iIndex ++)
            {
                (*LangList) [iIndex] = StrDup(gLanguages [iIndex].Unicode);
            }
        }
        *LangCount = gLangCount;
    }
    return EFI_SUCCESS;
}

/**
  Sets the language for JSON formations
    
  @param  Language      Language to set for JSON formation

  @return EFI_STATUS
  @retval EFI_SUCCESS for now
**/

EFI_STATUS SetLanguage (IN CHAR16 *Language)
{
    if (NULL != Language)
    {
        MemSet (gRemoteCurrentLang, sizeof (gRemoteCurrentLang), 0);
        StrCpy (gRemoteCurrentLang, Language);
    }
    return EFI_SUCCESS;
}

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
