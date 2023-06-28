//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2013, American Megatrends, Inc.        **//
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
// $Header: /SandBox/Lenovo_AmiScePkg_Driver/App/AdvScript.c 3     6/26/13 11:15a Arund $
//
// $Revision: 3 $
//
// $Date: 6/26/13 11:15a $
//
//*********************************************************************
// Revision History
// ----------------
// $Log: /SandBox/Lenovo_AmiScePkg_Driver/App/AdvScript.c $
// 
// 3     6/26/13 11:15a Arund
// Updated code with PLDM integration and Mapping language
//
// 43    3/01/13 2:28a Abinayar
// Review comments for EIP 116486 addressed.
//
// 42    2/08/13 9:45a Tanayaj
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
// 41    2/01/13 12:23a Abinayar
// [TAG]  		EIP112604
// [Category]  	Improvement
// [Description]  	Output subtitles as comments to SCE scripts
// [Files]  		App\AdvScript.cpp
// App\Special.h
// LIB\UEFI20Variable.cpp
// LIB\UEFI21HIIParser.cpp
// LIB\UEFI21HIIParser.h
// LIB\UEFI20HIIParser.cpp
// LIB\UEFI20HIIParser.h
// LIB\UEFI21Variable.cpp
//
// 40    1/29/13 11:09a Abinayar
// Review comments addressed.
//
// 39    1/24/13 9:47a Abinayar
// [TAG]  		EIP112604
// [Category]  	Improvement
// [Description]  	Output subtitles as comments to SCE scripts
// [Files]  		App\AdvScript.cpp
// App\Special.h
// LIB\UEFI20Variable.cpp
// LIB\UEFI21HIIParser.cpp
// LIB\UEFI21Variable.cpp
//
// 38    1/24/13 9:09a Arund
// [TAG]	EIP113274
// [Category]	BUG FIX
// [Severity]	Minor
// [Symptom]	Some versions of SCE lose the % character in question names
// [RootCause]	fprintf( AmisceData->VariablesFile, StrOutString ); is used
// to write the control to the file. "%" is considered as format specifier
// and it is not printed in the file.
// [Solution]	changing as, fprintf( AmisceData->VariablesFile, "%s",
// StrOutString );, will resolve the issue.
// [Files]	SRC\AMISCE\SRC\Efi\App\AdvScript.cpp
//
// 37    12/07/12 8:50a Tanayaj
// [TAG]  		EIP 108730
// [Category]  	Defect
// [Severity]  	Non-Urgent
// [Symptom]  	AMISCE unmatched question warning not suppressed by /q
// [RootCause]  	Not checking for quiet mode flag "g_QuietMode" while
// displaying warning message for unmatched question.
// [Solution]  	Added checking for g_QuietMode flag in
// Import21AdvScriptSettings() while displaying warning message for
// unmatched question.
// [Files]  	AdvScript.cpp
//
// 36    11/16/12 5:49a Abinayar
// Fix for EIP106236 added in ImportAdvScriptSettings method for UEFI 2.0
//
// 35    11/16/12 5:40a Abinayar
// [TAG]  		EIP106236
// [Category]  	Bug Fix
// [Severity]  	Minor
// [Symptom]  	AMISCE Boot Option Question unmatched
// [RootCause]  	The variable SetValue is not updated properly. so uParsed
// flag for the Setupquestion is not updated properly.
// [Solution]  	Reset the SetValue after setting the uparsed flag.
// [Files]  		AdvScript.cpp
//
// 34    10/15/12 7:42a Tanayaj
// [TAG]  		EIP64384
// [Category]  	Function Request
// [Severity]      Minor
// [Description]  	SCE: add comment to checkbox value indicating
// enabled/disabled values
// [Solution]      Added comment to checkbox control value as "Value = 0
// //Enabled = 1, Disabled = 0" in the export script.
// [Files]  	AdvScript.cpp
//
// 33    9/29/12 5:50a Abinayar
// Issue found during integration testing fixed. BIOS defaults are updated
// as junk in script file if the setup question having options , issue
// fixed.
//
// 32    9/17/12 7:23a Tanayaj
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
// 31    9/14/12 8:57a Abinayar
// [TAG]  		EIP100678
// [Category]  	Improvement
// [Description]  	AMISCE does not properly handle 32 and 64 bit numeric
// control nor can it properly limit check signed numeric controls.
// [Files]  		AdvScript.cpp
// PARSER.cpp
// Parser.h
// UEFI20HIIParser.h
// UEFI20Variable.cpp
// UEFI20Variable.h
// UEFI21HIIParser.cpp
//
// 30    8/17/12 12:37a Abinayar
// Review comments for EIP 98464 addressed.
//
// 29    8/16/12 5:31a Abinayar
// [TAG]		EIP98464
// [CATEGORY] 	Defect
// [DESCRIPTION] 	Correct SCE warning message portability problems
//
// 28    7/20/12 9:49a Tanayaj
// EIP 92631: modified format of displaying warning message.
//
// 27    7/19/12 11:01p Timothym
// Restore version 25
//
// 25    7/19/12 2:28a Tanayaj
// EIP 92631 : Modified format of dispalying warning message.
//
// 24    7/18/12 11:29a Tanayaj
// [TAG]  		EIP92631
// [Category]  	Enhancement
// [Description]   Improve handling of duplicated or unmatched questions
// SCE does not give an appropriate warning when import questions are
// duplicated or
// when they don't match any control in the IFR.
//
// 1.If user duplicates a setup question,and tries to import,the script
// file will not be updated,but,
// SCEDOS Message: "Script file imported successfully."
//
// 2.If user inputs any question in the script file which was not there in
// the exported script file,
// the script file will not be updated,but
// But the SCEDOS message is still "Script file imported successfully."
//
// [Files]  	App\AdvScript.cpp
//
// 23    4/10/12 2:17p Sandhyar
// Tag: EIP87038
// Category:  Defect
// Descripton:  AMISCE tool reporting success but check in BIOS value not
// changed/load default
// FilesModified:  Advscript.cpp
//
// 22    9/28/11 6:02a Helenlee
// [TAG]  		EIP64384
// [Category]  	Improvement
// [Description]  	- Fixed memory leak caused by running unnecessary
// functions _GetStdDefaults() and _GetMfgDefaults() at String Opcode
// case.
// - Fixed possible memory problems in STR2HEX() and other locations
// [Files]  		App\AdvScript.cpp
// App\PARSER.cpp
// LIB\UEFI20HIIParser.cpp
// LIB\UEFI21HIIParser.cpp
//
// 21    9/23/11 8:19a Helenlee
// [TAG]  		EIP64384
// [Category]  	Improvement
// [Description]  	Update for memory bug fix.
// [Files]  		App\AdvScript.cpp
// LIB\UEFI20HIIParser.cpp
// LIB\UEFI21HIIParser.cpp
//
// 20    9/15/11 7:54a Helenlee
// [TAG]  		EIP64384
// [Category]  	Improvement
// [Description]  	AMISCE string controls
// 1. Design Change:
// For printable characters, format remains as Value = "String Control".
// For non-printable characters, format is Value = {0053 0074 0072 0069
// 006e 0067 0020 0043 006f 006e 0074 0072 006f 006c } //Unicode, Do NOT
// change the format
//
// 2. Remove pointers set to 0 prior to calling free()
// 3. Remove +1 at malloc() in _GetUefiControlValueInfo and
// _GetControlValueInfo functions
// [Files]  		App\AdvScript.cpp
// App\PARSER.cpp
// LIB\UEFI20HIIParser.cpp
// LIB\UEFI21HIIParser.cpp
//
// 19    8/23/11 7:01a Helenlee
// [TAG]  		EIP64384
// [Category]  	Improvement
// [Description]  	1. In default script mode, string control now supports
// printable characters in string and non-printable characters in unicode.
// 2. Changes for minor issues.
// [Solution]
// For printable characters, format remains as Value = "String Control".
// For non-printable characters, format is Value = {5300 7400 7200 6900
// 6e00 6700 2000 4300 6f00 6e00 7400 7200 6f00 6c00} //Unicode, Do NOT
// change the format
// [Files]  		App\AdvScript.cpp
// App\PARSER.cpp
// App\Parser.h
// App\Win2\DRIVER.CPP
// LIB\UEFI20HIIParser.cpp
// LIB\UEFI21HIIParser.cpp
//
// 18    8/04/11 11:27p Helenlee
// [TAG]  		EIP64384
// [Category]  	Improvement
// [Description]  	string control implementation
// [Files]  		AdvScript.cpp
// Parse.cpp
// Parse.h
// UEFI20HIIParse.cpp
// UEFI21HIIParse.cpp
//
// 18    8/05/11 11:20a HelenLee
// [Category]  	Improvement
// [Severity]  	Normal
// [Symptom]  	String control is not implemented correctly.
// [RootCause]	String controls should be handled seperetely as charecters
// instead of normal HEX values.
// [Solution]  	Add conditions when control type EFI_IFR_STRING_OP is found.
// At import part, make sure the input is valid characters first. Then
// convert characters into little-endian HEX string and import back.
// [Files]	AdvScript.cpp
//		Parse.cpp
//		Parse.h
//		UEFI20HIIParse.cpp
//		UEFI21HIIParse.cpp
//
// 17    3/31/11 10:16a Chitaranjand
// For EIP: 41089
//
// 16    2/15/11 4:44p Timothym
// TAG]  		EIP53496
// [Category]  	Bug Fix
// [Severity]  	Normal
// [Symptom]  	Controls with extreme number of options cause SCE to not
// list all options.
// [RootCause]  	The program uses sprintf to place all the characters
// prior to calling fprintf(). EFI SCE over runs the buffer, probably
// corrupting the heap or other memory. All other versions use wsprintf()
// which has max size parameter so the output is truncated.
// [Solution]  	Replace sprintf() with direct use of fprintf(). Also
// replace occurences of fprintf(fp, string) with fprintf(fp, "%s",
// string). There have been past problems with "%" occuring in strings.
// [Files]  		AdvScript.cpp
//
// 15    1/27/11 9:29a Chitaranjand
// [TAG]  		EIP53018
// [Category]  	Improvement
// [Description]  	AMISCE - Command line switch to support quiet mode
// required
//
// [Files]  		App\AdvScript.cpp
// App\AMISCE.cpp
// App\Special.cpp
// LIB\UEFI20Variable.cpp
// LIB\UEFI21Variable.cpp
//
// 14    9/27/10 5:21p Timothym
// Correct typo.
//
// 13    9/25/10 10:34p Timothym
// EFI Shell port.
//
//********************************************************************

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:          AdvScript.cpp
//
// Description:   Contain functions for script import.
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>
#define _EFI 1

