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

/** @file ConditionalExpressionApi.h
    Contains declarations for ConditionalExpression.c

**/

#ifndef _CONDITIONAL_EXPRESSION_API_H_
#define	_CONDITIONAL_EXPRESSION_API_H_

#include <Uefi.h>

typedef struct _MAP_ID_LIST
{
    CHAR16 *MapID;
    struct _MAP_ID_LIST *Next;
}MAP_ID_LIST;

EFI_STATUS GetConditionalMapIdList (void **Buffer);
EFI_STATUS SetConditionalMapIdList (void *Buffer);
BOOLEAN AddConditionalMapIdToList(CHAR16 *MapID);
void FreeConditionalMapIdList(MAP_ID_LIST **MapIdList);

typedef EFI_STATUS (*GET_COND_MAPID_LIST) (void **Buffer);
typedef EFI_STATUS (*SET_COND_MAPID_LIST) (void *Buffer);
typedef BOOLEAN (*ADD_COND_MAPID_TO_LIST) (CHAR16 *MapID);
typedef VOID (*FREE_COND_MAPID_LIST) (MAP_ID_LIST **);

typedef struct _AMI_CONDITIONAL_MAPIDLIST_PROTOCOL 
{
    GET_COND_MAPID_LIST  GetConditionalMapIdList;
    SET_COND_MAPID_LIST  SetConditionalMapIdList;
    ADD_COND_MAPID_TO_LIST  AddConditionalMapIdToList;
    FREE_COND_MAPID_LIST  FreeConditionalMapIdList;
}AMI_CONDITIONAL_MAPIDLIST_PROTOCOL;

extern EFI_GUID AmiConditionalMapIdListProtocolbGuid;

#endif /* _CONDITIONAL_EXPRESSION_API_H_ */

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
