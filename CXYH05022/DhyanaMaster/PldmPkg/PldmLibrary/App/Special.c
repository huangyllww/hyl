//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Pkwy, Norcross, Suite 200, Georgia 30093      **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//**********************************************************************
// $Header: /SandBox/Lenovo_AmiScePkg_Driver/App/Special.c 3     7/23/13 12:04p Arund $
//
// $Revision: 3 $
//
// $Date: 7/23/13 12:04p $
//
//*********************************************************************
// Revision History
// ----------------
// $Log: /SandBox/Lenovo_AmiScePkg_Driver/App/Special.c $
//
// 3     7/23/13 12:04p Arund
// Fixed build issues, and updated for exporting Mapping table and import
// using Mapping table
//
// 1     5/29/13 11:38a Arund
//
// 29    3/06/13 9:26a Abinayar
// Issue faced in linux and BSD with language control fixed.
//
// 28    3/01/13 2:30a Abinayar
// Review comments for EIP 116486 addressed.
//
// 27    2/28/13 11:49a Abinayar
// EIP 116486  & 116488  fixes added.
//
// 26    2/13/13 9:48a Abinayar
// [TAG]  		EIP114981
// [Category]  	Bug Fix
// [Severity]  	Minor
// [Symptom]  	Output language code in script instead of trying to render
// full language name
// [RootCause]  	Fwprintf not able to print the chinese characters in the
// file .
// [Solution]  	Instead of LangString we are writing the Langcode to the
// file. Since lang code is always in English
// [Files]  		Spcial.cpp
//
// 25    2/08/13 9:45a Tanayaj
// [TAG]	EIP114822
// [Category]	BUG FIX
// [Severity]	Normal
// [Symptom]	BIOS Strings being updated in SUBTITLES but not TITLES in SCE
// [RootCause]	SCE can fail to output a '%' character embedded in a BIOS
// String that is being used as a 'TITLE' in the HII Formsets.
// fprintf( AmisceData->VariablesFile, StrOutString ); is used to write
// the TITLE to the file. "%" is considered as format specifier
// and it is not printed in the file.
// [Solution]	changing as, fprintf( AmisceData->VariablesFile,
// "%s",StrOutString );, will resolve the issue.
// [Files]	AdvScript.cpp Special.cpp
//
// 24    2/08/13 4:56a Abinayar
// [TAG]  		EIP114785
// [Category]  	Bug Fix
// [Severity]  	Normal
// [Symptom]  	SCE exported language control shows English twice. Fails to
// output '*' for selected chinese dialect.
// [RootCause]  	The string pack having both en and en-US string pack. But
// Lang will be en-US in both scenarios. So it is printing twice.
// Root cause for Missing "*" is the string size we are having ,
// UEFI21_LANGUAGE_SIZE 5 , for chinese simplified it will have the Lang
// code as zh-chs. So it is failing.
//
// [Solution]  	we need to break the loop once the lang code matches.
// Modified the UEFI21_LANGUAGE_SIZE macro value as 6
// [Files]  		Special.cpp
// UEFI21Variable.h
//
// 23    2/01/13 2:45a Abinayar
// In AptioV Sugarbay platform System lanage token options are not updated
// in Script file, since the string pack having the language code as "en"
// and the variable having en-us , fix for that issue added.
//
// 22    11/08/12 9:37a Arund
// [TAG]	EIP106102
// [Category]	BUG FIX
// [Severity]	Minor
// [Symptom]	SCE boot option question overflows buffer and uses wrong case
// for hex digits when reading Bootnnnn variable.
// [RootCause]	Buffer not enough to store if there are more boot options.
// Core version from 4.6.4.1 does not support lower case to read Boot
// variable.
// [Solution]	Each Boot option is written directly to the file, instead
// storing in the buffer. Updated code to check both cases (lower and
// upper) to get Boot variable.
// [Files]	SRC\AMISCE\SRC\Win\App\Special.cpp
//
// 21    9/29/12 5:46a Abinayar
// Build issue in 32bit efi build resolved.
//
// 20    9/17/12 7:23a Tanayaj
// [TAG]  		EIP101279
// [Category]  	Defect
// [Severity]      Normal
// [Description]   In Aptio 5, Warning messages displyed in advanced
// import operation like "Warning : Unmatched question... prompt: Boot
// Option #,Token:10a".
// [Root Cause]    The flag "uParsed" was not being set when special
// controls were being updated in NVRAM. Moreover, the verification of
// Boot Order control was not being done.
// For that "uParsed" flag was becoming 0 and thus results in displaying
// "Unmatched questions..BOOT Option #" warning.
// [Solution]      In  _VerifySpecialSetupQuestion() in Special.cpp, added
// proper comparison of setup questions under "case BOOT_ORDER_CONTROL:"
// and in Import21AdvScriptSettings() in AdvScript.cpp, set the flag
// "uParsed" for the updated special control.
// [Files]         Special.cpp AdvScript.cpp
//
// 19    7/19/12 5:41p Timothym
// [TAG]                EIP95654
// [Category]          BUG FIX
// [Severity]           Normal
// [Symptom]         [ASUS-DT] SCEWIN not work under Win8
// [Root Cause]     BIOS fails to deliver PlatformLang variable, causing a
// reference to a null pointer.
// [Solution]           Supress output of Language Control when current
// language not available
// [Files]                 Special.cpp
//
// 18    4/10/12 2:58p Timothym
// remove debugging printf
//
// 17    4/10/12 11:12a Tanayaj
// EIP 87085: solved the problem with GetLanguageOptions() due to misuse
// of Swprintf() to concatenate strings.
//
// 16    4/10/12 5:12a Tanayaj
// [TAG]              EIP87085
//
// [Category]       Defect
// [Severity]        NORMAL
// [Description]   SCE language control sets wrong language.SCE exports
// correct language selection but import sets back to wrong values.
// In a BIOS with english, french, and german, if the current setting is
// german and the script selects french, then after import the language
// is set to english.
// [Solution]        Modified the do-while loop in GetLanguageOptions()
// function.
// [Files]             App/Special.cpp
//
// 14    8/31/11 3:02p Timothym
// Avoid setting variable when device not in order list.
//
// 13    8/31/11 10:14a Timothym
// [TAG]      EIP60402
//
// [Category]   BUG FIX
// [Severity]   NORMAL
// [Symptom]    SCEWIN64 application crashing on export operation after
// performing import operation with specific advanced file(attached script
// file str1.txt).
// [RootCause]  When PXE boot is enabled the BIOS creates a second Legacy
// order list. Code for import of legacy device order corrupts the
// variable LegacyDevOrder causing SCE to fail on next export.
// [Solution]   Revise code to handle more than one order list.
// [Files]      App/Special.cpp
//
//*********************************************************************

