
#include "AutoDxioConfig.h"
#include "Token.h"
#include "AutoDxioConfig.h"
#define PcieSlotDescription(Die,Dev,Fun,RootLabel,DownPort,DownPort1,Slot) {Die,Dev,Fun,RootLabel,DownPort,DownPort1,Slot}

//<xuyj1001-20200401 Support new pcie topo +> 
BOARD_SLOT_DESCRIPTION    X740SingleWidthSlotDescList[]={
   {    //(111)
        0x7,//according to GPIO number to Calculation the value,if GPIO is invalid,it default value is 0
        {
  //Array 0
  //Socket 0 Die 0   basebaord slot 4 
  PcieSlotDescription(0,1,3,"SLM_C0P0_J157_X8",0xff,0xff,4),//Reversal J157 Lane 0~7 X8
  //Socket 0 Die 0   basebaord slot 6 
  PcieSlotDescription(0,1,6,"SLM_C0P0_J156_X8",0xff,0xff,6),//Reversal J156 Lane 8~15 X8
  //Socket 0 Die 2   basebaord slot 2 
  PcieSlotDescription(2,3,6,"SLM_C0P2_J163_X8",0xff,0xff,2),//Reversal J163 Lane 8~15 X8
  //Socket 0 Die 3   basebaord slot 3 
  PcieSlotDescription(3,3,3,"SLM_C0P3_J166_X8",0xff,0xff,3),//Reversal J166 Lane 0~7 X8
  //Socket 0 Die 3   basebaord slot 5 
  PcieSlotDescription(3,3,6,"SLM_C0P3_J165_X8",0xff,0xff,5),//Reversal J165 Lane 8~15 X8
  //Socket 1 Die 0   basebaord slot 10 
  PcieSlotDescription(4,1,3,"SLM_C1P0_J168_X8",0xff,0xff,10),//Reversal J168 Lane 0~7 X8
  //Socket 1 Die 0   basebaord slot 9 
  PcieSlotDescription(4,1,6,"SLM_C1P0_J167_X8",0xff,0xff,9),//Reversal J167 Lane 8~15 X8
  //Socket 1 Die 1   basebaord slot 11 
  PcieSlotDescription(5,1,7,"SLM_C1P1_J169_J170_X8",0xff,0xff,11),//Reversal J169/J170 Lane 0~15 X16
  //Socket 1 Die 3   basebaord slot 3 
  PcieSlotDescription(7,3,3,"SLM_C1P3_J174_X8",0xff,0xff,8),//Reversal J174 Lane 0~7 X8
  //Socket 1 Die 3   basebaord slot 5 
  PcieSlotDescription(7,3,6,"SLM_C1P3_J173_X8",0xff,0xff,7),//Reversal J173 Lane 8~15 X8

  PcieSlotDescription(INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,"",INVALID_SETTING,INVALID_SETTING,INVALID_SETTING)
     }
  }
};

