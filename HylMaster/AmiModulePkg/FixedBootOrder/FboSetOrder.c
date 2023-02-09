//***********************************************************************
//*                                                                     *
//*   Copyright (c) 1985-2019, American Megatrends International LLC.   *
//*                                                                     *
//*      All rights reserved. Subject to AMI licensing agreement.       *
//*                                                                     *
//***********************************************************************

/** @file FBOSetOrder.c

    FixedBootOrder Boot Option control
*/

//----------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------
#include <Efi.h>
#include <AmiDxeLib.h>
#include <Protocol/BlockIo.h>
#include <Library/DebugLib.h>
#include <IndustryStandard/Pci.h>

#include <BootOptions.h>
#include <Setup.h>
#include <setupdata.h>

#if CSM_SUPPORT
#include <Protocol/LegacyBios.h>
#include <Protocol/LegacyBiosExt.h>
#endif

#include "FixedBootOrder.h"
#include <DefaultFixedBootOrder.h>
#if LOCK_SETVAR_AT_ENDOFDXE
#include "../AMISetupNVLock/AMISetupNVLock.h"
#endif
//---------------------------------------------------------------------------
// Global Variable declarations
//---------------------------------------------------------------------------
EFI_GUID gFixedBootOrderGuid = FIXED_BOOT_ORDER_GUID;

//---------------------------------------------------------------------------
// External functions
//---------------------------------------------------------------------------
EFI_HANDLE GetPhysicalBlockIoHandle(EFI_HANDLE BlockIoHandle);
UINT32 GetUefiBootOptionTag(IN BOOT_OPTION *Option);
UINT16 GetDevicePortNumber(BBS_TABLE *BbsTable);
GET_BBS_ENTRY_DEVICE_TYPE BOOT_OPTION_GET_BBS_ENTRY_DEVICE_TYPE_FUNCTION;
UINT16 FboGetLegacyTagWithBbsIdx(UINT16 BbsIdx) ;
VOID PreProcessBootOptions();
extern UINT8 gBdsSkipFBOModule ;
extern UINT16 GetBbsEntryDeviceTypeDefault(BBS_TABLE *BbsEntry) ;
#if CSM_SUPPORT
UINT32 FindPositionInTheGroupByBbsIndex(LEGACY_DEVICE_ORDER *Group, UINT16 BbsIndex);
#endif
//---------------------------------------------------------------------------
// Function Definitions
//---------------------------------------------------------------------------
VOID FboSetupSizeCheck(VOID);
//---------------------------------------------------------------------------
// Type definitions
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Constant and Variables declarations
//---------------------------------------------------------------------------
// Following will be set to proper structure
// By default they are set in DefaultFixedBootOrder_Init()
FBODevMap *pFBODualDevMap = NULL;
FBODevMap *pFBOLegacyDevMap = NULL;
FBODevMap *pFBOUefiDevMap = NULL;           // default: FBOUefiDevMapData

EFI_FIXED_BOOT_ORDER_BDS_PROTOCOL gFixedBootOrderBdsProtocol =
{
    FboGetLegacyTagWithBbsIdx
} ;


UINT16 GetDevMapDataCount(FBODevMap *pFBODevMap)
{
    UINT16 count = 0;

    if (!pFBODevMap) return 0;           //end of data

    do
    {
        if (pFBODevMap[count].DevType == 0) break;
        count++;
    }
    while (1);

    return count;
}

/**
    Search DevMap data index by device type.

        
        IN FBODevMap *pFBODevMap
    @param DevType 

    @retval 
        OUT UINT16

    @note  None

**/
UINT16 SearchDevMapByType(FBODevMap *pFBODevMap, UINT16 DevType)
{
    UINT16 i = 0;

    do
    {
        if (pFBODevMap[i].DevType == 0) break;  //end of data
        if (pFBODevMap[i].DevType == DevType)
            return i;
        i++;
    }
    while (1);

    return 0xffff;
}

/**
    Get FixedBootOrder legacy type with bbs index.
    
    @param UINT16 BbsIdx
    
    @retval Legacy boot option tag
**/
UINT16
FboGetLegacyTagWithBbsIdx(
    UINT16 BbsIdx)
{
#if CSM_SUPPORT
    EFI_STATUS Status ;
    EFI_LEGACY_BIOS_PROTOCOL *LegacyBios = NULL;
    UINT16 HddCount;
    UINT16 BbsCount;
    HDD_INFO *HddInfo;
    BBS_TABLE *BbsTable;
    
    Status = pBS->LocateProtocol(&gEfiLegacyBiosProtocolGuid, NULL, &LegacyBios) ;
    if (EFI_ERROR(Status)) return INVALID_FBO_TAG ;
    
    Status = LegacyBios->GetBbsInfo(LegacyBios, &HddCount, &HddInfo, &BbsCount, &BbsTable);
    if (EFI_ERROR(Status)) return INVALID_FBO_TAG;
    
    return BOOT_OPTION_GET_BBS_ENTRY_DEVICE_TYPE_FUNCTION(&BbsTable[BbsIdx]);
#else
    return INVALID_FBO_TAG ;
#endif
}

/**
    For the passed Tag entry, return the correct boot priority based on
    the BootOptionTagPriorities global variable that is filled out
    based on the SDL Token BOOT_OPTION_TAG_PRIORITIES.

    @param UINT16 Tag - one of the following items of the BOOT_OPTION_TAG enum:

    @retval The index of this item in the FBODevMapData structure, which
        also corresponds to the boot priority that should be assigned to this
        class of device
**/
UINT32 FBO_FindTagPriority(UINT16 Tag)
{
    UINT32 i = 0;
    UINT16 DeviceMapCount;

    DeviceMapCount = GetDevMapDataCount(pFBOLegacyDevMap);

    for (i = 0; i < DeviceMapCount; i++)
        if (Tag == pFBOLegacyDevMap[i].DevType) return i;

    return UNASSIGNED_HIGHEST_TAG;
}

/**
    determine the device type of the USBKey size.

    @param EFI_HANDLE Handle

    @retval BOOLEAN
**/
BOOLEAN IsUSBkeyByHandle(EFI_HANDLE Handle)
{
    EFI_BLOCK_IO_PROTOCOL *BlkIo;
    UINT64  SizeInMb;
    EFI_GUID gEfiBlockIoProtocolGuid = EFI_BLOCK_IO_PROTOCOL_GUID;


    if (!EFI_ERROR(pBS->HandleProtocol(
                       Handle,
                       &gEfiBlockIoProtocolGuid,
                       &BlkIo)))
    {
        SizeInMb = MultU64x32(BlkIo->Media->LastBlock + 1, BlkIo->Media->BlockSize);
        SizeInMb = DivU64x32(SizeInMb, 1000000);

        if (SizeInMb <= USBKEY_RANGE_SIZE)
            return TRUE;
    }

    return FALSE;
}

#if CSM_SUPPORT
/**
    Using the passed BBS_TABLE entry pointer, determine the device
    type of the USBKey

    @param BBS_TABLE *BbsEntry - pointer to a BBS_TABLE entry

    @retval BOOLEAN
**/
BOOLEAN CheckIsUSBkey(BBS_TABLE *BbsEntry)
{
    if (BbsEntry->DeviceType == BBS_HARDDISK)
        return IsUSBkeyByHandle(*(VOID**)(&BbsEntry->IBV1));
    else
        return FALSE;
}
#endif

/**
    Using the passed BBS_TABLE entry pointer, determine the device
    type of the associated device

    @param BBS_TABLE *BbsEntry - pointer to a BBS_TABLE entry

    @retval UINT16 device type

**/
UINT16 FBO_GetBbsEntryDeviceType(BBS_TABLE *BbsEntry)
{
#if CSM_SUPPORT
    CHAR8   *AsciiNameStr;
    UINT16  DeviceType;
    UINT16  DeviceMapCount = GetDevMapDataCount(pFBOLegacyDevMap);
    UINT16  FboLegacyType;
    BOOLEAN IsUSBDevice;
    UINT16  i;
	


    if (gBdsSkipFBOModule) 
        return GetBbsEntryDeviceTypeDefault(BbsEntry) ;

    if (!BbsEntry || !DeviceMapCount)
        goto UNKNOWN_DEVICE;

    DeviceType = BbsEntry->DeviceType;
    IsUSBDevice = (BbsEntry->Class == 0x0C) && (BbsEntry->SubClass == 0x03);
#if USB_GROUP == 0
    IsUSBDevice = FALSE;
#endif

#if BBS_NETWORK_DEVICE_TYPE_SUPPORT
    if (BbsEntry->Class == PCI_CL_NETWORK
            && BbsEntry->DeviceType == BBS_BEV_DEVICE
       ) DeviceType = BBS_EMBED_NETWORK;
#endif

    
    if (DeviceType == BBS_HARDDISK)
    {
        if ((BbsEntry->Class == PCI_CL_SYSTEM_PERIPHERALS) && (BbsEntry->SubClass == PCI_CL_SYSTEM_PERIPHERALS_SCL_SD))
        {
            FboLegacyType = BoTagLegacySd;
        }
        else if ((BbsEntry->Class == PCI_CLASS_MASS_STORAGE) && (BbsEntry->SubClass == PCI_CLASS_MASS_STORAGE_SOLID_STATE))
        {
#if NVME_GROUP_SUPPORT
            FboLegacyType = BoTagLegacyNvme;
#else
            FboLegacyType = BoTagLegacyHardDisk;
#endif
        }
        else if (IsUSBDevice)
        {
            if (CheckIsUSBkey(BbsEntry))
            {
                FboLegacyType = BoTagLegacyUSBKey;
            }
            else 
            {
                FboLegacyType = BoTagLegacyUSBHardDisk;
            }
        }
        else 
        {
            for (i = 0; i < DeviceMapCount; i++)
            {
                if (!(pFBOLegacyDevMap[i].Attr & F_HDD))
                    continue;

                if (pFBOLegacyDevMap[i].BBSType != DeviceType)
                    continue;

                if (pFBOLegacyDevMap[i].Port == GetDevicePortNumber(BbsEntry))
                    return pFBOLegacyDevMap[i].DevType;
            }

            FboLegacyType = BoTagLegacyHardDisk;
        }
    }
    else if (DeviceType == BBS_CDROM)
    {
  
        if (IsUSBDevice) 
        {
             FboLegacyType = BoTagLegacyUSBCdrom;
        }
        else
        {
            FboLegacyType = BoTagLegacyCdrom;
        }

    }
    else if (DeviceType == BBS_EMBED_NETWORK)
    {
        if(IsUSBDevice)
        {
            FboLegacyType = BoTagLegacyUSBLan;
        }
        else
        {
            FboLegacyType =  BoTagLegacyEmbedNetwork;
        }
    }
    else if (DeviceType == BBS_FLOPPY)
    {
        FboLegacyType = BoTagLegacyUSBFloppy;
    }
    else if (DeviceType == BBS_USB)
    {
        FboLegacyType = BoTagLegacyUsb;
    }
    else
    {
        goto UNKNOWN_DEVICE;
    }

#if USB_GROUP == 1
    //
    //  BoTagLegacyUSBFloppy(8) -- BoTagLegacyUSBLan(12)
    //
    if (FboLegacyType >= BoTagLegacyUSBFloppy && FboLegacyType <= BoTagLegacyUSBLan) {
        FboLegacyType = BoTagLegacyUsb;
    }
#endif
    
    //
    //  Verify if the type is valid.
    //
    for (i = 0; i < DeviceMapCount; i++)
    {
        if (FboLegacyType == pFBOLegacyDevMap[i].DevType)
            return FboLegacyType;
    }

UNKNOWN_DEVICE:
    AsciiNameStr = (CHAR8*)(UINTN)((BbsEntry->DescStringSegment << 4)
                                   + BbsEntry->DescStringOffset);
    DEBUG((FBO_TRACE_LEVEL | DEBUG_WARN, "%a :: no matched group (UNKNOWN_DEVICE)\n"));

#if OTHER_GROUP == 1
    return BoTagLegacyOther;
#else
    return BBS_UNKNOWN;
#endif

#else  //CSM_SUPPORT
    return 0;
#endif  //#if CSM_SUPPORT

}

