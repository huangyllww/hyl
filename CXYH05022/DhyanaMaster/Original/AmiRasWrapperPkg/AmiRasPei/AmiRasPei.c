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

/** @file AmiRasPei.c
    This file contains code for RAS related functionality

**/

#include <Library/PeiServicesLib.h>
#include <Setup.h>
#include <AmiPeiLib.h>
#include <Ppi/ReadOnlyVariable2.h>

/**
    This function is the entry point for AmiRasPei PEIM.

    @param FileHandle Pointer to the FFS file header.
    @param PeiServices Pointer to the PEI services table.

    @retval EFI_STATUS EFI_SUCCESS

**/

EFI_STATUS
EFIAPI
AmiRasPeiEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
    EFI_STATUS                  Status = EFI_SUCCESS;
    
    SETUP_DATA                          SetupData;
    UINTN                               VariableSize;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI     *ReadOnlyVariable;
    EFI_GUID                            SetupGuid= SETUP_GUID;

    Status = (*PeiServices)->LocatePpi(
        PeiServices,
        &gEfiPeiReadOnlyVariable2PpiGuid,
        0, NULL,
        &ReadOnlyVariable
    );
    ASSERT_PEI_ERROR(PeiServices, Status);
    
    VariableSize = sizeof(SETUP_DATA);
    Status = ReadOnlyVariable->GetVariable(ReadOnlyVariable, L"Setup", &SetupGuid,
                                           NULL, &VariableSize, &SetupData);
    
    if(EFI_ERROR(Status)){
        // Set default value from RasWrapperSetup.sd
        PcdSetBool (PcdAmdCcxCfgPFEHEnable, FALSE);
        PcdSetBool(PcdMcaErrThreshEn, FALSE);
        PcdSet16(PcdMcaErrThreshCount, 0);
        return  EFI_SUCCESS;
    }

    if(SetupData.McaErrThreshCount == 0){
        PcdSetBool(PcdMcaErrThreshEn, FALSE);
    }
    PcdSet16(PcdMcaErrThreshCount, SetupData.McaErrThreshCount);
    PcdSetBool (PcdAmdCcxCfgPFEHEnable, SetupData.Pfeh);
    return  Status;
}

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
