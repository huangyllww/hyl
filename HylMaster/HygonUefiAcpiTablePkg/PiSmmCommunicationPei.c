//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file PiSmmCommunicationPei.c
    PiSmmCommunication PEI Driver.

**/

#include <PiPei.h>
#include <PiDxe.h>
#include <PiSmm.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Protocol/SmmCommunication.h>
#include <Ppi/SmmCommunication.h>
#include <Ppi/SmmControl.h>
#include <Guid/AcpiS3Context.h>
#include "ppi/ReadOnlyVariable2.h"
#include <token.h>

#include "PiSmmCommunicationPrivate.h"

/**
  Communicates with a registered handler.
  
  This function provides a service to send and receive messages from a registered UEFI service.

  @param[in] This                The EFI_PEI_SMM_COMMUNICATION_PPI instance.
  @param[in, out] CommBuffer     A pointer to the buffer to convey into SMRAM.
  @param[in, out] CommSize       The size of the data buffer being passed in.On exit, the size of data
                                 being returned. Zero if the handler does not wish to reply with any data.

  @retval EFI_SUCCESS            The message was successfully posted.
  @retval EFI_INVALID_PARAMETER  The CommBuffer was NULL.
  @retval EFI_NOT_STARTED        The service is NOT started.
**/
EFI_STATUS
EFIAPI
Communicate (
  IN CONST EFI_PEI_SMM_COMMUNICATION_PPI   *This,
  IN OUT VOID                              *CommBuffer,
  IN OUT UINTN                             *CommSize
  );

EFI_PEI_SMM_COMMUNICATION_PPI      mSmmCommunicationPpi = { Communicate };

EFI_PEI_PPI_DESCRIPTOR mPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiSmmCommunicationPpiGuid,
  &mSmmCommunicationPpi
};

CHAR16      gAmiAcpiTableAddrS3[] = AMI_ACPI_SMM_COMM_TABLE_ADDRESS_S3;
EFI_GUID    gAmiAcpiSmmCommTableGuid = AMI_ACPI_SMM_COMM_TABLE_GUID;

/**
  Communicates with a registered handler.
  
  This function provides a service to send and receive messages from a registered UEFI service.

  @param[in] This                The EFI_PEI_SMM_COMMUNICATION_PPI instance.
  @param[in, out] CommBuffer     A pointer to the buffer to convey into SMRAM.
  @param[in, out] CommSize       The size of the data buffer being passed in.On exit, the size of data
                                 being returned. Zero if the handler does not wish to reply with any data.

  @retval EFI_SUCCESS            The message was successfully posted.
  @retval EFI_INVALID_PARAMETER  The CommBuffer was NULL.
  @retval EFI_NOT_STARTED        The service is NOT started.
**/
EFI_STATUS
EFIAPI
Communicate (
  IN CONST EFI_PEI_SMM_COMMUNICATION_PPI   *This,
  IN OUT VOID                              *CommBuffer,
  IN OUT UINTN                             *CommSize
  )
{
  EFI_STATUS                       Status;
  PEI_SMM_CONTROL_PPI              *SmmControl;
  UINT8                            SmiCommand;
  UINTN                            Size;
  CONST EFI_PEI_SERVICES           **PeiServices;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *ReadOnlyVariable;
  UINTN                            VarSize = sizeof(EFI_PHYSICAL_ADDRESS);
  UINTN                            VarAttrib = 0;
  EFI_PHYSICAL_ADDRESS             CommunicationTableBufferPtr;

  DEBUG ((DEBUG_INFO, "PiSmmCommunicationPei Communicate Enter\n"));

  if (CommBuffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  PeiServices = GetPeiServicesTablePointer ();

  Status = (*PeiServices)->LocatePpi(
               PeiServices, &gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL,
               &ReadOnlyVariable );
  
  Status = ReadOnlyVariable->GetVariable( ReadOnlyVariable, \
                                          gAmiAcpiTableAddrS3, \
                                          &gAmiAcpiSmmCommTableGuid, \
                                          &VarAttrib, \
                                          &VarSize, \
                                          &CommunicationTableBufferPtr );
  DEBUG ((DEBUG_INFO, "GetVariable gAmiAcpiSmmCommTableGuid - %x\n", Status));
  DEBUG ((DEBUG_INFO, "GetVariable CommunicationTableBufferPtr - %x\n", CommunicationTableBufferPtr));

  if (Status != EFI_SUCCESS)
  {
      return Status;
  }
  //
  // Get needed resource
  //
  Status = PeiServicesLocatePpi (
             &gPeiSmmControlPpiGuid,
             0,
             NULL,
             (VOID **)&SmmControl
             );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_STARTED;
  }

  DEBUG ((DEBUG_INFO, "PiSmmCommunicationPei CommBuffer - %x\n", (UINTN)CommBuffer));

  *(EFI_PHYSICAL_ADDRESS *)(UINTN)CommunicationTableBufferPtr = (EFI_PHYSICAL_ADDRESS)(UINTN)CommBuffer;
  DEBUG ((DEBUG_INFO, "PiSmmCommunicationPei Buffer pointer - %x\n", (UINTN)CommunicationTableBufferPtr));

  //
  // Send command
  //

  SmiCommand = HYGON_UEFI_SW_SMI_VALUE;
  Size = sizeof(SmiCommand);
  Status = SmmControl->Trigger (
                         (EFI_PEI_SERVICES **)GetPeiServicesTablePointer (),
                         SmmControl,
                         (INT8 *)&SmiCommand,
                         &Size,
                         FALSE,
                         0
                         );
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_INFO, "PiSmmCommunicationPei Communicate Exit\n"));

  return EFI_SUCCESS;
}

/**
  Entry Point for PI SMM communication PEIM.

  @param  FileHandle              Handle of the file being invoked.
  @param  PeiServices             Pointer to PEI Services table.

  @retval EFI_SUCEESS     
  @return Others          Some error occurs.
**/
EFI_STATUS
EFIAPI
PiSmmCommunicationPeiEntryPoint (
  IN EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES    **PeiServices
  )
{
  EFI_BOOT_MODE                   BootMode;

  BootMode = GetBootModeHob ();
  if (BootMode != BOOT_ON_S3_RESUME) {
    return EFI_UNSUPPORTED;
  }

  PeiServicesInstallPpi (&mPpiList);

  return RETURN_SUCCESS;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
