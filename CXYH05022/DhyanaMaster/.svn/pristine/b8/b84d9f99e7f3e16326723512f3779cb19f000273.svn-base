/************************************************************************
 * OEM PCIeDev Module                                                 *
 * Enumerate PCIe Device Deprived from Root Bridge                      *
 * Support for OptionROM Selective Execution                            *
 * Date: 2015/5/27                                                      *
 ************************************************************************/

//----------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------
#include "OemPCIeDevSetup.h" //<Yaosr001-20191220 Realizes the linkage of 'BootOptionFilter' and 'PCIe OptionRom'>

//<Lvshh001-20200721 Define Independent variable and for Dynamic PCIE Dev to solve oob default and current issue +>
#include <OemPCIeData.h>
#define  OEMPCIEVAR  L"OEMPCIE_DEV"
//<Lvshh001-20200721 Define Independent variable and for Dynamic PCIE Dev to solve oob default and current issue ->

//<Yaosr001-20191220 Realizes the linkage of 'BootOptionFilter' and 'PCIe OptionRom'. +>
EFI_STATUS  BootOptionFilterCallback(
    IN  EFI_HII_HANDLE HiiHandle,
    IN  UINT16  Class,
    IN  UINT16  SubClass,
    IN  UINT16  KeyValue
)
{
  EFI_STATUS            Status = EFI_SUCCESS;
  EFI_GUID              gSetupGuid = SETUP_GUID;
  SETUP_DATA            pSetup;
  SETUP_DATA            *IfrNvData = &pSetup;
  UINTN                 BufferSize;
  
  //<Lvshh001-20200721 Define Independent variable and for Dynamic PCIE Dev to solve oob default and current issue +>
  EFI_GUID              gEfiSetupGuid = OEMPCIEDEV_VARSTORE_GUID;
  OEMPCIE_SETUP_DATA  pEfiSetup;
  OEMPCIE_SETUP_DATA  *PcieNvData = &pEfiSetup;
  UINTN                 PCIeBufferSize;
  //<Lvshh001-20200721 Define Independent variable and for Dynamic PCIE Dev to solve oob default and current issue ->
  
  CALLBACK_PARAMETERS   *CallbackParameters = NULL;
  UINT8                 Current_Value = 0xFF;

  DEBUG ((-1, "[BootOptionFilterCallback]: ***** INTERACTIVE callback.  KEY=%d\n", KeyValue));

  if ((KeyValue != CSM_BOOT_OPTION_FILTER_KEY))
    return EFI_INVALID_PARAMETER;

  CallbackParameters = GetCallbackParameters();
  if (CallbackParameters->Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD)
    return EFI_UNSUPPORTED;
  else if((CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGING) && (CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED))
    return EFI_INVALID_PARAMETER;

  //Retrieve uncommitted data from Browser
  BufferSize = sizeof(SETUP_DATA);
  Status = HiiLibGetBrowserData(&BufferSize, IfrNvData, &gSetupGuid, L"Setup");
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_INFO, "[BootOptionFilterCallback]: HiiLibGetBrowserData (%r)\n", Status));
    return Status;
  }
  
  //<Lvshh001-20200721 Define Independent variable and for Dynamic PCIE Dev to solve oob default and current issue +>
  PCIeBufferSize = sizeof(OEMPCIE_SETUP_DATA);
  Status = HiiLibGetBrowserData(&PCIeBufferSize, PcieNvData, &gEfiSetupGuid, OEMPCIEVAR);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_INFO, "[BootOptionFilterCallback]: HiiLibGetBrowserData (%r)\n", Status));
    return Status;
  }
  //<Lvshh001-20200721 Define Independent variable and for Dynamic PCIE Dev to solve oob default and current issue ->
  
  if (CallbackParameters->Action == EFI_BROWSER_ACTION_CHANGING) {

    Current_Value = CallbackParameters->Value->u8;  //current user selection
    if (Current_Value == 0x00) { // UEFI and Legacy
      PcieNvData->OpROMEN[PXEINDX] = 2;
      PcieNvData->OpROMEN[MASSINDX] = 2;
      PcieNvData->OpROMEN[VIDEOINDX] = 2;
      PcieNvData->OpROMEN[OLDINDX] = 1;
#if FIXED_BOOT_ORDER_SUPPORT == 1
      IfrNvData->BootMode = 2;
#endif
    } else if (Current_Value == 0x01) { //Legacy
      PcieNvData->OpROMEN[PXEINDX] = 2;
      PcieNvData->OpROMEN[MASSINDX] = 2;
      PcieNvData->OpROMEN[VIDEOINDX] = 2;
      PcieNvData->OpROMEN[OLDINDX] = 2;
#if FIXED_BOOT_ORDER_SUPPORT == 1
      IfrNvData->BootMode = 0;
#endif
    } else if (Current_Value == 0x02) { //UEFI
      PcieNvData->OpROMEN[PXEINDX] = 1;
      PcieNvData->OpROMEN[MASSINDX] = 1;
      PcieNvData->OpROMEN[VIDEOINDX] = 1;
      PcieNvData->OpROMEN[OLDINDX] = 1;
#if FIXED_BOOT_ORDER_SUPPORT == 1
      IfrNvData->BootMode = 1;
#endif
    }
  }

  Status = HiiLibSetBrowserData(BufferSize, IfrNvData, &gSetupGuid, L"Setup");
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_INFO, "[BootOptionFilterCallback]: HiiLibSetBrowserData (%r)\n", Status));
    return Status;
  }
  
  //<Lvshh001-20200721 Define Independent variable and for Dynamic PCIE Dev to solve oob default and current issue +>
  Status = HiiLibSetBrowserData(PCIeBufferSize, PcieNvData, &gEfiSetupGuid, OEMPCIEVAR);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_INFO, "[BootOptionFilterCallback]: HiiLibSetBrowserData (%r)\n", Status));
    return Status;
  }
  //<Lvshh001-20200721 Define Independent variable and for Dynamic PCIE Dev to solve oob default and current issue ->
  
  return EFI_SUCCESS;

}
//<Yaosr001-20191220 Realizes the linkage of 'BootOptionFilter' and 'PCIe OptionRom'. ->