BOARD_SLOT_DESCRIPTION    X740DoubleWidthSlotDescList[]={
   {    //(011)
        0x03,//according to GPIO number to Calculation the value,if GPIO is invalid,it default value is 0
        {
  //Array 0
  //Socket 0 Die 0   basebaord slot 6 
  PcieSlotDescription(0,1,7,"SLM_C0P0_J157_J156_X16",0xff,0xff,6),//Reversal J157&J156 Lane 0~15 X16
  //Socket 0 Die 2   basebaord slot 2 
  PcieSlotDescription(2,3,6,"SLM_C0P2_J163_X8",0xff,0xff,2),//Reversal J163 Lane 8~15 X8
 //Socket 0 Die 3   basebaord slot 3 
  PcieSlotDescription(3,3,7,"SLM_C0P3_J166_J165_X16",0xff,0xff,5),//Reversal J166&J165 Lane 0~15 X16
  //Socket 1 Die 0   basebaord slot 8 
  PcieSlotDescription(4,1,7,"SLM_C1P0_J168_J167_X16",0xff,0xff,8),//Reversal J168&J167 Lane 0~15 X16
  //Socket 1 Die 1   basebaord slot 11 
  PcieSlotDescription(5,1,7,"SLM_C1P1_J169_J170_X16",0xff,0xff,11),//Reversal J169&J170 Lane 0~15 X16
  //Socket 1 Die 1   basebaord slot 11 
  PcieSlotDescription(5,1,3,"SLM_C1P1_J170_X8",0xff,0xff,11),//Reversal J170 Lane 0~7 X8
  //Socket 1 Die 1   basebaord slot 10 
  PcieSlotDescription(5,1,6,"SLM_C1P1_J169_X8",0xff,0xff,10),//Reversal J169 Lane 8~15 X8
  //Socket 1 Die 2   basebaord slot 11 
  PcieSlotDescription(6,3,7,"SLM_C1P2_J171_J172_X16",0xff,0xff,11),//Reversal J171&J172 Lane 0~15 X16
  //Socket 1 Die 3   basebaord slot 7 
  PcieSlotDescription(7,3,7,"SLM_C1P3_J174_J173_X16",0xff,0xff,7),//Reversal J174&J173 Lane 0~15 X16
  
  PcieSlotDescription(INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,"",INVALID_SETTING,INVALID_SETTING,INVALID_SETTING)
     }
  }
};

BOARD_SLOT_DESCRIPTION    X780CommonSlotDescList[]={
   {    //(X10)
        0x2,//according to GPIO number to Calculation the value,if GPIO is invalid,it default value is 0
        {
  //Array 0
  //Socket 0 Die 0   basebaord slot 7 8 9 10 11 
  //Note: this is 0xff need to change according to basboard bridge dev number
  PcieSlotDescription(0,1,7,"SLM_C0P0_J157_J156_X16",0,0,10),//Reversal J157&J156 Lane 0~15 X16
  PcieSlotDescription(0,1,7,"SLM_C0P0_J157_J156_X16",0,16,9),//Reversal J157&J156 Lane 0~15 X16
  PcieSlotDescription(0,1,7,"SLM_C0P0_J157_J156_X16",4,0,8),//Reversal J157&J156 Lane 0~15 X16
  PcieSlotDescription(0,1,7,"SLM_C0P0_J157_J156_X16",4,16,11),//Reversal J157&J156 Lane 0~15 X16
  PcieSlotDescription(0,1,7,"SLM_C0P0_J157_J156_X16",8,16,7),//Reversal J157&J156 Lane 0~15 X16
  //Socket 0 Die 2   basebaord slot 1
  PcieSlotDescription(2,3,6,"SLM_C0P_J163_X8",0xff,0xff,1),//Reversal J157&J156 Lane 8-15 X8
 //Socket 0 Die 3   basebaord slot 2 3 4 5 6 
  PcieSlotDescription(3,3,7,"SLM_C0P3_J166_J165_X16",0,0,6),//Reversal J166&J165 Lane 0~15 X16
  PcieSlotDescription(3,3,7,"SLM_C0P3_J166_J165_X16",0,16,5),//Reversal J166&J165 Lane 0~15 X16
  PcieSlotDescription(3,3,7,"SLM_C0P3_J166_J165_X16",4,0,4),//Reversal J166&J165 Lane 0~15 X16
  PcieSlotDescription(3,3,7,"SLM_C0P3_J166_J165_X16",8,0,2),//Reversal J166&J165 Lane 0~15 X16
  PcieSlotDescription(3,3,7,"SLM_C0P3_J166_J165_X16",8,16,3),//Reversal J166&J165 Lane 0~15 X16
  //Socket 1 Die 1   basebaord slot 12 
  PcieSlotDescription(5,1,7,"SLM_C0P3_J169_J170_X16",0xff,0xff,12),//Reversal J169&J170 Lane 0~15 X16
  PcieSlotDescription(INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,"",INVALID_SETTING,INVALID_SETTING,INVALID_SETTING)
     }
  }
};

