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

/** @file TseFunctionsInit.c
    Contains implementation for creating index json file with all the mapping strings and it's current values

**/

//---------------------------------------------------------------------------

#include "SetupDataCreationLib.h"

#define BROWSERCALLBACK_MAX_ENTRIES 10

//---------------------------------------------------------------------------

extern EFI_BOOT_SERVICES    *gBS;

//---------------------------------------------------------------------------

static EFI_HANDLE   gCurrVarHandle = (EFI_HANDLE)NULL;
static UINT32       gCurrVarIndex = 0;
static EFI_HANDLE   gCallBackHandleStack[BROWSERCALLBACK_MAX_ENTRIES]; 
static UINT32       gCallBackVarIndexStack[BROWSERCALLBACK_MAX_ENTRIES];

/**
    function to free the pointers

    @param ptr 

    @retval void

**/
VOID MemFreePointer( VOID **ptr )
{
    if ( ( ptr == NULL ) || ( *ptr == NULL ) )
        return;

    gBS->FreePool( *ptr );
    *ptr = NULL;
}

/**
    function to copy a memory

    @param dest VOID *src, UINTN size

    @retval void

**/
VOID MemCopy( VOID *dest, VOID *src, UINTN size )
{
    gBS->CopyMem( dest, src, size );
}

/**
        Procedure   :   GetVariableNameByID

        Description :   function to get Variable Name based on ID

        Input       :   UINT32 VariableID

        Output      :   CHAR16 *

**/
CHAR16 *GetVariableNameByID( UINT32 VariableID )
{
    CHAR16 *varName = (CHAR16 *)EfiLibAllocateZeroPool(VARIABLE_NAME_LENGTH);
    if(varName != NULL)
    {
        switch(VariableID)
        {
            case VARIABLE_ID_LANGUAGE:
#if SETUP_SUPPORT_PLATFORM_LANG_VAR
                EfiStrCpy(varName, L"PlatformLang");
#else
                EfiStrCpy(varName, L"Lang");
#endif
                break;

            default:
                break;
        }
    }   

    return varName;
}

/**
        Procedure   :   GetGUIDNameByID

        Description :   function to get GUID Name based on Variable ID

        Input       :   UINT32 VariableID

        Output      :   CHAR16 *

**/
CHAR16 *GetGUIDNameByID( UINT32 VariableID )
{
    CHAR16 *guidName = (CHAR16 *)EfiLibAllocateZeroPool(VARIABLE_NAME_LENGTH);
    if(guidName != NULL)
    {
        switch(VariableID)
        {
            case VARIABLE_ID_LANGUAGE:
#if SETUP_SUPPORT_PLATFORM_LANG_VAR
                EfiStrCpy(guidName, L"PlatformLangCodes");
#else
                EfiStrCpy(guidName, L"LangCodes");
#endif
                break;
            default:
                break;
        }
    }
  return guidName;
}

/**
    Returns Length of a String of type CHAR8

    @param string 

    @retval UINTN
              

**/
UINTN StrLen8(CHAR8 *string)
{
    UINTN i=0;
    while(string[i])
        i++;

    return i;
}

/**
    function to reuse a allocated buffer

    @param oldBuffer UINTN oldSize, UINTN newSize

    @retval void

**/
VOID *MemReallocateZeroPool( VOID *oldBuffer, UINTN oldSize, UINTN newSize )
{
    VOID *buffer;

    buffer = EfiLibAllocateZeroPool( newSize );
    if ( buffer == NULL )
        return buffer;

    if ( newSize < oldSize )
        oldSize = newSize;

    if ( oldBuffer != NULL )
        MemCopy( buffer, oldBuffer, oldSize );

    MemFreePointer( (VOID **)&oldBuffer );

    return buffer;
}

/**
    Duplicates a String from CHAR16 to CHAR8

    @param String 

    @retval CHAR8 *
              

**/
CHAR8* StrDup16to8(CHAR16 *String)
{
    CHAR8 *text;
    UINTN len = 0, j = 0;

    while(String[j++])
        len++;
    text = EfiLibAllocateZeroPool( (1 + len) * sizeof(CHAR8));
    if(text != NULL)
    {
        j = 0;
        do
        {
            text[j] = (CHAR8)String[j];
        }while(j++ < len);
    }

    return text;
}

