//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**           5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093      **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file SataDevInfo.c
    This file contains functions to update the SataController and device information 
    for the setup page 

**/
#include <AmiDxeLib.h>
#include <Protocol/PciIo.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/IdeControllerInit.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HiiLib.h>
#include <Library/AmiHiiUpdateLib.h>
#include <Guid/MdeModuleHii.h>
#include <Protocol/HiiString.h>
#include <IndustryStandard/AmiAtaAtapi.h>
#include "SataDevInfo.h"
#include "SataControllerNameELink.h"
#include <Library/MemoryAllocationLib.h>//<Lvshh001-20181113 Add HDD SN & FW info>

#define _48_BIT_ADDRESS_FEATURE_SET_SUPPORTED   0x0400
#define ATAPI_DEVICE                            0x8000
EFI_GUID gSataConfigFormSetGuid                 = SATA_DEVICE_INFO_FORM_SET_GUID;
static EFI_HII_STRING_PROTOCOL *HiiString       = NULL;
static CHAR8 *SupportedLanguages                = NULL;
EFI_HANDLE  gSataHiiHandle;
static gSataInforInit = FALSE; //<xuyj1001-20200408 Support ASM1061R Device +>
typedef struct {
    UINT16      VendorID;
    UINT16      DeviceID;
    CHAR8       *ControllerName;
} OEM_CONTROLLER_NAME_LIST;

LIST_ENTRY     SataDeviceInfoList;
OEM_CONTROLLER_NAME_LIST SataContollerNameUpdatelist[] = {
    OEM_CONTROLLER_NAME_ELINK_LIST
    {0xFFFF, 0xFFFF, 0}
};

#if DISPLAY_HDD_FEATURE_INFO
//<Lvshh001-20181113 Add HDD SN & FW info +>
VOID
TrimSpace (
  IN  CHAR8  *Data,
  IN  UINT16 Size
  )
/*++
Routine Description:
  Trim space from CHAR8 data header and tail.

Arguments:
  Data  -  The data to swap.
  Size  -  Size of data to swap.

Returns:
  None.
--*/
{
  UINT16  Index;
  CHAR8   *TempBuffer = NULL;
  CHAR8   *TempBufferPtr = NULL;

  if (Data == NULL)
    return;

  //Remove the SPACE CHARACTER from the string header
  if (Data[0] == ' ') {
    TempBufferPtr = TempBuffer = AllocateZeroPool(Size);
    CopyMem(TempBuffer, Data, Size);
    Index = 0;
    while ((*TempBuffer == ' ') && (Index<Size)){
      Index++;
      TempBuffer++;
    }
    ZeroMem(Data, Size);
    CopyMem(Data, TempBuffer, Size-Index);

    if (TempBuffer != NULL) {
      FreePool(TempBufferPtr);
      TempBufferPtr = TempBuffer = NULL;
    }
  }

  //Remove the SPACE CHARACHER from the string tail end
  for (Index = Size; Index > 0; Index--) {
    if ( (Data[Index] != ' ')&&(Data[Index] != '\0') )
      break;

    if (Data[Index] == ' ')
      Data[Index] = '\0';
  }

  return ;
}
//<Lvshh001-20181113 Add HDD SN & FW info ->

EFI_HII_CONFIG_ACCESS_PROTOCOL CallBack = { NULL, NULL, SataDevInfoCallback };
CALLBACK_INFO SetupCallBack[] = {
    // Last field in every structure will be filled by the Setup
    { &gSataConfigFormSetGuid, &CallBack, SATA_DEVICE_INFO_FORM_SET_CLASS, 0, 0},
};

UINTN            gSataDeviceBaseKey = SATA_SETUP_DEVICE_BASE_KEY;
CHAR16           DeviceInfo[70]     = L"";
UINT8            MaxPort            = 0;
EFI_STRING_ID    SecurityInfoId;
EFI_STRING_ID    SmartInfoId;
EFI_STRING_ID    SecurityStatusId;
EFI_STRING_ID    SmartStatusId;
EFI_STRING_ID    OpalInfoId;
EFI_STRING_ID    AcousticInfoId;
EFI_STRING_ID    AcousticStatusId;
EFI_STRING_ID    HddNameStringId;
EFI_STRING_ID    HddSizeStringId;
EFI_STRING_ID    HddSNStringId;//<Lvshh001-20181113 Add HDD SN & FW info>
EFI_STRING_ID    HddFWStringId;//<Lvshh001-20181113 Add HDD SN & FW info>

//<xuyj1-20180904 add HDD Speed for ATA Devices+ >
EFI_STRING_ID    SpeedInfoId;
#define PCI_CFG_ADDR(bus,dev,func,reg) \
                        ((VOID*)(UINTN) (PCIEX_BASE_ADDRESS + \
                        ((bus) << 20) + ((dev) << 15) + \
                        ((func) << 12) + reg))
//<xuyj1-20180904 add HDD Speed for ATA Devices- >
/**
    @internal
  
    Displays additional details about Hard disc attached to the controller in a subform. 

    @param      UINT8   Device
    @param      UINT8   Controller
    
    @retval     None

    @endinternal
**/

