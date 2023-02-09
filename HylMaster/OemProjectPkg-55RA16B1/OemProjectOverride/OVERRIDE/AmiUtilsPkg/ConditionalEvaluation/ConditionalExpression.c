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

/** @file ConditionalEvaluation.c
    Contains Functions related to evaluating conditional expressions

**/

#include <ConditionalExpression.h>
#include "Efi.h"

#define FORMSET_MEM_ALLOCATION          20
#define CONTROLS_IN_FORMSET_ALLOCATION  500
#define SUPPORTED_CONDITIONS            2

#define EfiZeroMem(_Destination, _Length)  gBS->SetMem ((_Destination), (_Length), 0)

typedef struct _EFI_HII_VALUE
{
  UINT8               Type;
  EFI_IFR_TYPE_VALUE  Value;
}EFI_HII_VALUE;

CHAR8  *MapPropertyStrings [] = {
        "CurrentValue",
        "DefaultValue",
        "ReadOnly",
        "WriteOnly",
        "GrayOut",
        "Hidden",
        "LowerBound",
        "UpperBound",
        "MinLength",
        "MaxLength",
        "ScalarIncrement"
    };

CHAR8   *MapConditionSrings [] = {
        "EQU",
        "NEQ",
        "GTR",
        "GEQ",
        "LSS",
        "LEQ"
    };
    
CHAR8 *DependencyTypeStrings [] = {
        "Map"
    };

CHAR8 *MapTermsStrings [] = {
        "AND",
        "OR"
    };

FORMSET_MAPPING_LIST        *gFormsetMappingList;

EFI_STATUS
PopExpression (
  OUT EFI_HII_VALUE  *Value
  );

EFI_STATUS
PushExpression (
  IN EFI_HII_VALUE  *Value
  );

INTN
CompareHiiValue (
  IN  EFI_HII_VALUE   *Value1,
  IN  EFI_HII_VALUE   *Value2,
  IN  EFI_HII_HANDLE  HiiHandle OPTIONAL
  );

/**
    Adds the input details into mapping list
    
    @param  AttributeName - Control attribute name to be added into the list
    @param  ControlKey - Question ID to be added into the list
    @param  FormGuid - Formset guid for the control to be added into the list
    @param  AttributePtr - ATTRIBUTES for the control to be added into the list
    
    @return EFI_STATUS
    @retval EFI_SUCCESS - Mapping details added into the list successfully else other status
**/

EFI_STATUS AddMappingIdToList (CHAR16 *AttributeName, UINT16 ControlKey, EFI_GUID FormGuid, void *AttributePtr)
{
    MAPPING_ID_DETAILS          *MappingIdDetails;
    FORMSET_MAPPING_DETAILS     *FormsetMappingDetails;
    FORMSET_MAPPING_DETAILS     *CmpFormsetMappingDetails;
    UINTN                       iIndex = 0;
    UINTN                       jIndex = 0;
    EFI_STATUS                  Status = EFI_SUCCESS;
    
    if (NULL == gFormsetMappingList)
    {
        gFormsetMappingList = AllocateZeroPool (sizeof (FORMSET_MAPPING_LIST));
        if (NULL == gFormsetMappingList)
        {
            Status = EFI_OUT_OF_RESOURCES;
            goto DONE;
        }
        gFormsetMappingList->FormsetMappingCount = 0;
        gFormsetMappingList->FormsetMappingReservedCount = FORMSET_MEM_ALLOCATION;  //Lets assume 20 will be maximum formset. If more dynamic allocation will come into picture
        gFormsetMappingList->FormsetMappingDetails = AllocateZeroPool (sizeof (FORMSET_MAPPING_DETAILS) * gFormsetMappingList->FormsetMappingReservedCount);
        if (NULL == gFormsetMappingList->FormsetMappingDetails)
        {
            Status = EFI_OUT_OF_RESOURCES;
            goto DONE;
        }
    }
    if (gFormsetMappingList->FormsetMappingCount >= gFormsetMappingList->FormsetMappingReservedCount)
    {
        gFormsetMappingList->FormsetMappingDetails = MemReallocateZeroPool (
                                                                gFormsetMappingList->FormsetMappingDetails, 
                                                                (sizeof (FORMSET_MAPPING_DETAILS) * gFormsetMappingList->FormsetMappingReservedCount),
                                                                (sizeof (FORMSET_MAPPING_DETAILS) * (gFormsetMappingList->FormsetMappingReservedCount + FORMSET_MEM_ALLOCATION))
                                                                );
        if (NULL == gFormsetMappingList->FormsetMappingDetails)
        {
            Status = EFI_OUT_OF_RESOURCES;
            goto DONE;
        }
        gFormsetMappingList->FormsetMappingReservedCount += FORMSET_MEM_ALLOCATION;
    }
    for (iIndex = 0; iIndex < gFormsetMappingList->FormsetMappingCount; iIndex ++)
    {
        if (TRUE == CompareGuid (&gFormsetMappingList->FormsetMappingDetails [iIndex]->GuidValue, &FormGuid))
        {
            FormsetMappingDetails = gFormsetMappingList->FormsetMappingDetails [iIndex];
            break;
            
        }
        
    }
    if (iIndex < gFormsetMappingList->FormsetMappingCount)
    {
        CmpFormsetMappingDetails = gFormsetMappingList->FormsetMappingDetails [iIndex];
        if (!AllowDuplicateMapID ())
        {
            for (jIndex = 0; jIndex < CmpFormsetMappingDetails->MappingIdCount; jIndex ++)
            {
                if (!StrCmp (CmpFormsetMappingDetails->MappingIdDetails [jIndex]->MappingId, AttributeName))
                {
                    Status = EFI_OUT_OF_RESOURCES;
                    goto DONE;
                }
            }
        }
        if (CmpFormsetMappingDetails->MappingIdCount >= CmpFormsetMappingDetails->MappingIdReservedCount)
        {
            CmpFormsetMappingDetails->MappingIdDetails = MemReallocateZeroPool (
                                                                    CmpFormsetMappingDetails->MappingIdDetails, 
                                                                    (sizeof (MAPPING_ID_DETAILS) * CmpFormsetMappingDetails->MappingIdReservedCount),
                                                                    (sizeof (MAPPING_ID_DETAILS) * CmpFormsetMappingDetails->MappingIdReservedCount + CONTROLS_IN_FORMSET_ALLOCATION));
            if (NULL == CmpFormsetMappingDetails->MappingIdDetails)
            {
                Status = EFI_OUT_OF_RESOURCES;
                goto DONE;
            }
            CmpFormsetMappingDetails->MappingIdReservedCount += CONTROLS_IN_FORMSET_ALLOCATION;
        }
    }
    else
    {
        FormsetMappingDetails = AllocateZeroPool (sizeof (FORMSET_MAPPING_DETAILS));
        if (NULL == FormsetMappingDetails)
        {
            Status = EFI_OUT_OF_RESOURCES;
            goto DONE;
        }
        FormsetMappingDetails->MappingIdCount = 0;
        FormsetMappingDetails->MappingIdReservedCount = CONTROLS_IN_FORMSET_ALLOCATION;
        FormsetMappingDetails->MappingIdDetails = AllocateZeroPool (sizeof (MAPPING_ID_DETAILS) * FormsetMappingDetails->MappingIdReservedCount);
        MemCpy (&(FormsetMappingDetails->GuidValue), &FormGuid, sizeof (EFI_GUID));
        gFormsetMappingList->FormsetMappingDetails [gFormsetMappingList->FormsetMappingCount] = FormsetMappingDetails;
        gFormsetMappingList->FormsetMappingCount ++;
    }
    MappingIdDetails = AllocateZeroPool (sizeof (MAPPING_ID_DETAILS));
    if (NULL == MappingIdDetails)
    {
        Status = EFI_OUT_OF_RESOURCES;
        goto DONE;
    }
//<Wangbn001-20200110-Fixed POST hang 92 issue, memory overflow caused hang +>
//overflow occurred while copy the name of hard disk if the name more than 15 chars
    //StrCpy (MappingIdDetails->MappingId, AttributeName);
    StrCpyS (MappingIdDetails->MappingId, sizeof(MappingIdDetails->MappingId), AttributeName);
//<Wangbn001-20200110-Fixed POST Hang 92 issue if some Hard disk present ->
    MappingIdDetails->ControlKey = ControlKey;
    MappingIdDetails->AttributePtr = AttributePtr;
            
    FormsetMappingDetails->MappingIdDetails [FormsetMappingDetails->MappingIdCount] = MappingIdDetails;
    FormsetMappingDetails->MappingIdCount ++;

DONE:    
    return Status;
}

