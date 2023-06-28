//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//**********************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:    PspfTpmLibDxe.c
//
// Description: Psp fTPM DXE lib
//
//<AMI_FHDR_END>
//**********************************************************************
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Setup.h>
#include <Pi/PiPeiCis.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/ReadOnlyVariable2.h>

BOOLEAN
IsHygonFtpmEnabled ()
{
    DEBUG((DEBUG_INFO, "PcdHygonFtpm=%d\n", PcdGet8(PcdHygonFtpm)));
    return (PcdGet8(PcdHygonFtpm) == 1) ? TRUE : FALSE;
}

EFI_STATUS EFIAPI PspfTpmLibPeiConstructor (
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
)
{
    EFI_STATUS 	                Status = EFI_SUCCESS;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI  *ReadOnlyVariable;
    UINTN                          VariableSize = sizeof(SETUP_DATA);
    SETUP_DATA                     SetupData;
    EFI_GUID                       gSetupGuid = SETUP_GUID;

    if (PcdGet8(PcdHygonFtpm) != 0xFF) return EFI_SUCCESS;
    
    Status = PeiServicesLocatePpi (
            &gEfiPeiReadOnlyVariable2PpiGuid,
            0,
            NULL,
            (VOID **) &ReadOnlyVariable
    );

    if (EFI_ERROR(Status))
        return Status;

    Status = ReadOnlyVariable->GetVariable(ReadOnlyVariable,
            L"Setup",
            &gSetupGuid,
            NULL,
            &VariableSize,
            &SetupData);

    if (EFI_ERROR(Status)) {
        DEBUG(( -1," Get Setup variable failed with error %d\n", Status));
        return Status;
    }

    PcdSet8(PcdHygonFtpm, SetupData.HygonFtpm);

    DEBUG(( -1," PcdHygonFtpm=%d\n", PcdGet8(PcdHygonFtpm)));
    return Status;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
