#ifndef _OEM_PCIE_DATA_H_
#define _OEM_PCIE_DATA_H_
#include <Token.h>
#define OEMPCIEDEV_FORM_SET_GUID   {0x97cdc6be, 0x7792, 0x4782, 0xba, 0x6d, 0xe6, 0xe3, 0xbb, 0x56, 0x66, 0x66}
#define OEMPCIEDEV_VARSTORE_GUID   OEMPCIEDEV_FORM_SET_GUID
#define OEMPCIEDEV_FORM_SET_CLASS      0x66

#pragma pack(push, 1)
typedef struct _OEMPCIE_SETUP_DATA {
//<lvych00120160706+>//Machong 2014.11.14 Support for OpROM Selecting	{
	UINT8	RiserType[4];
	UINT8	PciDevEN[MAX_DEV_COUNT];
	UINT8	OpROMEN[PCIDEV_ENTRY_NUMBER];
//<bieyb-20180727 add function enable SRIOV by port+>
#if defined(OEM_SRIOV_ENABLE_BY_PORT_SUPPORT) && (OEM_SRIOV_ENABLE_BY_PORT_SUPPORT == 1)
	UINT8  SriovEN[MAX_DEV_COUNT];
#endif
//<bieyb-20180727 add function enable SRIOV by port->
#if defined(PCIE_LINK_SPEED_BY_DEVICE_SUPPORT) && (PCIE_LINK_SPEED_BY_DEVICE_SUPPORT == 1)
    UINT8   PciLnkSpeed[MAX_DEV_COUNT];//<Yaosr-20201201 Set PCIe link speed by device.>
#endif
//<lvych00120160706->//Machong 2014.11.14 Support for OpROM Selecting	}
} OEMPCIE_SETUP_DATA;
typedef struct _PCIE_POLICY_DATA {
    BOOLEAN                  IsNew;
    OEMPCIE_SETUP_DATA       OemPcieData;
} PCIE_POLICY_DATA;

#pragma pack(pop)
#endif

