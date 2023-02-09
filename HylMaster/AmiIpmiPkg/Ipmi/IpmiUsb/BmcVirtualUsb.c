/** @file BmcVirtualUsb.c
    This file provides Power Consumption Support feature in BMC
    over USB interface (enable or disable BMC USB interface through KCS 
    using functions provoded in this file).
    
**/

//-------------------------------------------------------------------------

#include "DxeIpmiUsb.h"
//<kuaibai-20190320-Auto control BMC virtual usb status +> 
#include <AmiDxeLib.h>
extern EFI_RUNTIME_SERVICES *gRT;
extern EFI_BOOT_SERVICES *gBS;
extern EFI_SYSTEM_TABLE *gST;
//<kuaibai-20190320-Auto control BMC virtual usb status -> 
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------

//NetFn
#define EFI_APP_ENABLE_MESSAGE_CHANNEL_RECEIVE  0x32
#define NETFN_OEM  0x3A //<kuaibai-20190320-Auto control BMC virtual usb status> 

// Virtual USB devices State
#define     VIRTUAL_USB_DEVICES_ENABLED     0x00
#define     VIRTUAL_USB_DEVICES_DISABLED    0x01
#define     COMMAND_SUCCESS                 0x01

// LUN definition
#ifndef BMC_LUN
    #define BMC_LUN     0x00
#endif

//<kuaibai-20190320-Auto control BMC virtual usb status +>
EFI_EVENT   gLegacyBootEvent;
EFI_EVENT   gExitBootServicesEvent;
//EFI_EVENT   gReadyToBootEvent;
//<kuaibai-20190320-Auto control BMC virtual usb status ->

//Command for setting
#define     BMC_OEM_COM_FOR_POWER_SAVING_SEND_COMMAND       0xAA
//Mizl-20220329-SGEZT22763#Disable USB Virtual Lan Directly For KCS Interface+>
#define     BMC_OEM_COM_DISABLE_USB_VIRTUAL_LAN_COMMAND     0xB5 
//Mizl-20220329-SGEZT22763#Disable USB Virtual Lan Directly For KCS Interface->

// Command to Get
#define     BMC_OEM_COM_FOR_POWER_SAVING_RECEIVE_COMMAND    0xAB

UINT8       gUsbEnableCommandData = 0;    // 0- Enabling BMC USB Interface
UINT8       gUsbDisableCommandData = 1;   // 1- Disabling BMC USB Interface
UINT8       gResponseData;

EFI_IPMI_TRANSPORT      *gIpmiTransport = NULL;

#if VIRTUAL_BMC_USB_INTERFACE
extern  BMC_USB_ENABLE_AND_DISABLE   BMC_USB_HOOK_LIST  EndOfBmcUsbInterfaceHookListFunctions;
BMC_USB_ENABLE_AND_DISABLE  *BmcUsbInterfaceHookList[]  =   {BMC_USB_HOOK_LIST NULL};
#endif  // #if VIRTUAL_BMC_USB_INTERFACE

//<kuaibai-20190320-Auto control BMC virtual usb status +>
typedef struct {
  BOOLEAN  Flag;
  UINT8    Type;
  UINT8    Reserved;
} OEM_OOB_FW;
//<kuaibai-20190320-Auto control BMC virtual usb status ->
//Mizl-20220329-SGEZT22763#Disable USB Virtual Lan Directly For KCS Interface+>
EFI_STATUS DisableUSBVirtualLanDirectly(VOID);
//Mizl-20220329-SGEZT22763#Disable USB Virtual Lan Directly For KCS Interface->

/**
    This routine calls the Bmc Usb Enable related functions.

    @param  Event Callback event
    @param  Context Pointer to calling context

    @retval VOID
**/
#if VIRTUAL_BMC_USB_INTERFACE
VOID
EFIAPI
BmcUsbInterfaceHook (
  IN    EFI_EVENT Event,
  IN    VOID      *Context
  )
{
//<kuaibai-20190320-Auto control BMC virtual usb status +>
  //UINTN   Index;
  EFI_STATUS    Status;
  SERVER_IPMI_DEBUG((DEBUG_INFO, "%a Entry...\n", __FUNCTION__));
  Status = BmcUsbInterface();
  SERVER_IPMI_DEBUG((DEBUG_INFO, "[SK] BmcUsbInterface status: %r\n", Status));

  //Mizl-20220329-SGEZT22763#Disable USB Virtual Lan Directly For KCS Interface+>
  Status = DisableUSBVirtualLanDirectly();
  SERVER_IPMI_DEBUG((DEBUG_INFO, "[SK] DisableUSBVirtualLanDirectly status: %r\n", Status));
  //Mizl-20220329-SGEZT22763#Disable USB Virtual Lan Directly For KCS Interface->

/*

   for (Index = 0; BmcUsbInterfaceHookList[Index]; Index++) {
         Status = BmcUsbInterfaceHookList[Index]();
    }
//<kuaibai-20190320-Auto control BMC virtual usb status ->
*/
  if (!EFI_ERROR(Status)) {
    gBS->CloseEvent (Event);
  }
}
#endif  // #if VIRTUAL_BMC_USB_INTERFACE

