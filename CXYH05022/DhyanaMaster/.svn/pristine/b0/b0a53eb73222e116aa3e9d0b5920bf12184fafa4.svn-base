//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2010, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**    5555 Oakbrook Pkwy, Norcross, Suite 200, Georgia 30093   **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Header: /SandBox/Lenovo_AmiScePkg_Driver/App/Efi2/os_hook.c 3     7/26/13 1:38p Arund $
//
// $Revision: 3 $
//
// $Date: 7/26/13 1:38p $
//
//*********************************************************************
// Revision History
// ----------------
// $Log: /SandBox/Lenovo_AmiScePkg_Driver/App/Efi2/os_hook.c $
// 
// 3     7/26/13 1:38p Arund
// 
// 1     5/29/13 11:39a Arund
//
// 2     1/31/11 4:28p Timothym
// [TAG]  		EIP52019
// [Category]  	Improvement
// [Description]  	SCEEFI fails when run under "DUET" shell. Change EFI
// version to use SMI instead of BIOS protocols.
//

// 1     9/28/10 9:00a Timothym
//
//********************************************************************

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:          os_hook.cpp
//
// Description:   Provides OS dependent functions.
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>
#include "types.h"
#include "ID_ERR.H"

//extern "C"
//{
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <time.h>
#include <string.h>
#include "AmiSceCommon.h"
#include "Library/UefiLib.h"
//}

ENUM_ERR_ID OsInit(void);
wchar_t* widestr;
//wchar_t* wcstok(wchar_t* Line, const wchar_t* delim);
void _wstrdate(wchar_t Date[]);
void _wstrtime(wchar_t Time[]);
//FILE* _tfopen( const wchar_t* ListingFile, const wchar_t* GenListingFile);
//wchar_t*  ConvertToWide(char* argument);
void freewidestr();
VOID _Nmemcpy( UINT16 *VarName,wchar_t *VariableName,int Size);
VOID _Wmemcpy( wchar_t *VariableName, UINT16 *VarName,int Size);
int wnstrcmp(wchar_t *wide, unsigned short *narrow);
VOID _Nwstrcpy(wchar_t *WideName, unsigned short *NarrowName);
VOID _Wnstrcpy(unsigned short *NarrowName, wchar_t *WideName);
size_t _Wstrlen(UINT16 *str);
//-----------------------------------------------------------------------------
// OsInit - Nothing to do on DOS
//-----------------------------------------------------------------------------
ENUM_ERR_ID OsInit()
{
#ifdef _LNX
    // set cpu affinity
     cpu_set_t           mask;
     unsigned int        masklen = sizeof(mask);
     CPU_ZERO(&mask);
     CPU_SET(0, &mask);
     sched_setaffinity(0, masklen, &mask);
     iopl(3);
#endif
    return eEID_NO_ERROR;
}

#if 0
wchar_t* wcstok(wchar_t* Line, const wchar_t* delim)
{
	wchar_t* ptr,*token;
	token = wcstok(Line,delim, &ptr);
	return token;
}
#endif

//extern "C"
int sleep(unsigned int);
void _wstrdate( wchar_t Date[9])
{
char curdate[10];
time_t now = time(0);
curdate[9] = 0;
strftime(curdate, 9, "%D", localtime(&now));
//wprintf(L"length of date string %d\n", strlen(curdate));
//printf("String is %s\n", curdate);
mbstowcs( Date, curdate, strlen(curdate));
//Date[9] = 0;
Date[8] = 0;    // mbstowcs fails to add null -- should be strlen + 1?
//wprintf(L"length of wide date string %d\n", wcslen(Date)); //sleep(10);
//wprintf(L"Wide string is %s\n", Date); sleep(15);
}


void _wstrtime( wchar_t Time[9] )
{
char curtime[10];
time_t now = time(0);
strftime(curtime, 9, "%T", localtime(&now));
mbstowcs( Time, curtime, strlen(curtime));
Time[7] = 0;
}

wchar_t*  ConvertToWide(wchar_t* argument)
{
	return argument;

}

#if 0
FILE* _tfopen( const wchar_t* ListingFile, const wchar_t* GenListingFile)
{
	//wprintf(L"%s:%d\n", __FUNCTION__, __LINE__);
	FILE* fp;
	int FileLen = (int) wcslen(ListingFile);
	int GenLen = (int) wcslen(GenListingFile);
        char *FileName = (char*) calloc(FileLen+1,sizeof(char));
        char *GenFile = (char*) calloc(GenLen+1,sizeof(char));
	wcstombs(FileName, ListingFile, FileLen+1);
	wcstombs(GenFile, GenListingFile, GenLen+1);
        fp = fopen(FileName,GenFile);
	return fp;

}
#endif
void freewidestr(){
// do nothing
}