//extern "C"
//{
#include "string.h"
#include "stdlib.h"
#include <stdio.h>
#include <wchar.h>
#include "Efi.h"
#include "Wrapper.h"
#include "Library/UefiLib.h"
#include "AmiDxeLib.h"
#include "AmiLib.h"
//}
#define _EFI 1

#include "types.h"
//#include "PldmApi.h"
#include "AdvScript.h"
#if  defined(_EFI)
#include "os_hook.h"
#endif
//#ifdef WIN32
//#include "wchar.h"
//#endif
#if defined(_EFI)
#include "../LIB/UEFI20HIIParser.h"
#include "../LIB/UEFI20Variable.h"
#include "../LIB/UEFI21Variable.h"
#include "../LIB/UEFI20StringManagement.h"
#include "../LIB/UEFI20DevicePath.h"
#endif

#include "Special.h"

#define RFC_3066_ENTRY_SIZE (42 + 1)

extern VOID _Nwstrcpy(wchar_t *WideName, unsigned short *NarrowName);
extern BOOLEAN g_QuietMode;
extern char* RTrim( char *StrKey );
extern char* LTrim( char *StrKey );

#if defined(_LNX) || defined(_BSD)
extern wchar_t* wcstok(wchar_t* Line, const wchar_t* delim);
//extern VOID _Nwstrcpy(wchar_t *WideName, unsigned short *NarrowName);
#endif

typedef struct _BOOT_OPTION
{
    UINT32	Active;
    UINT16	PathLength;
    CHAR16	Name[1];
    //EFI_DEVICE_PATH_PROTOCOL	DevicePath;
    //UINT8	LoadOptions[];
}
BOOT_OPTION;

LANGUAGE_DATA *gLanguages;
UINT32 gLangCount;

UINT8 *gBootOrder;

BOOT_OPTION **gBootOptions;
UINT64 gBootCount;

UINT8 *gLegacyDevOrder;
UINT64 gLegacyDevOrderCount;

extern VOID *GetNVRAMVariable( wchar_t *Name, UINT8 *VendorGuid, UINT32 *Attributes,
                               UINT64 *VarSize );
extern int SetNVRAMVariable( wchar_t *VariableName, UINT8 *VariableGUID,
                             UINT32 Attributes, UINT64 Size, UINT8 *Buffer );
extern wchar_t *PldmGetHiiString( UINT8 *DataTable, UINT8 *HiiData, UINT16 Token, BOOLEAN MapLang );

extern UINT32 GetUefi21HiiLanguages( UINT8 *PackageList, VOID* HiiPackage, LANGUAGE_DATA **Language );

extern UINT16 GetUEFI21LangDataWidth();

