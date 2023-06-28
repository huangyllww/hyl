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

/** @file
  Contains list of include files for WHEA

*/

#ifndef _EFI_WHEA_INIT_H
#define _EFI_WHEA_INIT_H

#include "Efi.h"
#include <IndustryStandard/Acpi.h>
#include <Protocol/AcpiTable.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include "Whea.h"
#include "Token.h"
#include <AmiCompatibilityPkg/Include/AcpiRes.h>

extern EFI_BOOT_SERVICES      *gBS,*pBS;
extern EFI_RUNTIME_SERVICES   *gRT;
extern EFI_SYSTEM_TABLE       *gST;

WHEA_MODULE_PRIVATE_DATA   *mWheaData;

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
