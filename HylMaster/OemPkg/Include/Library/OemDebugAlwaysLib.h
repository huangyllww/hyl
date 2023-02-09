#ifndef _OEM_DEBUG_ALWAYS_LIB_H_
#define _OEM_DEBUG_ALWAYS_LIB_H_

//
// Declare bits for Debug mask
//
// For this 1 byte cmos value, its high 3-bit are for boot phase flag, low 4-bit for Debug message mask
#define PEI_DEBUG_ENABLED 0x01
#define DXE_DEBUG_ENABLED 0x02
#define BDS_DEBUG_ENABLED 0x04
//Mizl-20220222-SGPZT#22027 Add Debug Level Lib Support+>
//#define MRC_DEBUG_ENABLED 0x08
#define RTS_DEBUG_ENABLED 0x08
//Mizl-20220222-SGPZT#22027 Add Debug Level Lib Support->
#define PEI_PHASE_FLAG 0x10
#define DXE_PHASE_FLAG 0x20
#define BDS_PHASE_FLAG 0x30

//Mizl-20220222-SGPZT#22027 Add Debug Level Lib Support+>
//#define DEBUG_MODE_PROPERTY_DEBUG_PRINT_ENABLED   (PEI_DEBUG_ENABLED|DXE_DEBUG_ENABLED|BDS_DEBUG_ENABLED|MRC_DEBUG_ENABLED)
#define DEBUG_MODE_PROPERTY_DEBUG_PRINT_ENABLED   (PEI_DEBUG_ENABLED|DXE_DEBUG_ENABLED|BDS_DEBUG_ENABLED|RTS_DEBUG_ENABLED)
//Mizl-20220222-SGPZT#22027 Add Debug Level Lib Support->

#define DEBUG_MODE_ENABLED_MASK                   DEBUG_MODE_PROPERTY_DEBUG_PRINT_ENABLED
#define DEBUG_MODE_PHASE_MASK                     0x70
#define DEBUG_MODE_VALUE_UPDATE_FLAG              0x80

typedef enum {
  DebugMsg        = 0,
  RcMsg
} MsgType;

UINT8 GetDebugFlag();
UINT8 GetDebugFlagEx();
VOID SetDebugFlag(UINT8 DebugValue,UINT8 Mask8);

BOOLEAN
EFIAPI
DebugPrintEnabledEx (
  UINT8 MsgType
  );

//<Wangyia001-20171128 Mantis 17453: BIOS debug message can not be got by SOL for Baidu +>
UINTN
EFIAPI
SerialPortWriteForIds (
  IN UINT8     *Buffer,
  IN UINTN     NumberOfBytes
  );
//<Wangyia001-20171128 Mantis 17453: BIOS debug message can not be got by SOL for Baidu ->


#endif