/**
    Checks the conditional pointer is with in control scope
    
    @param  CondPtr - Input conditional pointer to be verified for scope with in control pointer
    @param  ControlPtr - Control pointer
    @param  Offset - Output offset to be returned with parsed offset
    
    @return BOOLEAN
    @retval TRUE - If condition ptr found with in control scope
    @retval FALSE - If condition ptr not found with in control scope
**/

BOOLEAN CheckForValidCondition (void *CondPtr, void *ControlPtr, UINTN *Offset)
{
    EFI_IFR_OP_HEADER   *opHeader = (EFI_IFR_OP_HEADER *) CondPtr;
    BOOLEAN             Status = FALSE;
    UINTN               ScopeCount = 0;
    
    *Offset = 0;
    if (NULL == opHeader)
    {
        return Status;
    }
    if (opHeader->Scope)
    {
        ScopeCount++;
    }
    while (ScopeCount)
    {
        switch (opHeader->OpCode)
        {
            case EFI_IFR_END_OP:
                if (ScopeCount)
                {
                    ScopeCount --;
                }
                //break;
            default:
                if (ScopeCount)
                {
                    opHeader = (EFI_IFR_OP_HEADER *)((UINT8 *)opHeader + opHeader->Length);
                }
                break;
        }
        if (opHeader->Scope)
        {
            ScopeCount ++;
        }
    }
    if ((UINT8 *)opHeader >= (UINT8 *)ControlPtr)
    {
//        if (ScopeCount)
//        {
        Status = TRUE;
//        }
    }
    else
    {
        *Offset = (UINT8 *)opHeader - (UINT8 *)CondPtr;
    }
    return Status;
}

/**
    Checks for condition opcodes within next scope
    
    @param  EFI_IFR_OP_HEADER - Input Opcode header
    
    @return BOOLEAN
    @retval TRUE - If condition found with in next scope
    @retval FALSE - If condition not found with in next scope
**/

BOOLEAN CheckForNotAndOrWithInNextScope (EFI_IFR_OP_HEADER *opHeader)
{
    UINTN               ScopeCount = 0;
    EFI_STATUS          Status = FALSE;
    
    if (NULL == opHeader)
    {
        return FALSE;
    }
    if (opHeader->Scope)
    {
        ScopeCount++;
    }
    while (1 == ScopeCount)
    {
        switch (*(UINT8 *)opHeader)
        {
            case EFI_IFR_AND_OP:
            case EFI_IFR_OR_OP:
            case EFI_IFR_NOT_OP:
            case EFI_IFR_STRING_REF1_OP:
            case EFI_IFR_STRING_REF2_OP:
            case EFI_IFR_CONDITIONAL_OP:
            case EFI_IFR_UINT64_OP:
                return TRUE;
            case EFI_IFR_END_OP:
                return FALSE;
        }
        opHeader = (EFI_IFR_OP_HEADER *)((UINT8*)opHeader + opHeader->Length);
        if (opHeader->Scope)
        {
            ScopeCount --;
        }
    }
    return FALSE;
}

/**
    Parses the input CondPtr and creates conditional dependencies
    
	@param  ControlInfo - Input control info
    @param  CondPtr - CondPtr to be parsed and added to the conditional dependencies
    @param  Offset - Output the offset parsed with CondPtr
    @param  Dependencies - Output the dependency
    @param  AttributeName - Current control attribute name
	@param  ConstantProperty - Map property to return
    
    @return EFI_STATUS
    @retval EFI_SUCCESS - for success case else other status
**/