BOARD_SLOT_DESCRIPTION    X780BalancedModeSlotDescList[]={
   {    //(X01)
        0x1,//according to GPIO number to Calculation the value,if GPIO is invalid,it default value is 0
       {
  //Array 0
  //Socket 0 Die 0   basebaord slot 1 
  PcieSlotDescription(0,1,7,"SLM_C0P0_J157_J156_X16",0xff,0xff,1),//Reversal J157&J156 Lane 0~15 X16
  //Socket 0 Die 2   basebaord slot 1
  PcieSlotDescription(2,3,6,"SLM_C0P0_J163_X8",0xff,0xff,1),//Reversal J157&J156 Lane 8-15 X8
  //Note: this is 0xff need to change according to basboard bridge dev number
 //Socket 0 Die 3   basebaord slot 2 3 4 5 6 
  PcieSlotDescription(3,3,7,"SLM_C0P3_J166_J165_X16",0,0,6),//Reversal J166&J165 Lane 0~15 X16
  PcieSlotDescription(3,3,7,"SLM_C0P3_J166_J165_X16",0,16,5),//Reversal J166&J165 Lane 0~15 X16
  PcieSlotDescription(3,3,7,"SLM_C0P3_J166_J165_X16",4,0,4),//Reversal J166&J165 Lane 0~15 X16
  PcieSlotDescription(3,3,7,"SLM_C0P3_J166_J165_X16",8,0,2),//Reversal J166&J165 Lane 0~15 X16
  PcieSlotDescription(3,3,7,"SLM_C0P3_J166_J165_X16",8,16,3),//Reversal J166&J165 Lane 0~15 X16
  //Socket 1 Die 1   basebaord slot 12 
  PcieSlotDescription(5,1,7,"SLM_C0P3_J169_J170_X16",0xff,0xff,12),//Reversal J169&J170 Lane 0~15 X16
  //Socket 1 Die 3 baseboard slot 7 8 9 10 11
  PcieSlotDescription(7,3,7,"SLM_C0P0_J174_J173_X16",0,0,10),//Reversal J174&J173 Lane 0~15 X16
  PcieSlotDescription(7,3,7,"SLM_C0P0_J174_J173_X16",0,16,9),//Reversal J174&J173 Lane 0~15 X16
  PcieSlotDescription(7,3,7,"SLM_C0P0_J174_J173_X16",4,0,8),//Reversal J174&J173 Lane 0~15 X16
  PcieSlotDescription(7,3,7,"SLM_C0P0_J174_J173_X16",4,16,11),//Reversal J174&J173 Lane 0~15 X16
  PcieSlotDescription(7,3,7,"SLM_C0P0_J174_J173_X16",8,16,7),//Reversal J174&J173 Lane 0~15 X16
 
  PcieSlotDescription(INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,"",INVALID_SETTING,INVALID_SETTING,INVALID_SETTING)
     }
  }
};
/*
BOARD_SLOT_DESCRIPTION    X780CommonMultiOCPSlotDescList[]={
   {    //(10X1101)
        0x4D,//according to GPIO number to Calculation the value,if GPIO is invalid,it default value is 0
       {
  //Array 0
  //Socket 0 Die 0   basebaord slot 7 8 9 10 11 
  //Note: this is 0xff need to change according to basboard bridge dev number
  PcieSlotDescription(0,3,7,"SLM_C0P0_J157_J156_X16",0,0,10),//Reversal J157&J156 Lane 0~15 X16
  PcieSlotDescription(0,3,7,"SLM_C0P0_J157_J156_X16",0,16,9),//Reversal J157&J156 Lane 0~15 X16
  PcieSlotDescription(0,3,7,"SLM_C0P0_J157_J156_X16",4,0,8),//Reversal J157&J156 Lane 0~15 X16
  PcieSlotDescription(0,3,7,"SLM_C0P0_J157_J156_X16",4,16,11),//Reversal J157&J156 Lane 0~15 X16
  PcieSlotDescription(0,3,7,"SLM_C0P0_J157_J156_X16",8,16,7),//Reversal J157&J156 Lane 0~15 X16
 //Socket 0 Die 3   basebaord slot 2 3 4 5 6 
  PcieSlotDescription(3,3,7,"SLM_C0P3_J166_J165_X16",0,0,6),//Reversal J166&J165 Lane 0~15 X16
  PcieSlotDescription(3,3,7,"SLM_C0P3_J166_J165_X16",0,16,5),//Reversal J166&J165 Lane 0~15 X16
  PcieSlotDescription(3,3,7,"SLM_C0P3_J166_J165_X16",4,0,4),//Reversal J166&J165 Lane 0~15 X16
  PcieSlotDescription(3,3,7,"SLM_C0P3_J166_J165_X16",8,0,2),//Reversal J166&J165 Lane 0~15 X16
  PcieSlotDescription(3,3,7,"SLM_C0P3_J166_J165_X16",8,16,3),//Reversal J166&J165 Lane 0~15 X16
  //Socket 1 Die 1   basebaord slot 12 
  PcieSlotDescription(5,1,7,"SLM_C0P3_J169_J170_X16",0xff,0xff,12),//Reversal J169&J170 Lane 0~15 X16
  PcieSlotDescription(INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,"",INVALID_SETTING,INVALID_SETTING,INVALID_SETTING)
     }
  }
};

BOARD_SLOT_DESCRIPTION    X780Balanced8U2SlotDescList[]={
   {    //(00X0111)
        0x07,//according to GPIO number to Calculation the value,if GPIO is invalid,it default value is 0
       {
  //Array 0
  //Socket 0 Die 0   basebaord slot 1 
  //Note: this is 0xff need to change according to basboard bridge dev number
  PcieSlotDescription(0,3,7,"SLM_C0P0_J157_J156_X16",0xff,0xff,1),//Reversal J157&J156 Lane 0~15 X16
  //Socket 0 Die 3   basebaord slot 2 3 4 5 6 
  PcieSlotDescription(3,3,7,"SLM_C0P3_J166_J165_X16",0,0,6),//Reversal J166&J165 Lane 0~15 X16
  PcieSlotDescription(3,3,7,"SLM_C0P3_J166_J165_X16",0,16,5),//Reversal J166&J165 Lane 0~15 X16
  PcieSlotDescription(3,3,7,"SLM_C0P3_J166_J165_X16",4,0,4),//Reversal J166&J165 Lane 0~15 X16
  PcieSlotDescription(3,3,7,"SLM_C0P3_J166_J165_X16",8,0,2),//Reversal J166&J165 Lane 0~15 X16
  PcieSlotDescription(3,3,7,"SLM_C0P3_J166_J165_X16",8,16,3),//Reversal J166&J165 Lane 0~15 X16
  //Socket 1 Die 1   basebaord slot 3 
  PcieSlotDescription(5,1,7,"SLM_C0P3_J169_J170_X16",0xff,0xff,12),//Reversal J169&J170 Lane 0~15 X16
  //Socket 1 Die 3 baseboard slot 7 8 9 10 11
  PcieSlotDescription(7,3,7,"SLM_C0P0_J174_J173_X16",0,0,10),//Reversal J174&J173 Lane 0~15 X16
  PcieSlotDescription(7,3,7,"SLM_C0P0_J174_J173_X16",0,16,9),//Reversal J174&J173 Lane 0~15 X16
  PcieSlotDescription(7,3,7,"SLM_C0P0_J174_J173_X16",4,0,8),//Reversal J174&J173 Lane 0~15 X16
  PcieSlotDescription(7,3,7,"SLM_C0P0_J174_J173_X16",4,16,11),//Reversal J174&J173 Lane 0~15 X16
  PcieSlotDescription(7,3,7,"SLM_C0P0_J174_J173_X16",8,16,7),//Reversal J174&J173 Lane 0~15 X16
 
  PcieSlotDescription(INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,"",INVALID_SETTING,INVALID_SETTING,INVALID_SETTING)
     }
  }
};

BOARD_SLOT_DESCRIPTION    X780Balanced16U2SlotDescList[]={
   {    //(11X0111)
        0x67,//according to GPIO number to Calculation the value,if GPIO is invalid,it default value is 0
       {
  //Array 0

  //Note: this is 0xff need to change according to basboard bridge dev number
 //Socket 0 Die 3   basebaord slot 2 3 4 5 6 
  PcieSlotDescription(3,3,7,"SLM_C0P3_J166_J165_X16",0,0,6),//Reversal J166&J165 Lane 0~15 X16
  PcieSlotDescription(3,3,7,"SLM_C0P3_J166_J165_X16",0,16,5),//Reversal J166&J165 Lane 0~15 X16
  PcieSlotDescription(3,3,7,"SLM_C0P3_J166_J165_X16",4,0,4),//Reversal J166&J165 Lane 0~15 X16
  PcieSlotDescription(3,3,7,"SLM_C0P3_J166_J165_X16",8,0,2),//Reversal J166&J165 Lane 0~15 X16
  PcieSlotDescription(3,3,7,"SLM_C0P3_J166_J165_X16",8,16,3),//Reversal J166&J165 Lane 0~15 X16
  //Socket 1 Die 1   basebaord slot 3 
  PcieSlotDescription(5,1,7,"SLM_C0P3_J169_J170_X16",0xff,0xff,12),//Reversal J169&J170 Lane 0~15 X16
  //Socket 1 Die 3 baseboard slot 7 8 9 10 11
  PcieSlotDescription(7,3,7,"SLM_C0P0_J174_J173_X16",0,0,10),//Reversal J174&J173 Lane 0~15 X16
  PcieSlotDescription(7,3,7,"SLM_C0P0_J174_J173_X16",0,16,9),//Reversal J174&J173 Lane 0~15 X16
  PcieSlotDescription(7,3,7,"SLM_C0P0_J174_J173_X16",4,0,8),//Reversal J174&J173 Lane 0~15 X16
  PcieSlotDescription(7,3,7,"SLM_C0P0_J174_J173_X16",4,16,11),//Reversal J174&J173 Lane 0~15 X16
  PcieSlotDescription(7,3,7,"SLM_C0P0_J174_J173_X16",8,16,7),//Reversal J174&J173 Lane 0~15 X16
 
  PcieSlotDescription(INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,"",INVALID_SETTING,INVALID_SETTING,INVALID_SETTING)
     }
  }
};
*/
SLOT_DESCRIPTION    OnBoardDeviceList[7]={
   PcieSlotDescription(1,1,1,"OnBoard_00",0xff,0xff,0),//Aspeed AST2500 VGA
   PcieSlotDescription(1,1,2,"OnBoard_01",0xff,0xff,1),//ASMedia ASM1061R
   PcieSlotDescription(1,1,5,"OnBoard_RAID0",0xff,0xff,2),//RAID SLM
   PcieSlotDescription(2,3,3,"OnBoard_OCP0",0xff,0xff,3),//OCP Card SLM
   PcieSlotDescription(2,3,7,"OnBoard_OCP0",0xff,0xff,3),//OCP Card SLM
   PcieSlotDescription(6,3,3,"OnBoard_OCP1",0xff,0xff,3),//OCP Card SLM
   PcieSlotDescription(6,3,6,"OnBoard_RAID1",0xff,0xff,2),//RAID SLM
};


