//***********************************************************************
//*                                                                     *
//*   Copyright (c) 1985-2019, American Megatrends International LLC.   *
//*                                                                     *
//*      All rights reserved. Subject to AMI licensing agreement.       *
//*                                                                     *
//***********************************************************************

/** @file FixedBootOrderTSE.c

     TSE Related Functions
*/

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include <Efi.h>
#include <Token.h>
#include <AmiDxeLib.h>
#include "variable.h"
#if CSM_SUPPORT
#include <Protocol/LegacyBios.h>
#endif
#include <Protocol/HiiString.h>
#include <Protocol/HiiDatabase.h>
#include <Setup.h>
#include <setupdata.h>
#include "FixedBootOrder.h"
#include <AutoId.h>
#include <UefiHii.h>
#include <Library/DebugLib.h>

#include <boot.h>

#define VARIABLE_ATTRIBUTES \
    (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS)

#pragma pack(1)
typedef struct
{
    UINT32  Type;
    UINT16  Length;
    UINT16  Device[1];
}
LEGACY_DEVICE_ORDER;


#ifndef TSE_MAX_DRIVE_NAME
#define TSE_MAX_DRIVE_NAME 32
#endif
// To fix c compiler typedef is tagged but not implement.
typedef struct _BBS_ORDER_LIST
{
    UINT16  Index;
    CHAR16  Name[TSE_MAX_DRIVE_NAME];
}
BBS_ORDER_LIST;

#pragma pack()

//---------------------------------------------------------------------------
// External functions
//---------------------------------------------------------------------------
BOOLEAN BBSValidDevicePath(EFI_DEVICE_PATH_PROTOCOL *DevicePath);
CHAR16* Wcscpy(CHAR16 *string1, CHAR16* string2);
UINTN Wcslen(CHAR16 *string);

extern UINTN gBootOptionCount;
extern NVRAM_VARIABLE *gVariableList;
extern BOOT_DATA *gBootData;
extern BOOLEAN gBrowserCallbackEnabled;
//---------------------------------------------------------------------------
// Global Variable declarations
//---------------------------------------------------------------------------
EFI_GUID DefaultLegacyDevOrderGuid = DEFAULT_LEGACY_DEV_ORDER_VARIABLE_GUID;
EFI_GUID FixedBootOrderGuid = FIXED_BOOT_ORDER_GUID;
BOOLEAN LoadedDefault = FALSE;
static BOOLEAN LoadedDefaultAndSaved = FALSE;
FBODevMap *pFBOLegacyDevMap = NULL;
FBODevMap *pFBOUefiDevMap = NULL;
FBODevMap *pFBODualDevMap = NULL;
FBOHiiMap *pFBOHiiMap = NULL;
UINT16 *pBootOptionTokenMap = NULL;       //(EIP123284+)
FBO_DEVICE_INFORM *pFBONewDevDescription = NULL ;
UINT8 gTseSkipFBOModule = 0 ;
EFI_HII_HANDLE HiiHandle;

EFI_GUID gFboGroupFormFormSetGuid = FBO_GROUP_FORM_FORM_SET_GUID;

EFI_STATUS SetBootVariable(UINT16 BootIndex, BOOLEAN EnableDisable);
/**
    Get Dev Map Data Count

    @param pFBODevMap - FboDevMap

    @retval UINT16 - Index of DevMap
**/
UINT16 GetDevMapDataCount(FBODevMap *pFBODevMap)
{
    UINT16 count = 0;

    if (!pFBODevMap) return 0;

    do
    {
        if (pFBODevMap[count].DevType == 0) break;
        count++;
    }
    while (1);

    return count;
}

/**
    Get Hii Device Map Data Count

**/
UINT16 GetHiiMapMapCount()
{
    UINT16 count = 0;

    if (!pFBOHiiMap) return 0;

    do
    {
        if (pFBOHiiMap[count].DevType == 0) break;
        count++;
    }
    while (1);

    return count;
}

UINT16 SearchDevMapByType(FBODevMap *pFBODevMap, UINT16 DevType)
{
    UINT16 i = 0;

    if (!pFBODevMap)
    {
        return 0xFFFF;
    }

    do
    {
        if (pFBODevMap[i].DevType == 0) break;
        if (pFBODevMap[i].DevType == DevType)
            return i;
        i++;
    }
    while (1);

    return 0xFFFF;
}

UINT16 SearchHiiMapByType(UINT16 DevType)
{
    UINT16 i = 0;

    do
    {
        if (pFBOHiiMap[i].DevType == 0) break;
        if (pFBOHiiMap[i].DevType == DevType)
            return i;
        i++;
    }
    while (1);

    return 0;
}

/**
    Set Tse Skip FBO Module Flag

**/
VOID
TseSetSkipFBOModuleFlag()
{
    EFI_STATUS Status;
    UINTN  Size = 0;
    UINT32 Attr;
    UINT8 *Flag = NULL ;
    Status = GetEfiVariable(L"CurrentSkipFboModule", &FixedBootOrderGuid, &Attr, &Size, &Flag);
    if (!EFI_ERROR(Status))
    {
        if (*Flag) gTseSkipFBOModule = 1 ;
        else gTseSkipFBOModule = 0 ;
        pBS->FreePool(Flag) ;
    }
    else
        gTseSkipFBOModule = 0 ;
}

/**
    This is eLinked after ProcessEnterSetupHook to set FBO_EnterSetup = TRUE
    if setup is entered

        
    @param VOID

    @retval 
        FBO_EnterSetup

**/
BOOLEAN FBO_EnterSetup = FALSE;
EFI_STATUS FixedBootOrderSetupEnter(IN void)
{
    TseSetSkipFBOModuleFlag() ;
    if (gTseSkipFBOModule) return EFI_SUCCESS;
    DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "FixedBootOrder ELINK Setup Enter......\n"));
    FBO_EnterSetup = TRUE;
    return EFI_SUCCESS;
}

/** 
    Record boot option number of group-disabled device.

    @param IsUefi     Record the UEFI or Legacy boot option number
    @param GroupUsed  Indicate the group is disabled or not
    @param BootOrder  Used to record the boot option number
    @param Index      Index for BootOrder to record
**/
VOID RecordGroupDisabledBootOrder(BOOLEAN IsUefi, BOOLEAN* GroupUsed, UINT16 *BootOrder, UINT16 *Index)
{
    if (GROUP_BOOT_OPTIONS_BY_TAG == 1) return;

    if (IsUefi) 
    {
        UINTN j;
        EFI_STATUS Status;

        UEFI_DEVICE_ORDER *DevOrder;
        UEFI_DEVICE_ORDER *UefiGroup = NULL;
        UINTN UefiGroupSize = 0;

        Status = GetEfiVariable(L"UefiDevOrder", &FixedBootOrderGuid, NULL, &UefiGroupSize, &UefiGroup);
        if (EFI_ERROR(Status)) return;

        for (DevOrder = UefiGroup
             ; (UINT8*)DevOrder < (UINT8*)UefiGroup + UefiGroupSize
             ; DevOrder = (UEFI_DEVICE_ORDER*)((UINT8*)DevOrder + DevOrder->Length + sizeof(DevOrder->Type)))
        {
            if (TRUE == GroupUsed[DevOrder->Type]) continue;

            for (j = 0; j < DEVORDER_COUNT(DevOrder); j++)
            {

                BootOrder[*Index] = (UINT16)(DevOrder->Device[j]);
                (*Index)++;
            }
        }

        pBS->FreePool((VOID*)UefiGroup);
    }
    else
    {
        UINTN j;
        UINTN k;
        EFI_STATUS Status;

        LEGACY_DEVICE_ORDER *DevOrder;
        LEGACY_DEVICE_ORDER *LegacyGroup = NULL;
        UINTN LegacyGroupSize = 0;

        Status = GetEfiVariable(L"FboLegacyDevOrder", &FixedBootOrderGuid, NULL, &LegacyGroupSize, &LegacyGroup);
        if(EFI_ERROR(Status)) return;

        for (DevOrder = LegacyGroup
             ; (UINT8*)DevOrder < (UINT8*)LegacyGroup + LegacyGroupSize
             ; DevOrder = NEXT_DEVICE(LEGACY_DEVICE_ORDER, DevOrder))
        {
            if (TRUE == GroupUsed[DevOrder->Type]) continue;

            for (j = 0; j < DEVORDER_COUNT(DevOrder); j++)
            {
                UINT16 BbsIdx = DevOrder->Device[j] & FBO_LEGACY_ORDER_MASK;
                for (k = 0; k < gBootOptionCount; k++)
                {
                    if (gBootData[k].OrderList[0].Index == BbsIdx)
                    {
                        BootOrder[*Index] = gBootData[k].Option ;
                        (*Index)++;
                        break ;
                    }
                }
            }
        }

        pBS->FreePool((VOID*)LegacyGroup);
    }
} 

