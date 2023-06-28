
#ifndef _AMI_HYGON_ELOG_PROTOCOL_H_
#define _AMI_HYGON_ELOG_PROTOCOL_H_
#include <Token.h> //<Lvshh001-20210510 Optimized Memory Leaky Bucket_3>
extern EFI_GUID gAmiHygonElogProtocolGuid;

typedef struct _AMI_HYGON_ELOG_PROTOCOL AMI_HYGON_ELOG_PROTOCOL;
//<Lvshh001-20210510 Optimized Memory Leaky Bucket_3 +>
#if defined(MEMORY_LEAKY_BUCKET_UPDATE) && (MEMORY_LEAKY_BUCKET_UPDATE == 1)
typedef
EFI_STATUS
(EFIAPI *SEND_ELOG_EVENT_MCA) (
  IN  UINT8     *ErrorRecord,
  IN  DIMM_INFO *DimmInfo,
  IN  UINT8     BankIndex
);
#else//<Lvshh001-20210510 Optimized Memory Leaky Bucket_3 ->
typedef
EFI_STATUS
(EFIAPI *SEND_ELOG_EVENT_MCA) (
  IN  UINT8     *ErrorRecord,
  IN  DIMM_INFO *DimmInfo
);
#endif//<Lvshh001-20210510 Optimized Memory Leaky Bucket_3>

typedef
EFI_STATUS
(EFIAPI *SEND_ELOG_EVENT_NBIO) (
  IN  UINT8     *ErrorRecord
);

typedef
EFI_STATUS
(EFIAPI *SEND_ELOG_EVENT_PCIE) (
  IN  UINT8     *ErrorRecord
);

typedef struct _AMI_HYGON_ELOG_PROTOCOL {
  SEND_ELOG_EVENT_MCA   SendElogEventMca;
  SEND_ELOG_EVENT_NBIO  SendElogEventNbio;
  SEND_ELOG_EVENT_PCIE  SendElogEventPcie;
};

#endif
