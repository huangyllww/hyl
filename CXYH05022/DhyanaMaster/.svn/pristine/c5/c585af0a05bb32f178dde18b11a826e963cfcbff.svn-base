//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**       (C)Copyright 1985-2015, American Megatrends, Inc.     **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**    5555 Oakbrook Pkwy, Norcross, Suite 200, Georgia 30093   **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

//<AMI_FHDR_START>
//---------------------------------------------------------------------------
// Name:        AmiFwUpdateCoreIntf.h
//
// Description: AMI BIOS Firmware Update Application header
//---------------------------------------------------------------------------
//<AMI_FHDR_END>

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Guid/FileInfo.h>
#include <Guid/FileSystemInfo.h>
#include "AmiFwUpdateBmcCore.h"
#include "AmiFwUpdateBmc.h"
#include "AmiLib.h"
#include "AmiDxeLib.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define MAX_LARGE_STRING_SIZE   1024

//---------------------------------------------------------------------------
// Core functionalities
//---------------------------------------------------------------------------
void FWCore_SetFlashMethod();
void FWCore_SetFlashOptions();
void FWCore_SetUpdateFlag();
EFI_STATUS FWCore_SetRomFile(EFI_HANDLE FsHandle, CHAR16 *FileName);
void FWCore_SetSourceDiskHandle(EFI_HANDLE FsHandle);
BOOLEAN FWCore_SetRomFileName();
EFI_STATUS FWCore_OpenROMImageFile(void **Buffer, UINTN *Size);
EFI_STATUS FWCore_ValidateBIOSImage();
EFI_STATUS FWCore_StartBIOSUpdate();
EFI_STATUS FWCore_StartOtherFwUpdate();
void UpdateFlashingStatus(UINT16 token, UINT64 CurLoc, UINT64 Progress);
void UpdateFlashingSuccess(UINT16 token);
VOID DisplayCommonProgress (UINT64 CurLoc , UINT64 MaxLoc , UINT64 Size ,UINTN Flag);
BOOLEAN FWCore_IsSecureFlashSupported(UINT8 Policy);
CHAR16 AMI_LIB_WaitForKey();
void FWCore_ErrorMsg(EFI_STATUS Status) ;
UINTN Get_BCP_CheckBoxState() ;
BOOLEAN SetGlobalFlagForFlash(CHAR16 *CommandArgStr, UINT32 OptionSize);

//************************************************************************
//************************************************************************
//**                                                                  	**
//**        (C)Copyright 2015, American Megatrends, Inc.            	**
//**                                                                  	**
//**                       All Rights Reserved.                       	**
//**                                                                  	**
//**  5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093              **
//**                                                                  	**
//**                       Phone: (770)-246-8600                      	**
//**                                                                  	**
//************************************************************************
//************************************************************************



