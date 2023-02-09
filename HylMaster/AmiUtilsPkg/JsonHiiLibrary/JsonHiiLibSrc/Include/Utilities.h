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
// $Header: /SandBox/Lenovo_AmiScePkg_Driver/Include/Utilities.h 1     5/29/13 11:40a Arund $
//
// $Revision: 1 $
//
// $Date: 5/29/13 11:40a $
//*************************************************************************
// Revision History
// ----------------
// $Log: /SandBox/Lenovo_AmiScePkg_Driver/Include/Utilities.h $
// 
// 1     5/29/13 11:40a Arund
//
// 4     8/26/10 2:10a Paulr
// added BSD
//
// 2     8/25/10 9:44a Sandhyar
// Common Code BSD from Win common
//
// 1     8/23/10 2:52p Sandhyar
// SCEBSD source
//
// 1     8/17/10 12:12p Sandhyar
// Latest source from Win_Commom_UEFI2.1 2.00.1000
//
// 3     7/28/10 4:57p Sandhyar
// Updated Version works import export provided HII DB file
//
// 1     7/02/10 3:00p Sandhyar
//
// 3     3/17/10 12:22p Hariharanr
// 1) Changes for std/mfg defaults update feature.
// 2) Presanna's fixes for existing issues
//
// 2     1/20/10 9:28p Presannar
// Added Fn signature GetCurrentSystemLanguage
// Updated Fn signature GUID parameter to UINT8*
//
// 1     10/05/09 3:00p Presannar
// Initial Code Base
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
#ifndef _UTILITIES_H_
#define _UTILITIES_H_

#include <stdio.h>

// return codes
#define SCE_SUCCESS             0
#define SCE_BUFFER_TOO_SMALL    0x85
#define SCE_NOT_FOUND           0x8e

VOID *GetNVRAMVariable( wchar_t *Name, UINT8 *VendorGuid, UINT32 *Attributes,
                       UINT64 *VarSize );


wchar_t* ReadUnicodeLine(wchar_t* StrKey, FILE *ParsingFile);

int GetGuidFromString( GUID *Guid, wchar_t *StringGuid );
int SetNVRAMVariable( wchar_t *VariableName, UINT8 *VariableGuid,
                          UINT32 Attributes, UINT64 Size, UINT8 *Buffer );
int GetNextVariable( UINT64 *VariableNameSize, wchar_t *VariableName,
                           GUID *VariableGuid );

UINT8 *GetGuidAsString( GUID *GUID );

wchar_t *GetGuidAsUnicodeString( UINT8 *GUID );
wchar_t *GetCurrentSystemLanguage();
wchar_t *StrToLower(wchar_t *String);
wchar_t* UnicodeLTrim(wchar_t* StrKey);
wchar_t* UnicodeRTrim(wchar_t *StrKey);

char* LTrim( char *StrKey );
char* RTrim( char *StrKey );

#endif /* #ifndef _UTILITIES_H_ */