EFI_STATUS AddToDependencies (CONTROL_INFO *ControlInfo, UINT8 *CondPtr, UINTN *Offset, DEPENDENCIES  **Dependencies, CHAR16 *AttributeName, MAP_PROPERTY *ConstantProperty)
{
    UINTN               ScopeCount = 0;
    EFI_IFR_OP_HEADER   *opHeader = (EFI_IFR_OP_HEADER *) CondPtr;
    EFI_QUESTION_ID     QuestionId = 0;
    DEPENDENCIES        *TempDependencies = NULL;
    DEPENDENCIES        *ChildDependencies = NULL;
    MAP_PROPERTY        MapToProperty;
    EFI_IFR_EQ_ID_VAL_LIST  *EqIdValList = NULL;
    UINT16                  EqIdValListLength = 0;
    UINT16              EqIdValListEntry = 0;
    UINTN               SupportedCount = 0;
    MAP_CONDITION       MapCondition = EQU;
    EFI_STATUS          Status = EFI_SUCCESS;
    EFI_STATUS          RetStatus = EFI_SUCCESS;
    INTN                Result;
    EFI_HII_VALUE       Data1;
    EFI_HII_VALUE       Data2;
    EFI_HII_VALUE       Data3;
    EFI_HII_VALUE       *Value = NULL;
    EFI_HII_VALUE       *questionValue = NULL;
    EFI_HII_VALUE       *questionValue2 = NULL;
    
    *Offset = 0;
    *Dependencies = NULL;
    if (NULL == opHeader)
    {
        return EFI_INVALID_PARAMETER;
    }
    if (opHeader->Scope)
    {
        ScopeCount++;
    }
    ResetExpressionStack ();
    
    questionValue = (EFI_HII_VALUE*)EfiLibAllocateZeroPool(sizeof(EFI_HII_VALUE));
    if(questionValue == NULL)
    {
        goto ERROR;
    }

    questionValue2 = (EFI_HII_VALUE*)EfiLibAllocateZeroPool(sizeof(EFI_HII_VALUE));
    if(questionValue2 == NULL)
    {
        goto ERROR;
    }
    
    while (1 == ScopeCount)
    {
        EfiZeroMem (&Data1, sizeof (EFI_HII_VALUE));
        EfiZeroMem (&Data2, sizeof (EFI_HII_VALUE));
        EfiZeroMem (&Data3, sizeof (EFI_HII_VALUE));
        EfiZeroMem (questionValue, sizeof(EFI_HII_VALUE));
        EfiZeroMem (questionValue2, sizeof(EFI_HII_VALUE));
        
        Value = &Data3;
        Value->Type = EFI_IFR_TYPE_BOOLEAN;
        Status = EFI_SUCCESS;
        
        switch (*(UINT8 *)opHeader)
        {
            case EFI_IFR_EQ_ID_VAL_OP:
                if (SupportedCount >= SUPPORTED_CONDITIONS)
                {
                    DEBUG((DEBUG_ERROR, "\nEFI_IFR_EQ_ID_VAL_OP found after supported two conditions for mapping ID %S", AttributeName));
                    opHeader = (EFI_IFR_OP_HEADER *)((UINT8*)opHeader + opHeader->Length);
                    *Offset = (UINT8 *)opHeader - CondPtr;
                    goto ERROR;
                }
                
                Value = &Data1;
                Status = PopExpression (Value);
                if (!EFI_ERROR (Status)) {
                  DEBUG((DEBUG_ERROR, "\nEFI_IFR_EQ_ID_VAL_OP found together with constant evaluation for mapping ID %S", AttributeName));
                  goto ERROR;
                }
                if (NULL == *Dependencies)
                {
                    *Dependencies = AllocateZeroPool (sizeof (DEPENDENCIES));
                    if (*Dependencies)
                    {
                        TempDependencies = *Dependencies;
                    }
                }
                else if (NULL == ChildDependencies)
                {
                    ChildDependencies = AllocateZeroPool (sizeof (DEPENDENCIES));
                    if (ChildDependencies)
                    {
                        (*Dependencies)->Child = ChildDependencies;
                    }
                    TempDependencies = ChildDependencies;
                }
                QuestionId = ((EFI_IFR_EQ_ID_VAL*)opHeader)->QuestionId;
                TempDependencies->MapToProperty = MapToProperty;
                TempDependencies->ControlKey = QuestionId;
                TempDependencies->MapFromValue = ((EFI_IFR_EQ_ID_VAL*)opHeader)->Value;
                TempDependencies->MapCondition = CurrentValue;
                SupportedCount ++;
                break;
            case EFI_IFR_EQ_ID_ID_OP:
                DEBUG((DEBUG_ERROR, "\nEFI_IFR_EQ_ID_ID_OP condtion not supported for mapping ID %S", AttributeName));
                goto ERROR;
                break;
            case EFI_IFR_EQ_ID_VAL_LIST_OP:
                EqIdValList = (EFI_IFR_EQ_ID_VAL_LIST*)opHeader;
                EqIdValListLength = EqIdValList->ListLength;
                QuestionId = EqIdValList->QuestionId;
                if ((SupportedCount + EqIdValListLength) > SUPPORTED_CONDITIONS)
                {
                    DEBUG((DEBUG_ERROR, "\nEFI_IFR_EQ_ID_VAL_LIST_OP found after supported two conditions for mapping ID %S", AttributeName));
                    opHeader = (EFI_IFR_OP_HEADER *)((UINT8*)opHeader + opHeader->Length);
                    *Offset = (UINT8 *)opHeader - CondPtr;
                    goto ERROR;
                }
                Value = &Data1;
                Status = PopExpression (Value);
                if (!EFI_ERROR (Status)) {
                  DEBUG((DEBUG_ERROR, "\nEFI_IFR_EQ_ID_VAL_LIST_OP found together with constant evaluation for mapping ID %S", AttributeName));
                  goto ERROR;
                }
                
                EqIdValListEntry = 0;
                if (NULL == *Dependencies)
                {
                    *Dependencies = AllocateZeroPool (sizeof (DEPENDENCIES));
                    if (*Dependencies)
                    {
                        (*Dependencies)->MapToProperty = MapToProperty;
                        (*Dependencies)->ControlKey = QuestionId;
                        (*Dependencies)->MapFromValue = EqIdValList->ValueList [EqIdValListEntry];
                        (*Dependencies)->MapCondition = CurrentValue;
                        EqIdValListEntry ++;
                    }
                }
                if (NULL == ChildDependencies)
                {
                    ChildDependencies = AllocateZeroPool (sizeof (DEPENDENCIES));
                    if (ChildDependencies)
                    {
                        (*Dependencies)->Child = ChildDependencies;
                        ChildDependencies->MapToProperty = MapToProperty;
                        ChildDependencies->ControlKey = QuestionId;
                        ChildDependencies->MapFromValue = EqIdValList->ValueList [EqIdValListEntry];
                        ChildDependencies->MapCondition = CurrentValue;
                        if (1 == EqIdValListEntry)
                        {
                            ChildDependencies->MapTerms = OR;
                        }
                        EqIdValListEntry ++;
                    }
                }
                SupportedCount += EqIdValListEntry;
                break;
            case EFI_IFR_DUP_OP:
                DEBUG((DEBUG_ERROR, "\nEFI_IFR_DUP_OP condtion not supported for mapping ID %S", AttributeName));
                goto ERROR;
            case EFI_IFR_QUESTION_REF1_OP:
                DEBUG((DEBUG_ERROR, "\nEFI_IFR_QUESTION_REF1_OP condtion not supported for mapping ID %S", AttributeName));
                goto ERROR;
            case EFI_IFR_THIS_OP:
                DEBUG((DEBUG_ERROR, "\nEFI_IFR_THIS_OP condtion not supported for mapping ID %S", AttributeName));
                goto ERROR;                
            case EFI_IFR_QUESTION_REF3_OP:
                DEBUG((DEBUG_ERROR, "\nEFI_IFR_QUESTION_REF3_OP condtion not supported for mapping ID %S", AttributeName));
                goto ERROR;
            case EFI_IFR_RULE_REF_OP:
                DEBUG((DEBUG_ERROR, "\nEFI_IFR_RULE_REF_OP condtion not supported for mapping ID %S", AttributeName));
                goto ERROR;
            case EFI_IFR_STRING_REF1_OP:
                DEBUG((DEBUG_ERROR, "\nEFI_IFR_STRING_REF1_OP condtion not supported for mapping ID %S", AttributeName));
                goto ERROR;                
            case EFI_IFR_TRUE_OP:
                Value->Value.b = TRUE;
                Status = PushExpression (Value);
                if (EFI_ERROR (Status))
                {
                  goto ERROR;
                }
                break;
//                DEBUG((DEBUG_ERROR, "\nEFI_IFR_TRUE_OP condtion not supported for mapping ID %S", AttributeName));
//                goto ERROR;
            case EFI_IFR_FALSE_OP:
                Value->Value.b = FALSE;
                Status = PushExpression (Value);
                if (EFI_ERROR (Status))
                {
                  goto ERROR;
                }
                break;                
//                DEBUG((DEBUG_ERROR, "\nEFI_IFR_FALSE_OP condtion not supported for mapping ID %S", AttributeName));
//                goto ERROR;
            case EFI_IFR_ONE_OP:
#define ONE 1
                  Value->Type = EFI_IFR_TYPE_NUM_SIZE_8;
                  Value->Value.u8 = ONE;
                  Status = PushExpression (Value);
                  if (EFI_ERROR (Status))
                  {
                    goto ERROR;
                  }
                  break;
//                DEBUG((DEBUG_ERROR, "\nEFI_IFR_ONE_OP condtion not supported for mapping ID %S", AttributeName));
//                goto ERROR;                
            case EFI_IFR_ONES_OP:
#define ONES 0xFFFFFFFFFFFFFFFF
                  Value->Type = EFI_IFR_TYPE_NUM_SIZE_64;
                  Value->Value.u64 = ONES;
                  Status = PushExpression (Value);
                  if (EFI_ERROR (Status))
                  {
                    goto ERROR;
                  }
                  break;
//                DEBUG((DEBUG_ERROR, "\nEFI_IFR_ONES_OP condtion not supported for mapping ID %S", AttributeName));
//                goto ERROR;                
            case EFI_IFR_UINT8_OP:
                Value->Type = EFI_IFR_TYPE_NUM_SIZE_8;
                Value->Value.u8 = ((EFI_IFR_UINT8 *)opHeader)->Value;
                Status = PushExpression (Value);
                if (EFI_ERROR (Status))
                {
                  goto ERROR;
                }
                break;
//                DEBUG((DEBUG_ERROR, "\nEFI_IFR_UINT8_OP condtion not supported for mapping ID %S", AttributeName));
//                goto ERROR;                 
            case EFI_IFR_UINT16_OP:
                Value->Type = EFI_IFR_TYPE_NUM_SIZE_16;
                Value->Value.u16 = ((EFI_IFR_UINT16 *)opHeader)->Value;
                Status = PushExpression (Value);
                if (EFI_ERROR (Status))
                {
                  goto ERROR;
                }
                break;                
//                DEBUG((DEBUG_ERROR, "\nEFI_IFR_UINT16_OP condtion not supported for mapping ID %S", AttributeName));
//                goto ERROR;                 
            case EFI_IFR_UINT32_OP:
                Value->Type = EFI_IFR_TYPE_NUM_SIZE_32;
                Value->Value.u32 = ((EFI_IFR_UINT32 *)opHeader)->Value;
                Status = PushExpression (Value);
                if (EFI_ERROR (Status))
                {
                  goto ERROR;
                }
                break;                
//                DEBUG((DEBUG_ERROR, "\nEFI_IFR_UINT32_OP condtion not supported for mapping ID %S", AttributeName));
//                goto ERROR;                 
            case EFI_IFR_UINT64_OP:
                Value->Type = EFI_IFR_TYPE_NUM_SIZE_64;
                Value->Value.u64 = ((EFI_IFR_UINT64 *)opHeader)->Value;
                Status = PushExpression (Value);
                if (EFI_ERROR (Status))
                {
                  goto ERROR;
                }
                break;                
//                DEBUG((DEBUG_ERROR, "\nEFI_IFR_UINT64_OP condtion not supported for mapping ID %S", AttributeName));
//                goto ERROR;                 
            case EFI_IFR_UNDEFINED_OP:
                DEBUG((DEBUG_ERROR, "\nEFI_IFR_UNDEFINED_OP condtion not supported for mapping ID %S", AttributeName));
                goto ERROR;                
            case EFI_IFR_VERSION_OP:
                DEBUG((DEBUG_ERROR, "\nEFI_IFR_VERSION_OP condtion not supported for mapping ID %S", AttributeName));
                goto ERROR;                
            case EFI_IFR_ZERO_OP:
#define ZERO 0
                Value->Type = EFI_IFR_TYPE_NUM_SIZE_8;
                Value->Value.u8 = ZERO;
                Status = PushExpression (Value);
                if (EFI_ERROR (Status))
                {
                  goto ERROR;
                }
                break;
//                DEBUG((DEBUG_ERROR, "\nEFI_IFR_ZERO_OP condtion not supported for mapping ID %S", AttributeName));
//                goto ERROR;                
            case EFI_IFR_LENGTH_OP:
                DEBUG((DEBUG_ERROR, "\nEFI_IFR_LENGTH_OP condtion not supported for mapping ID %S", AttributeName));
                goto ERROR;
            case EFI_IFR_NOT_OP:
                DEBUG((DEBUG_ERROR, "\nEFI_IFR_NOT_OP condtion not supported for mapping ID %S", AttributeName));
                goto ERROR;
            case EFI_IFR_QUESTION_REF2_OP:
                DEBUG((DEBUG_ERROR, "\nEFI_IFR_QUESTION_REF2_OP condtion not supported for mapping ID %S", AttributeName));
                goto ERROR;
            case EFI_IFR_STRING_REF2_OP:
                DEBUG((DEBUG_ERROR, "\nEFI_IFR_STRING_REF2_OP condtion not supported for mapping ID %S", AttributeName));
                goto ERROR;
            case EFI_IFR_TO_BOOLEAN_OP:
                DEBUG((DEBUG_ERROR, "\nEFI_IFR_TO_BOOLEAN_OP condtion not supported for mapping ID %S", AttributeName));
                goto ERROR;                
            case EFI_IFR_TO_STRING_OP:
                DEBUG((DEBUG_ERROR, "\nEFI_IFR_TO_STRING_OP condtion not supported for mapping ID %S", AttributeName));
                goto ERROR;
            case EFI_IFR_TO_UINT_OP:
                DEBUG((DEBUG_ERROR, "\nEFI_IFR_TO_UINT_OP condtion not supported for mapping ID %S", AttributeName));
                goto ERROR;
            case EFI_IFR_TO_LOWER_OP:
                DEBUG((DEBUG_ERROR, "\nEFI_IFR_TO_LOWER_OP condtion not supported for mapping ID %S", AttributeName));
                goto ERROR;
            case EFI_IFR_TO_UPPER_OP:
                DEBUG((DEBUG_ERROR, "\nEFI_IFR_TO_UPPER_OP condtion not supported for mapping ID %S", AttributeName));
                goto ERROR;
            case EFI_IFR_BITWISE_NOT_OP:
                DEBUG((DEBUG_ERROR, "\nEFI_IFR_BITWISE_NOT_OP condtion not supported for mapping ID %S", AttributeName));
                goto ERROR;                
            case EFI_IFR_ADD_OP:
            case EFI_IFR_SUBTRACT_OP:
            case EFI_IFR_MULTIPLY_OP:
            case EFI_IFR_DIVIDE_OP:
            case EFI_IFR_MODULO_OP:
            case EFI_IFR_BITWISE_AND_OP:
            case EFI_IFR_BITWISE_OR_OP:
            case EFI_IFR_SHIFT_LEFT_OP:
            case EFI_IFR_SHIFT_RIGHT_OP:
                //
                // Pop an expression from the expression stack
                //
                Status = PopExpression (&Data2);
                if (EFI_ERROR (Status)) {
                  goto ERROR;
                }
                if (Data2.Type > EFI_IFR_TYPE_DATE) {
                  Status = EFI_INVALID_PARAMETER;
                  goto ERROR;
                }

                //
                // Pop another expression from the expression stack
                //
                Status = PopExpression (&Data1);
                if (EFI_ERROR (Status)) {
                  goto ERROR;
                }
                if (Data1.Type > EFI_IFR_TYPE_DATE) {
                  Status = EFI_INVALID_PARAMETER;
                  goto ERROR;
                }

                Value->Type = EFI_IFR_TYPE_NUM_SIZE_64;

                switch (opHeader->OpCode) {
                  case EFI_IFR_ADD_OP:
                    Value->Value.u64 = Data1.Value.u64 + Data2.Value.u64;
                    break;

                  case EFI_IFR_SUBTRACT_OP:
                    Value->Value.u64 = Data1.Value.u64 - Data2.Value.u64;
                    break;

                  case EFI_IFR_MULTIPLY_OP:
                    Value->Value.u64 = MultU64x32 (Data1.Value.u64, (UINT32)Data2.Value.u64);
                    break;

                  case EFI_IFR_DIVIDE_OP:
                    Value->Value.u64 = AmiTseDivU64x32 (Data1.Value.u64, (UINT32)Data2.Value.u64,NULL);
                    break;

                  case EFI_IFR_MODULO_OP:
                    Value->Value.u64 = 0;
                    AmiTseDivU64x32(Data1.Value.u64, (UINT32)Data2.Value.u64, (UINTN*)&Value->Value.u64);
                    break;

                  case EFI_IFR_BITWISE_AND_OP:
                    Value->Value.u64 = Data1.Value.u64 & Data2.Value.u64;
                    break;

                  case EFI_IFR_BITWISE_OR_OP:
                    Value->Value.u64 = Data1.Value.u64 | Data2.Value.u64;
                    break;

                  case EFI_IFR_SHIFT_LEFT_OP:
                    Value->Value.u64 = LShiftU64 (Data1.Value.u64, (UINTN) Data2.Value.u64);
                    break;

                  case EFI_IFR_SHIFT_RIGHT_OP:
                    Value->Value.u64 = RShiftU64 (Data1.Value.u64, (UINTN) Data2.Value.u64);
                    break;

                  default:
                    break;
                }
                Status = PushExpression (Value);
                if (EFI_ERROR (Status))
                {
                  goto ERROR;
                }
                break;
            case EFI_IFR_AND_OP:
            case EFI_IFR_OR_OP:
                if (NULL != ChildDependencies && (EFI_IFR_AND_OP == *(UINT8 *)opHeader))
                {
                    ChildDependencies->MapTerms = AND;
                }
                else if (NULL != ChildDependencies && (EFI_IFR_OR_OP == *(UINT8 *)opHeader))
                {
                    ChildDependencies->MapTerms = OR;
                }
                else if (NULL == *Dependencies)
                {
                    //
                    // Two Boolean operator
                    //
                    Status = PopExpression (&Data2);
                    if (EFI_ERROR (Status)) {
                      goto ERROR;
                    }
                    if (Data2.Type != EFI_IFR_TYPE_BOOLEAN) {
                      Status = EFI_INVALID_PARAMETER;
                      goto ERROR;
                    }

                    //
                    // Pop another expression from the expression stack
                    //
                    Status = PopExpression (&Data1);
                    if (EFI_ERROR (Status)) {
                      goto ERROR;
                    }
                    if (Data1.Type != EFI_IFR_TYPE_BOOLEAN) {
                      Status = EFI_INVALID_PARAMETER;
                      goto ERROR;
                    }

                    if (opHeader->OpCode == EFI_IFR_AND_OP) {
                      Value->Value.b = Data1.Value.b && Data2.Value.b;
                    } else {
                      Value->Value.b = Data1.Value.b || Data2.Value.b;
                    }
                    Status = PushExpression (Value);
                    if (EFI_ERROR (Status))
                    {
                      goto ERROR;
                    }
//                    DEBUG((DEBUG_ERROR, "\nEFI_IFR_AND_OP found in wrong place for mapping ID %S", AttributeName));
                }
                else 
                {
                    DEBUG((DEBUG_ERROR, "\nEFI_IFR_AND_OP or EFI_IFR_OR_OP found in wrong place for mapping ID %S", AttributeName));
                }

                break;
            case EFI_IFR_EQUAL_OP:
            case EFI_IFR_NOT_EQUAL_OP:
            case EFI_IFR_GREATER_EQUAL_OP:
            case EFI_IFR_GREATER_THAN_OP:
            case EFI_IFR_LESS_EQUAL_OP:
            case EFI_IFR_LESS_THAN_OP:
                //
                // Compare two integer, string, boolean or date/time
                //
                Status = PopExpression (&Data2);
                if (EFI_ERROR (Status)) {
                  goto ERROR;
                }
                if (Data2.Type > EFI_IFR_TYPE_BOOLEAN && Data2.Type != EFI_IFR_TYPE_STRING) {
                  Status = EFI_INVALID_PARAMETER;
                  goto ERROR;
                }

                //
                // Pop another expression from the expression stack
                //
                Status = PopExpression (&Data1);
                if (EFI_ERROR (Status)) {
                    goto ERROR;
                }

                Result = CompareHiiValue (&Data1, &Data2, ControlInfo->ControlHandle);
                if (Result == EFI_INVALID_PARAMETER) {
                  Status = EFI_INVALID_PARAMETER;
                  goto ERROR;
                }

                switch (opHeader->OpCode) {
                case EFI_IFR_EQUAL_OP:
                  Value->Value.b = (Result == 0) ? TRUE : FALSE;
                  break;

                case EFI_IFR_NOT_EQUAL_OP:
                  Value->Value.b = (Result != 0) ? TRUE : FALSE;
                  break;

                case EFI_IFR_GREATER_EQUAL_OP:
                  Value->Value.b = (Result >= 0) ? TRUE : FALSE;
                  break;

                case EFI_IFR_GREATER_THAN_OP:
                  Value->Value.b = (Result > 0) ? TRUE : FALSE;
                  break;

                case EFI_IFR_LESS_EQUAL_OP:
                  Value->Value.b = (Result <= 0) ? TRUE : FALSE;
                  break;

                case EFI_IFR_LESS_THAN_OP:
                  Value->Value.b = (Result < 0) ? TRUE : FALSE;
                  break;

                default:
                  break;
                }
                Status = PushExpression (Value);
                if (EFI_ERROR (Status))
                {
                  goto ERROR;
                }
                break;
            case EFI_IFR_MATCH_OP:
                DEBUG((DEBUG_ERROR, "\nEFI_IFR_MATCH_OP condtion not supported for mapping ID %S", AttributeName));
                goto ERROR;
            case EFI_IFR_MATCH2_OP:
                DEBUG((DEBUG_ERROR, "\nEFI_IFR_MATCH2_OP condtion not supported for mapping ID %S", AttributeName));
                goto ERROR;
            case EFI_IFR_CATENATE_OP:
                DEBUG((DEBUG_ERROR, "\nEFI_IFR_CATENATE_OP condtion not supported for mapping ID %S", AttributeName));
                goto ERROR;                
            case EFI_IFR_CONDITIONAL_OP:
                DEBUG((DEBUG_ERROR, "\nEFI_IFR_CONDITIONAL_OP condtion not supported for mapping ID %S", AttributeName));
                goto ERROR;                
            case EFI_IFR_FIND_OP:
                DEBUG((DEBUG_ERROR, "\nEFI_IFR_FIND_OP condtion not supported for mapping ID %S", AttributeName));
                goto ERROR;
            case EFI_IFR_MID_OP:
                DEBUG((DEBUG_ERROR, "\nEFI_IFR_MID_OP condtion not supported for mapping ID %S", AttributeName));
                goto ERROR;                
            case EFI_IFR_TOKEN_OP:
                DEBUG((DEBUG_ERROR, "\nEFI_IFR_TOKEN_OP condtion not supported for mapping ID %S", AttributeName));
                goto ERROR;
            case EFI_IFR_SPAN_OP:
                DEBUG((DEBUG_ERROR, "\nEFI_IFR_SPAN_OP condtion not supported for mapping ID %S", AttributeName));
                goto ERROR;
            case EFI_IFR_DISABLE_IF_OP:
            case EFI_IFR_SUPPRESS_IF_OP:
                MapToProperty = Hidden;
                break;
            case EFI_IFR_GRAY_OUT_IF_OP:
                MapToProperty = GrayOut;
                break;
        }
        opHeader = (EFI_IFR_OP_HEADER *)((UINT8*)opHeader + opHeader->Length);
        if (opHeader->Scope)
        {
            if (FALSE == CheckForNotAndOrWithInNextScope (opHeader))
            {
                ScopeCount++;
            }
        }
    }
    Value = &Data1;
    Status = PopExpression (Value);
    if ((!EFI_ERROR (Status)) && (NULL != *Dependencies)) {
      goto ERROR;
    }
    else if (!EFI_ERROR (Status))
    {
        if (1 == Value->Value.u8)
        {
            if (MapToProperty == GrayOut)
            {
                *ConstantProperty = GrayOut;
            }
            else
            {
                DEBUG((DEBUG_ERROR, "\nConditional expression evaluated to hidden for mapping ID %S", AttributeName));
                goto ERROR;
            }
        }
        else
        {
            *ConstantProperty = 0;
        }
		RetStatus = EFI_NOT_FOUND;
        
    }
    *Offset = (UINT8 *)opHeader - CondPtr;
    MemFreePointer((void**)&questionValue);
    MemFreePointer((void**)&questionValue2);
    
    return RetStatus;
ERROR:
    RetStatus = EFI_UNSUPPORTED;
    MemFreePointer((void**)&questionValue);
    MemFreePointer((void**)&questionValue2);

    if (NULL != (*Dependencies))
    {
        if (NULL != (*Dependencies)->Child)
        {
            MemFreePointer (&((*Dependencies)->Child));
        }
        MemFreePointer (&(*Dependencies));
    }
    return RetStatus;
}

