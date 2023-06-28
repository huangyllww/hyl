//***********************************************************************
//*                                                                     *
//*   Copyright (c) 1985-2019, American Megatrends International LLC.   *
//*                                                                     *
//*      All rights reserved. Subject to AMI licensing agreement.       *
//*                                                                     *
//***********************************************************************

/** @file FixedBootOrder.c

    This file is linked to Setup Binary for form callback
*/
#include <Token.h>
#include <Efi.h>
#include <AmiDxeLib.h>
#include <Setup.h>
#include <Library/DebugLib.h>

#if CSM_SUPPORT
#include <Protocol/LegacyBios.h>
#endif

#if EFI_SPECIFICATION_VERSION>0x20000
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/HiiString.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/FormBrowser2.h>
#include <Protocol/HiiConfigRouting.h>
#else   //#if EFI_SPECIFICATION_VERSION>0x20000         
#include <FormCallback.h>
#include <HIIDB.h>
#include <Protocol/ExtHii.h>
#endif

#include <FixedBootOrder.h>
#include <DefaultFixedBootOrder.h>
#if FBO_RUNTIME_CALLBACK_REGISTRATION
#include <Library/AmiSetupLibrary.h>
#endif
#include <AutoId.h>

#pragma pack(1)

typedef struct
{
    UINT32 Type;
    UINT16 Length;
    UINT16 Device[1];
} LEGACY_DEVICE_ORDER;

typedef struct
{
    UINT8 FormSet ;
    UINT16 FormMain ;
} FBO_SETUP_PAGE_INFORM;

#pragma pack()
#ifndef AMITSE_EVENT_BEFORE_BOOT_GUID
#define AMITSE_EVENT_BEFORE_BOOT_GUID \
  { 0x3677770f, 0xefb2, 0x43b2, 0xb8, 0xae, 0xb3, 0x2, 0xe9, 0x60, 0x48, 0x82 }
#endif
#ifndef AMITSE_EVENT_AFTER_BOOT_GUID
#define AMITSE_EVENT_AFTER_BOOT_GUID \
  {0x8c12a959, 0x70bc, 0x4362, 0xb4, 0x37, 0xb8, 0x05, 0x14, 0xa1, 0x91, 0x6e}
#endif
BOOLEAN LoadedDefault = FALSE;
BOOLEAN gDisplayDeviceName = TRUE;
#if CSM_SUPPORT
BBS_TABLE *BbsTable = NULL;
HDD_INFO  *HddInfo;
UINT16    HddCount = 0;
UINT16    BbsCount = 0;
#endif

EFI_HII_STRING_PROTOCOL *pHiiString = NULL;

EFI_HII_HANDLE FixedBoot_HiiHandle;

EFI_GUID EfiVariableGuid = EFI_GLOBAL_VARIABLE;
EFI_GUID LegacyDevOrderGuid = LEGACY_DEV_ORDER_GUID;
EFI_GUID FixedBootOrderGuid = FIXED_BOOT_ORDER_GUID;
EFI_GUID DefaultLegacyDevOrderGuid = DEFAULT_LEGACY_DEV_ORDER_VARIABLE_GUID;

FBODevMap *pFBODualDevMap = NULL;
FBODevMap *pFBOLegacyDevMap = NULL;
FBODevMap *pFBOUefiDevMap = NULL;
FBOHiiMap *pFBOHiiMap = NULL;
UINT16 *pBootOptionTokenMap = NULL;     //(EIP123284+)

#if !FBO_RUNTIME_CALLBACK_REGISTRATION
CALLBACK_PARAMETERS *CallbackValues = NULL;
CALLBACK_PARAMETERS *GetCallbackParameters();
#endif

UINT8 gHiiSkipFBOModule = 0 ;
UINT8 BootDeviceNum;
UINT8 TypeLegacyDeviceOptions[FIXED_BOOT_ORDER_GROUP_MAX_NUM][FIXED_BOOT_ORDER_SUBMENU_MAX_NUM];
UINT8 TypeLegacyDeviceCount[FIXED_BOOT_ORDER_GROUP_MAX_NUM];        //GROUP
UINT8 TypeUefiDeviceOptions[FIXED_BOOT_ORDER_GROUP_MAX_NUM][FIXED_BOOT_ORDER_SUBMENU_MAX_NUM];
UINT8 TypeUefiDeviceCount[FIXED_BOOT_ORDER_GROUP_MAX_NUM];          //GROUP

typedef EFI_STATUS(FBO_INITIAL_GROUP)();
extern FBO_INITIAL_GROUP INITIAL_LEGCAY_GROUP_FUNCTION;
FBO_INITIAL_GROUP *InitLegacyGroup = INITIAL_LEGCAY_GROUP_FUNCTION;
extern FBO_INITIAL_GROUP INITIAL_UEFI_GROUP_FUNCTION;
FBO_INITIAL_GROUP *InitUefiGroup = INITIAL_UEFI_GROUP_FUNCTION;


FBO_SETUP_PAGE_INFORM FboSetupPageInform[] = {FIXED_BOOT_ORDER_SETUP_INFORM {0,0}} ;

#if (FBO_DUAL_MODE == 1) && (CSM_SUPPORT == 1)
FBOHiiMap FBOHiiMapData[]={
    {  BoTagLegacyHardDisk,     STR_BOOT_DUAL_HDD,               HDD_BOOT_FORM_ID,         HDD_BOOT_FORM_LABEL,         EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyHDD)},
#if FBO_MULTI_HARD_DISK_GROUPS
    {  BoTagLegacyHardDisk1,    STR_BOOT_DUAL_HDD1,              HDD1_BOOT_FORM_ID,        HDD1_BOOT_FORM_LABEL,        EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyHDD1)},
    {  BoTagLegacyHardDisk2,    STR_BOOT_DUAL_HDD2,              HDD2_BOOT_FORM_ID,        HDD2_BOOT_FORM_LABEL,        EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyHDD2)},
    {  BoTagLegacyHardDisk3,    STR_BOOT_DUAL_HDD3,              HDD3_BOOT_FORM_ID,        HDD3_BOOT_FORM_LABEL,        EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyHDD3)},
    {  BoTagLegacyHardDisk4,    STR_BOOT_DUAL_HDD4,              HDD4_BOOT_FORM_ID,        HDD4_BOOT_FORM_LABEL,        EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyHDD4)},
    {  BoTagLegacyHardDisk5,    STR_BOOT_DUAL_HDD5,              HDD5_BOOT_FORM_ID,        HDD5_BOOT_FORM_LABEL,        EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyHDD5)},
#endif
#if NVME_GROUP_SUPPORT
    {  BoTagLegacyNvme,         STR_BOOT_DUAL_NVME,              NVME_BOOT_FORM_ID,        NVME_BOOT_FORM_LABEL,        EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyNVME)},
#endif
    {  BoTagLegacyCdrom,        STR_BOOT_DUAL_ODD,               ODD_BOOT_FORM_ID,         ODD_BOOT_FORM_LABEL,         EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyODD)},
#if SD_GROUP_SUPPORT
    {  BoTagLegacySd,           STR_BOOT_DUAL_SD,                SD_BOOT_FORM_ID,          SD_BOOT_FORM_LABEL,          EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacySD)},
#endif
    {  BoTagLegacyEmbedNetwork, STR_BOOT_DUAL_NETWORK,           NETWORK_BOOT_FORM_ID,     NETWORK_BOOT_FORM_LABEL,     EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyNETWORK)},
#if USB_GROUP == 2
    {  BoTagLegacyUSBFloppy,    STR_BOOT_DUAL_USBFDD,            USBFDD_BOOT_FORM_ID,      USBFDD_BOOT_FORM_LABEL,      EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyUSBFDD)},
    {  BoTagLegacyUSBHardDisk,  STR_BOOT_DUAL_USBHDD,            USBHDD_BOOT_FORM_ID,      USBHDD_BOOT_FORM_LABEL,      EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyUSBHDD)},
    {  BoTagLegacyUSBCdrom,     STR_BOOT_DUAL_USBODD,            USBODD_BOOT_FORM_ID,      USBODD_BOOT_FORM_LABEL,      EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyUSBODD)},
    {  BoTagLegacyUSBKey,       STR_BOOT_DUAL_USBKEY,            USBKEY_BOOT_FORM_ID,      USBKEY_BOOT_FORM_LABEL,      EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyUSBKEY)},
    {  BoTagLegacyUSBLan,       STR_BOOT_DUAL_USBLAN,            USBLAN_BOOT_FORM_ID,      USBLAN_BOOT_FORM_LABEL,      EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyUSBLAN)},
#endif
#if USB_GROUP == 1
    {  BoTagLegacyUsb,          STR_BOOT_DUAL_USB,               USB_BOOT_FORM_ID,         USB_BOOT_FORM_LABEL,         EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyUSB)},
#endif
#if OTHER_GROUP == 1
    {  BoTagLegacyOther,        STR_BOOT_DUAL_OTHER,             OTHER_BOOT_FORM_ID,       OTHER_BOOT_FORM_LABEL,       EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyOTHER)},
#endif
#if EFI_DEVICE_IN_ONE_GROUP
    {  BoTagUefi,               STR_BOOT_DUAL_UEFI_DEVICE,       UEFI_DEVICE_BOOT_FORM_ID, UEFI_DEVICE_BOOT_FORM_LABEL, EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiDEVICE)},
#else
    {  BoTagUefiHardDisk,       STR_BOOT_DUAL_UEFI_HDD,     UEFI_HDD_BOOT_FORM_ID,    UEFI_HDD_BOOT_FORM_LABEL,    EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiHDD)},
#if FBO_MULTI_HARD_DISK_GROUPS
    {  BoTagUefiHardDisk1,      STR_BOOT_DUAL_UEFI_HDD1,    UEFI_HDD1_BOOT_FORM_ID,   UEFI_HDD1_BOOT_FORM_LABEL,   EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiHDD1)},
    {  BoTagUefiHardDisk2,      STR_BOOT_DUAL_UEFI_HDD2,    UEFI_HDD2_BOOT_FORM_ID,   UEFI_HDD2_BOOT_FORM_LABEL,   EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiHDD2)},
    {  BoTagUefiHardDisk3,      STR_BOOT_DUAL_UEFI_HDD3,    UEFI_HDD3_BOOT_FORM_ID,   UEFI_HDD3_BOOT_FORM_LABEL,   EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiHDD3)},
    {  BoTagUefiHardDisk4,      STR_BOOT_DUAL_UEFI_HDD4,    UEFI_HDD4_BOOT_FORM_ID,   UEFI_HDD4_BOOT_FORM_LABEL,   EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiHDD4)},
    {  BoTagUefiHardDisk5,      STR_BOOT_DUAL_UEFI_HDD5,    UEFI_HDD5_BOOT_FORM_ID,   UEFI_HDD5_BOOT_FORM_LABEL,   EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiHDD5)},
#endif
#if NVME_GROUP_SUPPORT
    {  BoTagUefiNvme,           STR_BOOT_DUAL_UEFI_NVME,    UEFI_NVME_BOOT_FORM_ID,   UEFI_NVME_BOOT_FORM_LABEL,   EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiNVME)},
#endif
    {  BoTagUefiCdrom,          STR_BOOT_DUAL_UEFI_ODD,     UEFI_ODD_BOOT_FORM_ID,    UEFI_ODD_BOOT_FORM_LABEL,    EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiODD)},
#if SD_GROUP_SUPPORT
    {  BoTagUefiSd,             STR_BOOT_DUAL_UEFI_SD,      UEFI_SD_BOOT_FORM_ID,     UEFI_SD_BOOT_FORM_LABEL,     EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiSD)},
#endif
    {  BoTagUefiNetWork,        STR_BOOT_DUAL_UEFI_NETWORK, UEFI_NETWORK_BOOT_FORM_ID, UEFI_NETWORK_BOOT_FORM_LABEL, EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiNETWORK)},
#if USB_GROUP == 2
    {  BoTagUefiUsbHardDisk,    STR_BOOT_DUAL_UEFI_USBHDD,  UEFI_USBHDD_BOOT_FORM_ID, UEFI_USBHDD_BOOT_FORM_LABEL, EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiUSBHDD)},
    {  BoTagUefiUsbCdrom,       STR_BOOT_DUAL_UEFI_USBODD,  UEFI_USBODD_BOOT_FORM_ID, UEFI_USBODD_BOOT_FORM_LABEL, EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiUSBODD)},
    {  BoTagUefiUsbKey,         STR_BOOT_DUAL_UEFI_USBKEY,  UEFI_USBKEY_BOOT_FORM_ID, UEFI_USBKEY_BOOT_FORM_LABEL, EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiUSBKEY)},
    {  BoTagUefiUsbFloppy,      STR_BOOT_DUAL_UEFI_USBFDD,  UEFI_USBFDD_BOOT_FORM_ID, UEFI_USBFDD_BOOT_FORM_LABEL, EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiUSBFDD)},     //(EIP96232+)
    {  BoTagUefiUsbLan,         STR_BOOT_DUAL_UEFI_USBLAN,  UEFI_USBLAN_BOOT_FORM_ID, UEFI_USBLAN_BOOT_FORM_LABEL, EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiUSBLAN)},
#endif
#if USB_GROUP == 1
    {  BoTagUefiUsb,            STR_BOOT_DUAL_UEFI_USB,     UEFI_USB_BOOT_FORM_ID,    UEFI_USB_BOOT_FORM_LABEL,    EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiUSB)},
#endif
#if SHELL_GROUP_SUPPORT
    {  BoTagUefiApplication,    STR_BOOT_DUAL_UEFI_APPLICATION,  UEFI_APPLICATION_BOOT_FORM_ID, UEFI_APPLICATION_BOOT_FORM_LABEL, EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiAPPLICATION)}, //(EIP115686+)
#endif
#if OTHER_GROUP == 1
    {  BoTagUefiOther,          STR_BOOT_DUAL_UEFI_OTHER,   UEFI_OTHER_BOOT_FORM_ID,  UEFI_OTHER_BOOT_FORM_LABEL,  EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiOTHER)},
#endif
#endif
    {  0, 0 }   //end of data
};
#else
FBOHiiMap FBOHiiMapData[]={
    {  BoTagLegacyHardDisk,     STR_BOOT_HDD,          HDD_BOOT_FORM_ID,         HDD_BOOT_FORM_LABEL,         EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyHDD)},
#if FBO_MULTI_HARD_DISK_GROUPS
    {  BoTagLegacyHardDisk1,    STR_BOOT_HDD1,         HDD1_BOOT_FORM_ID,        HDD1_BOOT_FORM_LABEL,        EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyHDD1)},
    {  BoTagLegacyHardDisk2,    STR_BOOT_HDD2,         HDD2_BOOT_FORM_ID,        HDD2_BOOT_FORM_LABEL,        EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyHDD2)},
    {  BoTagLegacyHardDisk3,    STR_BOOT_HDD3,         HDD3_BOOT_FORM_ID,        HDD3_BOOT_FORM_LABEL,        EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyHDD3)},
    {  BoTagLegacyHardDisk4,    STR_BOOT_HDD4,         HDD4_BOOT_FORM_ID,        HDD4_BOOT_FORM_LABEL,        EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyHDD4)},
    {  BoTagLegacyHardDisk5,    STR_BOOT_HDD5,         HDD5_BOOT_FORM_ID,        HDD5_BOOT_FORM_LABEL,        EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyHDD5)},
#endif
#if NVME_GROUP_SUPPORT
    {  BoTagLegacyNvme,         STR_BOOT_NVME,         NVME_BOOT_FORM_ID,        NVME_BOOT_FORM_LABEL,        EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyNVME)},
#endif
    {  BoTagLegacyCdrom,        STR_BOOT_ODD,          ODD_BOOT_FORM_ID,         ODD_BOOT_FORM_LABEL,         EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyODD)},
#if SD_GROUP_SUPPORT
    {  BoTagLegacySd,           STR_BOOT_SD,           SD_BOOT_FORM_ID,          SD_BOOT_FORM_LABEL,          EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacySD)},
#endif
    {  BoTagLegacyEmbedNetwork, STR_BOOT_NETWORK,      NETWORK_BOOT_FORM_ID,     NETWORK_BOOT_FORM_LABEL,     EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyNETWORK)},
