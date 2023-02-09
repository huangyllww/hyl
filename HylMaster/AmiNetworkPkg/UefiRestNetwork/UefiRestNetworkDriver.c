//*****************************************************************
//*****************************************************************
//**                                                             **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.    **
//**                                                             **
//**                       All Rights Reserved.                  **
//**                                                             **
//**    5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093 **
//**                                                             **
//**                       Phone (770)-246-8600                  **
//**                                                             **
//*****************************************************************
//*****************************************************************

/** @file UefiRestNetworkDriver.c
    Installs the Uefi Rest Driver protocol

**/

//---------------------------------------------------------------------------

#include "Library/MemoryAllocationLib.h"
#include "UefiRestNetworkDriver.h"
#include "UefiRestNetworkProto.h"
#include "UefiRestNetworkSupport.h"
#include <Include/AmiDxeLib.h>
#include <Library/DebugLib.h>
#include "Library/NetLib.h"
#include "Token.h"

//---------------------------------------------------------------------------
EFI_REST_PROTOCOL gEfiRestProtocol = {EfiRestSendReceive, EfiRestGetServiceTime};

///
/// Driver Binding Protocol instance
///
EFI_DRIVER_BINDING_PROTOCOL gUefiRestNetworkDriverBinding = {
  UefiRestNetworkDriverBindingSupported,
  UefiRestNetworkDriverBindingStart,
  UefiRestNetworkDriverBindingStop,
  UEFI_REST_DRIVER_VERSION,
  NULL,
  NULL
};

/**
 This function checks if the controllerHandle supports the Udp4Protocol.

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

  @retval EFI_SUCCESS              if the ControllerHandle supports the specified protocol.
  @retval Others                   The ControllerHandle doesn't supports the specified protocol.
**/
EFI_STATUS
EFIAPI
UefiRestNetworkDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath OPTIONAL
  )
{
    EFI_STATUS 	Status = EFI_SUCCESS;

    //
    // Open the HTTP Protocol on behalf of ControllerHandle
    //
    Status = gBS->OpenProtocol(
            ControllerHandle, 
            &gEfiHttpServiceBindingProtocolGuid,
            NULL, 
            This->DriverBindingHandle,
            ControllerHandle, 
            EFI_OPEN_PROTOCOL_TEST_PROTOCOL);
    if (EFI_ERROR (Status)){
        return Status;
    }
    
    return Status;
}

/**
  This function checks if the Rest Protocol is installed on the Handle and installs the protocol if not available. 

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

  @retval EFI_SUCCESS              The protocol was installed.
  @retval EFI_ALREADY_STARTED      The protocol is already installed.
  @retval Others                   The protocol install failed.

**/
EFI_STATUS
EFIAPI
UefiRestNetworkDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath OPTIONAL
  )
{
    EFI_STATUS              Status  = EFI_SUCCESS;
    EFI_REST_PROTOCOL *RestProtocol = (EFI_REST_PROTOCOL *)NULL;
    EFI_IP4_CONFIG2_PROTOCOL *Ip4Config;
    REST_IP_NOTIFY_CONTEXT *NotifyContext;
    
    //Check if the Rest protocol is available
    Status = pBS->HandleProtocol (ControllerHandle, &gEfiRestProtocolGuid, &RestProtocol);
    DEBUG((DEBUG_ERROR, "UefiRestNetworkDriverBindingStart: HandleProtocol RestProtocol: %r\n", Status));
    if( !EFI_ERROR(Status) )
    	return EFI_ALREADY_STARTED;

    if (TRUE == PcdGetBool (PcdInstallRestOnlyOnIPAvailable))
    {
        DEBUG((DEBUG_ERROR, "UefiRestNetworkDriverBindingStart: ControllerHandle: %x\n", ControllerHandle));
        Status = IsIPConfiguredForHandle(ControllerHandle);
        DEBUG((DEBUG_ERROR, "\nUefiRestNetworkDriverBindingStart(),  IsIPConfigured1() status = %r", Status));
    }
    else
    {
        Status = EFI_SUCCESS;
    }
    if (!EFI_ERROR (Status))
    {
        Status = InstallRestHttpProtocol(This->ImageHandle, ControllerHandle);
        DEBUG((DEBUG_ERROR, "UefiRestNetworkDriverBindingStart: InstallRestHttpProtocol: %r\n", Status));
    }
    else
    {
        if(Status != EFI_NO_MEDIA)
        {
            Status = pBS->HandleProtocol(ControllerHandle, &gEfiIp4Config2ProtocolGuid, (VOID*) &Ip4Config);
            DEBUG((DEBUG_ERROR, "UefiRestNetworkDriverBindingStart: HandleProtocol Ip4Config: %r\n", Status));
            if (EFI_ERROR (Status))
            {
                return Status;
            }
            NotifyContext = AllocateZeroPool (sizeof (REST_IP_NOTIFY_CONTEXT));
            if (NotifyContext == NULL)
            {
                return EFI_OUT_OF_RESOURCES;
            }
            NotifyContext->ControllerHandle = ControllerHandle;
            NotifyContext->ImageHandle = This->ImageHandle;
            Status = pBS->CreateEvent( EVT_NOTIFY_SIGNAL,
                                   TPL_NOTIFY,
                                   IpConfigNotify,
                                   (VOID *)NotifyContext,
                                   &NotifyContext->RestNotifyEvent );
            DEBUG((DEBUG_ERROR, "UefiRestNetworkDriverBindingStart: CreateEvent Status : %r\n", Status));
            Status = Ip4Config->RegisterDataNotify( Ip4Config,
                                                Ip4Config2DataTypePolicy,
                                                NotifyContext->RestNotifyEvent );
            DEBUG((DEBUG_ERROR, "UefiRestNetworkDriverBindingStart: RegisterDataNotify Status : %r\n", Status));
        }
    }
    
    return Status;
}

