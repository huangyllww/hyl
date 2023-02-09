//***********************************************************************
//*                                                                     *
//*   Copyright (c) 1985-2019, American Megatrends International LLC.   *
//*                                                                     *
//*      All rights reserved. Subject to AMI licensing agreement.       *
//*                                                                     *
//***********************************************************************

/** @file OemFixedBootOrderIpmi.c

    Adjust Boot Sequence form IPMI.
    
    Update Boot Priority or BootNext form IPMI setting.


*/


#include <AmiDxeLib.h>
#include <BootOptions.h>
#include <Setup.h>
#include <../FixedBootOrder.h>
#include <DefaultFixedBootOrder.h>

#if IPMI2_SUPPORT
#define IPMI_HEADER(name)           <Ami##name>
#define IPMI_BOOT_DEVICE_TYPE(name) AMI_##name##_SELECTOR
#define IPMI_ENUM(name)             AmiIpmi##name
#else
#define IPMI_HEADER(name)           <name>
#define IPMI_BOOT_DEVICE_TYPE(name) name
#define IPMI_ENUM(name)             name
#endif
#include IPMI_HEADER(IpmiNetFnChassisDefinitions.h)

#define FBO_HIGHEST_BOOT_OPTION_PRIORITY  0x00
#define FBO_HIGHEST_BOOT_OPTION_TAG       0x00

extern UINT16 GetBbsEntryDeviceTypeDefault(BBS_TABLE *BbsEntry);

/**
   Keep the information of group order and restore them after non-persistent boot.
*/
typedef struct _FBO_IPMI_GROUP_ORDER {
    BOOLEAN   ModeFlag;         ///< TRUE for the same mode before IPMI changes and otherwise FALSE.
    UINT16    InterchangeIndex; ///< Index used to know which group interchanged with the first group.
} FBO_IPMI_GROUP_ORDER;

//
// extern from IpmiBoot.c
//
extern  BOOLEAN                                 gPersistentBoot;
extern  BOOLEAN                                 gUefiBoot;
extern  UINT8                                   gDeviceInstanceSelector;
extern  IPMI_BOOT_DEVICE_TYPE(IPMI_BOOT_DEVICE) gIpmiForceBootDevice;

//
// extern from FboSetOrder.c
//
extern  FBODevMap           *pFBOLegacyDevMap;
extern  FBODevMap           *pFBOUefiDevMap;
extern  UINT16  GetDevMapDataCount(FBODevMap *pFBODevMap);
extern  UINT32  GetUefiBootOptionTag(IN BOOT_OPTION *Option);
extern  UINT16  SearchDevMapByType(FBODevMap *pFBODevMap, UINT16 DevType );




/**
    Search the boot option which set by IPMI.
	
	@retval BOOT_OPTION* Boot option set by IPMI
**/
BOOT_OPTION* SearchBootOptionSetByIpmi() 
{
    DLINK       *Link;
    BOOT_OPTION *Option;

    FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option){
        //
        //  IPMI raises the tag/priority of target option to highest.
        //
        if (Option->Priority == FBO_HIGHEST_BOOT_OPTION_PRIORITY
                && Option->Tag == FBO_HIGHEST_BOOT_OPTION_TAG)
            return Option;
    }

    return NULL;
}

/**
    Save current group order before changing it with IPMI request,
    but only works on non-persistent boot.

    @param  OriginalBootMode  Mode current used.
    @param  AlteredBootMode   Mode ready to change.
    @param  InterchangeIndex  Indicates which group interchanged with group one.

**/
VOID SaveGroupOrder(BOOLEAN ModeFlag, UINT16 InterchangeIndex) 
{
    EFI_STATUS Status;
    FBO_IPMI_GROUP_ORDER GroupOrder;
    EFI_GUID FixedBootOrderGuid = FIXED_BOOT_ORDER_GUID;

    GroupOrder.ModeFlag = ModeFlag;
    GroupOrder.InterchangeIndex = InterchangeIndex;

    Status = pRS->SetVariable(
                L"FboGroupOrder",
                &FixedBootOrderGuid,
                EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                sizeof(FBO_IPMI_GROUP_ORDER),
                &GroupOrder);
    ASSERT_EFI_ERROR(Status);
}


