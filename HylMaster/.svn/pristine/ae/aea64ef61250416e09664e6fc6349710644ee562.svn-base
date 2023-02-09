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

/** @file JsonHiiLibSrc.c

**/

//---------------------------------------------------------------------------
#define HII_DBG
#ifdef HII_DBG
#define _DBG_(x) x
#else
#define _DBG_
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
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
#include "JsonHiiLibSrc.h"
#include "JsonHiiLibraryApi.h"
#include "EDKStructures21.h"
#include "AdvScript.h"
#include "CRC32.h"
#include "AMIHIILibrary.h"
#include <assert.h>
#include "Library/AmiJsonLib.h"
#include "JsonHiiLibElinks.h"
#include <Library/DebugLib.h>
#include "ConditionalExpressionApi.h"

#ifdef _LNX
#define wcslen16 _Wstrlen
extern size_t _Wstrlen(UINT16 *);
extern VOID _Wnstrcpy(unsigned short *NarrowName, wchar_t *WideName);
extern VOID _Nwstrcpy(wchar_t *WideName, unsigned short *NarrowName);
#else
#define wcslen16 _Wstrlen
#endif

#ifdef _LNX
#define EFI_NOT_FOUND                  (14 | EFI_ERROR_BIT) // 0xe
#define EFI_ABORTED                    (21 | EFI_ERROR_BIT) // 0x15
#define EFI_OUT_OF_RESOURCES           (9  | EFI_ERROR_BIT)
#define EFI_INVALID_PARAMETER          (2  | EFI_ERROR_BIT)
#define EFI_CRC_ERROR                  (27  | EFI_ERROR_BIT)
#define EFI_SUCCESS                     0
#endif
#include <Protocol/HiiConfigRouting.h>
#include <Protocol/HiiDatabase.h>

EFI_HII_CONFIG_ROUTING_PROTOCOL *gHiiConfigRouting;
extern EFI_HII_DATABASE_PROTOCOL *gHiiDatabaseProtocol;
#define IS_UNSIGNED(Type,Min,Max) ( ((Type)Min) < ((Type)Max) )
#define INT64_MAX_LIMIT 0x7FFFFFFFFFFFFFFF
#define INT64_MAX_MASK_BIT 0x8000000000000000

const CHAR8 *gSupportedMapIdList[] = {SUPPORTED_MAPPING_ID_LIST ""};
EFI_GUID gFormSetClassGuidListEntry[] = {EMA_FILTER_CLASSGUID_FORMSETS NULL_GUID};
int gMapIdCount = sizeof(gSupportedMapIdList)/sizeof(const CHAR8 *) - 1;
#define EFI_IFR_NUMERIC_OP 0x07 // defined the macro here, as including EfiCommon.h leads to lot of redifinition errors
#define EFI_IFR_STRING_OP 0x1C
#define EFI_IFR_CHECKBOX_OP 0x06

extern struct  __MainData  *gMD;

extern VARIABLE *gVariable ;
extern EFI_BOOT_SERVICES *gBS;
extern EFI_SYSTEM_TABLE *gST;
extern EFI_RUNTIME_SERVICES *gRT;

extern BOOLEAN gUseCache;
extern VOID _Nwstrcpy(wchar_t *WideName, unsigned short *NarrowName);
extern void  _FreeVariableList();   // in  AdvScript

static void GetHiiHandles();
static void FreeHiiHandles();

CONTROL_RESET_FLAGINFO *gResetControlInfo;

HII_DATA AmisceData;
EFI_HII_HANDLE *gHiiHandles = 0;
UINT32 gHiiNumHandles = 0;
EFI_HII_HANDLE *gFilterHandles = 0;
UINT32 gFilterCount = 0;
UINT32 gVariablesAdded = 0;

// when not null, tell addvariable to use configuration protocol
EFI_HII_HANDLE gCurrentHiiHandle = 0;

// forward declarations
static BOOLEAN gHiiGlobalsInit = FALSE;
extern UINT64 gHiiDataSize;
extern UINT8 *gHiiDBData;
HiiGlobals Globals;
extern DATAVECTOR gHiiBiosSetupBuf;
cJSON *gJsonImportObject;
BOOLEAN gHiiSetValue = FALSE;
UINT16 *gSetMapString = (UINT16 *)NULL;
void *gSetMapIdBuffer = NULL;
UINTN gSetMapIdSize = 0;
EFI_STATUS gSetValueStatus = EFI_UNSUPPORTED;
extern CHAR8 *gFailMapIdBuffer;
extern UINTN gFailMapIdAllocBuffSize ;
extern UINTN  gFailMapIdBuffSize ;

static void
FreeControl(CONTROL_INFO *Control);
CHAR16  *gBootOptionMapId;    

/**
    Duplicates a String from CHAR16 to CHAR8

    @param CHAR16* string 

    @retval CHAR8 *


**/
CHAR8* HiiLib_ConvertChar16toChar8(CHAR16 *String)
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
    Duplicates a String from CHAR8 to CHAR16

    @param CHAR8* string 

    @retval CHAR16 *


**/
CHAR16 *HiiLib_ConvertChar8toChar16( CHAR8 *string )
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

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    FindIfrPack
//
// Description:  Find IFR pack (if any) within an export package
//
// Parameters:   UINT8 *
//                  [in] Address of export package
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
// Procedure:   InitFirstUefiString
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
// Procedure:   GetNextUefiString
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
                Context->StringOffset = (int)((wcslen16((UINT16 *)(Buffer + Context->StringOffset)) + 1) * sizeof(UINT16));
            }

            // last string treatment
            if (Context->SkipCount == 0)
            {
                Context->BlockOffset = (int)((wcslen16((UINT16 *)(Buffer + Context->StringOffset)) + 1) * sizeof(UINT16));
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
// Procedure:    FindStringPack
//
// Description:  Find English string pack for formset
//
// Parameters:   UINT8 *
//                  [in] export package address
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
// Procedure:    Hii_GetStringByHandleIndex
//
// Description:  Get string based on Handleindex in string pack
//
// Parameters:   struct PackInfo *Pack
//               UINT16 HandleIndex
//
// Return: String on Success
//               NULL if not found
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT16 *
Hii_GetStringByHandleIndex( struct PackInfo *Pack, UINT16 HandleIndex )
{
    UINT16 *String = (UINT16 *)NULL;
    int j = 0;
    
    for (j = 0; j < Pack->NumStrings; j++)
    {
        if ( HandleIndex == j )
        {
                    
		if (Pack->Strings[j].String == NULL) {
			continue;
		}
            String = AllocateZeroPool( sizeof(UINT16) * (wcslen16(Pack->Strings[j].String) +1) );
            if( NULL != String)
            	Wcscpy( String, Pack->Strings[j].String );
            break;
        }
    }
    
    return String;
}
/**
    To compare GUID.

    @param Guid1 IN EFI_GUID *Guid1
    
    @param Guid2 IN EFI_GUID *Guid2

    @retval BOOLEAN. Return TRUE GUID1 matches with GUID2 else FALSE

**/
BOOLEAN
EfiCompareGuid (
  IN EFI_GUID *Guid1,
  IN EFI_GUID *Guid2
  )
{
    return (BOOLEAN)(0==MemCmp((UINT8*)Guid1,(UINT8*)Guid2,sizeof(EFI_GUID)));
}
/**
    This function filter formset and display formset based on ClassGUID mentioned in elink AMITSE_FILTER_CLASSGUID_FORMSETS 

    @param FormSet. Formset to compare with elink classGuid.

    @retval BOOLEAN. Return TRUE formset matches with elink ClassGuid else FALSE

**/
BOOLEAN ShowClassGuidFormsets (EFI_IFR_FORM_SET *FormSet )
{
    UINT16 FilterListcount = sizeof(gFormSetClassGuidListEntry) / sizeof (EFI_GUID);

    UINT16 index = 0;
    EFI_GUID*  ClassGuid = (EFI_GUID *)NULL ;

    ClassGuid    = (EFI_GUID *)(VOID *)((UINT8 *)FormSet + sizeof (EFI_IFR_FORM_SET));
    if (ClassGuid)
    {
        for (index = 0; index < (FilterListcount-1); index++)//(FilterListcount-1) To avoid check with NULL guid
        {
            if ( EfiCompareGuid ( ClassGuid, &gFormSetClassGuidListEntry[index]) )
            {
                return TRUE;
            }
        }
        return FALSE;
    }
    return TRUE;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    TraverseControls
//
// Description:  Traverse formset calling provided function for each control.
//               Constructs a fake CONTROL_INFO for each form and formset to
//               implement form and formset PLDM collections.
//               Skips over the 0x401 form with duplicate controls.
//
// Parameters:   int
//                  [in] Index to pack info structure in globals
//               Pointer to function
//                  function to call for each control
//               void *
//                  Additonal pointer to pass to called function
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
    AmiSceData.HIIDBData = gHiiDBData;
    CurrentPackageList = g->Packs[PackIndex].Package;
    CurrentFormSet = GetNextUefiFormSet( CurrentPackageList, &CurrentFormSet, ( UINT8 * )&Guid,
                                         &FormSetTitle, &FormSetHelp, &Class, &SubClass );

    // check for class does not work for every form set
    if (CurrentFormSet == NULL /* || Class == 0*/)
    {
        return EFI_NOT_FOUND;
    }
    if ( !ShowClassGuidFormsets( (EFI_IFR_FORM_SET*)CurrentFormSet) ) //If ClassGuid is matches then continue parsing to display formset
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
            
            //Check For Control Value set status
            if ( (TRUE == gHiiSetValue) && (EFI_SUCCESS == gSetValueStatus) )
                return Status;

        }
    }

    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    FreeControl
//
// Description:  free a control's options
//
// Parameters:   CONTROL_INFO *
//                  [in] Address of control info struct
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
// Procedure:    IsNeededControl
//
// Description:  Check a control to see if it is needed for PLDM
//
// Parameters:   CONTROL_INFO *
//                  [in] Address of control info struct
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
                return FALSE;
            case LEGACY_DEV_ORDER_CONTROL:              
                return FALSE;

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
            return TRUE;
        case EFI_IFR_PASSWORD_OP:
            return TRUE;
        case EFI_IFR_ORDERED_LIST_OP:
                   return FALSE;
    }

    return FALSE;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    CountFunc
//
// Description:  Control counter structure - called by TraverseControls()
//
// Parameters:   int
//                  [in] not used
//               CONTROL_INFO *
//                  [in] control to possibly count
//               void *
//                  [in out] pointer to integer counter
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
// Procedure:    CountControls
//
// Description:  Count number of controls in formst of interest to PLDM
//
// Parameters:   int
//                  [in] Index to PackInfo struct
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
// Procedure:    AddCrc
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
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    AddressGlobals
//
// Description:  Get the address of HII global data
//
// Parameters:   none
//
// Return: Struct InternalTables * - Address of HII global data
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static
struct InternalTables *AddressGlobals()
{
    return &Globals;
}