/**
   This function uninstalls the protocol for the Handle.
  
  @param[in]  This              A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param[in]  ControllerHandle  A handle to the device being stopped. The handle must 
                                support a bus specific I/O protocol for the driver 
                                to use to stop the device.
  @param[in]  NumberOfChildren  The number of child device handles in ChildHandleBuffer.
  @param[in]  ChildHandleBuffer An array of child handles to be freed. May be NULL 
                                if NumberOfChildren is 0.

  @retval EFI_SUCCESS           The protocol was uninstalled.
  @retval Others		        The protocol was failed to uninstall.

**/
EFI_STATUS
EFIAPI
UefiRestNetworkDriverBindingStop (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN UINTN                        NumberOfChildren,
  IN EFI_HANDLE                   *ChildHandleBuffer OPTIONAL
  )
{
    EFI_STATUS                      Status  = EFI_SUCCESS;
    UEFI_REST_NETWORK_INSTANCE_DATA *RestHttpInstance = NULL;
    
    //
    //  Determine if this driver is already attached
    //
    Status = pBS->OpenProtocol (
                      ControllerHandle,
                      &gEfiCallerIdGuid,
                      (VOID **) &RestHttpInstance,
                      This->DriverBindingHandle,
                      ControllerHandle,
                      EFI_OPEN_PROTOCOL_GET_PROTOCOL
                      );
    if(!EFI_ERROR ( Status ) && RestHttpInstance != NULL){
        DEBUG((DEBUG_INFO, "REST_HTTP: Stop OpenProtocol found\n"));
        gBS->UninstallMultipleProtocolInterfaces(ControllerHandle,
                &gEfiRestProtocolGuid, &RestHttpInstance->EfiRestProtocol,
                &gEfiCallerIdGuid, RestHttpInstance, NULL);
        if(!EFI_ERROR ( Status )){
            DEBUG((DEBUG_INFO, "REST_HTTP: Stop UninstallProtocolInterface done\n"));
            //HttpIoDestroyIo(&RestHttpInstance->HttpIo);
            FreePool(RestHttpInstance);
            RestHttpInstance = NULL;
        }
    }

    DEBUG((DEBUG_INFO, "REST_HTTP: Stop - %r\n", Status));
    
    return Status;
}

/**
    Procedure    :  UefiRestNetworkEntryPoint

    Description  :  Entry point for UefiRestNetwork module.

    Input        :  EFI_HANDLE ImageHandle - Image handle for this driver image
    EFI_SYSTEM_TABLE *SystemTable - pointer to the EFI system table

    Output       :  EFI_STATUS - Status of the operation
**/
EFI_STATUS UefiRestNetworkEntryPoint (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
)
{
    EFI_STATUS 	Status = EFI_SUCCESS;
    
    InitAmiLib (ImageHandle, SystemTable);

    //
    // Initialize the EFI Driver Library.
    //
    Status =  EfiLibInstallDriverBindingComponentName2 (
                ImageHandle,
                SystemTable,
                &gUefiRestNetworkDriverBinding,
                ImageHandle,
                &gUefiRestNetworkComponentName,
                &gUefiRestNetworkComponentName2
                );
    DEBUG((DEBUG_ERROR, "\nRestDriver: InstallDriver Status:%x",Status));
    
    return Status;
}