extern UINT8 gUEFIVersion;
extern BOOLEAN gMapLang ; //True , if /lang switch is present
VOID _SceGetNextLanguage(CHAR8 **LangCode, wchar_t *Lang)
{
    UINT32  Index;
    CHAR8  *StringPtr;
//  wchar_t *LangPtr = Lang;

    if (LangCode == NULL || *LangCode == NULL)
    {
        *Lang = 0;
        return;
    }

    Index = 0;
    StringPtr = *LangCode;

    while (StringPtr[Index] != 0 && StringPtr[Index] != ';')
    {
        Index++;
    }

    mbstowcs( Lang, ( char * )StringPtr, Index );
    //memcpy(Lang, StringPtr, Index);
    Lang[Index] = 0;

    if (StringPtr[Index] == ';')
    {
        Index++;
    }

    *LangCode = StringPtr + Index;
}

BOOLEAN _VerifySpecialSetupQuestion( SETUP_QUESTION_LIST *SetupQuestion, UINT8 *DataTable, UINT8 *HIIDBData, CONTROL_INFO *Control )
{
    wchar_t *PromptStr = (wchar_t *)NULL;
    wchar_t *SpecialPrompt = (wchar_t *)NULL;
    char *TempStr = (char *)NULL;
    char TempString[MAX_LENGTH];
//  UINT16 Key = 0;
    BOOLEAN Flag = FALSE;
    UINT16 KeyValue = 0;
    wchar_t *Pos;

    // Check if matching SetupQuestion
    PromptStr = PldmGetHiiString( DataTable, HIIDBData, Control->ControlPrompt , FALSE);
    SpecialPrompt = PromptStr;
    KeyValue = GetSpecialKey( Control->ConditionalVariable[0] );

    if ( ( PLATFORM_LANGUAGE_CONTROL != KeyValue ) && ( LANGUAGE_CONTROL != KeyValue ) )
    {
        SpecialPrompt = wcstok( PromptStr, L"%d", &Pos );
    }

    TempStr = ( char * )calloc( wcslen( SpecialPrompt ) + 1, sizeof( char ));

    if ( TempStr )
    {
        wcstombs( TempStr, SpecialPrompt, wcslen( SpecialPrompt ));
        TempStr = RTrim( LTrim( TempStr ) );

        switch ( KeyValue )
        {
            case LANGUAGE_CONTROL:
            case BOOT_ORDER_CONTROL:
                Flag = strcmp( TempStr, SetupQuestion->spScriptVal.cSetupPrompt ) == 0 ? TRUE : FALSE;
                break;

            case PLATFORM_LANGUAGE_CONTROL:
                Flag = strcmp( TempStr, SetupQuestion->spScriptVal.cSetupPrompt ) == 0 ? TRUE : FALSE;
                break;

            case LEGACY_DEV_ORDER_CONTROL:
                sprintf( TempString, " %s", TempStr );
                Flag = strstr( SetupQuestion->spScriptVal.cSetupPrompt, TempString ) != NULL ? TRUE : FALSE;
                break;

            default:
                break;
        }

        free(TempStr);
    }
    else
    {
        if (PromptStr)
        {
            free(PromptStr);
        }

        throw (L"Memory Allocation Failure");
    }

    if (PromptStr)
    {
        free(PromptStr);
    }

    return Flag;
}

UINT32 UpdateLanguageCodes(UINT32 ConditionalVariable)
{
    UINT8 *LangCodes = (UINT8 *)NULL;
    UINT8 *LangCodes1 = (UINT8 *)NULL;
    UINT32 Count = 0;
    wchar_t Language[RFC_3066_ENTRY_SIZE] = {0};

    LangCodes1 = LangCodes = GetLangCodes( ConditionalVariable );
    _SceGetNextLanguage( (CHAR8 **)&LangCodes, Language );

    while ( (int)NULL != *Language )
    {
        _SceGetNextLanguage((CHAR8 **) &LangCodes, Language );
        Count++;
    }

    gLanguages = ( LANGUAGE_DATA * )calloc( Count + 1 , sizeof( LANGUAGE_DATA ) );

    if ( NULL != gLanguages )
    {
        Count = 0;
        _SceGetNextLanguage( (CHAR8 **)&LangCodes1, Language );

        while ( (int)NULL != *Language )
        {
            gLanguages[Count].LangCode = (wchar_t *)calloc(wcslen(Language) + 1, sizeof(wchar_t));
            wcscpy(gLanguages[Count].LangCode , Language);
            _SceGetNextLanguage( (CHAR8 **)&LangCodes1, Language );
            Count++;
        }
    }

    return Count ;
}