VOID _Nmemcpy( UINT16 *VarName,wchar_t *VariableName,int Size)
{
  int TotalBytes = Size/2; //Size = VariableNameSize*sizeof(UINT16)
  while(TotalBytes)
  {
    *VarName++ = (UINT16)*VariableName++;
     TotalBytes--;
  }
}
VOID _Wmemcpy( wchar_t *VariableName, UINT16 *VarName,int Size)
{
  int TotalBytes = Size/2; //Size = VariableNameSize*sizeof(UINT16)
  while(TotalBytes)
  {
    *VariableName++ = *VarName++;
     TotalBytes--;
  }


}

int wnstrcmp(wchar_t *wide, unsigned short *narrow)
{
    while(*wide == *narrow++)
    if(*wide++ == 0)
    {
          return 0;
    }
    return (*(wchar_t*) wide - *(unsigned short*) (narrow-1));
}

VOID _Nwstrcpy(wchar_t *WideName, unsigned short *NarrowName)
{
	while((*WideName++ = *NarrowName++) != 0 );
 /* while(*NarrowName)
  {
	  *WideName++ = *NarrowName++;
  }
  *WideName = '\0';*/
}

VOID _Wnstrcpy(unsigned short *NarrowName, wchar_t *WideName)
{
  while((*NarrowName++ = *WideName++) != 0);
}

size_t _Wstrlen(UINT16 *str)
{
#if 1
    return SCE_StrLen((const wchar_t *)str);
//    return wcslen((const wchar_t *)str);
#else
  const UINT16 *s;
  for(s = str; *s ; ++s);
  return (size_t)(s - str);
#endif
}

#include "vars.h"
#include "os_hook.h"

// interfaces to efi to replace SMI
//#if 0       // changed to use SMI
#include "Utilities.h"
int SCE_GetVariable(wchar_t *VariableName, GUID *Guid, UINT32 *Attributes, UINT64 *VariableDataSize, void **VariableData)
{
    EFI_STATUS Status;
    UINTN DataSize = (UINTN)*VariableDataSize;
    Status = GetNvramVariable(VariableName, (EFI_GUID *)Guid, Attributes, &DataSize, (UINT8 *)*VariableData);
    *VariableDataSize = DataSize;
    // translate error returns
    if (Status == EFI_BUFFER_TOO_SMALL)
        Status = SCE_BUFFER_TOO_SMALL;
    else if (Status == EFI_NOT_FOUND)
        Status = SCE_NOT_FOUND;
    return (int)Status;
}

int SCE_SetVariable( wchar_t *VariableName, GUID *VendorGUID, UINT32 Attributes, UINT64 VariableDataSize, UINT8 *VariableData )
{
    return (int)SetNvramVariable(VariableName, (EFI_GUID *)VendorGUID, Attributes, (UINTN)VariableDataSize, VariableData);
}

int SCE_GetNextVariableName( UINT32 *VariableNameSize, wchar_t **VariableName, GUID *VendorGUID )
{
    EFI_STATUS Status;
    UINTN Size;     // 64 bits in x64 mode
    Status = GetNextNvramVariable(&Size, *VariableName, (EFI_GUID *)VendorGUID);
    *VariableNameSize = (UINT32)Size;
    if (Status == EFI_NOT_FOUND)
        Status = SCE_NOT_FOUND;
    return (int)Status;
}

// replacement for throw("memory allocation error")
void MallocFailure(wchar_t *Message)
{
//    wprintf(L"%s\n", Message);
    exit(1);
}


// temporary debug printing for functions that don't have stdlib.h
//extern "C"
void bprint(wchar_t *text, int n1, int n2)
{
    wprintf(L"%s %x %x\n", text, n1, n2);
}

// wcsicmp is missing from libc -- from AMILLIB/BIOS/wcstring.cpp on PBA
static
wchar_t WchUppertoLower(wchar_t wch){
	if((wch>='A')&&(wch<='Z')){
		return (wch + 'a' - 'A');
	}
	return wch;
}

int wcsicmp(const wchar_t *s1, const wchar_t *s2)
{
	do{
		if(WchUppertoLower(*s1)!=WchUppertoLower(*s2)){
			return (int)(*s1 - *s2);
		}
		s1++;
		s2++;
	}while((*s1!=0)&&(*s2!=0));
	return 0;
}

//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2010, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**    5555 Oakbrook Pkwy, Norcross, Suite 200, Georgia 30093   **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