//extern "C"
//{
#include "stdlib.h"
#include "wchar.h"
#include "string.h"
#define _UEFI21 1
#include "Efi.h"

#include "AmiSceCommon.h"
#include "Library/MemoryAllocationLib.h"
#include "os_hook.h"
#include "types.h"
//#include "PldmApi.h"
#include "AdvScript.h"
#include "Utilities.h"
#include  <errno.h>
#include  <MainData.h>
#include  <unistd.h>

typedef GUID AMI_GUID;
//Variables to be initialzed for StdLib functions
extern int __sse2_available;
extern struct  __MainData  *gMD;
struct __filedes   *gMfd;
extern EFI_BOOT_SERVICES *gBS;


#if defined(_LNX) || defined(_BSD) || defined(_EFI)
#include "../LIB/UEFI20HIIParser.h"
#include "../LIB/UEFI20StringManagement.h"
#include "../LIB/UEFI21HIIParser.h"
#else
#include "Lib/Uefi20StringManagement.h"
#include "Lib/Uefi20HiiParser.h"
#include "Lib/Uefi21HiiParser.h"
#endif
#include "Special.h"
#include "UEFI21Variable.h"

#if defined(_LNX) || defined(_BSD)
#include "os_hook.h"
#endif

//std and mfg defaults variable name
#define MFG_DEFAULTS L"MfgDefaults"
#define STD_DEFAULTS L"StdDefaults"

