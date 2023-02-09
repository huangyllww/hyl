/** @file
  Driver source for Oem PEI module of OemIpmiPei.

  Copyright (C) 2018 Dawning Information Industry Co.,LTD.All rights reserved.<BR>

**/

#include <Token.h>
#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/IpmiTransportPpi.h>
#include <OemIpmiNetFnDefinitions.h>
#include <IndustryStandard/IpmiNetFnStorage.h>
//<Kangmm-20201123 Add Reset SEL report +>
#include <Library/IoLib.h>
//<Kangmm-20201123 Add Reset SEL report ->
static
EFI_STATUS
EFIAPI
OemPeiIpmiCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

static EFI_PEI_NOTIFY_DESCRIPTOR mNotifyOnPeiIpmiList = {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiIpmiTransportPpiGuid,
    OemPeiIpmiCallback
};

#if defined(OEM_CLEAR_ASSET_INFO_SUPPORT) && (OEM_CLEAR_ASSET_INFO_SUPPORT == 1)
EFI_STATUS
OemCearAssetInfo(
  IN PEI_IPMI_TRANSPORT_PPI  *IpmiTransportPpi
)
{
  EFI_STATUS              Status;
  UINT8                   DataSize;
  UINT8                   Result;
  
  if(IpmiTransportPpi == NULL)
    return EFI_NOT_FOUND;
  
  DataSize = sizeof(Result);
  Status = IpmiTransportPpi->SendIpmiCommand (
            IpmiTransportPpi,
            NETFN_OEM,                // raw 0x3A 0xED
            BMC_LUN,
            0xED,                     // CMD_OEM_CLR_ASSET_INFO
            NULL,
            0,
            &Result,
            &DataSize );
  DEBUG((EFI_D_INFO, "Clear Asset Info, status = %r\n", Status));
  
  return Status;
}
#endif
//<Kangmm-20201123 Add Reset SEL report +>
#if defined(SYSTEM_RESET_SEL_SUPPORT) && (SYSTEM_RESET_SEL_SUPPORT == 1)
EFI_STATUS
SystemResetSel(
    IN PEI_IPMI_TRANSPORT_PPI  *IpmiTransportPpi
)
{
  EFI_STATUS                                    Status = EFI_SUCCESS;
  IPMI_SEL_EVENT_RECORD_DATA                    SelRecordData;
  UINT8                                         ResponseData[10];
  UINT8                                         ResponseDataSize = 10;
  
  SelRecordData.OEMEvData1 = 0x7; 
  IoWrite8(0xcd6, 0xc2); //S5ResetStatus bit 16-23
  SelRecordData.OEMEvData2 = IoRead8(0xcd7);  
  IoWrite8(0xcd6, 0xc3); //S5ResetStatus bit 24-31
  SelRecordData.OEMEvData3 = IoRead8(0xcd7);
  DEBUG((EFI_D_INFO, "S5ResetStatus Bit24-31 = 0x%x,Bit16-23 = 0x%x\n", SelRecordData.OEMEvData3,SelRecordData.OEMEvData2));

  SelRecordData.RecordId = 0x00;
  SelRecordData.RecordType = 0x02;   // Discrete
  SelRecordData.TimeStamp = 0x00;
  SelRecordData.GeneratorId = 0x21;
  SelRecordData.EvMRevision = 0x04;
  SelRecordData.SensorType = 0x1D;
  SelRecordData.SensorNumber = 0x00;
  SelRecordData.EventDirType = 0x6F;
  ResponseDataSize = sizeof (ResponseData);


  Status = IpmiTransportPpi->SendIpmiCommand(
                              IpmiTransportPpi,
                              IPMI_NETFN_STORAGE,
                              BMC_LUN,
                              IPMI_STORAGE_ADD_SEL_ENTRY,
                              (UINT8 *) &SelRecordData,
                              sizeof (SelRecordData),
                              (UINT8 *) &ResponseData,
                              (UINT8 *)&ResponseDataSize 
                              );
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_INFO, "[OemDbg]: Sync with S5ResetStat Register Fail\n"));
    return Status;
  }
  return EFI_SUCCESS;
}
#endif
//<Kangmm-20201123 Add Reset SEL report ->
/**
  Entry point of the notification callback function itself within the PEIM.
  Send OEM IPMI command to clear BMC Asset Info.

  @param  PeiServices       Indirect reference to the PEI Services Table.
  @param  NotifyDescriptor  Address of the notification descriptor data structure.
  @param  Ppi               Address of the PPI that was installed.

  @return Status of the notification.
          The status code returned from this function is ignored.
          
          EFI_SUCCESS       If the IPMI command execute successfully.
**/
static
EFI_STATUS
EFIAPI
OemPeiIpmiCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS              Status;
  PEI_IPMI_TRANSPORT_PPI  *IpmiTransportPpi;
  DEBUG((EFI_D_INFO, "OemPeiIpmiCallback Entering\n"));

  //
  //Locate IPMI Transport protocol to send commands to BMC.
  //
  Status = (*PeiServices)->LocatePpi (
            (CONST EFI_PEI_SERVICES**)PeiServices,
            &gEfiPeiIpmiTransportPpiGuid,
            0,
            NULL,
            (VOID **)&IpmiTransportPpi );
  if (EFI_ERROR(Status)) {
      return Status;
  }
  
#if defined(OEM_CLEAR_ASSET_INFO_SUPPORT) && (OEM_CLEAR_ASSET_INFO_SUPPORT == 1)
  Status = OemCearAssetInfo(IpmiTransportPpi);
#endif
//<Kangmm-20201123 Add Reset SEL report +>
#if defined(SYSTEM_RESET_SEL_SUPPORT) && (SYSTEM_RESET_SEL_SUPPORT == 1)
  Status = SystemResetSel(IpmiTransportPpi);
#endif
//<Kangmm-20201123 Add Reset SEL report ->  
  return Status;
}

/**
  Entry point function for the PEIM

  @param FileHandle      Handle of the file being invoked.
  @param PeiServices     Describes the list of possible PEI Services.

  @return EFI_SUCCESS    If we installed our PPI

**/
EFI_STATUS
EFIAPI
OemIpmiPeiEntry (
    IN       EFI_PEI_FILE_HANDLE  FileHandle,
    IN CONST EFI_PEI_SERVICES     **PeiServices
)
{
  EFI_STATUS  Status;
  
  Status = PeiServicesNotifyPpi(&mNotifyOnPeiIpmiList);
  
  return Status;
}
