//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//----------------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------------

#include "AutoDxioConfiguration.h"
#include "AutoDxioFunctionList.h"

extern AUTO_DXIO_FUNCTION AUTO_DXIO_FUNCTION_LIST EndOfAutoDxioFunctionList;

AUTO_DXIO_FUNCTION *AutoDxioFunctionList[] = {
    AUTO_DXIO_FUNCTION_LIST NULL
};

CHAR8 *AutoDxioFunctionListName[] = {
    AUTO_DXIO_FUNCTION_LIST_NAMES NULL
};

//-----------------------------------------------------------------------------

static EFI_PEI_NOTIFY_DESCRIPTOR PeiNotifyList[] = {
    { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST, \
    &gAmdCpmTablePpiGuid, PeiConfigDxioSettings}
};

/**
 * Read Riser ID GPIO and config DXIO setting per riser id.
 */
EFI_STATUS  
PeiConfigDxioSettings (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi )
{
  EFI_STATUS                          Status = EFI_SUCCESS;
  AMD_CPM_TABLE_PPI                   *CpmTablePpiPtr; 
  UINT8                               iAutoDxioFunction;


  DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]\n\n",__FUNCTION__,__LINE__));


    
  Status = (*PeiServices)->LocatePpi (
            (CPM_PEI_SERVICES**)PeiServices,
            &gAmdCpmTablePpiGuid,
            0,
            NULL,
            (VOID**)&CpmTablePpiPtr
            );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for(iAutoDxioFunction=0; AutoDxioFunctionList[iAutoDxioFunction]!=NULL; iAutoDxioFunction++)
  {
    DEBUG((EFI_D_INFO, "AutoDxio.%a(%p)\n\n",AutoDxioFunctionListName[iAutoDxioFunction], AutoDxioFunctionList[iAutoDxioFunction]));
    AutoDxioFunctionList[iAutoDxioFunction](CpmTablePpiPtr);
    DEBUG((EFI_D_INFO, "====================================================================================================\n"));
  }
  return EFI_SUCCESS;
}

//----------------------------------------------------------------------------
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   ProjectPEI_Init
//
// Description: This function is the entry point for this PEI. This function
//              initializes the Project PEI phase.
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

EFI_STATUS
AutoDxioConfigurationInit (
    IN       EFI_PEI_FILE_HANDLE   FileHandle,
    IN CONST EFI_PEI_SERVICES     **PeiServices )
{
  EFI_STATUS                          Status = EFI_SUCCESS;


  (*PeiServices)->NotifyPpi( PeiServices, PeiNotifyList );

  return Status;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