/**
    Update BootOrder variable if setup is entered

    @param FBO_EnterSetup=1: setup is entered

    @retval VOID

**/
extern BOOLEAN gEnterSetup;
EFI_STATUS FixedBootOrderToBoot(IN void)
{
    EFI_STATUS  Status;
    EFI_GUID    gSetupGuid = SETUP_GUID;
    EFI_GUID    gEfiGlobalVariableGuid = EFI_GLOBAL_VARIABLE;
    SETUP_DATA  *pSetupData = NULL;
    UINT32      SetupAttr;
    UINTN       SetupSize = 0;
    UINT16      i, j, k = 0;
    UINT16      BootIndex = 0, DisabledIndex = 0;
    UINT16      *NewBootOrder = NULL;
    UINT16      *DisabledBootOrder;
    VOID        *UefiDevOrderBuffer = NULL, *LegacyDevOrderBuffer = NULL;
    UINTN       UefiDevSize = 0, LegacyDevSize = 0;
    UINTN       NewBootOrderSize = 0;
    BOOLEAN     GroupUsed[FBO_GROUP_TYPE_MAX_NUM] = {FALSE}; // For Adjust disabled group boot options
    EFI_FIXED_BOOT_ORDER_PROTOCOL *FixedBootOrderProtocol;
    
    if (gTseSkipFBOModule) return EFI_SUCCESS;
    Status = pBS->LocateProtocol (&FixedBootOrderGuid, NULL, (VOID**)&FixedBootOrderProtocol);      
    if (EFI_ERROR(Status)) return EFI_UNSUPPORTED;
    
    DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "FixedBootOrderToBoot............\n"));
    DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "FBO_EnterSetup=%d\n", FBO_EnterSetup));
    //-------------------------------------------------------------------
    // if GROUP_BOOT_OPTIONS_BY_TAG is disabled, handle UefiDevOrder/FboLegacyDevOrder on every time.
    if (!FBO_EnterSetup && GROUP_BOOT_OPTIONS_BY_TAG) return EFI_SUCCESS;


    Status = GetEfiVariable(L"UefiDevOrder", &FixedBootOrderGuid, NULL, &UefiDevSize, &UefiDevOrderBuffer);
    if (!EFI_ERROR(Status))
        NewBootOrderSize = UefiDevSize;  // number of boot orders = size (which is definitely beyond the real size)

    Status = GetEfiVariable(L"FboLegacyDevOrder", &FixedBootOrderGuid, NULL, &LegacyDevSize, &LegacyDevOrderBuffer);
    if (!EFI_ERROR(Status)) 
        NewBootOrderSize += LegacyDevSize ;
    
    // max size of BootOrder = UEFI + legacy boot orders
    NewBootOrderSize += gBootOptionCount;
    NewBootOrder = MallocZ(NewBootOrderSize * 2);
    DisabledBootOrder = (UINT16*)((UINT8*)NewBootOrder + NewBootOrderSize);

    Status = GetEfiVariable(L"Setup", &gSetupGuid, &SetupAttr, &SetupSize, &pSetupData);

    if (pSetupData == NULL || EFI_ERROR(Status)) return Status;
#if FBO_DUAL_MODE
    if (pSetupData->BootMode == 2)   //dual MODE?
    {
        FBODevMap *DualDevMap = FixedBootOrderProtocol->GetDualDevMap();
        UINT16 DevCount = GetDevMapDataCount(DualDevMap);

        for (i = 0; i < DevCount; i++)
        {
            UINT16 DevIndex;
//            UEFI_DEVICE_ORDER *DevOrder;

            DevIndex = pSetupData->DualPriorities[i];
            DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "DualDevMap[%d].DevType=%x\n", DevIndex, DualDevMap[DevIndex].DevType));

            if (DualDevMap[DevIndex].DevType == 0) continue;         //Is Disable?

            if (DualDevMap[DevIndex].BBSType == 0 && UefiDevOrderBuffer) //Uefi boot option?
            {
                UEFI_DEVICE_ORDER *DevOrder;
                for (DevOrder = UefiDevOrderBuffer
                     ; (UINT8*)DevOrder < (UINT8*)UefiDevOrderBuffer + UefiDevSize
                     ; DevOrder = NEXT_DEVICE(UEFI_DEVICE_ORDER, DevOrder))
                {
                    if (DualDevMap[DevIndex].DevType == DevOrder->Type)
                    {
                        GroupUsed[DevOrder->Type] = TRUE;
                        for (j = 0; j < DEVORDER_COUNT(DevOrder); j++)
                        {
                            if (0 == (DevOrder->Device[j] & FBO_UEFI_DISABLED_MASK))
                            {
                                DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "Uefi BootOrder Number = %04x\n", DevOrder->Device[j]));
                                NewBootOrder[BootIndex] = DevOrder->Device[j] & FBO_UEFI_ORDER_MASK; // [31:24] is disabled flag
                                BootIndex++;
                            }
                            else
                            {
                                DisabledBootOrder[DisabledIndex] = DevOrder->Device[j] & FBO_UEFI_ORDER_MASK; // [31:24] is disabled flag
                                DisabledIndex++;
                            }
                        }
                        break;
                    }
                }
            }
            else if (LegacyDevOrderBuffer)
            {
                LEGACY_DEVICE_ORDER *DevOrder = NULL ;
#if GROUP_BOOT_OPTIONS_BY_TAG                
                for (j = 0; j < gBootOptionCount; j++)
                {
                    LEGACY_DEVICE_ORDER *DevOrder;

                    if (!BBSValidDevicePath(gBootData[j].DevicePath)) continue;

                    DevOrder = (LEGACY_DEVICE_ORDER*)((UINT8*)LegacyDevOrderBuffer + gBootData[j].LegacyEntryOffset);
                    if (DualDevMap[DevIndex].DevType == DevOrder->Type)
                    {
                        NewBootOrder[BootIndex] = gBootData[j].Option;
                        BootIndex++;
                    }
                }
#else
                for (DevOrder = LegacyDevOrderBuffer
                     ; (UINT8*)DevOrder < (UINT8*)LegacyDevOrderBuffer + LegacyDevSize
                     ; DevOrder = NEXT_DEVICE(LEGACY_DEVICE_ORDER, DevOrder))
                {
                    if (DualDevMap[DevIndex].DevType != DevOrder->Type)
                        continue ;
                    GroupUsed[DevOrder->Type] = TRUE;
                    for (j = 0; j < DEVORDER_COUNT(DevOrder); j++)
                    {
                        UINT16 BbsIdx = DevOrder->Device[j] & FBO_LEGACY_ORDER_MASK;
                        for (k = 0; k < gBootOptionCount; k++)
                        {
                            if (gBootData[k].OrderList[0].Index == BbsIdx)
                            {

                                if (0 == (DevOrder->Device[j] & FBO_LEGACY_DISABLED_MASK))
                                {
                                    NewBootOrder[BootIndex] = gBootData[k].Option ;
                                    BootIndex++;
                                }
                                else
                                {
                                    DisabledBootOrder[DisabledIndex] = gBootData[k].Option ;
                                    DisabledIndex++;
                                }
                                break ;
                            }
                        }
                    }
                    break;
                }
#endif
            }
        }
    }
    else