/**
    Update UefiDevOrder variable according to BootOptionList.
    It's used for setup and we need to sync it with BootOptionList before setup.

    @param DLIST *BootOptionList - The BDS boot option list

    @retval UefiDevOrder variable updated

**/
VOID BuildUEFIDevOrder(
)
{
    EFI_STATUS Status;
    UINT16 i, j;
    UINT16 k;
    DLINK *Link;
    BOOT_OPTION *Option;
    UINTN OldDvOrderSize = 0;
    UEFI_DEVICE_ORDER *OldDevOrder = NULL, *OldDevOrder2 = NULL;
    UEFI_DEVICE_ORDER *DevOrder = NULL, *DevOrderStart = NULL;
    UINT16 DeviceMapCount;

    // BootOrderFlag is an array corresponding to each item in BootOptionList
    // Each item in BootOptionList has a flag.
    // -1 (!=0): device order has been set for this boot option.
    UINT16 *BootOrderFlag = NULL;
    UINT16 OptionIndex;

    // if no BootOption, clean UefiDevOrder and return this function.
    if (BootOptionList == NULL || !BootOptionList->Size)
    {
    	pRS->SetVariable(L"UefiDevOrder", &gFixedBootOrderGuid,0, 0, NULL);
    	goto FreeAndExit;
    }
    
    // An array corresponding to DevOrder[]
    // Contains the EFI_LOAD_OPTION.Attributes

    BootOrderFlag = MallocZ(BootOptionList->Size * sizeof(UINT16));
    if (BootOrderFlag == NULL)
        goto FreeAndExit; // allocation failed

    Status = GetEfiVariable(L"UefiDevOrder", &gFixedBootOrderGuid, NULL, &OldDvOrderSize, &OldDevOrder);
    DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "FBOSetOrder.c BuildUEFIDevOrder :: GetEfiVariable UefiDevOrder (%r)\n", Status));
    if (EFI_ERROR(Status))
    {
        OldDvOrderSize = 0;
        OldDevOrder = NULL;
    }

    // allocates maximum possible size for UefiDevOrder,
    // i.e. One boot option for one group/tag (UEFI_DEVICE_ORDER)
    DevOrderStart = DevOrder = MallocZ((BootOptionList->Size + 1) * sizeof(UEFI_DEVICE_ORDER));

    if (DevOrder == NULL)
        goto FreeAndExit; // allocation failed

    DeviceMapCount = GetDevMapDataCount(pFBOUefiDevMap);
    for (i = 0; i < DeviceMapCount; i++)    // scan throgh all types in pFBOUefiDevMap
    {
        UINT16 DevType;
        DevType = pFBOUefiDevMap[i].DevType;
        if (DevType == 0) break;     // end of data
        DevOrder->Type = DevType;
        DevOrder->Length = sizeof(DevOrder->Length);    // length includes itself

        j = 0;  // index of this group/tag in UefiDevOrder

        //------------------------------------------------------------------------------
        // Use UefiDevOrder to determine the group's order and BootOptionList for numbers within the group
        // Assume BootOptionList has been sorted
        for (OldDevOrder2 = OldDevOrder
             ; (UINT8*)OldDevOrder2 < (UINT8*)OldDevOrder + OldDvOrderSize
             ; OldDevOrder2 = NEXT_DEVICE(UEFI_DEVICE_ORDER, OldDevOrder2))
        {
            if (DevType != OldDevOrder2->Type) continue;

            for (k = 0; k < DEVORDER_COUNT(OldDevOrder2); k++)
            {
                OptionIndex = -1;  // index to BootOptionList
                FOR_EACH_BOOT_OPTION(BootOptionList, Link, Option)
                {
                    CHAR16 BootStr[9] = {0};
                    UINTN BootSize = 0 ;
                    VOID *BootVar = NULL ;
                                            
                    OptionIndex++;
                    if (IsLegacyBootOption(Option) || Option->GroupHeader) continue;
                    if (BootOrderFlag[OptionIndex]) continue;    //(EIP129692+)
                    if (GetUefiBootOptionTag(Option) != DevType) continue;      //(EIP100223+)
					if (!IsBootOptionWithDevice(Option))continue;
                    // check device has BootXXXX variable
                    Swprintf(BootStr, L"Boot%04X", Option->BootOptionNumber);
                    Status = pRS->GetVariable(BootStr, &gEfiGlobalVariableGuid, NULL, &BootSize, BootVar) ;
                    if (Status != EFI_BUFFER_TOO_SMALL) continue ;
                    
                    if (Option->BootOptionNumber == (OldDevOrder2->Device[k] & FBO_UEFI_ORDER_MASK)) // highest byte is for disabled flag
                    {
                        DevOrder->Length += sizeof(DevOrder->Device);
                        DevOrder->Device[j] = Option->BootOptionNumber;
                        BootOrderFlag[OptionIndex] = -1;    // boot order assigned
                        j++;
                    }
                }
            }
        }
        //------------------------------------------------------------------------------

        // adds new devices not in UefiDevOrder
        OptionIndex = -1;  // index to BootOptionList
        FOR_EACH_BOOT_OPTION(BootOptionList, Link, Option)
        {
            OptionIndex++;

            if (IsLegacyBootOption(Option) || Option->GroupHeader) continue;
            if (BootOrderFlag[OptionIndex]) continue;    // already set?

			if (!IsBootOptionWithDevice(Option))continue;
            if (GetUefiBootOptionTag(Option) == DevType)
            {
                DevOrder->Length += sizeof(DevOrder->Device);
                DevOrder->Device[j] = Option->BootOptionNumber;
                BootOrderFlag[OptionIndex] = -1;    // boot order assigned
                j++;
            }
        }

        if (DevOrder->Length > sizeof(DevOrder->Length))    // any boot option in the type?
        {                                                                         //(EIP129692+)>
            UINT16 DeviceCount;
            DeviceCount = DEVORDER_COUNT(DevOrder);


            // Initialize DevOrder->Device[] [31:24] for disabled flag according to current BootOptionList
            FOR_EACH_BOOT_OPTION(BootOptionList, Link, Option)
            {
                for (k = 0; k < DeviceCount; k++)
                {
                    if ((DevOrder->Device[k]&FBO_UEFI_ORDER_MASK) == Option->BootOptionNumber)    // highest byte is for disabled flag
                        if (Option->Attributes&LOAD_OPTION_ACTIVE)
                            DevOrder->Device[k] &= FBO_UEFI_ORDER_MASK;     // enabled, [31:24]=0
                        else
                            DevOrder->Device[k] |= FBO_UEFI_DISABLED_MASK;  // disabled, [31:24]=0xff
                }
            }

            //
            // Set New DevOrder: enabled first, disabled later
            //
            {
                UEFI_DEVICE_ORDER *NewOrder = MallocZ(DevOrder->Length + sizeof(DevOrder->Type));
                if (NewOrder)
                {
                    UINT16 NewOrderCount = 0;
                    NewOrder->Type = DevOrder->Type;
                    NewOrder->Length = DevOrder->Length;

                    // set active boot order first
                    for (j = 0; j < DeviceCount; j++)
                    {
                        if (DevOrder->Device[j]&FBO_UEFI_DISABLED_MASK)  // disabled?
                            continue;
                        NewOrder->Device[NewOrderCount] = DevOrder->Device[j];
                        NewOrderCount++;
                    }

                    // then set disabled boot order
                    for (j = 0; j < DeviceCount; j++)
                    {
                        if (!(DevOrder->Device[j]&FBO_UEFI_DISABLED_MASK))  // enabled?
                            continue;
                        NewOrder->Device[NewOrderCount] = DevOrder->Device[j];
                        NewOrderCount++;
                    }

                    pBS->CopyMem(DevOrder, NewOrder, DevOrder->Length + sizeof(DevOrder->Type));
                    pBS->FreePool(NewOrder);
                }
            }                                                                     //<(EIP129692+)
            DevOrder = (UEFI_DEVICE_ORDER*)((UINT8*)DevOrder + DevOrder->Length + sizeof(DevOrder->Type));
        }
    }


    //
    // Set UefiDevOrder variable
    //
    {
        UINTN DevOrderSize;
        DevOrderSize = (UINT8*)DevOrder - (UINT8*)DevOrderStart;

        if (DevOrderSize)
        {
            pRS->SetVariable(
                L"UefiDevOrder", &gFixedBootOrderGuid,
                VARIABLE_ATTRIBUTES, DevOrderSize, DevOrderStart);
            pRS->SetVariable(
                L"OriUefiDevOrder", &gFixedBootOrderGuid,
                VARIABLE_ATTRIBUTES, DevOrderSize, DevOrderStart);
        }
        else
        {
            pRS->SetVariable(
                L"UefiDevOrder", &gFixedBootOrderGuid,
                0, 0, NULL);
            pRS->SetVariable(
                L"OriUefiDevOrder", &gFixedBootOrderGuid,
                0, 0, NULL);
        }

    }

FreeAndExit:
    if (BootOrderFlag != NULL)
        pBS->FreePool(BootOrderFlag);
    if (DevOrderStart != NULL)
        pBS->FreePool(DevOrderStart);
    if (OldDevOrder != NULL)
        pBS->FreePool(OldDevOrder);
}

//
//
/**
    Build FboLegacyDevOrder variable from BootOptionList.
    This variable should be set before we disable the undesried boot
    options for selected boot mode.  So we can get the correct
    BootOptionList.Attributes reflecting the selection of setup.

    @param VOID

    @retval FboLegacyDevOrder variable updated

**/

