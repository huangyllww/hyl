//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file UsbSb.c
    USB South Bridge Porting Hooks

**/

#include "Rt/AmiDef.h"
#include "Rt/UsbDef.h"
#include "Rt/AmiUsb.h"
#include "Rt/UsbKbd.h"

#if USB_ACPI_ENABLE_WORKAROUND
#include <Protocol/AcpiModeEnable.h>
#endif

#include <Library/AmiUsbSmmGlobalDataValidationLib.h>
#include <Library/DxeSmmUsbSbLib.h>
EFI_SMM_PERIODIC_TIMER_DISPATCH2_PROTOCOL *gPeriodicTimerDispatch = NULL;

EFI_HANDLE  gPeriodicTimerHandle = NULL;
EFI_HANDLE  gUsbIntTimerHandle = NULL;
BOOLEAN gInstallXhciHwSmiHandler = FALSE;
BOOLEAN gInstallUsbIntTimerHandler = FALSE;

extern  USB_GLOBAL_DATA     *gUsbData;
extern  EFI_GUID gEfiUsbPolicyProtocolGuid;
extern  BOOLEAN gLockSmiHandler;
extern  BOOLEAN gLockHwSmiHandler;

/**
    This function is registers periodic timer callbacks.

    @param 
        Pointer to the EFI System Table

    @retval 
        - EFI_SUCCESS if timers are initialized or function is not implemented
        - timer initialization error

    @note  
  If function is not implemented (timers are not needed for this chipset),
  function must return EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
USBSB_PeriodicTimerCallBack(
	EFI_HANDLE  DispatchHandle,
	CONST VOID  *Context,
	VOID    	*CommBuffer,
	UINTN   	*CommBufferSize
) 
{
	HC_STRUC*       HcStruc;
	UINT8		    i;
    DEV_INFO        *DevInfo = NULL;
    DEV_INFO        *Dev = gUsbData->aDevInfoTable;
    EFI_STATUS      Status;

    if ((USB_RUNTIME_DRIVER_IN_SMM == 2) && 
        (gUsbData->dUSBStateFlag & USB_FLAG_RUNNING_UNDER_EFI)) {
        return EFI_SUCCESS;
    }

    if (gLockSmiHandler == TRUE) {
        return EFI_SUCCESS;
    }

    Status = AmiUsbSmmGlobalDataValidation(gUsbData);

    ASSERT_EFI_ERROR(Status);
    
    if (EFI_ERROR(Status)) {
        gLockHwSmiHandler = TRUE;
        gLockSmiHandler = TRUE;
        return EFI_SUCCESS;
    }

    for (i = 0; i < gUsbData->HcTableCount; i++) {
	    HcStruc = gUsbData->HcTable[i];
        if (HcStruc == NULL) {
            continue;
        }
        if (HcStruc->HwSmiHandle != NULL) {
            continue;
        }
        if (HcStruc->dHCFlag & HC_STATE_RUNNING) {
	        (*gUsbData->aHCDriverTable[
				GET_HCD_INDEX(HcStruc->bHCType)].pfnHCDProcessInterrupt)(HcStruc);
	    }
	}

#if USB_HID_KEYREPEAT_USE_SETIDLE == 0

    if (gUsbData->fpKeyRepeatHCStruc != NULL) {
        return EFI_SUCCESS;
    }

    for (i = 0; i < USB_DEV_HID_COUNT; i++) {
        DevInfo = gUsbData->aUSBKBDeviceTable[i];
        if (DevInfo != NULL) {
            break;
        }
	}
    if (DevInfo == NULL) {
        for (i = 1; i < MAX_DEVICES;  ++i, ++Dev ) {
            if ( (Dev->Flag & DEV_INFO_VALID_STRUC) != 0 && 
                Dev->bDeviceType == BIOS_DEV_TYPE_HID &&
                (Dev->HidDevType & HID_DEV_TYPE_MOUSE) ) {
                DevInfo= Dev;    
                break; 
            }
        }
    }

    if (DevInfo != NULL) {
        USBKBDPeriodicInterruptHandler(gUsbData->HcTable[DevInfo->bHCNumber - 1]);
    }
    
#endif
    return EFI_SUCCESS;
}

/**
    This function registers XHCI hardware SMI callback function.
    
    @retval EFI_STATUS Status of the operation
    @note  
  Currently EHCI, UHCI and OHCI drivers install their SMI handlers in the
  corresponding Start functions. In the future all code related to SMI
  registration can be moved here.

**/