#endif
        if (pSetupData->BootMode == 1)   //uefi MODE?
        {
            if (UefiDevOrderBuffer != NULL)
            {
                FBODevMap *UefiDevMap = FixedBootOrderProtocol->GetUefiDevMap();
                UINT16 DevCount = GetDevMapDataCount(UefiDevMap);

                for (i = 0; i < DevCount; i++)
                {
                    UINT16 DevIndex;
                    UEFI_DEVICE_ORDER *DevOrder;

                    DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "pSetupData->UefiPriorities[%d]=%d\n", i, pSetupData->UefiPriorities[i]));
                    DevIndex = pSetupData->UefiPriorities[i];
                    if (UefiDevMap[DevIndex].DevType == 0) continue;         //Is Disable?

                    for (DevOrder = UefiDevOrderBuffer
                         ; (UINT8*)DevOrder < (UINT8*)UefiDevOrderBuffer + UefiDevSize
                         ; DevOrder = (UEFI_DEVICE_ORDER*)((UINT8*)DevOrder + DevOrder->Length + sizeof(DevOrder->Type)))
                    {
                        if (UefiDevMap[DevIndex].DevType == DevOrder->Type)
                        {
                            GroupUsed[DevOrder->Type] = TRUE;
                            for (j = 0; j < DEVORDER_COUNT(DevOrder); j++)
                            {
                                if (0 == (DevOrder->Device[j] & FBO_UEFI_DISABLED_MASK))
                                {
                                    NewBootOrder[BootIndex] = DevOrder->Device[j] & FBO_UEFI_ORDER_MASK; // [31:24] is disabled flag
                                    BootIndex++;
                                }
                                else
                                {
                                    DisabledBootOrder[DisabledIndex] = DevOrder->Device[j] & FBO_UEFI_ORDER_MASK; // [31:24] is disabled flag
                                    DisabledIndex++;
                                }
                            }
                            break;
                        }
                    }
                }
            }
        }
        else if (pSetupData->BootMode == 0)   //legacy MODE?
        {
            FBODevMap *LegacyDevMap = FixedBootOrderProtocol->GetLegacyDevMap();
            UINT16 LegacyDevCount = GetDevMapDataCount(LegacyDevMap);
#if GROUP_BOOT_OPTIONS_BY_TAG               
            for (i = 0; i < LegacyDevCount; i++)
            {
                UINT16 DevIndex = pSetupData->LegacyPriorities[i];
                if (LegacyDevMap[DevIndex].DevType == 0) continue;       //Is Disable?

                for (j = 0; j < gBootOptionCount; j++)
                {
                    LEGACY_DEVICE_ORDER *DevOrder;

                    if (!BBSValidDevicePath(gBootData[j].DevicePath)) continue;

                    DevOrder = (LEGACY_DEVICE_ORDER*)((UINT8*)LegacyDevOrderBuffer + gBootData[j].LegacyEntryOffset);
                    if (LegacyDevMap[DevIndex].DevType == DevOrder->Type)
                    {
                        NewBootOrder[BootIndex] = gBootData[j].Option;
                        BootIndex++;
                    }
                }
            }
#else
            if (LegacyDevOrderBuffer != NULL)
            {
                for (i = 0; i < LegacyDevCount; i++)
                {
                    UINT16 DevIndex;
                    LEGACY_DEVICE_ORDER *DevOrder;

                    DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "pSetupData->LegacyPriorities[%d]=%d\n", i, pSetupData->LegacyPriorities[i]));
                    DevIndex = pSetupData->LegacyPriorities[i];
                    if (LegacyDevMap[DevIndex].DevType == 0) continue;         //Is Disable?
                
                    for (DevOrder = LegacyDevOrderBuffer
                         ; (UINT8*)DevOrder < (UINT8*)LegacyDevOrderBuffer + LegacyDevSize
                         ; DevOrder = NEXT_DEVICE(LEGACY_DEVICE_ORDER, DevOrder))
                    {
                        if (LegacyDevMap[DevIndex].DevType != DevOrder->Type)
                            continue ;
                        
                        GroupUsed[DevOrder->Type] = TRUE;
                        for (j = 0; j < DEVORDER_COUNT(DevOrder); j++)
                        {
                            UINT16 BbsIdx = DevOrder->Device[j] & FBO_LEGACY_ORDER_MASK;
                            for (k = 0; k < gBootOptionCount; k++)
                            {
                                if (gBootData[k].OrderList[0].Index == BbsIdx)
                                {
                                    if (0 == (DevOrder->Device[j] & FBO_LEGACY_DISABLED_MASK))
                                    {
                                        NewBootOrder[BootIndex] = gBootData[k].Option ;
                                        BootIndex++;
                                    }
                                    else
                                    {
                                        DisabledBootOrder[DisabledIndex] = gBootData[k].Option ;
                                        DisabledIndex++;

                                    }
                                    break ;
                                }
                            }
                        }
                        break ;
                    }
                }
            }
#endif
        }

    //
    //
    //  With GROUP_BOOT_OPTIONS_BY_TAG disabled, legacy devices fails to synchronize the attribute 
    //  on next boot since BbsIndx may change and FboLegacyDevOrder/OriFboLegacyDevOrder variable 
    //  can't match the right boot option at all, so it has to arrange the devices without active
    //  attribute to BootOrder according to the sequence of FboLegacyDevOrder variable, then 
    //  it will have a benchmark to synchronize, however, this needs the support of TSE_PRESERVE_DISABLED_BBS_DEVICE_ORDER
    //  enabled, please refer the following note for original TSE behavior on reordering BootOrder.
    //  
    //
    //  Note: How TSE arranges the devices without active attribute to BootOrder?
    //
    //  TSE originally reorders the devices to BootOrder according to the sequence of gBootData
    //  which comes from the algorithm of searching every variable in nvram with patten BootXXXX,
    //  but this behavior can be changed by token TSE_PRESERVE_DISABLED_BBS_DEVICE_ORDER enabled,
    //  then devices won't be reordered.
    //  
    //
    if(GROUP_BOOT_OPTIONS_BY_TAG == 0)
    {
        RecordGroupDisabledBootOrder(FALSE, GroupUsed, DisabledBootOrder, &DisabledIndex);
        RecordGroupDisabledBootOrder(TRUE, GroupUsed, DisabledBootOrder, &DisabledIndex);

        //
        //  Set Enable attribute for Eanbled device.
        //
        for(i = 0; i < BootIndex; i++)
        {
            BOOT_DATA *BootData = BootGetBootData(NewBootOrder[i]);
            SetBootVariable(BootData->Option, TRUE);
        }

        //
        //  Set disable attribute for disabled device,
        //  and sync the disabled device to NewBootOrder.
        //
        for (i = 0; i < DisabledIndex; i++)
        {
            BOOT_DATA *BootData = BootGetBootData(DisabledBootOrder[i]);
            SetBootVariable(BootData->Option, FALSE);

            NewBootOrder[BootIndex++] = DisabledBootOrder[i];
        }

        //
        //  Add the remaining(unknown) boot option to boot order,
        //  then disable the attribute.
        //
        for (i = 0; i < gBootOptionCount; i++)
        {
            for (j = 0; j < BootIndex; j++)
            {
                if (NewBootOrder[j] == gBootData[i].Option)
                    break;
            }
            if (j == BootIndex)
            {
                NewBootOrder[BootIndex++] = gBootData[i].Option;
                SetBootVariable(gBootData[i].Option, FALSE);
            }
        }
    }
    else
    {
        // Above loop for getting NewBootOrder will be skip if the group is disabled:
        // pFBOXXXDevMap[DevIndex].DevType==0 (disable = last DevIndex in pFBOXXXDevMap which is 0)

    // Here we have boot options which are not added because the group is disabled
        
    for (i = 0; i < gBootOptionCount; i++)
    {
        // boot options with group enabled
        for (j = 0; j < BootIndex; j++)
        {
            if (NewBootOrder[j] == gBootData[i].Option)
            {
                if (BBSValidDevicePath(gBootData[i].DevicePath))
                {
                    // Legacy Boot Option
                    // Enable Legacy group option, attr |= ACTIVE
                    SetBootVariable(gBootData[i].Option, TRUE);
                }
                else
                {  
                    // UEFI Boot Option
                    // Due to EFI option is not group, so we need double check with device group sub menu.
                    UEFI_DEVICE_ORDER *DevOrder;
                    for (DevOrder = UefiDevOrderBuffer
                         ; (UINT8*)DevOrder < (UINT8*)UefiDevOrderBuffer + UefiDevSize
                         ; DevOrder = (UEFI_DEVICE_ORDER*)((UINT8*)DevOrder + DevOrder->Length + sizeof(DevOrder->Type)))
                    {
                        UINTN index ;
                        for (index = 0 ; index < DEVORDER_COUNT(DevOrder) ; index++)
                        {      
                            if ((UINT16)DevOrder->Device[index] == gBootData[i].Option)
                            {
                                // Check device is enable or not in sub menu
                                if (DevOrder->Device[index] & FBO_UEFI_DISABLED_MASK)        
                                    SetBootVariable(gBootData[i].Option, FALSE);
                                else
                                    SetBootVariable(gBootData[i].Option, TRUE);
                                break ;
                            }
                        }
                    }
                }
                break;
            }
        }

        // boot options with group disabled
        // need to disable the boot#### variable directly
        if (j == BootIndex)
        {
            NewBootOrder[BootIndex] = gBootData[i].Option;
            BootIndex++;
            SetBootVariable(gBootData[i].Option, FALSE);
        }
    }

#if FIXED_BOOT_INCLUDE_DISABLED_OPTION
    {
        // Put disable boot variable to last.
        UINT16 *TempBootOrder = MallocZ(NewBootOrderSize);
        UINT16 TempIndex = 0 ;
        
        
        for (i = 0; i < gBootOptionCount; i++)
        {
            BOOT_DATA *pBootData = BootGetBootData(NewBootOrder[i]) ;
            if (pBootData->Active & LOAD_OPTION_ACTIVE)
            {
                TempBootOrder[TempIndex] = NewBootOrder[i] ;
                NewBootOrder[i] = 0xffff ; //added flag
                TempIndex++ ;
            }
        }
        for (i = 0; i < gBootOptionCount; i++)
        {
            if (NewBootOrder[i] != 0xffff)
            {
                TempBootOrder[TempIndex] = NewBootOrder[i] ;
                TempIndex++ ;
            }
        }
        
        pBS->FreePool(NewBootOrder);
        NewBootOrder = TempBootOrder ;
    }
#endif
    }


    pRS->SetVariable(L"BootOrder",
                     &gEfiGlobalVariableGuid,
                     VARIABLE_ATTRIBUTES,
                     BootIndex * sizeof(UINT16),
                     NewBootOrder);

    pRS->SetVariable(L"OldBootOrder",
                     &FixedBootOrderGuid,
                     VARIABLE_ATTRIBUTES,
                     BootIndex * sizeof(UINT16),
                     NewBootOrder);

    if (pSetupData)
        pBS->FreePool(pSetupData);

    if (UefiDevOrderBuffer)
        pBS->FreePool(UefiDevOrderBuffer);
    
    if (LegacyDevOrderBuffer)
        pBS->FreePool(LegacyDevOrderBuffer);
    
    if (NewBootOrder)
        pBS->FreePool(NewBootOrder);


    return EFI_SUCCESS;
}