wchar_t *GetLanguageOptions(UINT8 *DataTable, UINT8 *HIIDb, CONTROL_INFO *Control)
{
    wchar_t *StrOutString = ( wchar_t * )calloc( MAX_LENGTH, sizeof( UINT16 ));
    UINT8 *Language = (UINT8 *)NULL;
    UINT8 *LangCodes = (UINT8 *)NULL;
//  UINT32 Index = 0;
    wchar_t Lang[RFC_3066_ENTRY_SIZE] = {0};
    UINT16 LangCount = 0;
    wchar_t CurrLang[MAX_LENGTH] = {0};

    if (NULL == StrOutString)
    {
        throw (L"Memory Allocation Failure");
    }

    if ( gLanguages == NULL )
    {
        gLangCount = UpdateLanguageCodes(Control->ConditionalVariable[0]);
    }

    Language = SceGetQuestionValue( (UINT16)Control->VariableStorage, Control->DataOffset, GetLangDataWidth() );

    if (Language == 0)
    {
        if(StrOutString)
            free(StrOutString);
        return 0;
    }

    LangCodes = GetLangCodes( Control->ConditionalVariable[0] );

    _SceGetNextLanguage( (CHAR8 **)&LangCodes, Lang );

    do
    {
        mbstowcs( CurrLang, ( char * )Language, strlen( ( char * )Language ));

        if ( wcscmp( Lang, CurrLang ) == 0 )
        {
#if 0
            swprintf(StrOutString, MAX_LENGTH, L"%"_S L"*[%02X]%"_S, StrOutString, LangCount, gLanguages[LangCount].LangCode );
#else
            swprintf(StrOutString + wcslen(StrOutString), MAX_LENGTH, L"*[%02X]%"_S, LangCount, gLanguages[LangCount].LangCode );
#endif
        }
        else
        {
#if 0
            swprintf(StrOutString, MAX_LENGTH, L"%"_S L"[%02X]%"_S, StrOutString, LangCount, gLanguages[LangCount].LangCode );
#else
            swprintf(StrOutString + wcslen(StrOutString), MAX_LENGTH, L"[%02X]%"_S, LangCount, gLanguages[LangCount].LangCode );
#endif
        }

        LangCount++;
        _SceGetNextLanguage( (CHAR8 **)&LangCodes, Lang );

        if ( *Lang )
        {
#if 0
            swprintf(StrOutString, MAX_LENGTH, L"%"_S L"\n\t\t", StrOutString );
#else
            swprintf(StrOutString + wcslen(StrOutString), MAX_LENGTH, L"\n\t\t");
#endif
        }
    } while ( *Lang != (int)NULL );

    return StrOutString;
}

VOID _PopulateGlobalBootOptions()
{
    UINT8 *BootOrder = (UINT8 *)NULL;
    UINT64 Size = 0;
    UINT16 Index = 0;
    UINT8 *BootXXXX = (UINT8 *)NULL;
    UINT64 BootSize = 0;
    wchar_t BootStr[MAX_LENGTH] = {0};
    UINT32 Attributes = 0;
    GUID gEfiGlobalVariableGuid = EFI_GLOBAL_VARIABLE_GUID;

    // Retrieve the BootOrder Variable
    gBootOrder = GetVariableByName( L"BootOrder", &Size );
    BootOrder = gBootOrder;

    if ( BootOrder != NULL )
    {
        // Allocate gBootOptions
        // use cast because EFI lacks 64 bit divide
        gBootCount = (UINT32)Size / sizeof( UINT16 );
        gBootOptions = ( BOOT_OPTION ** )calloc( sizeof( BOOT_OPTION * ), ( size_t )gBootCount);

        if ( gBootOptions != NULL )
        {
            BOOT_OPTION **BootOptions = gBootOptions;

            // For each Boot Option retrieve the BootXXXX varaible
            for ( Index = 0; Index < Size; Index += 2 )
            {
                DEBUG((DEBUG_ERROR, "\nBootSize: %x", Size));
                // UEFI spec defines Boot Order Index as Upper case eg. "Boot000A".
                // But Aptio uses it in lower case. Change L"Boot%02x%02x" to L"Boot%02X%02X"
                // once Aptio changes it.
                BootSize = 0;
                Attributes = 0;
                swprintf( BootStr, MAX_LENGTH, L"Boot%02X%02X", BootOrder[Index + 1], BootOrder[Index]);
                BootXXXX = VarGetNvramName( BootStr, &gEfiGlobalVariableGuid, &Attributes, (UINTN*)&BootSize );

                if ( BootXXXX && BootSize != 0 )
                {
                    // If Valid Add to gBootOptions
                    memcpy( BootOptions, &BootXXXX, sizeof( BOOT_OPTION * ));
                    BootOptions++;
                }
                else
                {
                    swprintf( BootStr, MAX_LENGTH, L"Boot%02x%02x", BootOrder[Index + 1], BootOrder[Index]);
                    BootXXXX = ( UINT8 * )VarGetNvramName( BootStr, &gEfiGlobalVariableGuid, &Attributes, (UINTN*)&BootSize );

                    if ( BootXXXX && BootSize != 0 )
                    {
                        // If Valid Add to gBootOptions
                        memcpy( BootOptions, &BootXXXX, sizeof( BOOT_OPTION * ));
                        BootOptions++;
                    }
                }
            }
        }
        else
        {
            throw (L"Memory Allocation Failure");
        }
    }
}

