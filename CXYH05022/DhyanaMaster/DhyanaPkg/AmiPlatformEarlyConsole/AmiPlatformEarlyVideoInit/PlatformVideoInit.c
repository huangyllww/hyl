//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************

/** @file PlatformVideoInit.c
    LIB driver for initializing the Video in PEI 

**/

#include <Include/AmiLib.h>
#include <Library/FabricRegisterAccLib.h>
#include <Library/BaseFabricTopologyLib.h>

#include <PlatformVideoInit.h>

extern EFI_GUID gVideoInitDoneSentGuid;
UINT32 VideoInit(VIDEO_PARAMETERS parameters);
UINT32 AspeedVgaInit(VIDEO_PARAMETERS Parameters, UINT8 vBus);
UINT32 AssignResources(VIDEO_PARAMETERS parameters, UINT8 vBus);
UINT32 EnableVgaDecode(VIDEO_PARAMETERS parameters);
UINT32 AssignBusNumbers(VIDEO_PARAMETERS parameters, UINT8 *VideoBus);
VOID PviPciWrite8(UINT32 CfgBase, UINT8 bus, UINT8 dev, UINT8 func, UINT16 reg, UINT8 data);
VOID PviPciWrite16(UINT32 CfgBase, UINT8 bus, UINT8 dev, UINT8 func, UINT16 reg, UINT16 data);
VOID PviPciWrite32(UINT32 CfgBase, UINT8 bus, UINT8 dev, UINT8 func, UINT16 reg, UINT32 data);
VOID PviPciRead8(UINT32 CfgBase, UINT8 bus, UINT8 dev, UINT8 func, UINT16 reg, UINT8 *data);
VOID PviPciRead16(UINT32 CfgBase, UINT8 bus, UINT8 dev, UINT8 func, UINT16 reg, UINT16 *data);
VOID PviPciRead32(UINT32 CfgBase, UINT8 bus, UINT8 dev, UINT8 func, UINT16 reg, UINT32 *data);

CONST EFI_PEI_PPI_DESCRIPTOR gVideoInitDoneSentSignalPpi = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gVideoInitDoneSentGuid,
  NULL
};

UINT8 FabricId [] = { 04, 0x24, 0x44, 0x64, 0x84, 0xA4, 0xC4, 0xE4 };

DEV_PATH mVideoVidPath [] = {
        {VGA_ROOT_PORT_BUS,0x1,VGA_ROOT_PORT_FUN},     //P2P bridge
        {VGA_ROOT_PORT_BUS + 1,0x0,0x0},     //PCIe-to-PCI Video Controller Bridge
        {VGA_ROOT_PORT_BUS + 2,0x0,0x0}      //Video Controller
};