/**
    Set Boot#### variable Enable/Disable.

    @param UINT16 BootIndex
        BOOLEAN EnableDisable

    @retval VOID

**/
EFI_STATUS SetBootVariable(UINT16 BootIndex, BOOLEAN EnableDisable)
{
    EFI_GUID EfiVariableGuid = EFI_GLOBAL_VARIABLE;
    EFI_STATUS Status;
    UINTN OptionSize = 0;

    CHAR16 BootVarName[15];
    UINT8   Buffer[0x200];
    EFI_LOAD_OPTION *BootOption = NULL;     //buffer for BootXXX variables

    Swprintf(BootVarName, gBootName, BootIndex);

    BootOption = (EFI_LOAD_OPTION*)Buffer;

    OptionSize = sizeof(Buffer);
    Status = pRS->GetVariable(BootVarName, &EfiVariableGuid, NULL, &OptionSize, BootOption);


    if (!EFI_ERROR(Status))
    {
        BOOT_DATA *pBootData = BootGetBootData(BootIndex);
        if (EnableDisable)
        {
            BootOption->Attributes |= LOAD_OPTION_ACTIVE;
            if (pBootData) pBootData->Active |= LOAD_OPTION_ACTIVE;
        }
        else
        {
            BootOption->Attributes &= ~(LOAD_OPTION_ACTIVE);
            if (pBootData) pBootData->Active &= ~(LOAD_OPTION_ACTIVE);
        }

        Status = pRS->SetVariable(BootVarName,
                                  &EfiVariableGuid,
                                  VARIABLE_ATTRIBUTES,
                                  OptionSize,
                                  BootOption);

        if (EFI_ERROR(Status))
            DEBUG((FBO_TRACE_LEVEL | DEBUG_ERROR, "[FixedBootOrderTSE.c]::SetBootVariable(%d) Boot%04x=(%r)\n", EnableDisable, BootIndex, Status));
    }

    return Status;

}
/**
    Adjust DefaultLegacyDevOrder value. 

    @param **DevOrderBuffer
    @param *Size
    
    @retval NONE

**/
VOID
AdjustLegacyDevOrder(
    VOID**  DevOrderBuffer,
    UINTN   *Size)
{
#if CSM_SUPPORT
    LEGACY_DEVICE_ORDER     *DevOrder = *DevOrderBuffer;
    UINT16                  BBSIndex = 0;
    UINTN                   Size1 = 0, i = 0;
    VOID                    *DevOrderBuffer1 = NULL;
    LEGACY_DEVICE_ORDER     *DevOrder1 = NULL;
    EFI_STATUS              Status = EFI_SUCCESS;
    EFI_GUID                FboBdsGuid = FIXED_BOOT_ORDER_BDS_GUID ;
    EFI_FIXED_BOOT_ORDER_BDS_PROTOCOL *FboBdsProtocol = NULL ;
    
#if GROUP_BOOT_OPTIONS_BY_TAG
    Status = GetEfiVariable(L"FboLegacyDevOrder", &FixedBootOrderGuid, NULL, &Size1, &DevOrderBuffer1);
    if(EFI_ERROR(Status)) return;
    for (
            ; (UINT8*)DevOrder < (UINT8*)*DevOrderBuffer + *Size
            ; DevOrder = (LEGACY_DEVICE_ORDER*)((UINT8*)DevOrder + DevOrder->Length + sizeof(DevOrder->Type)))
    {
        // Use the first one to check the device type
        BBSIndex = DevOrder->Device[0] & FBO_LEGACY_ORDER_MASK;
        for (DevOrder1 = DevOrderBuffer1
             ; (UINT8*)DevOrder1 < (UINT8*)DevOrderBuffer1 + Size1
             ; DevOrder1 = (LEGACY_DEVICE_ORDER*)((UINT8*)DevOrder1 + DevOrder1->Length + sizeof(DevOrder1->Type)))
        {
            for (i = 0; i < DEVORDER_COUNT(DevOrder1); i++)
            {
                if(BBSIndex == (DevOrder1->Device[i] & FBO_LEGACY_ORDER_MASK))
                {
                    DevOrder->Type = DevOrder1->Type;
                    // break these 2 loop
                    DevOrder1 = (LEGACY_DEVICE_ORDER*)((UINT8*)DevOrderBuffer1 + Size1);
                    break;
                }
            }
        }
    }
    pBS->FreePool(DevOrderBuffer1);
#else
    Status = pBS->LocateProtocol(&FboBdsGuid, NULL, &FboBdsProtocol) ;
    if (EFI_ERROR(Status)) return;
    
    Status = GetEfiVariable(L"FboLegacyDevOrder", &FixedBootOrderGuid, NULL, &Size1, &DevOrderBuffer1);
    if(EFI_ERROR(Status)) return;
    
    for (DevOrder1 = DevOrderBuffer1
         ; (UINT8*)DevOrder1 < (UINT8*)DevOrderBuffer1 + Size1
         ; DevOrder1 = NEXT_DEVICE(LEGACY_DEVICE_ORDER, DevOrder1))
            
    {
        UINT16 Idx = 0 ;
        for (DevOrder = *DevOrderBuffer
             ; (UINT8*)DevOrder < (UINT8*)*DevOrderBuffer + *Size
             ; DevOrder = NEXT_DEVICE(LEGACY_DEVICE_ORDER, DevOrder))
        {
            UINT16 LegacyFboType = FboBdsProtocol->GetLegacyTag(DevOrder->Device[0]) ;
            if (LegacyFboType == INVALID_FBO_TAG) continue ;
            
            if (LegacyFboType == (UINT16)DevOrder1->Type)
            {
                DevOrder1->Device[Idx] = DevOrder->Device[0] ;
                Idx++ ;
            }
        } 
    }
    
    pBS->FreePool(*DevOrderBuffer);
    *DevOrderBuffer = DevOrderBuffer1 ;
    *Size = Size1 ;
#endif  //#if GROUP_BOOT_OPTIONS_BY_TAG
#endif  //#if CSM_SUPPORT
}

