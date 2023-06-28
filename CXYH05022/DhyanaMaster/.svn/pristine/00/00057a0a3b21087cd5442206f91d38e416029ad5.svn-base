#ifndef __ALI_IPMI_SYNC_SETUP_H__
#define __ALI_IPMI_SYNC_SETUP_H__


//#include <Library/UefiLib.h>
//#include <Uefi/UefiSpec.h>



#define TCPA_PPIOP_CLEAR                    5

#define ALI_IPMI_NET_FUN                    0x3E

#define ALI_IPMI_WRITE_BIOS_OPTIONS         0x5C
#define ALI_IPMI_READ_BIOS_OPTIONS          0x5D
#define ALI_IPMI_BIOS_SETCONFIG_TO_BMC      0x5E
#define ALI_IPMI_USER_GETCONFIG_FROM_BIOS   0x5F

#define MAX_SETUP_OPTIONS                   17

#define OEM_IPMI_SETUP_NUM_OF_RETRY         3

#define OEM_NEED_NO_RESET                   0x00
#define OEM_NEED_WARM_RESET                 0x01
#define OEM_NEED_COLD_RESET                 0x02

#define ConvertToIpmiValue(Item)      (Item!=0)?0x81:0x80
#define ReversalConvertToIpmiValue(Item)      (Item!=0)?0x80:0x81

#define ConvertIpmiToNvData(NvData, IpmiData, Flag, NeedReset)      \
                        if (((IpmiData) & 0x80) != NvData)          \
                        {                                           \
                            (NvData) = ((IpmiData) == 0x81)?1:0;    \
                            (Flag)   = 1;                           \
                            *UpdatedSetupFlag |= NeedReset;         \
                        }

#define ReversalConvertIpmiToNvData(NvData, IpmiData, Flag, NeedReset)         \
                        if (((IpmiData) & 0x80) == NvData)          \
                        {                                           \
                            (NvData) = ((IpmiData) == 0x80)?1:0;    \
                            (Flag)   = 1;                           \
                            *UpdatedSetupFlag = NeedReset;          \
                        }

//-----------------------------------------------------------------------------
//**************** OEM IPMI Command-Setup *******************************
//-----------------------------------------------------------------------------
#pragma pack(push)
#pragma	pack(1)

typedef struct {
  UINT8   Index;
  UINT8   Length;
} IPMI_SETUP_READ_REQUEST;

typedef struct {
  //UINT8   CompletionCode; //IPMI v1 without CompletionCode
  UINT8   Length;
  UINT8   SetupValue[MAX_SETUP_OPTIONS];
} IPMI_SETUP_READ_RESPONSE;

typedef struct {
  UINT8   Index;
  UINT8   Length;
  UINT8   SetupValue[MAX_SETUP_OPTIONS];
} IPMI_SETUP_WRITE_REQUEST;

#pragma pack(pop)

#endif // __ALI_IPMI_SYNC_SETUP_H__
