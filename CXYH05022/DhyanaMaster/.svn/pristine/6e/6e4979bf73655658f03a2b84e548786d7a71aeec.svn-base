#include "Porting.h"
#include "AmdPcieComplex.h"
#include "AutoDxioConfig.h"
#include "Token.h"
#include "AmdServerHotplug.h" //#mizl-20201014-Update For Signle CPU NVMe Config +>

//<xuyj1001-20200401 Support new pcie topo +> 
//XMachine H40  PCIe Lane Configuration
//GPU Baseboard GPIO Porting
//AGPIO9_3
#define GPU_BASE_BOARD_TYPE_ID0_N(ValidSeeting) {9,DIE3,SOCKET0,ValidSeeting}//J156 A30
//AGPIO10_3
#define GPU_BASE_BOARD_TYPE_ID1_N(ValidSeeting) {10,DIE3,SOCKET0,ValidSeeting}//J173 A30
//AGPIO6
#define GPU_BASE_BOARD_TYPE_ID2_N(ValidSeeting) {6,DIE0,SOCKET0,ValidSeeting}//J157 B27

//CPU Port Type GPIO Defined
//CPU0 P0
//APGIO9_0
#define GPU_CPU0_P0_TYPE_ID0_N(ValidSeeting) {9,DIE0,SOCKET0,ValidSeeting}
//APGIO23_0
#define GPU_CPU0_P0_TYPE_ID1_N(ValidSeeting) {23,DIE0,SOCKET0,ValidSeeting}

//CPU0 P2
//APGIO19
#define GPU_CPU0_P2_TYPE_ID0_N(ValidSeeting) {19,DIE0,SOCKET0,ValidSeeting}
//APGIO20
#define GPU_CPU0_P2_TYPE_ID1_N(ValidSeeting) {20,DIE0,SOCKET0,ValidSeeting}

//CPU1 P1
//APGIO139
#define GPU_CPU1_P1_TYPE_ID0_N(ValidSeeting) {139,DIE0,SOCKET0,ValidSeeting}
//APGIO23_3
#define GPU_CPU1_P1_TYPE_ID1_N(ValidSeeting) {23,DIE3,SOCKET0,ValidSeeting}

//CPU1 P2
//APGIO10_0
#define GPU_CPU1_P2_TYPE_ID0_N(ValidSeeting) {10,DIE0,SOCKET0,ValidSeeting}
//APGIO40_0
#define GPU_CPU1_P2_TYPE_ID1_N(ValidSeeting) {40,DIE0,SOCKET0,ValidSeeting}


//#mizl-20201014-Update For Signle CPU NVMe Config ->
#define INVALID_SETTING_ARRARY {INVALID_GPIO_SETTING,INVALID_SETTING,INVALID_SETTING,INVALID_SETTING}//,"INVALID_SETTING"}