UINT8 mVideoFontData[] = {
//0x20, ' '
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//0x21, '!'
0x00, 0x00, 0x18, 0x3c, 0x3c, 0x3c, 0x18, 0x18,
0x18, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,
//0x22, '"'
0x00, 0x66, 0x66, 0x66, 0x24, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//0x23, '#'
0x00, 0x00, 0x00, 0x6c, 0x6c, 0xfe, 0x6c, 0x6c,
0x6c, 0xfe, 0x6c, 0x6c, 0x00, 0x00, 0x00, 0x00,
//0x24, '$'
0x18, 0x18, 0x7c, 0xc6, 0xc2, 0xc0, 0x7c, 0x06,
0x06, 0x86, 0xc6, 0x7c, 0x18, 0x18, 0x00, 0x00,
//0x25, '%'
0x00, 0x00, 0x00, 0x00, 0xc2, 0xc6, 0x0c, 0x18,
0x30, 0x60, 0xc6, 0x86, 0x00, 0x00, 0x00, 0x00,
//0x26, '&'
0x00, 0x00, 0x38, 0x6c, 0x6c, 0x38, 0x76, 0xdc,
0xcc, 0xcc, 0xcc, 0x76, 0x00, 0x00, 0x00, 0x00,
//0x27, '''
0x00, 0x30, 0x30, 0x30, 0x60, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//0x28, '('
0x00, 0x00, 0x0c, 0x18, 0x30, 0x30, 0x30, 0x30,
0x30, 0x30, 0x18, 0x0c, 0x00, 0x00, 0x00, 0x00,
//0x29, ')'
0x00, 0x00, 0x30, 0x18, 0x0c, 0x0c, 0x0c, 0x0c,
0x0c, 0x0c, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00,
//0x2a, '*'
0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x3c, 0xff,
0x3c, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//0x2b, '+'
0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x7e,
0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//0x2c, ','
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x18, 0x18, 0x18, 0x30, 0x00, 0x00, 0x00,
//0x2d, '-'
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//0x2e, '.'
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,
//0x2f, '/'
0x00, 0x00, 0x00, 0x00, 0x02, 0x06, 0x0c, 0x18,
0x30, 0x60, 0xc0, 0x80, 0x00, 0x00, 0x00, 0x00,
//0x30, '0'
0x00, 0x00, 0x3c, 0x66, 0xc3, 0xc3, 0xdb, 0xdb,
0xc3, 0xc3, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00,
//0x31, '1'
0x00, 0x00, 0x18, 0x38, 0x78, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x7e, 0x00, 0x00, 0x00, 0x00,
//0x32, '2'
0x00, 0x00, 0x7c, 0xc6, 0x06, 0x0c, 0x18, 0x30,
0x60, 0xc0, 0xc6, 0xfe, 0x00, 0x00, 0x00, 0x00,
//0x33, '3'
0x00, 0x00, 0x7c, 0xc6, 0x06, 0x06, 0x3c, 0x06,
0x06, 0x06, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00,
//0x34, '4'
0x00, 0x00, 0x0c, 0x1c, 0x3c, 0x6c, 0xcc, 0xfe,
0x0c, 0x0c, 0x0c, 0x1e, 0x00, 0x00, 0x00, 0x00,
//0x35, '5'
0x00, 0x00, 0xfe, 0xc0, 0xc0, 0xc0, 0xfc, 0x06,
0x06, 0x06, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00,
//0x36, '6'
0x00, 0x00, 0x38, 0x60, 0xc0, 0xc0, 0xfc, 0xc6,
0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00,
//0x37, '7'
0x00, 0x00, 0xfe, 0xc6, 0x06, 0x06, 0x0c, 0x18,
0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00,
//0x38, '8'
0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0x7c, 0xc6,
0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00,
//0x39, '9'
0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0x7e, 0x06,
0x06, 0x06, 0x0c, 0x78, 0x00, 0x00, 0x00, 0x00,
//0x3a, ':'
0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00,
0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00,
//0x3b, ';'
0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00,
0x00, 0x18, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00,
//0x3c, '<'
0x00, 0x00, 0x00, 0x06, 0x0c, 0x18, 0x30, 0x60,
0x30, 0x18, 0x0c, 0x06, 0x00, 0x00, 0x00, 0x00,
//0x3d, '='
0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00,
0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//0x3e, '>'
0x00, 0x00, 0x00, 0x60, 0x30, 0x18, 0x0c, 0x06,
0x0c, 0x18, 0x30, 0x60, 0x00, 0x00, 0x00, 0x00,
//0x3f, '?'
0x00, 0x00, 0x7c, 0xc6, 0xc6, 0x0c, 0x18, 0x18,
0x18, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,
//0x40, '@'
0x00, 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xde, 0xde,
0xde, 0xdc, 0xc0, 0x7c, 0x00, 0x00, 0x00, 0x00,
//0x41, 'A'
0x00, 0x00, 0x10, 0x38, 0x6c, 0xc6, 0xc6, 0xfe,
0xc6, 0xc6, 0xc6, 0xc6, 0x00, 0x00, 0x00, 0x00,
//0x42, 'B'
0x00, 0x00, 0xfc, 0x66, 0x66, 0x66, 0x7c, 0x66,
0x66, 0x66, 0x66, 0xfc, 0x00, 0x00, 0x00, 0x00,
//0x43, 'C'
0x00, 0x00, 0x3c, 0x66, 0xc2, 0xc0, 0xc0, 0xc0,
0xc0, 0xc2, 0x66, 0x3c, 0x00, 0x00, 0x00, 0x00,
//0x44, 'D'
0x00, 0x00, 0xf8, 0x6c, 0x66, 0x66, 0x66, 0x66,
0x66, 0x66, 0x6c, 0xf8, 0x00, 0x00, 0x00, 0x00,
//0x45, 'E'
0x00, 0x00, 0xfe, 0x66, 0x62, 0x68, 0x78, 0x68,
0x60, 0x62, 0x66, 0xfe, 0x00, 0x00, 0x00, 0x00,
//0x46, 'F'
0x00, 0x00, 0xfe, 0x66, 0x62, 0x68, 0x78, 0x68,
0x60, 0x60, 0x60, 0xf0, 0x00, 0x00, 0x00, 0x00,
//0x47, 'G'
0x00, 0x00, 0x3c, 0x66, 0xc2, 0xc0, 0xc0, 0xde,
0xc6, 0xc6, 0x66, 0x3a, 0x00, 0x00, 0x00, 0x00,
//0x48, 'H'
0x00, 0x00, 0xc6, 0xc6, 0xc6, 0xc6, 0xfe, 0xc6,
0xc6, 0xc6, 0xc6, 0xc6, 0x00, 0x00, 0x00, 0x00,
//0x49, 'I'
0x00, 0x00, 0x3c, 0x18, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x3c, 0x00, 0x00, 0x00, 0x00,
//0x4a, 'J'
0x00, 0x00, 0x1e, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
0xcc, 0xcc, 0xcc, 0x78, 0x00, 0x00, 0x00, 0x00,
//0x4b, 'K'
0x00, 0x00, 0xe6, 0x66, 0x66, 0x6c, 0x78, 0x78,
0x6c, 0x66, 0x66, 0xe6, 0x00, 0x00, 0x00, 0x00,
//0x4c, 'L'
0x00, 0x00, 0xf0, 0x60, 0x60, 0x60, 0x60, 0x60,
0x60, 0x62, 0x66, 0xfe, 0x00, 0x00, 0x00, 0x00,
//0x4d, 'M'
0x00, 0x00, 0xc3, 0xe7, 0xff, 0xff, 0xdb, 0xc3,
0xc3, 0xc3, 0xc3, 0xc3, 0x00, 0x00, 0x00, 0x00,
//0x4e, 'N'
0x00, 0x00, 0xc6, 0xe6, 0xf6, 0xfe, 0xde, 0xce,
0xc6, 0xc6, 0xc6, 0xc6, 0x00, 0x00, 0x00, 0x00,
//0x4f, 'O'
0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6,
0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00,
//0x50, 'P'
0x00, 0x00, 0xfc, 0x66, 0x66, 0x66, 0x7c, 0x60,
0x60, 0x60, 0x60, 0xf0, 0x00, 0x00, 0x00, 0x00,
//0x51, 'Q'
0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6,
0xc6, 0xd6, 0xde, 0x7c, 0x0c, 0x0e, 0x00, 0x00,
//0x52, 'R'
0x00, 0x00, 0xfc, 0x66, 0x66, 0x66, 0x7c, 0x6c,
0x66, 0x66, 0x66, 0xe6, 0x00, 0x00, 0x00, 0x00,
//0x53, 'S'
0x00, 0x00, 0x7c, 0xc6, 0xc6, 0x60, 0x38, 0x0c,
0x06, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00,
//0x54, 'T'
0x00, 0x00, 0xff, 0xdb, 0x99, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x3c, 0x00, 0x00, 0x00, 0x00,
//0x55, 'U'
0x00, 0x00, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6,
0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00,
//0x56, 'V'
0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
0xc3, 0x66, 0x3c, 0x18, 0x00, 0x00, 0x00, 0x00,
//0x57, 'W'
0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xdb,
0xdb, 0xff, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00,
//0x58, 'X'
0x00, 0x00, 0xc3, 0xc3, 0x66, 0x3c, 0x18, 0x18,
0x3c, 0x66, 0xc3, 0xc3, 0x00, 0x00, 0x00, 0x00,
//0x59, 'Y'
0x00, 0x00, 0xc3, 0xc3, 0xc3, 0x66, 0x3c, 0x18,
0x18, 0x18, 0x18, 0x3c, 0x00, 0x00, 0x00, 0x00,
//0x5a, 'Z'
0x00, 0x00, 0xff, 0xc3, 0x86, 0x0c, 0x18, 0x30,
0x60, 0xc1, 0xc3, 0xff, 0x00, 0x00, 0x00, 0x00,
//0x5b, '['
0x00, 0x00, 0x3c, 0x30, 0x30, 0x30, 0x30, 0x30,
0x30, 0x30, 0x30, 0x3c, 0x00, 0x00, 0x00, 0x00,
//0x5c, '\'
0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0x70, 0x38,
0x1c, 0x0e, 0x06, 0x02, 0x00, 0x00, 0x00, 0x00,
//0x5d, ']'
0x00, 0x00, 0x3c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
0x0c, 0x0c, 0x0c, 0x3c, 0x00, 0x00, 0x00, 0x00,
//0x5e, '^'
0x10, 0x38, 0x6c, 0xc6, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//0x5f, '_'
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00,
//0x60, '`'
0x30, 0x30, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//0x61, 'a'
0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x0c, 0x7c,
0xcc, 0xcc, 0xcc, 0x76, 0x00, 0x00, 0x00, 0x00,
//0x62, 'b'
0x00, 0x00, 0xe0, 0x60, 0x60, 0x78, 0x6c, 0x66,
0x66, 0x66, 0x66, 0x7c, 0x00, 0x00, 0x00, 0x00,
//0x63, 'c'
0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xc6, 0xc0,
0xc0, 0xc0, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00,
//0x64, 'd'
0x00, 0x00, 0x1c, 0x0c, 0x0c, 0x3c, 0x6c, 0xcc,
0xcc, 0xcc, 0xcc, 0x76, 0x00, 0x00, 0x00, 0x00,
//0x65, 'e'
0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xc6, 0xfe,
0xc0, 0xc0, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00,
//0x66, 'f'
0x00, 0x00, 0x38, 0x6c, 0x64, 0x60, 0xf0, 0x60,
0x60, 0x60, 0x60, 0xf0, 0x00, 0x00, 0x00, 0x00,
//0x67, 'g'
0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0xcc, 0xcc,
0xcc, 0xcc, 0xcc, 0x7c, 0x0c, 0xcc, 0x78, 0x00,
//0x68, 'h'
0x00, 0x00, 0xe0, 0x60, 0x60, 0x6c, 0x76, 0x66,
0x66, 0x66, 0x66, 0xe6, 0x00, 0x00, 0x00, 0x00,
//0x69, 'i'
0x00, 0x00, 0x18, 0x18, 0x00, 0x38, 0x18, 0x18,
0x18, 0x18, 0x18, 0x3c, 0x00, 0x00, 0x00, 0x00,
//0x6a, 'j'
0x00, 0x00, 0x06, 0x06, 0x00, 0x0e, 0x06, 0x06,
0x06, 0x06, 0x06, 0x06, 0x66, 0x66, 0x3c, 0x00,
//0x6b, 'k'
0x00, 0x00, 0xe0, 0x60, 0x60, 0x66, 0x6c, 0x78,
0x78, 0x6c, 0x66, 0xe6, 0x00, 0x00, 0x00, 0x00,
//0x6c, 'l'
0x00, 0x00, 0x38, 0x18, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x3c, 0x00, 0x00, 0x00, 0x00,
//0x6d, 'm'
0x00, 0x00, 0x00, 0x00, 0x00, 0xe6, 0xff, 0xdb,
0xdb, 0xdb, 0xdb, 0xdb, 0x00, 0x00, 0x00, 0x00,
//0x6e, 'n'
0x00, 0x00, 0x00, 0x00, 0x00, 0xdc, 0x66, 0x66,
0x66, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00,
//0x6f, 'o'
0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xc6, 0xc6,
0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00,
//0x70, 'p'
0x00, 0x00, 0x00, 0x00, 0x00, 0xdc, 0x66, 0x66,
0x66, 0x66, 0x66, 0x7c, 0x60, 0x60, 0xf0, 0x00,
//0x71, 'q'
0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0xcc, 0xcc,
0xcc, 0xcc, 0xcc, 0x7c, 0x0c, 0x0c, 0x1e, 0x00,
//0x72, 'r'
0x00, 0x00, 0x00, 0x00, 0x00, 0xdc, 0x76, 0x66,
0x60, 0x60, 0x60, 0xf0, 0x00, 0x00, 0x00, 0x00,
//0x73, 's'
0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xc6, 0x60,
0x38, 0x0c, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00,
//0x74, 't'
0x00, 0x00, 0x10, 0x30, 0x30, 0xfc, 0x30, 0x30,
0x30, 0x30, 0x36, 0x1c, 0x00, 0x00, 0x00, 0x00,
//0x75, 'u'
0x00, 0x00, 0x00, 0x00, 0x00, 0xcc, 0xcc, 0xcc,
0xcc, 0xcc, 0xcc, 0x76, 0x00, 0x00, 0x00, 0x00,
//0x76, 'v'
0x00, 0x00, 0x00, 0x00, 0x00, 0xc3, 0xc3, 0xc3,
0xc3, 0x66, 0x3c, 0x18, 0x00, 0x00, 0x00, 0x00,
//0x77, 'w'
0x00, 0x00, 0x00, 0x00, 0x00, 0xc3, 0xc3, 0xc3,
0xdb, 0xdb, 0xff, 0x66, 0x00, 0x00, 0x00, 0x00,
//0x78, 'x'
0x00, 0x00, 0x00, 0x00, 0x00, 0xc3, 0x66, 0x3c,
0x18, 0x3c, 0x66, 0xc3, 0x00, 0x00, 0x00, 0x00,
//0x79, 'y'
0x00, 0x00, 0x00, 0x00, 0x00, 0xc6, 0xc6, 0xc6,
0xc6, 0xc6, 0xc6, 0x7e, 0x06, 0x0c, 0xf8, 0x00,
//0x7a, 'z'
0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xcc, 0x18,
0x30, 0x60, 0xc6, 0xfe, 0x00, 0x00, 0x00, 0x00,
};

