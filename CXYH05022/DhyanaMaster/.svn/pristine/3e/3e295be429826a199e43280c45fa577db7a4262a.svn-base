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

#ifndef _PLATFORM_RAS_IPMI_LIB_H_
#define _PLATFORM_RAS_IPMI_LIB_H_

#include <AmdRas.h>
#include <AmdCpmRas.h>

EFI_STATUS
McaToIpmi(
  IN  RAS_MCA_ERROR_INFO    *McaErrorRecord,
  IN  UINTN                 BankIndex,
  IN  OUT UINT8             *ErrorBuffer,
  IN  DIMM_INFO             *DimmInfo
  );

EFI_STATUS
NbioToIpmi(
  IN  RAS_NBIO_ERROR_INFO   *NbioErrorRecord,
  IN OUT UINT8              *ErrorBuffer
  );

EFI_STATUS
PcieToIpmi(
  IN  GENERIC_PCIE_AER_ERR_ENTRY  *GenPcieAerErrEntry,
  IN OUT UINT8                    *ErrorBuffer
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
