//**        (C)Copyright 1985-2017, American Megatrends, Inc.    **

/** @file ConditionalExpression.h
    Contains declarations for ConditionalExpression.c

**/

#ifndef _REMOTE_CONDITIONAL_EXPRESSION_H_
#define	_REMOTE_CONDITIONAL_EXPRESSION_H_

#include "Library/MemoryAllocationLib.h"
#include <Include/AmiDxeLib.h>
#include "Library/BaseMemoryLib.h"
#include "setupdata.h"
#include "BiosAttribTypeLib.h"
#include "BiosAttribTypeLibHelper.h"
#include "Library/AmiJsonLib.h"
#include <TseCommon.h>

typedef struct DEPENDENCY_OBJECTS
{
    cJSON                       *DependencyObject;
    struct DEPENDENCY_OBJECTS   *Next;
} DEPENDENCY_OBJECTS;


EFI_STATUS  AddMappingIdToList (CHAR16 *AttributeName, UINT16 ControlKey, EFI_GUID FormGuid, void *AttributePtr);
VOID        *MemReallocateZeroPool( VOID *oldBuffer, UINTN oldSize, UINTN newSize );
CHAR8       *StrDup16to8(CHAR16 *String);
VOID        MemFreePointer( VOID **ptr );

VOID
ResetExpressionStack (
  VOID
  );

UINT64
AmiTseDivU64x32 (
  IN  UINT64  Dividend,
  IN  UINTN   Divisor,
  OUT UINTN   *Remainder OPTIONAL
  );

#endif /* _REMOTE_CONDITIONAL_EXPRESSION_H_ */

//*****************************************************************
//*****************************************************************
//**                                                             **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.    **
//**                                                             **
//**                       All Rights Reserved.                  **
//**                                                             **
//**    5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093 **
//**                                                             **
//**                       Phone (770)-246-8600                  **
//**                                                             **
//*****************************************************************
//*****************************************************************