/**
    Frees the AttributeDependencies list
    
    @param  DEPENDENCIES - Memory to be freed
    @retval void
**/

void FreeAttribDependencies (DEPENDENCIES  **AttribDependencies)
{
    DEPENDENCIES    *TempAttribDependencies = NULL;
    
    if (NULL == *AttribDependencies)
    {
        return;
    }
    TempAttribDependencies = *AttribDependencies;
    while (TempAttribDependencies)
    {
        TempAttribDependencies = TempAttribDependencies->Next;
        
        if (NULL != (*AttribDependencies))
        {
            if (NULL != (*AttribDependencies)->Child)
            {
                MemFreePointer (&((*AttribDependencies)->Child));
            }
            MemFreePointer (&(*AttribDependencies));
        }
        (*AttribDependencies) = TempAttribDependencies;
    }
}

/**
    Creates dependencies for input controlInfo
    
    @param  controlInfo - Control Info to which dependencies to be created
    @param  AttribDependencies - Output the dependencies here
    @param  AttributeName - Current control attribute name
	@param  ReadOnly - OUT to denote grayout true condition
    
    @return EFI_STATUS
    @retval EFI_SUCCESS - for success case else other status
**/

EFI_STATUS AddControlDependencies (CONTROL_INFO *controlInfo, DEPENDENCIES  **AttribDependencies, CHAR16 *AttributeName, BOOLEAN *ReadOnly)
{
    UINT8           *CondPtr = NULL;
    UINTN           Offset = 0;
    EFI_STATUS      Status = EFI_SUCCESS;
    DEPENDENCIES    *Dependencies = NULL;
    DEPENDENCIES    *TempAttribDependencies = NULL;
    MAP_PROPERTY    ConstantProperty = 0;
    MAP_PROPERTY    TempConstantProperty = 0;
    
    
    if (NULL == controlInfo->ControlPtr || NULL == controlInfo->ControlConditionalPtr || controlInfo->ControlFlags.ControlRWEvaluate)
    {
        return EFI_UNSUPPORTED;
    }
    CondPtr = (UINT8 *)controlInfo->ControlConditionalPtr;
    
    if (
            (*CondPtr != EFI_IFR_DISABLE_IF_OP) &&
            (*CondPtr != EFI_IFR_SUPPRESS_IF_OP) &&
            (*CondPtr != EFI_IFR_GRAY_OUT_IF_OP)
        )
    {
        return EFI_UNSUPPORTED;
    }
    while ((UINT8*)CondPtr < (UINT8*)controlInfo->ControlPtr)
    {
        Offset = 0;
        switch (*CondPtr)
        {
            case EFI_IFR_DISABLE_IF_OP:
            case EFI_IFR_SUPPRESS_IF_OP:
            case EFI_IFR_GRAY_OUT_IF_OP:
                Dependencies = NULL;
                if (TRUE == CheckForValidCondition (CondPtr, controlInfo->ControlPtr, &Offset))
                {
                    Offset = 0;
                    Status = AddToDependencies (controlInfo, CondPtr, &Offset, &Dependencies, AttributeName, &TempConstantProperty);
                    if (!EFI_ERROR (Status) && (EFI_NOT_FOUND != Status))
                    {
                        if (NULL == *AttribDependencies)
                        {
                            *AttribDependencies = Dependencies;
                        }
                        else
                        {
                            TempAttribDependencies = *AttribDependencies;
                            while (TempAttribDependencies->Next)
                            {
                                TempAttribDependencies = TempAttribDependencies->Next;
                            }
                            TempAttribDependencies->Next = Dependencies;
                        }
                    }
                    else if (EFI_NOT_FOUND == Status)
                    {
                        Status = EFI_SUCCESS;
                        ConstantProperty |= TempConstantProperty;
                    }
                    else
                    {
                        FreeAttribDependencies (AttribDependencies);
                        *AttribDependencies = NULL;
                        return EFI_INVALID_PARAMETER;
                    }
                }
                CondPtr += Offset;
                break;
            default:
                CondPtr = (UINT8*)CondPtr + ((EFI_IFR_OP_HEADER*)CondPtr)->Length;
                break;
        }
    }
    if ((ConstantProperty & GrayOut) == GrayOut)
    {
        *ReadOnly = TRUE;
        //FreeAttribDependencies (AttribDependencies);
        //*AttribDependencies = NULL;
    }
    return Status;
}

