//#include "wchar.h"
//#include "memory.h"
//#include "stdlib.h"
#define _EFI 1

#include "EfiCommon.h"
#include "AmiSceCommon.h"
//Added for EDK
#include "EDKStructures.h"
//Removed for EDK
/*#ifdef _WIN32
#include EFI_PROTOCOL_DEFINITION(Hii)
#endif*/

typedef EFI_GUID AMI_GUID;
#include "AdvScript.h"
#include "UEFI20HIIParser.h"
#include "UEFI20Variable.h"
#include "UEFI20StringManagement.h"

#define MAX_LENGTH 1024

static UINT32 VarSelect = 0;
extern UINT64 gHiiDataSize;

VOID _GetControlOption(UINT8 *HiiData, VALUE_INFO **ValueInfo );
UINT16 _GetMaximumValue(UINT8 *HiiData);
UINT16 _GetMinimumValue(UINT8 *HiiData);
CONTROL_INFO *GetNextControl(UINT8 *CurrentForm, UINT8 * , HII_DATA *AmisceData , UINT8 **CurrentControl);
void WriteSubTitleToFile(HII_DATA *AmisceData,UINT8 *HiiData , UINT8 *DataTable);
#if 0
VOID _AddFormSetVariable(UINT8 *HiiData)
{
  AddVariable( HiiData );
}
#endif

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	_GetControlDataOffset
//
// Description:
//
// Input:	UINT8 *HiiData
//
// Output:	UINT16
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
#if 0
UINT16 _GetControlDataOffset(UINT8 * HiiData)
{
  EFI_IFR_OP_HEADER *OpHeader = ( EFI_IFR_OP_HEADER * )HiiData;
  UINT16 QuestionId = 0;

  switch(OpHeader->OpCode)
  {
  case EFI_IFR_ONE_OF_OP:
    QuestionId = (( EFI_IFR_ONE_OF * )HiiData)->QuestionId;
    break;
  case EFI_IFR_CHECKBOX_OP:
    QuestionId = (( EFI_IFR_CHECK_BOX * )HiiData)->QuestionId;
    break;
  case EFI_IFR_NUMERIC_OP:
    QuestionId = (( EFI_IFR_NUMERIC * )HiiData)->QuestionId;
    break;
  case EFI_IFR_STRING_OP:
    QuestionId = (( EFI_IFR_STRING * )HiiData)->QuestionId;
    break;
  case EFI_IFR_ORDERED_LIST_OP:
    QuestionId = (( EFI_IFR_ORDERED_LIST * )HiiData)->QuestionId;
    break;
  default:
    break;
  }

  return QuestionId;
}
#endif

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	_GetControlDataWidth
//
// Description:
//
// Input:	UINT8 *HiiData
//
// Output:	UINT16
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
#if 0
UINT16 _GetControlDataWidth(UINT8 * HiiData)
{
  EFI_IFR_OP_HEADER *OpHeader = ( EFI_IFR_OP_HEADER * )HiiData;
  UINT16 Width = 0;

  switch(OpHeader->OpCode)
  {
  case EFI_IFR_ONE_OF_OP:
    Width = (( EFI_IFR_ONE_OF * )HiiData)->Width;
    break;
  case EFI_IFR_CHECKBOX_OP:
    Width = (( EFI_IFR_CHECK_BOX * )HiiData)->Width;
    break;
  case EFI_IFR_NUMERIC_OP:
    Width = (( EFI_IFR_NUMERIC * )HiiData)->Width;
    break;
  case EFI_IFR_STRING_OP:
    Width = (( EFI_IFR_STRING * )HiiData)->Width;
    break;
  case EFI_IFR_ORDERED_LIST_OP:
    Width = (( EFI_IFR_ORDERED_LIST * )HiiData)->MaxEntries;
    break;
  default:
    break;
  }

  return Width;
}
#endif

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	_GetControlDefaults
//
// Description:
//
// Input:	UINT8 *HiiData, CONTROL_INFO *Control
//
// Output:	UINT16
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
#if 0
VOID _GetControlDefaults(UINT8 *HiiData, VALUE_INFO **ValueInfo )
{
  EFI_IFR_OP_HEADER *OpHeader = ( EFI_IFR_OP_HEADER * )HiiData;
  VALUE_INFO *Value = ( VALUE_INFO * )*ValueInfo;
  UINT8 Flags = 0;

  switch( OpHeader->OpCode )
  {
  case EFI_IFR_ONE_OF_OP:
    _GetControlOption( HiiData, ValueInfo );
    break;
  case EFI_IFR_CHECKBOX_OP:
    Flags = (( EFI_IFR_CHECK_BOX * )HiiData )->Flags;
    Value->OptimalDefaults = (Flags & EFI_IFR_FLAG_DEFAULT)? 1 : 0;
    Value->FailSafeDefaults = ( Flags & EFI_IFR_FLAG_MANUFACTURING )? 1 : 0;
    break;
  case EFI_IFR_NUMERIC_OP:
    Value->FailSafeDefaults = Value->OptimalDefaults = (( EFI_IFR_NUMERIC * )HiiData )->Default;
    break;
  case EFI_IFR_STRING_OP:
    break;
  case EFI_IFR_ORDERED_LIST_OP:
    _GetControlOption( HiiData, ValueInfo );
    break;
  default:
    break;
  }
}
#endif

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
#if 0
UINT16 _GetControlHelp( UINT8 *ifr )
{
	EFI_IFR_OP_HEADER *headerPtr = (EFI_IFR_OP_HEADER *)ifr;
	UINT16 token = 0;

	switch ( headerPtr->OpCode )
	{
		case EFI_IFR_TEXT_OP:
			{
				EFI_IFR_TEXT *ptr = (EFI_IFR_TEXT *)headerPtr;
				token = ptr->Help;
			}
			break;
		case EFI_IFR_ORDERED_LIST_OP:
			{
				EFI_IFR_ORDERED_LIST *ptr = (EFI_IFR_ORDERED_LIST *)headerPtr;
				token = ptr->Help;
			}
			break;
		case EFI_IFR_ONE_OF_OP:
			{
				EFI_IFR_ONE_OF *ptr = (EFI_IFR_ONE_OF *)headerPtr;
				token = ptr->Help;
			}
			break;
		case EFI_IFR_CHECKBOX_OP:
			{
				EFI_IFR_CHECK_BOX *ptr = (EFI_IFR_CHECK_BOX *)headerPtr;
				token = ptr->Help;
			}
			break;
		case EFI_IFR_DATE_OP:
		case EFI_IFR_TIME_OP:
		case EFI_IFR_NUMERIC_OP:
			{
				EFI_IFR_NUMERIC *ptr = (EFI_IFR_NUMERIC *)headerPtr;
				token = ptr->Help;
			}
			break;
		case EFI_IFR_PASSWORD_OP:
			{
				EFI_IFR_PASSWORD *ptr = (EFI_IFR_PASSWORD *)headerPtr;
				token = ptr->Help;
			}
			break;
		case EFI_IFR_REF_OP:
			{
				EFI_IFR_REF *ptr = (EFI_IFR_REF *)headerPtr;
				token = ptr->Help;
			}
			break;
		case EFI_IFR_STRING_OP:
			{
				EFI_IFR_STRING *ptr = (EFI_IFR_STRING *)headerPtr;
				token = ptr->Help;
			}
			break;
		case EFI_IFR_SAVE_DEFAULTS_OP:
			{
				EFI_IFR_SAVE_DEFAULTS *ptr = (EFI_IFR_SAVE_DEFAULTS *)headerPtr;
				token = ptr->Help;
			}
			break;
		case EFI_IFR_INVENTORY_OP:
			{
				EFI_IFR_INVENTORY *ptr = (EFI_IFR_INVENTORY *)headerPtr;
				token = ptr->Help;
			}
			break;
		default:
			break;
	}

	return token;
}
#endif