#define VALUE_OFFSET  17
#define EFI_IFR_NUMERIC_OP 0x07 // defined the macro here, as including EfiCommon.h leads to lot of redifinition errors
#define EFI_IFR_STRING_OP 0x1C
#define EFI_IFR_CHECKBOX_OP 0x06
#define SPACE_WCHAR L" "

BOOLEAN gIsStdBuf = 0;
BOOLEAN gIsMfgBuf = 0;
char	*gpStdBuf =(char *) NULL;
char	*gpMfgBuf =(char *) NULL;
UINT32	gu32StdAttributes = 0;
UINT32	gu32MfgAttributes = 0;
UINT64	gu64StdSize = 0;
UINT64	gu64MfgSize = 0;
UINT8	gu8ErasePolarity = 0xFF;

wchar_t *gBuffer = NULL;

BOOLEAN g_QuietMode = FALSE; // True, if /q switch is present
UINT8 gUEFIVersion = 0; //UEFI version of BIOS
BOOLEAN gUEFI21 = FALSE;// True, if /u switch is present
BOOLEAN gStdDefaults = FALSE; //True , if /ds switch is present
BOOLEAN gMfgDefaults = FALSE; //True , if /dm switch is present
BOOLEAN gMapLang = TRUE; //True , if /lang switch is present
extern bool ParseScriptFileAndUpdateData (CHAR8 *fpScriptFile, FILE *fpTransFile, BYTE bBiosIdMask, unsigned int HIICrc);
extern wchar_t *UEFI21HiiGetString( UINT8 *PackageList, VOID* HiiPackage, UINT16 Token ,BOOLEAN MapLang);
//extern VOID MemAllocFailure();
BOOLEAN ValidateValue(CONTROL_INFO* Control, UINT64 pValue);
VOID _GetStdMfgDefaultsBuffer();
VOID _SetStdMfgDefaultsBuffer();
BOOLEAN _GetStdDefaults(CONTROL_INFO* Control);
BOOLEAN _GetMfgDefaults(CONTROL_INFO* Control);
BOOLEAN _SetStdDefaults(CONTROL_INFO* Control);
BOOLEAN _SetMfgDefaults(CONTROL_INFO* Control);
VOID _FreeSetupQuestionList();
VOID _FreeVariableList();
extern VOID _Wnstrcpy(unsigned short *NarrowName, wchar_t *WideName);
UINT16 _GetUefiControlPrompt( UINT8 *ifr );
CONTROL_INFO *GetNextControl(UINT8 *CurrentForm, UINT8 * , HII_DATA *AmisceData , UINT8 **CurrentControl);
extern BOOLEAN _VerifySpecialSetupQuestion( SETUP_QUESTION_LIST *SetupQuestion, UINT8 *DataTable, UINT8 *HIIDBData, CONTROL_INFO *Control );

