//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2008, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//*************************************************************************
// $Header: /SandBox/Lenovo_AmiScePkg_Driver/App/Utilities.c 1     5/29/13 11:38a Arund $
//
// $Revision: 1 $
//
// $Date: 5/29/13 11:38a $
//*************************************************************************
// Revision History
// ----------------
// $Log: /SandBox/Lenovo_AmiScePkg_Driver/App/Utilities.c $
//
// 1     5/29/13 11:38a Arund
//
// 14    12/18/12 4:05p Timothym
// Fix typo
//
// 13    12/18/12 12:45p Timothym
// Overlapping strcpy fails with new version of glibc on Linux.
// This causes mismatch of question prompts with leading spaces.
//
// 12    10/01/10 2:03p Sandhyar
// Took Code base from the lable 1014 and modified to display the entire
// GUID when given /v option(Only part of the GUID was displaying before).
// Also in the stderr in the output stream.
//
// 11    9/26/10 9:22a Timothym
// EFI Shell port.
//
// 10    9/07/10 3:10p Sandhyar
// Changed CRC32 from ULONG to UINT32 as it requires only 8 bytes.No
// typecast is required.
//
// 8     9/07/10 12:33p Sandhyar
// Changed CRC32 from ULONG to UINT32 as it requires only 8 bytes.No
// typecast is required.
//
// 1     8/23/10 2:50p Sandhyar
// Source SCEBSD
//
// 1     8/17/10 12:08p Sandhyar
// Latest SCE from win_common_uefi2.1
//
// 6     8/09/10 6:13p Sandhyar
// Made common source for Win and Linux
//
// 5     7/29/10 4:25p Sandhyar
// removed reading HII file
//
// 4     7/29/10 3:38p Sandhyar
// commented reading HII file part
//
// 3     7/28/10 4:57p Sandhyar
// Updated Version works import export provided HII DB file
//
// 1     7/02/10 2:11p Sandhyar
// Source for AMISCE
//
// 6     4/08/10 5:56a Hariharanr
// Restored the null check after fgets/fgetws in ReadLine/ReadUnicodeLine.
//
// 5     4/01/10 1:18p Hariharanr
// Addressed Tm's review comments for mem alloc null checks.
//
// 4     3/30/10 8:13a Hariharanr
// Added null checks after "calloc" and before "free".
//
// 3     3/17/10 12:22p Hariharanr
// 1) Changes for std/mfg defaults update feature.
// 2) Presanna's fixes for existing issues
//
// 2     1/20/10 9:27p Presannar
// Added fn GetCurrentSystemLanguage
// Modified Fn Signature GUID parameter type from GUID to UINT8 *
//
// 1     10/05/09 3:00p Presannar
// Initial Code Base
//
// 3     5/21/09 1:03p Presannar
// Addressed Code Revie comments
//
// 2     5/18/09 6:34p Presannar
// Added GetGuidAsUnicodeString and GetNextVariable
// modified GetGuidAsString to not return wide character
//
// 1     5/12/09 10:53a Presannar
// AMISCE Initial Source
//
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:  <This File's Name>
//
// Description:
// <A description of the file>
//
//<AMI_FHDR_END>
//*************************************************************************

//----------------------------------------------------------------------------
//extern "C"
//{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <ctype.h>
#include <wctype.h>
#include "Efi.h"
#include "Library/UefiLib.h"
//}

#define _EFI 1

#if defined(_EFI)
#include "os_hook.h"
#endif

#include "CRC32.h"
#include "Utilities.h"
#include "UEFI21Variable.h"
//----------------------------------------------------------------------------
#define STRING_GUID_LEN 36
#define WC_MAX_LENGTH 1024
#define MAX_LENGTH 1024

#define RFC_4646_LENGTH 3
#define ISO_639_2_LENGTH 5

#define UEFI_21_VERSION		0x021
#define UEFI_20_VERSION		0x020

//#define EFI_GLOBAL_VARIABLE \
//{0x8BE4DF61,0x93CA,0x11d2,0xAA,0x0D,0x00,0xE0,0x98,0x03,0x2B,0x8C}

