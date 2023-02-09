
#include "OemDmiInfo.h"


EFI_IPMI_TRANSPORT  		*mIpmiTransport;


EFI_STATUS
OemIpmiSetUUIDInfo(EFI_GUID *UuidBuf)
{
  EFI_STATUS            Status; 
  UINT8                 ResponseData[16];
  UINT8                 ResponseDataSize;
  

  ResponseDataSize = sizeof(ResponseData);
  Status = pBS->LocateProtocol (
                        &gEfiDxeIpmiTransportProtocolGuid,
                        NULL,
                        &mIpmiTransport
                        );
  if ( EFI_ERROR (Status) ) {
    return Status;
  }
  //TRACE((-1, "Before Set UUID:%g %r\n",*UuidBuf, Status));
  Status = mIpmiTransport->SendIpmiCommand (
                        mIpmiTransport,	
                        NETFN_OEM, 
                        BMC_LUN,        
                        SET_SYS_UUID,   
                        (UINT8 *)UuidBuf,
                        sizeof(EFI_GUID),
                        ResponseData,
                        &ResponseDataSize
                        );
  
  if (EFI_ERROR (Status)) {
    return Status;
  }
  pBS->Stall(30000);
  TRACE((-1, "IPMI Set UUID: %r\n", Status));
  
  return EFI_SUCCESS;
}

EFI_STATUS
OemDmiInfoEntryPoint (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
  )
{
    EFI_STATUS  	      Status = EFI_SUCCESS;
    EFI_GUID            UuidBuf = {0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff};

    InitAmiLib(ImageHandle, SystemTable);
    
    Status = NetLibGetSystemGuid(&UuidBuf);
    TRACE((-1, "Get System UUID: %g %r\n", UuidBuf, Status));

    Status = OemIpmiSetUUIDInfo(&UuidBuf);

    return  Status;
}     

EFI_STATUS
EFIAPI
NetLibGetSystemGuid (
  OUT EFI_GUID              *SystemGuid
  )
{
  EFI_STATUS                Status;
  SMBIOS_TABLE_ENTRY_POINT  *SmbiosTable;
  SMBIOS_STRUCTURE_POINTER  Smbios;
  SMBIOS_STRUCTURE_POINTER  SmbiosEnd;
  CHAR8                     *String;
  AMI_SMBIOS_PROTOCOL       *SmbiosProtocol;

  SmbiosTable = NULL;
  
  Status = pBS->LocateProtocol (&gAmiSmbiosProtocolGuid, NULL, &SmbiosProtocol);
  //ASSERT_EFI_ERROR(Status);
  if(EFI_ERROR(Status)) {
      return Status;
  }

  SmbiosTable = SmbiosProtocol->SmbiosGetTableEntryPoint();
  if (SmbiosTable == NULL) {
    return EFI_NOT_FOUND;
  }

  Smbios.Hdr    = (SMBIOS_STRUCTURE *) (UINTN) SmbiosTable->TableAddress;
  SmbiosEnd.Raw = (UINT8 *) (UINTN) (SmbiosTable->TableAddress + SmbiosTable->TableLength);

  do {
    if (Smbios.Hdr->Type == 1) {
      if (Smbios.Hdr->Length < 0x19) {
        return EFI_NOT_FOUND;
      }
      CopyMem (SystemGuid, &Smbios.Type1->Uuid, sizeof (EFI_GUID));
      return EFI_SUCCESS;
    }

    String = (CHAR8 *) (Smbios.Raw + Smbios.Hdr->Length);
    do {
      for ( ; *String != 0; String++) {
      }

      if (*(UINT8*)++String == 0) {
        Smbios.Raw = (UINT8 *)++String;
        break;
      }    
    } while (TRUE);
  } while (Smbios.Raw < SmbiosEnd.Raw);
  return EFI_NOT_FOUND;
}