wchar_t *PldmGetHiiString( UINT8 *DataTable, UINT8 *HiiData, UINT16 Token ,BOOLEAN MapLangFlag)
{
  wchar_t *Text = (wchar_t *)NULL;

  if( UEFI_20_VERSION == gUEFIVersion )
  {
#if 0
	Text = UEFI20HiiGetString( DataTable, HiiData, Token );
#endif
  }
  else if(UEFI_21_VERSION == gUEFIVersion )
  {
	Text = UEFI21HiiGetString( DataTable, HiiData, Token ,MapLangFlag);
  }
  return Text;
}

BOOLEAN _ImportSpecialControlToNVRAM( SETUP_QUESTION_LIST *SetupQuestion, UINT8 *DataTable, UINT8 *HiiData, CONTROL_INFO *Control )
{
  BOOLEAN Flag = FALSE;

  switch( GetSpecialKey( Control->ConditionalVariable[0] ))
  {
  case LANGUAGE_CONTROL:
  case PLATFORM_LANGUAGE_CONTROL:
    Flag = ImportLanguageControlToNVRAM( SetupQuestion, DataTable, HiiData, Control );
    break;
  case BOOT_ORDER_CONTROL:
    Flag = ImportBootOrderContrlToNVRAM( SetupQuestion, DataTable, HiiData, Control );
    break;
  case LEGACY_DEV_ORDER_CONTROL:
    Flag = ImportLegacyDevOrderControlToNVRAM( SetupQuestion, DataTable, HiiData, Control );
    break;
  default:
    break;
  }

  return Flag;
}

void FreeLanguageBuffer()
{
	UINT32 Index = 0;
	if(gLanguages)
	{
		while ( Index < gLangCount )
		{
			if ( NULL != gLanguages[Index].LangCode )
			  free(gLanguages[Index].LangCode);
			Index ++;
		}
  		free(gLanguages);
  		gLanguages = NULL;
	}
}


VOID _GetStdMfgDefaultsBuffer()
{
	GUID DefaultsGuid =
		{ 0x4599d26f, 0x1A11, 0x49b8, { 0xb9, 0x1f, 0x85, 0x87, 0x45, 0xcf, 0xf8, 0x24 } };

	gu32StdAttributes = 0;
	gu64StdSize = 0;
	gpStdBuf = ( char* )VarGetNvramName( STD_DEFAULTS, &DefaultsGuid, &gu32StdAttributes, (UINTN *)&gu64StdSize );
	if(gpStdBuf != NULL)
		gIsStdBuf = 1;

	gu32MfgAttributes = 0;
	gu64MfgSize = 0;
	gpMfgBuf = ( char* )VarGetNvramName( MFG_DEFAULTS, &DefaultsGuid, &gu32MfgAttributes, (UINTN *)&gu64MfgSize );
	if(gpMfgBuf != NULL)
		gIsMfgBuf = 1;
}

