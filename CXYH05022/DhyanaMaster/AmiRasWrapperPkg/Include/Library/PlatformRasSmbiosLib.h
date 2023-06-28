//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

#ifndef _PLATFORM_RAS_SMBIOS_LIB_H_
#define _PLATFORM_RAS_SMBIOS_LIB_H_

EFI_STATUS
McaToSmbios(
  IN  RAS_MCA_ERROR_INFO    *McaErrorRecord,
  IN  UINTN                 BankIndex,
  IN OUT UINT8              *SmbiosErrorBuffer
  );

EFI_STATUS
NbioToSmbios(
  IN  RAS_NBIO_ERROR_INFO   *NbioErrorRecord,
  IN OUT UINT8              *SmbiosErrorBuffer
  );

EFI_STATUS
PcieToSmbios(
  IN GENERIC_PCIE_AER_ERR_ENTRY *GenPcieAerErrEntry,
  IN OUT UINT8                  *SmbiosErrorBuffer
  );

#endif

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
