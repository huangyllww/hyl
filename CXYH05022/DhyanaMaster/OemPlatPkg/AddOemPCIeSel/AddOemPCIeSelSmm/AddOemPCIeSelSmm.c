#include "Token.h"

#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Include/IpmiNetFnStorageDefinitions.h>

#include <Protocol/IpmiTransportProtocol.h>
#include <Protocol/RedirElogProtocol.h>

#include "OemIpmiNetFnDefinitions.h"
#include "Library/PCIeRegAccessLib.h"
#include <Include/BmcElog.h>
#include "AMD.h"
#include "Library/PciLib.h"
#include <Library/SmmServicesTableLib.h>

//----------------------------------------------------------------------

//----------------------------------------------------------------------

//
// Module Global variables
//
EFI_BMC_ELOG_INSTANCE_DATA      *gRedirProtoPrivate = NULL;
EFI_IPMI_TRANSPORT              *gIpmiTransport = NULL;



EFI_STATUS
EFIAPI
SetOEMSelLogForPCIeError(
    IN EFI_SM_ELOG_REDIR_PROTOCOL *This,
    IN UINT8 *ElogData,
    IN EFI_SM_ELOG_REDIR_TYPE DataType,
    IN BOOLEAN AlertEvent,
    IN UINTN Size,
    OUT UINT64 *RecordId)
{
  EFI_BMC_ELOG_INSTANCE_DATA *BmcElogPrivateData;
  OEM_SEL_EPSR_E0_REQUEST EventMessage;

  UINT8 ResponseDataSize;
  UINT8 ResponseData[2] = {0};
  EFI_STATUS Status;
  UINT16 AerCapPtr;
  PCI_ADDR PciPortAddr;
  BmcElogPrivateData = INSTANCE_FROM_EFI_SM_ELOG_REDIR_THIS(This);

  DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]\r\n",__FUNCTION__,__LINE__));
  if (BmcElogPrivateData->DataType != DataType)
  {
    return EFI_NOT_FOUND;
  }

  if (Size > sizeof(EventMessage))
  {
    SERVER_IPMI_DEBUG((EFI_D_ERROR, " Request data field length limit exceeded. Status: EFI_UNSUPPORTED \n"));
    return EFI_UNSUPPORTED;
  }

  CopyMem((UINT8 *)&EventMessage, ElogData, sizeof(OEM_SEL_EPSR_E0_REQUEST));
  ResponseDataSize = 0;

  //Update PCIe Error Status
  PciPortAddr.AddressValue = 0;
  PciPortAddr.Address.Bus = EventMessage.ERPD.Field.Bus;
  PciPortAddr.Address.Device = EventMessage.ERPD.Field.Device;
  PciPortAddr.Address.Function = EventMessage.ERPD.Field.Function;

  AerCapPtr = OemFindPcieExtendedCapability(PciPortAddr.AddressValue, PCIE_EXT_AER_CAP_ID);

  if (EventMessage.ERTP.Field.ErrorType != 0)
    EventMessage.ErrorStatus = PciRead32(PciPortAddr.AddressValue + AerCapPtr + PCIE_UNCORR_STATUS_PTR);
  else
    EventMessage.ErrorStatus = PciRead32(PciPortAddr.AddressValue + AerCapPtr + PCIE_CORR_STATUS_PTR);

  EventMessage.ERTP.Field.ERPhase =1;//Runtime
  
  DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]BDF:%02x:%02x.%02x\r\n",__FUNCTION__,__LINE__,EventMessage.ERPD.Field.Bus,EventMessage.ERPD.Field.Device,EventMessage.ERPD.Field.Function));
  DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]ERPhase:%x ErrorType:%x\r\n",__FUNCTION__,__LINE__,EventMessage.ERTP.Field.ERPhase,EventMessage.ERTP.Field.ErrorType));
  DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]ErrorStatus:%x\r\n",__FUNCTION__,__LINE__,EventMessage.ErrorStatus));

  
  Status = gIpmiTransport->SendIpmiCommand(
      gIpmiTransport,
      IPMI_NETFN_STORAGE,
      BMC_LUN,
      IPMI_STORAGE_ADD_SEL_ENTRY,
      (UINT8 *)&EventMessage,
      sizeof(OEM_SEL_EPSR_E0_REQUEST),
      NULL,
      (UINT8 *)&ResponseDataSize);
  SERVER_IPMI_DEBUG((EFI_D_INFO, " IPMI_STORAGE_ADD_SEL_ENTRY Status: %r \n", Status));

  return Status;
}

//@todo  Unsupportted


EFI_STATUS
EFIAPI
ActivateOEMSelLogForPCIeError(
    IN EFI_SM_ELOG_REDIR_PROTOCOL *This,
    IN EFI_SM_ELOG_REDIR_TYPE DataType,
    IN BOOLEAN *EnableElog,
    OUT BOOLEAN *ElogStatus)
{

  DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]\r\n",__FUNCTION__,__LINE__));
  if(EfiElogRedirSmOEM !=DataType)
    return EFI_NOT_FOUND;
    
  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
