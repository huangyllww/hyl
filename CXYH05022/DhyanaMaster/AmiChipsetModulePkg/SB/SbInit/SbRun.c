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
/** @file SbRun.c
    This file contains code for SouthBridge runtime
    protocol

**/
//*************************************************************************

// Module specific Includes
#include <Uefi.h>
#include "token.h"
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <Library/AmiChipsetRuntimeServiceLib.h>
#include <Library/PcdLib.h>

// Produced Protocols
#include <Protocol\Metronome.h>
#include <Protocol\RealTimeClock.h>

//----------------------------------------------------------------------------
//          Variable Declaration
//----------------------------------------------------------------------------

// Function Prototypes
EFI_STATUS
WaitForTick (
    IN  EFI_METRONOME_ARCH_PROTOCOL     *This,
    IN  UINT32                          TickNumber
);

// Architectural Protocol Definitions
EFI_METRONOME_ARCH_PROTOCOL mMetronomeProtocol = {
    WaitForTick,
    100
};

// Function Definitions

//----------------------------------------------------------------------------
//   USUALLY NO PORTING REQUIRED FOR THE FOLLOWING ROUTINES
//----------------------------------------------------------------------------

/**
    This function calculates the time needed to delay and then
    calls a library function to delay that amount of time

    @param This Pointer to the instance of
        the Metronome Arch Protocol
    @param TickNumber Number of ticks needed based off of tick period
        defined in Protocol Definiton

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/

EFI_STATUS
WaitForTick (
    IN  EFI_METRONOME_ARCH_PROTOCOL     *This,
    IN  UINT32                          TickNumber
    )
{
    EFI_STATUS          Status;
    UINT32              TotalTime;

    // Manipulate TickNumber into a valid value for the library function call
    // the Current Resolution is 10us.
    // The Library uses Microseconds to count delayed time.
    TotalTime = (TickNumber * This->TickPeriod) / 10;

    // Make Library Function call here
    Status = CountTime( TotalTime, PcdGet16 (PcdAcpiIoPortBaseAddress) );

    return Status;
}

/**
    Submit runtime services both SMM and runtime.

    @param ImageHandle Image handle
    @param SystemTable Pointer to the system table

              
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
EFI_STATUS
SbRuntimeServiceInit (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
    pRS->GetTime        = AmiChipsetGetTime;
    pRS->SetTime        = AmiChipsetSetTime;
    pRS->GetWakeupTime  = AmiChipsetGetWakeupTime;
    pRS->SetWakeupTime  = AmiChipsetSetWakeupTime;

    return EFI_SUCCESS;
}

/**
    This function is the entry point for this DXE. This function
    installs the runtime services related to SB

    @param ImageHandle Image handle
    @param SystemTable Pointer to the system table

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
EFI_STATUS
SbRuntimeInitEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{

    EFI_STATUS      Status = EFI_SUCCESS;
    EFI_HANDLE      Handle = NULL;

    InitAmiRuntimeLib(ImageHandle, SystemTable, NULL, NULL);
    PROGRESS_CODE(DXE_SBRUN_INIT);

    // CspLib PM Specific function to check and Report
    // CMOS Battary and Power Supply Power loss/failure
    CspLibCheckPowerLoss();

    Status = SbRuntimeServiceInit(ImageHandle, SystemTable);
    ASSERT_EFI_ERROR(Status);

    Status = pBS->InstallProtocolInterface (
                    &ImageHandle, \
                    &gEfiMetronomeArchProtocolGuid, \
                    EFI_NATIVE_INTERFACE, \
                    &mMetronomeProtocol
                    );
    ASSERT_EFI_ERROR(Status);

    // This protocol is to notify core that the Runtime Table has been
    // updated, so it can update the runtime table CRC.
    //
    Handle = NULL;
    return pBS->InstallMultipleProtocolInterfaces (
                  &Handle, \
                  &gEfiRealTimeClockArchProtocolGuid, \
                  NULL, \
                  NULL
                  );
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
