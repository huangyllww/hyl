//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file RestIpmiDriver.c
 The driver binding and service binding protocol for Rest over Ipmi driver.
**/

#include <RestIpmiConfig.h>
#include "RestIpmiCommandDefines.h"
#include "RestIpmiDriver.h"
//mizl-2021-12-09-Sync Setup Setting With BIOS Setup Layout V0.5+>
#if defined(Redfish_Option_SUPPORT) && (Redfish_Option_SUPPORT==1)
#include <Setup.h> //<ChengXW-20201204 Add OOB option Switch>
#endif
//mizl-2021-12-09-Sync Setup Setting With BIOS Setup Layout V0.5->
//<kuaibai-20180807-post time improvement in B4 when redfish disable+>
#include <Protocol\IpmiTransportProtocol.h>
#define CMD_OEM_GET_REDFISH_SERVICE     0x3d
#define  NETFN_OEM                      0x3A
//<kuaibai-20180807-post time improvement in B4 when redfish disable->
EFI_HTTP_UTILITIES_PROTOCOL *gHttpUtilities = NULL;
REST_IPMI_USB_INSTANCE_DATA *gRestIpmiUsbInstance = NULL;

///
/// Driver Binding Protocol instance
///

EFI_DRIVER_BINDING_PROTOCOL gRestIpmiUsbDxeDriverBinding =
		{ RestIpmiDriverBindingSupported,
		RestIpmiDriverBindingStart,
		RestIpmiDriverBindingStop,
		REST_IPMI_USB_DRIVER_VERSION, NULL, NULL };

/**
 The event process routine when the http utilities protocol is installed
 in the system.

 @param[in]     Event         Not used.
 @param[in]     Context       The pointer to the instance data.

 **/
VOID
EFIAPI
HttpUtilitiesInstalledCallback( IN EFI_EVENT Event, IN VOID *Context) 
{
	gBS->LocateProtocol(&gEfiHttpUtilitiesProtocolGuid, NULL,
			(VOID **) &gHttpUtilities);

	//
	// Close the event if Http utilities protocol is located.
	//
	if (gHttpUtilities != NULL && Event != NULL) {
		SERVER_REST_DEBUG((EFI_D_INFO, "REST: gHttpUtilities installed\n"));
		gBS->CloseEvent(Event);
	}
}

//<kuaibai-20180807-post time improvement in B4 when redfish disable+>
/**
    This function will be called to check if bmc redfish service be enabled,
    if disabled we won't go run following OOB process.

    @param NONE

    @retval TRUE
    @retval FALSE
    
**/
BOOLEAN
BmcRedfishServiceEnable()
{
  EFI_STATUS            Status = EFI_SUCCESS;
  EFI_IPMI_TRANSPORT    *IpmiTransportProtocol;
  UINT8                 ResponseDataSize;
  UINT8                 ResponseData[10] = {0};
  UINT8                 BMC_Error_Count = 0;
//mizl-2021-12-09-Sync Setup Setting With BIOS Setup Layout V0.5+>
#if defined(Redfish_Option_SUPPORT) && (Redfish_Option_SUPPORT==1)
  //<ChengXW-20201204 Add OOB option Switch +>
    SETUP_DATA            SetupData;
    UINT32                SetupAttributes = 0;
    UINTN                 VarSize;
    
    EFI_GUID SetupGuid = SETUP_GUID;
    VarSize = sizeof(SETUP_DATA);
    Status = gST->RuntimeServices->GetVariable (
                    L"Setup",
                    &SetupGuid,
                    &SetupAttributes,
                    &VarSize,
                    &SetupData
                    );
    
    if ((!EFI_ERROR(Status))&&(!SetupData.RFOptionSwitch)) {
      return FALSE;
    }
    //<ChengXW-20201204 Add OOB option Switch -> 
#endif
//mizl-2021-12-09-Sync Setup Setting With BIOS Setup Layout V0.5->
  //Locate IPMI transport.
  Status = gBS->LocateProtocol (&gEfiDxeIpmiTransportProtocolGuid, NULL, (VOID **)&IpmiTransportProtocol);
  
  if (!EFI_ERROR(Status))
  {
    _IPMI_RETRY:
    ResponseDataSize = 10;
    Status = IpmiTransportProtocol->SendIpmiCommand (
                                IpmiTransportProtocol,
                                NETFN_OEM,
                                BMC_LUN,
                                CMD_OEM_GET_REDFISH_SERVICE,
                                NULL,
                                0,
                                ResponseData,
                                &ResponseDataSize
                                );
    DEBUG((DEBUG_ERROR, "\n[SK] Rest driver OOB SendIpmiCommand = %r !!\n",Status));
    if (EFI_ERROR(Status))
    {
      if (BMC_Error_Count < 3)
      {
          BMC_Error_Count++;   
          goto _IPMI_RETRY;
      }
      return FALSE;
    }
    
    if (!ResponseData[0])
    {
      DEBUG((DEBUG_ERROR, "\n[SK] redfish service is disabled! \n"));
      return FALSE;
    }
    else
    {
      DEBUG((DEBUG_ERROR, "\n[SK] redfish service is enabled! \n"));
      return TRUE;
    }
  }//if (!EFI_ERROR(Status))
  
  DEBUG((DEBUG_ERROR, "\n[SK] Locate IpmiTransport Protocol fail! \n"));
  return FALSE;
}
//<kuaibai-20180807-post time improvement in B4 when redfish disable->

