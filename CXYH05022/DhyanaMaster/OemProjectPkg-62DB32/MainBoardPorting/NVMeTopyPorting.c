#include "Porting.h"
#include "AmdPcieComplex.h"
#include "AutoDxioConfig.h"
#include "Token.h"
#include "AmdServerHotplug.h" //#mizl-20201014-Update For Signle CPU NVMe Config +>

#define INVALID_SETTING_ARRARY INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,"INVALID_SETTING"
//AGPIO10_3
#define RISERA_TYPE0_N(ValidSeeting) 10,DIE3,SOCKET0,ValidSeeting,"RISERA_TYPE0_N"
//AGPIO40_3
#define RISERA_TYPE1_N(ValidSeeting) 40,DIE3,SOCKET0,ValidSeeting,"RISERA_TYPE1_N"
//AGPIO23_3
#define RISERA_NVME1_DETECT_N(ValidSeeting) 23,DIE3,SOCKET0,ValidSeeting,"RISERA_NVME1_DETECT_N" 
//AGPIO23_3
#define RISERB_NVME1_DETECT_N(ValidSeeting) 23,DIE3,SOCKET1,ValidSeeting,"RISERB_NVME1_DETECT_N"
//AGPIO9_3
#define RISERB_NVME0_DETECT_N(ValidSeeting) 9,DIE3,SOCKET1,ValidSeeting,"RISERB_NVME0_DETECT_N" 
//AGPIO6
#define RISERC_NVME_DETECT_N(ValidSeeting) 6,DIE0,SOCKET1,ValidSeeting,"RISERC_NVME_DETECT_N" 

