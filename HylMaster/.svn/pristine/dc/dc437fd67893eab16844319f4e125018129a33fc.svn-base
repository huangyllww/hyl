#include "Porting.h"
#include "AmdPcieComplex.h"
#include "AutoDxioConfig.h"
#include "Token.h"
#include "AmdServerHotplug.h" //#mizl-20201014-Update For Signle CPU NVMe Config +>

//#mizl-20201014-Update For Signle CPU NVMe Config ->
#define INVALID_SETTING_ARRARY {INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,"INVALID_SETTING"}
//EGPIO136
#define SATA_CPU0_DETECT0_N(ValidSeeting) 136,DIE0,SOCKET0,ValidSeeting,"SATA_CPU0_DETECT0_N"
//EGPIO152
#define RAID_SLOT_NVME_DET_N(ValidSeeting) 152,DIE0,SOCKET0,ValidSeeting,"RAID_SLOT_NVME_DET_N"
//EGPIO138
#define SATA_CPU0_DETECT1_N(ValidSeeting) 138,DIE0,SOCKET0,ValidSeeting,"SATA_CPU0_DETECT1_N"
//EGPIO140
#define OCP_PCIEX16_DET_N(ValidSeeting) 140,DIE0,SOCKET0,ValidSeeting,"OCP_PCIEX16_DET_N"
//AGPIO10_3
#define RISERA_TYPE0_N(ValidSeeting) 10,DIE3,SOCKET0,ValidSeeting,"RISERA_TYPE0_N"
//AGPIO40_3
#define RISERA_TYPE1_N(ValidSeeting) 40,DIE3,SOCKET0,ValidSeeting,"RISERA_TYPE1_N"
//AGPIO5
#define SATA_CPU1_DETECT0_N(ValidSeeting) 5,DIE0,SOCKET1,ValidSeeting,"SATA_CPU1_DETECT0_N"
//AGPIO4
#define OCP_PCIEX8_DET_N(ValidSeeting) 4,DIE0,SOCKET1,ValidSeeting,"OCP_PCIEX8_DET_N"
//EGPIO10_3
#define RISERB_TYPE0_N(ValidSeeting) 10,DIE3,SOCKET1,ValidSeeting,"RISERB_TYPE0_N"
//EGPIO40_3
#define RISERB_TYPE1_N(ValidSeeting) 40,DIE3,SOCKET1,ValidSeeting,"RISERB_TYPE1_N"
//AGPIO3
#define RISERC_TYPE0_N(ValidSeeting) 3,DIE0,SOCKET1,ValidSeeting,"RISERC_TYPE0_N"
//AGPIO6
#define RISERC_NVME_DETECT_N(ValidSeeting) 6,DIE0,SOCKET1,ValidSeeting,"RISERC_NVME_DETECT_N"
//EGPIO142
#define PCIEX16_CPU0_P3_N(ValidSeeting) 142,DIE0,SOCKET0,ValidSeeting,"PCIEX16_CPU0_P3_N" //Add For For V1.4

