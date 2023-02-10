//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file CrbSmi.c
    This file contains code for all CRB SMI events

**/

//---------------------------------------------------------------------------
// Include(s)
//---------------------------------------------------------------------------

#include <PiSmm.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/DebugLib.h>

#include <token.h>
#include <AmiDxeLib.h> // Optional. should use Mde Library instead.
#include <AmiCspLib.h> // Optional. should use Mde Library instead.

// Produced Protocols

// Consumed Protocols
#include <Protocol/SmmBase2.h>
#include <Protocol/S3SmmSaveState.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmSxDispatch2.h>
#include <Protocol/SmmPeriodicTimerDispatch2.h>
#include <Protocol/SmmUsbDispatch2.h>
#include <Protocol/SmmGpiDispatch2.h>
#include <Protocol/SmmStandbyButtonDispatch2.h>
#include <Protocol/SmmPowerButtonDispatch2.h>
#include <Protocol/SmmIoTrapDispatch2.h>

#if defined(HYGONCRBEC_SUPPORT)&&(HYGONCRBEC_SUPPORT == 1)
#include <AmdCrbEc/PlatformEC/AmdCrbEc.h>
VOID
TiEcWrite (
  IN UINT8  Index,
  IN UINT8  Value
  );
UINT8
TiEcRead(
  IN UINT8  Index
  );
#endif
//---------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//---------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

#define CRB_GPI_SMI_TEST            0 // switch to turn on GPI smi test.
#if CRB_GPI_SMI_TEST
	//
	// from PI1.2 VOLUME 4:
	// 	6.6 SMM General Purpose Input (GPI) Dispatch Protocol
	//  we should use bits mask as GpiNum.
	//
	// this macro is for compatible some Aptio-4 component to use index base.
	//
    #if GPI_DISPATCH_BY_BITMAP == 1
        #define MACRO_CONVER_TO_GPI(x)     BIT##x
    #else
        #define MACRO_CONVER_TO_GPI(x)     x
    #endif
#else
    #define MACRO_CONVER_TO_GPI(x)     x
#endif

// Type Definition(s)

// Function Prototype(s)

//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
// Variable Declaration(s)


// GUID Definition(s)

// Protocol Definition(s)

// External Declaration(s)

// Function Definition(s)

//---------------------------------------------------------------------------

/**
    This is a template CRB SMI GetContext for Porting.

    @param VOID

    @retval BOOLEAN FALSE

    @note  Here is the control flow of this function:
              1. Check if CRB Smi source.
              2. If yes, return TRUE.
              3. If not, return FALSE.
**/

BOOLEAN
GetCrbSmiContext (
  VOID
  )
{
    // Porting if needed
    return FALSE;
}

/**
    This is a template CRB SMI Handler for Porting.

    @param VOID

    @retval VOID
**/

VOID
CrbSmiHandler (
  VOID
  )
{
    // Porting if needed
}

//DM1-238 >>>
/**
    This is save IOAPIC data to BootScript.

    @param This, EFI_S3_SMM_SAVE_STATE_PROTOCOL protocol
	@param Index, APIC Index

    @retval EFI_STATUS
**/
EFI_STATUS S3SaveIOAPICDataToBootScript(
        EFI_S3_SMM_SAVE_STATE_PROTOCOL  *This, 
        UINT8                           Index
)
{
    UINT8        Data;
    EFI_STATUS   Status;
    
    Data = ReadIo8IdxData(PIRQ_IO_INDEX, Index);
    TRACE((TRACE_ALWAYS, "S3SaveIOAPICDataToBootScript Index=%X, Data=%02X!\n", Index, Data));
    Status = This->Write(This, EFI_BOOT_SCRIPT_IO_WRITE_OPCODE, EfiBootScriptWidthUint8, PIRQ_IO_INDEX, 1, &Index);
    Status = This->Write(This, EFI_BOOT_SCRIPT_IO_WRITE_OPCODE, EfiBootScriptWidthUint8, PIRQ_IO_DATA, 1, &Data);
    return Status;
}

