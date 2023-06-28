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


#include "RestBoot.h"
#include <Protocol/IpmiUsbTransportProtocol.h>

EFI_STATUS EFIAPI GetBootOptionsFromBMC(
		EFI_REST_PROTOCOL 	*RestInstance
);

/**
 The event process routine when the REST protocol is installed
 in the system.

 @param[in]     Event         Not used.
 @param[in]     Context       The pointer to the instance data.

 **/
VOID
EFIAPI
RestProtocolInstalledCallback( IN EFI_EVENT Event, IN VOID *Context) 
{
	EFI_STATUS 			Status = EFI_SUCCESS;
	BOOLEAN 			RestIpmiFound = FALSE;
	UINTN 				HandleCount = 0;
	UINTN 				iIndex = 0;
	EFI_HANDLE 			*Handles = NULL;
	EFI_REST_PROTOCOL 		*RestInstance = (EFI_REST_PROTOCOL *)NULL;
	void                		*DxeIpmiUsbInstance = NULL;
	
	DEBUG((EFI_D_INFO, "[SK] RestProtocolInstalledCallback;\n"));
	
	Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiRestProtocolGuid, NULL, &HandleCount, &Handles);
	DEBUG((EFI_D_INFO, "[SK] LocateHandleBuffer gEfiRestProtocolGuid = %r;\n",Status));
	
	if (!EFI_ERROR(Status))
	{
		for (iIndex = 0; iIndex < HandleCount; iIndex++ )
		{
			Status = gBS->HandleProtocol (Handles [iIndex], &gEfiRestProtocolGuid, &RestInstance);
			DEBUG((EFI_D_INFO, "[SK] HandleProtocol gEfiRestProtocolGuid = %r;\n",Status));
			if (!EFI_ERROR(Status))
			{
				//Spiral - if (!EFI_ERROR(gBS->HandleProtocol (Handles [iIndex], &gEfiDxeIpmiUsbTransportProtocolGuid, (void**)&DxeIpmiUsbInstance)))
			  Status = gBS->HandleProtocol (Handles [iIndex], &gEfiDxeIpmiUsbTransportProtocolGuid, (void**)&DxeIpmiUsbInstance);//Spiral
			  DEBUG((EFI_D_INFO, "[SK] HandleProtocol gEfiDxeIpmiUsbTransportProtocolGuid = %r;\n",Status)); //Spiral
			  if (!EFI_ERROR(Status)) //Spiral
				{
					RestIpmiFound = TRUE;
					DEBUG((EFI_D_INFO, "[SK] Callback GetBootOptionsFromBMC;\n")); //Spiral
					GetBootOptionsFromBMC(RestInstance);
					break;
				}
			}
		}
		FreePool (Handles);
	}
	DEBUG((EFI_D_INFO, "[SK] RestIpmiFound = %x;\n",RestIpmiFound)); //Spiral
	if(RestIpmiFound){
		gBS->CloseEvent(Event);
	}
}


EFI_HANDLE gRestBootImageHandle = NULL;

EFI_STATUS EFIAPI RestBootEntryPoint(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
	EFI_STATUS 			Status = EFI_NOT_FOUND;
	BOOLEAN 			RestIpmiFound = FALSE;
	UINTN 				HandleCount = 0;
	UINTN 				iIndex = 0;
	EFI_HANDLE 			*Handles = NULL;
	EFI_REST_PROTOCOL 		*RestInstance = (EFI_REST_PROTOCOL *)NULL;
	void                		*DxeIpmiUsbInstance = NULL;
	VOID 				*Registration;
	DEBUG((EFI_D_INFO, "[SK] RestBootEntryPoint;\n")); //Spiral
	gRestBootImageHandle = ImageHandle;
	Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiRestProtocolGuid, NULL, &HandleCount, &Handles);
	if (!EFI_ERROR(Status))
	{
		for (iIndex = 0; iIndex < HandleCount; iIndex++ )
		{
			Status = gBS->HandleProtocol (Handles [iIndex], &gEfiRestProtocolGuid, &RestInstance);
			if (!EFI_ERROR(Status))
			{
			    DEBUG((EFI_D_INFO, "[SK] RestInstance ok;\n")); //Spiral
				if (!EFI_ERROR(gBS->HandleProtocol (Handles [iIndex], &gEfiDxeIpmiUsbTransportProtocolGuid, (void**)&DxeIpmiUsbInstance)))
				{
					RestIpmiFound = TRUE;
					Status = GetBootOptionsFromBMC(RestInstance);
					DEBUG((EFI_D_INFO, "[SK] GetBootOptionsFromBMC = %r;\n",Status)); //Spiral
					break;
				}
			}
		}
		FreePool (Handles);
	}
	if(!RestIpmiFound){
		Status = EFI_SUCCESS;
		EfiCreateProtocolNotifyEvent(&gEfiRestProtocolGuid,
				TPL_CALLBACK, RestProtocolInstalledCallback,
				NULL, &Registration);
	}
	return Status;
}


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
