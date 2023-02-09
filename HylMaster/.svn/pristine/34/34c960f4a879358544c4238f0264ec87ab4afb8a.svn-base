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

/** @file MapIdList.c
    Contains functions map ID list creation, deletion etc.,

**/

#include <ConditionalExpression.h>
#include <ConditionalExpressionApi.h>

MAP_ID_LIST *gConditionalMapIdList = (MAP_ID_LIST *)NULL;

AMI_CONDITIONAL_MAPIDLIST_PROTOCOL  gAmiCondMapIdListProtocol = {
        GetConditionalMapIdList,
        SetConditionalMapIdList,
        AddConditionalMapIdToList,
        FreeConditionalMapIdList
        };

/**
  free map id List

  @retval void

**/

void FreeConditionalMapIdList(MAP_ID_LIST **MapIdList)
{
    MAP_ID_LIST *Visit = (MAP_ID_LIST*)NULL;
    MAP_ID_LIST *Temp = (MAP_ID_LIST*)NULL;
    Visit = *MapIdList;
    
    while(Visit!= NULL)
    {
        Temp = Visit;
        if(Temp->MapID)
        MemFreePointer(&Temp->MapID);
        Visit=Visit->Next;
        MemFreePointer(&Temp);
    }
    *MapIdList = (MAP_ID_LIST*)NULL;
}

EFI_STATUS GetConditionalMapIdList (VOID **Buffer)
{
    EFI_STATUS Status = EFI_NOT_FOUND;
    
    if(!gConditionalMapIdList)
        return Status;
    
    *Buffer = (VOID *)gConditionalMapIdList;
    
    return EFI_SUCCESS;
}

EFI_STATUS SetConditionalMapIdList (VOID *Buffer)
{
    EFI_STATUS Status = EFI_NOT_FOUND;
    
    if(gConditionalMapIdList)
        FreeConditionalMapIdList(&gConditionalMapIdList);
    
    gConditionalMapIdList = (MAP_ID_LIST *)Buffer;
    
    return EFI_SUCCESS;
}

/**
  Add map id to global list

  @param CHAR16* 

  @retval BOOLEAN

**/

BOOLEAN AddConditionalMapIdToList(CHAR16 *MapID)
{
    MAP_ID_LIST *Temp = (MAP_ID_LIST*)NULL;
    MAP_ID_LIST *Visit = (MAP_ID_LIST*)NULL;
    MAP_ID_LIST *Prev = (MAP_ID_LIST*)NULL;
    BOOLEAN DuplicateMapIdStatus = FALSE;
    
    if( NULL == MapID )
        return TRUE;
    
    if(NULL == gConditionalMapIdList)
    {
        gConditionalMapIdList = AllocateZeroPool((UINTN)sizeof(MAP_ID_LIST));
        if(gConditionalMapIdList)
        {
            gConditionalMapIdList->Next =(MAP_ID_LIST*)NULL;
            gConditionalMapIdList->MapID = AllocateZeroPool( (StrLen(MapID)+1)*2);
            StrCpy(gConditionalMapIdList->MapID,MapID);
        }
    }
    else
    {
        for(Visit = gConditionalMapIdList;Visit!= NULL;Visit=Visit->Next)
        {
            Prev = Visit;
            if( !StrCmp(Visit->MapID,MapID) )
            {
                DuplicateMapIdStatus = TRUE;
                break;
            }
        }
        if(FALSE == DuplicateMapIdStatus)
        {
            Temp = AllocateZeroPool(sizeof(MAP_ID_LIST));
            if(Temp)
            {
                Temp->Next =(MAP_ID_LIST*)NULL;
                Temp->MapID = AllocateZeroPool( (StrLen(MapID)+1)*2);
                StrCpy(Temp->MapID,MapID);
            }
            Prev->Next = Temp;
        }
    }
    return DuplicateMapIdStatus;
}

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
