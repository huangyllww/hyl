#include "OemLib.h"
#include "Ppi/IpmiTransportPpi.h"
#include "OemIpmiNetFnDefinitions.h"
#include "IpmiNetFnAppDefinitions.h"
#include "OemDebugAlways_DebugLevel.h"
#include "Ppi/OemDebugLevelSerivce.h"
#include "Library/DebugLib.h"
#include "Pi/PiPeiCis.h"
#include "Library/PeiServicesLib.h"

#include "OemDebugAlways_SyncDebugMode.h"
#include "AmiDxeLib.h"
#include "Library/OemDebugLevelLib.h"

#include "Library/OemDebugAlwaysLib.h"
#include "AmiLib.h"
//Global Declare Part
PEI_IPMI_TRANSPORT_PPI *IpmiTransportPpi = NULL;
OEM_DEBUG_LEVEL_DATABASE *pOemDebugLevelDataBase = NULL;
UINT8 Response_GetModifyData[OEM_IPMI_SETUP_RESPONSE_LENGTH]={0};
/**
 * Read Riser ID GPIO and config DXIO setting per riser id.
 */
EFI_STATUS
SyncDebugLevelWithBMC_NotifyFunction(
    IN EFI_PEI_SERVICES **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
    IN VOID *InvokePpi);

static EFI_PEI_NOTIFY_DESCRIPTOR PeiNotifyList[] = {
    {EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
     &gEfiPeiIpmiTransportPpiGuid, SyncDebugLevelWithBMC_NotifyFunction}};

EFI_STATUS GetDebugModeFromBMC(
    IN CONST EFI_PEI_SERVICES **PeiServices)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT8 Cmd_GetModifyDebugModeSetting[2] = {BiosSetup_DebugOption, BiosSetup_ModifiedData}, Cmd_Size = sizeof(Cmd_GetModifyDebugModeSetting);
  UINT8 Response_Size = OEM_IPMI_SETUP_RESPONSE_LENGTH;

  MODULE_START
  //
  //Locate IPMI Transport PPI to send commands to BMC.
  //
  if (IpmiTransportPpi == NULL)
  {
    Status = (*PeiServices)->LocatePpi(PeiServices, &gEfiPeiIpmiTransportPpiGuid, 0, NULL, (VOID **)&IpmiTransportPpi);
    if (EFI_ERROR(Status))
      MODULE_END(Status)
  }
  
  (*PeiServices)->SetMem((VOID *)&Response_GetModifyData[0], sizeof(UINT8)*Response_Size, 0);

  Status = IpmiTransportPpi->SendIpmiCommand(IpmiTransportPpi,NETFN_OEM, // raw 0x3A 0xC1
                                            BMC_LUN,CMD_OEM_GET_SETUP_BOOT_OPTIONS, // CMD_GET_DEBUG_FLAG
                                            (UINT8 *)&Cmd_GetModifyDebugModeSetting,
                                            Cmd_Size,(UINT8 *)&Response_GetModifyData,(UINT8 *)&Response_Size);
  DEBUG((EFI_D_ERROR, "\n[OemDbg][%a][%d]Get Mod Data Status:%r = %d\n\n", __FUNCTION__, __LINE__, Status, Response_GetModifyData[0]));

  //Print Debug Message
  DumpResponseData(Response_GetModifyData)

  //Check Response Status
  CheckResponseStatus(Status, Response_GetModifyData[0])

  MODULE_END(Status = EFI_SUCCESS)
}
/**
GC_TODO: SyncDebugLevelWithBMC_NotifyFunction

@retval EFI_SUCCESS   - GC_TODO: add retval description
@retval EFI_NOT_FOUND - GC_TODO: add retval description
**/
EFI_STATUS
SyncDebugLevelWithBMC_NotifyFunction(
    IN EFI_PEI_SERVICES **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
    IN VOID *InvokePpi)
{
  EFI_STATUS Status;
  BMC_DEBUG_OPTION *pBMCDebugOption = NULL;

  MODULE_START

  Status = GetDebugModeFromBMC(PeiServices);
  if (EFI_ERROR(Status))
    MODULE_END(Status)

  //Check Debug Option
  pBMCDebugOption = (BMC_DEBUG_OPTION *)&Response_GetModifyData[1];

  if (pBMCDebugOption == NULL)
    MODULE_END(Status = EFI_OUT_OF_RESOURCES)

  //Check Debug Support
  if (!(pBMCDebugOption->DebugMode_Byte3.Bits.Byte3ValidFlag | pBMCDebugOption->DebugMode_Byte4.Bits.Byte4ValidFlag))//SGEZT#29630:Improve Debug Level Module Logic
    MODULE_END(Status = EFI_ALREADY_STARTED)

  DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]Debug Support:%x\r\n", __FUNCTION__, __LINE__, pBMCDebugOption->DebugMode_Byte3.Bits.DebugMode));

  pOemDebugLevelDataBase = GetOemDebugLevelDataBase();

  if (pOemDebugLevelDataBase == NULL)
    MODULE_END(Status = EFI_OUT_OF_RESOURCES)

  pOemDebugLevelDataBase->DebugModeSupport = pBMCDebugOption->DebugMode_Byte3.Bits.DebugMode;
  //Check Debug Mode & Debug Level
  if (pBMCDebugOption->DebugMode_Byte4.Bits.Byte4ValidFlag)
  {

    pOemDebugLevelDataBase->PeiPrintSupport = pBMCDebugOption->DebugMode_Byte4.Bits.PeiDebugSupport;
    pOemDebugLevelDataBase->RTSPrintSupport = pBMCDebugOption->DebugMode_Byte4.Bits.RTSDebugSupport;
    pOemDebugLevelDataBase->DxePrintSupport = pBMCDebugOption->DebugMode_Byte4.Bits.DxeDebugSupport;//SGEZT#29630:Improve Debug Level Module Logic
  }
  
  if (pBMCDebugOption->DebugMode_Byte8.Bits.Byte8ValidFlag)
  {

    pOemDebugLevelDataBase->PeiInfoLevel = pBMCDebugOption->DebugMode_Byte8.Bits.InfoDebugSupport;
    pOemDebugLevelDataBase->PeiWarnLevel = pBMCDebugOption->DebugMode_Byte8.Bits.WarnningDebugSupport;
  }

  if (pBMCDebugOption->DebugMode_Byte12.Bits.Byte12ValidFlag)
    pOemDebugLevelDataBase->PeiErrorLevel = pBMCDebugOption->DebugMode_Byte12.Bits.ErrorDebugSupport;

  SyncDebugModeWithCMOS(pOemDebugLevelDataBase);

  MODULE_END(Status = EFI_SUCCESS)
}

/**
GC_TODO: SyncDebugLevelWithBMC_Entry

@retval EFI_SUCCESS   - GC_TODO: add retval description
@retval EFI_NOT_FOUND - GC_TODO: add retval description
**/
EFI_STATUS
SyncDebugLevelWithBMC_Entry(
    IN EFI_PEI_FILE_HANDLE FileHandle,
    IN CONST EFI_PEI_SERVICES **PeiServices)
{
  EFI_STATUS Status = EFI_SUCCESS;

  MODULE_START

  Status = (*PeiServices)->NotifyPpi(PeiServices, PeiNotifyList);

  MODULE_END(Status)
}