#if USB_GROUP == 2
    {  BoTagLegacyUSBFloppy,    STR_BOOT_USBFDD,       USBFDD_BOOT_FORM_ID,      USBFDD_BOOT_FORM_LABEL,      EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyUSBFDD)},
    {  BoTagLegacyUSBHardDisk,  STR_BOOT_USBHDD,       USBHDD_BOOT_FORM_ID,      USBHDD_BOOT_FORM_LABEL,      EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyUSBHDD)},
    {  BoTagLegacyUSBCdrom,     STR_BOOT_USBODD,       USBODD_BOOT_FORM_ID,      USBODD_BOOT_FORM_LABEL,      EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyUSBODD)},
    {  BoTagLegacyUSBKey,       STR_BOOT_USBKEY,       USBKEY_BOOT_FORM_ID,      USBKEY_BOOT_FORM_LABEL,      EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyUSBKEY)},
    {  BoTagLegacyUSBLan,       STR_BOOT_USBLAN,       USBLAN_BOOT_FORM_ID,      USBLAN_BOOT_FORM_LABEL,      EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyUSBLAN)},
#endif
#if USB_GROUP == 1
    {  BoTagLegacyUsb,          STR_BOOT_USB,          USB_BOOT_FORM_ID,         USB_BOOT_FORM_LABEL,         EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyUSB)},
#endif
#if OTHER_GROUP == 1
    {  BoTagLegacyOther,        STR_BOOT_OTHER,        OTHER_BOOT_FORM_ID,       OTHER_BOOT_FORM_LABEL,       EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, LegacyOTHER)},
#endif
#if EFI_DEVICE_IN_ONE_GROUP
    {  BoTagUefi,               STR_BOOT_UEFI_DEVICE,  UEFI_DEVICE_BOOT_FORM_ID, UEFI_DEVICE_BOOT_FORM_LABEL, EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiDEVICE)},
#else
    {  BoTagUefiHardDisk,       STR_BOOT_UEFI_HDD,     UEFI_HDD_BOOT_FORM_ID,    UEFI_HDD_BOOT_FORM_LABEL,    EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiHDD)},
#if FBO_MULTI_HARD_DISK_GROUPS
    {  BoTagUefiHardDisk1,      STR_BOOT_UEFI_HDD1,    UEFI_HDD1_BOOT_FORM_ID,   UEFI_HDD1_BOOT_FORM_LABEL,   EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiHDD1)},
    {  BoTagUefiHardDisk2,      STR_BOOT_UEFI_HDD2,    UEFI_HDD2_BOOT_FORM_ID,   UEFI_HDD2_BOOT_FORM_LABEL,   EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiHDD2)},
    {  BoTagUefiHardDisk3,      STR_BOOT_UEFI_HDD3,    UEFI_HDD3_BOOT_FORM_ID,   UEFI_HDD3_BOOT_FORM_LABEL,   EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiHDD3)},
    {  BoTagUefiHardDisk4,      STR_BOOT_UEFI_HDD4,    UEFI_HDD4_BOOT_FORM_ID,   UEFI_HDD4_BOOT_FORM_LABEL,   EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiHDD4)},
    {  BoTagUefiHardDisk5,      STR_BOOT_UEFI_HDD5,    UEFI_HDD5_BOOT_FORM_ID,   UEFI_HDD5_BOOT_FORM_LABEL,   EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiHDD5)},
#endif
#if NVME_GROUP_SUPPORT
    {  BoTagUefiNvme,           STR_BOOT_UEFI_NVME,    UEFI_NVME_BOOT_FORM_ID,   UEFI_NVME_BOOT_FORM_LABEL,   EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiNVME)},
#endif
    {  BoTagUefiCdrom,          STR_BOOT_UEFI_ODD,     UEFI_ODD_BOOT_FORM_ID,    UEFI_ODD_BOOT_FORM_LABEL,    EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiODD)},
#if SD_GROUP_SUPPORT
    {  BoTagUefiSd,             STR_BOOT_UEFI_SD,      UEFI_SD_BOOT_FORM_ID,     UEFI_SD_BOOT_FORM_LABEL,    EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiSD)},
#endif
    {  BoTagUefiNetWork,        STR_BOOT_UEFI_NETWORK, UEFI_NETWORK_BOOT_FORM_ID, UEFI_NETWORK_BOOT_FORM_LABEL, EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiNETWORK)},
#if USB_GROUP == 2
    {  BoTagUefiUsbHardDisk,    STR_BOOT_UEFI_USBHDD,  UEFI_USBHDD_BOOT_FORM_ID, UEFI_USBHDD_BOOT_FORM_LABEL, EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiUSBHDD)},
    {  BoTagUefiUsbCdrom,       STR_BOOT_UEFI_USBODD,  UEFI_USBODD_BOOT_FORM_ID, UEFI_USBODD_BOOT_FORM_LABEL, EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiUSBODD)},
    {  BoTagUefiUsbKey,         STR_BOOT_UEFI_USBKEY,  UEFI_USBKEY_BOOT_FORM_ID, UEFI_USBKEY_BOOT_FORM_LABEL, EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiUSBKEY)},
    {  BoTagUefiUsbFloppy,      STR_BOOT_UEFI_USBFDD,  UEFI_USBFDD_BOOT_FORM_ID, UEFI_USBFDD_BOOT_FORM_LABEL, EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiUSBFDD)},      //(EIP96232+)
    {  BoTagUefiUsbLan,         STR_BOOT_UEFI_USBLAN,  UEFI_USBLAN_BOOT_FORM_ID, UEFI_USBLAN_BOOT_FORM_LABEL, EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiUSBLAN)},
#endif
#if USB_GROUP == 1
    {  BoTagUefiUsb,            STR_BOOT_UEFI_USB,     UEFI_USB_BOOT_FORM_ID,    UEFI_USB_BOOT_FORM_LABEL,    EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiUSB)},
#endif
#if SHELL_GROUP_SUPPORT
    {  BoTagUefiApplication,    STR_BOOT_UEFI_APPLICATION,  UEFI_APPLICATION_BOOT_FORM_ID, UEFI_APPLICATION_BOOT_FORM_LABEL, EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiAPPLICATION)},  //(EIP115686+)
#endif
#if OTHER_GROUP == 1
    {  BoTagUefiOther,          STR_BOOT_UEFI_OTHER,   UEFI_OTHER_BOOT_FORM_ID,  UEFI_OTHER_BOOT_FORM_LABEL,  EFI_FIELD_OFFSET(FIXED_BOOT_GROUP, UefiOTHER)},
#endif
#endif
    {  0, 0 }   //end of data
};
#endif
//(EIP123284+)>
UINT16 BootOptionStringToken[]={
    STR_LEGACY_BOOT_ORDER_1,
    STR_LEGACY_BOOT_ORDER_2,
    STR_LEGACY_BOOT_ORDER_3,
    STR_LEGACY_BOOT_ORDER_4,
    STR_LEGACY_BOOT_ORDER_5,
    STR_LEGACY_BOOT_ORDER_6,
    STR_LEGACY_BOOT_ORDER_7,
    STR_LEGACY_BOOT_ORDER_8,
    STR_LEGACY_BOOT_ORDER_9,
    STR_LEGACY_BOOT_ORDER_10,
    STR_LEGACY_BOOT_ORDER_11,
    STR_LEGACY_BOOT_ORDER_12,
    STR_LEGACY_BOOT_ORDER_13,
    STR_LEGACY_BOOT_ORDER_14,
    STR_LEGACY_BOOT_ORDER_15,
    STR_LEGACY_BOOT_ORDER_16,
    STR_LEGACY_BOOT_ORDER_17,
    STR_LEGACY_BOOT_ORDER_18,
    STR_UEFI_BOOT_ORDER_1,
    STR_UEFI_BOOT_ORDER_2,
    STR_UEFI_BOOT_ORDER_3,
    STR_UEFI_BOOT_ORDER_4,
    STR_UEFI_BOOT_ORDER_5,
    STR_UEFI_BOOT_ORDER_6,
    STR_UEFI_BOOT_ORDER_7,
    STR_UEFI_BOOT_ORDER_8,
    STR_UEFI_BOOT_ORDER_9,
    STR_UEFI_BOOT_ORDER_10,
    STR_UEFI_BOOT_ORDER_11,
    STR_UEFI_BOOT_ORDER_12,
    STR_UEFI_BOOT_ORDER_13,
    STR_UEFI_BOOT_ORDER_14,
    STR_UEFI_BOOT_ORDER_15,
    STR_UEFI_BOOT_ORDER_16,
    STR_UEFI_BOOT_ORDER_17,
    STR_UEFI_BOOT_ORDER_18
#if FBO_DUAL_MODE
    ,
    STR_DUAL_BOOT_ORDER_1,
    STR_DUAL_BOOT_ORDER_2,
    STR_DUAL_BOOT_ORDER_3,
    STR_DUAL_BOOT_ORDER_4,
    STR_DUAL_BOOT_ORDER_5,
    STR_DUAL_BOOT_ORDER_6,
    STR_DUAL_BOOT_ORDER_7,
    STR_DUAL_BOOT_ORDER_8,
    STR_DUAL_BOOT_ORDER_9,
    STR_DUAL_BOOT_ORDER_10,
    STR_DUAL_BOOT_ORDER_11,
    STR_DUAL_BOOT_ORDER_12,
    STR_DUAL_BOOT_ORDER_13,
    STR_DUAL_BOOT_ORDER_14,
    STR_DUAL_BOOT_ORDER_15,
    STR_DUAL_BOOT_ORDER_16,
    STR_DUAL_BOOT_ORDER_17,
    STR_DUAL_BOOT_ORDER_18,
    STR_DUAL_BOOT_ORDER_19,
    STR_DUAL_BOOT_ORDER_20,
    STR_DUAL_BOOT_ORDER_21,
    STR_DUAL_BOOT_ORDER_22,
    STR_DUAL_BOOT_ORDER_23,
    STR_DUAL_BOOT_ORDER_24,
    STR_DUAL_BOOT_ORDER_25,
    STR_DUAL_BOOT_ORDER_26,
    STR_DUAL_BOOT_ORDER_27,
    STR_DUAL_BOOT_ORDER_28,
    STR_DUAL_BOOT_ORDER_29,
    STR_DUAL_BOOT_ORDER_30,
    STR_DUAL_BOOT_ORDER_31,
    STR_DUAL_BOOT_ORDER_32,
    STR_DUAL_BOOT_ORDER_33,
    STR_DUAL_BOOT_ORDER_34,
    STR_DUAL_BOOT_ORDER_35,
    STR_DUAL_BOOT_ORDER_36
#endif
};
//<(EIP123284+)


/**
    Get the FBODevMap data amount.

    @param FBODevMap - FBODevMap

    @retval UINT16 - Number of FBODevMap

    @note  None

**/
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
    get the FBOHiiMap data amount.

    @retval UINT16 - Number of FBODevMap

    @note  None

**/
UINT16 GetHiiMapDataCount()
{
    UINT16 count = 0;

    if (!pFBOHiiMap) return 0;           //end of data

    do
    {
        if (pFBOHiiMap[count].DevType == 0) break;
        count++;
    }
    while (1);

    return count;
}

/**
    Search DevMap data index by device type.

        
    @param  FBODevMap - Device Map
    @param  DevType - Device Type want to search 

    @retval UINT16 - Index of Device Type in Device Map

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
    Search HiiMap data index by device type.

        
    @param DevType - Device Type want to Search

    @retval UINT16 - Index of the Device Type

    @note  None

**/
UINT16 SearchHiiMapByType(UINT16 DevType)
{
    UINT16 i = 0;

    do
    {
        if (pFBOHiiMap[i].DevType == 0) break;  //end of data
        if (pFBOHiiMap[i].DevType == DevType)
            return i;
        i++;
    }
    while (1);

    return 0xffff;
}

/**
    Compare whether two HDD device paths are the same.

    @param  DevDp1  Device path in comparison.
    @param  DevDp2  Device path in comparison.

    @retval  EFI_SUCCESS    Two HDD paths are the same.
    @retval  EFI_NOT_FOUND  Two Hdd paths are not the same.
*/
EFI_STATUS CompareHddDevicePath( EFI_DEVICE_PATH_PROTOCOL *DevDp1, EFI_DEVICE_PATH_PROTOCOL *DevDp2 )
{

    if ( DevDp1->Type == MEDIA_DEVICE_PATH &&
            DevDp1->SubType == MEDIA_HARDDRIVE_DP )
    {
        if (MemCmp(DevDp1+1, DevDp2+1, sizeof(HARDDRIVE_DEVICE_PATH)-sizeof(EFI_DEVICE_PATH_PROTOCOL)) == 0) //Skip Header EFI_DEVICE_PATH_PROTOCOL.
        {
            DevDp1 = NEXT_NODE(DevDp1);
            if ( DevDp1->Type == MEDIA_DEVICE_PATH &&
                    DevDp1->SubType == MEDIA_FILEPATH_DP ) Wcsupr( (CHAR16*)DevDp1+1 );

            DevDp2 = NEXT_NODE(DevDp2);
            if ( DevDp2->Type == MEDIA_DEVICE_PATH &&
                    DevDp2->SubType == MEDIA_FILEPATH_DP ) Wcsupr( (CHAR16*)DevDp2+1 );

            if (MemCmp(DevDp1, DevDp2, DPLength(DevDp2)) == 0)
                return EFI_SUCCESS;
        }
    }

    return EFI_NOT_FOUND;
}

/**
    Judge Dp1 and Dp2 is same device path

        
    @param Dp1 - Device Path want to compare 
    @param Dp2 - Device Path want to compare 
    
    @retval TRUE - Same Device Path.
    @retval FALSE - Not same.
    
    @note  EIP 143657

**/
BOOLEAN IsDevicePathMatch(
    IN EFI_DEVICE_PATH_PROTOCOL *Dp1, 
    IN EFI_DEVICE_PATH_PROTOCOL *Dp2)
{
    // Check null device path
    if (!Dp1 || !Dp2)
        return FALSE ;

    if (DPLength(Dp1) != DPLength(Dp2))
        return FALSE ;
    
    if (!MemCmp(Dp1, Dp2, DPLength(Dp1)))
        return TRUE ;
    else if (!CompareHddDevicePath(Dp1, Dp2))
        return TRUE ;

    return FALSE ;
}

/**
    Change Uefi Device option name in setup utility

        
    @param Handle - The Handle of Device
    @param DevName - Device Name Buffer
 
    @retval DevName - Device Name from Description Protocol
    @retval TRUE - Changed.
    @retval FALSE - Not Changed.
    
    @note  EIP 143657

**/
BOOLEAN ChangeLegacyDeviceOptionName(
    IN EFI_HANDLE Handle, 
    IN OUT CHAR16 *DevName)
{
    EFI_STATUS Status ;
    EFI_FIXED_BOOT_ORDER_PROTOCOL *FBOProtocol ; 
    Status = pBS->LocateProtocol (&FixedBootOrderGuid, NULL, &FBOProtocol);
    if (!EFI_ERROR(Status))
    {       
        EFI_DEVICE_PATH_PROTOCOL *Dp ;
        Status = pBS->HandleProtocol(Handle, &gEfiDevicePathProtocolGuid, &Dp);
        if (!EFI_ERROR(Status))
        {
            FBO_DEVICE_INFORM *DescriptionPtr ;
            DescriptionPtr = FBOProtocol->GetNewDescription() ;
            while( DescriptionPtr )
            {
                if (IsDevicePathMatch(Dp, DescriptionPtr->DevPath))
                {
                    MemSet(DevName,0x100,0) ;
                    StrCpy(DevName, DescriptionPtr->DevName) ;
                    DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "Set New Legacy Device Name: %S", DevName)) ;
                    return TRUE ;
                }
                DescriptionPtr = DescriptionPtr->Next ;
            }
        }
    }
    return FALSE ;
    
}

/**
    Change Uefi Device option name in setup utility

        
    @param BootOption - BootOption to be updated
    @param DevName - Device Name Buffer

    @retval DevName - Device Name
    @retval TRUE - Changed.
    @retval FALSE - Not Changed.
    
    @note  EIP 143657

**/
BOOLEAN ChangeUefiDeviceOptionName(
    IN EFI_LOAD_OPTION *BootOption, 
    IN OUT VOID **DevName)
{
    EFI_STATUS Status ;
    EFI_FIXED_BOOT_ORDER_PROTOCOL *FBOProtocol ;
    Status = pBS->LocateProtocol (&FixedBootOrderGuid, NULL, &FBOProtocol);
    if (!EFI_ERROR(Status))
    {
        FBO_DEVICE_INFORM *DescriptionPtr ;
        UINT8 *FilePathList ;
        UINTN OldNameSize ;
        OldNameSize = (Wcslen((CHAR16*)(BootOption+1))+1)*sizeof(CHAR16);
        FilePathList = (UINT8*)(BootOption+1)+OldNameSize;
        DescriptionPtr = FBOProtocol->GetNewDescription() ;
        while( DescriptionPtr )
        {
            if (IsDevicePathMatch((EFI_DEVICE_PATH_PROTOCOL*)FilePathList, DescriptionPtr->DevPath))
            {
                *DevName = DescriptionPtr->DevName ;
                DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE,"Set New Uefi Device Name: %S", (CHAR16*)*DevName)) ;
                return TRUE ;
            }
            DescriptionPtr = DescriptionPtr->Next ;
        }
    }
    return FALSE ;
}

