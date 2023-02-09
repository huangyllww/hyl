#ifndef __AUTO_DXIO_CONFIG_H__
#define __AUTO_DXIO_CONFIG_H__

#include "Porting.h"

#define MaxStringLen 30

typedef struct _GPIO_CONFIG
{
  /* data */
  UINT8       GpioNumber;
  UINT8       DieNumber;
  UINT8       SocketNumber;
  UINT8       Validvalue;
  CHAR8       GpioDescription[MaxStringLen];
}GPIOCONFIG;

typedef struct _PCIE_TOPY_CONFIG
{
  /* data */
  UINT8         DieNumber;
  UINT8         StartLane;
  UINT8         EndLane;
  UINT8         LaneType;
  UINT8         PreFixedDev;
  UINT8         PreFixedFun;
  GPIOCONFIG    GpioMap[MaxGpioNumber];
}PCIE_TOPY_CONFIG;

typedef struct _NVME_CONFIG
{
  /* data */
  UINT8         DieNumber;
  UINT8         StartLane;
  UINT8         EndLane;
  UINT8         SlaveAddress;
  UINT8         ByteMap;
  UINT8         ChannelNumber;
  GPIOCONFIG    GpioMap[MaxGpioNumberNVMe];
}NVME_CONFIG;

#define  INVALID_SETTING   0xff
#define   VALID_PORT 0x00

typedef  enum{
  DIE0,
  DIE1,
  DIE2,
  DIE3,
  DIE4,
  DIE5,
  DIE6,
  DIE7
}DIE_NUMBER;

typedef enum{
  SOCKET0,
  SOCKET1
}SOCKET_NUMBER;

typedef enum{
  InputLow,
  InputHigh
}GPIO_STATUS;

//#mizl-20201014-Update For Signle CPU NVMe Config +>
typedef enum {
    PCA9545_Bus0 = 0,
    PCA9545_Bus1,
    PCA9545_Bus2,
    PCA9545_Bus3
} PCA9545_CHANNEL_NUMBER;

//Hugc-20211222-Add Riser/NVMe control function >>>
#define OEM_NVME_CONFIG_LIST_HOB_GUID \
        {0xdf3fe840, 0xae7a, 0x44d2, 0xbe, 0x54, 0x93, 0x80, 0xbc, 0x9b, 0xa4, 0x13}


typedef struct {
  EFI_HOB_GUID_TYPE   Header;
  NVME_CONFIG         NVMeConfigList[0x30];
} OEM_NVME_CONFIG_LIST_HOB;

//Hugc-20211222-Add Riser/NVMe control function<<<


#endif