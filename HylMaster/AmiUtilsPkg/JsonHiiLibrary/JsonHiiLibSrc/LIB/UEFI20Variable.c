
#include "stdlib.h"
#include "wchar.h"
#include "AmiSceCommon.h"
#include "Wrapper.h"
#include "string.h"
#include "Library/UefiLib.h"
#include "AmiDxeLib.h"

#define _EFI 1
#if defined(_LNX) || defined(_BSD) || defined(_EFI)
#include "os_hook.h"
#include "EDKStructures.h"
#endif
#include "EfiCommon.h"
//Added for EDK
#include "NvramLib.h"
#include "UEFI20Variable.h"
#include <Library/DebugLib.h>

// WARNING duplicate definition. Duplicate definition of _VARIABLE causes an
// error if UEFI21Variable.h is included here
VarSetNVRAMVariable( VARIABLE *varInfo, VOID *buffer, UINTN size );
#include "AdvScript.h"
#include "JsonHiiLibraryApi.h"
#if defined(_LNX) || defined(_EFI)
#define _S L"ls"
#endif

VARIABLE *gVariable;

extern CONTROL_RESET_FLAGINFO *gResetControlInfo;
BOOLEAN	gAmiSceReboot = FALSE;

extern EFI_BOOT_SERVICES 		*gBS;

#define	BOOT_MANAGER_GUID \
	{ 0xB4909CF3, 0x7B93, 0x4751, 0x9B, 0xD8, 0x5B, 0xA8, 0x22, 0x0B, 0x9B, 0xB2 }
#define	LEGACY_DEV_ORDER_GUID	\
	{ 0xA56074DB, 0x65FE, 0x45F7, 0xBD, 0x21, 0x2D, 0x2B, 0xDD, 0x8E, 0x96, 0x52 }

#define RFC_3066_LANGUAGE_SIZE 3
#define UEFI_21_VERSION		0x021
#define UEFI_20_VERSION		0x020

typedef struct _SPECIAL_GUID_INFO SPECIAL_GUID_INFO;

struct _SPECIAL_GUID_INFO
{
	EFI_GUID    Guid;
	//CHAR16* Name;
	wchar_t* Name;
	UINT16  Key;
};

struct _VARIABLE
{
	EFI_GUID	VariableGuid;		//Varies	AMI_GUID for the NVRAM variable store for this data
	wchar_t		VariableName[VARIABLE_NAME_LENGTH];	//Varies	Variable store name
	UINT32		VariableAttributes;	//Varies	Attributes of the variable
	UINT32		VariableID;			//Varies	unique variable number, 0x0FFFF for our own variables.
	UINT16		VariableSize;		//Varies	The size of Buffer Varstore}VARIABLE;
	UINT16    VariableIndex;
	UINT8     *VariableBuffer;
	VARIABLE  *Next;
	UINT8      Referenced;          // Indicates referenced by a control
	// additional fields from tse
#define	VARIABLE_ATTRIBUTE_VARSTORE		0x0000	// Buffer varstore accessed via Config. Access protocol exposed by the driver.
#define	VARIABLE_ATTRIBUTE_EFI_VARSTORE	0x1000	// EFI Variable, GetVeriable and SetVariable Can be used.
#define	VARIABLE_ATTRIBUTE_NAMEVALUE	0x2000	// Namevalue varstore accessed via Config. Access protocol exposed by the driver.
#define AMI_SPECIAL_NO_DEFAULTS			0x00000001
#define AMI_SPECIAL_VARIABLE_NO_SET		0x00000002
#define AMI_SPECIAL_VARIABLE_NO_GET		0x00000004
	UINT32		ExtendedAttibutes;	//Varies	Extended Attributes to identify type of Variable.
	UINT16		VariableNameId;		//Varies	Id for the Variable Name
	VOID       *VariableHandle;     // HII handle of formset
};