EFI_STATUS
EFIAPI
USBSB_InstallXhciHwSmiHandler()
{
	EFI_STATUS  Status = EFI_SUCCESS;
	EFI_HANDLE	Handle = NULL;

#if	XHCI_SUPPORT
#if XHCI_EVENT_SERVICE_MODE != 0
//GPI service
	EFI_SMM_GPI_DISPATCH2_PROTOCOL          *GpiDispatch = NULL;
	EFI_SMM_GPI_REGISTER_CONTEXT            Context;
	
	UINT32		HwSmiPinTable[] = {USB_XHCI_EXT_HW_SMI_PINS};
	UINT8		Index;

    if (gInstallXhciHwSmiHandler) {
        return Status;
    }

    gInstallXhciHwSmiHandler = TRUE;

	Status = pSmst->SmmLocateProtocol(&gEfiSmmGpiDispatch2ProtocolGuid, NULL, (VOID**)&GpiDispatch);
	ASSERT_EFI_ERROR(Status);	// driver dependencies?

	if (!EFI_ERROR(Status)) {
		for (Index = 0; Index < sizeof(HwSmiPinTable)/sizeof(UINT32); Index++) {
			if(HwSmiPinTable[Index] == 0xFFFFFFFF) continue;
                                        //(EIP61556)>
#if defined(GPI_DISPATCH_BY_BITMAP) && (GPI_DISPATCH_BY_BITMAP == 0)
            Context.GpiNum = HwSmiPinTable[Index];
#else
			Context.GpiNum = (UINTN)1 << HwSmiPinTable[Index];
#endif
                                        //<(EIP61556)
			GpiDispatch->Register(GpiDispatch, XhciHwSmiHandler, &Context, &Handle);
		}
	}
#endif

#if XHCI_EVENT_SERVICE_MODE != 1
	Status = USBSB_InstallUsbIntTimerHandler();
#endif
#endif

	return Status;
}

/**
    This function registers periodic timer callback function.
    @retval EFI_STATUS Status of the operation
**/

EFI_STATUS
EFIAPI
USBSB_InstallUsbIntTimerHandler()
{
	EFI_STATUS  Status;
	EFI_SMM_PERIODIC_TIMER_REGISTER_CONTEXT  TimerContext;
    UINT64      *SmiTickInterval;

	if (gInstallUsbIntTimerHandler) {
		return EFI_SUCCESS;
	}

	gInstallUsbIntTimerHandler = TRUE;

	Status = pSmst->SmmLocateProtocol (
			&gEfiSmmPeriodicTimerDispatch2ProtocolGuid, 
			NULL, 
			(VOID**)&gPeriodicTimerDispatch);
	ASSERT_EFI_ERROR(Status);	// driver dependencies?

	if (!EFI_ERROR(Status)) {
		TimerContext.Period = 160000;	//16Ms 
		TimerContext.SmiTickInterval = 160000; 
        SmiTickInterval = NULL;
        //Check SmiTickInterval that are supported by the chipset.
		do {
            Status = gPeriodicTimerDispatch->GetNextShorterInterval(
                                           gPeriodicTimerDispatch,
                                           &SmiTickInterval
                                           );
            if (EFI_ERROR(Status)) {
                break;
            }
            if (SmiTickInterval != NULL) {
                if (*SmiTickInterval <= TimerContext.SmiTickInterval) {
                    TimerContext.SmiTickInterval = *SmiTickInterval; 
                    break;
                }
            }
        } while (SmiTickInterval != NULL);
			 
		Status = gPeriodicTimerDispatch->Register(
						gPeriodicTimerDispatch, 
						USBSB_PeriodicTimerCallBack, 
						&TimerContext,
						&gUsbIntTimerHandle);
		ASSERT_EFI_ERROR(Status);
	}

	return Status;
}