VOID BuildFboLegacyDevOrder()
{
    // Due to BDS use different way to generate LegacyDevOrder between 
    // GROUP_BOOT_OPTIONS_BY_TAG enabled and disabled.
    // Using different way to handle it, FboLegacyDevOrder always follow 
    // GROUP_BOOT_OPTIONS_BY_TAG enabled style(same type device in one group).
#if CSM_SUPPORT
    UINTN DevOrderSize;
    LEGACY_DEVICE_ORDER *DevOrder, *DevOrderStart;
    LEGACY_DEVICE_ORDER *NewDevOrder, *NewDevOrderStart;
    UINT16 DeviceCount;
    UINT16 i, j;
    
    // return if no BootOption
    if(!BootOptionList->Size)
    {
        pRS->SetVariable(
            L"FboLegacyDevOrder", &gFixedBootOrderGuid,
            VARIABLE_ATTRIBUTES, 0, NULL
        );
        pRS->SetVariable(
            L"OriFboLegacyDevOrder", &gFixedBootOrderGuid,
            VARIABLE_ATTRIBUTES, 0, NULL
        );
        return;
    }
    
    SortList(BootOptionList, ComparePriorityThenBootOptionNumber);
    BuildLegacyDevOrderBuffer(&DevOrder, &DevOrderSize);

#if GROUP_BOOT_OPTIONS_BY_TAG == 0
    {
        LEGACY_DEVICE_ORDER *FboLegacyDevOrder, *Walker ;
        LEGACY_DEVICE_ORDER *FirstLoop, *SecondLoop ;
        UINTN Size = 0 ;
        
        Walker = FboLegacyDevOrder = MallocZ(DevOrderSize) ;
        
        for (FirstLoop = DevOrder
             ; (UINT8*)FirstLoop < (UINT8*)DevOrder + DevOrderSize
             ; FirstLoop = NEXT_DEVICE(LEGACY_DEVICE_ORDER, FirstLoop))
        {
            if (FirstLoop->Type == INVALID_FBO_TAG) continue ;
            
            i = 0 ;
            Walker->Type = FirstLoop->Type ;
            Walker->Length = sizeof(Walker->Length) + sizeof(Walker->Device) ;
            Walker->Device[i++] = FirstLoop->Device[0] ;   
            FirstLoop->Type = INVALID_FBO_TAG ; // mark to added
            
            for (SecondLoop = DevOrder
                 ; (UINT8*)SecondLoop < (UINT8*)DevOrder + DevOrderSize
                 ; SecondLoop = NEXT_DEVICE(LEGACY_DEVICE_ORDER, SecondLoop))
            {
                if (Walker->Type == SecondLoop->Type)
                {
                    Walker->Length += sizeof(Walker->Device) ;
                    Walker->Device[i++] = SecondLoop->Device[0] ;   
                    SecondLoop->Type = INVALID_FBO_TAG ; // mark to added
                }
            }
            Size += Walker->Length + sizeof(Walker->Type) ;
            Walker = NEXT_DEVICE(LEGACY_DEVICE_ORDER, Walker);
        }
        
        pBS->FreePool(DevOrder) ;
        DevOrder = FboLegacyDevOrder ;
        DevOrderSize = Size ; 
    }
#endif
    DevOrderStart = DevOrder;
    //
    // Crerate the NewDevOrder from DevOrder for changing order
    //
    NewDevOrderStart = NewDevOrder = MallocZ(DevOrderSize);
    if (NewDevOrder == NULL)
    {
        pBS->FreePool(DevOrderStart);
        return;
    }
    pBS->CopyMem(NewDevOrder, DevOrder, DevOrderSize);

    //
    // We need to keep the disabled ones at last
    //
    for (; (UINT8*)NewDevOrder < (UINT8*)NewDevOrderStart + DevOrderSize;)
    {
        j = 0;
        DeviceCount = DEVORDER_COUNT(NewDevOrder);

        //
        // Save the enabled ones first
        //
        for (i = 0; i < DeviceCount; i++)
        {
            if (DevOrder->Device[i] & FBO_LEGACY_DISABLED_MASK)
            {
                continue;
            }
            NewDevOrder->Device[j++] = DevOrder->Device[i];
        }
        //
        // Save the disabled ones later
        //
        for (i = 0; i < DeviceCount; i++)
        {
            if (!(DevOrder->Device[i] & FBO_LEGACY_DISABLED_MASK))
            {
                continue;
            }
            NewDevOrder->Device[j++] = DevOrder->Device[i];
        }

        //
        // Next
        //
        NewDevOrder = (LEGACY_DEVICE_ORDER*)((UINT8*)NewDevOrder + NewDevOrder->Length + sizeof(NewDevOrder->Type));
        DevOrder = (LEGACY_DEVICE_ORDER*)((UINT8*)DevOrder + DevOrder->Length + sizeof(DevOrder->Type));
    }

    pRS->SetVariable(
        L"FboLegacyDevOrder", &gFixedBootOrderGuid,
        VARIABLE_ATTRIBUTES, DevOrderSize, NewDevOrderStart
    );
    pRS->SetVariable(
        L"OriFboLegacyDevOrder", &gFixedBootOrderGuid,
        VARIABLE_ATTRIBUTES, DevOrderSize, NewDevOrderStart
    );

    pBS->FreePool(DevOrderStart);
    pBS->FreePool(NewDevOrderStart);
#endif  //#if CSM_SUPPORT
}

/**
    This function returns result of compairing two DLINK tags.

    @param 
        Link1 - pointer to the first tag for compare
        Link1 - pointer to the second tag for compare

    @retval INT32 result of comparing.

**/
INT32 FBO_CompareTagThenBbsIndex(IN DLINK *Link1, IN DLINK *Link2)
{
    BOOT_OPTION *Option1 = (BOOT_OPTION*)Link1;
    BOOT_OPTION *Option2 = (BOOT_OPTION*)Link2;

    // Compare Tags first
    if (Option1->Tag < Option2->Tag)
        return -1;
    else if (Option1->Tag > Option2->Tag)
        return 1;

    // Compare by BBS index
    if (Option1->BbsIndex < Option2->BbsIndex)
        return -1;
    else if (Option1->BbsIndex > Option2->BbsIndex)
        return 1;

    return 0;
}


/**
    Go through the master boot option list and create
    memory representation of the default uefi order variable

    @param DLIST *BootOptionList - the master boot option list

    @retval VOID

**/
VOID FBO_SetDefaultUEFIDevOrder(
)
{
    UINT16 i, j;
    DLINK *Link;
    BOOT_OPTION *Option;
    UEFI_DEVICE_ORDER *DevOrder = NULL, *DevOrder2 = NULL;
    UINT16 DeviceMapCount;

    if (gBdsSkipFBOModule) return ;
    // if no BootOption, clean DefaultUefiDevOrder and return this function.
    if (BootOptionList==NULL || !BootOptionList->Size)
    {
    	pRS->SetVariable(L"DefaultUefiDevOrder", &gFixedBootOrderGuid,0, 0, NULL);
    	return ;
    }
    
    DevOrder2 = DevOrder = MallocZ((BootOptionList->Size + 1) * sizeof(UEFI_DEVICE_ORDER));   // maximum order is number of boot options
    
    if (DevOrder == NULL)
        return;

    SortList(BootOptionList, FBO_CompareTagThenBbsIndex);

    DeviceMapCount = GetDevMapDataCount(pFBOUefiDevMap);
    for (i = 0; i < DeviceMapCount; i++)
    {
        UINT16 DevType;
        DevType = pFBOUefiDevMap[i].DevType;
        if (DevType == 0) break;     //end of data
        DevOrder->Type = DevType;
        DevOrder->Length = sizeof(DevOrder->Length);    // length includes itself

        j = 0;
        FOR_EACH_BOOT_OPTION(BootOptionList, Link, Option)
        {

            if (IsLegacyBootOption(Option) || Option->GroupHeader) continue;

            if (GetUefiBootOptionTag(Option) == DevType)
            {
                DevOrder->Length += sizeof(DevOrder->Device);
                DevOrder->Device[j] = Option->BootOptionNumber;

                j++;
            }
        }

        if (DevOrder->Length > sizeof(DevOrder->Length))    // any boot option?
            DevOrder = (UEFI_DEVICE_ORDER*)((UINT8*)DevOrder + DevOrder->Length + sizeof(DevOrder->Type));
    }

    {
        UINTN DevOrderSize;
        DevOrderSize = (UINT8*)DevOrder - (UINT8*)DevOrder2;

        if (DevOrderSize)
        {
            pRS->SetVariable(
                L"DefaultUefiDevOrder", &gFixedBootOrderGuid,
                VARIABLE_ATTRIBUTES, DevOrderSize, DevOrder2);
        }
        else
        {
            pRS->SetVariable(
                L"DefaultUefiDevOrder", &gFixedBootOrderGuid,
                0, 0, NULL);
        }

    }

    pBS->FreePool(DevOrder2);
}

/**
    Set legacy boot option priority for the DeviceType group.
    Set LOAD_OPTION_ACTIVE for group header because it is the
    boot#### option.

    @param DeviceType - Legacy Device type to check
        *Priority - Initial priority

    @retval *Priority updated to next one

**/
VOID SetLegacyBootOptionPriority(IN UINT16 DeviceType, IN OUT UINT32 *Priority)
{
    DLINK *Link;
    BOOT_OPTION *Option;
    BBS_BBS_DEVICE_PATH *BbsDp;


    // Set group header priority
    FOR_EACH_BOOT_OPTION(BootOptionList, Link, Option)
    {
        BbsDp = (BBS_BBS_DEVICE_PATH*)Option->FilePathList;
        if (BbsDp->DeviceType != DeviceType) continue;
        if (!Option->GroupHeader) continue;

        Option->Priority = *Priority;
        *Priority = GetNextBootOptionPriority(*Priority);
        Option->Attributes |= LOAD_OPTION_ACTIVE; // enable the group header

        DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[FBOSetOrder.c]Legacy Option->Priority=%x Name=%S\n", Option->Priority, Option->Description));
        break;
    }

    //
    // non-group header boot options: assign enabled first then disabled priorities
    //

    // Set priorities for enabled legacy boot options
    FOR_EACH_BOOT_OPTION(BootOptionList, Link, Option)
    {
        if (!IsLegacyBootOption(Option))
            continue;   // not legacy, skip

        if (Option->GroupHeader // group header?
                || !(Option->Attributes&LOAD_OPTION_ACTIVE))    // disabled?
            continue;

        // ENABLED, not group and legacy boot option here

        BbsDp = (BBS_BBS_DEVICE_PATH*)Option->FilePathList;
        if (BbsDp->DeviceType != DeviceType) continue;
        Option->Priority = *Priority;
        *Priority = GetNextBootOptionPriority(*Priority);
        DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[FBOSetOrder.c] ENABLED legacy Option->Priority==%x Name=%S\n", Option->Priority, Option->Description));
    }

    // Set priorities for disabled legacy boot options
    FOR_EACH_BOOT_OPTION(BootOptionList, Link, Option)
    {
        if (!IsLegacyBootOption(Option) // NOT legacy?
                || Option->GroupHeader
                || Option->Attributes&LOAD_OPTION_ACTIVE)    // enabled?
            continue;

        // DISABLED, not group and legacy boot option here

        BbsDp = (BBS_BBS_DEVICE_PATH*)Option->FilePathList;
        if (BbsDp->DeviceType != DeviceType) continue;
        Option->Priority = *Priority;
        *Priority = GetNextBootOptionPriority(*Priority);
        DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[FBOSetOrder.c] DISABLED legacy Option->Priority=%x Name=%S\n", Option->Priority, Option->Description));
    }

}