/**
    This is save Mem data to BootScript.

    @param This, EFI_S3_SMM_SAVE_STATE_PROTOCOL protocol
    @param Width, EFI_BOOT_SCRIPT_WIDTH
    @param Index, APIC Index

    @retval EFI_STATUS
**/
EFI_STATUS S3SaveMemDataToBootScript(
        EFI_S3_SMM_SAVE_STATE_PROTOCOL  *This, 
        EFI_BOOT_SCRIPT_WIDTH           Width,
        UINTN                           MemoryPtr
)
{
    UINTN        Data;
    EFI_STATUS   Status;
    
    switch (Width)
    {
    case EfiBootScriptWidthUint8:
        Data = MmioRead8(MemoryPtr);
        break;

    case EfiBootScriptWidthUint16:
        Data = MmioRead16(MemoryPtr);
        break;

    case EfiBootScriptWidthUint32:
        Data = MmioRead32(MemoryPtr);
        break;
        
    case EfiBootScriptWidthUint64:
        Data = MmioRead64(MemoryPtr);
        break;
    
    default:
        return EFI_INVALID_PARAMETER;
    }
    Status = This->Write(This, EFI_BOOT_SCRIPT_MEM_WRITE_OPCODE, Width, MemoryPtr, 1, &Data);
    TRACE((TRACE_ALWAYS, "S3SaveMemDataToBootScript MemoryPtr=%lX, Data=%lX!\n", MemoryPtr, Data));
    return Status;
}

/**
    This is save IOAPIC data.

    @param VOID

    @retval VOID
**/

VOID CrbSaveS3ToBootScriptCallback(VOID)
{
    UINT8                           Index;
    EFI_S3_SMM_SAVE_STATE_PROTOCOL  *S3SaveProtocol;
    EFI_STATUS                      Status;
    
    Status = gSmst->SmmLocateProtocol(&gEfiS3SmmSaveStateProtocolGuid, NULL, &S3SaveProtocol);
    TRACE((TRACE_ALWAYS, "CrbSaveS3ToBootScriptCallback locate S3SaveProtocol Status=%r!\n", Status));
    if(EFI_ERROR(Status))
    {
        return;
    }
    
    //IOAPIC 
    Index = 0x08;
    S3SaveIOAPICDataToBootScript(S3SaveProtocol, Index);
    
    Index = 0x10;
    S3SaveIOAPICDataToBootScript(S3SaveProtocol, Index);
    
    Index = 0x74;
    S3SaveIOAPICDataToBootScript(S3SaveProtocol, Index);
    
    Index = 0x75;
    S3SaveIOAPICDataToBootScript(S3SaveProtocol, Index);

    //0x80 ~ 0x07
    for(Index = 0x80; Index < 0x88; Index++)
    {
        S3SaveIOAPICDataToBootScript(S3SaveProtocol, Index);
    }
    
    Index = 0x90;
    S3SaveIOAPICDataToBootScript(S3SaveProtocol, Index);
    
    Index = 0x97;
    S3SaveIOAPICDataToBootScript(S3SaveProtocol, Index);
    
    Index = 0x9A;
    S3SaveIOAPICDataToBootScript(S3SaveProtocol, Index);
    
    Index = 0xC3;
    S3SaveIOAPICDataToBootScript(S3SaveProtocol, Index);
    
    Index = 0xE2;
    S3SaveIOAPICDataToBootScript(S3SaveProtocol, Index);
    
    //0xF0 ~ 0xF9
    for(Index = 0xF0; Index < 0xFA; Index++)
    {
        S3SaveIOAPICDataToBootScript(S3SaveProtocol, Index);
    }
    
    S3SaveMemDataToBootScript(S3SaveProtocol, EfiBootScriptWidthUint16, 0xFEDC0020);
}
//DM1-238 <<<

/**
    This is a template CRB software SMI Handler for Porting.

    @param DispatchHandle 
    @param Context OPTIONAL,
    @param CommBuffer OPTIONAL,
    @param CommBufferSize OPTIONAL

    @retval VOID
**/

EFI_STATUS
CrbSwSmiHandler (
  IN EFI_HANDLE	DispatchHandle,
  IN CONST VOID	*Context OPTIONAL,
  IN OUT VOID  	*CommBuffer OPTIONAL,
  IN OUT UINTN 	*CommBufferSize OPTIONAL
  )
{
	EFI_STATUS						Status = EFI_SUCCESS;
	//EFI_SMM_SW_REGISTER_CONTEXT  	*DispatchContext = (EFI_SMM_SW_REGISTER_CONTEXT*)Context;

	// Porting if needed
    if (CommBuffer != NULL) {
        if (((EFI_SMM_SW_CONTEXT*)CommBuffer)->CommandPort == CRB_SWSMI) {
        	// do something if needed.
        }
    }

    {
		//
		// following items is Aptio-4 compatible.
		// We should use Mde library instead.
		//   only one reason to use this library is for get EFI_RUNTIME_SERVICES which located in SMM.
		//
    	EFI_TIME                    Time;
    	TRACE((-1,"<< CrbSwSmiHandler 001 %x >>\n",pRS->GetTime));
    	pRS->GetTime(&Time, NULL);
    	TRACE((-1,"<< CrbSwSmiHandler Year(%x) Month(%x) Day(%x) Hour(%x) >>\n"
    		,Time.Year
			,Time.Month
			,Time.Day
			,Time.Hour
    		));
    	pRS->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);
    }

    return Status;
}

