#ifndef __AUTO_DXIO_CONFIG_H__
#define __AUTO_DXIO_CONFIG_H__

//#include "Porting.h"

#define MaxStringLen 30
#define FchSmnGpio                      0x02D02500
#define FchSmnRmtGpio                   0x02D02200
#define MAX_GPIO_NUM                    255 // 0xFF
#define GPIO_NUM_DEF(die, gpio)         ((die << 10) + gpio)
#define TOTAL_SSP_REMOTE_GPIO_PIN       (16 + 6)

typedef struct _GPIO_CONFIG
{
  /* data */
  UINT16       GpioNumber;
  UINT8       DieNumber;
  UINT8       SocketNumber;
  UINT8       Validvalue;
//  CHAR8       GpioDescription[MaxStringLen];
}GPIOCONFIG;

typedef struct _PCIE_TOPY_CONFIG
{
  /* data */
  GPIOCONFIG    PortTypeGpioMap[MaxPortGpioNumber];
  UINT8         DieNumber;
  UINT8         StartLane;
  UINT8         EndLane;
  UINT8         LaneType;
  UINT8         OCPHotplugSupport;//<xuyj1202010406 Add ocp hotplug support +>
 // GPIOCONFIG    GpioMap[MaxGpioNumber];
}PCIE_TOPY_CONFIG;

typedef struct _BOARD_PCIE_TOPY_CONFIG
{
  /* data */
  GPIOCONFIG        BoardGpioMap[MaxGpioNumber];
  PCIE_TOPY_CONFIG  PcieTopy[MaxPcieTopoNumber];
}BOARD_PCIE_TOPY_CONFIG;

typedef struct _NVME_CONFIG
{
  /* data */
  GPIOCONFIG    PortTypeGpioMap[MaxPortGpioNumber];
  UINT8         DieNumber;
  UINT8         StartLane;
  UINT8         EndLane;
  UINT8         SlaveAddress;
  UINT8         ByteMap;
  UINT8         ChannelNumber;
  UINT8         SlotNumber;
  //GPIOCONFIG    GpioMap[MaxGpioNumberNVMe];
}NVME_CONFIG;
typedef struct _BOARD_NVME_TOPY_CONFIG
{
  /* data */
    GPIOCONFIG        BoardGpioMap[MaxGpioNumber];
    NVME_CONFIG       NVMETopy[MaxNVMETopoNumber];  
}BOARD_NVME_TOPY_CONFIG;
#define  INVALID_SETTING   0xff
#define  INVALID_GPIO_SETTING   0xffff
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
    PCA9545_Bus3,
    PCA9545No=7
} PCA9545_CHANNEL_NUMBER;


typedef struct _SLOT_DESCRIPTION{
  UINT8   DieNumber;
  UINT8   RootDevice;
  UINT8   RootFunction;
  CHAR8   RootLabel[64];
  UINT8   DownStreamDevice;
  UINT8   DownStreamDevice1;
  UINT8   SlotNumber;
}SLOT_DESCRIPTION;

typedef struct _BOARD_SLOT_DESCRIPTION{
    UINT8               BoardGpioNumber;
    SLOT_DESCRIPTION    SlotDesc[20];
}BOARD_SLOT_DESCRIPTION;

#endif