/**
    Alter group order by IPMI request.

    @param  Option   The boot option set by IPMI.
    @param  Save     Save the info of group order or not.

    @retval  EFI_INVALID_PARAMETER  Option is invalid or BbsEntry is invalid when legacy boot.
	@retval  EFI_NOT_FOUND          No group matched.
    @retval  EFI_SUCCESS            Change done.
    @retval  Status                 Return from GetVariable call.

**/
EFI_STATUS AlterGroupOrder(BOOT_OPTION *Option, BOOLEAN Save)
{
    EFI_STATUS Status;
    BOOLEAN    UpdateVariable = TRUE;
    UINT16     i;
    UINT16     Temp;
    UINTN      DeviceMapCount;
    UINTN      SetupSize = sizeof(SETUP_DATA);
    UINT32     SetupAttr;
    EFI_GUID   SetupGuid = SETUP_GUID;
    SETUP_DATA SetupData;
    UINT32     Type;

    if (!Option) return EFI_INVALID_PARAMETER;

    Status = pRS->GetVariable( L"Setup", &SetupGuid, &SetupAttr, &SetupSize, &SetupData);
    if(EFI_ERROR(Status)) Status;

    if(!gUefiBoot){     // Legacy Boot?

        //
        // Update SetupData.LegacyPriorities
        //
        // Below is sample, please implement how to match options and change proper priority order.  >>>>
        //
        if (!Option->BbsEntry) return EFI_INVALID_PARAMETER;
        DeviceMapCount = GetDevMapDataCount( pFBOLegacyDevMap );
        Type = GetBbsEntryDeviceType(Option->BbsEntry);
        for(i=0 ;i<DeviceMapCount;i++){

            if(SetupData.LegacyPriorities[i] == SearchDevMapByType(pFBOLegacyDevMap,Type)
                    && SetupData.LegacyPriorities[i] != DeviceMapCount){
               
                Temp = SetupData.LegacyPriorities[0];
                SetupData.LegacyPriorities[0] = SetupData.LegacyPriorities[i];
                SetupData.LegacyPriorities[i] = Temp;

                UpdateVariable = TRUE;
                break;
            }
        }
        //
        // Above is sample, please implement how to match options and change boot priority order.  <<<<
        //
    
    }
    else{               // Uefi boot
        // Update SetupData.UefiPriorities
        //
        // Below is sample, please implement how to match options and change boot priority order.  >>>>
        //
        DeviceMapCount = GetDevMapDataCount( pFBOUefiDevMap );
        Type = GetUefiBootOptionTag(Option);
        for(i=0 ;i<DeviceMapCount;i++){
            if(SetupData.UefiPriorities[i] == SearchDevMapByType(pFBOUefiDevMap,Type)
                    && SetupData.UefiPriorities[i] != DeviceMapCount){
               
                Temp = SetupData.UefiPriorities[0];
                SetupData.UefiPriorities[0] = SetupData.UefiPriorities[i];
                SetupData.UefiPriorities[i] = Temp;
                
                UpdateVariable = TRUE;
                break;
            }
        }
        //
        // Above is sample, please implement how to match options and change proper priority order.  <<<<
        //
    }

    if (i == DeviceMapCount) return EFI_NOT_FOUND;

    if(UpdateVariable) {
        if(Save){
            SaveGroupOrder(SetupData.BootMode != gUefiBoot, i);
        }
        SetupData.BootMode = gUefiBoot;
        Status = pRS->SetVariable(L"Setup", &SetupGuid, SetupAttr, SetupSize, &SetupData);
        ASSERT_EFI_ERROR(Status);
    }

    return EFI_SUCCESS;
}

/**
    Restore group changed by IPMI request, 
    but only works on non-persistent boot.
**/
VOID RestoreGroupOrder (VOID)
{
    EFI_STATUS             Status;
    UINT16                 Temp;
    FBO_IPMI_GROUP_ORDER   GroupOrder;
    UINTN                  GroupOrderSize = sizeof(FBO_IPMI_GROUP_ORDER);
    EFI_GUID               FixedBootOrderGuid = FIXED_BOOT_ORDER_GUID;

    UINTN                  SetupSize = sizeof(SETUP_DATA);
    UINT32                 SetupAttr;
    EFI_GUID               SetupGuid = SETUP_GUID;
    SETUP_DATA             SetupData;

    Status = pRS->GetVariable( L"FboGroupOrder", &FixedBootOrderGuid, NULL, &GroupOrderSize, &GroupOrder);
    if(EFI_ERROR(Status)) return;

    Status = pRS->GetVariable( L"Setup", &SetupGuid, &SetupAttr, &SetupSize, &SetupData);
    if(EFI_ERROR(Status)) return;

    if(SetupData.BootMode){
        Temp = SetupData.UefiPriorities[0];
        SetupData.UefiPriorities[0] = SetupData.UefiPriorities[GroupOrder.InterchangeIndex];
        SetupData.UefiPriorities[GroupOrder.InterchangeIndex] = Temp;
    } 
    else{
        Temp = SetupData.LegacyPriorities[0];
        SetupData.LegacyPriorities[0] = SetupData.LegacyPriorities[GroupOrder.InterchangeIndex];
        SetupData.LegacyPriorities[GroupOrder.InterchangeIndex] = Temp;
    }
    SetupData.BootMode = SetupData.BootMode ^ GroupOrder.ModeFlag;

    Status = pRS->SetVariable(L"Setup", &SetupGuid, SetupAttr, SetupSize, &SetupData);
    ASSERT_EFI_ERROR(Status);

    Status = pRS->SetVariable(L"FboGroupOrder", &FixedBootOrderGuid, 0, 0, NULL);
    ASSERT_EFI_ERROR(Status);
}

