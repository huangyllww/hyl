
#include <Uefi.h>
#include <Uefi/UefiBaseType.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/OemDebugLevelSerivce.h>
#include "FileGuidNameErrorLevelTableHeader.h"
#include <Library/UefiRuntimeServicesTableLib.h>
#include "OemDebugAlways_DebugLevel.h"
#include "OemLib.h"
#include "AmiDxeLib.h"
#include "OemIpmiNetFnDefinitions.h"
#include "OemDebugAlways_SyncDebugMode.h"
#include "Protocol/IpmiTransportProtocol.h"
#include "OemDebugAlways.h"
#include "Library/OemDebugLevelLib.h"
#include "Library/OemDebugAlwaysLib.h"
EFI_IPMI_TRANSPORT *gIpmiTransport = NULL;
OEM_DEBUG_LEVEL_DATABASE *pOemDebugLevel_SetupConfig = NULL;
EFI_GUID gOEMDebugLevelDataBaseGuid = OEM_DEBUG_LEVEL_DATABASE_GUID;
UINT8 Response_GetModifyData[OEM_IPMI_SETUP_RESPONSE_LENGTH] = {0};

#define SyncDebugLevelDataBaseSetting(ByteIndex, ValidFlag, SetupConfigName, BMCConfigName)                                                                                        \
  if (pDebugOptionGet->DebugMode_Byte##ByteIndex.Bits.##ValidFlag)                                                                                                                 \
  {                                                                                                                                                                                \
    fSyncDebugLevelWithBMC = TRUE;                                                                                                                                                 \
    pDebugOptionSet->DebugMode_Byte##ByteIndex.Bits.BMCConfigName = pOemDebugLevel_SetupConfig->##SetupConfigName = pDebugOptionGet->DebugMode_Byte##ByteIndex.Bits.BMCConfigName; \
  }                                                                                                                                                                                \
  else if (pDebugOptionGet->DebugMode_Byte##ByteIndex.Bits.BMCConfigName != pOemDebugLevel_SetupConfig->##SetupConfigName)                                                         \
  {                                                                                                                                                                                \
    fSyncDebugLevelWithBIOS = TRUE;                                                                                                                                                \
    pDebugOptionSet->DebugMode_Byte##ByteIndex.Bits.BMCConfigName = pOemDebugLevel_SetupConfig->##SetupConfigName;                                                                 \
  }

/**
GC_TODO: GetDebugModeFromBMC - Get Debug Level Modify Data From BMC

@retval EFI_SUCCESS   - GC_TODO: add retval description
@retval EFI_NOT_FOUND - GC_TODO: add retval description
**/
EFI_STATUS GetDebugModeFromBMC(void)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT8 Cmd_GetModifyDebugModeSetting[2] = {BiosSetup_DebugOption, BiosSetup_ModifiedData}, Cmd_Size = sizeof(Cmd_GetModifyDebugModeSetting);
  UINT8 Response_Size = OEM_IPMI_SETUP_RESPONSE_LENGTH;

  MODULE_START

  if (gIpmiTransport == NULL)
  {
    Status = gBS->LocateProtocol(&gEfiDxeIpmiTransportProtocolGuid, NULL, (VOID **)&gIpmiTransport);
    if (EFI_ERROR(Status))
      MODULE_END(Status)
  }

  Status = gIpmiTransport->SendIpmiCommand(
      gIpmiTransport,
      NETFN_OEM, // raw 0x3A 0xC1
      BMC_LUN,
      CMD_OEM_GET_SETUP_BOOT_OPTIONS, // CMD_GET_DEBUG_FLAG
      (UINT8 *)&Cmd_GetModifyDebugModeSetting,
      Cmd_Size,
      (UINT8 *)&Response_GetModifyData,
      (UINT8 *)&Response_Size);
  if (EFI_ERROR(Status))
    MODULE_END(Status)

  //Print Debug Message
  DumpResponseData(Response_GetModifyData)

  //Check Response Status
  CheckResponseStatus(Status, Response_GetModifyData[0])
  MODULE_END(Status = EFI_SUCCESS)
}

/**
GC_TODO: SyncBMCSetting - Sync Debug Level Setting With BMC

@retval EFI_SUCCESS   - GC_TODO: add retval description
@retval EFI_NOT_FOUND - GC_TODO: add retval description
**/
EFI_STATUS
SyncBMCSetting(
    IN UINT8 *BmcDebugOption)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT8 Cmd_SetDebugModeSetting[] = {0x01, BiosSetup_DebugOption, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, Cmd_Size = sizeof(Cmd_SetDebugModeSetting);
  UINT8 Response_GetCurrentData[OEM_IPMI_SETUP_RESPONSE_LENGTH] = {0}, Response_Size = OEM_IPMI_SETUP_RESPONSE_LENGTH;

  MODULE_START

  if (gIpmiTransport == NULL)
  {
    Status = gBS->LocateProtocol(&gEfiDxeIpmiTransportProtocolGuid, NULL, (VOID **)&gIpmiTransport);
    if (EFI_ERROR(Status))
      MODULE_END(Status)
  }

  MemCpy(&Cmd_SetDebugModeSetting[2], BmcDebugOption, sizeof(UINT8) * 16);

  Status = gIpmiTransport->SendIpmiCommand(
      gIpmiTransport,
      NETFN_OEM, // raw 0x3A 0xC1
      BMC_LUN,
      CMD_OEM_SET_SETUP_BOOT_OPTIONS, // CMD_GET_DEBUG_FLAG
      (UINT8 *)&Cmd_SetDebugModeSetting,
      Cmd_Size,
      (UINT8 *)&Response_GetCurrentData,
      (UINT8 *)&Response_Size);

  if (EFI_ERROR(Status))
    MODULE_END(Status)

  MODULE_END(Status)
}

/**
GC_TODO: SyncSetupSetting - Sync Debug To NVRAM
@retval EFI_SUCCESS   - GC_TODO: add retval description
@retval EFI_NOT_FOUND - GC_TODO: add retval description
**/
EFI_STATUS
SyncSetupSetting()
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINTN Size;
  OEM_DEBUG_LEVEL_DATABASE *OemDebugLevelConfigData = NULL;
  UINT32 Attributes;
  OEM_COMMON_SETUP_DATA OEMSetupData;
  EFI_GUID OemSetupVariableGuid = OEM_COMMON_VAR_GUID;

  MODULE_START
  // Get DynamicDebug Nvram Variable
  Size = 0;
  Status = gRT->GetVariable(OEM_DEBUG_LEVEL_DATA_NAME, &gOEMDebugLevelDataBaseGuid, NULL, &Size, NULL);
  if (Status == EFI_BUFFER_TOO_SMALL)
  {
    // Allocate memory for Dynamic Debug Nvram Data
    OemDebugLevelConfigData = (OEM_DEBUG_LEVEL_DATABASE *)AllocateZeroPool(Size);
    if (OemDebugLevelConfigData == NULL)
      MODULE_END(Status = EFI_OUT_OF_RESOURCES)

    // Get DynamicDebug Nvram Variable
    Status = gRT->GetVariable(OEM_DEBUG_LEVEL_DATA_NAME, &gOEMDebugLevelDataBaseGuid, &Attributes, &Size, OemDebugLevelConfigData);
    if (EFI_ERROR(Status))
    {
      FreePool(OemDebugLevelConfigData);
      MODULE_END(Status)
    }
  }
  OemDebugLevelConfigData->DebugModeSupport = pOemDebugLevel_SetupConfig->DebugModeSupport;

  OemDebugLevelConfigData->DxePrintSupport = pOemDebugLevel_SetupConfig->DxePrintSupport;
  OemDebugLevelConfigData->PeiPrintSupport = pOemDebugLevel_SetupConfig->PeiPrintSupport;
  OemDebugLevelConfigData->RTSPrintSupport = pOemDebugLevel_SetupConfig->RTSPrintSupport;

  OemDebugLevelConfigData->PeiErrorLevel = pOemDebugLevel_SetupConfig->PeiErrorLevel;
  OemDebugLevelConfigData->PeiInfoLevel = pOemDebugLevel_SetupConfig->PeiInfoLevel;
  OemDebugLevelConfigData->PeiWarnLevel = pOemDebugLevel_SetupConfig->PeiWarnLevel;

  OemDebugLevelConfigData->DxeErrorLevel = pOemDebugLevel_SetupConfig->DxeErrorLevel;
  OemDebugLevelConfigData->DxeInfoLevel = pOemDebugLevel_SetupConfig->DxeInfoLevel;
  OemDebugLevelConfigData->DxeWarnLevel = pOemDebugLevel_SetupConfig->DxeWarnLevel;

  DumpDebugSetting(OemDebugLevelConfigData);

  // Set DynamicDebug Nvram Variable
  Status = gRT->SetVariable(OEM_DEBUG_LEVEL_DATA_NAME, &gOEMDebugLevelDataBaseGuid, Attributes, Size, OemDebugLevelConfigData);
  if (EFI_ERROR(Status))
    MODULE_END(Status)

  //Sync Debug Mask
  Size = sizeof(OEM_COMMON_SETUP_DATA);
  Status = gRT->GetVariable(L"OEM_VAR", &OemSetupVariableGuid, &Attributes, &Size, &OEMSetupData);
  if (EFI_ERROR(Status))
    MODULE_END(Status)

  OEMSetupData.DebugModeMask = OemDebugLevelConfigData->DebugModeSupport;
  Status = gRT->SetVariable(L"OEM_VAR", &OemSetupVariableGuid, Attributes, Size, &OEMSetupData);
  if (EFI_ERROR(Status))
    MODULE_END(Status)

  MODULE_END(Status)
}