BOOLEAN ImportLanguageControlToNVRAM( SETUP_QUESTION_LIST *SetupQuestion, UINT8 *DataTable, UINT8 *HIIDb, CONTROL_INFO *Control )
{
    BOOLEAN Flag = FALSE;
//  UINT8 *Language = (UINT8 *)NULL;
//  UINT8 *LangCodes = (UINT8 *)NULL;
//  UINT32 Index = 0;
    CHAR8 *Lang = (CHAR8 *)NULL;
    UINT32 Length = 0;

    if ( SetupQuestion )
    {
        // Verify that the setup question is actually pointing to Language
        Flag = _VerifySpecialSetupQuestion( SetupQuestion, DataTable, HIIDb, Control );

        if ( Flag == FALSE )
        {
            goto DONE;
        }
    }

    if ( (SetupQuestion->spStatus.uOptimalSetting || SetupQuestion->spStatus.uFailSafeSetting) && (FALSE == g_QuietMode) )
    {
        wprintf(L"Warning : Special controls should not have BIOS/MFG defaults entry in the script file \n");
    }

    if ( gLanguages == NULL )
    {
        // Populate gLanguages
        gLangCount = UpdateLanguageCodes(Control->ConditionalVariable[0]);
    }

    // Update Selected Language to Variable
    if ( SetupQuestion->spScriptVal.bValue < gLangCount )
    {
        // Update Language Variable with gLanguages
        Length = ( UINT32 )wcslen( gLanguages[(UINT32)SetupQuestion->spScriptVal.bValue].LangCode );

        Lang = ( CHAR8 * )calloc( sizeof( CHAR8 ), GetLangDataWidth() );

        if ( Lang == NULL )
        {
            throw (L"Memory Allocation Failure");
        }

        wcstombs( ( char * )Lang, gLanguages[(UINT32)SetupQuestion->spScriptVal.bValue].LangCode, Length);
        SceSetQuestionValue( (UINT16)Control->VariableStorage, Control->DataOffset, GetLangDataWidth(), ( UINT8 * )Lang );

        if ( Lang )
        {
            free( Lang );
        }
    }
    else if ( FALSE == g_QuietMode )
    {
        wprintf( L"Warning: Invalid Index value for Language Option Control\n" );
    }

DONE:
    return Flag;
}