#if 0
UINT16 _GetControlKey(UINT8 *HiiData)
{
  EFI_IFR_OP_HEADER *OpHeader = ( EFI_IFR_OP_HEADER * )HiiData;
  UINT16 Key = 0;

  switch(OpHeader->OpCode)
  {
  case EFI_IFR_ONE_OF_OP:
    Key = 0; // EFI_IFR_ONE_OF does not contain a Key field
    break;
  case EFI_IFR_CHECKBOX_OP:
    Key = (( EFI_IFR_CHECK_BOX * )HiiData)->Key;
	break;
  case EFI_IFR_NUMERIC_OP:
    Key = (( EFI_IFR_NUMERIC * )HiiData)->Key;
	break;
  case EFI_IFR_STRING_OP:
    Key = (( EFI_IFR_STRING * )HiiData)->Key;
	break;
  case EFI_IFR_ORDERED_LIST_OP:
    Key = 0; // EFI_IFR_ORDERED_LIST does not contain a Key field
	break;
  default:
    break;
  }

  return Key;
}
#endif

#if 0
VOID _GetControlOption(UINT8 *HiiData, VALUE_INFO **ValueInfo )
{
  EFI_IFR_OP_HEADER *OpHeader = ( EFI_IFR_OP_HEADER * )HiiData;
  EFI_IFR_ONE_OF_OPTION *OneOf = ( EFI_IFR_ONE_OF_OPTION * )NULL;
  OPTION_LIST *Option = NULL;

  if( OpHeader->OpCode != EFI_IFR_ONE_OF_OP &&
    OpHeader->OpCode != EFI_IFR_ORDERED_LIST_OP )
  {
    goto DONE;
  }

  OneOf = ( EFI_IFR_ONE_OF_OPTION * )(( UINT8 * )OpHeader + OpHeader->Length );

  do
  {
    OPTION_LIST *Temp = ( OPTION_LIST * )calloc(sizeof( OPTION_LIST ), sizeof( UINT8 ));

    if( Temp == NULL )
		throw(L"Memory Allocation Failure");

    Temp->Option = OneOf->Option;
    Temp->Value = OneOf->Value;
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
  }while( OneOf->Header.OpCode != EFI_IFR_END_ONE_OF_OP );


DONE:
  return;
}
#endif

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	_GetControlPrompt
//
// Description:
//
// Input:	UINT8 *ifr
//
// Output:	UINT16
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
#if 0
UINT16 _GetControlPrompt( UINT8 *ifr )
{
	EFI_IFR_OP_HEADER *headerPtr = (EFI_IFR_OP_HEADER *)ifr;
	UINT16 token = 0;

	switch ( headerPtr->OpCode )
	{
		case EFI_IFR_TEXT_OP:
			{
				EFI_IFR_TEXT *ptr = (EFI_IFR_TEXT *)headerPtr;
				token = ptr->Text;
			}
			break;
		case EFI_IFR_ONE_OF_OP:
			{
				EFI_IFR_ONE_OF *ptr = (EFI_IFR_ONE_OF *)headerPtr;
				token = ptr->Prompt;
			}
			break;
		case EFI_IFR_CHECKBOX_OP:
			{
				EFI_IFR_CHECK_BOX *ptr = (EFI_IFR_CHECK_BOX *)headerPtr;
				token = ptr->Prompt;
			}
			break;
		case EFI_IFR_DATE_OP:
		case EFI_IFR_TIME_OP:
		case EFI_IFR_NUMERIC_OP:
			{
				EFI_IFR_NUMERIC *ptr = (EFI_IFR_NUMERIC *)headerPtr;
				token = ptr->Prompt;
			}
			break;
		case EFI_IFR_PASSWORD_OP:
			{
				EFI_IFR_PASSWORD *ptr = (EFI_IFR_PASSWORD *)headerPtr;
				token = ptr->Prompt;
			}
			break;
		case EFI_IFR_REF_OP:
			{
				EFI_IFR_REF *ptr = (EFI_IFR_REF *)headerPtr;
				token = ptr->Prompt;
			}
			break;
		case EFI_IFR_STRING_OP:
			{
				EFI_IFR_STRING *ptr = (EFI_IFR_STRING *)headerPtr;
				token = ptr->Prompt;
			}
			break;
		case EFI_IFR_SAVE_DEFAULTS_OP:
			{
				EFI_IFR_SAVE_DEFAULTS *ptr = (EFI_IFR_SAVE_DEFAULTS *)headerPtr;
				token = ptr->Prompt;
			}
			break;
		case EFI_IFR_INVENTORY_OP:
			{
				EFI_IFR_INVENTORY *ptr = (EFI_IFR_INVENTORY *)headerPtr;
				token = ptr->Text;
			}
			break;
		case EFI_IFR_SUBTITLE_OP:
			{
				EFI_IFR_SUBTITLE *ptr = (EFI_IFR_SUBTITLE *)headerPtr;
				token = ptr->SubTitle;
			}
			break;
		default:
			break;
	}

	return token;
}
#endif

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	_GetControlValueInfo
//
// Description:
//
// Input:	UINT8 *HiiData, VALUE_INFO **ValueInfo
//
// Output:	VOID
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
#if 0
VOID _GetControlValueInfo(UINT8 *HiiData, VALUE_INFO **ValueInfo)
{
  VALUE_INFO *Value = NULL;

  Value = ( VALUE_INFO * )calloc( sizeof( VALUE_INFO ), sizeof( UINT8 ));
  if( Value == NULL )
	throw(L"Memory Allocation Failure");

  Value->MinValue = _GetMinimumValue( HiiData );
  Value->MaxValue = _GetMaximumValue( HiiData );
  _GetControlDefaults( HiiData, &Value );

  *ValueInfo = Value;
  return;
}
#endif

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	_GetMaximumValue
//
// Description:
//
// Input:	UINT8 *HiiData
//
// Output:	UINT16
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
#if 0
UINT16 _GetMaximumValue(UINT8 *HiiData)
{
  EFI_IFR_OP_HEADER *OpHeader = ( EFI_IFR_OP_HEADER * )HiiData;
  UINT16 MaxVal = 0;

  switch( OpHeader->OpCode )
  {
  case EFI_IFR_NUMERIC_OP:
    MaxVal = (( EFI_IFR_NUMERIC * )HiiData )->Maximum;
    break;
  case EFI_IFR_STRING_OP:
    MaxVal = (( EFI_IFR_STRING * )HiiData )->MaxSize;
    break;
  default:
    break;
  }

  return MaxVal;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	_GetMinimumValue
//
// Description:
//
// Input:	UINT8 *HiiData
//
// Output:	UINT16
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT16 _GetMinimumValue(UINT8 *HiiData)
{
  EFI_IFR_OP_HEADER *OpHeader = ( EFI_IFR_OP_HEADER * )HiiData;
  UINT16 MinVal = 0;

  switch( OpHeader->OpCode )
  {
  case EFI_IFR_NUMERIC_OP:
    MinVal = (( EFI_IFR_NUMERIC * )HiiData )->Minimum;
    break;
  case EFI_IFR_STRING_OP:
    MinVal = (( EFI_IFR_STRING * )HiiData )->MinSize;
    break;
  default:
    break;
  }

  return MinVal;
}
#endif

//
//
#if 0
VOID _PopulateNewControl( UINT8 *HiiData, CONTROL_INFO **NewControl )
{
  CONTROL_INFO *Control = ( CONTROL_INFO * )( *NewControl );

  Control->ControlPtr = ( VOID * )HiiData;
  Control->ControlKey = _GetControlKey( HiiData );
  Control->ControlPrompt = _GetControlPrompt( HiiData );
  Control->ControlHelp = _GetControlHelp( HiiData );
  Control->DataOffset = _GetControlDataOffset( HiiData );
  Control->DataWidth = _GetControlDataWidth( HiiData );
  _GetControlValueInfo( HiiData, &Control->ValueInfo );
}
#endif

#if 0
UINT16 GetNumberOfDataTable(UINT8 *HiiData)
{
  return (( EFI_HII_EXPORT_TABLE * )HiiData )->NumberOfHiiDataTables;
}

UINT8 *GetNextDataTable(UINT8 * HiiData, UINT8 **CurrentDataTable)
{
  if( *CurrentDataTable != NULL )
  {
    *CurrentDataTable = ( UINT8 * )( *CurrentDataTable + (( EFI_HII_DATA_TABLE * )*CurrentDataTable )->DataTableSize );
  }else
  {
    *CurrentDataTable = ( UINT8 * )(( UINT8 * )HiiData + sizeof( EFI_HII_EXPORT_TABLE ));
  }


  return ( UINT8 * )*CurrentDataTable;
}
#endif

#if 0
UINT8 *GetNextFormSet(UINT8 *HiiDataTable, UINT8 **CurrentFormSet, UINT8 *Guid,
                      UINT16 *FormSetTitle, UINT16 *FormSetHelp, UINT16 *Class, UINT16 *SubClass)
{
  EFI_IFR_FORM_SET *FormSet = ( EFI_IFR_FORM_SET * )NULL;
  EFI_HII_DATA_TABLE *DataTable = ( EFI_HII_DATA_TABLE * )HiiDataTable;

  if( *CurrentFormSet == NULL )
  {
    if( DataTable->IfrDataOffset )
      FormSet = ( EFI_IFR_FORM_SET * )(( UINT8 * )DataTable + DataTable->IfrDataOffset + sizeof( EFI_HII_PACK_HEADER ));
  }else
  {
    // Is it possible to have more than one FormSet in an Export Package?
  }

  if( FormSet && FormSet->Class )
  {
    _AddFormSetVariable( ( UINT8 * )FormSet );
    // Get FormSet Data
    memcpy( Guid, &FormSet->Guid, sizeof( EFI_GUID ));
    *FormSetTitle = FormSet->FormSetTitle;
    *FormSetHelp = FormSet->Help;
    *Class = FormSet->Class;
    *SubClass = FormSet->SubClass;
    *CurrentFormSet = ( UINT8 * )FormSet;
  }
  VarSelect = 0;    // clear for each formset
  return *CurrentFormSet;
}
#endif

#if 0
UINT8 *GetNextForm(UINT8 *FormSet, UINT8 **CurrentForm, UINT16 *FormID, UINT16 *FormTitle)
{
  EFI_IFR_OP_HEADER *OpHeader = ( EFI_IFR_OP_HEADER * )NULL;

  *FormID = *FormTitle = 0;
  OpHeader = *CurrentForm == NULL? ( EFI_IFR_OP_HEADER * )FormSet : ( EFI_IFR_OP_HEADER * )*CurrentForm;

  do
  {
    //Skip Until Next Form
    switch( OpHeader->OpCode )
    {
    case EFI_IFR_VARSTORE_OP:
      AddVariable(( UINT8 * )OpHeader);
      break;
    case EFI_IFR_END_FORM_OP:
      *CurrentForm = ( UINT8 * )NULL;
      break;
    case EFI_IFR_END_FORM_SET_OP:
      // Empty FormSet or reached end of FormSet
      *CurrentForm = ( UINT8 * )NULL;
      goto DONE;
      break;
    default:
      break;
    }
    OpHeader = ( EFI_IFR_OP_HEADER * )(( UINT8 * )OpHeader + OpHeader->Length );
  }while( OpHeader->OpCode != EFI_IFR_FORM_OP );

  // this actually cleared it for every form which is not consistent with vfr compiler
  //VarSelect = *CurrentForm == NULL? 0 : VarSelect;	// New FormSet Reset Control VarSelect
  *CurrentForm = ( UINT8 * )OpHeader;
  *FormID = (( EFI_IFR_FORM * )OpHeader)->FormId;
  *FormTitle = (( EFI_IFR_FORM * )OpHeader)->FormTitle;

DONE:
  return *CurrentForm;
}

UINT16 GetEQIDValue( UINT8 *HiiData )
{
  EFI_IFR_OP_HEADER *OpHeader = ( EFI_IFR_OP_HEADER * )HiiData;
  UINT16 Value = 0;
  BOOLEAN Found = FALSE;

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
      Value = (( EFI_IFR_EQ_ID_LIST * )OpHeader )->ValueList[0];
      Found = TRUE;
      break;
    default:
      break;
    }
  }while( !Found && OpHeader->OpCode != EFI_IFR_END_FORM_OP );

  return Value;
}
#endif