/**
    Change FBO Group Name
 
    @note  EIP 143657

**/
VOID
FBO_AdjustGroupName()
{
    EFI_STATUS Status ;
    CHAR8* Languages = NULL, *OrgLanguages = NULL;
    UINT16 x ;
    UINT16 HiiSize = GetHiiMapDataCount() ;
    UINT32 Attr ;
    UINTN *GroupPtrAddr = NULL;
    UINTN LangSize = 0, Size = 0 ;
    UINTN OptionMaxSize = 0x100;
    BOOLEAN LastLanguage = FALSE;
    FBO_GROUP_OPTION *Group = NULL, *PreGroup = NULL ;
    
    if (pHiiString == NULL)
    {
        Status = pBS->LocateProtocol(&gEfiHiiStringProtocolGuid, NULL, &pHiiString);
        if (EFI_ERROR(Status))return;
    }

    Status = pHiiString->GetLanguages(pHiiString, FixedBoot_HiiHandle, Languages, &LangSize);
    if (Status == EFI_BUFFER_TOO_SMALL)
    {
        Status = pBS->AllocatePool(EfiBootServicesData, LangSize, &Languages);
        if (EFI_ERROR(Status)) return;        //not enough resources to allocate string
        Status = pHiiString->GetLanguages(pHiiString, FixedBoot_HiiHandle, Languages, &LangSize);
    }
    if (EFI_ERROR(Status)) return;  
    
    // Get Pointer Address 
    Status = GetEfiVariable(L"FboGroupNameData", &FixedBootOrderGuid, &Attr, &Size, &GroupPtrAddr);
    if ( EFI_ERROR(Status)) return ;
    
    OrgLanguages = Languages ;
    while(!LastLanguage) 
    {
        CHAR8* CurrentLanguage, StrSaveTemp ;
        CurrentLanguage = Languages;        //point CurrentLanguage 
        while (*Languages != ';' && *Languages != 0)
            Languages++;
        
        if(CurrentLanguage[0] == 'x' && CurrentLanguage[1] == '-')
        {
            if(*Languages == 0)
                LastLanguage = TRUE;
            Languages++;
            
            continue;
        }
        
        StrSaveTemp = *Languages;
        *Languages = 0;         //put null-terminator
        
        Group = (FBO_GROUP_OPTION*)*GroupPtrAddr ;
        while (Group)
        {
            for ( x=0; x < HiiSize ; x++ )
            {
                if (Group->StrToken == pFBOHiiMap[x].StrToken )
                {
                    DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE,"FixedBootOrder.c AdjustFBOGroupName Change Group Name(StrToken)\n")) ;
                    Status = pHiiString->SetString(pHiiString, FixedBoot_HiiHandle, pFBOHiiMap[x].StrToken, 
                                               CurrentLanguage, Group->NewGroupName, NULL);
                }
                else
                {
                    CHAR16 *TempString = MallocZ(OptionMaxSize) ;
                    Size = OptionMaxSize ;  
                    Status = pHiiString->GetString(pHiiString, CurrentLanguage, FixedBoot_HiiHandle, 
                                                   pFBOHiiMap[x].StrToken, TempString, &Size, NULL);
                    // if string match, update the new group name.
                    if (Group->OrgGroupName && !StrCmp(TempString, Group->OrgGroupName))
                    {
                        DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE,"FixedBootOrder.c AdjustFBOGroupName Change Group Name(OrgName)\n")) ;
                        Status = pHiiString->SetString(pHiiString, FixedBoot_HiiHandle, pFBOHiiMap[x].StrToken, 
                                                       CurrentLanguage, Group->NewGroupName, NULL);
                    }
                    pBS->FreePool(TempString) ;
                }   
            }     
            Group = Group->Next ;
        }
        *Languages = StrSaveTemp;
        
        if (*Languages == 0)    //last language in language list
            LastLanguage = TRUE ;
        else
            Languages++ ;
    }
    
    
    // Free all pointer and content
    while (Group)
    {
       PreGroup = Group;
       Group = Group->Next;
       pBS->FreePool(PreGroup);
    }
    Status = pRS->SetVariable(L"FboGroupNameData", &FixedBootOrderGuid, 0, 0, NULL);
    pBS->FreePool(OrgLanguages) ;
    pBS->FreePool(GroupPtrAddr) ;
}

/**
    New the string with language that was first found.

        
    @param HiiHandle  
    @param StrId 
    @param String 

    @retval EFI_STATUS

    @note  None

**/
EFI_STATUS 
NewStringWithFirstFoundLanguage
(
  IN EFI_HANDLE HiiHandle, 
  IN EFI_STRING_ID* StrId, 
  IN CHAR16 *String
)
{
    EFI_STATUS Status;
    CHAR8* Languages = NULL;
    UINTN  i;
    UINTN LangSize = 0;

    //
    // Get Hii String Protocol
    //
    if (pHiiString == NULL)
    {
        Status = pBS->LocateProtocol(&gEfiHiiStringProtocolGuid, NULL, &pHiiString);
        if (EFI_ERROR(Status)) return Status;
    }

    //
    // Get Languages
    //
    Status = pHiiString->GetLanguages(pHiiString, HiiHandle, Languages, &LangSize);
    if (Status == EFI_BUFFER_TOO_SMALL) {
        Status = pBS->AllocatePool(EfiBootServicesData, LangSize, &Languages);
        if (EFI_ERROR(Status))
            return Status;        //not enough resources to allocate string
        Status = pHiiString->GetLanguages(pHiiString, HiiHandle, Languages, &LangSize);
    }
    if (EFI_ERROR(Status)) {
        return Status;
    }

    // Search for ';'
    for(i = 0 ; (i < LangSize) && (*(Languages + i) != ';') ; i ++);                
    if(i != LangSize)
        // Find ';' , set a null-terminated
        *(Languages + i) = 0;

    Status = pHiiString->NewString(pHiiString, HiiHandle, StrId, Languages, NULL, String, NULL);
    pBS->FreePool(Languages);

    return Status;
}

/**
    Get Variable Boot#### and return data pointer.

        
    @param BootIndex - Boot Variable Number 

    @retval EFI_LOAD_OPTION - Pointer of Load Option
    @retval NULL - Load Option not found

**/
EFI_LOAD_OPTION* 
GetUEFIBootName
(
  IN UINT16 BootIndex
)
{
    EFI_STATUS Status;
    UINTN OptionSize = 0;
    CHAR16 BootVarName[15];
    EFI_LOAD_OPTION *BootOption = NULL;

    Swprintf(BootVarName, gBootName, BootIndex);             //(EIP35562)

    Status = GetEfiVariable(BootVarName, &EfiVariableGuid, NULL, &OptionSize, &BootOption);
    if (!EFI_ERROR(Status))
        return BootOption;

    return NULL;

}
//(EIP101408+)>
/**
    Update device item multi-language Hii string

    @param StrToken - Sring Token to be updated
    @param *DeviceName - Device Name

**/
VOID 
UpdateDeviceHiiString
(
  IN UINT16 StrToken, 
  IN CHAR16 *DeviceName
)
{
    EFI_STATUS Status;
    CHAR8* Languages = NULL;
    CHAR8* OrgLanguages = NULL;
    UINTN LangSize = 0;
    BOOLEAN LastLanguage = FALSE;
    CHAR8 StrSaveTemp;
    UINTN  i, Size = 0;
    CHAR16 OldStr[0x100+1], DeviceStr[0x100+1];

    if (StrToken < 0x100)
    {
        ASSERT(StrToken > 0x100) ;
        return ;
    }
    
    if (pHiiString == NULL)
    {
        Status = pBS->LocateProtocol(&gEfiHiiStringProtocolGuid, NULL, (VOID **) & pHiiString);
        if (EFI_ERROR(Status))
            return;
    }

    Status = pHiiString->GetLanguages(pHiiString, FixedBoot_HiiHandle, Languages, &LangSize);
    if (Status == EFI_BUFFER_TOO_SMALL)
    {
        Status = pBS->AllocatePool(EfiBootServicesData, LangSize, &Languages);
        if (EFI_ERROR(Status))
            return;        //not enough resources to allocate string
        Status = pHiiString->GetLanguages(pHiiString, FixedBoot_HiiHandle, Languages, &LangSize);
    }
    if (EFI_ERROR(Status))
        return;

    OrgLanguages = Languages;
    while (!LastLanguage)
    {
        CHAR8* CurrentLanguage;
        CurrentLanguage = Languages;        //point CurrentLanguage to start of new language
        while (*Languages != ';' && *Languages != 0)
            Languages++;
        
        if(CurrentLanguage[0] == 'x' && CurrentLanguage[1] == '-')
        {
            if(*Languages == 0)
                LastLanguage = TRUE;
            Languages++;
            
            continue;
        }
        
        StrSaveTemp = *Languages;
        *Languages = 0;         //put null-terminator
        Size = 0x100;
        DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "CurrentLanguage=%a\n", CurrentLanguage));
        Status = pHiiString->GetString(pHiiString, CurrentLanguage, FixedBoot_HiiHandle, StrToken, OldStr, &Size, NULL);
        *Languages = StrSaveTemp;

        if (EFI_ERROR(Status))       //Language not define in .uni
        {
            if (*Languages == 0)    //last language in language list
                LastLanguage = TRUE;
            else
            {
                *Languages = ';';       //restore original character
                Languages++;
            }
            continue;
        }


        //removec char ":"
        for (i = 0; i < Wcslen(OldStr); i++)
        {
            if (OldStr[i] == 0x003A)
            {
                OldStr[i] = (CHAR16)0;
                break;
            }
        }

        if (DeviceName == NULL)
            Swprintf(DeviceStr, L"%s", OldStr);
        else
            Swprintf(DeviceStr, L"%s:%s", OldStr, DeviceName);

        if (*Languages == 0)        //last language in language list
        {
            LastLanguage = TRUE;
            Status = pHiiString->SetString(pHiiString, FixedBoot_HiiHandle, StrToken, CurrentLanguage, DeviceStr, NULL);
            if (EFI_ERROR(Status))
            {
                pBS->FreePool(OrgLanguages);
                return;
            }
        }
        else
        {
            *Languages = 0;         //put null-terminator
            Status = pHiiString->SetString(pHiiString, FixedBoot_HiiHandle, StrToken, CurrentLanguage, DeviceStr, NULL);
            *Languages = ';';       //restore original character
            Languages++;
            if (EFI_ERROR(Status))
            {
                pBS->FreePool(OrgLanguages);
                return;
            }
        }
    }

    if (OrgLanguages)
        pBS->FreePool(OrgLanguages);
}


/**
    Sync legacy string to Group form HII handle.

    @param  Offset      
    @param  DeviceName      New string.

    @retval EFI_SUCCESS    Update new string to Group form success.
*/
EFI_STATUS
SyncLegacyStringWithGroupFormHii(
    IN UINT16   Offset, 
    IN CHAR16   *DeviceName)
{
    EFI_STATUS Status ;
    EFI_HII_HANDLE FboGfHiiHandle ;
    HII_DEVICE_INFO *LegacyHiiToken = NULL ;
    UINTN Size = sizeof(EFI_HII_HANDLE) ;
    UINTN LegacyHiiSize = 0 ;
    
    Status = pRS->GetVariable(L"FboGfHiiHandle", &FixedBootOrderGuid, NULL, &Size, &FboGfHiiHandle );
    if (EFI_ERROR(Status)) return Status ;
    
    Status = GetEfiVariable(L"FixedGfHiiLegacyDevData", 
                            &FixedBootOrderGuid, 
                            NULL, 
                            &LegacyHiiSize, 
                            &LegacyHiiToken);
    if (EFI_ERROR(Status)) return Status ;
    
    InitString(FboGfHiiHandle, (UINT16)(*((UINT8*)LegacyHiiToken + Offset)), L"%s", DeviceName);
    pBS->FreePool(LegacyHiiToken) ;
    return Status ;
}

/**
    Sync UEFI string to Group form HII handle.

    @param  Offset      
    @param  DeviceName      New string.

    @retval EFI_SUCCESS    Update new string to Group form success.
*/
EFI_STATUS
SyncUefiStringWithGroupFormHii(
    IN UINT16   Offset, 
    IN CHAR16   *DeviceName)
{
    EFI_STATUS Status ;
    EFI_HII_HANDLE FboGfHiiHandle ;
    HII_DEVICE_INFO *UefiHiiToken = NULL ;
    UINTN Size = sizeof(EFI_HII_HANDLE) ;
    UINTN UefiHiiSize = 0 ;
    
    Status = pRS->GetVariable(L"FboGfHiiHandle", &FixedBootOrderGuid, NULL, &Size, &FboGfHiiHandle );
    if (EFI_ERROR(Status)) return Status ;
    
    Status = GetEfiVariable(L"FixedGfHiiUefiDevData", 
                            &FixedBootOrderGuid, 
                            NULL, 
                            &UefiHiiSize, 
                            &UefiHiiToken);
    if (EFI_ERROR(Status)) return Status ;
    
    InitString(FboGfHiiHandle, (UINT16)(*((UINT8*)UefiHiiToken + Offset)), L"%s", DeviceName);
    pBS->FreePool(UefiHiiToken) ;
    return Status ;
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
    
    if(BbsTable == NULL) return;
    
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
   Initialize Legacy Hii String with OriFboLegacyDevOrder variable.

   @retval EFI_SUCCESS    fucntion works successfully.
   @retval !=EFI_SUCCESS  GetEfiVariable failed.
**/
EFI_STATUS
InitFboLegacyHiiString()
{
#if CSM_SUPPORT
    EFI_STATUS Status;
    VOID       *DevOrderBuffer = NULL;
    UINT16     i, j;
    UINTN      BBSNamePtr;
    UINTN      Size = 0;
    CHAR16     DeviceName[0x100+1];
    UINTN      HiiDevDataSize = 0;
    HII_DEVICE_INFO *pHiiDevTokenData = NULL;

    Status = GetEfiVariable(L"OriFboLegacyDevOrder", &FixedBootOrderGuid, NULL, &Size, (VOID**)&DevOrderBuffer);

    if (!EFI_ERROR(Status) && Size > EFI_FIELD_OFFSET(LEGACY_DEVICE_ORDER, Device))
    {
        LEGACY_DEVICE_ORDER *DevOrder;
        HII_DEVICE_INFO *HiiDevOrder;

        Status = GetEfiVariable(L"FixedHiiLegacyDevData", &FixedBootOrderGuid, NULL, &HiiDevDataSize, &pHiiDevTokenData);
        if (EFI_ERROR(Status)) return Status;

        HiiDevOrder = pHiiDevTokenData;
        for (DevOrder = DevOrderBuffer
                        ; (UINT8*)DevOrder < (UINT8*)DevOrderBuffer + Size
                ; DevOrder = (LEGACY_DEVICE_ORDER*)((UINT8*)DevOrder + DevOrder->Length + sizeof(DevOrder->Type)))
        {
            UINT16 SubmenuIndex;

            if(DevOrder->Type == BBS_UNKNOWN) continue;

            SubmenuIndex = SearchHiiMapByType(DevOrder->Type);
            if (pFBOHiiMap[SubmenuIndex].FormID != 0)
            {
                for (HiiDevOrder = pHiiDevTokenData
                                   ; (UINT8*)HiiDevOrder < (UINT8*)pHiiDevTokenData + HiiDevDataSize
                        ; HiiDevOrder = (HII_DEVICE_INFO*)((UINT8*)HiiDevOrder + HiiDevOrder->Length + sizeof(HiiDevOrder->Type)))
                {
                    if (HiiDevOrder->Type == DevOrder->Type)
                        break;
                }
            }

            for (i = 0; i < DEVORDER_COUNT(DevOrder); i++)
            {
                j = DevOrder->Device[i] & FBO_LEGACY_ORDER_MASK; // high byte is used to indicate if it is disabled

                if (pFBOHiiMap[SubmenuIndex].FormID != 0)
                {
                    BBSNamePtr = (UINTN)(((UINTN)BbsTable[j].DescStringSegment << 4)
                                         + BbsTable[j].DescStringOffset);

                    Swprintf(DeviceName, L"%a", BBSNamePtr);
                    ChangeLegacyDeviceOptionName((EFI_HANDLE)BbsTable[j].IBV1,(CHAR16*)&DeviceName) ;//(EIP143657+)
                    InitString(FixedBoot_HiiHandle, HiiDevOrder->StrToken[i], L"%s", DeviceName);

                    DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "BBS DEVICE(%d):%S \n", i, DeviceName));
                }
            }
        }
    }

    if (DevOrderBuffer)
        pBS->FreePool(DevOrderBuffer);
    if (pHiiDevTokenData)
        pBS->FreePool(pHiiDevTokenData);