/**
 This is the declaration of an EFI image entry point. This entry point is
 the same for UEFI Applications, UEFI OS Loaders, and UEFI Drivers including
 both device drivers and bus drivers.

 @param  ImageHandle           The firmware allocated handle for the UEFI image.
 @param  SystemTable           A pointer to the EFI System Table.

 @retval EFI_SUCCESS           The operation completed successfully.
 @retval Others                An unexpected error occurred.

 **/
EFI_STATUS
EFIAPI
RestIpmiDriverEntryPoint( IN EFI_HANDLE ImageHandle,
		IN EFI_SYSTEM_TABLE *SystemTable) 
{
	VOID *Registration;
	
	//<kuaibai-20180807-post time improvement in B4 when redfish disable+>
	EFI_STATUS            Status = EFI_SUCCESS;
  if(FALSE == BmcRedfishServiceEnable())
  {
    DEBUG ((DEBUG_ERROR, "\n[SK] Rest driver BMC Redfish Service is disabled!!\n"));
    return Status;
  }
  //<kuaibai-20180807-post time improvement in B4 when redfish disable->
  
	gBS->LocateProtocol(&gEfiHttpUtilitiesProtocolGuid, NULL,
			(VOID **) &gHttpUtilities);

	if (gHttpUtilities == NULL) {
		//
		// No Http utilities protocol, register a notify.
		//
		EfiCreateProtocolNotifyEvent(&gEfiHttpUtilitiesProtocolGuid,
				TPL_CALLBACK, HttpUtilitiesInstalledCallback,
				NULL, &Registration);
	}


	//
	// Install UEFI Driver Model protocol(s).
	//
	return gBS->InstallMultipleProtocolInterfaces(
			&gRestIpmiUsbDxeDriverBinding.DriverBindingHandle,
			&gEfiDriverBindingProtocolGuid,
			&gRestIpmiUsbDxeDriverBinding, NULL);
}

/**
 1. Checks for IPMI USB protocol in given handle matching registration value

 @param EFI_HANDLE   UsbIoHandle

 @retval EFI_SUCCESS BMC USB Interface is found.
 @retval EFI_NOT_FOUND BMC USB Interface is not found.

 **/

/**
 Tests to see if this driver supports a given controller. If a child device is provided, 
 it further tests to see if this driver supports creating a handle for the specified child device.


 @param[in]  This                 A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
 @param[in]  ControllerHandle     The handle of the controller to test. This handle 
 must support a protocol interface that supplies 
 an I/O abstraction to the driver.
 @param[in]  RemainingDevicePath  A pointer to the remaining portion of a device path.  This 
 parameter is ignored by device drivers, and is optional for bus 
 drivers. For bus drivers, if this parameter is not NULL, then 
 the bus driver must determine if the bus controller specified 
 by ControllerHandle and the child controller specified 
 by RemainingDevicePath are both supported by this 
 bus driver.

 @retval EFI_SUCCESS              The device specified by ControllerHandle and
 RemainingDevicePath is supported by the driver specified by This.
 @retval EFI_ALREADY_STARTED      The device specified by ControllerHandle and
 RemainingDevicePath is already being managed by the driver
 specified by This.
 @retval EFI_ACCESS_DENIED        The device specified by ControllerHandle and
 RemainingDevicePath is already being managed by a different
 driver or an application that requires exclusive access.
 Currently not implemented.
 @retval EFI_UNSUPPORTED          The device specified by ControllerHandle and
 RemainingDevicePath is not supported by the driver specified by This.
 **/
