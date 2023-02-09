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
#include "Token.h"  //huangjin004-20190925 Fix build error with PLATFORM_PER_DIMM_THRESHOLD_SUPPORT enabled

//<Lvshh001-20210510 Optimized Memory Leaky Bucket_3 +>
#if defined(MEMORY_LEAKY_BUCKET_UPDATE) && (MEMORY_LEAKY_BUCKET_UPDATE == 1)
EFI_STATUS
McaToIpmi(
  IN  RAS_MCA_ERROR_INFO    *McaErrorRecord,
  IN  OUT UINT8             *ErrorBuffer,
  IN  DIMM_INFO             *DimmInfo,
  IN    UINT8               BankIndex
  );
#else //<Lvshh001-20210510 Optimized Memory Leaky Bucket_3 ->
EFI_STATUS
McaToIpmi(
  IN  RAS_MCA_ERROR_INFO    *McaErrorRecord,
  IN  UINTN                 BankIndex,
  IN  OUT UINT8             *ErrorBuffer,
  IN  DIMM_INFO             *DimmInfo
  );
#endif //<Lvshh001-20210510 Optimized Memory Leaky Bucket_3>

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
#if defined(PLATFORM_PER_DIMM_THRESHOLD_SUPPORT) && (PLATFORM_PER_DIMM_THRESHOLD_SUPPORT == 1)
#define MAX_CPU_SOCKETS   2
#define MAX_CHANNEL       8
#define MAX_DIMM          2
#define CMOS_INDEX  0x70
#define CMOS_DATA   0x71
#define RTC_STATUS_REG_A    0xA
#define RTC_ADDRESS_SECONDS           0   // R/W  Range 0..59
#define RTC_ADDRESS_MINUTES           2   // R/W  Range 0..59
#define RTC_ADDRESS_HOURS             4   // R/W  Range 1..12 or 0..23 Bit 7 is AM/PM
#define RTC_ADDRESS_DAY_OF_THE_MONTH  7   // R/W  Range 1..31
#define RTC_ADDRESS_MONTH             8   // R/W  Range 1..12
#define RTC_ADDRESS_YEAR              9   // R/W  Range 0..99
typedef struct {
  UINT16  Year;
  UINT8   Month;
  UINT8   Day;
  UINT8   Hour;
  UINT8   Minute;
  UINT8   Second;
} TIME_STAMP;
#endif
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