#endif //#if CSM_SUPPORT
    return EFI_SUCCESS;

}

/**
   Initialize Uefi Hii String with OriFboUefiDevOrder variable.

   @retval EFI_SUCCESS    fucntion works successfully.
   @retval !=EFI_SUCCESS  GetEfiVariable failed.
**/
EFI_STATUS
InitFboUefiHiiString()
{
    EFI_STATUS        Status;
    EFI_GUID          FixedBootOrderGuid = FIXED_BOOT_ORDER_GUID;
    VOID              *DevOrderBuffer = NULL;
    UINT16            i, j;
    UINTN             Size = 0;
    HII_DEVICE_INFO   *pHiiDevTokenData = NULL;
    UINTN             HiiDevDataSize = 0;

    Status = GetEfiVariable(L"OriUefiDevOrder", &FixedBootOrderGuid, NULL, &Size, &DevOrderBuffer);

    if (!EFI_ERROR(Status) && Size > EFI_FIELD_OFFSET(UEFI_DEVICE_ORDER, Device))
    {
        UEFI_DEVICE_ORDER *DevOrder;
        HII_DEVICE_INFO *HiiDevOrder;

        Status = GetEfiVariable(L"FixedHiiUefiDevData", &FixedBootOrderGuid, NULL, &HiiDevDataSize, &pHiiDevTokenData);
        if (EFI_ERROR(Status)) return Status;

        for (DevOrder = DevOrderBuffer
                        ; (UINT8*)DevOrder < (UINT8*)DevOrderBuffer + Size
                ; DevOrder = (UEFI_DEVICE_ORDER*)((UINT8*)DevOrder + DevOrder->Length + sizeof(DevOrder->Type)))
        {
            UINT16 SubmenuIndex;

            SubmenuIndex = SearchHiiMapByType(DevOrder->Type);
            if (pFBOHiiMap[SubmenuIndex].FormID != 0)
            {

                for (HiiDevOrder = pHiiDevTokenData
                                   ; (UINT8*)HiiDevOrder < (UINT8*)pHiiDevTokenData + HiiDevDataSize
                        ; HiiDevOrder = (HII_DEVICE_INFO*)((UINT8*)HiiDevOrder + HiiDevOrder->Length + sizeof(HiiDevOrder->Type)))
                {
                    if (HiiDevOrder->Type == DevOrder->Type)
                        break;
                }
            }

            for (i = 0; i < DEVORDER_COUNT(DevOrder); i++)
            {
                EFI_LOAD_OPTION *BootOption = NULL;

                j = DevOrder->Device[i] & FBO_UEFI_ORDER_MASK; // highest byte is used to indicate if it is disabled

                BootOption = GetUEFIBootName(j);
                if (BootOption)
                {
                    if (pFBOHiiMap[SubmenuIndex].FormID != 0)
                    {
                        CHAR16 *NameStr;

                        NameStr = (CHAR16*)((UINT8*)BootOption + sizeof(EFI_LOAD_OPTION));
                        ChangeUefiDeviceOptionName(BootOption,&NameStr) ;
                        InitString(FixedBoot_HiiHandle, HiiDevOrder->StrToken[i], L"%s", NameStr);

                        DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "UEFI DEVICE(%d):%S \n", i, NameStr));
                    }
                    pBS->FreePool(BootOption);
                }
            }
        }
    }

    if (DevOrderBuffer)
        pBS->FreePool(DevOrderBuffer);
    if (pHiiDevTokenData)
        pBS->FreePool(pHiiDevTokenData);

    return EFI_SUCCESS;
}

//<(EIP101408+)
/**
    Update boot option type string token

        
    @param DevType 
    @param Index 

**/
VOID 
UpdateLegacyDeviceHiiString
(
  IN UINT16 DevType, 
  IN UINT8 Index
)
{
#if CSM_SUPPORT
    EFI_STATUS Status;
    VOID *DevOrderBuffer = NULL;
    UINTN BBSNamePtr;
    UINTN Size = 0;
    CHAR16 DeviceName[0x100+1];
    UINT16 HiiDevMapIndex, HiiDevMapCount;

    HiiDevMapCount = GetHiiMapDataCount();
    for (HiiDevMapIndex = 0; HiiDevMapIndex < HiiDevMapCount; HiiDevMapIndex++)
    {
        if (pFBOHiiMap[HiiDevMapIndex].DevType == DevType)
            break;
    }

    if (Index >= FIXED_BOOT_ORDER_SUBMENU_MAX_NUM)
    {
        UpdateDeviceHiiString(pFBOHiiMap[HiiDevMapIndex].StrToken, NULL);     //(EIP101408+)
    }
    else
    {
        if (LoadedDefault)
        {
            Status = GetEfiVariable(L"DefaultLegacyDevOrder", &DefaultLegacyDevOrderGuid, NULL, &Size, &DevOrderBuffer);
            AdjustLegacyDevOrder(&DevOrderBuffer, &Size);
        }
        else
            Status = GetEfiVariable(L"OriFboLegacyDevOrder", &FixedBootOrderGuid, NULL, &Size, &DevOrderBuffer);

        if (!EFI_ERROR(Status) && Size >= EFI_FIELD_OFFSET(LEGACY_DEVICE_ORDER,Device))
        {
            UINTN j;
            LEGACY_DEVICE_ORDER *DevOrder;

            for (DevOrder = DevOrderBuffer
                            ; (UINT8*)DevOrder < (UINT8*)DevOrderBuffer + Size
                    ; DevOrder = (LEGACY_DEVICE_ORDER*)((UINT8*)DevOrder + DevOrder->Length + sizeof(DevOrder->Type)))
            {

                if (DevOrder->Type != DevType) continue;
                if (DevOrder->Length <= sizeof(DevOrder->Length)) continue;

                j = DevOrder->Device[Index] & FBO_LEGACY_ORDER_MASK; // high byte is used to indicate if it is disabled for LegacyDevOrder

                {
                    char *pStr;
                    UINTN x ;
                    BBSNamePtr = (UINTN)(((UINTN)BbsTable[j].DescStringSegment << 4)
                                         + BbsTable[j].DescStringOffset);

                    pStr = (char *)BBSNamePtr;
                    for (x = 0; x < 10 ; x++)
                    {
                        if (*pStr == ':')
                        {
                            BBSNamePtr += x + 1;
                            break;
                        }
                        else
                            pStr++;
                    }
                    Swprintf(DeviceName, L"%a", BBSNamePtr);
                    ChangeLegacyDeviceOptionName((EFI_HANDLE)BbsTable[j].IBV1,(CHAR16*)&DeviceName) ; 
                    UpdateDeviceHiiString(pFBOHiiMap[HiiDevMapIndex].StrToken, DeviceName);
                }
                break;
            } //for(  DevOrder = DevOrderBuffer..........
        } //if (!EFI_ERROR(Status) && Size>=6)

        if (DevOrderBuffer != NULL)
            pBS->FreePool(DevOrderBuffer);
    }
#endif //#if CSM_SUPPORT
}

/**
    Update boot option type string token

        
    @param DevType 
    @param Index 

**/
void UpdateUefiDeviceHiiString(IN UINT16 DevType, IN UINT8 Index)
{
    EFI_STATUS Status;
    EFI_GUID FixedBootOrderGuid = FIXED_BOOT_ORDER_GUID;
    VOID *DevOrderBuffer = NULL;
    UINTN Size = 0;
    UINT16 HiiDevMapIndex, HiiDevMapCount;

    HiiDevMapCount = GetHiiMapDataCount();
    for (HiiDevMapIndex = 0; HiiDevMapIndex < HiiDevMapCount; HiiDevMapIndex++)
    {
        if (pFBOHiiMap[HiiDevMapIndex].DevType == DevType)
            break;
    }

    if (Index >= FIXED_BOOT_ORDER_SUBMENU_MAX_NUM)
    {
        UpdateDeviceHiiString(pFBOHiiMap[HiiDevMapIndex].StrToken, NULL);         //(EIP101408+)
    }
    else
    {
        if (LoadedDefault)
            Status = GetEfiVariable(L"DefaultUefiDevOrder", &FixedBootOrderGuid, NULL, &Size, &DevOrderBuffer);
        else
            Status = GetEfiVariable(L"OriUefiDevOrder", &FixedBootOrderGuid, NULL, &Size, &DevOrderBuffer);

        if (!EFI_ERROR(Status) && Size >= EFI_FIELD_OFFSET(UEFI_DEVICE_ORDER,Device))
        {
            UINTN j;
            UEFI_DEVICE_ORDER *DevOrder;

            for (DevOrder = DevOrderBuffer
                            ; (UINT8*)DevOrder < (UINT8*)DevOrderBuffer + Size
                    ; DevOrder = (UEFI_DEVICE_ORDER*)((UINT8*)DevOrder + DevOrder->Length + sizeof(DevOrder->Type)))
            {

                if (DevOrder->Type != DevType) continue;
                if (DevOrder->Length <= sizeof(DevOrder->Length)) continue;

                j = DevOrder->Device[Index] & FBO_UEFI_ORDER_MASK;   // [31:24] is disabled flag

                {
                    EFI_LOAD_OPTION *BootOption = NULL;
                    BootOption = GetUEFIBootName((UINT16)j);
                    if (BootOption)
                    {
                        UINT8 *NameStr;

                        NameStr = ((UINT8*)BootOption) + sizeof(EFI_LOAD_OPTION);
                        ChangeUefiDeviceOptionName(BootOption,&NameStr) ;
                        UpdateDeviceHiiString(pFBOHiiMap[HiiDevMapIndex].StrToken, (CHAR16*)NameStr);
                        pBS->FreePool(BootOption);
                    }
                }
                break;
            } //for(  DevOrder = DevOrderBuffer..........
        } //if (!EFI_ERROR(Status) && Size>=6)

        if (DevOrderBuffer != NULL)
            pBS->FreePool(DevOrderBuffer);
    }
}

/**
    Exchange group order on Setup.
    
    @param KeyValue         Indicate which Setup item pressed 
    @param UserSelectValue  Indicate which group changed to
   
    @retval EFI_UNSUPPORTED  KeyValue not for FBO
    @retval EFI_SUCCESS      No need to swap group or succeed to swap
    @retval Status           Error status from HiiLibGetBrowserData call or AllocatePool call
**/
EFI_STATUS FboExchangeGroup(UINT16 KeyValue, UINT16 UserSelectValue)
{
    EFI_STATUS  Status = EFI_SUCCESS;
    EFI_GUID    gSetupGuid = SETUP_GUID;
    UINT16      Swap = 0xffff, MapPri = 0;
    UINT16*     Mapped = NULL;
    UINT16      i = 0, DeviceCount;
    BOOLEAN     Uefi_Boot_item = FALSE;
#if FBO_DUAL_MODE
    BOOLEAN     Dual_Boot_item = FALSE;
#endif
    SETUP_DATA  pSetup;
    SETUP_DATA  *IfrNvData = &pSetup;
    UINTN       BufferSize;
    BufferSize = sizeof(SETUP_DATA);

    //Check Is FixedBootOrder assigned keys?
    if (KeyValue < FixedBootOrderStratKey 
        || KeyValue > (UefiForms_StartKey+FIXED_BOOT_ORDER_TOTAL_DEVICES_MAX_NUM)) 
        return EFI_UNSUPPORTED;

    //--------------------------------------
    //Legacy & Uefi Boot item change process
    //--------------------------------------
#if FBO_DUAL_MODE
    if (KeyValue >= DualBootItemKey)
    {
        DeviceCount = GetDevMapDataCount(pFBODualDevMap);
        Dual_Boot_item = TRUE;
    }
    else
#endif
        if (KeyValue >= UefiBootItemKey)
        {
            DeviceCount = GetDevMapDataCount(pFBOUefiDevMap);
            Uefi_Boot_item = TRUE;
        }
        else
        {
            DeviceCount = GetDevMapDataCount(pFBOLegacyDevMap);
            Uefi_Boot_item = FALSE;
        }

    KeyValue = (KeyValue & 0x7f) + 1;
    //
    // Retrieve uncommitted data from Browser
    //
    Status = HiiLibGetBrowserData(&BufferSize, IfrNvData, &gSetupGuid, L"Setup");
    if (EFI_ERROR(Status))
    {
        DEBUG((FBO_TRACE_LEVEL | DEBUG_ERROR, "[FixedBootOrder.c] HiiLibGetBrowserData (%r)\n", Status));
        return Status;
    }

    DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[FixedBootOrder.c] UserSelectValue=%X\n", UserSelectValue));
    
    Status = pBS->AllocatePool(EfiBootServicesData, DeviceCount * sizeof(UINT16), &Mapped);
    if (EFI_ERROR(Status))
    {
        DEBUG((FBO_TRACE_LEVEL | DEBUG_ERROR, "[FixedBootOrder.c] AllocatePool for Mapped (%r)\n", Status));
        return Status;
    }
    
    // Init all Mapped to FALSE
    for (i = 0; i < DeviceCount; i++)
        Mapped[i] = FALSE;

    // Find duplicated boot order to (Swap)
    for (i = 0; i < DeviceCount; i++)
    {
#if FBO_DUAL_MODE
        if (Dual_Boot_item)
            MapPri = IfrNvData->DualPriorities[i];
        else
#endif
            if (Uefi_Boot_item)
                MapPri = IfrNvData->UefiPriorities[i];
            else
                MapPri = IfrNvData->LegacyPriorities[i];

        DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[FixedBoot] Priorities[%d]=%d\n", i, MapPri));

        if (MapPri == UserSelectValue && i + 1 != KeyValue && UserSelectValue < DeviceCount)
        {
            // i = duplicated index
            Swap = i;
            DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[Fixed Boot] Need to change Priorities[%d]\n", Swap));
        }
#if FIXED_BOOT_INCLUDE_DISABLED_OPTION
        if (MapPri == DeviceCount) continue;     //is Disable?
#endif
        Mapped[MapPri] = TRUE;      // TRUE = priority set
    }

    if (Swap == 0xffff)     // not found duplicated?
    {
        if(Mapped)
            pBS->FreePool(Mapped);            
        return EFI_SUCCESS;
    }

    // Change the duplicated value according to Mapped[]
    for (i = 0; i < DeviceCount; i++)
    {
        if (Mapped[i] == FALSE)     // This priority #i is not set yet
        {
#if FIXED_BOOT_SWAP_POLICY == 0        // just swap the duplicated
#if FBO_DUAL_MODE
            if (Dual_Boot_item)
                IfrNvData->DualPriorities[Swap] = i; // not minus nor plus, just make no duplication
            else
#endif
                if (Uefi_Boot_item)
                    IfrNvData->UefiPriorities[Swap] = i; // not minus nor plus, just make no duplication
                else
                    IfrNvData->LegacyPriorities[Swap] = i; // not minus nor plus, just make no duplication
#endif  //#if FIXED_BOOT_SWAP_POLICY == 0

#if FIXED_BOOT_SWAP_POLICY == 1        // move up or down
            UINT16      Val1;
            UINT16      Val2;
            BOOLEAN     PlusWrap;
            BOOLEAN     MinusWrap;
            UINT16      NewVal;
#if FBO_DUAL_MODE
            if (Dual_Boot_item)
                NewVal = IfrNvData->DualPriorities[Swap];  // new value
            else
#endif
                if (Uefi_Boot_item)
                    NewVal = IfrNvData->UefiPriorities[Swap];
                else
                    NewVal = IfrNvData->LegacyPriorities[Swap];  // new value

            if (MinusWrap = (i == 0 && NewVal == DeviceCount - 1))    // old = 0, new = last, wrapped?
            {
#if FBO_DUAL_MODE
                if (Dual_Boot_item)
                    IfrNvData->DualPriorities[KeyValue-1] = 0;
                else
#endif
                    if (Uefi_Boot_item)
                        IfrNvData->UefiPriorities[KeyValue-1] = 0;
                    else
                        IfrNvData->LegacyPriorities[KeyValue-1] = 0;
            }

            if (PlusWrap = (i == DeviceCount - 1 && NewVal == 0))     // old = last, new = 0, wrapped?
            {
#if FBO_DUAL_MODE
                if (Dual_Boot_item)
                    IfrNvData->DualPriorities[KeyValue-1] = i;
                else
#endif
                    if (Uefi_Boot_item)
                        IfrNvData->UefiPriorities[KeyValue-1] = i;
                    else
                        IfrNvData->LegacyPriorities[KeyValue-1] = i;
            }
            // Prohibit wrapping

            if (KeyValue == 1)      // first one?
            {
                if (MinusWrap)      // wrap?
                    continue;       // skip
                if (NewVal == i - 1)  // -1?
                {
#if FBO_DUAL_MODE
                    if (Dual_Boot_item)
                        IfrNvData->DualPriorities[KeyValue-1] = NewVal + 1;
                    else
#endif
                        if (Uefi_Boot_item)
                            IfrNvData->UefiPriorities[KeyValue-1] = NewVal + 1;
                        else
                            IfrNvData->LegacyPriorities[KeyValue-1] = NewVal + 1;
                    continue;
                }
            }

            if (KeyValue == DeviceCount)        // last one?
            {
                if (PlusWrap)       // wrap?
                    continue;       // skip
                if (NewVal == i + 1)  // +1?
                {
#if FBO_DUAL_MODE
                    if (Dual_Boot_item)
                        IfrNvData->DualPriorities[KeyValue-1] = NewVal - 1;
                    else
#endif
                        if (Uefi_Boot_item)
                            IfrNvData->UefiPriorities[KeyValue-1] = NewVal - 1;
                        else
                            IfrNvData->LegacyPriorities[KeyValue-1] = NewVal - 1;
                    continue;
                }
            }

            if (NewVal == i - 1 || MinusWrap)     // new = old-1 or wrapped?
            {
                // Minus
                {
#if FBO_DUAL_MODE
                    if (Dual_Boot_item)
                    {
                        if (!MinusWrap)
                            IfrNvData->DualPriorities[KeyValue-1]++;   // +1 because it was -1 by "-" key

                        Val1 = IfrNvData->DualPriorities[KeyValue-2];    // previous
                        Val2 = IfrNvData->DualPriorities[KeyValue-1];  // user select value
                        IfrNvData->DualPriorities[KeyValue-2] = Val2;
                        IfrNvData->DualPriorities[KeyValue-1] = Val1;
                    }
                    else
#endif
                        if (Uefi_Boot_item)
                        {
                            if (!MinusWrap)
                                IfrNvData->UefiPriorities[KeyValue-1]++;   // +1 because it was -1 by "-" key

                            Val1 = IfrNvData->UefiPriorities[KeyValue-2];    // previous
                            Val2 = IfrNvData->UefiPriorities[KeyValue-1];  // user select value
                            IfrNvData->UefiPriorities[KeyValue-2] = Val2;
                            IfrNvData->UefiPriorities[KeyValue-1] = Val1;
                        }
                        else
                        {
                            if (!MinusWrap)
                                IfrNvData->LegacyPriorities[KeyValue-1]++;   // +1 because it was -1 by "-" key

                            Val1 = IfrNvData->LegacyPriorities[KeyValue-2];    // previous
                            Val2 = IfrNvData->LegacyPriorities[KeyValue-1];  // user select value
                            IfrNvData->LegacyPriorities[KeyValue-2] = Val2;
                            IfrNvData->LegacyPriorities[KeyValue-1] = Val1;
                        }
                }
            }
            else
            {
                if (NewVal == i + 1 || PlusWrap)              // new = old+1 or wrapped?
                {
                    // Plus
                    {
#if FBO_DUAL_MODE
                        if (Dual_Boot_item)
                        {

                            if (!PlusWrap)
                                IfrNvData->DualPriorities[KeyValue-1]--;      // -1 because it was +1 by "+" key

                            Val1 = IfrNvData->DualPriorities[KeyValue-1];    // user select value
                            Val2 = IfrNvData->DualPriorities[KeyValue];    // next
                            IfrNvData->DualPriorities[KeyValue-1] = Val2;
                            IfrNvData->DualPriorities[KeyValue] = Val1;
                        }
                        else
#endif
                            if (Uefi_Boot_item)
                            {
                                if (!PlusWrap)
                                    IfrNvData->UefiPriorities[KeyValue-1]--;      // -1 because it was +1 by "+" key

                                Val1 = IfrNvData->UefiPriorities[KeyValue-1];    // user select value
                                Val2 = IfrNvData->UefiPriorities[KeyValue];    // next
                                IfrNvData->UefiPriorities[KeyValue-1] = Val2;
                                IfrNvData->UefiPriorities[KeyValue] = Val1;
                            }
                            else
                            {
                                if (!PlusWrap)
                                    IfrNvData->LegacyPriorities[KeyValue-1]--;      // -1 because it was +1 by "+" key

                                Val1 = IfrNvData->LegacyPriorities[KeyValue-1];    // user select value
                                Val2 = IfrNvData->LegacyPriorities[KeyValue];    // next
                                IfrNvData->LegacyPriorities[KeyValue-1] = Val2;
                                IfrNvData->LegacyPriorities[KeyValue] = Val1;
                            }
                    }
                }
                else
                {
#if FBO_DUAL_MODE
                    if (Dual_Boot_item)
                        IfrNvData->DualPriorities[Swap] = i; // not minus nor plus, just make no duplication
                    else
#endif
                        if (Uefi_Boot_item)
                            IfrNvData->UefiPriorities[Swap] = i; // not minus nor plus, just make no duplication
                        else
                            IfrNvData->LegacyPriorities[Swap] = i; // not minus nor plus, just make no duplication
                }
            }
#endif  //#if FIXED_BOOT_SWAP_POLICY == 1
#if FBO_DUAL_MODE
            if (Dual_Boot_item)
                DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[FixedBoot] NEW DualPriorities[%d]=%d\n", Swap, IfrNvData->DualPriorities[Swap]));
            else
#endif
                if (Uefi_Boot_item)
                    DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[FixedBoot] NEW UefiPriorities[%d]=%d\n", Swap, IfrNvData->UefiPriorities[Swap]));
                else
                    DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[FixedBoot] NEW LegacyPriorities[%d]=%d\n", Swap, IfrNvData->LegacyPriorities[Swap]));
            break;
        }

    }

#if EFI_SPECIFICATION_VERSION>0x20000
    Status = HiiLibSetBrowserData(BufferSize, IfrNvData, &gSetupGuid, L"Setup");
    DEBUG((FBO_TRACE_LEVEL | DEBUG_ERROR, "[FixedBootOrder.c] HiiLibSetBrowserData (%r)\n", Status));
#endif
    
    if(Mapped)
        pBS->FreePool(Mapped);

    return Status;
}