/**
    Set the BOOT_OPTION.Priority in BootOptionList according to
    DeviceType and UefiDevOrder variable.
    If the DeviceType is matched in UefiDevOrder then set all
    priorities within this group (DeviceType) from UefiDevOrder
    to BootOptionList.

    @param DeviceType - Uefi Device type
        Priority - pointer of Priority(UINT32).
        UefiDevOrder variable - contains priority within group

    @retval VOID

**/
VOID SetUefiBootOptionPriority(UINT16 DeviceType, UINT32 *Priority)
{
    UEFI_DEVICE_ORDER *DevOrder = NULL, *DevOrder2 = NULL;
    UINTN DevOrderSize = 0;
    DLINK *Link;
    BOOT_OPTION *Option;
    UINT16 j;

    GetEfiVariable(L"UefiDevOrder", &gFixedBootOrderGuid, NULL, &DevOrderSize, &DevOrder);

    DevOrder2 = DevOrder;

    for (DevOrder = DevOrder2
         ; (UINT8*)DevOrder < (UINT8*)DevOrder2 + DevOrderSize
         ; DevOrder = NEXT_DEVICE(UEFI_DEVICE_ORDER, DevOrder))
    {
        if (DeviceType != DevOrder->Type) continue;

        for (j = 0; j < DEVORDER_COUNT(DevOrder); j++)
        {
            DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[FBOSetOrder.c]DevOrder->Device[%d]=%x\n", j, DevOrder->Device[j]));

            FOR_EACH_BOOT_OPTION(BootOptionList, Link, Option)
            {
                if (IsLegacyBootOption(Option)) continue;
                if (Option->BootOptionNumber != (DevOrder->Device[j]&FBO_UEFI_ORDER_MASK))   // [31:24] = disabled flag
                    continue;

                // only set priority for active boot option
                if (Option->Attributes&LOAD_OPTION_ACTIVE)     // active?
                {
                    Option->Priority = *Priority;
                    *Priority = GetNextBootOptionPriority(*Priority);
                }

                DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[FBOSetOrder.c]UEFI BootOptionNumber=%x Priority=%x Name=%S\n", Option->BootOptionNumber, Option->Priority, Option->Description));
                break;
            }
        }
    }

    if (DevOrder2)
        pBS->FreePool(DevOrder2);
}


/**
    Some third-party utility will change BootOrder, so the function
    will follow the BootOrder sequence to adjust group priority.

    @param pSetupData - UefiPriorities/DualPriorities

    @retval VOID

**/
VOID
AdjustGroupPriorityByBootOrder(
    SETUP_DATA *pSetupData)
{
    UINT16 *BootOrder = NULL;
    UINTN Size = 0 ;
    UINT16 TypeTag[FIXED_BOOT_ORDER_GROUP_MAX_NUM], TagCount = 0 ;
    UINT16 DeviceMapCount = 0, i, j;
    DLINK *Link;
    BOOT_OPTION *Option;
    EFI_STATUS Status ;
    
    Status = GetEfiVariable(L"BootOrder", &gEfiGlobalVariableGuid, NULL, &Size, &BootOrder);
    if (EFI_ERROR(Status)) return ;
 
    if (pSetupData->BootMode == 1) // UEFI mode
    {
        for (i = 0 ; i < Size/sizeof(UINT16) ; i++)
        {
            FOR_EACH_BOOT_OPTION(BootOptionList, Link, Option)
            {
                UINT32 Tag ;
                if (IsLegacyBootOption(Option) || Option->BootOptionNumber != BootOrder[i]) 
                    continue ;
                Tag = GetUefiBootOptionTag(Option) ;
                for (j = 0 ; j < TagCount ; j++)
                    if (Tag == TypeTag[j]) break ;
                if (j == TagCount)
                {
                    TypeTag[TagCount++] = Tag ;
                }
            }
        }
        
        // Disable priorities if it have device...
        DeviceMapCount = GetDevMapDataCount(pFBOUefiDevMap);
        for (i = 0 ; i < TagCount ; i++)
        {
            for (j = 0 ; j < DeviceMapCount ; j++)
            {
                UINT16 Idx = SearchDevMapByType(pFBOUefiDevMap, TypeTag[i]) ;
                if (Idx != 0xffff && pSetupData->UefiPriorities[j] == Idx)
                {
                    pSetupData->UefiPriorities[j] = INVALID_FBO_TAG ;
                    break ;
                }
            }
            if (j == DeviceMapCount) TypeTag[i] = INVALID_FBO_TAG ; // Invalid tag
        }
        
        // Reset priorities
        for (i = 0 ; i < DeviceMapCount ; i++)
        {
            if (pSetupData->UefiPriorities[i] == INVALID_FBO_TAG)
                for (j = 0 ; j < TagCount ; j++)
                    if (TypeTag[j] != INVALID_FBO_TAG)
                    {
                        UINT16 Idx = SearchDevMapByType(pFBOUefiDevMap, TypeTag[j]) ;
                        if (Idx != 0xffff) pSetupData->UefiPriorities[i] = Idx ;
                        TypeTag[j] = INVALID_FBO_TAG ;
                        break ;
                    }
        }
    }
#if CSM_SUPPORT
    else if (pSetupData->BootMode == 0) // Legacy mode
    {
        // BootOrder variable can't changed in legacy mode, so we don't need handle it.
    }
#if FBO_DUAL_MODE
    else if (pSetupData->BootMode == 2) // Dual mode
    {
        for (i = 0 ; i < Size/sizeof(UINT16) ; i++)
        {
            FOR_EACH_BOOT_OPTION(BootOptionList, Link, Option)
            {
                UINT32 Tag ;
                if (Option->BootOptionNumber != BootOrder[i]) continue ;
                
                Tag = IsLegacyBootOption(Option) ? (0xff & Option->Tag) 
                                                 : GetUefiBootOptionTag(Option) ; 
                for (j = 0 ; j < TagCount ; j++)
                    if (Tag == TypeTag[j]) break ;
                if (j == TagCount)
                {
                    TypeTag[TagCount++] = Tag ;
                }
            }
        }
        // Disable priorities if it have device...
        DeviceMapCount = GetDevMapDataCount(pFBODualDevMap);
        for (i = 0 ; i < TagCount ; i++)
        {
            for (j = 0 ; j < DeviceMapCount ; j++)
            {
                UINT16 Idx = SearchDevMapByType(pFBODualDevMap, TypeTag[i]) ;
                if (Idx != 0xffff && pSetupData->DualPriorities[j] == Idx)
                {
                    pSetupData->DualPriorities[j] = INVALID_FBO_TAG ;
                    break ;
                }
            }
            if (j == DeviceMapCount) TypeTag[i] = INVALID_FBO_TAG ; // Invalid tag
        }
        
        // Reset priorities
        for (i = 0 ; i < DeviceMapCount ; i++)
        {
            if (pSetupData->DualPriorities[i] == INVALID_FBO_TAG)
                for (j = 0 ; j < TagCount ; j++)
                    if (TypeTag[j] != INVALID_FBO_TAG)
                    {
                        UINT16 Idx = SearchDevMapByType(pFBODualDevMap, TypeTag[j]) ;
                        if (Idx != 0xffff) pSetupData->DualPriorities[i] = Idx ;
                        TypeTag[j] = INVALID_FBO_TAG ;
                        break ;
                    }
        }
    }
#endif
#endif
    
    pBS->FreePool(BootOrder) ;
}

/**
    Get device option by BootOrder sequence
    Once matched between BootOrder and DevList, it will remove BootOrder[x].

    @param BootOrder    
    @param OrderCount   - BootOrder count
    @param DevList
    @param ListCount    - DevList count

    @retval DevList[index]

**/
UINT32
GetDevByBootOrderSequence(
    UINT16 *BootOrder,
    UINTN OrderCount,
    UINT32 *DevList,
    UINTN ListCount)
{
    UINT16 i, j ;
    
    for (i = 0 ; i < OrderCount ; i++)
    {
        for (j = 0 ; j < ListCount ; j++)
        {
            if (BootOrder[i] == INVALID_FBO_TAG || (DevList[j]&FBO_UEFI_DISABLED_MASK))
                continue ;
            if (BootOrder[i] == (UINT16)(DevList[j]&FBO_UEFI_ORDER_MASK))
            {
                UINT32 Option = DevList[j] ;
                BootOrder[i] = INVALID_FBO_TAG ;
                DevList[j] = FBO_UEFI_INVALID_ORDER ;
                return Option ;
            }
        }
    }
    return FBO_UEFI_INVALID_ORDER ;
}

/**
    Some third-party utility will change BootOrder, so the function
    will follow the BootOrder sequence to adjust UEFI device priority.

    @param VOID

    @retval VOID

**/
VOID
AdjustUefiDevPriorityByBootOrder()
{
    EFI_STATUS Status ;
    UEFI_DEVICE_ORDER *OriDevOrder = NULL, *DevOrder = NULL;
    // BOOLEAN FailFlag = FALSE ;
    UINT16 *BootOrder = NULL ;
    UINT32 DevList[FIXED_BOOT_ORDER_SUBMENU_MAX_NUM], VarAttr = 0 ;
    UINTN DevSize = 0, OrderSize = 0  ;
    UINTN i, j, ListCount, OrderCount ;
    
    
    // Get necessary variable
    Status = GetEfiVariable(L"BootOrder", &gEfiGlobalVariableGuid, NULL, &OrderSize, &BootOrder);
    if (EFI_ERROR(Status)) return ;
    
    Status = GetEfiVariable(L"UefiDevOrder", &gFixedBootOrderGuid, &VarAttr, &DevSize, &OriDevOrder);
    if (EFI_ERROR(Status))
    {
        pBS->FreePool(BootOrder) ;
        return ;
    }
    
    OrderCount = OrderSize/sizeof(UINT16) ;
    
    // Resort UefiDevOrder
    for (DevOrder = OriDevOrder
         ; ((UINT8*)DevOrder < (UINT8*)OriDevOrder + DevSize)
         ; DevOrder = NEXT_DEVICE(UEFI_DEVICE_ORDER, DevOrder))
    {
        MemCpy(DevList, DevOrder->Device, DevOrder->Length-sizeof(DevOrder->Length)) ;
        ListCount = DEVORDER_COUNT(DevOrder) ;
        
        // Resort DevOrder->Device[i]
        for (i = 0 ; i < ListCount ; i++)
        {
            UINT32 Option = GetDevByBootOrderSequence(BootOrder, OrderCount, DevList, ListCount) ;

            // if Option == INVALID ORDER, means BootOrder doesn't match DevList totally.
            if (Option == FBO_UEFI_INVALID_ORDER) break ;       
            DevOrder->Device[i] = Option ; 
        }
        
        // When BootOptionList have a new UEFI device, the device(BootXXXX) doesn't restore in BootOrder in this moment.
        // So we need handle it and add into DevOrder->Device[i].
        for (j = 0 ; j < ListCount ; j++)
        {
            if (!(DevList[j]&FBO_UEFI_DISABLED_MASK) && DevList[j] != FBO_UEFI_INVALID_ORDER)
            {
                DevOrder->Device[i++] = DevList[j] ;
                DevList[j] = FBO_UEFI_INVALID_ORDER ;
            }
        }
        // Handle disable device
        for (j = 0 ; j < ListCount ; j++)
        {
            if (DevList[j] != FBO_UEFI_INVALID_ORDER)
            {
                DevOrder->Device[i++] = DevList[j] ;
                DevList[j] = FBO_UEFI_INVALID_ORDER ;
            }
        }
    }
    
    pRS->SetVariable(L"UefiDevOrder", 
                     &gFixedBootOrderGuid,
                     VarAttr, 
                     DevSize, 
                     OriDevOrder);
    pRS->SetVariable(L"OriUefiDevOrder", 
                     &gFixedBootOrderGuid,
                     VARIABLE_ATTRIBUTES, 
                     DevSize, 
                     OriDevOrder);
    
    pBS->FreePool(BootOrder) ;
    pBS->FreePool(OriDevOrder) ;
}

