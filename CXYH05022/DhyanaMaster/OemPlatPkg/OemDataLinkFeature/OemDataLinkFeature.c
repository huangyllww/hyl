#include <Library/PeiServicesLib.h>
#include <Setup.h>
#include <AmiPeiLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library\DebugLib.h>

/**
    This function is the entry point for DataLinkFeature PEIM.

    @param FileHandle Pointer to the FFS file header.
    @param PeiServices Pointer to the PEI services table.

    @retval EFI_STATUS EFI_SUCCESS

**/

EFI_STATUS
EFIAPI
DataLinkFeatureEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
    EFI_STATUS                  Status = EFI_SUCCESS;
    
    SETUP_DATA                          SetupData;
    UINTN                               VariableSize;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI     *ReadOnlyVariable;
    EFI_GUID                            SetupGuid= SETUP_GUID;

    DEBUG ((EFI_D_INFO, "OemDebug %a Start\n",__FUNCTION__));

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
    
    if(!EFI_ERROR(Status))
    {
        // Set default value from RasWrapperSetup.sd
        PcdSet8(PcdDataLinkFeature, SetupData.DataLinkFeature);
        DEBUG ((EFI_D_INFO, "DataLinkFeature PcdSet = %d\n",SetupData.DataLinkFeature));
       
        return  EFI_SUCCESS;
    }
    
    return  Status;
}