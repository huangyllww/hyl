#ifndef __OEM_SYNC_DEBUG_MODE_H__
#define __OEM_SYNC_DEBUG_MODE_H__

#include "Token.h"


#define OEM_IPMI_SETUP_REQUEST_LENGTH      20
#define OEM_IPMI_SETUP_RESPONSE_LENGTH     20


#define CheckResponseStatus(Status,DataValidFlag)\
          if(EFI_ERROR (Status))\
            MODULE_END(Status)\
          \
          if(DataValidFlag == 0)\
            MODULE_END(Status = EFI_UNSUPPORTED)

#define DumpResponseData(ReponseData)\
  {\
    UINT8 iMod = 0;\
    for (iMod = 0; iMod < OEM_IPMI_SETUP_RESPONSE_LENGTH; iMod++)\
      DEBUG((EFI_D_ERROR, "%02x ", ReponseData[iMod]));\
    DEBUG((EFI_D_ERROR, "\n"));\
  }
  
#pragma pack(push, 1)

typedef union {
    
  UINT8  Value;
  struct {
    UINT8 PeiDebugSupport:1;            
    UINT8 DxeDebugSupport:1;           
    UINT8 RTSDebugSupport:1;             
    UINT8 :4;//Reserved_BIT3_BIT6          
    UINT8 Byte4ValidFlag:1;       
  }Bits;
} BMC_DEBUG_OPTION_DEBUG_MODE_BYTE4;


typedef union {
  struct {                          
    UINT8 :1;//Reserved Bit0
    UINT8 WarnningDebugSupport:1;       
    UINT8 :4;//Reserved Bit2~Bit5    
    UINT8 InfoDebugSupport:1;           
    UINT8 Byte8ValidFlag:1;//Reserved_BIT3_BIT6      
  }Bits;
  UINT8  Value;
} BMC_DEBUG_OPTION_DEBUG_LEVEL_OFFSET_BYTE0;



typedef union {
  struct {                          
    UINT8 :3;//Reserved Bit0~Bit2
    UINT8 ErrorDebugSupport:1;     
    UINT8 :3;//Reserved Bit4~Bit6  
    UINT8 Byte12ValidFlag:1;
  }Bits;
  UINT8  Value;
} BMC_DEBUG_OPTION_DEBUG_LEVEL_OFFSET_BYTE5;


typedef union {
  struct {                          
    UINT8 DebugMode:1;
    UINT8 :6;       //Reserved Bit2~Bit7
    UINT8 Byte3ValidFlag:1;
  }Bits;
  UINT8  Value;
} BMC_DEBUG_OPTION_DEBUG_SUPPORT;

typedef struct
{
    /* data */
    //Byte 3
    BMC_DEBUG_OPTION_DEBUG_SUPPORT   DebugMode_Byte3;
    //Byte 4~7
    BMC_DEBUG_OPTION_DEBUG_MODE_BYTE4   DebugMode_Byte4;
    UINT8   DebugMode_Byte5_Byte7[3];
    //Byte 8~12
    BMC_DEBUG_OPTION_DEBUG_LEVEL_OFFSET_BYTE0  DebugMode_Byte8;
    UINT8   DebugLevel_Byte9_Byte11[3];
    BMC_DEBUG_OPTION_DEBUG_LEVEL_OFFSET_BYTE5 DebugMode_Byte12;
    //Byte 13~17
    BMC_DEBUG_OPTION_DEBUG_LEVEL_OFFSET_BYTE0  DebugMode_Byte13;
    UINT8   DebugLevel_Byte14_Byte16[3];
    BMC_DEBUG_OPTION_DEBUG_LEVEL_OFFSET_BYTE5 DebugMode_Byte17;
}BMC_DEBUG_OPTION;

#pragma pack(pop)

#endif