PCIE_TOPY_CONFIG    PcieTopyConfigList[]={
  //----------------------------------------------------------------------------
  //Die - 0
  //X4 SATA Lane4~7
  {DIE0,4,7,DxioSATAEngine,SATA_CPU0_DETECT0_N(InputLow),INVALID_SETTING_ARRARY},
  //X4 PCIe Lane4~7
  {DIE0,4,7,DxioPcieEngine,SATA_CPU0_DETECT0_N(InputHigh),INVALID_SETTING_ARRARY},
  //X4 PCIe X2 Lane8~11 Lane12~15
  {DIE0,8,11,DxioPcieEngine,RAID_SLOT_NVME_DET_N(InputLow),INVALID_SETTING_ARRARY},
  {DIE0,12,15,DxioPcieEngine,RAID_SLOT_NVME_DET_N(InputLow),INVALID_SETTING_ARRARY},
  //X8 PCIe Lane8~15
  {DIE0,8,15,DxioPcieEngine,RAID_SLOT_NVME_DET_N(InputHigh),INVALID_SETTING_ARRARY},
  //----------------------------------------------------------------------------
  //Die 1
  //X4  PCIe Lane0~3 Lane 4~7
  {DIE1,0,3,DxioPcieEngine,SATA_CPU0_DETECT1_N(InputHigh),OCP_PCIEX16_DET_N(InputHigh)},
  {DIE1,4,7,DxioPcieEngine,SATA_CPU0_DETECT1_N(InputHigh),OCP_PCIEX16_DET_N(InputHigh)},
  //X8 Sata Lane0~7
  {DIE1,0,7,DxioSATAEngine,SATA_CPU0_DETECT1_N(InputLow),INVALID_SETTING_ARRARY},
  //X8 PCIe Lane8~15
  {DIE1,8,15,DxioPcieEngine,SATA_CPU0_DETECT1_N(InputLow),INVALID_SETTING_ARRARY},
  //X8 PCIe Lane8~15
  {DIE1,8,15,DxioPcieEngine,SATA_CPU0_DETECT1_N(InputHigh),OCP_PCIEX16_DET_N(InputHigh)},
  //X16 PCIe Lane0~15
  {DIE1,0,15,DxioPcieEngine,SATA_CPU0_DETECT1_N(InputHigh),OCP_PCIEX16_DET_N(InputLow)},
  //----------------------------------------------------------------------------
  //Die 2
  //X4 PCIe Lane0~3 Lane4~7
  {DIE2,0,3,DxioPcieEngine,RISERA_TYPE0_N(InputLow),RISERA_TYPE1_N(InputHigh)},
  {DIE2,4,7,DxioPcieEngine,RISERA_TYPE0_N(InputLow),RISERA_TYPE1_N(InputHigh)},
  //X8 PCIe Lane0~7
  {DIE2,0,7,DxioPcieEngine,RISERA_TYPE0_N(InputLow),RISERA_TYPE1_N(InputLow)},
  {DIE2,0,7,DxioPcieEngine,RISERA_TYPE0_N(InputHigh),RISERA_TYPE1_N(InputHigh)},
  //X4 PCIe Lane8~11 Lane12~15
  {DIE2,8,11,DxioPcieEngine,RISERA_TYPE0_N(InputLow),RISERA_TYPE1_N(InputHigh)},
  {DIE2,12,15,DxioPcieEngine,RISERA_TYPE0_N(InputLow),RISERA_TYPE1_N(InputHigh)},
  {DIE2,8,11,DxioPcieEngine,RISERA_TYPE0_N(InputHigh),RISERA_TYPE1_N(InputHigh)},
  {DIE2,12,15,DxioPcieEngine,RISERA_TYPE0_N(InputHigh),RISERA_TYPE1_N(InputHigh)},
  //X8 PCIe Lane8~15
  {DIE2,8,15,DxioPcieEngine,RISERA_TYPE0_N(InputLow),RISERA_TYPE1_N(InputLow)},
  //X16 PCIe Lane0~15
  {DIE2,0,15,DxioPcieEngine,RISERA_TYPE0_N(InputHigh),RISERA_TYPE1_N(InputLow)},
  //----------------------------------------------------------------------------
  //Die 3
  {DIE3,0,3,DxioPcieEngine,RISERA_TYPE1_N(InputHigh),PCIEX16_CPU0_P3_N(InputHigh)},
  {DIE3,4,7,DxioPcieEngine,RISERA_TYPE1_N(InputHigh),PCIEX16_CPU0_P3_N(InputHigh)},
  {DIE3,0,3,DxioPcieEngine,RISERA_TYPE1_N(InputLow),PCIEX16_CPU0_P3_N(InputHigh)},
  {DIE3,4,7,DxioPcieEngine,RISERA_TYPE1_N(InputLow),PCIEX16_CPU0_P3_N(InputHigh)},
  //X4 PCIe Lane8~11 Lane12~15
  {DIE3,8,11,DxioPcieEngine,RISERA_TYPE1_N(InputHigh),PCIEX16_CPU0_P3_N(InputHigh)},
  {DIE3,12,15,DxioPcieEngine,RISERA_TYPE1_N(InputHigh),PCIEX16_CPU0_P3_N(InputHigh)},
  //X8 PCIe Lane8~15
  {DIE3,8,15,DxioPcieEngine,RISERA_TYPE1_N(InputLow),PCIEX16_CPU0_P3_N(InputHigh)},
  //X16 PCIe Lane0~15
  {DIE3,0,15,DxioPcieEngine,PCIEX16_CPU0_P3_N(InputLow),INVALID_SETTING_ARRARY},
  //----------------------------------------------------------------------------
  //Die 4
  //X4 PCIe Lane0~3 Lane4~7
  {DIE4,0,3,DxioPcieEngine,SATA_CPU1_DETECT0_N(InputHigh),INVALID_SETTING_ARRARY},
  {DIE4,4,7,DxioPcieEngine,SATA_CPU1_DETECT0_N(InputHigh),INVALID_SETTING_ARRARY},
  //X8 SATA Lane0~7
  {DIE4,0,7,DxioSATAEngine,SATA_CPU1_DETECT0_N(InputLow),INVALID_SETTING_ARRARY},
  //X4 PCIe Lane8~11 Lane12~15
  {DIE4,8,11,DxioPcieEngine,OCP_PCIEX8_DET_N(InputHigh),INVALID_SETTING_ARRARY},
  {DIE4,12,15,DxioPcieEngine,OCP_PCIEX8_DET_N(InputHigh),INVALID_SETTING_ARRARY},
  //X8 PCIe Lane8~15
  {DIE4,8,15,DxioPcieEngine,OCP_PCIEX8_DET_N(InputLow),INVALID_SETTING_ARRARY},
  //----------------------------------------------------------------------------
  //Die5
  //X4 PCIe Lane0~3 Lane4~7
  {DIE5,0,3,DxioPcieEngine,RISERB_TYPE0_N(InputLow),RISERB_TYPE1_N(InputHigh)},
  {DIE5,4,7,DxioPcieEngine,RISERB_TYPE0_N(InputLow),RISERB_TYPE1_N(InputHigh)},
  //X8 PCIe Lane0~7
  {DIE5,0,7,DxioPcieEngine,RISERB_TYPE0_N(InputLow),RISERB_TYPE1_N(InputLow)},
  {DIE5,0,7,DxioPcieEngine,RISERB_TYPE0_N(InputHigh),RISERB_TYPE1_N(InputHigh)},
  //X4 PCIe Lane8~11 Lane12~15
  {DIE5,8,11,DxioPcieEngine,RISERB_TYPE0_N(InputLow),RISERB_TYPE1_N(InputHigh)},
  {DIE5,12,15,DxioPcieEngine,RISERB_TYPE0_N(InputLow),RISERB_TYPE1_N(InputHigh)},
  {DIE5,8,11,DxioPcieEngine,RISERB_TYPE0_N(InputHigh),RISERB_TYPE1_N(InputHigh)},
  {DIE5,12,15,DxioPcieEngine,RISERB_TYPE0_N(InputHigh),RISERB_TYPE1_N(InputHigh)},
  //X8 PCIe Lane8~15
  {DIE5,8,15,DxioPcieEngine,RISERB_TYPE0_N(InputLow),RISERB_TYPE1_N(InputLow)},
  //X16 PCIe Lane0~15
  {DIE5,0,15,DxioPcieEngine,RISERB_TYPE0_N(InputHigh),RISERB_TYPE1_N(InputLow)},
  //----------------------------------------------------------------------------
  //Die6
  //X4 PCIe Lane0~3 Lane4~7 Lane8~11 Lane12~15
  {DIE6,0,3,DxioPcieEngine,RISERB_TYPE1_N(InputHigh),INVALID_SETTING_ARRARY},
  {DIE6,4,7,DxioPcieEngine,RISERB_TYPE1_N(InputHigh),INVALID_SETTING_ARRARY},
  {DIE6,8,11,DxioPcieEngine,RISERB_TYPE1_N(InputHigh),INVALID_SETTING_ARRARY},
  {DIE6,12,15,DxioPcieEngine,RISERB_TYPE1_N(InputHigh),INVALID_SETTING_ARRARY},
  //X16 PCIe Lane0~15
  {DIE6,0,15,DxioPcieEngine,RISERB_TYPE1_N(InputLow),INVALID_SETTING_ARRARY},
  //----------------------------------------------------------------------------
  //Die7
  //X4 PCIe Lane0~3 Lane4~7
  {DIE7,0,3,DxioPcieEngine,RISERC_NVME_DETECT_N(InputLow),INVALID_SETTING_ARRARY},
  {DIE7,4,7,DxioPcieEngine,RISERC_NVME_DETECT_N(InputLow),INVALID_SETTING_ARRARY},
  //X8 PCIe Lane0~7
  {DIE7,0,7,DxioPcieEngine,RISERC_TYPE0_N(InputLow),RISERC_NVME_DETECT_N(InputHigh)},
  //X4 PCIe Lane8~11 Lane12~15
  {DIE7,8,11,DxioPcieEngine,RISERC_NVME_DETECT_N(InputLow),INVALID_SETTING_ARRARY},
  {DIE7,12,15,DxioPcieEngine,RISERC_NVME_DETECT_N(InputLow),INVALID_SETTING_ARRARY},
  //X8 PCIe Lane8~15
  {DIE7,8,15,DxioPcieEngine,RISERC_TYPE0_N(InputLow),RISERC_NVME_DETECT_N(InputHigh)},
  //X16 PCIe Lane0~15
  {DIE7,0,15,DxioPcieEngine,RISERC_TYPE0_N(InputHigh),RISERC_NVME_DETECT_N(InputHigh)},
  // //Table End
  {INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,RISERC_TYPE0_N(InputHigh),RISERC_NVME_DETECT_N(InputHigh)},
};