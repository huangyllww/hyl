//extern "C"
//{
#include "string.h"
#include "stdlib.h"
#include "Efi.h"
#include "Wrapper.h"
//}
//#include "memory.h"
//#include "stdio.h"
//#include "types.h"
#define _EFI 1

#if defined(_LNX) || defined(_BSD) || defined(_EFI)
#include "os_hook.h"
#include "NvramLib.h"
#endif

#include "UEFI21HIIParser.h"
#include "UEFI21Variable.h"
#include "EDKStructures21.h"
#include "Library/MemoryAllocationLib.h"
#include "AmiDxeLib.h"
#include <wchar.h>
#include <Library/DebugLib.h>
#include <Token.h>

//#include "MEMMGR.h"

//#include "malloc.h"


#define HII_TSE_GET_NUMERIC_BASE(a) \
    ((( ((EFI_IFR_NUMERIC*)a)->Flags&EFI_IFR_DISPLAY)==EFI_IFR_DISPLAY_UINT_HEX)?AMI_BASE_HEX:(((((EFI_IFR_NUMERIC*)a)->Flags&EFI_IFR_DISPLAY)==EFI_IFR_DISPLAY_INT_DEC) ? AMI_BASE_INT_DEC :AMI_BASE_DEC))

#define AMI_BASE_BIN        2
#define AMI_BASE_OCT        8
#define AMI_BASE_DEC        10
#define AMI_BASE_HEX        16
#define AMI_BASE_INT_DEC    32

UINT64 gHiiDataSize = 0;
UINT8 *gHiiDBData = (UINT8 *)NULL;

static UINT32 ScopeCount = 0;   //UINTN to UINT32
static UINT32 FormScopeCount = 0; ////UINTN to UINT32
static UINT8 *gCurrentFormSet;

UINT32 _IsAMISpecialControl( UINT8 * HiiData, UINT32 *ConditionalVarId );