/**
    This is a template CRB Sx SMI Handler for Porting.

    @param DispatchHandle 
    @param Context OPTIONAL,
    @param CommBuffer OPTIONAL,
    @param CommBufferSize OPTIONAL

    @retval VOID
**/

EFI_STATUS
CrbSxSmiHandler (
  IN EFI_HANDLE	DispatchHandle,
  IN CONST VOID	*Context OPTIONAL,
  IN OUT VOID  	*CommBuffer OPTIONAL,
  IN OUT UINTN 	*CommBufferSize OPTIONAL
  )
{
	EFI_STATUS						Status = EFI_SUCCESS;
	EFI_SMM_SX_REGISTER_CONTEXT  	*DispatchContext = (EFI_SMM_SX_REGISTER_CONTEXT*)Context;
	
	//DM1-238 >>>
    if (DispatchContext->Type == SxS3) {
        CrbSaveS3ToBootScriptCallback(); 
    }
	//DM1-238 <<<

    if (DispatchContext->Type == SxS4) {
	//### Remove SxS3 due to S3 WOL fail ### || DispatchContext->Type == SxS3) {
    // Workaround to avoid post time too long if user do following step.
    //		OS-> Shutdown-> BIOS Setup-> Reset button-> OS-> post time validation.
	#if defined(HYGONCRBEC_SUPPORT)&&(HYGONCRBEC_SUPPORT == 1)
		UINT8 Data;
    // Porting if needed
		Data = TiEcRead (TI_EC_RAM_AC);
		if (Data & BIT0) {
			Data &= ~BIT7;
			TiEcWrite(TI_EC_RAM_AC, Data);
		}
	#endif
    }
	return Status;
}

/**
    This is a template CRB Gpi SMI Handler for Porting.

    @param DispatchHandle 
    @param Context OPTIONAL,
    @param CommBuffer OPTIONAL,
    @param CommBufferSize OPTIONAL

    @retval VOID
**/

EFI_STATUS
CrbGpiSmiHandler (
  IN EFI_HANDLE	DispatchHandle,
  IN CONST VOID	*Context OPTIONAL,
  IN OUT VOID  	*CommBuffer OPTIONAL,
  IN OUT UINTN 	*CommBufferSize OPTIONAL
  )
{
	EFI_STATUS						Status = EFI_SUCCESS;
	EFI_SMM_GPI_REGISTER_CONTEXT  	*DispatchContext = (EFI_SMM_GPI_REGISTER_CONTEXT*)Context;

    // Porting if needed

    // For GPI 5 for PCIE express card
    if (DispatchContext->GpiNum == MACRO_CONVER_TO_GPI(5)) {

    }

    if (DispatchContext->GpiNum == MACRO_CONVER_TO_GPI(23)) {

    }

    return Status;
}

/**
    This is an entry for CRB SMM Child Dispatcher Handler.

    @param DispatchHandle 
    @param Context OPTIONAL,
    @param CommBuffer OPTIONAL,
    @param CommBufferSize OPTIONAL

    @retval EFI_STATUS EFI_SUCCESS

    @note  Here is the control flow of this function:
              1. Read SMI source status registers.
              2. If source, call handler.
              3. Repeat #2 for all sources registered.
**/

EFI_STATUS
CrbChildDispatcher (
  IN EFI_HANDLE	DispatchHandle,
  IN CONST VOID	*Context OPTIONAL,
  IN OUT VOID 	*CommBuffer OPTIONAL,
  IN OUT UINTN 	*CommBufferSize OPTIONAL
  )
{
	EFI_STATUS						Status = EFI_SUCCESS;

    if (GetCrbSmiContext()) CrbSmiHandler();

    return Status;
}