#define AMITSE_VSPrint  Swprintf_s_va_list
/**
    SPrint function to process format and place the results in Buffer

    @param Buffer Wide char buffer to print the results of the parsing of Format into.
    @param BufferSize Maximum number of characters to put into buffer. Zero means no limit.
    @param Format Format string see file header for more details.
        ...    - Vararg list consumed by processing Format.

    @retval Number of characters printed.

**/
UINTN
SPrint (
  OUT CHAR16        *Buffer,
  IN  UINTN         BufferSize,
  IN  CONST CHAR16  *Format,
  ...
  )
{
  UINTN   Return;
  VA_LIST Marker;

  VA_START (Marker, Format);
//  Return = AMITSE_VSPrint(Buffer, BufferSize, Format, Marker);
  Return = AMITSE_VSPrint(Buffer, BufferSize, (CHAR16*)Format, Marker);
  VA_END (Marker);

  return Return;
}

/**
    function to perform the esc key operation

    @param string 

    @retval string

**/
CHAR16 *SkipEscCode(CHAR16 *String)
{
    CHAR16    *TempString;
    UINTN     i=0,j=0;

    if (NULL == String) {
        return NULL;    
    }
    // Check For escape Code and remove that from the string
    TempString = StrDup(String);
    while(String[i])
    {
        if((String[i] == 0x001b) && (String[i+1] == L'[')) // ESC Code
        {   
            while(String[i] != L'm')i++;
            i++;
        }
        TempString[j] = String[i];
        i++; j++;
    }
    TempString[j]=0;    

    return TempString;
}

/**
    function to dump and return the string

    @param String 

    @retval String

**/
CHAR16 *StrDup( CHAR16 *string )
{
    CHAR16 *text;

    if(string == NULL)
        return NULL;
    
    text = EfiLibAllocateZeroPool( (1 + EfiStrLen( string )) * sizeof(CHAR16) );
    if ( text != NULL )
        EfiStrCpy( text, string );

    return text;
}

/**
    function to convet a string to integer

    @param string UINT8 base

    @retval integer

**/
UINTN atoi_base(CHAR16 *string, UINT8 base)
{
    UINTN dval = 0;
    CHAR16 ch;

    if ( base == 10 )
    {
        while ( (ch = *string++ ) != 0x0)
        {
            if ( ch >= L'0' && ch <= L'9' )
                dval = dval * 10 + ch - L'0';
            else
                break;

        }
    }
    else if ( base == 16 )
    {
        while ( (ch = *string++) != 0x0 )
        {
            if ( ch >= L'0' && ch <= L'9' )
                dval = dval * 16 + ch - L'0';
            else if ( ch >= L'a' && ch <= L'f' )
                dval = dval * 16 + ch - L'a' + 10;
            else if ( ch >= L'A' && ch <= L'F' )
                dval = dval * 16 + ch - L'A' + 10;
            else
                break;
        }
    }
    else if( base == 32)
    {
        if(string[0]== L'-')
        {
            string++;
            while ( (ch = *string++ ) != 0x0)
            {
                if ( ch >= L'0' && ch <= L'9' )
                dval = dval * 10 + ch - L'0';
            else
                break;
            }
            dval= (INTN)((0xffffffffffffffff - dval )+ 0x1);
        }
        else
        {
            while ( (ch = *string++ ) != 0x0)
            {
                if ( ch >= L'0' && ch <= L'9' )
                dval = dval * 10 + ch - L'0';
            else
                break;
            }
        }
    }
    return dval;
}

/**
    Duplicates a String from CHAR8 to CHAR16

    @param string 

    @retval CHAR16 *
              

**/
CHAR16 *StrDup8to16( CHAR8 *string )
{
    CHAR16  *text;
    UINTN   i;

    text = EfiLibAllocateZeroPool( (1 + StrLen8( string )) * sizeof(CHAR16));
    if ( text != NULL )
    {
        i=0;
        while(text[i] = (CHAR16)string[i])
            i++;
    }

    return text;
}

/**
    Returns pointer to the PAGE_INFO struct of given PgIndex

    @param PgIndex Page index
    @param PageInfo Pointer to the page information struct

    @retval EFI_STATUS status - EFI_SUCCESS if successful, else EFI_ERROR
**/
EFI_STATUS _GetPageInfo(UINT32 PgIndex, PAGE_INFO **PageInfo)
{
  EFI_STATUS status = EFI_SUCCESS;

  if((gApplicationData == NULL) || (gPages == NULL))
  {
    status = EFI_NOT_READY;
    goto DONE;
  }

  if(PgIndex > gPages->PageCount)
  {
    status = EFI_INVALID_PARAMETER;
    goto DONE;
  }

  *PageInfo = (PAGE_INFO*)((UINTN)gApplicationData + gPages->PageList[PgIndex]);

DONE:
  return status;
}

