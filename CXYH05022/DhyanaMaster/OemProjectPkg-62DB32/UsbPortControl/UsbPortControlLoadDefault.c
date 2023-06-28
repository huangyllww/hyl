#include <AmiDxeLib.h>
#include "token.h"
#include <Include\Setup.h>
#include <Library\DebugLib.h>
#include "OemLib.h"
#include "TseSetupTransfer.h"
#include "UsbPortControl.h"

#define USBStringID(Socket, Die, Port,String) 0x0

USB_PORT_CONTROL UsbPortControl[] = {
    {CPU0_DIE0_Port0,   {TRUE, L"NULL",L"NULL",USBStringID(0, 0, 0,_PROMPT),USBStringID(0, 0, 0,_PROMPT_HELP)}},
    {CPU0_DIE0_Port1,   {TRUE, L"NULL",L"NULL",USBStringID(0, 0, 0,_PROMPT),USBStringID(0, 0, 0,_PROMPT_HELP)}},
    {CPU0_DIE1_Port0,   {TRUE, L"NULL",L"NULL",USBStringID(0, 0, 0,_PROMPT),USBStringID(0, 0, 0,_PROMPT_HELP)}},
    {CPU0_DIE1_Port1,   {TRUE, L"NULL",L"NULL",USBStringID(0, 0, 0,_PROMPT),USBStringID(0, 0, 0,_PROMPT_HELP)}},
    {PortTerminate,     {TRUE, L"NULL",L"NULL",USBStringID(0, 0, 0,_PROMPT),USBStringID(0, 0, 0,_PROMPT_HELP)}}};


VOID LoadUSBPortPresent()
{
  EFI_STATUS Status;
  UINT8 iPort;
  UINTN VariableSize = sizeof(SETUP_DATA);
  SETUP_DATA *SetupData;
  EFI_GUID gEfiSetupGuid = SETUP_GUID;
  EFI_GUID gTseTransferGuid = TSE_TRANSFER_GUID;
  Tse_Setup_transfer *pTseSetuptransferInterface = NULL;

  if (pTseSetuptransferInterface == NULL)
  {
    Status = pBS->LocateProtocol(&gTseTransferGuid, NULL, &pTseSetuptransferInterface);
    DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]Status:%r\r\n",__FUNCTION__,__LINE__,Status));
    if (EFI_ERROR(Status))
      return;
  }

  if (pTseSetuptransferInterface == NULL)
    return;

  Status = pTseSetuptransferInterface->VarGetBuffer(L"Setup", gEfiSetupGuid, (UINT8 **)&SetupData, &VariableSize);
  DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]Status:%r\r\n",__FUNCTION__,__LINE__,Status));

  if (EFI_ERROR(Status))
    return;

  /**
    Note:Disable all Port as disabled in default
    **/
  for (iPort = 0; iPort < TOTAL_USB_COUNT; iPort++)
    SetupData->USBPortPresent[iPort] = USB_PORT_DISABLE;

  /**
   Note:Enable USB Port By Porting Table
   **/
  for (iPort = 0; UsbPortControl[iPort].CPUUsbPortLocation != PortTerminate; iPort++)
    SetupData->USBPortPresent[UsbPortControl[iPort].CPUUsbPortLocation] = 1;

  for (iPort = 0; iPort < TOTAL_USB_COUNT; iPort++)
    DEBUG((EFI_D_INFO, "Port:%x USBPortPresent:%x USBPortControl:%x\r\n", iPort, SetupData->USBPortPresent[iPort], SetupData->USBPortControl[iPort]));

  return;
}