/**
    This function unregisters all the periodic timer handles.
    @retval EFI_STATUS Status of the operation
**/

EFI_STATUS
EFIAPI
USBSB_UninstallTimerHandlers()
{
	EFI_STATUS  Status = EFI_SUCCESS;

	if (gPeriodicTimerDispatch == NULL) {
		return Status;
	}

	if (gUsbIntTimerHandle) {
		Status = gPeriodicTimerDispatch->UnRegister (
						gPeriodicTimerDispatch, 
						gUsbIntTimerHandle);
		ASSERT_EFI_ERROR(Status);

		gUsbIntTimerHandle = NULL;
	}

	if (gPeriodicTimerHandle) {
		Status = gPeriodicTimerDispatch->UnRegister (
						gPeriodicTimerDispatch, 
						gPeriodicTimerHandle);
		ASSERT_EFI_ERROR(Status);

		gPeriodicTimerHandle = NULL;
	}

	return Status;
}

#if USB_ACPI_ENABLE_WORKAROUND
/**
    This is ACPI mode enable callback function. It is a workaround for non 
    XHCI/EHCI aware OSes.

**/

VOID
EFIAPI
AcpiEnableCallBack(
	IN EFI_HANDLE   DispatchHandle
)
{
	USB_StopUnsupportedHc();
    gUsbData->dUSBStateFlag |= USB_FLAG_RUNNING_UNDER_OS;
    gLockSmiHandler = TRUE;
    gUsbData->KbShiftKeyStatusUnderOs = gUsbData->bUSBKBShiftKeyStatus;
}

/**
    This function registers ACPI enable callback function.

**/

EFI_STATUS
EFIAPI
RegisterAcpiEnableCallBack(
    IN EFI_ACPI_DISPATCH_PROTOCOL   *This,
    IN EFI_ACPI_DISPATCH            Function,
    OUT EFI_HANDLE                  *Handle
)
{
	EFI_STATUS                      Status;
	EFI_HANDLE                      RegisterHandle;
	EFI_ACPI_DISPATCH_PROTOCOL      *AcpiEnDispatch;

	Status = pSmst->SmmLocateProtocol(&gEfiAcpiEnDispatchProtocolGuid, NULL, (VOID**)&AcpiEnDispatch);
	if (EFI_ERROR(Status)) {
        return Status;
    }
    RegisterHandle = (EFI_HANDLE)USB_HANDLE;
	Status = AcpiEnDispatch->Register(AcpiEnDispatch, AcpiEnableCallBack, &RegisterHandle);
	ASSERT_EFI_ERROR(Status);

	return Status;
}
#endif

/**
    This function is called from USBRT entry point inside SMM. Any SMI handlers
    registration related to USB driver can be done here.
    @retval EFI_STATUS Status of the operation
**/

