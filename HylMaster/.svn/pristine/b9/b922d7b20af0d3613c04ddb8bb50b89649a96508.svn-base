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

//**********************************************************************
// $Header: /SandBox/Pldm/Pldm.cpp 42    9/17/13 5:46p Timothym $
//
// $Revision: 42 $
//
// $Date: 9/17/13 5:46p $
//**********************************************************************
// Revision History
// ----------------
// $Log: /SandBox/Pldm/Pldm.cpp $
//
// 42    9/17/13 5:46p Timothym
// correct typos
//
// 41    9/17/13 4:35p Timothym
// add PHDR
//
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  		Pldm.c*
//
// Description:	 Pldm tables for SCE environment
//
//<AMI_FHDR_END>
#define PLDM_DBG
#ifdef PLDM_DBG
#define _DBG_(x) x
#else
#define _DBG_
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "AmiLib.h"
#include <wchar.h>
#include "types.h"
#include "Efi.h"
#include "Token.h"
#ifndef _LNX
#include "AmiSceCommon.h"
#endif
#include "Library/MemoryAllocationLib.h"
#include "Utilities.h"
#include "UEFI20StringManagement.h"
#include "UEFI21HIIParser.h"
#include "UEFI21Variable.h"
#pragma pack(push, 1)
#include "UEFI20DevicePath.h"
#pragma pack(pop)
#include "Special.h"
#include "os_hook.h"
#include "NvramLib.h"
#include "AmiDxeLib.h"
#include "MemFile.h"
#include "Pldm.h"
#include "PldmApi.h"
#include "EDKStructures21.h"
#include "AdvScript.h"
#include "CRC32.h"
#include "AMIHIILibrary.h"
#include <assert.h>
#include <Library/PcdLib.h>
#include <AmiLib.h>
#ifdef _LNX
#define wcslen16 _Wstrlen
extern size_t _Wstrlen(UINT16 *);
extern VOID _Wnstrcpy(unsigned short *NarrowName, wchar_t *WideName);
extern VOID _Nwstrcpy(wchar_t *WideName, unsigned short *NarrowName);
#else
#define wcslen16 _Wstrlen
#endif
#include "Token.h"

#ifdef _LNX
#define EFI_NOT_FOUND                  (14 | EFI_ERROR_BIT) // 0xe
#define EFI_ABORTED                    (21 | EFI_ERROR_BIT) // 0x15
#define EFI_OUT_OF_RESOURCES           (9  | EFI_ERROR_BIT)
#define EFI_INVALID_PARAMETER          (2  | EFI_ERROR_BIT)
#define EFI_CRC_ERROR                  (27  | EFI_ERROR_BIT)
#define EFI_SUCCESS                     0
#endif

#define EFI_IFR_NUMERIC_OP 0x07 // defined the macro here, as including EfiCommon.h leads to lot of redifinition errors
#define EFI_IFR_STRING_OP 0x1C
#define EFI_IFR_CHECKBOX_OP 0x06

extern struct  __MainData  *gMD;

//Global buffers to save the Pldm Tables
UINTN       gRbHiiSize = 0;
UINT8       *gRbHiiData = NULL;
UINT8       *gStringTable;
UINT32      gStringSize;
UINT8       *gAttributeTable;
UINT32      gAttributeSize;
UINT8       *gValueTable;
UINT32      gValueSize;
UINT8       *gMapStringTable;
UINT32      gMapStringSize;

MEMFILE AttribBuffer;
MEMFILE ValueBuffer;
MEMFILE StringBuffer;
MEMFILE MapBuffer;

PLDM_ATTRIBUTE      *gControlAttribute = (PLDM_ATTRIBUTE *)NULL;
VALUE_POINTER gControlValue = {0};
VALUE_POINTER gPasswordCtrlValue;
PLDM_VALUE *gControlWriteValue = (PLDM_VALUE *)NULL;
BOOLEAN gPldmAvailable = FALSE;
MEMFILE MapIdBuffer = {0};
EFI_STATUS  gSetValueStatus = EFI_SUCCESS;
BOOLEAN gInternalTables = FALSE;
BOOLEAN gSetUsingMapID = FALSE;
BOOLEAN gRedfishEnabled = FALSE;

// forward declarations
EFI_STATUS
CreateVarTables(
	UINT8  *HiiData,       // pointer to HII export data
	UINT32  HiiSize,       // size of Hii Data
	MEMFILE *afp,           // attributes buffer
	UINT16  HandleA,       // 1st available attribute handle
	MEMFILE *sfp,           // strings buffer
	UINT16  HandleS,       // 1st available string handle
	MEMFILE *vfp            // values buffer
);

EFI_STATUS
CreateBootVarTables(
	UINT8  *HiiData,       // pointer to HII export data
	UINT32  HiiSize,       // size of Hii Data
	MEMFILE *afp,           // attributes buffer
	UINT16  HandleA,       // 1st available attribute handle
	MEMFILE *sfp,           // strings buffer
	UINT16  HandleS,       // 1st available string handle
	MEMFILE *vfp            // values buffer
);

BOOLEAN
InFilter(EFI_HII_HANDLE Pack);

extern VARIABLE *gVariable ;
extern EFI_BOOT_SERVICES *gBS;
extern EFI_SYSTEM_TABLE *gST;
extern EFI_RUNTIME_SERVICES *gRT;

extern BOOLEAN gUseCache;
//kuaibai - UINTN gPreserveMode = 0;
UINTN gPreserveMode = 1;//kuaibai +
extern VOID _Nwstrcpy(wchar_t *WideName, unsigned short *NarrowName);
extern void  _FreeVariableList();   // in  AdvScript

static void GetHiiHandles();
static void FreeHiiHandles();

CONTROL_RESET_FLAGINFO *gResetControlInfo;
char OptimalBuffer[64] = {0};

HII_DATA AmisceData;
EFI_HII_HANDLE *gHiiHandles = 0;
UINT32 gHiiNumHandles = 0;
EFI_HII_HANDLE *gFilterHandles = 0;
UINT32 gFilterCount = 0;
UINT32 gVariablesAdded = 0;

// when TRUE, skip the variables in a package having Mapping language pack
BOOLEAN gSkipVarbyHiiHandles = TRUE;

// when not null, tell addvariable to use configuration protocol
EFI_HII_HANDLE gCurrentHiiHandle = 0;

// forward declarations

// find an attribute given its handle
static
PLDM_ATTRIBUTE *
AttributeByHandle(UINT8 *Attributes, UINT32 Size, UINT16 Handle);

static void
FreeControl(CONTROL_INFO *Control);

// debugging functions
static
void Print16String(UINT16 *s)
{
	if (0 == s)
	{
		wprintf(L"(null)");
		return;
	}

	while (*s)
	{
		wprintf(L"%lc", (wchar_t)*s++);
	}
}

static
void Print16StringN(UINT16 *s, int n)
{
	int i;

	for (i = 0; i < n; i++)
	{
		if (*s == 0)
		{
			break;
		}

		wprintf(L"%lc", (wchar_t)*s++);
	}
}

static
void PrintPldmString(UINT16 Handle)
{
	// locate the string
	struct InternalTables *g = &Globals;
	int PackIndex;

	for (PackIndex = 0; PackIndex < g->nPacks; PackIndex++)
	{
		struct PackInfo *Pack = &g->Packs[PackIndex];
		int First = Pack->FirstStringHandle;
		int Last = Pack->FirstStringHandle + Pack->NumStrings - 1;

		if (First <= Handle && Handle <= Last)
		{
			wprintf(L"P: %x h: %x ", PackIndex, Handle);
#ifdef _LNX
			Print16String(Pack->Strings[Handle - First].String);
#else
			wprintf(L"%s", Pack->Strings[Handle - First].String);
#endif
			return;
		}
	}

	wprintf(L"string %x not found!\n", Handle);
}

static
void
PrintAttribute(PLDM_ATTRIBUTE *Attrib)
{
	ATTRIBUTE_POINTER p;
	int i;
	p.CommonHeader = Attrib;

	wprintf(L"Attrib %x %x", p.CommonHeader->Handle, p.CommonHeader->NameHandle);
	PrintPldmString(p.CommonHeader->NameHandle);

	if (p.CommonHeader->Type == BIOSEnumeration)
	{
		struct Table6Defaults *EnumDefs;
		int length;

		for (i = 0; i < p.Enumeration->PossibleValues; i++)
		{
			wprintf(L" ");
			PrintPldmString(p.Enumeration->Handle[i]);
		}

		length =  sizeof(*p.Enumeration) + p.Enumeration->PossibleValues * sizeof(p.Enumeration->Handle[0]);
		EnumDefs = (struct Table6Defaults *)((char *)p.Enumeration + length);

		if (EnumDefs->NumDefaults)
		{
			wprintf(L" ");
			PrintPldmString(EnumDefs->Handle[0]);
		}
	}
	else if (p.CommonHeader->Type == BIOSBootConfigSetting)
	{
		for (i = 0; i < p.Boot->NumPossible; i++)
		{
			wprintf(L" ");
			PrintPldmString(p.Boot->PossibleDevices[i]);
		}
	}
	else if (p.CommonHeader->Type == BIOSInteger)
	{
		wprintf(L" L %llx U %llx S %x D %llx", p.Numeric->LowerBound, p.Numeric->UpperBound, p.Numeric->ScalarIncrement, p.Numeric->DefaultValue);
	}
	else if (p.CommonHeader->Type == BIOSPassword)
	{
		wprintf(L" Pword Min %x Max %x ", p.PassWord->MinLength, p.PassWord->MaxLength);
	}
}

static
void
PrintValue(PLDM_VALUE *Value)
{
	VALUE_POINTER p;
	int i;

	p.CommonHeader = Value;
	wprintf(L"Value ");

	switch (p.CommonHeader->Type)
	{
		case BIOSEnumeration:
		case BIOSEnumerationReadOnly:
			wprintf(L"EnumIndex %x\n", p.Enumeration->ValueIndex[0]);

			if (p.Enumeration->NumberOfValues != 1)
			{
				wprintf(L"Number of values %x!\n", p.Enumeration->NumberOfValues);
			}

			return;

		case BIOSInteger:
			wprintf(L"numeric %x\n", (UINT32)p.Numeric->Value);
			return;

		case BIOSBootConfigSetting:
			wprintf(L"Boot ");

			for (i = 0; i < p.Boot->NumSettings; i++)
			{
				wprintf(L" %x", p.Boot->Devices[i]);
			}

		default:
			return;
	}
}