/**
    Legacy: Save FboLegacyDevOrder with disabled information (upper byte = 0xff)
    and rearrange the disabled indexes to the last of each group.
              UEFI:   Update Boot####.LOAD_OPTION_ACTIVE if disabled and rearrange
                      the disabled boot numbers in UefiDevorder to the last of each group.

    @param VOID

    @retval VOID

**/
EFI_STATUS FixedBootOrderSaveChange(IN void)
{
    EFI_STATUS Status;
    UINTN Size, i = 0;
    UINTN VariableSize;
    VOID  *DevOrderBuffer = NULL;
    EFI_GUID LegacyDevOrderGuid = LEGACY_DEV_ORDER_GUID;
    EFI_GUID gSetupGuid = SETUP_GUID;
    FIXED_BOOT_SETUP FixedBootSetupData;
    LEGACY_DEVICE_ORDER *LegacyGroup;
    LEGACY_DEVICE_ORDER *FboLegacyDevOrder;
    UINT32              Attr, LegacyDevAttr;
    UEFI_DEVICE_ORDER   *UefiGroup;
    UEFI_DEVICE_ORDER   *UefiDevBuffer;
    EFI_FIXED_BOOT_ORDER_PROTOCOL *FixedBootOrderProtocol;

    if (gTseSkipFBOModule) return EFI_SUCCESS;
    Status = pBS->LocateProtocol (&FixedBootOrderGuid, NULL, (VOID**)&FixedBootOrderProtocol);      
    if (EFI_ERROR(Status)) return EFI_UNSUPPORTED;

    VariableSize = sizeof(FIXED_BOOT_SETUP);

    Status = pRS->GetVariable(L"FixedBoot",
                              &gFboGroupFormFormSetGuid,
                              NULL,
                              &VariableSize,
                              &FixedBootSetupData);

    if (EFI_ERROR(Status))
    {
        DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[FixedBoot SaveChange] GetVariable(FixedBoot):%r\n", Status));
        return Status;
    }

    //-------------------------------------------------------------------------------------------------------
    // Update Variable "FboLegacyDevOrder"
    //-------------------------------------------------------------------------------------------------------
    Size = 0;
    // Get LegacyDevOrder Attrib
    Status = GetEfiVariable(L"LegacyDevOrder", &LegacyDevOrderGuid, &LegacyDevAttr, &Size, &DevOrderBuffer);
    
    // Set Size to 0 for GetEfiVariable, DevOrderBuffer will also be clear in GetEfiVariable
    Size = 0;
    if (LoadedDefault)
    {
        Status = GetEfiVariable(L"DefaultLegacyDevOrder", &DefaultLegacyDevOrderGuid, &Attr, &Size, &DevOrderBuffer);
        DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[FixedBootOrderTSE.c] GetVariable(DefaultLegacyDevOrder):%r\n", Status));
        AdjustLegacyDevOrder(&DevOrderBuffer, &Size);
        //
        // To indicate the Setup had loaded default and save the changes.
        //
        LoadedDefaultAndSaved = TRUE;
    }
    else
    {
        Status = GetEfiVariable(L"OriFboLegacyDevOrder", &FixedBootOrderGuid, &Attr, &Size, &DevOrderBuffer);
        DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[FixedBootOrderTSE.c] GetVariable(FboLegacyDevOrder):%r\n", Status));
    }


    FboLegacyDevOrder = LegacyGroup = MallocZ(Size);

    if (!EFI_ERROR(Status) && Size >= EFI_FIELD_OFFSET(LEGACY_DEVICE_ORDER,Device))
    {
        LEGACY_DEVICE_ORDER *DevOrder;
        UINT16 *OrderData;
        UINT8  *NVramOrderIndex;
        UINT16 Index;
        UINT16 DevDataIndex;
        FBODevMap *LegacyDevMap;
        
        LegacyDevMap = FixedBootOrderProtocol->GetLegacyDevMap();
        for (DevOrder = DevOrderBuffer
                        ; (UINT8*)DevOrder < (UINT8*)DevOrderBuffer + Size
                ; DevOrder = (LEGACY_DEVICE_ORDER*)((UINT8*)DevOrder + DevOrder->Length + sizeof(DevOrder->Type)))
        {
            UINT16 *NewOrderData;
            DevDataIndex = SearchDevMapByType(LegacyDevMap, DevOrder->Type);

            pBS->CopyMem(LegacyGroup,
                         DevOrder,
                         DevOrder->Length + sizeof(DevOrder->Type));

            //
            //  Put this after CopyMem for keeping unknown groups.
            //
            if(DevDataIndex == 0xFFFF)
                continue;

            NewOrderData = (UINT16*) & LegacyGroup->Device[0];
            OrderData = (UINT16*) & DevOrder->Device[0];
            NVramOrderIndex = &FixedBootSetupData.LegacyDevice[DevDataIndex * FIXED_BOOT_ORDER_SUBMENU_MAX_NUM];

            DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[FixedBootOrderTSE.c] DeviceType:%x (", DevOrder->Type));
            for (i = 0; i < DEVORDER_COUNT(DevOrder); i++)
            {
                DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "%x ", OrderData[i]));
            }
            DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, ")\n"));

            DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[FixedBootOrderTSE.c] FixedBootSetupData DeviceType:%x (", DevOrder->Type));

            DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "Enable: "));
            // Set enabled BBS indexes to this group first
            for (i = 0; i < DEVORDER_COUNT(DevOrder); i++)
            {
                if (i >= FIXED_BOOT_ORDER_SUBMENU_MAX_NUM) break;

                Index = NVramOrderIndex[i];
                if (Index >= FIXED_BOOT_ORDER_SUBMENU_MAX_NUM)   // disabled?
                    break;

                DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "%x ", Index));
                NewOrderData[i] = OrderData[Index] & FBO_LEGACY_ORDER_MASK;  // clear high byte for enabled boot order
                DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "OrderData[Index]:%X", OrderData[Index])) ;
                OrderData[Index] = 0xffff;  // mark as processed
            }

#if FIXED_BOOT_INCLUDE_DISABLED_OPTION
            // The rest of BBS indexes are disabled
            // Disabled BBS indexes follow the enabled ones in the group
            if (i < DEVORDER_COUNT(DevOrder))  // any remaining indexes?
            {
                UINTN j;
                DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "Disable: "));
                for (j = 0; j < DEVORDER_COUNT(DevOrder); j++)
                {
                    if (OrderData[j] == 0xffff) // processed above already?
                        continue;

                    DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "%x ", OrderData[j]));
                    NewOrderData[i] = OrderData[j] | FBO_LEGACY_DISABLED_MASK; // high byte = 0xff = disable for FboLegacyDevOrder.Device[]
                    i++;
                }
            }
#endif
            DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "\n"));

            LegacyGroup = (LEGACY_DEVICE_ORDER*)((UINT8*)LegacyGroup
                                                 + LegacyGroup->Length
                                                 + sizeof(LegacyGroup->Type));
        }


        Status = pRS->SetVariable(L"FboLegacyDevOrder", &FixedBootOrderGuid, VARIABLE_ATTRIBUTES, Size, FboLegacyDevOrder);
        // Priorities changed !! Sync with LegacyDevOrder
        {
            LEGACY_DEVICE_ORDER*            DevOrder;
            EFI_LEGACY_BIOS_PROTOCOL        *LegacyBios;
            UINT16                          HddCount = 0;
            HDD_INFO                        *LocalHddInfo = NULL;
            UINT16                          BbsCount = 0;
            BBS_TABLE                       *BbsTable;
            EFI_GUID                        gEfiLegacyBiosProtocolGuid = EFI_LEGACY_BIOS_PROTOCOL_GUID;
            UINT16*                         OrderData;
            UINTN                           Offset = 0;
            LEGACY_DEVICE_ORDER*            LegacyDevOrder = NULL ;
            UINTN                           LegacyDevOrderSize = 0 ;
            UINTN                           DevCount = 0, Idx = 0 ;
            
            Status = pBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, &LegacyBios);      
            if (!EFI_ERROR (Status)) {      
                Status = LegacyBios->GetBbsInfo (
                                      LegacyBios,
                                      &HddCount, 
                                      &LocalHddInfo, 
                                      &BbsCount,
                                      &BbsTable
                                      );
            }

