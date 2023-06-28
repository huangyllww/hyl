#ifndef _UEFI_2_0_HII_PARSER_H
#define _UEFI_2_0_HII_PARSER_H

typedef struct _HIILIB_CONTROL_INFO CONTROL_INFO;
typedef struct _OPTION_LIST OPTION_LIST;
typedef struct _VALUE_INFO VALUE_INFO;
typedef UINT32 CONTROL_FLAG;

struct _OPTION_LIST
{
  UINT16      Option;
  UINT64      Value;
  UINT8       Flag;
  OPTION_LIST *Next;
};

struct _VALUE_INFO
{
  UINT64      MinValue;
  UINT64      MaxValue;
  UINT64     OptimalDefaults;
  UINT64      FailSafeDefaults;
  CONTROL_FLAG  IsSpecial : 1;
  CONTROL_FLAG  Reserved : 31;
  OPTION_LIST *Options;
};

struct _HIILIB_CONTROL_INFO
{
  VOID        *ControlPtr;
  VOID        *ConditionalPtr;
  UINT16      ControlType;
  UINT16      ControlKey;
  UINT16      ControlPrompt;
  UINT16      ControlHelp;
  UINT32      VariableStorage;
  UINT32		  ConditionalVariable[2];
  UINT16      DataOffset;
  UINT16      DataWidth;
  VALUE_INFO  *ValueInfo;
  UINT8 Flags;
};

UINT16 GetNumberOfDataTable(UINT8 *HiiData);
//wchar_t *GetOptionsAsString(UINT8 * DataTable, UINT8 *HiiData, UINT8 *ControlPtr, UINT32 Value);
UINT8 *GetNextDataTable(UINT8 * HiiData, UINT8 **CurrentDataTable);
UINT8 *GetNextFormSet(UINT8 *DataTable, UINT8 **CurrentFormSet, UINT8 *Guid,
                      UINT16 *FormSetTitle, UINT16 *FormSetHelp, UINT16 *Class, UINT16 *SubClass);
UINT8 *GetNextForm(UINT8 *FormSet, UINT8 **CurrentForm, UINT16 *FormID, UINT16 *FormTitle);
BOOLEAN IfControlHasOptions(UINT16 ControlType);
BOOLEAN IsOneOfControl(UINT16 ControlType);
BOOLEAN IsOEMSpecialControl(CONTROL_INFO *Control);
BOOLEAN IsValidUefi20Pack( UINT8* HiiData );
#endif /* #ifndef _UEFI_2_0_HII_PARSER_H */
