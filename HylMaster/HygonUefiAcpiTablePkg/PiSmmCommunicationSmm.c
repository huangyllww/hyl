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

/** @file PiSmmCommunicationSmm.c
    PiSmmCommunication SMM Driver.

**/

#include <PiSmm.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmReadyToLock.h>
#include <Protocol/SmmCommunication.h>
#include <Protocol/AcpiTable.h>
#include <Ppi/SmmCommunication.h>
#include <Guid/Acpi.h>
#include <Library/IoLib.h>
#include <Protocol/SmmVariable.h>
#include <token.h>
#include "PiSmmCommunicationPrivate.h"
#include <Library/AmiBufferValidationLib.h>

EFI_SMM_COMMUNICATION_CONTEXT  SmmCommunicationContext = {
  SMM_COMMUNICATION_SIGNATURE
};

EFI_SMM_COMMUNICATION_ACPI_TABLE  SmmCommunicationAcpiTable = {
  EFI_ACPI_4_0_UEFI_ACPI_DATA_TABLE_SIGNATURE,
  sizeof (EFI_SMM_COMMUNICATION_ACPI_TABLE),
  0x1,   // Revision
  0x0,   // Checksum
  {0x0}, // OemId[6]
  0x0,   // OemTableId
  0x0,   // OemRevision
  0x0,   // CreatorId
  0x0,   // CreatorRevision
  {0x0},                                                 // Identifier
  OFFSET_OF (EFI_SMM_COMMUNICATION_ACPI_TABLE, SwSmiNumber),  // DataOffset
  0x0,                                                   // SwSmiNumber
  0x0                                                    // BufferPtrAddress
};

CHAR16      gAmiAcpiTableAddrS3[] = AMI_ACPI_SMM_COMM_TABLE_ADDRESS_S3;
EFI_GUID    gAmiAcpiSmmCommTableGuid = AMI_ACPI_SMM_COMM_TABLE_GUID;

/**
  Set SMM communication context.
**/
/*
VOID
SetCommunicationContext (
  VOID
  )
{
  EFI_STATUS  Status;

  Status = gSmst->SmmInstallConfigurationTable (
                    gSmst,
                    &gEfiPeiSmmCommunicationPpiGuid,
                    &mSmmCommunicationContext,
                    sizeof(mSmmCommunicationContext)
                    );
  ASSERT_EFI_ERROR (Status);
}
*/

/**
  Allocate EfiACPIMemoryNVS below 4G memory address.

  This function allocates EfiACPIMemoryNVS below 4G memory address.

  @param  Size         Size of memory to allocate.
  
  @return Allocated address for output.

**/
VOID*
AllocateAcpiNvsMemoryBelow4G (
  IN   UINTN   Size
  )
{
  UINTN                 Pages;
  EFI_PHYSICAL_ADDRESS  Address;
  EFI_STATUS            Status;
  VOID*                 Buffer;

  Pages = EFI_SIZE_TO_PAGES (Size);
  Address = 0xffffffff;

  Status  = gBS->AllocatePages (
                   AllocateMaxAddress,
                   EfiACPIMemoryNVS,
                   Pages,
                   &Address
                   );
  ASSERT_EFI_ERROR (Status);

  Buffer = (VOID *) (UINTN) Address;
  ZeroMem (Buffer, Size);

  return Buffer;
}

/**
  Dispatch function for a Software SMI handler.

  @param DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param Context         Points to an optional handler context which was specified when the
                         handler was registered.
  @param CommBuffer      A pointer to a collection of data in memory that will
                         be conveyed from a non-SMM environment into an SMM environment.
  @param CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS Command is handled successfully.

**/
EFI_STATUS
EFIAPI
PiSmmCommunicationHandler (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context         OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  )
{
  UINTN                            CommSize;
  EFI_STATUS                       Status;
  EFI_SMM_COMMUNICATE_HEADER       *CommunicateHeader;
  EFI_PHYSICAL_ADDRESS             *BufferPtrAddress;

  DEBUG ((DEBUG_INFO, "PiSmmCommunicationHandler Enter\n"));

  BufferPtrAddress = (EFI_PHYSICAL_ADDRESS *)(UINTN)SmmCommunicationContext.BufferPtrAddress;
  CommunicateHeader = (EFI_SMM_COMMUNICATE_HEADER *)(UINTN)*BufferPtrAddress;
  DEBUG ((DEBUG_INFO, "PiSmmCommunicationHandler CommunicateHeader - %x\n", CommunicateHeader));
  if (CommunicateHeader == NULL)
  {
      DEBUG ((DEBUG_INFO, "PiSmmCommunicationHandler is NULL, needn't to call dispatch function\n"));
      return EFI_SUCCESS;
  }
  CommSize = (UINTN)CommunicateHeader->MessageLength;
  Status = AmiValidateMemoryBuffer((VOID *)BufferPtrAddress, CommSize);
  if (Status != EFI_SUCCESS)
      return Status;

  //
  // Call dispatch function
  //
  DEBUG ((DEBUG_INFO, "PiSmmCommunicationHandler Data - %x\n", &CommunicateHeader->Data[0]));
  Status = gSmst->SmiManage (
                    &CommunicateHeader->HeaderGuid,
                    NULL,
                    &CommunicateHeader->Data[0],
                    &CommSize
                    );
  DEBUG ((DEBUG_INFO, "PiSmmCommunicationHandler %r\n", Status));
  DEBUG ((DEBUG_INFO, "PiSmmCommunicationHandler Exit\n"));

  return (Status == EFI_SUCCESS) ? EFI_SUCCESS : EFI_INTERRUPT_PENDING;
}

