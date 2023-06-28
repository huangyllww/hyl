//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file SataDevInfo.h
    Header file the SataDevInfo has the function definition and
    Sata Setup related definition 
     
**/

#ifndef _SATA_SETUP_VAR_H
#define _SATA_SETUP_VAR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <Token.h>

#define SATA_DEVICE_INFO_MAIN_FORM_ID            0x1
#define SATA_DEVICE_INFO_MAIN_FORM_LABEL_START   0x1000
#define SATA_DEVICE_INFO_MAIN_FORM_LABEL_END     0x1001

#if DISPLAY_HDD_FEATURE_INFO

#define SATA_DEVICE_INFO_SUB_FORM_LABEL_START    0x2000
#define SATA_DEVICE_INFO_SUB_FORM_LABEL_END      0x2001
#define SATA_DEVICE_INFO_SUB_FORM_ID             0x2
#define SATA_SETUP_DEVICE_BASE_KEY               0x200
#define AUTOMATIC_ACOUSTIC_FEATURE_SET_SUPPORTED 0x0200
#define OEM_HDD_SN_LENGTH                        20    //<Lvshh001-20181113 Add HDD SN & FW info>
#define OEM_HDD_FIRMWARE_VERSION_LENGTH          8     //<Lvshh001-20181113 Add HDD SN & FW info>

#ifndef VFRCOMPILE
EFI_STATUS 
EFIAPI
SataDevInfoCallback(
    CONST EFI_HII_CONFIG_ACCESS_PROTOCOL*, 
    EFI_BROWSER_ACTION, 
    EFI_QUESTION_ID, 
    UINT8,
    EFI_IFR_TYPE_VALUE*, 
    EFI_BROWSER_ACTION_REQUEST*
    );
#endif
#endif

#define SATA_DEVICE_INFO_FORM_SET_GUID \
        { 0x5d9265f7, 0xe3ec, 0x4be1, { 0xa9, 0x95, 0x85, 0xd8, 0x60, 0xa5, 0xa4, 0x2e } }
#define SATA_DEVICE_INFO_FORM_SET_CLASS         0xE1
#ifndef VFRCOMPILE
#define PCI_VID_DID                                 0x0000
typedef struct {
    UINT8           BusNum;
    UINT8           DevNum;
    UINT8           FunNum;
    UINT8           SegNum;
    UINT8           MaxPortSupported;
    EFI_HANDLE      *DeviceHandle;
    EFI_STRING_ID   ControllerStrId;
    EFI_STRING_ID   *gHddNameStrId;
    EFI_STRING_ID   *gHddSizeStrId;
    EFI_STRING_ID   *gSataPortStrId;
    LIST_ENTRY      SataControllerLink;
} SATA_CONTROLLER_INFO;

EFI_STATUS
EFIAPI
SataDeviceInfoEntryPoint (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
);

VOID 
EFIAPI
InitSataDynamicIfrContents (
    IN EFI_EVENT        Event,
    IN VOID             *Context
);

EFI_STATUS GetSataControllerDetails();

EFI_STATUS
GetDeviceInfo (
    IN EFI_HANDLE               *ControllerHandle,
    IN SATA_CONTROLLER_INFO     *SataControllerInterface
);

VOID 
SwapEntries(
    IN CHAR8            *Data,
    IN UINT16           Size
);

EFI_STRING_ID
SataHiiAddString (
    IN  EFI_HII_HANDLE  HiiHandle,
    IN  CHAR16 *        String
);

EFI_STRING_ID
SataHiiSetString (
    IN EFI_HII_HANDLE   HiiHandle,
    IN CHAR16           *String,
    IN EFI_STRING_ID    StringId
);

EFI_STRING_ID
SataHiiAddStringInternal (
    IN  EFI_HII_HANDLE  HiiHandle,
    IN  CHAR16 *        String,
    IN  EFI_STRING_ID   StringId
);

#endif

#ifdef __cplusplus
}
#endif
#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