EFI_STATUS
EFIAPI
RestIpmiDriverBindingSupported( IN EFI_DRIVER_BINDING_PROTOCOL *This,
		IN EFI_HANDLE ControllerHandle,
		IN EFI_DEVICE_PATH_PROTOCOL *RemainingDevicePath OPTIONAL
		) {
	EFI_STATUS Status;
	IPMI_USB_TRANSPORT *IpmiUsb = NULL;

	
	//
	// Open the IPMI Protocol on behalf of ControllerHandle
	//
#if AMI_IPMI_EMULATOR_IS_AVAILABLE == 0
	Status = gBS->OpenProtocol(ControllerHandle, &gEfiDxeIpmiUsbTransportProtocolGuid,
			(VOID **) &IpmiUsb, This->DriverBindingHandle,
			ControllerHandle, EFI_OPEN_PROTOCOL_BY_DRIVER);
#else
	Status = gBS->OpenProtocol(ControllerHandle, &gDxeRestIpmiEmulatorGuid,
			(VOID **) &IpmiUsb, This->DriverBindingHandle,
			ControllerHandle, EFI_OPEN_PROTOCOL_BY_DRIVER);
#endif
	if (EFI_ERROR (Status)){
		return Status;
	}
	
	//
	// Close the IPMI Protocol that was opened on behalf of ControllerHandle
	//
#if AMI_IPMI_EMULATOR_IS_AVAILABLE == 0
	Status = gBS->CloseProtocol(ControllerHandle, &gEfiDxeIpmiUsbTransportProtocolGuid,
			This->DriverBindingHandle, ControllerHandle);
#else	
	Status = gBS->CloseProtocol(ControllerHandle, &gDxeRestIpmiEmulatorGuid,
			This->DriverBindingHandle, ControllerHandle);
#endif	
	return Status;
}


/**
 Starts a device controller or a bus controller.

 The Start() function is designed to be invoked from the EFI boot service ConnectController().
 As a result, much of the error checking on the parameters to Start() has been moved into this 
 common boot service. It is legal to call Start() from other locations, 
 but the following calling restrictions must be followed, or the system behavior will not be deterministic.
 1. ControllerHandle must be a valid EFI_HANDLE.
 2. If RemainingDevicePath is not NULL, then it must be a pointer to a naturally aligned
 EFI_DEVICE_PATH_PROTOCOL.
 3. Prior to calling Start(), the Supported() function for the driver specified by This must
 have been called with the same calling parameters, and Supported() must have returned EFI_SUCCESS.  

 @param[in]  This                 A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
 @param[in]  ControllerHandle     The handle of the controller to start. This handle 
 must support a protocol interface that supplies 
 an I/O abstraction to the driver.
 @param[in]  RemainingDevicePath  A pointer to the remaining portion of a device path.  This 
 parameter is ignored by device drivers, and is optional for bus 
 drivers. For a bus driver, if this parameter is NULL, then handles 
 for all the children of Controller are created by this driver.  
 If this parameter is not NULL and the first Device Path Node is 
 not the End of Device Path Node, then only the handle for the 
 child device specified by the first Device Path Node of 
 RemainingDevicePath is created by this driver.
 If the first Device Path Node of RemainingDevicePath is 
 the End of Device Path Node, no child handle is created by this
 driver.

 @retval EFI_SUCCESS              The device was started.
 @retval EFI_ALREADY_STARTED      This device is already running on ControllerHandle.
 @retval EFI_DEVICE_ERROR         The device could not be started due to a device error.Currently not implemented.
 @retval EFI_OUT_OF_RESOURCES     The request could not be completed due to a lack of resources.
 @retval Others                   The driver failed to start the device.

 **/
