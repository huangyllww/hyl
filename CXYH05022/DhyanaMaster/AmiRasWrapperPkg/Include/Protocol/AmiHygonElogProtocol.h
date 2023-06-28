
#ifndef _AMI_HYGON_ELOG_PROTOCOL_H_
#define _AMI_HYGON_ELOG_PROTOCOL_H_

extern EFI_GUID gAmiHygonElogProtocolGuid;

typedef struct _AMI_HYGON_ELOG_PROTOCOL AMI_HYGON_ELOG_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *SEND_ELOG_EVENT_MCA) (
  IN  UINT8     *ErrorRecord,
  IN  DIMM_INFO *DimmInfo
);

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
