//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//*************************************************************************
/** @file SmmChildDispatchMain.c
    This file contains implementation of module entry point,
    generic RegisterHandler and UnregisterHandler routines
    and main dispatcher loop.

**/
//*************************************************************************

#include <PiSmm.h>
#include <Library/SmmServicesTableLib.h>

#include <AmiDxeLib.h>
#include <token.h>
//---#include <Smm.h>
#include "SmmChildDispatch.h"
#include "SmmChildDispatchProtocol.h"
#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x0001000A)
#include <Protocol\SmmBase2.h>
#include <Protocol\SmmCpu.h>
#endif

//---------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//---------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)

EFI_STATUS
SwSmiHook (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  );

//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
// Variable Declaration(s)

/*
#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x0001000A)
EFI_SMM_BASE2_PROTOCOL  *gSmmBase2 = NULL;
EFI_SMM_SYSTEM_TABLE2   *gSmst2 = NULL;
EFI_SMM_CPU_PROTOCOL    *gEfiSmmCpuProtocol;
#endif

FCH_SMM_SW_DISPATCH2_PROTOCOL            *gAmdSbSmmSwDispatchProtocol = NULL;
FCH_SMM_SX_DISPATCH2_PROTOCOL            *gAmdSbSmmSxDispatchProtocol = NULL;
FCH_SMM_USB_DISPATCH2_PROTOCOL           *gAmdSbSmmUsbDispatchProtocol = NULL;
FCH_SMM_GPI_DISPATCH2_PROTOCOL           *gAmdSbSmmGpeDispatchProtocol = NULL;
FCH_SMM_PWRBTN_DISPATCH2_PROTOCOL        *gAmdSbSmmPwrBtnDispatchProtocol = NULL;
FCH_SMM_IO_TRAP_DISPATCH2_PROTOCOL       *gAmdSbSmmIoTrapDispatchProtocol = NULL;
FCH_SMM_PERIODICAL_DISPATCH2_PROTOCOL    *gAmdSbSmmPeriodicalDispatchProtocol = NULL;
struct {
	EFI_GUID	*Guid;
	VOID		*Protocol;
} AmdSmmProtocolTbl[] = {
		{&gFchSmmSwDispatch2ProtocolGuid, &gAmdSbSmmSwDispatchProtocol},
		//##{&gFchSmmSwDispatch2ProtocolGuid, &gAmdSbSmmSwDispatchProtocol},
		//##{&gFchSmmSxDispatch2ProtocolGuid, &gAmdSbSmmSxDispatchProtocol},
		//##{&gFchSmmPeriodicalDispatch2ProtocolGuid, &gAmdSbSmmPeriodicalDispatchProtocol},
		//##{&gFchSmmUsbDispatch2ProtocolGuid, &gAmdSbSmmUsbDispatchProtocol},
		//##{&gFchSmmGpiDispatch2ProtocolGuid, &gAmdSbSmmGpeDispatchProtocol},
		//##{&gFchSmmPwrBtnDispatch2ProtocolGuid, &gAmdSbSmmPwrBtnDispatchProtocol},
		//##{&gFchSmmIoTrapDispatch2ProtocolGuid, &gAmdSbSmmIoTrapDispatchProtocol},
		{NULL, NULL},
};
*/
//###<AAV> provide later ###EFI_GUID gEfiSmmIoTrapDispatch2ProtocolGuid = EFI_SMM_IO_TRAP_DISPATCH2_PROTOCOL_GUID;

// External Declaration(s)

// Function Definition(s)

/**
    This function implements main SMI dispatcher loop

    @param SmmImageHandle SMM image handle
    @param CommunicationBuffer pointer to optional communication buffer
    @param SourceSize pointer to size of communication buffer

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
EFI_STATUS
ChildDispatcher (
  IN EFI_HANDLE           DispatchHandle,
  IN CONST VOID           *DispatchContext OPTIONAL,
  IN OUT VOID             *CommBuffer OPTIONAL,
  IN OUT UINTN            *CommBufferSize OPTIONAL
  )
{
/*    UINT32 Index;
    BOOLEAN HandledSmi;

    do
    {
        HandledSmi = FALSE;
        for(Index = EfiSmmSwSmi; Index < EfiSmmMaxSmi; Index++)
        {
            if(SmmHandler[Index].HandlerProtocol.GetContext != NULL &&
               SmmHandler[Index].HandlerProtocol.GetContext())
            {
                SmmHandler[Index].HandlerProtocol.DispatchSmi();
                HandledSmi = TRUE;
            }
        }
    } while (HandledSmi);

    ClearAllSmi();
*/
    return EFI_HANDLER_SUCCESS;
}

/**
    This function initializes Child dispatcher in SMM mode

    @param ImageHandle Image handle
    @param SystemTable pointer to system table

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
EFI_STATUS
InSmmFunction(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS		Status = EFI_SUCCESS;
/*
    for (i = 0; AmdSmmProtocolTbl[i].Guid != NULL; i++) {
		Status = pBS->LocateProtocol (
					AmdSmmProtocolTbl[i].Guid,
					NULL,
					AmdSmmProtocolTbl[i].Protocol
					);
		if (EFI_ERROR (Status)) return Status;
    }
*/
    return Status;
}

/**
    SMM Child Dispatcher module entry point

    @param ImageHandle Image handle
    @param SystemTable pointer to system table

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
EFI_STATUS
SmmChildDispatch2EntryPoint (
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
	EFI_STATUS		Status = EFI_SUCCESS;
	EFI_HANDLE		Handle;
	VOID			*Registration;

	//--gSmst->SmiManage();
	Status = gSmst->SmiHandlerRegister(ChildDispatcher, NULL, &Handle);

	Status = gSmst->SmmRegisterProtocolNotify(&gEfiSmmSwDispatch2ProtocolGuid, SwSmiHook, &Registration);

    //
    // following items is Aptio-4 compatible.
    // We should use Mde library instead.
    //   only one reason to use this library is for get EFI_RUNTIME_SERVICES which located in SMM.
    //
    InitAmiLib(ImageHandle, SystemTable);
    Status = InitSmmHandler(ImageHandle, SystemTable, InSmmFunction, NULL);

	return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