#if GROUP_BOOT_OPTIONS_BY_TAG
            for (DevOrder = FboLegacyDevOrder
                 ; (UINT8*)DevOrder < (UINT8*)FboLegacyDevOrder + Size
                 ; DevOrder = NEXT_DEVICE(LEGACY_DEVICE_ORDER, DevOrder))
            {
                OrderData = (UINT16*) & DevOrder->Device[0];
                DevOrder->Type = BbsTable[(UINT8)OrderData[0]].DeviceType;
                
                // Due to TSE SaveBBSOrder function will reassign gBootData->LegacyEntryOffset
                // We need calculate offset again.
                for (i = 0; i < gBootOptionCount; i++)
                {
                    UINTN j;
                    BBS_ORDER_LIST *OrderList = gBootData[i].OrderList ;
                    
                    for (j =0; j < gBootData[i].LegacyDevCount ; j++)
                        if ((OrderList[j].Index & FBO_LEGACY_ORDER_MASK) == DevOrder->Device[0])
                            break ;
                    
                    if (j != gBootData[i].LegacyDevCount)
                    {
                        // Matched, update gBootData offset.
                        gBootData[i].LegacyEntryOffset = Offset ;
                        break ;
                    }
                }
                Offset += DevOrder->Length + sizeof(DevOrder->Type) ;
            }
            // Need to sync with LegacyDevOrder for TSE to boot (in case it continues to boot without reset)
            Status = pRS->SetVariable(L"LegacyDevOrder", &LegacyDevOrderGuid, 
                    LegacyDevAttr, Size, FboLegacyDevOrder);
#else
            // calculate total count
            for (DevOrder = FboLegacyDevOrder
                 ; (UINT8*)DevOrder < (UINT8*)FboLegacyDevOrder + Size
                 ; DevOrder = NEXT_DEVICE(LEGACY_DEVICE_ORDER, DevOrder))
                DevCount += DEVORDER_COUNT(DevOrder);
            
            LegacyDevOrder = MallocZ(DevCount*sizeof(LEGACY_DEVICE_ORDER)) ;
            
            for (DevOrder = FboLegacyDevOrder
                 ; (UINT8*)DevOrder < (UINT8*)FboLegacyDevOrder + Size
                 ; DevOrder = NEXT_DEVICE(LEGACY_DEVICE_ORDER, DevOrder))
            {
                for (i = 0 ; i < DEVORDER_COUNT(DevOrder) ; i++)
                {
                    UINT16 j ;
                    
                    OrderData = (UINT16*)&DevOrder->Device[i];
                    LegacyDevOrder[Idx].Type = BbsTable[(UINT8)OrderData[0]].DeviceType;
                    LegacyDevOrder[Idx].Length = sizeof(DevOrder->Length) + sizeof(DevOrder->Device);
                    LegacyDevOrder[Idx].Device[0] = DevOrder->Device[i] ;
                    
                    // Due to TSE SaveBBSOrder function will reassign gBootData->LegacyEntryOffset
                    // We need calculate offset again.
                    for (j = 0; j < gBootOptionCount; j++)
                    {
                        BBS_ORDER_LIST *OrderList = gBootData[j].OrderList ;
                        if ((OrderList[0].Index & FBO_LEGACY_ORDER_MASK) == DevOrder->Device[i])
                            gBootData[j].LegacyEntryOffset = Idx * sizeof(LEGACY_DEVICE_ORDER) ;
                    }
                    Idx++ ;
                }
            }
            Status = pRS->SetVariable(L"LegacyDevOrder", &LegacyDevOrderGuid, 
                    LegacyDevAttr, DevCount*sizeof(LEGACY_DEVICE_ORDER), LegacyDevOrder);
            pBS->FreePool(LegacyDevOrder) ;
#endif
        }

#ifdef EFI_DEBUG
        // DEBUG: list the saved FboLegacyDevOrder
        {
            UINT16 j;

            DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[FixedBootOrderSaveChange] Saved LegacyDevOrder=%r ", Status));
            for (j = 0; j < Size; j++)
                DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "%02x ", *((UINT8*)FboLegacyDevOrder + j)));
            DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "\n"));
        }
#endif

        pBS->FreePool(DevOrderBuffer);

    }

    if (FboLegacyDevOrder)
        pBS->FreePool(FboLegacyDevOrder);

    //-------------------------------------------------------------------------------------------------------
    // Update Variable "UefiDevOrder"
    //-------------------------------------------------------------------------------------------------------
    Size = 0;
    DevOrderBuffer = NULL;
    if (LoadedDefault)
    {
        Status = GetEfiVariable(L"DefaultUefiDevOrder", &FixedBootOrderGuid, &Attr, &Size, &DevOrderBuffer);
        DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[FixedBootOrderTSE.c] GetVariable(DefaultUefiDevOrder):%r\n", Status));
    }
    else
    {
        Status = GetEfiVariable(L"OriUefiDevOrder", &FixedBootOrderGuid, &Attr, &Size, &DevOrderBuffer);
        DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[FixedBootOrderTSE.c]  GetVariable(UefiDevOrder):%r\n", Status));
    }

    UefiDevBuffer = UefiGroup = MallocZ(Size);

    if (!EFI_ERROR(Status) && Size > sizeof(UINT32) + sizeof(UINT16))  // > size of UEFI_DEVICE_ORDER.Type+Length: at least one order
    {
        UEFI_DEVICE_ORDER *DevOrder;
        UINT32 *OrderData;
        UINT8 *NVramOrderIndex;
        UINT16 Index;
        UINT16 DevDataIndex;
        FBODevMap *UefiDevMap;

        UefiDevMap = FixedBootOrderProtocol->GetUefiDevMap();
        for (DevOrder = DevOrderBuffer
                        ; (UINT8*)DevOrder < (UINT8*)DevOrderBuffer + Size
                ; DevOrder = (UEFI_DEVICE_ORDER*)((UINT8*)DevOrder + DevOrder->Length + sizeof(DevOrder->Type)))
        {
            UINT32 *NewOrderData;

            DevDataIndex = SearchDevMapByType(UefiDevMap, DevOrder->Type);

            pBS->CopyMem(UefiGroup,
                         DevOrder,
                         DevOrder->Length + sizeof(DevOrder->Type));

            //
            //  Put this after CopyMem for keeping unknown groups.
            //
            if(DevDataIndex == 0xFFFF)
                continue;

            NewOrderData = (UINT32*) & UefiGroup->Device[0];
            OrderData = (UINT32*) & DevOrder->Device[0];
            NVramOrderIndex = &FixedBootSetupData.UefiDevice[DevDataIndex * FIXED_BOOT_ORDER_SUBMENU_MAX_NUM];

            DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[FixedBootOrderTSE.c] UEFI DeviceType:%x (", DevOrder->Type));
            for (i = 0; i < DEVORDER_COUNT(DevOrder); i++)
                DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "%x ", OrderData[i]));
            DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, ")\n"));

            // Process enabled boot options first
            DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[FixedBootOrderTSE.c]FixedBootSetupData UEFI DeviceType:%x ", DevOrder->Type));
            for (i = 0; i < DEVORDER_COUNT(DevOrder); i++)
            {
                if (i >= FIXED_BOOT_ORDER_SUBMENU_MAX_NUM) break;

                Index = NVramOrderIndex[i];
                if (Index >= FIXED_BOOT_ORDER_SUBMENU_MAX_NUM)
                    break;  //is Disable

                DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[%x] ", Index));
                NewOrderData[i] = OrderData[Index] & FBO_UEFI_ORDER_MASK;    // [31:24] = disabled flag
                SetBootVariable(NewOrderData[i], TRUE);      // set boot#### ACTIVE
                OrderData[Index] = 0xffff;  // mark as processed
            }
            DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "\n"));