FONT_MAP_INFO mVideoFontMapInfo = {
    mVideoFontData,
    0x20, //start character ' ' (space)
    0x5b, //character count -- includes all characters from ' ' to 'z' (see below)
    0x10, //16-bits per character.
    0x00, //use the first font table.
};

/**
    Initialize IO registers for each Die evenly.
 
    @param[in]         SocketNumber             System socket count
    @param[in]         DieNumberPerSocket       Die number per socket
 
 **/
EFI_STATUS
AmiFabricInitIo (
  IN       UINT8                SocketNumber,
  IN       UINT8                DieNumberPerSocket
  )
{
  UINT8             i,j,x,y;
  UINT8             RegIndex;
  UINT8             DstFabricIDSysOffset;
  UINT32            IoBase;
  UINT32            IoSize;
  UINT32            IoSizeForThisDie;
  UINT32            LegacyIoSize;
  X86IO_BASE_ADDR   IoBaseReg;
  X86IO_LIMIT_ADDR  IoLimitReg;

  IoBase = 0;
  RegIndex = 0;
  LegacyIoSize = X86_LEGACY_IO_SIZE;

  // most IO registers have a 16bit limit
  // reserve LegacyIoSize for Die 0
  IoSize = ((0x10000 - LegacyIoSize) / (SocketNumber * DieNumberPerSocket)) & 0xFFFFF000; // 12 bit alignment

  for (i = 0; i < SocketNumber; i++) {
    for (j = 0; j < DieNumberPerSocket; j++) {
      DstFabricIDSysOffset = (UINT8) FabricTopologyGetDieSystemOffset (i, j);
      if ((i == (SocketNumber - 1)) && (j == (DieNumberPerSocket - 1))) {
        // 1. first, check if it's the last Die. Set IO limit up to 0x1FFFFFF for last Die.
        IoSizeForThisDie = X86IO_LIMIT - IoBase;
      } else if ((i == 0) && (j == 0)) {
        // 2. second, if it's Die 0 and it's not the last Die, plus size with LegacyIoSize
        IoSizeForThisDie = IoSize + LegacyIoSize;
      } else {
        IoSizeForThisDie = IoSize;
      }

      //FabricZenZpSetIoReg (SocketNumber, DieNumberPerSocket, RegIndex, DstFabricIDSysOffset, IoBase, IoSizeForThisDie);
      IoBaseReg.Value  = 0;
      IoLimitReg.Value = FabricRegisterAccRead (0, 0, 0x0, (X86IO_LIMIT_ADDRESS_REG0 + RegIndex * 8), FABRIC_REG_ACC_BC);

      for (x = 0; x < SocketNumber; x++) {
        for (y = 0; y < DieNumberPerSocket; y++) {
          IoBaseReg.Field.RE = 1;
          IoBaseReg.Field.WE = 1;
          IoBaseReg.Field.IOBase = (IoBase & 0xFFFFF000) >> 12;
          FabricRegisterAccWrite (x, y, 0x0, (X86IO_BASE_ADDRESS_REG0 + RegIndex * 8), FABRIC_REG_ACC_BC, IoBaseReg.Value, TRUE);

          IoLimitReg.Field.DstFabricID &= 0x1F;
          IoLimitReg.Field.DstFabricID |= DstFabricIDSysOffset;
          IoLimitReg.Field.IOLimit = ((IoBase + IoSizeForThisDie - 1) & 0xFFFFF000) >> 12;
          FabricRegisterAccWrite (x, y, 0x0, (X86IO_LIMIT_ADDRESS_REG0 + RegIndex * 8), FABRIC_REG_ACC_BC, IoLimitReg.Value, TRUE);
        }
      }
   
      IoBase += IoSizeForThisDie;
      RegIndex++;
    }
  }

  return EFI_SUCCESS;
}