/**
    use setup data LegacyPriorities/UefiPriorities/DualPriorities
    ,set legacy/uefi boot option list priority.

    @param VOID

    @retval VOID

**/
VOID FBO_UpdateBootListPriority()
{
    EFI_STATUS Status;
    EFI_GUID gSetupGuid = SETUP_GUID;
    SETUP_DATA *pSetupData = NULL;
    UINT32 SetupAttr;
    UINTN SetupSize = 0;
    DLINK *Link;
    BOOT_OPTION *Option;
    UINT16 i; //,j;
    UINT16 DeviceMapCount = 0;
    UINT16 TypeIndex;
    UINT32 PreviousPriority = IndexToBootPriority(0);

    if (gBdsSkipFBOModule) return ;
//(EIP99368+)>>
    FOR_EACH_BOOT_OPTION(BootOptionList, Link, Option)
    {
        if (IsLegacyBootOption(Option)) continue;
        if (!Wcscmp(Option->Description, L"USB Entry for Windows To Go Logo Test"))
            return;
    }
//<<(EIP99368+)

    BuildUEFIDevOrder();
#if ADJUST_DEVICE_PRIORITY_BY_BOOTORDER
    AdjustUefiDevPriorityByBootOrder() ;
#endif
    
    Status = GetEfiVariable(L"Setup", &gSetupGuid, &SetupAttr, &SetupSize, &pSetupData);

    if (EFI_ERROR(Status)) return;
    
#if ADJUST_GROUP_PRIORITY_BY_BOOTORDER
    AdjustGroupPriorityByBootOrder(pSetupData) ;
    pRS->SetVariable(L"Setup", &gSetupGuid, SetupAttr, SetupSize, pSetupData);
#endif
    
#if FBO_DUAL_MODE
    if (pSetupData->BootMode == 2)  //Is Dual boot mode?
    {
        //-----------------------------------------------------
        //Search Legacy Group Header and Set is disable
        FOR_EACH_BOOT_OPTION(BootOptionList, Link, Option)
        {
            Option->Priority = LOWEST_BOOT_OPTION_PRIORITY;
            Option->Attributes &= ~LOAD_OPTION_HIDDEN;
            if (IsLegacyBootOption(Option))                                             //(EIP114186+)
            {
                // FboLegacyDevOrder does not have group header disabled flag
                // Disable first and enable it later in SetLegacyBootOptionPriority()
                if (Option->GroupHeader)
                    Option->Attributes &= ~LOAD_OPTION_ACTIVE;
            }
        }
        //-----------------------------------------------------
        DeviceMapCount = GetDevMapDataCount(pFBODualDevMap);
        for (i = 0; i < DeviceMapCount; i++)
        {
            TypeIndex = pSetupData->DualPriorities[i];
            if (!pFBODualDevMap[TypeIndex].BBSType)          //0, Uefi device
            {
                DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[FBOSetOrder.c]DualPriorities[%d] pFBODualDevMap[%d].DevType=%x\n", i, TypeIndex, pFBODualDevMap[TypeIndex].DevType));
                SetUefiBootOptionPriority(pFBODualDevMap[TypeIndex].DevType, &PreviousPriority);
            }
            else
            {
                if (pFBODualDevMap[TypeIndex].DevType == 0) continue;    //Disable?      //(EIP114186+)
                DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[FBOSetOrder.c]DualPriorities[%d] pFBODualDevMap[%d].DevType=%x\n", i, TypeIndex, pFBODualDevMap[TypeIndex].DevType));
                SetLegacyBootOptionPriority(pFBODualDevMap[TypeIndex].DevType, &PreviousPriority);
            }
        }

        // Disabled all boot options for disabled group
        // Disabled group's pFBOUefiDevMap[TypeIndex].DevType=0
        // so that no priority was assigned in SetUefiBootOptionPriority
        FOR_EACH_BOOT_OPTION(BootOptionList, Link, Option)
        {
            if (!IsLegacyBootOption(Option) // UEFI?
                    && Option->Priority == LOWEST_BOOT_OPTION_PRIORITY && IsBootOptionWithDevice(Option)) // AND priority is not set?
                Option->Attributes &= ~LOAD_OPTION_ACTIVE;          // disable undesired boot options
        }
    }
    else
#endif
        if (pSetupData->BootMode == 1)
        {
            //
            // UEFI mode
            //

            //-----------------------------------------------------
            // Set all boot options to lowest priority first, arrange them later
            FOR_EACH_BOOT_OPTION(BootOptionList, Link, Option)
            {
                Option->Priority = LOWEST_BOOT_OPTION_PRIORITY; // reset all priorities

                if (!IsLegacyBootOption(Option))
                    Option->Attributes &= ~LOAD_OPTION_HIDDEN;  // clear HIDDEN for all UEFI boot options
            }
            //-----------------------------------------------------
            // Set group and device priorities according to setup
            DeviceMapCount = GetDevMapDataCount(pFBOUefiDevMap);
            for (i = 0; i < DeviceMapCount; i++)
            {
                TypeIndex = pSetupData->UefiPriorities[i];

                DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[FBOSetOrder.c]UefiPriorities[%d] pFBOUefiDevMap[%d].DevType=%x\n", i, TypeIndex, pFBOUefiDevMap[TypeIndex].DevType));
                SetUefiBootOptionPriority(pFBOUefiDevMap[TypeIndex].DevType, &PreviousPriority);
            }

            // Disabled all boot options for disabled group
            // Disabled group's pFBOUefiDevMap[TypeIndex].DevType=0
            // so that no priority was assigned in SetUefiBootOptionPriority
            FOR_EACH_BOOT_OPTION(BootOptionList, Link, Option)
            {
                if (!IsLegacyBootOption(Option) // UEFI?
                        && Option->Priority == LOWEST_BOOT_OPTION_PRIORITY) // AND priority is not set?
                    Option->Attributes &= ~LOAD_OPTION_ACTIVE;          // disable undesired boot options
            }
            //-----------------------------------------------------
        }
        else if (pSetupData->BootMode == 0)
        {
            //
            // Legacy mode
            //

            //-----------------------------------------------------
            // Set all boot options to lowest priority first, arrange them later
            FOR_EACH_BOOT_OPTION(BootOptionList, Link, Option)
            {
                Option->Priority = LOWEST_BOOT_OPTION_PRIORITY; // reset all priorities
                if (IsLegacyBootOption(Option))
                {
                    Option->Attributes &= ~LOAD_OPTION_HIDDEN;  // clear HIDDEN for all legacy boot options

                    // FboLegacyDevOrder does not have group header disabled flag
                    // Disable first and enable it later in SetLegacyBootOptionPriority()
                    if (Option->GroupHeader)
                        Option->Attributes &= ~LOAD_OPTION_ACTIVE;
                }
            }

            DeviceMapCount = GetDevMapDataCount(pFBOLegacyDevMap);
            DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[FBOSetOrder.c] DeviceMapCount=%x\n", DeviceMapCount));
            for (i = 0; i < DeviceMapCount; i++)
            {
                TypeIndex = pSetupData->LegacyPriorities[i];
                if (pFBOLegacyDevMap[TypeIndex].DevType == 0) continue;      //Disable?      //(EIP114186+)
                SetLegacyBootOptionPriority(pFBOLegacyDevMap[TypeIndex].DevType, &PreviousPriority);
            }
        }

    // Set Legacy priorities
    //
    // We need to set legacy priorities because of SetDefaultTseBootOrder function would sort
    // the boot options with CompareTagThenBbsIndex then legacy boot options were re-ordered
    // according to BbsIndex, if without priorities, we can't sort them into
    // ComparePriorityThenBootOptionNumber sequence in SaveBootOptions function and that casued
    // we set the wrong attributes to options by looking at OldLegacyDevOrder variable.
    FOR_EACH_BOOT_OPTION(BootOptionList, Link, Option)
    {
        if (!IsLegacyBootOption(Option)
                || Option->Priority != LOWEST_BOOT_OPTION_PRIORITY ) continue;

        Option->Priority = PreviousPriority | BIT00; // BIT00: Flag which indicates it should remove ACTIVE attribute later since it is enclosed by disabled group.
        PreviousPriority = GetNextBootOptionPriority(PreviousPriority);
    }

    BuildFboLegacyDevOrder();
#if !GROUP_BOOT_OPTIONS_BY_TAG
    FOR_EACH_BOOT_OPTION(BootOptionList, Link, Option)
    {
        if(Option->Priority & BIT00)
            Option->Attributes &= ~LOAD_OPTION_ACTIVE;
    }
#endif
    //
    // Undesried boot options should be not able to boot (ACTIVE=0)
    // This code must not be executed before BuildFboLegacyDevOrder
    // in order to keep the FboLegacyDevOrder has the same disabled information as setup
    //
    FOR_EACH_BOOT_OPTION(BootOptionList, Link, Option)
    {
        UINT16 LegacyFlag = IsLegacyBootOption(Option);

        if (pSetupData->BootMode == 2  // Dual mode?
                || (pSetupData->BootMode == 0 && LegacyFlag)    // legacy mode and legacy option?
                || (pSetupData->BootMode == 1 && !LegacyFlag)   // UEFI mode and UEFI option?
           )
        {
            // option should be ACTIVE to boot and NOT HIDDEN for TSE
            Option->Attributes &= ~LOAD_OPTION_HIDDEN;  // clear HIDDEN for all legacy boot options
        }
        else
        {
            // undesried boot options for selected boot mode
            // HIDDEN for TSE not sees it
            // NOT ACTIVE for not booting from it
            Option->Attributes |= LOAD_OPTION_HIDDEN;   // HIDDEN
            Option->Attributes &= ~LOAD_OPTION_ACTIVE;  // Disable undesired UEFI boot options

        }
    }

    PreProcessBootOptions();
    pBS->FreePool(pSetupData);

    DUMP_BOOT_OPTION_LIST(BootOptionList, "After UpdateBootListPriority");
}

/**
    Backup variable "BootOrder" to "OldBootOrder"
    OldBootOrder is used in CheckBootOrder() to check if any
    BootOrder changed between each boot

    @param VOID

    @retval VOID

**/
EFI_STATUS CreateOldBootOrder()
{
    EFI_STATUS Status;
    UINT16 *BootOrder = NULL;
    UINTN BootOrderSize = 0;

    if (gBdsSkipFBOModule) return EFI_SUCCESS;
    Status = GetEfiVariable(
                 L"BootOrder", &EfiVariableGuid, NULL, &BootOrderSize, &BootOrder
             );

    if (!EFI_ERROR(Status))
    {
        pRS->SetVariable(
            L"OldBootOrder", &gFixedBootOrderGuid,
            VARIABLE_ATTRIBUTES, BootOrderSize, BootOrder
        );
    }
    else
    {
        pRS->SetVariable(
            L"OldBootOrder", &gFixedBootOrderGuid,
            VARIABLE_ATTRIBUTES, 0, NULL
        );
    }

    return Status;
}

