#include "Porting.h"
#include "AmdPcieComplex.h"
#include "AutoDxioConfig.h"
#include "Token.h"
#include "AmdServerHotplug.h" //#mizl-20201014-Update For Signle CPU NVMe Config +>

//#mizl-20201014-Update For Signle CPU NVMe Config ->
#define INVALID_SETTING_ARRARY                                                            \
  {                                                                                       \
    INVALID_SETTING, INVALID_SETTING, INVALID_SETTING, INVALID_SETTING, "INVALID_SETTING" \
  }
//EGPIO136
#define SATA_CPU0_DETECT0_N(ValidSeeting) 136, DIE0, SOCKET0, ValidSeeting, "SATA_CPU0_DETECT0_N"
//EGPIO152
#define RAID_SLOT_NVME_DET_N(ValidSeeting) 152, DIE0, SOCKET0, ValidSeeting, "RAID_SLOT_NVME_DET_N"
//EGPIO138
#define SATA_CPU0_DETECT1_N(ValidSeeting) 138, DIE0, SOCKET0, ValidSeeting, "SATA_CPU0_DETECT1_N"
//EGPIO140
#define OCP_PCIEX16_DET_N(ValidSeeting) 140, DIE0, SOCKET0, ValidSeeting, "OCP_PCIEX16_DET_N"
//AGPIO10_3
#define RISERA_TYPE0_N(ValidSeeting) 10, DIE3, SOCKET0, ValidSeeting, "RISERA_TYPE0_N"
//AGPIO40_3
#define RISERA_TYPE1_N(ValidSeeting) 40, DIE3, SOCKET0, ValidSeeting, "RISERA_TYPE1_N"
//AGPIO5
#define SATA_CPU1_DETECT0_N(ValidSeeting) 5, DIE0, SOCKET1, ValidSeeting, "SATA_CPU1_DETECT0_N"
//AGPIO4
#define OCP_PCIEX8_DET_N(ValidSeeting) 4, DIE0, SOCKET1, ValidSeeting, "OCP_PCIEX8_DET_N"
//EGPIO10_3
#define RISERB_TYPE0_N(ValidSeeting) 10, DIE3, SOCKET1, ValidSeeting, "RISERB_TYPE0_N"
//EGPIO40_3
#define RISERB_TYPE1_N(ValidSeeting) 40, DIE3, SOCKET1, ValidSeeting, "RISERB_TYPE1_N"
//AGPIO3
#define RISERC_TYPE0_N(ValidSeeting) 3, DIE0, SOCKET1, ValidSeeting, "RISERC_TYPE0_N"
//AGPIO6
#define RISERC_NVME_DETECT_N(ValidSeeting) 6, DIE0, SOCKET1, ValidSeeting, "RISERC_NVME_DETECT_N"
//EGPIO142
#define PCIEX16_CPU0_P3_N(ValidSeeting) 142, DIE0, SOCKET0, ValidSeeting, "PCIEX16_CPU0_P3_N" //Add For For V1.4
//AGPIO23_3
#define RISERA_NVME1_DETECT_N(ValidSeeting) 23, DIE3, SOCKET0, ValidSeeting, "RISERA_NVME1_DETECT_N"
//AGPIO23_3
#define RISERB_NVME1_DETECT_N(ValidSeeting) 23, DIE3, SOCKET1, ValidSeeting, "RISERB_NVME1_DETECT_N"
//AGPIO9_3
#define RISERB_NVME0_DETECT_N(ValidSeeting) 9, DIE3, SOCKET1, ValidSeeting, "RISERB_NVME0_DETECT_N"

