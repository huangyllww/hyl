#ifndef __SETUP_CONTROL_H__
#define __SETUP_CONTROL_H__

#include <AMIVfr.h>

//Mizl-20200616-Mantis 0040303: USB 2.0 Control Item Works Abnormal +>
#define USB_PORT_DISABLE 0
//USB3 CONTAINER Registers
#define FCH_SMN_USB_CONTAINER 0x16D80000ul
#define FCH_TS_USB_PORT_DISABLE0 FCH_SMN_USB_CONTAINER + 0x128 //0x128

typedef struct
{
    BOOLEAN UsedFlag;
    CHAR16 PortDescription[256];
    CHAR16 PortHelpDescription[256];
    EFI_STRING_ID   StringID;
    EFI_STRING_ID   HelpStringID;
} USB_PORT;

typedef struct
{
    UINT8 CPUUsbPortLocation;
    USB_PORT UsbPort[1];
} USB_PORT_CONTROL;

//7 Serial CPU Only Support 2 USB Ports
//CPU0_DIE1_Port2 ->CPU0_DIE1_Port0
//CPU0_DIE1_Port3 ->CPU0_DIE1_Port1
typedef enum
{
    CPU0_DIE0_Port0,
    CPU0_DIE0_Port1,
    CPU0_DIE0_Port2,
    CPU0_DIE0_Port3,
    CPU0_DIE1_Port0,//CPU0_DIE1_Port2
    CPU0_DIE1_Port1,//CPU0_DIE1_Port3
    CPU0_DIE1_Port2,
    CPU0_DIE1_Port3,
    PortTerminate,
} USB_PORT_LOCATION;

typedef struct
{
    UINT8 Socket;
    UINT8 Die;
    UINT8 Port;
} CPU_USB_MAP;

//7 Serial CPU Only Support 2 USB Ports
//CPU0_DIE1_Port2 ->CPU0_DIE1_Port0
//CPU0_DIE1_Port3 ->CPU0_DIE1_Port1
CPU_USB_MAP gaCPUUsbMap[] = {
    {0, 0, 0}, 
    {0, 0, 1}, 
    {0, 0, 2}, 
    {0, 0, 3}, 
    {0, 1, 0}, 
    {0, 1, 1}, 
    {0, 1, 0}, 
    {0, 1, 1}, 
    {0xFF, 0, 0}};

#define USB_PORT_OFFSET(Socket, Die, PortCount) CPU##Socket##_DIE##Die##_Port##PortCount

#endif