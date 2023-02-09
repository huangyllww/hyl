//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header: /Alaska/Tools/template.c 6     1/13/10 2:13p Felixp $
//
// $Revision: 6 $
//
// $Date: 1/13/10 2:13p $
//**********************************************************************
// Revision History
// ----------------
// $Log: /Alaska/Tools/template.c $
// 
// 6     1/13/10 2:13p Felixp
// 
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  <This File's Name>
//
// Description:	
//
//<AMI_FHDR_END>
//**********************************************************************

//----------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------
#include <Efi.h>
#include <Pei.h>
#include <Token.h>
#include <AmiLib.h>
#include <AmiPeiLib.h>
#include <Hob.h>
#include <Setup.h>
#include <AmiCspLib.h>
#include <OemRtcResetHook.h>
//----------------------------------------------------------------------
// External Function/Variable Declaration
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal Function Declaration
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Global Variable Declaration
//----------------------------------------------------------------------
//<Wangyia002-20170331 BIOS Recovery Pin Support +> 
#define ACPI_MMIO_BASE          0xFED80000ul
#define GPIO_BANK0_BASE         0x1500
//<Wangyia002-20170331 BIOS Recovery Pin Support -> 

//----------------------------------------------------------------------
// Function Definitions
//----------------------------------------------------------------------

//<Wangyia002-20170331 BIOS Recovery Pin Support +> 
#if defined(BIOS_RECOVERY_PIN_SUPPORT)&& (BIOS_RECOVERY_PIN_SUPPORT == 1)
BOOLEAN DetectRecoveryPin( IN EFI_PEI_SERVICES **PeiServices )
{
    UINT8     RecoveryPinValue = 1;
    BOOLEAN   IsRecovery = FALSE;

    RecoveryPinValue = MmioRead8 (ACPI_MMIO_BASE + GPIO_BANK0_BASE + (BIOS_RECOVERY_PIN << 2) + 2) & 0x01;
    DEBUG ((DEBUG_ERROR, "Recoveryvalue:0x%x\n",RecoveryPinValue)); 
    if (0 == RecoveryPinValue) {
      IsRecovery = TRUE;
    } 
    return IsRecovery;
}
#endif
//<Wangyia002-20170331 BIOS Recovery Pin Support -> 

//<Wangyia003-20170331 Add password clear pin function +>
#if defined(BIOS_PASSWORD_CLEAR_PIN_SUPPORT)&& (BIOS_PASSWORD_CLEAR_PIN_SUPPORT == 1)
BOOLEAN DetectPasswordClearPin(IN EFI_PEI_SERVICES **PeiServices)
{
    UINT8     PwdClrPinValue = 1;
    BOOLEAN   IsPwdClear = FALSE;
    
    PwdClrPinValue = MmioRead8 (ACPI_MMIO_BASE + GPIO_BANK0_BASE + (PWD_CLEAR_PIN << 2) + 2) & 0x01;
    DEBUG ((DEBUG_ERROR, "PwdClrPinValue:0x%x\n",PwdClrPinValue)); 
    if (0 == PwdClrPinValue) {
      IsPwdClear = TRUE;
    } 

    return IsPwdClear;    
}
#endif
//<Wangyia003-20170331 Add password clear pin function ->

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   OemPEI_Init
//
// Description: This function is the entry point for this PEI. This function
//              initializes the chipset Project
//
// Parameters:  FfsHeader   Pointer to the FFS file header
//              PeiServices Pointer to the PEI services table
//
// Returns:     Return Status based on errors that occurred while waiting for
//              time to expire.
//
// Notes:       This function could initialize Project for anything.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS EFIAPI OemPEI_Init (
    IN EFI_FFS_FILE_HEADER      *FfsHeader,
    IN EFI_PEI_SERVICES         **PeiServices
)
{
    EFI_STATUS         Status = EFI_SUCCESS;
//<Wangyia003-20170331 Add password clear pin function +>
#if defined(BIOS_PASSWORD_CLEAR_PIN_SUPPORT)&& (BIOS_PASSWORD_CLEAR_PIN_SUPPORT == 1)
    BOOLEAN            IsPWDClr;
    OEM_INFO_DATA_HOB  *pOemInfoHob;
    EFI_GUID           OemInfoDataHobGuid = OEM_INFO_DATA_HOB_GUID;
#endif
//<Wangyia003-20170331 Add password clear pin function -> 

    //TODO: Put your code here start 
    PEI_TRACE((-1, PeiServices,"OemPEI_Init Function Called...\n"));

//<Wangyia003-20170331 Add password clear pin function +>
#if defined(BIOS_PASSWORD_CLEAR_PIN_SUPPORT)&& (BIOS_PASSWORD_CLEAR_PIN_SUPPORT == 1)
    
    IsPWDClr = DetectPasswordClearPin(PeiServices);
    // Create Recovery Hob
    Status = (*PeiServices)->CreateHob(
        PeiServices, EFI_HOB_TYPE_GUID_EXTENSION, 
        sizeof(OEM_INFO_DATA_HOB), &pOemInfoHob);
    if (EFI_ERROR(Status)) 
        return Status;

    pOemInfoHob->Header.Name = OemInfoDataHobGuid;
    pOemInfoHob->IsPWDClear = IsPWDClr;
    DEBUG ((DEBUG_ERROR, "PWD:%x.\n",IsPWDClr)); 
#endif
//<Wangyia003-20170331 Add password clear pin function ->

    //TODO: Put your code here end
    return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
