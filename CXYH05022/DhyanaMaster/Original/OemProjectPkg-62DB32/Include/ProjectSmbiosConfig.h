#include "OemSmbios.h"
#include "AutoDxioConfig.h"
#include "Token.h"


//tigerhu>>>
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
//AGPIO23_3
#define RISERA_NVME1_DETECT_N(ValidSeeting) 23,DIE3,SOCKET0,ValidSeeting,"RISERA_NVME1_DETECT_N" 
//AGPIO23_3
#define RISERB_NVME1_DETECT_N(ValidSeeting) 23,DIE3,SOCKET1,ValidSeeting,"RISERB_NVME1_DETECT_N"
//AGPIO9_3
#define RISERB_NVME0_DETECT_N(ValidSeeting) 9,DIE3,SOCKET1,ValidSeeting,"RISERB_NVME0_DETECT_N" 

//tigerhu<<<

typedef struct {
    UINT8                     SlotType;
    UINT8                     SlotDataBusWidth;
    UINT16                    SlotID;
    UINT16                    RearSlotID;//dengyh-20220117 change the physical slot ID to the slot ID at rear 
    CHAR8*                    sSlotDes;
    CHAR16*                   DevicePathText;  //bridge device path
    BOOLEAN                   MatchDevPath;
    GPIOCONFIG                GpioMap[MaxGpioSetting]; //defined based on platform
} SMBIOS_TYPE_9;

EFI_STATUS
OemCheckPlatformSetting(
  IN    SMBIOS_TYPE_9 *SlotsInfo
);