//X740 Eight Single Width Configuration
BOARD_PCIE_TOPY_CONFIG    X740SingleWidthPCIeTopyList[]={
        {     
   //Board  ID GPIO Porting
  {GPU_BASE_BOARD_TYPE_ID2_N(InputHigh),GPU_BASE_BOARD_TYPE_ID1_N(InputHigh),GPU_BASE_BOARD_TYPE_ID0_N(InputHigh)} , 
  {
  //PCIe Lane Configuration
  //Die - 0 2x8 
  {{INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},DIE0,0,7,DxioPcieEngine,0}, //Connect Baseboard Slot 4
  {{INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},DIE0,8,15,DxioPcieEngine,0},//Connect Baseboard Slot 6

  //Die - 2 x16 OCP
  {{GPU_CPU0_P2_TYPE_ID0_N(InputLow),GPU_CPU0_P2_TYPE_ID1_N(InputLow)},DIE2,0,15,DxioPcieEngine,1}, //OCP
  //Die - 2 2x8 OCP Slot2
  {{GPU_CPU0_P2_TYPE_ID0_N(InputLow),GPU_CPU0_P2_TYPE_ID1_N(InputHigh)},DIE2,0,7,DxioPcieEngine,0}, //X8-J29-Slot2
  {{GPU_CPU0_P2_TYPE_ID0_N(InputLow),GPU_CPU0_P2_TYPE_ID1_N(InputHigh)},DIE2,8,15,DxioPcieEngine,1}, //X8-J158-OCP
  //Die - 2 4x4
  {{GPU_CPU0_P2_TYPE_ID0_N(InputHigh),GPU_CPU0_P2_TYPE_ID1_N(InputHigh)},DIE2,0,3,DxioPcieEngine,0}, //U.2 SSD I2C Address 0x44
  {{GPU_CPU0_P2_TYPE_ID0_N(InputHigh),GPU_CPU0_P2_TYPE_ID1_N(InputHigh)},DIE2,4,7,DxioPcieEngine,0}, //U.2 SSD I2C Address 0x44
  {{GPU_CPU0_P2_TYPE_ID0_N(InputHigh),GPU_CPU0_P2_TYPE_ID1_N(InputHigh)},DIE2,8,11,DxioPcieEngine,0}, //U.2 SSD I2C Address 0x46
  {{GPU_CPU0_P2_TYPE_ID0_N(InputHigh),GPU_CPU0_P2_TYPE_ID1_N(InputHigh)},DIE2,12,15,DxioPcieEngine,0},//U.2 SSD I2C Address 0x46
  
  //Die - 3 2x8
  {{INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},DIE3,0,7,DxioPcieEngine,0},//Connect Baseboard Slot 3
  {{INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},DIE3,8,15,DxioPcieEngine,0},//Connect Baseboard Slot 5
  
  //Die - 4 2x8
  {{INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},DIE4,0,7,DxioPcieEngine,0},//Connect Baseboard Slot 10
  {{INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},DIE4,8,15,DxioPcieEngine,0},//Connect Baseboard Slot 9
  
  //Die - 5 x16
  {{GPU_CPU1_P1_TYPE_ID0_N(InputLow),GPU_CPU1_P1_TYPE_ID1_N(InputLow)},DIE5,0,15,DxioPcieEngine,0}, //x16-J30(J169)/J31(J170)-Slot11(00)
  //Die - 5 4x4
  {{GPU_CPU1_P1_TYPE_ID0_N(InputHigh),GPU_CPU1_P1_TYPE_ID1_N(InputHigh)},DIE5,0,3,DxioPcieEngine,0}, //U.2 SSD I2C Address 0x48
  {{GPU_CPU1_P1_TYPE_ID0_N(InputHigh),GPU_CPU1_P1_TYPE_ID1_N(InputHigh)},DIE5,4,7,DxioPcieEngine,0}, //U.2 SSD I2C Address 0x48
  {{GPU_CPU1_P1_TYPE_ID0_N(InputHigh),GPU_CPU1_P1_TYPE_ID1_N(InputHigh)},DIE5,8,11,DxioPcieEngine,0}, //U.2 SSD I2C Address 0x4a
  {{GPU_CPU1_P1_TYPE_ID0_N(InputHigh),GPU_CPU1_P1_TYPE_ID1_N(InputHigh)},DIE5,12,15,DxioPcieEngine,0},//U.2 SSD I2C Address 0x4a
  
  //Die - 6 2x8
  {{GPU_CPU1_P2_TYPE_ID0_N(InputLow),INVALID_SETTING_ARRARY},DIE6,0,7,DxioPcieEngine,0}, //X8-J158-OCP
  {{GPU_CPU1_P2_TYPE_ID0_N(InputLow),INVALID_SETTING_ARRARY},DIE6,8,15,DxioPcieEngine,0}, //X8-RAID
  //Die - 6 2x8
  {{INVALID_SETTING_ARRARY,GPU_CPU1_P2_TYPE_ID1_N(InputLow)},DIE6,0,7,DxioPcieEngine,0}, //X8-J158-OCP
  {{INVALID_SETTING_ARRARY,GPU_CPU1_P2_TYPE_ID1_N(InputLow)},DIE6,8,15,DxioPcieEngine,0}, //X8-RAID
  
  //Die - 6 4x4
  {{GPU_CPU1_P2_TYPE_ID0_N(InputHigh),GPU_CPU1_P2_TYPE_ID1_N(InputHigh)},DIE6,0,3,DxioPcieEngine,0},//U.2 SSD I2C Address 0x40
  {{GPU_CPU1_P2_TYPE_ID0_N(InputHigh),GPU_CPU1_P2_TYPE_ID1_N(InputHigh)},DIE6,4,7,DxioPcieEngine,0},//U.2 SSD I2C Address 0x40
  {{GPU_CPU1_P2_TYPE_ID0_N(InputHigh),GPU_CPU1_P2_TYPE_ID1_N(InputHigh)},DIE6,8,11,DxioPcieEngine,0},//U.2 SSD I2C Address 0x42
  {{GPU_CPU1_P2_TYPE_ID0_N(InputHigh),GPU_CPU1_P2_TYPE_ID1_N(InputHigh)},DIE6,12,15,DxioPcieEngine,0},//U.2 SSD I2C Address 0x42
  
  //Die - 7 2x8
  {{INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},DIE7,0,7,DxioPcieEngine,0},//Connect Baseboard Slot 8
  {{INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},DIE7,8,15,DxioPcieEngine,0},//Connect Baseboard Slot 7
  // //Table End
  {{INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,INVALID_SETTING}

  }
        }
};
//X740 four Double Width Configuration
BOARD_PCIE_TOPY_CONFIG    X740DoubleWidthPCIeTopyList[]={
        {     
   //Board  ID GPIO Porting
  {GPU_BASE_BOARD_TYPE_ID2_N(InputLow),GPU_BASE_BOARD_TYPE_ID1_N(InputHigh),GPU_BASE_BOARD_TYPE_ID0_N(InputHigh)} , 
  {
  //PCIe Lane Configuration
  //Die - 0 x16
  {{INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},DIE0,0,15,DxioPcieEngine,0}, //x16-J157(J8)/J156(J7) Connect Baseboard Slot 6
  
  //Die - 2 x16 OCP
  {{GPU_CPU0_P2_TYPE_ID0_N(InputLow),GPU_CPU0_P2_TYPE_ID1_N(InputLow)},DIE2,0,15,DxioPcieEngine,1}, //OCP
  //Die - 2 2x8 OCP Slot2
  {{GPU_CPU0_P2_TYPE_ID0_N(InputLow),GPU_CPU0_P2_TYPE_ID1_N(InputHigh)},DIE2,0,7,DxioPcieEngine,0}, //X8-J29-Slot2
  {{GPU_CPU0_P2_TYPE_ID0_N(InputLow),GPU_CPU0_P2_TYPE_ID1_N(InputHigh)},DIE2,8,15,DxioPcieEngine,1}, //X8-J158-OCP
  //Die - 2 4x4
  {{GPU_CPU0_P2_TYPE_ID0_N(InputHigh),GPU_CPU0_P2_TYPE_ID1_N(InputHigh)},DIE2,0,3,DxioPcieEngine,0}, //U.2 SSD I2C Address 0x44
  {{GPU_CPU0_P2_TYPE_ID0_N(InputHigh),GPU_CPU0_P2_TYPE_ID1_N(InputHigh)},DIE2,4,7,DxioPcieEngine,0}, //U.2 SSD I2C Address 0x44
  {{GPU_CPU0_P2_TYPE_ID0_N(InputHigh),GPU_CPU0_P2_TYPE_ID1_N(InputHigh)},DIE2,8,11,DxioPcieEngine,0}, //U.2 SSD I2C Address 0x46
  {{GPU_CPU0_P2_TYPE_ID0_N(InputHigh),GPU_CPU0_P2_TYPE_ID1_N(InputHigh)},DIE2,12,15,DxioPcieEngine,0},//U.2 SSD I2C Address 0x46
  
  //Die - 3 x16
  {{INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},DIE3,0,15,DxioPcieEngine,0},//Connect Baseboard Slot 5
  
  //Die - 4 x16
  {{INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},DIE4,0,15,DxioPcieEngine,0},//Connect Baseboard Slot 8
  
  //Die - 5 x16
  {{GPU_CPU1_P1_TYPE_ID0_N(InputLow),GPU_CPU1_P1_TYPE_ID1_N(InputLow)},DIE5,0,15,DxioPcieEngine,0}, //x16-J30(J169)/J31(J170)-Slot11(00)
  
  //Die - 5 2x8
  {{GPU_CPU1_P1_TYPE_ID0_N(InputLow),GPU_CPU1_P1_TYPE_ID1_N(InputHigh)},DIE5,0,7,DxioPcieEngine,0}, //X8-RAID
  {{GPU_CPU1_P1_TYPE_ID0_N(InputLow),GPU_CPU1_P1_TYPE_ID1_N(InputHigh)},DIE5,8,15,DxioPcieEngine,0}, //X8-J158-OCP
  //Die - 5 4x4
  {{GPU_CPU1_P1_TYPE_ID0_N(InputHigh),GPU_CPU1_P1_TYPE_ID1_N(InputHigh)},DIE5,0,3,DxioPcieEngine,0}, //U.2 SSD I2C Address 0x48
  {{GPU_CPU1_P1_TYPE_ID0_N(InputHigh),GPU_CPU1_P1_TYPE_ID1_N(InputHigh)},DIE5,4,7,DxioPcieEngine,0}, //U.2 SSD I2C Address 0x48
  {{GPU_CPU1_P1_TYPE_ID0_N(InputHigh),GPU_CPU1_P1_TYPE_ID1_N(InputHigh)},DIE5,8,11,DxioPcieEngine,0}, //U.2 SSD I2C Address 0x4a
  {{GPU_CPU1_P1_TYPE_ID0_N(InputHigh),GPU_CPU1_P1_TYPE_ID1_N(InputHigh)},DIE5,12,15,DxioPcieEngine,0},//U.2 SSD I2C Address 0x4a
  
  //Die - 6 2x8
  {{GPU_CPU1_P2_TYPE_ID0_N(InputLow),INVALID_SETTING_ARRARY},DIE6,0,7,DxioPcieEngine,0}, //X8-J158-OCP
  {{GPU_CPU1_P2_TYPE_ID0_N(InputLow),INVALID_SETTING_ARRARY},DIE6,8,15,DxioPcieEngine,0}, //X8-RAID
  //Die - 6 2x8
  {{INVALID_SETTING_ARRARY,GPU_CPU1_P2_TYPE_ID1_N(InputLow)},DIE6,0,7,DxioPcieEngine,0}, //X8-J158-OCP
  {{INVALID_SETTING_ARRARY,GPU_CPU1_P2_TYPE_ID1_N(InputLow)},DIE6,8,15,DxioPcieEngine,0}, //X8-RAID
  //Die - 6 4x4
  {{GPU_CPU1_P2_TYPE_ID0_N(InputHigh),GPU_CPU1_P2_TYPE_ID1_N(InputHigh)},DIE6,0,3,DxioPcieEngine,0},//U.2 SSD I2C Address 0x40
  {{GPU_CPU1_P2_TYPE_ID0_N(InputHigh),GPU_CPU1_P2_TYPE_ID1_N(InputHigh)},DIE6,4,7,DxioPcieEngine,0},//U.2 SSD I2C Address 0x40
  {{GPU_CPU1_P2_TYPE_ID0_N(InputHigh),GPU_CPU1_P2_TYPE_ID1_N(InputHigh)},DIE6,8,11,DxioPcieEngine,0},//U.2 SSD I2C Address 0x42
  {{GPU_CPU1_P2_TYPE_ID0_N(InputHigh),GPU_CPU1_P2_TYPE_ID1_N(InputHigh)},DIE6,12,15,DxioPcieEngine,0},//U.2 SSD I2C Address 0x42
  
  //Die - 7 x16
  {{INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},DIE7,0,15,DxioPcieEngine,0},//Connect Baseboard Slot 7
  // //Table End
  {{INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,INVALID_SETTING}

  }
        }
};

//X780 Switch Common Configuration
BOARD_PCIE_TOPY_CONFIG    X780CommonPCIeTopyList[]={
        {     
   //Board  ID GPIO Porting
  {INVALID_SETTING_ARRARY,GPU_BASE_BOARD_TYPE_ID1_N(InputHigh),GPU_BASE_BOARD_TYPE_ID0_N(InputLow)} , 
  {
  //PCIe Lane Configuration
  //Die - 0 x16 
  {{INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},DIE0,0,15,DxioPcieEngine,0}, //Connect Baseboard Slot 7 8 9 10 11
  
  //Die - 2 x16 OCP
  {{GPU_CPU0_P2_TYPE_ID0_N(InputLow),GPU_CPU0_P2_TYPE_ID1_N(InputLow)},DIE2,0,15,DxioPcieEngine,1}, //x16-J158/J159-OCP
  
  //Die - 2 2x8 OCP Slot1
  {{GPU_CPU0_P2_TYPE_ID0_N(InputLow),GPU_CPU0_P2_TYPE_ID1_N(InputHigh)},DIE2,0,7,DxioPcieEngine,0}, //X8-J29-Slot1
  {{GPU_CPU0_P2_TYPE_ID0_N(InputLow),GPU_CPU0_P2_TYPE_ID1_N(InputHigh)},DIE2,8,15,DxioPcieEngine,1}, //X8-J158-OCP
  
  //Die - 2 4x4
  {{GPU_CPU0_P2_TYPE_ID0_N(InputHigh),GPU_CPU0_P2_TYPE_ID1_N(InputHigh)},DIE2,0,3,DxioPcieEngine,0}, //U.2 SSD I2C Address 0x44
  {{GPU_CPU0_P2_TYPE_ID0_N(InputHigh),GPU_CPU0_P2_TYPE_ID1_N(InputHigh)},DIE2,4,7,DxioPcieEngine,0}, //U.2 SSD I2C Address 0x44
  {{GPU_CPU0_P2_TYPE_ID0_N(InputHigh),GPU_CPU0_P2_TYPE_ID1_N(InputHigh)},DIE2,8,11,DxioPcieEngine,0}, //U.2 SSD I2C Address 0x46
  {{GPU_CPU0_P2_TYPE_ID0_N(InputHigh),GPU_CPU0_P2_TYPE_ID1_N(InputHigh)},DIE2,12,15,DxioPcieEngine,0},//U.2 SSD I2C Address 0x46
  
  //Die - 3 x16
  {{INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},DIE3,0,15,DxioPcieEngine,0},//Connect Baseboard Slot 2 3 4 5 6
  
  //Die - 4 4x4
  {{INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},DIE4,0,3,DxioPcieEngine,0}, //U.2 SSD I2C Address 0x40
  {{INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},DIE4,4,7,DxioPcieEngine,0}, //U.2 SSD I2C Address 0x40
  {{INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},DIE4,8,11,DxioPcieEngine,0}, //U.2 SSD I2C Address 0x42
  {{INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},DIE4,12,15,DxioPcieEngine,0},//U.2 SSD I2C Address 0x42
  
  //Die - 5 x16
  {{INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},DIE5,0,15,DxioPcieEngine,0}, //Connect Baseboard Slot 12
  
  //Die - 6 2x8
  {{GPU_CPU1_P2_TYPE_ID0_N(InputLow),INVALID_SETTING_ARRARY},DIE6,0,7,DxioPcieEngine,0}, //X8-J158-OCP
  {{GPU_CPU1_P2_TYPE_ID0_N(InputLow),INVALID_SETTING_ARRARY},DIE6,8,15,DxioPcieEngine,0}, //X8-RAID
  //Die - 6 2x8
  {{INVALID_SETTING_ARRARY,GPU_CPU1_P2_TYPE_ID1_N(InputLow)},DIE6,0,7,DxioPcieEngine,0}, //X8-J158-OCP
  {{INVALID_SETTING_ARRARY,GPU_CPU1_P2_TYPE_ID1_N(InputLow)},DIE6,8,15,DxioPcieEngine,0}, //X8-RAID
  //Die - 6 4x4
  {{GPU_CPU1_P2_TYPE_ID0_N(InputHigh),GPU_CPU1_P2_TYPE_ID1_N(InputHigh)},DIE6,0,3,DxioPcieEngine,0},//U.2 SSD I2C Address 0x48
  {{GPU_CPU1_P2_TYPE_ID0_N(InputHigh),GPU_CPU1_P2_TYPE_ID1_N(InputHigh)},DIE6,4,7,DxioPcieEngine,0},//U.2 SSD I2C Address 0x48
  {{GPU_CPU1_P2_TYPE_ID0_N(InputHigh),GPU_CPU1_P2_TYPE_ID1_N(InputHigh)},DIE6,8,11,DxioPcieEngine,0},//U.2 SSD I2C Address 0x4a
  {{GPU_CPU1_P2_TYPE_ID0_N(InputHigh),GPU_CPU1_P2_TYPE_ID1_N(InputHigh)},DIE6,12,15,DxioPcieEngine,0},//U.2 SSD I2C Address 0x4a
  
  //Die - 7 x16
  {{INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},DIE7,0,15,DxioPcieEngine,0}, //default to x16.no slot
  // //Table End
  {{INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,INVALID_SETTING}

  }
        }
};

//X780 Switch Balanced Mode configuration
BOARD_PCIE_TOPY_CONFIG    X780BalancedModePCIeTopyList[]={
        {     
   //Board  ID GPIO Porting
  {INVALID_SETTING_ARRARY,GPU_BASE_BOARD_TYPE_ID1_N(InputLow),GPU_BASE_BOARD_TYPE_ID0_N(InputHigh)} , 
  {
  //PCIe Lane Configuration
  //Die - 0 x16 
  {{GPU_CPU0_P0_TYPE_ID0_N(InputLow),GPU_CPU0_P0_TYPE_ID1_N(InputLow)},DIE0,0,15,DxioPcieEngine,0}, //x16-J157(J1)/J156(J2)-Slot1
  
  //Die - 0 4X4 
  {{GPU_CPU0_P0_TYPE_ID0_N(InputHigh),GPU_CPU0_P0_TYPE_ID1_N(InputHigh)},DIE0,0,3,DxioPcieEngine,0}, //U.2 SSD I2C Address 0x44
  {{GPU_CPU0_P0_TYPE_ID0_N(InputHigh),GPU_CPU0_P0_TYPE_ID1_N(InputHigh)},DIE0,4,7,DxioPcieEngine,0}, //U.2 SSD I2C Address 0x44
  {{GPU_CPU0_P0_TYPE_ID0_N(InputHigh),GPU_CPU0_P0_TYPE_ID1_N(InputHigh)},DIE0,8,11,DxioPcieEngine,0}, //U.2 SSD I2C Address 0x46
  {{GPU_CPU0_P0_TYPE_ID0_N(InputHigh),GPU_CPU0_P0_TYPE_ID1_N(InputHigh)},DIE0,12,15,DxioPcieEngine,0},//U.2 SSD I2C Address 0x46
  
  //Die - 2 x16 OCP
  {{GPU_CPU0_P2_TYPE_ID0_N(InputLow),GPU_CPU0_P2_TYPE_ID1_N(InputLow)},DIE2,0,15,DxioPcieEngine,1}, //x16-J158/J159-OCP
  
  //Die - 2 2x8 OCP Slot1
  {{GPU_CPU0_P2_TYPE_ID0_N(InputLow),GPU_CPU0_P2_TYPE_ID1_N(InputHigh)},DIE2,0,7,DxioPcieEngine,0}, //X8-J29-Slot1
  {{GPU_CPU0_P2_TYPE_ID0_N(InputLow),GPU_CPU0_P2_TYPE_ID1_N(InputHigh)},DIE2,8,15,DxioPcieEngine,1}, //X8-J158-OCP
  
  //Die - 2 4X4
  {{GPU_CPU0_P2_TYPE_ID0_N(InputHigh),GPU_CPU0_P2_TYPE_ID1_N(InputHigh)},DIE2,0,3,DxioPcieEngine,0}, //U.2 SSD I2C Address 0x4c
  {{GPU_CPU0_P2_TYPE_ID0_N(InputHigh),GPU_CPU0_P2_TYPE_ID1_N(InputHigh)},DIE2,4,7,DxioPcieEngine,0}, //U.2 SSD I2C Address 0x4c
  {{GPU_CPU0_P2_TYPE_ID0_N(InputHigh),GPU_CPU0_P2_TYPE_ID1_N(InputHigh)},DIE2,8,11,DxioPcieEngine,0}, //U.2 SSD I2C Address 0x4e
  {{GPU_CPU0_P2_TYPE_ID0_N(InputHigh),GPU_CPU0_P2_TYPE_ID1_N(InputHigh)},DIE2,12,15,DxioPcieEngine,0},//U.2 SSD I2C Address 0x4e
  
  //Die - 3 x16
  {{INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},DIE3,0,15,DxioPcieEngine,0},//Connect Baseboard Slot 2 3 4 5 6
  
  //Die - 4 4X4
  {{INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},DIE4,0,3,DxioPcieEngine,0}, //U.2 SSD I2C Address 0x4c
  {{INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},DIE4,4,7,DxioPcieEngine,0}, //U.2 SSD I2C Address 0x4c
  {{INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},DIE4,8,11,DxioPcieEngine,0}, //U.2 SSD I2C Address 0x4e
  {{INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},DIE4,12,15,DxioPcieEngine,0},//U.2 SSD I2C Address 0x4e
  
  //Die - 5 x16
  {{INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},DIE5,0,15,DxioPcieEngine,0}, //Connect Baseboard Slot 12
  
  //Die - 6 2x8
  {{GPU_CPU1_P2_TYPE_ID0_N(InputLow),INVALID_SETTING_ARRARY},DIE6,0,7,DxioPcieEngine,0}, //X8-J158-OCP
  {{GPU_CPU1_P2_TYPE_ID0_N(InputLow),INVALID_SETTING_ARRARY},DIE6,8,15,DxioPcieEngine,0}, //X8-RAID
  //Die - 6 2x8
  {{INVALID_SETTING_ARRARY,GPU_CPU1_P2_TYPE_ID1_N(InputLow)},DIE6,0,7,DxioPcieEngine,0}, //X8-J158-OCP
  {{INVALID_SETTING_ARRARY,GPU_CPU1_P2_TYPE_ID1_N(InputLow)},DIE6,8,15,DxioPcieEngine,0}, //X8-RAID
  //Die - 6 4x4
  {{GPU_CPU1_P2_TYPE_ID0_N(InputHigh),GPU_CPU1_P2_TYPE_ID1_N(InputHigh)},DIE6,0,3,DxioPcieEngine,0},//U.2 SSD I2C Address 0x48
  {{GPU_CPU1_P2_TYPE_ID0_N(InputHigh),GPU_CPU1_P2_TYPE_ID1_N(InputHigh)},DIE6,4,7,DxioPcieEngine,0},//U.2 SSD I2C Address 0x48
  {{GPU_CPU1_P2_TYPE_ID0_N(InputHigh),GPU_CPU1_P2_TYPE_ID1_N(InputHigh)},DIE6,8,11,DxioPcieEngine,0},//U.2 SSD I2C Address 0x4a
  {{GPU_CPU1_P2_TYPE_ID0_N(InputHigh),GPU_CPU1_P2_TYPE_ID1_N(InputHigh)},DIE6,12,15,DxioPcieEngine,0},//U.2 SSD I2C Address 0x4a
  
  //Die - 7 x16
  {{INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},DIE7,0,15,DxioPcieEngine,0},//Connect Baseboard Slot 7 8 9 10 11
  // //Table End
  {{INVALID_SETTING_ARRARY,INVALID_SETTING_ARRARY},INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,INVALID_SETTING}

  }
        }
};
/*
//X780 Switch Common mulityHost OCP Configuration
BOARD_PCIE_TOPY_CONFIG    X780CommonMultiOCPPCIeTopyList[]={
        {     
   //Board  ID GPIO Porting
  {GPU_NODE_TYPE_ID6_N(InputHigh),GPU_NODE_TYPE_ID5_N(InputLow),INVALID_SETTING_ARRARY,GPU_NODE_TYPE_ID3_N(InputHigh),GPU_NODE_TYPE_ID2_N(InputHigh),GPU_NODE_TYPE_ID1_N(InputLow),GPU_NODE_TYPE_ID0_N(InputHigh)} , 
  {
  //PCIe Lane Configuration
  //Die - 0 x16 
  {DIE0,0,15,DxioPcieEngine}, //Connect Baseboard Slot 7 8 9 10 11
  //Die - 2 2x8 OCP
  {DIE2,0,7,DxioPcieEngine},//OCP
  {DIE2,8,15,DxioPcieEngine},//OCP Connect OCP J158(0-7)
  //Die - 3 x16
  {DIE3,0,15,DxioPcieEngine},//Connect Baseboard Slot 2 3 4 5 6
  //Die - 4 4x4
  {DIE4,0,3,DxioPcieEngine}, //U.2 SSD I2C Address 0x44
  {DIE4,4,7,DxioPcieEngine}, //U.2 SSD I2C Address 0x44
  {DIE4,8,11,DxioPcieEngine}, //U.2 SSD I2C Address 0x46
  {DIE4,12,15,DxioPcieEngine},//U.2 SSD I2C Address 0x46
  //Die - 5 x16
  {DIE5,0,15,DxioPcieEngine}, //Connect Baseboard Slot 12
  //Die - 6 2x8
  {DIE6,0,7,DxioPcieEngine},
  {DIE6,8,15,DxioPcieEngine},//OCP Connect OCP J159(8-15)
  //Die - 7 4x4
  {DIE7,0,3,DxioPcieEngine}, //U.2 SSD I2C Address 0x40
  {DIE7,4,7,DxioPcieEngine}, //U.2 SSD I2C Address 0x40
  {DIE7,8,11,DxioPcieEngine}, //U.2 SSD I2C Address 0x42
  {DIE7,12,15,DxioPcieEngine},//U.2 SSD I2C Address 0x42
  // //Table End
  {INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,INVALID_SETTING}

  }
        }
};

//X780 Switch Balanced 8 U.2 configuration
BOARD_PCIE_TOPY_CONFIG    X780Balanced8U2PCIeTopyList[]={
        {     
   //Board  ID GPIO Porting
  {GPU_NODE_TYPE_ID6_N(InputLow),GPU_NODE_TYPE_ID5_N(InputLow),INVALID_SETTING_ARRARY,GPU_NODE_TYPE_ID3_N(InputLow),GPU_NODE_TYPE_ID2_N(InputHigh),GPU_NODE_TYPE_ID1_N(InputHigh),GPU_NODE_TYPE_ID0_N(InputHigh)} , 
  {
  //PCIe Lane Configuration
  //Die - 0 x16
  {DIE0,0,15,DxioPcieEngine}, //Connect Baseboard Slot 1
  //Die - 2 x16 OCP
  {DIE2,0,15,DxioPcieEngine},//OCP
  //Die - 3 x16
  {DIE3,0,15,DxioPcieEngine},//Connect Baseboard Slot 2 3 4 5 6
  //Die - 4 4x4
  {DIE4,0,3,DxioPcieEngine}, //U.2 SSD I2C Address 0x44
  {DIE4,4,7,DxioPcieEngine}, //U.2 SSD I2C Address 0x44
  {DIE4,8,11,DxioPcieEngine}, //U.2 SSD I2C Address 0x46
  {DIE4,12,15,DxioPcieEngine},//U.2 SSD I2C Address 0x46
  //Die - 5 x16
  {DIE5,0,15,DxioPcieEngine}, //Connect Baseboard Slot 12
  //Die - 6 4x4
  {DIE6,0,3,DxioPcieEngine}, //U.2 SSD I2C Address 0x40
  {DIE6,4,7,DxioPcieEngine}, //U.2 SSD I2C Address 0x40
  {DIE6,8,11,DxioPcieEngine}, //U.2 SSD I2C Address 0x42
  {DIE6,12,15,DxioPcieEngine},//U.2 SSD I2C Address 0x42
  //Die - 7 x16
  {DIE7,0,15,DxioPcieEngine},//Connect Baseboard Slot 7 8 9 10 11
  // //Table End
  {INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,INVALID_SETTING}

  }
        }
};


//X780 Switch Balanced 16 U.2 configuration
BOARD_PCIE_TOPY_CONFIG    X780Balanced16U2PCIeTopyList[]={
        {     
   //Board  ID GPIO Porting
  {GPU_NODE_TYPE_ID6_N(InputHigh),GPU_NODE_TYPE_ID5_N(InputHigh),INVALID_SETTING_ARRARY,GPU_NODE_TYPE_ID3_N(InputLow),GPU_NODE_TYPE_ID2_N(InputHigh),GPU_NODE_TYPE_ID1_N(InputHigh),GPU_NODE_TYPE_ID0_N(InputHigh)} , 
  {
  //PCIe Lane Configuration
  //Die - 0 4X4 
  {DIE0,0,3,DxioPcieEngine}, //U.2 SSD I2C Address 0x44
  {DIE0,4,7,DxioPcieEngine}, //U.2 SSD I2C Address 0x44
  {DIE0,8,11,DxioPcieEngine}, //U.2 SSD I2C Address 0x46
  {DIE0,12,15,DxioPcieEngine},//U.2 SSD I2C Address 0x46
  //Die - 2 4X4
  {DIE2,0,3,DxioPcieEngine}, //U.2 SSD I2C Address 0x40
  {DIE2,4,7,DxioPcieEngine}, //U.2 SSD I2C Address 0x40
  {DIE2,8,11,DxioPcieEngine}, //U.2 SSD I2C Address 0x42
  {DIE2,12,15,DxioPcieEngine},//U.2 SSD I2C Address 0x42
  //Die - 3 x16
  {DIE3,0,15,DxioPcieEngine},//Connect Baseboard Slot 2 3 4 5 6
  //Die - 4 4X4
  {DIE4,0,3,DxioPcieEngine}, //U.2 SSD I2C Address 0x4c
  {DIE4,4,7,DxioPcieEngine}, //U.2 SSD I2C Address 0x4c
  {DIE4,8,11,DxioPcieEngine}, //U.2 SSD I2C Address 0x4e
  {DIE4,12,15,DxioPcieEngine},//U.2 SSD I2C Address 0x4e
  //Die - 5 x16
  {DIE5,0,15,DxioPcieEngine}, //Connect Baseboard Slot 12
  //Die - 6 4x4
  {DIE6,0,3,DxioPcieEngine}, //U.2 SSD I2C Address 0x48
  {DIE6,4,7,DxioPcieEngine}, //U.2 SSD I2C Address 0x48
  {DIE6,8,11,DxioPcieEngine}, //U.2 SSD I2C Address 0x4a
  {DIE6,12,15,DxioPcieEngine},//U.2 SSD I2C Address 0x4a
  //Die - 7 x16
  {DIE7,0,15,DxioPcieEngine},//Connect Baseboard Slot 7 8 9 10 11
  // //Table End
  {INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,INVALID_SETTING}

  }
        }
};
*/
//<xuyj1001-20200401 Support new pcie topo -> 
