//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2017, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************

//****************************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//****************************************************************************
/** @file CrbSmbios.h
    CrbSmbios Header file.

**/
//****************************************************************************

#ifndef __OEM_SMBIOS__H__
#define __OEM_SMBIOS__H__
#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------

#include <Protocol/Smbios.h>
#include <Protocol/AmiSmbios.h>

//------------------------------------------------------------------------


#pragma pack (1)

#define MAX_SMBIOS_STRING_SIZE  0x40

typedef struct {
    SMBIOS_PORT_CONN_INFO     PortInfo;
    CHAR8*                    sIntRefDesig;
    CHAR8*                    sExtRefDesig;
} SMBIOS_TYPE_8;

typedef struct {
  SMBIOS_STRUCTURE            Hdr;
  SMBIOS_TABLE_STRING         SlotDesignation;
  UINT8                       SlotType;                 ///< The enumeration value from MISC_SLOT_TYPE.
  UINT8                       SlotDataBusWidth;         ///< The enumeration value from MISC_SLOT_DATA_BUS_WIDTH.
  UINT8                       CurrentUsage;             ///< The enumeration value from MISC_SLOT_USAGE.
  UINT8                       SlotLength;               ///< The enumeration value from MISC_SLOT_LENGTH.
  UINT16                      SlotID;
  MISC_SLOT_CHARACTERISTICS1  SlotCharacteristics1;
  MISC_SLOT_CHARACTERISTICS2  SlotCharacteristics2;
  //
  // Add for smbios 2.6
  //
  UINT16                      SegmentGroupNum;
  UINT8                       BusNum;
  UINT8                       DevFuncNum;
  //
  // Add for smbios 3.2
  //
  //UINT8                       DataBusWidth; //NO SUPPORT FOR OLD OS
  //UINT8                       PeerGroupingCount; //NO SUPPORT FOR OLD OS
  //MISC_SLOT_PEER_GROUP        PeerGroups[1]; //NO SUPPORT FOR OLD OS
} SMBIOS_TABLE_TYPE_9;

typedef struct {
    SMBIOS_ONBOARD_DEV_EXT_INFO     OnboardDevExtInfo;
    CHAR8*                          sRefDesignation;
} SMBIOS_TYPE_41;