SPECIAL_GUID_INFO gSpecialGuid[] =
{
	{EFI_GLOBAL_VARIABLE_GUID ,	L"LangCodes",	LANGUAGE_CONTROL},
	{EFI_GLOBAL_VARIABLE_GUID ,	L"PlatformLangCodes",	PLATFORM_LANGUAGE_CONTROL},
	{BOOT_MANAGER_GUID , 		L"BootManager",	BOOT_ORDER_CONTROL},
	{LEGACY_DEV_ORDER_GUID , 		L"LegacyGroup",	LEGACY_DEV_ORDER_CONTROL}
};

extern VOID *GetNVRAMVariable( wchar_t *Name, UINT8 *VendorGuid, UINT32 *Attributes, UINT64 *VarSize );
extern int SetNVRAMVariable( wchar_t *VariableName, UINT8 *VariableGuid, UINT32 Attributes, UINT64 Size, UINT8 *Buffer );

//extern EFI_FILE_HANDLE 	FileSystemHandle;

//extern VOID *GetNVRAMVariable( UINT16 *Name, UINT8 *VendorGuid, UINT32 *Attributes,UINT64 *VarSize );
//extern int SetNVRAMVariable( UINT16 *VariableName, UINT8 *VariableGuid, UINT32 Attributes, UINT64 Size, UINT8 *Buffer );
extern wchar_t *GetGuidAsUnicodeString( UINT8 *GUID );
extern BOOLEAN IsVerbose();
extern UINT8 gUEFIVersion; //UEFI version of BIOS
extern BOOLEAN gUEFI21;// True, if /u switch is present
extern BOOLEAN g_QuietMode;

#define AMI_BASE_BIN        2
#define AMI_BASE_OCT        8
#define AMI_BASE_DEC        10
#define AMI_BASE_HEX        16
#define AMI_BASE_INT_DEC    32

BOOLEAN gUseCache = FALSE;