EFI_STATUS
EFIAPI
RestIpmiDriverBindingStart( IN EFI_DRIVER_BINDING_PROTOCOL *This,
		IN EFI_HANDLE ControllerHandle,
		IN EFI_DEVICE_PATH_PROTOCOL *RemainingDevicePath OPTIONAL
		) {

	EFI_STATUS Status;
	IPMI_USB_TRANSPORT *IpmiUsb = NULL;
	UINT32 MaxPacketSize = 0;

	
#if AMI_IPMI_EMULATOR_IS_AVAILABLE == 0
	Status = gBS->OpenProtocol(ControllerHandle, &gEfiDxeIpmiUsbTransportProtocolGuid,
			(VOID **) &IpmiUsb, This->DriverBindingHandle,
			ControllerHandle, EFI_OPEN_PROTOCOL_BY_DRIVER);
#else
	Status = gBS->OpenProtocol(ControllerHandle, &gDxeRestIpmiEmulatorGuid,
			(VOID **) &IpmiUsb, This->DriverBindingHandle,
			ControllerHandle, EFI_OPEN_PROTOCOL_BY_DRIVER);
#endif

	if (EFI_ERROR (Status)){
		SERVER_REST_DEBUG((EFI_D_ERROR, "REST: Start Failed with code - %d\n", Status));
		return Status;
	}

	//
	// Allocate and clear memory for REST Instance
	//
	gRestIpmiUsbInstance = AllocateRuntimeZeroPool(
			sizeof(REST_IPMI_USB_INSTANCE_DATA));
	if (gRestIpmiUsbInstance == NULL) {
		SERVER_REST_DEBUG((EFI_D_ERROR, "REST: Failed to allocate memory for gRestIpmiUsbInstance \n"));
#if AMI_IPMI_EMULATOR_IS_AVAILABLE == 0
		gBS->CloseProtocol(ControllerHandle, &gEfiDxeIpmiUsbTransportProtocolGuid,
				This->DriverBindingHandle, ControllerHandle);
#else
		gBS->CloseProtocol(ControllerHandle, &gDxeRestIpmiEmulatorGuid,
				This->DriverBindingHandle, ControllerHandle);
#endif
		return EFI_OUT_OF_RESOURCES;
	}

	//
	// This is the right way to get max size of package BMC can accept
	//
	Status = GetMaxRestIpmiPacketSize(IpmiUsb, &MaxPacketSize);
	if (EFI_ERROR (Status)) {
		SERVER_REST_DEBUG((EFI_D_ERROR, "REST: Failed to get MaxRestIpmiPacketSize\n"));
		gBS->FreePool(gRestIpmiUsbInstance);
		gRestIpmiUsbInstance = NULL;
		return Status;
	}
	if(MaxPacketSize == 0) MaxPacketSize = MAX_IPMI_PACKET_SIZE;
	SERVER_REST_DEBUG((EFI_D_INFO, "REST: MaxRestIpmiPacketSize = %d\n", MaxPacketSize));

	//
	// Fill the REST IPMI USB Instance private data
	//
	gRestIpmiUsbInstance->Signature = REST_IPMI_USB_SIGNATURE;

	//
	// Save pointers to PeiServices and gIpmiUsb PPI
	//
	gRestIpmiUsbInstance->IpmiUsbHandle = ControllerHandle;
	gRestIpmiUsbInstance->IpmiUsb = IpmiUsb;


	//
	// Fill the Rest IPMI USB Transport Protocol
	//
	gRestIpmiUsbInstance->RestIpmiUsbTransport.SendReceive =
			SendRecieveRestIpmiUsbCommand;
	gRestIpmiUsbInstance->RestIpmiUsbTransport.GetServiceTime =
			GetServiceTimeRestIpmiUsbCommand;
	gRestIpmiUsbInstance->MaxRestIpmiPacketSize = MaxPacketSize;

	//
	//  Install REST Ipmi Usb transport protocol on same handle as IPMI Usb protocol
	//
	Status = gBS->InstallProtocolInterface(
			&gRestIpmiUsbInstance->IpmiUsbHandle,
			&gEfiRestProtocolGuid,
			EFI_NATIVE_INTERFACE,
			&gRestIpmiUsbInstance->RestIpmiUsbTransport);

	if (EFI_ERROR (Status)) {
		SERVER_REST_DEBUG((EFI_D_ERROR, "REST Install RestIpmiUsbTransportProtocol Error - Status: %r\n", Status));
		//
		// Control comes here only when IPMI interface is found
		// But error happened in later stage. So free the allocated memory.
		//
#if AMI_IPMI_EMULATOR_IS_AVAILABLE == 0
		gBS->CloseProtocol(gRestIpmiUsbInstance->IpmiUsbHandle, &gEfiDxeIpmiUsbTransportProtocolGuid,
				This->DriverBindingHandle, gRestIpmiUsbInstance->IpmiUsbHandle);
#else
		gBS->CloseProtocol(gRestIpmiUsbInstance->IpmiUsbHandle, &gDxeRestIpmiEmulatorGuid,
				This->DriverBindingHandle, gRestIpmiUsbInstance->IpmiUsbHandle);
#endif
		gBS->FreePool(gRestIpmiUsbInstance);
		gRestIpmiUsbInstance = NULL;
	}

	return Status;

}