#if FBO_SHOW_HIDE_BBS_PRIORITY_SUBMENU
/**
    Adjust the variables related to show/hide the submenu.

    @param  Mode         Indicate which mode to alter
**/
VOID AdjustShowHideSubmenuVariable(UINT8 Mode)
{
    EFI_STATUS        Status;
    UINTN             Size;
    UINT16            *Priorities;
    UINT16            DevCount;
    UINT16            HiiMapIndex;
    UINT16            i;
    EFI_GUID          SetupGuid = SETUP_GUID;  
    FBODevMap         *DevMap = NULL;
    FBOHiiMap         *HiiMap;
    SETUP_DATA        Setup;
    FBO_SUBMENU_SHOW  SubmenuShowVar;
    UINT8             *SubmenuShow = (UINT8*)&SubmenuShowVar;
    EFI_FIXED_BOOT_ORDER_PROTOCOL *FboProtocol; 

    Status = pBS->LocateProtocol(&FixedBootOrderGuid, NULL, &FboProtocol);
    if (EFI_ERROR(Status)) return;

    Size = sizeof(SETUP_DATA);
    Status = HiiLibGetBrowserData(&Size, &Setup, &SetupGuid, L"Setup");
    if (EFI_ERROR(Status))
    {
        Status = pRS->GetVariable(
                    L"Setup",
                    &SetupGuid,
                    NULL,
                    &Size,
                    &Setup);
        if (EFI_ERROR(Status)) return;

        Size = sizeof(FBO_SUBMENU_SHOW);
        Status = pRS->GetVariable(
                    L"FboSubmenuShow",
                    &SetupGuid,
                    NULL,
                    &Size,
                    SubmenuShow);
        if (EFI_ERROR(Status))
        {
            pBS->SetMem(SubmenuShow, sizeof(FBO_SUBMENU_SHOW), 0);
        }
    }
    else
    {
        Size = sizeof(FBO_SUBMENU_SHOW);
        Status = HiiLibGetBrowserData(&Size, SubmenuShow, &SetupGuid, L"FboSubmenuShow");
        if (EFI_ERROR(Status))
        {
            DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[AdjustShowHideSubmenuVariable] HiiLibGetBrowserData() Status %r\n", Status));
            return;
        }
    }

    if (Mode == 2)
    {
#if FBO_DUAL_MODE
        DevMap = FboProtocol->GetDualDevMap();
        Priorities = Setup.DualPriorities;
#endif
    }
    else if (Mode == 1)
    {
        DevMap = FboProtocol->GetUefiDevMap();
        Priorities = Setup.UefiPriorities;
    }
    else if (Mode == 0)
    {
        DevMap = FboProtocol->GetLegacyDevMap();
        Priorities = Setup.LegacyPriorities;
    }
    else
    {
        DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[AdjustShowHideSubmenuVariable] Invalid mode\n"));
        return;
    }
    if (!DevMap) return;

    DevCount = GetDevMapDataCount(DevMap);

    HiiMap = FboProtocol->GetHiiMap();
    if (!HiiMap) return;

    //
    //  Reset fields to 0 regarding to the mode.
    //
    for (i = 0; i < DevCount; i++)
    {
        if ((HiiMapIndex = SearchHiiMapByType(DevMap[i].DevType)) != 0xFFFF)
            SubmenuShow[HiiMap[HiiMapIndex].DeviceCountOffset] = 0; // Hide this submenu
    }

    for (i = 0; i < DevCount; i++)
    {
        if (Priorities[i] >= DevCount)
            continue;

        if ((HiiMapIndex = SearchHiiMapByType(DevMap[Priorities[i]].DevType)) != 0xFFFF)
            SubmenuShow[HiiMap[HiiMapIndex].DeviceCountOffset] = 1; // Show this submenu
    }

    Status = HiiLibSetBrowserData(sizeof(FBO_SUBMENU_SHOW), SubmenuShow, &SetupGuid, L"FboSubmenuShow");
    if (EFI_ERROR(Status)) {
        Status = pRS->SetVariable(
                    L"FboSubmenuShow",
                    &SetupGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    sizeof(FBO_SUBMENU_SHOW),
                    SubmenuShow);
        if (EFI_ERROR(Status))
        {
            DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[ShowHideBbsPrioritySubmenu] Set FboSubmenuShow Status = %r\n", Status));
        }
    }
}

/**
    Show/Hide the submenu of BBS PRIORITY.

    @note If HiiLibGetBrowserData() call failed then it should be 
          on StringInit(), otherwise on callback().
**/
VOID ShowHideBbsPrioritySubmenu()
{
    UINTN       Size;
    SETUP_DATA  Setup;
    EFI_GUID    SetupGuid = SETUP_GUID;  
    EFI_STATUS  Status;

    Size = sizeof(SETUP_DATA);
    Status = HiiLibGetBrowserData(&Size, &Setup, &SetupGuid, L"Setup");
    if (!EFI_ERROR(Status))
    {
        AdjustShowHideSubmenuVariable(Setup.BootMode);
    }
    else
    {
        //
        //  0, 1, and 2 means boot mode.
        //
        AdjustShowHideSubmenuVariable(0);
        AdjustShowHideSubmenuVariable(1);
        AdjustShowHideSubmenuVariable(2);
    }
}

/**
    Fixed Boot Order BootMode callback Function.

    @param HiiHandle - Hii Handle
    @param Class - Class
    @param SubClass - SubClass
    @param KeyValue - KeyValue

    @retval EFI_SUCCESS           action is EFI_BROWSER_ACTION_CHANGING or EFI_BROWSER_ACTION_CHANGED
    @retval EFI_UNSUPPORTED       action is EFI_BROWSER_ACTION_DEFAULT_STANDARD
    @retval EFI_INVALID_PARAMETER action except EFI_BROWSER_ACTION_CHANGING and
                                  EFI_BROWSER_ACTION_DEFAULT_STANDARD and EFI_BROWSER_ACTION_CHANGED
**/
EFI_STATUS FboBootModeFormCallback(
    IN EFI_HII_HANDLE HiiHandle,
    IN UINT16 Class,
    IN UINT16 SubClass,
    IN UINT16 KeyValue
)
{

    CALLBACK_PARAMETERS *CallbackValues = GetCallbackParameters();

#if (EFI_SPECIFICATION_VERSION >= 0x2000A)
    if (CallbackValues->Action == EFI_BROWSER_ACTION_CHANGING)
        return EFI_SUCCESS;
    else if (CallbackValues->Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD)
        return EFI_UNSUPPORTED;
    else if (CallbackValues->Action != EFI_BROWSER_ACTION_CHANGED)
        return EFI_INVALID_PARAMETER;
#endif


    ShowHideBbsPrioritySubmenu();
    return EFI_SUCCESS;
}
#endif

#if FBO_RUNTIME_CALLBACK_REGISTRATION
/**
    Fixed Boot Order Setup callback Function.

    @param CallbackValues - Callback variable.

    @retval EFI_SUCCESS - Success
**/
EFI_STATUS FixedBootOrderFormCallBack(
    IN AMI_HII_CALLBACK_PARAMETERS *CallbackValues
)
#else
/**
    Fixed Boot Order Setup callback Function.

    @param HiiHandle - Hii Handle
    @param Class - Class
    @param SubClass - SubClass
    @param KeyValue - KeyValue

    @retval EFI_SUCCESS - Success
**/
EFI_STATUS FixedBootOrderFormCallBack(
    IN EFI_HII_HANDLE HiiHandle,
    IN UINT16 Class,
    IN UINT16 SubClass,
    IN UINT16 KeyValue
)
#endif
{
#if FBO_RUNTIME_CALLBACK_REGISTRATION
    UINT16      KeyValue;
#endif
    EFI_STATUS  Status;

    if (gHiiSkipFBOModule) return EFI_SUCCESS;

#if FBO_RUNTIME_CALLBACK_REGISTRATION
    KeyValue  = (UINT16)(CallbackValues->QuestionId);
#else
    CallbackValues = GetCallbackParameters();
    KeyValue = CallbackValues->KeyValue;
#endif

#if (EFI_SPECIFICATION_VERSION >= 0x2000A)
    if (CallbackValues->Action == EFI_BROWSER_ACTION_CHANGING)
        return EFI_SUCCESS;
    else if (CallbackValues->Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD)
        return EFI_UNSUPPORTED;
    else if (CallbackValues->Action != EFI_BROWSER_ACTION_CHANGED)
        return EFI_INVALID_PARAMETER;
#endif

    DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[FixedBootOrder.c] ***** INTERACTIVE callback.  KEY=%X\n", KeyValue));

    Status = FboExchangeGroup(KeyValue, CallbackValues->Value->u16);
#if FBO_SHOW_HIDE_BBS_PRIORITY_SUBMENU
    ShowHideBbsPrioritySubmenu();
#endif
    
    return Status;
}