static
void
PrintControlStruct(struct Control *Control)
{
	if (Control->Attribute)
	{
		PrintAttribute(Control->Attribute);
		wprintf(L"\n");
		PrintValue(Control->Value);
	}
	else
	{
		wprintf(L" Attribute pointer is null");
	}

	wprintf(L"\n");
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: 	InitFirstUefiString
//
// Description: Initialize context for getting strings from pack
//
// Parameters:  STRING_CONTEXT *
//                 [in] string context to initialize
//                 [in] Address of string package
//
// Return: none
//
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

static
void
InitFirstUefiString(
	STRING_CONTEXT *Context,
	UINT8 *StringPack)      // address of string package in data buffer
{
	memset(Context, 0, sizeof(*Context));
	Context->Buffer = StringPack;
	Context->Length = ((EFI_HII_STRING_PACKAGE_HDR *)StringPack)->Header.Length;
	Context->BlockOffset = ((EFI_HII_STRING_PACKAGE_HDR *)StringPack)->StringInfoOffset;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: 	GetNextUefiString
//
// Description: Get the next string from a string pack and update context
//              structure with string offset and number.
//
// Parameters:  STRING_CONTEXT *
//                 [in out] string context to initialize
//
// Return: EFI_SUCCESS on success, EFI_NOT_FOUND if end marker found.
//               exits with error message on corrupt data.
//
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

static
EFI_STATUS GetNextUefiString(STRING_CONTEXT *Context)
{
	UINT8 BlockType;
	UINT8 *Buffer = Context->Buffer;
	int StringSize;
	BlockType = *(Context->BlockOffset + Buffer);

	switch (BlockType)
	{
		case EFI_HII_SIBT_STRING_UCS2:  // singleton string
			Context->StringOffset = Context->BlockOffset + sizeof(EFI_HII_STRING_BLOCK);
			StringSize = (int)wcslen16((UINT16 *) (Context->StringOffset + Buffer));
			Context->StringIndex++;
			// advance to next block
			Context->BlockOffset += (StringSize + 1) * sizeof(UINT16) + sizeof(EFI_HII_STRING_BLOCK);
			break;

		case EFI_HII_SIBT_STRINGS_UCS2: // multiple strings in sequence
		{
			EFI_HII_SIBT_STRINGS_UCS2_BLOCK *StringBlock;
			StringBlock = (EFI_HII_SIBT_STRINGS_UCS2_BLOCK *)(Context->Buffer + Context->BlockOffset);

			if (Context->StringOffset < Context->BlockOffset)
			{
				// this is first string
				if (0 == StringBlock->StringCount)
				{
					// empty!
					Context->BlockOffset += sizeof(*StringBlock);
					return GetNextUefiString(Context);
				}

				Context->SkipCount = StringBlock->StringCount;
				Context->StringOffset = Context->BlockOffset + sizeof(*StringBlock);
				Context->StringIndex++;
				Context->SkipCount--;
			}
			else
			{
				// skip previous string
				Context->StringIndex++;
				Context->StringOffset = (int)(wcslen16((UINT16 *)(Buffer + Context->StringOffset) + 1) * sizeof(UINT16));
			}

			// last string treatment
			if (Context->SkipCount == 0)
			{
				Context->BlockOffset = ((int)wcslen16((UINT16 *)(Buffer + Context->StringOffset) + 1) * sizeof(UINT16));
			}
		}
		break;

		case EFI_HII_SIBT_SKIP2:
		{
			EFI_HII_SIBT_SKIP2_BLOCK *StringBlock;
			StringBlock = (EFI_HII_SIBT_SKIP2_BLOCK *)(Context->Buffer + Context->BlockOffset);
			Context->StringIndex += StringBlock->SkipCount;
			Context->BlockOffset += sizeof(*StringBlock);
			return GetNextUefiString(Context);
		}
		break;

		case EFI_HII_SIBT_SKIP1:
		{
			EFI_HII_SIBT_SKIP1_BLOCK *StringBlock;
			StringBlock = (EFI_HII_SIBT_SKIP1_BLOCK *)(Context->Buffer + Context->BlockOffset);
			Context->StringIndex += StringBlock->SkipCount;
			Context->BlockOffset += sizeof(*StringBlock);
			return GetNextUefiString(Context);
		}
		break;

		case EFI_HII_SIBT_END:
			return EFI_NOT_FOUND;

		default:
			fwprintf(stderr, L"The BIOS has unsupported string blocks.\n");
			exit(1);
	}

	return EFI_SUCCESS;
}

// find string pack for given language
static
UINT8
*FindLangStringPack(UINT8 *Package, char *Lang)
{
	EFI_HII_PACKAGE_LIST_HEADER *PackageList = ( EFI_HII_PACKAGE_LIST_HEADER * )Package;
	EFI_HII_STRING_PACKAGE_HDR *StringPack;
	EFI_HII_PACKAGE_HEADER *PackageHdr;
	UINT32 Offset;

	for (Offset = sizeof(*PackageList); Offset < PackageList->PackageLength; Offset += PackageHdr->Length)
	{
		PackageHdr = (EFI_HII_PACKAGE_HEADER *)(Package + Offset);

		if (PackageHdr->Type == EFI_HII_PACKAGE_STRINGS)
		{
			StringPack = (EFI_HII_STRING_PACKAGE_HDR * )PackageHdr;

			if (strcmp(Lang, (char *)StringPack + LANG_OFFSET_UEFI21) == 0)
			{
				return (UINT8 *)Package + Offset;
			}

		}
	}

	return 0;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 FindStringPack
//
// Description:  Find English string pack for formset
//
// Parameters:	 UINT8 *
//					[in] export package address
//
// Return: UINT8 * - string pack address on Success, NULL on Failure
//
//
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
UINT8
*FindStringPack(UINT8 *Package)
{
	EFI_HII_PACKAGE_LIST_HEADER *PackageList = ( EFI_HII_PACKAGE_LIST_HEADER * )Package;
	EFI_HII_STRING_PACKAGE_HDR *StringPack;
	EFI_HII_PACKAGE_HEADER *PackageHdr;
	UINT32 Offset;

	for (Offset = sizeof(*PackageList); Offset < PackageList->PackageLength; Offset += PackageHdr->Length)
	{
		PackageHdr = (EFI_HII_PACKAGE_HEADER *)(Package + Offset);

		if (PackageHdr->Type == EFI_HII_PACKAGE_STRINGS)
		{
			StringPack = (EFI_HII_STRING_PACKAGE_HDR * )PackageHdr;

			if (strcmp("en-US", (char *)StringPack + LANG_OFFSET_UEFI21) == 0
					|| strcmp("en", (char *)StringPack + LANG_OFFSET_UEFI21) == 0)
			{
				return (UINT8 *)Package + Offset;
			}

		}
	}

	return 0;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 AddressGlobals
//
// Description:  Get the address of PLDM global data
//
// Parameters:	 none
//
// Return: Struct InternalTables * - Address of PLDM global data
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
struct InternalTables *AddressGlobals()
{
	return &Globals;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 GetPldmString
//
// Description:  Get address of PLDM string structure for given handle
//
// Parameters:	 UINT16
//					[in] handle of the string
//
// Return: struct *String - non-null if found
//
// Modified:
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
struct String
*GetPldmString(UINT16 Handle)
{
	struct InternalTables *g = &Globals;
	int PackIndex;

	// locate the string
	for (PackIndex = 0; PackIndex < g->nPacks; PackIndex++)
	{
		struct PackInfo *Pack = &g->Packs[PackIndex];
		int First = Pack->FirstStringHandle;
		int Last = Pack->FirstStringHandle + Pack->NumStrings - 1;

		if (First <= Handle && Handle <= Last)
		{
			return &Pack->Strings[Handle - First];
		}
	}

	DEBUG((DEBUG_ERROR, "String not found handle %x\n", Handle));
	return 0;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 NewString
//
// Description:  Make a new string table entry (for special controls)
//
// Parameters:	 int
//					[in] Index of the Packinfo structure
//				 UINT16 *
//				    [in] null terminated UCS-2 string
//
// Return: efi handle of new string
//
// Modified: Pack->MapStrings[]
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
UINT16
NewString(int PackIndex, UINT16 * pString)  // create a new string table entry
{
	struct InternalTables *g = AddressGlobals();
	struct PackInfo *Pack = g->Packs + PackIndex;
	struct String String;

	String.EfiHandle = Pack->NumStrings++;  // efi numbered from 1
	String.Referenced = TRUE;
	String.Allocated = 0;
	String.String = pString;
	// add to end of current pack
	Pack->Strings = (struct String *)realloc(Pack->Strings, Pack->NumStrings * sizeof(Pack->Strings[0]));

	if (0 == Pack->Strings)
	{
		throw (L"NewString: memory exhausted");
	}

	Pack->Strings[Pack->NumStrings - 1] = String;
	Pack->ExtraStrings++;

	// add a copy to map strings as well
	if (Pack->MapStrings)
	{
		Pack->MapStrings = (struct String *)realloc(Pack->MapStrings, Pack->NumStrings * sizeof(Pack->MapStrings[0]));

		if (0 == Pack->MapStrings)
		{
			throw (L"NewString: memory exhausted");
		}

		Pack->MapStrings[Pack->NumStrings - 1] = String;
	}

	return String.EfiHandle;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 FindIfrPack
//
// Description:  Find IFR pack (if any) within an export package
//
// Parameters:	 UINT8 *
//					[in] Address of export package
//
// Return: Address if IFR if found, NULL otherwise
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
UINT8
*FindIfrPack(UINT8 *Package)
{
	EFI_HII_PACKAGE_LIST_HEADER *PackageList = ( EFI_HII_PACKAGE_LIST_HEADER * )Package;
	EFI_HII_PACKAGE_HEADER *PackageHdr;
	UINT32 Offset;

	for (Offset = sizeof(*PackageList); Offset < PackageList->PackageLength; Offset += PackageHdr->Length)
	{
		PackageHdr = (EFI_HII_PACKAGE_HEADER *)(Package + Offset);

		if (PackageHdr->Type == EFI_HII_PACKAGE_FORM_CONFIG)
		{
			return (UINT8 *)Package + Offset;

		}
	}

	return 0;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 GetAttributeLength
//
// Description:  Determine size of PLDM attribute table entry
//
// Parameters:	 PLDM_ATTRIBUTE *
//					[in] Pointer to attribute
//
// Return: int length of entry in bytes, 0 on unknown type
//
// Modified:
//
// Referral(s):
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
int
GetAttributeLength(PLDM_ATTRIBUTE *pAttribute)
{
	ATTRIBUTE_POINTER vp;
	struct Table6Defaults *EnumDefs;
	int length;
	vp.CommonHeader = pAttribute;

	switch (vp.CommonHeader->Type)
	{
		case BIOSEnumeration:
		case BIOSEnumerationReadOnly:
			length =  sizeof(*vp.Enumeration) + vp.Enumeration->PossibleValues * sizeof(vp.Enumeration->Handle[0]);
			EnumDefs = (struct Table6Defaults *)((char *)pAttribute + length);
			return length + sizeof(*EnumDefs) + EnumDefs->NumDefaults * sizeof EnumDefs->Handle[0];

		case BIOSString:
		case BIOSStringReadOnly:
			return sizeof(*vp.String) + vp.String->DefLength;

		case BIOSPassword:
		case BIOSPasswordReadOnly:
			return sizeof(*vp.PassWord) + vp.PassWord->DefLength;

		case BIOSInteger:
		case BIOSIntegerReadOnly:
			return sizeof(*vp.Numeric);

		case BIOSBootConfigSetting:
		case BIOSBootConfigSettingReadOnly:
			return sizeof(*vp.Boot) + vp.Boot->NumPossible * sizeof(vp.Boot->PossibleDevices[0]);

		case BIOSCollection:
		case BIOSCollectionReadOnly:
			return -1;

		default:
			return 0;
	}
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 GetNextString
//
// Description:  Get next string offset from buffer containing PLDM string
//               table
//
// Parameters:	 UINT8 *
//					[in] Address of PLDM string table (including CRC)
//				 INT32
//				    [in] Size of table in bytes
//				 INT32 *
//				    [in out] Place to update with next value of offset
//				           initial value of -1 return offset of first string (0)
//
// Return: EFI_SUCCESS on Success, EFI_NOT_FOUND after final string,
//               EFI_ABORTED if error detected
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
GetNextString(UINT8 *Buffer, INT32 BufferSize, INT32 *CurrentOffset)
{
	PLDM_STRING *String;
	INT32 Offset = *CurrentOffset;
	INT32 Len;

	if (-1 == Offset)
	{
		*CurrentOffset = 0;
		return EFI_SUCCESS;
	}

	BufferSize -= 4;        // deduct CRC32

	if (Offset > BufferSize)
	{
		return EFI_ABORTED;
	}

	if (BufferSize % 4)
	{
		return EFI_ABORTED;    // pad bytes assure multiple of 4
	}

	// check for end of table
	if (BufferSize - Offset < 4)
	{
		return EFI_ABORTED;    // we are already at end
	}

	String = (PLDM_STRING *)(Buffer + Offset);
	Len = sizeof(*String) + String->Length;
	Offset += Len;  // skip current string

	if (BufferSize - Offset < 4)
	{
		return EFI_NOT_FOUND;    // we are at end
	}

	if (Offset > BufferSize)
	{
		return EFI_ABORTED;
	}

	*CurrentOffset = Offset;
	return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 StringByHandle
//
// Description:  Get a PLDM string table entry given a string handle
//
// Parameters:	 UINT8 *
//					[in] Address of PLDM string table (includeing CRC)
//				 INT32
//				    [in] Size of table in bytes
//				 UINT16
//				    [in] PLDM string handle
//
// Return: PLDM_STRING * address of string, - 0 on failure to find
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
PLDM_STRING *
StringByHandle(UINT8 *Strings, int Size, UINT16 Handle)
{
	PLDM_STRING *String;
	INT32 Offset;

	for (Offset = -1; EFI_SUCCESS == GetNextString(Strings, Size, &Offset); )
	{
		String = (PLDM_STRING *)(Strings + Offset);

		if (String->Handle == Handle)
		{
			return String;
		}
	}

	return NULL;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 GetNextAttribute
//
// Description:  Get next attribute from  PLDM attribute table
//
// Parameters:	 UINT8 *
//					[in] Address of PLDM attribute table
//				 INT32
//				    [in] Size of table in bytes
//				 INT32 *
//				    [in out] Place to update with next value of offset
//				           initial value of -1 return offset of first attribute (0)
//
// Return: EFI_SUCCESS on Success, EFI_NOT_FOUND after final attribute,
//               EFI_ABORTED if error detected
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
GetNextAttribute(UINT8 *Buffer, INT32 BufferSize, INT32 *CurrentOffset)
{
	PLDM_ATTRIBUTE *Val;
	INT32 Offset = *CurrentOffset;
	INT32 Len;

	if (-1 == Offset)
	{
		*CurrentOffset = 0;
		return EFI_SUCCESS;
	}

	BufferSize -= 4;        // deduct CRC32

	if (Offset > BufferSize)
	{
		return EFI_ABORTED;
	}

	if (BufferSize % 4)
	{
		return EFI_ABORTED;    // pad bytes assure multiple of 4
	}

	// check for end of table
	if (BufferSize - Offset < 4) // every value is at least 4 bytes
	{
		return EFI_ABORTED;    // we are already at end
	}

	Val = (PLDM_ATTRIBUTE *)(Buffer + Offset);
	Len = GetAttributeLength(Val);

	if (0 == Len)
	{
		return EFI_ABORTED;
	}

	Offset += Len;  // skip current control

	// check for end of table
	if (BufferSize - Offset < 4)
	{
		return EFI_NOT_FOUND;
	}

	if (Offset > BufferSize)
	{
		return EFI_ABORTED;
	}

	*CurrentOffset = Offset;
	return EFI_SUCCESS;
}
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 GetValueLength
//
// Description:  Determine size in bytes of given attribute
//
// Parameters:	 PLDM_VALUE *
//					[in] Attribute Address
//
// Return:       int RetValue - non 0 on Success, 0 on Failure
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
int
GetValueLength(PLDM_VALUE *pValue)
{
	VALUE_POINTER vp;
	vp.CommonHeader = pValue;

	switch (vp.CommonHeader->Type)
	{
		case BIOSEnumeration:
		case BIOSEnumerationReadOnly:
			return sizeof(*vp.Enumeration) + vp.Enumeration->NumberOfValues;

		case BIOSString:
		case BIOSStringReadOnly:
			return sizeof(*vp.String) + vp.String->Length;

		case BIOSPassword:
		case BIOSPasswordReadOnly:
			return sizeof(*vp.PassWord) + vp.PassWord->Length;

		case BIOSInteger:
		case BIOSIntegerReadOnly:
			return sizeof(*vp.Numeric);

		case BIOSBootConfigSetting:
		case BIOSBootConfigSettingReadOnly:
			return sizeof(*vp.Boot) + vp.Boot->NumSettings;

		case BIOSCollection:
		case BIOSCollectionReadOnly:
			return -1;

		default:
			return 0;
	}
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 AttributeByHandle
//
// Description:  Find an attribute given its handle
//
// Parameters:	 UINT8 *
//					[in] Address of PLDM attribute table
//				 INT32
//				    [in] Size of table in bytes
//				 UINT16
//				    [in] attribute handle
//
// Return:       PLDM_ATTRIBUTE * - non 0 pointer on Success, 0 on Failure
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
PLDM_ATTRIBUTE *
AttributeByHandle(UINT8 *Attributes, UINT32 Size, UINT16 Handle)
{
	int Offset;
	PLDM_ATTRIBUTE *Attribute;

	for (Offset = -1; EFI_SUCCESS == GetNextAttribute(Attributes, Size, &Offset); )
	{
		Attribute = (PLDM_ATTRIBUTE *)(Attributes + Offset);

		if (Attribute->Handle == Handle)
		{
			return Attribute;
		}
	}

	return NULL;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 GetNextValue
//
// Description:  Get next value from  PLDM value table
//
// Parameters:	 UINT8 *
//					[in] Address of PLDM value table
//				 INT32
//				    [in] Size of table in bytes
//				 INT32 *
//				    [in out] Place to update with next value of offset
//				           initial value of -1 return offset of first value (0)
//
// Return: EFI_SUCCESS on Success, EFI_NOT_FOUND after final value,
//               EFI_ABORTED if error detected
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
GetNextValue(UINT8 *Buffer, INT32 BufferSize, INT32 *CurrentOffset)
{
	PLDM_VALUE *Val;
	INT32 Offset = *CurrentOffset;
	INT32 Len;

	if (-1 == Offset)
	{
		*CurrentOffset = 0;
		return EFI_SUCCESS;
	}

	BufferSize -= 4;        // deduct CRC32

	if (Offset > BufferSize)
	{
		return EFI_ABORTED;
	}

	if (BufferSize % 4)
	{
		return EFI_ABORTED;    // pad bytes assure multiple of 4
	}

	// check for end of table
	if (BufferSize - Offset < 4)
	{
		return EFI_ABORTED;    // we are already at end
	}

	Val = (PLDM_VALUE *)(Buffer + Offset);
	Len = GetValueLength(Val);

	if (0 == Len)
	{
		return EFI_ABORTED;
	}

	Offset += Len;

	// check for end of table
	if (BufferSize - Offset < 4)
	{
		return EFI_NOT_FOUND;
	}

	if (Offset > BufferSize)
	{
		return EFI_ABORTED;
	}

	*CurrentOffset = Offset;
	return EFI_SUCCESS;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 TraverseControls
//
// Description:  Traverse formset calling provided function for each control.
//               Constructs a fake CONTROL_INFO for each form and formset to
//               implement form and formset PLDM collections.
//               Skips over the 0x401 form with duplicate controls.
//
// Parameters:	 int
//					[in] Index to pack info structure in globals
//				 Pointer to function
//				    function to call for each control
//				 void *
//				    Additonal pointer to pass to called function
//
// Return:       EFI_SUCCESS on success. If called function returns non-zero
//               status then return immediately with that value.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
EFI_STATUS
TraverseControls(int PackIndex, EFI_STATUS (*func)(int PackIndex, CONTROL_INFO *Control, void *farg), void *farg)
{
	UINT8 *CurrentFormSet = 0;
	UINT8 *CurrentForm = 0;
	UINT16 FormSetTitle = 0;
	UINT8* CurrentPackageList = 0;
	UINT8 *CurrentControl = 0;
	UINT16 Class = 0;
	UINT16 SubClass = 0;
	UINT16 FormSetHelp;
	UINT16 FormID = 0;
	UINT16 FormTitle = 0;
	GUID Guid = {0};
	EFI_STATUS Status = EFI_SUCCESS;
	struct InternalTables *g = &Globals;
	CONTROL_INFO *Control;
	CONTROL_INFO NewControl;
	// dummy amisce structure for GetNextUefiControl()
	HII_DATA AmiSceData;
	memset(&AmiSceData, 0, sizeof(AmiSceData));
    AmiSceData.HIIDBData = g->HIIDBData;
	CurrentPackageList = g->Packs[PackIndex].Package;
	CurrentFormSet = GetNextUefiFormSet( CurrentPackageList, &CurrentFormSet, ( UINT8 * )&Guid,
										 &FormSetTitle, &FormSetHelp, &Class, &SubClass );

    // check for class does not work for every form set
    if (CurrentFormSet == NULL /* || Class == 0*/)
	{
		return EFI_NOT_FOUND;
	}

	for (CurrentForm = 0; (CurrentForm = GetNextUefiForm( CurrentFormSet, &CurrentForm, &FormID, &FormTitle)); )
	{
		// each form in formset
		for (Control = 0; (Control = GetNextUefiControl( CurrentForm, &CurrentControl, &AmiSceData, CurrentPackageList, &NewControl)); )
		{
			// each control in form
			// skip controls the 401 form with blank name -- it will create ambiguous map strings.
			if (FormID == 0x401 && FormTitle == 2)
			{
				FreeControl(Control);
				continue;
			}

			Status = (*func)(PackIndex, Control, farg);

		}
	}

	return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 FreeControl
//
// Description:  free a control's options
//
// Parameters:	 CONTROL_INFO *
//					[in] Address of control info struct
//
// Return:
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
void
FreeControl(CONTROL_INFO *Control)
{
	OPTION_LIST *temp, *temp1;

	if (Control->ValueInfo)
	{
		if (Control->ValueInfo->Options)
		{
			temp = Control->ValueInfo->Options;

			while (temp)
			{
				temp1 = temp->Next;
				free(temp);
				temp = temp1;
			}
		}

		free(Control->ValueInfo);
	}
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 IsNeededControl
//
// Description:  Check a control to see if it is needed for PLDM
//
// Parameters:	 CONTROL_INFO *
//					[in] Address of control info struct
//
// Return:      TRUE if needed, FALSE otherwise
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
BOOLEAN
IsNeededControl(CONTROL_INFO *Control)
{
	UINT16 Type = Control->ControlType;
	VARIABLE Variable;
	UINT8 *CurValue = NULL;
	UINT64 Value = 0;	
    struct InternalTables *g = AddressGlobals();

	// reject controls with 0 prompt string
	if (Control->ControlPrompt == 0)
	{
		return FALSE;
	}

	// Check for valid variable
	if (GetVariableById(Control->VariableStorage, &Variable))
	{
		//Don't export the control if the variable size is 0 / control's offset + width exceeds variable size
		if ((Variable.VariableSize == 0) || ((Control->DataOffset + Control->DataWidth) > Variable.VariableSize))
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}

	if (IsOEMSpecialControl(Control))
	{
		switch ( GetSpecialKey( Control->ConditionalVariable[0] ))
		{
			case LANGUAGE_CONTROL:
			case PLATFORM_LANGUAGE_CONTROL:
				return TRUE;

			case BOOT_ORDER_CONTROL:
				if ( PcdGetBool(PcdBootOrderPreserveSupport) )
				{
					return TRUE;
				}
				else
				{
					return FALSE;
				}

			case LEGACY_DEV_ORDER_CONTROL:
				if ( PcdGetBool(PcdLegacyDevOrderPreserveSupport) )
				{
					return TRUE;
				}
				else
				{
					return FALSE;
				}

			default:
				return FALSE;
		}
	}

	switch (Type)
	{
		case EFI_IFR_NUMERIC_OP:
		case EFI_IFR_CHECKBOX_OP:
		case EFI_IFR_ONE_OF_OP:
		case EFI_IFR_STRING_OP:
		case EFI_IFR_DATE_OP:
		case EFI_IFR_TIME_OP:
		case EFI_IFR_ORDERED_LIST_OP:
            // if OptimalDefaults value and Current Value is same then those controls will be added to table
            // else those controls will be ignored.
		    if(!gPreserveMode)
		    {
                if((Control->ControlType != EFI_IFR_STRING_OP)&&(Control->ControlType != EFI_IFR_PASSWORD_OP))
                {
                    if (0 == gIsStdBuf)
                        return FALSE;	// no defaults variable!!
                    if (0 == _GetStdDefaults(Control))
                    {
                        return FALSE;	// this control has no defaults
                    }
                    else
                    {
                        BOOLEAN IsEqual = FALSE;
                        CurValue = SceGetQuestionValue( Control->VariableStorage, Control->DataOffset, Control->DataWidth );
                        if(Control->ControlType != EFI_IFR_DATE_OP && Control->ControlType != EFI_IFR_TIME_OP)
                        {
                            Value = GetValueFromBuffer( CurValue, Control->DataWidth );
                        }

                        if(Control->ControlType == EFI_IFR_DATE_OP)
                        {
                            EFI_HII_DATE* Currentdate = NULL;
                            EFI_HII_DATE* Defaultdate = NULL;
                            Currentdate = (EFI_HII_DATE*)CurValue;
                            Defaultdate = (EFI_HII_DATE*)&(Control->ValueInfo->OptimalDefaults);
                            if(Currentdate->Year == Defaultdate->Year && Currentdate->Month == Defaultdate->Month && Currentdate->Day == Defaultdate->Day)
                            {
                                IsEqual = TRUE;
                            }
                        }
                        else if(Control->ControlType == EFI_IFR_TIME_OP)
                        {

                            EFI_HII_TIME* CurrentTime = NULL;
                            EFI_HII_TIME* DefaultTime = NULL;


                            CurrentTime = (EFI_HII_TIME*)CurValue;
                            DefaultTime = (EFI_HII_TIME*)&(Control->ValueInfo->OptimalDefaults);
                            if(CurrentTime->Hour == DefaultTime->Hour && CurrentTime->Minute == DefaultTime->Minute && CurrentTime->Second == DefaultTime->Second)
                            {
                                IsEqual = TRUE;
                            }
                        }
                        else if(Value == Control->ValueInfo->OptimalDefaults)
                        {
                            IsEqual = TRUE;
                        }

                        if(IsEqual)
                        {
                            if(CurValue)
                            {
                                free(CurValue);
                                CurValue = NULL;
                            }
                            return FALSE;
                        }
                        else
                        {
                            if(CurValue)
                            {
                                free(CurValue);
                                CurValue = NULL;
                            }
                            return TRUE;
                        }
                    }
                }
		    }
		    return TRUE ;
        case EFI_IFR_PASSWORD_OP:
            if ( PcdGetBool(PcdPasswordPreserveSupport) )
            {
                return TRUE;
            }
	}

	return FALSE;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 CountFunc
//
// Description:  Control counter structure - called by TraverseControls()
//
// Parameters:	 int
//                  [in] not used
//               CONTROL_INFO *
//					[in] control to possibly count
//				 void *
//				    [in out] pointer to integer counter
//
// Return:      TRUE if needed, FALSE otherwise
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
EFI_STATUS
CountFunc(int Pack, CONTROL_INFO *Control, void *Count)
{
	if (IsNeededControl(Control) == FALSE)
	{
		FreeControl(Control);
		return 0;
	}

	*((int *)Count) += 1;
	FreeControl(Control);
	return 0;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 CountControls
//
// Description:  Count number of controls in formst of interest to PLDM
//
// Parameters:	 int
//					[in] Index to PackInfo struct
//
// Return:      Number of controls
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
int
CountControls(int PackNo)
{
	int Count = 0;
	TraverseControls(PackNo, CountFunc, (void *)&Count);
	// free variables
	_FreeVariableList();
	// languages????
	return Count;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 MakeLangAttribute
//
// Description:  Make language special control attribute
//               encoded like a oneof
//
// Parameters:	 CONTROL_INFO *
//					[in] pointer to info struct
//				 int
//				    index to PackInfo structure
//
// Return:       PLDM_ATTRIBUTE * - pointer to created attribute
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
PLDM_ATTRIBUTE
*MakeLangAttribute(CONTROL_INFO *Control, int PackIndex)
{
	struct InternalTables *g = AddressGlobals();
	struct PackInfo *Pack = g->Packs + PackIndex;
	UINT8 *Language = NULL;     // the current language
	UINT32 i = 0;
	UINT32 Setting = 0;
	wchar_t Lang[RFC_3066_ENTRY_SIZE] = {0};
	ATTRIBUTE_POINTER p;
	struct Table6 Table6;
	MEMFILE Buffer;
	PLDM_ATTRIBUTE Header = {0};
	struct Table6Defaults Default;

	// get the language options and current value
	if (NULL == gLanguages)
	{
		gLangCount = UpdateLanguageCodes(Control->ConditionalVariable[0]);

		if (gLangCount == 0)
		{
			return NULL;
		}
	}

	Language = SceGetQuestionValue( Control->VariableStorage, Control->DataOffset, GetLangDataWidth() );

	if (Language == 0)
	{
		return 0;
	}

	// convert curent language to wchar_t for comparison below
	mbstowcs(Lang, (char *)Language, strlen((char *)Language));
	free(Language);
	// make our own copy of strings in gLanguages array
	g->nLangStrings = gLangCount;
	g->LangStrings = (UINT16 **)calloc(gLangCount , sizeof(UINT16 *));

	if (0 == g->LangStrings)
	{
		throw (L"MakeLangAttribute: memory exhausted");
	}

	for (i = 0; i < gLangCount; i++)
	{
		wchar_t *LangCode = gLanguages[i].LangCode;
		g->LangStrings[i] = (UINT16 *)malloc((wcslen(LangCode) + 1) * sizeof(UINT16));

		if (0 == g->LangStrings[i])
		{
			throw (L"MakeLangAttribute: memory exhausted");
		}

		// copy wchar_t to UINT16
#ifdef _LNX
		_Wnstrcpy(g->LangStrings[i], LangCode);
#else
		wcscpy(g->LangStrings[i], LangCode);
#endif

// save current setting
		if (0 == wcscmp(Lang, LangCode))
		{
			g->LangSetting = Setting = i;
		}
	}

	// now make a Enumeration
	MemCreat(&Buffer);
	p.Enumeration = &Table6;
	Header.Type = BIOSEnumeration;
	Header.NameHandle = Control->ControlPrompt - 1 + Pack->FirstStringHandle;
	Pack->Strings[Control->ControlPrompt - 1].Referenced = 1;
	*p.CommonHeader = Header;
	p.Enumeration->PossibleValues = gLangCount;
	MemWrite(&Buffer, p.Enumeration, sizeof(*p.Enumeration));

	// loop through languages to get name strings
	for (i = 0; i < gLangCount; i++)
	{
		UINT16 Handle;
		Handle = NewString(PackIndex, g->LangStrings[i]);  // create a new string table entry
		Handle += Pack->FirstStringHandle;
		MemWrite(&Buffer, &Handle, sizeof(Handle));
	}

	// write 0 for NumDefaults
	Default.NumDefaults = 0;
	MemWrite(&Buffer, &Default, sizeof Default.NumDefaults);
	p.CommonHeader = (PLDM_ATTRIBUTE *)Buffer.Buffer;
	Buffer.Buffer = 0;

	if (MemClose(&Buffer))
	{
		throw (L"MakeLangAttribute: Memory exhaustion");
	}

	return p.CommonHeader;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 MakeBootAttribute
//
// Description:  Make boot special control attribute
//               Makes a new string for each boot option
//
// Parameters:	 CONTROL_INFO *
//					[in] pointer to info struct
//				 int
//				    index to PackInfo structure
//
// Return:       PLDM_ATTRIBUTE * - pointer to created attribute
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
PLDM_ATTRIBUTE
*MakeBootAttribute(CONTROL_INFO *Control, int PackIndex)
{
	struct InternalTables *g = AddressGlobals();
	struct PackInfo *Pack = g->Packs + PackIndex;
	ATTRIBUTE_POINTER p;
	struct Table10 Table10;
	MEMFILE Buffer;
	int Index;
	UINT16 Handle;
	UINT16 *Title16 = L"Boot Options";

	if (gBootOptions)
	{
		FreeBootOptions();
	}

	_PopulateGlobalBootOptions();

	p.Boot = &Table10;
	p.CommonHeader->Type = BIOSBootConfigSetting;
// need to get rid of #%d !!!!!!!!!!!!!!
	//p.CommonHeader->NameHandle = Control->ControlPrompt - 1 + Pack->FirstStringHandle;
	p.CommonHeader->NameHandle = NewString(PackIndex, Title16) + Pack->FirstStringHandle;
	p.Boot->Type = 3;       // ??????????????????????????
	Pack->Strings[Control->ControlPrompt - 1].Referenced = 1;

	MemCreat(&Buffer);
	MemWrite(&Buffer, p.Boot, sizeof(*p.Boot));

	DEBUG((DEBUG_ERROR, "\ngBootCount: %x", gBootCount));
	// For each BootOption retrieve the name and create a new string
	for (Index = 0; Index < (int)gBootCount; Index++)
	{
		BOOT_OPTION *BootOption = gBootOptions[Index];
		DEBUG((DEBUG_ERROR, "\nIndex: %x", Index));

		if (BootOption)
		{
			DEBUG((DEBUG_ERROR, "\nAdd Boot"));
			// create the new string
			Handle = NewString(PackIndex, BootOption->Name);
			Handle += Pack->FirstStringHandle;

			if (Index == 0)
			{
				Pack->FirstBootString = Handle;
			}

			MemWrite(&Buffer, &Handle, sizeof(Handle));
		}
		else
		{
			DEBUG((DEBUG_ERROR, "\nNot added"));
			break;
		}
	}

	p.CommonHeader = (PLDM_ATTRIBUTE *)Buffer.Buffer;
	Buffer.Buffer = 0;

	if (MemClose(&Buffer))
	{
		throw (L"MakeBootAttribute: Memory exhaustion");
	}

	p.Boot->NumPossible = Index;
	p.Boot->Mode = 3;   // ???????????????????
	p.Boot->Type = BOOTDefaultAndNext;   // ???????????????????
	p.Boot->MinNumSettings = 1;
	p.Boot->MaxNumSettings = p.Boot->NumPossible;
	return p.CommonHeader;
}

// put me in pldm.h!!
#pragma pack(push, 1)
struct LegacyBoot
{
	LEGACY_ORDER_ENTRY Entry;
	UINT16             Order[0];
};
#pragma pack(pop)
#define MAX_BOOT 1024

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 MakeLegacyAttribute
//
// Description:  Make Legacy special control attribute(s)
//               Makes a new string for each BBS device
//               Note well: an attribute is created for each sublist
//
// Parameters:	 CONTROL_INFO *
//					[in] pointer to info struct
//				 int
//				    index to PackInfo structure
//
// Return:       PLDM_ATTRIBUTE * - pointer to created attribute
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
PLDM_ATTRIBUTE
*MakeLegacyAttribute(CONTROL_INFO *Control, int PackIndex)
{
	struct InternalTables *g = AddressGlobals();
	struct PackInfo *Pack = g->Packs + PackIndex;
	GUID LegacyDevGuid = LEGACY_DEV_ORDER_GUID;
	ATTRIBUTE_POINTER p;
	struct LegacyBoot *LegacyBoot;
	UINT64 DevOrderCount = 0;
	wchar_t *Title;
	//wchar_t *TitleStr;
	wchar_t TitleString[MAX_BOOT];
	UINT16 *Title16;
	int i;

	if (NULL == g->LegacyDevOrder)
	{
		UINT32 Attributes = 0;
		// this is first time, get the variable
		g->LegacyDevOrder = ( UINT8 * )VarGetNvramName( (wchar_t *)L"LegacyDevOrder", &LegacyDevGuid, &Attributes, (UINTN *)&DevOrderCount );
		// and initialize data items
		g->OrderOffset = 0;

		// count number of orders and validate variable
		for (LegacyBoot = (struct LegacyBoot *)g->LegacyDevOrder;
				(UINT8 *)LegacyBoot < g->LegacyDevOrder + DevOrderCount;
				// Note: length of list is number of bytes including the length field
				LegacyBoot  = (struct LegacyBoot *)((UINT8 *)LegacyBoot + sizeof(*LegacyBoot) + LegacyBoot->Entry.Length - sizeof(LegacyBoot->Entry.Length))
			)
		{
			g->NumLegacy++;
		}

		if ((UINT8 *)LegacyBoot > g->LegacyDevOrder + DevOrderCount)
		{
			fwprintf(stderr, L"Warning: LegacyDevOrder variable corrupt\n");
			g->NumLegacy = 0;
			return 0;
		}
	}

	// set pointer to current order
	LegacyBoot = (struct LegacyBoot *)(g->LegacyDevOrder + g->OrderOffset);
	// allocate control
	p.CommonHeader = (PLDM_ATTRIBUTE *)malloc(sizeof(*p.Boot) + LegacyBoot->Entry.Length);

	if (NULL == p.CommonHeader)
	{
		throw (L"MakeLegacyAttribute: Memory Exhausted");
	}

	// make control name BBS_ .... Boot #
#ifdef _LNX // temporary
	Title = PldmGetHiiString(Pack->Package, g->HIIDBData, Control->ControlPrompt);
#else
	Title = PldmGetHiiString(Pack->Package, g->HIIDBData, Control->ControlPrompt, FALSE);
#endif

	if (wcschr(Title, '%'))
	{
		*wcschr(Title, L'%') = 0;
	}

	swprintf(TitleString, MAX_BOOT, L"%"_S L" %"_S, GetDevicePathTypeString(LegacyBoot->Entry.DeviceType), Title);
	Title16 = (UINT16 *)malloc((wcslen(TitleString) + 1) * sizeof(TitleString[0]));

	if (0 == Title16)
	{
		throw (L"MakeLegacyAttribute: Memory Exhausted");
	}

#ifdef _LNX
	_Wnstrcpy(Title16, TitleString);
#else
	wcscpy(Title16, TitleString);
#endif
	p.CommonHeader->NameHandle = NewString(PackIndex, Title16) + Pack->FirstStringHandle;
	// need to mark string as allocated so it can be freed when done
	//Pack->Strings[Pack->NumStrings - 1].Allocated = 1;
	p.CommonHeader->Type = BIOSBootConfigSetting;
	p.Boot->Type = BOOTUnknown;       // ??????????????????????????
	p.Boot->Mode = 3;                   // ????????????????
	Pack->Strings[Control->ControlPrompt - 1].Referenced = 1;

	for (i = 0; i < LegacyBoot->Entry.Length / 2 - 1; i++)
	{
		// make the string for each device BBS_ ... followed by number in hex
		swprintf(TitleString, MAX_BOOT, L"%"_S L" %04x", GetDevicePathTypeString(LegacyBoot->Entry.DeviceType), LegacyBoot->Order[i]);
		Title16 = (UINT16 *)malloc((wcslen(TitleString) + 1) * sizeof(TitleString[0]));

		if (Title16)
#ifdef _LNX
			_Wnstrcpy(Title16, TitleString);

#else
			wcscpy(Title16, TitleString);
#endif
		else
		{
			throw (L"MakeLegacyAttibute: Memory Exhausted");
		}

		p.Boot->PossibleDevices[i] = NewString(PackIndex, Title16) + Pack->FirstStringHandle;
	}

	p.Boot->NumPossible = i;
	p.Boot->MinNumSettings = 1;
	p.Boot->MaxNumSettings = p.Boot->NumPossible;

	g->OrderOffset += sizeof(LegacyBoot->Entry) + LegacyBoot->Entry.Length - sizeof(LegacyBoot->Entry.Length);
	free(Title);
	return p.CommonHeader;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 MakeSpecialAttribute
//
// Description:  make a special control attribute
//
// Parameters:
//               CONTROL_INFO *
//                  [in] pointer to control info struct
//               int
//					[in] Index to PackInfo struct
//
// Return:       PLDM_ATTRIBUTE * - pointer to created attribute
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
PLDM_ATTRIBUTE
*MakeSpecialAttribute(CONTROL_INFO *Control, int PackIndex)
{
	switch ( GetSpecialKey( Control->ConditionalVariable[0] ))
	{
		case LANGUAGE_CONTROL:
		case PLATFORM_LANGUAGE_CONTROL:
			return MakeLangAttribute(Control, PackIndex);

		case BOOT_ORDER_CONTROL:
			if (PcdGetBool(PcdBootOrderPreserveSupport) )
			{
				return MakeBootAttribute(Control, PackIndex);
			}
			else
			{
				return NULL;
			}

		case LEGACY_DEV_ORDER_CONTROL:
			if ( PcdGetBool(PcdLegacyDevOrderPreserveSupport) )
			{
				return MakeLegacyAttribute(Control, PackIndex);
			}
			else
			{
				return NULL;
			}

		default:
			return NULL;
	}
}

// given a control, create a PLDM attribute table entry
static
PLDM_ATTRIBUTE // need a way to return an error?
*MakeAttribute(CONTROL_INFO *Control, int PackIndex)
{
	MEMFILE Buffer;
	struct InternalTables *g = AddressGlobals();
	struct PackInfo *Pack = &g->Packs[PackIndex];
	PLDM_ATTRIBUTE Header;
	PLDM_ATTRIBUTE *ReturnValue;
	BOOLEAN bStdDef = 0; //Will be made 1 if the control has std default
	BOOLEAN bMfgDef = 0; //Will be made 1 if the control has mfg default


	if (IsOEMSpecialControl(Control))
	{
		DEBUG((DEBUG_ERROR, "\nMake Special"));
		return MakeSpecialAttribute(Control, PackIndex);
	}

	if (Control->ControlType != EFI_IFR_STRING_OP && Control->ControlType != EFI_IFR_PASSWORD_OP && Control->ControlType != EFI_IFR_ORDERED_LIST_OP)
	{
		// fails for string op because data is memcpy()d to 64 bit number
        // For OrderedList op also, it is copied to 64bit number, but actually we need a list.
		if (gIsStdBuf)
		{
			bStdDef = _GetStdDefaults(Control);
		}

		if (gIsMfgBuf)
		{
			bMfgDef = _GetMfgDefaults(Control);
		}
	}

	// mark control prompt as used  // check value??
	Pack->Strings[Control->ControlPrompt - 1].Referenced = 1;
	Header.Handle = 0;      // caller assigns
	Header.NameHandle = Control->ControlPrompt - 1 + Pack->FirstStringHandle;

	if (Control->ControlType == EFI_IFR_NUMERIC_OP || Control->ControlType == EFI_IFR_CHECKBOX_OP 
	        || Control->ControlType == EFI_IFR_DATE_OP || Control->ControlType == EFI_IFR_TIME_OP)
	{
		// numeric or checkbox
		ATTRIBUTE_POINTER p;
		p.CommonHeader = (PLDM_ATTRIBUTE *)malloc(sizeof(*p.Numeric));

		if (0 == p.CommonHeader)
		{
			throw (L"MakeAttribute: Memory exhausted");
		}

		Header.Type = BIOSInteger;
		Pack->Strings[Control->ControlPrompt - 1].Referenced = 1;
		// let caller fill in handle
		*p.CommonHeader = Header;
		p.Numeric->DefaultValue = Control->ValueInfo->OptimalDefaults;

		if (Control->ControlType == EFI_IFR_CHECKBOX_OP)
		{
			p.Numeric->ScalarIncrement = 0;
			p.Numeric->LowerBound = 0;
			p.Numeric->UpperBound = 1;
		}
		else if (Control->ControlType == EFI_IFR_DATE_OP || Control->ControlType == EFI_IFR_TIME_OP)
        {
            p.Numeric->ScalarIncrement = 0;
            p.Numeric->LowerBound = Control->ValueInfo->MinValue;
            p.Numeric->UpperBound = Control->ValueInfo->MaxValue;
        }
		else // numeric
		{
			EFI_IFR_NUMERIC *Numeric = (EFI_IFR_NUMERIC*)Control->ControlPtr;
			p.Numeric->LowerBound = Control->ValueInfo->MinValue;
			p.Numeric->UpperBound = Control->ValueInfo->MaxValue;
#if 0   // SCE does not implement
			p.Numeric->ScalarIncrement = Control->ValueInfo->Step;
#else

			switch (Numeric->Flags & EFI_IFR_NUMERIC_SIZE)
			{
				case EFI_IFR_NUMERIC_SIZE_1:
					p.Numeric->ScalarIncrement = Numeric->data.u8.Step;
					break;

				case EFI_IFR_NUMERIC_SIZE_2:
					p.Numeric->ScalarIncrement = Numeric->data.u16.Step;
					break;

				case EFI_IFR_NUMERIC_SIZE_4:
					p.Numeric->ScalarIncrement = Numeric->data.u32.Step;
					break;

				case EFI_IFR_NUMERIC_SIZE_8:
					p.Numeric->ScalarIncrement = (UINT32)Numeric->data.u64.Step;
					break;

				default:
					p.Numeric->ScalarIncrement = 1;
			}

#endif
		}

		return p.CommonHeader;
	}

	// remainder are variable length
	MemCreat(&Buffer);

	if (Control->ControlType == EFI_IFR_STRING_OP)
	{
		// string control
		ATTRIBUTE_POINTER p;
		p.CommonHeader = (PLDM_ATTRIBUTE *)malloc(sizeof(*p.String));

		if (0 == p.CommonHeader)
		{
			throw (L"MakeAttribute: Memory exhausted");
		}

		Header.Type = BIOSString;
		Pack->Strings[Control->ControlPrompt - 1].Referenced = 1;
		*p.CommonHeader = Header;
		p.String->MinLength = (UINT16)Control->ValueInfo->MinValue;
		p.String->MaxLength = (UINT16)Control->ValueInfo->MaxValue;
		p.String->DefLength = 0;
		MemWrite(&Buffer, p.String, sizeof(*p.String));
		// Sce does't know how to do string defaults yet
		free(p.CommonHeader);
	}
	else if (Control->ControlType == EFI_IFR_ONE_OF_OP)
	{
		// enumeration type
		ATTRIBUTE_POINTER p;
		OPTION_LIST *Option;
		struct Table6 Table6;
		struct Table6Defaults Defaults;
		UINT16 DefaultHandle;
		p.Enumeration = &Table6;
		Defaults.NumDefaults = 0;
		Header.Type = BIOSEnumeration;
		Pack->Strings[Control->ControlPrompt - 1].Referenced = 1;
		*p.CommonHeader = Header;
		p.Enumeration->PossibleValues = 0;  // update after loop
		MemWrite(&Buffer, p.Enumeration, sizeof(*p.Enumeration));

		// loop through options to get default names
		for (Option = Control->ValueInfo->Options; Option; Option = Option->Next)
		{
			UINT16 Handle;
			Handle = Option->Option - 1 + Pack->FirstStringHandle;
			Pack->Strings[Option->Option - 1].Referenced = 1;
			MemWrite(&Buffer, &Handle, sizeof(Handle));
			// change the pointer to we can update PossibleValues
			p.Enumeration = (struct Table6 *)Buffer.Buffer;

			if (p.Enumeration->PossibleValues >= 0xff)
			{
				fwprintf(stderr, L"WARNING: Too many options\n");
				MemClose(&Buffer);
				return 0;
			}

			if (bStdDef && (Option->Value == Control->ValueInfo->OptimalDefaults))
			{
				// this one is the default
				DefaultHandle = Handle;
				Defaults.NumDefaults = 1;
			}

			p.Enumeration->PossibleValues++;
		}

		MemWrite(&Buffer, &Defaults, sizeof Defaults );

		if (Defaults.NumDefaults)
		{
			MemWrite(&Buffer, &DefaultHandle, sizeof DefaultHandle);
		}

	}
    else if (Control->ControlType == EFI_IFR_ORDERED_LIST_OP)
    {
        // enumeration type
        ATTRIBUTE_POINTER p;
        OPTION_LIST *Option;
        struct Table6 Table6;
        struct Table6Defaults Defaults;
        p.Enumeration = &Table6;
        Defaults.NumDefaults = 0;
        Header.Type = BIOSEnumeration;
        Pack->Strings[Control->ControlPrompt - 1].Referenced = 1;
        *p.CommonHeader = Header;
        p.Enumeration->PossibleValues = 0;  // update after loop
        MemWrite(&Buffer, p.Enumeration, sizeof(*p.Enumeration));

        // loop through options to get default names
        for (Option = Control->ValueInfo->Options; Option; Option = Option->Next)
        {
            UINT16 Handle;
            Handle = Option->Option - 1 + Pack->FirstStringHandle;
            Pack->Strings[Option->Option - 1].Referenced = 1;
            MemWrite(&Buffer, &Handle, sizeof(Handle));
            // change the pointer to we can update PossibleValues
            p.Enumeration = (struct Table6 *)Buffer.Buffer;

            if (p.Enumeration->PossibleValues >= 0xff)
            {
                fwprintf(stderr, L"WARNING: Too many options\n");
                MemClose(&Buffer);
                return 0;
            }

            p.Enumeration->PossibleValues++;
        }

        MemWrite(&Buffer, &Defaults, sizeof Defaults );
    }
	else if (Control->ControlType == EFI_IFR_PASSWORD_OP)
	{
		ATTRIBUTE_POINTER p;
		p.CommonHeader = (PLDM_ATTRIBUTE *)malloc(sizeof(*p.PassWord));

		if (0 == p.CommonHeader)
		{
			throw (L"MakeAttribute: Memory exhausted");
		}

		Header.Type = BIOSPassword;
		Pack->Strings[Control->ControlPrompt - 1].Referenced = 1;
		*p.CommonHeader = Header;
		p.PassWord->EncodeType = 0xff;  // OEM
		// note: Min and Max are in terms of unicode chars
		p.PassWord->MinLength = (UINT16)Control->ValueInfo->MinValue * sizeof(UINT16);
		p.PassWord->MaxLength = (UINT16)Control->ValueInfo->MaxValue * sizeof(UINT16);
		// Sce does't know how to do large defaults yet
		p.PassWord->DefLength = 0;
		MemWrite(&Buffer, p.String, sizeof(*p.String));
		free(p.CommonHeader);
	}
	else
	{
		// don't know how to do this one
		MemClose(&Buffer);
		DEBUG((DEBUG_ERROR, "Unknown control type %x pack %d\n", Control->ControlType, PackIndex));
		return 0;
	}

	ReturnValue = (PLDM_ATTRIBUTE *)Buffer.Buffer;
	Buffer.Buffer = 0;

	if (MemClose(&Buffer))
	{
		throw (L"MakeAttribute: Memory exhaustion");
	}

	return ReturnValue;
}
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 MakeLangValue
//
// Description:  Make value for language control
//
// Parameters:
//               CONTROL_INFO *
//                  [in] pointer to control info struct
//               int
//					[in] Index to PackInfo struct
//
// Return:       PLDM_VALUE * - pointer to created value
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
PLDM_VALUE
*MakeLangValue(CONTROL_INFO *Control, int PackIndex)
{
	struct InternalTables *g = AddressGlobals();
	VALUE_POINTER p;
	p.CommonHeader = (PLDM_VALUE *)malloc(sizeof(*p.Enumeration) + sizeof(UINT8));

	if (0 == p.CommonHeader)
	{
		throw (L"MakeLangValue: Memory exhausted");
	}

	p.CommonHeader->Type = BIOSEnumeration;
	p.Enumeration->NumberOfValues = 1;
	p.Enumeration->ValueIndex[0] = g->LangSetting;
	return p.CommonHeader;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 MakeBootValue
//
// Description:  Make value for boot order control
//
// Parameters:
//               CONTROL_INFO *
//                  [in] pointer to control info struct
//               int
//					[in] Index to PackInfo struct
//
// Return:       PLDM_VALUE * - pointer to created value
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
PLDM_VALUE
*MakeBootValue(CONTROL_INFO *Control, int PackIndex)
{
	VALUE_POINTER p;
	int i;
	p.CommonHeader = (PLDM_VALUE *)malloc(sizeof(*p.Boot) + sizeof(UINT16) * (size_t)gBootCount);

	if (0 == p.CommonHeader)
	{
		throw (L"MakeBootValue: Memory exhausted");
	}

	p.CommonHeader->Type = BIOSBootConfigSetting;
	p.Boot->Mode = 3;   // order and failed through -- no idea what to use here
	p.Boot->Type = 3;   // default and next                     ditto

	for (i = 0; i < (int)gBootCount; i++)
	{
		BOOT_OPTION *BootOption = gBootOptions[i];

		if (BootOption)
		{
			p.Boot->Devices[i] = i;
		}
		else
		{
			break;
		}
	}

	p.Boot->NumSettings = i;
	return p.CommonHeader;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 MakeLegacyValue
//
// Description:  Make a value for legacy boot
//
// Parameters:
//               CONTROL_INFO *
//                  [in] pointer to control info struct
//               ATTRIBUTE_POINTER
//                  [in] pointer union to attribute
//               int
//					[in] Index to PackInfo struct
//
// Return:       PLDM_VALUE * - pointer to created value
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
PLDM_VALUE
*MakeLegacyValue(CONTROL_INFO *Control, ATTRIBUTE_POINTER Attrib, int PackIndex)
{
	VALUE_POINTER p;
	int i;
	p.CommonHeader = (PLDM_VALUE *)malloc(sizeof(*p.Boot) + sizeof(UINT16) * Attrib.Boot->NumPossible);

	if (0 == p.CommonHeader)
	{
		throw (L"MakeLegacyValue: Memory exhausted");
	}

	p.CommonHeader->Type = BIOSBootConfigSetting;
	p.Boot->Type = Attrib.Boot->Type;
	p.Boot->Mode = Attrib.Boot->Mode;
	p.Boot->NumSettings = Attrib.Boot->NumPossible;

	for (i = 0; i < Attrib.Boot->NumPossible; i++)
	{
		p.Boot->Devices[i] = i;
	}

	return p.CommonHeader;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 MakeSpecialValue
//
// Description:  Make a value for special control
//
// Parameters:
//               CONTROL_INFO *
//                  [in] pointer to control info struct
//               ATTRIBUTE_POINTER
//                  [in] pointer union to attribute
//               int
//					[in] Index to PackInfo struct
//
// Return:       PLDM_VALUE * - pointer to created value
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
PLDM_VALUE
*MakeSpecialValue(CONTROL_INFO *Control, ATTRIBUTE_POINTER Attrib, int PackIndex)
{
	switch ( GetSpecialKey( Control->ConditionalVariable[0] ))
	{
		case LANGUAGE_CONTROL:
		case PLATFORM_LANGUAGE_CONTROL:
			return MakeLangValue(Control, PackIndex);

		case BOOT_ORDER_CONTROL:
			return MakeBootValue(Control, PackIndex);

		case LEGACY_DEV_ORDER_CONTROL:
			return MakeLegacyValue(Control, Attrib, PackIndex);

		default:
			return NULL;
	}
}

// given a control, create a PLDM value table entry.
static
PLDM_VALUE
*MakeValue(CONTROL_INFO *Control, ATTRIBUTE_POINTER Attrib, int PackIndex)
{
	MEMFILE Buffer;
	PLDM_VALUE Header = {0};
	UINT8 *CurrentValue = (UINT8 *)NULL;
	UINT64 Value;
	VALUE_POINTER p;


	if (IsOEMSpecialControl(Control))
	{
		return MakeSpecialValue(Control, Attrib, PackIndex);
	}

	if (Control->ControlType == EFI_IFR_NUMERIC_OP || Control->ControlType == EFI_IFR_CHECKBOX_OP)
	{
		// numeric or checkbox
		p.CommonHeader = (PLDM_VALUE *)malloc(sizeof(*p.Numeric));

		if (0 == p.CommonHeader)
		{
			throw (L"MakeValue: Memory exhausted");
		}

		Header.Type = BIOSInteger;
		Header.Handle = 0;      // caller assigns
		*p.CommonHeader = Header;
		// Get the Control Value
		CurrentValue = SceGetQuestionValue( (UINT16)Control->VariableStorage, (UINT16)Control->DataOffset, (UINT16)Control->DataWidth );
		p.Numeric->Value = GetValueFromBuffer( CurrentValue, Control->DataWidth );
		free(CurrentValue);
		return p.CommonHeader;
	}
	else if (Control->ControlType == EFI_IFR_DATE_OP || Control->ControlType == EFI_IFR_TIME_OP)
    {
        // Date or Time
        p.CommonHeader = (PLDM_VALUE *)malloc(sizeof(*p.Numeric));

        if (0 == p.CommonHeader)
        {
            throw (L"MakeValue: Memory exhausted");
        }

        Header.Type = BIOSInteger;
        Header.Handle = 0;      // caller assigns
        *p.CommonHeader = Header;
        // Get the Control Value
        CurrentValue = SceGetQuestionValue( (UINT16)Control->VariableStorage, (UINT16)Control->DataOffset, (UINT16)Control->DataWidth );
        p.Numeric->Value = *(UINT64*)CurrentValue;
        free(CurrentValue);
        return p.CommonHeader;
    }
    else if (Control->ControlType == EFI_IFR_ONE_OF_OP)
	{
		// enumeration
		OPTION_LIST *Option;
		int OptNum = 0;
		p.CommonHeader = (PLDM_VALUE *)malloc(sizeof(*p.Enumeration) + sizeof(UINT8));

		if (0 == p.CommonHeader)
		{
			throw (L"MakeValue: Memory exhausted");
		}

		Header.Type = BIOSEnumeration;
		*p.CommonHeader = Header;
		p.Enumeration->NumberOfValues = 0;  // it is possible to not have a valid initial value
		// search for option that matches current value
		CurrentValue = SceGetQuestionValue( (UINT16)Control->VariableStorage, (UINT16)Control->DataOffset, (UINT16)Control->DataWidth );
		Value = GetValueFromBuffer(CurrentValue, Control->DataWidth);

		for (OptNum = 0, Option = Control->ValueInfo->Options; Option; Option = Option->Next, OptNum++)
		{
			if (Option->Value == Value)
			{
				p.Enumeration->ValueIndex[0] = OptNum;
				p.Enumeration->NumberOfValues = 1;
				break;
			}

		}

		free(CurrentValue);
		return p.CommonHeader;

	}
    else if (Control->ControlType == EFI_IFR_ORDERED_LIST_OP)
    {
        // enumeration
        OPTION_LIST *Option;
        UINT8 NoOfOptions = 0;
        UINT8 PossibleValues = 0;
        UINT64 Handle[255];
        int OptNum = 0;
        int OptIndex = 0;
        
        //Get the max number of options in the list
        NoOfOptions = _GetUefiMaxEntries(Control->ControlPtr);

        p.CommonHeader = (PLDM_VALUE *)malloc(sizeof(*p.Enumeration) + (NoOfOptions * sizeof(UINT8)));

        if (0 == p.CommonHeader)
        {
            throw (L"MakeValue: Memory exhausted");
        }

        Header.Type = BIOSEnumeration;
        *p.CommonHeader = Header;
        p.Enumeration->NumberOfValues = 0;  // it is possible to not have a valid initial value
        // search for option that matches current value
        CurrentValue = SceGetQuestionValue( (UINT16)Control->VariableStorage, (UINT16)Control->DataOffset, (NoOfOptions * (UINT16)Control->DataWidth) );

        //Get the Possible Values of the Ordered List
        for (Option = Control->ValueInfo->Options; Option; Option = Option->Next)
        {
            Handle[PossibleValues] = Option->Value;
            PossibleValues++;
        }
        
        //Update the Indexes for ValueTable based on the current list in the variable.
        for (OptNum = 0; OptNum < NoOfOptions; OptNum++)
        {
            Value = GetValueFromBuffer(CurrentValue+(OptNum * Control->DataWidth), Control->DataWidth);
            OptIndex = 0;
            for (OptIndex = 0; OptIndex < PossibleValues; OptIndex++)
            {
                if(Handle[OptIndex] == Value)
                    break;
            }
            
            p.Enumeration->ValueIndex[p.Enumeration->NumberOfValues] = OptIndex;
            p.Enumeration->NumberOfValues++;
        }

        free(CurrentValue);
        return p.CommonHeader;
    }
	
	// remainder are variable length
	if (Control->ControlType == EFI_IFR_STRING_OP)
	{
		// string control
        UINT16 *String;
		UINT16 Width;
		VALUE_POINTER p;
		MemCreat(&Buffer);
		Header.Type = BIOSString;
		p.CommonHeader = (PLDM_VALUE *)malloc(sizeof * p.String);

		if (0 == p.CommonHeader)
		{
			throw (L"MakeValue: Memory exhausted");
		}

		*p.CommonHeader = Header;
		String = (UINT16 *)SceGetQuestionValue( (UINT16)Control->VariableStorage, (UINT16)Control->DataOffset, (UINT16)Control->DataWidth );

        if (0 == String)
		{
            Width = 0;
		}

        else if (String[Control->DataWidth - 1])
		{
			Width = Control->DataWidth;    // depends on not having embedded nulls
		}
		else
		{
			Width = (UINT16)wcslen16(String) * sizeof(UINT16);
		}

		p.String->Length = Width;
		MemWrite(&Buffer, p.String, sizeof(*p.String));
		MemWrite(&Buffer, String, Width);
        free(String);
		p.CommonHeader = (PLDM_VALUE *)Buffer.Buffer;
		Buffer.Buffer = 0;

		if (MemClose(&Buffer))
		{
			throw (L"MakeValue: Memory exhaustion");
		}

		return p.CommonHeader;
	}
	else if (Control->ControlType == EFI_IFR_PASSWORD_OP)
	{
		// output as raw binary of variable content
		VALUE_POINTER v;
		char *PassWord = NULL;
		
		PassWord = (char *)SceGetQuestionValue( (UINT16)Control->VariableStorage, (UINT16)Control->DataOffset, (UINT16)Control->DataWidth );

		if (PassWord == NULL)
		{
			return NULL;
		}
				
		MemCreat(&Buffer);
		Header.Type = BIOSPassword;
		v.CommonHeader = (PLDM_VALUE *)malloc(sizeof * v.PassWord);

		if (0 == v.CommonHeader)
		{
			throw (L"MakeValue: Memory exhausted");
		}

		*v.CommonHeader = Header;

		v.PassWord->Length = Control->DataWidth;
		MemWrite(&Buffer, v.PassWord, sizeof(*v.PassWord));
		MemWrite(&Buffer, PassWord, v.PassWord->Length);
		free(PassWord);
		v.CommonHeader = (PLDM_VALUE *)Buffer.Buffer;
		Buffer.Buffer = 0;

		if (MemClose(&Buffer))
		{
			throw (L"MakeValue: Memory exhaustion");
		}

		return v.CommonHeader;
	}

	return NULL;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 GetPackControls
//
// Description:  Fill in CONTROL structure and add to list.
//               Called from TraverseControls().
//               Calls itself for each additional legacy boot sublist.
//
// Parameters:
//               int
//					[in] Index to PackInfo struct
//               CONTROL_INFO *
//                  [in] pointer to control info struct
//               void *
//                  [in out] pointer control counter
//
// Return:       EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
//
static EFI_STATUS
GetPackControls(int PackIndex, CONTROL_INFO *Control, void *pCount)
{
	struct InternalTables *g = AddressGlobals();
	struct PackInfo *Pack = g->Packs + PackIndex;
	int Count = *(int *)pCount;
	ATTRIBUTE_POINTER Attribute;
	int NumLegacy = g->NumLegacy;

	if (FALSE == IsNeededControl(Control))
	{
		FreeControl(Control);
		return EFI_SUCCESS;
	}

	//Pack.Controls[Count] = Control;
	// construct attribute entry
	Attribute.CommonHeader = Pack->Controls[Count].Attribute = MakeAttribute(Control, PackIndex);

	// construct value entry
	if (Pack->Controls[Count].Attribute)
	{
		// assign handle
		Pack->Controls[Count].Attribute->Handle = Count + Pack->FirstControlHandle;

		Pack->Controls[Count].Value = MakeValue(Control, Attribute, PackIndex);

		if (Pack->Controls[Count].Value == NULL)
		{
			FreeControl(Control);
			return EFI_SUCCESS;
		}

		Pack->Controls[Count].Value->Handle = Count + Pack->FirstControlHandle;
		*(int *)pCount += 1;

		// if there are more legacy controls to make, enlarge the list and then call recursively
		if (0 == NumLegacy && g->NumLegacy)
		{
			// just encountered legacy boot control
			// make any needed addtional controls
			// and put them out at end control array to avoid changing handles of following controls
			int i;

			for (i = 1; i < g->NumLegacy; i++)
			{
				int LegCount = Pack->NumControls;
				Pack->NumControls++;
				Pack->Controls = (struct Control *)realloc(Pack->Controls, sizeof(Pack->Controls[0]) * Pack->NumControls);

				if (NULL == Pack->Controls)
				{
					throw (L"GetPackControls: Memory exhausted");
				}

				GetPackControls(PackIndex, Control, &LegCount);
				Pack->ExtraControls++;
			}
		}
	}
	else
	{//if attribute construct failed, just increment the control count
	    DEBUG((DEBUG_ERROR, "\n GetPack Attribute is zero"));
		*(int *)pCount += 1;
	}

	// free the control info
	if (NumLegacy == 0 || Pack->ExtraControls == NumLegacy - 1)
	{
		FreeControl(Control);
	}

	return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 FreeGlobals
//
// Description:  Free global memory
//
// Parameters:
//
// Return:
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
void FreeGlobals()
{
	struct InternalTables *g = AddressGlobals();
	int PackIndex;

	for (PackIndex = 0; PackIndex < g->nPacks; PackIndex++)
	{
		int i;
		struct PackInfo *Pack = &g->Packs[PackIndex];

		for (i = 0; i < Pack->NumControls; i++)
		{
			free(Pack->Controls[i].Attribute);
			free(Pack->Controls[i].Value);
		}

		free(Pack->Controls);
		Pack->Controls = NULL;
		/*for (i = 0; i < Pack->NumStrings; i++)
		    if (Pack->Strings[i].Allocated)
		        free(Pack->Strings[i].String);*/
		free(Pack->Strings);
		Pack->Strings = NULL;
		// Note: map strings are never allocated
		free(Pack->MapStrings);
		Pack->MapStrings = NULL;
	}

	memset(g, 0, sizeof(*g));
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 BuildInternalTables
//
// Description:  Build struct InternalTables
//
// Parameters:
//
// Return:      EFI_SUCCESS on success, EFI_ABORT on failure
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
EFI_STATUS
BuildInternalTables()
{

	struct InternalTables *g = AddressGlobals();
	int i;
	UINTN j;
	//int Offset;
	UINT8 *CurrentPackageList = NULL;
	UINT8 *StringPack;
	int LastStringHandle = 0;
	int LastControlHandle = 0;
	int Count = 0;
	int PackageCount = 0;
	int GoodPackages = 0;
	CHAR8 **MappingLangArr = (CHAR8 **)NULL; 
	UINTN MappingLangCount = 0;
	
	PackageCount = GetTotalPackageList( g->HIIDBData );
	GoodPackages = 0;
	
	MappingLangCount = GetPLDMMappingCount();
	MappingLangArr = GetPLDMMappingLanguageID();
	
	_GetStdMfgDefaultsBuffer();

	g->nPacks = 0;
	GetHiiHandles();
	// for each package with IFR
	DEBUG((DEBUG_ERROR, "\n Total PackageCount: %d", PackageCount));

	for (i = 0; i < PackageCount; i++)
	{
		STRING_CONTEXT StringContext;
		EFI_STATUS Status;
		//MEMFILE StringBuffer;
		struct String String;

		struct PackInfo *Pack;

		CurrentPackageList = GetNextPackageList(g->HIIDBData, &CurrentPackageList);

		if (0 == FindIfrPack(CurrentPackageList))
		{
			continue;    // no controls
		}

		if (0 == (StringPack = FindStringPack(CurrentPackageList)))
		{
			continue;    // no english strings
		}

		gCurrentHiiHandle = gHiiHandles[i];
// check maximum here
		Pack = &g->Packs[g->nPacks];
		Pack->Package = CurrentPackageList;
		Pack->NumControls = CountControls(g->nPacks);

		if (Pack->NumControls == 0)
		{
			continue;    // no good controls
		}

		GoodPackages++;
		Pack->HiiIndex = i; // for later access to Hii handle
		Pack->FirstStringHandle = LastStringHandle + STRING_GAP; // leave a gap for added strings
		Pack->FirstControlHandle = LastControlHandle + CONTROL_GAP;  // leave gap for added controls

		// count strings in this pack
		InitFirstUefiString(&StringContext, StringPack);

		//Pack->NumControls = CountControls(g->nPacks);
		for (; EFI_SUCCESS == GetNextUefiString(&StringContext); )
			;

		Pack->NumStrings = StringContext.StringIndex;
		LastStringHandle = Pack->FirstStringHandle + Pack->NumStrings;
		// allocate memory for strings
		Pack->Strings = (struct String *)calloc(Pack->NumStrings, sizeof(Pack->Strings[0]));

		if (NULL == Pack->Strings)
		{
			throw (L"BuildInternalTables: Memory exhausted");
		}

		// build list of strings for this pack
		InitFirstUefiString(&StringContext, StringPack);

		for (; ; )
		{
			Status = GetNextUefiString(&StringContext);

            if (EFI_NOT_FOUND == Status)
			{
				DEBUG((DEBUG_ERROR, "\n String Pack is broken !!"));
				break;
			}

			if (Status)
			{
				DEBUG((DEBUG_ERROR, "\n Failed to Get String"));
				return EFI_ABORTED;
			}

			String.EfiHandle = StringContext.StringIndex;
			String.String = (UINT16 *)(StringPack + StringContext.StringOffset);
			String.Referenced = 0;
			String.Allocated = 0;
			Pack->Strings[StringContext.StringIndex - 1] = String;
		}

		// build list of mapping strings for this pack
        for ( j = 0; j < MappingLangCount; j++) 
        {
			if(MappingLangArr[j])
			{
	            if ((StringPack = FindLangStringPack(CurrentPackageList, MappingLangArr[j])))
	            {
	                //Allocate Memory once for a package
	                if( NULL == Pack->MapStrings)
	                {
	                    // allocate strings
	                    Pack->MapStrings = (struct String *)calloc(Pack->NumStrings, sizeof(Pack->MapStrings[0]));
	                }
	                InitFirstUefiString(&StringContext, StringPack);
        
	                for (; EFI_NOT_FOUND != (Status = GetNextUefiString(&StringContext)); )
	                {
	                    if (Status)
	                    {
		                    DEBUG((DEBUG_ERROR, "Failed to Get Mapping String\n")); 
	                        return EFI_ABORTED;
	                    }
        
	                    String.EfiHandle = StringContext.StringIndex;
	                    String.String = (UINT16 *)(StringPack + StringContext.StringOffset);
	                    String.Referenced = 0;
	                    String.Allocated = 0;
	                    // better check index
	                    Pack->MapStrings[StringContext.StringIndex - 1] = String;
					}
				}
	    	}
		}

		// build list of controls of interest for this pack
		LastControlHandle = Pack->FirstControlHandle + Pack->NumControls;

		// create control table
		Pack->Controls = (struct Control *)calloc(Pack->NumControls, sizeof(Pack->Controls[0]));

		if (NULL == Pack->Controls)
		{
			throw (L"BuildInternalTables: Memory exhausted");
		}

		Count = 0;
		TraverseControls(g->nPacks, GetPackControls, (void *)&Count);

		_FreeVariableList();
		g->nPacks++;
	}

	if (gpStdBuf)
	{
		free(gpStdBuf);
	}

	if (gpMfgBuf)
	{
		free(gpMfgBuf);
	}

	FreeHiiHandles();
	return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 AddCrc
//
// Description:  Add pad bytes and crc32 to end
//
// Parameters:
//               MEMFILE *
//                  [in] pointer to MEMFILE to write CRC
//
// Return:
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
// Add pad bytes and crc32 to end
void
AddCrc(MEMFILE *MBuff)
{
	UINT32 Crc;
	UINT8 zero = 0;

	while (MBuff->FileSize % 4)
	{
		// pad required
		MemWrite(MBuff, &zero, 1);
	}

	CalculateCRC_32(MBuff->Buffer, MBuff->FileSize, &Crc);
	MemWrite(MBuff, &Crc, sizeof(Crc));
}

extern UINT32 gHiiDataSize;

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    CreatePldmTables
//
// Description:  This function constructs string, attribute, and value tables
//               Values come from NVRAM.
//
// Parameter:    UINT8 *HiiData
//					[in]  Pointer to HII export data base
//				 UINT32 HiiSize
//					[in]  Size of export data base
//				 UINT8  **StringTable
//					[out] return pointer to string table
//				 UINT32 *StringSize
//				    [out] return string table size
//               UINT8 **AttributeTable
//                  [out] return pointer to attribute table
//               UINT32 *AttributeSize
//                  [out] return attribute table size
//               UINT8 **ValueTable
//                  [out] return pointer to value table
//               UINT8  *ValueSize
//                  [out] return value table size
//				 UINT8  **MapStringTable
//					[out] return pointer to map string table
//				 UINT32 *MapStringSize
//				    [out] return map string table size
//				 BOOLEAN SaveVars
//				    [in] Save unreferenced variables
//
// Return value: EFI_STATUS
//
// Modified:
//
// Referral(s):
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
CreatePldmTables(
	UINT8  *HiiData,       // pointer to HII export data
	UINT32  HiiSize,       // size of Hii Data
	UINT8 **StringTable,   // return string table pointer
	UINT32 *StringSize,    // return string table size
	UINT8 **AttributeTable,// return attribute table pointer
	UINT32 *AttributeSize, // return attribute table size
	UINT8 **ValueTable,    // return value table pointer
	UINT32 *ValueSize,     // return value size
	UINT8 **MapStringTable,// return map string table pointer
	UINT32 *MapStringSize, // return map string table size
	BOOLEAN SaveVars       // save unreferenced variables
)
{
	struct InternalTables *g = AddressGlobals();
	int i;
	int j;
	MEMFILE BufferA;
	MEMFILE BufferV;
	MEMFILE BufferS;
	MEMFILE BufferM;
	struct PackInfo *Pack;
	EFI_STATUS Status;
	g->HIIDBData = HiiData;
	gHiiDataSize = HiiSize;

	// maybe check parameters
	*StringSize = 0;
	*AttributeSize = 0;
	*ValueSize = 0;
	*MapStringSize = 0;

	//Check the gMD structure and Initialize it if gMD is NULL
	if(NULL == gMD)
	{
		InitializePldmLibrary();
		DEBUG((DEBUG_ERROR, "\nCreatePldmTables InitializePldmLibrary Done"));
	}

	AmisceData.HIIDBData = HiiData;
	gHiiDataSize = (UINT64)HiiSize;
	FindUefiVersion( AmisceData.HIIDBData );

	Status = BuildInternalTables();

	if (EFI_SUCCESS != Status)
	{
		DEBUG((DEBUG_ERROR, "Failed to build internal tables\n"));
		return Status;
	}

	if (gFilterCount)
	{
		GetHiiHandles();
	}

	// output string Table
	MemCreat(&BufferS);

	for (i = 0; i < g->nPacks; i++)
	{
		Pack = &g->Packs[i];

		if (gFilterCount && !InFilter(gHiiHandles[Pack->HiiIndex]))
		{
			continue;
		}

		for (j = 0; j < Pack->NumStrings; j++)
		{
			if (Pack->Strings[j].Referenced)
			{
				PLDM_STRING S;
				S.Handle = j + Pack->FirstStringHandle;

                if (Pack->Strings[j].String)
                    S.Length = (UINT16)(sizeof(UINT16) * wcslen16((UINT16 *)Pack->Strings[j].String));
                else
                    S.Length = 0;
				MemWrite(&BufferS, &S, sizeof(S));
				MemWrite(&BufferS, Pack->Strings[j].String, S.Length);
#ifdef PLDM_DBG
				{
					//DEBUG((DEBUG_ERROR, "String %x len %x ", S.Handle, S.Length));
					Print16StringN(Pack->Strings[j].String, S.Length / 2);
					//DEBUG((DEBUG_ERROR, "\n"));
				}
#endif
			}
		}
	}

	// output map strings, if present
	MemCreat(&BufferM);

	for (i = 0; i < g->nPacks; i++)
	{
		Pack = &g->Packs[i];

		if (NULL == Pack->MapStrings)
		{
			continue;    // no map strings here
		}

		if (gFilterCount && !InFilter(gHiiHandles[Pack->HiiIndex]))
		{
			continue;
		}

		for (j = 0; j < Pack->NumStrings; j++)
		{
			if (Pack->Strings[j].Referenced)
			{
				if (Pack->MapStrings[j].String)
				{
					PLDM_STRING S;
					S.Handle = j + Pack->FirstStringHandle;
					S.Length = sizeof(UINT16) * (UINT16)wcslen16((UINT16 *)Pack->MapStrings[j].String);
					MemWrite(&BufferM, &S, sizeof(S));
					MemWrite(&BufferM, Pack->MapStrings[j].String, sizeof(UINT16) * (UINT16)wcslen16((UINT16 *)Pack->MapStrings[j].String));
#ifdef PLDM_DBG
					{
						//DEBUG((DEBUG_ERROR, "MapString %x len %x ", S.Handle, S.Length));
						Print16StringN(Pack->MapStrings[j].String, S.Length / 2);
						//DEBUG((DEBUG_ERROR, "\n"));
					}
#endif
				}
			}
		}
	}

	// add crc32 and pad
	if (BufferM.FileSize)
	{
		AddCrc(&BufferM);
		*MapStringTable = BufferM.Buffer;
		*MapStringSize = BufferM.FileSize;
		BufferM.Buffer = 0;
	}
	else
	{
		*MapStringTable = 0;
	}

	if (MemClose(&BufferM))
	{
		throw (L"CreatePldmTables: Memory exhaustion");
	}

	// output attribute table
	MemCreat(&BufferA);

	for (i = 0; i < g->nPacks; i++)
	{
		Pack = &g->Packs[i];

		if (gFilterCount && !InFilter(gHiiHandles[Pack->HiiIndex]))
		{
			continue;
		}

		for (j = 0; j < Pack->NumControls; j++)
		{
			if (NULL == Pack->Controls[j].Attribute)
			{
				continue;    // control was skipped due to no variable
			}

			MemWrite(&BufferA, Pack->Controls[j].Attribute, GetAttributeLength(Pack->Controls[j].Attribute));
		}
	}

	// output the value table
	MemCreat(&BufferV);

	for (i = 0; i < g->nPacks; i++)
	{
		Pack = &g->Packs[i];

		if (gFilterCount && !InFilter(gHiiHandles[Pack->HiiIndex]))
		{
			continue;
		}

		for (j = 0; j < Pack->NumControls; j++)
		{
			if (NULL == Pack->Controls[j].Attribute)
			{
				continue;    // control was skipped due to no variable
			}

			MemWrite(&BufferV, Pack->Controls[j].Value, GetValueLength(Pack->Controls[j].Value));
#ifdef PLDM_DBG
			PrintControlStruct(&Pack->Controls[j]);
#endif
		}
	}

	DEBUG((DEBUG_ERROR, "\n VarHandle: %x ", g->Packs[g->nPacks - 1].NumControls + g->Packs[g->nPacks - 1].FirstControlHandle));
	// now add selected variables as strings
	if (SaveVars)
		CreateVarTables(
			HiiData,       // pointer to HII export data
			HiiSize,       // size of Hii Data
			&BufferA,
			g->nPacks
			?  g->Packs[g->nPacks - 1].NumControls + g->Packs[g->nPacks - 1].FirstControlHandle
			: 0,
			&BufferS,
			g->nPacks
			? g->Packs[g->nPacks - 1].NumStrings + g->Packs[g->nPacks - 1].FirstStringHandle
			: 0,
			&BufferV
		);
	
	DEBUG((DEBUG_ERROR, "\n gVariablesAdded: %x ", gVariablesAdded));

	if ( PcdGetBool(PcdBootVariablesPreserveSupport) )
	{
		DEBUG((DEBUG_ERROR, "\n BootVarHand: %x ", g->Packs[g->nPacks - 1].NumControls + g->Packs[g->nPacks - 1].FirstControlHandle + gVariablesAdded));
		//Support to preserve BootOrder, BootXXXX and LegacyDevOrder variables
		CreateBootVarTables(
				HiiData,       // pointer to HII export data
				HiiSize,       // size of Hii Data
				&BufferA,
				g->nPacks
				?  g->Packs[g->nPacks - 1].NumControls + g->Packs[g->nPacks - 1].FirstControlHandle + gVariablesAdded
				: 0,
				&BufferS,
				g->nPacks
				? g->Packs[g->nPacks - 1].NumStrings + g->Packs[g->nPacks - 1].FirstStringHandle + gVariablesAdded
				: 0,
				&BufferV
			);
	}

	// add crc32 and pad
	AddCrc(&BufferS);
	// must be last thing before close due to possible buffer realloc
	*StringTable = BufferS.Buffer;
	*StringSize = BufferS.FileSize;

	AddCrc(&BufferV);
	*ValueTable = BufferV.Buffer;
	*ValueSize = BufferV.FileSize;

	AddCrc(&BufferA);
	*AttributeTable = BufferA.Buffer;
	*AttributeSize = BufferA.FileSize;

	BufferV.Buffer = 0;

	if (MemClose(&BufferV))
	{
		throw (L"CreatePldmTables: Memory exhaustion");
	}

	BufferS.Buffer = 0;

	if (MemClose(&BufferS))
	{
		throw (L"CreatePldmTables: Memory exhaustion");
	}

	BufferA.Buffer = 0;

	if (MemClose(&BufferA))
	{
		throw (L"CreatePldmTables: Memory exhaustion");
	}

	FreeGlobals();
	FreeHiiHandles();

	//Caller responsibilty to clean up the gMD structure
	//CleanUpPldmLibrary(0);

	return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 ImportLegacyValue
//
// Description:  Import a Legacy Boot Order Value
//               Calls SCE function ImportLegacyDevOrderControlToNVRAM()
//               which has known limitations
//
// Parameters:
//               VALUE_POINTER
//                  [in] pointer union to value
//               CONTROL_INFO *
//                  [in] pointer to control info struct
//               UINT32
//					[in] Index to PackInfo struct
//				 UINT32
//				    [in] Index to control within PackInfo Controls[]
//
// Return:       EFI_STATUS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
EFI_STATUS
ImportLegacyValue(VALUE_POINTER v, CONTROL_INFO *Control, UINT32 PackIndex, UINT32 ControlIndex)
{
	struct InternalTables *g = AddressGlobals();
	struct PackInfo *Pack = g->Packs + PackIndex;
	int i;
	//struct LegacyBoot *LegacyBoot;
	// fake for SCE import function
	SETUP_QUESTION_LIST Question;
	UINT16 *Prompt16;
	UINT8  *Prompt = 0; // question prompt from attribute
	int     PromptLen; // number of chars in prompt
	UINT16  Boot[100];

	if (v.CommonHeader->Type != BIOSBootConfigSetting)
	{
		return EFI_ABORTED;    // handle mismatch!
	}

	// make a prompt string
	Prompt16 = GetPldmString(Pack->Controls[ControlIndex].Attribute->NameHandle)->String;
	PromptLen = (int)wcslen16(Prompt16);
	Prompt = (UINT8 *)malloc((wcslen16(Prompt16) + 1) * sizeof(char));

	for (i = 0; i < PromptLen; i++)
	{
		Prompt[i] = (UINT8)Prompt16[i];
	}

	Prompt[i] = 0;

	for (i = 0; i < v.Boot->NumSettings; i++)
	{
// range check here!
		// get device number from corresponding attribute string
		ATTRIBUTE_POINTER a;
		UINT16  Device = 100;
		struct String *String;
		int Index;
		wchar_t digits[5];
		a.CommonHeader = Pack->Controls[ControlIndex].Attribute;
		String = GetPldmString(a.Boot->PossibleDevices[i]);
		assert(String);
		Index = (int)wcslen16(String->String) - 4;
		assert(Index > 0);
		// last 4 characters of name contain the number
		digits[0] = String->String[Index + 0];
		digits[1] = String->String[Index + 1];
		digits[2] = String->String[Index + 2];
		digits[3] = String->String[Index + 3];
		digits[4] = 0;
		swscanf(digits, L"%hx", &Device);
		Boot[i] = Device;
	}

	memset(&Question, 0, sizeof(Question));
	Question.spScriptVal.cSetupPrompt = (char *)Prompt;
	Question.spScriptVal.bOrder = Boot;
	Question.spScriptVal.bValue = v.Boot->NumSettings;
	ImportLegacyDevOrderControlToNVRAM(&Question, Pack->Package, g->HIIDBData, Control);
	free(Prompt);
	return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 ImportBootValue
//
// Description:  Import a Boot Order Value
//               Calls SCE function ImportBootDevOrderControlToNVRAM()
//               which has known limitations
//
// Parameters:
//               VALUE_POINTER
//                  [in] pointer union to value
//               CONTROL_INFO *
//                  [in] pointer to control info struct
//               UINT32
//					[in] Index to PackInfo struct
//				 UINT32
//				    [in] Index to control within PackInfo Controls[]
//
// Return:       EFI_STATUS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
EFI_STATUS
ImportBootValue(VALUE_POINTER v, CONTROL_INFO *Control, UINT32 PackIndex, UINT32 ControlIndex)
{
	struct InternalTables *g = AddressGlobals();
	struct PackInfo *Pack = g->Packs + PackIndex;
	int i;
	// fake for SCE import function
	SETUP_QUESTION_LIST Question;
	UINT16 *Boot;

	// check value type!
	if (v.CommonHeader->Type != BIOSBootConfigSetting)
	{
		return EFI_ABORTED;    // handle mismatch!
	}

	memset(&Question, 0, sizeof(Question));
	Boot = (UINT16 *)malloc(sizeof(UINT16) * v.Boot->NumSettings);

	if (0 == Boot)
	{
		throw (L"ImportBootValue: Memory exhausted");
	}

	for (i = 0; i < v.Boot->NumSettings; i++)
	{
// range check here!
		Boot[i] = gBootOrder[v.Boot->Devices[i]];
	}

	Question.spScriptVal.cSetupPrompt = (char *)"Boot Option #";
	Question.spScriptVal.bOrder = Boot;
	Question.spScriptVal.bValue = v.Boot->NumSettings;
	// Note: ImportBootOrderContrlToNVRAM() updates legacy dev order
	// It complains if number of elements in boot order changes and
	// it does not check the values
	ImportBootOrderContrlToNVRAM(&Question, Pack->Package, g->HIIDBData, Control);
	return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 ImportLangValue
//
// Description:  Import a Language Value.
//
// Parameters:
//               VALUE_POINTER
//                  [in] pointer union to value
//               CONTROL_INFO *
//                  [in] pointer to control info struct
//
// Return:       EFI_STATUS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
EFI_STATUS
ImportLangValue(VALUE_POINTER v, CONTROL_INFO *Control)
{
	int Index;

	if ( gLanguages == NULL )
	{
		// Populate gLanguages
		gLangCount = UpdateLanguageCodes(Control->ConditionalVariable[0]);
	}

// check some things
	// Update Selected Language to Variable
	if ((Index = v.Enumeration->ValueIndex[0]) < (int)gLangCount )
	{
		// Update Language Variable with gLanguages
		CHAR8 *Lang = NULL;
		UINT32 Length = ( UINT32 )wcslen( gLanguages[Index].LangCode );

		Lang = ( CHAR8 * )calloc( sizeof( CHAR8 ), GetLangDataWidth() );

		if ( Lang == NULL )
		{
			throw (L"Memory Allocation Failure");
		}

		wcstombs( ( char * )Lang, gLanguages[Index].LangCode, Length);
		SceSetQuestionValue( Control->VariableStorage, Control->DataOffset, GetLangDataWidth(), ( UINT8 * )Lang );

		if ( Lang )
		{
			free( Lang );
		}
	}
	else
	{
		wprintf( L"Warning: Invalid Index value for Language Option Control\n" );
	}

	return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 ImportSpecialValue
//
// Description:  Import a special control Value
//
// Parameters:
//               VALUE_POINTER
//                  [in] pointer union to value
//               CONTROL_INFO *
//                  [in] pointer to control info struct
//               UINT32
//					[in] Index to PackInfo struct
//				 UINT32
//				    [in] Index to control within PackInfo Controls[]
//
// Return:       EFI_STATUS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
EFI_STATUS
ImportSpecialValue(VALUE_POINTER v, CONTROL_INFO *Control, UINT32 PackIndex, UINT32 ControlIndex)
{
	switch ( GetSpecialKey( Control->ConditionalVariable[0] ))
	{
		case LANGUAGE_CONTROL:
		case PLATFORM_LANGUAGE_CONTROL:
			return ImportLangValue(v, Control);

		case BOOT_ORDER_CONTROL:
			return ImportBootValue(v, Control, PackIndex, ControlIndex);

		case LEGACY_DEV_ORDER_CONTROL:
			return ImportLegacyValue(v, Control, PackIndex, ControlIndex);

		default:
			return EFI_ABORTED;
	}
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 ImportControlValue
//
// Description:  Import a control Value
//
// Parameters:
//               VALUE_POINTER
//                  [in] pointer union to value
//               CONTROL_INFO *
//                  [in] pointer to control info struct
//               UINT32
//					[in] Index to PackInfo struct
//				 UINT32
//				    [in] Index to control within PackInfo Controls[]
//
// Return:       EFI_STATUS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
EFI_STATUS
ImportControlValue(int PackIndex, CONTROL_INFO *Control, void *pControlIndex)
{
	struct InternalTables *g = AddressGlobals();
	struct PackInfo *Pack = g->Packs + PackIndex;
	UINT16 Handle;
	int ValueOffset;
	int ControlIndex = *(int *)pControlIndex;
	int IsSpecial , IsLegacy = 0;
	EFI_STATUS Status;

	if (IsNeededControl(Control) == FALSE)
	{
		FreeControl(Control);
		return 0;
	}

	IsSpecial = IsOEMSpecialControl(Control);
	IsLegacy = IsSpecial && LEGACY_DEV_ORDER_CONTROL == GetSpecialKey( Control->ConditionalVariable[0] );
#ifdef EFI_DBG
	{
		wchar_t *Prompt = 0;
		DEBUG((DEBUG_ERROR, "Import %"_S L"\n", Prompt = PldmGetHiiString(Pack->Package, g->HIIDBData, Control->ControlPrompt, FALSE)));
		free(Prompt);
	}
#endif
	// compute control handle
	// use index from control structure array in pack
	Handle = Pack->FirstControlHandle + ControlIndex;

	// find value with same handle
	for (ValueOffset = -1; EFI_SUCCESS == GetNextValue(g->ImportValues, g->ImportSize, &ValueOffset); )
	{
		// each value in imported table
		VALUE_POINTER Value;
		Value.CommonHeader = (PLDM_VALUE *)(g->ImportValues + ValueOffset);

		if (Value.CommonHeader->Handle > Pack->FirstControlHandle + Pack->NumControls)
		{
			continue;    // input not required to be in order
		}

		if (Value.CommonHeader->Handle == Handle)
		{
			// have a match on handle
			DEBUG((DEBUG_ERROR, "Matched handle: %x\n", Handle));

			// check control type against value type
			if (Value.CommonHeader->Type == Pack->Controls[ControlIndex].Attribute->Type)
			{
				// update control value
				if (IsOEMSpecialControl(Control))
				{
					Status = ImportSpecialValue(Value, Control, PackIndex, ControlIndex);

					if (Status == EFI_ABORTED)
					{
						return Status;
					}

					continue;
				}

				if (Control->ControlType == EFI_IFR_NUMERIC_OP || Control->ControlType == EFI_IFR_CHECKBOX_OP)
				{
					if (Value.CommonHeader->Type != BIOSInteger)
					{
						DEBUG((DEBUG_ERROR, "Value type and Control type mismatch!\n"));
						return EFI_ABORTED;
					}

					// Bounds check the value
					if (EFI_IFR_CHECKBOX_OP == Control->ControlType)
					{
						if (Value.Numeric->Value > 1)
						{
							DEBUG((DEBUG_ERROR, "Bad value for checkbox %x\n", Value.Numeric->Value));
							continue;
						}
					}
					else
					{
						if (Value.Numeric->Value > Control->ValueInfo->MaxValue
								|| Value.Numeric->Value < Control->ValueInfo->MinValue)
						{
							DEBUG((DEBUG_ERROR, "Bad value for numeric control %x\n", Value.Numeric->Value));
							continue;
						}
					}

					SceSetQuestionValue((UINT16)Control->VariableStorage, Control->DataOffset, Control->DataWidth, (UINT8 *)&Value.Numeric->Value);
					continue;
				}
				else if (EFI_IFR_STRING_OP == Control->ControlType )
				{
					if (Value.CommonHeader->Type != BIOSString)
					{
						DEBUG((DEBUG_ERROR, "Value type and Control type mismatch!\n"));
						return EFI_ABORTED;
					}

					// check string length
					if (Value.String->Length <= Control->DataWidth * sizeof(UINT16)
							&& Value.String->Length >= Control->ValueInfo->MinValue * sizeof(UINT16)
							&& Value.String->Length <= Control->ValueInfo->MaxValue * sizeof(UINT16))
					{
						int i;
						SceSetQuestionValue((UINT16)Control->VariableStorage, Control->DataOffset, Value.String->Length, (UINT8 *)&Value.String->String);

						// pad with nulls if string is smaller than maximum
						for (i = Value.String->Length; i < Control->DataWidth; i++)
						{
							UINT8 Null = 0;
							SceSetQuestionValue((UINT16)Control->VariableStorage, Control->DataOffset + i, 1, &Null);
						}
					}
					else
					{
						DEBUG((DEBUG_ERROR, "String Value size error\n"));
					}

					break;  // assume value table entries are unique
				}
				else if (EFI_IFR_ONE_OF_OP == Control->ControlType)
				{
// need to give error if more than one value
					// find default value corresonding to string
					OPTION_LIST *Option;
					int OpNum;
					BOOLEAN Found = FALSE;

					if (Value.CommonHeader->Type != BIOSEnumeration)
					{
						DEBUG((DEBUG_ERROR, "Value type and Control type mismatch!\n"));
						return EFI_ABORTED;
					}

					if (Value.Enumeration->NumberOfValues < 1)
					{
						break;
					}

					for (OpNum = 0, Option = Control->ValueInfo->Options; Option; Option = Option->Next, OpNum++)
					{
						if (OpNum == Value.Enumeration->ValueIndex[0])
						{
							SceSetQuestionValue((UINT16)Control->VariableStorage, Control->DataOffset, Control->DataWidth, (UINT8 *)&Option->Value);
							Found = TRUE;
							break;
// should we handle duplicates??
						}
					}

					if (Found == FALSE)
					{
						DEBUG((DEBUG_ERROR, "Option Not found!\n"));
						return EFI_ABORTED;
					}

					break;
				}
				else if (EFI_IFR_PASSWORD_OP == Control->ControlType)
				{
					if (Value.CommonHeader->Type != BIOSPassword)
					{
						DEBUG((DEBUG_ERROR, "Value type and Control type mismatch!\n"));
						return EFI_ABORTED;
					}

					// check password length -- since we send binary encrypted there is no reason for length to change
					if (Value.PassWord->Length != Control->DataWidth)
					{
						DEBUG((DEBUG_ERROR, "Password length out of differs from control %d\n", Value.PassWord->Length));
						break;
					}

					// set the value
					SceSetQuestionValue((UINT16)Control->VariableStorage, Control->DataOffset, Control->DataWidth, (UINT8 *)&Value.PassWord->PassWord);
					break;
				}
			}
			else
			{
				// type mismatch!!
				DEBUG((DEBUG_ERROR, "\nType missmatch handle %x control type %x value type %x Offset %x\n", Handle, Control->ControlType, Value.CommonHeader->Type, ValueOffset));
				return EFI_ABORTED;   // this is bad
			}

			// mark as processed -- can't do this with changing GetNextValue()
			//Value.CommonHeader->Type |= 0x40;   // make type invalid
			break;  // assume unique
		}
		else if (IsLegacy && Value.CommonHeader->Handle - Pack->FirstControlHandle >= Pack->NumControls - Pack->ExtraControls)
		{
			Status = ImportSpecialValue(Value, Control, PackIndex, Value.CommonHeader->Handle - Pack->FirstControlHandle);

			if (Status == EFI_ABORTED)
			{
				return Status;
			}

			DEBUG((DEBUG_ERROR, "Matched handle: %x\n", Value.CommonHeader->Handle));
			break;  // assume unique
		}
	}

	*(int *)pControlIndex = ++ControlIndex;
	FreeControl(Control);
	return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    ImportPldmValues
//
// Description:  This function imports a PLDM value table into NVRAM
//
//
// Parameter:    UINT8 *HiiData
//					[in]  Pointer to HII export data base
//				 UINT32 HiiSize
//					[in]  Size of export data base
//				 UINT8 *ValueTable
//				    [in]  Pointer to PLDM value table
//				 UINT32 TableSize
//				    [in]  Size of value table
// Return value: EFI_STATUS
//
// Modified:    NVRAM values for questions
//
// Referral(s):
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
ImportPldmValues(
	UINT8  *HiiData,       // pointer to HII export data
	UINT32  HiiSize,       // size of Hii Data
	UINT8  *ValueTable,    // value table pointer
	UINT32  TableSize
)
{
	struct InternalTables *g = AddressGlobals();
	UINT32 Crc;
	int     ControlCount = 0;
	int     i;
	EFI_STATUS Status = EFI_SUCCESS;

	if (HiiData == NULL || ValueTable == NULL)
	{
		return EFI_INVALID_PARAMETER;
	}

	if (TableSize < sizeof(Crc))
	{
		return EFI_INVALID_PARAMETER;
	}

	// check crc of value table
	CalculateCRC_32(ValueTable, TableSize - sizeof(Crc), &Crc);

	if (Crc != ((UINT32 *)(ValueTable + TableSize - sizeof(Crc)))[0])
	{
#ifdef EFI_DEBUG
		DEBUG((DEBUG_ERROR, "\n CRC ERROR !!! Crc : %x \n", Crc));
#endif
		return EFI_CRC_ERROR;
	}

	g->HIIDBData = HiiData;
	g->ImportValues = ValueTable;
	g->ImportSize = TableSize;
	gHiiDataSize = HiiSize;

	Status = BuildInternalTables();

	if (EFI_SUCCESS != Status)
	{
		DEBUG((DEBUG_ERROR, "Failed to build internal tables\n"));
		return Status;
	}

	// for each package with IFR
	for (i = 0; i < g->nPacks; i++)
	{
		ControlCount = 0;   // keep count relative to pack
		TraverseControls(i, ImportControlValue, (void *)&ControlCount);
		// Update Variables
		ImportVariableToNVRAM();
		_FreeVariableList();
	}

	FreeGlobals();
	return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 ImportMappedLangValue
//
// Description:  Import a Languae Value.
//
// Parameters:
//               VALUE_POINTER
//                  [in] pointer union to value
//               CONTROL_INFO *
//                  [in] pointer to control info struct
//
// Return:       EFI_STATUS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
EFI_STATUS
ImportMappedLangValue(VALUE_POINTER v, CONTROL_INFO *Control)
{
	int Offset;
	int i;
	// PLDM_ATTRIBUTE       *Attribute;
	ATTRIBUTE_POINTER     a;
	struct InternalTables *g = AddressGlobals();
	PLDM_STRING          *String;
	// get foreign string handle for the language from the attribute
	a.CommonHeader = 0;

	// find foreign attribute for this value
	a.CommonHeader =  AttributeByHandle(g->ImportAttributes, g->ImportAttributeSize, v.CommonHeader->Handle);

	if (0 == a.CommonHeader)
	{
		wprintf(L"No attribute found for language control!\n");
		return 0;   // no attribute!
	}

	if (v.Enumeration->ValueIndex[0] >= a.Enumeration->PossibleValues)
	{
		return EFI_NOT_FOUND;
	}

	// find the string for the language in the English import strings
	for (Offset = -1; EFI_SUCCESS == GetNextString(g->ImportEngStrings, g->ImportEngStringSize, &Offset); )
	{
		String = (PLDM_STRING *)(g->ImportEngStrings + Offset);

		if (String->Handle == a.Enumeration->Handle[v.Enumeration->ValueIndex[0]])
		{
			break;
		}
		else
		{
			String = 0;
		}
	}

	if (String == 0)
	{
		return EFI_NOT_FOUND;
	}

	// search for the the language in the language list
	if ( gLanguages == NULL )
	{
		// Populate gLanguages
		gLangCount = UpdateLanguageCodes(Control->ConditionalVariable[0]);
	}

	// find language in array
	for (i = 0; i < (int)gLangCount; i++)
	{
		if (String->Length != (UINT16)wcslen16(g->LangStrings[i]) * sizeof(UINT16))
		{
			continue;
		}

		if (0 == memcmp(g->LangStrings[i], String->String, String->Length))
		{
			char    Lang[10];         // language code
			// convert language code to char
			wcstombs(Lang, gLanguages[i].LangCode, sizeof(Lang));
			SceSetQuestionValue( Control->VariableStorage, Control->DataOffset, GetLangDataWidth(), (UINT8 *)Lang );
			return 0;
		}
	}

	return EFI_NOT_FOUND;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 ImportMappedBootValue
//
// Description:  Import a Boot Order Value
//               Calls SCE function ImportBootDevOrderControlToNVRAM()
//               which has known limitations
//
// Parameters:
//               VALUE_POINTER
//                  [in] pointer union to value
//               CONTROL_INFO *
//                  [in] pointer to control info struct
//               UINT32
//					[in] Index to PackInfo struct
//				 UINT32
//				    [in] Index to control within PackInfo Controls[]
//
// Return:       EFI_STATUS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
EFI_STATUS
ImportMappedBootValue(VALUE_POINTER v, CONTROL_INFO *Control, UINT32 PackIndex, UINT32 ControlIndex)
{
	// int Offset;
	int i;
	//PLDM_ATTRIBUTE       *Attribute;
	ATTRIBUTE_POINTER     a;
	struct InternalTables *g = AddressGlobals();
	struct PackInfo       *Pack = g->Packs + PackIndex;
	PLDM_STRING          *String;
	UINT16               Order[256];            // the list of device numbers
	int                  NumOrder = 0;
	int                  Index;
	BOOLEAN              Found;
	// fake for SCE import function
	SETUP_QUESTION_LIST Question;

	a.CommonHeader =  AttributeByHandle(g->ImportAttributes, g->ImportAttributeSize, v.CommonHeader->Handle);

	if (0 == a.CommonHeader)
	{
		DEBUG((DEBUG_ERROR, "No attribute found for boot order!\n"));
		return 0;   // no attribute!
	}
	
	if(gBootOptions)
	{
		FreeBootOptions();
		DEBUG((DEBUG_ERROR, "ImportMappedBootValue FreeBootOptions done"));
	}
		
	_PopulateGlobalBootOptions();

	for (i = 0; i < v.Boot->NumSettings; i++)
	{
		// for each boot device in value
		// find the string for the boot device
		String = StringByHandle(g->ImportEngStrings, g->ImportEngStringSize, a.Boot->PossibleDevices[v.Boot->Devices[i]]);

		if (0 == String)
		{
			DEBUG((DEBUG_ERROR, "Boot device name not found in imported strings!\n"));
			return 0;
		}

		// now look for matching string in boot info table
		Found = FALSE;

		for (Index = 0; Index < (int)gBootCount; Index++)
		{
			BOOT_OPTION *BootOption = gBootOptions[Index];

			if (BootOption)
			{
				if ((UINT16)wcslen16(BootOption->Name) * sizeof(UINT16) != String->Length)
				{
					continue;
				}

				if (0 == memcmp(String->String, BootOption->Name, String->Length))
				{
					// found the device in this platform
					Order[NumOrder++] = gBootOrder[Index];
					Found = TRUE;
					break;
				}
			}
			else
			{
				break;
			}
		}

		if (FALSE == Found)
		{
			DEBUG((DEBUG_ERROR, "Boot Device not found\n"));
			//wprintf(L"Boot Device not found ");
			//Print16String(String->String);
			//wprintf(L"\n");
		}
	}

	// submit list for update
	if (NumOrder)
	{
		DEBUG((DEBUG_ERROR, "\nImport BootOrder"));
		memset(&Question, 0, sizeof(Question));
		Question.spScriptVal.cSetupPrompt = (char *)"Boot Option #";
		Question.spScriptVal.bOrder = Order;
		Question.spScriptVal.bValue = NumOrder;
		ImportBootOrderContrlToNVRAM(&Question, Pack->Package, g->HIIDBData, Control);
	}

	return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 ImportMappedLegacyValue
//
// Description:  Import a maped legacy Boot Order Value
//               Calls SCE function ImportLegacyDevOrderControlToNVRAM()
//               which has known limitations
//
// Parameters:
//               VALUE_POINTER
//                  [in] pointer union to value
//               CONTROL_INFO *
//                  [in] pointer to control info struct
//               UINT32
//					[in] Index to PackInfo struct
//				 UINT32
//				    [in] Index to control within PackInfo Controls[]
//
// Return:       EFI_STATUS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
EFI_STATUS
ImportMappedLegacyValue(VALUE_POINTER v, CONTROL_INFO *Control, UINT32 PackIndex, UINT32 ControlIndex)
{
	int i;
	ATTRIBUTE_POINTER     a;
	struct InternalTables *g = AddressGlobals();
	struct PackInfo       *Pack = g->Packs + PackIndex;
	PLDM_STRING          *String;
	// fake for SCE import function
	SETUP_QUESTION_LIST Question;
	UINT16 *Prompt16;
	UINT8  *Prompt = 0; // question prompt from attribute
	int     PromptLen; // number of chars in prompt
	UINT16  Boot[100];

	if (v.CommonHeader->Type != BIOSBootConfigSetting)
	{
		return EFI_ABORTED;    // handle mismatch!
	}

	// make a prompt string
	Prompt16 = GetPldmString(Pack->Controls[ControlIndex].Attribute->NameHandle)->String;
	PromptLen = (int)wcslen16(Prompt16);
	Prompt = (UINT8 *)malloc((wcslen16(Prompt16) + 1) * sizeof(char));

	for (i = 0; i < PromptLen; i++)
	{
		Prompt[i] = (UINT8)Prompt16[i];
	}

	Prompt[i] = 0;
	// get foreign attribute
	a.CommonHeader =  AttributeByHandle(g->ImportAttributes, g->ImportAttributeSize, v.CommonHeader->Handle);

	if (0 == a.CommonHeader)
	{
		DEBUG((DEBUG_ERROR, "No attribute found for boot order!\n"));
		if(Prompt)
			free(Prompt);
		return 0;   // no attribute!
	}

	assert(v.Boot->NumSettings < 100);

	for (i = 0; i < v.Boot->NumSettings; i++)
	{
		// for each string handle in dev list
		UINT16 Device;
		int    Index;
		wchar_t digits[5];
		// find the string for the boot device
		String = StringByHandle(g->ImportEngStrings, g->ImportEngStringSize, a.Boot->PossibleDevices[v.Boot->Devices[i]]);

		if (0 == String)
		{
			DEBUG((DEBUG_ERROR, "Legacy device name not found in imported strings!\n"));
			if(Prompt)
				free(Prompt);
			return 0;
		}

		// get device number from corresponding attribute string
		// last 4 characters of name contain the number
		Index = String->Length / sizeof(UINT16) - 4;
		assert(Index > 0);
		digits[0] = String->String[Index + 0];
		digits[1] = String->String[Index + 1];
		digits[2] = String->String[Index + 2];
		digits[3] = String->String[Index + 3];
		digits[4] = 0;
		swscanf(digits, L"%hx", &Device);
		Boot[i] = Device;
	}

	memset(&Question, 0, sizeof(Question));
	Question.spScriptVal.cSetupPrompt = (char *)Prompt;
	Question.spScriptVal.bOrder = Boot;
	Question.spScriptVal.bValue = v.Boot->NumSettings;
	ImportLegacyDevOrderControlToNVRAM(&Question, Pack->Package, g->HIIDBData, Control);
	free(Prompt);
	return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 ImportMappedSpecialValue
//
// Description:  Import a special control Value
//
// Parameters:
//               VALUE_POINTER
//                  [in] pointer union to value
//               CONTROL_INFO *
//                  [in] pointer to control info struct
//               UINT32
//					[in] Index to PackInfo struct
//				 UINT32
//				    [in] Index to control within PackInfo Controls[]
//
// Return:       EFI_STATUS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
EFI_STATUS
ImportMappedSpecialValue(VALUE_POINTER v, CONTROL_INFO *Control, UINT32 PackIndex, UINT32 ControlIndex)
{
	switch ( GetSpecialKey( Control->ConditionalVariable[0] ))
	{
		case LANGUAGE_CONTROL:
		case PLATFORM_LANGUAGE_CONTROL:
			return ImportMappedLangValue(v, Control);

		case BOOT_ORDER_CONTROL:
			if ( PcdGetBool(PcdBootOrderPreserveSupport) )
			{
				return ImportMappedBootValue(v, Control, PackIndex, ControlIndex);
			}
			else
			{
				return 0;
			}

		case LEGACY_DEV_ORDER_CONTROL:
			if ( PcdGetBool(PcdLegacyDevOrderPreserveSupport) )
			{
				return ImportMappedLegacyValue(v, Control, PackIndex, ControlIndex);
			}
			else
			{
				return 0;
			}

		default:
			return EFI_ABORTED;
	}

	return 0;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 GetMapStringHandle
//
// Description:  Translate our (EFI) string handle to that of the foreign map
//               string table. Searches for match on string text.
//
// Parameters:
//               int
//					[in] Index to PackInfo struct
//               UINT16
//                  [in] EFI handle
//               CONTROL_INFO *
//                  [in] pointer to control info struct
//
// Return:       int - foreign map string handle, -1 if not found
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
int
GetMapStringHandle(int PackIndex, UINT16 OurHandle)
{
	struct InternalTables *g = AddressGlobals();
	struct PackInfo       *Pack = g->Packs + PackIndex;
	int Offset;
	PLDM_STRING *String;
	struct String *OurString;

	if (NULL == Pack->MapStrings)
	{
		DEBUG((DEBUG_ERROR, "\n No map strings in this formset"));
		return -1;    // no map strings in this formset
	}

	if(OurHandle > Pack->NumStrings)
	{
		DEBUG((DEBUG_ERROR, "\n String handle out of range"));
		return -1;    // handle out of range
	}
	
	if (NULL == Pack->MapStrings[OurHandle].String)
	{
		return -1;    // no map string for this handle
	}

	OurString = &Pack->MapStrings[OurHandle];

	if (wcslen16(OurString->String) == 0)
	{
		return -1;    // string is empty so matching not meaningful
	}

	for (Offset = -1; EFI_SUCCESS == GetNextString(g->ImportMapStrings, g->ImportMapStringSize, &Offset); )
	{
		// for each string in import strings
		String = (PLDM_STRING *)(g->ImportMapStrings + Offset);

		if (String->Length != wcslen16(OurString->String) * sizeof(UINT16))
		{
			continue;
		}

		if (0 == memcmp(String->String, OurString->String, String->Length))
		{
			return String->Handle;
		}
	}

	return -1;  // not found
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 GetMapAttributeHandle
//
// Description:  Search foreign attibute table for given name handle.
//
// Parameters:
//               int
//					[in] Index to PackInfo struct
//               UINT16
//                  [in] EFI handle
//               CONTROL_INFO *
//                  [in] pointer to control info struct
//
// Return:       int - attribute handle, -1 if not found
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
int
GetMapAttributeHandle(UINT16 Handle)
{
	struct InternalTables *g = AddressGlobals();
	int Offset;
	PLDM_ATTRIBUTE *Attribute;

	for (Offset = -1; EFI_SUCCESS == GetNextAttribute(g->ImportAttributes, g->ImportAttributeSize, &Offset); )
	{
		Attribute = (PLDM_ATTRIBUTE *)(g->ImportAttributes + Offset);

		if (Attribute->NameHandle == Handle)
		{
			return Attribute->Handle;
		}
	}

	return -1;  // not found
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 GetOptionEfiHandle
//
// Description:  Translate option value index to local string handle.
//
// Parameters:
//               int
//					[in] Index to PackInfo struct
//               UINT16
//                  [in] EFI handle
//               CONTROL_INFO *
//                  [in] pointer to control info struct
//
// Return:       int - option handle, -1 if not found
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
//
// returns negative number on failure to find
static
int
GetOptionEfiHandle(int PackIndex, UINT16 Handle, int Index, OPTION_LIST *OptionHead)
{
	struct InternalTables *g = AddressGlobals();
	struct PackInfo       *Pack = g->Packs + PackIndex;
	int Offset;
	int                 i;
	int                 Found = FALSE;
	PLDM_STRING        *String;
	ATTRIBUTE_POINTER   Attribute;
	UINT16              PldmHandle;

	// find the attribute table entry
	for (Offset = -1; EFI_SUCCESS == GetNextAttribute(g->ImportAttributes, g->ImportAttributeSize, &Offset); )
	{
		Attribute.CommonHeader = (PLDM_ATTRIBUTE *)(g->ImportAttributes + Offset);

		if (Attribute.CommonHeader->Handle == Handle)
		{
			Found = TRUE;
			break;
		}
	}

	if (FALSE == Found)
	{
		return -1;
	}

	assert(Attribute.CommonHeader->Type == BIOSEnumeration);

	// get the PLDM string handle
	if (Attribute.Enumeration->PossibleValues <= Index)
	{
		DEBUG((DEBUG_ERROR, "\n Value is corrupted for the attribute : %x", Attribute.CommonHeader->Handle));
		return -1;
	}

	//assert(Attribute.Enumeration->PossibleValues > Index);
	PldmHandle = Attribute.Enumeration->Handle[Index];
	// find the string in string table by its handle
	Found = FALSE;

	for (Offset = -1; EFI_SUCCESS == GetNextString(g->ImportEngStrings, g->ImportEngStringSize, &Offset); )
	{
		String = (PLDM_STRING *)(g->ImportEngStrings + Offset);

		if (String->Handle == PldmHandle)
		{
			Found = TRUE;
			break;
		}
	}

	if (FALSE == Found)
	{
		return -1;
	}

	// find the string in our strings -- duplicates are possible
	Found = FALSE;

	for (i = 0; i < Pack->NumStrings; i++)
	{
		struct String *OurString;
		// change to regular strings -- maybe search map strings first
		OurString = &Pack->Strings[i];

		if (NULL == OurString->String)
		{
			continue;
		}

		if (String->Length != wcslen16(OurString->String) * sizeof(UINT16))
		{
			continue;
		}

		if (0 == memcmp(String->String, OurString->String, String->Length))
		{
			// have a matching string
			// check to see if string in option list
			OPTION_LIST *Option;

			for (Option = OptionHead; Option; Option = Option->Next)
			{
				if (Option->Option == i + 1)
				{
					Found = TRUE;
					break;
				}
			}

			if (TRUE == Found)
			{
				break;
			}
		}
	}

	if (FALSE == Found)
	{
		return -1;
	}

	// return Efi Handle of found string
	return i + 1;
}

static
UINT16 BbsType[] = {'B', 'B', 'S', '_', 'T', 'Y', 'P', 'E', '_', 0};

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 ImportMappedControlValue
//
// Description:  Import a mapped control Value
//               Called from TraverseControls()
//               Searches imported values looking for a match on import name string and
//               control prompt
//
// Parameters:
//               UINT32
//					[in] Index to PackInfo struct
//               CONTROL_INFO *
//                  [in] pointer to control info struct
//				 void *
//				    [in out] pointer to control index
//
// Return:       EFI_STATUS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
EFI_STATUS
ImportMappedControlValue(int PackIndex, CONTROL_INFO *Control, void *pControlIndex)
{
	struct InternalTables *g = AddressGlobals();
	struct PackInfo       *Pack = g->Packs + PackIndex;
	EFI_STATUS            Status;
	int                   MapStringHandle;
	int                   MapAttributeHandle;
	int                   ValueOffset;  // current offset withing value table
	int                   ControlIndex = *(int *)pControlIndex;
	int                   IsSpecial = 0;
	int                   IsLegacy =  0;
	
	CONTROL_RESET_FLAGINFO *ResetControlInfo = (CONTROL_RESET_FLAGINFO *)NULL;
	CONTROL_RESET_FLAGINFO *NewControlInfo = (CONTROL_RESET_FLAGINFO *)NULL;

	if (IsNeededControl(Control) == FALSE)
	{
		FreeControl(Control);
		return 0;
	}

    if(ControlIndex > Pack->NumControls)
    {
	    DEBUG((DEBUG_ERROR, "\n Control index is out of range"));
	    return 0;
    }

    if(Pack->Controls[ControlIndex].Attribute == 0)
    {
    	 DEBUG((DEBUG_ERROR, "\n Attribute is zero"));
        *(int *)pControlIndex = ++ControlIndex;
        return 0;
    }

    if( PcdGetBool(PcdDoSystemResetForPldm) )
    {
		//gResetControlInfo Structures stores the data of controls having RESET_REQUIRED flag
		if (Control->Flags ==  EFI_IFR_FLAG_RESET_REQUIRED)
		{
			ResetControlInfo = (CONTROL_RESET_FLAGINFO *)gResetControlInfo;
	
			NewControlInfo = ( CONTROL_RESET_FLAGINFO * )calloc( sizeof( CONTROL_RESET_FLAGINFO ), sizeof( UINT8 ));
	
			if (NULL == NewControlInfo)
			{
				throw (L"Memory Allocation Failure for reset control Info");
				return 1;
			}
	
			if ( gResetControlInfo == NULL )
			{
				gResetControlInfo = NewControlInfo;
			}
			else
			{
				//loop until last control
				while (ResetControlInfo->Next)
				{
					ResetControlInfo = ResetControlInfo->Next;
				}
	
				ResetControlInfo->Next = NewControlInfo;
			}
	
			//store control info into resetcontrolinfo structure
			NewControlInfo->VariableIndex = (UINT16)Control->VariableStorage;
			NewControlInfo->DataOffset = (UINT16)Control->DataOffset;
			NewControlInfo->DataWidth = (UINT16)Control->DataWidth;
		}
    }

	IsSpecial = IsOEMSpecialControl(Control);
	IsLegacy = IsSpecial && LEGACY_DEV_ORDER_CONTROL == GetSpecialKey(Control->ConditionalVariable[0]);
#ifdef PLDM_DBG
	{
		/*wchar_t *Prompt = NULL;
		Print(L"\nImport %s", Prompt = PldmGetHiiString(Pack->Package, g->HIIDBData, Control->ControlPrompt, FALSE));
		free(Prompt);*/
	}
#endif
	// translate local attribute name handle to foreign Name handle
	MapStringHandle = GetMapStringHandle(PackIndex, Pack->Controls[ControlIndex].Attribute->NameHandle - Pack->FirstStringHandle);

	if (MapStringHandle < 0)
	{
		// this control not in foreign strings
		*(int *)pControlIndex = ++ControlIndex;
		FreeControl(Control);
		return 0;
	}

	// find handle of foreign attribute with MapStringHandle
	MapAttributeHandle = GetMapAttributeHandle(MapStringHandle);

	if (MapAttributeHandle < 0)
	{
		// this control not in foreign strings
		FreeControl(Control);
		*(int *)pControlIndex = ++ControlIndex;
		return 0;
	}

	// search imported value table for matching handle(s)
	for (ValueOffset = -1; EFI_SUCCESS == GetNextValue(g->ImportValues, g->ImportSize, &ValueOffset); )
	{
		VALUE_POINTER Value;
		Value.CommonHeader = (PLDM_VALUE *)(g->ImportValues + ValueOffset);

		if (Value.CommonHeader->Handle != MapAttributeHandle)
		{
			if (!IsLegacy)
			{
				continue;    // keep looking
			}

			if (IsLegacy)
			{
				// check name string of attribute for "BBS_TYPE_"
				PLDM_STRING *String;
				PLDM_ATTRIBUTE *Attribute;
				Attribute = AttributeByHandle(g->ImportAttributes, g->ImportAttributeSize, Value.CommonHeader->Handle);

				if (0 == Attribute)
				{
					continue;
				}

				String = StringByHandle(g->ImportMapStrings, g->ImportMapStringSize, Attribute->NameHandle);

				if (0 == String)
				{
					continue;
				}

				if (String->Length < wcslen16(BbsType))
				{
					continue;
				}

				if (0 != memcmp(String->String, BbsType, wcslen16(BbsType)))
				{
					continue;
				}

				// have a match based on name starting with BBS_TYPE_
			}
		}

		// have a match for Control
		//_DBG_(wprintf(L"Matched mapped handle: %x\n", MapAttributeHandle));
		if (IsSpecial)
		{
			// ImportSpecial
			Status = ImportMappedSpecialValue(Value, Control, PackIndex, ControlIndex);

			if (EFI_ABORTED == Status)
			{
				*(int *)pControlIndex = ++ControlIndex;
				FreeControl(Control);
		        gSetValueStatus = EFI_ABORTED;
				return Status;
			}

			continue;
		}
		else if (Control->ControlType == EFI_IFR_NUMERIC_OP || Control->ControlType == EFI_IFR_CHECKBOX_OP)
		{
			if (Value.CommonHeader->Type != BIOSInteger)
			{
				DEBUG((DEBUG_ERROR, " Int: Value type(%x) and Control type(%x) mismatch!\n", Value.CommonHeader->Type, Control->ControlType));
				*(int *)pControlIndex = ++ControlIndex;
				FreeControl(Control);
				 gSetValueStatus = EFI_ABORTED; 
				return EFI_ABORTED;
			}

			// Bounds check the value
			if (EFI_IFR_CHECKBOX_OP == Control->ControlType)
			{
				if (Value.Numeric->Value > 1)
				{
				    gSetValueStatus = EFI_ABORTED; 
					DEBUG((DEBUG_ERROR, "Bad value for checkbox %x\n", Value.Numeric->Value));
					break;
				}
			}
			else
			{
				if (Value.Numeric->Value > Control->ValueInfo->MaxValue
						|| Value.Numeric->Value < Control->ValueInfo->MinValue)
				{
				    gSetValueStatus = EFI_ABORTED; 
					DEBUG((DEBUG_ERROR, "Bad value for numeric control %x\n", Value.Numeric->Value));
					break;
				}
			}

			SceSetQuestionValue((UINT16)Control->VariableStorage, Control->DataOffset, Control->DataWidth, (UINT8 *)&Value.Numeric->Value);
			// assume that values are unique
			break;
		}
        else if (Control->ControlType == EFI_IFR_DATE_OP || Control->ControlType == EFI_IFR_TIME_OP)
        {
            if (Value.CommonHeader->Type != BIOSInteger)
            {
                DEBUG((DEBUG_ERROR, " Int: Value type(%x) and Control type(%x) mismatch!\n", Value.CommonHeader->Type, Control->ControlType));
                *(int *)pControlIndex = ++ControlIndex;
                FreeControl(Control);
                gSetValueStatus = EFI_ABORTED; 
                return EFI_ABORTED;
            }

            // Bounds check the value.
            if (Control->ControlType == EFI_IFR_DATE_OP)
            {
            	EFI_HII_DATE *Date = (EFI_HII_DATE *)(UINT8 *)&Value.Numeric->Value;

            	if (Date->Month > 12 || Date->Day > 31 || Date->Year > 9999)
            	{
            	    gSetValueStatus = EFI_ABORTED; 
            		DEBUG((DEBUG_ERROR, "Bad value for date control %x\n", Value.Numeric->Value));
					break;	
            	}
            }
            else
            {
            	EFI_HII_TIME *Time = (EFI_HII_TIME *)(UINT8 *)&Value.Numeric->Value;
            	
            	if (Time->Hour > 23 || Time->Minute > 59 || Time->Second > 59)
            	{
            	    gSetValueStatus = EFI_ABORTED; 
            		DEBUG((DEBUG_ERROR, "Bad value for Time control %x\n", Value.Numeric->Value));
					break;
            	}
            }

            SceSetQuestionValue((UINT16)Control->VariableStorage, Control->DataOffset, Control->DataWidth, (UINT8 *)&Value.Numeric->Value);
            // assume that values are unique
            break;
        }
		else if (Control->ControlType == EFI_IFR_STRING_OP)
		{
			if (Value.CommonHeader->Type != BIOSString)
			{
				DEBUG((DEBUG_ERROR, " String: Value type(%x) and Control type(%x) mismatch!\n", Value.CommonHeader->Type, Control->ControlType));
				*(int *)pControlIndex = ++ControlIndex;
				FreeControl(Control);
				gSetValueStatus = EFI_ABORTED; 
				return EFI_ABORTED;
			}

			// check string length
			if (Value.String->Length <= Control->DataWidth * sizeof(UINT16)
					&& Value.String->Length >= Control->ValueInfo->MinValue * sizeof(UINT16)
					&& Value.String->Length <= Control->ValueInfo->MaxValue * sizeof(UINT16))
			{
				int i;
				SceSetQuestionValue((UINT16)Control->VariableStorage, Control->DataOffset, Value.String->Length, (UINT8 *)&Value.String->String);

				// pad with nulls if string is smaller than maximum
				for (i = Value.String->Length; i < Control->DataWidth; i++)
				{
					UINT8 Null = 0;
					SceSetQuestionValue((UINT16)Control->VariableStorage, Control->DataOffset + i, 1, &Null);
				}
			}
			else
			{
			    gSetValueStatus = EFI_ABORTED; 
				DEBUG((DEBUG_ERROR, "String Value size error\n"));
			}

			break;
		}
		else if (Control->ControlType == EFI_IFR_ONE_OF_OP)
		{
// need to give error if more than one value
			// find default value corresonding to string
			OPTION_LIST *Option;
			int OpNum;
			BOOLEAN Found = FALSE;
			UINT16 EfiHandle;   // string handle of selected option

			if (Value.CommonHeader->Type != BIOSEnumeration)
			{
				DEBUG((DEBUG_ERROR, " Enum: Value type(%x) and Control type(%x) mismatch!\n", Value.CommonHeader->Type, Control->ControlType));
				*(int *)pControlIndex = ++ControlIndex;
				FreeControl(Control);
				gSetValueStatus = EFI_ABORTED; 
				return EFI_ABORTED;
			}

			if (Value.Enumeration->NumberOfValues == 0)
			{
			    gSetValueStatus = EFI_ABORTED; 
				break;    // didn't have a valid value when exported
			}

			// map selected string to our English string's efi handle
			EfiHandle = GetOptionEfiHandle(PackIndex, MapAttributeHandle, Value.Enumeration->ValueIndex[0], Control->ValueInfo->Options);
			// then look for match with Option->Option

			for (OpNum = 0, Option = Control->ValueInfo->Options; Option; Option = Option->Next, OpNum++)
			{
				if (EfiHandle == Option->Option)
				{
					SceSetQuestionValue((UINT16)Control->VariableStorage, Control->DataOffset, Control->DataWidth, (UINT8 *)&Option->Value);
					Found = TRUE;
					break;
				}
			}

			if (Found == FALSE)
			{
			    gSetValueStatus = EFI_ABORTED; 
				DEBUG((DEBUG_ERROR, " Enum: Option Not found!: %x\n", EfiHandle));
			}

			break;
		}
        else if (Control->ControlType == EFI_IFR_ORDERED_LIST_OP)
        {
            // find default value corresonding to string
            OPTION_LIST *Option;
            int OpNum;
            BOOLEAN Found = FALSE;
            UINT16 EfiHandle;   // string handle of selected option
            UINT8 NoOfOptions = 0;
            VOID *OptionsList = NULL;
            
            if (Value.CommonHeader->Type != BIOSEnumeration)
            {
                DEBUG((DEBUG_ERROR, " OrderList: Value type(%x) and Control type(%x) mismatch!\n", Value.CommonHeader->Type, Control->ControlType));
                *(int *)pControlIndex = ++ControlIndex;
                FreeControl(Control);
                gSetValueStatus = EFI_ABORTED; 
                return EFI_ABORTED;
            }

            if (Value.Enumeration->NumberOfValues == 0)
            {
                gSetValueStatus = EFI_ABORTED; 
                break;    // didn't have a valid value when exported
            }

            //Get the macimum number of options for the List
            NoOfOptions = _GetUefiMaxEntries(Control->ControlPtr);

            //Create a New List
            OptionsList = (VOID *)AllocateZeroPool( Control->DataWidth * NoOfOptions );
            
            for(OpNum = 0; OpNum < NoOfOptions; OpNum++)
            {
                Found = FALSE;
                // map selected string to our English string's efi handle
                EfiHandle = GetOptionEfiHandle(PackIndex, MapAttributeHandle, Value.Enumeration->ValueIndex[OpNum], Control->ValueInfo->Options);
                for (Option = Control->ValueInfo->Options; Option; Option = Option->Next)
                {
                    // then look for match with Option->Option
                    if(EfiHandle == Option->Option)
                    {
                        //Update the List with options in the order available in Value Table
                        memcpy((UINT8 *)OptionsList + (OpNum * Control->DataWidth), (UINT8 *)&Option->Value, Control->DataWidth);
                        Found = TRUE;
                        break;
                    }
                    else
                        Found = FALSE;
                }

                if (Found == FALSE)
                {
                    gSetValueStatus = EFI_ABORTED; 
                    //If any one option present in Value Table is not present in the current HII, then abort.
                    break;
                }
            }

            if (Found == TRUE)
            {
                //Update the New list for the control
                SceSetQuestionValue((UINT16)Control->VariableStorage, Control->DataOffset, NoOfOptions * Control->DataWidth, (UINT8 *)OptionsList);
            }
            else
            {
                DEBUG((DEBUG_ERROR, " OrderedList: Option Not found!: %x\n", EfiHandle));
            }
            FreePool(OptionsList);

            break;
        }
		else if (EFI_IFR_PASSWORD_OP == Control->ControlType)
		{
			if (Value.CommonHeader->Type != BIOSPassword)
			{
				DEBUG((DEBUG_ERROR, " Pass: Value type(%x) and Control type(%x) mismatch!\n", Value.CommonHeader->Type, Control->ControlType));
				*(int *)pControlIndex = ++ControlIndex;
				FreeControl(Control);
				gSetValueStatus = EFI_ABORTED; 
				return EFI_ABORTED;
			}

			// check password length -- since we send binary encrypted there is no reason for length to change
			if (Value.PassWord->Length != Control->DataWidth)
			{
				DEBUG((DEBUG_ERROR, "Password length differs from control %d\n", Value.PassWord->Length));
				break;
			}

			// set the value
			SceSetQuestionValue((UINT16)Control->VariableStorage, Control->DataOffset, Control->DataWidth, (UINT8 *)&Value.PassWord->PassWord);
			break;
		}
	}

	*(int *)pControlIndex = ++ControlIndex;
	FreeControl(Control);
	return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    ImportMappedPldmValues
//
// Description:  This function imports a mapped PLDM value table into NVRAM
//
//
// Parameter:    UINT8 *HiiData
//					[in]  Pointer to HII export data base
//				 UINT32 HiiSize
//					[in]  Size of export data base
//				 UINT8 *ValueTable
//				    [in]  Pointer to PLDM value table
//				 UINT32 ValueSize
//				    [in]  Size of value table
//				 UINT8  *AttributeTable
//				    [in] attribute table from foreign system
//				 UINT32 AttributeSize
//				    [in] Size of foreign attribute table
//				 UINT8 *MapStringTable
//				    [in] x-UEFI_AMI string table from foreign system
//				 UINT32 MapStringSize
//				    [in] Size of foreign map string table
//				 UINT8 *EngStringTable
//				    [in] English string table from foreign system
//				 UINT32 EngStringSize
//				    [in] Size of English string table
//
// Return value: EFI_STATUS
//
// Modified:    NVRAM values for questions
//
// Referral(s):
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
ImportMappedPldmValues(
	UINT8  *HiiData,        // pointer to HII export data
	UINT32  HiiSize,        // size of Hii Data
	UINT8  *ValueTable,     // value table pointer
	UINT32  ValueSize,
	UINT8  *AttributeTable, // attribute table from foreign system
	UINT32  AttributeSize,
	UINT8  *MapStringTable, // x-UEFI-AMI string table from foreign system
	UINT32  MapStringSize,
	UINT8  *EngStringTable, // english string table from foreign system
	UINT32  EngStringSize
)
{
	struct InternalTables *g = AddressGlobals();
	UINT32 Crc;
	int     ControlCount = 0;
	int     i;
	EFI_STATUS Status = EFI_SUCCESS;

	if (HiiData == NULL || ValueTable == NULL || AttributeTable == NULL || MapStringTable == NULL || EngStringTable == NULL)
	{
		return EFI_INVALID_PARAMETER;
	}

	if (ValueSize < sizeof(Crc))
	{
		return EFI_INVALID_PARAMETER;
	}

	if (AttributeSize < sizeof(Crc))
	{
		return EFI_INVALID_PARAMETER;
	}

	if (MapStringSize < sizeof(Crc))
	{
		return EFI_INVALID_PARAMETER;
	}

	if (EngStringSize < sizeof(Crc))
	{
		return EFI_INVALID_PARAMETER;
	}

	//Check the gMD structure and Initialize it if gMD is NULL
	if(NULL == gMD)
	{
		InitializePldmLibrary();
		DEBUG((DEBUG_ERROR, "\nImportMappedPldmValues InitializePldmLibrary Done"));
	}

	AmisceData.HIIDBData = HiiData;
	gHiiDataSize = (UINT64)HiiSize;
	FindUefiVersion( AmisceData.HIIDBData );

	// check crc of value table
	CalculateCRC_32(ValueTable, ValueSize - sizeof(Crc), &Crc);

	if (Crc != ((UINT32 *)(ValueTable + ValueSize - sizeof(Crc)))[0])
	{
		DEBUG((DEBUG_ERROR, "\n ValueTable CRC ERROR !!! Crc : %x \n", Crc));
		return EFI_CRC_ERROR;
	}

	g->ImportValues = ValueTable;
	g->ImportSize = ValueSize;
	// check crc of attribute table
	CalculateCRC_32(AttributeTable, AttributeSize - sizeof(Crc), &Crc);

	if (Crc != ((UINT32 *)(AttributeTable + AttributeSize - sizeof(Crc)))[0])
	{
		DEBUG((DEBUG_ERROR, "\n AttributeTable CRC ERROR !!! Crc : %x \n", Crc));
		return EFI_CRC_ERROR;
	}

	g->ImportAttributes = AttributeTable;
	g->ImportAttributeSize = AttributeSize;
	// check crc of map string table
	CalculateCRC_32(MapStringTable, MapStringSize - sizeof(Crc), &Crc);

	if (Crc != ((UINT32 *)(MapStringTable + MapStringSize - sizeof(Crc)))[0])
	{
		DEBUG((DEBUG_ERROR, "\n MapStringTable CRC ERROR !!! Crc : %x \n", Crc));
		return EFI_CRC_ERROR;
	}

	g->ImportMapStrings = MapStringTable;
	g->ImportMapStringSize = MapStringSize;
	// check crc of English string table
	CalculateCRC_32(EngStringTable, EngStringSize - sizeof(Crc), &Crc);

	if (Crc != ((UINT32 *)(EngStringTable + EngStringSize - sizeof(Crc)))[0])
	{
		DEBUG((DEBUG_ERROR, "\n StringTable CRC ERROR !!! Crc : %x \n", Crc));
		return EFI_CRC_ERROR;
	}

	g->ImportEngStrings = EngStringTable;
	g->ImportEngStringSize = EngStringSize;

	g->HIIDBData = HiiData;
	gHiiDataSize = HiiSize;
	SetPreserveMode(1);
	if(gInternalTables == FALSE)
	{
        Status = BuildInternalTables();
        DEBUG((DEBUG_ERROR, "BuildInternalTables Status:%x\n",Status));
        
        if (EFI_SUCCESS != Status)
        {
            DEBUG((DEBUG_ERROR, "Failed to build internal tables\n"));
            return Status;
        }
        if (gSetUsingMapID == TRUE)
            gInternalTables = TRUE;
	}

	GetHiiHandles();

	// for each package with IFR
	for (i = 0; i < g->nPacks; i++)
	{
		struct PackInfo *Pack = g->Packs + i;

		if (NULL == Pack->MapStrings)
		{
			continue;
		}

		gCurrentHiiHandle = gHiiHandles[Pack->HiiIndex];
		ControlCount = 0;   // keep count relative to pack
		TraverseControls(i, ImportMappedControlValue, (void *)&ControlCount);

		// Update Variables
		if (!gUseCache)
		{
			ImportVariableToNVRAM();
		}

		_FreeVariableList();
		if( PcdGetBool(PcdDoSystemResetForPldm ))
		{
			FreeResetControlInfo();
		}
	}

	FreeHiiHandles();
	
	if(gSetUsingMapID == FALSE)
	    FreeGlobals();
	//Caller responsibilty to clean up the gMD structure
	//CleanUpPldmLibrary(0);

	return Status;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure     : FreePldmGlobals
//
// Description   : Free the allocated gloabal structure tree
//
// Parameter     : NONE
//
// Return Value  : VOID
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID FreePldmGlobals()
{
    FreeGlobals();
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure	 : 	FreeResetControlInfo
//
// Description : 	Free the allocated resetcontrolinfo structure tree
//
// Parameter	:	 NONE
//
// Return Value : NONE
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
void FreeResetControlInfo(void)
{
	CONTROL_RESET_FLAGINFO *ResetControlInfo = (CONTROL_RESET_FLAGINFO *)gResetControlInfo;
	CONTROL_RESET_FLAGINFO *TempControlInfo = (CONTROL_RESET_FLAGINFO *)NULL;

	while (ResetControlInfo)
	{
		TempControlInfo = ResetControlInfo->Next;

		if (ResetControlInfo)
		{
			free(ResetControlInfo);
		}

		ResetControlInfo = TempControlInfo;
	}

	gResetControlInfo = (CONTROL_RESET_FLAGINFO *)NULL;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure	 : 	LoadOptimalDefaultsOfAmiSce
//
// Description : 	Loads the optimal defaults to the setup controls
//
// Parameter	:	NONE
//
// Return Value : EFI_STATUS
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS  LoadOptimalDefaultsOfAmiSce(void)
{
	EFI_GUID  SceDefaultBootOrderGuid = DEFAULT_BOOT_ORDER_VARIABLE_GUID;
	EFI_GUID  SceDefaultLegacyDevOrderGuid = DEFAULT_LEGACY_DEV_ORDER_VARIABLE_GUID;
	VARIABLE Variable = {0};
	EFI_STATUS Status = EFI_SUCCESS;
	HII_DATA AmisceData = {0};
	UINT8 *CurrentFormSet = 0;
	UINT8 *CurrentForm = 0;
	UINT16 FormSetTitle = 0;
	UINT8* CurrentPackageList = 0;
	UINT8 *CurrentControl = 0;
	UINT16 Class = 0;
	UINT16 SubClass = 0;
	UINT16 FormSetHelp;
	UINT16 FormID = 0;
	UINT16 FormTitle = 0;
	GUID Guid = {0};
	UINT32 CRC32 = 0;
	UINTN DBSize = 0;
	UINT32 i = 0;
	UINT32 PackageCount = 0;
	CONTROL_INFO *Control;
	CONTROL_INFO NewControl;
	UINTN DefBuffSize = 0;
	UINT32 Attributes = 0;

	Status = AMI_RetrieveHIIData( &AmisceData.HIIDBData, &DBSize, &CRC32 );

	if (Status != EFI_SUCCESS)
	{
		DEBUG((DEBUG_ERROR, "\n Cannot access HII data !!\n"));
		return Status;
	}

	AmisceData.HIIDBCRC = CRC32;

	//gets the default values
	_GetStdMfgDefaultsBuffer();
	GetHiiHandles();
	PackageCount = GetTotalPackageList( AmisceData.HIIDBData );

	for (i = 0; i < PackageCount; i++)
	{
		CurrentPackageList = GetNextPackageList(AmisceData.HIIDBData , &CurrentPackageList);
		CurrentFormSet = GetNextUefiFormSet( CurrentPackageList, &CurrentFormSet, ( UINT8 * )&Guid,
											 &FormSetTitle, &FormSetHelp, &Class, &SubClass );

		gCurrentHiiHandle = gHiiHandles[i];
		if (CurrentFormSet == NULL /*|| Class == 0*/)
		{
			continue;
		}

		for (CurrentForm = 0; (CurrentForm = GetNextUefiForm( CurrentFormSet, &CurrentForm, &FormID, &FormTitle)); )
		{
			// each form in formset
			for (Control = 0; (Control = GetNextUefiControl( CurrentForm, &CurrentControl, &AmisceData, CurrentPackageList, &NewControl)); )
			{
				// each control in form
				char *OptimalDefBuffer = NULL;

				// skip controls the 401 form with blank name -- it will create ambiguous map strings.
				if (FormID == 0x401 && FormTitle == 2)
				{
					FreeControl(Control);
					continue;
				}

				if (IsNeededControl(Control) == FALSE)
				{
					FreeControl(Control);
					continue;
				}

				//gets the control's variable name
				GetVariableById(Control->VariableStorage, &Variable);

				if ((!wcscmp(Variable.VariableName, L"PlatformLang")) ||
						(!wcscmp(Variable.VariableName, L"DelBootOption")) ||
						(!wcscmp(Variable.VariableName, L"AddBootOption")) ||
						(!wcscmp(Variable.VariableName, L"UserDefaults")) ||
						(!wcscmp(Variable.VariableName, L"IDESecDev")) ||
						(!wcscmp(Variable.VariableName, L"LegacyDev")) ||
						(!wcscmp(Variable.VariableName, L"LegacyGroup")) ||
						(!wcscmp(Variable.VariableName, L"AddDriverOption")) ||
						(!wcscmp(Variable.VariableName, L"DelDriverOption")) ||
						((PcdGetBool(PcdPldmDonotLoadPasswordForDefaults)) && !wcscmp(Variable.VariableName, L"AMITSESetup") ))
				{
					FreeControl(Control);
					continue;
				}

				else if (!wcscmp(Variable.VariableName, L"BootOrder"))
				{
					VOID *DefBuffer = NULL;
					DefBuffSize = 0;
					
					Status = gRT->GetVariable(L"DefaultBootOrder", &SceDefaultBootOrderGuid, &Attributes, &DefBuffSize, DefBuffer);
					if (Status == EFI_NOT_FOUND)
					{
						FreeControl(Control);
						continue;
					}
					if ( Status == EFI_BUFFER_TOO_SMALL )
					{
						DefBuffer = AllocateZeroPool( DefBuffSize );
						if ( NULL == DefBuffer )
						{
							DEBUG((DEBUG_ERROR, "\n\rAllocatePool failed"));
							FreeControl(Control);
							return Status;
						}
						Status = gRT->GetVariable(L"DefaultBootOrder", &SceDefaultBootOrderGuid, &Attributes, &DefBuffSize, DefBuffer);
					}
					if ( Status != EFI_SUCCESS )
					{
						DEBUG((DEBUG_ERROR, "\n\rGet the DefaultBootOrder failed"));
						FreeControl(Control);
						return Status;
					}
					OptimalDefBuffer = AllocateZeroPool( DefBuffSize );
					if(NULL == OptimalDefBuffer )
					{
						DEBUG((DEBUG_ERROR, "\n\r AllocatePool failed"));
						FreeControl(Control);
						return Status;
					}
					memcpy(OptimalDefBuffer, (UINT8 *)DefBuffer, DefBuffSize);
					SceSetQuestionValue((UINT16)Control->VariableStorage, Control->DataOffset, (UINT16)DefBuffSize, (UINT8 *)OptimalDefBuffer);
					FreePool(DefBuffer);
					FreePool(OptimalDefBuffer);
					FreeControl(Control);
				}
				else if (!wcscmp(Variable.VariableName, L"LegacyDevOrder"))
				{
					VOID *DefBuffer = NULL;
					DefBuffSize = 0;
					Status = gRT->GetVariable( L"DefaultLegacyDevOrder", &SceDefaultLegacyDevOrderGuid, &Attributes, &DefBuffSize, DefBuffer);

					if (Status == EFI_NOT_FOUND)
					{
						FreeControl(Control);
						continue;
					}

					if ( Status == EFI_BUFFER_TOO_SMALL )
					{
						DefBuffer = AllocateZeroPool( DefBuffSize );
						
						if ( NULL == DefBuffer )
						{
							DEBUG((DEBUG_ERROR, "\n\rAllocatePool failed"));
							FreeControl(Control);
							return Status;
						}

						Status = gRT->GetVariable( L"DefaultLegacyDevOrder", &SceDefaultLegacyDevOrderGuid, &Attributes, &DefBuffSize, DefBuffer);
					}

					if ( Status != EFI_SUCCESS )
					{
						DEBUG((DEBUG_ERROR, "\n\rGet the DefaultLegacyDevOrder failed"));
						FreeControl(Control);
						return Status;
					}
					OptimalDefBuffer = AllocateZeroPool( DefBuffSize );
					if(NULL == OptimalDefBuffer )
					{
						DEBUG((DEBUG_ERROR, "\n\r AllocatePool failed"));
						FreeControl(Control);
						return Status;
					}
					memcpy(OptimalDefBuffer, (UINT8 *)DefBuffer, DefBuffSize);
					SceSetQuestionValue((UINT16)Control->VariableStorage, Control->DataOffset, (UINT16)DefBuffSize, (UINT8 *)OptimalDefBuffer);
					FreePool(DefBuffer);
					FreePool(OptimalDefBuffer);
					FreeControl(Control);
				}
				else
				{
					//gets the default value for the control
					_GetStdDefaults(Control);
					OptimalDefBuffer = AllocateZeroPool( Control->DataWidth );
					if(NULL == OptimalDefBuffer )
					{
						DEBUG((DEBUG_ERROR, "\n\r AllocatePool failed"));
						return Status;
					}
					memcpy(OptimalDefBuffer, (UINT8 *) & (Control->ValueInfo->OptimalDefaults), Control->DataWidth);
					//sets the optimal default value to the control
					SceSetQuestionValue((UINT16)Control->VariableStorage, Control->DataOffset, Control->DataWidth, (UINT8 *)OptimalDefBuffer);
					FreePool(OptimalDefBuffer);
				}
			}
		}
	}
	
	FreeHiiHandles();

	//import the changed variable buffer data to the NVRAM
	ImportVariableToNVRAM();
	_FreeVariableList();
	return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 ValueByHandle
//
// Description:  Find an value table entry given its handle
//
// Parameters:	 UINT8 *
//					[in] Address of PLDM value table
//				 INT32
//				    [in] Size of table in bytes
//				 UINT16
//				    [in] value handle
//
// Return:       PLDM_VALUE * - non 0 pointer on Success, 0 on Failure
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
PLDM_VALUE *
ValueByHandle(UINT8 *Values, int Size, UINT16 Handle)
{
	PLDM_VALUE *Value;
	INT32 Offset;

	for (Offset = -1; EFI_SUCCESS == GetNextValue(Values, Size, &Offset); )
	{
		Value = (PLDM_VALUE *)(Values + Offset);

		if (Value->Handle == Handle)
		{
			return Value;
		}
	}

	return NULL;
}

#include <Protocol/HiiConfigRouting.h>
#include <Protocol/HiiDatabase.h>

EFI_HII_CONFIG_ROUTING_PROTOCOL *gHiiConfigRouting;
//static EFI_GUID gEfiHiiConfigRoutingProtocolGuid = EFI_HII_CONFIG_ROUTING_PROTOCOL_GUID;
extern
EFI_HII_DATABASE_PROTOCOL *gHiiDatabaseProtocol;

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 GetHiiHandles
//
// Description:  Get the HII handle array and store in gHiiHandles. Assumes
//              that handles have a one-for-one correspondence to the export
//              packages in the database export data.
//
// Parameters:	 None
//
// Return:       None
//
// Modified:    gHiiHandles, gHiiNumHandles
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
void
GetHiiHandles()
{
	EFI_STATUS Status = EFI_SUCCESS;
	UINTN Length = 0;
	EFI_HANDLE *HiiHandles;
	int i = 0;

	free(gHiiHandles);
	gHiiHandles = 0;
	gHiiNumHandles = 0;
	HiiHandles = (EFI_HANDLE *)malloc(1);

	Status = gBS->LocateProtocol(&gEfiHiiDatabaseProtocolGuid, NULL, (VOID**)&gHiiDatabaseProtocol);
	Status = gHiiDatabaseProtocol->ListPackageLists(gHiiDatabaseProtocol, EFI_HII_PACKAGE_TYPE_ALL, NULL,
			 &Length, HiiHandles);
	free(HiiHandles);

	if (Status != EFI_BUFFER_TOO_SMALL)
	{
		return;
	}

	HiiHandles = (EFI_HANDLE *)malloc(Length);

	if (NULL == HiiHandles)
	{
		return;
	}

	Status = gHiiDatabaseProtocol->ListPackageLists(gHiiDatabaseProtocol, EFI_HII_PACKAGE_TYPE_ALL, NULL,
			 &Length, HiiHandles);

	if (Status != EFI_SUCCESS)
	{
		return;
	}

	gHiiHandles = HiiHandles;
	gHiiNumHandles = (UINT)Length / sizeof(*HiiHandles);
	/*Print(L"\n GetHiiHandles(): Hii Handles");
	for (i = 0; i < (int)gHiiNumHandles; i++)
	{
	    Print(L"HiiHandles[%d]:%x\r\n",i, gHiiHandles[i]);
	}*/
}

static
void
FreeHiiHandles()
{
	free(gHiiHandles);
	gHiiHandles = 0;
	gCurrentHiiHandle = 0;
}

extern
UINTN DPLength(EFI_DEVICE_PATH_PROTOCOL *pDp);

BOOLEAN
InFilter(EFI_HII_HANDLE Pack)
{
	UINT32 i;

	for (i = 0; i < gFilterCount; i++)
	{
		if (Pack == gFilterHandles[i])
		{
			return TRUE;
		}
	}

	return FALSE;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 GetPath
//
// Description:  Get the device path for a export pack given its index.
//
// Parameters:	 int
//                  Index to export package with HII data
//
// Return:       Pointer to device path
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
CHAR16 *
GetPath(int PackIndex)
{
	EFI_STATUS Status;
	EFI_HANDLE DriverHandle;
	EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
	CHAR16 *Path;
	int Length; // length of DevicePath in bytes
	int i;

	if (NULL == gHiiHandles)
	{
		return NULL;
	}

	// get driver handle
	Status = gHiiDatabaseProtocol->GetPackageListHandle (gHiiDatabaseProtocol, gHiiHandles[PackIndex], &DriverHandle);

	if (Status != EFI_SUCCESS)
	{
		return L"";
	}

	// Get device path
	Status = gBS->HandleProtocol (
				 DriverHandle,
				 &gEfiDevicePathProtocolGuid,
				 (VOID**)&DevicePath
			 );

	if (Status != EFI_SUCCESS)
	{
		return NULL;
	}

	// convert to string format
	Length = (int)DPLength(DevicePath);
	// each byte bloats to 2 unicodes
	Path = (CHAR16 *)malloc(Length * 4 + sizeof(CHAR16));

	if (NULL == Path)
	{
		return 0;
	}

	for (i = 0; i < Length; i++)
	{
		swprintf(&Path[i * 2], 5, L"%02x", ((UINT8 *)DevicePath)[i]);
	}

	return Path;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 MakeVarAttribute
//
// Description:  Make an attribute for the string representation of a variable
//              Default string is Hii config string for the variable
//
// Parameters:
//               MEMFILE *
//					[in] Buffer for attribute table
//               VARIABLE *
//                  [in] pointer to variable
//				 CHAR16 *
//				    [in] pointer to device path for variable
//				 int
//				    [in] Attribute handle
//				 int
//				    [in] Name handle
//
// Return:       none
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
void
MakeVarAttribute(MEMFILE *fp, VARIABLE *Variable, CHAR16 *Path, int Handle, int NameHandle)
{
	struct Table7 string;
	CHAR16 NumBuf[12];
	UINT8 *guid = (UINT8 *)&Variable->VariableGuid;
	// remember where attribute starts
	int AttrOffset = fp->CurrentOffset;
	ATTRIBUTE_POINTER a;
	int DefOffset; // offset to default data
	int i;
	DefOffset = fp->CurrentOffset + sizeof(string);

	string.CommonHeader.Handle = Handle;
	string.CommonHeader.NameHandle = NameHandle;
	string.CommonHeader.Type = BIOSStringReadOnly;
	string.Type = 0xff;
	string.MinLength = 0xffff;
	string.MaxLength = 0xffff;
	MemWrite(fp, &string, sizeof(string));
	// write default value
	MemWrite(fp, L"GUID=", (int)wcslen(L"GUID=") * 2);

	for (i = 0; i < sizeof(Variable->VariableGuid); i++)
	{
		CHAR16 buf[3];
		swprintf(buf, 3, L"%02x", guid[i]);
		MemWrite(fp, buf, 4);
	}

	MemWrite(fp, L"&NAME=", (int)wcslen(L"&NAME=") * 2);

	for (i = 0; i < (int)wcslen(Variable->VariableName); i++)
	{
		CHAR16 buf[5];
		swprintf(buf, 5, L"%04x", Variable->VariableName[i]);
		MemWrite(fp, buf, 4 * sizeof(buf[0]));
	}

	MemWrite(fp, L"&PATH=", (int)wcslen(L"&PATH=") * 2);
	MemWrite(fp, Path, (int)wcslen(Path) * 2);
	// offset and length for entire variable
	MemWrite(fp, L"&OFFSET=0", (int)wcslen(L"&OFFSET=0") * 2);
	MemWrite(fp, L"&WIDTH=", (int)wcslen(L"&WIDTH=") * 2);
	swprintf(NumBuf, sizeof(NumBuf) / sizeof(NumBuf[0]), L"%x", Variable->VariableSize);
	MemWrite(fp, NumBuf, (int)wcslen(NumBuf) * sizeof(CHAR16) + sizeof(CHAR16));   // null terminate
	// fix default length
	a.CommonHeader = (PLDM_ATTRIBUTE *)(fp->Buffer + AttrOffset);
	a.String->DefLength = fp->CurrentOffset  - DefOffset;
	gVariablesAdded++; //increment the variable count for handle index
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 MakeVarString
//
// Description:  Make a string for the string representation of a variable
//              String is the variable name. The string is not used for import.
//
// Parameters:
//               MEMFILE *
//					[in] Buffer for string table
//               VARIABLE *
//                  [in] pointer to variable
//				 int
//				    [in] attribute handle
//
// Return:       none
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
void
MakeVarString(MEMFILE *fp, VARIABLE *Variable, int Handle)
{
	// any name should do
	PLDM_STRING String;
	String.Handle = Handle;
	String.Length = ((UINT16)wcslen(Variable->VariableName) * sizeof(CHAR16)) + sizeof(CHAR16);
	MemWrite(fp, &String, sizeof(String));
	MemWrite(fp, Variable->VariableName, ((int)wcslen(Variable->VariableName) + 1) * sizeof(CHAR16));
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:	 MakeVarValue
//
// Description:  Make a value for the string representation of a variable
//              string is in hii config format "&VALUE=" followed by the data bytes as hex.
//              Note that the data bytes are in reverse order!
//
// Parameters:
//               MEMFILE *
//					[in] Buffer for value table
//               VARIABLE *
//                  [in] pointer to variable
//				 int
//				    [in] attribute handle
//
// Return:       none
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
void
MakeVarValue(MEMFILE *fp, VARIABLE *Variable, int Handle)
{
	struct Table16 string;
	CHAR16 NumBuf[12];
	int i;

	string.CommonHeader.Handle = Handle;
	string.CommonHeader.Type = BIOSStringReadOnly;
	string.Length = 4 * Variable->VariableSize;
	string.Length += (UINT16)((int)wcslen(L"&VALUE=") * 2);
	string.Length += 2; // null terminate
	MemWrite(fp, &string, sizeof(string));
	MemWrite(fp, L"&VALUE=", (int)wcslen(L"&VALUE=") * 2);

	// variable data are in reverse order!!
	for (i = Variable->VariableSize - 1; i >= 0; i--)
	{
		swprintf(NumBuf, sizeof(NumBuf) / sizeof(CHAR16), L"%02x", Variable->VariableBuffer[i]);
		MemWrite(fp, NumBuf, 4);
	}

	MemWrite(fp, L"", 2);
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    CreateVarTables
//
// Description:  Adds string controls representing variables not referenced by
//              any question with a mapping string prompt to end of tables.
//
//
// Parameter:    UINT8 *
//					[in]  Pointer to HII export data base
//				 UINT32
//					[in]  Size of export data base
//				 MEMFILE *
//				    [in]  Buffer for attributes
//				 UINT16
//				    [in]  First attribute handle to use
//				 MEMFILE  *
//				    [in]  Buffer for strings
//				 UINT16
//				    [in]  First string handle to use
//				 MEMFILE *
//				    [in]  Buffer for values
//
// Return value: EFI_STATUS
//
// Modified:
//
// Referral(s): Called by CreatePldmTables().
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
CreateVarTables(
	UINT8  *HiiData,       // pointer to HII export data
	UINT32  HiiSize,       // size of Hii Data
	MEMFILE *afp,
	UINT16  HandleA,       // 1st available attribute handle
	MEMFILE *sfp,
	UINT16  HandleS,       // 1st available string handle
	MEMFILE *vfp
)
{
	UINT8 *CurrentFormSet = 0;
	UINT8 *CurrentForm = 0;
	UINT16 FormSetTitle = 0;
	UINT8* CurrentPackageList = 0;
	UINT8 *CurrentControl = 0;
	UINT16 Class = 0;
	UINT16 SubClass = 0;
	UINT16 FormSetHelp;
	UINT16 FormID = 0;
	UINT16 FormTitle = 0;
	CONTROL_INFO *Control;
	CONTROL_INFO NewControl;
	GUID Guid = {0};
	int PackageCount;
	int i;
	UINTN j;
	CHAR16 *Path;
	VARIABLE *pVariable;
	UINTN Index = 0;
	UINT8 *StringPack;
	BOOLEAN StringPackFound = FALSE;
    CHAR8 **MappingLangArr = (CHAR8 **)NULL; 
    UINTN MappingLangCount = 0;
    
	GetHiiHandles();

	PackageCount = GetTotalPackageList(HiiData);

    MappingLangCount = GetPLDMMappingCount();
    MappingLangArr = GetPLDMMappingLanguageID();
   
    
	for (i = 0; i < PackageCount; i++)
	{
		CurrentPackageList = GetNextPackageList(AmisceData.HIIDBData , &CurrentPackageList);
		gCurrentHiiHandle = gHiiHandles[i];

		if (gFilterCount && !InFilter(gCurrentHiiHandle))
		{
			DEBUG((DEBUG_ERROR, "\n InFilter(%x) failed:%d", gCurrentHiiHandle, i));
			continue;
		}

		if (gSkipVarbyHiiHandles == TRUE)
		{
		    StringPackFound = FALSE;
	       
			for ( j = 0; j < MappingLangCount; j++) 
			{
				if ((StringPack = FindLangStringPack(CurrentPackageList, MappingLangArr[j])))
				{
				    StringPackFound = TRUE;
				    break;
				}
	        }
	        
			
	        if( StringPackFound == FALSE)
			{
				DEBUG((DEBUG_ERROR, "\n Map string present"));
				continue;   // Mapping strings present
			}
		}

		CurrentFormSet = 0;
		CurrentFormSet = GetNextUefiFormSet( CurrentPackageList, &CurrentFormSet, ( UINT8 * )&Guid,
											 &FormSetTitle, &FormSetHelp, &Class, &SubClass );

		if (CurrentFormSet == NULL /*|| Class == 0*/)
		{
			continue;
		}

		// get device path for formset
		for (CurrentForm = 0; (CurrentForm = GetNextUefiForm( CurrentFormSet, &CurrentForm, &FormID, &FormTitle)); )
		{
			// each form in formset
			// pass through the controls to mark variables referenced
			// by controls with x-UEFI-AMI strings
			for (Control = 0; (Control = GetNextUefiControl( CurrentForm, &CurrentControl, &AmisceData, CurrentPackageList, &NewControl)); )
			{
				// each control in form
				// check for x-UEFI-AMI prompt string
				UINT16 *Title;
				Title = PldmGetHiiString(CurrentPackageList, HiiData, Control->ControlPrompt, TRUE);

				if (Title && Title[0])
				{
					// Mark variable as having at least one mapped control
					MarkVarReferenced(Control->VariableStorage);
				}

				FreeControl(Control);
				free(Title);
			}
		}

		// Get Device path for hii
		Path = GetPath(i);

		// add strings for unreferenced variables
        for (pVariable = 0; pVariable = GetNextVariableInfo(pVariable); )
		{
			// each question
			if (pVariable->Referenced == 0 && pVariable->VariableSize && Path)
			{
                if (0 == pVariable->VariableName[0])
                    continue;   // name value declaration
                if (pVariable->VariableBuffer == NULL)
                    continue;
                // add the string to tables
				if ((!wcscmp(pVariable->VariableName, L"PlatformLang")) ||
						(!wcscmp(pVariable->VariableName, L"DelBootOption")) ||
						(!wcscmp(pVariable->VariableName, L"AddBootOption")) ||
						(!wcscmp(pVariable->VariableName, L"UserDefaults")) ||
						(!wcscmp(pVariable->VariableName, L"IDESecDev")) ||
						(!wcscmp(pVariable->VariableName, L"LegacyDev")) ||
						(!wcscmp(pVariable->VariableName, L"LegacyGroup")) ||
						(!wcscmp(pVariable->VariableName, L"AddDriverOption")) ||
						(!wcscmp(pVariable->VariableName, L"DelDriverOption")) ||
						((PcdGetBool(PcdPldmDonotLoadPasswordForDefaults )) && !wcscmp(pVariable->VariableName, L"AMITSESetup") ))
				{
					continue;
				}

				// create device path from export pack GUID
				MakeVarAttribute(afp, pVariable, Path, HandleA, HandleS);
				MakeVarValue(vfp, pVariable, HandleA++);
				MakeVarString(sfp, pVariable, HandleS++);
			}
		}

		free(Path);
		_FreeVariableList();
	}

	FreeHiiHandles();
	return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    CreateBootVarTables
//
// Description:  Boot Variables are added as read onyl string controls at 
//              the end of the Tables
//
//
// Parameter:    UINT8 *
//					[in]  Pointer to HII export data base
//				 UINT32
//					[in]  Size of export data base
//				 MEMFILE *
//				    [in]  Buffer for attributes
//				 UINT16
//				    [in]  First attribute handle to use
//				 MEMFILE  *
//				    [in]  Buffer for strings
//				 UINT16
//				    [in]  First string handle to use
//				 MEMFILE *
//				    [in]  Buffer for values
//
// Return value: EFI_STATUS
//
// Modified:
//
// Referral(s): Called by CreatePldmTables() before adding CRCs.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
CreateBootVarTables(
	UINT8  *HiiData,       // pointer to HII export data
	UINT32  HiiSize,       // size of Hii Data
	MEMFILE *afp,
	UINT16  HandleA,       // 1st available attribute handle
	MEMFILE *sfp,
	UINT16  HandleS,       // 1st available string handle
	MEMFILE *vfp
)
{
	UINT8 *CurrentFormSet = 0;
	UINT8 *CurrentForm = 0;
	UINT16 FormSetTitle = 0;
	UINT8* CurrentPackageList = 0;
	UINT8 *CurrentControl = 0;
	UINT16 Class = 0;
	UINT16 SubClass = 0;
	UINT16 FormSetHelp;
	UINT16 FormID = 0;
	UINT16 FormTitle = 0;
	GUID Guid = {0};
	int PackageCount;
	int i;
	CHAR16 *Path;
	VARIABLE *pVariable;
	UINTN Index = 0;

	DEBUG((DEBUG_ERROR, "\n CreateBootVarTables"));

	GetHiiHandles();

	PackageCount = GetTotalPackageList(HiiData);

	for (i = 0; i < PackageCount; i++)
	{
		CurrentPackageList = GetNextPackageList(AmisceData.HIIDBData , &CurrentPackageList);
		gCurrentHiiHandle = gHiiHandles[i];

		if (gFilterCount && !InFilter(gCurrentHiiHandle))
		{
			DEBUG((DEBUG_ERROR, "\n InFilter(%x) failed:%d", gCurrentHiiHandle, i));
			continue;
		}
		
		CurrentFormSet = 0;
		CurrentFormSet = GetNextUefiFormSet( CurrentPackageList, &CurrentFormSet, ( UINT8 * )&Guid,
											 &FormSetTitle, &FormSetHelp, &Class, &SubClass );

		if (CurrentFormSet == NULL /*|| Class == 0*/)
		{
			continue;
		}

		// get device path for formset
		for (CurrentForm = 0; (CurrentForm = GetNextUefiForm( CurrentFormSet, &CurrentForm, &FormID, &FormTitle)); )
		{
		}

		// Get Device path for hii
		Path = GetPath(i);

		// add strings for unreferenced variables
		for (pVariable = 0; pVariable = GetNextVariableInfo(pVariable); )
		{
			//Save the BootOrder Variable
			if (!wcscmp(pVariable->VariableName, L"BootOrder"))
			{
				UINT8 *BootOrder = (UINT8 *)NULL;
				UINT64 Size = 0;
				UINT16 Index = 0;
				UINT8 *BootXXXX = (UINT8 *)NULL;
				UINT64 BootSize = 0;
				wchar_t BootStr[MAX_LENGTH] = {0};
				UINT32 Attributes = 0;
				EFI_GUID gEfiGlobalVariableGuid = EFI_GLOBAL_VARIABLE_GUID;

				DEBUG((DEBUG_ERROR, "\n Create BootOrder"));
				MakeVarAttribute(afp, pVariable, Path, HandleA, HandleS);
				MakeVarValue(vfp, pVariable, HandleA++);
				MakeVarString(sfp, pVariable, HandleS++);
				
				// Retrieve the BootOrder Variable
				BootOrder = VarGetNvramName( L"BootOrder", &gEfiGlobalVariableGuid, &Attributes, (UINTN *)&Size );
				if ( BootOrder != NULL )
				{
					// For each Boot Option retrieve the BootXXXX variable
					for ( Index = 0; Index < Size; Index += 2 )
					{
						VARIABLE *pBootVariable;
						UINT8* Temp = NULL;

						BootSize = 0;
						Attributes = 0;
						
						// UEFI spec defines Boot Order Index as Upper case eg. "Boot000A".
						// But Aptio uses it in lower case. Change L"Boot%02x%02x" to L"Boot%02X%02X"
						// once Aptio changes it.
						swprintf( BootStr, MAX_LENGTH, L"Boot%02X%02X", BootOrder[Index + 1], BootOrder[Index]);

#if EFI_DEBUG
						Temp = (UINT8*) calloc(wcslen(BootStr)+1, sizeof(UINT8));
						wcstombs(Temp, BootStr, wcslen(BootStr));
			        		DEBUG((DEBUG_ERROR, "\n Boot VariableName: %a", Temp));
			        		free(Temp);
#endif
			        		
						BootXXXX = VarGetNvramName( BootStr, &gEfiGlobalVariableGuid, &Attributes, (UINTN*)&BootSize );

						if ( BootXXXX && BootSize != 0 )
						{
							pBootVariable = ( VARIABLE * )calloc( sizeof( VARIABLE ), sizeof( UINT8 ));

							if (NULL == pBootVariable)
							{
								DEBUG((DEBUG_ERROR, "Memory Allocation Failure")); //MemAllocFailure();
							}

							//Contruct Variable structure
							//Var->VariableIndex = Index; //not needed
							//Var->VariableID = VarID; //not needed
							memcpy(( &pBootVariable->VariableGuid), &gEfiGlobalVariableGuid, sizeof( EFI_GUID ));
							pBootVariable->VariableAttributes = Attributes;
							wcscpy( pBootVariable->VariableName, BootStr );
							pBootVariable->VariableSize = (UINT16)BootSize;
							//Var->Referenced = 0; //not needed
							// hii handle, if available, 0 otherwise
							pBootVariable->VariableHandle = gCurrentHiiHandle;
							//Var->ExtendedAttibutes = ExtendedAttr; //not needed
							//Var->VariableNameId = 0; //not needed
							pBootVariable->VariableBuffer = BootXXXX;

							//Add BootXXXX variable to Tables
							MakeVarAttribute(afp, pBootVariable, Path, HandleA, HandleS);
							MakeVarValue(vfp, pBootVariable, HandleA++);
							MakeVarString(sfp, pBootVariable, HandleS++);

							free(pBootVariable);
						}
						else
						{
							swprintf( BootStr, MAX_LENGTH, L"Boot%02x%02x", BootOrder[Index + 1], BootOrder[Index]);
							BootXXXX = ( UINT8 * )VarGetNvramName( BootStr, &gEfiGlobalVariableGuid, &Attributes, (UINTN*)&BootSize );
			
							if ( BootXXXX && BootSize != 0 )
							{
								pBootVariable = ( VARIABLE * )calloc( sizeof( VARIABLE ), sizeof( UINT8 ));

								if (NULL == pBootVariable)
								{
									DEBUG((DEBUG_ERROR, "Memory Allocation Failure")); //MemAllocFailure();
								}
								
								//Contruct Variable structure
								memcpy(( &pBootVariable->VariableGuid), &gEfiGlobalVariableGuid, sizeof( EFI_GUID ));
								pBootVariable->VariableAttributes = Attributes;
								wcscpy( pBootVariable->VariableName, BootStr );
								pBootVariable->VariableSize = (UINT16)BootSize;
								pBootVariable->VariableHandle = gCurrentHiiHandle;
								pBootVariable->VariableBuffer = BootXXXX;
								
								//Add BootXXXX variable to Tables
								MakeVarAttribute(afp, pBootVariable, Path, HandleA, HandleS);
								MakeVarValue(vfp, pBootVariable, HandleA++);
								MakeVarString(sfp, pBootVariable, HandleS++);

								free(pBootVariable);
							}
						}
					}
				}
				else
				{
				throw (L"Memory Allocation Failure");
				}
			}
			
			//Save the LegacyDevOrder Variable
			if (!wcscmp(pVariable->VariableName, L"LegacyDevOrder"))
			{
				UINTN Varsize = 0;
				UINT32 Attributes = 0;
				EFI_GUID LegacyDevGuid = LEGACY_DEV_ORDER_GUID;
				
				DEBUG((DEBUG_ERROR, "\n GetVar LegacyDevOrder"));
				pVariable->VariableBuffer = ( UINT8 * )VarGetNvramName( (wchar_t *)L"LegacyDevOrder", &LegacyDevGuid, &Attributes, (UINTN *)&Varsize );

				pVariable->VariableSize = (UINT16)Varsize;
				
				DEBUG((DEBUG_ERROR, "\n Create LegacyDevOrder, Varsize: %x", Varsize));
				MakeVarAttribute(afp, pVariable, Path, HandleA, HandleS);
				MakeVarValue(vfp, pVariable, HandleA++);
				MakeVarString(sfp, pVariable, HandleS++);
			}
		}

		free(Path);
		_FreeVariableList();
	}

	FreeHiiHandles();
	return EFI_SUCCESS;
}

static
EFI_STATUS
ReplacePath(CHAR16 *ConfigIn, CHAR16 **ConfigOut, EFI_HII_HANDLE Handle)
{
	int Length;
	EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
	EFI_HANDLE DriverHandle;
	EFI_STATUS Status;
	MEMFILE Buffer;
	CHAR16 *String;
	int i;
	*ConfigOut = 0;

	if (gHiiDatabaseProtocol == NULL)
	{
		Status = gBS->LocateProtocol(&gEfiHiiDatabaseProtocolGuid, NULL, (VOID**)&gHiiDatabaseProtocol);

		if (Status != EFI_SUCCESS)
		{
			return Status;
		}
	}

	// get device handle for hii handle
	Status = gHiiDatabaseProtocol->GetPackageListHandle (gHiiDatabaseProtocol, Handle, &DriverHandle);

	if (Status != EFI_SUCCESS)
	{
		return Status;
	}

	// Get device path
	Status = gBS->HandleProtocol (
				 DriverHandle,
				 &gEfiDevicePathProtocolGuid,
				 (VOID**)&DevicePath
			 );

	if (Status != EFI_SUCCESS)
	{
		return Status;
	}

	// copy chars before path
	Length = (int)DPLength(DevicePath);
	String = wcsstr(ConfigIn, L"&PATH=");

	if (0 == String)
	{
		return EFI_NOT_FOUND;
	}

	MemCreat(&Buffer);
	String += wcslen(L"&PATH=");
	MemWrite(&Buffer, ConfigIn, (UINT32)((String - ConfigIn) * sizeof(CHAR16)));

	// copy new path
	for (i = 0; i < Length; i++)
	{
		CHAR16 Buf[3];
		swprintf(Buf, 3, L"%02x", ((UINT8 *)DevicePath)[i]);
		MemWrite(&Buffer, Buf, (UINT32)(wcslen(L"xx") * sizeof(CHAR16)));
	}

	// copy remaining chars
	String = wcschr(String, L'&');

	if (String == 0)
	{
		return EFI_ABORTED;
	}

	MemWrite(&Buffer, String, (UINT32)((wcslen(String) + 1) * sizeof(CHAR16)));
	*ConfigOut = (CHAR16 *)Buffer.Buffer;
	Buffer.Buffer = 0;

	if (MemClose(&Buffer))
	{
		throw (L"ReplacePath: Memory exhaustion");
	}

	return EFI_SUCCESS;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    ImportVarValuesByHiiHandle
//
// Description:  This function imports saved variables
//
//
// Parameter:    EFI_HII_HANDLE *Handle
//					[in]  Handle for device path substitution, if null no substitution
//				 UINT32 HiiSize
//					[in]  Size of export data base
//				 UINT8 *ValueTable
//				    [in]  Pointer to PLDM value table
//				 UINT32 ValueSize
//				    [in]  Size of value table
//				 UINT8  *AttributeTable
//				    [in] attribute table from foreign system
//				 UINT32 AttributeSize
//				    [in] Size of foreign attribute table
//				 UINT8 *EngStringTable
//				    [in] English string table from foreign system
//				 UINT32 EngStringSize
//				    [in] Size of English string table
//
// Return value: EFI_STATUS
//
// Modified:    NVRAM values for questions
//
// Referral(s):
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
ImportVarValuesByHiiHandle(
	EFI_HII_HANDLE Handle,
	UINT8  *ValueTable,
	UINT32  ValueSize,
	UINT8  *AttributeTable,
	UINT32  AttributeSize,
	UINT8  *StringTable,
	UINT32  StringSize
)
{
	int Offset;
	ATTRIBUTE_POINTER Attribute;
	VALUE_POINTER Value;
	EFI_STATUS Status;
	EFI_STATUS RouteConfigStatus = EFI_SUCCESS;
	
    Status = gBS->LocateProtocol(&gEfiHiiDatabaseProtocolGuid, NULL, (VOID**)&gHiiDatabaseProtocol);
    if (Status)
        return Status;
	// check parameters
	// check CRCs    int Offset;
	for (Offset = -1; EFI_SUCCESS == GetNextAttribute(AttributeTable, AttributeSize, &Offset); )
	{
		Attribute.CommonHeader = (PLDM_ATTRIBUTE *)(AttributeTable + Offset);

		if (Attribute.CommonHeader->Type != BIOSStringReadOnly)
		{
			continue;    // not a string control
		}

		if (Attribute.String->Type != 0xff)
		{
			continue;    // not a string attribute representing a variable
		}

		// get the value
		Value.CommonHeader = ValueByHandle(ValueTable, ValueSize, Attribute.CommonHeader->Handle);

		if (Value.CommonHeader)
		{
			// write the variable
			// concatenate data with routing string
			CHAR16 *s;
			CHAR16 *Progress;
			int Size = Attribute.String->DefLength + Value.String->Length + 2;
			s = (CHAR16*)calloc(Size, sizeof(CHAR16));

			if (NULL == s)
			{
				return EFI_OUT_OF_RESOURCES;
			}

			DEBUG((DEBUG_ERROR, "\n Import Var Handle: %x", Attribute.CommonHeader->Handle));
			// efi string functions assert even alignment
			memcpy(s, Attribute.String->DefString, Attribute.String->DefLength); // string includes a null
			memcpy(s + wcslen(s), Value.String->String, Value.String->Length);   // string includes a null
			// send out via routing protocol
			Status = gBS->LocateProtocol(&gEfiHiiConfigRoutingProtocolGuid, NULL, (VOID**)&gHiiConfigRouting);

			// if there is a handle, substitute its device path
			if (Handle)
			{
				CHAR16 *NewS;
				Status = ReplacePath(s, &NewS, Handle);
				free(s);

				if (Status == EFI_SUCCESS)
				{
					s = NewS;
				}
				else
				{
					return EFI_ABORTED;
				}
			}

			RouteConfigStatus = gHiiConfigRouting->RouteConfig(gHiiConfigRouting, s, &Progress);
			DEBUG((DEBUG_ERROR, "\n RouteConfig Status: %x", RouteConfigStatus));
			free(s);
		}
	}

	return RouteConfigStatus;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    CreatePldmTablesByHiiHandles
//
// Description:  This function constructs string, attribute, and value tables
//               Values come from NVRAM.
//
// Parameter:    EFI_HII_HANDLE *HiiHandles
//					[in]  Pointer to array of handles
//				 UINT32 HiiHandleCount
//					[in]  Size of export data base
//				 UINT8  **StringTable
//					[out] return pointer to string table
//				 UINT32 *StringSize
//				    [out] return string table size
//               UINT8 **AttributeTable
//                  [out] return pointer to attribute table
//               UINT32 *AttributeSize
//                  [out] return attribute table size
//               UINT8 **ValueTable
//                  [out] return pointer to value table
//               UINT8  *ValueSize
//                  [out] return value table size
//				 UINT8  **MapStringTable
//					[out] return pointer to map string table
//				 UINT32 *MapStringSize
//				    [out] return map string table size
//				 BOOLEAN SaveVars
//				    [in] Save unreferenced variables
//
// Return value: EFI_STATUS
//
// Modified:
//
// Referral(s):
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
CreatePldmTablesByHiiHandles(
	EFI_HII_HANDLE  *HiiHandles,	// pointer to HII export data
	UINT32  HiiHandleCount,     	// size of Hii Data
	UINT8 **StringTable,		// return string table pointer
	UINT32 *StringSize,   		// return string table size
	UINT8 **AttributeTable,		// return attribute table pointer
	UINT32 *AttributeSize, 		// return attribute table size
	UINT8 **ValueTable,    		// return value table pointer
	UINT32 *ValueSize,     		// return value size
	UINT8 **MapStringTable,		// return map string table pointer
	UINT32 *MapStringSize,  	// return map string table size
	BOOLEAN SaveVars
)
{
	UINTN HiiSize;
	UINT8 *HiiData;
	UINT32 Crc32;
	EFI_STATUS Status;
	// Get the Hii data
	Status = AMI_RetrieveHIIData(&HiiData, &HiiSize, &Crc32);

	if (Status != EFI_SUCCESS)
	{
		return Status;
	}

	gFilterHandles = HiiHandles;
	gFilterCount = (UINT32)HiiHandleCount;
	// call regular interface
	Status = CreatePldmTables(
				 HiiData,
				 (UINT32)HiiSize,
				 StringTable,
				 StringSize,
				 AttributeTable,
				 AttributeSize,
				 ValueTable,
				 ValueSize,
				 MapStringTable,
				 MapStringSize,
				 SaveVars
			 );
	DEBUG((DEBUG_ERROR, "CreatePldmTablesByHiiHandles status:%x\r\n", Status));
	gFilterHandles = 0;
	gFilterCount = 0;
	
	return Status;
}

/**
    Duplicates a String from CHAR8 to CHAR16

    @param CHAR8* string 

    @retval CHAR16 *


**/
CHAR16 *StrDup8to16( CHAR8 *string )
{
    CHAR16  *text;
    UINTN   i;

    text = AllocateZeroPool ( (1 + strlen ( string )) * sizeof(CHAR16));
    if ( text != NULL )
    {
        i=0;
        while(text[i] = (CHAR16)string[i])
            i++;
        
        return text;
    }
    return NULL;
}

/**
    Duplicates a String from CHAR16 to CHAR8

    @param CHAR16* string

    @retval CHAR8 *


**/
CHAR8* StrDup16to8(CHAR16 *String)
{
	CHAR8 *text;
	UINTN len = 0, j = 0;

	while(String[j++])
		len++;
	
	text = AllocateZeroPool( (1 + len) * sizeof(CHAR8));

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

/**
    Gets the handle in BIOS attribute table for attribute handle

    @param IN CHAR16 * -> Mapping string
    @param IN UINT16 -> BIOS attribute handle
    @param OUT void ** -> Value to be returned
    @param OUT UINTN * -> Size of the return value

    @retval EFI_STATUS

**/
EFI_STATUS GetValueFromValueTable (IN CHAR16 *MappingString, IN UINT16 BiosAttributeHandle, OUT void **ValueString, OUT UINTN *ValueSize)
{
    INT32           ValueOffset = 0;
    PLDM_VALUE      *Value = NULL;
    void            *ValueFromTable = NULL;
    void            *TempValueFromTable = NULL;
    EFI_STATUS      Status = EFI_NOT_FOUND;
    CHAR16 *EnumString = (CHAR16 *)NULL;
    
    for (ValueOffset = -1; EFI_SUCCESS == GetNextValue(gValueTable, gValueSize, &ValueOffset); )
    {
        Value = (PLDM_VALUE *)(gValueTable + ValueOffset);

        if (Value->Handle == BiosAttributeHandle)
        {
            switch (Value->Type)
            {
                case BIOSEnumeration:
                {
					 if( (FALSE == gRedfishEnabled))
					 {
                        ValueFromTable = AllocateZeroPool (sizeof (UINT8) + sizeof (CHAR16));   //1 byte of index and 1 byte for NULL
                        if (NULL == ValueFromTable)
                        {
                            Status = EFI_OUT_OF_RESOURCES;
                            goto DONE;
                        }
                        (void)sprintf (ValueFromTable, "%d", ((struct Table15 *)Value)->ValueIndex [0]);
                        
                        *ValueString = ValueFromTable;
                        *ValueSize = sizeof (UINT8);
                        Status = EFI_SUCCESS;
					 }
					 else
					 {
                        int         Offset;
                        PLDM_STRING *String;
                        
                        struct Table6 *EnumControl = (struct Table6 *)gControlAttribute;
                        UINT16 EnumStringHandle = EnumControl->Handle[((struct Table15 *)Value)->ValueIndex [0]];
                        
                        for (Offset = -1; EFI_SUCCESS == GetNextString (gStringTable, gStringSize, &Offset); )
                        {
                            String = (PLDM_STRING *)(gStringTable + Offset);
                            
                            if (String->Handle != EnumStringHandle)
                            {
                                continue;
                            }
                            EnumString = AllocateZeroPool (String->Length +2);
                            if (NULL == EnumString)
                            {
                                Status = EFI_OUT_OF_RESOURCES;
                                goto DONE;
                            }
                            
                            memcpy (EnumString, String->String, String->Length);
                            
                            ValueFromTable = StrDup16to8(EnumString);
                            
                            *ValueString = ValueFromTable;
                            //Conversion from char16 to char8. Hence buffersize is also halved.
                            *ValueSize = (String->Length)/2;
                            Status = EFI_SUCCESS;
                            FreePool(EnumString);
                            break;               
                        }
					}
					break;               
                }
                case BIOSString:
                {
                    struct Table16 *StringControl = (struct Table16 *)Value;
                    
                    if( 0 == StringControl->Length)
                    {
                    	*ValueString = NULL;
						*ValueSize = 0;
						Status = EFI_SUCCESS;
						break;
                    }
                    TempValueFromTable = AllocateZeroPool (StringControl->Length + 4); //+4 for NULL and other for safety
                    if (NULL == TempValueFromTable)
                    {
                        Status = EFI_OUT_OF_RESOURCES;
                        goto DONE;
                    }
                    memcpy (TempValueFromTable, StringControl->String, StringControl->Length);
					ValueFromTable = StrDup16to8 (TempValueFromTable);
					FreePool (TempValueFromTable);	
                    *ValueString = ValueFromTable;
					//Conversion from char16 to char8. Hence buffersize is also halved.
					*ValueSize = (StringControl->Length)/2;
                    Status = EFI_SUCCESS;
                    break;
                }
                case BIOSPassword:
                case BIOSPasswordReadOnly:
                {
                    struct Table17 *PasswordControl = (struct Table17 *)Value;
                    
                    ValueFromTable = AllocateZeroPool (PasswordControl->Length );
                    if (NULL == ValueFromTable)
                    {
                        Status = EFI_OUT_OF_RESOURCES;
                        goto DONE;
                    }
                    memcpy (ValueFromTable, PasswordControl->PassWord, PasswordControl->Length);

                    *ValueString = ValueFromTable;
                    *ValueSize = PasswordControl->Length;
                    Status = EFI_SUCCESS;
                    break;
                }
                case BIOSInteger:
                {
                    struct Table18 IntegerValue = *(struct Table18 *)Value;
                    
                    ValueFromTable = AllocateZeroPool (20);
                    if (NULL == ValueFromTable)
                    {
                        Status = EFI_OUT_OF_RESOURCES;
                        goto DONE;
                    }
                    (void)sprintf (ValueFromTable, "%lld", IntegerValue.Value);
                    *ValueString = ValueFromTable;
                    *ValueSize = sizeof (IntegerValue.Value);
                    Status = EFI_SUCCESS;
                    break;
                }
                default:
                    break;
            }
            break;          //Match founds so break
        }
    }
DONE:
    return Status;
}
/**
    Gets the handle in BIOS attribute table for attribute handle

    @param IN UINT16 -> Mapping string handle
    @param OUT UINT16 * -> Handle for the mapping string handle in BIOS attribute table
    @param OUT UINT8 * -> Type of the control
    
    @retval EFI_STATUS

**/
EFI_STATUS FindMappingAttributeHandle (IN UINT16 MappingStringHandle, OUT UINT16 *BiosAttributeHandle, OUT UINT8 *ControlType)
{
    int                 Offset = 0;
    PLDM_ATTRIBUTE      *Attribute;
    EFI_STATUS          Status = EFI_NOT_FOUND;

    for (Offset = -1; EFI_SUCCESS == GetNextAttribute (gAttributeTable, gAttributeSize, &Offset); )
    {
        Attribute = (PLDM_ATTRIBUTE *)(gAttributeTable + Offset);

        if (Attribute->NameHandle == MappingStringHandle)
        {
            *BiosAttributeHandle = Attribute->Handle;
            Status = EFI_SUCCESS;
            //Used for creating attribute table
            gControlAttribute = (PLDM_ATTRIBUTE *)(gAttributeTable + Offset);
            if( (TRUE == gRedfishEnabled) && (BIOSInteger == Attribute->Type) )
            {
            	ATTRIBUTE_POINTER p;
            	
            	p.CommonHeader = gControlAttribute;
            	
            	if( (1 == p.Numeric->UpperBound) && ( 0 == p.Numeric->LowerBound ) )
            	{
            		*ControlType = BiosCheckBox;
            	}
            	else
            		*ControlType = BIOSInteger;
            }
            else
            {
                *ControlType = Attribute->Type; 
            }
            break;
        }
    }
    return Status;
}

/**
    Gets the handle in mapping string table for mapping string

    @param IN CHAR16 * -> Mapping string
    @param OUT UINT16 * -> Handle for the mapping string in the MappingStringTable
    @param BOOLEAN -> Flag to show value to set or get

    @retval EFI_STATUS

**/
EFI_STATUS FindMappingStringHandle (IN CHAR16 *MappingString, OUT UINT16 *MappingStringHandle, BOOLEAN SetValue)
{
    int         Offset;
    PLDM_STRING *String;
    EFI_STATUS  Status = EFI_NOT_FOUND;
    
    if (NULL == MappingString)
    {
        return Status;
    }
    for (Offset = -1; EFI_SUCCESS == GetNextString (gMapStringTable, gMapStringSize, &Offset); )
    {
        String = (PLDM_STRING *)(gMapStringTable + Offset);
        
        if (String->Length != StrLen (MappingString) * sizeof (UINT16))
        {
            continue;
        }
        if (0 == memcmp (String->String, MappingString, String->Length))
        {
            *MappingStringHandle = String->Handle;
            Status = EFI_SUCCESS;
            //Create the string table only for Set Control Value
            if (TRUE == SetValue)
            {
                PLDM_STRING S;
                
                // output string Table
                MemCreat (&MapBuffer);
                
                S.Handle = String->Handle;
                S.Length = String->Length;
                
                MemWrite(&MapBuffer, &S, sizeof(S));
                MemWrite(&MapBuffer, String->String, S.Length);
            }
            break;
        }
    }
    return Status;
}

/**
    Add the strings to the string Table for the Matching string handle

    @param IN UINT16 -> Mapping string handle

    @retval EFI_STATUS

**/
EFI_STATUS AddStringByHandle( UINT16 Handle)
{
    PLDM_STRING *String;
    INT32 Offset;

    for (Offset = -1; EFI_SUCCESS == GetNextString(gStringTable, gStringSize, &Offset); )
    {
        String = (PLDM_STRING *)(gStringTable + Offset);

        if (String->Handle == Handle)
        {
            PLDM_STRING S;
                            
            S.Handle = String->Handle;
            S.Length = String->Length;
            
            MemWrite(&StringBuffer, &S, sizeof(S));
            MemWrite(&StringBuffer, String->String, S.Length);
                            
            return EFI_SUCCESS;
        }
    }

    return EFI_NOT_FOUND;
}
/**
    Add the strings to the string Table for the Enumeration controltype

    @param IN UINT16 -> Mapping string handle

    @retval EFI_STATUS

**/
EFI_STATUS AddStringsForEnum( UINT16 MappingStringHandle)
{
    int                 Offset = 0;
    PLDM_ATTRIBUTE      *Attribute;
    EFI_STATUS          Status = EFI_NOT_FOUND;
    ATTRIBUTE_POINTER EnumAttribute;
    UINT16 Handle;
    int i;
    
    for (Offset = -1; EFI_SUCCESS == GetNextAttribute (gAttributeTable, gAttributeSize, &Offset); )
    {
        Attribute = (PLDM_ATTRIBUTE *)(gAttributeTable + Offset);
    
        if (Attribute->NameHandle == MappingStringHandle)
        {
            Status = EFI_SUCCESS;
            EnumAttribute.CommonHeader = (PLDM_ATTRIBUTE *)(gAttributeTable + Offset);
            for(i=0; i < EnumAttribute.Enumeration->PossibleValues; i++)
            {
                Handle = EnumAttribute.Enumeration->Handle[i];
                AddStringByHandle(Handle);
            }
            break;
        }
    }
    return Status;
}
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    ValueByHandle
//
// Description:  Find an value table entry given its handle
//
// Parameters:   UINT8 *
//                  [in] Address of PLDM value table
//               INT32
//                  [in] Size of table in bytes
//               UINT16
//                  [in] value handle
//
// Return:       PLDM_VALUE * - non 0 pointer on Success, 0 on Failure
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
EFI_STATUS
FindValueByAttribHandle(UINT8 *Values, int Size, UINT16 Handle)
{
	VALUE_POINTER ControlValueInfo;
	PLDM_VALUE *Value;
    INT32 Offset;

    for (Offset = -1; EFI_SUCCESS == GetNextValue(Values, Size, &Offset); )
    {
        Value = (PLDM_VALUE *)(Values + Offset);

        if (Value->Handle == Handle)
        {
        	ControlValueInfo.CommonHeader = (PLDM_VALUE *)(Values + Offset);
        	if( Value->Type == BIOSInteger )
        	{
        		gControlValue.CommonHeader = (PLDM_VALUE *)malloc(sizeof(*gControlValue.Numeric));
        	}
        	else if( Value->Type == BIOSEnumeration )
        	{
        		gControlValue.CommonHeader = (PLDM_VALUE *)malloc(sizeof(*gControlValue.Enumeration) + sizeof(UINT8));
        	}
        	else if( Value->Type == BIOSString )
			{
				gControlValue.CommonHeader = (PLDM_VALUE *)malloc(sizeof * gControlValue.String);
			}
        	else if( (Value->Type == BIOSPassword) || (Value->Type == BIOSPasswordReadOnly) )
			{
        	    gPasswordCtrlValue.CommonHeader = (PLDM_VALUE *)(Values + Offset);
				gControlValue.CommonHeader = (PLDM_VALUE *)malloc(sizeof * gControlValue.PassWord);
			}
        	if (NULL == gControlValue.CommonHeader)
			{
				return EFI_NOT_FOUND;
			}
        	//Assign the handle value and type
        	(*gControlValue.CommonHeader).Handle = Value->Handle;
        	(*gControlValue.CommonHeader).Type = Value->Type;
        }
    }

    return EFI_SUCCESS;
}

/**
    Inits the PLDM tables

    @retval EFI_STATUS

**/
EFI_STATUS InitPldmTables ()
{
    
    EFI_STATUS  Status = EFI_SUCCESS;
    
    Status = CreatePldmTablesByHiiHandles (
                NULL,
                0,
                &gStringTable,
                &gStringSize,
                &gAttributeTable,
                &gAttributeSize,
                &gValueTable,
                &gValueSize,
                &gMapStringTable,
                &gMapStringSize,
                TRUE
            );
    
    return Status;
}

/**
    Get the value for mapping string

    @param IN CHAR16 * -> Mapping string 
    @param OUT void ** -> Buffer to return
    @param OUT UINTN * -> Size of the buffer
    @param OUT UINT8 * -> Type of the control

    @retval EFI_STATUS

**/
EFI_STATUS GetValueUsingMapID (IN CHAR16 *MappingString, OUT void **Buffer, OUT UINTN *BufferSize, OUT UINT8 *ControlType)
{
    EFI_STATUS  Status = EFI_SUCCESS;
    UINT16      MappingStringHandle;
    UINT16      BiosAttributeHandle;
    UINTN       ValueSize = 0;
    char        *ValueString = (char *)NULL;
    
    //Check whether Pldm tables are created, if not create the tables
    if( FALSE == gPldmAvailable )
    {
        Status = InitPldmTables();
        if(EFI_SUCCESS == Status)
        {
            gPldmAvailable = TRUE;
        }
        else
        {
           return Status;
        }
    }
    
    Status = FindMappingStringHandle (MappingString, &MappingStringHandle, FALSE);
    if (EFI_ERROR (Status))
    {
        return Status;
    }
    
    Status = FindMappingAttributeHandle (MappingStringHandle, &BiosAttributeHandle, ControlType);
    if (EFI_ERROR (Status))
    {
        return Status;
    }
    Status = GetValueFromValueTable (MappingString, BiosAttributeHandle, &ValueString, &ValueSize);
    if (EFI_ERROR (Status))
    {
        return Status;
    }
    *Buffer = ValueString;
    *BufferSize = ValueSize;
    
    DEBUG((DEBUG_ERROR, "GetValueUsingMapID Status:%x\r\n", Status));
    return Status;
}

/**
    Set the value for mapping string

    @param IN CHAR16 * -> Mapping string 
    @param IN void ** -> Buffer to set
    @param IN UINTN -> Size of the buffer

    @retval EFI_STATUS

**/
EFI_STATUS SetValueUsingMapID (IN CHAR16 *MappingString, IN void *Buffer, IN UINTN BufferSize)
{
    EFI_STATUS  Status = EFI_SUCCESS;
    UINT16      MappingStringHandle = 0;
    UINT16      BiosAttributeHandle = 0;
    UINT8 *StringTable = NULL;
    UINT8 *AttributeTable = NULL;
    UINT8 *ValueTable = NULL;
    UINT8 *MapStringTable = NULL;
    UINT32 StringSize = 0;
    UINT32 AttributeSize = 0;
    UINT32 ValueSize = 0;
    UINT32 MapStringSize = 0;
    UINT32  Crc32;
    UINTN   IntegerValue = 0;
    UINT8   ControlType = BIOSEnumeration;

    gSetValueStatus = EFI_SUCCESS;

    if (NULL == gRbHiiData)
    {
        Status = AMI_RetrieveHIIData (&gRbHiiData, &gRbHiiSize, &Crc32);
    
        if (EFI_ERROR (Status))
        {
            return Status;
        }
    }
    
    //Check whether Pldm tables are created, if not create the tables
    if( FALSE == gPldmAvailable )
    {
        Status = InitPldmTables();
        if(EFI_SUCCESS == Status)
        {
            gPldmAvailable = TRUE;
        }
        else
        {
            return Status;
        }
    }
        
    //Find the mapping string handle for the Mapping String 
    Status = FindMappingStringHandle (MappingString, &MappingStringHandle, TRUE);
    if (EFI_ERROR (Status))
    {
        return Status;
    }
    // Create string Table for the control
    MemCreat(&StringBuffer);
    Status = AddStringByHandle(MappingStringHandle);
    if (EFI_ERROR (Status))
    {
        return Status;
    }
    //Find the attribute handle for the MappingStringHandle 
    Status = FindMappingAttributeHandle (MappingStringHandle, &BiosAttributeHandle, &ControlType);
    if (EFI_ERROR (Status))
    {
        return Status;
    }
    //Get the value for the attribute handle
    Status = FindValueByAttribHandle (gValueTable, gValueSize, BiosAttributeHandle);
    if( Status != EFI_SUCCESS)
    {
    	return Status;
    }
    
    switch (gControlAttribute->Type)
    {
        case BIOSEnumeration:
        {
            //Add the all possible strings to the string Table
            Status = AddStringsForEnum (MappingStringHandle);
            if (EFI_ERROR (Status))
            {
                return Status;
            }
            break;
        }
        default:
            break;
    }
    
    AddCrc(&StringBuffer);
    StringTable = StringBuffer.Buffer;
    StringSize = StringBuffer.FileSize;
    
    switch (gControlAttribute->Type)
    {
        case BIOSInteger:
        {
            if(TRUE == gRedfishEnabled)
            {
                if(0 == CompareMem(Buffer, "true", StrLen(L"true")))
                    IntegerValue = 1;
                else if(0 == CompareMem(Buffer, "false", StrLen(L"false")))
                    IntegerValue = 0;
                else
                    IntegerValue = atoi (Buffer);
            }
            else
                IntegerValue = atoi (Buffer);
            gControlValue.Numeric->Value = IntegerValue;
            break;
        }
        case BIOSEnumeration:
        {
            if(TRUE == gRedfishEnabled)
            {
                BOOLEAN OptionFound = FALSE;
                UINT16 *EnumString = NULL;

                if(NULL != Buffer)
                {
                    ATTRIBUTE_POINTER EnumAttribute;
                    int i;
                    PLDM_STRING          *String;
                    
                    EnumString = AllocateZeroPool(AsciiStrLen((CHAR8*)Buffer));
                
                    AsciiStrToUnicodeStr((CHAR8*)Buffer, EnumString);
                    
                    EnumAttribute.CommonHeader = (PLDM_ATTRIBUTE *)gControlAttribute;
                    for(i=0; i < EnumAttribute.Enumeration->PossibleValues; i++)
                    {
                        String = StringByHandle(StringTable, StringSize, EnumAttribute.Enumeration->Handle[i]);

                        if (0 == String)
                        {
                            continue;
                        }

                        if (String->Length < wcslen16(EnumString))
                        {
                            continue;
                        }

                        if (0 != memcmp(String->String, EnumString, wcslen16(EnumString)))
                        {
                            continue;
                        }
                        
                        OptionFound = TRUE;
                        IntegerValue = i;

                        break;
                    }
                    
                    if(OptionFound == FALSE)
                    {
                        gSetValueStatus = EFI_ABORTED;
                        return Status;
                    }
                }
                else
                {
                    gSetValueStatus = EFI_ABORTED;
                    return EFI_INVALID_PARAMETER;
                }
            }
            else
            {
                IntegerValue = atoi (Buffer);
            }
            
            gControlValue.Enumeration->ValueIndex [0] = (UINT8)IntegerValue;
            gControlValue.Enumeration->NumberOfValues = 1;
            break;
        }
        default:
            break;
    }
    //Update the ControlValue Buffer
    gControlWriteValue = (PLDM_VALUE *)gControlValue.CommonHeader;
    
    // output attribute table
    MemCreat(&AttribBuffer);
    MemWrite(&AttribBuffer, gControlAttribute, GetAttributeLength(gControlAttribute));
     
    // output the value table
    MemCreat(&ValueBuffer);
    
    if(gControlAttribute->Type == BIOSString)
    {
    	CHAR16 *StrBuffer = NULL;
		StrBuffer = StrDup8to16 (Buffer);
		BufferSize = StrLen(StrBuffer)*sizeof(CHAR16);
		
		gControlValue.String->Length = (UINT16)BufferSize;
	    MemWrite( &ValueBuffer, gControlWriteValue, (UINT32)(sizeof(*gControlValue.String)) );
	    MemWrite( &ValueBuffer, StrBuffer, (UINT32)BufferSize );
    }
    else if( (gControlAttribute->Type == BIOSPassword) ||  (gControlAttribute->Type == BIOSPasswordReadOnly) )
    {
        memcpy (gPasswordCtrlValue.PassWord->PassWord, Buffer, BufferSize); 
		gControlValue.PassWord->Length = (UINT16)BufferSize;
    	MemWrite( &ValueBuffer, gControlWriteValue, (UINT32)(sizeof (* gControlValue.PassWord)) );
    	MemWrite(&ValueBuffer, Buffer,(UINT32)BufferSize);
    }
    else
    {
    	MemWrite(&ValueBuffer, gControlWriteValue, GetValueLength(gControlWriteValue));
    }
    
    
    /******Writer the Table to the Buffer************/
    // add crc32 and pad
    
    AddCrc(&MapBuffer);
    MapStringTable = MapBuffer.Buffer;
    MapStringSize = MapBuffer.FileSize;
    
    AddCrc(&StringBuffer);
    StringTable = StringBuffer.Buffer;
    StringSize = StringBuffer.FileSize;

    AddCrc(&ValueBuffer);
    ValueTable = ValueBuffer.Buffer;
    ValueSize = ValueBuffer.FileSize;

    AddCrc(&AttribBuffer);
    AttributeTable = AttribBuffer.Buffer;
    AttributeSize = AttribBuffer.FileSize;

    gSetUsingMapID = TRUE;
    Status = ImportMappedPldmValues(
                            gRbHiiData,
                            (UINT32)gRbHiiSize,
                             ValueTable,
                             ValueSize,
                             AttributeTable, // attribute table from foreign system
                             AttributeSize,
                             MapStringTable, // x-UEFI-AMI string table from foreign system
                             MapStringSize,
                             StringTable, // english string table from foreign system
                             StringSize
                         );
    
    gSetUsingMapID = FALSE;
    DEBUG((DEBUG_ERROR, "SetValueUsingMapID Status:%x\r\n", Status));
    
    //Free the allocated buffer
    if (MemClose(&MapBuffer))
	{
		return EFI_OUT_OF_RESOURCES;
	}
	MapBuffer.Buffer = 0;
	
	if (MemClose(&ValueBuffer))
	{
		return EFI_OUT_OF_RESOURCES;
	}
	ValueBuffer.Buffer = 0;

	if (MemClose(&StringBuffer))
	{
		return EFI_OUT_OF_RESOURCES;
	}
	StringBuffer.Buffer = 0;

	if (MemClose(&AttribBuffer))
	{
		return EFI_OUT_OF_RESOURCES;
	}
	AttribBuffer.Buffer = 0;
	
    if(gControlValue.CommonHeader)
    {
    	free(gControlValue.CommonHeader);
    	gControlValue.CommonHeader = NULL;
    	gControlWriteValue = NULL;
    }

    if(EFI_ERROR(Status) || EFI_ERROR(gSetValueStatus))
        return (Status|gSetValueStatus);
    else
        return Status;
}

/**
    Gets the MapString control list by parsing each control in Hii

    @param IN UINT32 PackIndex -> Package Index
  
    @retval EFI_STATUS

**/
EFI_STATUS GetMapIdBuffer ( IN UINT32 PackIndex )
{
    UINT8 *CurrentFormSet = 0;
    UINT8 *CurrentForm = 0;
    UINT16 FormSetTitle = 0;
    UINT8* CurrentPackageList = 0;
    UINT8 *CurrentControl = 0;
    UINT16 Class = 0;
    UINT16 SubClass = 0;
    UINT16 FormSetHelp;
    UINT16 FormID = 0;
    UINT16 FormTitle = 0;
    GUID Guid = {0};
    EFI_STATUS Status = EFI_SUCCESS;
    struct InternalTables *g = &Globals;
    CONTROL_INFO *Control;
    CONTROL_INFO NewControl;
    struct PackInfo *Pack = g->Packs + PackIndex;
    PLDM_MAPIDINFO MapIdData = {0};//strucure to hold the mapstring data
    // dummy amisce structure for GetNextUefiControl()
    HII_DATA AmiSceData;
    
    memset(&AmiSceData, 0, sizeof(AmiSceData));
    AmiSceData.HIIDBData = g->HIIDBData;
    CurrentPackageList = g->Packs[PackIndex].Package;
    CurrentFormSet = GetNextUefiFormSet( CurrentPackageList, &CurrentFormSet, ( UINT8 * )&Guid,
                                         &FormSetTitle, &FormSetHelp, &Class, &SubClass );

    // check for class does not work for every form set
    if (CurrentFormSet == NULL /* || Class == 0*/)
    {
        return EFI_NOT_FOUND;
    }

    for (CurrentForm = 0; (CurrentForm = GetNextUefiForm( CurrentFormSet, &CurrentForm, &FormID, &FormTitle)); )
    {
        // each form in formset
        for (Control = 0; (Control = GetNextUefiControl( CurrentForm, &CurrentControl, &AmiSceData, CurrentPackageList, &NewControl)); )
        {
            wchar_t *Prompt = NULL;
            
            // each control in form skip controls the 401 form with blank name -- it will create ambiguous map strings.
            if (FormID == 0x401 && FormTitle == 2)
            {
                FreeControl(Control);
                continue;
            }
            
            if (IsNeededControl(Control) == FALSE)
            {
                FreeControl(Control);
                continue;
            }
            
            //Get the mapping string for the control
            Prompt = PldmGetHiiString(Pack->Package, g->HIIDBData, Control->ControlPrompt, TRUE);
            if(NULL != Prompt)
            {
				MapIdData.ControlType = Control->ControlType;
				MapIdData.Length = (UINT16)(sizeof(UINT16) * wcslen16((UINT16 *)Prompt));
				
				//Write to the buffer
				MemWrite(&MapIdBuffer, &MapIdData, sizeof(MapIdData));
				MemWrite(&MapIdBuffer, Prompt, MapIdData.Length);
				
				free(Prompt);                    
            }
            
            FreeControl(Control);
        }
    }
    

    return Status;
}

/**
    Get the total mapping language string list present in the Hii

    @param OUT UINT8 ** -> MapIdList returns the mapping string list
    @param OUT UINT32 * -> MapIdListSize

    @retval EFI_STATUS

**/
EFI_STATUS GetMapStringList ( OUT UINT8 **MapIdList, OUT UINT32 *MapIdListSize ) 
{
    struct InternalTables *g = AddressGlobals();
    UINT8 *CurrentPackageList = NULL;
    UINT8 *StringPack;
    int PackageCount = 0;
    EFI_STATUS Status = EFI_NOT_FOUND;
    HII_DATA AmisceData = {0};
    UINT32 CRC32 = 0;
    UINTN DBSize = 0;
    int i;
    UINTN j;
    CHAR8 **MappingLangArr = (CHAR8 **)NULL; 
    UINTN MappingLangCount = 0;
    BOOLEAN StringPackFound = FALSE;

	//Check the gMD structure and Initialize it if gMD is NULL
	if(NULL == gMD)
	{
		InitializePldmLibrary();
		DEBUG((DEBUG_ERROR, "\nCreatePldmTables InitializePldmLibrary Done"));
	}
    	
    //Retrieve the Hii data
    Status = AMI_RetrieveHIIData( &AmisceData.HIIDBData, &DBSize, &CRC32 );
    
    if (Status != EFI_SUCCESS)
    {
        DEBUG((DEBUG_ERROR, "\n Cannot access HII data !!\n"));
        return Status;
    }
        
    g->HIIDBData = AmisceData.HIIDBData;
    gHiiDataSize = (UINT32)DBSize;
    FindUefiVersion( AmisceData.HIIDBData );
    
    PackageCount = GetTotalPackageList( g->HIIDBData );
    
    g->nPacks = 0;
    GetHiiHandles();
    // for each package with IFR
    DEBUG((DEBUG_ERROR, "\n Total PackageCount: %d", PackageCount));

    MappingLangCount = GetPLDMMappingCount();
    MappingLangArr = GetPLDMMappingLanguageID(); 
	   
    MemCreat(&MapIdBuffer);

    for (i = 0; i < PackageCount; i++)
    {
    
        EFI_STATUS Status;
        struct PackInfo *Pack;
        
        CurrentPackageList = GetNextPackageList(g->HIIDBData, &CurrentPackageList);
        if (0 == FindIfrPack(CurrentPackageList))
        {
            continue;    // no controls
        }
        
        if (0 == (StringPack = FindStringPack(CurrentPackageList)))
        {
            continue;    // no english strings
        }
        
        gCurrentHiiHandle = gHiiHandles[i];
        
        //Get the current package info
        Pack = &g->Packs[g->nPacks];
        Pack->Package = CurrentPackageList;
        
        //Get the number of controls available in the package
        Pack->NumControls = CountControls(g->nPacks);
        if (Pack->NumControls == 0)
        {
            continue;    // no good controls
        }
        
        StringPackFound = FALSE;
        for ( j = 0; j < MappingLangCount; j++)
        {
            if ((StringPack = FindLangStringPack(CurrentPackageList, MappingLangArr[j])))
            {
                StringPackFound = TRUE;
                break;
            }
        }
       
        if( StringPackFound == FALSE)
        {
            continue;    // no Map strings
        }
        
        //Get the MapId list in the package
        Status = GetMapIdBuffer ( g->nPacks );
        if(Status != EFI_SUCCESS)
        {
            DEBUG((DEBUG_ERROR, "GetMapStringList GetMapIdBuffer Status:%x\r\n",Status));
            return Status;
        }
        
        _FreeVariableList();
        g->nPacks++;
    }
   
    if(Status == EFI_SUCCESS)
    {
       
       AddCrc(&MapIdBuffer);

       *MapIdList = MapIdBuffer.Buffer;
       *MapIdListSize = MapIdBuffer.FileSize;
       
       MapIdBuffer.Buffer = 0;
       if (MemClose(&MapIdBuffer))
       {
           DEBUG((DEBUG_ERROR, "GetMapStringList Memory exhaustion\r\n"));
       }
    }
    
    FreeHiiHandles();
    memset(g, 0, sizeof(*g));
    
    return Status;
}

/**
    Set the preserve mode, full setup datya import or changed value import

    @param IN UINTN -> preserve mode

    @retval void

**/
void SetPreserveMode(UINTN Mode)
{
    gPreserveMode = Mode ;
}
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2017, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**    5555 Oakbrook Pkwy, Norcross, Suite 200, Georgia 30093   **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