EFI_STATUS
EFIAPI
USBSB_InstallSmiEventHandlers()
{
    EFI_STATUS  Status;
	EFI_SMM_USB_REGISTER_CONTEXT                    UsbContext;
    EFI_SMM_USB_DISPATCH2_PROTOCOL                  *UsbDispatch;
    EFI_SMM_HANDLER_ENTRY_POINT2	                UsbCallback;
    EFI_HANDLE                                      Handle = NULL;
    EFI_USB_POLICY_PROTOCOL                         *EfiUsbPolicyProtocol;	
    UINT8		Index;
    AMI_USB_HW_SMI_HC_CONTEXT                       *HcContext;
    
#if EXTERNAL_USB_CONTROLLER_SUPPORT
#if USB_REGISTER_PERIODIC_TIMER_IN_DXE
    USBSB_InstallUsbIntTimerHandler();
#endif
#endif

    Status = pSmst->SmmLocateProtocol(
            &gEfiSmmUsbDispatch2ProtocolGuid,
            NULL,
            (VOID**)&UsbDispatch);

    if (!EFI_ERROR(Status)) {
    
        Status = gBS->LocateProtocol(
                    &gEfiUsbPolicyProtocolGuid,
                    NULL,
                    (VOID**)&EfiUsbPolicyProtocol);

        if (!EFI_ERROR(Status)) {
            gUsbData->RegisterUsbSmiBeforeEndOfDxe = EfiUsbPolicyProtocol->RegisterUsbSmiBeforeEndOfDxe;
            for (Index = 0; Index < EfiUsbPolicyProtocol->AmiUsbHwSmiHcTable.HcCount; Index++) {

                HcContext = EfiUsbPolicyProtocol->AmiUsbHwSmiHcTable.HcContext[Index];
           
            	switch (HcContext->Type) {
#if UHCI_SUPPORT
            		case USB_HC_UHCI:
            			UsbCallback = UhciHWSMIHandler;
            			break;
#endif
#if OHCI_SUPPORT
            		case USB_HC_OHCI:
            			UsbCallback = OhciHWSMIHandler;
            			break;
#endif
#if EHCI_SUPPORT
            		case USB_HC_EHCI:
            			UsbCallback = EhciHWSMIHandler;
            			break;
#endif
#if XHCI_SUPPORT
            		case USB_HC_XHCI:
            			UsbCallback = XhciHwSmiHandler;
            			break;
#endif
            		default:
            			UsbCallback = NULL;
                        break;
            	}

                if (UsbCallback == NULL) {
                    continue;
                }

                UsbContext.Type = UsbLegacy;
                UsbContext.Device = HcContext->Device;

                Status = UsbDispatch->Register(
                            UsbDispatch,
                            UsbCallback,
                            &UsbContext,
                            &Handle);

                USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "AMIUSB HC type %x HW SMI registation status:: %r\n", 
                            HcContext->Type, Status);
                if (!EFI_ERROR(Status)) {
                    HcContext->HwSmiHandle = Handle;
                }
            }

        }
    }
#if XHCI_SUPPORT
#if XHCI_EVENT_SERVICE_MODE != 0
    {
        //GPI service
    	EFI_SMM_GPI_DISPATCH2_PROTOCOL          *GpiDispatch = NULL;
    	EFI_SMM_GPI_REGISTER_CONTEXT            Context;
    	UINT32		HwSmiPinTable[] = {USB_XHCI_EXT_HW_SMI_PINS};

    	Status = pSmst->SmmLocateProtocol(&gEfiSmmGpiDispatch2ProtocolGuid, NULL, (VOID**)&GpiDispatch);
    	ASSERT_EFI_ERROR(Status);	// driver dependencies?

        if (!EFI_ERROR(Status)) {
            for (Index = 0; Index < sizeof(HwSmiPinTable) / sizeof(UINT32); Index++) {
                if (HwSmiPinTable[Index] == 0xFFFFFFFF) {
                    continue;
                }
                                        //(EIP61556)>
#if defined(GPI_DISPATCH_BY_BITMAP) && (GPI_DISPATCH_BY_BITMAP == 0)
                Context.GpiNum = HwSmiPinTable[Index];
#else
			    Context.GpiNum = (UINTN)1 << HwSmiPinTable[Index];
#endif
                                            //<(EIP61556)
    			GpiDispatch->Register(GpiDispatch, XhciHwSmiHandler, &Context, &Handle);
    		}
    	}
    }
#endif
#endif
#if USB_ACPI_ENABLE_WORKAROUND
	{
		EFI_ACPI_DISPATCH_PROTOCOL	*AcpiEnDispatch;
		VOID		                    *Reg;
	
		Status = pSmst->SmmLocateProtocol(&gEfiAcpiEnDispatchProtocolGuid, NULL, (VOID**)&AcpiEnDispatch);
		if (!EFI_ERROR(Status)) {
			RegisterAcpiEnableCallBack(NULL, NULL, NULL);
		} else {
			Status = pSmst->SmmRegisterProtocolNotify(
							&gEfiAcpiEnDispatchProtocolGuid,
							RegisterAcpiEnableCallBack,
							&Reg);
			ASSERT_EFI_ERROR(Status);
		}
	}
#endif
    return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