/**
    Set Windows To Go Boot Option to Frist Priority


    @param VOID

    @retval VOID

**/
void FBO_AdjustWtgPriority()
{
    EFI_STATUS  Status;
    UINT16      *BootOrder = NULL;
    UINTN       BootOrderSize = 0;
    UINT32      SetupAttr;
    UINTN       SetupSize = 0;
    SETUP_DATA  *pSetupData = NULL;
    DLINK       *Link;
    BOOT_OPTION *Option;
    EFI_GUID    gSetupGuid = SETUP_GUID;
    UINT16      i, TypeIndex;
    UINT16      DeviceMapCount = 0;
    BOOLEAN     UpdateSetup = FALSE, WindowsToGoFound = FALSE;

    if (gBdsSkipFBOModule) return ;
    Status = GetEfiVariable(L"Setup", &gSetupGuid, &SetupAttr, &SetupSize, &pSetupData);
    if (EFI_ERROR(Status))
        return;

    // Search for Windwos To Go Boot Option
    FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option)
    {
        if (IsLegacyBootOption(Option))
            continue;

        // Found !!
        if (!Wcscmp(Option->Description,L"USB Entry for Windows To Go"))	//if WTG?
        {
            Status=GetEfiVariable(
                       L"BootOrder",
                       &EfiVariableGuid,
                       NULL,
                       &BootOrderSize,
                       &BootOrder);

            if (EFI_ERROR(Status))
            {
                pBS->FreePool( pSetupData );
                return;
            }

            // There are 2 Options in "Windows To Go Startup Options"
            // Choose Yes, Windows will set Windows to Go BootOption to Frist Priority in BootOrder.
            // Choose No,  User Need to change PC's Firmware Setting.
            // So, We Only handle the "Yes" Option

            if (BootOrder[0] != Option->BootOptionNumber)
            {
                pBS->FreePool( pSetupData );
                pBS->FreePool( BootOrder );
                return;
            } else
            {
                WindowsToGoFound = TRUE;
                break;
            }
        }
    }
    if (!WindowsToGoFound)
    {
        UINT16 *FboUefiPriority = NULL, *FboDualPriority = NULL ;
        UINTN UefiSize = 0, DualSize = 0;

        Status = GetEfiVariable(L"WtgUefiPriority", &gFixedBootOrderGuid, NULL, &UefiSize, &FboUefiPriority);
        if (!EFI_ERROR(Status))
        {
            pBS->CopyMem(pSetupData->UefiPriorities, FboUefiPriority, UefiSize);
            pRS->SetVariable(L"Setup", &gSetupGuid, SetupAttr, SetupSize, pSetupData);
            pRS->SetVariable(L"WtgUefiPriority", &gFixedBootOrderGuid, SetupAttr, 0, NULL);
            pBS->FreePool(FboUefiPriority) ;
        }
#if FBO_DUAL_MODE
        Status = GetEfiVariable(L"WtgDualPriority", &gFixedBootOrderGuid, NULL, &DualSize, &FboDualPriority);
        if (!EFI_ERROR(Status))
        {
            pBS->CopyMem(pSetupData->DualPriorities, FboDualPriority, DualSize);
            pRS->SetVariable(L"Setup", &gSetupGuid, SetupAttr, SetupSize, pSetupData);
            pRS->SetVariable(L"WtgDualPriority", &gFixedBootOrderGuid, SetupAttr, 0, NULL);
            pBS->FreePool(FboDualPriority) ;
        }
#endif
        pBS->FreePool( pSetupData );
        return;
    }

#if FBO_DUAL_MODE
    TypeIndex = SearchDevMapByType( pFBODualDevMap,GetUefiBootOptionTag(Option));
    DeviceMapCount=GetDevMapDataCount( pFBODualDevMap );
    {
        UINT16 *FboDualPriority = NULL;
        UINTN Size = 0;
        Status = GetEfiVariable(L"WtgDualPriority", &gFixedBootOrderGuid, NULL, &Size, &FboDualPriority);
        if (EFI_ERROR(Status))
        {
            pRS->SetVariable(L"WtgDualPriority",
                     &gFixedBootOrderGuid,
                     SetupAttr,
                     sizeof(UINT16)*DeviceMapCount,
                     &pSetupData->DualPriorities[0]);
        }
        pBS->FreePool(FboDualPriority) ;
    }
    for (i=0; i<DeviceMapCount; i++)
    {
        if (TypeIndex == pSetupData->DualPriorities[i])
        {
            UINT16 j;
            UINT16 FirstPriority = pSetupData->DualPriorities[i];

            for (j=i; j>0; j--)
                pSetupData->DualPriorities[j] = pSetupData->DualPriorities[j-1];

            pSetupData->DualPriorities[0] = FirstPriority;
            UpdateSetup=TRUE;
            break;
        }
    }
#endif
    TypeIndex = SearchDevMapByType( pFBOUefiDevMap,GetUefiBootOptionTag(Option));
    DeviceMapCount = GetDevMapDataCount( pFBOUefiDevMap );
    {
        UINT16 *FboUefiPriority = NULL;
        UINTN Size = 0;
        Status = GetEfiVariable(L"WtgUefiPriority", &gFixedBootOrderGuid, NULL, &Size, &FboUefiPriority);
        if (EFI_ERROR(Status))
        {
            pRS->SetVariable(L"WtgUefiPriority",
                     &gFixedBootOrderGuid,
                     SetupAttr,
                     sizeof(UINT16)*DeviceMapCount,
                     &pSetupData->UefiPriorities[0]);
        }
        pBS->FreePool(FboUefiPriority) ;
    }
    for (i = 0 ; i < DeviceMapCount ; i++)
    {
        if (TypeIndex == pSetupData->UefiPriorities[i])
        {
            UINT16 j;
            UINT16 FirstPriority = pSetupData->UefiPriorities[i];

            for (j = i ; j > 0 ; j--)
                pSetupData->UefiPriorities[j] = pSetupData->UefiPriorities[j-1];

            pSetupData->UefiPriorities[0] = FirstPriority;
            UpdateSetup=TRUE;
            break;
        }
    }

    if (UpdateSetup)
        Status = pRS->SetVariable(L"Setup",&gSetupGuid, SetupAttr, SetupSize, pSetupData);

    pBS->FreePool( pSetupData );
    pBS->FreePool( BootOrder );

    return;
}

/**
    Initialize the module memory, and device map data.

    @param VOID

    @retval VOID

**/
EFI_STATUS FBO_Init(IN void)
{
    EFI_STATUS Status;
    EFI_HANDLE Handle = NULL;
    EFI_FIXED_BOOT_ORDER_PROTOCOL *pFBO = NULL;
    EFI_GUID FboBdsGuid = FIXED_BOOT_ORDER_BDS_GUID ;

    if (gBdsSkipFBOModule) return EFI_SUCCESS;
    Status = pBS->LocateProtocol(&gFixedBootOrderGuid, NULL, &pFBO);

    if (!EFI_ERROR(Status))
    {
        pFBOLegacyDevMap = pFBO->GetLegacyDevMap();
        pFBOUefiDevMap = pFBO->GetUefiDevMap();
#if FBO_DUAL_MODE
        pFBODualDevMap = pFBO->GetDualDevMap();
#endif
    }

    // install FixedBootOrderBdsProtocol
    Status = pBS->InstallProtocolInterface(
                 &Handle,
                 &FboBdsGuid,
                 EFI_NATIVE_INTERFACE,
                 &gFixedBootOrderBdsProtocol);
    
    FboSetupSizeCheck();

    return Status;
}

/**
    Go through the legacy device order structure and find the legacy dev order
    group type that we want then return this group.

    @param LEGACY_DEVICE_ORDER *DevOrder - pointer to the legacy device order
        UINTN DevOrderSize - size of the legacy dev order structure
        UINT32 Type - Group type to match

    @retval LEGACY_DEVICE_ORDER *

**/
#if CSM_SUPPORT
LEGACY_DEVICE_ORDER* FindLegacyDeviceGroupByType(
    LEGACY_DEVICE_ORDER *DevOrder, UINTN DevOrderSize, UINT32 Type
)
{
    LEGACY_DEVICE_ORDER *LegacyGroup = DevOrder;

    for (; (UINT8*)LegacyGroup < (UINT8*)DevOrder + DevOrderSize
            ; LegacyGroup = NEXT_DEVICE(LEGACY_DEVICE_ORDER, LegacyGroup))
    {
        if (LegacyGroup->Type == Type) {
            return LegacyGroup;
        }
    }

    return NULL;
}
#endif