EFI_STATUS InstallRestHttpProtocol(
        IN EFI_HANDLE           ImageHandle,
        IN EFI_HANDLE           ControllerHandle
        )
{
    EFI_STATUS  Status = EFI_NOT_FOUND;
//    HTTP_IO_CONFIG_DATA ConfigData;
    UEFI_REST_NETWORK_INSTANCE_DATA *RestHttpInstance = NULL;

    //
    // Allocate and clear memory for REST Instance
    //
    RestHttpInstance = AllocateZeroPool(sizeof(UEFI_REST_NETWORK_INSTANCE_DATA));
    if (RestHttpInstance == NULL) {
        DEBUG((DEBUG_INFO, "REST_HTTP: Failed to allocate memory for RestHttpInstance \n"));
        return EFI_OUT_OF_RESOURCES;
    }

    RestHttpInstance->Signature = UEFI_REST_PRIVATE_DATA_SIGNATURE;
    RestHttpInstance->ImageHandle = ImageHandle;
    RestHttpInstance->ControllerHandle = ControllerHandle;
    
/*    ZeroMem (&ConfigData, sizeof (HTTP_IO_CONFIG_DATA));
    ConfigData.Config4.HttpVersion    = HttpVersion11;
    ConfigData.Config4.RequestTimeOut = 0x2710;
    ConfigData.Config4.UseDefaultAddress = TRUE;
    
    Status = HttpIoCreateIo (
            ImageHandle,
            ControllerHandle,
            IP_VERSION_4,
            &ConfigData,
            &RestHttpInstance->HttpIo
            );
    DEBUG((DEBUG_ERROR, "REST_HTTP: Install HttpIoCreateIo - Status: %r\n", Status));
    if (EFI_ERROR (Status)) {
        FreePool(RestHttpInstance);
        RestHttpInstance = NULL;
        return Status;
    }*/

    //
    // Fill the Rest HTTP Transport Protocol
    //
    RestHttpInstance->EfiRestProtocol.SendReceive = EfiRestSendReceive;
    RestHttpInstance->EfiRestProtocol.GetServiceTime = EfiRestGetServiceTime;

    //
    //  Install REST HTTP transport protocol on same handle as HTTP protocol
    //
    //
    Status = gBS->InstallMultipleProtocolInterfaces(
            &ControllerHandle, &gEfiCallerIdGuid, RestHttpInstance,
            &gEfiRestProtocolGuid,
            &RestHttpInstance->EfiRestProtocol, NULL);

    DEBUG((DEBUG_ERROR, "REST_HTTP: Install RestHttpTransportProtocol - Status: %r\n", Status));

    if (EFI_ERROR (Status)) {
        //
        // Control comes here only when HTTP interface is found
        // But error happened in later stage. So free the allocated memory.
        //
        //HttpIoDestroyIo(&RestHttpInstance->HttpIo);
        FreePool(RestHttpInstance);
        RestHttpInstance = NULL;
    }
    return Status;
}

void IpConfigNotify (EFI_EVENT Event, VOID *Context)
{
    EFI_STATUS  Status;
    EFI_HANDLE    ControllerHandle;
    EFI_TPL     OldTpl = TPL_APPLICATION;
    REST_IP_NOTIFY_CONTEXT *NotifyContext;
    
    OldTpl = pBS->RaiseTPL (TPL_HIGH_LEVEL);    // guarantees that RestoreTPL won't ASSERT
    pBS->RestoreTPL (TPL_APPLICATION);

    NotifyContext = (REST_IP_NOTIFY_CONTEXT *)Context;
    ControllerHandle = NotifyContext->ControllerHandle;
    DEBUG((DEBUG_ERROR, "IpConfigNotify: ControllerHandle: %x\n", ControllerHandle));
    Status = IsIPConfiguredForHandle(ControllerHandle);
    DEBUG((DEBUG_ERROR, "IpConfigNotify: IsIPConfiguredForHandle: %r\n", Status));
    if (!EFI_ERROR (Status) && Event)
    {
        NotifyContext = (REST_IP_NOTIFY_CONTEXT *)Context;
        Status = InstallRestHttpProtocol(NotifyContext->ImageHandle, NotifyContext->ControllerHandle);
        DEBUG((DEBUG_ERROR, "IpConfigNotify: InstallRestHttpProtocol: %r\n", Status));
        pBS->CloseEvent(Event);
    }

    pBS->RaiseTPL (TPL_HIGH_LEVEL);             // guarantees that RestoreTPL won't ASSERT
    pBS->RestoreTPL (OldTpl);
}