#if FIXED_BOOT_INCLUDE_DISABLED_OPTION
            // Then disabled boot options
            if (i < DEVORDER_COUNT(DevOrder))
            {
                UINTN j;
                DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "\nDisable BOOT####: "));
                for (j = 0; j < DEVORDER_COUNT(DevOrder); j++)
                {
                    if (OrderData[j] == 0xffff) // processed above already?
                        continue;

                    DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "%x ", OrderData[j]));
                    NewOrderData[i] = OrderData[j] | FBO_UEFI_DISABLED_MASK;    //(EIP129692+)  // marked as disabled
                    SetBootVariable(OrderData[j], FALSE);        //set boot#### disable.
                    i++;
                }
            }
#endif
            DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "\n"));

            UefiGroup = (UEFI_DEVICE_ORDER*)((UINT8*)UefiGroup
                                             + UefiGroup->Length
                                             + sizeof(UefiGroup->Type));
        }

        Status = pRS->SetVariable(L"UefiDevOrder", &FixedBootOrderGuid, Attr, Size, UefiDevBuffer);
        
#ifdef EFI_DEBUG
        // DEBUG: list the saved UefiDevOrder
        {
            UINT16 j;

            DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[FixedBootOrderSaveChange] Saved UefiDevOrder=%r ", Status));
            for (j = 0; j < Size; j++)
                DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "%02x ", *((UINT8*)UefiDevBuffer + j)));
            DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "\n"));
        }
#endif

        pBS->FreePool(DevOrderBuffer);
    }

    if (UefiDevBuffer)
        pBS->FreePool(UefiDevBuffer);

    FixedBootOrderToBoot();     //(EIP129692+)

    return Status;
}


SETUP_DATA  SetupBuffer;
SETUP_DATA  *pOriginalSetupData = &SetupBuffer;
FIXED_BOOT_SETUP *IfrNvDataSubMenu = NULL;
FIXED_BOOT_GROUP *FixedBootGroup = NULL;
//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Procedure:	PreserveFboSetting
//
// Description:	 Get Fbo settings before LoadSetupDefaults.
//
// Input:
// 	None
// Output:
//	None
// Modified:
//	None  
// Referrals: 
//      None
// Notes:	
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID PreserveFboSetting(VOID)
{
    EFI_STATUS  Status;
    EFI_GUID    gSetupGuid = SETUP_GUID;
    UINTN       BufferSize = sizeof (FIXED_BOOT_GROUP);
    BOOLEAN     OrignalState = gBrowserCallbackEnabled;

    pBS->CopyMem (pOriginalSetupData, 
        (SETUP_DATA*)gVariableList[VARIABLE_ID_SETUP].Buffer, sizeof(SETUP_DATA));
    
    Status = pBS->AllocatePool(EfiBootServicesData, BufferSize, &FixedBootGroup);
    if(EFI_ERROR(Status)) return;
    
    gBrowserCallbackEnabled = TRUE;
    Status = HiiLibGetBrowserData( &BufferSize, (VOID*)FixedBootGroup, &gSetupGuid, L"FixedBootGroup");

    if(EFI_ERROR(Status))
    {
        pBS->FreePool (FixedBootGroup);
        FixedBootGroup = NULL;
    }

#if DONOT_LOAD_DEFAULT_IN_SETUP
    BufferSize = sizeof (FIXED_BOOT_GROUP);
    Status = pBS->AllocatePool(EfiBootServicesData, BufferSize, &IfrNvDataSubMenu);
    if(EFI_ERROR(Status))
    {
        gBrowserCallbackEnabled = OrignalState;
        return;
    }
    
    Status = HiiLibGetBrowserData( &BufferSize, (VOID*)IfrNvDataSubMenu, &gFboGroupFormFormSetGuid, L"FixedBoot");
    if(EFI_ERROR(Status))
    {
        pBS->FreePool (IfrNvDataSubMenu);
        IfrNvDataSubMenu = NULL;
    }
#endif

    gBrowserCallbackEnabled = OrignalState;
}

/**
    TSE LoadDefault ELINK Hook.

        
    @param VOID
         
    @retval VOID
**/
EFI_STATUS
FixedBootOrderLoadedConfigDefaults(IN void)
{
#if DONOT_LOAD_DEFAULT_IN_SETUP


	SETUP_DATA *pSetupData = (SETUP_DATA*)gVariableList[VARIABLE_ID_SETUP].Buffer;

    pBS->CopyMem (pSetupData->LegacyPriorities, 
        pOriginalSetupData->LegacyPriorities, sizeof(UINT16) * 16);

    pBS->CopyMem (pSetupData->UefiPriorities, 
        pOriginalSetupData->UefiPriorities, sizeof(UINT16) * 16);


#if FBO_DUAL_MODE
    pBS->CopyMem (pSetupData->DualPriorities, 
        pOriginalSetupData->DualPriorities, sizeof(UINT16) * 32);
#endif

    pSetupData->BootMode = pOriginalSetupData->BootMode;

#if USE_BCP_DEFAULT_PRIORITIES
    pBS->CopyMem (pSetupData->BcpDefaultPriorities, 
        pOriginalSetupData->BcpDefaultPriorities, sizeof(UINT16) * 8);
#if FBO_DUAL_MODE
    pBS->CopyMem (pSetupData->BcpDefaultDualPriorities, 
        pOriginalSetupData->BcpDefaultDualPriorities, sizeof(UINT16) * 32);
#endif
#endif
    if(IfrNvDataSubMenu)
    {
        UINTN       BufferSize = sizeof (FIXED_BOOT_SETUP);
        BOOLEAN     OrignalState = gBrowserCallbackEnabled;

        gBrowserCallbackEnabled = TRUE;
        HiiLibSetBrowserData( BufferSize, IfrNvDataSubMenu, &gFboGroupFormFormSetGuid, L"FixedBoot");        
        gBrowserCallbackEnabled = OrignalState; 

        pBS->FreePool (IfrNvDataSubMenu);
    }

    {
        EFI_STATUS Status;
        EFI_GUID FixedBootOrderSetupGuid = FIXED_BOOT_ORDER_SETUP_GUID;
        EFI_FIXED_BOOT_ORDER_SETUP_PROTOCOL *FboSetupProtocol;

        Status = pBS->LocateProtocol(&FixedBootOrderSetupGuid, NULL, &FboSetupProtocol);
        if (!EFI_ERROR(Status))
            FboSetupProtocol->Callback(FBO_CALLBACK_SHOW_HIDE_BBS_PRIORITY_SUBMENU, &gBrowserCallbackEnabled);
    }
#else
    EFI_STATUS Status;
    EFI_GUID FixedBootOrderSetupGuid = FIXED_BOOT_ORDER_SETUP_GUID;
    EFI_FIXED_BOOT_ORDER_SETUP_PROTOCOL *pFBOSetup = NULL;

    if (gTseSkipFBOModule) return EFI_SUCCESS;
    Status = pBS->LocateProtocol(&FixedBootOrderSetupGuid, NULL, &pFBOSetup);
    if (!EFI_ERROR(Status))
        pFBOSetup->load_default_string(); // TRUE : LoadDefaultString

    LoadedDefault = TRUE;

    pFBOSetup->Callback(FBO_CALLBACK_SHOW_HIDE_BBS_PRIORITY_SUBMENU, &gBrowserCallbackEnabled);
#endif

    if(FixedBootGroup)
    {
        EFI_GUID    gSetupGuid = SETUP_GUID;
        UINTN       BufferSize = sizeof (FIXED_BOOT_SETUP);
        BOOLEAN     OrignalState = gBrowserCallbackEnabled;

        gBrowserCallbackEnabled = TRUE;
        HiiLibSetBrowserData( BufferSize, FixedBootGroup, &gSetupGuid, L"FixedBootGroup");        
        gBrowserCallbackEnabled = OrignalState; 

        pBS->FreePool (FixedBootGroup);

    }
    return EFI_SUCCESS;
}