PCIE_TOPY_CONFIG PcieTopyConfigList[] = {
    //----------------------------------------------------------------------------
    //Die - 0
    {DIE0,  4,  7, DxioSATAEngine, 0xFF, 0xFF, SATA_CPU0_DETECT0_N(InputLow), INVALID_SETTING_ARRARY, INVALID_SETTING_ARRARY},        //Offset = 0x00 | X4 | SATA |Lane4~7
    {DIE0,  4,  7, DxioPcieEngine, 1, 5, SATA_CPU0_DETECT0_N(InputHigh), INVALID_SETTING_ARRARY, INVALID_SETTING_ARRARY},             //Offset = 0x01 | X4 | PCIe |Lane4~7
    {DIE0,  8, 11, DxioPcieEngine, 1, 7, RAID_SLOT_NVME_DET_N(InputLow), INVALID_SETTING_ARRARY, INVALID_SETTING_ARRARY},             //Offset = 0x02 | X2 | PCIe |Lane8~11
    {DIE0, 12, 15, DxioPcieEngine, 1, 6, RAID_SLOT_NVME_DET_N(InputLow), INVALID_SETTING_ARRARY, INVALID_SETTING_ARRARY},             //Offset = 0x03 | X2 | PCIe |Lane12~15
    {DIE0,  8, 15, DxioPcieEngine, 1, 6, RAID_SLOT_NVME_DET_N(InputHigh), INVALID_SETTING_ARRARY, INVALID_SETTING_ARRARY},            //Offset = 0x04 | X8 | PCIe |Lane8~15
    //----------------------------------------------------------------------------
    //Die 1
    {DIE1,  0,  3, DxioPcieEngine, 1, 1, SATA_CPU0_DETECT1_N(InputHigh), OCP_PCIEX16_DET_N(InputHigh), INVALID_SETTING_ARRARY},       //Offset = 0x05 | X4 | PCIe |Lane0~3
    {DIE1,  4,  7, DxioPcieEngine, 1, 2, SATA_CPU0_DETECT1_N(InputHigh), OCP_PCIEX16_DET_N(InputHigh), INVALID_SETTING_ARRARY},       //Offset = 0x06 | X4 | PCIe |Lane4~7
    {DIE1,  0,  7, DxioSATAEngine, 0xFF, 0XFF, SATA_CPU0_DETECT1_N(InputLow), INVALID_SETTING_ARRARY, INVALID_SETTING_ARRARY},        //Offset = 0x07 | X8 | Sata |Lane0~7
    {DIE1,  8, 15, DxioPcieEngine, 1, 3, SATA_CPU0_DETECT1_N(InputLow), INVALID_SETTING_ARRARY, INVALID_SETTING_ARRARY},              //Offset = 0x08 | X8 | PCIe |Lane8~15
    {DIE1,  8, 15, DxioPcieEngine, 1, 3, SATA_CPU0_DETECT1_N(InputHigh), OCP_PCIEX16_DET_N(InputHigh), INVALID_SETTING_ARRARY},       //Offset = 0x09 | X8 | PCIe |Lane8~15
    {DIE1,  0, 15, DxioPcieEngine, 1, 4, SATA_CPU0_DETECT1_N(InputHigh), OCP_PCIEX16_DET_N(InputLow), INVALID_SETTING_ARRARY},        //Offset = 0x0a | X16 | PCIe |Lane0~15
    //----------------------------------------------------------------------------
    //Die 2
    {DIE2,  0,  3, DxioPcieEngine, 3, 1, RISERA_TYPE0_N(InputLow), RISERA_TYPE1_N(InputHigh), INVALID_SETTING_ARRARY},                //Offset = 0x0b | X4 | PCIe |Lane0~3
    {DIE2,  4,  7, DxioPcieEngine, 3, 2, RISERA_TYPE0_N(InputLow), RISERA_TYPE1_N(InputHigh), INVALID_SETTING_ARRARY},                //Offset = 0x0c | X4 | PCIe |Lane4~7
    {DIE2,  0,  7, DxioPcieEngine, 3, 3, RISERA_TYPE0_N(InputLow), RISERA_TYPE1_N(InputLow), RISERA_NVME1_DETECT_N(InputHigh)},       //Offset = 0x0d | X8 | PCIe |Lane0~7
    {DIE2,  0,  7, DxioPcieEngine, 3, 3, RISERA_TYPE0_N(InputHigh), RISERA_TYPE1_N(InputHigh), RISERA_NVME1_DETECT_N(InputHigh)},     //Offset = 0x0e | X8 | PCIe |Lane0~7
    {DIE2,  8, 11, DxioPcieEngine, 3, 4, RISERA_TYPE0_N(InputLow), RISERA_TYPE1_N(InputHigh), INVALID_SETTING_ARRARY},                //Offset = 0x0f | X4 | PCIe |Lane8~11
    {DIE2, 12, 15, DxioPcieEngine, 3, 5, RISERA_TYPE0_N(InputLow), RISERA_TYPE1_N(InputHigh), INVALID_SETTING_ARRARY},                //Offset = 0x10 | X4 | PCIe |Lane12~15
    {DIE2,  8, 11, DxioPcieEngine, 3, 4, RISERA_TYPE0_N(InputHigh), RISERA_TYPE1_N(InputHigh), INVALID_SETTING_ARRARY},               //Offset = 0x11 | X4 | PCIe |Lane8~11
    {DIE2, 12, 15, DxioPcieEngine, 3, 5, RISERA_TYPE0_N(InputHigh), RISERA_TYPE1_N(InputHigh), INVALID_SETTING_ARRARY},               //Offset = 0x12 | X4 | PCIe |Lane12~15
    {DIE2,  8, 15, DxioPcieEngine, 3, 6, RISERA_TYPE0_N(InputLow), RISERA_TYPE1_N(InputLow), RISERA_NVME1_DETECT_N(InputHigh)},       //Offset = 0x13 | X8 | PCIe |Lane8~15
    {DIE2,  0, 15, DxioPcieEngine, 3, 7, RISERA_TYPE0_N(InputHigh), RISERA_TYPE1_N(InputLow), RISERA_NVME1_DETECT_N(InputHigh)},      //Offset = 0x14 | X16 | PCIe |Lane0~15
    {DIE2,  0,  3, DxioPcieEngine, 3, 5, RISERA_NVME1_DETECT_N(InputLow), INVALID_SETTING_ARRARY, INVALID_SETTING_ARRARY},            //Offset = 0x15 | X4 | PCIe |Lane0~3
    {DIE2,  4,  7, DxioPcieEngine, 3, 4, RISERA_NVME1_DETECT_N(InputLow), INVALID_SETTING_ARRARY, INVALID_SETTING_ARRARY},            //Offset = 0x16 | X4 | PCIe |Lane4~7
    {DIE2,  8, 11, DxioPcieEngine, 3, 2, RISERA_NVME1_DETECT_N(InputLow), INVALID_SETTING_ARRARY, INVALID_SETTING_ARRARY},            //Offset = 0x17 | X4 | PCIe |Lane8~11
    {DIE2, 12, 15, DxioPcieEngine, 3, 1, RISERA_NVME1_DETECT_N(InputLow), INVALID_SETTING_ARRARY, INVALID_SETTING_ARRARY},            //Offset = 0x18 | X4 | PCIe |Lane12~15
    //----------------------------------------------------------------------------
    //Die 3
    {DIE3,  8, 15, DxioPcieEngine, 3, 5, RISERA_TYPE1_N(InputLow), PCIEX16_CPU0_P3_N(InputHigh), RISERA_TYPE0_N(InputLow)},            //Offset = 0x19 | X8 | PCIe |Lane8~15
    {DIE3,  8, 15, DxioPcieEngine, 3, 5, RISERA_TYPE1_N(InputLow), PCIEX16_CPU0_P3_N(InputHigh), RISERA_TYPE0_N(InputHigh)},           //Offset = 0x1a | X8 | PCIe |Lane8~15
    {DIE3,  8, 15, DxioPcieEngine, 3, 5, RISERA_TYPE1_N(InputLow), PCIEX16_CPU0_P3_N(InputHigh), INVALID_SETTING_ARRARY},              //Offset = 0x1b | X8 | PCIe |Lane8~15
    {DIE3,  0,  3, DxioPcieEngine, 3, 2, RISERA_TYPE1_N(InputHigh), PCIEX16_CPU0_P3_N(InputHigh), INVALID_SETTING_ARRARY},             //Offset = 0x1c | X4 | PCIe |Lane0~3
    {DIE3,  4,  7, DxioPcieEngine, 3, 1, RISERA_TYPE1_N(InputHigh), PCIEX16_CPU0_P3_N(InputHigh), INVALID_SETTING_ARRARY},             //Offset = 0x1d | X4 | PCIe |Lane4~7
    {DIE3,  0,  3, DxioPcieEngine, 3, 2, RISERA_TYPE1_N(InputLow), PCIEX16_CPU0_P3_N(InputHigh), INVALID_SETTING_ARRARY},              //Offset = 0x1e | X4 | PCIe |Lane0~3
    {DIE3,  4,  7, DxioPcieEngine, 3, 1, RISERA_TYPE1_N(InputLow), PCIEX16_CPU0_P3_N(InputHigh), INVALID_SETTING_ARRARY},              //Offset = 0x1f | X4 | PCIe |Lane4~7
    {DIE3,  8, 11, DxioPcieEngine, 3, 4, RISERA_TYPE1_N(InputHigh), PCIEX16_CPU0_P3_N(InputHigh), INVALID_SETTING_ARRARY},             //Offset = 0x20 | X4 | PCIe |Lane8~11
    {DIE3, 12, 15, DxioPcieEngine, 3, 3, RISERA_TYPE1_N(InputHigh), PCIEX16_CPU0_P3_N(InputHigh), INVALID_SETTING_ARRARY},             //Offset = 0x21 | X4 | PCIe |Lane12~15
    {DIE3,  0, 15, DxioPcieEngine, 3, 6, PCIEX16_CPU0_P3_N(InputLow), INVALID_SETTING_ARRARY, INVALID_SETTING_ARRARY},                 //Offset = 0x22 | X16 | PCIe |Lane0~15
    //----------------------------------------------------------------------------
    //Die 4
    {DIE4,  0,  3, DxioPcieEngine, 1, 1, SATA_CPU1_DETECT0_N(InputHigh), INVALID_SETTING_ARRARY, INVALID_SETTING_ARRARY},              //Offset = 0x23 | X4 | PCIe |Lane0~3
    {DIE4,  4,  7, DxioPcieEngine, 1, 2, SATA_CPU1_DETECT0_N(InputHigh), INVALID_SETTING_ARRARY, INVALID_SETTING_ARRARY},              //Offset = 0x24 | X4 | PCIe |Lane4~7
    {DIE4,  0,  7, DxioSATAEngine, 0xFF, 0xFF, SATA_CPU1_DETECT0_N(InputLow), INVALID_SETTING_ARRARY, INVALID_SETTING_ARRARY},         //Offset = 0x25 | X8 | SATA |Lane0~7
    {DIE4,  8, 11, DxioPcieEngine, 1, 4, OCP_PCIEX8_DET_N(InputHigh), INVALID_SETTING_ARRARY, INVALID_SETTING_ARRARY},                 //Offset = 0x26 | X4 | PCIe |Lane8~11
    {DIE4, 12, 15, DxioPcieEngine, 1, 3, OCP_PCIEX8_DET_N(InputHigh), INVALID_SETTING_ARRARY, INVALID_SETTING_ARRARY},                 //Offset = 0x27 | X4 | PCIe |Lane12~15
    {DIE4,  8, 15, DxioPcieEngine, 1, 5, OCP_PCIEX8_DET_N(InputLow), INVALID_SETTING_ARRARY, INVALID_SETTING_ARRARY},                  //Offset = 0x28 | X8 | PCIe |Lane8~15
    //----------------------------------------------------------------------------
    //Die5
    {DIE5,  0,  3, DxioPcieEngine, 1, 1, RISERB_TYPE0_N(InputLow), RISERB_TYPE1_N(InputHigh), INVALID_SETTING_ARRARY},                  //Offset = 0x29 | X4 | PCIe |Lane0~3
    {DIE5,  4,  7, DxioPcieEngine, 1, 2, RISERB_TYPE0_N(InputLow), RISERB_TYPE1_N(InputHigh), INVALID_SETTING_ARRARY},                  //Offset = 0x2a | X4 | PCIe |Lane4~7
    {DIE5,  0,  7, DxioPcieEngine, 1, 3, RISERB_TYPE0_N(InputLow), RISERB_TYPE1_N(InputLow), RISERB_NVME1_DETECT_N(InputHigh)},         //Offset = 0x2b | X8 | PCIe |Lane0~7
    {DIE5,  0,  7, DxioPcieEngine, 1, 3, RISERB_TYPE0_N(InputHigh), RISERB_TYPE1_N(InputHigh), RISERB_NVME1_DETECT_N(InputHigh)},       //Offset = 0x2c | X8 | PCIe |Lane0~7
    {DIE5,  8, 11, DxioPcieEngine, 1, 4, RISERB_TYPE0_N(InputLow), RISERB_TYPE1_N(InputHigh), INVALID_SETTING_ARRARY},                  //Offset = 0x2d | X4 | PCIe |Lane8~11
    {DIE5, 12, 15, DxioPcieEngine, 1, 5, RISERB_TYPE0_N(InputLow), RISERB_TYPE1_N(InputHigh), INVALID_SETTING_ARRARY},                  //Offset = 0x2e | X4 | PCIe |Lane12~15
    {DIE5,  8, 11, DxioPcieEngine, 1, 4, RISERB_TYPE0_N(InputHigh), RISERB_TYPE1_N(InputHigh), INVALID_SETTING_ARRARY},                 //Offset = 0x2f | X4 | PCIe |Lane8~11
    {DIE5, 12, 15, DxioPcieEngine, 1, 5, RISERB_TYPE0_N(InputHigh), RISERB_TYPE1_N(InputHigh), INVALID_SETTING_ARRARY},                 //Offset = 0x30 | X4 | PCIe |Lane12~15
    {DIE5,  8, 15, DxioPcieEngine, 1, 6, RISERB_TYPE0_N(InputLow), RISERB_TYPE1_N(InputLow), RISERB_NVME1_DETECT_N(InputHigh)},         //Offset = 0x31 | X8 | PCIe |Lane8~15
    {DIE5,  0, 15, DxioPcieEngine, 1, 7, RISERB_TYPE0_N(InputHigh), RISERB_TYPE1_N(InputLow), RISERB_NVME1_DETECT_N(InputHigh)},        //Offset = 0x32 | X16 | PCIe |Lane0~15
    {DIE5,  0,  3, DxioPcieEngine, 1, 5, RISERB_NVME1_DETECT_N(InputLow), INVALID_SETTING_ARRARY, INVALID_SETTING_ARRARY},              //Offset = 0x33 | X4 | PCIe |Lane0~3
    {DIE5,  4,  7, DxioPcieEngine, 1, 4, RISERB_NVME1_DETECT_N(InputLow), INVALID_SETTING_ARRARY, INVALID_SETTING_ARRARY},              //Offset = 0x34 | X4 | PCIe |Lane4~7
    {DIE5,  8, 11, DxioPcieEngine, 1, 2, RISERB_NVME1_DETECT_N(InputLow), INVALID_SETTING_ARRARY, INVALID_SETTING_ARRARY},              //Offset = 0x35 | X4 | PCIe |Lane8~11
    {DIE5, 12, 15, DxioPcieEngine, 1, 1, RISERB_NVME1_DETECT_N(InputLow), INVALID_SETTING_ARRARY, INVALID_SETTING_ARRARY},              //Offset = 0x36 | X4 | PCIe |Lane12~15
    //----------------------------------------------------------------------------
    //Die6
    {DIE6,  0, 15, DxioPcieEngine, 3, 5, RISERB_TYPE1_N(InputLow), RISERB_NVME0_DETECT_N(InputHigh), RISERB_TYPE0_N(InputLow)},         //Offset = 0x37 | X16 | PCIe |Lane0~15
    {DIE6,  0, 15, DxioPcieEngine, 3, 5, RISERB_TYPE1_N(InputLow), RISERB_NVME0_DETECT_N(InputHigh), RISERB_TYPE0_N(InputHigh)},        //Offset = 0x38 | X16 | PCIe |Lane0~15
    {DIE6,  0, 15, DxioPcieEngine, 3, 5, RISERB_TYPE1_N(InputLow), RISERB_NVME0_DETECT_N(InputHigh), INVALID_SETTING_ARRARY},           //Offset = 0x39 | X16 | PCIe |Lane0~15
    {DIE6,  0,  3, DxioPcieEngine, 3, 2, RISERB_TYPE1_N(InputHigh), INVALID_SETTING_ARRARY, INVALID_SETTING_ARRARY},                    //Offset = 0x3a | X4 | PCIe |Lane0~3
    {DIE6,  4,  7, DxioPcieEngine, 3, 1, RISERB_TYPE1_N(InputHigh), INVALID_SETTING_ARRARY, INVALID_SETTING_ARRARY},                    //Offset = 0x3b | X4 | PCIe |Lane4~7
    {DIE6,  8, 11, DxioPcieEngine, 3, 4, RISERB_TYPE1_N(InputHigh), INVALID_SETTING_ARRARY, INVALID_SETTING_ARRARY},                    //Offset = 0x3c | X4 | PCIe |Lane8~11
    {DIE6, 12, 15, DxioPcieEngine, 3, 3, RISERB_TYPE1_N(InputHigh), INVALID_SETTING_ARRARY, INVALID_SETTING_ARRARY},                    //Offset = 0x3d | X4 | PCIe |Lane12~15
    {DIE6,  0,  3, DxioPcieEngine, 3, 4, RISERB_NVME0_DETECT_N(InputLow), INVALID_SETTING_ARRARY, INVALID_SETTING_ARRARY},              //Offset = 0x3e | X4 | PCIe |Lane0~3
    {DIE6,  4,  7, DxioPcieEngine, 3, 3, RISERB_NVME0_DETECT_N(InputLow), INVALID_SETTING_ARRARY, INVALID_SETTING_ARRARY},              //Offset = 0x3f | X4 | PCIe |Lane4~7
    {DIE6,  8, 11, DxioPcieEngine, 3, 2, RISERB_NVME0_DETECT_N(InputLow), INVALID_SETTING_ARRARY, INVALID_SETTING_ARRARY},              //Offset = 0x40 | X4 | PCIe |Lane8~11
    {DIE6, 12, 15, DxioPcieEngine, 3, 1, RISERB_NVME0_DETECT_N(InputLow), INVALID_SETTING_ARRARY, INVALID_SETTING_ARRARY},              //Offset = 0x41 | X4 | PCIe |Lane12~15
    //----------------------------------------------------------------------------
    //Die7
    {DIE7,  0,  3, DxioPcieEngine, 3, 2, RISERC_NVME_DETECT_N(InputLow), INVALID_SETTING_ARRARY, INVALID_SETTING_ARRARY},               //Offset = 0x42 | X4 | PCIe |Lane0~3
    {DIE7,  4,  7, DxioPcieEngine, 3, 1, RISERC_NVME_DETECT_N(InputLow), INVALID_SETTING_ARRARY, INVALID_SETTING_ARRARY},               //Offset = 0x43 | X4 | PCIe |Lane4~7
    {DIE7,  0,  7, DxioPcieEngine, 3, 3, RISERC_TYPE0_N(InputLow), RISERC_NVME_DETECT_N(InputHigh), INVALID_SETTING_ARRARY},            //Offset = 0x44 | X8 | PCIe |Lane0~7
    {DIE7,  8, 11, DxioPcieEngine, 3, 5, RISERC_NVME_DETECT_N(InputLow), INVALID_SETTING_ARRARY, INVALID_SETTING_ARRARY},               //Offset = 0x45 | X4 | PCIe |Lane8~11
    {DIE7, 12, 15, DxioPcieEngine, 3, 4, RISERC_NVME_DETECT_N(InputLow), INVALID_SETTING_ARRARY, INVALID_SETTING_ARRARY},               //Offset = 0x46 | X4 | PCIe |Lane12~15
    {DIE7,  8, 15, DxioPcieEngine, 3, 6, RISERC_TYPE0_N(InputLow), RISERC_NVME_DETECT_N(InputHigh), INVALID_SETTING_ARRARY},            //Offset = 0x47 | X8 | PCIe |Lane8~15
    {DIE7,  0, 15, DxioPcieEngine, 3, 7, RISERC_TYPE0_N(InputHigh), RISERC_NVME_DETECT_N(InputHigh), INVALID_SETTING_ARRARY},           //Offset = 0x48 | X16 | PCIe |Lane0~15
    // //Table End
    {INVALID_SETTING, INVALID_SETTING, INVALID_SETTING, INVALID_SETTING, 0xFF, 0xFF, RISERC_TYPE0_N(InputHigh), RISERC_NVME_DETECT_N(InputHigh), INVALID_SETTING_ARRARY}, //Offset =
};

//---------------------------------------------------------------------------------------------------------------------
//          Important Notice
//          All PCIe Topy Change Should Be Added From The Table List
//          And Sync PCIe Preset Table
//
//--------------------------------------------------------------------------------------------------------------------