//riser type 0x11
GLOBAL_REMOVE_IF_UNREFERENCED SMBIOS_TYPE_9 SystemSlotsInfo[] = {
    {    SYS_SLOT_PCIE_4,    DBW_8X, 0x13, 0x3,                 "RiserA_Slot1",    L"PciRoot(0x2)/Pci(0x3,0x3)",    FALSE, RISERA_TYPE1_N(InputHigh), RISERA_TYPE0_N(InputHigh), INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,    DBW_8X, 0x14, 0xff,            "RiserA_Slim_x8_1",    L"PciRoot(0x2)/Pci(0x3,0x4)",    FALSE, INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,    DBW_8X, 0x15, 0xff,            "RiserA_Slim_x8_1",    L"PciRoot(0x2)/Pci(0x3,0x5)",    FALSE, INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,    DBW_8X, 0x1C, 0xff,            "RiserA_Slim_x8_2",    L"PciRoot(0x3)/Pci(0x3,0x3)",    FALSE, INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,    DBW_8X, 0x1B, 0xff,            "RiserA_Slim_x8_2",    L"PciRoot(0x3)/Pci(0x3,0x4)",    FALSE, INVALID_SETTING_ARRARY },

    //riser type 0x01, PCIEX16_CPU0_P3_N =0, no retimer card support
    //SMBIOS_TYPE_9 RISERA_TYPE_0X01_SLOT1_X16[] = {
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x17, 0x3,                 "RiserA_Slot2",    L"PciRoot(0x2)/Pci(0x3,0x7)",    FALSE, RISERA_TYPE1_N(InputLow), RISERA_TYPE0_N(InputHigh), PCIEX16_CPU0_P3_N(InputLow), INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x1E, 0x1,                 "RiserA_Slot1",    L"PciRoot(0x3)/Pci(0x3,0x6)",    FALSE, INVALID_SETTING_ARRARY },

    //riser type 0x01, PCIEX16_CPU0_P3_N = 1, RISERA_NVME1_DETECT_N = 1, Switch Riser: R3E6R-U2(No retimer support)
    //SMBIOS_TYPE_9 RISERA_TYPE_0X01_SWITCH[] = {
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x17, 0x3,                 "RiserA_Slot3",    L"PciRoot(0x2)/Pci(0x3,0x7)/Pci(0x0,0x0)/Pci(0x1,0x0)",    TRUE, RISERA_TYPE1_N(InputLow), RISERA_TYPE0_N(InputHigh), PCIEX16_CPU0_P3_N(InputHigh), RISERA_NVME1_DETECT_N(InputHigh) }, //TODO
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x17, 0x2,                 "RiserA_Slot2",    L"PciRoot(0x2)/Pci(0x3,0x7)/Pci(0x0,0x0)/Pci(0x0,0x0)",    TRUE, INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x1D, 0x1,                 "RiserA_Slot1",    L"PciRoot(0x3)/Pci(0x3,0x5)/Pci(0x0,0x0)/Pci(0x0,0x0)",    TRUE, INVALID_SETTING_ARRARY },

    //riser type 0x01, PCIEX16_CPU0_P3_N = 1, RISERA_NVME1_DETECT_N = 1, no retimer connect
    //SMBIOS_TYPE_9 RISERA_TYPE_0X01_SLOT1_X8[] = {
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x17, 0x3,                 "RiserA_Slot2",    L"PciRoot(0x2)/Pci(0x3,0x7)",    FALSE, INVALID_SETTING_ARRARY },
    {SYS_SLOT_PCIE_4_X16,    DBW_8X, 0x1D, 0x1,                 "RiserA_Slot1",    L"PciRoot(0x3)/Pci(0x3,0x5)",    FALSE, INVALID_SETTING_ARRARY },

    //riser type 0x01, PCIEX16_CPU0_P3_N = 1, RISERA_NVME1_DETECT_N = 0, retimer connect
    //SMBIOS_TYPE_9 RISERA_TYPE_0X01_SLOT1_X8_RETIMER[] = {
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x12, 0xff,                "RiserA_Slot2",    L"PciRoot(0x2)/Pci(0x3,0x1)",    FALSE, RISERA_TYPE1_N(InputLow), RISERA_TYPE0_N(InputHigh), PCIEX16_CPU0_P3_N(InputHigh), RISERA_NVME1_DETECT_N(InputLow) },
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x11, 0xff,                "RiserA_Slot2",    L"PciRoot(0x2)/Pci(0x3,0x2)",    FALSE, INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x15, 0xff,                "RiserA_Slot2",    L"PciRoot(0x2)/Pci(0x3,0x4)",    FALSE, INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x14, 0xff,                "RiserA_Slot2",    L"PciRoot(0x2)/Pci(0x3,0x5)",    FALSE, INVALID_SETTING_ARRARY },
    {SYS_SLOT_PCIE_4_X16,    DBW_8X, 0x1D, 0xff,                "RiserA_Slot1",    L"PciRoot(0x3)/Pci(0x3,0x5)",    FALSE, INVALID_SETTING_ARRARY },

    //SMBIOS_TYPE_9 RISERA_TYPE_0X00[] = {
    {    SYS_SLOT_PCIE_4,    DBW_8X, 0x13, 0x1,                 "RiserA_Slot1",    L"PciRoot(0x2)/Pci(0x3,0x3)",    FALSE, RISERA_TYPE1_N(InputLow), RISERA_TYPE0_N(InputLow), INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,    DBW_8X, 0x16, 0x2,                 "RiserA_Slot2",    L"PciRoot(0x2)/Pci(0x3,0x6)",    FALSE, INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,    DBW_8X, 0x1D, 0x3,                 "RiserA_Slot3",    L"PciRoot(0x3)/Pci(0x3,0x5)",    FALSE, INVALID_SETTING_ARRARY },

    //SMBIOS_TYPE_9 RISERA_TYPE_0X10[] = {
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x11, 0xff,            "RiserA_Slim_x8_2",    L"PciRoot(0x2)/Pci(0x3,0x1)",    FALSE, RISERA_TYPE1_N(InputHigh), RISERA_TYPE0_N(InputLow), INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x12, 0xff,            "RiserA_Slim_x8_2",    L"PciRoot(0x2)/Pci(0x3,0x2)",    FALSE, INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x14, 0xff,            "RiserA_Slim_x8_3",    L"PciRoot(0x2)/Pci(0x3,0x3)",    FALSE, INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x15, 0xff,            "RiserA_Slim_x8_3",    L"PciRoot(0x2)/Pci(0x3,0x4)",    FALSE, INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x1B, 0xff,            "RiserA_Slim_x8_1",    L"PciRoot(0x3)/Pci(0x3,0x3)",    FALSE, INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x1C, 0xff,            "RiserA_Slim_x8_1",    L"PciRoot(0x3)/Pci(0x3,0x4)",    FALSE, INVALID_SETTING_ARRARY },

    //riser type 0x11
    //SMBIOS_TYPE_9 RISERB_TYPE_0X11[] = {
    {    SYS_SLOT_PCIE_4,    DBW_8X, 0x2B, 0x3,                 "RiserB_Slot1",    L"PciRoot(0x5)/Pci(0x1,0x3)",    FALSE, RISERB_TYPE1_N(InputHigh), RISERB_TYPE0_N(InputHigh), INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,    DBW_8X, 0x2C, 0xff,            "RiserB_Slim_x8_1",    L"PciRoot(0x5)/Pci(0x1,0x4)",    FALSE, INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,    DBW_8X, 0x2D, 0xff,            "RiserB_Slim_x8_1",    L"PciRoot(0x5)/Pci(0x1,0x5)",    FALSE, INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,    DBW_8X, 0x34, 0xff,            "RiserB_Slim_x8_2",    L"PciRoot(0x6)/Pci(0x3,0x3)",    FALSE, INVALID_SETTING_ARRARY },    //reversal
    {    SYS_SLOT_PCIE_4,    DBW_8X, 0x33, 0xff,            "RiserB_Slim_x8_2",    L"PciRoot(0x6)/Pci(0x3,0x4)",    FALSE, INVALID_SETTING_ARRARY },

    //riser type 0x01, RISERB_NVMEx_DETECT_N = 1, Switch Riser: R3E6R-U2
    //SMBIOS_TYPE_9 RISERB_TYPE_0X01_SWITCH[] = {
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x2F, 0x6,                 "RiserB_Slot3",    L"PciRoot(0x5)/Pci(0x1,0x7)/Pci(0x0,0x0)/Pci(0x1,0x0)",    TRUE, RISERB_TYPE1_N(InputLow), RISERB_TYPE0_N(InputHigh), RISERB_NVME0_DETECT_N(InputHigh), RISERB_NVME1_DETECT_N(InputHigh) }, //TODO
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x2F, 0x5,                 "RiserB_Slot2",    L"PciRoot(0x5)/Pci(0x1,0x7)/Pci(0x0,0x0)/Pci(0x0,0x0)",    TRUE, INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x35, 0x4,                 "RiserB_Slot1",    L"PciRoot(0x6)/Pci(0x3,0x5)/Pci(0x0,0x0)/Pci(0x0,0x0)",    TRUE, INVALID_SETTING_ARRARY },

    //riser type 0x01, RISERB_NVMEx_DETECT_N = 1, no retimer connect
    //SMBIOS_TYPE_9 RISERB_TYPE_0X01_2SLOT[] = {
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x2F, 0x6,                 "RiserB_Slot2",    L"PciRoot(0x5)/Pci(0x1,0x7)",    FALSE, INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x35, 0x4,                 "RiserB_Slot1",    L"PciRoot(0x6)/Pci(0x3,0x5)",    FALSE, INVALID_SETTING_ARRARY },

    //riser type 0x01, RISERB_NVMEx_DETECT_N = 0, retimer connect
    //SMBIOS_TYPE_9 RISERB_TYPE_0X01_RETIMER_SLOT2[] = {
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x2A, 0xff,                "RiserB_Slot2",    L"PciRoot(0x5)/Pci(0x1,0x1)",    FALSE, RISERB_TYPE1_N(InputLow), RISERB_TYPE0_N(InputHigh), RISERB_NVME1_DETECT_N(InputLow), INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x29, 0xff,                "RiserB_Slot2",    L"PciRoot(0x5)/Pci(0x1,0x2)",    FALSE, INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x2C, 0xff,                "RiserB_Slot2",    L"PciRoot(0x5)/Pci(0x1,0x4)",    FALSE, INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x2D, 0xff,                "RiserB_Slot2",    L"PciRoot(0x5)/Pci(0x1,0x5)",    FALSE, INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x35, 0x4,                 "RiserB_Slot1",    L"PciRoot(0x6)/Pci(0x3,0x5)",    FALSE, INVALID_SETTING_ARRARY },

    //riser type 0x01, RISERB_NVMEx_DETECT_N = 0, retimer connect
    //SMBIOS_TYPE_9 RISERB_TYPE_0X01_RETIMER_SLOT1[] = {
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x2F, 0x6,                 "RiserB_Slot2",    L"PciRoot(0x5)/Pci(0x1,0x7)",    FALSE, RISERB_TYPE1_N(InputLow), RISERB_TYPE0_N(InputHigh), RISERB_NVME0_DETECT_N(InputLow), INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x31, 0xff,                "RiserB_Slot1",    L"PciRoot(0x6)/Pci(0x3,0x1)",    FALSE, INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x32, 0xff,                "RiserB_Slot1",    L"PciRoot(0x6)/Pci(0x3,0x2)",    FALSE, INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x33, 0xff,                "RiserB_Slot1",    L"PciRoot(0x6)/Pci(0x3,0x3)",    FALSE, INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x34, 0xff,                "RiserB_Slot1",    L"PciRoot(0x6)/Pci(0x3,0x4)",    FALSE, INVALID_SETTING_ARRARY },
    
    //SMBIOS_TYPE_9 RISERB_TYPE_0X00[] = {
    {    SYS_SLOT_PCIE_4,    DBW_8X, 0x2B, 0x4,                 "RiserB_Slot1",    L"PciRoot(0x5)/Pci(0x1,0x3)",    FALSE, RISERB_TYPE1_N(InputLow), RISERB_TYPE0_N(InputLow), RISERB_NVME0_DETECT_N(InputHigh),INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,    DBW_8X, 0x2E, 0x5,                 "RiserB_Slot2",    L"PciRoot(0x5)/Pci(0x1,0x6)",    FALSE, INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x35, 0x6,                 "RiserB_Slot3",    L"PciRoot(0x6)/Pci(0x3,0x5)",    FALSE, INVALID_SETTING_ARRARY },
    
    //riser type 0x01, RISERB_NVMEx_DETECT_N = 0, retimer connect
    //SMBIOS_TYPE_9 RISERB_TYPE_0X00_RETIMER_SLOT1[] = {
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x31, 0xff,                "RiserB_Slot3",    L"PciRoot(0x6)/Pci(0x3,0x1)",    FALSE, RISERB_TYPE1_N(InputLow), RISERB_TYPE0_N(InputLow), RISERB_NVME0_DETECT_N(InputLow), INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x32, 0xff,                "RiserB_Slot3",    L"PciRoot(0x6)/Pci(0x3,0x2)",    FALSE, INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x33, 0xff,                "RiserB_Slot3",    L"PciRoot(0x6)/Pci(0x3,0x3)",    FALSE, INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x34, 0xff,                "RiserB_Slot3",    L"PciRoot(0x6)/Pci(0x3,0x4)",    FALSE, INVALID_SETTING_ARRARY },    
    {    SYS_SLOT_PCIE_4,    DBW_8X, 0x2E, 0x5,                 "RiserB_Slot2",    L"PciRoot(0x5)/Pci(0x1,0x6)",    FALSE, INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,    DBW_8X, 0x2B, 0x4,                 "RiserB_Slot1",    L"PciRoot(0x5)/Pci(0x1,0x3)",    FALSE, INVALID_SETTING_ARRARY },
    
    //SMBIOS_TYPE_9 RISERB_TYPE_0X10[] = {
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x29, 0xff,            "RiserB_Slim_x8_2",    L"PciRoot(0x5)/Pci(0x1,0x1)",    FALSE, RISERB_TYPE1_N(InputHigh), RISERB_TYPE0_N(InputLow), INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x2A, 0xff,            "RiserB_Slim_x8_2",    L"PciRoot(0x5)/Pci(0x1,0x2)",    FALSE, INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x2C, 0xff,            "RiserB_Slim_x8_3",    L"PciRoot(0x5)/Pci(0x1,0x4)",    FALSE, INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x2D, 0xff,            "RiserB_Slim_x8_3",    L"PciRoot(0x5)/Pci(0x1,0x5)",    FALSE, INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x32, 0xff,            "RiserB_Slim_x8_1",    L"PciRoot(0x6)/Pci(0x3,0x1)",    FALSE, INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x31, 0xff,            "RiserB_Slim_x8_1",    L"PciRoot(0x6)/Pci(0x3,0x2)",    FALSE, INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x34, 0xff,            "RiserB_Slim_x8_0",    L"PciRoot(0x6)/Pci(0x3,0x3)",    FALSE, INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x33, 0xff,            "RiserB_Slim_x8_0",    L"PciRoot(0x6)/Pci(0x3,0x4)",    FALSE, INVALID_SETTING_ARRARY },    //REVERSAL

    //RISER C TYPE 0
    {SYS_SLOT_PCIE_4_X16,    DBW_8X, 0x3A, 0x8,                 "RiserC_Slot2",    L"PciRoot(0x7)/Pci(0x3,0x3)",    FALSE, RISERC_TYPE0_N(InputLow), INVALID_SETTING_ARRARY },
    {SYS_SLOT_PCIE_4_X16,    DBW_8X, 0x3D, 0x7,                 "RiserC_Slot1",    L"PciRoot(0x7)/Pci(0x3,0x6)",    FALSE, INVALID_SETTING_ARRARY },

    //RISER C SWITCH
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x3E, 0x7,                 "RiserC_Slot1",    L"PciRoot(0x7)/Pci(0x3,0x7)/Pci(0x0,0x0)/Pci(0x0,0x0)/Pci(0x0,0x0)/Pci(0x0,0x0)",    TRUE, RISERC_TYPE0_N(InputHigh), INVALID_SETTING_ARRARY }, //TODO
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x3E, 0x8,                 "RiserC_Slot2",    L"PciRoot(0x7)/Pci(0x3,0x7)/Pci(0x0,0x0)/Pci(0x4,0x0)/Pci(0x0,0x0)/Pci(0x0,0x0)",    TRUE, INVALID_SETTING_ARRARY },

    //RISER C TYPE 1, Only report when switch not present
    {    SYS_SLOT_PCIE_4,   DBW_16X, 0x3E, 0x8,                 "RiserC_Slot1",    L"PciRoot(0x7)/Pci(0x3,0x7)",    FALSE, RISERC_TYPE0_N(InputHigh), INVALID_SETTING_ARRARY },
    
    
    {SYS_SLOT_PCIE_4_X16,    DBW_8X, 0x38, 0xff,            "RiserC_Slim_x8_0",    L"PciRoot(0x7)/Pci(0x3,0x1)",    FALSE, RISERC_NVME_DETECT_N(InputLow), INVALID_SETTING_ARRARY },
    {SYS_SLOT_PCIE_4_X16,    DBW_8X, 0x39, 0xff,            "RiserC_Slim_x8_0",    L"PciRoot(0x7)/Pci(0x3,0x2)",    FALSE, INVALID_SETTING_ARRARY },
    {SYS_SLOT_PCIE_4_X16,    DBW_8X, 0x3B, 0xff,            "RiserC_Slim_x8_1",    L"PciRoot(0x7)/Pci(0x3,0x4)",    FALSE, INVALID_SETTING_ARRARY },
    {SYS_SLOT_PCIE_4_X16,    DBW_8X, 0x3C, 0xff,            "RiserC_Slim_x8_1",    L"PciRoot(0x7)/Pci(0x3,0x5)",    FALSE, INVALID_SETTING_ARRARY },

    {    SYS_SLOT_PCIE_4,    DBW_8X, 0x1A, 0xff,             "Onbrd_Slim_x8_2",    L"PciRoot(0x3)/Pci(0x3,0x1)",    FALSE, PCIEX16_CPU0_P3_N(InputHigh), INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,    DBW_8X, 0x1B, 0xff,             "Onbrd_Slim_x8_2",    L"PciRoot(0x3)/Pci(0x3,0x2)",    FALSE, INVALID_SETTING_ARRARY },

    {    SYS_SLOT_PCIE_4,   DBW_16X, 0xC,  0xff,                         "OCP",    L"PciRoot(0x1)/Pci(0x1,0x4)",    FALSE, OCP_PCIEX16_DET_N(InputLow), INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,    DBW_8X, 0xB,  0xff,                         "OCP",    L"PciRoot(0x1)/Pci(0x1,0x3)",    FALSE, OCP_PCIEX16_DET_N(InputHigh), INVALID_SETTING_ARRARY },

    {    SYS_SLOT_PCIE_4,    DBW_8X, 0x9,  0xff,             "Onbrd_Slim_x8_1",    L"PciRoot(0x1)/Pci(0x1,0x1)",    FALSE, OCP_PCIEX16_DET_N(InputHigh), SATA_CPU0_DETECT1_N(InputHigh), INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,    DBW_8X, 0xA,  0xff,             "Onbrd_Slim_x8_1",    L"PciRoot(0x1)/Pci(0x1,0x2)",    FALSE, INVALID_SETTING_ARRARY },

    {    SYS_SLOT_PCIE_4,    DBW_8X, 0x5,  0xff,                        "Mezz",    L"PciRoot(0x0)/Pci(0x1,0x6)",    FALSE, RAID_SLOT_NVME_DET_N(InputHigh), INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,    DBW_8X, 0x5,  0xff,              "Mezz_Slim_x8_1",    L"PciRoot(0x0)/Pci(0x1,0x6)",    FALSE, RAID_SLOT_NVME_DET_N(InputLow), INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,    DBW_8X, 0x4,  0xff,              "Mezz_Slim_x8_1",    L"PciRoot(0x0)/Pci(0x1,0x7)",    FALSE, INVALID_SETTING_ARRARY },

    {    SYS_SLOT_PCIE_4,    DBW_4X, 0x3,  0xff,             "Onbrd_Slim_x4_0",    L"PciRoot(0x0)/Pci(0x1,0x5)",    FALSE, SATA_CPU0_DETECT0_N(InputHigh), INVALID_SETTING_ARRARY },

    {    SYS_SLOT_PCIE_4,    DBW_8X, 0x21, 0xff,             "Onbrd_Slim_x8_3",    L"PciRoot(0x4)/Pci(0x1,0x1)",    FALSE, SATA_CPU1_DETECT0_N(InputHigh), INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,    DBW_8X, 0x22, 0xff,             "Onbrd_Slim_x8_3",    L"PciRoot(0x4)/Pci(0x1,0x2)",    FALSE, INVALID_SETTING_ARRARY },

    {    SYS_SLOT_PCIE_4,    DBW_8X, 0x24, 0xff,             "Onbrd_Slim_x8_4",    L"PciRoot(0x4)/Pci(0x1,0x3)",    FALSE, OCP_PCIEX8_DET_N(InputHigh), INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,    DBW_8X, 0x23, 0xff,             "Onbrd_Slim_x8_4",    L"PciRoot(0x4)/Pci(0x1,0x4)",    FALSE, INVALID_SETTING_ARRARY },
    {    0xFF, 0xFF, 0xFF,0xFF, NULL, NULL, FALSE, INVALID_SETTING_ARRARY },
};

GLOBAL_REMOVE_IF_UNREFERENCED UINT8 TotalCount = sizeof(SystemSlotsInfo) / sizeof(SMBIOS_TYPE_9);