/**
  Returns the ATTRIBUTES structure for the passed conditions
    
  @param  DEPENDENCIES - Condition for the control
  @param  EFI_GUID - Formset guid to verify the condition

  @return ATTRIBUTES *
  @retval ATTRIBUTES * - If current condition found in the mapping list
  @retval NULL - If current condition doesn't found in the mapping list
**/

ATTRIBUTES *GetAttributesDetail (DEPENDENCIES *Dependencies, EFI_GUID PageGuid)
{
    BOOLEAN RetVal = FALSE;
    UINTN   iIndex = 0;
    UINTN   jIndex = 0;
    DEPENDENCIES *TempDependencies = Dependencies;
    FORMSET_MAPPING_DETAILS *FromsetMappingDetails = NULL;
    
    if (NULL == gFormsetMappingList)
    {
        return NULL;
    }
    for (iIndex = 0; iIndex < gFormsetMappingList->FormsetMappingCount; iIndex ++)
    {
        if (TRUE == CompareGuid (&gFormsetMappingList->FormsetMappingDetails [iIndex]->GuidValue, &PageGuid))
        {
            FromsetMappingDetails = gFormsetMappingList->FormsetMappingDetails [iIndex];
            for (jIndex = 0; jIndex < FromsetMappingDetails->MappingIdCount; jIndex ++)
            {
                if (FromsetMappingDetails->MappingIdDetails [jIndex]->ControlKey == TempDependencies->ControlKey)
                {
                    return FromsetMappingDetails->MappingIdDetails [jIndex]->AttributePtr;
                }
            }
            break;
        }        
    }
    return NULL;
}