/**
    This function is Aptio-4 compatible. not required no more.
    We should use Mde library instead.

    @param ImageHandle Image handle
    @param SystemTable Pointer to the system table

    @retval EFI_STATUS EFI_SUCCESS

**/

EFI_STATUS
EFIAPI
CrbDummyFunction (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable )
{
	EFI_STATUS           	Status = EFI_SUCCESS;
    return Status;
}

/**
    Installs CRB SMM Child Dispatcher Handler.

    @param ImageHandle Image handle
    @param SystemTable Pointer to the system table

    @retval EFI_NOT_FOUND The SMM Base protocol is not found.
    @retval EFI_SUCCESS Installs CRB SMM Child Dispatcher Handler successfully.
**/

EFI_STATUS
EFIAPI
CrbSmiInit (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
    EFI_STATUS                		Status;
	EFI_SMM_SW_DISPATCH2_PROTOCOL  	*SwDispatch = NULL;
	EFI_SMM_SX_DISPATCH2_PROTOCOL  	*SxDispatch = NULL;
#if CRB_GPI_SMI_TEST
	EFI_SMM_GPI_DISPATCH2_PROTOCOL 	*GpiDispatch = NULL;
#endif
    EFI_SMM_SW_REGISTER_CONTEXT     SwContext = {CRB_SWSMI};
    EFI_SMM_SX_REGISTER_CONTEXT     SxContext = {SxS3, SxEntry};
#if CRB_GPI_SMI_TEST
    EFI_SMM_GPI_REGISTER_CONTEXT    GpiContext = {MACRO_CONVER_TO_GPI(5)}; // _L05 default for PCIExpress card
#endif
    EFI_HANDLE                      Handle = NULL;

    Status  = gSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, &SwDispatch);
    ASSERT_EFI_ERROR(Status);
    if (!EFI_ERROR(Status)) {
        Status  = SwDispatch->Register( SwDispatch, \
                                         CrbSwSmiHandler, \
                                         &SwContext, \
                                         &Handle );
        ASSERT_EFI_ERROR(Status);
    }

    Status  = gSmst->SmmLocateProtocol (&gEfiSmmSxDispatch2ProtocolGuid, NULL, &SxDispatch);
    ASSERT_EFI_ERROR(Status);
    if (!EFI_ERROR(Status)) {
        Status  = SxDispatch->Register( SxDispatch, \
                                         CrbSxSmiHandler, \
                                         &SxContext, \
                                         &Handle );
        ASSERT_EFI_ERROR(Status);
        SxContext.Type =  SxS4;
        Status  = SxDispatch->Register( SxDispatch, \
                                         CrbSxSmiHandler, \
                                         &SxContext, \
                                         &Handle );
        ASSERT_EFI_ERROR(Status);
    }

#if CRB_GPI_SMI_TEST
    Status  = gSmst->SmmLocateProtocol (&gEfiSmmGpiDispatch2ProtocolGuid, NULL, &GpiDispatch);
    ASSERT_EFI_ERROR(Status);
    if (!EFI_ERROR(Status)) {
        Status  = GpiDispatch->Register( GpiDispatch, \
                                         CrbGpiSmiHandler, \
                                         &GpiContext, \
                                         &Handle );
        ASSERT_EFI_ERROR(Status);

        GpiContext.GpiNum = MACRO_CONVER_TO_GPI(23);
        Status  = GpiDispatch->Register( GpiDispatch, \
                                         CrbGpiSmiHandler, \
                                         &GpiContext, \
                                         &Handle );
        ASSERT_EFI_ERROR(Status);
    }
#endif

    Handle = NULL;
    //Register call backs
    Status = gSmst->SmiHandlerRegister(CrbChildDispatcher, NULL, &Handle);


    //
    // locate S3 Save protocol.
    //
    //### 5.004 not support yet ###Status = gSmst->SmmLocateProtocol (&gEfiS3SmmSaveStateProtocolGuid, \
    //### 5.004 not support yet ###                              NULL, \
    //### 5.004 not support yet ###                              &gBootScriptSave );
    //### 5.004 not support yet ###ASSERT_EFI_ERROR(Status);



    //
    // following items is Aptio-4 compatible.
    // We should use Mde library instead.
    //   only one reason to use this library is for get EFI_RUNTIME_SERVICES which located in SMM.
    //
    InitAmiLib(ImageHandle, SystemTable);
    Status = InitSmmHandler(ImageHandle, SystemTable, CrbDummyFunction, NULL);

    return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