EFI_STATUS IsIPConfiguredForHandle(IN EFI_HANDLE ControllerHandle)
{
    EFI_STATUS  Status = EFI_NOT_FOUND;
    UINTN       Size = 1;
    UINTN       Index = 0;
    EFI_HANDLE  Handle;
    EFI_SIMPLE_NETWORK_PROTOCOL *SimpleNet;
#if (MDE_PKG_VERSION >= 0xa)
    EFI_IP4_CONFIG2_PROTOCOL *Ip4Config;
    EFI_IP4_CONFIG2_INTERFACE_INFO *Ip4ConfigData = (EFI_IP4_CONFIG2_INTERFACE_INFO *)NULL;
#else
    EFI_IP4_CONFIG_PROTOCOL *Ip4Config;
    EFI_IP4_IPCONFIG_DATA *Ip4ConfigData;
#endif

    Handle = ControllerHandle;
    
    Status = pBS->HandleProtocol(
            Handle,
            &gEfiSimpleNetworkProtocolGuid,
            (VOID*) &SimpleNet);
    
    if (EFI_ERROR(Status))
    {
        return Status;
    }
    
#if (MDE_PKG_VERSION >= 0xa)
    Status = gBS->HandleProtocol(Handle, &gEfiIp4Config2ProtocolGuid, (VOID*) &Ip4Config);
#else
    Status = gBS->HandleProtocol(Handle, &gIp4ConfigProtocolGuid, (VOID*) &Ip4Config);
#endif
    
    if (!EFI_ERROR(Status))
    {
        DEBUG((DEBUG_ERROR, "RestNetwork: MediaPresent: %d, MediaPresentSupported: %d\n", SimpleNet->Mode->MediaPresent, SimpleNet->Mode->MediaPresentSupported));
        if (SimpleNet->Mode->MediaPresent && SimpleNet->Mode->MediaPresentSupported)
        {
            Ip4ConfigData = AllocateZeroPool(Size);
#if (MDE_PKG_VERSION >= 0xa)
            Status = Ip4Config->GetData(Ip4Config, Ip4Config2DataTypeInterfaceInfo, &Size, Ip4ConfigData);

            if (Status == EFI_BUFFER_TOO_SMALL)
            {
                FreePool(Ip4ConfigData);
                Ip4ConfigData = AllocateZeroPool(Size);

                Status = Ip4Config->GetData(Ip4Config, Ip4Config2DataTypeInterfaceInfo, &Size, Ip4ConfigData);
            }
#else
            Status = Ip4Config->GetData(Ip4Config, &Size, Ip4ConfigData);

            if (Status == EFI_BUFFER_TOO_SMALL)
            {
                FreePool(Ip4ConfigData);
                Ip4ConfigData = AllocateZeroPool(Size);

                Status = Ip4Config->GetData(Ip4Config, &Size, Ip4ConfigData);
            }
#endif
            if (!EFI_ERROR (Status))
            {
                CHAR8 SystemIP [20];
                
                Sprintf (SystemIP, "%d.%d.%d.%d",
                        Ip4ConfigData->StationAddress.Addr[0],
                        Ip4ConfigData->StationAddress.Addr[1],
                        Ip4ConfigData->StationAddress.Addr[2],
                        Ip4ConfigData->StationAddress.Addr[3]);
                DEBUG((DEBUG_ERROR, "RestNetwork: Configured SystemIP: %a\n", SystemIP));
                if(Strcmp(SystemIP,"0.0.0.0"))
                {
                    //If IP is not equal to 0.0.0.0, then IP is configured for atleast one port.
                    Status = EFI_SUCCESS;
                }
                else
                {
                    Status = EFI_NOT_FOUND;
                }
            }
            if (Ip4ConfigData)
            {
                FreePool(Ip4ConfigData);
                Ip4ConfigData = NULL;
            }
        }
        else
        {
            Status = EFI_NO_MEDIA;
        }
    }
    return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