VOID
SataDevInfoDynamicSubFormContents(
    UINT8    Device,
    UINT8    Controller
)
{
    EFI_STATUS              Status;
    SATA_CONTROLLER_INFO    *SataControllerInterface = NULL;
    EFI_HANDLE              DeviceHandle;
    IDENTIFY_DATA           *IdentifyData = NULL;
    EFI_DISK_INFO_PROTOCOL  *DiskInfo;
    UINT32                  BufferSize = 0;
    UINT16                  SecurityStatus = 0;
    UINT16                  SmartStatus = 0;
    UINT16                  OpalStatus = 0;
    UINT8                   AcousticLevel = 0;
    LIST_ENTRY              *Link;
    VOID                    *StartOpCodeHandle;
    VOID                    *EndOpCodeHandle;
    EFI_IFR_GUID_LABEL      *StartLabel;
    EFI_IFR_GUID_LABEL      *EndLabel;
    CHAR16                  *DeviceString;
    EFI_IDENTIFY_DATA       *IdentifyDriveInfo = NULL;//<Lvshh001-20181113 Add HDD SN & FW info>
    UINT32                  Size = 0;//<Lvshh001-20181113 Add HDD SN & FW info>
    CHAR8                   HddSN[OEM_HDD_SN_LENGTH+2];//<Lvshh001-20181113 Add HDD SN & FW info>
    CHAR8                   HddFirmwareVersion[OEM_HDD_FIRMWARE_VERSION_LENGTH+2];//<Lvshh001-20181113 Add HDD SN & FW info>
    //<xuyj1-20180904 add HDD Speed for ATA Devices+ >
    UINT32                   PciAbar;
    UINT8                    CurrentPortSpeed=0;
    //<xuyj1-20180904 add HDD Speed for ATA Devices- >
    //<Libj-20201112 Add Chinese language of SATA options +>
    EFI_STRING              Str = NULL;
    //<Libj-20201112 Add Chinese language of SATA options ->
    
    Status = pBS->AllocatePool(EfiBootServicesData, 0x100, &DeviceString);
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return;
    }
    
    StartOpCodeHandle = HiiAllocateOpCodeHandle ();
    EndOpCodeHandle = HiiAllocateOpCodeHandle ();
    
    // Create Hii Extended Label OpCode as the start and end opcode
    StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (
            StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
    
    StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
 
    EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (
            EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
    
    EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;

    StartLabel->Number = SATA_DEVICE_INFO_SUB_FORM_LABEL_START;
    EndLabel->Number = SATA_DEVICE_INFO_SUB_FORM_LABEL_END;
    
    // Getting the interface of the controller to which the device is connected and printing the 
    // device informations and status like Hdd, SMART and OPAL etc.
    for (Link = SataDeviceInfoList.ForwardLink; Controller != 0; Link = Link->ForwardLink,Controller--); 
    
    SataControllerInterface = BASE_CR(Link,SATA_CONTROLLER_INFO,SataControllerLink );
    
    // Get the DiskInfo interface for the Device and using Identify command get
    // the security, Opal, Acoustic and SMART status
    DeviceHandle =  SataControllerInterface->DeviceHandle[Device];
    Status = pBS->HandleProtocol(DeviceHandle,
                      &gEfiDiskInfoProtocolGuid,
                      &DiskInfo
                      );
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return;
    }
    
    // Allocate the Memory for the Device Identify Data.
    Status = pBS->AllocatePool(EfiBootServicesData, sizeof(IDENTIFY_DATA), &IdentifyData);
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return;
    }            
    pBS->SetMem(IdentifyData, sizeof(IDENTIFY_DATA), 0);
    BufferSize = sizeof(IDENTIFY_DATA);
     
    Status = DiskInfo->Identify(
                         DiskInfo,
                         IdentifyData,
                         &BufferSize
                         );
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return;
    }
    
    //<Lvshh001-20181113 Add HDD SN & FW info +>
    // Allocate the Memory for the Device Identify Data
    Status = pBS->AllocatePool(EfiBootServicesData, sizeof(EFI_IDENTIFY_DATA), &IdentifyDriveInfo);
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return ;
    }
    pBS->SetMem(IdentifyDriveInfo, sizeof(EFI_IDENTIFY_DATA), 0);
    Size = sizeof(EFI_IDENTIFY_DATA);
    Status = DiskInfo->Identify(
                           DiskInfo,
                           IdentifyDriveInfo,
                           &Size
                           ); 
    if (EFI_ERROR(Status)) {
       ASSERT_EFI_ERROR(Status);
       return;
    }
    //<Lvshh001-20181113 Add HDD SN & FW info ->
    
    //Check the if the Security feature is supported or not and display the status.
    if (IdentifyData->Command_Set_Supported_82 & 0x2) {
        SecurityStatus = IdentifyData->Security_Status_128;
    }
    if (SecurityStatus & 0x1) {
        //<Libj-20201112 Add Chinese language of SATA options +>
        //Swprintf(DeviceInfo, L"Security Feature :      Supported");
        Str = HiiGetString( gSataHiiHandle, STRING_TOKEN(STR_SECURITY_FEATURE_SUPPORT),NULL );
        Swprintf(DeviceInfo, Str);
        //<Libj-20201112 Add Chinese language of SATA options ->
    } else {
        //<Libj-20201112 Add Chinese language of SATA options +>
        //Swprintf(DeviceInfo, L"Security Feature :      Not Supported");
        Str = HiiGetString( gSataHiiHandle, STRING_TOKEN(STR_SECURITY_FEATURE_NOSUPPORT),NULL );
        Swprintf(DeviceInfo, Str);
        //<Libj-20201112 Add Chinese language of SATA options ->
    }
    SecurityInfoId = SataHiiAddString(gSataHiiHandle, DeviceInfo);
    
    //Check the Security Status of the Hdd and display the status.
    if (SecurityStatus & 0x2) {
        //<Libj-20201112 Add Chinese language of SATA options +>
        //Swprintf(DeviceInfo, L"Security Status  :      Enabled");
        Str = HiiGetString( gSataHiiHandle, STRING_TOKEN(STR_SECURITY_STATUS_ENABLE),NULL );
        Swprintf(DeviceInfo, Str);
        //<Libj-20201112 Add Chinese language of SATA options ->
    } else {
        //<Libj-20201112 Add Chinese language of SATA options +>
        //Swprintf(DeviceInfo, L"Security Status  :      Disabled");
        Str = HiiGetString( gSataHiiHandle, STRING_TOKEN(STR_SECURITY_STATUS_DISABLE),NULL );
        Swprintf(DeviceInfo, Str);
        //<Libj-20201112 Add Chinese language of SATA options ->
    }
    SecurityStatusId = SataHiiAddString(gSataHiiHandle, DeviceInfo);    
    
    //Check the if the SMART feature is supported or not and display the status.
    SmartStatus = IdentifyData->Command_Set_Supported_82;
    if (SmartStatus & 0x1) {
        //<Libj-20201112 Add Chinese language of SATA options +>
        //Swprintf(DeviceInfo, L"SMART Feature    :      Supported");
        Str = HiiGetString( gSataHiiHandle, STRING_TOKEN(STR_SMART_FEATURE_SUPPORT),NULL );
        Swprintf(DeviceInfo, Str);
        //<Libj-20201112 Add Chinese language of SATA options ->
    } else {
        //<Libj-20201112 Add Chinese language of SATA options +>
        //Swprintf(DeviceInfo, L"SMART Feature    :      Not Supported");
        Str = HiiGetString( gSataHiiHandle, STRING_TOKEN(STR_SMART_FEATURE_NOSUPPORT),NULL );
        Swprintf(DeviceInfo, Str);
        //<Libj-20201112 Add Chinese language of SATA options ->
    }
    SmartInfoId = SataHiiAddString(gSataHiiHandle, DeviceInfo);
    
    // Check whether the device is an OPAL or not.
    OpalStatus = IdentifyData->Trusted_Computing_Support;
    if (OpalStatus & 0x01) {
        //<Libj-20201112 Add Chinese language of SATA options +>
        //Swprintf(DeviceInfo, L"Opal Feature     :      Supported");
        Str = HiiGetString( gSataHiiHandle, STRING_TOKEN(STR_OPAL_FEATURE_SUPPORT),NULL );
        Swprintf(DeviceInfo, Str);
        //<Libj-20201112 Add Chinese language of SATA options ->
    } else {
        //<Libj-20201112 Add Chinese language of SATA options +>
        //Swprintf(DeviceInfo, L"Opal Feature     :      Not Supported");
        Str = HiiGetString( gSataHiiHandle, STRING_TOKEN(STR_OPAL_FEATURE_NOSUPPORT),NULL );
        Swprintf(DeviceInfo, Str);
        //<Libj-20201112 Add Chinese language of SATA options ->
    }
    OpalInfoId = SataHiiAddString(gSataHiiHandle, DeviceInfo);
    
    // Check the Acoustic Status and display the acoustic level of the device.
    AcousticLevel = IdentifyData->Acoustic_Level_94 & 0XFF;
    if (IdentifyData->Command_Set_Supported_83 & AUTOMATIC_ACOUSTIC_FEATURE_SET_SUPPORTED) {
        //<Libj-20201112 Add Chinese language of SATA options +>
        //Swprintf(DeviceInfo, L"Acoustic Feature :      Supported");
        Str = HiiGetString( gSataHiiHandle, STRING_TOKEN(STR_ACOUSTIC_FEATURE_SUPPORT),NULL );
        Swprintf(DeviceInfo, Str);
        //<Libj-20201112 Add Chinese language of SATA options ->
        AcousticStatusId = SataHiiAddString(gSataHiiHandle, DeviceInfo);
        if (AcousticLevel == IdentifyData->Acoustic_Level_94 >> 8) {
            //<Libj-20201112 Add Chinese language of SATA options +>
            //Swprintf(DeviceInfo, L"Acoustic Level   :      ByPass ");
            Str = HiiGetString( gSataHiiHandle, STRING_TOKEN(STR_ACOUSTIC_FEATURE_LEVEL_PASS),NULL );
            Swprintf(DeviceInfo, Str);
            //<Libj-20201112 Add Chinese language of SATA options ->
        } else if (AcousticLevel == 0x80) {
            //<Libj-20201112 Add Chinese language of SATA options +>
            //Swprintf(DeviceInfo, L"Acoustic Level   :      Quiet ");
            Str = HiiGetString( gSataHiiHandle, STRING_TOKEN(STR_ACOUSTIC_FEATURE_LEVEL_QUIET),NULL );
            Swprintf(DeviceInfo, Str);
            //<Libj-20201112 Add Chinese language of SATA options ->
        } else if (AcousticLevel == 0xFE) {
            //<Libj-20201112 Add Chinese language of SATA options +>
            //Swprintf(DeviceInfo, L"Acoustic Level   :      Max Performance ");
            Str = HiiGetString( gSataHiiHandle, STRING_TOKEN(STR_ACOUSTIC_FEATURE_LEVEL_MAX_PERFORMANCE),NULL );
            Swprintf(DeviceInfo, Str);
            //<Libj-20201112 Add Chinese language of SATA options ->
        } else {
            //<Libj-20201112 Add Chinese language of SATA options +>
            //Swprintf(DeviceInfo, L"Acoustic Level   :      %x ",AcousticLevel);
            Str = HiiGetString( gSataHiiHandle, STRING_TOKEN(STR_ACOUSTIC_FEATURE_LEVEL),NULL );          
            Swprintf(DeviceInfo,L"%s%x",Str,AcousticLevel);
            //<Libj-20201112 Add Chinese language of SATA options ->
        }
        AcousticInfoId = SataHiiAddString(gSataHiiHandle, DeviceInfo);
    } else {
        //<Libj-20201112 Add Chinese language of SATA options +>
        //Swprintf(DeviceInfo, L"Acoustic Feature :      Not Supported");
        Str = HiiGetString( gSataHiiHandle, STRING_TOKEN(STR_ACOUSTIC_FEATURE_NOSUPPORT),NULL );
        Swprintf(DeviceInfo, Str);
        //<Libj-20201112 Add Chinese language of SATA options ->
        AcousticStatusId = SataHiiAddString(gSataHiiHandle, DeviceInfo);
    }
    //<Lvshh001-20181113 Add HDD SN & FW info +>
    pBS->SetMem(HddSN, OEM_HDD_SN_LENGTH+2, 0);
    pBS->CopyMem(HddSN, IdentifyDriveInfo->AtaData.SerialNo, OEM_HDD_SN_LENGTH);
    SwapEntries(HddSN, OEM_HDD_SN_LENGTH);
    HddSN[OEM_HDD_SN_LENGTH] = 0;
    TrimSpace(HddSN, OEM_HDD_SN_LENGTH);           
    //<Libj-20201112 Add Chinese language of SATA options +>
    //Swprintf(DeviceInfo, L"Serial Number    :      %a", HddSN);
    Str = HiiGetString( gSataHiiHandle, STRING_TOKEN(STR_SERIAL_NUMBER),NULL );          
    Swprintf(DeviceInfo,L"%s%a",Str,HddSN);
    //<Libj-20201112 Add Chinese language of SATA options ->
    HddSNStringId = SataHiiAddString(gSataHiiHandle, DeviceInfo);

    pBS->SetMem(HddFirmwareVersion, OEM_HDD_FIRMWARE_VERSION_LENGTH+2, 0);
    pBS->CopyMem(HddFirmwareVersion, IdentifyDriveInfo->AtaData.FirmwareVer, OEM_HDD_FIRMWARE_VERSION_LENGTH);
    SwapEntries(HddFirmwareVersion, OEM_HDD_FIRMWARE_VERSION_LENGTH);
    HddFirmwareVersion[OEM_HDD_FIRMWARE_VERSION_LENGTH] = 0;
    TrimSpace(HddFirmwareVersion, OEM_HDD_FIRMWARE_VERSION_LENGTH);
    //<Libj-20201112 Add Chinese language of SATA options +>
    //Swprintf(DeviceInfo, L"Firmware         :      %a", HddFirmwareVersion);
    Str = HiiGetString( gSataHiiHandle, STRING_TOKEN(STR_FIRMWARE),NULL );          
    Swprintf(DeviceInfo,L"%s%a",Str,HddFirmwareVersion);
    //<Libj-20201112 Add Chinese language of SATA options ->

    HddFWStringId = SataHiiAddString(gSataHiiHandle, DeviceInfo);
    //<Lvshh001-20181113 Add HDD SN & FW info ->
    //<xuyj1-20180904 add HDD Speed for ATA Devices+ >
    PciAbar = *(UINT32*)PCI_CFG_ADDR(SataControllerInterface->BusNum, SataControllerInterface->DevNum, SataControllerInterface->FunNum, 0x24);
    CurrentPortSpeed = MmioRead8(PciAbar + Device * 0x80 + 0x128) >> 4;
    //<Libj-20201112 Add Chinese language of SATA options +>
    //Swprintf(DeviceInfo, L"Current Speed    :      GEN%d", CurrentPortSpeed);
    Str = HiiGetString( gSataHiiHandle, STRING_TOKEN(STR_CURRENT_SPEED),NULL );          
    Swprintf(DeviceInfo,L"%s%d",Str,CurrentPortSpeed);
    //<Libj-20201112 Add Chinese language of SATA options ->
    SpeedInfoId = SataHiiAddString(gSataHiiHandle, DeviceInfo);
    //<xuyj1-20180904 add HDD Speed for ATA Devices- > 
    // Updating the Hdd Name 
    DeviceString = HiiGetString(gSataHiiHandle,SataControllerInterface->gHddNameStrId[Device],NULL);
    //<Libj-20201112 Add Chinese language of SATA options +>
    //Swprintf(DeviceInfo, L"Model Number     :      %s",DeviceString);
    Str = HiiGetString( gSataHiiHandle, STRING_TOKEN(STR_MODEL_NUMBER),NULL );
    Swprintf(DeviceInfo, L"%s%s",Str,DeviceString);
    //<Libj-20201112 Add Chinese language of SATA options ->
    HddNameStringId = SataHiiAddString(gSataHiiHandle, DeviceInfo);
    
    //Updating the Hdd Size for ATA devices alone 
    if (!(IdentifyData->General_Config_0 & ATAPI_DEVICE)) {    
        DeviceString = HiiGetString(gSataHiiHandle,SataControllerInterface->gHddSizeStrId[Device],NULL);
        //<Libj-20201112 Add Chinese language of SATA options +>
        //Swprintf(DeviceInfo, L"Size             :      %s",DeviceString);
        Str = HiiGetString( gSataHiiHandle, STRING_TOKEN(STR_SIZE),NULL );          
        Swprintf(DeviceInfo,L"%s%s",Str,DeviceString); 
        //<Libj-20201112 Add Chinese language of SATA options ->

        HddSizeStringId = SataHiiAddString(gSataHiiHandle, DeviceInfo);
    }

    //Updating the Devices details dynamically on the sub form.

    HiiCreateSubTitleOpCode(StartOpCodeHandle,
                            HddNameStringId,
                            STRING_TOKEN(STR_EMPTY),
                            0,0);
    // Ignore displaying the Size details for ATAPI device
    if (!(IdentifyData->General_Config_0 & ATAPI_DEVICE)) {
        HiiCreateSubTitleOpCode(StartOpCodeHandle,
                                HddSizeStringId,
                                STRING_TOKEN(STR_EMPTY),
                                0,0);
    }
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
                            SmartInfoId,
                            STRING_TOKEN(STR_EMPTY),
                            0,0);
    
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
                            SecurityInfoId,
                            STRING_TOKEN(STR_EMPTY),
                            0,0);
    
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
                            SecurityStatusId,
                            STRING_TOKEN(STR_EMPTY),
                            0,0);
    
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
                            OpalInfoId,
                            STRING_TOKEN(STR_EMPTY),
                            0,0);
    
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
                            AcousticStatusId,
                            STRING_TOKEN(STR_EMPTY),
                            0,0);
    //<Lvshh001-20181113 Add HDD SN & FW info +>
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
                            HddSNStringId,
                            STRING_TOKEN(STR_EMPTY),
                            0,0);
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
                            HddFWStringId,
                            STRING_TOKEN(STR_EMPTY),
                            0,0);
    //<Lvshh001-20181113 Add HDD SN & FW info ->
	//<xuyj1-20180904 add HDD Speed for ATA Devices+ >
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
                                SpeedInfoId,
                                STRING_TOKEN(STR_EMPTY),
                                0,0);
      //<xuyj1-20180904 add HDD Speed for ATA Devices+ >

    
    // Displaying the Acoustic level only if Acoustic feature is supported.
    if (IdentifyData->Command_Set_Supported_83 & AUTOMATIC_ACOUSTIC_FEATURE_SET_SUPPORTED) {
        HiiCreateSubTitleOpCode(StartOpCodeHandle,
                                AcousticInfoId,
                                STRING_TOKEN(STR_EMPTY),
                                0,0);
    }

    // Updating the Sata Sub form.
    HiiUpdateForm (
                gSataHiiHandle,
                &gSataConfigFormSetGuid,
                SATA_DEVICE_INFO_SUB_FORM_ID,
                StartOpCodeHandle,
                EndOpCodeHandle
            );
    // Free the Memory Allocated
    
    //<Lvshh001-20181113 Add HDD SN & FW info +>
    if (IdentifyDriveInfo) {
           pBS->FreePool(IdentifyDriveInfo);
           IdentifyDriveInfo = NULL;
    }
    //<Lvshh001-20181113 Add HDD SN & FW info ->
    
    if (IdentifyData) {
        pBS->FreePool(IdentifyData);
        IdentifyData = NULL;
    }
    if (DeviceString) {
        pBS->FreePool(DeviceString);
        DeviceString = NULL;
    }
    if (StartOpCodeHandle != NULL) {
        HiiFreeOpCodeHandle (StartOpCodeHandle);
        HiiFreeOpCodeHandle (EndOpCodeHandle);
    }
    return;
}