/**
    Initializes video controller with VGA standard init. 

    @param   None 

    @retval  EFI_NOT_FOUND
**/
EFI_STATUS 
AmiVideoInit
(
    IN  EFI_PEI_SERVICES  **PeiServices
)
{
    VIDEO_PARAMETERS    *DevInfo;
    EFI_STATUS          Status;
    UINT32              MmioBase, MmioLimit, MmioCtrl, MmioSize, IoBase;
    UINT8               Reg8;
    
    Status = (*PeiServices)->AllocatePool( PeiServices,
        sizeof(VIDEO_PARAMETERS),
        &DevInfo
        );
    if (EFI_ERROR(Status)) {
        return Status;    
    }
    
    Status = AmiFabricInitIo(2,4);
    if (EFI_ERROR(Status)) {
            return Status;    
        }
    /*
    Fixed memory regions:
    0xED00_0000..0xFFFF_FFFF    RB0
    0xEC00_0000..0xECFF_FFFF    RB1
    0xEB00_0000..0xEBFF_FFFF    RB2
    0xEA00_0000..0xEAFF_FFFF    RB3
    0xE900_0000..0xE9FF_FFFF    RB4
    0xE800_0000..0xE8FF_FFFF    RB5
    0xE700_0000..0xE7FF_FFFF    RB6
    0xE600_0000..0xE6FF_FFFF    RB7
    0xE000_0000..0xE5FF_FFFF    -Early GFX 96MB
    */

    MmioBase=0xE000;
    MmioLimit=0xE5FF;
    MmioCtrl = (FabricId[VGA_ROOT_PORT_BUS/0x10] << 4)| 3;

    FabricRegisterAccWrite (0, 0, 0, (MMIO_BASE_ADDRESS_REG_0 + 0x80), FABRIC_REG_ACC_BC, MmioBase,0);
    FabricRegisterAccWrite (0, 0, 0, (MMIO_LIMIT_ADDRESS_REG_0 + 0x80), FABRIC_REG_ACC_BC, MmioLimit,0);
    FabricRegisterAccWrite (0, 0, 0, (MMIO_CONTROL_ADDRESS_REG_0 + 0x80), FABRIC_REG_ACC_BC, MmioCtrl,0);
    
//    MmioBase = FabricRegisterAccRead (0, 0, 0, (MMIO_BASE_ADDRESS_REG_0 + 0x10), FABRIC_REG_ACC_BC);
//    MmioLimit = FabricRegisterAccRead (0, 0, 0, (MMIO_LIMIT_ADDRESS_REG_0 + 0x10), FABRIC_REG_ACC_BC);
    IoBase = FabricRegisterAccRead (0, 0, 0, (X86IO_BASE_ADDRESS_REG0 + 8 * (VGA_ROOT_PORT_BUS/0x10)), FABRIC_REG_ACC_BC);
    
    MmioBase = (MmioBase & 0xFFFF) << 16;
    MmioLimit = ((MmioLimit << 16) | 0xFFFF);
    MmioSize = (MmioLimit + 1) - MmioBase;
    IoBase = (IoBase & 0xF000);
    // for Die0 IO base starts from 0, resources given to PCIE - from 0x1000,
    // adjust for that case
    if(IoBase == 0) IoBase = 0x1000;
        
    // Fill the platform related parameters for Video Init
    DevInfo->RootBus            = mVideoVidPath[0].Bus;
    DevInfo->DevPath            = mVideoVidPath;   //Pointer to a buffer of dev path structure that define the location of the video controller in the PCI topology  
    DevInfo->DevPathEntries     = 3;                    //Count of entries in the preceding buffer.        
    DevInfo->PciExpressCfgBase  = (UINTN) PcdGet64(PcdPciExpressBaseAddress);           //The base address of PCI Express Memory Mapped Configuration Space.
    DevInfo->MemBase            = MmioBase;           //The base address of the region where MMIO BARs are assigned when configuring the VGA controller
    DevInfo->MemSizeMax         = MmioSize;           //The maximum amount of memory to allow during BAR assignment.
    DevInfo->IoBase             = IoBase;               //The base address of the region where IO BARs are assigned when configuring the VGA controller    
    DevInfo->IoSizeMax          = 0x1000;               //The maximum amount of IO to allow during BAR assignment. 
    DevInfo->FontMap            = &mVideoFontMapInfo;   //Pointer to a buffer of font map information structures that define the font map(s) to load.
    DevInfo->FontMapCount       = 1;                    //Count of entries in the preceding buffer.
    DevInfo->PeiServices        = PeiServices;

    // Check if bus need to be adjusted - 1P/2P case, assuming 0x10 Buses per Die for 2P
    // Read Base bus number for Die1
    PviPciRead8(DevInfo->PciExpressCfgBase, 0, 0x18, 0, 0xA6, &Reg8);
    if(Reg8 != 0x10){
        DevInfo->RootBus = Reg8 * ( DevInfo->RootBus/0x10);
    }
	
    // Initialize VGA Hardware
    Status = VideoInit (*DevInfo); 
    
    if(Status == 0){
        Status = (*PeiServices)->InstallPpi(PeiServices, &gVideoInitDoneSentSignalPpi);
    }
    
    return Status;
}