//<kuaibai-20190320-Auto control BMC virtual usb status +>
/**
    This function checks the status of BMC Virtual USB devices.
        00h = Virtual USB devices enabled
        01h = Virtual USB devices disabled

    @param  VirtualUsbDevicesState Address of Virtual Usb Device State to get the Status
    @return EFI_STATUS Status
**/
EFI_STATUS
BmcUsbStatus (
  UINT8  *VirtualUsbDevicesState
)
{
  // Checking USB Enabled or not
  EFI_STATUS  Status;
  UINT8       ResponseDataSize = 0;
  UINT8       tResponseData = 0;

  ResponseDataSize = sizeof(tResponseData);
  Status = gIpmiTransport->SendIpmiCommand (
                           gIpmiTransport,
                           NETFN_OEM,
                           BMC_LUN,
                           BMC_OEM_COM_FOR_POWER_SAVING_RECEIVE_COMMAND,
                           NULL,
                           0,
                           &tResponseData,
                           &ResponseDataSize
                           );
  DEBUG((DEBUG_INFO, "[SK] Send 0xab in USB Status:%r Response:%x \n", Status, tResponseData));

  if (!EFI_ERROR(Status)) {  // Returning response data
    if (tResponseData == VIRTUAL_USB_DEVICES_ENABLED || tResponseData == VIRTUAL_USB_DEVICES_DISABLED) {
      *VirtualUsbDevicesState = tResponseData;
      return EFI_SUCCESS;
    }
  }
  //other than response code
  return EFI_DEVICE_ERROR;
}

/**
    This function enables BMC Virtual USB devices if not disabled,
    else exits.

    @param  NONE
    @retval VOID
**/
VOID
EFIAPI
EnableUsbInterface ()
{
  EFI_STATUS  Status;
  UINT8       ResponseDataSize = 0;
  UINT8       tResponseData = 0;

  if (gIpmiTransport != NULL)
  {
    ResponseDataSize = sizeof(tResponseData);
    Status = gIpmiTransport->SendIpmiCommand (
                             gIpmiTransport,
                             NETFN_OEM,
                             BMC_LUN,
                             BMC_OEM_COM_FOR_POWER_SAVING_SEND_COMMAND,
                             &gUsbEnableCommandData,
                             1,
                             &tResponseData,
                             &ResponseDataSize
                             );
    DEBUG((DEBUG_INFO, "[SK] Enable Status: %r and ResponseData: %x \n", Status, tResponseData));

    if (!EFI_ERROR(Status))
    {
      if (tResponseData == COMMAND_SUCCESS)
        DEBUG((DEBUG_INFO, "[SK] Virtual USB device enabled ok!\n"));
      else
        DEBUG((DEBUG_ERROR, "[SK] Virtual USB device enabled fail!\n"));
    }
  }
  else
    DEBUG((DEBUG_ERROR, "[SK] gIpmiTransport == NULL\n"));
}

BOOLEAN
isSavePldm()
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT8       ResponseData[10]={0};
  UINT8       ResponseDataSize = 0;

  Status = gIpmiTransport->SendIpmiCommand(gIpmiTransport,
                           NETFN_OEM,
                           BMC_LUN,
                           0x3b,
                           NULL,
                           0,
                           (UINT8*)ResponseData,
                           &ResponseDataSize
                           );
  DEBUG((-1, "[SK] isSavePldm: Get 0x3b Status: %r\n", Status));
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "[SK] isSavePldm: Get 0x3b fail! \n"));
    return TRUE;
  }

  if ( (gIpmiTransport->CommandCompletionCode==0) && \
      (ResponseData[0]==0x01 || ResponseData[0]==0x02) )
  {
    DEBUG((-1, "[SK] need save!\n"));
    return FALSE;
  }
  else
  {
    DEBUG((-1, "[SK] save already!\n"));
    return TRUE;
  }
}