/**
    This function is called by Setup browser to perform callback

    @param  EFI_HII_CONFIG_ACCESS_PROTOCOL      *This           - Pointer to the EFI_HII_CONFIG_ACCESS_PROTOCOL
    @param  EFI_BROWSER_ACTION                  Action          - Action type
    @param  EFI_QUESTION_ID                     KeyValue        - 2 byte key value
    @param  EFI_IFR_TYPE_VALUE                  *Value          - Pointer to the IFR type value
    @param  EFI_BROWSER_ACTION_REQUEST          *ActionRequest  - Request for action type

    @retval EFI_SUCCESS       - callback is successful                  
            EFI_UNSUPPORTED   - callback not supported

**/

EFI_STATUS
EFIAPI
SataDevInfoCallback(
    IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL     *This,
    IN EFI_BROWSER_ACTION                       Action,
    IN EFI_QUESTION_ID                          KeyValue,
    IN UINT8                                    Type,
    IN EFI_IFR_TYPE_VALUE                       *Value,
    OUT EFI_BROWSER_ACTION_REQUEST              *ActionRequest
)
{
    UINT8                  DeviceNum;
    SATA_CONTROLLER_INFO   *SataControllerInterface = NULL;
    LIST_ENTRY             *Link;
    UINT8                  ControllerCount = 0;
    
    if (Action != EFI_BROWSER_ACTION_CHANGING) {
        return EFI_UNSUPPORTED; 
    }
    // Obtaining the Port number and Controller number to which device is connected by traversing the list.
    DeviceNum = (UINT8)(KeyValue - gSataDeviceBaseKey);
    
    for (Link = SataDeviceInfoList.ForwardLink;Link != &SataDeviceInfoList;Link = Link->ForwardLink) {
        SataControllerInterface = BASE_CR(Link,SATA_CONTROLLER_INFO,SataControllerLink );
        // Checking the controller to which the device is connected.
        if (DeviceNum < SataControllerInterface->MaxPortSupported) {
            break;
        }
        DeviceNum = DeviceNum - SataControllerInterface->MaxPortSupported;
        ControllerCount++;
    }
    // Function to update the Hdd informations in a subform.
    SataDevInfoDynamicSubFormContents(DeviceNum,ControllerCount);
    return EFI_SUCCESS;
}
#endif
/**
    @internal
    
    Add/Set the String to HII Database using HiiString Protocol

    @param HiiHandle
    @param String
    @param StringId

    @retval VOID

    @endinternal
**/
EFI_STRING_ID
SataHiiAddStringInternal (
    IN  EFI_HII_HANDLE  HiiHandle,
    IN  CHAR16 *        String,
    IN  EFI_STRING_ID   StringId
)
{
    EFI_STATUS      Status;
    CHAR8*          Languages = NULL;
    UINTN           LangSize = 0;
    CHAR8*          CurrentLanguage;
    BOOLEAN         LastLanguage = FALSE;

    if (HiiString == NULL) {
        Status = pBS->LocateProtocol(&gEfiHiiStringProtocolGuid, NULL, (VOID **) &HiiString);
        if (EFI_ERROR(Status)) {
            return 0;
        }
    }
    if (SupportedLanguages == NULL) {
        Status = HiiString->GetLanguages(HiiString, HiiHandle, Languages, &LangSize);
        if (Status == EFI_BUFFER_TOO_SMALL) {
            Status = pBS->AllocatePool(EfiBootServicesData, LangSize, (VOID**)&Languages);
            if (EFI_ERROR(Status)) {
                //not enough resources to allocate string
                return 0;
            }
            Status = HiiString->GetLanguages(HiiString, HiiHandle, Languages, &LangSize);
            if(EFI_ERROR(Status)) {
                return 0;
            }
        }
        SupportedLanguages=Languages;
    } else {
        Languages=SupportedLanguages;
    }

    while(!LastLanguage) {
        //
        //point CurrentLanguage to start of new language
        //
        CurrentLanguage = Languages;
        while(*Languages != ';' && *Languages != 0)
            Languages++;

        //
        //Last language in language list
        //
        if (*Languages == 0) {
            LastLanguage = TRUE;
            if (StringId == 0) {
                Status = HiiString->NewString(HiiString, HiiHandle, &StringId, CurrentLanguage, NULL, String, NULL);
            } else {
                Status = HiiString->SetString(HiiString, HiiHandle, StringId, CurrentLanguage, String, NULL);
            }
            if (EFI_ERROR(Status)) {
                return 0;
            }
        } else {
            //
            //put null-terminator
            //
            *Languages = 0;
            if (StringId == 0) {
                Status = HiiString->NewString(HiiString, HiiHandle, &StringId, CurrentLanguage, NULL, String, NULL);
            } else {
                Status = HiiString->SetString(HiiString, HiiHandle, StringId, CurrentLanguage, String, NULL);
            }
            *Languages = ';';       //restore original character
            Languages++;
            if (EFI_ERROR(Status)) {
                return 0;
            }
        }
    }
    return StringId;
}
/**
    @internal
    
    Set the String to HII Database using HiiString Protocol

    @param HiiHandle
    @param String
    @param StringId

    @retval VOID

    @endinternal
**/
EFI_STRING_ID
SataHiiSetString (
    IN EFI_HII_HANDLE   HiiHandle,
    IN CHAR16           *String,
    IN EFI_STRING_ID    StringId
)
{
    return SataHiiAddStringInternal(HiiHandle, String, StringId);
}