void
breakout()
{
}

#if 0
CONTROL_INFO *GetNextControl(UINT8 *Form,  UINT8 *CurrentPackageList , HII_DATA *AmisceData ,UINT8 **CurrentOpCode)
{
  CONTROL_INFO *NewControl = ( CONTROL_INFO * )NULL;
  EFI_IFR_OP_HEADER *OpHeader = ( EFI_IFR_OP_HEADER * )NULL;
  BOOLEAN Found = FALSE;
  UINT32 ConditionalVarId1 = 0;
  UINT32 ConditionalVarId2 = 0;
  UINT16 CtrlType = 0;
  UINT16 Value = 0;

  //static UINT32 VarSelect = 0;
  static BOOLEAN UpdateConditionalPtr = TRUE;
  static VOID *ConditionalPtr = NULL;
  static BOOLEAN SuppressOp = FALSE;

  OpHeader = *CurrentOpCode == ( UINT8 * )NULL? ( EFI_IFR_OP_HEADER * )Form : ( EFI_IFR_OP_HEADER * )*CurrentOpCode;
  // Move to Next control
  do
  {
    OpHeader = ( EFI_IFR_OP_HEADER * )(( UINT8 * )OpHeader + OpHeader->Length );
    switch( OpHeader->OpCode )
    {
    case EFI_IFR_SUPPRESS_IF_OP:
      SuppressOp = TRUE;
      Value = GetEQIDValue(( UINT8 * )OpHeader );
    case EFI_IFR_GRAYOUT_IF_OP:
    case EFI_IFR_HIDDEN_OP:
      if( UpdateConditionalPtr )
      {
        ConditionalPtr = ( VOID * )OpHeader;
        UpdateConditionalPtr = FALSE;
        OpHeader = ( EFI_IFR_OP_HEADER * )(( UINT8 * )OpHeader + OpHeader->Length );
        switch( OpHeader->OpCode )
        {
          case EFI_IFR_VARSTORE_SELECT_OP:
            ConditionalVarId1 = (( EFI_IFR_VARSTORE_SELECT * )OpHeader )->VarId;
			VarSelect = ConditionalVarId1;
            break;
          case EFI_IFR_VARSTORE_SELECT_PAIR_OP:
            ConditionalVarId1 = (( EFI_IFR_VARSTORE_SELECT_PAIR* )OpHeader )->VarId;
            ConditionalVarId2 = (( EFI_IFR_VARSTORE_SELECT_PAIR* )OpHeader )->SecondaryVarId;
            break;
        }
      }
      break;
    case EFI_IFR_END_IF_OP:
      UpdateConditionalPtr = TRUE;
      ConditionalPtr = NULL;
      SuppressOp = FALSE;
      break;
    case EFI_IFR_VARSTORE_SELECT_OP:
      VarSelect = (( EFI_IFR_VARSTORE_SELECT * )OpHeader )->VarId;
      ConditionalVarId1 = UpdateConditionalPtr? VarSelect : ConditionalVarId1;
      break;
    case EFI_IFR_VARSTORE_SELECT_PAIR_OP:
      VarSelect = (( EFI_IFR_VARSTORE_SELECT_PAIR* )OpHeader )->VarId;
      ConditionalVarId1 = UpdateConditionalPtr? VarSelect : ConditionalVarId1;
      ConditionalVarId2 = (( EFI_IFR_VARSTORE_SELECT_PAIR* )OpHeader )->SecondaryVarId;
      break;
    // Controls We Need
    case EFI_IFR_ONE_OF_OP:
    case EFI_IFR_CHECKBOX_OP:
    case EFI_IFR_NUMERIC_OP:
    case EFI_IFR_STRING_OP:
    case EFI_IFR_ORDERED_LIST_OP:
      // Found the Next Control
      Found = TRUE;
      CtrlType = OpHeader->OpCode;
if (OpHeader->OpCode == EFI_IFR_STRING_OP)
	breakout();
      break;
	case EFI_IFR_SUBTITLE_OP:
		if( AmisceData->Verbose  )
		{
		  WriteSubTitleToFile(AmisceData, ( UINT8 * )OpHeader, CurrentPackageList);
		}
		break;
    default:
      //Skip The Rest
      break;
    }
  }while( OpHeader->OpCode != EFI_IFR_END_FORM_OP && !Found );

  if( Found )
  {
    NewControl = ( CONTROL_INFO * )calloc( sizeof( CONTROL_INFO ), sizeof( UINT8 ));
    if( NewControl == NULL )
		throw(L"Memory Allocation Failure");
    NewControl->ControlType = CtrlType;
    NewControl->VariableStorage = GetVarNumFromVarID( VarSelect );
    NewControl->ConditionalVariable[0] = GetVarNumFromVarID( ConditionalVarId1 );
    NewControl->ConditionalVariable[1] = GetVarNumFromVarID( ConditionalVarId2 );
    _PopulateNewControl(( UINT8 * )OpHeader, &NewControl );
    *CurrentOpCode = ( UINT8 * )OpHeader;
    if( SuppressOp && Value == 0xFFFF )
    {
      NewControl->ValueInfo->IsSpecial = 1;
    }
  }else
  {
    *CurrentOpCode = ( UINT8 * )NULL;
  }

  return NewControl;
}
#endif