BOOLEAN ImportBootOrderContrlToNVRAM( SETUP_QUESTION_LIST *SetupQuestion, UINT8 *DataTable, UINT8 *HIIDb, CONTROL_INFO *Control )
{
    BOOLEAN Flag = FALSE;
    UINT8 *CurrentValue = (UINT8 *)NULL;
    GUID LegacyDevGuid = LEGACY_DEV_ORDER_GUID;
    UINT64 TempDevOrderSize = 0;
    UINT32 Attributes = 0;
    UINT8 DeviceType = 0;
    LEGACY_ORDER_ENTRY *LegacyDevPtr = (LEGACY_ORDER_ENTRY *)NULL;
    LEGACY_ORDER_ENTRY *TempLegacyDevPtr = (LEGACY_ORDER_ENTRY *)NULL;
    UINT64 DevOrderSize = 0;
    UINT8 *TempDevOrder = (UINT8 *)NULL;	// working buffer for order entry from script
    LEGACY_ORDER_ENTRY *TempDevPtr;
    UINT32 TempSize;       // size of order entry from script
    UINT64 OrderCount = 0;
    UINT16 Index = 0;
    UINT8 *gTempLegacyDevOrder;
//  UINT32 OrderIndex= 0;
    UINT32 ptrIndex = 0;
    UINT16 Length = 0;
    wchar_t BootName[MAX_LENGTH] = {0};
    BOOT_OPTION *BootOption = (BOOT_OPTION *)NULL;

    UINT64 LegacyDevOrderCount = 0;
    UINT8 *LegacyDevOrder;
    UINT16 VariableID;
    wchar_t  Buffer[MAX_LENGTH] = {0};

    if ( SetupQuestion )
    {
        //Populate the boot options list
        if (gBootOptions == NULL)
        {
            _PopulateGlobalBootOptions();
        }

        // Verify that the setup question is actually pointing to OrderedList Setup Question
        Flag = _VerifySpecialSetupQuestion( SetupQuestion, DataTable, HIIDb, Control );

        if ( Flag )
        {
            if (gBootCount == (UINT64)SetupQuestion->spScriptVal.bValue)
            {
                if ((SetupQuestion->spStatus.uOptimalSetting || SetupQuestion->spStatus.uFailSafeSetting) && (FALSE == g_QuietMode) )
                {
                    wprintf(L"Warning : Special controls should not have BIOS/MFG defaults entry in the script file \n");
                }

                SceSetQuestionValue( (UINT16)Control->VariableStorage, Control->DataOffset, (UINT16)SetupQuestion->spScriptVal.bValue * sizeof( UINT16 ), ( UINT8 * )SetupQuestion->spScriptVal.bOrder );

//Below Code added to Rearrange the LEGACYDEVORDER in sync with the Boot Order list.
                if (gBootOptions)
                {
                    FreeBootOptions();
                }

                //Populate the boot options list
                if (gBootOptions == NULL)
                {
                    _PopulateGlobalBootOptions();
                }

                LegacyDevOrder = GetVariableByName( L"LegacyDevOrder", &LegacyDevOrderCount );
                VariableID = GetVariableIndexByName(L"LegacyDevOrder");

                LegacyDevPtr = ( LEGACY_ORDER_ENTRY * )LegacyDevOrder;
                DevOrderSize = TempDevOrderSize = LegacyDevOrderCount;

                if ( LegacyDevPtr )
                {
                    gTempLegacyDevOrder = (UINT8 *)malloc((size_t)DevOrderSize);

                    //Rearrange the gLegacyDevOrder in sync with the Boot Order List and store it in gTempLegacyDevOrder.
                    for ( Index = 0; Index < gBootCount; Index++ )
                    {
                        BootOption = gBootOptions[Index];

                        if ( BootOption )
                        {
                            UINT8 *DevicePath = (UINT8 *)NULL;
                            size_t length = 0;
                            _Nwstrcpy(BootName, BootOption->Name);
                            length = wcslen(BootName);
                            memset(BootName, 0, (MAX_LENGTH * sizeof(wchar_t)) );

                            TempLegacyDevPtr = ( LEGACY_ORDER_ENTRY * )LegacyDevOrder;

                            DevicePath = ( UINT8 * )(( UINT8 * )BootOption->Name + ( length + 1 ) * sizeof( UINT16 ));

                            if ( GetDevicePathType( DevicePath ) == 0x05 )
                            {
                                DeviceType =  (UINT8)GetBBSDevicePathDeviceType( DevicePath );

                                // find the order list for this device type in legacy lists
                                ptrIndex = 0;

                                do
                                {
                                    if ( TempLegacyDevPtr->DeviceType == DeviceType )
                                    {
                                        Length = sizeof( UINT32 ) + TempLegacyDevPtr->Length;

                                        TempDevOrder = (UINT8 *)calloc(sizeof(UINT8), TempSize = Length);
                                        if ( TempDevOrder == NULL )
                                        {
                                            throw (L"Memory Allocation Failure");
                                        }
										
										TempDevPtr = (LEGACY_ORDER_ENTRY *)TempDevOrder;
                                        
                                        //Copy DeviceType
                                        TempDevPtr->DeviceType = TempLegacyDevPtr->DeviceType;

                                        TempDevPtr->Length = TempLegacyDevPtr->Length;

                                        memcpy(TempDevOrder + sizeof(UINT32) + sizeof(UINT16), (char *)TempLegacyDevPtr + sizeof(UINT32) + sizeof(UINT16), TempLegacyDevPtr->Length - sizeof(UINT16));

                                        break;
                                    }

                                    // Update Index and Move to Next Order
                                    ptrIndex += sizeof( UINT32 ) + TempLegacyDevPtr->Length;
                                    TempLegacyDevPtr = ( LEGACY_ORDER_ENTRY * )(( UINT8 * )TempLegacyDevPtr + sizeof( UINT32 ) + TempLegacyDevPtr->Length );
                                } while ( ptrIndex < DevOrderSize );

                                if (NULL != TempDevOrder)
                                {
                                    memcpy(gTempLegacyDevOrder + OrderCount, TempDevOrder, Length);
                                    free(TempDevOrder);
                                }

                                TempDevOrder = NULL;
                                OrderCount += Length;
                            }
                        }
                    }

                    if (OrderCount == LegacyDevOrderCount && gTempLegacyDevOrder)
                    {
                        //Store the Rearranged list in global buffer.
                        SceSetQuestionValue( VariableID, 0, ( UINT16 )OrderCount, gTempLegacyDevOrder  );

                        if (LegacyDevOrder)
                        {
                            free (LegacyDevOrder);
                        }

                    }

                    if (gTempLegacyDevOrder)
                    {
                        free(gTempLegacyDevOrder);
                    }
                }
            }
            else
            {
//		  fwprintf(stderr,L"\nWarning : Setup question '%s' is not imported, as Boot count in the script file mismatches with boot count in NVRAM",SetupQuestion->spScriptVal.cSetupPrompt);
            }
        }

        if (gBootOptions)
        {
            FreeBootOptions();
        }
    }

    return Flag;
}

