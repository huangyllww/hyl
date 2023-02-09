#ifndef _VARIABLE_H_
#define _VARIABLE_H_

#include "Efi.h"

#define VARIABLE_NAME_LENGTH 40

// GUID definitions
#define EFI_GLOBAL_VARIABLE_GUID \
  { 0x8BE4DF61, 0x93CA, 0x11d2, 0xAA, 0x0D, 0x00, 0xE0, 0x98, 0x03, 0x2B, 0x8C }
#define	LEGACY_DEV_ORDER_GUID	\
	{ 0xA56074DB, 0x65FE, 0x45F7, 0xBD, 0x21, 0x2D, 0x2B, 0xDD, 0x8E, 0x96, 0x52 }

#define LANGUAGE_CONTROL          0x01
#define PLATFORM_LANGUAGE_CONTROL 0x02
#define BOOT_ORDER_CONTROL        0x03
#define LEGACY_DEV_ORDER_CONTROL  0x04

typedef struct _VARIABLE VARIABLE;

UINT8 *SceGetQuestionValue( UINT16 VariableId, UINT16 Offset, UINT16 Width );
UINT8 *GetLangCodes(UINT32 ControlConditionVar);
UINT8 *GetVariableByName(wchar_t *VariableName, UINT64 *Size);

VOID SceSetQuestionValue(UINT16 VariableId, UINT16 Offset, UINT16 Width, UINT8 *Buffer);
VOID ImportVariableToNVRAM();

int AddVariable( UINT8 *IFRData );

UINT16 GetSpecialKey(UINT32 ControlConditionVar);
UINT16 GetLangDataWidth();
UINT16 GetUEFI20LangDataWidth();

UINT32 GetVarNumFromVarID( UINT32 ID );
UINT64 GetValueFromBuffer(UINT8* Buffer, UINT16 Size);

VARIABLE *GetNextVariableInfo(VARIABLE *Variable);

UINT16 GetVariableIdByName(wchar_t *VariableName);
BOOLEAN GetVariableById(UINT32 VariableId, VARIABLE* Var);
UINT16 GetVariableIndexByName(wchar_t *VariableName);
VARIABLE *GetVariableInfobyName(wchar_t *VariableName);
BOOLEAN MarkVarReferenced(UINT32 VariableId);

#endif /* #ifndef _VARIABLE_H_ */