#if 0
int AddVariable(UINT8 *IFRData)
{
	int Status = EFI_SUCCESS;
	EFI_IFR_OP_HEADER *OpHeader = ( EFI_IFR_OP_HEADER * )IFRData;
	EFI_IFR_FORM_SET *FormSet = ( EFI_IFR_FORM_SET * )NULL;
	EFI_GUID *Guid = ( EFI_GUID * )NULL;
	VARIABLE *Var = ( VARIABLE * )NULL;
	VARIABLE *PrevVar = ( VARIABLE * )NULL;
	EFI_IFR_VARSTORE *Varstore = ( EFI_IFR_VARSTORE * )NULL;

	char *Str = ( char * )NULL;
	wchar_t *VarNamePtr = NULL;
	UINT16 Index = 0;
	UINT16 VarID = 0;
	wchar_t VarName[40] = {0};
	UINT16 Size = 0;
	UINTN Found = 0;
	UINT64 BufferSize = 0;

	switch (OpHeader->OpCode)
	{
		case EFI_IFR_FORM_SET_OP:
			FormSet = ( EFI_IFR_FORM_SET * )IFRData;
			Guid = &( FormSet->Guid );
			wcscpy( (wchar_t*)VarName, L"Setup" );
			break;

		case EFI_IFR_VARSTORE_OP:
			Varstore = ( EFI_IFR_VARSTORE * )IFRData;
			Guid = &(Varstore->Guid);
			VarID = Varstore->VarId;
			Size = Varstore->Size;
			Str = (char*)(( UINT8 * )Varstore + sizeof( EFI_IFR_VARSTORE ));

			//Convert to unicode string
			for (VarNamePtr = VarName; *Str; *VarNamePtr = *Str, Str++, VarNamePtr++);

			break;

		default:
			break;
	}

	// check that the Var is not already in the list
	for ( Var = gVariable, Index = 0; Var; Var = Var->Next, Index++ )
	{
		if ( CompareMem(Guid, &(Var->VariableGuid), sizeof( EFI_GUID )) == 0 ) //Compare Guid
		{
			if ( wcscmp((const wchar_t*)VarName, (const wchar_t*)Var->VariableName) == 0 )
			{
				Found = 1;
				break;
			}
		}

		PrevVar = Var;
	}

	if (!Found)
	{
		Var = ( VARIABLE * )calloc( sizeof( VARIABLE ), sizeof( UINT8 ));

		if (NULL == Var)
		{
			throw (L"Memory Allocation Failure");
		}

		if ( gVariable == NULL )
		{
			gVariable = Var;
		}
		else
		{
			PrevVar->Next = Var;
		}

		Var->VariableIndex = Index;
		Var->VariableID = VarID;
		memcpy(( &Var->VariableGuid), Guid, sizeof( EFI_GUID ));
		Var->VariableAttributes = 0;
		wcscpy( (wchar_t*)Var->VariableName, (const wchar_t*)VarName );
		Var->VariableSize = Size;
	}
	else
	{
		Var->VariableID = VarID;
#if 0
		Var->VariableSize = Size;
#endif
	}

	if ( Var->VariableBuffer == NULL )
	{
		Var->VariableBuffer = ( UINT8 * )GetNVRAMVariable( (wchar_t*)Var->VariableName, ( UINT8 * )&Var->VariableGuid, &Var->VariableAttributes, &BufferSize );

		if ( ( UINT16 )BufferSize != Var->VariableSize )
		{
			//CHAR16 *GuidStr = GetGuidAsUnicodeString(( UINT8 * )&Var->VariableGuid );

			wchar_t *GuidStr = GetGuidAsUnicodeString(( UINT8 * )&Var->VariableGuid );

			Var->VariableSize = ( UINT16 )BufferSize;
			Status = -1;

			if (GuidStr)
			{
				free( GuidStr );
			}
		}
	}

	return Status;
}
#endif

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	GetVarNumFromVarID
//
// Description: Function to get variable number from var ID
//
// Input:		UINT32 ID
//
// Output:		Variable Index
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT32 GetVarNumFromVarID(UINT32 ID)
{
	UINT32 Index = 0;
	VARIABLE *Variable = ( VARIABLE * )gVariable;

	while (Variable)
	{
		if (Variable->VariableID == ID)
		{
			break;
		}

		Index++;
		Variable = Variable->Next;
	}

	return Index;
}

UINT8 *SceGetQuestionValue(UINT16 VariableId, UINT16 Offset, UINT16 Width)
{
	UINTN BufferSize = 0;
	VOID *CacheBuffer = NULL;
	EFI_GUID VarStoreGuid = {0};
	wchar_t VarStoreName[VARIABLE_NAME_LENGTH];
	UINT8 *Buffer = (UINT8 *)NULL;
	VARIABLE *Variable = ( VARIABLE * )gVariable;
	EFI_STATUS Status = EFI_SUCCESS;

	for (; Variable && Variable->VariableIndex != VariableId; Variable = Variable->Next );

	if (gUseCache)
	{
		VarStoreGuid = Variable->VariableGuid;
		wcscpy(VarStoreName, Variable->VariableName);
		BufferSize = Variable->VariableSize;
		CacheBuffer = (VOID*)calloc(BufferSize, sizeof(VOID*));
		Status = HiiLibGetBrowserData(&BufferSize, CacheBuffer, &VarStoreGuid, VarStoreName);

		if ( (CacheBuffer != NULL) && (Status == EFI_SUCCESS))
		{
			if ( Offset + Width <= (UINT16)BufferSize )
			{
				Buffer = ( UINT8 * )calloc( Width, sizeof( UINT8 * ));

				if ( Buffer != NULL )
				{
					memcpy( Buffer, (UINT8 *)CacheBuffer + Offset, Width );
				}
				else
				{
					throw (L"Memory Allocation Failure");
				}
			}

			free(CacheBuffer);
		}
	}
	else
	{
		if ( Variable != NULL )
		{
			if ( Offset + Width <= Variable->VariableSize )
			{
				Buffer = ( UINT8 * )calloc( Width, sizeof( UINT8 * ));

				if ( Buffer != NULL )
				{
					memcpy( Buffer, &Variable->VariableBuffer[Offset], Width );
				}
				else
				{
					throw (L"Memory Allocation Failure");
				}
			}
		}
	}

	return Buffer;
}