UINT32 VideoInit(VIDEO_PARAMETERS parameters)
{
    UINT8   vBus;
    UINT32  Status;
    UINT8   devPathIdx;
       
    Status = AssignBusNumbers(parameters, &vBus);
    if (Status != VIDEO_SUCCESS) return Status;
    
    Status = EnableVgaDecode(parameters);
    if (Status != VIDEO_SUCCESS) return Status;
    
    Status = AssignResources(parameters, vBus);
    if (Status != VIDEO_SUCCESS) return Status;
        
    Status = AspeedVgaInit(parameters, vBus);
    if (Status != VIDEO_SUCCESS) return Status;
    
    // Clear Secondary and subordinate bus numbers in bridges
    for (devPathIdx = 0; devPathIdx < parameters.DevPathEntries - 1; devPathIdx++) {
        PviPciWrite8(parameters.PciExpressCfgBase, parameters.RootBus + devPathIdx, parameters.DevPath[devPathIdx].Dev, parameters.DevPath[devPathIdx].Func, 0x19, 0);
        PviPciWrite8(parameters.PciExpressCfgBase, parameters.RootBus + devPathIdx, parameters.DevPath[devPathIdx].Dev, parameters.DevPath[devPathIdx].Func, 0x1A, 0);
    }
    
    return VIDEO_SUCCESS;
}