//----------------------------------------------------------------------------
extern UINT8 gUEFIVersion; //UEFI version of BIOS
extern VARIABLE *GetVariableInfobyName(wchar_t *VariableName);


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   LTrim
//
// Description: Left trims the white space of a String
//
// Paramaters:  CHAR8* StrKey
//                  [in] The string that needs to be trimmed
//
// Return:      CHAR8* - String that is left trimmed.
//
// Modified:
//
// Referral(s):
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
char* LTrim(char* StrKey)
{

    char* TempStr = (char*)NULL;
    int i = 0;

    if ( ( StrKey != NULL ) && ( strlen( StrKey ) > 0 ) )
    {
        TempStr = StrKey;

        while ( isspace( *( TempStr + i ) ) )
        {
            TempStr++;
        }

        i = ( int )strlen( TempStr );
        // overlapping strcpy fails on later linux glibc
        //strcpy( StrKey,TempStr );
        memmove(StrKey, TempStr, strlen(TempStr));
        StrKey[i] = 0;
    }

    return StrKey;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   RTrim
//
// Description: Right trims the white space in a String
//
// Paramaters:  CHAR8* StrKey
//                  [in] The string that needs to be trimmed
//
// Return:      CHAR8* - String that is right trimmed.
//
// Modified:
//
// Referral(s):
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
char* RTrim(char *StrKey)
{
    int n = (int) (strlen(StrKey) - 1);

    while (n > 0)
    {
        if (!isspace(StrKey[n]))
        {
            *(StrKey + n + 1) = '\0';
            break;
        }
        else
        {
            n--;
        }
    }

    return StrKey;
}

/*CHAR16 *StrToLower(CHAR16 *String)
{
  CHAR16 *StrPtr = NULL;
  CHAR16 *Lower = ( CHAR16 * )calloc( wcslen( String ) + 1, sizeof( CHAR16 ));

  if( Lower )
  {
    for( StrPtr = String; *StrPtr; Lower++, StrPtr++ )
      *Lower = towlower( *StrPtr );
  }
  else
	throw(L"Memory Allocation Failure");

  return Lower;
}*/


wchar_t *StrToLower(wchar_t *String)
{
    wchar_t *StrPtr = (wchar_t *)NULL;
    wchar_t *Lower = ( wchar_t * )calloc( wcslen( String ) + 1, sizeof( wchar_t ));

    if ( Lower )
    {
        for ( StrPtr = String; *StrPtr; Lower++, StrPtr++ )
        {
            *Lower = (wchar_t)towlower( *StrPtr );
        }
    }
    else
    {
        throw (L"Memory Allocation Failure");
    }

    return Lower;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   UnicodeLTrim
//
// Description: Left trims the white space of a String
//
// Paramaters:  wchar_t* StrKey
//                  [in] The string that needs to be trimmed
//
// Return:      wchar_t* - String that is left trimmed.
//
// Modified:
//
// Referral(s):
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
wchar_t* UnicodeLTrim(wchar_t* StrKey)
{
    char* TempStr = ( char * )calloc( wcslen( StrKey ) + 1, sizeof( UINT8 ) );

    if (TempStr)
    {
        wcstombs( TempStr, StrKey, wcslen( StrKey ) );
        TempStr = LTrim(TempStr);
        mbstowcs( StrKey, TempStr, strlen( TempStr ) );
        free( TempStr );
    }
    else
    {
        throw (L"Memory Allocation Failure");
    }

    return StrKey;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   UnicodeRTrim
//
// Description: Right trims the white space in a String
//
// Paramaters:  wchar_t* StrKey
//                  [in] The string that needs to be trimmed
//
// Return:      wchar_t* - String that is right trimmed.
//
// Modified:
//
// Referral(s):
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
wchar_t* UnicodeRTrim(wchar_t *StrKey)
{
    char* TempStr = ( char * )calloc( wcslen( StrKey ) + 1, sizeof( UINT8 ) );

    if (TempStr)
    {
        wcstombs( TempStr, StrKey, wcslen( StrKey ) );
        TempStr = RTrim(TempStr);
        mbstowcs( StrKey, TempStr, strlen( TempStr ) );
        free( TempStr );
    }
    else
    {
        throw (L"Memory Allocation Failure");
    }

    return StrKey;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   GetGuidAsUnicodeString
//
// Description: Convert GUID to String Format
//
// Paramaters:  IN GUID *Guid - EFI GUID
//
// Return:      wchar_t* - GUID as String.
//
// Modified:
//
// Referral(s):
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
wchar_t *GetGuidAsUnicodeString( UINT8 *GuidPtr )
{
    wchar_t *GuidString = ( wchar_t * )calloc( ( STRING_GUID_LEN + 1 ) * sizeof( wchar_t ), sizeof( UINT8 ) );
    GUID *Guid = ( GUID * )GuidPtr;

    if (GuidString)
    {
        swprintf( GuidString, STRING_GUID_LEN, L"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                  Guid->Data1,
                  Guid->Data2,
                  Guid->Data3,
                  Guid->Data4[0],
                  Guid->Data4[1],
                  Guid->Data4[2],
                  Guid->Data4[3],
                  Guid->Data4[4],
                  Guid->Data4[5],
                  Guid->Data4[6],
                  Guid->Data4[7]
                );
    }
    else
    {
        throw (L"Memory Allocation Failure");
    }

    return GuidString;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   GetGuidAsString
//
// Description: Convert GUID to String Format
//
// Paramaters:  IN GUID *Guid - EFI GUID
//
// Return:      UINT8* - GUID as String.
//
// Modified:
//
// Referral(s):
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT8 *GetGuidAsString( GUID *Guid )
{
    UINT8 *GuidString = ( UINT8 * )calloc( ( STRING_GUID_LEN + 1 ) * sizeof( UINT8 ), sizeof( UINT8 ) );

    if (GuidString)
    {
        sprintf ( ( char * )GuidString, "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                  Guid->Data1,
                  Guid->Data2,
                  Guid->Data3,
                  Guid->Data4[0],
                  Guid->Data4[1],
                  Guid->Data4[2],
                  Guid->Data4[3],
                  Guid->Data4[4],
                  Guid->Data4[5],
                  Guid->Data4[6],
                  Guid->Data4[7]
                );
    }
    else
    {
        throw (L"Memory Allocation Failure");
    }

    return GuidString;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   SetNVRAMVariable
//
// Description: Set Variable to NVRAM
//
// Paramaters:  IN wchar_t *VariableName - Variable Name
//              IN GUID *VariableGuid - Variable GUID
//              IN UINT32 Attributes - Variable Attributes
//              IN UINT64 Size - Variable Data Size
//              IN UINT8 *Buffer - Variable Data
//
// Return:      int
//
// Modified:
//
// Referral(s):
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
int SetNVRAMVariable( wchar_t *VariableName, UINT8 *VariableGUID,
                      UINT32 Attributes, UINT64 Size, UINT8 *Buffer )
{

    EFI_STATUS Status;
    VARIABLE *Var;

    Var = GetVariableInfobyName(VariableName);

    Status = VarSetNVRAMVariable( Var, Buffer, (UINTN)Size );
//   return SCE_SetVariable(VariableName, ( GUID * )VariableGUID, Attributes, Size, Buffer );

    return (int)Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   GetNVRAMVariable
//
// Description: Get Variable from NVRAM
//
// Paramaters:  IN wchar_t *Name - Variable Name
//              IN GUID *VendorGuid - Variable GUID
//              OUT UINT32 *Attributes - Variable Attribute
//              OUT UINT64 *VarSize - Variable Size
//
// Return:      VOID* - Variable Data
//
// Modified:
//
// Referral(s):
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID *GetNVRAMVariable( wchar_t *VariableName, UINT8 *VendorGUID, UINT32 *Attributes, UINT64 *VarSize )
{
//    int Status;
    VOID *Buffer = NULL;
    VARIABLE *Var;

    Var = GetVariableInfobyName(VariableName);

    Buffer = VarGetNVRAMVariable( Var, VarSize );

    /**VarSize = 1;
    Buffer = ( VOID * )calloc( ( UINT32 )*VarSize, sizeof( UINT8 ));
    if( Buffer == NULL )
    	throw(L"Memory Allocation Failure");

    Status = SCE_GetVariable(VariableName, ( GUID * )VendorGUID, Attributes, VarSize, &Buffer );

    if( Status == SCE_BUFFER_TOO_SMALL )
    {
      free( Buffer );
      Buffer = ( VOID * )calloc( ( UINT32 )*VarSize, sizeof( UINT8 ));
      if( Buffer == NULL )
    	throw(L"Memory Allocation Failure");

      Status = SCE_GetVariable(VariableName, ( GUID * )VendorGUID, Attributes, VarSize, &Buffer );
    }

    if( Status != SCE_SUCCESS )
    {
      free( Buffer );
      Buffer = NULL;
      *VarSize = 0;
    }*/

    return Buffer;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   GetNextVariable
//
// Description: Get the Next Variable Info from NVRAM
//
// Paramaters:  OUT UINT64 *VariableNameSize - Variable Size
//              OUT wchar_t *VariableName - Variable Name
//              IN GUID *VariableGuid - Variable GUID
//
// Return:      VOID* - Variable Data
//
// Modified:
//
// Referral(s):
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
int GetNextVariable( UINT64 *VariableNameSize, wchar_t *VariableName, GUID *VariableGUID )
//int GetNextVariable( UINT64 *VariableNameSize, UINT16 *VariableName, GUID *VariableGUID )
{

    return SCE_GetNextVariableName(( UINT32 * )VariableNameSize, &VariableName, VariableGUID );

}

wchar_t *GetCurrentSystemLanguage()
{
    GUID Guid = EFI_GLOBAL_VARIABLE;
  UINTN Size = 0;
    UINT32 Attrib = 0;
    char *Lang = (char *)NULL;
    wchar_t *Language = (wchar_t *)NULL;

#if 0   // not supported
    if ( UEFI_20_VERSION == gUEFIVersion )
    {
        Lang = ( char * )HiiLib_VarGetNvramName( L"Lang", &Guid, &Attrib, (UINTN*)&Size );

        if ( Lang )
        {
            Language = ( wchar_t * )calloc( RFC_4646_LENGTH + 1, sizeof( wchar_t ));

            if (NULL == Language)
            {
                throw (L"Memory Allocation Failure");
            }

            mbstowcs( Language, Lang, RFC_4646_LENGTH);
            free( Lang );
        }
    }
    else if ( UEFI_21_VERSION == gUEFIVersion )
#endif
    {
    Lang = (char *)HiiLib_VarGetNvramName(L"PlatformLang", &Guid, &Attrib, &Size);

        if ( Lang )
        {
            Language = ( wchar_t * )calloc( ISO_639_2_LENGTH + 1, sizeof( wchar_t ));

            if (NULL == Language)
            {
                throw (L"Memory Allocation Failure");
            }

            mbstowcs( Language, Lang, ISO_639_2_LENGTH);
            free( Lang );
        }
    }

    return Language;
}