/**
    UefiDevOrder and FboLegacyDevOrder variables contain the
    disabled flags saved by setup.  Need to update the disabled
    flag to BootOptionList right before any other functions
    that will process the BootOptionList.
    BootOptionList will be set to NOT ACTIVE for disabled
    DevOrder, ACTIVE for enabled one.

    @param VOID

    @retval VOID

**/
VOID
FboSyncBootOptionsFromDevOrder()
{
    UEFI_DEVICE_ORDER *UefiOrder = NULL, *UefiGroup = NULL;
    LEGACY_DEVICE_ORDER *LegacyOrder = NULL, *LegacyGroup = NULL ;
    UINTN DevOrderSize = 0, LegacyDevSize = 0;
    DLINK *Link;
    BOOT_OPTION *Option;
    INT32 j;
    EFI_STATUS Status;
    UINT16 DeviceCount;

    if (gBdsSkipFBOModule) return ;
    //
    // Set ACTIVE information according to UefiDevOrder
    //
    Status = GetEfiVariable(L"UefiDevOrder", &gFixedBootOrderGuid, NULL, &DevOrderSize, &UefiOrder);
    DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[fbo] Get UefiDevOrder: %r\n", Status));
    if (!EFI_ERROR(Status))
    {
        for (UefiGroup = UefiOrder
             ; (UINT8*)UefiGroup < (UINT8*)UefiOrder + DevOrderSize
             ; UefiGroup = NEXT_DEVICE(UEFI_DEVICE_ORDER, UefiGroup))
        {
            DeviceCount = DEVORDER_COUNT(UefiGroup);

            // loop through boot option numbers within this group
            for (j = 0; j < DeviceCount; j++)
            {
                // set up Option->Attributes.LOAD_OPTION_ACTIVE
                FOR_EACH_BOOT_OPTION(BootOptionList, Link, Option)
                {
                    if (IsLegacyBootOption(Option))
                        continue;

                    if ((UefiGroup->Device[j]&FBO_UEFI_ORDER_MASK) == Option->BootOptionNumber)  // [31:24] = disabled flag
                    {
                        DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[fbo] %S.Device[%d] = %x\n", Option->Description, j, UefiGroup->Device[j]));
                        if (UefiGroup->Device[j] & FBO_UEFI_DISABLED_MASK)  // disabled?
                            Option->Attributes &= ~LOAD_OPTION_ACTIVE;
                        else
                            Option->Attributes |= LOAD_OPTION_ACTIVE;
                    }
                }
            }
        }

    }
    
    if (UefiOrder != NULL)
        pBS->FreePool(UefiOrder);

#if CSM_SUPPORT
    {
        //
        // Set ACTIVE information according to FboLegacyDevOrder
        //
    
        // The flow of this FboLegacyDevOrder variable:
        // 1. Updated the BootOptionList.Attributes from FboLegacyDevOrder right after boot options are
        //    read from NVRAM in FboSyncBootOptionsFromDevOrder() to reflect the setup controls.
        // 2. Attributes will be changed to HIDDEN and NOT ACTIVE according to boot mode in FBO_UpdateBootListPriority().
        // 3. Save the disabled information here for setup to use before we modifying the Attributes later
        //    in FBO_UpdateBootListPriority().
        // 4. OldLegacyDevOrder variable indicates the sequence of legacy boot options by group in BootOptionList
        //    after reading from NVRAM, we need it to compare with FboLegacyDevOrder variable to find out options
        //    should set ACTIVE attribute or INACTIVE attribute.
        
        LEGACY_DEVICE_ORDER *LegacyOrder = NULL, *LegacyGroup = NULL;
        LEGACY_DEVICE_ORDER *OldLegacyOrder = NULL, *OldLegacyGroup = NULL;
        UINTN OldDevOrderSize = 0;
    
        Status = GetEfiVariable(L"FboLegacyDevOrder", &gFixedBootOrderGuid, NULL, &DevOrderSize, &LegacyOrder);
        DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[fbo] Get FboLegacyDevOrder: %r\n", Status));
    
        if (!EFI_ERROR(Status))
        {
            Status = GetEfiVariable(L"OriFboLegacyDevOrder", &gFixedBootOrderGuid, NULL, &OldDevOrderSize, &OldLegacyOrder);
            DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[fbo] Get OldLegacyDevOrder: %r\n", Status));
    
            if (!EFI_ERROR(Status))
            {
                INT32 DevCount[FBO_GROUP_TYPE_MAX_NUM] = {0};
                UINT32 PreviousType = 0;

                FOR_EACH_BOOT_OPTION(BootOptionList, Link, Option)
                {
                    UINT32 DevType;
                    INT32  DevIndex;

                    if (!IsLegacyBootOption(Option) || Option->GroupHeader) continue;

                    if (!Option->BbsEntry) 
                    {
                        //
                        //  orphan option
                        //
                        EFI_DEVICE_PATH *Dp;
                        EFI_GUID FboBoTagDpGuid = FBO_BO_TAG_DEVICE_PATH_GUID;

                        DevType = BoTagLegacyOther;
                        Dp = Option->FilePathList;
                        do{
                            if(Dp->Type == HARDWARE_DEVICE_PATH
                               && Dp->SubType == HW_VENDOR_DP)
                            {
                               FBO_BO_TAG_DEVICE_PATH *FboTagDp= (FBO_BO_TAG_DEVICE_PATH*)Dp;
                               if (!MemCmp(&FboTagDp->Header.Guid, &FboBoTagDpGuid, sizeof(EFI_GUID)))
                               {
                                    DevType = FboTagDp->BoTag;
                                    break;
                               }
                            }
                        }while(!isEndNode(Dp) && (Dp = NEXT_NODE(Dp)));
                    }
                    else
                    {
                        DevType = BOOT_OPTION_GET_BBS_ENTRY_DEVICE_TYPE_FUNCTION(Option->BbsEntry); 
                    }

                    if (PreviousType != DevType)
                    {
                        PreviousType = DevType;
                        LegacyGroup = FindLegacyDeviceGroupByType(LegacyOrder, DevOrderSize, DevType);
                        OldLegacyGroup = FindLegacyDeviceGroupByType(OldLegacyOrder, OldDevOrderSize, DevType);
                    }

                    if (LegacyGroup == NULL || OldLegacyGroup == NULL) continue;
    
                    j = DevCount[DevType]++;
#if GROUP_BOOT_OPTIONS_BY_TAG
                    DevIndex = FindPositionInTheGroupByBbsIndex(LegacyGroup, OldLegacyGroup->Device[j]);
#else
                    DevIndex = j;
#endif
                    // FixedBootOrderSaveChange function doesn't save FboLegacyDevOrder variable as 
                    // group sequence we change in Setup, and that cause AdjustLegacyBootOptionPriorities
                    // function update the wrong Priority to Option, hence, we disable 
                    // AdjustLegacyBootOptionPriorities function by SDL and update correct priority here.
                    Option->Priority += (DevIndex - j) * DEFAULT_PRIORITY_INCREMENT;
                    j++;
    
                    if (LegacyGroup->Device[DevIndex] & FBO_LEGACY_DISABLED_MASK)
                    {
                        Option->Attributes &= ~LOAD_OPTION_ACTIVE;
                    }
                    else
                    {
                        Option->Attributes |= LOAD_OPTION_ACTIVE;
                    }
                }
            }
        }
    
        if (OldLegacyOrder != NULL)
            pBS->FreePool(OldLegacyOrder);
        if (LegacyOrder != NULL)
            pBS->FreePool(LegacyOrder);
    }

#endif  //#if CSM_SUPPORT
}

/**
    Adjust Ethernet default priority (IPV4 and IPV6)
    Token ETHERNET_DEFAULT_PRIORITY will control ipv4 and ipv6 in network group.
    ETHERNET_DEFAULT_PRIORITY is 0, ipv4 at the front.
    ETHERNET_DEFAULT_PRIORITY is 1, ipv6 at the front.
    
    @param VOID

    @retval VOID

**/
VOID
FboAdjustEthernetDefaultPriority()
{
    UINT8 EthernetDefaultPriorityPolicy = FBO_ETHERNET_DEFAULT_PRIORITY_POLICY;
    EFI_HANDLE *EthernetHandle ;
    UINTN HandleCount = 0, x ;
    EFI_STATUS Status;
    BOOT_DEVICE *Device;
    DLINK *Link;

    if (!BootDeviceList->Size) return ;
    EthernetHandle = MallocZ(BootDeviceList->Size * sizeof(EFI_HANDLE)) ;
    
    FOR_EACH_BOOT_DEVICE(BootDeviceList, Link, Device)
    {
        EFI_DEVICE_PATH_PROTOCOL *Dp = NULL;
        
        // Skip legacy boot device.
        if (Device->BbsIndex != INVALID_BBS_INDEX
            || Device->BbsEntry) continue;
        // Get device path
        Status = pBS->HandleProtocol(Device->DeviceHandle, 
                                     &gEfiDevicePathProtocolGuid, 
                                     &Dp);
        if (EFI_ERROR(Status)) continue ;
        
        // Skip not Ethernet device .
        for ( ; !(isEndNode(Dp)); Dp = NEXT_NODE(Dp))
        {
            if (Dp->Type == MESSAGING_DEVICE_PATH
                &&(Dp->SubType == MSG_IPv4_DP || Dp->SubType == MSG_IPv6_DP))
            {
                if (!EthernetDefaultPriorityPolicy && Dp->SubType == MSG_IPv4_DP)
                {
                    EthernetHandle[HandleCount] = Device->DeviceHandle ;
                    DeleteBootDevice(BootDeviceList, Device);
                    HandleCount++ ;
                }
                else if (EthernetDefaultPriorityPolicy && Dp->SubType == MSG_IPv6_DP)
                {
                    EthernetHandle[HandleCount] = Device->DeviceHandle ;
                    DeleteBootDevice(BootDeviceList, Device);
                    HandleCount++ ;
                }
            }
        } 
    }
    
    for ( x=0 ; x<HandleCount ; x++)
        CreateBootDevice(BootDeviceList,EthernetHandle[x],INVALID_BBS_INDEX,NULL);
    HandleCount = 0 ;
    
    FOR_EACH_BOOT_DEVICE(BootDeviceList, Link, Device)
    {
        EFI_DEVICE_PATH_PROTOCOL *Dp = NULL;
        
        // Skip legacy boot device.
        if (Device->BbsIndex != INVALID_BBS_INDEX
            || Device->BbsEntry) continue;
        // Get device path
        Status = pBS->HandleProtocol(Device->DeviceHandle, 
                                     &gEfiDevicePathProtocolGuid, 
                                     &Dp);
        if (EFI_ERROR(Status)) continue ;
        
        // Skip not Ethernet device .
        for ( ; !(isEndNode(Dp)); Dp = NEXT_NODE(Dp))
        {
            if (Dp->Type == MESSAGING_DEVICE_PATH
                &&(Dp->SubType == MSG_IPv4_DP || Dp->SubType == MSG_IPv6_DP))
            {
                if (!EthernetDefaultPriorityPolicy && Dp->SubType == MSG_IPv6_DP)
                {
                    EthernetHandle[HandleCount] = Device->DeviceHandle ;
                    DeleteBootDevice(BootDeviceList, Device);
                    HandleCount++ ;
                }
                else if (EthernetDefaultPriorityPolicy && Dp->SubType == MSG_IPv4_DP)
                {
                    EthernetHandle[HandleCount] = Device->DeviceHandle ;
                    DeleteBootDevice(BootDeviceList, Device);
                    HandleCount++ ;
                }
            }
        } 
    } 
    
    for ( x=0 ; x<HandleCount ; x++)
        CreateBootDevice(BootDeviceList,EthernetHandle[x],INVALID_BBS_INDEX,NULL);
    pBS->FreePool(EthernetHandle) ;
}