VOID _SetStdMfgDefaultsBuffer()
{
	GUID DefaultsGuid =
		{ 0x4599d26f, 0x1A11, 0x49b8, { 0xb9, 0x1f, 0x85, 0x87, 0x45, 0xcf, 0xf8, 0x24 } };
	UINT32 Status = 0;

	if(gIsStdBuf)
	{
		NvramLibInit(gpStdBuf, (UINTN)gu64StdSize, 0xff);
		NvramCompact();
		Status = SetNVRAMVariable( STD_DEFAULTS, ( UINT8 * )&DefaultsGuid,
							gu32StdAttributes, gu64StdSize, (UINT8*)gpStdBuf);
		//if( Status )
			//fwprintf( stderr, L"Error Writing Variable %"_S L"to NVRAM\n", STD_DEFAULTS );
	}

	if(gIsMfgBuf)
	{
		Status = SetNVRAMVariable( MFG_DEFAULTS, ( UINT8 * )&DefaultsGuid,
							gu32MfgAttributes, gu64MfgSize, (UINT8*)gpMfgBuf);
		//if( Status )
		//	fwprintf( stderr, L"Error Writing Variable %"_S L"to NVRAM\n", MFG_DEFAULTS );
	}
}

BOOLEAN _GetStdDefaults(CONTROL_INFO* Control)
{
	UINTN DataSize = 0;
	char* Data = (char* )NULL;
	unsigned int Attributes = 0;
	CHAR16 VarName[MAX_LENGTH] = {0};
	VARIABLE* lpVar = (VARIABLE*)calloc(1, sizeof(VARIABLE));
	EFI_STATUS Status;

	if(NULL == lpVar)
		throw(L"Memory Allocation Failure");

	if ( !GetVariableById(Control->VariableStorage, lpVar) )
	{
		free(lpVar);
		return 0;
	}

	if(gpStdBuf == NULL)
		return 0;
	if(gu8ErasePolarity == 0xFF)
		NvramLibInit(gpStdBuf, (UINTN)gu64StdSize, 0x01); //Assume erase polarity as 1, if erase polarity is not known
	else
		NvramLibInit(gpStdBuf, (UINTN)gu64StdSize, gu8ErasePolarity); //Use gu8ErasePolarity, if it's known. (0 or 1)

	#if defined(_LNX) || defined(_BSD)
		_Wnstrcpy(VarName,lpVar->VariableName);
	#else
		_Wnstrcpy(VarName,lpVar->VariableName);
	#endif

	Status = SceGetVariable(
		VarName, &lpVar->VariableGuid,
		&Attributes,
		&DataSize, NULL);

	if(gu8ErasePolarity == 0xFF) // Execute only if gu8ErasePolarity is not known
	{
		if(DataSize != 0)
			gu8ErasePolarity = 0x01; //Erase polarity is 1, if variable size is non-zero
		else
		{
			NvramLibInit(gpStdBuf, (UINTN)gu64StdSize, 0x00); //Assume erase polarity as 0, if variable size is 0 when using erase polarity 1

			Status = SceGetVariable(
				VarName, &lpVar->VariableGuid,
				&Attributes,
				&DataSize, NULL);

			if(DataSize != 0)
				gu8ErasePolarity = 0x00; //Erase polarity is 0, if variable size is non-zero
		}
	}

	if (Status == EFI_BUFFER_TOO_SMALL)
	{
		Data = (char*)calloc(DataSize, sizeof(char));
		if (Data == NULL)
		{
			free(lpVar);
			throw(L"Memory Allocation Failure");
		}
		Status = SceGetVariable(
			VarName, &lpVar->VariableGuid,
			&Attributes,
			&DataSize, Data);
	}
	if((Status == FV_SUCCESS) || (Control->DataOffset < DataSize))
	{
		memset(&Control->ValueInfo->OptimalDefaults, 0, sizeof(UINT64));
		memcpy(&Control->ValueInfo->OptimalDefaults, Data + Control->DataOffset, Control->DataWidth);
		//memcpy(OptimalBuffer, Data + Control->DataOffset, Control->DataWidth);
	}
	if(Status || (Control->DataOffset >= DataSize))
	{
		if(Data)
			free(Data);
		free(lpVar);
		return 0;
	}


	free(Data);
	free(lpVar);
	return 1;
}