/**
  Entry Point for PI SMM communication SMM driver.

  @param[in] ImageHandle  Image handle of this driver.
  @param[in] SystemTable  A Pointer to the EFI System Table.

  @retval EFI_SUCEESS     
  @return Others          Some error occurs.
**/
EFI_STATUS
EFIAPI
PiSmmCommunicationSmmEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                    Status;
  EFI_SMM_SW_DISPATCH2_PROTOCOL *SmmSwDispatch2;
  EFI_SMM_SW_REGISTER_CONTEXT   SmmSwDispatchContext;
  EFI_HANDLE                    DispatchHandle;
  EFI_ACPI_TABLE_PROTOCOL       *AcpiTableProtocol;
  UINTN                         TableKey;
  EFI_PHYSICAL_ADDRESS          *BufferPtrAddress;
  UINTN                         VariableSize = sizeof(EFI_PHYSICAL_ADDRESS);
  UINT32                        Attribute = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS;
  EFI_PHYSICAL_ADDRESS          OldBufferPtrAddress;
#if defined (NVRAM_VERSION) && (NVRAM_VERSION > 6)
  EFI_SMM_VARIABLE_PROTOCOL     *SmmVariable;
#endif

  //
  // Register software SMI handler
  //
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmSwDispatch2ProtocolGuid,
                    NULL,
                    (VOID **)&SmmSwDispatch2
                    );
  ASSERT_EFI_ERROR (Status);

  SmmSwDispatchContext.SwSmiInputValue = (UINTN)HYGON_UEFI_SW_SMI_VALUE;
  Status = SmmSwDispatch2->Register (
                             SmmSwDispatch2,
                             PiSmmCommunicationHandler,
                             &SmmSwDispatchContext,
                             &DispatchHandle
                             );
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_INFO, "SmmCommunication SwSmi: %x\n", (UINTN)SmmSwDispatchContext.SwSmiInputValue));

  //
  // Set ACPI table
  //
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &AcpiTableProtocol);
  ASSERT_EFI_ERROR (Status);

  SmmCommunicationAcpiTable.SwSmiNumber = (UINT32)SmmSwDispatchContext.SwSmiInputValue;
  BufferPtrAddress = AllocateAcpiNvsMemoryBelow4G (sizeof(EFI_PHYSICAL_ADDRESS));
  DEBUG ((DEBUG_INFO, "BufferPtrAddress: %x\n", BufferPtrAddress));
  CopyMem (&SmmCommunicationAcpiTable.UefiAcpiDataTable.Identifier, &gEfiSmmCommunicationProtocolGuid, sizeof(gEfiSmmCommunicationProtocolGuid));
  SmmCommunicationAcpiTable.BufferPtrAddress = (EFI_PHYSICAL_ADDRESS)(UINTN)BufferPtrAddress;

  Status = AcpiTableProtocol->InstallAcpiTable (
                                AcpiTableProtocol,
                                &SmmCommunicationAcpiTable,
                                sizeof(SmmCommunicationAcpiTable),
                                &TableKey
                                );
  ASSERT_EFI_ERROR (Status);

  //
  // Save context
  //
  SmmCommunicationContext.SwSmiNumber = (UINT32)SmmSwDispatchContext.SwSmiInputValue;
  SmmCommunicationContext.BufferPtrAddress = (EFI_PHYSICAL_ADDRESS)(UINTN)BufferPtrAddress;
  DEBUG ((DEBUG_INFO, "SmmCommunicationAcpiTable Buffer pointer address: %x\n", SmmCommunicationContext.BufferPtrAddress));

#if defined (NVRAM_VERSION) && (NVRAM_VERSION > 6)
  Status = gSmst->SmmLocateProtocol(&gEfiSmmVariableProtocolGuid, NULL, (VOID**)&SmmVariable);

  if (Status == EFI_SUCCESS)
  {
      OldBufferPtrAddress = 0;
      Status = SmmVariable->SmmGetVariable(
                  gAmiAcpiTableAddrS3,
                  &gAmiAcpiSmmCommTableGuid,
                  &Attribute,
                  &VariableSize,
                  &OldBufferPtrAddress);

      DEBUG ((DEBUG_INFO, "Old Buffer pointer address: %x\n", OldBufferPtrAddress));
      if (OldBufferPtrAddress != SmmCommunicationContext.BufferPtrAddress)
      {
          Status = SmmVariable->SmmSetVariable(
                      gAmiAcpiTableAddrS3,
                      &gAmiAcpiSmmCommTableGuid,
                      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                      sizeof(EFI_PHYSICAL_ADDRESS),
                      &SmmCommunicationContext.BufferPtrAddress
          );
      }
  }
#else
  
  OldBufferPtrAddress = 0;
  Status = gRT->GetVariable(
              gAmiAcpiTableAddrS3,
              &gAmiAcpiSmmCommTableGuid,
              &Attribute,
              &VariableSize,
              &OldBufferPtrAddress);

  DEBUG ((DEBUG_INFO, "Old Buffer pointer address: %x\n", OldBufferPtrAddress));
  if (OldBufferPtrAddress != SmmCommunicationContext.BufferPtrAddress)
  {
      Status = gRT->SetVariable(
                  gAmiAcpiTableAddrS3,
                  &gAmiAcpiSmmCommTableGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  sizeof(EFI_PHYSICAL_ADDRESS),
                  &SmmCommunicationContext.BufferPtrAddress
      );
  }

#endif

//  SetCommunicationContext ();

  return Status;
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