typedef enum {
    SYS_SLOT_OTHER      =     0x1,
    SYS_SLOT_UNKNOWN    =     0x2,
    SYS_SLOT_ISA        =     0x3,
    SYS_SLOT_MCA        =     0x4,
    SYS_SLOT_EISA       =     0x5,
    SYS_SLOT_PCI        =     0x6,
    SYS_SLOT_PCMCIA     =     0x7,
    SYS_SLOT_VL_VESA    =     0x8,
    SYS_SLOT_PROPRIETARY    = 0x9,
    SYS_SLOT_PROC_CARD_SLOT = 0xA,
    SYS_SLOT_PROP_CARD_SLOT = 0xB,
    SYS_SLOT_IO_RISER_SLOT  = 0xC,
    SYS_SLOT_NU_BUS     =     0xD,
    SYS_SLOT_PCI_66MHZ  =     0xE,
    SYS_SLOT_AGP        =     0xF,
    SYS_SLOT_AGP_2X     =     0x10,
    SYS_SLOT_AGP_4X     =     0x11,
    SYS_SLOT_PCI_X      =     0x12,
    SYS_SLOT_AGP_8X     =     0x13,
    SYS_SLOT_M2_SK1_DP  =     0x14,
    SYS_SLOT_M2_SK1_SD  =     0x15,
    SYS_SLOT_M2_SK2     =     0x16,
    SYS_SLOT_M2_SK3     =     0x17,
    SYS_SLOT_MXM_TYPE1  =     0x18,
    SYS_SLOT_MXM_TYPE2  =     0x19,
    SYS_SLOT_MXM_TYPE3_STD =  0x1A,
    SYS_SLOT_MXM_TYPE3_HE =   0x1B,
    SYS_SLOT_MXM_TYPE4  =     0x1C,
    SYS_SLOT_MXM_3_A    =     0x1D,
    SYS_SLOT_MXM_3_B    =     0x1E,
    SYS_SLOT_PCIE_G2    =     0x1F,
    SYS_SLOT_PCIE_G3    =     0x20,
    SYS_SLOT_PC98_C20   =     0xA0,
    SYS_SLOT_PC98_C24   =     0xA1,
    SYS_SLOT_PC98_E     =     0xA2,
    SYS_SLOT_PC98_LOCAL =     0xA3,
    SYS_SLOT_PC98_CARD  =     0xA4,
    SYS_SLOT_PCIE       =     0xA5,
    SYS_SLOT_PCIE_X1    =     0xA6,
    SYS_SLOT_PCIE_X2    =     0xA7,
    SYS_SLOT_PCIE_X4    =     0xA8,
    SYS_SLOT_PCIE_X8    =     0xA9,
    SYS_SLOT_PCIE_X16   =     0xAA,
    SYS_SLOT_PCIE_2     =     0xAB,
    SYS_SLOT_PCIE_2_X1  =     0xAC,
    SYS_SLOT_PCIE_2_X2  =     0xAD,
    SYS_SLOT_PCIE_2_X4  =     0xAE,
    SYS_SLOT_PCIE_2_X8  =     0xAF,
    SYS_SLOT_PCIE_2_X16 =     0xB0,
    SYS_SLOT_PCIE_3     =     0xB1,
    SYS_SLOT_PCIE_3_X1  =     0xB2,
    SYS_SLOT_PCIE_3_X2  =     0xB3,
    SYS_SLOT_PCIE_3_X4  =     0xB4,
    SYS_SLOT_PCIE_3_X8  =     0xB5,
    SYS_SLOT_PCIE_3_X16 =     0xB6,
    SYS_SLOT_PCIE_4     =     0xB8,
    SYS_SLOT_PCIE_4_X1  =     0xB9,
    SYS_SLOT_PCIE_4_X2  =     0xBA,
    SYS_SLOT_PCIE_4_X4  =     0xBB,
    SYS_SLOT_PCIE_4_X8  =     0xBC,
    SYS_SLOT_PCIE_4_X16 =     0xBD
}SLOT_TYPE;

typedef enum {
    DBW_OTHER = 1,
    DBW_UNKNOWN,
    DBW_8BIT,
    DBW_16BIT,
    DBW_32BIT,
    DBW_64BIT,
    DBW_128BIT,
    DBW_1X,
    DBW_2X,
    DBW_4X,
    DBW_8X,
    DBW_12X,
    DBW_16X,
    DBW_32X,
}SLOT_DATA_BUS_WIDTH;

typedef enum {
    OBD_OTHER = 1,
    OBD_UNKNOWN,
    OBD_VIDEO,
    OBD_SCSI,
    OBD_ETHERNET,
    OBD_TOKEN_RING,
    OBD_SOUND,
    OBD_PATA,
    OBD_SATA,
    OBD_SAS,
}ONBOARD_DEVICE_TYPE;

//Chassis structure
typedef struct {
    UINT8   MbTypeId;     //Motherboard Type ID
    UINT8   ProductType[16];
    UINT8   ProductId;
    UINT8   ProductName[48];
    UINT8   ProductSn[32];

    UINT8   MaxCpuNum;
    UINT8   MaxMemNum;
    UINT8   MaxHddNum;
    UINT8   MaxFanNum;
    UINT8   MaxPsuNum;
    UINT8   MaxPcieNum;
    UINT8   MaxNicNum;
    UINT8   MaxMemChannelNum;
    UINT8   MaxMemDimmNum;

    UINT8   PresentCpuNum;
    UINT8   PresentMemNum;
    UINT8   PresentHddNum;
    UINT8   PresentNicNum;
    UINT8   PresentPcieNum;
    UINT8   PresentFanNum;
    UINT8   PresentPsuNum;
} PACKED_OEM_IPMI_CHASSIS_INFO;

#pragma pack()


/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2017, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