BOOLEAN _GetMfgDefaults(CONTROL_INFO* Control)
{
	UINTN DataSize = 0;
	char* Data = (char*)NULL;
	unsigned int Attributes = 0;
	CHAR16 VarName[MAX_LENGTH] = {0};
	VARIABLE* lpVar = (VARIABLE*)calloc(1, sizeof(VARIABLE));
	EFI_STATUS Status;

	if(NULL == lpVar)
		throw(L"Memory Allocation Failure");

	if ( !GetVariableById(Control->VariableStorage, lpVar) )
	{
		free(lpVar);
		return 0;
	}
	
	if(gpMfgBuf == NULL)
		return 0;
	if(gu8ErasePolarity == 0xFF)
		NvramLibInit(gpMfgBuf, (UINTN)gu64MfgSize, 0x01);
	else
		NvramLibInit(gpMfgBuf, (UINTN)gu64MfgSize, gu8ErasePolarity);

	#if defined(_LNX) || defined(_BSD)
		_Wnstrcpy(VarName,lpVar->VariableName);
	#else
		wcscpy((wchar_t*)VarName,(const wchar_t*)lpVar->VariableName);
	#endif
	Status = SceGetVariable(
		VarName, &lpVar->VariableGuid,
		&Attributes,
		&DataSize, NULL);

	if(gu8ErasePolarity == 0xFF)
	{
		if(DataSize != 0)
			gu8ErasePolarity = 0x01;
		else
		{
			NvramLibInit(gpMfgBuf, (UINTN)gu64MfgSize, 0x00);

			Status = SceGetVariable(
				VarName, &lpVar->VariableGuid,
				&Attributes,
				&DataSize, NULL);

			if(DataSize != 0)
				gu8ErasePolarity = 0x00;
		}
	}

	if (Status == EFI_BUFFER_TOO_SMALL)
	{
		Data = (char*)malloc(DataSize);
		if (Data == NULL)
		{
			free(lpVar);
			throw(L"Memory Allocation Failure");
		}
		Status = SceGetVariable(
			VarName, &lpVar->VariableGuid,
			&Attributes,
			&DataSize, Data);
	}
	if((Status == FV_SUCCESS) && (Control->DataOffset < DataSize))
	{
		memset(&Control->ValueInfo->FailSafeDefaults, 0, sizeof(UINT64));
		memcpy(&Control->ValueInfo->FailSafeDefaults, Data + Control->DataOffset, Control->DataWidth);
	}
	if(Status || (Control->DataOffset >= DataSize))
	{
		if(Data)
			free(Data);
		free(lpVar);
		return 0;
	}

	free(Data);
	free(lpVar);
	return 1;
}

BOOLEAN _SetStdDefaults(CONTROL_INFO* Control)
{
	UINTN DataSize = 0;
	char* Data = (char*)NULL;
	unsigned int Attributes = 0;
	CHAR16 VarName[MAX_LENGTH] = {0};
	VARIABLE* lpVar = (VARIABLE*)calloc(1, sizeof(VARIABLE));
	EFI_STATUS Status;

	if(NULL == lpVar)
		throw(L"Memory Allocation Failure");

	if ( !GetVariableById(Control->VariableStorage, lpVar) )
	{
		free(lpVar);
		return 0;
	}

	if(gpStdBuf == NULL)
		return 0;
	if(gu8ErasePolarity == 0xFF)
		NvramLibInit(gpStdBuf, (UINTN)gu64StdSize, 0x01);
	else
		NvramLibInit(gpStdBuf, (UINTN)gu64StdSize, gu8ErasePolarity);

	#if defined(_LNX) || defined(_BSD)
		_Wnstrcpy(VarName, lpVar->VariableName);
	#else
		wcscpy((wchar_t*)VarName,(const wchar_t*)lpVar->VariableName);
	#endif
	Status = SceGetVariable(
		VarName, &lpVar->VariableGuid,
		&Attributes,
		&DataSize, NULL);

	if(gu8ErasePolarity == 0xFF)
	{
		if(DataSize != 0)
			gu8ErasePolarity = 0x01;
		else
		{
			NvramLibInit(gpStdBuf, (UINTN)gu64StdSize, 0x00);

			Status = SceGetVariable(
				VarName, &lpVar->VariableGuid,
				&Attributes,
				&DataSize, NULL);

			if(DataSize != 0)
				gu8ErasePolarity = 0x00;
		}
	}

	if (Status == EFI_BUFFER_TOO_SMALL)
	{
		Data = (char*)malloc(DataSize);
		if (Data == NULL)
		{
			free(lpVar);
			throw(L"Memory Allocation Failure");
		}
		Status = SceGetVariable(
			VarName, &lpVar->VariableGuid,
			&Attributes,
			&DataSize, Data);
	}
	if((Status == FV_SUCCESS) && (Control->DataOffset < DataSize))
	{
		memcpy(Data + Control->DataOffset, &Control->ValueInfo->OptimalDefaults, Control->DataWidth);
		Status = SetVariable(
			VarName, &lpVar->VariableGuid,
			Attributes,
			DataSize, Data);
	}
	if(Status || (Control->DataOffset >= DataSize))
	{
		if(Data)
			free(Data);
		free(lpVar);
		return 0;
	}

	NvramCompact();  //makes sure no wasted space is left in nvram data area
	free(Data);
	free(lpVar);
	return 1;

}

