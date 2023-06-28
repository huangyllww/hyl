//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//*************************************************************************
/** @file PowerButton.c
    Provide functions to register and handle Powerbutton
	functionality.  This code is generic and as long as PM
	base address SDL token is defined properly this should
	work fine.

**/
//*************************************************************************


#include <PiSmm.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmPowerButtonDispatch2.h>

#include <Token.h>
#include <AmiCspLib.h>
#include <AmiChipsetIoLib.h>

EFI_SMM_POWER_BUTTON_REGISTER_CONTEXT	DispatchContext = {EfiPowerButtonExit};

/**
    If the power button is pressed, then this function is called.

  @param[in]     DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param[in]     Context         Points to an optional handler context which was specified when the
                                 handler was registered.
  @param[in,out] CommBuffer      A pointer to a collection of data in memory that will
                                 be conveyed from a non-SMM environment into an SMM environment.
  @param[in,out] CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS                         The interrupt was handled and quiesced. No other handlers
                                              should still be called.
  @retval EFI_WARN_INTERRUPT_SOURCE_QUIESCED  The interrupt has been quiesced but other handlers should
                                              still be called.
  @retval EFI_WARN_INTERRUPT_SOURCE_PENDING   The interrupt is still pending and other handlers should still
                                              be called.
  @retval EFI_INTERRUPT_PENDING               The interrupt could not be quiesced.
**/
EFI_STATUS
EFIAPI
PowerButtonActivated(
    IN EFI_HANDLE    DispatchHandle,
    IN CONST VOID    *Context OPTIONAL,
    IN OUT VOID	     *CommBuffer OPTIONAL,
    IN OUT UINTN     *CommBufferSize OPTIONAL)
{
	EFI_STATUS                				Status = EFI_SUCCESS;
	EFI_SMM_POWER_BUTTON_REGISTER_CONTEXT	*PbDispatchContext = (EFI_SMM_POWER_BUTTON_REGISTER_CONTEXT*)Context;

	if (PbDispatchContext->Phase == EfiPowerButtonExit) {
		// EIP172271
		// FCH_MISCx50_JTAG_CONTROL_ECO
        RW_MMIO32_MISC(0x50, BIT16, 0); // Clear BIT16
	} else if (PbDispatchContext->Phase == EfiPowerButtonEntry) {

	}

    return Status;
}

/**
	This is the entrypoint of the Power button driver.

    @param ImageHandle Image handle
    @param SystemTable Pointer to the system table

    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
EFI_STATUS
EFIAPI
PowerButtonEntry(
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
    )
{
    EFI_STATUS                				Status;
	EFI_SMM_POWER_BUTTON_DISPATCH2_PROTOCOL	*PowerButton = NULL;
    EFI_HANDLE                      		Handle = NULL;

    Status  = gSmst->SmmLocateProtocol (&gEfiSmmPowerButtonDispatch2ProtocolGuid, NULL, &PowerButton);
    ASSERT_EFI_ERROR(Status);
    if (!EFI_ERROR(Status)) {
        Status  = PowerButton->Register( PowerButton, \
                                         PowerButtonActivated, \
                                         &DispatchContext, \
                                         &Handle );
        ASSERT_EFI_ERROR(Status);
    }

    return Status;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