NVME_CONFIG    gNVMeConfigList_DualCPUInstalled[]={
  //{DieNumber,StartLane,EndLane,SlaveAddress,ByteMap,ChannelNumber}
  //Socket 0 Die1
  //Remove For V1.4 {DIE1,0,3,0x44,Low8BitGpio,PCA9545_Bus0,RISERA_TYPE0_N(InputHigh),RISERA_TYPE1_N(InputHigh)},
  //Remove For V1.4 {DIE1,4,7,0x44,High8BitGpio,PCA9545_Bus0,RISERA_TYPE0_N(InputHigh),RISERA_TYPE1_N(InputHigh)},
  //Socket 0 Die2
  {DIE2,0,3,0x40,Low8BitGpio,PCA9545_Bus0,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE2,4,7,0x40,High8BitGpio,PCA9545_Bus0,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE2,8,11,0x42,Low8BitGpio,PCA9545_Bus0,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE2,12,15,0x42,High8BitGpio,PCA9545_Bus0,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  //Socket 0 Die3
  {DIE3,0,3,0x44,High8BitGpio,PCA9545_Bus0,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE3,4,7,0x44,Low8BitGpio,PCA9545_Bus0,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  // {DIE3,8,11,0x44,High8BitGpio,PCA9545_Bus0,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  // {DIE3,12,15,0x44,Low8BitGpio,PCA9545_Bus0,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE3,8,11,0x46,High8BitGpio,PCA9545_Bus0,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE3,12,15,0x46,Low8BitGpio,PCA9545_Bus0,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  
  //Remove For V1.4 {DIE3,8,11,0x44,High8BitGpio,PCA9545_Bus0,RISERA_TYPE0_N(InputLow),RISERA_TYPE1_N(InputHigh)},
  //Remove For V1.4 {DIE3,12,15,0x44,Low8BitGpio,PCA9545_Bus0,RISERA_TYPE0_N(InputLow),RISERA_TYPE1_N(InputHigh)},
  
  //Remove For V1.4 {DIE3,8,11,0x44,High8BitGpio,PCA9545_Bus0,RISERA_TYPE0_N(InputHigh),RISERA_TYPE1_N(InputLow)},
  //Remove For V1.4 {DIE3,12,15,0x44,Low8BitGpio,PCA9545_Bus0,RISERA_TYPE0_N(InputHigh),RISERA_TYPE1_N(InputLow)},
  //
  //Socket 1 Die0
  {DIE4,0,3,0x40,Low8BitGpio,PCA9545_Bus1,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE4,4,7,0x40,High8BitGpio,PCA9545_Bus1,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE4,8,11,0x42,High8BitGpio,PCA9545_Bus1,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE4,12,15,0x42,Low8BitGpio,PCA9545_Bus1,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  
  //Socket 1 Die1
  {DIE5,0,3,0x44,Low8BitGpio,PCA9545_Bus1,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE5,4,7,0x44,High8BitGpio,PCA9545_Bus1,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE5,8,11,0x46,Low8BitGpio,PCA9545_Bus1,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE5,12,15,0x46,High8BitGpio,PCA9545_Bus1,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},

  //Socket 1 Die2
  {DIE6,0,3,0x48,High8BitGpio,PCA9545_Bus1,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE6,4,7,0x48,Low8BitGpio,PCA9545_Bus1,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE6,8,11,0x4a,High8BitGpio,PCA9545_Bus1,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE6,12,15,0x4a,Low8BitGpio,PCA9545_Bus1,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  //Socket 1 Die3
  {DIE7,0,3,0x4c,High8BitGpio,PCA9545_Bus1,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE7,4,7,0x4c,Low8BitGpio,PCA9545_Bus1,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE7,8,11,0x4e,High8BitGpio,PCA9545_Bus1,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE7,12,15,0x4e,Low8BitGpio,PCA9545_Bus1,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  //
  {INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},

};

//#mizl-20201014-Update For Signle CPU NVMe Config +>
NVME_CONFIG    gNVMeConfigList_SingleCPUInstalled[]={
  //Socket0 Die0
  {DIE0,8,11,0x48,High8BitGpio,PCA9545_Bus0,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE0,12,15,0x48,Low8BitGpio,PCA9545_Bus0,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  //Socket0 Die1
  {DIE1,0,3,0x4A,Low8BitGpio,PCA9545_Bus0,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE1,4,7,0x4A,High8BitGpio,PCA9545_Bus0,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},

  //Remove For V1.4{DIE1,0,3,0x44,Low8BitGpio,PCA9545_Bus0,RISERA_TYPE0_N(InputLow),RISERA_TYPE1_N(InputHigh)},
  //Remove For V1.4{DIE1,4,7,0x44,High8BitGpio,PCA9545_Bus0,RISERA_TYPE0_N(InputLow),RISERA_TYPE1_N(InputHigh)},

  //Remove For V1.4{DIE1,0,3,0x44,Low8BitGpio,PCA9545_Bus0,RISERA_TYPE0_N(InputHigh),RISERA_TYPE1_N(InputLow)},
  //Remove For V1.4{DIE1,4,7,0x44,High8BitGpio,PCA9545_Bus0,RISERA_TYPE0_N(InputHigh),RISERA_TYPE1_N(InputLow)},

  //Remove For V1.4{DIE1,0,3,0x40,Low8BitGpio,PCA9545_Bus0,RISERA_TYPE0_N(InputHigh),RISERA_TYPE1_N(InputHigh)},
  //Remove For V1.4{DIE1,4,7,0x40,High8BitGpio,PCA9545_Bus0,RISERA_TYPE0_N(InputHigh),RISERA_TYPE1_N(InputHigh)},
  //Socket0 Die2
  {DIE2,0,3,0x40,Low8BitGpio,PCA9545_Bus0,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE2,4,7,0x40,High8BitGpio,PCA9545_Bus0,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  
  {DIE2,8,11,0x42,Low8BitGpio,PCA9545_Bus0,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE2,12,15,0x42,High8BitGpio,PCA9545_Bus0,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},

  //Remove For V1.4{DIE2,8,11,0x42,Low8BitGpio,PCA9545_Bus0,RISERA_TYPE0_N(InputLow),RISERA_TYPE1_N(InputHigh)},
  //Remove For V1.4{DIE2,12,15,0x42,High8BitGpio,PCA9545_Bus0,RISERA_TYPE0_N(InputLow),RISERA_TYPE1_N(InputHigh)},

  //Remove For V1.4{DIE2,8,11,0x42,High8BitGpio,PCA9545_Bus0,RISERA_TYPE0_N(InputHigh),RISERA_TYPE1_N(InputHigh)},
  //Remove For V1.4{DIE2,12,15,0x42,Low8BitGpio,PCA9545_Bus0,RISERA_TYPE0_N(InputHigh),RISERA_TYPE1_N(InputHigh)},

  //Remove For V1.4{DIE2,8,11,0x42,Low8BitGpio,PCA9545_Bus0,RISERA_TYPE0_N(InputHigh),RISERA_TYPE1_N(InputLow)},
  //Remove For V1.4{DIE2,12,15,0x42,High8BitGpio,PCA9545_Bus0,RISERA_TYPE0_N(InputHigh),RISERA_TYPE1_N(InputLow)},
  //Socket0 Die3
  {DIE3,0,3,0x44,High8BitGpio,PCA9545_Bus0,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE3,4,7,0x44,Low8BitGpio,PCA9545_Bus0,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE3,8,11,0x46,High8BitGpio,PCA9545_Bus0,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE3,12,15,0x46,Low8BitGpio,PCA9545_Bus0,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},

  {INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
};
//#mizl-20201014-Update For Signle CPU NVMe Config ->
//Hugc-20211222-Add Riser/NVMe control function >>>
NVME_CONFIG    gNVMeConfigListInstalled[]={
  //{DieNumber,StartLane,EndLane,SlaveAddress,ByteMap,ChannelNumber}
  //Socket0 Die0
  {DIE0,8,11,0x48,High8BitGpio,PCA9545_Bus0,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE0,12,15,0x48,Low8BitGpio,PCA9545_Bus0,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  //Socket0 Die1
  {DIE1,0,3,0x4A,Low8BitGpio,PCA9545_Bus0,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE1,4,7,0x4A,High8BitGpio,PCA9545_Bus0,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY}, 
  //Socket 0 Die2
  {DIE2,0,3,0x40,Low8BitGpio,PCA9545_Bus0,RISERA_NVME1_DETECT_N(InputHigh),INVALID_SETTING_ARRARY},
  {DIE2,4,7,0x40,High8BitGpio,PCA9545_Bus0,RISERA_NVME1_DETECT_N(InputHigh),INVALID_SETTING_ARRARY},
  {DIE2,8,11,0x42,Low8BitGpio,PCA9545_Bus0,RISERA_NVME1_DETECT_N(InputHigh),INVALID_SETTING_ARRARY},
  {DIE2,12,15,0x42,High8BitGpio,PCA9545_Bus0,RISERA_NVME1_DETECT_N(InputHigh),INVALID_SETTING_ARRARY},
  
  //Socket 0 Die3
  {DIE3,0,3,0x44,High8BitGpio,PCA9545_Bus0,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE3,4,7,0x44,Low8BitGpio,PCA9545_Bus0,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE3,8,11,0x46,High8BitGpio,PCA9545_Bus0,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE3,12,15,0x46,Low8BitGpio,PCA9545_Bus0,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY}, 
  //Socket 1 Die0
  {DIE4,0,3,0x40,Low8BitGpio,PCA9545_Bus1,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE4,4,7,0x40,High8BitGpio,PCA9545_Bus1,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE4,8,11,0x42,High8BitGpio,PCA9545_Bus1,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE4,12,15,0x42,Low8BitGpio,PCA9545_Bus1,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  
  //Socket 1 Die1
  {DIE5,0,3,0x44,Low8BitGpio,PCA9545_Bus1,RISERB_NVME1_DETECT_N(InputHigh),INVALID_SETTING_ARRARY},
  {DIE5,4,7,0x44,High8BitGpio,PCA9545_Bus1,RISERB_NVME1_DETECT_N(InputHigh),INVALID_SETTING_ARRARY},
  {DIE5,8,11,0x46,Low8BitGpio,PCA9545_Bus1,RISERB_NVME1_DETECT_N(InputHigh),INVALID_SETTING_ARRARY},
  {DIE5,12,15,0x46,High8BitGpio,PCA9545_Bus1,RISERB_NVME1_DETECT_N(InputHigh),INVALID_SETTING_ARRARY},
  
  
  //Socket 1 Die2
  {DIE6,0,3,0x48,High8BitGpio,PCA9545_Bus1,RISERB_NVME0_DETECT_N(InputHigh),INVALID_SETTING_ARRARY},
  {DIE6,4,7,0x48,Low8BitGpio,PCA9545_Bus1,RISERB_NVME0_DETECT_N(InputHigh),INVALID_SETTING_ARRARY},
  {DIE6,8,11,0x4a,High8BitGpio,PCA9545_Bus1,RISERB_NVME0_DETECT_N(InputHigh),INVALID_SETTING_ARRARY},
  {DIE6,12,15,0x4a,Low8BitGpio,PCA9545_Bus1,RISERB_NVME0_DETECT_N(InputHigh),INVALID_SETTING_ARRARY},
  
  
  //Socket 1 Die3
  {DIE7,0,3,0x4c,High8BitGpio,PCA9545_Bus1,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE7,4,7,0x4c,Low8BitGpio,PCA9545_Bus1,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE7,8,11,0x4e,High8BitGpio,PCA9545_Bus1,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  {DIE7,12,15,0x4e,Low8BitGpio,PCA9545_Bus1,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},
  //
  //retimer Reversal
  {DIE2,0,3,0x40,High8BitGpio,PCA9545_Bus0, RISERA_NVME1_DETECT_N(InputLow),INVALID_SETTING_ARRARY},
  {DIE2,4,7,0x40,Low8BitGpio,PCA9545_Bus0,RISERA_NVME1_DETECT_N(InputLow),INVALID_SETTING_ARRARY},
  {DIE2,8,11,0x42,High8BitGpio,PCA9545_Bus0,RISERA_NVME1_DETECT_N(InputLow),INVALID_SETTING_ARRARY},
  {DIE2,12,15,0x42,Low8BitGpio,PCA9545_Bus0,RISERA_NVME1_DETECT_N(InputLow),INVALID_SETTING_ARRARY},
  //retimer Reversal
  {DIE5,0,3,0x44,High8BitGpio,PCA9545_Bus1,RISERB_NVME1_DETECT_N(InputLow),INVALID_SETTING_ARRARY},
  {DIE5,4,7,0x44,Low8BitGpio,PCA9545_Bus1,RISERB_NVME1_DETECT_N(InputLow),INVALID_SETTING_ARRARY},
  {DIE5,8,11,0x46,High8BitGpio,PCA9545_Bus1,RISERB_NVME1_DETECT_N(InputLow),INVALID_SETTING_ARRARY},
  {DIE5,12,15,0x46,Low8BitGpio,PCA9545_Bus1,RISERB_NVME1_DETECT_N(InputLow),INVALID_SETTING_ARRARY},
  //retimer Reversal
  {DIE6,0,3,0x48,High8BitGpio,PCA9545_Bus1, RISERB_NVME0_DETECT_N(InputLow),INVALID_SETTING_ARRARY},
  {DIE6,4,7,0x48,Low8BitGpio,PCA9545_Bus1,RISERB_NVME0_DETECT_N(InputLow),INVALID_SETTING_ARRARY},
  {DIE6,8,11,0x4a,High8BitGpio,PCA9545_Bus1,RISERB_NVME0_DETECT_N(InputLow),INVALID_SETTING_ARRARY},
  {DIE6,12,15,0x4a,Low8BitGpio,PCA9545_Bus1,RISERB_NVME0_DETECT_N(InputLow),INVALID_SETTING_ARRARY},
    //retimer Reversal
  {DIE7,0,3,0x4c,Low8BitGpio,PCA9545_Bus1, RISERC_NVME_DETECT_N(InputLow),INVALID_SETTING_ARRARY},
  {DIE7,4,7,0x4c,High8BitGpio,PCA9545_Bus1,RISERC_NVME_DETECT_N(InputLow),INVALID_SETTING_ARRARY},
  {DIE7,8,11,0x4e,Low8BitGpio,PCA9545_Bus1,RISERC_NVME_DETECT_N(InputLow),INVALID_SETTING_ARRARY},
  {DIE7,12,15,0x4e,High8BitGpio,PCA9545_Bus1,RISERC_NVME_DETECT_N(InputLow),INVALID_SETTING_ARRARY},
  {INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},

};
//Hugc-20211222-Add Riser/NVMe control function <<<