BOOLEAN ImportLegacyDevOrderControlToNVRAM( SETUP_QUESTION_LIST *SetupQuestion, UINT8 *DataTable, UINT8 *HIIDb, CONTROL_INFO *Control )
{
    LEGACY_ORDER_ENTRY *LegacyDevPtr = (LEGACY_ORDER_ENTRY *)NULL;
    UINT64 DevOrderSize = 0;
    UINT8 *NewOrder = 0;       // buffer for variable update
    UINT8 *TempDevOrder = (UINT8 *)NULL;	// working buffer for order entry from script
    LEGACY_ORDER_ENTRY *TempDevPtr = (LEGACY_ORDER_ENTRY *)NULL;
    UINT32 TempSize = 0;       // size of order entry from script
    UINT64 VarSize = 0;
    GUID LegacyDevGuid = LEGACY_DEV_ORDER_GUID;
    UINT64 TempDevOrderSize = 0;
    UINT32 Attributes = 0;
    UINT8 DeviceType = 0;
    UINT16 Index = 0;
    wchar_t *DeviceTypeStr = (wchar_t *)NULL;
    CHAR8 *DPString = (CHAR8 *)NULL;
    UINT16 Length = 0;
    UINT16 OrderIndex = 0;

    UINT64 LegacyDevOrderCount = 0;
    UINT8 *LegacyDevOrder;

    /*
    Limitations:
      1. An order may be replaced but neither created nor deleted.
      2. Only first occurence in script ends up in a call to this function.
         This is because a single control generates what look like multiple
         controls in the script.
      3. Changes that result in variable length change are ignored.
      4. LegacyDevChecksum variable is not updated.
    */

    BOOLEAN Flag = FALSE;

    if ( SetupQuestion )
    {
        // Verify that the setup question is actually pointing to LegacyDevOrder Setup Question
        Flag = _VerifySpecialSetupQuestion( SetupQuestion, DataTable, HIIDb, Control );

        if ( Flag )
        {
            if ((SetupQuestion->spStatus.uOptimalSetting || SetupQuestion->spStatus.uFailSafeSetting) && (FALSE == g_QuietMode) )
            {
                wprintf(L"Warning : Special controls should not have BIOS/MFG defaults entry in the script file \n");
            }

            LegacyDevOrder = GetVariableByName( L"LegacyDevOrder", &LegacyDevOrderCount );

            LegacyDevPtr = ( LEGACY_ORDER_ENTRY * )LegacyDevOrder;
            DevOrderSize = TempDevOrderSize = LegacyDevOrderCount;

            if ( LegacyDevPtr )
            {
                // Get DeviceType String
                for ( Index = 1; Index <= GetBBSDeviceCount(); Index++ )
                {
                    // Compare DeviceType String with Setup Question Prompt
                    DeviceTypeStr = GetDevicePathTypeString( Index );

                    DPString = ( CHAR8 * )calloc( sizeof( CHAR8 ), wcslen( DeviceTypeStr ) + 1 );

                    if ( DPString == NULL )
                    {
                        throw (L"Memory Allocation Failure");
                    }

                    wcstombs( ( char * )DPString, DeviceTypeStr, wcslen( DeviceTypeStr ));

                    // Check if DevicePath String is part of Setup Question Prompt
                    if ( strstr( SetupQuestion->spScriptVal.cSetupPrompt, ( char * )DPString ) != NULL )
                    {
                        if ( DPString )
                        {
                            free( DPString );
                        }

                        break;
                    }

                    if ( DPString )
                    {
                        free( DPString );
                    }
                }

                // Check if Index is Valid
                if ( Index == GetBBSDeviceCount() )
                {
                    // device type not found in table
                    goto DONE;
                }

                DeviceType = GetBBSDeviceTypeByIndex( Index - 1 );

                // Iterate LegacyDevOrder for DeviceType
                Index = 0;

                do
                {
                    if ( LegacyDevPtr->DeviceType == DeviceType )
                    {
                        // For matching Dev Path get setup question order and enter in temp LegacyDevOrder
                        OrderIndex = 0;

                        // Length is SetupQuestion->spScriptVal.bValue + size of Length field
                        Length = ( (UINT16) SetupQuestion->spScriptVal.bValue + 1 ) * sizeof( UINT16 );

                        TempDevOrder = (UINT8 *)calloc(sizeof(UINT8), TempSize = Length + sizeof(UINT32));
                        if ( TempDevOrder == NULL )
                        {
                            throw (L"Memory Allocation Failure");
                        }
						
						TempDevPtr = (LEGACY_ORDER_ENTRY *)TempDevOrder;
									               
                        //Copy DeviceType
                        TempDevPtr->DeviceType = LegacyDevPtr->DeviceType;

                        TempDevPtr->Length = Length;

                        for ( OrderIndex = 0; OrderIndex < SetupQuestion->spScriptVal.bValue; OrderIndex++ )
                        {
                            // Copy DeviceOrder (structure not packed!)
                            ((UINT16 *)(TempDevOrder + sizeof(UINT32) + sizeof(UINT16)))[OrderIndex] = SetupQuestion->spScriptVal.bOrder[OrderIndex];
                        }

                        break;
                    }

                    // Update Index and Move to Next Order
                    Index += sizeof( UINT32 ) + LegacyDevPtr->Length;
                    LegacyDevPtr = ( LEGACY_ORDER_ENTRY * )(( UINT8 * )LegacyDevPtr + sizeof( UINT32 ) + LegacyDevPtr->Length );
                } while ( Index < DevOrderSize );

                // Save Value back to NVRAM
                if ( Index < DevOrderSize )
                {
                    //SetNVRAMVariable( L"LegacyDevOrder", ( UINT8 * )&LegacyDevGuid, Attributes, TempDevOrderSize, TempLegacyDevOrder );
                    // compute size of new variable
                    VarSize = TempDevPtr->Length - LegacyDevPtr->Length;
                    VarSize += DevOrderSize;
                    NewOrder = (UINT8 *)malloc((size_t)VarSize);

                    if ( NewOrder == NULL )
                    {
                        throw (L"Memory Allocation Failure");
                    }

                    // copy data before updated listorder
                    memcpy(NewOrder, LegacyDevOrder, Index);
                    // copy updated listorder
					if(TempDevOrder != NULL)
						memcpy(NewOrder + Index, TempDevOrder, TempSize);
                    // copy data after updated listorder
                    memcpy(NewOrder + Index + TempSize, (char *)LegacyDevPtr + LegacyDevPtr->Length + sizeof(UINT32), (size_t)(DevOrderSize - LegacyDevPtr->Length - sizeof(UINT32) - Index));

                    // SetQuestion value cannot change the size of a variable
                    // so ignore for now
                    if (VarSize == LegacyDevOrderCount)
                    {
                        SceSetQuestionValue( (UINT16)Control->VariableStorage, Control->DataOffset, ( UINT16 )VarSize, NewOrder  );
                    }

                    if ( TempDevOrder )
                    {
                        free( TempDevOrder );
                    }
                }
                else
                {
                    // didn't find in variable
                    Flag = FALSE;
                }
            }
        }
    }

DONE:

    if (NewOrder)
    {
        free(LegacyDevOrder);
        LegacyDevOrder = NewOrder;
        LegacyDevOrderCount = VarSize;
    }

    return Flag;
}