/**
    Each group device name Hii String Update, and fixed HiiDB Oneof
    Data.

    @retval EFI_SUCCESS - SUCCESS
**/
EFI_STATUS
InitLegacyGroupDefault
(
  VOID
)
{
#if CSM_SUPPORT
    EFI_STATUS Status;
    VOID *DevOrderBuffer = NULL;
    UINT16 i, j, DevMapIndex;
    UINTN BBSNamePtr;
    UINTN Size = 0;
    UINT8 DeviceItem = 0, DisableItem = 0;
    CHAR16 DeviceName[0x100+1];
    HII_DEVICE_INFO *pHiiDevTokenData = NULL;
    UINTN HiiDevDataSize = 0;

    if (LoadedDefault)
    {
        Status = GetEfiVariable(L"DefaultLegacyDevOrder", &DefaultLegacyDevOrderGuid, NULL, &Size, &DevOrderBuffer);
        AdjustLegacyDevOrder(&DevOrderBuffer, &Size);
    }
    else
        Status = GetEfiVariable(L"FboLegacyDevOrder", &FixedBootOrderGuid, NULL, &Size, &DevOrderBuffer);

    if (!EFI_ERROR(Status) && Size > EFI_FIELD_OFFSET(LEGACY_DEVICE_ORDER, Device))
    {
        LEGACY_DEVICE_ORDER *DevOrder;
        HII_DEVICE_INFO *HiiDevOrder;

        Status = GetEfiVariable(L"FixedHiiLegacyDevData", &FixedBootOrderGuid, NULL, &HiiDevDataSize, &pHiiDevTokenData);
        if (EFI_ERROR(Status)) return Status;

        HiiDevOrder = pHiiDevTokenData;
        for (DevOrder = DevOrderBuffer
                        ; (UINT8*)DevOrder < (UINT8*)DevOrderBuffer + Size
                ; DevOrder = (LEGACY_DEVICE_ORDER*)((UINT8*)DevOrder + DevOrder->Length + sizeof(DevOrder->Type)))
        {
            UINT16 SubmenuIndex;
            DeviceItem = 0;
            DisableItem = 0;

            if(DevOrder->Type == BBS_UNKNOWN) continue;

            SubmenuIndex = SearchHiiMapByType(DevOrder->Type);
            DevMapIndex = SearchDevMapByType(pFBOLegacyDevMap, DevOrder->Type);
            if(SubmenuIndex == 0xFFFF || DevMapIndex == 0xFFFF)
                continue;
            if (pFBOHiiMap[SubmenuIndex].FormID != 0)
            {
                for (HiiDevOrder = pHiiDevTokenData
                                   ; (UINT8*)HiiDevOrder < (UINT8*)pHiiDevTokenData + HiiDevDataSize
                        ; HiiDevOrder = (HII_DEVICE_INFO*)((UINT8*)HiiDevOrder + HiiDevOrder->Length + sizeof(HiiDevOrder->Type)))
                {
                    if (HiiDevOrder->Type == DevOrder->Type)
                        break;
                }
            }
            if((UINT8*)HiiDevOrder >= (UINT8*)pHiiDevTokenData + HiiDevDataSize) continue;

            for (i = 0; i < DEVORDER_COUNT(DevOrder); i++)
            {
                j = DevOrder->Device[i] & FBO_LEGACY_ORDER_MASK; // high byte is used to indicate if it is disabled

                if (pFBOHiiMap[SubmenuIndex].FormID != 0)
                {
                    BBSNamePtr = (UINTN)(((UINTN)BbsTable[j].DescStringSegment << 4)
                                         + BbsTable[j].DescStringOffset);

                    Swprintf(DeviceName, L"%a", BBSNamePtr);
                    ChangeLegacyDeviceOptionName((EFI_HANDLE)BbsTable[j].IBV1,(CHAR16*)&DeviceName) ;//(EIP143657+)
                    ASSERT(HiiDevOrder->StrToken[i] > 0x100) ;
                    if (HiiDevOrder->StrToken[i] > 0x100)
                    {
                        // Hii index checking.
                        InitString(FixedBoot_HiiHandle, HiiDevOrder->StrToken[i], L"%s", DeviceName);
                        SyncLegacyStringWithGroupFormHii((UINT16)((UINT8*)(&HiiDevOrder->StrToken[i]) - (UINT8*)pHiiDevTokenData), DeviceName) ;
                        DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "InitLegacyGroupDefault :: BBS DEVICE(%d):%S \n", i, DeviceName));
                    }
                }
#if FIXED_BOOT_INCLUDE_DISABLED_OPTION
                if (DevOrder->Device[i] & FBO_LEGACY_DISABLED_MASK)   // for BBS indexes in LegacyDevOrder, high byte = 0xff means disabled
                {
                    TypeLegacyDeviceOptions[DevMapIndex][DeviceItem+DisableItem] = FIXED_BOOT_ORDER_SUBMENU_MAX_NUM;
                    DisableItem++;
                }
                else
#endif
                {
                    TypeLegacyDeviceOptions[DevMapIndex][DeviceItem+DisableItem] = DeviceItem + DisableItem;
                    DeviceItem++;
                }
                if ((DeviceItem + DisableItem) >= FIXED_BOOT_ORDER_SUBMENU_MAX_NUM) break;

            }

            DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[FixedBoot] DeviceItem=%d DisableItem=%d \n", DeviceItem, DisableItem));

#if FIXED_BOOT_DISPLAY_DEVICE_NAME_IN_SETUP
            if (DeviceItem)
                UpdateLegacyDeviceHiiString(DevOrder->Type, 0);
            else
                UpdateLegacyDeviceHiiString(DevOrder->Type, FIXED_BOOT_ORDER_SUBMENU_MAX_NUM);       //no device, show type
#endif

            TypeLegacyDeviceCount[DevMapIndex] = DeviceItem;
#if FIXED_BOOT_INCLUDE_DISABLED_OPTION
            if (DisableItem)
                TypeLegacyDeviceCount[DevMapIndex] += DisableItem;
#endif
            DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[FixedBoot] Type(%x)DevCount=%x\n", DevMapIndex, TypeLegacyDeviceCount[DevMapIndex]));
        }
    }

    if (DevOrderBuffer)
        pBS->FreePool(DevOrderBuffer);
    if (pHiiDevTokenData)
        pBS->FreePool(pHiiDevTokenData);
#endif //#if CSM_SUPPORT
    return EFI_SUCCESS;
}

/**
    Each group device name Hii String Update, and fixed HiiDB Oneof
    Data.

    @retval EFI_STATUS
**/
EFI_STATUS
InitUefiGroupDefault
(
  VOID
)
{
    EFI_STATUS      Status;
    EFI_GUID        FixedBootOrderGuid = FIXED_BOOT_ORDER_GUID;
    VOID            *DevOrderBuffer = NULL;
    UINT16          i, j, DevMapIndex;
    UINTN           Size = 0;
    UINT8           DeviceItem = 0, DisableItem = 0;
    HII_DEVICE_INFO     *pHiiDevTokenData = NULL;
    UINTN           HiiDevDataSize = 0;

    if (LoadedDefault)
        Status = GetEfiVariable(L"DefaultUefiDevOrder", &FixedBootOrderGuid, NULL, &Size, &DevOrderBuffer);
    else
        Status = GetEfiVariable(L"UefiDevOrder", &FixedBootOrderGuid, NULL, &Size, &DevOrderBuffer);

    if (!EFI_ERROR(Status) && Size > EFI_FIELD_OFFSET(UEFI_DEVICE_ORDER, Device))
    {
        UEFI_DEVICE_ORDER *DevOrder;
        HII_DEVICE_INFO *HiiDevOrder;

        Status = GetEfiVariable(L"FixedHiiUefiDevData", &FixedBootOrderGuid, NULL, &HiiDevDataSize, &pHiiDevTokenData);
        if (EFI_ERROR(Status)) return Status;

        for (DevOrder = DevOrderBuffer
                        ; (UINT8*)DevOrder < (UINT8*)DevOrderBuffer + Size
                ; DevOrder = (UEFI_DEVICE_ORDER*)((UINT8*)DevOrder + DevOrder->Length + sizeof(DevOrder->Type)))
        {
            UINT16 SubmenuIndex;
            DeviceItem = 0;
            DisableItem = 0;

            SubmenuIndex = SearchHiiMapByType(DevOrder->Type);
            DevMapIndex = SearchDevMapByType(pFBOUefiDevMap, DevOrder->Type);
            if(SubmenuIndex == 0xFFFF || DevMapIndex == 0xFFFF)
                continue;
            if (pFBOHiiMap[SubmenuIndex].FormID != 0)
            {

                for (HiiDevOrder = pHiiDevTokenData
                                   ; (UINT8*)HiiDevOrder < (UINT8*)pHiiDevTokenData + HiiDevDataSize
                        ; HiiDevOrder = (HII_DEVICE_INFO*)((UINT8*)HiiDevOrder + HiiDevOrder->Length + sizeof(HiiDevOrder->Type)))
                {
                    if (HiiDevOrder->Type == DevOrder->Type)
                        break;
                }
            }
            if((UINT8*)HiiDevOrder >= (UINT8*)pHiiDevTokenData + HiiDevDataSize) continue;

            for (i = 0; i < DEVORDER_COUNT(DevOrder); i++)
            {
                EFI_LOAD_OPTION *BootOption = NULL;

                j = DevOrder->Device[i] & FBO_UEFI_ORDER_MASK; // highest byte is used to indicate if it is disabled

                BootOption = GetUEFIBootName(j);
                if (BootOption)
                {
                    if (pFBOHiiMap[SubmenuIndex].FormID != 0)
                    {
                        CHAR16 *NameStr;
                        NameStr = (CHAR16*)((UINT8*)BootOption + sizeof(EFI_LOAD_OPTION));
                        ChangeUefiDeviceOptionName(BootOption,&NameStr) ;
                        ASSERT(HiiDevOrder->StrToken[i] > 0x100) ;
                        if (HiiDevOrder->StrToken[i] > 0x100)
                        {
                            // Hii index checking.
                            InitString(FixedBoot_HiiHandle, HiiDevOrder->StrToken[i], L"%s", NameStr);
                            SyncUefiStringWithGroupFormHii((UINT16)((UINT8*)(&HiiDevOrder->StrToken[i]) - (UINT8*)pHiiDevTokenData), NameStr) ;
                            DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "UEFI DEVICE(%d):%S \n", i, NameStr));
                        }
                    }
                    pBS->FreePool(BootOption);
                }
#if FIXED_BOOT_INCLUDE_DISABLED_OPTION
                if (DevOrder->Device[i] & FBO_UEFI_DISABLED_MASK)   // for UefiDevOrder, highest byte = 0xff means disabled
                {
                    TypeUefiDeviceOptions[DevMapIndex][DeviceItem+DisableItem] = FIXED_BOOT_ORDER_SUBMENU_MAX_NUM;
                    DisableItem++;
                }
                else
#endif
                {
                    TypeUefiDeviceOptions[DevMapIndex][DeviceItem+DisableItem] = DeviceItem + DisableItem;
                    DeviceItem++;
                }
                if ((DeviceItem + DisableItem) >= FIXED_BOOT_ORDER_SUBMENU_MAX_NUM) break;
            }
            DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[FixedBoot] DeviceItem=%d DisableItem=%d \n", DeviceItem, DisableItem));

#if FIXED_BOOT_DISPLAY_DEVICE_NAME_IN_SETUP
            if (DeviceItem)
                UpdateUefiDeviceHiiString(DevOrder->Type, 0);
            else
                UpdateUefiDeviceHiiString(DevOrder->Type, FIXED_BOOT_ORDER_SUBMENU_MAX_NUM);     //no device, show type
#endif

            TypeUefiDeviceCount[DevMapIndex] = DeviceItem;
#if FIXED_BOOT_INCLUDE_DISABLED_OPTION
            if (DisableItem)
                TypeUefiDeviceCount[DevMapIndex] += DisableItem;
#endif
            DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[FixedBoot] Type(%x)DevCount=%x\n", DevMapIndex, TypeUefiDeviceCount[DevMapIndex]));
        }
    }

    if (DevOrderBuffer)
        pBS->FreePool(DevOrderBuffer);
    if (pHiiDevTokenData)
        pBS->FreePool(pHiiDevTokenData);

    return EFI_SUCCESS;
}
/**
    Get Supported Language from List

    @param LanguageList - Language List
    @retval Language - Language supported
**/
EFI_STATUS
GetSupportedLanguage
(
    IN  EFI_HII_HANDLE   HiiHandle,
    IN  CHAR8*           LanguageList,
    OUT CHAR8**          SupportedLang
)
{
    CHAR8       *TempLanguage = LanguageList, *pLanguage = LanguageList;
    UINTN       Size = 0x100;
    CHAR16      BootOptionStr[0x100+1];
    EFI_STATUS  Status;
    
    if (pHiiString == NULL)
    {
        Status = pBS->LocateProtocol(&gEfiHiiStringProtocolGuid,
                                     NULL, 
                                     (VOID **) & pHiiString);
        if (EFI_ERROR(Status))
            return Status;
    }
    
    while (*pLanguage != 0)
    {
        if((*pLanguage == ';'))
        {
            CHAR8    TempChar = *pLanguage;
            
            *pLanguage = 0;
            Status = pHiiString->GetString(pHiiString, 
                                           TempLanguage, 
                                           HiiHandle, 
                                           STRING_TOKEN(STR_BOOT_OPTION), 
                                           BootOptionStr, &Size, NULL);
            
            if(!EFI_ERROR(Status))
            {
                // Supported Language found, dont need pLanguage anymore
                Status = pBS->AllocatePool(EfiBootServicesData, 
                                           Strlen(TempLanguage) + 1, 
                                           SupportedLang);
                // If fail, return Orginal pointer
                if(EFI_ERROR(Status))
                {
                    *pLanguage = TempChar;
                    return Status;
                }
                
                pBS->CopyMem(*SupportedLang, 
                             TempLanguage, 
                             Strlen(TempLanguage) + 1);
                *pLanguage = TempChar;
                return EFI_SUCCESS;
            }
            *pLanguage = TempChar;
            TempLanguage = pLanguage + 1;
        }
        pLanguage++;
    }
    
    Status = pHiiString->GetString(
                         pHiiString, 
                         TempLanguage, 
                         HiiHandle, 
                         STRING_TOKEN(STR_BOOT_OPTION), 
                         BootOptionStr, 
                         &Size, 
                         NULL);
    if(EFI_ERROR(Status))
        return Status;

    Status = pBS->AllocatePool(EfiBootServicesData, Strlen(TempLanguage) + 1, SupportedLang);
    if(EFI_ERROR(Status))
        return Status;
    
    pBS->CopyMem(*SupportedLang, TempLanguage, Strlen(TempLanguage) + 1);
    // The Last Language, just return the status
    return EFI_SUCCESS;
}
//(EIP101408+)>
/**
    Install multi-language for STR_BOOT_OPTION.

        
    @param HiiHandle - Hii Handle
    @param BootOptionNumber - BootOption Number
    
    @retval STRING_REF - String reference

**/
STRING_REF
HiiAddLanguageBootOptionString
(
  IN EFI_HII_HANDLE HiiHandle, 
  IN UINT16 BootOptionNumber
)
{
    EFI_STATUS Status;
    CHAR8* Languages = NULL;
    CHAR8 *CurrentLanguage = NULL;
    CHAR8* OrgLanguages = NULL;
    UINTN LangSize = 0;
    BOOLEAN LastLanguage = FALSE;
    EFI_STRING_ID  StringId = 0;

    UINTN  Size = 0;
    CHAR16 BootOptionStr[0x100+1];
    CHAR16 BootStr[0x100+1];

    if (pHiiString == NULL)
    {
        Status = pBS->LocateProtocol(&gEfiHiiStringProtocolGuid, NULL, (VOID **) & pHiiString);
        if (EFI_ERROR(Status))
            return 0;
    }

    Status = pHiiString->GetLanguages(pHiiString, HiiHandle, Languages, &LangSize);
    if (Status == EFI_BUFFER_TOO_SMALL)
    {
        Status = pBS->AllocatePool(EfiBootServicesData, LangSize, &Languages);
        if (EFI_ERROR(Status))
            return 0;        //not enough resources to allocate string
        Status = pHiiString->GetLanguages(pHiiString, HiiHandle, Languages, &LangSize);
    }
    if (EFI_ERROR(Status))
        return 0;

    // CurrentLanguage contains the individual language code inside Languages
    Status = pBS->AllocatePool(EfiBootServicesData, LangSize, &CurrentLanguage);
    if (EFI_ERROR(Status))
        return 0;


    OrgLanguages = Languages;
    while (!LastLanguage)
    {
        UINTN lc = 0;

        // Copy language code to CurrentLanguage
        while (*Languages != ';' && *Languages != 0 && lc < LangSize)
        {
            CurrentLanguage[lc++] = *Languages;
            Languages++;
        }
        CurrentLanguage[lc] = 0;

        if (*Languages == 0)    // last language in language list
            LastLanguage = TRUE;

        Size = 0x100;
        Status = pHiiString->GetString(pHiiString, 
                                       CurrentLanguage, 
                                       HiiHandle, 
                                       STRING_TOKEN(STR_BOOT_OPTION), 
                                       BootOptionStr, &Size, NULL);
        if (EFI_ERROR(Status))   // Language not defined in .uni
        {
            CHAR8*  LangBuffer = NULL;
            // Get support in OrgLanguages
            Status = GetSupportedLanguage(HiiHandle, OrgLanguages, &LangBuffer);
            if(!EFI_ERROR(Status))
            {
                Status = pHiiString->GetString(
                         pHiiString, LangBuffer, HiiHandle, 
                         STRING_TOKEN(STR_BOOT_OPTION), 
                         BootOptionStr, &Size, NULL);
                pBS->FreePool(LangBuffer);
            }
            if (EFI_ERROR(Status))
            {
                Languages++;    // skip ';'
                continue;
            }
        }

        Swprintf(BootStr, BootOptionStr, BootOptionNumber);

        if (StringId == 0)
            Status = pHiiString->NewString(
                                 pHiiString, HiiHandle, &StringId, 
                                 CurrentLanguage, NULL, BootStr, NULL);
        else
            Status = pHiiString->SetString(
                                 pHiiString, HiiHandle, StringId, 
                                 CurrentLanguage, BootStr, NULL);
            
        Languages++;
        if (EFI_ERROR(Status))
        {
            pBS->FreePool(OrgLanguages);
            return 0;
        }
    }

    if (OrgLanguages)
        pBS->FreePool(OrgLanguages);

    if (CurrentLanguage)
        pBS->FreePool(CurrentLanguage);

    return StringId;
}
//<(EIP101408+)