VOID SceSetQuestionValue(UINT16 VariableId, UINT16 Offset, UINT16 Width, UINT8 *Buffer)
{
	VARIABLE *Variable = ( VARIABLE * )gVariable;
	UINTN BufferSize = 0;
	VOID *CacheBuffer = NULL;
	VOID *GetCacheBuffer = NULL;
	EFI_GUID VarStoreGuid = {0};
	wchar_t VarStoreName[VARIABLE_NAME_LENGTH];
	EFI_STATUS Status = EFI_SUCCESS;

	for (; Variable && Variable->VariableIndex != VariableId; Variable = Variable->Next );

	wcscpy(VarStoreName, Variable->VariableName);

	if (gUseCache)
	{
		VarStoreGuid = Variable->VariableGuid;
		BufferSize = Variable->VariableSize;

		CacheBuffer = (VOID*)calloc(BufferSize, sizeof(VOID*));

		if ( CacheBuffer != NULL )
		{
			GetCacheBuffer = (VOID*)calloc(BufferSize, sizeof(VOID*));

			if ( GetCacheBuffer == NULL )
			{
				return;
			}

			Status = HiiLibGetBrowserData(&BufferSize, GetCacheBuffer, &VarStoreGuid, VarStoreName);

			if (Status != EFI_SUCCESS)
			{
				memcpy( (UINT8 *)CacheBuffer, Variable->VariableBuffer, BufferSize );
			}
			else
			{
				memcpy( (UINT8 *)CacheBuffer, GetCacheBuffer, BufferSize );
			}

			if ( Offset + Width <= (UINT16)BufferSize )
			{
				if ( Buffer != NULL )
				{
					memcpy( (UINT8 *)CacheBuffer + Offset, Buffer, Width );
					Status = HiiLibSetBrowserData(BufferSize, CacheBuffer, &VarStoreGuid, VarStoreName);
				}
				else
				{
					throw (L"Memory Allocation Failure");
				}
			}

			free(CacheBuffer);
			free(GetCacheBuffer);
		}
	}
	else
	{
		if ( Variable != NULL )
		{
			if ( Offset + Width <= Variable->VariableSize )
			{
				if ( Buffer != NULL )
				{
					memcpy( &Variable->VariableBuffer[Offset], Buffer, Width );
				}
			}
		}
	}
}

UINT64 GetValueFromBuffer(UINT8* Buffer, UINT16 Size)
{
	UINT64 value = 0;

	if ( Buffer )
	{
		switch (Size)
		{
			case 1:
				value = *(UINT8*)Buffer;
				break;

			case 2:
				value = *(UINT16*)Buffer;
				break;

			case 4:
				value = *(UINT32*)Buffer;
				break;

			case 8:
				value = *(UINT64*)Buffer;
				break;
		}
	}

	return value;
}

/**
    function to get the UINT64 numeric value from the value pointer

    @param UINT8 **		- Value buffer
	@param UINT16		- Data width
	@param UINT8	 	- Base 
    
    @retval UINT64

**/
UINT64 GetNumericValueFromBuffer(UINT8* Buffer, UINT16 Size, UINT8 Base)
{
    UINT64 value = 0;

    if ( Buffer )
    {
        switch(Base)
        {
            case AMI_BASE_INT_DEC:
                if(Size == 1)
                {
                    value = *(INT8*)Buffer;
                    break;
                }
                else if(Size == 2)
                {
                    value = *(INT16*)Buffer;
                    break;
                }
                else if(Size == 4)
                {
                    value = *(INT32*)Buffer;
                    break;
                }
                else if(Size == 8)
                {
                    value = *(INT64*)Buffer;
                    break;
                }
            default:
                if(Size == 1)
                {
                   value = *(UINT8*)Buffer;
                   break;
                }
                else if(Size == 2)
                {
                   value = *(UINT16*)Buffer;
                   break;
                }
                else if(Size == 4)
                {
                   value = *(UINT32*)Buffer;
                   break;
                }
                else if(Size == 8)
                {
                   value = *(UINT64*)Buffer;
                   break;
                }
                break;
        }
    }
    
    return value;
}

