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
#include <Library/PcdLib.h>
#include <Library/PeiServicesLib.h>

//----------------------------------------------------------------------
// External Function/Variable Declaration
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal Function Declaration
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Global Variable Declaration
//----------------------------------------------------------------------


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
EFI_STATUS EFIAPI ProjectPei_Init (
    IN EFI_FFS_FILE_HEADER      *FfsHeader,
    IN EFI_PEI_SERVICES         **PeiServices
)
{
    EFI_STATUS         Status = EFI_SUCCESS;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI     *VariablePpi;
    EFI_GUID SetupVariableGuid = SETUP_GUID;    
    UINTN                               Size;
    SETUP_DATA                          SetupData;

    Status = PeiServicesLocatePpi (
               &gEfiPeiReadOnlyVariable2PpiGuid,
               0,
               NULL,
               (VOID **) &VariablePpi
               );
    if (!EFI_ERROR (Status)) {
        Size = sizeof (SETUP_DATA);
        Status = VariablePpi->GetVariable (
                        VariablePpi,
                        L"Setup",
                        &SetupVariableGuid,
                        NULL,
                        &Size,
                        &SetupData );
        DEBUG ((DEBUG_INFO, "ReadVariablePpi->GetVariable Status %r  \n", Status));
        if (EFI_ERROR(Status)) 
            return Status;
        if (SetupData.ComPortforLegacy < PCI_SERIAL_PORT_0_INDEX)
            PcdSetBool (PcdFchOscout1ClkContinous, TRUE);
        else
            PcdSetBool (PcdFchOscout1ClkContinous, FALSE);
        DEBUG ((DEBUG_INFO, "PcdFchOscout1ClkContinous %x SetupData.ComPortforLegacy %x \n", PcdGetBool (PcdFchOscout1ClkContinous), SetupData.ComPortforLegacy));
    }

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
