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

/** @file SecMain.h
    SEC definitions

**/


#ifndef _SECMAIN_H_
#define _SECMAIN_H_

#include <PiPei.h>
#include <Ppi/SecPlatformInformation.h>
#include <Library/BaseLib.h>

#include <Ppi/TemporaryRamSupport.h>

#include <Library/BaseMemoryLib.h>
#include <Library/DebugAgentLib.h>
#include <Library/UefiCpuLib.h>
#include <Library/DebugLib.h>
#include <Library/PeCoffGetEntryPointLib.h>
#include <Library/PeCoffExtraActionLib.h>
#include <CpuCspLib.h>
#include <CPU.h>

#define PEI_ENTRY_POINTER_FIXED 0xFFFFFFE0

VOID
EFIAPI
SecCPhase2 (
  IN    VOID    *Context
);

EFI_STATUS TemporaryRamMigration(
  IN CONST EFI_PEI_SERVICES   **PeiServices,
  IN EFI_PHYSICAL_ADDRESS     TemporaryMemoryBase,
  IN EFI_PHYSICAL_ADDRESS     PermanentMemoryBase,
  IN UINTN                    CopySize
);

VOID
EFIAPI
SecSwitchStack (
  UINT32   OldStack,
  UINT32   NewStack
  );

#endif

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