/**
    Create legacy boot item device Hii Item data.

    @param HiiHandle - HiiHandle
    
    @retval EFI_STATUS

**/
EFI_STATUS
InitialLegacyDevHiiToken(EFI_HANDLE HiiHandle)
{
#if CSM_SUPPORT
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_GUID FixedBootOrderGuid = FIXED_BOOT_ORDER_GUID;
    UINTN DevOrderSize = 0, HiiDevInfoSize = 0;
    STRING_REF Strid = 0;
    HII_DEVICE_INFO *pHiiDevTokenData = NULL, *pHiiDevTokenData2 = NULL;
    LEGACY_DEVICE_ORDER *DevOrder = NULL, *DevOrderBuffer = NULL;
    
    if (pFBOHiiMap == NULL)
    {
        DEBUG((FBO_TRACE_LEVEL | DEBUG_ERROR, "pFBOHiiMap == NULL\n"));
        ASSERT(pFBOHiiMap);
    }
    if (pHiiString == NULL)
    {
        Status = pBS->LocateProtocol(&gEfiHiiStringProtocolGuid, NULL, &pHiiString);
        if (EFI_ERROR(Status)) return Status ;
    }
    
    Status = GetEfiVariable(L"FboLegacyDevOrder", &FixedBootOrderGuid, NULL, &DevOrderSize, &DevOrderBuffer);
    if (EFI_ERROR(Status) || DevOrderSize <= EFI_FIELD_OFFSET(LEGACY_DEVICE_ORDER,Device))  // any order existed?
        return Status;

    pHiiDevTokenData2 = pHiiDevTokenData = MallocZ(DevOrderSize);
    if (pHiiDevTokenData == NULL) return EFI_OUT_OF_RESOURCES;

    for (DevOrder = DevOrderBuffer ; 
         (UINT8*)DevOrder < (UINT8*)DevOrderBuffer + DevOrderSize ; 
         DevOrder = (LEGACY_DEVICE_ORDER*)((UINT8*)DevOrder + DevOrder->Length + sizeof(DevOrder->Type)))
    {
        UINT16 i;

        if(DevOrder->Type == BBS_UNKNOWN) continue;

        pHiiDevTokenData->Type = DevOrder->Type;
        pHiiDevTokenData->Length = sizeof(pHiiDevTokenData->Length) + DEVORDER_COUNT(DevOrder) * sizeof(pHiiDevTokenData->StrToken);
                
        DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "pHiiDevTokenData->Length:%X",pHiiDevTokenData->Length)) ;
        //Create BootOption item string token ID.
        for (i = 0; i < DEVORDER_COUNT(DevOrder); i++)
        {
            if (i >= FIXED_BOOT_ORDER_SUBMENU_MAX_NUM) break;
            //
            // This string would be updated later by InitLegacyGroup.
            //
            Status = NewStringWithFirstFoundLanguage(HiiHandle, &Strid, L" ");
            if (EFI_ERROR(Status)) return Status;
            
            pHiiDevTokenData->StrToken[i] = Strid;
        }
        pHiiDevTokenData = (HII_DEVICE_INFO*)((UINT8*)pHiiDevTokenData + pHiiDevTokenData->Length + sizeof(pHiiDevTokenData->Type));
    }
    
    HiiDevInfoSize = (UINT16)((UINT8*)pHiiDevTokenData - (UINT8*)pHiiDevTokenData2);
    if (HiiDevInfoSize)
    {
        pRS->SetVariable(L"FixedHiiLegacyDevData",
                         &FixedBootOrderGuid,
                         EFI_VARIABLE_BOOTSERVICE_ACCESS,
                         HiiDevInfoSize,
                         pHiiDevTokenData2);
    }
    pBS->FreePool(pHiiDevTokenData2);
#endif
    return EFI_SUCCESS;
}


/**
    Create legacy boot item device Hii Item data.

    @param HiiHandle - HiiHandle
    
    @retval EFI_STATUS

**/
EFI_STATUS
InitialUefiDevHiiToken(EFI_HANDLE HiiHandle)
{
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_GUID FixedBootOrderGuid = FIXED_BOOT_ORDER_GUID;
    UINTN DevOrderSize = 0, HiiDevInfoSize = 0;
    STRING_REF Strid = 0;
    HII_DEVICE_INFO *pHiiDevTokenData = NULL, *pHiiDevTokenData2 = NULL;
    UEFI_DEVICE_ORDER *DevOrder = NULL, *DevOrderBuffer = NULL;
    
    if (pFBOHiiMap == NULL)
    {
        DEBUG((FBO_TRACE_LEVEL | DEBUG_ERROR, "pFBOHiiMap == NULL\n"));
        ASSERT(pFBOHiiMap);
    }
    if (pHiiString == NULL)
    {
        Status = pBS->LocateProtocol(&gEfiHiiStringProtocolGuid, NULL, &pHiiString);
        if (EFI_ERROR(Status)) return Status ;
    }

     Status = GetEfiVariable(L"UefiDevOrder", &FixedBootOrderGuid, NULL, &DevOrderSize, &DevOrderBuffer);
    if (EFI_ERROR(Status) || DevOrderSize <= EFI_FIELD_OFFSET(UEFI_DEVICE_ORDER,Device))  // any order existed?
        return Status;

    pHiiDevTokenData2 = pHiiDevTokenData = MallocZ(DevOrderSize);
    if (pHiiDevTokenData == NULL) return EFI_OUT_OF_RESOURCES;

    for (DevOrder = DevOrderBuffer ; 
         (UINT8*)DevOrder < (UINT8*)DevOrderBuffer + DevOrderSize ; 
         DevOrder = (UEFI_DEVICE_ORDER*)((UINT8*)DevOrder + DevOrder->Length + sizeof(DevOrder->Type)))
    {
        UINT16 i;

        pHiiDevTokenData->Type = DevOrder->Type;
        pHiiDevTokenData->Length = sizeof(pHiiDevTokenData->Length) + DEVORDER_COUNT(DevOrder) * sizeof(pHiiDevTokenData->StrToken);
                
        DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "pHiiDevTokenData->Length:%X",pHiiDevTokenData->Length)) ;
        //Create BootOption item string token ID.
        for (i = 0; i < DEVORDER_COUNT(DevOrder); i++)
        {
            if (i >= FIXED_BOOT_ORDER_SUBMENU_MAX_NUM) break;
            //
            // This string would be updated later by InitLegacyGroup.
            //
            Status = NewStringWithFirstFoundLanguage(HiiHandle, &Strid, L" ");
            if (EFI_ERROR(Status)) return Status;
            
            pHiiDevTokenData->StrToken[i] = Strid;
        }
        pHiiDevTokenData = (HII_DEVICE_INFO*)((UINT8*)pHiiDevTokenData + pHiiDevTokenData->Length + sizeof(pHiiDevTokenData->Type));
    }
    
    HiiDevInfoSize = (UINT16)((UINT8*)pHiiDevTokenData - (UINT8*)pHiiDevTokenData2);
    if (HiiDevInfoSize)
    {
        pRS->SetVariable(L"FixedHiiUefiDevData",
                         &FixedBootOrderGuid,
                         EFI_VARIABLE_BOOTSERVICE_ACCESS,
                         HiiDevInfoSize,
                         pHiiDevTokenData2);
    }
    pBS->FreePool(pHiiDevTokenData2);
    return EFI_SUCCESS;
}

/*
    Fbo callback which called from the TSE.

    @param  Action   For what to do
    @param  Context  Variable for the Action

    @retval EFI_SUCCESS      Action is working
    @retval EFI_UNSUPPORTED  Unknown action
*/
EFI_STATUS FboCallback(UINTN Action, VOID *Context)
{
#if FBO_SHOW_HIDE_BBS_PRIORITY_SUBMENU
    if (Action == FBO_CALLBACK_SHOW_HIDE_BBS_PRIORITY_SUBMENU)
    {
        BOOLEAN *BrowserCallbackEnable;
        BOOLEAN OrignalState;
        if (Context)
        {
            BrowserCallbackEnable = (BOOLEAN*)Context;
            OrignalState = *BrowserCallbackEnable;

            *BrowserCallbackEnable = TRUE;
            ShowHideBbsPrioritySubmenu();
            *BrowserCallbackEnable = OrignalState;

            return EFI_SUCCESS;
        }
    }
#endif

    return EFI_UNSUPPORTED;
}

/**
    This function called after setup utility loaded config defaults
    based on user input. change fixedbootorder display default
    devcie string.

    @param LoadDefaultString - Load Default Flag

    @retval EFI_STATUS

**/
EFI_STATUS 
load_default_string()
{
    DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[FixedBootOrder.c] Load Default String\n"));

    LoadedDefault = TRUE;

    //Initial Legacy Device HiiString name.
    InitLegacyGroup();
    //Initial Uefi Device HiiString.
    InitUefiGroup();

    return EFI_SUCCESS;
}

/**
    Elink to roll some specially previous values back for FixedBootOrder. 

    @param TseFixedBoot  FixedBoot variable from TSE. 

    @retval EFI_SUCCESS            Function works successfully
    @retval EFI_INVALID_PARAMETER  Parameter TseFixedBoot is NULL
    @return GetEfiVariable error

**/
EFI_STATUS 
load_previous_string
(
  VOID    *TseFixedBoot,
  BOOLEAN LoadedDefaultAndSaved
)
{
    FIXED_BOOT_SETUP     *FixedBoot = (FIXED_BOOT_SETUP*)TseFixedBoot;

    DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "[FixedBootOrder.c] Load Previous String\n"));

    //
    // Setup data had been loaded back and needs to sync with
    // TypeUefiDeviceOptions and TypeUefiDeviceOptions variables
    // for correct operation of exchanging items.
    //
    if(FixedBoot)
    {
        MemCpy(TypeUefiDeviceOptions, FixedBoot->UefiDevice
                ,FIXED_BOOT_ORDER_GROUP_MAX_NUM * FIXED_BOOT_ORDER_SUBMENU_MAX_NUM * sizeof(FixedBoot->UefiDevice[0]));
#if CSM_SUPPORT
        MemCpy(TypeLegacyDeviceOptions, FixedBoot->LegacyDevice
                ,FIXED_BOOT_ORDER_GROUP_MAX_NUM * FIXED_BOOT_ORDER_SUBMENU_MAX_NUM * sizeof(FixedBoot->LegacyDevice[0]));
#endif
    }
    else
    {
        DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "FixedBootOrder.c(load_previous_string):: FixedBoot is NULL\n"));
        return EFI_INVALID_PARAMETER;
    }

    //
    // if loaded default before, it had initialized the strings
    // by default variable, and here to roll them back.
    //
    if(LoadedDefault && !LoadedDefaultAndSaved) // loaded default but not save yet.
    {
        InitFboUefiHiiString();
#if CSM_SUPPORT
        InitFboLegacyHiiString();
#endif

        LoadedDefault = FALSE;
    }
    
#if FIXED_BOOT_DISPLAY_DEVICE_NAME_IN_SETUP
    {
        UINTN                Size = 0;
        EFI_STATUS           Status;
        UEFI_DEVICE_ORDER    *UefiGroup, *UefiDevOrder = NULL;
        UINT16               DevMapIndex;
        //
        // If user loaded default without save, then it should not 
        // load previous values by default variable, otherwise it
        // fails to load previous valued at first boot.
        //
        if(LoadedDefaultAndSaved)
        {
            Status = GetEfiVariable(L"DefaultUefiDevOrder", &FixedBootOrderGuid, NULL, &Size, &UefiDevOrder);
        }
        else
        {
            Status = GetEfiVariable(L"OriUefiDevOrder", &FixedBootOrderGuid, NULL, &Size, &UefiDevOrder);
        }

        if(EFI_ERROR(Status))
            return Status;

        for (UefiGroup = UefiDevOrder
            ; (UINT8*)UefiGroup < (UINT8*)UefiDevOrder + Size
            ; UefiGroup = (UEFI_DEVICE_ORDER*)((UINT8*)UefiGroup
                    + UefiGroup->Length + sizeof(UefiGroup->Type)))
        {
            DevMapIndex = SearchDevMapByType(pFBOUefiDevMap, UefiGroup->Type);
            if(DevMapIndex == 0xFFFF)
                continue;
            UpdateUefiDeviceHiiString(UefiGroup->Type, TypeUefiDeviceOptions[DevMapIndex][0]);
        }

        if(UefiDevOrder)
            pBS->FreePool(UefiDevOrder);

#if CSM_SUPPORT

        {
            LEGACY_DEVICE_ORDER  *LegacyGroup, *LegacyDevOrder = NULL;

            if(LoadedDefaultAndSaved)
            {
                Status = GetEfiVariable(L"DefaultLegacyDevOrder", &DefaultLegacyDevOrderGuid, NULL, &Size, &LegacyDevOrder);
                AdjustLegacyDevOrder(&LegacyDevOrder, &Size);
            }
            else
            {
                Status = GetEfiVariable(L"OriFboLegacyDevOrder", &FixedBootOrderGuid, NULL, &Size, (VOID**)&LegacyDevOrder);
            }

            if(EFI_ERROR(Status))
                    return Status;

            for (LegacyGroup = LegacyDevOrder
                ; (UINT8*)LegacyGroup < (UINT8*)LegacyDevOrder + Size
                ; LegacyGroup = (LEGACY_DEVICE_ORDER*)((UINT8*)LegacyGroup
                        + LegacyGroup->Length + sizeof(LegacyGroup->Type)))
            {
                DevMapIndex = SearchDevMapByType(pFBOLegacyDevMap, LegacyGroup->Type);
                if(DevMapIndex == 0xFFFF)
                    continue;
                UpdateLegacyDeviceHiiString(LegacyGroup->Type, TypeLegacyDeviceOptions[DevMapIndex][0]);
            }

            if(LegacyDevOrder)
                pBS->FreePool(LegacyDevOrder);
        }
#endif //CSM_SUPPORT
    }
#endif //FIXED_BOOT_DISPLAY_DEVICE_NAME_IN_SETUP

    return EFI_SUCCESS;
}

static EFI_FIXED_BOOT_ORDER_SETUP_PROTOCOL gFixedBootOrderSetupProtocol =
{
    load_default_string,
    load_previous_string,
    FboCallback,
    UpdateLegacyDeviceHiiString,
    UpdateUefiDeviceHiiString
};

/**
    Set Hii Skip FBO Module Flag

**/
VOID
HiiSetSkipFBOModuleFlag
(
  VOID
)
{
    EFI_STATUS Status;
    UINTN  Size = 0;
    UINT32 Attr;
    UINT8 *Flag = NULL ;
    Status = GetEfiVariable(L"CurrentSkipFboModule", &FixedBootOrderGuid, &Attr, &Size, &Flag);
    if (!EFI_ERROR(Status))
    {
        if (*Flag) gHiiSkipFBOModule = 1 ;
        else gHiiSkipFBOModule = 0 ;
        pBS->FreePool(Flag) ;
    }
    else
        gHiiSkipFBOModule = 0 ;
}