BOOLEAN _SetMfgDefaults(CONTROL_INFO* Control)
{
	UINTN DataSize = 0;
	char* Data = (char*) NULL;
	unsigned int Attributes = 0;
	CHAR16 VarName[MAX_LENGTH] = {0};
	VARIABLE* lpVar = (VARIABLE*)calloc(1, sizeof(VARIABLE));
	EFI_STATUS Status;

	if(NULL == lpVar)
		throw(L"Memory Allocation Failure");

	if ( !GetVariableById(Control->VariableStorage, lpVar) )
	{
		free(lpVar);
		return 0;
	}

	if(gpMfgBuf == NULL)
		return 0;
	if(gu8ErasePolarity == 0xFF)
		NvramLibInit(gpMfgBuf, (UINTN)gu64MfgSize, 0x01);
	else
		NvramLibInit(gpMfgBuf, (UINTN)gu64MfgSize, gu8ErasePolarity);

	#if defined(_LNX) || defined(_BSD)
   		_Wnstrcpy(VarName, lpVar->VariableName);
    #else
	    _Wnstrcpy(VarName,lpVar->VariableName);
    #endif

	Status = SceGetVariable(
		VarName, &lpVar->VariableGuid,
		&Attributes,
		&DataSize, NULL);

	if(gu8ErasePolarity == 0xFF)
	{
		if(DataSize != 0)
			gu8ErasePolarity = 0x01;
		else
		{
			NvramLibInit(gpMfgBuf, (UINTN)gu64MfgSize, 0x00);

			Status = SceGetVariable(
				VarName, &lpVar->VariableGuid,
				&Attributes,
				&DataSize, NULL);

			if(DataSize != 0)
				gu8ErasePolarity = 0x00;
		}
	}

	if (Status == EFI_BUFFER_TOO_SMALL)
	{
		Data = (char*)malloc(DataSize);
		if (Data == NULL)
		{
			free(lpVar);
			throw(L"Memory Allocation Failure");
		}
		Status = SceGetVariable(
			VarName, &lpVar->VariableGuid,
			&Attributes,
			&DataSize, Data);
	}
	if((Status == FV_SUCCESS) && (Control->DataOffset < DataSize))
	{
		memcpy(Data + Control->DataOffset, &Control->ValueInfo->FailSafeDefaults, Control->DataWidth);
		Status = SetVariable(
			VarName, &lpVar->VariableGuid,
			Attributes,
			DataSize, Data);
	}
	if(Status || (Control->DataOffset >= DataSize))
	{
		if(Data)
			free(Data);
		free(lpVar);
		return 0;
	}

	NvramCompact();  //makes sure no wasted space is left in nvram data area
	free(Data);
	free(lpVar);
	return 1;
}

// Return data address and length of a variable nested inside "StdDefaults"
VOID *GetVarFromDefaults(wchar_t *Name, EFI_GUID *Guid, UINT32 *Attributes, UINT32 *Size)
{
	CHAR16 Name16[MAX_LENGTH];
	EFI_STATUS Status;
	UINTN VarSize = 0;
	*Size = 0;
	if (gpStdBuf == 0)
		return NULL;
	_Wnstrcpy(Name16, Name);
	if(gu8ErasePolarity == 0xFF)
		NvramLibInit(gpStdBuf, (UINTN)gu64StdSize, 0x01); 
	else
		NvramLibInit(gpStdBuf, (UINTN)gu64StdSize, gu8ErasePolarity); 
    Status = SceGetVariable(
		Name16, Guid,
		Attributes,
		&VarSize, 0);
	*Size = (UINT32)VarSize;

	if(gu8ErasePolarity == 0xFF)
	{	// erase polarity not currently known
		if(Size != 0)
			gu8ErasePolarity = 0x01;
		else
		{
			NvramLibInit(gpStdBuf, (UINTN)gu64StdSize, 0x00);

			Status = SceGetVariable(
				Name16, Guid,
				Attributes,
				&VarSize, NULL);
            *Size = (UINT32)VarSize;
			if(Size != 0)
				gu8ErasePolarity = 0x00;
		}
	}
	if (Status == EFI_BUFFER_TOO_SMALL)
	{
 		void *Data;
		Data = malloc(*Size);
		if (Data == NULL)
			throw(L"Memory Allocation failure");
		Status = SceGetVariable(
			Name16, Guid,
			Attributes,
			&VarSize, Data);
		*Size = (UINT32)VarSize;
		return Data;

	}
	return NULL;
}



