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

  Contains list of include files for Whea
*/

#ifndef _EFI_SMM_WHEA_INIT_H
#define _EFI_SMM_WHEA_INIT_H

#include "Efi.h"
#include <IndustryStandard/Acpi.h>
#include <Protocol/AcpiTable.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include "SmmWhea.h"
#include <Library/UefiLib.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmBase2.h>
#include <Library/SmmServicesTableLib.h>
#include <Include/AmiLib.h>
#include "Token.h"

extern EFI_BOOT_SERVICES             *gBS,*pBS;
extern EFI_RUNTIME_SERVICES          *gRT;
extern EFI_SYSTEM_TABLE              *gST;
extern EFI_SMM_SYSTEM_TABLE2         *gSmst;

WHEA_MODULE_PRIVATE_DATA   *mWheaData;

#define SOURCE_ID0                 0
#define SOURCE_ID1                 1
#define RELATED_SOURCE_ID_NO_ALT   0xffff
#define RECORD_SECTION_ENABLE      0x03
#define ERR_SOURCE_ENABLED         0x01
#define RECORDS_PREALLOCATE1       0x01
#define SECTIONS_PER_RECORD1       0x01
#define NOTIFY_TYPE_NMI            0x04
#define NOTIFY_TYPE_POLLED         0
#define NOTIFY_TYPE_SCI            3
#define INT_VECTOR_NMI             2

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