/**
    Update first device name in setup.

    @param  IsUefi  Choose to update the uefi/legacy part
    @param  Name    Name to get update varialbe
    @param  Guid    Guid to get update variable
**/
VOID
UpdateFirstDeviceNameInSetup(
    IN BOOLEAN  IsUefi,
    IN CHAR16   *Name,
    IN EFI_GUID *Guid
)
{
    EFI_STATUS Status;
    VOID *DevOrderBuffer = NULL;
    UINTN Size = 0;
    UINT16 HiiDevMapIndex;
    UINT16 HiiDevMapCount = GetHiiMapDataCount();

    Status = GetEfiVariable(Name, Guid, NULL, &Size, &DevOrderBuffer);
    if (EFI_ERROR(Status)) return;
    
    if (IsUefi)
    {
        UEFI_DEVICE_ORDER *DevOrder;

        if (Size < EFI_FIELD_OFFSET(UEFI_DEVICE_ORDER,Device))
        {
            pBS->FreePool(DevOrderBuffer);
            return;
        }

        for (DevOrder = (UEFI_DEVICE_ORDER*)DevOrderBuffer
                        ; (UINT8*)DevOrder < (UINT8*)DevOrderBuffer + Size
                ; DevOrder = (UEFI_DEVICE_ORDER*)((UINT8*)DevOrder + DevOrder->Length + sizeof(DevOrder->Type)))
        {

            if (DevOrder->Length <= sizeof(DevOrder->Length)) continue;

            for (HiiDevMapIndex = 0; HiiDevMapIndex < HiiDevMapCount; HiiDevMapIndex++)
            {
                if (pFBOHiiMap[HiiDevMapIndex].DevType == DevOrder->Type)
                    break;
            }
            if (HiiDevMapCount == HiiDevMapIndex) continue;

            if (!gDisplayDeviceName 
                    || DevOrder->Device[0] & FBO_UEFI_DISABLED_MASK)
            {
                UpdateDeviceHiiString(pFBOHiiMap[HiiDevMapIndex].StrToken, NULL);
                continue;
            }
                
            {
                EFI_LOAD_OPTION *BootOption = NULL;
                BootOption = GetUEFIBootName((UINT16)DevOrder->Device[0]);
                if (BootOption)
                {
                    UINT8 *NameStr;

                    NameStr = ((UINT8*)BootOption) + sizeof(EFI_LOAD_OPTION);
                    ChangeUefiDeviceOptionName(BootOption,&NameStr) ;
                    UpdateDeviceHiiString(pFBOHiiMap[HiiDevMapIndex].StrToken, (CHAR16*)NameStr);
                    pBS->FreePool(BootOption);
                }
            }
        }
    }
    else
    {
#if CSM_SUPPORT
        LEGACY_DEVICE_ORDER *DevOrder;
        CHAR16 DeviceName[0x100+1];
        UINTN BBSNamePtr;

        if (Size < EFI_FIELD_OFFSET(LEGACY_DEVICE_ORDER,Device))
        {
            pBS->FreePool(DevOrderBuffer);
            return;
        }

        for (DevOrder = (LEGACY_DEVICE_ORDER*)DevOrderBuffer
                        ; (UINT8*)DevOrder < (UINT8*)DevOrderBuffer + Size
                ; DevOrder = (LEGACY_DEVICE_ORDER*)((UINT8*)DevOrder + DevOrder->Length + sizeof(DevOrder->Type)))
        {

            if (DevOrder->Length <= sizeof(DevOrder->Length)) continue;

            for (HiiDevMapIndex = 0; HiiDevMapIndex < HiiDevMapCount; HiiDevMapIndex++)
            {
                if (pFBOHiiMap[HiiDevMapIndex].DevType == DevOrder->Type)
                    break;
            }
            if (HiiDevMapCount == HiiDevMapIndex) continue;

            if (!gDisplayDeviceName 
                    || DevOrder->Device[0] & FBO_LEGACY_DISABLED_MASK)
            {
                UpdateDeviceHiiString(pFBOHiiMap[HiiDevMapIndex].StrToken, NULL);
                continue;
            }

            {
                char *pStr;
                UINTN x ;
                BBSNamePtr = (UINTN)(((UINTN)BbsTable[DevOrder->Device[0]].DescStringSegment << 4)
                                     + BbsTable[DevOrder->Device[0]].DescStringOffset);

                pStr = (char *)BBSNamePtr;
                for (x = 0; x < 10 ; x++)
                {
                    if (*pStr == ':')
                    {
                        BBSNamePtr += x + 1;
                        break;
                    }
                    else
                        pStr++;
                }
                Swprintf(DeviceName, L"%a", BBSNamePtr);
                ChangeLegacyDeviceOptionName((EFI_HANDLE)BbsTable[DevOrder->Device[0]].IBV1,(CHAR16*)&DeviceName) ; 
                UpdateDeviceHiiString(pFBOHiiMap[HiiDevMapIndex].StrToken, DeviceName);
            }
        }
#endif
    }

    pBS->FreePool(DevOrderBuffer);

}

#if FIXED_BOOT_DISPLAY_DEVICE_NAME_IN_SETUP    
/**
    Remove device name in Setup.

    @param  Event    Event be signaled
    @param  Context  Context to pass in
**/
VOID
RemoveDeviceNameInSetup(
    IN EFI_EVENT        Event,
    IN VOID             *Context
)
{
    gDisplayDeviceName = FALSE;

    if(LoadedDefault)
    {
        //
        //  It should not be here just in case, since it should reboot if load-default happens.
        //
        UpdateFirstDeviceNameInSetup(FALSE, L"DefaultLegacyDevOrder", &DefaultLegacyDevOrderGuid);
        UpdateFirstDeviceNameInSetup(TRUE, L"DefaultUefiDevOrder", &FixedBootOrderGuid);
    }
    else
    {
        //
        //  Why not Ori-variables (ex. OriUefiDevOrder)?
        //  Ori-variables should cope with FixedBoot variables, but it seems that we can't get
        //  the right contents of FixedBoot variable here.
        //
        UpdateFirstDeviceNameInSetup(FALSE, L"FboLegacyDevOrder", &FixedBootOrderGuid);
        UpdateFirstDeviceNameInSetup(TRUE, L"UefiDevOrder", &FixedBootOrderGuid);
    }
}

/**
    Show device name in Setup.

    @param  Event    Event be signaled
    @param  Context  Context to pass in
**/
VOID
UpdateDeviceNameInSetup (
    IN EFI_EVENT        Event,
    IN VOID             *Context
)
{
    gDisplayDeviceName = TRUE;

    if(LoadedDefault)
    {
        //
        //  It should not be here just in case, since it should reboot if load-default happens.
        //
        UpdateFirstDeviceNameInSetup(FALSE, L"DefaultLegacyDevOrder", &DefaultLegacyDevOrderGuid);
        UpdateFirstDeviceNameInSetup(TRUE, L"DefaultUefiDevOrder", &FixedBootOrderGuid);
    }
    else
    {
        //
        //  Why not Ori-variables (ex. OriUefiDevOrder)?
        //  Ori-variables should cope with FixedBoot variables, but it seems that we can't get
        //  the right contents of FixedBoot variable here.
        //
        UpdateFirstDeviceNameInSetup(FALSE, L"FboLegacyDevOrder", &FixedBootOrderGuid);
        UpdateFirstDeviceNameInSetup(TRUE, L"UefiDevOrder", &FixedBootOrderGuid);
    }
}
#endif

/**
    String Init function.

        
    @param HiiHandle - HiiHandle 
    @param Class - Class

    @retval EFI_STATUS

**/
EFI_STATUS FixedBootOrderStringInit(
    IN EFI_HII_HANDLE   HiiHandle,
    IN UINT16       Class
)
{
#if CSM_SUPPORT
    EFI_GUID gEfiLegacyBiosProtocolGuid = EFI_LEGACY_BIOS_PROTOCOL_GUID;
    EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios = NULL;
#endif
    EFI_GUID gSetupGuid = SETUP_GUID;
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_FIXED_BOOT_ORDER_PROTOCOL *pFBO = NULL;
    FIXED_BOOT_GROUP FixedBootGroup;
    UINT8 *pFixedBootGroup = (UINT8*) & FixedBootGroup;
    UINT8 i, j;
    UINT16 QuestionKey = FixedBootOrderStratKey;
    BOOLEAN FormSetFlag = FALSE ;
    EFI_GUID FboGroupFormFormSetGuid = FBO_GROUP_FORM_FORM_SET_GUID ;
    
    HiiSetSkipFBOModuleFlag() ;
    if (gHiiSkipFBOModule) return EFI_SUCCESS;
    // if (Class != FBO_STRING_INIT_FORM_SET_CLASS) return EFI_SUCCESS;
    
    for ( i = 0 ; FboSetupPageInform[i].FormSet ; i++ )
        if ( Class == FboSetupPageInform[i].FormSet ) 
            FormSetFlag = TRUE ;
    if (!FormSetFlag) return EFI_SUCCESS;
    
    pRS->SetVariable(
        L"FixedBootOrderHii",
        &FixedBootOrderGuid,
        EFI_VARIABLE_BOOTSERVICE_ACCESS,
        sizeof(EFI_HII_HANDLE),
        &HiiHandle);

    DEBUG((FBO_TRACE_LEVEL | DEBUG_VERBOSE, "FixedBootOrderStringInit HiiHanlde = %x\n", HiiHandle));
    FixedBoot_HiiHandle = HiiHandle;

#if CSM_SUPPORT
    Status = pBS->LocateProtocol(&gEfiLegacyBiosProtocolGuid, NULL, &LegacyBios);
    if (!EFI_ERROR(Status))
    {

        Status = LegacyBios->GetBbsInfo(LegacyBios, &HddCount, &HddInfo, &BbsCount, &BbsTable);
        if (EFI_ERROR(Status)) return Status;
    }
#endif
    Status = pBS->LocateProtocol(&FixedBootOrderGuid, NULL, &pFBO);
    if (!EFI_ERROR(Status))
    {
        pFBO->SetHiiMap(FBOHiiMapData) ;
        pFBO->SetBootOptionTokenMap(BootOptionStringToken) ; //(EIP123284+)
        
        pFBOLegacyDevMap = pFBO->GetLegacyDevMap();
        pFBOUefiDevMap = pFBO->GetUefiDevMap();
        pFBODualDevMap = pFBO->GetDualDevMap();
        pFBOHiiMap = pFBO->GetHiiMap();
        FBO_AdjustGroupName() ;
        //(EIP123284+)>
#if FBO_USE_BOOT_OPTION_NUMBER_UNI
        pBootOptionTokenMap = pFBO->GetBootOptionTokenMap();
#else
        pBootOptionTokenMap = NULL;
#endif
        //<(EIP123284+)
    }

    pBS->SetMem(TypeLegacyDeviceCount, sizeof(TypeLegacyDeviceCount), 0);
    pBS->SetMem(TypeUefiDeviceCount, sizeof(TypeUefiDeviceCount), 0);
    pBS->SetMem(&FixedBootGroup, sizeof(FIXED_BOOT_GROUP), 0);

#if CSM_SUPPORT
    //Initial Legacy Device HiiString name.
    InitialLegacyDevHiiToken(HiiHandle) ;
    InitLegacyGroup();
#endif
    
    //Initial Uefi Device HiiString.
    InitialUefiDevHiiToken(HiiHandle) ;
    InitUefiGroup();

#if FBO_RUNTIME_CALLBACK_REGISTRATION
    for( ; QuestionKey < LegacyForms_StartKey ; QuestionKey++)
    {
        Status = AmiSetupRegisterCallbackHandler(QuestionKey, FixedBootOrderFormCallBack); 
        if(EFI_ERROR(Status))
            DEBUG((FBO_TRACE_LEVEL | DEBUG_ERROR, "Register Setup Callback Error at Key : %X\n", LegacyBootItemKey + i));
    }
#endif

    //Set all types device count and set variable FixedBootGroup.
    {
        UINT16 DevMapCount;
        UINT16 HiiDevIndex;
        UINT16 DeviceCountOffset;


        DevMapCount = GetDevMapDataCount(pFBOLegacyDevMap);
        for (i = 0; i < DevMapCount; i++)
        {
            HiiDevIndex = SearchHiiMapByType(pFBOLegacyDevMap[i].DevType);
            if (HiiDevIndex == 0xffff) continue ;
            if (pFBOHiiMap[HiiDevIndex].FormID == 0) continue;  //no group submenu
            DeviceCountOffset = pFBOHiiMap[HiiDevIndex].DeviceCountOffset;
            pFixedBootGroup[DeviceCountOffset] = TypeLegacyDeviceCount[i];
        }

        DevMapCount = GetDevMapDataCount(pFBOUefiDevMap);
        for (i = 0; i < DevMapCount; i++)
        {
            HiiDevIndex = SearchHiiMapByType(pFBOUefiDevMap[i].DevType);
            if (HiiDevIndex == 0xffff) continue ;
            if (pFBOHiiMap[HiiDevIndex].FormID == 0) continue;  //no group submenu
            DeviceCountOffset = pFBOHiiMap[HiiDevIndex].DeviceCountOffset;
            pFixedBootGroup[DeviceCountOffset] = TypeUefiDeviceCount[i];
        }

        pRS->SetVariable(
            L"FixedBootGroup",
            &gSetupGuid,
            VARIABLE_ATTRIBUTES,
            sizeof(FIXED_BOOT_GROUP),
            &FixedBootGroup);

    }

    //Initial ALL "FixedBoot" data.
    {
        FIXED_BOOT_SETUP pFixedBoot;

        for (i = 0; i < FIXED_BOOT_ORDER_GROUP_MAX_NUM; i++)
            for (j = 0; j < FIXED_BOOT_ORDER_SUBMENU_MAX_NUM; j++)
                pFixedBoot.LegacyDevice[(i*FIXED_BOOT_ORDER_SUBMENU_MAX_NUM)+j] = TypeLegacyDeviceOptions[i][j];
        for (i = 0; i < FIXED_BOOT_ORDER_GROUP_MAX_NUM; i++)
            for (j = 0; j < FIXED_BOOT_ORDER_SUBMENU_MAX_NUM; j++)
                pFixedBoot.UefiDevice[(i*FIXED_BOOT_ORDER_SUBMENU_MAX_NUM)+j] = TypeUefiDeviceOptions[i][j];

        Status = pRS->SetVariable(L"FixedBoot",
                                  &FboGroupFormFormSetGuid,
#if FboSce_SUPPORT
                                  VARIABLE_ATTRIBUTES,
#else
                                  BOOT_NV_ATTRIBUTES,
#endif
                                  sizeof(pFixedBoot),
                                  &pFixedBoot);
        if(EFI_ERROR(Status))
        {
            // Delete Variable
            Status = pRS->SetVariable(L"FixedBoot",
                                      &FboGroupFormFormSetGuid,
                                      0,
                                      0,
                                      NULL);
            // Set again
            Status = pRS->SetVariable(L"FixedBoot",
                                      &FboGroupFormFormSetGuid,
#if FboSce_SUPPORT
                                      VARIABLE_ATTRIBUTES,
#else
                                      BOOT_NV_ATTRIBUTES,
#endif
                                      sizeof(pFixedBoot),
                                      &pFixedBoot);            
        }
    }
    
    {
        EFI_HANDLE Handle = NULL;
        EFI_GUID FixedBootOrderSetupGuid = FIXED_BOOT_ORDER_SETUP_GUID;

        Status = pBS->InstallProtocolInterface(
                     &Handle,
                     &FixedBootOrderSetupGuid,
                     EFI_NATIVE_INTERFACE,
                     &gFixedBootOrderSetupProtocol);
    }
#if FIXED_BOOT_DISPLAY_DEVICE_NAME_IN_SETUP    
    {
        EFI_STATUS                  Status = EFI_SUCCESS;
        EFI_EVENT                   UpdateStringCallbackEvent, RemoveStringCallbackEvent;
        VOID                        *UpdateStringCallbackReg, *RemoveStringCallbackReg;
        EFI_GUID                    BeforeBootGuid = AMITSE_EVENT_BEFORE_BOOT_GUID; 
        EFI_GUID                    AfterBootGuid = AMITSE_EVENT_AFTER_BOOT_GUID; 

        Status = pBS->CreateEventEx(
            EVT_NOTIFY_SIGNAL, TPL_NOTIFY, 
            RemoveDeviceNameInSetup,
            &RemoveStringCallbackReg, &gEfiEventLegacyBootGuid,
            &RemoveStringCallbackEvent
        );
        if (EFI_ERROR(Status))
        {
            DEBUG((FBO_TRACE_LEVEL | DEBUG_ERROR, "Register RemoveDeviceNameInGroup call on LegacyBoot Event failed\n"));
        }

        Status = RegisterProtocolCallback (
                                &BeforeBootGuid,
                                RemoveDeviceNameInSetup,
                                NULL,
                                &RemoveStringCallbackEvent,
                                &RemoveStringCallbackReg );
        if (EFI_ERROR(Status))
        {
            DEBUG((FBO_TRACE_LEVEL | DEBUG_ERROR, "Register RemoveDeviceNameInGroup call on BeforeBootGuid failed\n"));
        }

        Status = RegisterProtocolCallback (
                                    &AfterBootGuid,
                                    UpdateDeviceNameInSetup,
                                    NULL,
                                    &UpdateStringCallbackEvent,
                                    &UpdateStringCallbackReg );
        if (EFI_ERROR(Status))
        {
            DEBUG((FBO_TRACE_LEVEL | DEBUG_ERROR, "Register UpdateDeviceNameInGroup call on AfterBootGuid failed\n"));
        }
    }
#endif

#if FBO_SHOW_HIDE_BBS_PRIORITY_SUBMENU
    ShowHideBbsPrioritySubmenu();
#endif

    return Status;
}