BOOLEAN IfControlHasOptions( UINT16 ControlType )
{
  BOOLEAN HasOptions = FALSE;

  switch( ControlType )
  {
  case EFI_IFR_ONE_OF_OP:
  case EFI_IFR_ORDERED_LIST_OP:
    HasOptions = TRUE;
    break;
  default:
    break;
  }
  return HasOptions;
}

BOOLEAN IsOneOfControl(UINT16 ControlType)
{
  return (( ControlType == EFI_IFR_ONE_OF_OP )? TRUE : FALSE);
}

BOOLEAN IsOEMSpecialControl(CONTROL_INFO *Control)
{
  return (BOOLEAN)Control->ValueInfo->IsSpecial;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	IsValidUefi20Pack
//
// Description:
//
// Input:	UINT8* HiiData
//
// Output:	BOOLEAN
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>


#if 0
BOOLEAN IsValidUefi20Pack( UINT8* HiiData )
{
	UINT16 HiiDataTablesCount = 0;
	EFI_HII_DATA_TABLE* DataTable = NULL;
	UINT32 Offset = 0;
	UINT16 TempCount = 0;

	DataTable = ( EFI_HII_DATA_TABLE* )(( UINT8* ) HiiData + sizeof( EFI_HII_EXPORT_TABLE ));
	HiiDataTablesCount = GetNumberOfDataTable( HiiData );
	Offset = sizeof( EFI_HII_EXPORT_TABLE );

	while( Offset < gHiiDataSize )
	{
		++TempCount;

		if( TempCount > HiiDataTablesCount )
			return FALSE;

		if( DataTable->DataTableSize )
			Offset += DataTable->DataTableSize;
		else
			Offset += sizeof( EFI_HII_DATA_TABLE );

		if(( Offset >= gHiiDataSize ) && ( TempCount != HiiDataTablesCount ))
			return FALSE;

		DataTable = ( EFI_HII_DATA_TABLE* )((( UINT8* )DataTable) + DataTable->DataTableSize );
	}

  return TRUE;
}
#endif