/**
    TSE LoadPreviousValues ELINK Hook.

        
    @param VOID
         
    @retval VOID
**/
EFI_STATUS
FixedBootOrderLoadPreviousString(IN void)
{
    EFI_STATUS Status;
    EFI_GUID FixedBootOrderSetupGuid = FIXED_BOOT_ORDER_SETUP_GUID;
    EFI_FIXED_BOOT_ORDER_SETUP_PROTOCOL *pFBOSetup = NULL;
    EFI_GUID    gSetupGuid = SETUP_GUID;

    if (gTseSkipFBOModule) return EFI_SUCCESS;

    Status = pBS->LocateProtocol(&FixedBootOrderSetupGuid, NULL, &pFBOSetup);
    if (!EFI_ERROR(Status))
    {
	        UINTN       BufferSize = sizeof (FIXED_BOOT_SETUP);
	        FIXED_BOOT_SETUP *IfrNvDataSubMenu = NULL;
	        BOOLEAN     OrignalState = gBrowserCallbackEnabled;
	        
	        if(LoadedDefault && !LoadedDefaultAndSaved) // Loaded default but not save yet.
	            LoadedDefault = FALSE;
	        Status = pBS->AllocatePool(EfiBootServicesData, 
			                           BufferSize, &IfrNvDataSubMenu);
	        if(EFI_ERROR(Status)) return Status;
	        
	        gBrowserCallbackEnabled = TRUE;
	        Status = HiiLibGetBrowserData( &BufferSize, (VOID*)IfrNvDataSubMenu, 
			                               &gFboGroupFormFormSetGuid, L"FixedBoot");
            gBrowserCallbackEnabled = OrignalState;

	        if(EFI_ERROR(Status))
	            return Status;
	        
	        Status = pFBOSetup->load_previous_string((VOID*)IfrNvDataSubMenu, 
			                                          LoadedDefaultAndSaved); //FALSE : Load Previous Value
	        pBS->FreePool(IfrNvDataSubMenu);
	        
	        return Status;
    }

    return Status;
}

/**
    Set Legacy DevMap data to pFBOLegacyDevMap

    @param 
        FBODevMap *p
    @retval 
        EFI_STATUS
**/
EFI_STATUS SetLegacyDevMap(IN FBODevMap *p)
{
    pFBOLegacyDevMap = p;
    return EFI_SUCCESS;
}

/**
    Set Uefi DevMap data to pFBOUefiDevMap

    @param 
        FBODevMap *p
    @retval 
        EFI_STATUS
**/
EFI_STATUS SetUefiDevMap(IN FBODevMap *p)
{
    pFBOUefiDevMap = p;
    return EFI_SUCCESS;
}

/**
    Set Dual DevMap data to pFBODaulDevMap

    @param 
        FBODevMap *p
    @retval 
        EFI_STATUS
**/
EFI_STATUS SetDualDevMap(IN FBODevMap *p)
{
    pFBODualDevMap = p;
    return EFI_SUCCESS;
}

/**
    Get Legacy DevMap data.

        
    @param VOID
    @retval 
        *FBODevMap
**/
FBODevMap *GetLegacyDevMap(IN VOID)
{
    return pFBOLegacyDevMap;
}

/**
    Get Uefi DevMap data.

        
    @param VOID
    @retval 
        *FBODevMap
**/
FBODevMap *GetUefiDevMap(IN VOID)
{
    return pFBOUefiDevMap;
}

/**
    Get Daul DevMap data.

    @param 
        *FBODevMap
    @retval 
        EFI_STATUS
**/
FBODevMap *GetDualDevMap(IN VOID)
{
    return pFBODualDevMap;
}


/**
    Set Uefi DevMap data to pFBOHiiMap

    @param 
        FBOHiiMap *p
    @retval 
        EFI_STATUS
**/
EFI_STATUS SetHiiMap(IN FBOHiiMap *p)
{
    pFBOHiiMap = p;
    return EFI_SUCCESS;
}

/**
    Get HiiMap data.

        
    @param VOID
    @retval 
        *pFBOHiiMap
**/
FBOHiiMap *GetHiiMap(IN VOID)
{
    return pFBOHiiMap;
}

//(EIP123284+)>
/**
    Set BootOptionTokenMap data

    @param 
        UINT16 *p
    @retval 
        EFI_STATUS
**/
EFI_STATUS SetBootOptionTokenMap(IN UINT16 *p)
{
    pBootOptionTokenMap = p;
    return EFI_SUCCESS;
}

/**
    Get BootOptionTokenMap data.

        
    @param VOID
    @retval 
        *UINT16
**/
UINT16 *GetBootOptionTokenMap(IN VOID)
{
    return pBootOptionTokenMap;
}
//<(EIP123284+)

/**
    Set New Description data

    @param 
        FBO_DEVICE_INFORM *p
    @retval 
        EFI_STATUS
**/
EFI_STATUS SetNewDescription(IN FBO_DEVICE_INFORM *p)
{
    if (pFBONewDevDescription == NULL) 
    {
        pFBONewDevDescription = p ;
        return EFI_SUCCESS ;
    }
    else
    {
        FBO_DEVICE_INFORM *DevInfom = pFBONewDevDescription ;
        while( DevInfom->Next )
            DevInfom = DevInfom->Next ;
        DevInfom->Next = p ;
        return EFI_SUCCESS ;
    }
    
}

/**
    Get New Description data.

        
    @param VOID
    @retval 
        *FBO_DEVICE_INFORM
**/
FBO_DEVICE_INFORM *GetNewDescription(IN VOID)
{
    return pFBONewDevDescription;
}

/**
    Set Current Skip FBO Module Flag by SkipFboModule Variable
 
    @note  TRUE:     Skip FBO Module
            FALSE:    None Skip FBO Module    

**/
BOOLEAN
SetCurrentSkipFBOModuleFlag()
{
    EFI_STATUS Status;
    UINTN  Size = 0;
    UINT32 Attr;
    UINT8 *Flag = NULL ;
    EFI_GUID FixedBootOrderGuid = FIXED_BOOT_ORDER_GUID;
    BOOLEAN SkipFbo = FALSE ;
    Status = GetEfiVariable(L"SkipFboModule", &FixedBootOrderGuid, &Attr, &Size, &Flag);
    if (!EFI_ERROR(Status))
    {
        if (*Flag)
        {
            pRS->SetVariable( L"CurrentSkipFboModule", &FixedBootOrderGuid, 
                    EFI_VARIABLE_BOOTSERVICE_ACCESS, sizeof(UINT8), Flag );
            SkipFbo = TRUE ;
        }
        else
            pRS->SetVariable( L"CurrentSkipFboModule", &FixedBootOrderGuid, 
                    EFI_VARIABLE_BOOTSERVICE_ACCESS, sizeof(UINT8), Flag );
        
        pBS->FreePool(Flag) ;
    }
    else
        pRS->SetVariable( L"CurrentSkipFboModule", &FixedBootOrderGuid, 0, 0, NULL );
    return SkipFbo ;
}

static EFI_FIXED_BOOT_ORDER_PROTOCOL gFixedBootOrderProtocol =
{
    SetLegacyDevMap,
    SetUefiDevMap,
    SetDualDevMap,
    SetHiiMap,
    SetBootOptionTokenMap,     //(EIP123284+)
    SetNewDescription,
    GetLegacyDevMap,
    GetUefiDevMap,
    GetDualDevMap,
    GetHiiMap,
    GetBootOptionTokenMap,     //(EIP123284+)
    GetNewDescription
};

/**
    Initialize EFI_FIXED_BOOT_ORDER_PROTOCOL function service.

        
    @param VOID
         
    @retval VOID
**/
void FBO_Protocol_install(IN void)
{
    EFI_HANDLE Handle = NULL;
    EFI_GUID FixedBootOrderGuid = FIXED_BOOT_ORDER_GUID;
    EFI_STATUS Status;

    if (SetCurrentSkipFBOModuleFlag()) return ;
    Status = pBS->InstallProtocolInterface(
                 &Handle,
                 &FixedBootOrderGuid,
                 EFI_NATIVE_INTERFACE,
                 &gFixedBootOrderProtocol
             );

    // Install protocol interface
    DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "Install FixedBootOrder protocol (%r)\n", Status));
}