/**
    Build a Device Path Contains BoTag

    @param VOID

    @retval VOID

**/
VOID
FboBuildTagPath
(
    IN  BOOT_OPTION *Option, 
    IN  UINT16      BoTag
)
{
    static FBO_BO_TAG_DEVICE_PATH FboBoTagDpTemplate = {
        { 
            { HARDWARE_DEVICE_PATH, HW_VENDOR_DP, sizeof(FBO_BO_TAG_DEVICE_PATH) },
            FBO_BO_TAG_DEVICE_PATH_GUID
        },
        0
    };
    static EFI_DEVICE_PATH_PROTOCOL EndOfDevicePathNode = {
        END_DEVICE_PATH, END_ENTIRE_SUBTYPE,
        {sizeof(EFI_DEVICE_PATH_PROTOCOL),0}
    };
    EFI_DEVICE_PATH_PROTOCOL    *Dp, *NewDp;
    EFI_GUID    gFboBoTagDevicePath = FBO_BO_TAG_DEVICE_PATH_GUID;
    

    // Try To Find BoTag Device Path
    Dp = Option->FilePathList;
    do{
        
        if(Dp->Type == HARDWARE_DEVICE_PATH &&
           Dp->SubType == HW_VENDOR_DP &&
           !MemCmp((Dp + 1), &gFboBoTagDevicePath, sizeof(EFI_GUID)))
        {
            // Found !!
            return;
        }
        Dp = NEXT_NODE(Dp);
    }while(!isEndNode(Dp));    
    
    // Update BoTag Device path
    FboBoTagDpTemplate.BoTag = BoTag;
    NewDp = Dp = Malloc(sizeof(FboBoTagDpTemplate) + Option->FilePathListLength);
    MemCpy(Dp, Option->FilePathList, NODE_LENGTH(Option->FilePathList));
    *(UINT8**)&Dp += NODE_LENGTH(Option->FilePathList);
    MemCpy(Dp, &FboBoTagDpTemplate, sizeof(FBO_BO_TAG_DEVICE_PATH));
    *(UINT8**)&Dp += sizeof(FBO_BO_TAG_DEVICE_PATH);    
    MemCpy(Dp, ((UINT8*)Option->FilePathList + NODE_LENGTH(Option->FilePathList)), Option->FilePathListLength - NODE_LENGTH(Option->FilePathList));
    pBS->FreePool(Option->FilePathList);
    Option->FilePathList = NewDp;
    Option->FilePathListLength +=   sizeof(FBO_BO_TAG_DEVICE_PATH);
    return;
}
/**
    Set Legacy Boot Option Device Type to Native Type

    @param VOID

    @retval VOID

**/
VOID
FboSetLegacyDeviceType()
{
    DLINK *Link;
    BOOT_OPTION *Option;
    
    // FBO will restore Device type to BoTag for Fixed Boot Order
    // save OldLegacyDevOrder with BoTag
    // FboSyncBootOptionsFromDevOrder() will check OldLegacyDevOrder and FboLegacyDevOrder
#if defined(CSM_SUPPORT) && CSM_SUPPORT
    UINTN DevOrderSize;
    LEGACY_DEVICE_ORDER *DevOrder;

    SortList(BootOptionList, ComparePriorityThenBootOptionNumber);

    BuildLegacyDevOrderBuffer(&DevOrder, &DevOrderSize);
    pRS->SetVariable(
        L"OldLegacyDevOrder", &LegacyDevOrderGuid,
        BOOT_VARIABLE_ATTRIBUTES,DevOrderSize,DevOrder
    );
    pBS->FreePool(DevOrder);
#endif
    FOR_EACH_BOOT_OPTION(BootOptionList, Link, Option)
    {
        if (!IsLegacyBootOption(Option)) continue;
        
        // Save FboBoTag into Device path
        FboBuildTagPath(Option, ((BBS_BBS_DEVICE_PATH*)Option->FilePathList)->DeviceType);
                    
        // Update GroupHeader Device type from child boot option
        if(Option->GroupHeader)
        {
            BOOT_OPTION *Option1;
            DLINK *Link1;
            FOR_EACH_BOOT_OPTION(BootOptionList, Link1, Option1)
            {
                if(!Option1->GroupHeader && 
                   (Option1->BootOptionNumber == Option->BootOptionNumber))
                {
                    ((BBS_BBS_DEVICE_PATH*)Option->FilePathList)->DeviceType = 
                            GetBbsEntryDeviceTypeDefault(Option1->BbsEntry);
                    break;
                }
            }
        }else 
        {
            
            ((BBS_BBS_DEVICE_PATH*)Option->FilePathList)->DeviceType = 
                    GetBbsEntryDeviceTypeDefault(Option->BbsEntry);
        }

    }
#if defined(CSM_SUPPORT) && CSM_SUPPORT
    SortList(BootOptionList, ComparePriorityThenBootOptionNumber);

    BuildLegacyDevOrderBuffer(&DevOrder, &DevOrderSize);
    pRS->SetVariable(
        L"LegacyDevOrder", &LegacyDevOrderGuid,
        BOOT_VARIABLE_ATTRIBUTES,DevOrderSize,DevOrder
    );
    pBS->FreePool(DevOrder);
#endif
    
}
/**
    Set BoTag to Legacy Boot Option Device Type.
    Fbo Need BoTag to orgnize Legacy Options

    @param VOID

    @retval VOID

**/
VOID
FboUpdateBoTagDeviceType()
{
    DLINK *Link;
    BOOT_OPTION *Option;
    EFI_GUID    gFboBoTagDevicePath = FBO_BO_TAG_DEVICE_PATH_GUID;


    FOR_EACH_BOOT_OPTION(BootOptionList, Link, Option)
    {
        FBO_BO_TAG_DEVICE_PATH      *Dp;
        if (!IsLegacyBootOption(Option)) continue;
        
        // Get BoTag
        Dp = (FBO_BO_TAG_DEVICE_PATH*)Option->FilePathList;
        do{
            
            if(Dp->Header.Header.Type == HARDWARE_DEVICE_PATH &&
               Dp->Header.Header.SubType == HW_VENDOR_DP &&
               !MemCmp(&Dp->Header.Guid, &gFboBoTagDevicePath, sizeof(EFI_GUID)))
            {
                // Update Device type
                ((BBS_BBS_DEVICE_PATH*)Option->FilePathList)->DeviceType = Dp->BoTag;
                break;
            }
            Dp = (FBO_BO_TAG_DEVICE_PATH*)NEXT_NODE((EFI_DEVICE_PATH_PROTOCOL*)Dp);
        }while(!isEndNode((EFI_DEVICE_PATH_PROTOCOL*)Dp));
    }
}
/**
    Check For new Device, set "AmiEmaIndications"

    @param VOID

    @retval VOID

**/
VOID
Fbo_CheckForNewDevice
(
    VOID
)
{
    DLINK       *Link;
    BOOT_OPTION *Option;
    EFI_STATUS  Status;
    UINT32      EmaIndications = BIT00;
    UINTN       EMAIndicationSize = sizeof(UINT32);
    BOOLEAN     DeviceChanged = FALSE;
    EFI_GUID    gEMAIndicationsGuid = EMA_INDICATION_GUID;

    FOR_EACH_BOOT_OPTION(BootOptionList, Link, Option)
    {
        if(Option->GroupHeader)
            continue;
        
        // Orphan Boot Option, Device is removed
        if(!IsBootOptionWithDevice(Option))
            DeviceChanged = TRUE;
        
        // New Device
        if(Option->BootOptionNumber == INVALID_BOOT_OPTION_NUMBER)
            DeviceChanged = TRUE;
    }
    
    if(DeviceChanged)
    {
        Status = pRS->SetVariable(
                     L"AmiEmaIndications",
                     &gEMAIndicationsGuid,
                     EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                     EMAIndicationSize,
                     &EmaIndications);
    }
}

#if !TseDefaultBootOrder_SUPPORT
VOID FboSetDefaultTseBootOrder(VOID)
{
    #if defined(CSM_SUPPORT) && CSM_SUPPORT == 1
    UINTN DevOrderSize;
    LEGACY_DEVICE_ORDER *DevOrder;
    EFI_GUID DefaultLegacyDevOrderGuid = DEFAULT_LEGACY_DEV_ORDER_VARIABLE_GUID;
    LEGACY_DEVICE_ORDER *Order;
    UINTN  i;
    #endif  // #if defined(CSM_SUPPORT) && CSM_SUPPORT == 1
    UINTN BootOrderSize;

    EFI_GUID DefaultBootOrderGuid = DEFAULT_BOOT_ORDER_VARIABLE_GUID;

    // PRECONDITION: All Boot Option Numbers are set
    BootOrderSize = BootOptionList->Size * sizeof(UINT16);
    if (BootOrderSize == 0) 
    {
#if defined(CSM_SUPPORT) && CSM_SUPPORT == 1
        // if the boot order is empty, delete the Default boot order variables
        pRS->SetVariable(
            L"DefaultLegacyDevOrder", 
            &DefaultLegacyDevOrderGuid,
            BOOT_VARIABLE_ATTRIBUTES,
            0,
            NULL
            );
#endif  // #if defined(CSM_SUPPORT) && CSM_SUPPORT == 1
        return;
    }
    
    //SortList(BootOptionList, CompareTagThenBbsIndex);

    #if defined(CSM_SUPPORT) && CSM_SUPPORT == 1
    // Build DefaultLegacyDevOrder variable
    BuildLegacyDevOrderBuffer(&DevOrder, &DevOrderSize);

    // Remove disable mask from buffer DevOrder
    for( Order = DevOrder; 
         (UINT8*)Order < (UINT8*)DevOrder + DevOrderSize; 
         Order = (LEGACY_DEVICE_ORDER*)((UINT8*)Order + Order->Length + sizeof(Order->Type)) )
        for( i = 0; i < (Order->Length / sizeof(UINT16) - 1); i++ ){
            Order->Device[i] &= 0x00FF;
            DEBUG((-1,"Order->Device[%d]= %x \n",i,Order->Device[i]));
        }

    // Store "DefaultLegacyDevOrder" EFI variable
    pRS->SetVariable(
        L"DefaultLegacyDevOrder", 
        &DefaultLegacyDevOrderGuid,
        BOOT_VARIABLE_ATTRIBUTES,
        DevOrderSize,
        DevOrder
    );
    pBS->FreePool(DevOrder);
    #endif  // #if defined(CSM_SUPPORT) && CSM_SUPPORT == 1
}
#endif

/**
    Adjust Setup Priorities Buffer.

    @param SetupBuffer       Buffer of group priorities
    @param CurrentGroupSize  Number of the groups    
    @param BufferSize        Size of the buffer

    @retval EFI_SUCCESS        The number of the group doesn't change
    @retval EFI_MEDIA_CHANGED  The number of the group changed

**/
EFI_STATUS
AdjustSetupData
(
        UINT16* SetupBuffer,
        UINT16  CurrentGroupSize,
        UINT16  BufferSize
)
{
    UINT16      i, j;
    EFI_STATUS  Status = EFI_SUCCESS;
    BOOLEAN     LoadDefault = FALSE;
    
    for(i = 0 ; i < BufferSize && *(SetupBuffer + i) != 0xFF ; i++);

    if(i != CurrentGroupSize)
        LoadDefault = TRUE;
    if(LoadDefault)
    {
        // Load default when Group size is changed.
        // We don't try to sort old settings and save them into new BIOS setup.
        // Because 0 can be every group
        // EX : 0 is HDD in old BIOS and it can be NVME in this BIOS
        for(j = 0 ; j < CurrentGroupSize ; j++)
            *(SetupBuffer + j) = j;
        if(j < BufferSize)
            *(SetupBuffer + j) = 0xFF;
        Status = EFI_MEDIA_CHANGED;
    }
    return Status;
}

/**
    FBO group size might be changed after BIOS updated.
    Handle different size here

    @param VOID

    @retval VOID

**/
VOID
FboSetupSizeCheck
(
    VOID
)
{
    EFI_STATUS  Status;
    EFI_GUID    gSetupGuid = SETUP_GUID;
    SETUP_DATA  *pSetupData = NULL;    
    UINT32      SetupAttr;
    UINTN       SetupSize = 0;

    Status = GetEfiVariable(L"Setup", &gSetupGuid, &SetupAttr, &SetupSize, &pSetupData);
    if (EFI_ERROR(Status)) return;
    Status = AdjustSetupData((UINT16*)&pSetupData->LegacyPriorities[0], GetDevMapDataCount(pFBOLegacyDevMap), 16);
    Status |= AdjustSetupData((UINT16*)&pSetupData->UefiPriorities[0], GetDevMapDataCount(pFBOUefiDevMap), 16);
#if FBO_DUAL_MODE
    Status |= AdjustSetupData((UINT16*)&pSetupData->DualPriorities[0], GetDevMapDataCount(pFBODualDevMap), 32);
#endif
    if(Status == EFI_MEDIA_CHANGED)
    {
#if LOCK_SETVAR_AT_ENDOFDXE
        SETUPNVLOCK_MAILBOX NvLockMailbox ; //communication between DXE and SMM
        EFI_GUID    NVLockMailboxVariableGuid = SETUPNVLOCK_MAILBOX_ADDRESS_VARIABLE_GUID;
        UINTN       VarSize = sizeof(NvLockMailbox);
        UINTN       OriLockState;
        Status =  pRS->GetVariable(L"NvLockMailbox", 
                                    &NVLockMailboxVariableGuid, 
                                    NULL, 
                                    &VarSize, 
                                    &NvLockMailbox );
        if(!EFI_ERROR(Status))
        {
            OriLockState = NvLockMailbox.NvLockState;
            NvLockMailbox.NvLockState = 0; 
            pRS->SetVariable( L"NvLockMailbox", 
                                  &NVLockMailboxVariableGuid,
                                  EFI_VARIABLE_BOOTSERVICE_ACCESS , 
                                  sizeof(NvLockMailbox), 
                                  &NvLockMailbox );
        }
#endif        
        pRS->SetVariable(L"Setup", &gSetupGuid, SetupAttr, SetupSize, pSetupData);

#if LOCK_SETVAR_AT_ENDOFDXE
        if(!EFI_ERROR(Status))
        {
            NvLockMailbox.NvLockState = OriLockState; 
            Status = pRS->SetVariable( L"NvLockMailbox", 
                                          &NVLockMailboxVariableGuid,
                                          EFI_VARIABLE_BOOTSERVICE_ACCESS , 
                                          sizeof(NvLockMailbox), 
                                          &NvLockMailbox );
        }
#endif
    }
}