/**
    @internal
    
    Add the String to HII Database using HiiString Protocol

    @param HiiHandle
    @param String

    @retval VOID

    @endinternal
**/
EFI_STRING_ID
SataHiiAddString (
    IN  EFI_HII_HANDLE  HiiHandle,
    IN  CHAR16 *        String
)
{
    return SataHiiAddStringInternal(HiiHandle, String, 0);
}

/**
    @internal
    
    This function is swaps the (2) bytes in the array of words.
    Basically changes the endian format.

    @param  CHAR8    *Data
    @param  UINT16   Size

    @retval None

    @endinternal
**/
VOID 
SwapEntries(
    IN CHAR8    *Data,
    IN UINT16   Size
)
{
    UINT16  Index;
    CHAR8   Temp8;

    for (Index = 0; (Index + 1) < Size; Index += 2) {
        Temp8           = Data[Index];
        Data[Index]     = Data[Index + 1];
        Data[Index + 1] = Temp8;
    }
}

/**
    @internal
    
    Gets details about connected Sata devices on the controller

    @param  EFI_HANDLE            *ControllerHandle
    @param  SATA_CONTROLLER_INFO  *SataControllerInterface
    
    @retval EFI_STATUS 

    @endinternal
**/
EFI_STATUS
GetDeviceInfo (
    IN EFI_HANDLE               *ControllerHandle,
    IN SATA_CONTROLLER_INFO     *SataControllerInterface
)
{
    EFI_STATUS                              Status;
    EFI_OPEN_PROTOCOL_INFORMATION_ENTRY     *Entries;
    UINTN                                   Count;
    UINTN                                   i;
    UINTN                                   Index;
    EFI_DISK_INFO_PROTOCOL                  *DiskInfo;
    UINT32                                  IdeChannel;
    UINT32                                  IdeDevice;
    EFI_IDENTIFY_DATA                       *IdentifyDriveInfo = NULL;
    UINT32                                  BufferSize = 0;
    CHAR8                                   ModelNumber[42];
    UINT64                                  NumSectors = 0;
    UINT64                                  DriveSizeInBytes = 0;
    UINTN                                   RemainderInBytes = 0;
    UINT32                                  DriveSizeInGB = 0;
    UINTN                                   NumTenthsOfGB = 0;
    CHAR16                                  *NewString;
    CHAR16                                  *SizeString;
    UINT32                                  SectorSize;
    //<Libj-20201112 Add Chinese language of SATA options +>
    EFI_STRING              Str = NULL;
    //<Libj-20201112 Add Chinese language of SATA options ->
#if DISPLAY_HDD_FEATURE_INFO
    CHAR16                                  *NewStringText;
    
    // Allocating memory for storing SATA Device Name and Port informations
    Status = pBS->AllocatePool(EfiBootServicesData, 0x100, &NewStringText);
    if (EFI_ERROR(Status)) {
       return Status;
    }
#endif
    // Allocating memory for storing SATA Device Name and size informations
    Status = pBS->AllocatePool(EfiBootServicesData, 0x100, &NewString);
    if (EFI_ERROR(Status)) {
       return Status;
    }
    Status = pBS->AllocatePool(EfiBootServicesData, 0x100, &SizeString);
    if (EFI_ERROR(Status)) {
       return Status;
    }
    
    // Check if the IdeControllerInitProtocol is present on the Controller Handle
    Status = pBS->OpenProtocolInformation( ControllerHandle, 
                                           &gEfiIdeControllerInitProtocolGuid, 
                                           &Entries, 
                                           &Count
                                           );
    if (EFI_ERROR(Status)) {
       return Status;
    }
    
    // Get the DiskInfo Protocol's for the Controller Handle to find the device information.
    // Each device on the controller handle would produce the DiskInfo Protocol 
    // and DisInfo Protocol would be child handle for the controller handle 
    //
    for (i = 0; i < Count; i++) {
        if (!(Entries[i].Attributes & EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER)) {
            continue;
        }
        Status = pBS->HandleProtocol(Entries[i].ControllerHandle, 
                                    &gEfiDiskInfoProtocolGuid, 
                                    &DiskInfo
                                    );
        if (EFI_ERROR(Status)) {
           return Status;
        }
        
        // Display only the AHCI Sata Controller devices information 
        if (CompareGuid(&(DiskInfo->Interface),&gEfiDiskInfoAhciInterfaceGuid)) {
        
           // Get the device Port Number and PM port Number
           Status = DiskInfo->WhichIde(
                                   DiskInfo,
                                   &IdeChannel,
                                   &IdeDevice
                                   );
                            
           if (EFI_ERROR(Status)) {
               ASSERT_EFI_ERROR(Status);
               continue;
           }
           // Storing the device handle to obtain the device informations and display in the sub form.
           SataControllerInterface->DeviceHandle[IdeChannel] = Entries[i].ControllerHandle;
           
           // Allocate the Memory for the Device Identify Data
           Status = pBS->AllocatePool(EfiBootServicesData, sizeof(EFI_IDENTIFY_DATA), &IdentifyDriveInfo);
           if (EFI_ERROR(Status)) {
               ASSERT_EFI_ERROR(Status);
               return Status;
           }
           pBS->SetMem(IdentifyDriveInfo, sizeof(EFI_IDENTIFY_DATA), 0);
           BufferSize = sizeof(EFI_IDENTIFY_DATA);
           
           Status = DiskInfo->Identify(
                                  DiskInfo,
                                  IdentifyDriveInfo,
                                  &BufferSize
                                  );
           
           if (EFI_ERROR(Status)) {
              ASSERT_EFI_ERROR(Status);
              continue;
           }
           
           pBS->SetMem(ModelNumber, 42, 0);
           pBS->CopyMem(ModelNumber, IdentifyDriveInfo->AtaData.ModelName, 40);
           
           // Swapping the data to obtain the exact model number.
           SwapEntries(ModelNumber, 40);
           ModelNumber[40] = 0;
           
           // Remove the spaces from the end of the device name
           for (Index = 39; Index > 0; Index-- ) {
               if (ModelNumber[Index] != 0x20) { 
                   break;
               }
               ModelNumber[Index] = 0;
           }
           
           //Update the Device Model Number in the New String
           Swprintf(NewString, L"%a", ModelNumber);
           
           // For HardDisk append the size. Otherwise display atapi
           if (!(IdentifyDriveInfo->AtaData.config & ATAPI_DEVICE)) {
               
              // Find the Size of the ATA Device
              if (IdentifyDriveInfo->AtaData.command_set_supported_83 & _48_BIT_ADDRESS_FEATURE_SET_SUPPORTED) {
                  pBS->CopyMem(&NumSectors, IdentifyDriveInfo->AtaData.maximum_lba_for_48bit_addressing, sizeof(UINT64));
              } else {
                   NumSectors = (IdentifyDriveInfo->AtaData.user_addressable_sectors_hi << 16) | \
                           IdentifyDriveInfo->AtaData.user_addressable_sectors_lo;
              }

              // Word 106 contain valid information only when bit 14 of word 106 is set to one and bit 15 of word 106 is cleared to zero,
              // If bit-12 set to 1 indicates that the device has been formatted with a logical sector size larger than 512            
              if((IdentifyDriveInfo->AtaData.phy_logic_sector_support & 0x4000) &&     // WORD 106 valid? - BIT 14 - 1
              (!(IdentifyDriveInfo->AtaData.phy_logic_sector_support & 0x8000)) &&    // WORD 106 valid? - BIT 15 - 0
              (IdentifyDriveInfo->AtaData.phy_logic_sector_support & 0x1000))         // WORD 106 bit 12 - Sectorsize > 256 words
              { 
                  SectorSize = (UINT32)(IdentifyDriveInfo->AtaData.logic_sector_size_lo + \
                                       (IdentifyDriveInfo->AtaData.logic_sector_size_hi << 16)) * 2;
              } else {
            	  // default sector size
            	  SectorSize = 512;
              }

              DriveSizeInBytes = Mul64(NumSectors, SectorSize);
              
              //DriveSizeInGB is DriveSizeInBytes / 1 GB (1 Decimal GB = 10^9 bytes)
              DriveSizeInGB = (UINT32) Div64(DriveSizeInBytes, 1000000000, &RemainderInBytes);
              
              //Convert the Remainder, which is in bytes, to number of tenths of a Decimal GB.
              NumTenthsOfGB = RemainderInBytes / 100000000;
              
              // Update the ATA Device Size
			  //<Wangyia-20170616 Mantis #14331 Remove the Decimal part for ATA Device Size +>
              //Swprintf(SizeString, L"%d.%dGB", DriveSizeInGB, NumTenthsOfGB);
        	  Swprintf(SizeString, L"%dGB", DriveSizeInGB); 
			  //<Wangyia-20170616 Mantis #14331 Remove the Decimal part for ATA Device Size ->

           } else {
               Swprintf(NewString, L"%a ATAPI", ModelNumber);
           }
#if DISPLAY_HDD_FEATURE_INFO
           //Store the Port number and Device Model Number. This text is used for creating a control
           //to a sub form. 
           //<Libj-20201112 Add Chinese language of SATA options +>
           Str = HiiGetString( gSataHiiHandle, STRING_TOKEN(STR_SATA_PORT),NULL);
           Swprintf(NewStringText, L"%s %d                %a",Str,IdeChannel,ModelNumber); 
           //Swprintf(NewStringText, L"Port %d                %a",IdeChannel,ModelNumber); 
           
           //Store the Port number and Model number of the device.
           SataControllerInterface->gSataPortStrId[IdeChannel] = SataHiiAddString(gSataHiiHandle, NewStringText);
           Str = HiiGetString( gSataHiiHandle, STRING_TOKEN(STR_SATA_PORT),"en-US" );
           Swprintf(NewStringText, L"%s %d                %a",Str,IdeChannel,ModelNumber); 
           HiiSetString(gSataHiiHandle, SataControllerInterface->gSataPortStrId[IdeChannel], NewStringText, "en-US");
           Str = HiiGetString( gSataHiiHandle, STRING_TOKEN(STR_SATA_PORT),"zh-chs" );
           Swprintf(NewStringText, L"%s %d                %a",Str,IdeChannel,ModelNumber); 
           HiiSetString(gSataHiiHandle, SataControllerInterface->gSataPortStrId[IdeChannel], NewStringText, "zh-chs");
           //<Libj-20201112 Add Chinese language of SATA options ->
#endif
           SataControllerInterface->gHddNameStrId[IdeChannel] = SataHiiAddString(gSataHiiHandle, NewString);
           
           // Store the size for ATA device alone
           if (!(IdentifyDriveInfo->AtaData.config & ATAPI_DEVICE)) {
               SataControllerInterface->gHddSizeStrId[IdeChannel] = SataHiiAddString(gSataHiiHandle, SizeString);
           } else {
               SataControllerInterface->gHddSizeStrId[IdeChannel] = 0;
           }
           if (IdentifyDriveInfo) {
               pBS->FreePool(IdentifyDriveInfo);
               IdentifyDriveInfo = NULL;
           }
        }
    }
#if DISPLAY_HDD_FEATURE_INFO 
    pBS->FreePool(NewStringText);
#endif
    pBS->FreePool(NewString);
    pBS->FreePool(SizeString);
    return Status;
     
}