BOOLEAN RTIfrUpdateVariableInfoWrapper(UINT32 ControlVariable, EFI_HANDLE Handle)
{
#if SETUP_RUNTIME_IFR_PROCESSING
    RTIfrUpdateVariableInfo(ControlVariable, Handle);
  return TRUE;
#else 
    return FALSE;
#endif
}

/**
    Finds the languages that the firmware supports.

    @param VOID

    @retval VOID

**/
VOID GetBootLanguages( VOID )
{
    CHAR16  *langString;
    CHAR8   *langCodes, *langPtr;
    UINTN   i, count, size = 0;

    langCodes = VarGetNvramName( L"LangCodes", &gEfiGlobalVariableGuid, NULL, &size );
    count = size /3;

    if ( size == 0 )
    {
        langCodes = VarGetNvramName( L"Lang", &gEfiGlobalVariableGuid, NULL, &size );
        if ( size != 0) {
            count = 1;
        }
        else {
            gLanguages = EfiLibAllocateZeroPool( sizeof(LANGUAGE_DATA) );
            if ( gLanguages == NULL )
                return;

            gLanguages[0].LangCode = StrDup8("eng");
            gLanguages[0].Unicode = StrDup(L"eng");
            gLangCount = 1;
            langString = Uefi21HiiGetStringLanguage( (VOID*)(UINTN)INVALID_HANDLE, 1, gLanguages[0].Unicode );
            gLanguages[0].Token = HiiAddString( gHiiHandle, langString );
            gLanguages[0].LangString = langString;
            return;
        }
    }

    gLanguages = EfiLibAllocateZeroPool( count * sizeof(LANGUAGE_DATA) );
    if ( gLanguages == NULL )
        return;

    for ( langPtr = langCodes, i = 0; i < count; i++, langPtr += 3 )
    {
        gLanguages[i].LangCode = EfiLibAllocateZeroPool( 4 * sizeof(CHAR8));
        MemCopy( gLanguages[i].LangCode, langPtr, 3 * sizeof(CHAR8) );
        gLanguages[i].Unicode = StrDup8to16(gLanguages[i].LangCode);
    }

    gLangCount = count;

    for ( langPtr = langCodes, i = 0; i < count; i++, langPtr += 3 )
    {
        langString = Uefi21HiiGetStringLanguage( (VOID*)(UINTN)INVALID_HANDLE, 1, gLanguages[i].Unicode );
        gLanguages[i].Token = HiiAddString( gHiiHandle, langString );
        gLanguages[i].LangString = langString;
    }

    MemFreePointer( (VOID **)&langCodes );
}

/**
    Set the variable inforamtion

    @param VarHandle Variable handle to process
    @param VarIndex Variable index in the gVariables->VariableList

    @retval VOID
**/
VOID SetCallBackControlInfo(EFI_HANDLE VarHandle, UINT32 VarIndex)
{
    static int ContextVar=0;
//    SETUP_DEBUG_UEFI_CALLBACK (  "[TSE] Entering SetCallBackControlInfo, HIIHandle =%x \n\n", VarHandle ); 
    if(VarHandle != NULL){ //If the varaible handle is valid, set handle and variable index
        if(ContextVar >= BROWSERCALLBACK_MAX_ENTRIES)
        {
//            SETUP_DEBUG_TSE ("\n[TSE] Too many nested Browser Callbacks!\n\n");
            ASSERT (0);    
        }
        gCurrVarHandle = VarHandle;
        gCurrVarIndex = VarIndex;
        //Use Stack to manage gCurrVarHandle and gCurrVarIndex
        gCallBackHandleStack[ContextVar] = gCurrVarHandle;
        gCallBackVarIndexStack[ContextVar] = gCurrVarIndex;
        ContextVar++;
        
    } else{ //Else reset global the varaible handle and variable index
    
        if(ContextVar == 0)
            return;
                    
        ContextVar--;
        
        if(ContextVar == 0)
        {
            gCurrVarHandle = (EFI_HANDLE)NULL;
            gCurrVarIndex = 0;
        }else{
            //Reset gCurrVarHandle and gCurrVarIndex to previous stack entry
            gCurrVarHandle = gCallBackHandleStack[ContextVar-1] ;
            gCurrVarIndex = gCallBackVarIndexStack[ContextVar-1] ;
        }
            
    }
    
//    SETUP_DEBUG_UEFI_CALLBACK (  "[TSE] Exiting SetCallBackControlInfo HIIHandle =%x \n\n", VarHandle ); 
}

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