/**
  Checks all the conditions or valid as per the Redfish schema
    
  @param  DEPENDENCIES - Conditions array for the control
  @param  EFI_GUID - Formset guid to verify the conditions

  @return BOOLEAN
  @retval TRUE - If conditions are satisfied as per schema
  @retval FALSE - If conditions are not satisfied as per schema
**/

BOOLEAN CheckValidDependency (DEPENDENCIES *Dependencies, EFI_GUID PageGuid)
{
    BOOLEAN RetVal = FALSE;
    UINTN   iIndex = 0;
    UINTN   jIndex = 0;
    BOOLEAN RetValue = FALSE;
    DEPENDENCIES *TempDependencies = Dependencies;
    
    FORMSET_MAPPING_DETAILS *FromsetMappingDetails = NULL;
    
    if (NULL == gFormsetMappingList)
    {
        return FALSE;
    }
    while (TempDependencies)
    {
        for (iIndex = 0; iIndex < gFormsetMappingList->FormsetMappingCount; iIndex ++)
        {
            if (TRUE == CompareGuid (&gFormsetMappingList->FormsetMappingDetails [iIndex]->GuidValue, &PageGuid))
            {
                FromsetMappingDetails = gFormsetMappingList->FormsetMappingDetails [iIndex];
                for (jIndex = 0; jIndex < FromsetMappingDetails->MappingIdCount; jIndex ++)
                {
                    if (FromsetMappingDetails->MappingIdDetails [jIndex]->ControlKey == TempDependencies->ControlKey)
                    {
                        if (TempDependencies->Child)
                        {
                            return CheckValidDependency (TempDependencies->Child, PageGuid);
                        }
						else
						{
							return TRUE;
						}
                    }
                }
                if (jIndex == FromsetMappingDetails->MappingIdCount)
                {
                    return FALSE;
                }
            }        
        }
        if (iIndex == gFormsetMappingList->FormsetMappingCount)
        {
            return FALSE;
        }
        TempDependencies = TempDependencies->Next;
    }
    return TRUE;;
}