/**
    @internal
    
    Gets details about Sata Controllers present in the system. 

    @param  None

    @retval EFI_STATUS

    @endinternal
**/
EFI_STATUS GetSataControllerDetails()
{
    EFI_STATUS                          Status;
    EFI_HANDLE                          *HandleBuffer;
    UINT8                               Index;
    EFI_IDE_CONTROLLER_INIT_PROTOCOL    *IdeControllerInit;
    EFI_PCI_IO_PROTOCOL                 *PciIo; 
    UINTN                               SegNum;
    UINTN                               BusNum;
    UINTN                               DevNum;
    UINTN                               FuncNum;
    UINT32                              PciVidDid;
    UINT16                              PciVendorId;
    UINT16                              PciDeviceId;
    CHAR16                              pString[100] = L"";
    CHAR16                              *String;
    UINT8                               i;
    UINT8                               PortIndex;
    UINTN                               HandleCount = 0;
    EFI_STRING_ID                       NullString; 
    SATA_CONTROLLER_INFO                *SataControllerInterface = NULL;
    
    //<Libj-20201112 Add Chinese language of SATA options +>
    EFI_STRING              Str = NULL,Str1=NULL;
    //<Libj-20201112 Add Chinese language of SATA options ->
    
    // Get the number of Sata Controllers
    Status = pBS->LocateHandleBuffer (
                      ByProtocol,
                      &gEfiIdeControllerInitProtocolGuid,
                      NULL,
                      &HandleCount,
                      &HandleBuffer
                      );
    if (EFI_ERROR(Status)) {
        return Status;
    }
    
    Status = pBS->AllocatePool(EfiBootServicesData, 0x100, &String);
    if (EFI_ERROR(Status)) {
       return Status;
    }
#if !DISPLAY_HDD_FEATURE_INFO
    // If device not found store "Not Present" and update for the corresponding ports
    Swprintf(String, L"Not Present");
    NullString = SataHiiAddString(gSataHiiHandle, String);
#endif
    
    // Initializing the List to store the Controller details
    InitializeListHead(&SataDeviceInfoList);

    for (Index = 0; Index < HandleCount; Index++) {
        // Allocating memory for storing the Controller details
        Status = pBS->AllocatePool (EfiBootServicesData,
                                   sizeof(SATA_CONTROLLER_INFO),
                                   (VOID**)&SataControllerInterface
                                   );
        if (EFI_ERROR(Status)) {
            pBS->FreePool(String); 
            ASSERT_EFI_ERROR(Status);
            return Status;
        }
         
        pBS->SetMem(SataControllerInterface, sizeof(SATA_CONTROLLER_INFO), 0); 
       
        // Check if the IdeControllerInitProtocol is present on the Controller Handle
        Status = pBS->HandleProtocol(
                          HandleBuffer[Index],
                          &gEfiIdeControllerInitProtocolGuid,
                          &IdeControllerInit);
        if (EFI_ERROR(Status)) {
            ASSERT_EFI_ERROR(Status);
            continue;
        }
        Status = pBS->HandleProtocol(
                          HandleBuffer[Index],
                          &gEfiPciIoProtocolGuid,
                          &PciIo);
        if (EFI_ERROR(Status)) {
            ASSERT_EFI_ERROR(Status);
            continue;
        }
         
        // Retrieves Sata controller’s PCI bus, device and function number.
        
        Status = PciIo->GetLocation(PciIo, 
                                   &SegNum, 
                                   &BusNum, 
                                   &DevNum, 
                                   &FuncNum);
        if(EFI_ERROR(Status)) {
            ASSERT_EFI_ERROR(Status);
            continue;
        }
        SataControllerInterface->SegNum = (UINT8)SegNum;
        SataControllerInterface->BusNum = (UINT8)BusNum;
        SataControllerInterface->DevNum = (UINT8)DevNum;
        SataControllerInterface->FunNum = (UINT8)FuncNum;
        SataControllerInterface->MaxPortSupported = IdeControllerInit->ChannelCount;
        
        // Retrieves Sata controller’s PCI Vendor ID and Device ID of the controller.
        Status = PciIo->Pci.Read (
                            PciIo,
                            EfiPciIoWidthUint32,
                            PCI_VID_DID,
                            1,
                            &PciVidDid
                            );
        if(EFI_ERROR(Status)) {
            ASSERT_EFI_ERROR(Status);
            continue;
        }
         
        PciVendorId = (UINT16)(PciVidDid & 0xffff);
        PciDeviceId = (UINT16)(PciVidDid >> 16);
               
        // Checking for OEM defined Name. If present update the OEM defined name for
        // the Sata controller name 
        for (i = 0; SataContollerNameUpdatelist[i].VendorID != 0xFFFF ; i++) {
            if((SataContollerNameUpdatelist[i].VendorID == PciVendorId) && (SataContollerNameUpdatelist[i].DeviceID == PciDeviceId) ) {
                //<Libj-20201112 Add Chinese language of SATA options +>
                Str = HiiGetString( gSataHiiHandle, STRING_TOKEN(STR_SATA_CONTROLLER),NULL );
                Swprintf(String, L"%s (S:%02X B:%02X D:%02X F:%02X) %a",\
                        Str,SegNum,BusNum,DevNum,FuncNum,SataContollerNameUpdatelist[i].ControllerName);
                //Swprintf(String, L"SATA Controller (S:%02X B:%02X D:%02X F:%02X) %a",\
                //        SegNum,BusNum,DevNum,FuncNum,SataContollerNameUpdatelist[i].ControllerName);
                SataControllerInterface->ControllerStrId = SataHiiSetString(gSataHiiHandle, 
                                                                         String, 
                                                                         SataControllerInterface->ControllerStrId
                                                                         );
                Str = HiiGetString( gSataHiiHandle, STRING_TOKEN(STR_SATA_CONTROLLER),"zh-chs" );
                Swprintf(String, L"%s (S:%02X B:%02X D:%02X F:%02X) %a",\
                        Str,SegNum,BusNum,DevNum,FuncNum,SataContollerNameUpdatelist[i].ControllerName);
                HiiSetString(gSataHiiHandle, SataControllerInterface->ControllerStrId, String, "zh-chs");
                
                Str = HiiGetString( gSataHiiHandle, STRING_TOKEN(STR_SATA_CONTROLLER),"en-US" );
                Swprintf(String, L"%s (S:%02X B:%02X D:%02X F:%02X) %a",\
                        Str,SegNum,BusNum,DevNum,FuncNum,SataContollerNameUpdatelist[i].ControllerName);
                HiiSetString(gSataHiiHandle, SataControllerInterface->ControllerStrId, String, "en-US");
                //<Libj-20201112 Add Chinese language of SATA options ->
                break;
            } 
        }
        // If the OEM defined name not present for the controller update only the 
        // Bus, Device and Function details for the controller
        if (SataContollerNameUpdatelist[i].VendorID == 0xFFFF) {
            //<Libj-20201112 Add Chinese language of SATA options +>
            Str = HiiGetString( gSataHiiHandle, STRING_TOKEN(STR_SATA_CONTROLLER),"zh-chs" );
            Swprintf(pString, L"%s (S:%02X B:%02X D:%02X F:%02X)",Str,SegNum,BusNum,DevNum,NULL);
            //Swprintf(pString, L"SATA Controller (S:%02X B:%02X D:%02X F:%02X)",SegNum,BusNum,DevNum,FuncNum);
            SataControllerInterface->ControllerStrId = SataHiiAddString(gSataHiiHandle, pString);
            Str = HiiGetString( gSataHiiHandle, STRING_TOKEN(STR_SATA_CONTROLLER),"zh-chs" );
            Swprintf(pString, L"%s (S:%02X B:%02X D:%02X F:%02X)",Str,SegNum,BusNum,DevNum,FuncNum);
            HiiSetString(gSataHiiHandle, SataControllerInterface->ControllerStrId, pString, "zh-chs");
            
            Str = HiiGetString( gSataHiiHandle, STRING_TOKEN(STR_SATA_CONTROLLER),"en-US" );
            Swprintf(pString, L"%s (S:%02X B:%02X D:%02X F:%02X)",Str,SegNum,BusNum,DevNum,FuncNum);
            HiiSetString(gSataHiiHandle, SataControllerInterface->ControllerStrId, pString, "en-US");
            //<Libj-20201112 Add Chinese language of SATA options ->
        }
        
        // Allocating Memory for storing the Device details
        Status = pBS->AllocatePool (EfiBootServicesData,
                                    sizeof(EFI_STRING_ID) * SataControllerInterface->MaxPortSupported,
                                    (VOID**)&SataControllerInterface->gHddNameStrId
                                    );
        if (EFI_ERROR(Status)) {
            pBS->FreePool(String); 
            ASSERT_EFI_ERROR(Status);
            return Status;
        }
         
        pBS->SetMem(SataControllerInterface->gHddNameStrId, 
                    sizeof(EFI_STRING_ID) * SataControllerInterface->MaxPortSupported, 
                    0
                    );
        Status = pBS->AllocatePool (EfiBootServicesData,
                                    sizeof(EFI_STRING_ID) * SataControllerInterface->MaxPortSupported,
                                    (VOID**)&SataControllerInterface->gHddSizeStrId
                                    );
        if (EFI_ERROR(Status)) {
            pBS->FreePool(String); 
            ASSERT_EFI_ERROR(Status);
            return Status;
        }
        pBS->SetMem(SataControllerInterface->gHddSizeStrId, 
                    sizeof(EFI_STRING_ID) * SataControllerInterface->MaxPortSupported, 
                    0
                    );
            
        Status = pBS->AllocatePool (EfiBootServicesData,
                                    sizeof(EFI_STRING_ID) * SataControllerInterface->MaxPortSupported,
                                   (VOID**)&SataControllerInterface->gSataPortStrId
                                    );
        if (EFI_ERROR(Status)) {
            pBS->FreePool(String); 
            ASSERT_EFI_ERROR(Status);
            return Status;
        }
        pBS->SetMem(SataControllerInterface->gSataPortStrId, 
                    sizeof(EFI_STRING_ID) * SataControllerInterface->MaxPortSupported, 
                    0
                    );
        
        Status = pBS->AllocatePool (EfiBootServicesData,
                                    sizeof(EFI_HANDLE) * SataControllerInterface->MaxPortSupported,
                                   (VOID**)&SataControllerInterface->DeviceHandle
                                    );
        if (EFI_ERROR(Status)) {
            pBS->FreePool(String); 
            ASSERT_EFI_ERROR(Status);
            return Status;
        }
        
        pBS->SetMem(SataControllerInterface->DeviceHandle, 
                    sizeof(EFI_HANDLE) * SataControllerInterface->MaxPortSupported, 
                    0
                    );
             
        //Gets the details of the devices attached to the Controllers
        Status = GetDeviceInfo(HandleBuffer[Index], SataControllerInterface);
        if (EFI_ERROR(Status)) {
             pBS->FreePool(String); 
             ASSERT_EFI_ERROR(Status);
             return Status;
        }
        
        for (PortIndex = 0; PortIndex < SataControllerInterface->MaxPortSupported; PortIndex++) {
#if DISPLAY_HDD_FEATURE_INFO             
            // If device not found store "Not Present" for the remaining ports in which device 
            // is not present. 
            //<Libj-20201112 Add Chinese language of SATA options +>
            Str = HiiGetString( gSataHiiHandle, STRING_TOKEN(STR_SATA_PORT),NULL );
            Str1 = HiiGetString( gSataHiiHandle, STRING_TOKEN(STR_NOT_PRESENT),NULL );
            Swprintf(String, L"%s %d                %s",Str,PortIndex,Str1);
            //Swprintf(String, L"Port %d                Not Present",PortIndex);
            NullString = SataHiiAddString(gSataHiiHandle, String);
            Str = HiiGetString( gSataHiiHandle, STRING_TOKEN(STR_SATA_PORT),"en-US" );
            Str1 = HiiGetString( gSataHiiHandle, STRING_TOKEN(STR_NOT_PRESENT),"en-US" );
            Swprintf(String, L"%s %d                %s",Str,PortIndex,Str1);
            HiiSetString(gSataHiiHandle, NullString, String, "en-US");
            NullString= SataHiiSetString(gSataHiiHandle, String, NullString);
            Str = HiiGetString( gSataHiiHandle, STRING_TOKEN(STR_SATA_PORT),"zh-chs" );
            Str1 = HiiGetString( gSataHiiHandle, STRING_TOKEN(STR_NOT_PRESENT),"zh-chs" );
            Swprintf(String, L"%s %d                %s",Str,PortIndex,Str1);
            HiiSetString(gSataHiiHandle, NullString, String, "zh-chs");
            //<Libj-20201112 Add Chinese language of SATA options ->
            if (SataControllerInterface->gHddNameStrId[PortIndex] == 0) {
                 SataControllerInterface->gSataPortStrId[PortIndex] = NullString;
            }
#else        
            // Store the String for number of Ports supported by the Controller
            Swprintf(String, L"Port %d", PortIndex);
            SataControllerInterface->gSataPortStrId[PortIndex] = SataHiiAddString(gSataHiiHandle, String);
            // Store Not present if device is not found.
            if (SataControllerInterface->gHddNameStrId[PortIndex] == 0) {
                SataControllerInterface->gHddNameStrId[PortIndex] = NullString;
            } 
#endif  
        }
        InsertTailList(&SataDeviceInfoList, &SataControllerInterface->SataControllerLink);
    }
    pBS->FreePool(String); 
    return Status;
}