/* Function :FindUefiVersion
   To find the version of the System BIOS
*/
VOID FindUefiVersion( UINT8 * pValue )
{
	//BOOLEAN UefiVersion = 0;

	if( TRUE == gUEFI21 ) // '/u' switch present
	{
		if( TRUE == IsValidUefi21Pack( pValue))
		{
			gUEFIVersion = UEFI_21_VERSION;
		}
#if 0
		else if( TRUE == IsValidUefi20Pack( pValue ))
		{
			gUEFIVersion = UEFI_20_VERSION;
		}
#endif
		else
		{
			//fwprintf( stderr, L"BIOS UEFI version not supported.\n");
			exit(1);
		}
	}
	else // if /u switch is not present
	{
#if 0
		if( TRUE == IsValidUefi20Pack( pValue))
		{
			gUEFIVersion = UEFI_20_VERSION;
		}
#endif
		if( TRUE == IsValidUefi21Pack( pValue ))
		{
			gUEFIVersion = UEFI_21_VERSION;
		}
		else
		{
			//fwprintf( stderr, L"BIOS UEFI version not supported.\n");
			exit(1);
		}
	}
}
extern VARIABLE *gVariable;

VOID _FreeVariableList()
{
	VARIABLE *Variable = gVariable;
	VARIABLE *TempVariable = (VARIABLE *)NULL;

	while( Variable )
	{
		if( Variable->VariableBuffer )
		{
			FreePool( Variable->VariableBuffer );
		}
		TempVariable = Variable->Next;
		FreePool( Variable );
		Variable = TempVariable;
	}
    gVariable = 0;
}
//Fix for EIP 87038
//Add function to validate the value
BOOLEAN ValidateValue(CONTROL_INFO* Control, UINT64 pValue)
{

	BOOL Flag=FALSE;
	OPTION_LIST *OptList;

	if( IfControlHasOptions( Control->ControlType ) == TRUE )
	{
		OptList = ( OPTION_LIST * )Control->ValueInfo->Options;
		for( ; OptList; OptList = OptList->Next )
		{
		  if( OptList->Value == pValue)
		  {
			return 1;
		  }
		}
		if(Flag==FALSE)
		{
			return 0;
		}
	}

	return 1;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure	: CleanUpPldmLibrary
//
// Description	: Clean up data as required by the exit() function.
//
// Parameter	: INTN
//
// Return Value: VOID
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
void	CleanUpPldmLibrary(INTN ExitVal)
{
	void (*CleanUp)(void);   // Pointer to Cleanup Function
	int i = 0;

	if(gMD != NULL) {
		gMD->ExitValue = (int)ExitVal;
		CleanUp = gMD->cleanup; // Preserve the pointer to the Cleanup Function

		// Call all registered atexit functions in reverse order
		i = gMD->num_atexit;
		if( i > 0) {
			do {
				(gMD->atexit_handler[--i])();
			} while( i > 0);
		}

		if (CleanUp != NULL) {
			CleanUp();
		}
	}
	for(i = OPEN_MAX - 1; i >= 0; --i) {
			(void)close(i);   // Close properly handles closing a closed file.
		}

		/* Free the global MainData structure */
		if(gMD != NULL) {
			if(gMD->NCmdLine != NULL) {
				FreePool( gMD->NCmdLine );
			}
			FreePool( gMD );
		}
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure	: InitializePldmLibrary
//
// Description	: Initialize gMD structure, which is used by StdLib library functions.
//
// Parameter	: INTN
//
// Return Value: VOID
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID InitializePldmLibrary(VOID)
{
	UINTN   ExitVal;
	UINTN i = 0;
	ExitVal = (INTN)RETURN_SUCCESS;
	gMD = (struct  __MainData  *)AllocateZeroPool(sizeof(struct __MainData));
	if( gMD == NULL ) 
	{
		ExitVal = (INTN)RETURN_OUT_OF_RESOURCES;
	}
	else 
	{
		/* Initialize data */
		__sse2_available      = 0;
		_fltused              = 1;
		errno                 = 0;
		EFIerrno              = 0;

		gMD->ClocksPerSecond  = 1;
		gMD->AppStartTime     = (clock_t)((UINT32)time(NULL));

		// Initialize file descriptors
		gMfd = gMD->fdarray;
		for(i = 0; i < (FOPEN_MAX); ++i)
		{
			gMfd[i].MyFD = (UINT16)i;
		}
	}
}


//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2013, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**    5555 Oakbrook Pkwy, Norcross, Suite 200, Georgia 30093   **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