VOID ImportVariableToNVRAM()
{
	VARIABLE *Variable = gVariable;

	//wprintf(L"\n ImportVariableToNVRAM");
	for ( ; Variable; Variable = Variable->Next )
	{
		UINT32 Attributes = 0;
		UINT64 BufferSize = 0;
		CONTROL_RESET_FLAGINFO *ResetControlInfo = (CONTROL_RESET_FLAGINFO *)NULL;
		UINT8 *Buffer = ( UINT8 * )GetNVRAMVariable( Variable->VariableName, ( UINT8 * )&Variable->VariableGuid, &Attributes, &BufferSize );

		////////////// ignore all except setup ///////////////
		/*if (wcsicmp(L"Setup", Variable->VariableName))
		    return;*/

		if ( CompareMem( Buffer, Variable->VariableBuffer, ( size_t )BufferSize ) != 0 )
		{
			UINT32 Status = 0;
			
			

#if 0
		Status = SetNVRAMVariable( Variable->VariableName, ( UINT8 * )&Variable->VariableGuid,
				Attributes, BufferSize, Variable->VariableBuffer );
#else
        Status = VarSetNVRAMVariable(Variable, Variable->VariableBuffer, (UINTN)BufferSize);
#endif

			if ( Status != 0 && FALSE == g_QuietMode )
			{
			    DEBUG((DEBUG_ERROR, "WARNING : Error in writing variable %s to NVRAM\n", Variable->VariableName));
				//  fwprintf( stderr, L"WARNING : Error in writing variable %"_S L"to NVRAM\n", Variable->VariableName );
			}

		}
	}
}

VARIABLE *GetNextVariableInfo(VARIABLE *Variable)
{
	return (( Variable == NULL ) ? gVariable : Variable->Next);
}

UINT16 GetSpecialKey(UINT32 ControlConditionVar)
{
	VARIABLE *Variable = ( VARIABLE * )NULL;
	UINT16 SpecialValue = 0;
	UINT32 Index = 0;
	UINT32 Count = 0;	//sizeof( gSpecialGuid ) / sizeof( SPECIAL_GUID_INFO );

	Count = (UINT32)(sizeof( gSpecialGuid ) / sizeof( SPECIAL_GUID_INFO ));

	for ( Variable = ( VARIABLE * )gVariable; Variable && Variable->VariableIndex != ControlConditionVar; Variable = Variable->Next )
		;

	for ( Index = 0; Index < Count; Index++ )
	{
		SPECIAL_GUID_INFO *GuidInfo = ( SPECIAL_GUID_INFO * )&gSpecialGuid[Index];

		if ( Variable && (CompareGuid( &Variable->VariableGuid, &GuidInfo->Guid ) == 1) &&
				wcscmp((const wchar_t*) Variable->VariableName, (const wchar_t*)GuidInfo->Name ) == 0 )
		{
			SpecialValue = GuidInfo->Key;
			break;
		}
	}

	return SpecialValue;
}

UINT8 *GetLangCodes(UINT32 ControlConditionVar)
{
	VARIABLE *Variable = ( VARIABLE * )NULL;
	UINT8 *LangCodes = (UINT8 *)NULL;

	for ( Variable = ( VARIABLE * )gVariable; Variable && Variable->VariableIndex != ControlConditionVar; Variable = Variable->Next )
		;

	if ( Variable )
	{
		LangCodes = ( UINT8 * )calloc( sizeof( UINT8 ), Variable->VariableSize + 1 );

		if (LangCodes)
		{
			memcpy( LangCodes, Variable->VariableBuffer, Variable->VariableSize );
		}
		else
		{
			throw (L"Memory Allocation Failure");
		}
	}

	return LangCodes;
}