extern UINT16 GetSpecialKey(UINT32 ControlConditionVar);
extern UINT16 GetVariableIndexByName(wchar_t *VariableName);
extern UINT32 GetVarNumFromVarID( UINT32 ID );
extern long *g_PkgFilePos;
extern UINT16 g_PkgCount;
extern long g_pkglen;
void WriteSubTitleToFile(HII_DATA *AmisceData,UINT8 *HiiData , UINT8 *DataTable);
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	_AddUefiFormSetVariable
//
// Description:
//
// Input:	UINT8 *HiiData
//
// Output:	VOID
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID _AddUefiFormSetVariable(UINT8 *HiiData)
{
	AddUefiVariable( HiiData );
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	_GetUefiControlDataOffset
//
// Description:
//
// Input:	UINT8 *HiiData
//
// Output:	UINT16
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT16 _GetUefiControlDataOffset(UINT8 * HiiData)
{
  EFI_IFR_OP_HEADER *OpHeader = ( EFI_IFR_OP_HEADER * )HiiData;
  UINT16 QuestionId = 0;

  switch(OpHeader->OpCode)
  {
	  case EFI_IFR_ONE_OF_OP:
		QuestionId = (( EFI_IFR_ONE_OF * )HiiData)->TSE_COMMON_QUESTION_ID;
		break;
	  case EFI_IFR_CHECKBOX_OP:
		QuestionId = (( EFI_IFR_CHECKBOX * )HiiData)->TSE_COMMON_QUESTION_ID;
		break;
	  case EFI_IFR_DATE_OP:
		QuestionId = (( EFI_IFR_DATE * )HiiData)->TSE_COMMON_QUESTION_ID;
		break;
	  case EFI_IFR_TIME_OP:
		QuestionId = (( EFI_IFR_TIME * )HiiData)->TSE_COMMON_QUESTION_ID;
		break;
	  case EFI_IFR_NUMERIC_OP:
		QuestionId = (( EFI_IFR_NUMERIC * )HiiData)->TSE_COMMON_QUESTION_ID;
		break;
	  case EFI_IFR_STRING_OP:
		QuestionId = (( EFI_IFR_STRING * )HiiData)->TSE_COMMON_QUESTION_ID;
		break;
	  case EFI_IFR_ORDERED_LIST_OP:
		QuestionId = (( EFI_IFR_ORDERED_LIST * )HiiData)->TSE_COMMON_QUESTION_ID;
		break;
      case EFI_IFR_PASSWORD_OP:
		QuestionId = (( EFI_IFR_PASSWORD * )HiiData)->TSE_COMMON_QUESTION_ID;
	  default:
		break;
  }

  return QuestionId;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   _GetUefiMaxEntries
//
// Description:
//
// Input:   UINT8 *HiiData
//
// Output:  UINT16
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT8 _GetUefiMaxEntries(UINT8 * HiiData)
{
    EFI_IFR_OP_HEADER *OpHeader=(EFI_IFR_OP_HEADER *)HiiData;
    UINT8 Value = 0;
    
    switch(OpHeader->OpCode)
    {
        case EFI_IFR_ORDERED_LIST_OP:
            Value = ((EFI_IFR_ORDERED_LIST*)OpHeader)->MaxContainers;
            break;
        default:
            break;
    }
    return Value;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	_GetUefiControlDataWidth
//
// Description:
//
// Input:	UINT8 *HiiData
//
// Output:	UINT16
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT16 _GetUefiControlDataWidth(UINT8 * HiiData)
{
	EFI_IFR_OP_HEADER *OpHeader = ( EFI_IFR_OP_HEADER * )HiiData;
	UINT16 Width = 0;
	
    if(OpHeader->OpCode == EFI_IFR_ORDERED_LIST_OP)
    {
        EFI_IFR_ONE_OF_OPTION *OneOfOptionOp = NULL;
        
        do
        {
            OpHeader = (EFI_IFR_OP_HEADER*)(HiiData + OpHeader->Length);
        }while(OpHeader->OpCode != EFI_IFR_ONE_OF_OPTION_OP);

        OneOfOptionOp = (EFI_IFR_ONE_OF_OPTION*)OpHeader;

        switch(OneOfOptionOp->Type)
        {
            case EFI_IFR_TYPE_NUM_SIZE_16:
                Width = sizeof(UINT16);
                break;
            case EFI_IFR_TYPE_NUM_SIZE_32:
                Width = sizeof(UINT32);
                break;
            case EFI_IFR_TYPE_NUM_SIZE_64:
                Width = sizeof(UINT64);
                break;
            case EFI_IFR_TYPE_NUM_SIZE_8:
            default:
                Width = sizeof(UINT8);
                break;
        }
    }
    else if(OpHeader->OpCode == EFI_IFR_DATE_OP || OpHeader->OpCode == EFI_IFR_TIME_OP)
    {
        switch(OpHeader->OpCode)
        {
            case EFI_IFR_DATE_OP:
                Width = sizeof(EFI_HII_DATE);
                break;
            case EFI_IFR_TIME_OP:
                Width = sizeof(EFI_HII_TIME);
                break;
        }
    }
    else
    {
        Width = ( UINT16 )UefiGetWidth( OpHeader );
    }
    
	return Width;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    FindScopeEnd
//
// Description:  Finds the end of scope for IFR operator
//
// Parameters:      BYTE    *Control
//                      [IN] Pointer to IFR operation
//
// Return value:    UINT8 *: Pointer to scope closing opcode
//                          Points to next operator if not found
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT8 *
FindScopeEnd(UINT8 *Control)
{
    EFI_IFR_OP_HEADER *Header;
    int Scope;

    Header = (EFI_IFR_OP_HEADER *)Control;
    Control += Header->Length;
    for (Scope = Header->Scope; Scope; Control += Header->Length)
    {
        Header = (EFI_IFR_OP_HEADER *)Control;
        if (Header->OpCode == EFI_IFR_END_OP)
            Scope--;
        else if (Header->Scope)
            Scope++;
    }
    return (UINT8 *)Control;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	_GetUefiControlOption
//
// Description:
//
// Input:	UINT8 *HiiData, VALUE_INFO **ValueInfo
//
// Output:	VOID
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID _GetUefiControlOption(UINT8 *HiiData, VALUE_INFO **ValueInfo )
{
	EFI_IFR_OP_HEADER *OpHeader = ( EFI_IFR_OP_HEADER * )HiiData;
	EFI_IFR_ONE_OF_OPTION *OneOf = ( EFI_IFR_ONE_OF_OPTION * )NULL;
	OPTION_LIST *Option = (OPTION_LIST *)NULL;
    UINT8   *EndScope;    // address of end of scope op
    OPTION_LIST *Temp;

	if( OpHeader->OpCode != EFI_IFR_ONE_OF_OP &&
		OpHeader->OpCode != EFI_IFR_ORDERED_LIST_OP )
	{
		return;
	}
    EndScope = FindScopeEnd(HiiData);

	OneOf = ( EFI_IFR_ONE_OF_OPTION * )(( UINT8 * )OpHeader + OpHeader->Length );

	while( OneOf->Header.OpCode == EFI_IFR_ONE_OF_OPTION_OP || (UINT8 *)OneOf < EndScope)
	{
        if (OneOf->Header.OpCode != EFI_IFR_ONE_OF_OPTION_OP)
        {
            OneOf = ( EFI_IFR_ONE_OF_OPTION * )(( UINT8 * )OneOf + OneOf->Header.Length );
            continue;
        }
		Temp = ( OPTION_LIST * )calloc(sizeof( OPTION_LIST ), sizeof( UINT8 ));

		if( Temp == NULL )
		{
			 throw(L"Memory Allocation Failure");//MemAllocFailure();
		}

		Temp->Option = OneOf->Option;
		Temp->Value = ( UINT16 )TSE_GET_ONE_OF_OPTION_VALUE( OneOf );
		Temp->Flag = OneOf->Flags;

		if( Option == NULL )
		{
			Option = Temp;
			( *ValueInfo )->Options = Option;
		}else
		{
			Option->Next = Temp;
			Option = Option->Next;
		}
		OneOf = ( EFI_IFR_ONE_OF_OPTION * )(( UINT8 * )OneOf + OneOf->Header.Length );
	}
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	_GetUefiControlDefaults
//
// Description:
//
// Input:	UINT8 *HiiData, CONTROL_INFO *Control
//
// Output:	UINT16
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID _GetUefiControlDefaults(UINT8 *HiiData, VALUE_INFO **ValueInfo )
{
	EFI_IFR_OP_HEADER *OpHeader = ( EFI_IFR_OP_HEADER * )HiiData;
//	VALUE_INFO *Value = ( VALUE_INFO * )*ValueInfo;
//	UINT8 Flags = 0;

	switch( OpHeader->OpCode )
	{
	  case EFI_IFR_ONE_OF_OP:
		_GetUefiControlOption( HiiData, ValueInfo );
		break;
	  case EFI_IFR_DATE_OP:
		break;
	  case EFI_IFR_TIME_OP:
		break;
	  case EFI_IFR_CHECKBOX_OP:
		// Defaults willbe retrieved from StdDefaults and MfgDefaults NVRAM variables
		/*Flags = (( EFI_IFR_CHECK_BOX * )HiiData )->Flags;
		Value->OptimalDefaults = (Flags & EFI_IFR_FLAG_DEFAULT)? 1 : 0;
		Value->FailSafeDefaults = ( Flags & EFI_IFR_FLAG_MANUFACTURING )? 1 : 0;*/
		break;
	  case EFI_IFR_NUMERIC_OP:
		// Value->FailSafeDefaults = Value->OptimalDefaults = (( EFI_IFR_NUMERIC * )HiiData )->Default;
		break;
	  case EFI_IFR_STRING_OP:
		break;
	  case EFI_IFR_ORDERED_LIST_OP:
		_GetUefiControlOption( HiiData, ValueInfo );
		break;
	  default:
		break;
	}
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	_GetControlHelp
//
// Description:
//
// Input:	UINT8 *ifr
//
// Output:	UINT16
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT16 _GetUefiControlHelp( UINT8 *ifr )
{
	EFI_IFR_OP_HEADER *headerPtr = (EFI_IFR_OP_HEADER *)ifr;
	UINT16 token = 0;

	switch ( headerPtr->OpCode )
	{
		case EFI_IFR_TEXT_OP:
			{
				EFI_IFR_TEXT *ptr = (EFI_IFR_TEXT *)headerPtr;
				token = ptr->TSE_TEXT_HELP;
			}
			break;
		case EFI_IFR_ORDERED_LIST_OP:
			{
				EFI_IFR_ORDERED_LIST *ptr = (EFI_IFR_ORDERED_LIST *)headerPtr;
				token = ptr->TSE_COMMON_HELP;
			}
			break;
		case EFI_IFR_ONE_OF_OP:
			{
				EFI_IFR_ONE_OF *ptr = (EFI_IFR_ONE_OF *)headerPtr;
				token = ptr->TSE_COMMON_HELP;
			}
			break;
		case EFI_IFR_CHECKBOX_OP:
			{
				EFI_IFR_CHECKBOX *ptr = (EFI_IFR_CHECKBOX *)headerPtr;
				token = ptr->TSE_COMMON_HELP;
			}
			break;
		case EFI_IFR_DATE_OP:
			{
				EFI_IFR_DATE *ptr = (EFI_IFR_DATE *)headerPtr;
				token = ptr->TSE_COMMON_HELP;
			}
			break;
		case EFI_IFR_TIME_OP:
			{
				EFI_IFR_TIME *ptr = (EFI_IFR_TIME *)headerPtr;
				token = ptr->TSE_COMMON_HELP;
			}
			break;
		case EFI_IFR_NUMERIC_OP:
			{
				EFI_IFR_NUMERIC *ptr = (EFI_IFR_NUMERIC *)headerPtr;
				token = ptr->TSE_COMMON_HELP;
			}
			break;
		case EFI_IFR_STRING_OP:
			{
				EFI_IFR_STRING *ptr = (EFI_IFR_STRING *)headerPtr;
				token = ptr->TSE_COMMON_HELP;
			}
			break;
		case EFI_IFR_PASSWORD_OP:
			{
				EFI_IFR_PASSWORD *ptr = (EFI_IFR_PASSWORD *)headerPtr;
				token = ptr->TSE_COMMON_HELP;
			}
			break;
		default:
			break;
	}
	return token;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	_GetUefiControlKey
//
// Description:
//
// Input:	UINT8 *HiiData
//
// Output:	UINT16
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

UINT16 _GetUefiControlKey(UINT8 *HiiData)
{
	EFI_IFR_OP_HEADER *OpHeader = ( EFI_IFR_OP_HEADER * )HiiData;
	UINT16 Key = 0;

	switch(OpHeader->OpCode)
	{
		case EFI_IFR_ONE_OF_OP:
			Key = (( EFI_IFR_ONE_OF * )HiiData)->Question.QuestionId;
			break;
		case EFI_IFR_CHECKBOX_OP:
			Key = (( EFI_IFR_CHECKBOX * )HiiData)->Question.QuestionId;
			break;
		case EFI_IFR_DATE_OP:
			Key = (( EFI_IFR_DATE * )HiiData)->Question.QuestionId;
			break;
		case EFI_IFR_TIME_OP:
			Key = (( EFI_IFR_TIME * )HiiData)->Question.QuestionId;
			break;
		case EFI_IFR_NUMERIC_OP:
			Key = (( EFI_IFR_NUMERIC * )HiiData)->Question.QuestionId;
			break;
		case EFI_IFR_STRING_OP:
			Key = (( EFI_IFR_STRING * )HiiData)->Question.QuestionId;
			break;
		case EFI_IFR_ORDERED_LIST_OP:
			Key = (( EFI_IFR_ORDERED_LIST * )HiiData)->Question.QuestionId;
			break;
		case EFI_IFR_PASSWORD_OP:
			Key = (( EFI_IFR_PASSWORD * )HiiData)->Question.QuestionId;
			break;
		default:
			break;
	}

	return Key;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	_GetUefiControlPrompt
//
// Description:
//
// Input:	UINT8 *ifr
//
// Output:	UINT16
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT16 _GetUefiControlPrompt( UINT8 *ifr )
{
	EFI_IFR_OP_HEADER *headerPtr = (EFI_IFR_OP_HEADER *)ifr;
	UINT16 token = 0;

	switch ( headerPtr->OpCode )
	{
		case EFI_IFR_TEXT_OP:
			{
				EFI_IFR_TEXT *ptr = (EFI_IFR_TEXT *)headerPtr;
				token = ptr->TSE_TEXT_TEXT;
			}
			break;
		case EFI_IFR_ONE_OF_OP:
			{
				EFI_IFR_ONE_OF *ptr = (EFI_IFR_ONE_OF *)headerPtr;
				token = ptr->TSE_COMMON_PROMPT;
			}
			break;
		case EFI_IFR_CHECKBOX_OP:
			{
				EFI_IFR_CHECKBOX *ptr = (EFI_IFR_CHECKBOX *)headerPtr;
				token = ptr->TSE_COMMON_PROMPT;
			}
			break;
		case EFI_IFR_DATE_OP:
			{
				EFI_IFR_DATE *ptr = (EFI_IFR_DATE *)headerPtr;
				token = ptr->TSE_COMMON_PROMPT;
			}
			break;
		case EFI_IFR_TIME_OP:
			{
				EFI_IFR_TIME *ptr = (EFI_IFR_TIME *)headerPtr;
				token = ptr->TSE_COMMON_PROMPT;
			}
			break;
		case EFI_IFR_NUMERIC_OP:
			{
				EFI_IFR_NUMERIC *ptr = (EFI_IFR_NUMERIC *)headerPtr;
				token = ptr->TSE_COMMON_PROMPT;
			}
			break;
		case EFI_IFR_STRING_OP:
			{
				EFI_IFR_STRING *ptr = (EFI_IFR_STRING *)headerPtr;
				token = ptr->TSE_COMMON_PROMPT;
			}
			break;

		case EFI_IFR_ORDERED_LIST_OP:
			{
				EFI_IFR_ORDERED_LIST *ptr = (EFI_IFR_ORDERED_LIST *)headerPtr;
				token = ptr->TSE_COMMON_PROMPT;
			}
			break;
		case EFI_IFR_SUBTITLE_OP:
			{
				EFI_IFR_SUBTITLE *ptr = (EFI_IFR_SUBTITLE *)headerPtr;
				token = ptr->Statement.Prompt;
			}
			break;
		case EFI_IFR_PASSWORD_OP:
			{
				EFI_IFR_PASSWORD *ptr = (EFI_IFR_PASSWORD *)headerPtr;
				token = ptr->Question.Header.Prompt;
			}
			break;
		default:
			break;
	}

	return token;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	_GetUefiMaximumValue
//
// Description:
//
// Input:	UINT8 *HiiData
//
// Output:	UINT16
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT64 _GetUefiMaximumValue(UINT8 *HiiData)
{
  EFI_IFR_OP_HEADER *OpHeader = ( EFI_IFR_OP_HEADER * )HiiData;
  UINT64 MaxVal = 0;

  switch( OpHeader->OpCode )
  {
	  case EFI_IFR_NUMERIC_OP:
		MaxVal = ( UINT64 )TSE_GET_NUMERIC_MAXVALUE( HiiData );
		break;
	  case EFI_IFR_STRING_OP:
		MaxVal = (( EFI_IFR_STRING * )HiiData )->MaxSize;
		break;
      case EFI_IFR_PASSWORD_OP:
        MaxVal = (( EFI_IFR_PASSWORD *)HiiData )->MaxSize;
		break;
	  default:
		break;
  }

  return MaxVal;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	_GetUefiMinimumValue
//
// Description:
//
// Input:	UINT8 *HiiData
//
// Output:	UINT16
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT64 _GetUefiMinimumValue(UINT8 *HiiData)
{
	EFI_IFR_OP_HEADER *OpHeader = ( EFI_IFR_OP_HEADER * )HiiData;
	UINT64 MinVal = 0;

	switch( OpHeader->OpCode )
	{
		case EFI_IFR_NUMERIC_OP:
			MinVal = ( UINT64 )TSE_GET_NUMERIC_MINVALUE( HiiData );
			break;
		case EFI_IFR_STRING_OP:
			MinVal = (( EFI_IFR_STRING * )HiiData )->MinSize;
			break;
        case EFI_IFR_PASSWORD_OP:
            MinVal = (( EFI_IFR_PASSWORD *)HiiData )->MinSize;
		default:
		break;
	}

	return MinVal;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	_GetUefiControlValueInfo
//
// Description:
//
// Input:	UINT8 *HiiData, VALUE_INFO **ValueInfo
//
// Output:	VOID
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID _GetUefiControlValueInfo(UINT8 *HiiData, VALUE_INFO **ValueInfo)
{
  VALUE_INFO *Value = (VALUE_INFO *)NULL;


  Value = ( VALUE_INFO * )calloc( sizeof( VALUE_INFO ), sizeof( UINT8 ));
  if( Value == NULL )
  {
	   throw(L"Memory Allocation Failure");//MemAllocFailure();
  }

  Value->MinValue = _GetUefiMinimumValue( HiiData );
  Value->MaxValue = _GetUefiMaximumValue( HiiData );
  _GetUefiControlDefaults( HiiData, &Value );
  *ValueInfo = Value;
  return;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	_GetUefiControlVarStorage
//
// Description:
//
// Input:	UINT8 *HiiData
//
// Output:	VOID
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

UINT32 _GetUefiControlVarStorage( UINT8 *HiiData )
{
	EFI_IFR_OP_HEADER *OpHeader = (EFI_IFR_OP_HEADER *)HiiData;
	UINT32 VarID = 0;

	switch( OpHeader->OpCode )
	{
		case EFI_IFR_ONE_OF_OP:
			VarID = (( EFI_IFR_ONE_OF * )HiiData)->Question.VarStoreId;
			break;
		case EFI_IFR_CHECKBOX_OP:
			VarID = (( EFI_IFR_CHECKBOX * )HiiData)->Question.VarStoreId;
			break;
		case EFI_IFR_DATE_OP:
			VarID = (( EFI_IFR_DATE * )HiiData)->Question.VarStoreId;
			break;
		case EFI_IFR_TIME_OP:
			VarID = (( EFI_IFR_TIME * )HiiData)->Question.VarStoreId;
			break;
		case EFI_IFR_NUMERIC_OP:
			VarID = (( EFI_IFR_NUMERIC * )HiiData)->Question.VarStoreId;
			break;
		case EFI_IFR_STRING_OP:
			VarID = (( EFI_IFR_STRING * )HiiData)->Question.VarStoreId;
			break;
		case EFI_IFR_ORDERED_LIST_OP:
			VarID = (( EFI_IFR_ORDERED_LIST * )HiiData)->Question.VarStoreId;
			break;
		case EFI_IFR_PASSWORD_OP:
			VarID = (( EFI_IFR_PASSWORD * )HiiData)->Question.VarStoreId;
		default:
			break;
	}
	return VarID;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	_GetUefiFlagsValue
//
// DescriptioneSUBTITLE
//
// Input:	UINT8 *HiiData
//
// Output:	UINT16
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT8 _GetUefiFlagsValue(UINT8 *HiiData)
{
	EFI_IFR_OP_HEADER *OpHeader = ( EFI_IFR_OP_HEADER * )HiiData;
	UINT8 Flags = 0;

	switch( OpHeader->OpCode )
	{
		case EFI_IFR_ONE_OF_OP:
			Flags = (( EFI_IFR_ONE_OF * )HiiData)->Question.Flags;
			break;
		case EFI_IFR_CHECKBOX_OP:
			Flags = (( EFI_IFR_CHECKBOX * )HiiData)->Question.Flags;
			break;
		case EFI_IFR_DATE_OP:
			Flags = (( EFI_IFR_DATE * )HiiData)->Question.Flags;
			break;
		case EFI_IFR_TIME_OP:
			Flags = (( EFI_IFR_TIME * )HiiData)->Question.Flags;
			break;
		case EFI_IFR_NUMERIC_OP:
			Flags = (( EFI_IFR_NUMERIC * )HiiData)->Question.Flags;
			break;
		case EFI_IFR_STRING_OP:
			Flags = (( EFI_IFR_STRING * )HiiData)->Question.Flags;
			break;
		case EFI_IFR_ORDERED_LIST_OP:
			Flags = (( EFI_IFR_ORDERED_LIST * )HiiData)->Question.Flags;
			break;
		case EFI_IFR_PASSWORD_OP:
			Flags = (( EFI_IFR_PASSWORD * )HiiData)->Question.Flags;
		default:
		break;
	}

	return Flags;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	GetTotalPackageList
//
// Description:
//
// Input:	UINT8 *HiiData
//
// Output:	UINT16
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

UINT16 GetTotalPackageList( UINT8 *HiiData )
{
	UINT16 Count = 0;
	EFI_HII_PACKAGE_LIST_HEADER* PackageList = ( EFI_HII_PACKAGE_LIST_HEADER* ) HiiData;
	UINT32 Offset = 0;

	if( PackageList )
	{
		while( Offset <= gHiiDataSize )
		{
			if( PackageList->PackageLength )
				Offset += PackageList->PackageLength;
			else
				break;

			++Count;

			if(( Offset + sizeof( EFI_HII_PACKAGE_LIST_HEADER )) < gHiiDataSize )
				PackageList = ( EFI_HII_PACKAGE_LIST_HEADER* ) (( UINT8* )HiiData + Offset );
			else
				break;
		}
	}
	return Count;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	GetNextPackageList
//
// Description:
//
// Input:	UINT8 * HiiData, UINT8 **CurrentPackageList
//
// Output:	UINT8*
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

UINT8 *GetNextPackageList( UINT8 * HiiData, UINT8 **CurrentPackageList )
{
	if( *CurrentPackageList != NULL )
	{
		*CurrentPackageList = ( UINT8 * )( *CurrentPackageList + (( EFI_HII_PACKAGE_LIST_HEADER* )*CurrentPackageList )->PackageLength );
	}
	else
	{
		*CurrentPackageList = HiiData;
	}
	return *CurrentPackageList;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	GetNextUefiFormSet
//
// Description:
//
// Input:	UINT8 *CurrentPackageList, UINT8 **CurrentFormSet, UINT8 *Guid,
//				UINT16 *FormSetTitle, UINT16 *FormSetHelp, UINT16 *Class, UINT16 *SubClass
//
// Output:	UINT8*
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

UINT8 *GetNextUefiFormSet( UINT8 *CurrentPackageList, UINT8 **CurrentFormSet, UINT8 *Guid,
                      UINT16 *FormSetTitle, UINT16 *FormSetHelp, UINT16 *Class, UINT16 *SubClass )
{
	EFI_IFR_OP_HEADER *OpHeader = (EFI_IFR_OP_HEADER *)NULL;
	UINT32 OpLength = 0;

    *Class = 0;
    *SubClass = 0;
    gCurrentFormSet = (UINT8 *)NULL;
	if( CurrentPackageList )
	{
#if 0   // code for multiple formsets in package never implemented -- probably can't happen
        // causes failure to find form set if caller forgets to 0
		if(NULL == *CurrentFormSet)
#endif
		{
			EFI_HII_PACKAGE_HEADER* PackageHeader = ( EFI_HII_PACKAGE_HEADER* )	(( UINT8* )CurrentPackageList +
																						sizeof( EFI_HII_PACKAGE_LIST_HEADER ));
			EFI_IFR_FORM_SET *FormSet = ( EFI_IFR_FORM_SET * )NULL;
			UINT32 PackageLength = (( EFI_HII_PACKAGE_LIST_HEADER* )CurrentPackageList )->PackageLength;
			UINT32 Offset = sizeof( EFI_HII_PACKAGE_LIST_HEADER );
            UINT32 IfrPackLength = 0;
            UINT32 IfrPackOffset = 0;       // offset within package list

			while(( PackageHeader->Type != EFI_HII_PACKAGE_END ) && ( Offset < PackageLength))
			{   // find the IFR package
				if( PackageHeader->Type != EFI_HII_PACKAGE_FORMS )
				{
					if( PackageHeader->Length )
						Offset += PackageHeader->Length;
					else
					{
						return (UINT8 *)NULL;
					}
					PackageHeader = ( EFI_HII_PACKAGE_HEADER* )	((UINT8*)CurrentPackageList + Offset );
					continue;
				}
                IfrPackLength = ((EFI_HII_PACKAGE_HEADER *)(UINT8 *)(CurrentPackageList + Offset))->Length;
                IfrPackOffset = Offset;
                Offset += sizeof(EFI_HII_PACKAGE_HEADER);
				OpHeader = (EFI_IFR_OP_HEADER *)NULL;
				OpLength = 0;

				OpLength = sizeof( EFI_HII_PACKAGE_HEADER );
				OpHeader = ( EFI_IFR_OP_HEADER * )((( UINT8 *)PackageHeader ) + OpLength );
				if( OpHeader->OpCode != EFI_IFR_FORM_SET_OP )
				{
					return (UINT8 *)NULL;
				}
				else
				{
					FormSet = ( EFI_IFR_FORM_SET * )((( UINT8* )PackageHeader ) + OpLength );
					break;
				}
			}

			if( FormSet )
			{
				// Get class and subclass for the Formset
				BOOLEAN Flag = TRUE;
				EFI_IFR_OP_HEADER *OpHeader = ( EFI_IFR_OP_HEADER* )FormSet;

				while( Flag )
				{
					//[EIP:99563] If this exceeds the package length, then return
					if (Offset >= PackageLength)
						return (UINT8 *)NULL;
                    if (Offset == IfrPackOffset + IfrPackLength)
                        break; // this pack does not have class and subclass

					switch( OpHeader->OpCode )
					{
						case EFI_IFR_GUID_OP:
						{
							UINT8 ExtOpcode = ( UINT8 )*((( UINT8* )OpHeader ) + sizeof( EFI_IFR_GUID ));
							switch( ExtOpcode )
							{
								case EFI_IFR_EXTEND_OP_CLASS:
								{
									EFI_IFR_GUID_CLASS *GuidClass = ( EFI_IFR_GUID_CLASS* )OpHeader;
									*Class = GuidClass->Class;
								}
								break;
								case EFI_IFR_EXTEND_OP_SUBCLASS:
								{
									EFI_IFR_GUID_SUBCLASS *GuidSubClass = ( EFI_IFR_GUID_SUBCLASS* )OpHeader;
									*SubClass = GuidSubClass->SubClass;
									Flag = FALSE;
								}
								break;
							}
						}
						default:
							//[EIP:99563] If the length is zero, then return to avoid loop
							if (OpHeader->Length == 0)
								return (UINT8 *)NULL;
							Offset += OpHeader->Length;
							OpHeader = ( EFI_IFR_OP_HEADER* )((( UINT8* )OpHeader ) + OpHeader->Length );
							break;
					}
				}
#if 0   // form set will not always have class
				if( *Class )
#endif
				{
					_AddUefiFormSetVariable( ( UINT8 * )FormSet );

					// Get FormSet Data
					memcpy( Guid, &FormSet->Guid, sizeof( EFI_GUID ));
					*FormSetTitle = FormSet->FormSetTitle;
					*FormSetHelp = FormSet->Help;
					gCurrentFormSet = *CurrentFormSet = ( UINT8 * )FormSet;

					return *CurrentFormSet;
				}
			}
		}
#if 0
		else
		{
			// Is it possible to have more than one FormSet in an Export Package?
		}
#endif
	}

	return (UINT8 *)NULL;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	GetNextUefiForm
//
// Description:
//
// Input:	UINT8 *FormSet, UINT8 **CurrentForm, UINT16 *FormID, UINT16 *FormTitle
//
// Output:	UINT8 *
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

UINT8 *GetNextUefiForm( UINT8 *FormSet, UINT8 **CurrentForm, UINT16 *FormID, UINT16 *FormTitle )
{
  EFI_IFR_OP_HEADER *OpHeader = ( EFI_IFR_OP_HEADER * )NULL;
//  BOOLEAN EndForm = FALSE;
//  UINT8 *FormBegin = (UINT8 *)NULL;

  *FormID = *FormTitle = 0;
  OpHeader = ( *CurrentForm == NULL )? ( EFI_IFR_OP_HEADER * )FormSet : ( EFI_IFR_OP_HEADER * )*CurrentForm;


  if(*CurrentForm == NULL)
  {
    ScopeCount = 0;
  }

  do
  {
    //Skip Until Next Form
    switch( OpHeader->OpCode )
    {
		case EFI_IFR_VARSTORE_OP:
		case EFI_IFR_VARSTORE_EFI_OP:
		//EFI_IFR_VARSTORE_NAME_VALUE_OP not needed for test version
		  AddUefiVariable(( UINT8 * )OpHeader);
		  break;
        case EFI_IFR_VARSTORE_NAME_VALUE_OP:
		  break;
		case EFI_IFR_END_OP:
		  ScopeCount -= ScopeCount? 1 : 0;
		  if( 0 == ScopeCount )
		  {
			  return (UINT8 *)NULL; //End of the formset.
		  }
		  break;
		default:
		  break;
    }
    if( OpHeader->Scope )
	{
		++ScopeCount;
    }
    OpHeader = ( EFI_IFR_OP_HEADER * )(( UINT8 * )OpHeader + OpHeader->Length );
	//[EIP:99563] If the length is zero, then return to avoid loop
	if (OpHeader->Length == 0)
		return (UINT8 *)NULL;
  }while( OpHeader->OpCode != EFI_IFR_FORM_OP );

  if( OpHeader->OpCode == EFI_IFR_FORM_OP )
  {
	if( OpHeader->Scope )
	{
		++ScopeCount;
    }
  	*CurrentForm = ( UINT8 * )OpHeader;
  	*FormID = (( EFI_IFR_FORM * )OpHeader)->FormId;
  	*FormTitle = (( EFI_IFR_FORM * )OpHeader)->FormTitle;
  }

  return *CurrentForm;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	GetUefiEQIDValue
//
// Description:
//
// Input:	UINT8 *HiiData
//
// Output:	UINT16
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

UINT16 GetUefiEQIDValue( UINT8 *HiiData )
{
	EFI_IFR_OP_HEADER *OpHeader = ( EFI_IFR_OP_HEADER * )HiiData;
	UINT16 Value = 0;
	BOOLEAN Found = FALSE;
	UINT32 OpScopeCount = FormScopeCount;

	do
	{
		OpHeader = ( EFI_IFR_OP_HEADER * )(( UINT8 * )OpHeader + OpHeader->Length );
		switch( OpHeader->OpCode )
		{
			case EFI_IFR_EQ_ID_VAL_OP:
				Value = (( EFI_IFR_EQ_ID_VAL * )OpHeader )->Value;
				Found = TRUE;
				break;
			case EFI_IFR_EQ_ID_LIST_OP:
				Value = (( EFI_IFR_EQ_ID_VAL_LIST * )OpHeader )->ValueList[0];
				Found = TRUE;
				break;
			case EFI_IFR_END_OP:
				OpScopeCount -= OpScopeCount? 1 : 0;
				if( OpScopeCount < FormScopeCount )
				{
					return Value; //End of the form
				}
			default:
				break;
		}
		if( OpHeader->Scope )
		{
			++OpScopeCount;
		}
	}while( !Found );

	return Value;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	_PopulateNewUefiControl
//
// Description:
//
// Input:	UINT8 *HiiData, CONTROL_INFO **NewControl
//
// Output:	VOID
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID _PopulateNewUefiControl( UINT8 *HiiData, CONTROL_INFO **NewControl )
{
  CONTROL_INFO *Control = ( CONTROL_INFO * )( *NewControl );

  Control->ControlPtr = ( VOID * )HiiData;
  Control->ControlKey = _GetUefiControlKey( HiiData );
  Control->ControlPrompt = _GetUefiControlPrompt( HiiData );
  Control->ControlHelp = _GetUefiControlHelp( HiiData );
  Control->VariableStorage = GetVarNumFromVarID( _GetUefiControlVarStorage( HiiData ) );
  Control->DataOffset = _GetUefiControlDataOffset( HiiData );
  Control->DataWidth = _GetUefiControlDataWidth( HiiData );
    _GetUefiControlValueInfo( HiiData, &Control->ValueInfo );
    Control->Flags = _GetUefiFlagsValue( HiiData );
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   HiiLib_UefiGetFlagsField
//
// Description:
//
// Input:   VOID *IfrPtr
//
// Output:  UINT8
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT8 HiiLib_UefiGetFlagsField(VOID *IfrPtr)
{
    EFI_IFR_OP_HEADER *OpHeader=(EFI_IFR_OP_HEADER *)IfrPtr;
    UINT8 Flag = 0;
    
    switch(OpHeader->OpCode)
    {
        case EFI_IFR_REF_OP:
            Flag = ((EFI_IFR_REF *)IfrPtr)->Question.Flags;
            break;
        case EFI_IFR_TIME_OP:
            Flag = ((EFI_IFR_TIME *)IfrPtr)->Flags;
            break;
        case EFI_IFR_DATE_OP:
            Flag = ((EFI_IFR_DATE *)IfrPtr)->Flags;
            break;
        default:
            break;
    }
    
    return Flag;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	GetNextUefiControl
//
// Description:
//
// Input:	UINT8 *CurrentForm, UINT8 **CurrentControl
//
// Output:	CONTROL_INFO
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

CONTROL_INFO *GetNextUefiControl( UINT8 *CurrentForm, UINT8 **CurrentControl, HII_DATA *AmisceData, UINT8 *CurrentPackageList, CONTROL_INFO *Control )
{
	CONTROL_INFO *NewControl = ( CONTROL_INFO * )NULL;
	EFI_IFR_OP_HEADER *OpHeader = ( EFI_IFR_OP_HEADER * )NULL;
	BOOLEAN Found = FALSE;
	UINT32 ConditionalVarId1 = 0;
//	UINT32 ConditionalVarId2 = 0;
  UINT32 isSpecial = 0;
	UINT16 CtrlType = 0;
//	UINT16 Value = 0;
	UINT8 Flags = 0;
#if SUPPRESS_COND_EXPRESSION_CONTROLS
	static BOOLEAN ControlStatus = FALSE ;
	static UINTN ConditionalPtrScope = 0 ;
#endif

	static UINT32 VarSelect = 0;
	static BOOLEAN UpdateConditionalPtr = TRUE;
	static VOID *ConditionalPtr = NULL;
	static BOOLEAN SuppressOp = FALSE;
	static UINT32 IfScopeCount = 0;

	if( *CurrentControl == NULL )
	{
		FormScopeCount = ScopeCount;
	}

	OpHeader = *CurrentControl == ( UINT8 * )NULL? ( EFI_IFR_OP_HEADER * )CurrentForm : ( EFI_IFR_OP_HEADER * )*CurrentControl;
	// Move to Next control
	do
	{
		OpHeader = ( EFI_IFR_OP_HEADER * )(( UINT8 * )OpHeader + OpHeader->Length );
		switch( OpHeader->OpCode )
		{
			case EFI_IFR_SUPPRESS_IF_OP:
#if SUPPRESS_COND_EXPRESSION_CONTROLS
			case EFI_IFR_DISABLE_IF_OP:
			case EFI_IFR_NO_SUBMIT_IF_OP:
			case EFI_IFR_INCONSISTENT_IF_OP:
			case EFI_IFR_WARNING_IF_OP:
#endif
				++IfScopeCount;
			case EFI_IFR_GRAY_OUT_IF_OP:
        		isSpecial = _IsAMISpecialControl(( UINT8 * )OpHeader, &ConditionalVarId1 );
#if SUPPRESS_COND_EXPRESSION_CONTROLS
                // Assign the Conditional pointer address
                // Take the backup of the scope count
                if(ControlStatus == FALSE)
        		{
        		    ConditionalPtr = (void *)OpHeader;
        		    ControlStatus = TRUE;
        		    ConditionalPtrScope = ScopeCount;
        		}
#endif
				break;
			case EFI_IFR_END_OP:
				ScopeCount -= ScopeCount? 1 : 0;
				if( IfScopeCount )
				{
					--IfScopeCount;
					if( !IfScopeCount) //End of If scope
					{
						// Set SupressOP to False if the Suppressif scope ends.
						// This will be used if a non interactive control is within
						// the scope of the suppress if opcode. So We can ignore
						// the suppressif for the next interactive control.
						SuppressOp = FALSE;
					}
				}
#if SUPPRESS_COND_EXPRESSION_CONTROLS
				// identifying the exact conditional control 
				// if backup scope count and current scope count matches
				// means Conditional pointer must be NULL
				// reaches EFI_IFR_END_OP not matched means valid conditional control
				if(ScopeCount == ConditionalPtrScope)
				{
				    ConditionalPtr = 0;
                    ControlStatus = FALSE;
				}
#endif
				    
				if( FormScopeCount > ScopeCount )
				{
					*CurrentControl = ( UINT8 * )NULL;
					return (CONTROL_INFO *)NULL; //End of the form
				}
				break;
			// Controls We Need
			case EFI_IFR_ONE_OF_OP:
			case EFI_IFR_CHECKBOX_OP:
			case EFI_IFR_NUMERIC_OP:
			case EFI_IFR_STRING_OP:
			case EFI_IFR_ORDERED_LIST_OP:
            case EFI_IFR_PASSWORD_OP:
				// Found the Next Control
				Found = TRUE;
				CtrlType = OpHeader->OpCode;
				break;
            case EFI_IFR_DATE_OP:
            case EFI_IFR_TIME_OP:
                // Found the Date/Time Control
                Flags = HiiLib_UefiGetFlagsField(OpHeader) ;
                if( ((Flags & QF_TIME_STORAGE) == QF_TIME_STORAGE_NORMAL)  ||
                    ((Flags & EFI_QF_DATE_STORAGE) == QF_DATE_STORAGE_NORMAL) )
                {
                    Found = TRUE;
                    CtrlType = OpHeader->OpCode;
                }
                break;
			case EFI_IFR_SUBTITLE_OP:
/*				if( AmisceData->Verbose  )
   				{
				  WriteSubTitleToFile(AmisceData, ( UINT8 * )OpHeader, CurrentPackageList);
				}*/
				break;
			default:
				//Skip The Rest
				break;
		}
		if( OpHeader->Scope )
		{
			++ScopeCount;
			if( IfScopeCount && ( OpHeader->OpCode != EFI_IFR_SUPPRESS_IF_OP )
				&& ( OpHeader->OpCode != EFI_IFR_GRAY_OUT_IF_OP ))
			{
				++IfScopeCount;
			}
		}
	}while( !Found );

	if( Found )
	{

		NewControl = ( CONTROL_INFO * )Control;
    	memset( NewControl, 0, sizeof( CONTROL_INFO ));
		NewControl->ControlType = CtrlType;
		NewControl->ConditionalVariable[0] = ConditionalVarId1;
		_PopulateNewUefiControl(( UINT8 * )OpHeader, &NewControl );
		*CurrentControl = ( UINT8 * )OpHeader;
    	NewControl->ValueInfo->IsSpecial = isSpecial;
    	NewControl->ConditionalPtr = ConditionalPtr;
	}else
	{
		*CurrentControl = ( UINT8 * )NULL;
	}

	return NewControl;
}





//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	IsValidUefi21Pack
//
// Description:
//
// Input:	UINT8* HiiData
//
// Output:	BOOLEAN
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

BOOLEAN IsValidUefi21Pack( UINT8* HiiData )
{
	EFI_HII_PACKAGE_LIST_HEADER* PackageList = ( EFI_HII_PACKAGE_LIST_HEADER* ) HiiData;
	UINT32 Offset = 0;

	while( Offset < gHiiDataSize )
	{
		if( PackageList->PackageLength )
		{
			Offset += PackageList->PackageLength;
		}
		else
		{
			return FALSE;
		}

		if(( Offset >= gHiiDataSize ) && ( Offset != gHiiDataSize ))
		{
			return FALSE;
		}

		PackageList = ( EFI_HII_PACKAGE_LIST_HEADER* )((( UINT8* )PackageList ) + PackageList->PackageLength );
	}

	return TRUE;
}

UINT32 _IsAMISpecialControl( UINT8 * HiiData, UINT32 *ConditionalVarId )
{
  EFI_IFR_OP_HEADER *OpHeader = ( EFI_IFR_OP_HEADER * )HiiData;
  UINT32 QuestionID = 0;
  UINT32 InScope = 0;
  BOOLEAN Found = FALSE;
  UINT32 IsSpecial = 0;
  UINT16 Value = 0;
  UINT16 VarStoreId = 0;

  do
  {
    switch( OpHeader->OpCode )
    {
    case EFI_IFR_EQ_ID_VAL_OP:
      QuestionID = (( EFI_IFR_EQ_ID_VAL *)OpHeader )->QuestionId;
      Value = (( EFI_IFR_EQ_ID_VAL *)OpHeader )->Value;
      Found = TRUE;
      break;
	  case EFI_IFR_EQ_ID_LIST_OP:
      QuestionID = (( EFI_IFR_EQ_ID_VAL_LIST *)OpHeader )->QuestionId;
      Value = (( EFI_IFR_EQ_ID_VAL_LIST *)OpHeader )->ValueList[0];
      Found = TRUE;
      break;
	  case EFI_IFR_END_OP:
      InScope--;
      break;
    }
    if( OpHeader->Scope )
      InScope++;
    OpHeader = ( EFI_IFR_OP_HEADER * )(( UINT8 * )OpHeader + OpHeader->Length );
  }while( !Found && InScope != 0 );

  if( Value != 0xFFFF )
  {
    IsSpecial = 0;
    goto DONE;
  }

  InScope = 0;
  Found = FALSE;
  OpHeader = ( EFI_IFR_OP_HEADER * )gCurrentFormSet;

  do
  {
    EFI_IFR_QUESTION_HEADER *QuestionHdr = (EFI_IFR_QUESTION_HEADER *)NULL;

    switch( OpHeader->OpCode )
    {
    case EFI_IFR_VARSTORE_OP:
      if( VarStoreId )
      {
        EFI_IFR_VARSTORE *Var = ( EFI_IFR_VARSTORE * )OpHeader;

        if( Var->VarStoreId == VarStoreId )
        {
			UINT32 VarId;
          wchar_t *VarName = ( wchar_t * )calloc( strlen(( char * )Var->Name ) + 1, sizeof( wchar_t ));

          if( VarName == NULL )
          {
            IsSpecial = 0;
            goto DONE;
          }

          mbstowcs( VarName, ( char * )Var->Name, strlen(( char * )Var->Name ));
          VarId = GetVariableIndexByName( VarName );//GetVarNumFromVarID( QuestionHdr->VarStoreId );

          if( GetSpecialKey( VarId ) != 0 )
          {
            *ConditionalVarId = VarId;
            IsSpecial = 1;
          }
          Found = TRUE;
        }
      }
      break;
    case EFI_IFR_ONE_OF_OP:
    case EFI_IFR_CHECKBOX_OP:
    case EFI_IFR_NUMERIC_OP:
    case EFI_IFR_PASSWORD_OP:
    case EFI_IFR_ACTION_OP:
    case EFI_IFR_REF_OP:
    case EFI_IFR_DATE_OP:
    case EFI_IFR_TIME_OP:
    case EFI_IFR_STRING_OP:
    case EFI_IFR_ORDERED_LIST_OP:
      QuestionHdr = ( EFI_IFR_QUESTION_HEADER * )(( UINT8 * )OpHeader + sizeof( EFI_IFR_OP_HEADER ));
      if( VarStoreId == 0 && QuestionHdr->QuestionId == QuestionID )
      {
        VarStoreId = QuestionHdr->VarStoreId;
        OpHeader = ( EFI_IFR_OP_HEADER * )gCurrentFormSet;
        InScope = 0;
      }
      break;
    case EFI_IFR_END_OP:
      InScope--;
      break;
    default:
      break;
    }
    if( OpHeader->Scope )
      InScope++;
    OpHeader = ( EFI_IFR_OP_HEADER * )(( UINT8 * )OpHeader + OpHeader->Length );
  }while( !Found && InScope > 0 );

DONE:
  return IsSpecial;
}

/**
    Gets the base value for the Numeric value

    @param EFI_IFR_NUMERIC * Numeric Value
    
    @retval UINT8
**/
UINT8 GetNumericBase(EFI_IFR_NUMERIC *a)
{
	UINT8 Base;
	
	Base = ((a->Flags&EFI_IFR_DISPLAY)==EFI_IFR_DISPLAY_UINT_HEX)?AMI_BASE_HEX:(((a->Flags&EFI_IFR_DISPLAY)==EFI_IFR_DISPLAY_INT_DEC)?AMI_BASE_INT_DEC:AMI_BASE_DEC);
	
	return Base;
}

/**
    Gets the base type value for the Hiidata value pointer of the numeric value

    @param VOID *IfrPtr IfrPtr value
    
    @retval UINT8
**/
UINT8 Hii_UefiGetBaseValue(VOID *IfrPtr)
{
  EFI_IFR_OP_HEADER *OpHeader=(EFI_IFR_OP_HEADER *)IfrPtr;
  UINT8 Base = EFI_IFR_DISPLAY_UINT_DEC;

  switch(OpHeader->OpCode)
  {
  case EFI_IFR_NUMERIC_OP:
    //Base = (((EFI_IFR_NUMERIC *)OpHeader)->Flags & EFI_IFR_DISPLAY);
//    Base = HII_TSE_GET_NUMERIC_BASE(IfrPtr);
    Base = GetNumericBase((EFI_IFR_NUMERIC*)IfrPtr);
    break;
  default:
    break;
  }
  return Base;
}

/**
    function used to check the range for the negative numbers and positive nubmers.

    @param VALUE_INFO * - ValueInfo Pointer
    @param UINT8		- Base value
	@param UINT16		- Data width
	@param UINT64 * 	- Number 
    
    @retval BOOLEAN

**/
BOOLEAN CheckForNumericRange(VALUE_INFO *numeric, UINT8 Base, UINT16 DataWidth, UINT64 *Num)
{
    if(Base == 32)
    {
        if(DataWidth == 1)
        {
            if(((INT8)*Num < (INT8)numeric->MinValue) ||((INT8)*Num > (INT8)numeric->MaxValue))
                return TRUE;
        }
        else if(DataWidth == 2)
        {
            if(((INT16)*Num < (INT16)numeric->MinValue) ||((INT16)*Num > (INT16)numeric->MaxValue))
                return TRUE;
        }
        else if(DataWidth == 4)
        {
            if(((INT32)*Num < (INT32)numeric->MinValue) ||((INT32)*Num > (INT32)numeric->MaxValue))
                return TRUE;
        }
        else if(DataWidth == 8)
        {
            if(((INT64)*Num < (INT64)numeric->MinValue) ||((INT64)*Num > (INT64)numeric->MaxValue))
                return TRUE;
        }
        else 
            return FALSE;
    }
    else
    {
        if(((*Num) < numeric->MinValue) ||(*Num > numeric->MaxValue))
            return TRUE;
        else
            return FALSE;
    }
return FALSE;

}

/**
    function to print the value into the buffer based on the base value.

    @param void **		- Value buffer
    @param UINT64		- Number
	@param UINT16		- Data width
	@param UINT8	 	- Base 
    
    @retval VOID

**/
CHAR8* HiiLib_ConvertChar16toChar8(CHAR16 *String);
VOID NumSPrintf(void **Buffer, UINT64 Num, UINT16 Width, UINT8 Base)
{
    CHAR16 *Buffer16 = (CHAR16 *)NULL;
    
    Buffer16 = AllocateZeroPool ( (Width+1)*3);
    if( NULL == Buffer16)
    {
        DEBUG((DEBUG_ERROR, "NumSPrintf AllocateZeroPool failure\n"));
        return;
    }
    switch(Base)
    {
        //case AMI_BASE_HEX:
           // Swprintf( Buffer16, L"%lx", Num );
           //(void)sprintf (Buffer, "%lx", Num);
           // break;
        case AMI_BASE_INT_DEC:
            if(Width == 1)
            {
                if((INT8)Num < 0)
                {
                    Swprintf( Buffer16, L"%d", (INT8)Num );
                    break;
                }
            }
            else if(Width == 2)
            {
                if((INT16)Num < 0)
                {
                    Swprintf( Buffer16, L"%d", (INT16)Num );
                    break;
                }
            }
            else if(Width == 4)
            {
                if((INT32)Num < 0)
                {
                    Swprintf( Buffer16, L"%d", (INT32)Num );
                    break;
                }
            }
            else if(Width == 8)
            {
                if((INT64)Num < 0)
                {
                    Swprintf( Buffer16, L"%ld", (INT64)Num );
                    break;
                }
            }
            Swprintf( Buffer16, L"%d", Num );
            break;
        case AMI_BASE_DEC:
        default:
            Swprintf( Buffer16, L"%ld", Num );
            break;
    }
    *Buffer = (void *)HiiLib_ConvertChar16toChar8(Buffer16);
}