SLOT_DESCRIPTION    SmbiosRAIDOCPList[6]={
        PcieSlotDescription(1,1,5,"OnBoard_RAID0",0xff,0xff,0),//RAID SLM
        PcieSlotDescription(6,3,6,"OnBoard_RAID1",0xff,0xff,1),//RAID SLM
        PcieSlotDescription(2,3,3,"OnBoard_OCP0",0xff,0xff,2),//OCP Card SLM
        PcieSlotDescription(2,3,7,"OnBoard_OCP0",0xff,0xff,2),//OCP Card SLM
        PcieSlotDescription(6,3,3,"OnBoard_OCP1",0xff,0xff,3),//OCP Card SLM
        PcieSlotDescription(INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,"",INVALID_SETTING,INVALID_SETTING,INVALID_SETTING)
};

//<xuyj1001-20200401 Support new pcie topo -> 

SLOT_DESCRIPTION    NvmeList[16]={
   PcieSlotDescription(0,1,1,"U.2",0xff,0xff,1),//Aspeed AST2500 VGA
   PcieSlotDescription(0,1,2,"U.2",0xff,0xff,2),//ASMedia ASM1061R
   PcieSlotDescription(0,1,4,"U.2",0xff,0xff,3),//RAID SLM
   PcieSlotDescription(0,1,5,"U.2",0xff,0xff,4),//OCP Card SLM
   PcieSlotDescription(2,3,1,"U.2",0xff,0xff,5),//OCP Card SLM
   PcieSlotDescription(2,3,2,"U.2",0xff,0xff,6),//OCP Card SLM
   PcieSlotDescription(2,3,4,"U.2",0xff,0xff,7),//RAID SLM
   PcieSlotDescription(2,3,5,"U.2",0xff,0xff,8),//Aspeed AST2500 VGA
   PcieSlotDescription(4,1,1,"U.2",0xff,0xff,9),//ASMedia ASM1061R
   PcieSlotDescription(4,1,2,"U.2",0xff,0xff,10),//RAID SLM
   PcieSlotDescription(4,1,4,"U.2",0xff,0xff,11),//OCP Card SLM
   PcieSlotDescription(4,1,5,"U.2",0xff,0xff,12),//OCP Card SLM
   PcieSlotDescription(6,3,1,"U.2",0xff,0xff,13),//OCP Card SLM
   PcieSlotDescription(6,3,2,"U.2",0xff,0xff,14),//RAID SLM
   PcieSlotDescription(6,3,4,"U.2",0xff,0xff,15),//OCP Card SLM
   PcieSlotDescription(6,3,5,"U.2",0xff,0xff,16),//RAID SLM
};