/**
    Restore device type to make IPMI works normally.
**/
VOID FboRestoreBbsDpDeviceTypeForIpmi(){

    DLINK           *Link;
    BOOT_OPTION     *Option;
    
    FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option){
        BBS_BBS_DEVICE_PATH *BbsDp;

        if (!IsLegacyBootOption(Option) || Option->GroupHeader) 
            continue;

        BbsDp = (BBS_BBS_DEVICE_PATH*)Option->FilePathList;
        if (BbsDp
                && BbsDp->Header.Type == BBS_DEVICE_PATH
                && BbsDp->Header.SubType == BBS_BBS_DP)
        {
            BbsDp->DeviceType = GetBbsEntryDeviceTypeDefault(Option->BbsEntry);
        }
    }

}

/** 
    Set device type according to FBO's settings.
**/
VOID FboSetBbsDpDeviceType(){
    DLINK           *Link;
    BOOT_OPTION     *Option;
    
    FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option){
        BBS_BBS_DEVICE_PATH *BbsDp;

        if (!IsLegacyBootOption(Option) || Option->GroupHeader) 
            continue;

        BbsDp = (BBS_BBS_DEVICE_PATH*)Option->FilePathList;
        if (BbsDp
                && BbsDp->Header.Type == BBS_DEVICE_PATH
                && BbsDp->Header.SubType == BBS_BBS_DP)
        {
            BbsDp->DeviceType = GetBbsEntryDeviceType(Option->BbsEntry);
        }
    }
}

/**
    Apply IPMI boot request on FBO.
**/
VOID ApplyIpmiBootRequest(){
    
    //
    // Below sample doesn't consider about FBO DualMode.
    //
    UINT8           TargetType = 0;
    BOOT_OPTION     *Option = NULL;

    RestoreGroupOrder();

    switch(gIpmiForceBootDevice){
        case IPMI_ENUM(ForcePxe):
        case IPMI_ENUM(ForceBootHardDrive):
        case IPMI_ENUM(ForceBootCdDvd):
        case IPMI_ENUM(ForceBootFloppy):
            break;

        case IPMI_ENUM(NoOverride):
        default:
            return;
    }

    if(!(Option = SearchBootOptionSetByIpmi())){
        return;
    }

    if(gPersistentBoot){
        AlterGroupOrder(Option, FALSE); // Do not save the info of group order.
    }
    else{

        //
        // Use BootNext solution for Next boot only request.
        //

        EFI_STATUS      Status;
        Status = pRS->SetVariable (
                     L"BootNext", 
                     &gEfiGlobalVariableGuid,
                     EFI_VARIABLE_NON_VOLATILE| EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                     sizeof(Option->BootOptionNumber), 
                     &Option->BootOptionNumber);
        ASSERT_EFI_ERROR(Status);

#if FBO_NON_PERSISTENT_BOOT_ON_GROUP
        AlterGroupOrder(Option, TRUE); // Save the info of group order.
#endif
    } 
}


/**
    Sync BootOptionTag for Group
    IpmiBoot Set Priority and Tag of the desired bootoption to 0.
    BDS will delete the Group Header because TAG is different.
    Then BootNext doesn't work property

    @param VOID

    @retval VOID

**/
VOID
FboIpmiBootTag
(
        VOID
)
{
    DLINK           *Link = NULL, *Link1 = NULL;
    BOOT_OPTION     *Option = NULL, *Option1 = NULL;
    
    if(gUefiBoot)
        return;
    
    SortList(BootOptionList, CompareTagThenPriority);
    FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option)
    {
        // Skip Uefi Boot Option
        if(!IsLegacyBootOption(Option))
            continue;
        
        if(Option->GroupHeader)
        {
            FOR_EACH_BOOT_OPTION(BootOptionList,Link1,Option1)
            {
                // Skip Uefi BootOption and GroupHeader
                if(Option1->GroupHeader || (!IsLegacyBootOption(Option1)) )
                    continue;
                
                if(Option->BootOptionNumber == Option1->BootOptionNumber)
                    Option1->Tag = Option->Tag;
            }
        } 
    }
}