UINT16 GetUEFI20LangDataWidth()
{
	return RFC_3066_LANGUAGE_SIZE;
}

UINT8 *GetVariableByName(wchar_t *VariableName, UINT64 *Size)
{
	VARIABLE *Variable = (VARIABLE *)NULL;
	UINT8 *VariableData = (UINT8 *)NULL;

	for ( Variable = ( VARIABLE * )gVariable; Variable && wcscmp( (const wchar_t*)Variable->VariableName, (const wchar_t*)VariableName ) != 0; Variable = Variable->Next )
		;

	if ( Variable )
	{
		VariableData = ( UINT8 * )calloc( sizeof( UINT8 ), Variable->VariableSize );

		if ( VariableData != NULL )
		{
			memcpy( VariableData, Variable->VariableBuffer, Variable->VariableSize );
			*Size = Variable->VariableSize;
		}
		else
		{
			throw (L"Memory Allocation Failure");
		}
	}

	return VariableData;
}
UINT16 GetVariableIdByName(wchar_t *VariableName)
{
	VARIABLE *Variable = (VARIABLE *)NULL;
//  UINT8 *VariableData = (UINT8 *)NULL;

	for ( Variable = ( VARIABLE * )gVariable; Variable && wcscmp( (const wchar_t*)Variable->VariableName, (const wchar_t*)VariableName ) != 0; Variable = Variable->Next )
		;

	if (Variable)
	{
		return (UINT16)Variable->VariableID;
	}

	return (UINT16)~0;

}

VARIABLE *GetVariableInfobyName(wchar_t *VariableName)
{
	VARIABLE *Variable = (VARIABLE *)NULL;

	for ( Variable = ( VARIABLE * )gVariable; Variable && wcscmp( (const wchar_t*)Variable->VariableName, (const wchar_t*)VariableName ) != 0; Variable = Variable->Next )
		;

	if (Variable)
	{
		return Variable;
	}

	return NULL;
}

BOOLEAN GetVariableById(UINT32 VariableId, VARIABLE* Var)
{
	VARIABLE *Variable = (VARIABLE *)NULL;
//  UINT8 *VariableData = (UINT8 *)NULL;

	for ( Variable = ( VARIABLE * )gVariable; Variable && (GetVarNumFromVarID(Variable->VariableID) != VariableId); Variable = Variable->Next )
		;

	if (Variable)
	{
		wcscpy((wchar_t*)Var->VariableName, (const wchar_t*)Variable->VariableName);
		memcpy(&Var->VariableGuid, &Variable->VariableGuid, sizeof(EFI_GUID));
		Var->VariableSize = Variable->VariableSize;
		Var->Referenced = Variable->Referenced;
		return 1;
	}

	return 0;
}

BOOLEAN
MarkVarReferenced(UINT32 VariableId)
{
	VARIABLE *Variable = (VARIABLE *)NULL;

	for ( Variable = ( VARIABLE * )gVariable; Variable && (GetVarNumFromVarID(Variable->VariableID) != VariableId); Variable = Variable->Next )
		;

	if (Variable)
	{
		Variable->Referenced = 1;
		return 1;
	}

	return 0;
}

UINT16 GetVariableIndexByName(wchar_t *VariableName)
{
	VARIABLE *Variable = (VARIABLE *)NULL;
//  UINT8 *VariableData = (UINT8 *)NULL;

	for ( Variable = ( VARIABLE * )gVariable; Variable && wcscmp( Variable->VariableName, VariableName ) != 0; Variable = Variable->Next )
		;

	if (Variable)
	{
		return ( UINT16 )Variable->VariableIndex;
	}

	return 0;

}