UINT32 GetHiiLanguages( UINT8 *IFRDataTable, VOID* HiiPackage, LANGUAGE_DATA **Language )
{
    UINT32 LangCount;
#if 0

    if ( UEFI_20_VERSION == gUEFIVersion )
    {
        LangCount = GetUefi20HiiLanguages( IFRDataTable, HiiPackage, Language );
    }
    else if ( UEFI_21_VERSION == gUEFIVersion )
#endif
        LangCount = GetUefi21HiiLanguages( IFRDataTable, HiiPackage, Language );

    return LangCount;

}

UINT16 GetLangDataWidth()
{

#if 0

    if ( UEFI_20_VERSION == gUEFIVersion )
    {
        return GetUEFI20LangDataWidth();
    }
    else
    {
#endif
        return GetUEFI21LangDataWidth();
#if 0
    }

#endif
}

VOID FreeBootOptions()
{
    int i;

    for (i = 0; i < gBootCount; i++)
    {
        if (gBootOptions[i])
        {
            free(gBootOptions[i]);
        }
    }

    if (gBootOptions)
    {
        free(gBootOptions);
        gBootOptions = NULL;
    }
}


//;****************************************************************************;
//;****************************************************************************;
//;**                                                                        **;
//;**            (C)Copyright 1985-2011, American Megatrends Inc.            **;
//;**                                                                        **;
//;**                          All Rights Reserved.                          **;
//;**                                                                        **;
//;**          5555 Oakbrook Pkwy, Norcross, Suite 200, Georgia 30093        **;
//;**                                                                        **;
//;**                         Phone-(770)-246-8600.                          **;
//;**                                                                        **;
//;****************************************************************************;
//;****************************************************************************;