GetOEMSelLogForPCIeError(
    IN EFI_SM_ELOG_REDIR_PROTOCOL *This,
    IN UINT8 *ElogData,
    IN EFI_SM_ELOG_REDIR_TYPE DataType,
    IN OUT UINTN *Size,
    IN OUT UINT64 *RecordId)
{

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
EraseOEMSelLogForPCIeError(
    IN EFI_SM_ELOG_REDIR_PROTOCOL *This,
    IN EFI_SM_ELOG_REDIR_TYPE DataType,
    IN OUT UINT64 *RecordId)
{
  return EFI_SUCCESS;
}

EFI_STATUS
InitializeSmmOEMSelLog(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable)
{

  EFI_HANDLE NewHandle;
  EFI_STATUS Status;
  IPMI_GET_DEVICE_ID_RESPONSE GetDeviceIdResponse = {0};
  UINT8 GetDeviceIdResponseSize = sizeof(GetDeviceIdResponse);
  BOOLEAN EnableElog = TRUE;
  BOOLEAN ElogStatus = TRUE;

  SERVER_IPMI_DEBUG((EFI_D_INFO, "%a Entry... \n", __FUNCTION__));

  Status = gSmst->SmmLocateProtocol(
      &gEfiSmmIpmiTransportProtocolGuid,
      NULL,
      (VOID **)&gIpmiTransport);
  SERVER_IPMI_DEBUG((EFI_D_INFO, " gEfiSmmIpmiTransportProtocolGuid Status: %r \n", Status));
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "Couldn't find SMM IPMI TRANSPORT protocol: %r\n", Status));
    return Status;
  }

  //
  // Get the device ID information from the BMC.
  //
  Status = gIpmiTransport->SendIpmiCommand(
      gIpmiTransport,
      IPMI_NETFN_APP,
      BMC_LUN,
      IPMI_APP_GET_DEVICE_ID,
      NULL,
      0,
      (UINT8 *)&GetDeviceIdResponse,
      (UINT8 *)&GetDeviceIdResponseSize);
  SERVER_IPMI_DEBUG((EFI_D_INFO, "Get Device ID status %r\n", Status));
  if (EFI_ERROR(Status))
  {
    return Status;
  }

  SERVER_IPMI_DEBUG((EFI_D_INFO, "GetDeviceIdResponse.SelDeviceSupport %X\n", GetDeviceIdResponse.SelDeviceSupport));
  if (!GetDeviceIdResponse.SelDeviceSupport)
  {
    SERVER_IPMI_DEBUG((EFI_D_ERROR, "SEL device is not supported. So returning EFI_UNSUPPORTED \n"));
    return EFI_UNSUPPORTED;
  }

  SERVER_IPMI_DEBUG((EFI_D_INFO, "GetDeviceIdResponse.SpecificationVersion %X\n", GetDeviceIdResponse.SpecificationVersion));
  if (!((GetDeviceIdResponse.SpecificationVersion == IPMI_SPEC_VERSION_1_5) ||
        (GetDeviceIdResponse.SpecificationVersion == IPMI_SPEC_VERSION_2_0)))
  {
    SERVER_IPMI_DEBUG((EFI_D_ERROR, "IPMI Spec version is not supported. So returning EFI_UNSUPPORTED \n"));
    return EFI_UNSUPPORTED;
  }
  //
  // Allocate memory for Redir Protocol private data
  //
  gRedirProtoPrivate = AllocateRuntimeZeroPool(sizeof(EFI_BMC_ELOG_INSTANCE_DATA));
  if (gRedirProtoPrivate == NULL)
  {
    SERVER_IPMI_DEBUG((EFI_D_ERROR, "Failed to allocate memory for gRedirProtoPrivate! \n"));
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Fill the Redir Protocol data
  //
  gRedirProtoPrivate->Signature = SM_ELOG_REDIR_SIGNATURE;
  gRedirProtoPrivate->DataType = EfiElogRedirSmOEM;
  gRedirProtoPrivate->BmcElog.ActivateEventLog = ActivateOEMSelLogForPCIeError;
  gRedirProtoPrivate->BmcElog.EraseEventLogData = EraseOEMSelLogForPCIeError;
  gRedirProtoPrivate->BmcElog.GetEventLogData = GetOEMSelLogForPCIeError;
  gRedirProtoPrivate->BmcElog.SetEventLogData = SetOEMSelLogForPCIeError;

  //
  // Install the protocol
  //
  NewHandle = NULL;
  Status = gSmst->SmmInstallProtocolInterface(
      &NewHandle,
      &gEfiRedirElogProtocolGuid,
      EFI_NATIVE_INTERFACE,
      &gRedirProtoPrivate->BmcElog);
  SERVER_IPMI_DEBUG((EFI_D_INFO, "gEfiRedirElogProtocolGuid protocol  status %r\n", Status));
  if (EFI_ERROR(Status))
  {
    //
    // Error in installing the protocol. So free the allocated memory.
    //
    FreePool(gRedirProtoPrivate);
    return Status;
  }

  SERVER_IPMI_DEBUG((EFI_D_INFO, "%a Exiting... \n", __FUNCTION__));

  return EFI_SUCCESS;
}