/**
    This function will dynamically update SATA configuration setup page
    using HII library functions.

    @param  EFI_EVENT   Event       - Handle to an event structure
    @param  VOID        *Context    - Pointer to the notification function's context

    @retval None

**/
VOID 
EFIAPI
InitSataDynamicIfrContents (
    IN EFI_EVENT        Event,
    IN VOID             *Context
)
{
    EFI_STATUS                  Status;
    VOID                        *StartOpCodeHandle;
    VOID                        *EndOpCodeHandle;
    EFI_IFR_GUID_LABEL          *StartLabel;
    EFI_IFR_GUID_LABEL          *EndLabel;
    LIST_ENTRY                  *Link;
    UINT8                       PortIndex = 0;
    SATA_CONTROLLER_INFO        *SataControllerInterface = NULL;
    SATA_CONTROLLER_INFO        *TempSataCtrlInterface = NULL;
//<xuyj1001-20200408 Support ASM1061R Device +>
    if(gSataInforInit)
   		 return;
//<xuyj1001-20200408 Support ASM1061R Device ->
    StartOpCodeHandle = HiiAllocateOpCodeHandle ();
    EndOpCodeHandle = HiiAllocateOpCodeHandle ();
    
    // Create Hii Extended Label OpCode as the start and end opcode
    StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (
            StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
    
    StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;

    EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (
            EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
    
    EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;

    StartLabel->Number = SATA_DEVICE_INFO_MAIN_FORM_LABEL_START;
    EndLabel->Number = SATA_DEVICE_INFO_MAIN_FORM_LABEL_END;
    
    // Function to get the SATA controller and ATA/ATAPI device details
    
    Status = GetSataControllerDetails();
    if (EFI_ERROR(Status)) {
    	if(Status!=EFI_NOT_FOUND){
    		ASSERT_EFI_ERROR(Status);
    	}
        return;
    }
    
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
                            STRING_TOKEN(STR_EMPTY),
                            STRING_TOKEN(STR_EMPTY),
                            0,0);

    for (Link = SataDeviceInfoList.ForwardLink;Link != &SataDeviceInfoList;Link = Link->ForwardLink) {
        SataControllerInterface = BASE_CR(Link,SATA_CONTROLLER_INFO,SataControllerLink );
        
        // Updating SATA controller information dynamically
        HiiCreateSubTitleOpCode(StartOpCodeHandle,
                                    SataControllerInterface->ControllerStrId,
                                    STRING_TOKEN(STR_EMPTY),
                                    0,0);
        
        for (PortIndex = 0; PortIndex < SataControllerInterface->MaxPortSupported; PortIndex++) {
            // Updating SATA devices information dynamically in the subform
#if DISPLAY_HDD_FEATURE_INFO   
            if (SataControllerInterface->gHddNameStrId[PortIndex] != 0) {
                // Add goto control
                HiiCreateGotoOpCode (
                    StartOpCodeHandle,
                    SATA_DEVICE_INFO_SUB_FORM_ID,
                    SataControllerInterface->gSataPortStrId[PortIndex],
                    STRING_TOKEN(STR_EMPTY),
                    EFI_IFR_FLAG_CALLBACK,
                    (EFI_QUESTION_ID)(gSataDeviceBaseKey + PortIndex + MaxPort)
                );
            } else {
                 HiiCreateSubTitleOpCode(StartOpCodeHandle,
                                        SataControllerInterface->gSataPortStrId[PortIndex],
                                        STRING_TOKEN(STR_EMPTY),
                                        0,0);
            }
#else
            // Update the Device Name.
            HiiCreateTextOpCode(StartOpCodeHandle,
                                SataControllerInterface->gSataPortStrId[PortIndex],
                                STRING_TOKEN(STR_EMPTY),
                                SataControllerInterface->gHddNameStrId[PortIndex]);
            if (SataControllerInterface->gHddSizeStrId[PortIndex] != 0) {
                // Update the Size Information
                HiiCreateTextOpCode(StartOpCodeHandle,
                                    STRING_TOKEN(STR_EMPTY),
                                    STRING_TOKEN(STR_EMPTY),
                                    SataControllerInterface->gHddSizeStrId[PortIndex]);
            }
#endif
        }
        HiiCreateSubTitleOpCode(StartOpCodeHandle,       
                                STRING_TOKEN(STR_EMPTY),
                                STRING_TOKEN(STR_EMPTY),
                                0,0);
#if DISPLAY_HDD_FEATURE_INFO
        MaxPort = MaxPort +  SataControllerInterface->MaxPortSupported;
#else  
        // Free the Memory Allocated
        pBS->FreePool(SataControllerInterface->gHddSizeStrId);
        pBS->FreePool(SataControllerInterface->gHddNameStrId);
        pBS->FreePool(SataControllerInterface->gSataPortStrId);
        
        // Free the Previous Node
        if (TempSataCtrlInterface != NULL) {
            pBS->FreePool(TempSataCtrlInterface);    
            TempSataCtrlInterface= NULL;
        }
        
        TempSataCtrlInterface=SataControllerInterface;
#endif          
    }
    // Free the Last node
    if (TempSataCtrlInterface != NULL) {
        pBS->FreePool(TempSataCtrlInterface);        
    }
    // Updating the Sata form
    HiiUpdateForm (
                gSataHiiHandle,
                &gSataConfigFormSetGuid,
                SATA_DEVICE_INFO_MAIN_FORM_ID,
                StartOpCodeHandle,
                EndOpCodeHandle
            );
    if (StartOpCodeHandle != NULL) {
        HiiFreeOpCodeHandle (StartOpCodeHandle);
        HiiFreeOpCodeHandle (EndOpCodeHandle);
    }
    gSataInforInit=TRUE;//<xuyj1001-20200408 Support ASM1061R Device +>
    return;
}

/**
    This function is the entry point for the Driver. 
    Registers callback for the Setup entry Event and update the Sata String in the callback 

    @param  EFI_HANDLE          ImageHandle     - A collection of related interfaces
    @param  EFI_SYSTEM_TABLE    *SystemTable    - Pointer to the system table

    @retval Status

**/
EFI_STATUS
EFIAPI
SataDeviceInfoEntryPoint (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
)
{
    EFI_STATUS      Status;
    VOID            *SetupRegistration;
    EFI_GUID        SetupEnterGuid = AMITSE_SETUP_ENTER_GUID;
    EFI_EVENT       SetupEnterEvent;
    
    InitAmiLib(ImageHandle, SystemTable);
    
#if !DISPLAY_HDD_FEATURE_INFO
    // Loads ImageHandle related strings
    Status = LoadStrings( ImageHandle, &gSataHiiHandle );
#else
    // Load setup page and update strings
    Status = LoadResources(ImageHandle, sizeof(SetupCallBack) / sizeof(CALLBACK_INFO), SetupCallBack, NULL);
    gSataHiiHandle = SetupCallBack[0].HiiHandle;
#endif 
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return Status;
    }
    // Register a callback when the system enter into Setup
    RegisterProtocolCallback(
                        &SetupEnterGuid, 
                        InitSataDynamicIfrContents,
                        NULL, 
                        &SetupEnterEvent, 
                        &SetupRegistration
                        );
    return Status;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