VOID SyncDebugModeWitBmcNotifyFunction(
    IN EFI_EVENT Event,
    IN VOID *Context)
{
  EFI_STATUS Status;
  UINT8 aDebugOptionSet[OEM_IPMI_SETUP_RESPONSE_LENGTH] = {0};
  BMC_DEBUG_OPTION *pDebugOptionGet = NULL, *pDebugOptionSet = (BMC_DEBUG_OPTION *)&aDebugOptionSet[0];
  BOOLEAN fSyncDebugLevelWithBMC = FALSE, fSyncDebugLevelWithBIOS = FALSE;
  OEM_DEBUG_LEVEL_SERVICE_PROTOCOL *gOemDebugLevelServiceProtocol = NULL;
  OEM_DEBUG_LEVEL_OBJECT *OemDebugLevelObject = NULL;
  MODULE_START

  Status = GetDebugModeFromBMC();
  if (EFI_ERROR(Status))
    MODULE_VOID_END

  pOemDebugLevel_SetupConfig = GetOemDebugLevelDataBase();
  //Check Debug Option
  pDebugOptionGet = (BMC_DEBUG_OPTION *)&Response_GetModifyData[1];

  MemSet(pDebugOptionSet, sizeof(UINT8) * OEM_IPMI_SETUP_RESPONSE_LENGTH, 0); //Clear data

  SyncDebugLevelDataBaseSetting(3, Byte3ValidFlag, DebugModeSupport, DebugMode);
  //Sync Debug Phase Setting
  SyncDebugLevelDataBaseSetting(4, Byte4ValidFlag, PeiPrintSupport, PeiDebugSupport);
  SyncDebugLevelDataBaseSetting(4, Byte4ValidFlag, DxePrintSupport, DxeDebugSupport);
  SyncDebugLevelDataBaseSetting(4, Byte4ValidFlag, RTSPrintSupport, RTSDebugSupport);

  //Sync Pei Phase Debug Level
  SyncDebugLevelDataBaseSetting(8, Byte8ValidFlag, PeiInfoLevel, InfoDebugSupport);
  SyncDebugLevelDataBaseSetting(8, Byte8ValidFlag, PeiWarnLevel, WarnningDebugSupport);

  SyncDebugLevelDataBaseSetting(12, Byte12ValidFlag, PeiErrorLevel, ErrorDebugSupport);

  //Sync Dxe Phase Debug Level
  SyncDebugLevelDataBaseSetting(13, Byte8ValidFlag, DxeInfoLevel, InfoDebugSupport);
  SyncDebugLevelDataBaseSetting(13, Byte8ValidFlag, DxeWarnLevel, WarnningDebugSupport);

  SyncDebugLevelDataBaseSetting(17, Byte12ValidFlag, DxeErrorLevel, ErrorDebugSupport);

  DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]fSyncDebugLevelWithBMC=%x   fSyncDebugLevelWithBIOS=%x\r\n", __FUNCTION__, __LINE__, fSyncDebugLevelWithBMC, fSyncDebugLevelWithBIOS));
  //Sync Setup Setting
  if (fSyncDebugLevelWithBMC)
    Status = SyncSetupSetting();

  if (fSyncDebugLevelWithBIOS || fSyncDebugLevelWithBMC)
    Status = SyncBMCSetting(&aDebugOptionSet[0]);

  SyncDebugModeWithCMOS(pOemDebugLevel_SetupConfig);
  pBS->CloseEvent(Event);

  MODULE_VOID_END
}

EFI_STATUS
SyncDLWithBmcEntry(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_STATUS Status;
  VOID *mReg;
  EFI_EVENT mEvent;

  MODULE_START

  InitAmiLib(ImageHandle, SystemTable);

  // The Event after PciIoProtocol
  Status = pBS->CreateEvent(
      EFI_EVENT_NOTIFY_SIGNAL,
      TPL_CALLBACK,
      SyncDebugModeWitBmcNotifyFunction,
      NULL,
      &mEvent);

  if (EFI_ERROR(Status))
    MODULE_END(Status)

  Status = pBS->RegisterProtocolNotify(
      &gEfiDxeIpmiTransportProtocolGuid,
      mEvent,
      &mReg);
  if (EFI_ERROR(Status))
    MODULE_END(Status)

  MODULE_END(Status = EFI_SUCCESS)
}