/**
    Assigns bus numbers along the device path from the root bus to the video controller.
 
    @param parameters - VIDEO_PARAMETERS structure specifying parameters for video initialization
    @param VideoBus - pointer to a UINT8 which will be updated with the bus number assigned to the video controller.
 
    @retval VIDEO_SUCCESS if everything worked, one of the video error codes if something did not work or bad input was provided.
 **/
UINT32 
AssignBusNumbers(
  VIDEO_PARAMETERS parameters, 
  UINT8 *VideoBus)
{
    UINT8   bus, dev, func, devPathIdx,data8;
    UINT8   Sbus, SubBus;
    UINT16  data16;
    UINT32  base;
    UINT8   PrimaryBus;

    bus = parameters.RootBus;
    base = parameters.PciExpressCfgBase;
    //
    // Iterate through the bridges in the device path and assign bus numbers
    //
       
    bus = parameters.RootBus;
    for (devPathIdx = 0; devPathIdx < parameters.DevPathEntries-1; devPathIdx++) {      
        dev = parameters.DevPath[devPathIdx].Dev;
        func = parameters.DevPath[devPathIdx].Func;
        DEBUG((EFI_D_INFO,"[Early Video] Assign Bus Number 1: Bus: %x | Dev: %x | Fun %x \n", bus, dev, func));

        //
        // Read header type and validate that this is a bridge
        //
        
        //Find primary bus number
        PviPciRead8(base, bus, dev, func, 0x18, &PrimaryBus);
        
        //Assign bus number to enable IDs
        PviPciWrite8(base, bus, dev, func, 0x18, bus);
        
        PviPciRead16(base, bus, dev, func, 0x02, &data16);
        if (data16 == 0xFFFF) {
            DEBUG((EFI_D_ERROR,"Error: Vendor ID is not valid %x \n", data16));
            //Reassign old primary bus number
            PviPciWrite8(base, bus, dev, func, 0x18, PrimaryBus);
            return VIDEO_ERR_BAD_DEVPATH;
        }
        
        PviPciRead16(base, bus, dev, func, 0x00, &data16);
        if (data16 == 0xFFFF) {
            DEBUG((EFI_D_ERROR,"Error: Device ID is not valid %x \n", data16));
            PviPciWrite8(base, bus, dev, func, 0x18, PrimaryBus);
            return VIDEO_ERR_BAD_DEVPATH;
        }
        
        PviPciRead8(base, bus, dev, func, 0x0E, &data8);
        if ( (data8 & 0x7f) != 0x01) {
            DEBUG((EFI_D_ERROR,"Error: Device is not bridge %x \n", data16));
            PviPciWrite8(base, bus, dev, func, 0x18, PrimaryBus);
            return VIDEO_ERR_BAD_DEVPATH;
        }        

        Sbus = bus + 1;
        if(devPathIdx == 0){
            SubBus = bus + 2; 
        } else {
            SubBus = bus + 1;
        }

        //Assign primary bus number
        PviPciWrite8(base, bus, dev, func, 0x18, bus);
        
        //Assign secondary and subordinate bus numbers
        PviPciWrite8(base, bus, dev, func, 0x19, Sbus);
        PviPciWrite8(base, bus, dev, func, 0x1A, SubBus);
        
        bus++;
    }
    DEBUG((EFI_D_INFO,"[Early Video] Assign Bus Number 2: Bus: %x | Dev: %x | Fun %x \n", bus, dev, func));

    //
    // Read header type and validate that this is an endpoint device
    //
    dev = parameters.DevPath[devPathIdx].Dev;
    func = parameters.DevPath[devPathIdx].Func;
    
    PviPciRead8(base, bus, dev, func, 0x0E, &data8);
    PviPciRead16(base, bus, dev, func, 0x00, &data16);
    PviPciRead16(base, bus, dev, func, 0x02, &data16);

    if ( (data8 & 0x7f) != 0x00) {
        return VIDEO_ERR_BAD_DEVPATH;
    }

    *VideoBus = bus;

    return VIDEO_SUCCESS;
}