// given a control, create a HII attribute table entry
static
HII_ATTRIBUTE // need a way to return an error?
*MakeAttribute(CONTROL_INFO *Control, int PackIndex)
{
    MEMFILE Buffer;
    struct InternalTables *g = AddressGlobals();
    struct PackInfo *Pack = &g->Packs[PackIndex];
    HII_ATTRIBUTE Header;
    HII_ATTRIBUTE *ReturnValue;
    BOOLEAN bStdDef = 0; //Will be made 1 if the control has std default
    BOOLEAN bMfgDef = 0; //Will be made 1 if the control has mfg default


    if (IsOEMSpecialControl(Control))
    {
	DEBUG((DEBUG_ERROR, "\nMake Special"));
        return 0;//MakeSpecialAttribute(Control, PackIndex);
    }
#if SUPPRESS_COND_EXPRESSION_CONTROLS
    if(( (EFI_IFR_OP_HEADER*)Control->ConditionalPtr) != NULL )
    if(( (EFI_IFR_OP_HEADER*)Control->ConditionalPtr)->OpCode == EFI_IFR_SUPPRESS_IF_OP
             || ( (EFI_IFR_OP_HEADER*)Control->ConditionalPtr)->OpCode == EFI_IFR_NO_SUBMIT_IF_OP
             || ( (EFI_IFR_OP_HEADER*)Control->ConditionalPtr)->OpCode == EFI_IFR_INCONSISTENT_IF_OP
             || ( (EFI_IFR_OP_HEADER*)Control->ConditionalPtr)->OpCode == EFI_IFR_GRAY_OUT_IF_OP
             || ( (EFI_IFR_OP_HEADER*)Control->ConditionalPtr)->OpCode == EFI_IFR_DISABLE_IF_OP
             || ( (EFI_IFR_OP_HEADER*)Control->ConditionalPtr)->OpCode == EFI_IFR_WARNING_IF_OP )
    {
        return 0;
    }
#endif
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
        p.CommonHeader = (HII_ATTRIBUTE *)malloc(sizeof(*p.Numeric));

        if (0 == p.CommonHeader)
        {
            DEBUG((DEBUG_ERROR, "\nMakeAttribute: Memory exhausted"));
            return 0;
        }

        Header.Type = (UINT8)Control->ControlType;
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
#if 0   
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
        p.CommonHeader = (HII_ATTRIBUTE *)malloc(sizeof(*p.String));

        if (0 == p.CommonHeader)
        {
        	DEBUG((DEBUG_ERROR, "\nMakeAttribute: Memory exhausted"));
            return 0;
        }

        Header.Type = (UINT8)Control->ControlType;
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
        Header.Type = (UINT8)Control->ControlType;
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
        Header.Type = (UINT8)Control->ControlType;
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
        p.CommonHeader = (HII_ATTRIBUTE *)malloc(sizeof(*p.PassWord));

        if (0 == p.CommonHeader)
        {
            DEBUG((DEBUG_ERROR, "\nMakeAttribute: Memory exhausted"));
            return 0;
        }

        Header.Type = (UINT8)Control->ControlType;
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

    ReturnValue = (HII_ATTRIBUTE *)Buffer.Buffer;
    Buffer.Buffer = 0;

    if (MemClose(&Buffer))
    {
	DEBUG((DEBUG_ERROR, "\nMakeAttribute: Memory exhausted"));
        return 0;
    }

    return ReturnValue;
}