SLOT_DESCRIPTION    SmbiosNvmeList[17]={
   PcieSlotDescription(0,1,1,"U.2",0xff,0xff,0),//Aspeed AST2500 VGA
   PcieSlotDescription(0,1,2,"U.2",0xff,0xff,1),//ASMedia ASM1061R
   PcieSlotDescription(0,1,4,"U.2",0xff,0xff,2),//RAID SLM
   PcieSlotDescription(0,1,5,"U.2",0xff,0xff,3),//OCP Card SLM
   PcieSlotDescription(2,3,1,"U.2",0xff,0xff,4),//OCP Card SLM
   PcieSlotDescription(2,3,2,"U.2",0xff,0xff,5),//OCP Card SLM
   PcieSlotDescription(2,3,4,"U.2",0xff,0xff,6),//RAID SLM
   PcieSlotDescription(2,3,5,"U.2",0xff,0xff,7),//Aspeed AST2500 VGA
   PcieSlotDescription(4,1,1,"U.2",0xff,0xff,8),//ASMedia ASM1061R
   PcieSlotDescription(4,1,2,"U.2",0xff,0xff,9),//RAID SLM
   PcieSlotDescription(4,1,4,"U.2",0xff,0xff,10),//OCP Card SLM
   PcieSlotDescription(4,1,5,"U.2",0xff,0xff,11),//OCP Card SLM
   PcieSlotDescription(6,3,1,"U.2",0xff,0xff,12),//OCP Card SLM
   PcieSlotDescription(6,3,2,"U.2",0xff,0xff,13),//RAID SLM
   PcieSlotDescription(6,3,4,"U.2",0xff,0xff,14),//OCP Card SLM
   PcieSlotDescription(6,3,5,"U.2",0xff,0xff,15),//RAID SLM
   PcieSlotDescription(INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,"",INVALID_SETTING,INVALID_SETTING,INVALID_SETTING)
};