/**
    Enables legacy VGA decode down to the video controller
 
    @param parameters - VIDEO_PARAMETERS structure specifying parameters for video initialization
 
    @retval VIDEO_SUCCESS if everything worked, one of the video error codes if something did not work or bad input was provided.
 **/
UINT32 
EnableVgaDecode(
  VIDEO_PARAMETERS parameters)
{
    UINT8  bus, dev, func, devPathIdx, Node;
    UINT16 data16;
    UINT32 base, data32;

    base = parameters.PciExpressCfgBase;
    bus = parameters.RootBus;

    //
    // Iterate through all the bridges in the dev path and enable Mem/IO decode and VGA decode.
    //
    for (devPathIdx = 0; devPathIdx < parameters.DevPathEntries - 1; devPathIdx++) {
        dev = parameters.DevPath[devPathIdx].Dev;
        func = parameters.DevPath[devPathIdx].Func;
        
        //
        //Enable VGA decode
        //
        PviPciRead16(base, bus, dev, func, 0x3E, &data16);
        data16 |= 0x08;
        PviPciWrite16(base, bus, dev, func, 0x3E, data16);
        
        //
        //Enable Mem & IO decode
        //
        PviPciRead16(base, bus, dev, func, 0x04, &data16);
        data16 |= 0x03;
        PviPciWrite16(base, bus, dev, func, 0x04, data16);        

        //
        // Next device in the path should be on bus+1
        //
        bus++;
    }
    
    //
    //Enable Mem & IO decode for endpoint VGA device
    //
    dev = parameters.DevPath[devPathIdx].Dev;
    func = parameters.DevPath[devPathIdx].Func;

    PviPciRead16(base, bus, dev, func, 0x04, &data16);
    data16 |= 0x03;
    PviPciWrite16(base, bus, dev, func, 0x04, data16);
    
    //
    //Enable VGAEn (D18F0x080)
    //
    for (Node = 0; Node < 0x8; Node++) { //SystemDieCount = 8
            PviPciRead32(base, 0x0, (0x18+Node), 0x0, 0x80, &data32);
            data32 |= (FabricId[VGA_ROOT_PORT_BUS/0x10] << 4);
            data32 |= 0x01;
            PviPciWrite32(base, 0x0, (0x18+Node), 0x0, 0x80, data32);
    }
    
    return VIDEO_SUCCESS;
}

/**
    Program BARs in the video controller and apertures in the upstream bridges
    
    @param VIDEO_PARAMETERS structure specifying parameters for video initialization
    @param vBus - bus number assigned to the video controller
 
    @retval VIDEO_SUCCESS if everything worked, one of the video error codes if something did not work or bad input was provided.
 **/