/**
    This function disables BMC Virtual USB devices if not disabled,
    else exits.

    @param  Event Callback event
    @param  Context Pointer to calling context

    @retval VOID
**/
VOID
EFIAPI
DisableUsbInterface (
  IN  EFI_EVENT Event,
  IN  VOID      *Context
)
{
  EFI_STATUS    Status = EFI_SUCCESS;
  UINT8         ResponseDataSize = 0;
  UINTN         VarSize = 0;
  OEM_OOB_FW  *VarData = NULL;

  if (isSavePldm())
    DEBUG((-1,"[SK] Go on!\n"));
  else
    return;

  // Checking BMC USB Status and getting the Response Data
  VarSize = sizeof(OEM_OOB_FW);
  Status = gRT->GetVariable(
                L"OEM_OOB_FW_VAR",
                &gAmiTseOemPortingVar1Guid,
                NULL,
                &VarSize,
                (VOID **)&VarData
                );
  DEBUG((EFI_D_INFO, "[SK] VarData->Flag = %x %r;\n",VarData->Flag, Status));

  if (!EFI_ERROR(Status) && (TRUE==VarData->Flag)){
    DEBUG((EFI_D_INFO, "[SK] Still need virtual USB, so Clear&return!\n"));
    Status = gRT->SetVariable(
                  L"OEM_OOB_FW_VAR",
                  &gAmiTseOemPortingVar1Guid,
                  0,
                  0, 
                  NULL
                  );
    DEBUG((DEBUG_INFO, "[SK] OEM_OOB_FW_VAR clear:%r\n",Status));
    return;
  } else {
    DEBUG((DEBUG_INFO, "[SK] go to disable virtual USB device!\n"));
    ResponseDataSize = sizeof(gResponseData);
    Status = gIpmiTransport->SendIpmiCommand (
                               gIpmiTransport,
                               NETFN_OEM,
                               BMC_LUN,
                               BMC_OEM_COM_FOR_POWER_SAVING_SEND_COMMAND,
                               &gUsbDisableCommandData,
                               1,
                               &gResponseData,
                               &ResponseDataSize
                               );
    DEBUG((DEBUG_INFO, "[SK] Disable Status: %r and ResponseData: %x \n", Status, gResponseData));
    if (!EFI_ERROR(Status)) {
      if (gResponseData == COMMAND_SUCCESS)
        DEBUG((DEBUG_INFO, "Virtual USB devices are Disabled Using Send Ipmi Command\n"));
      else
        DEBUG((DEBUG_ERROR, "Virtual USB devices Disabling FAILED Using Send Ipmi Command\n"));
    }
  }
}

/**
    This function enables BMC Virtual USB devices if not enabled,
    else exits.

    @param  VOID
    @return EFI_STATUS Return Status
**/

EFI_STATUS
BmcUsbInterface (VOID)
{
  EFI_STATUS  Status;

  Status = gBS->LocateProtocol (&gEfiDxeIpmiTransportProtocolGuid, NULL, &gIpmiTransport);
  DEBUG((DEBUG_INFO, "[SK] LocateProtocol gIpmiTransport :: Status - %r\n", Status));

  if (!EFI_ERROR(Status))
  {
    // Status = BmcUsbStatus (&VirtualUsbDevicesState);
     Status = gBS->CreateEvent (
                    EVT_SIGNAL_EXIT_BOOT_SERVICES,
                    TPL_NOTIFY,
                    DisableUsbInterface,
                    NULL,
                    &gExitBootServicesEvent
                    );
    DEBUG((DEBUG_INFO, "[SK] register gExitBootServicesEvent %r\n",Status));

    Status = EfiCreateEventLegacyBootEx (
                    TPL_NOTIFY,
                    DisableUsbInterface,
                    NULL,
                    &gLegacyBootEvent
                    );
    DEBUG((DEBUG_INFO, "[SK] register gLegacyBootEvent %r\n",Status));

    EnableUsbInterface();

 //  Status = EfiCreateEventReadyToBootEx( 
 //                   TPL_CALLBACK,
 //                   DisableUsbInterface,
 //                   NULL,
 //                   &gReadyToBootEvent
 //                   );
    DEBUG((DEBUG_INFO, "[SK] register gReadyToBootEvent %r\n",Status));
  }
  return Status;
}
//<kuaibai-20190320-Auto control BMC virtual usb status ->
//Mizl-20220329-SGEZT22763#Disable USB Virtual Lan Directly For KCS Interface+>
EFI_STATUS
DisableUSBVirtualLanDirectly(VOID)
{
  EFI_STATUS  Status;
  UINT8       ResponseDataSize = 0;
  UINT8       tResponseData = 0;
  UINT8       DisableVUsbLanParam = 1;    // 1- Disable BMC USB Lan Interface

  if (gIpmiTransport != NULL)
  {
    ResponseDataSize = sizeof(tResponseData);
    Status = gIpmiTransport->SendIpmiCommand (
                             gIpmiTransport,
                             NETFN_OEM,
                             BMC_LUN,
                             BMC_OEM_COM_DISABLE_USB_VIRTUAL_LAN_COMMAND,
                             &DisableVUsbLanParam,
                             1,
                             &tResponseData,
                             &ResponseDataSize
                             );
    DEBUG((DEBUG_INFO, "[SK] Disable USB Virtual Lan Status: %r and ResponseData: %x \n", Status, tResponseData));

    if (EFI_ERROR(Status))
      return Status;
    
    if (tResponseData == COMMAND_SUCCESS)
      DEBUG((DEBUG_INFO, "[SK] Virtual USB Lan Disable ok!\n"));
    else
      DEBUG((DEBUG_ERROR, "[SK] Virtual USB device Disable fail!\n"));
    
  }
  else
    DEBUG((DEBUG_ERROR, "[SK] gIpmiTransport == NULL\n"));

  return Status;    
}
//Mizl-20220329-SGEZT22763#Disable USB Virtual Lan Directly For KCS Interface+>