/**
 Stops a device controller or a bus controller.
 
 The Stop() function is designed to be invoked from the EFI boot service DisconnectController(). 
 As a result, much of the error checking on the parameters to Stop() has been moved 
 into this common boot service. It is legal to call Stop() from other locations, 
 but the following calling restrictions must be followed, or the system behavior will not be deterministic.
 1. ControllerHandle must be a valid EFI_HANDLE that was used on a previous call to this
 same driver's Start() function.
 2. The first NumberOfChildren handles of ChildHandleBuffer must all be a valid
 EFI_HANDLE. In addition, all of these handles must have been created in this driver's
 Start() function, and the Start() function must have called OpenProtocol() on
 ControllerHandle with an Attribute of EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER.
 
 @param[in]  This              A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
 @param[in]  ControllerHandle  A handle to the device being stopped. The handle must 
 support a bus specific I/O protocol for the driver 
 to use to stop the device.
 @param[in]  NumberOfChildren  The number of child device handles in ChildHandleBuffer.
 @param[in]  ChildHandleBuffer An array of child handles to be freed. May be NULL 
 if NumberOfChildren is 0.

 @retval EFI_SUCCESS           The device was stopped.
 @retval EFI_DEVICE_ERROR      The device could not be stopped due to a device error.

 **/
EFI_STATUS
EFIAPI
RestIpmiDriverBindingStop( IN EFI_DRIVER_BINDING_PROTOCOL *This,
		IN EFI_HANDLE ControllerHandle, IN UINTN NumberOfChildren,
		IN EFI_HANDLE *ChildHandleBuffer OPTIONAL
		) {

	EFI_STATUS		Status;
	IPMI_USB_TRANSPORT	*IpmiUsb = NULL;
	
	if(gRestIpmiUsbInstance != NULL){
	
		
#if AMI_IPMI_EMULATOR_IS_AVAILABLE == 0
		Status = gBS->OpenProtocol(ControllerHandle, &gEfiDxeIpmiUsbTransportProtocolGuid,
				(VOID **) &IpmiUsb, This->DriverBindingHandle,
				ControllerHandle, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
#else
		Status = gBS->OpenProtocol(ControllerHandle, &gDxeRestIpmiEmulatorGuid,
				(VOID **) &IpmiUsb, This->DriverBindingHandle,
				ControllerHandle, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
#endif
		if (EFI_ERROR (Status)) {
			return EFI_DEVICE_ERROR;
		}
	
		gBS->UninstallProtocolInterface(gRestIpmiUsbInstance->IpmiUsbHandle,
				&gEfiRestProtocolGuid,
				&gRestIpmiUsbInstance->RestIpmiUsbTransport);
	
#if AMI_IPMI_EMULATOR_IS_AVAILABLE == 0
		gBS->CloseProtocol(gRestIpmiUsbInstance->IpmiUsbHandle, &gEfiDxeIpmiUsbTransportProtocolGuid,
				This->DriverBindingHandle, gRestIpmiUsbInstance->IpmiUsbHandle);
#else
		gBS->CloseProtocol(gRestIpmiUsbInstance->IpmiUsbHandle, &gDxeRestIpmiEmulatorGuid,
				This->DriverBindingHandle, gRestIpmiUsbInstance->IpmiUsbHandle);
#endif
		gBS->FreePool(gRestIpmiUsbInstance);
		
		gRestIpmiUsbInstance = NULL;
	}

	return EFI_SUCCESS;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