/**
  Writes the dependencies for incoming attribute
    
  @param  ATTRIBUTES - Attribute to which dependencies to be written

  @return EFI_STATUS
  @retval EFI_SUCCESS - for success case else other value
**/

EFI_STATUS WriteDependencyToFile (ATTRIBUTES *AttributeList)
{
    EFI_STATUS  Status = EFI_NOT_FOUND;
    cJSON       *DependenciesObject = NULL;
    cJSON       *DependencyObject = NULL;
    cJSON       *MapFromArray = NULL;
    cJSON       *MapFromFields = NULL;
    CHAR8       *String8 = NULL;
    CHAR16      ValueName [128] = {0};
    CHAR16      PrevAttribName [128] = {0};
    UINTN       iIndex = 0;
    UINTN       jIndex = 0;
    UINT8       kIndex  = 0;
    UINT8       lIndex  = 0;
    UINT8       ConditionCount = 0;
    DEPENDENCIES *TempDependencies = NULL;
    DEPENDENCIES **ConditionsDependencyArray= NULL;
    ATTRIBUTES  *DependencyAttributes = NULL;
    ATTRIBUTES  *DependencyForMapFrom = NULL;
    DEPENDENCY_OBJECTS *DependencyObjectsStruct = NULL;
    DEPENDENCY_OBJECTS *ConsDependencyObjectsStruct = NULL;
    DEPENDENCY_OBJECTS *TempDependencyObjectsStruct = NULL;
    extern cJSON *gDependencyArray;
    
    if (NULL == gDependencyArray)
    {
        return Status;
    }
    if (NULL == AttributeList->Dependencies)
    {
        if ((_CONTROL_TYPE_SUBMENU == AttributeList->_Type) || (_CONTROL_TYPE_TEXT == AttributeList->_Type) || (_CONTROL_TYPE_MEMO == AttributeList->_Type))
        {
            return EFI_UNSUPPORTED;
        }
        else if (AttributeList->_AttributeName && (!StrCmp (AttributeList->_AttributeName, SYSTEM_ACCESS_MAPPINGID)))
        {
            return EFI_UNSUPPORTED;
        }
        return EFI_SUCCESS;
    }
    if (FALSE == CheckValidDependency (AttributeList->Dependencies,  AttributeList->PageGuid))
    {
        DEBUG((DEBUG_ERROR, "\nConditional expression failed for lacking mapping ID %S", AttributeList->_AttributeName ));
        return EFI_UNSUPPORTED;
    }
    TempDependencies = AttributeList->Dependencies;
	ConditionsDependencyArray = AllocateZeroPool (2 * sizeof (DEPENDENCIES));
    ConditionsDependencyArray [0] = AllocateZeroPool (sizeof (DEPENDENCIES));
    ConditionsDependencyArray [1] = AllocateZeroPool (sizeof (DEPENDENCIES));
    while (TempDependencies)
    {
		ConditionCount = 0;
		StrCpy (PrevAttribName, L"");
        if (TempDependencies->Child)
        {
            ConditionsDependencyArray [0] = TempDependencies;
            ConditionsDependencyArray [1] = TempDependencies->Child;
            ConditionCount = 2;
        }
        else
        {
            ConditionCount = 1;
            ConditionsDependencyArray [0] = TempDependencies;
        }
        for (kIndex = 0; kIndex < ConditionCount; kIndex ++)
        {
            DependencyAttributes = GetAttributesDetail (ConditionsDependencyArray [kIndex],  AttributeList->PageGuid);
            if (NULL == DependencyAttributes)
            {
//<Wangbn001-20200106-Fixed memory leak issue +>
                if (NULL != ConditionsDependencyArray){
                    MemFreePointer ((void **)&ConditionsDependencyArray);
                }
//<Wangbn001-20200106-Fixed memory leak issue ->
                return EFI_UNSUPPORTED;
            }
            if (!(StrCmp (PrevAttribName, DependencyAttributes->_AttributeName)))
            {
                continue;
            }
            StrCpy (PrevAttribName, DependencyAttributes->_AttributeName);
            
            if (NULL != DependencyAttributes)
            {
                DependenciesObject = cJSON_CreateObject ();
                String8 = StrDup16to8 (DependencyAttributes->_AttributeName);
                cJSON_AddStringToObject (DependenciesObject, "DependencyFor", String8);
                MemFreePointer (&String8);
                cJSON_AddStringToObject (DependenciesObject, "Type", DependencyTypeStrings [0]);
                
                DependencyObject = cJSON_CreateObject();
                cJSON_AddItemToObject (DependenciesObject, "Dependency", DependencyObject);
                
                /*MapFrom Starts*/
                MapFromArray = cJSON_CreateArray ();
                
                /*MapFrom object starts*/
                for (lIndex = 0; lIndex < ConditionCount; lIndex ++)
                {
                    DependencyForMapFrom = GetAttributesDetail (ConditionsDependencyArray [lIndex],  AttributeList->PageGuid);
                    if (NULL != DependencyForMapFrom)
                    {
                        cJSON_AddItemToArray (MapFromArray, MapFromFields = cJSON_CreateObject());
                        if (0 != lIndex)
                        {
                            cJSON_AddStringToObject (MapFromFields, "MapTerms", MapTermsStrings [ConditionsDependencyArray [lIndex]->MapTerms ]);
                        }
                        String8 = StrDup16to8 (DependencyForMapFrom->_AttributeName);
                        cJSON_AddStringToObject (MapFromFields, "MapFromAttribute", String8);
                        MemFreePointer (&String8);
                        
                        cJSON_AddStringToObject (MapFromFields, "MapFromProperty", "CurrentValue");
                        cJSON_AddStringToObject (MapFromFields, "MapFromCondition", MapConditionSrings[ConditionsDependencyArray [lIndex]->MapCondition]);
                        
                        if (_CONTROL_TYPE_POPUPSEL == DependencyForMapFrom->_Type)
                        {
                            swprintf (ValueName, 128 , L"%d", ConditionsDependencyArray [lIndex]->MapFromValue);
                            for (iIndex = 0; iIndex < DependencyForMapFrom->_ValueCount ; iIndex ++)
                            {
                                if (0 == StrCmp (ValueName, DependencyForMapFrom->_Value[iIndex]._ValueName))
                                {
                                    
                                    jIndex = FindEnglishString (DependencyForMapFrom->_Value[iIndex]._ValueDisplayName);
                                    String8 = StrDup16to8 (DependencyForMapFrom->_Value[iIndex]._ValueDisplayName [jIndex].String);
                                    cJSON_AddStringToObject (MapFromFields, "MapFromValue", String8);
                                    MemFreePointer (&String8);
                                    break;
                                }
                            }
                        }
                        else if (_CONTROL_TYPE_CHECKBOX == DependencyForMapFrom->_Type)
                        {
                            cJSON_AddBoolToObject (MapFromFields, "MapFromValue", (int)ConditionsDependencyArray [lIndex]->MapFromValue);
                        }
                        else if (_CONTROL_TYPE_NUMERIC == DependencyForMapFrom->_Type)
                        {
                            cJSON_AddNumberIntToObject (MapFromFields, "MapFromValue", ConditionsDependencyArray [lIndex]->MapFromValue);
                        }
                        else
                        {
                            cJSON_AddNumberIntToObject (MapFromFields, "MapFromValue", 0);  //BUGBUG TODO
                        }
                    }
                }
                /*MapFrom object ends*/
                
                cJSON_AddItemToObject (DependencyObject, "MapFrom", MapFromArray);
                
                /*MapFrom Ends*/
                
                String8 = StrDup16to8 (AttributeList->_AttributeName);
                cJSON_AddStringToObject (DependencyObject, "MapToAttribute", String8);
                MemFreePointer (&String8);
                
                cJSON_AddStringToObject (DependencyObject, "MapToProperty", MapPropertyStrings [AttributeList->Dependencies->MapToProperty]);
                cJSON_AddStringToObject (DependencyObject, "MapToValue", "true");
                //cJSON_AddItemToArray (gDependencyArray, DependenciesObject);
                DependencyObjectsStruct = AllocateZeroPool (sizeof (DEPENDENCY_OBJECTS));
                if (NULL != DependencyObjectsStruct)
                {
                    DependencyObjectsStruct->DependencyObject = DependenciesObject;
                }
                if (NULL == ConsDependencyObjectsStruct)
                {
                    ConsDependencyObjectsStruct = DependencyObjectsStruct;
                }
                else
                {
                    TempDependencyObjectsStruct = ConsDependencyObjectsStruct;
                    while (TempDependencyObjectsStruct->Next)
                    {
                        TempDependencyObjectsStruct = TempDependencyObjectsStruct->Next;
                    }
                    TempDependencyObjectsStruct->Next = DependencyObjectsStruct;
                }
               
                Status = EFI_SUCCESS;
            }
            else
            {
                //cJSON_Delete (TempDependencyArray);
                TempDependencyObjectsStruct = ConsDependencyObjectsStruct;
                while (TempDependencyObjectsStruct)
                {
                    DependencyObjectsStruct = TempDependencyObjectsStruct;
                    TempDependencyObjectsStruct = TempDependencyObjectsStruct->Next; 
                    cJSON_Delete (DependencyObjectsStruct->DependencyObject);
                    MemFreePointer (&DependencyObjectsStruct);
                }
                return EFI_INVALID_PARAMETER;
                //TODO BUGBUG
            }
        }
        TempDependencies = TempDependencies->Next;
    }
//<Wangbn001-20200106-Fixed memory leak issue +>
/*
    if (NULL != ConsDependencyObjectsStruct)
    {
        TempDependencyObjectsStruct = ConsDependencyObjectsStruct;
        while (TempDependencyObjectsStruct)
        {
            cJSON_AddItemToArray (gDependencyArray, TempDependencyObjectsStruct->DependencyObject );
            TempDependencyObjectsStruct = TempDependencyObjectsStruct->Next;
        }
//        MemFreePointer (&ConsDependencyObjectsStruct);
    }
*/
    while (ConsDependencyObjectsStruct)
    {
        TempDependencyObjectsStruct = ConsDependencyObjectsStruct;
        cJSON_AddItemToArray (gDependencyArray, ConsDependencyObjectsStruct->DependencyObject );
        ConsDependencyObjectsStruct = ConsDependencyObjectsStruct->Next;
        MemFreePointer (&TempDependencyObjectsStruct);
    }
//<Wangbn001-20200106-Fixed memory leak issue ->
    if (NULL != ConditionsDependencyArray){
        MemFreePointer ((void **)&ConditionsDependencyArray);
    }
    /*
    if (NULL != ConditionsDependencyArray)
    {
        if (NULL != ConditionsDependencyArray [0])
        {
            MemFreePointer ((void **)ConditionsDependencyArray [0]);
            MemFreePointer ((void **)&(ConditionsDependencyArray [0]));
        }
        if (NULL != ConditionsDependencyArray [1])
        {
            MemFreePointer ((void **)ConditionsDependencyArray [1]);
            MemFreePointer ((void **)&(ConditionsDependencyArray [1]));
        }
        MemFreePointer ((void **)&ConditionsDependencyArray);
    }
    */
    if ((_CONTROL_TYPE_SUBMENU == AttributeList->_Type) || (_CONTROL_TYPE_TEXT == AttributeList->_Type) || (_CONTROL_TYPE_MEMO == AttributeList->_Type))
    {
        return EFI_NOT_READY;
    }
    return EFI_SUCCESS;
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