// given a control, create a HII value table entry.
static
HII_VALUE
*MakeValue(CONTROL_INFO *Control, ATTRIBUTE_POINTER Attrib, int PackIndex)
{
    MEMFILE Buffer;
    HII_VALUE Header = {0};
    UINT8 *CurrentValue = (UINT8 *)NULL;
    UINT64 Value;
    VALUE_POINTER p;


    if (IsOEMSpecialControl(Control))
    {
        return 0;//MakeSpecialValue(Control, Attrib, PackIndex);
    }

    if (Control->ControlType == EFI_IFR_NUMERIC_OP || Control->ControlType == EFI_IFR_CHECKBOX_OP)
    {
        // numeric or checkbox
        p.CommonHeader = (HII_VALUE *)malloc(sizeof(*p.Numeric));

        if (0 == p.CommonHeader)
        {
            DEBUG((DEBUG_ERROR, "\nMakeValue: Memory exhausted"));
            return 0;
        }

        Header.Type = (UINT8)Control->ControlType;
        Header.Handle = 0;      // caller assigns
        *p.CommonHeader = Header;
        // Get the Control Value
        CurrentValue = SceGetQuestionValue( (UINT16)Control->VariableStorage, (UINT16)Control->DataOffset, (UINT16)Control->DataWidth );
        p.Numeric->Base = Hii_UefiGetBaseValue(Control->ControlPtr); 
        p.Numeric->Datawidth = Control->DataWidth;
        p.Numeric->Value = GetNumericValueFromBuffer( CurrentValue, Control->DataWidth, p.Numeric->Base );
        free(CurrentValue);
        return p.CommonHeader;
    }
    else if (Control->ControlType == EFI_IFR_DATE_OP || Control->ControlType == EFI_IFR_TIME_OP)
    {
        // Date or Time
        p.CommonHeader = (HII_VALUE *)malloc(sizeof(*p.Numeric));

        if (0 == p.CommonHeader)
        {
            DEBUG((DEBUG_ERROR, "\nMakeValue: Memory exhausted"));           
            return 0;
        }

        Header.Type = (UINT8)Control->ControlType;
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
        p.CommonHeader = (HII_VALUE *)malloc(sizeof(*p.Enumeration) + sizeof(UINT8));

        if (0 == p.CommonHeader)
        {
            DEBUG((DEBUG_ERROR, "\nMakeValue: Memory exhausted"));
            return 0;
        }

        Header.Type = (UINT8)Control->ControlType;
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

        p.CommonHeader = (HII_VALUE *)malloc(sizeof(*p.Enumeration) + (NoOfOptions * sizeof(UINT8)));

        if (0 == p.CommonHeader)
        {
            DEBUG((DEBUG_ERROR, "\nMakeValue: Memory exhausted"));
            return 0;
        }

        Header.Type = (UINT8)Control->ControlType;
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
        Header.Type = (UINT8)Control->ControlType;
        p.CommonHeader = (HII_VALUE *)malloc(sizeof * p.String);

        if (0 == p.CommonHeader)
        {
            DEBUG((DEBUG_ERROR, "\nMakeValue: Memory exhausted"));           
            return 0;
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
        p.CommonHeader = (HII_VALUE *)Buffer.Buffer;
        Buffer.Buffer = 0;

        if (MemClose(&Buffer))
        {
            DEBUG((DEBUG_ERROR, "\nMakeValue: Memory exhausted"));
            return 0;
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
        Header.Type = (UINT8)Control->ControlType;
        v.CommonHeader = (HII_VALUE *)malloc(sizeof * v.PassWord);

        if (0 == v.CommonHeader)
        {
            DEBUG((DEBUG_ERROR, "\nMakeValue: Memory exhausted"));
            return 0;
        }

        *v.CommonHeader = Header;

        v.PassWord->Length = Control->DataWidth;
        MemWrite(&Buffer, v.PassWord, sizeof(*v.PassWord));
        MemWrite(&Buffer, PassWord, v.PassWord->Length);
        free(PassWord);
        v.CommonHeader = (HII_VALUE *)Buffer.Buffer;
        Buffer.Buffer = 0;

        if (MemClose(&Buffer))
        {
            DEBUG((DEBUG_ERROR, "\nMakeValue: Memory exhausted"));
            return 0;
        }

        return v.CommonHeader;
    }

    return NULL;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    GetPackControls
//
// Description:  Fill in CONTROL structure and add to list.
//               Called from TraverseControls().
//               Calls itself for each additional legacy boot sublist.
//
// Parameters:
//               int
//                  [in] Index to PackInfo struct
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
    UINT16 *MapIdName = (UINT16 *)NULL;
    
    //check for MapStringname and Mark variable as having at least one mapped control 
    MapIdName = HiiLib_GetHiiString(Pack->Package, gHiiDBData, Control->ControlPrompt, TRUE);
    if (MapIdName && MapIdName[0])
    {
        // Mark variable as having at least one mapped control
        MarkVarReferenced(Control->VariableStorage);
    }
                
    if (FALSE == IsNeededControl(Control))
    {
        FreeControl(Control);
        if(MapIdName)
            free(MapIdName);
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
            if(MapIdName)
                free(MapIdName);
            return EFI_SUCCESS;
        }

        Pack->Controls[Count].Value->Handle = Count + Pack->FirstControlHandle;
        *(int *)pCount += 1;
        
        //Update String and MapStrig details
        //Get the mapping string for the control
        Pack->Controls[Count].MapString = MapIdName;
        Pack->Controls[Count].String = HiiLib_GetHiiString(Pack->Package, gHiiDBData, Control->ControlPrompt, FALSE);
        
    }
    else
    {//if attribute construct failed, just increment the control count
	DEBUG((DEBUG_ERROR, "\n GetPack Attribute is zero"));
        if(MapIdName)
            free(MapIdName);
        *(int *)pCount += 1;
    }

    // free the control info
    FreeControl(Control);

    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    BuildInternalTables
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
    int j;
    UINT8 *CurrentPackageList = NULL;
    UINT8 *StringPack;
    int LastStringHandle = 0;
    int LastControlHandle = 0;
    int Count = 0;
    int PackageCount = 0;
    int GoodPackages = 0;

    PackageCount = GetTotalPackageList( gHiiDBData );
    GoodPackages = 0;

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

        CurrentPackageList = GetNextPackageList(gHiiDBData, &CurrentPackageList);

        //<kuaibai-20190218 Fix hang92 when execute OOB cycles in some cases +>
        if (i >= MAX_PACKAGES)
        {
            DEBUG((DEBUG_ERROR, "\n[SK] Number of packages exceeds the limit, please increase the MAX_PACKAGES macro"));
            ASSERT(0);
            return EFI_NOT_FOUND;
        }
        //<kuaibai-20190218 Fix hang92 when execute OOB cycles in some cases ->

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
        Pack->MapStrings = NULL;
        Pack->VariableList = NULL;
        
        if (Pack->NumControls == 0)
        {
            continue;    // no good controls
        }

        GoodPackages++;
        Pack->HiiHandle = gCurrentHiiHandle; // for later access to Hii handle
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
            DEBUG((DEBUG_ERROR, "\nBuildInternalTables: Memory exhausted"));
            return EFI_NOT_FOUND;
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
        //Parse the MapLangcodes and get the string pack
        for (j = 0; j < gMapIdCount; j++) 
        {
            if ((StringPack = FindLangStringPack(CurrentPackageList, gSupportedMapIdList[j])))
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

        // build list of controls of interest for this pack
        LastControlHandle = Pack->FirstControlHandle + Pack->NumControls;

        // create control table
        Pack->Controls = (struct Control *)calloc(Pack->NumControls, sizeof(Pack->Controls[0]));

        if (NULL == Pack->Controls)
        {
            DEBUG((DEBUG_ERROR, "\nBuildInternalTables: Memory exhausted"));
            return EFI_NOT_FOUND;
        }

        Count = 0;
        TraverseControls(g->nPacks, GetPackControls, (void *)&Count);

        //_FreeVariableList();
        Pack->VariableList = gVariable;
        gVariable = NULL;
        g->nPacks++;
    }

    if (gpStdBuf)
    {
        FreePool(gpStdBuf); 
        gpStdBuf = NULL;
    }

    if (gpMfgBuf)
    {
        FreePool(gpMfgBuf); 
        gpMfgBuf = NULL;
    }

    FreeHiiHandles();
    
    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    GetOptionEfiHandle
//
// Description:  Translate option value index to local string handle.
//
// Parameters:
//               int
//                  [in] Index to PackInfo struct
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
GetOptionEfiHandle(int PackIndex, UINT16 *String, OPTION_LIST *OptionHead)
{
    struct InternalTables *g = AddressGlobals();
    struct PackInfo       *Pack = g->Packs + PackIndex;
    int                 Found = FALSE;
    UINT16 Length = 0;
    OPTION_LIST *Option = (OPTION_LIST *)NULL;
    
    for (Option = OptionHead; Option; Option = Option->Next)
    {
       struct String *OurString = ( struct String *)NULL;
       CHAR16 *FormattedText   = (CHAR16*)NULL;
       
       //Get the string
       OurString = &Pack->Strings[Option->Option-1];
       
       if( NULL == OurString)
           continue;
       
       //Remove Special characters in the string
       FormattedText = HiiLib_SkipEscCode (OurString->String);
       HiiLib_RemoveSpecialChars (&FormattedText);
       
       Length = (UINT16)(wcslen16(String) * sizeof(UINT16));
       if (0 == memcmp(String, FormattedText, Length))
       {
           Found = TRUE;
           break;
       }
       FreePool(FormattedText);
    }
    
#if 0
    for (i = 0; i < Pack->NumStrings; i++)
    {
        struct String *OurString;
        // change to regular strings -- maybe search map strings first
        OurString = &Pack->Strings[i];

        if (NULL == OurString->String)
        {
            continue;
        }
        Length = (UINT16)(wcslen16(String) * sizeof(UINT16));

        if (0 == memcmp(String, OurString->String, Length))
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
#endif
    
    if (FALSE == Found)
    {
        return -1;
    }

    // return Efi Handle of found string
    return Option->Option;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    ImportMappedControlValue
//
// Description:  Import a mapped control Value
//               Called from TraverseControls()
//               Searches imported values looking for a match on import name string and
//               control prompt
//
// Parameters:
//               UINT32
//                  [in] Index to PackInfo struct
//               CONTROL_INFO *
//                  [in] pointer to control info struct
//               void *
//                  [in out] pointer to control index
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
    int                   ControlIndex = *(int *)pControlIndex;
    int                   IsSpecial = 0;
    EFI_STATUS Status = EFI_SUCCESS;
    cJSON *ImportValue = (cJSON *) NULL;
    CHAR8 *MapString8 = (CHAR8 *)NULL;
    UINT16 ImportControlType = 0 ;
    VALUE_POINTER Value;
    wchar_t *Prompt;
    MEMFILE Buffer;
    HII_VALUE Header = {0};
    
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
	DEBUG((DEBUG_ERROR, "\n Attribute is zero")) ;
        *(int *)pControlIndex = ++ControlIndex;
        return 0;
    }

    IsSpecial = IsOEMSpecialControl(Control);
    if (IsSpecial)
    {
        *(int *)pControlIndex = ++ControlIndex;
        FreeControl(Control);
        return 0;   
    }

    //Get Control mapstring and compare with Pack structure string
    Prompt = HiiLib_GetHiiString(Pack->Package, gHiiDBData, Control->ControlPrompt, TRUE);
    if( (Prompt == NULL) || (Pack->Controls[ControlIndex].MapString == NULL) )
    {
        // this control not in foreign strings
        *(int *)pControlIndex = ++ControlIndex;
        FreeControl(Control);
        return 0;
    }
    if ( wcscmp(Prompt, Pack->Controls[ControlIndex].MapString) )
    {
        // this control not in foreign strings
        *(int *)pControlIndex = ++ControlIndex;
        FreeControl(Control);
        return 0; 
    }
    
    if(gHiiSetValue == TRUE)
    {
        if ( wcscmp(gSetMapString, Pack->Controls[ControlIndex].MapString) ) 
        {
            // this control not in foreign strings
            *(int *)pControlIndex = ++ControlIndex;
            FreeControl(Control);
            return 0; 
        }
    }
    else
    {
        MapString8 =  HiiLib_ConvertChar16toChar8(Pack->Controls[ControlIndex].MapString);
    
        //Get Mapstring control value from JsonInput
        ImportValue= cJSON_GetObjectItem(gJsonImportObject, MapString8);
        if( NULL == ImportValue )
        {
            //Control Value not exist for this mapstring
            *(int *)pControlIndex = ++ControlIndex;
            FreeControl(Control);
            return 0;
        }
    }
    
    ImportControlType = (UINT16)Pack->Controls[ControlIndex].Value->Type;
    Value.CommonHeader = (HII_VALUE *)Pack->Controls[ControlIndex].Value;
            
    if (Control->ControlType == EFI_IFR_NUMERIC_OP || Control->ControlType == EFI_IFR_CHECKBOX_OP)
    {
        UINT64 IntegerValue = 0;

        if ( !(ImportControlType == EFI_IFR_NUMERIC_OP ||
                ImportControlType == EFI_IFR_CHECKBOX_OP) )
        {
            DEBUG((DEBUG_ERROR, " Int: Value type(%x) and Control type(%x) mismatch!\n", Value.CommonHeader->Type, Control->ControlType));
            Status = EFI_ABORTED;
            goto DONE;
        }

        if(gHiiSetValue == TRUE)
        {
            if( EFI_IFR_CHECKBOX_OP == ImportControlType )
            {
               if(0 == CompareMem(gSetMapIdBuffer, "true", StrLen(L"true")))
                   IntegerValue = 1;
               else if(0 == CompareMem(gSetMapIdBuffer, "false", StrLen(L"false")))
                   IntegerValue = 0;
            }
            else
               IntegerValue = atoll (gSetMapIdBuffer);
        }
        else    
        {
        	if( EFI_IFR_CHECKBOX_OP == ImportControlType )
        	{
        		IntegerValue = (UINT64)ImportValue->valueint;
        	}
        	else
        	{
				CHAR8 *ValueBufferr = NULL; 
	
				ValueBufferr = cJSON_Print(ImportValue);
				IntegerValue = atoll (ValueBufferr);
        	}
        }
        // Bounds check the value
		if (EFI_IFR_CHECKBOX_OP == Control->ControlType)
		{
			if (IntegerValue > 1)
			{
				DEBUG((DEBUG_ERROR, "Bad value for checkbox %x\n", Value.Numeric->Value));
				Status = EFI_ABORTED;
				goto DONE;
			}
		}
		else
		{
		    if( CheckForNumericRange(Control->ValueInfo, Value.Numeric->Base, Value.Numeric->Datawidth, &IntegerValue) )
			//if (IntegerValue > Control->ValueInfo->MaxValue
					//|| IntegerValue < Control->ValueInfo->MinValue)
			{
			        DEBUG((DEBUG_ERROR, "Bad value for numeric control %x\n", Value.Numeric->Value));
				Status = EFI_ABORTED;
				goto DONE;
			}
		}
		
		Value.Numeric->Value = IntegerValue;
		 
        SceSetQuestionValue((UINT16)Control->VariableStorage, Control->DataOffset, Control->DataWidth, (UINT8 *)&Value.Numeric->Value);
        gSetValueStatus = EFI_SUCCESS;
        // assume that values are unique
        goto DONE;
    }
    else if (Control->ControlType == EFI_IFR_DATE_OP || Control->ControlType == EFI_IFR_TIME_OP)
    {
    	UINT64 IntegerValue = 0;

        if ( !(ImportControlType == EFI_IFR_DATE_OP ||
                        ImportControlType == EFI_IFR_TIME_OP) )
        {
            DEBUG((DEBUG_ERROR, " Int: Value type(%x) and Control type(%x) mismatch!\n", Value.CommonHeader->Type, Control->ControlType));
            Status = EFI_ABORTED;
            goto DONE;
        }

        if(gHiiSetValue == TRUE)
        	IntegerValue = atoi (gSetMapIdBuffer);
        else
        	IntegerValue = (UINT64)ImportValue->valueint;
        
        // Bounds check the value.
        if (Control->ControlType == EFI_IFR_DATE_OP)
        {
            EFI_HII_DATE *Date = (EFI_HII_DATE *)(UINT8 *)&IntegerValue;

            if (Date->Month > 12 || Date->Day > 31 || Date->Year > 9999)
            {
        	DEBUG((DEBUG_ERROR, "Bad value for date control %x\n", Value.Numeric->Value));
                Status = EFI_ABORTED;
                goto DONE; 
            }
        }
        else
        {
            EFI_HII_TIME *Time = (EFI_HII_TIME *)(UINT8 *)&IntegerValue;
            
            if (Time->Hour > 23 || Time->Minute > 59 || Time->Second > 59)
            {
        	DEBUG((DEBUG_ERROR, "Bad value for Time control %x\n", Value.Numeric->Value));
                Status = EFI_ABORTED;
                goto DONE;
            }
        }
        
        Value.Numeric->Value = IntegerValue;
                
        SceSetQuestionValue((UINT16)Control->VariableStorage, Control->DataOffset, Control->DataWidth, (UINT8 *)&Value.Numeric->Value);
        gSetValueStatus = EFI_SUCCESS;
        // assume that values are unique
        goto DONE;
    }
    else if (Control->ControlType == EFI_IFR_STRING_OP)
    {
        VALUE_POINTER v;
        int i;
        UINT16 *String;
        UINT16 StringLength;
        
        if ( ImportControlType != EFI_IFR_STRING_OP )
        {
            DEBUG((DEBUG_ERROR, " String: Value type(%x) and Control type(%x) mismatch!\n", Value.CommonHeader->Type, Control->ControlType));
            Status = EFI_ABORTED;
            goto DONE;
        }

        if(gHiiSetValue == TRUE)
        {
            
            String = HiiLib_ConvertChar8toChar16( gSetMapIdBuffer );
            StringLength = (UINT16)( gSetMapIdSize * sizeof(UINT16) );
        }
        else
        {        
            String = HiiLib_ConvertChar8toChar16( ImportValue->valuestring );
            StringLength = (UINT16)wcslen16(String) * sizeof(UINT16);
        }
        
         //check string length
        if (StringLength <= Control->DataWidth * sizeof(UINT16) 
                && StringLength >= Control->ValueInfo->MinValue * sizeof(UINT16)
                && StringLength <= Control->ValueInfo->MaxValue * sizeof(UINT16))
        {
            
            Value.String->Length = StringLength;
            
            MemCreat(&Buffer);
            Header.Handle = Pack->Controls[ControlIndex].Value->Handle;
            Header.Type = (UINT8)Control->ControlType;
            v.CommonHeader = (HII_VALUE *)malloc(sizeof * v.String);

            if (0 == v.CommonHeader)
            {
        	DEBUG((DEBUG_ERROR, "\nMemory exhausted"));
                Status = EFI_ABORTED;
                goto DONE;
            }
            
            *v.CommonHeader = Header;

            v.String->Length = Value.String->Length;
            MemWrite(&Buffer, v.String, sizeof(*v.String));
            //convert string  char8 to char16
            
            MemWrite(&Buffer, String, Value.String->Length);
            v.CommonHeader = (HII_VALUE *)Buffer.Buffer;
            Buffer.Buffer = 0;
            
            if (MemClose(&Buffer))
            {
               DEBUG((DEBUG_ERROR, "\n Memory exhausted"));
               Status = EFI_OUT_OF_RESOURCES;
               goto DONE;
            }
            
            //Assign the buffer to value pointer
            free(Pack->Controls[ControlIndex].Value);
            Pack->Controls[ControlIndex].Value = v.CommonHeader;
            Value.CommonHeader = (HII_VALUE *)Pack->Controls[ControlIndex].Value;
            
            SceSetQuestionValue((UINT16)Control->VariableStorage, Control->DataOffset, Value.String->Length, (UINT8 *)&Value.String->String);
            gSetValueStatus = EFI_SUCCESS;

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
            Status = EFI_ABORTED;
        }

        FreePool(String);
        goto DONE;
    }
    else if (Control->ControlType == EFI_IFR_ONE_OF_OP)
    {
        // need to give error if more than one value
        // find default value corresonding to string
        OPTION_LIST *Option;
        int OpNum;
        BOOLEAN Found = FALSE;
        UINT16 EfiHandle;   // string handle of selected option
        CHAR16 *String;
        
        if (Value.CommonHeader->Type != EFI_IFR_ONE_OF_OP)
        {
            DEBUG((DEBUG_ERROR, " Enum: Value type(%x) and Control type(%x) mismatch!\n", Value.CommonHeader->Type, Control->ControlType));
            Status = EFI_ABORTED;
            goto DONE;
        }

        if (Value.Enumeration->NumberOfValues == 0)
        {
            Status = EFI_ABORTED;
            goto DONE;    // didn't have a valid value when exported
        }

        if(gHiiSetValue == TRUE)
        {
            String = HiiLib_ConvertChar8toChar16( gSetMapIdBuffer );
        }
        else
        {  
            String = HiiLib_ConvertChar8toChar16( ImportValue->valuestring );
        }
        // map selected string to our English string's efi handle
        EfiHandle = GetOptionEfiHandle(PackIndex,String, Control->ValueInfo->Options);
        FreePool(String);
        
        // then look for match with Option->Option
        for (OpNum = 0, Option = Control->ValueInfo->Options; Option; Option = Option->Next, OpNum++)
        {
            if (EfiHandle == Option->Option)
            {
                SceSetQuestionValue((UINT16)Control->VariableStorage, Control->DataOffset, Control->DataWidth, (UINT8 *)&Option->Value);
                gSetValueStatus = EFI_SUCCESS;
                Value.Enumeration->ValueIndex[0] = OpNum;
                Found = TRUE;
                goto DONE;
            }
        }

        if (Found == FALSE)
        {
            DEBUG((DEBUG_ERROR, " Enum: Option Not found!: %x\n", EfiHandle));
            Status = EFI_ABORTED;
        }

        goto DONE;
    }
    else if (Control->ControlType == EFI_IFR_ORDERED_LIST_OP)
    {
        // find default value corresonding to string
        OPTION_LIST *Option;
        int OpNum;
        BOOLEAN Found = FALSE;
        UINT16 EfiHandle = 0;   // string handle of selected option
        UINT8 NoOfOptions = 0;
        VOID *OptionsList = NULL;
        
        if (Value.CommonHeader->Type != EFI_IFR_ORDERED_LIST_OP)
        {
            DEBUG((DEBUG_ERROR, " OrderList: Value type(%x) and Control type(%x) mismatch!\n", Value.CommonHeader->Type, Control->ControlType));
            Status = EFI_ABORTED;
            goto DONE;
        }

        if (Value.Enumeration->NumberOfValues == 0)
        {
            Status = EFI_ABORTED;
            goto DONE;    // didn't have a valid value when exported
        }

        //Get the macimum number of options for the List
        NoOfOptions = _GetUefiMaxEntries(Control->ControlPtr);

        //Create a New List
        OptionsList = (VOID *)AllocateZeroPool( Control->DataWidth * NoOfOptions );
        
        for(OpNum = 0; OpNum < NoOfOptions; OpNum++)
        {
            Found = FALSE;
            // map selected string to our English string's efi handle
           // EfiHandle = GetOptionEfiHandle(PackIndex, 1, Value.Enumeration->ValueIndex[OpNum], Control->ValueInfo->Options);
            for (Option = Control->ValueInfo->Options; Option; Option = Option->Next)
            {
                // then look for match with Option->Option
                if(EfiHandle == Option->Option)
                {
                    //Update the List with options in the order available in Value Table
                    memcpy((UINT8 *)OptionsList + (OpNum * Control->DataWidth), (UINT8 *)&Option->Value, Control->DataWidth);
                    Found = TRUE;
                    goto DONE;
                }
                else
                    Found = FALSE;
            }

            if (Found == FALSE)
            {
                Status = EFI_ABORTED;
                //If any one option present in Value Table is not present in the current HII, then abort.
                goto DONE;
            }
        }

        if (Found == TRUE)
        {
            //Update the New list for the control
            SceSetQuestionValue((UINT16)Control->VariableStorage, Control->DataOffset, NoOfOptions * Control->DataWidth, (UINT8 *)OptionsList);
            gSetValueStatus = EFI_SUCCESS;
        }
        else
        {
            Status = EFI_ABORTED;
            DEBUG((DEBUG_ERROR, " OrderedList: Option Not found!: %x\n", EfiHandle));
        }
        FreePool(OptionsList);

        goto DONE;
    }
    else if (EFI_IFR_PASSWORD_OP == Control->ControlType)
    {
        if (Value.CommonHeader->Type != EFI_IFR_PASSWORD_OP)
        {
            DEBUG((DEBUG_ERROR, " Pass: Value type(%x) and Control type(%x) mismatch!\n", Value.CommonHeader->Type, Control->ControlType));
            Status = EFI_ABORTED;
            goto DONE;
        }
        
        if(gHiiSetValue == TRUE)
        {
            VALUE_POINTER v;

            if (gSetMapIdSize != Control->DataWidth)
            {
                DEBUG((DEBUG_ERROR, "Password length differs from control %d\n", gSetMapIdSize));
                Status = EFI_ABORTED;
                goto DONE;
            }
            
            MemCreat(&Buffer);
            Header.Handle = Pack->Controls[ControlIndex].Value->Handle;
            Header.Type = (UINT8)Control->ControlType;
            v.CommonHeader = (HII_VALUE *)malloc(sizeof * v.PassWord);

            if (0 == v.CommonHeader)
            {
        	DEBUG((DEBUG_ERROR, "\nMemory exhausted"));
                Status = EFI_ABORTED;
                goto DONE;
            }
            
            *v.CommonHeader = Header;

           v.PassWord->Length = Control->DataWidth;
           MemWrite(&Buffer, v.PassWord, (UINT32)sizeof(*v.PassWord));
           MemWrite(&Buffer, gSetMapIdBuffer, (UINT32)gSetMapIdSize);
           v.CommonHeader = (HII_VALUE *)Buffer.Buffer;
           Buffer.Buffer = 0;

           if (MemClose(&Buffer))
           {
               DEBUG((DEBUG_ERROR, "\n Memory exhausted"));
               Status = EFI_OUT_OF_RESOURCES;
               goto DONE;
           }
           
           //Assign the buffer to value pointer
           free(Pack->Controls[ControlIndex].Value);
           Pack->Controls[ControlIndex].Value = v.CommonHeader;
           Value.CommonHeader = (HII_VALUE *)Pack->Controls[ControlIndex].Value;
        }

        // check password length -- since we send binary encrypted there is no reason for length to change
        if (Value.PassWord->Length != Control->DataWidth)
        {
            DEBUG((DEBUG_ERROR, "Password length differs from control %d\n", Value.PassWord->Length));
            Status = EFI_ABORTED;
            goto DONE;
        }

        // set the value
        SceSetQuestionValue((UINT16)Control->VariableStorage, Control->DataOffset, Control->DataWidth, (UINT8 *)&Value.PassWord->PassWord);
        gSetValueStatus = EFI_SUCCESS;
        goto DONE;
    }

 DONE:
    *(int *)pControlIndex = ++ControlIndex;
    FreeControl(Control);
    
    //Update the Fail MapIds List
    if( MapString8 && (EFI_SUCCESS !=Status) )
    {
        SaveFailedMapId (MapString8);
        FreePool(MapString8);
    }
        
    return Status;
}

/**
  This function initializes the global structures data for the Controls by parsing the entire Hiidatabase.

  @param[IN] HiiData           Hiidatabase
  @param[IN] HiiSize           Hiidatabase size

  @retval 	 EFI_SUCCESS          initialization success
  @retval 	 Others               initialization failed
**/
EFI_STATUS InitializeHiiGlobals( IN UINT8  *HiiData, IN UINTN  HiiSize)
{
    EFI_STATUS Status = EFI_SUCCESS;
    HII_DATA AmisceData = {0};
    UINTN DBSize = 0;
    UINT32 CRC32 = 0;
    struct InternalTables *g = AddressGlobals();
    
    //Check the gMD structure and Initialize it if gMD is NULL
    if(NULL == gMD)
    {
        InitializeStdLibrary();
        DEBUG((DEBUG_ERROR, "\nInitializeHiiGlobals: InitializeStdLibrary Done"));
    }
    
    if( (NULL == HiiData) || (0 == HiiSize) )
    {
        Status = HiiLib_RetrieveHIIData( &gHiiDBData, (UINTN *)&gHiiDataSize, &CRC32 );
        
        if (Status != EFI_SUCCESS)
        {
            DEBUG((DEBUG_ERROR, "\nInitializeHiiGlobals: Cannot access HII data !!\n"));
            return Status;
        }
        
    }
    else
    {
        gHiiDBData = HiiData;
        gHiiDataSize = (UINT64)HiiSize;
    }
    
    //Check if structure already initialized
    if( TRUE == gHiiGlobalsInit)
        FreeHiiGlobals();
    
    FindUefiVersion( gHiiDBData );
    
    //Build the controls data structures
    Status = BuildInternalTables();

    if (EFI_SUCCESS != Status)
    {
	DEBUG((DEBUG_ERROR, "InitializeHiiGlobals:Failed to build internal tables\n"));
        return Status;
    }

    gHiiGlobalsInit = TRUE;
    
    return Status;
}

/**
  Gets the available MapString names of all controls in the Hii database.

  @param[OUT] 	MapStringList     The list of Mapstring names 
  @param[OUT] 	MapListSize       Mapstring list size

  @retval 		EFI_SUCCESS       Mapstring list get success
  @retval 		Others            Mapstring list get failed
**/
EFI_STATUS GetHiiMapStringList( OUT UINT8 **MapStringList, OUT UINT32 *MapListSize)
{
    EFI_STATUS      Status = EFI_NOT_FOUND;
    struct InternalTables *g = (struct InternalTables *)NULL;
    int             i = 0;
    int             j = 0;
    BOOLEAN         MapIdListFound = FALSE;
    MEMFILE         MapIdBuffer = {0};
    HII_MAPIDINFO   MapIdData = {0};//strucure to hold the mapstring data
    struct PackInfo *Pack;
    MAP_ID_LIST     *MapIdList = (MAP_ID_LIST*)NULL;
    AMI_CONDITIONAL_MAPIDLIST_PROTOCOL *AmiCondMapIdListProtocol = NULL;

    MemCreat(&MapIdBuffer);
    //<kuaibai-20180708> Status = pBS->LocateProtocol (&gAmiConditionalMapIdListProtocolGuid, NULL, (void**)&AmiCondMapIdListProtocol);
    Status = gBS->LocateProtocol (&gAmiConditionalMapIdListProtocolGuid, NULL, (void**)&AmiCondMapIdListProtocol);
    if(!EFI_ERROR(Status))
    {
        Status = AmiCondMapIdListProtocol->GetConditionalMapIdList((VOID *)&MapIdList);
    
        if(!EFI_ERROR(Status))
        {
            MapIdListFound = TRUE;
            while(MapIdList)
            {
                MapIdData.ControlType = 0;  //Initializing with 0. 0 is not ControlType.
                MapIdData.Length = (UINT16)(sizeof(UINT16) * wcslen16((UINT16 *)MapIdList->MapID));
                
                //Write to the buffer
                MemWrite(&MapIdBuffer, &MapIdData, sizeof(MapIdData));
                MemWrite(&MapIdBuffer, MapIdList->MapID, MapIdData.Length);  
                Status = EFI_SUCCESS;
                MapIdList = MapIdList->Next;
            }
        }
    }
    if (FALSE == MapIdListFound)
    {
        //Check whether the Hii Tables initialized
        if( FALSE == gHiiGlobalsInit)
        {
            Status = InitializeHiiGlobals(NULL, 0);
            if( EFI_ERROR(Status))
                return Status;
            
        }
        
        g = AddressGlobals();
        
        //Parse the Globals and create the MapId buffer
        for (i = 0; i < g->nPacks; i++)
        {
        
            Pack = &g->Packs[i];
            
            for (j = 0; j < Pack->NumControls; j++)
            {
                if (NULL != Pack->Controls[j].MapString)
                {
                    
                    MapIdData.ControlType = (UINT16)Pack->Controls[j].Attribute->Type;
                    MapIdData.Length = (UINT16)(sizeof(UINT16) * wcslen16((UINT16 *)Pack->Controls[j].MapString));
                    
                    //Write to the buffer
                    MemWrite(&MapIdBuffer, &MapIdData, sizeof(MapIdData));
                    MemWrite(&MapIdBuffer, Pack->Controls[j].MapString, MapIdData.Length);  
                    Status = EFI_SUCCESS;
                }
            }
        }
    }
    //Add CRC to the end of buffer
    if(Status == EFI_SUCCESS)
    {
       AddCrc(&MapIdBuffer);

       *MapStringList = MapIdBuffer.Buffer;
       *MapListSize = MapIdBuffer.FileSize;
       
       MapIdBuffer.Buffer = 0;
       if (MemClose(&MapIdBuffer))
       {
	   DEBUG((DEBUG_ERROR, "GetMapStringList: Memory exhaustion\r\n"));
       }
    }
    
    return Status;
}

/**
  Gets the entire Hii database MapString controls name and value in Json Name:Value pair format

  @param[IN] 	HiiDatabase       Hiidatabase
  @param[IN] 	HiiSize           Hiidatabase size
  @param[OUT] 	MapControlInfo    Mapcontrols Json data buffer 
  @param[OUT] 	BufferSize        buffer size

  @retval 		EFI_SUCCESS       Control database information get success
  @retval 		Others            get failed
**/
EFI_STATUS GetMapStringControlDatabase ( IN UINT8 *HiiDatabase, IN UINTN  HiiSize, OUT UINT8 **MapControlInfo, OUT UINT32 *BufferSize )
{
    EFI_STATUS Status = EFI_NOT_FOUND;
    struct InternalTables *g = (struct InternalTables *)NULL;
    int i;
    int j;
    struct PackInfo *Pack;
    CHAR8 *JsonStr8 = (CHAR8*)NULL;
    CHAR8 *MappingId8 = NULL;
    
    if( (NULL != HiiDatabase) && (0 != HiiSize) )
    {
    	if( TRUE == gHiiGlobalsInit)
    		FreeHiiGlobals();
    	
        Status = InitializeHiiGlobals(HiiDatabase, HiiSize);
        if( EFI_ERROR(Status))
            return Status;
    }
    
    //Check whether the Hii Tables initilazed
    if( FALSE == gHiiGlobalsInit)
    {
        Status = InitializeHiiGlobals(NULL, 0);
        if( EFI_ERROR(Status))
            return Status;
    }
    
    g = AddressGlobals();
    
    if(0 == g->nPacks)
        return EFI_NOT_FOUND;
    
    JsonStr8 = HiiLib_GetJsonDataStr(HII_JSON_OBJECT_OPEN,1,(CHAR8*)NULL,(CHAR8*)NULL);
    HiiLib_UpdateData(&JsonStr8);
                    
    //Parse the Globals and create the mapstring database
    for (i = 0; i < g->nPacks; i++)
    {
    
        Pack = &g->Packs[i];
        
        for (j = 0; j < Pack->NumControls; j++)
        {
            if (NULL != Pack->Controls[j].MapString)
            {
                void        *Buffer = NULL;
                CHAR8 *JsonStr8 = (CHAR8*)NULL;
                UINT16 ControlType;
                
                MappingId8 = HiiLib_ConvertChar16toChar8(Pack->Controls[j].MapString);
                ControlType = (UINT16)Pack->Controls[j].Attribute->Type;
                
                if (ControlType == EFI_IFR_NUMERIC_OP || ControlType == EFI_IFR_CHECKBOX_OP ||
                        ControlType == EFI_IFR_DATE_OP || ControlType == EFI_IFR_TIME_OP )
                {
                    Buffer = AllocateZeroPool (21);     //Max value of UINT64 will come around 19 digits
                    if (NULL == Buffer)
                    {
                        return EFI_OUT_OF_RESOURCES;
                    }
                    (void)sprintf (Buffer, "%lld", (*(struct Table18 *)(Pack->Controls[j].Value)).Value); 
                    //update
                    if(EFI_IFR_CHECKBOX_OP == ControlType)
                        JsonStr8 = HiiLib_GetJsonDataStr(HII_JSON_DATA_BOOLEAN,3,MappingId8,Buffer);
                    else
                    {
                        UINT64 IntegerValue = (*(struct Table18 *)(Pack->Controls[j].Value)).Value;
                        
                        Buffer = AllocateZeroPool ( ((*(struct Table18 *)(Pack->Controls[j].Value)).Datawidth) + 1);
                        if( NULL == Buffer)
                        {
                            return EFI_OUT_OF_RESOURCES;
                        }
                        
                        NumSPrintf(&Buffer, IntegerValue, (*(struct Table18 *)(Pack->Controls[j].Value)).Datawidth,(*(struct Table18 *)(Pack->Controls[j].Value)).Base);
                        JsonStr8 = HiiLib_GetJsonDataStr(HII_JSON_DATA_INTEGER,3,MappingId8,Buffer);
                    }
                }
                else if (ControlType == EFI_IFR_ONE_OF_OP || ControlType == EFI_IFR_ORDERED_LIST_OP)
                {
                    UINT16 *String;
                    CHAR16 *FormattedText   = (CHAR16*)NULL;
                    
                    struct Table6 *EnumControl = (struct Table6 *)Pack->Controls[j].Attribute;
                    
                    UINT16 EnumStringHandle =  EnumControl->Handle[(*(struct Table15 *)(Pack->Controls[j].Value)).ValueIndex[0]];
                    
                    String = Hii_GetStringByHandleIndex (Pack, EnumStringHandle - Pack->FirstStringHandle);
                    if (NULL == String)
                    {
                        return EFI_OUT_OF_RESOURCES;
                    }
                    
                    //Remove Special characters in the string
                    FormattedText = HiiLib_SkipEscCode (String);
                    FreePool (String);
                    HiiLib_RemoveSpecialChars (&FormattedText);
                                            
                    Buffer = HiiLib_ConvertChar16toChar8(FormattedText);
                    JsonStr8 = HiiLib_GetJsonDataStr(HII_JSON_DATA,3,MappingId8,Buffer);
                    FreePool(FormattedText);
                }
                else if (ControlType == EFI_IFR_STRING_OP)
                {
                    void *StringBuffer = NULL;
                    
                    StringBuffer = AllocateZeroPool ( (*(struct Table16 *)Pack->Controls[j].Value).Length + 4);
                    if (NULL == StringBuffer)
                    {
                        return EFI_OUT_OF_RESOURCES;
                    }
                    
                    memcpy (StringBuffer, (*(struct Table16 *)Pack->Controls[j].Value).String, (*(struct Table16 *)Pack->Controls[j].Value).Length);
                    Buffer = HiiLib_ConvertChar16toChar8(StringBuffer);
                    JsonStr8 = HiiLib_GetJsonDataStr(HII_JSON_DATA,3,MappingId8,Buffer);
                    FreePool(StringBuffer);
                }
                else if (ControlType == EFI_IFR_PASSWORD_OP)
                {
                       
                    Buffer = AllocateZeroPool ((*(struct Table17 *)Pack->Controls[j].Value).Length );
                    if (NULL == Buffer)
                    {
                        return EFI_OUT_OF_RESOURCES;
                    }
                    memcpy (Buffer, (*(struct Table17 *)Pack->Controls[j].Value).PassWord, (*(struct Table17 *)Pack->Controls[j].Value).Length);
                    
                    ////Encoded format of Password not supported in JSON?????
                    //JsonStr8 = HiiLib_GetJsonDataStr(HII_JSON_DATA,3,MappingId8,Buffer);
                    //FreePool(Buffer);
                }
                
                HiiLib_UpdateData(&JsonStr8);
                
                if( Buffer )        
                    FreePool(Buffer);
                FreePool(MappingId8);
            }
        }
    }
    
    JsonStr8 = HiiLib_GetJsonDataStr(HII_JSON_OBJECT_CLOSE,1,(CHAR8*)NULL,(CHAR8*)NULL);
    HiiLib_UpdateData(&JsonStr8);
    
    if(NULL != gHiiBiosSetupBuf._String8)
    {
        *MapControlInfo = gHiiBiosSetupBuf._String8;
        *BufferSize = (UINT32)gHiiBiosSetupBuf._SizeUsed;
    }
    
    return EFI_SUCCESS;
}

/**
  Parses the provided MapString controls name and value in Json Name:Value pair format data and imports the values to NVRAM
  Returns the Set Failed MapId control names as a buffer  

  @param[IN] 	HiiDatabase           Hiidatabase
  @param[IN] 	HiiSize           Hiidatabase size
  @param[IN] 	MapControlInfo    Mapcontrols Json data buffer 
  @param[IN] 	BufferSize        buffer size
  @param[OUT] 	FailMapIdList     Set Failed Mapstring control names
  @param[OUT] 	FailMapListSize   Fail mapId buffer size

  @retval 		EFI_SUCCESS       Set Control Values success
  @retval 		Others            Set failed
**/
EFI_STATUS SetMapStringControlDatabase (IN UINT8 *HiiDatabase, UINTN  HiiSize,IN UINT8 *MapControlInfo, IN UINT32 BufferSize, OUT void **FailMapIdList, OUT UINT32 *FailMapListSize )
{

    EFI_STATUS Status = EFI_SUCCESS;
    struct InternalTables *g = (struct InternalTables *)NULL;
    int i;
    int     ControlCount = 0;
    CHAR8 *JsonStr8 = (CHAR8*)NULL;
    UINTN   size = 0;
    EFI_GUID    BootOptNameGuid = {0x5b5f93b7, 0x8f58, 0x47ce, 0x80, 0x3b, 0x48, 0x23, 0x6e, 0x80, 0x31, 0xaf};
    CHAR16      *BootOptMapId = NULL;
    cJSON *ImportValue = (cJSON *) NULL;
    
   // Obtain boot option MapId saved
    if( NULL == BootOptMapId)
    {
        Status = pRS->GetVariable (L"BootOptMapId", &BootOptNameGuid, NULL, &size, BootOptMapId);
        if( EFI_BUFFER_TOO_SMALL == Status)
        {
            BootOptMapId =  calloc(size, sizeof(CHAR16));
            Status = pRS->GetVariable (L"BootOptMapId", &BootOptNameGuid, NULL, &size, BootOptMapId);
        }
    }
    
    if( (NULL != HiiDatabase) && (0 != HiiSize) )
    {
    	if( TRUE == gHiiGlobalsInit)
    		FreeHiiGlobals();
    	
        Status = InitializeHiiGlobals(HiiDatabase, HiiSize);
        if( EFI_ERROR(Status))
            return Status;
    }
    
    //Check whether the Hii Tables initilazed
    if( FALSE == gHiiGlobalsInit)
    {
        Status = InitializeHiiGlobals(NULL, 0);
        if( EFI_ERROR(Status))
            return Status;
    }
    
    g = AddressGlobals();
    
    //Parse the input JSON data
    gJsonImportObject = cJSON_Parse(MapControlInfo);
    if( NULL == gJsonImportObject )
    {
        //Invalid Json format data
        return EFI_INVALID_PARAMETER;
    }
    
    //Get the Attributes object
    gJsonImportObject= cJSON_GetObjectItem(gJsonImportObject, "Attributes");
    if( NULL == gJsonImportObject )
    {
        //Invalid Json object
        return EFI_INVALID_PARAMETER;
    }
    
    gSetValueStatus = EFI_UNSUPPORTED;
    //Open file to update the fail MapId List
    JsonStr8 = HiiLib_GetJsonDataStr(HII_JSON_OBJECT_OPEN,1,(CHAR8*)NULL,(CHAR8*)NULL);
    HiiLib_UpdateData(&JsonStr8);
    
    GetHiiHandles();
    // Parse each package and import
    for (i = 0; i < g->nPacks; i++)
    {
        struct PackInfo *Pack = g->Packs + i;

        if (NULL == Pack->MapStrings)
        {
            continue;
        }

        gCurrentHiiHandle = Pack->HiiHandle;
        ControlCount = 0;   // keep count relative to pack
        TraverseControls(i, ImportMappedControlValue, (void *)&ControlCount);

        // Update Variables
        if (!gUseCache)
        {
            ImportVariableToNVRAM();
        }

        _FreeVariableList();
    }
    
    //Get Mapstring control value from JsonInput
    ImportValue= cJSON_GetObjectItem(gJsonImportObject, HiiLib_ConvertChar16toChar8(BootOptMapId));
    if( NULL != ImportValue )
    {
        Status = ProcessBootOptions(ImportValue->valuestring, strlen(ImportValue->valuestring)); 
        free(BootOptMapId);
        free(gBootOptions);
        free(gBootOrder);   
        gBootCount = 0;
//<kuaibai001-20180813 Fix hang when importing bootorder changes from OOB +>
//       gSetValueStatus = EFI_SUCCESS;//kuaibai add update >>>
        return Status;
//<kuaibai001-20180813 Fix hang when importing bootorder changes from OOB ->
    }
    FreeHiiHandles();
    
    cJSON_Delete (gJsonImportObject); 
    
    //update the fail MapId List BUffer
    if(gFailMapIdBuffer && NULL != gHiiBiosSetupBuf._String8)
    {
        HiiLib_UpdateData(&gFailMapIdBuffer);
        gFailMapIdBuffer = NULL;
        
        JsonStr8 = HiiLib_GetJsonDataStr(HII_JSON_OBJECT_CLOSE,1,(CHAR8*)NULL,(CHAR8*)NULL);
        HiiLib_UpdateData(&JsonStr8);
            
        *FailMapIdList = gHiiBiosSetupBuf._String8;
        *FailMapListSize = (UINT32)gHiiBiosSetupBuf._SizeUsed;
    }
    else
    {
    	FreePool(gHiiBiosSetupBuf._String8);
    
    }
       
    //Free the buffer parameters
	gHiiBiosSetupBuf._String8 = NULL;
	gHiiBiosSetupBuf._SizeFree = 0;
	gHiiBiosSetupBuf._SizeUsed = 0;
    gFailMapIdAllocBuffSize = 0;
    gFailMapIdBuffSize = 0;
    
    return gSetValueStatus;
}

/**
  check for unsigned integer 

  @param CONTROL_INFO* 

  @retval UINT8

**/

UINT8 HiiIsUnSignedInteger(struct Table9 *ValueInfo,UINT16 size)
{
	EFI_STATUS Status = EFI_SUCCESS;
	UINT64 Min = (UINT64)ValueInfo->LowerBound;
	UINT64 Max = (UINT64)ValueInfo->UpperBound;
	UINT8 UnsignedStatus = 1;

	switch(size)
	{
		case 1:
			UnsignedStatus = (UINT8)(IS_UNSIGNED(UINT8,Min,Max));
			break;
		case 2:
			UnsignedStatus = (UINT8)IS_UNSIGNED(UINT16,Min,Max);
			break;
		case 4:
			UnsignedStatus = (UINT8)IS_UNSIGNED(UINT32,Min,Max);
			break;
		case 8:
			UnsignedStatus = (UINT8)IS_UNSIGNED(UINT64,Min,Max);
			break;
		default:
			UnsignedStatus = (UINT8)(IS_UNSIGNED(UINT8,Min,Max));
			break;
	}
	return UnsignedStatus;
}

/**
    Convert unsigned number to string

    @param UINT64 InputVal,CHAR16 *String

    @retval void
**/

void HiiConvertIntToString(UINT64 InputVal,CHAR16 *String)
{
	UINT64 Val = 0;
	UINT8 Digit = 0;
	CHAR8 RevString[512];
	//CHAR16 *String = NULL;
	INT32 Index =0,ValidIndex =0;
	INT32 Count =0;
	BOOLEAN ValidDigitFound = FALSE;
	Val = InputVal;

	if(Val & INT64_MAX_MASK_BIT)
	{
		Val = INT64_MAX_LIMIT; 
	}
	
	while(Val!= 0)
	{
		Digit = Val%10;
		RevString[Count] = Digit+48;
		Val = Val/10;
		Count++;
	}
	
	//String = AllocateZeroPool( 512 *sizeof(CHAR16));
	for(Index=Count-1;Index >= 0;--Index)
	{
		if(FALSE == ValidDigitFound)
		{
			if(RevString[Index] == 0)
				continue;
		}
		ValidDigitFound = TRUE;
		String[ValidIndex] = (CHAR16)(RevString[Index]);
		ValidIndex++;
	}
	if(FALSE == ValidDigitFound)
	{
		String[ValidIndex] = (CHAR16)(48);
		ValidIndex++;
	}
		
	String[ValidIndex] = L'\0';
}

CHAR16* HiiConvVfrNumToString(UINT64 Num,UINTN ControlDataWidth,UINT8 IsUnSigned)
{
	CHAR16 *Text = NULL;
	Text = (CHAR16*)AllocateZeroPool(128 * sizeof(CHAR16));
	
	if (1 ==IsUnSigned )
		HiiConvertIntToString((UINT64)Num,Text);
	else
	{
		switch(ControlDataWidth)
		{
			case 1:
					Swprintf( Text, L"%d", (INT8)Num );	
				break;
			case 2:
					Swprintf( Text,L"%d", (INT16)Num);	
				break;
			case 4:
					Swprintf( Text, L"%d", (INT32)Num);		
				break;
			case 8:
					Swprintf( Text, L"%ld", (INT64)Num );		
				break;
			default:
					Swprintf( Text, L"%d", (UINT8)Num);
				break;
		}
	}
	
	return Text;
}

/**
  This function will get the control value buffer for the provided Mapstring control name  

  @param[IN] 	MappingID         MapString name for the control
  @param[OUT] 	ValueBuffer       Control value buffer
  @param[OUT] 	ValueSize    	  Control value buffer size
  @param[OUT] 	ControlType       Control Type

  @retval 		EFI_SUCCESS       Get Control Value success
  @retval 		Others            Get failed
**/
EFI_STATUS GetHiiMapStringValue(IN CHAR16 *MappingID, OUT void **ValueBuffer, OUT UINTN *ValueSize, OUT UINT16 *MapControlType)
{
    EFI_STATUS Status = EFI_NOT_FOUND;
    struct InternalTables *g = (struct InternalTables *)NULL;
    int i;
    int j;
    struct PackInfo *Pack;
    UINT8 IsUnsigned =0;
    
    //Check whether the Hii Tables initilazed
    if( FALSE == gHiiGlobalsInit)
    {
        Status = InitializeHiiGlobals(NULL, 0);
        if( EFI_ERROR(Status))
            return Status;
    }
    
    g = AddressGlobals();
    
    for (i = 0; i < g->nPacks; i++)
    {
        Pack = &g->Packs[i];
        
        for (j = 0; j < Pack->NumControls; j++)
        {
            if ( (Pack->Controls[j].MapString ) && (!wcscmp(MappingID, Pack->Controls[j].MapString)) )
            {
                UINT16 ControlType;
                void *Buffer = NULL;
                CHAR16 *Buffer16 = NULL;
                
                ControlType = (UINT16)Pack->Controls[j].Value->Type;
                *MapControlType = ControlType;
                switch(ControlType)
                {
                    case EFI_IFR_NUMERIC_OP:
                    case EFI_IFR_CHECKBOX_OP:
                    case EFI_IFR_DATE_OP:
                    case EFI_IFR_TIME_OP:
                    {
                        UINT64 IntegerValue = (*(struct Table18 *)(Pack->Controls[j].Value)).Value;
                        CHAR8  *BoolArray[] = {"false","true"};
                        
                        if( EFI_IFR_CHECKBOX_OP == ControlType )
                        {
                            if(0 == IntegerValue)
                            {
                                Buffer = AllocateZeroPool (strlen(BoolArray[0]) + 1);
                                if( NULL == Buffer)
                                {
                                    Status = EFI_OUT_OF_RESOURCES;
                                    goto DONE;
                                }
                                (void)sprintf (Buffer, "%s", BoolArray[0]);
                            }
                            else
                            {
                                Buffer = AllocateZeroPool (strlen(BoolArray[1]) + 1);
                                if( NULL == Buffer)
                                {
                                    Status = EFI_OUT_OF_RESOURCES;
                                    goto DONE;
                                }
                                (void)sprintf (Buffer, "%s", BoolArray[1]);
                            }
                            *ValueBuffer = Buffer;
                            *ValueSize = strlen(Buffer); 
                            Status = EFI_SUCCESS;
                        }
                        else
                        {
                            IsUnsigned = HiiIsUnSignedInteger((struct Table9 *)Pack->Controls[j].Attribute,(*(struct Table18 *)(Pack->Controls[j].Value)).Datawidth);
                            Buffer16 = HiiConvVfrNumToString(IntegerValue,(*(struct Table18 *)(Pack->Controls[j].Value)).Datawidth,IsUnsigned);
                            if(Buffer16)
                            {
                            	Buffer = (void *)HiiLib_ConvertChar16toChar8(Buffer16);
                            	FreePool(Buffer16);
                                *ValueBuffer = Buffer;
                                *ValueSize = strlen(Buffer);
                            }
                            //NumSPrintf(&Buffer, IntegerValue, (*(struct Table18 *)(Pack->Controls[j].Value)).Datawidth,(*(struct Table18 *)(Pack->Controls[j].Value)).Base);
 
                            Status = EFI_SUCCESS;
                        }
                        break;
                    }
                    case EFI_IFR_ONE_OF_OP:
                    case EFI_IFR_ORDERED_LIST_OP:
                    {
                        
                        UINT16 *String;
                        CHAR16 *FormattedText   = (CHAR16*)NULL;
                        
                        struct Table6 *EnumControl = (struct Table6 *)Pack->Controls[j].Attribute;
                        
                        UINT16 EnumStringHandle =  EnumControl->Handle[(*(struct Table15 *)(Pack->Controls[j].Value)).ValueIndex[0]];
                        
                        String = Hii_GetStringByHandleIndex (Pack, EnumStringHandle - Pack->FirstStringHandle);
                        if (NULL == String)
                        {
                            return EFI_OUT_OF_RESOURCES;
                        }
                        
                        //Remove Special characters in the string
                        FormattedText = HiiLib_SkipEscCode (String);
                        FreePool (String);
                        HiiLib_RemoveSpecialChars (&FormattedText);
                                        
                        Buffer = HiiLib_ConvertChar16toChar8(FormattedText);
                        *ValueBuffer = Buffer;
                        *ValueSize = strlen(Buffer);  
                        FreePool(FormattedText);
                        Status = EFI_SUCCESS;
                        break;
                    }
                    case EFI_IFR_STRING_OP:
                    {
                        void *StringBuffer = NULL;
                        
                        StringBuffer = AllocateZeroPool ( (*(struct Table16 *)Pack->Controls[j].Value).Length + 4);
                        if( NULL == StringBuffer)
                        {
                            Status = EFI_OUT_OF_RESOURCES;
                            goto DONE;
                        }
                        
                        memcpy (StringBuffer, (*(struct Table16 *)Pack->Controls[j].Value).String, (*(struct Table16 *)Pack->Controls[j].Value).Length);
                        Buffer = HiiLib_ConvertChar16toChar8(StringBuffer);
                        
                        *ValueBuffer = Buffer;
                        *ValueSize = ( (*(struct Table16 *)Pack->Controls[j].Value).Length )/2;
                        
                        FreePool(StringBuffer);
                        Status = EFI_SUCCESS;
                        break;
                    }
                    case EFI_IFR_PASSWORD_OP:
                    {
                        Buffer = AllocateZeroPool ((*(struct Table17 *)Pack->Controls[j].Value).Length );
                        if( NULL == Buffer)
                        {
                            Status = EFI_OUT_OF_RESOURCES;
                            goto DONE;
                        }
                        memcpy (Buffer, (*(struct Table17 *)Pack->Controls[j].Value).PassWord, (*(struct Table17 *)Pack->Controls[j].Value).Length);
                        
                        *ValueBuffer = Buffer;
                        *ValueSize = (*(struct Table17 *)Pack->Controls[j].Value).Length;
                        Status = EFI_SUCCESS;
                        break;
                    }
                }
            }
        }
    }
    
DONE:
    return Status;     
}

/**
  This function will set the control value buffer for the provided Mapstring control name control 

  @param[IN] 	MappingID       MapString name for the control
  @param[IN] 	Buffer       	Control value buffer
  @param[IN] 	BufferSize    	Control value buffer size

  @retval 		EFI_SUCCESS       Get Control Value success
  @retval 		Others            Get failed
**/
EFI_STATUS  SetHiiMapStringValue ( IN CHAR16 *MappingID, IN void *Buffer, IN UINTN BufferSize )
{
    EFI_STATUS Status = EFI_SUCCESS;
    struct InternalTables *g = (struct InternalTables *)NULL;
    int i;
    int     ControlCount = 0;
    UINTN   size = 0;
    EFI_GUID    BootOptNameGuid = {0x5b5f93b7, 0x8f58, 0x47ce, 0x80, 0x3b, 0x48, 0x23, 0x6e, 0x80, 0x31, 0xaf};

   // Obtain boot option MapId saved
    if( NULL == gBootOptionMapId)
    {
        Status = pRS->GetVariable (L"BootOptMapId", &BootOptNameGuid, NULL, &size, gBootOptionMapId);
        if( EFI_BUFFER_TOO_SMALL == Status)
        {
            gBootOptionMapId =  calloc(size, sizeof(CHAR16));
            Status = pRS->GetVariable (L"BootOptMapId", &BootOptNameGuid, NULL, &size, gBootOptionMapId);
        }
    }

	// If Current MapId is of BootOrder, parse and process boot option entries
    if( 0 == StrCmp(gBootOptionMapId, MappingID) )
    {
        Status = ProcessBootOptions(Buffer, BufferSize); 
        free(gBootOptionMapId);
        return Status;
    }
    
    //Check whether the Hii Tables initilazed
    if( FALSE == gHiiGlobalsInit)
    {
        Status = InitializeHiiGlobals(NULL, 0);
        if( EFI_ERROR(Status))
            return Status;
    }
    
    g = AddressGlobals();
    
    gHiiSetValue = TRUE;
    
    gSetMapString = MappingID;
    gSetMapIdBuffer = Buffer;
    gSetMapIdSize = BufferSize;
    gSetValueStatus = EFI_UNSUPPORTED;
    
    GetHiiHandles();
    // for each package with IFR
    for (i = 0; i < g->nPacks; i++)
    {
        struct PackInfo *Pack = g->Packs + i;

        if (NULL == Pack->MapStrings)
        {
            continue;
        }

        gCurrentHiiHandle = Pack->HiiHandle;
        ControlCount = 0;   // keep count relative to pack
        TraverseControls(i, ImportMappedControlValue, (void *)&ControlCount);

        // Update Variables
        if (!gUseCache)
        {
            ImportVariableToNVRAM();
        }

        _FreeVariableList();
        
        //Check MapString Control Value set status
        if ( EFI_SUCCESS == gSetValueStatus )
            break;
    }

    FreeHiiHandles();
    gHiiSetValue = FALSE;
    
    return gSetValueStatus; 
}

/**
  This function will get the structure array holding the entire Hiidatabase controls information  

  @param[IN] 	HiiDatabase     HiiDatabase
  @param[IN] 	HiiSize       	HiiSize
  @param[OUT] 	HiiControlInfo  structure array holding the entire Hiidatabase controls information  

  @retval 		EFI_SUCCESS       Get success
  @retval 		Others            Get failed
**/
EFI_STATUS GetHiiControlsDataArray ( IN UINT8 *HiiDatabase, UINTN  HiiSize, OUT const struct InternalTables **HiiControlInfo)
{
    EFI_STATUS Status = EFI_SUCCESS;
    struct InternalTables *g = (struct InternalTables *)NULL;
    int     ControlCount = 0;
    
    if( (NULL != HiiDatabase) && (0 != HiiSize) )
    {
		if( TRUE == gHiiGlobalsInit)
			FreeHiiGlobals();
		
		Status = InitializeHiiGlobals(HiiDatabase, HiiSize);
		if( EFI_ERROR(Status))
			return Status;
    }
       
    //Check whether the Hii Tables initilazed
    if( FALSE == gHiiGlobalsInit)
    {
       Status = InitializeHiiGlobals(NULL, 0);
       if( EFI_ERROR(Status))
           return Status;
    }
    
    g = AddressGlobals(); 
    
    if(g->nPacks)
        *HiiControlInfo = g;
    
    return Status;
}

/**
  Free the Allocated memory for all the array of global structures

  @param 	VOID 

  @retval 	VOID
**/
void FreeHiiGlobals()
{
    struct InternalTables *g = AddressGlobals();
    int PackIndex;

    for (PackIndex = 0; PackIndex < g->nPacks; PackIndex++)
    {
        int i;
        struct PackInfo *Pack = &g->Packs[PackIndex];

        for (i = 0; i < Pack->NumControls; i++)
        {
            if(Pack->Controls[i].MapString)
                free(Pack->Controls[i].MapString);
            if(Pack->Controls[i].String)
                free(Pack->Controls[i].String);
            
            free(Pack->Controls[i].Attribute);
            free(Pack->Controls[i].Value);
        }

        free(Pack->Controls);
        Pack->Controls = NULL;
       
        free(Pack->Strings);
        Pack->Strings = NULL;
        // Note: map strings are never allocated
        free(Pack->MapStrings);
        Pack->MapStrings = NULL;
        
        //Free the Variable list
        gVariable = (VARIABLE *)Pack->VariableList;
        _FreeVariableList();
        Pack->VariableList = NULL;
    }

    memset(g, 0, sizeof(*g));
    gHiiGlobalsInit = FALSE;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    GetHiiHandles
//
// Description:  Get the HII handle array and store in gHiiHandles. Assumes
//              that handles have a one-for-one correspondence to the export
//              packages in the database export data.
//
// Parameters:   None
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
}

static
void
FreeHiiHandles()
{
    free(gHiiHandles);
    gHiiHandles = 0;
    gCurrentHiiHandle = 0;
}

//<kuaibai001-20180813 Fix hang when importing bootorder changes from OOB +>
//kuaibai add update >>>
/**
    Skips character from a string returns the string
    @param  CHAR16 * - String
    @param  CHAR16 * - character to be skipped
    @retval  CHAR16 * - modified string
**/
/*
CHAR16* SkipCharInString(CHAR16 *String16, CHAR16 *ch)
{
    CHAR16 *OutString16 = (CHAR16 *)NULL;
    UINTN Strlen16 = 0;
    UINTN Index = 0, SubIndex = 0;
    
    if ((NULL == String16) || (*String16 == '\0'))
    {
        return (CHAR16*)NULL;
    }
    
    if( NULL == StrStr(String16,ch) )
        return String16;
    
    Strlen16 = StrLen(String16);
    OutString16 = AllocateZeroPool(Strlen16+1);

    while(String16[Index] != '\0')
    {
        if( *ch != String16[Index] )
        {
            OutString16[SubIndex++] = String16[Index];
        }
        Index++;
    }

    OutString16[SubIndex] = '\0';
        
    return OutString16;
}
*/
/**
  The current boot order can be changed one from remote configuration. This function checks threat in the remote option

  @param    Buffer - Remote boot options buffer
  @param    BufferSize - Size of the buffer 
  
  @retval   EFI_STATUS
**/
/*
EFI_STATUS  CheckThreatForRemoteBootOption (void *Buffer, UINTN BufferSize)
{
    char    *TempBuffer = Buffer;
    char    BootOrderBuffer[16] = {"0"};
    UINTN   BuffIndex = 0;
    UINT16  iIndex = 0;
    UINT16  jIndex = 0;
    UINTN   RemoteBootCount = 0;
    UINTN   TotalBootCount = 0;
    wchar_t BootStr[MAX_LENGTH] = {0};
    CHAR16  *BootStr16WihtNoComma = NULL;
    CHAR8   *BootStr8 = NULL;
    UINT8   *BootXXXX = (UINT8 *)NULL;
    UINT32  Attributes = 0;
    UINTN   BootSize = 0;
    BOOT_OPTION *BootOption = NULL;
    EFI_STATUS Status = EFI_SUCCESS;
    BOOT_OPTION_NAME_INDEX *BootOptionNameIndex = NULL;

    BootOptionNameIndex = AllocateZeroPool (100 * sizeof (BOOT_OPTION_NAME_INDEX));     //Supports only 100 remote options
    if (NULL == BootOptionNameIndex)
    {
        return EFI_OUT_OF_RESOURCES;
    }
    
    for (BuffIndex = 0; BuffIndex < BufferSize; iIndex ++)
    {
        //Getting boot option name
        jIndex = 0;
        while ((',' != TempBuffer[BuffIndex]) && (BuffIndex < BufferSize))
        {
            BootOptionNameIndex[iIndex].BootOptionName [jIndex] = TempBuffer[BuffIndex];
            jIndex ++;
            BuffIndex ++;
        }
        BuffIndex ++;
        
        //Getting boot index
        jIndex = 0;
        memset(BootOrderBuffer, 0, sizeof (BootOrderBuffer));
        while ((',' != TempBuffer[BuffIndex]) && (BuffIndex < BufferSize))
        {
            BootOrderBuffer[jIndex] = TempBuffer[BuffIndex];
            jIndex ++;
            BuffIndex ++;
        }
        BootOptionNameIndex[iIndex].BootOptionIndex = (UINT16)HiiLib_Rbatoi_base (BootOrderBuffer+2, 16);
        BuffIndex ++;
        
        for (; ((';' != TempBuffer[BuffIndex]) && (BuffIndex < BufferSize)); BuffIndex ++);
        BuffIndex ++;
        
        RemoteBootCount ++;
        
    }
    
    //Compare current boot option with remote boot option
    for (iIndex = 0; iIndex < (gBootCount * 2); iIndex += 2) //*2 because gBootCount is UINT8
    {
        swprintf( BootStr, MAX_LENGTH, L"Boot%02X%02X", gBootOrder[iIndex + 1], gBootOrder[iIndex]);
        BootSize = 0;
        BootXXXX = HiiLib_VarGetNvramName( BootStr, &gEfiGlobalVariableGuid, &Attributes, (UINTN*)&BootSize );
        if (NULL == BootXXXX || 0 == BootSize)
        {
            Status = EFI_NOT_FOUND;
            goto DONE;
        }
        BootOption = (BOOT_OPTION *)BootXXXX;    
        for (jIndex = 0; jIndex < RemoteBootCount; jIndex ++)      //We won't send hidden options to server. so checking with RemoteBootCount
        {
            
            if ((*((UINT16 *)(gBootOrder+iIndex))) ==  BootOptionNameIndex[jIndex].BootOptionIndex)
            {
                //compare the boot option name
                BootStr16WihtNoComma = SkipCharInString (BootOption->Name, L",");
                BootStr8 = HiiLib_ConvertChar16toChar8 (BootStr16WihtNoComma);
                if (strcmp (BootStr8, BootOptionNameIndex[jIndex].BootOptionName))
                {
                    Status = EFI_NOT_FOUND;
                    HiiLib_MemFreePointer( (VOID *)&BootStr16WihtNoComma );
                    HiiLib_MemFreePointer( (VOID *)&BootStr8 );
                    goto DONE;
                }
                HiiLib_MemFreePointer( (VOID *)&BootStr16WihtNoComma );
                HiiLib_MemFreePointer( (VOID *)&BootStr8 );
                TotalBootCount ++;
                break;
            }
        }
        if (jIndex == RemoteBootCount)
        {
            //If not also hidden option then boot order chnaged don't import boot option from remote
            if (!(BootOption->Active & LOAD_OPTION_HIDDEN))
            {
                Status = EFI_NOT_FOUND;
                break;
            }
            else
            {
                TotalBootCount ++;
            }
        }
        HiiLib_MemFreePointer( (VOID *)&BootXXXX );
    }
    //If any device removed/added this will be helpful
    if (gBootCount != TotalBootCount)
    {
        Status = EFI_NOT_FOUND;
    }
DONE:
    HiiLib_MemFreePointer(&BootOptionNameIndex);
    return Status;
}
//kuaibai add update <<<
*/
//<kuaibai001-20180813 Fix hang when importing bootorder changes from OOB ->

/**
  Set BootOrder and disable boot option based on the input in the string

  @param 	VOID* 
  @param 	UINTN 
  
  @retval 	VOID
**/
EFI_STATUS  ProcessBootOptions( void *Buffer, UINTN BufferSize )
{
    EFI_STATUS  Status = EFI_UNSUPPORTED;
    char    *TempBuffer = Buffer;
    char    BootOrderBuffer[16] = {"0"};
    UINTN   BuffIndex = 0;
    UINT16  Index = 0;
    GUID    gEfiGlobalVariableGuid = EFI_GLOBAL_VARIABLE_GUID;
    UINT32  Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE;
    wchar_t BootStr[MAX_LENGTH] = {0};
    UINT8   *BootXXXX = (UINT8 *)NULL;
    UINT64  BootSize = 0;
    UINT16	BootIndex = 0;
    UINT32  OptEnabled = 0;
    
    if( NULL == gBootOrder)
        _PopulateGlobalBootOptions();
//<kuaibai001-20180813 Fix hang when importing bootorder changes from OOB +>
//kuaibai add update >>>
//    Status = CheckThreatForRemoteBootOption (Buffer, BufferSize);
//    if (EFI_ERROR (Status))
//    {
//        return EFI_SUCCESS;
//    }
//kuaibai add update <<< 
//<kuaibai001-20180813 Fix hang when importing bootorder changes from OOB ->
	// Format: <BootOptionName>,<BootOrderIndex>,<Enabled/Disabled>;
    for( ; BuffIndex < BufferSize; Index+= 2)
    {
        //skip boot name i.e traverse till first comma
        for( ; ((',' != TempBuffer[BuffIndex]) && (BuffIndex < BufferSize) ) ; BuffIndex++ );

        //traverse till next comma to get modified boot order, change to int and save to global boot order 
        for( BuffIndex++, BootIndex = 0 ; ',' != TempBuffer[BuffIndex] ; BuffIndex++, BootIndex++ )
        {
            BootOrderBuffer[BootIndex] = TempBuffer[BuffIndex];            
        }
            
        BootIndex = (UINT16)HiiLib_Rbatoi_base (BootOrderBuffer+2, 16);
        memcpy( &(gBootOrder[Index]), &BootIndex, sizeof(UINT16));

        if( 0 == strncmp( &TempBuffer[BuffIndex+1], "false", 5) )
            OptEnabled = 0;
        else
        	OptEnabled = 1;

        BootSize = 0;
		swprintf( BootStr, MAX_LENGTH, L"Boot%02X%02X", gBootOrder[Index + 1], gBootOrder[Index]);
		BootXXXX = HiiLib_VarGetNvramName( BootStr, &gEfiGlobalVariableGuid, &Attributes, (UINTN*)&BootSize );
		if ( BootXXXX && BootSize != 0 )
		{
			memcpy(BootXXXX, &OptEnabled, sizeof(OptEnabled));
			Status = HiiLib_VarSetNvramName( BootStr, &gEfiGlobalVariableGuid, Attributes, BootXXXX, (UINTN)BootSize );
			HiiLib_MemFreePointer( (VOID *)&BootXXXX );
		}

		// traverse upto next option
        for( ; ((';' != TempBuffer[BuffIndex]) && (BuffIndex < BufferSize) ) ; BuffIndex++ );
        BuffIndex++;
    }
//<kuaibai001-20180813 Fix hang when importing bootorder changes from OOB +>
//kuaibai add update >>>
    //set new boot order
    //Replaced Index with gBootCount because Index will not have hidden boot options included
    //Assuming always hidden boot options will present at last
    Status = pRS->SetVariable (L"BootOrder", &gEfiGlobalVariableGuid, Attributes, Index , gBootOrder);
	//Status = pRS->SetVariable (L"BootOrder", &gEfiGlobalVariableGuid, Attributes, (gBootCount * sizeof( UINT16 )) , gBootOrder);
//kuaibai add update <<<
//<kuaibai001-20180813 Fix hang when importing bootorder changes from OOB ->
    return Status;
}
/**
    function to dump and return the string

    @param String 

    @retval String

**/
CHAR16 *HiiLib_StrDup( CHAR16 *string )
{
    CHAR16 *text;

    if(string == NULL)
        return NULL;
    
    text = AllocateZeroPool( (1 + Wcslen( string )) * sizeof(CHAR16) );
    if ( text != NULL )
        Wcscpy( text, string );

    return text;
}
/**
    function to perform the esc key operation

    @param string 

    @retval string

**/
CHAR16 *HiiLib_SkipEscCode(CHAR16 *String)
{
    CHAR16    *TempString;
    UINTN     i=0,j=0;

    if (NULL == String) {
        return NULL;    
    }
    // Check For escape Code and remove that from the string
    TempString = HiiLib_StrDup(String);
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
    Parse hex string
    
    @param char *

    @retval unsigned
**/
static unsigned HiiLib_parse_hex4(const char *str)
{
    unsigned h=0;
    
    if (*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && *str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
    h=h<<4;str++;
    if (*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && *str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
    h=h<<4;str++;
    if (*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && *str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
    h=h<<4;str++;
    if (*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && *str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
    return h;
}
/**
    Parse string and remove illegal json characters
    
    @param CHAR8*

    @retval CHAR8*
**/

char *HiiLib_parse_string(char *str)
{
    char *ptr;char *ptr2;char *out;int len=0;unsigned uc,uc2;
    const unsigned char firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

    ptr = str;
    while (*ptr && ++len) if (*ptr++ == '\\')ptr++;;  /* Skip escaped quotes. */
    out=(char*)AllocateZeroPool(len+1); /* This is how long we need for the string, roughly. */
    if (!out) return 0;

    // Skip " and \ Allow \",\\,\b,\f,\n,\r,\t,\uWXYZ, where WXYZ are 4 Hexadecimal digits
    ptr = str;ptr2=out;
    while (*ptr)
    {
        if ( (*ptr!='\\') && (*ptr!='"')) *ptr2++=*ptr++;
        else
        {
            ptr++;
            if( '\0' == *ptr )
                break;
            switch (*ptr)
            {
                case 'b': *ptr2++='\b'; break;
                case 'f': *ptr2++='\f'; break;
                case 'n': *ptr2++='\n'; break;
                case 'r': *ptr2++='\r'; break;
                case 't': *ptr2++='\t'; break;
                case 'u':    /* transcode utf16 to utf8. */
                uc=HiiLib_parse_hex4(ptr+1);ptr+=4;    /* get the unicode char. */
                
                if ((uc>=0xDC00 && uc<=0xDFFF) || uc==0)    break;  /* check for invalid.   */
                
                if (uc>=0xD800 && uc<=0xDBFF)   /* UTF16 surrogate pairs.   */
                {
                    if (ptr[1]!='\\' || ptr[2]!='u')    break;  /* missing second-half of surrogate.    */
                    uc2=HiiLib_parse_hex4(ptr+3);ptr+=6;
                    if (uc2<0xDC00 || uc2>0xDFFF)       break;  /* invalid second-half of surrogate.    */
                    uc=0x10000 + (((uc&0x3FF)<<10) | (uc2&0x3FF));
                }
                
                len=4;if (uc<0x80) len=1;else if (uc<0x800) len=2;else if (uc<0x10000) len=3; ptr2+=len;
                
                switch (len) {
                case 4: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
                case 3: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
                case 2: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
                case 1: *--ptr2 =(uc | firstByteMark[len]);
                }
                ptr2+=len;
                break;
                default:  *ptr2++=*ptr; break;
            }
            ptr++;
        }
    }
    return out;
}

ssize_t  EncodeUtf8(char *Dest, wchar_t ch);
int     OneWcToMcLen(const wchar_t InCh);
/**
    Removes the illegal chars in json for the incoming string

    @param CHAR16 ** -> String to which illegal chars will be removed and returned in same.
    
    @retval EFI_STATUS
**/
EFI_STATUS HiiLib_RemoveSpecialChars (CHAR16 **String)
{
    char    *str, *out;
    CHAR16  *TempStr = *String, ch;
    UINTN   NumChar = 0;
    UINTN iIndex = 0;
    
    //Skip the characters \r and \n
    while ((*String) [iIndex] != L'\0')
    {
        if (((*String) [iIndex] == L'\r' ) || ((*String) [iIndex] == L'\n'))
            (*String) [iIndex] = L' ';
        
        iIndex ++;
    }
    
    //Estimate bytes to represent unicode chars and calculate string length
    for ( ;*TempStr; TempStr++)
    {
        if (*TempStr > 0x80)
            NumChar += OneWcToMcLen(*TempStr);
        else
            NumChar++;
    }
        
    str = AllocateZeroPool(NumChar+1);
    if (!str) return EFI_OUT_OF_RESOURCES;
    out = str;
    
    //Convert unicode char into multibyte character
    for ( TempStr = *String; *TempStr; TempStr++ )
    {
        ch = *TempStr;
        if (ch < 0x80)
            *str++ = (char)ch;
        else
        {
            NumChar = EncodeUtf8(str,ch);
            str+= NumChar;
        }
    }

     str = out;
     out = HiiLib_parse_string(str);
     FreePool (*String);
     *String = HiiLib_ConvertChar8toChar16(out);
     HiiLib_MemFreePointer(&str);   
     HiiLib_MemFreePointer(&out);
     return EFI_SUCCESS;
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
