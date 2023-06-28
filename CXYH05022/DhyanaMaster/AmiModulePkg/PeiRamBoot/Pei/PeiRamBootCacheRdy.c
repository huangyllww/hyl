//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
/** @file PeiRamBootCacheRdy.c
    PEI RAM BOOT Pei driver.

**/
//----------------------------------------------------------------------------
// Includes
// Statements that include other files
#include <PEI.h>
#include <AmiPeiLib.h>
#include <PeiRamBoot.h>
//----------------------------------------------------------------------------
// Function Externs

//----------------------------------------------------------------------------
// Local prototypes
EFI_GUID gRomCacheEnablePpiGuid = ROM_CACHE_ENABLE_PPI_GUID;

// PPI to be installed
EFI_PEI_PPI_DESCRIPTOR  RomCacheEnablePpiList[] = {
    {
        EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
        &gRomCacheEnablePpiGuid, NULL
    }
};
//----------------------------------------------------------------------------
// Local Variables

//----------------------------------------------------------------------------
// Function Definitions
/**
 *   PEI Entry Point for PeiRamBootCacheRdy Driver.
 *       
 *   @param FfsHeader 
 *   @param PeiServices 
 *   @retval EFI_SUCCESS Success
**/
EFI_STATUS
PeiRamBootCacheRdyEntry (
    IN EFI_FFS_FILE_HEADER  *FfsHeader,
    IN EFI_PEI_SERVICES     **PeiServices
)
{
    EFI_STATUS          Status;
    EFI_BOOT_MODE       BootMode;

    Status = (*PeiServices)->GetBootMode (PeiServices, &BootMode);
    if (EFI_ERROR(Status)) return Status;
    if ((BootMode == BOOT_ON_FLASH_UPDATE) || (BootMode == BOOT_ON_S3_RESUME) || \
        (BootMode == BOOT_IN_RECOVERY_MODE)) return Status;
    Status = (*PeiServices)->InstallPpi (PeiServices, RomCacheEnablePpiList);
    return Status;
}
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