UINT32 
AssignResources(
  VIDEO_PARAMETERS parameters, 
  UINT8 vBus)
{
    UINT32 NextMemAddr, NextIoAddr, CurrentReqSize, data32, base;
    UINT16 barOffset;
    UINT8 vDev, vFunc, bus, devPathIdx;

    //
    // Sanity check on input.
    //
    if ((parameters.MemBase & 0xFFFFF) != 0) return VIDEO_ERR_BAD_PARM;
    if ((parameters.MemSizeMax & 0xFFFFF) != 0) return VIDEO_ERR_BAD_PARM;
    if ((parameters.IoBase & 0xFFF) != 0) return VIDEO_ERR_BAD_PARM;
    if ((parameters.IoSizeMax & 0xFFF) != 0) return VIDEO_ERR_BAD_PARM;

    NextMemAddr = parameters.MemBase;
    NextIoAddr = parameters.IoBase;
    base = parameters.PciExpressCfgBase;

    //
    // Determine Resource requirements and assign resources to the video device (last device in the path).
    //
    vDev = parameters.DevPath[parameters.DevPathEntries - 1].Dev;
    vFunc = parameters.DevPath[parameters.DevPathEntries - 1].Func;

    for (barOffset = 0x10; barOffset < 0x28; barOffset+=4) {
        //
        //Write all FFs to the bar to determine bar size
        //
        PviPciWrite32(base, vBus, vDev, vFunc, barOffset, 0xFFFFFFFF);
        PviPciRead32(base, vBus, vDev, vFunc, barOffset, &data32);

        //
        //Skip if BAR is not used.
        //
        if (data32 == 0x0) continue;

        CurrentReqSize = ~(data32 & 0xFFFFFFF0);

        //
        // Determine placement for this bar.
        //
        if ( (data32 & 0x01) == 0) { // memory bar
            if ( (NextMemAddr & CurrentReqSize) != 0 ) {
                NextMemAddr = (NextMemAddr + CurrentReqSize + 1) & ~CurrentReqSize;
            }
            data32 &= 0xF;
            data32 |= NextMemAddr;
            NextMemAddr+=CurrentReqSize+1;
        } else {    //I/O bar
            if ((NextIoAddr & CurrentReqSize) != 0 ) {
                NextIoAddr = (NextIoAddr + CurrentReqSize + 1) & ~CurrentReqSize;
            }
            data32 &= 0xF;
            data32 |= NextIoAddr;
            NextIoAddr+=CurrentReqSize+1;
        }
        if ((NextMemAddr > (parameters.MemBase + parameters.MemSizeMax)) ||
            (NextIoAddr > ((UINT32)parameters.IoBase + (UINT32)parameters.IoSizeMax)))
            return VIDEO_ERR_OUT_OF_RESOURCES;

        //
        // Program the BAR.
        //
        PviPciWrite32(base, vBus, vDev, vFunc, barOffset, data32);

        //
        // Deal with 64-bit memory BARs -- need to zero the upper half.
        //
        if ( (data32 & 0x01) == 0 && ((data32 >> 1) & 0x03) == 0x2) {
            barOffset+=4;
            PviPciWrite32(base, vBus, vDev, vFunc, barOffset, 0);
        }

    }

    //
    // Now program the apertures for all upstream bridges.
    //
    bus = parameters.RootBus;
    for (devPathIdx = 0; devPathIdx < parameters.DevPathEntries - 1; devPathIdx++) {

        vDev = parameters.DevPath[devPathIdx].Dev;
        vFunc = parameters.DevPath[devPathIdx].Func;

        //
        // Program memory aperture (or close the aperture if no memory required).
        //
        if (NextMemAddr > parameters.MemBase) {
            PviPciWrite16(base, bus, vDev, vFunc, 0x20, (UINT16)((parameters.MemBase>> 16) & 0xFFF0));
            PviPciWrite16(base, bus, vDev, vFunc, 0x22, (UINT16)((NextMemAddr >> 16) & 0xFFF0));
        } else {
            PviPciWrite16(base, bus, vDev, vFunc, 0x20, 0xFFF0);
            PviPciWrite16(base, bus, vDev, vFunc, 0x22, 0);
        }

        //
        // Program I/O aperture (or close if not required).
        //
        if (NextIoAddr > parameters.IoBase) {
            PviPciWrite8(base, bus, vDev, vFunc, 0x1C, (UINT8)((parameters.IoBase >> 8) & 0xF0));
            PviPciWrite8(base, bus, vDev, vFunc, 0x1D, (UINT8)((NextIoAddr >> 8) & 0xF0));
            PviPciWrite16(base, bus, vDev, vFunc, 0x30, 0);
            PviPciWrite16(base, bus, vDev, vFunc, 0x32, 0);
        } else {
            PviPciWrite8(base, bus, vDev, vFunc, 0x1C, 0xF0);
            PviPciWrite8(base, bus, vDev, vFunc, 0x1D, 0x00);
            PviPciWrite16(base, bus, vDev, vFunc, 0x30, 0xFF);
            PviPciWrite16(base, bus, vDev, vFunc, 0x32, 0);
        }

        //
        // Close the prefetchable aperture (all memory assigned into non-prefetchable)
        //
        PviPciWrite16(base, bus, vDev, vFunc, 0x24, 0xFFF0);
        PviPciWrite16(base, bus, vDev, vFunc, 0x26, 0);
        PviPciWrite32(base, bus, vDev, vFunc, 0x28, 0xFFFFFFFF);
        PviPciWrite32(base, bus, vDev, vFunc, 0x2C, 0);

        bus++;
    }

    return VIDEO_SUCCESS;
}

VOID
PviPciWrite8(
  UINT32 CfgBase, 
  UINT8 bus, 
  UINT8 dev, 
  UINT8 func, 
  UINT16 reg, 
  UINT8 data) 
{
    UINT8 *regAddr;

    regAddr = (UINT8 *)(CfgBase | PCIE_REG_ADDR(bus, dev, func, reg));
    *(volatile UINT8 *)regAddr = data;
}

VOID
PviPciWrite16(
  UINT32 CfgBase, 
  UINT8 bus, 
  UINT8 dev, 
  UINT8 func, 
  UINT16 reg, 
  UINT16 data)
{
    UINT8 *regAddr;

    regAddr = (UINT8 *)(CfgBase | PCIE_REG_ADDR(bus, dev, func, reg));
    *(volatile UINT16 *)regAddr = data;
}

VOID
PviPciWrite32(
  UINT32 CfgBase, 
  UINT8 bus, 
  UINT8 dev, 
  UINT8 func, 
  UINT16 reg, 
  UINT32 data) 
{
    UINT8 *regAddr;

    regAddr = (UINT8 *)(CfgBase | PCIE_REG_ADDR(bus, dev, func, reg));
    *(volatile UINT32 *)regAddr = data;
}

VOID
PviPciRead8(
  UINT32 CfgBase, 
  UINT8 bus, 
  UINT8 dev, 
  UINT8 func, 
  UINT16 reg, 
  UINT8 *data)
{
    UINT8 *regAddr;
    
    regAddr = (UINT8 *)(CfgBase | PCIE_REG_ADDR(bus, dev, func, reg));
    *data = *(volatile UINT8 *)regAddr;
}

VOID
PviPciRead16(
  UINT32 CfgBase, 
  UINT8 bus, 
  UINT8 dev, 
  UINT8 func, 
  UINT16 reg, 
  UINT16 *data)
{
    UINT8 *regAddr;

    regAddr = (UINT8 *)(CfgBase | PCIE_REG_ADDR(bus, dev, func, reg));
    *data = *(volatile UINT16 *)regAddr;
}

VOID 
PviPciRead32(
  UINT32 CfgBase, 
  UINT8 bus, 
  UINT8 dev, 
  UINT8 func, 
  UINT16 reg, 
  UINT32 *data)
{
    UINT8 *regAddr;

    regAddr = (UINT8 *)(CfgBase | PCIE_REG_ADDR(bus, dev, func, reg));
    *data = *(volatile UINT32 *)regAddr;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************