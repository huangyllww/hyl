/*************************************************************************
 *  Module Name: initvga.c
 *  Descriprtion: 
 *  Note:
 *  Change Log:
 *
 **************************************************************************/

#include "devlib.h"
#include "vgahw.h"
#include "vgamode.h"
#include "dp501.h"
#include "Token.h"
#include <Library/BaseMemoryLib.h>
#include <library/PcdLib.h>  //<lvych001-20170210 Add display boot progress bar function.>
#include "font8x16.h"
//PORT80 Font
#define FONT_HEIGHT        16
#define FONT_WIDTH         8

#define LOGO_WIDTH		800
#define LOGO_HEIGHT		600
#define CP_COORDINATE_X FV_X
#define CP_COORDINATE_Y FV_Y
#define CP_BPP			FV_BIT_DEPTH
#define CP_STARTADDR(X,Y)  ( ((CP_BPP+1)/8)*((Y)*CP_COORDINATE_X + (X)) )

#if ((FV_X==1024) && (FV_BIT_DEPTH==32))
  #define FV_RESOLUTION_OVERRIDE "1024x768x32"
#elif ((FV_X==1024) && (FV_BIT_DEPTH==8))
	#define FV_RESOLUTION_OVERRIDE "1024x768x8"
#elif ((FV_X==800) && (FV_BIT_DEPTH==32))  //<bieyb002-20170518 fix build error when set FV_x to 800>
	#define FV_RESOLUTION_OVERRIDE "800x600x32"
#else ((FV_X==800) && (FV_BIT_DEPTH==8))  //<bieyb002-20170518 fix build error when set FV_x to 800>
	#define FV_RESOLUTION_OVERRIDE "800x600x8"
#endif

#if FV_GraphicMode_StringOutput_EN
void ShowChar(_DEVInfo *DevInfo, PVBIOS_MODE_INFO pVGAModeInfo, _FontInfo *FontInfo, ULONG *StartAddr, CHAR16/*UCHAR*/ ch);
void ShowString(_DEVInfo *DevInfo, PVBIOS_MODE_INFO pVGAModeInfo, CHAR16 *String);
#endif

//<lizhq040-20170120 Auto identify logo resolution+>
typedef EFI_GRAPHICS_OUTPUT_BLT_PIXEL EFI_UGA_PIXEL;

typedef struct {
  UINT8 Blue;
  UINT8 Green;
  UINT8 Red;
//  UINT8 Reserved;
} EFI_GRAPHICS_OUTPUT_BLT_PIXEL_24BPP;
//<lvych001-20170210 Add display boot progress bar function.+>

#define MAX_CPCOUNT  		56  
#define MRC_CPCOUNT         ((75 * MAX_CPCOUNT)/100)
#define MRC_CODE            0x01
#define OTHER_CODE          0x02
#define STOP_CPCODE         0x94    //<lvych001-20170805 Stop fastvideo checkpoint output when post done>

//<lvych001-20170805 Stop fastvideo checkpoint output when post done+>
typedef union {
  struct {
    UINT32  Count         : 16;
    UINT32  Percp         : 8;
    UINT32  Reserved      : 5;
    UINT32  InitDone      : 1;
    UINT32  CpStop        : 1;
    UINT32  BarSupport    : 1;
  } Bits;
  UINT32  Uint32;
} BOOT_DATA;

typedef struct {
	UINT8 phase;
	UINT8 cp;
	UINT8 percent;
} CPTOPERCENT_DATA;

CPTOPERCENT_DATA Cp2PercentTable[] = {
	    {OTHER_CODE	, 0x4F,76},
	    {OTHER_CODE , 0x79,95},
	    {OTHER_CODE , STOP_CPCODE,100},
};
//<lvych001-20170805 Stop fastvideo checkpoint output when post done->
//<lvych001-20170210 Add display boot progress bar function.->
//<lizhq040-20170120 Auto identify logo resolution->
//
// static
//
UCHAR jVersion[] = "v.1.01.00";
_PCI_DEVICE Device_List[] = {
    {0x1A03, 0x2000},
    {0xFFFF, 0xFFFF}
};	

//<Lizhq-20170807 Failsafe if did not get the correct image buffer. +>
VBIOS_DAC_INFO StaticPalette[0x400] = {
    //Automatic generation from 800x600 image buffer
    {0x00, 0x00, 0x00}, {0x01, 0x01, 0x01}, {0x02, 0x02, 0x02}, {0x03, 0x03, 0x03},
    {0x04, 0x04, 0x04}, {0x05, 0x05, 0x05}, {0x06, 0x06, 0x06}, {0x07, 0x07, 0x07},
    {0x08, 0x08, 0x08}, {0x09, 0x09, 0x09}, {0x0a, 0x0a, 0x0a}, {0x0b, 0x0b, 0x0b},
    {0x0c, 0x0c, 0x0c}, {0x0d, 0x0d, 0x0d}, {0x0e, 0x0e, 0x0e}, {0x0f, 0x0f, 0x0f},
    {0x10, 0x10, 0x10}, {0x11, 0x11, 0x11}, {0x12, 0x12, 0x12}, {0x13, 0x13, 0x13},
    {0x14, 0x14, 0x14}, {0x15, 0x15, 0x15}, {0x16, 0x16, 0x16}, {0x17, 0x17, 0x17},
    {0x18, 0x18, 0x18}, {0x19, 0x19, 0x19}, {0x1a, 0x1a, 0x1a}, {0x1b, 0x1b, 0x1b},
    {0x1c, 0x1c, 0x1c}, {0x1d, 0x1d, 0x1d}, {0x1e, 0x1e, 0x1e}, {0x1f, 0x1f, 0x1f},
    {0x20, 0x20, 0x20}, {0x21, 0x21, 0x21}, {0x22, 0x22, 0x22}, {0x23, 0x23, 0x23},
    {0x24, 0x24, 0x24}, {0x25, 0x25, 0x25}, {0x26, 0x26, 0x26}, {0x27, 0x27, 0x27},
    {0x28, 0x28, 0x28}, {0x29, 0x29, 0x29}, {0x2a, 0x2a, 0x2a}, {0x2b, 0x2b, 0x2b},
    {0x2c, 0x2c, 0x2c}, {0x2d, 0x2d, 0x2d}, {0x2e, 0x2e, 0x2e}, {0x2f, 0x2f, 0x2f},
    {0x30, 0x30, 0x30}, {0x31, 0x31, 0x31}, {0x32, 0x32, 0x32}, {0x33, 0x33, 0x33},
    {0x34, 0x34, 0x34}, {0x35, 0x35, 0x35}, {0x36, 0x36, 0x36}, {0x37, 0x37, 0x37},
    {0x38, 0x38, 0x38}, {0x39, 0x39, 0x39}, {0x3a, 0x3a, 0x3a}, {0x3b, 0x3b, 0x3b},
    {0x3c, 0x3c, 0x3c}, {0x3d, 0x3d, 0x3d}, {0x3e, 0x3e, 0x3e}, {0x3f, 0x3f, 0x3f},
    {0x40, 0x40, 0x40}, {0x41, 0x41, 0x41}, {0x42, 0x42, 0x42}, {0x43, 0x43, 0x43},
    {0x44, 0x44, 0x44}, {0x45, 0x45, 0x45}, {0x46, 0x46, 0x46}, {0x47, 0x47, 0x47},
    {0x48, 0x48, 0x48}, {0x49, 0x49, 0x49}, {0x4a, 0x4a, 0x4a}, {0x4b, 0x4b, 0x4b},
    {0x4c, 0x4c, 0x4c}, {0x4d, 0x4d, 0x4d}, {0x4e, 0x4e, 0x4e}, {0x4f, 0x4f, 0x4f},
    {0x50, 0x50, 0x50}, {0x51, 0x51, 0x51}, {0x52, 0x52, 0x52}, {0x53, 0x53, 0x53},
    {0x54, 0x54, 0x54}, {0x55, 0x55, 0x55}, {0x56, 0x56, 0x56}, {0x57, 0x57, 0x57},
    {0x58, 0x58, 0x58}, {0x59, 0x59, 0x59}, {0x5a, 0x5a, 0x5a}, {0x5b, 0x5b, 0x5b},
    {0x5c, 0x5c, 0x5c}, {0x5d, 0x5d, 0x5d}, {0x5e, 0x5e, 0x5e}, {0x5f, 0x5f, 0x5f},
    {0x60, 0x60, 0x60}, {0x61, 0x61, 0x61}, {0x62, 0x62, 0x62}, {0x63, 0x63, 0x63},
    {0x64, 0x64, 0x64}, {0x65, 0x65, 0x65}, {0x66, 0x66, 0x66}, {0x67, 0x67, 0x67},
    {0x68, 0x68, 0x68}, {0x69, 0x69, 0x69}, {0x6a, 0x6a, 0x6a}, {0x6b, 0x6b, 0x6b},
    {0x6c, 0x6c, 0x6c}, {0x6d, 0x6d, 0x6d}, {0x6e, 0x6e, 0x6e}, {0x6f, 0x6f, 0x6f},
    {0x70, 0x70, 0x70}, {0x71, 0x71, 0x71}, {0x72, 0x72, 0x72}, {0x73, 0x73, 0x73},
    {0x74, 0x74, 0x74}, {0x75, 0x75, 0x75}, {0x76, 0x76, 0x76}, {0x77, 0x77, 0x77},
    {0x78, 0x78, 0x78}, {0x79, 0x79, 0x79}, {0x7a, 0x7a, 0x7a}, {0x7b, 0x7b, 0x7b},
    {0x7c, 0x7c, 0x7c}, {0x7d, 0x7d, 0x7d}, {0x7e, 0x7e, 0x7e}, {0x7f, 0x7f, 0x7f},
    {0x80, 0x80, 0x80}, {0x81, 0x81, 0x81}, {0x82, 0x82, 0x82}, {0x83, 0x83, 0x83},
    {0x84, 0x84, 0x84}, {0x85, 0x85, 0x85}, {0x86, 0x86, 0x86}, {0x87, 0x87, 0x87},
    {0x88, 0x88, 0x88}, {0x89, 0x89, 0x89}, {0x8a, 0x8a, 0x8a}, {0x8b, 0x8b, 0x8b},
    {0x8c, 0x8c, 0x8c}, {0x8d, 0x8d, 0x8d}, {0x8e, 0x8e, 0x8e}, {0x8f, 0x8f, 0x8f},
    {0x90, 0x90, 0x90}, {0x91, 0x91, 0x91}, {0x92, 0x92, 0x92}, {0x93, 0x93, 0x93},
    {0x94, 0x94, 0x94}, {0x95, 0x95, 0x95}, {0x96, 0x96, 0x96}, {0x97, 0x97, 0x97},
    {0x98, 0x98, 0x98}, {0x99, 0x99, 0x99}, {0x9a, 0x9a, 0x9a}, {0x9b, 0x9b, 0x9b},
    {0x9c, 0x9c, 0x9c}, {0x9d, 0x9d, 0x9d}, {0x9e, 0x9e, 0x9e}, {0x9f, 0x9f, 0x9f},
    {0xa0, 0xa0, 0xa0}, {0xa1, 0xa1, 0xa1}, {0xa2, 0xa2, 0xa2}, {0xa3, 0xa3, 0xa3},
    {0xa4, 0xa4, 0xa4}, {0xa5, 0xa5, 0xa5}, {0xa6, 0xa6, 0xa6}, {0xa7, 0xa7, 0xa7},
    {0xa8, 0xa8, 0xa8}, {0xa9, 0xa9, 0xa9}, {0xaa, 0xaa, 0xaa}, {0xab, 0xab, 0xab},
    {0xac, 0xac, 0xac}, {0xad, 0xad, 0xad}, {0xae, 0xae, 0xae}, {0xaf, 0xaf, 0xaf},
    {0xb0, 0xb0, 0xb0}, {0xb1, 0xb1, 0xb1}, {0xb2, 0xb2, 0xb2}, {0xb3, 0xb3, 0xb3},
    {0xb4, 0xb4, 0xb4}, {0xb5, 0xb5, 0xb5}, {0xb6, 0xb6, 0xb6}, {0xb7, 0xb7, 0xb7},
    {0xb8, 0xb8, 0xb8}, {0xb9, 0xb9, 0xb9}, {0xba, 0xba, 0xba}, {0xbb, 0xbb, 0xbb},
    {0xbc, 0xbc, 0xbc}, {0xbd, 0xbd, 0xbd}, {0xbe, 0xbe, 0xbe}, {0xbf, 0xbf, 0xbf},
    {0xc0, 0xc0, 0xc0}, {0xc1, 0xc1, 0xc1}, {0xc2, 0xc2, 0xc2}, {0xc3, 0xc3, 0xc3},
    {0xc4, 0xc4, 0xc4}, {0xc5, 0xc5, 0xc5}, {0xc6, 0xc6, 0xc6}, {0xc7, 0xc7, 0xc7},
    {0xc8, 0xc8, 0xc8}, {0xc9, 0xc9, 0xc9}, {0xca, 0xca, 0xca}, {0xcb, 0xcb, 0xcb},
    {0xcc, 0xcc, 0xcc}, {0xcd, 0xcd, 0xcd}, {0xce, 0xce, 0xce}, {0xcf, 0xcf, 0xcf},
    {0xd0, 0xd0, 0xd0}, {0xd1, 0xd1, 0xd1}, {0xd2, 0xd2, 0xd2}, {0xd3, 0xd3, 0xd3},
    {0xd4, 0xd4, 0xd4}, {0xd5, 0xd5, 0xd5}, {0xd6, 0xd6, 0xd6}, {0xd7, 0xd7, 0xd7},
    {0xd8, 0xd8, 0xd8}, {0xd9, 0xd9, 0xd9}, {0xda, 0xda, 0xda}, {0xdb, 0xdb, 0xdb},
    {0xdc, 0xdc, 0xdc}, {0xdd, 0xdd, 0xdd}, {0xde, 0xde, 0xde}, {0xdf, 0xdf, 0xdf},
    {0xe0, 0xe0, 0xe0}, {0xe1, 0xe1, 0xe1}, {0xe2, 0xe2, 0xe2}, {0xe3, 0xe3, 0xe3},
    {0xe4, 0xe4, 0xe4}, {0xe5, 0xe5, 0xe5}, {0xe6, 0xe6, 0xe6}, {0xe7, 0xe7, 0xe7},
    {0xe8, 0xe8, 0xe8}, {0xe9, 0xe9, 0xe9}, {0xea, 0xea, 0xea}, {0xeb, 0xeb, 0xeb},
    {0xec, 0xec, 0xec}, {0xed, 0xed, 0xed}, {0xee, 0xee, 0xee}, {0xef, 0xef, 0xef},
    {0xf0, 0xf0, 0xf0}, {0xf1, 0xf1, 0xf1}, {0xf2, 0xf2, 0xf2}, {0xf3, 0xf3, 0xf3},
    {0xf4, 0xf4, 0xf4}, {0xf5, 0xf5, 0xf5}, {0xf6, 0xf6, 0xf6}, {0xf7, 0xf7, 0xf7},
    {0xf8, 0xf8, 0xf8}, {0xf9, 0xf9, 0xf9}, {0xfa, 0xfa, 0xfa}, {0xfb, 0xfb, 0xfb},
    {0xfc, 0xfc, 0xfc}, {0xfd, 0xfd, 0xfd}, {0xfe, 0xfe, 0xfe}, {0xff, 0xff, 0xff},
};
//<Lizhq-20170807 Failsafe if did not get the correct image buffer. ->

//<Lizhq-20170807 Get dynamic MMIO base for fast video frame buffer.(Mantis:15241) +>
extern VOID AST2300_PEInit(_DEVInfo* pDevInfo);

UINT32 GetFrameBufferAddress()
{
  UINT32 Mmiobase;

  Mmiobase = PcdGet32(PcdFastVideoMmioBaseAddress);

  return Mmiobase;
}

VOID SetFrameBufferAddress(UINT32 Mmiobase)
{

  PcdSet32 (PcdFastVideoMmioBaseAddress, Mmiobase);
}
//<Lizhq-20170807 Get dynamic MMIO base for fast video frame buffer.(Mantis:15241) ->

EFI_STATUS FBBltImage(
		UINT8	      *Image,
		UINT16		bpp //<lizhq040-20170120 Auto identify logo resolution>
);

extern BOOLEAN InitBmpLogoImage(VOID **BmpImage, UINT32 *Width, UINT32 *Height, UINT16 *Depth, VOID **Palette);

EFI_STATUS InitVGA (ULONG OemData, UCHAR *argv)
{
	EFI_STATUS Status = EFI_SUCCESS;
    _PCI_DEVICE *pDeviceList;
    _DEVInfo DevInfo;
    ULONG InitOption = Init_MEM;
    
    UINT8 *BmpImage;
    UINT8 *Palette;
    UINT32 Width=0, Height=0;
    UINT16 Depth=0;
    BOOT_DATA   Bootdata;
	
    EFI_PEI_SERVICES **PeiServices = (EFI_PEI_SERVICES **)OemData;
    //if (DebugPrintEnabledEx()) return EFI_SUCCESS; //<lvych00120161009>
    DEBUG((0xffffffff, "ASPEED Graphics Init Utility %s \n", jVersion));
    //<Machong-20170722-Remove hard code of pcie enumeration+>
    //<Lizhq-20170807 Get dynamic MMIO base for fast video frame buffer.(Mantis:15241) +>
    ZeroMem((VOID *)&DevInfo, sizeof(_DEVInfo));

    //DevInfo.ulFBBase = GetFrameBufferAddress();
    //DEBUG((EFI_D_ERROR, "GetFrameBufferAddress: 0x%x", DevInfo.ulFBBase));
    
    //if (DevInfo.ulFBBase == 0)
    // return EFI_OUT_OF_RESOURCES;

    //DevInfo.ulMMIOBase = DevInfo.ulFBBase + SIZE_4MB; //Reserved 4MB for BAR0
    //AST2300_PEInit(&DevInfo);
    //<Lizhq-20170807 Get dynamic MMIO base for fast video frame buffer.(Mantis:15241) ->
    //<Machong-20170722-Remove hard code of pcie enumeration->
    //Get Chip Information
    pDeviceList  = Device_List;
    if (GetDevInfo (PeiServices, pDeviceList->usVendorID, pDeviceList->usDeviceID, &DevInfo) == 0)
    {
		DEBUG ((0xffffffff, "[ERROR] Can't Find the Device\n"));
    	return EFI_NOT_FOUND;
    }
    
    SetFrameBufferAddress(DevInfo.ulFBBase);
    DEBUG((EFI_D_ERROR, "FrameBufferAddress: 0x%x\n", DevInfo.ulFBBase));
    //Init HW
    InitVGA_HW(PeiServices, &DevInfo, InitOption);           
    //Initialize BMP Logo here so that we can config VGA  Palette

    //<Lizhq-20170807 Failsafe if did not get the correct image buffer. +>
    Bootdata.Uint32 = PcdGet32(PcdBootData);
    if(Bootdata.Bits.BarSupport) {
      BmpImage = NULL;
      
      //Init palette for 8 bpp mode, bypass for 32 bpp mode
      Palette = (UINT8*)StaticPalette;
    } else {
      if (!InitBmpLogoImage((VOID **)&BmpImage, &Width, &Height, &Depth, (VOID **)&Palette)) {
        BmpImage = NULL;

        //Init palette for 8 bpp mode, bypass for 32 bpp mode
        Palette = (UINT8*)StaticPalette;
      }
    //<Lizhq-20170807 Failsafe if did not get the correct image buffer. ->
    }
    #ifdef FV_RESOLUTION_OVERRIDE
    argv = FV_RESOLUTION_OVERRIDE;
    #endif
    //Init Mode
    if (SetMode(PeiServices, &DevInfo, argv, 0,/*Null for Normal Mode*/ Palette) == FALSE)
    {
		DEBUG ((0xffffffff, "[Warn] Can't find the assgined mode %s \n", argv));	       
    }
    DEBUG ((0xffffffff,  "\n BMP Logo width = %d, Height = %d, Depth = %d...", Width, Height, Depth));	 
    
    //Show time, draw BMP LOGO into frame buffer.
    //<lizhq040-20170120 Auto identify logo resolution+>
//#if BOOT_PROGRESS_BAR_SUPPORT == 0 //<lvych001-20170210 Add display boot progress bar function.+>
    Bootdata.Bits.InitDone = 1;
    PcdSet32 (PcdBootData, Bootdata.Uint32);
    if(Bootdata.Bits.BarSupport)
      return Status;
    Status = FBBltImage(BmpImage, CP_BPP);
//#endif   //<lvych001-20170210 Add display boot progress bar function.->
    //<lizhq040-20170120 Auto identify logo resolution->
    return Status;
    
} //InitVGA
//<lizhq040-20170120 Auto identify logo resolution+>
EFI_STATUS FBBltImage(
		UINT8	     *FileImage,
		UINT16		bpp
		)
{
  UINT32      CoordinateX;
  UINT32      CoordinateY;
  UINT32      Width;
  UINT32      Height;
	UINT8       *FrameBuffer;
	UINT16      X,Y;
	UINT32      startAddress;
  UINT32      Pixel = 0;
  //<bieyb-20180122  Set Font color by logo background color+>
  UINT32      PostFontBGColor=0;
  UINT32      PixelBGColor = 0;
  //<bieyb-20180122  Set Font color by logo background color->
	UINT8       *Image;
  UINT8       *VbeBuffer;
  UINT32      *PalettePtr;
  UINT8       PaletteIndex;
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL_24BPP  *Blt;

	if (FileImage == NULL)
	  return EFI_NOT_FOUND;

	FrameBuffer = (UINT8 *)GetFrameBufferAddress(); //<Lizhq-20170807 Get dynamic MMIO base for fast video frame buffer.(Mantis:15241)>
	if (FrameBuffer == 0)
	  return EFI_OUT_OF_RESOURCES;

	Image =  FileImage + *(UINT32 *)(FileImage + 0xa); // Bitmap offset is 0xa

  Width = *(UINT32 *)(FileImage + 18);  //Bitmap info Width offset 18
  Height = *(UINT32 *)(FileImage + 22); //Bitmap info Height offset 22
  bpp = *(UINT16 *)(FileImage + 28);    //Bitmap info of bit count
  PalettePtr = (UINT32 *)(FileImage + 54);
  
  CoordinateX = (CP_COORDINATE_X-Width)/2;
  CoordinateY = (CP_COORDINATE_Y-Height)/2;

  //Image resolution should be equal or less than screen resolution
  if (Width > CP_COORDINATE_X)
    return EFI_NOT_FOUND;

  //1024x768x8 & 800x600x8 mode
  if (CP_BPP == 8) {
    //1024x768x8 & 800x600x8 mode not support 32bit count image
    if (bpp > 8)
      return EFI_NOT_FOUND;

    for (Y = 0; Y<Height; Y++) {
      for (X=0; X<Width; X++) {
        PaletteIndex = *(Image + Width * (Height-1) - Y*Width + X);
        startAddress = (Y + CoordinateY)*CP_COORDINATE_X + X + CoordinateX;
        FrameBuffer[startAddress] = PaletteIndex;
        Pixel = PalettePtr[PaletteIndex];
      }
    }
  } else if (CP_BPP == 32) { //1024x768x32 & 800x600x32 mode
    for (Y = 0; Y<Height; Y++) {
        for (X=0; X<Width; X++) {
//<lizhq041-20170208 Optimization for draw BMP image line-by-line under 32BIT BPP mode+>
          switch (bpp) {
          case 8:
            PaletteIndex = *(Image + Width * (Height-1) - Y*Width + X);
            if (PaletteIndex == 0)
              continue;
            Pixel = PalettePtr[PaletteIndex];
            break;
          case 24:
          case 32:
            Blt = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL_24BPP *)(Image + (Y*Width + X)*sizeof(EFI_UGA_PIXEL));
            Pixel = (Blt->Blue) | (Blt->Green << 8) | (Blt->Red <<16);
            break;
          default:
            return EFI_SUCCESS;
            break;
          }
          if (Pixel == 0)
            continue;

          startAddress = (Y + CoordinateY)*CP_COORDINATE_X + X + CoordinateX;
          VbeBuffer = FrameBuffer + startAddress*sizeof(EFI_UGA_PIXEL);
//<lizhq041-20170208 Optimization for draw BMP image line-by-line under 32BIT BPP mode->
          CopyMem ((UINT8*)VbeBuffer, &Pixel, sizeof(EFI_UGA_PIXEL));
        }
      }
  }
//<bieyb-20180122  Set Font color by logo background color+>
  PixelBGColor =Pixel & 0xffffff;  // the highest byte Reserved
  DEBUG ((0xffffffff, "PixelBGColor %x\n",PixelBGColor));
  PostFontBGColor= PcdGet32(PcdPostFontBackGround);
  if(PostFontBGColor!=PixelBGColor)
      PcdSet32 (PcdPostFontBackGround, PixelBGColor);
//<bieyb-20180122  Set Font color by logo background color->
	return EFI_SUCCESS;
}
//<lvych001-20170210 Add display boot progress bar function.+>

/**
    function to draw draw a block

    @param DestX UINTN DestY, UINTN Width, UINTN Height, EFI_UGA_PIXEL *BltBuffer

    @retval void

**/
//<licm-20210226 VGA and UART Display 16bit Port80 SUPPORT +>
#if defined(VGA_UART_DIS16BIT_PORT80_SUPPORT) && (VGA_UART_DIS16BIT_PORT80_SUPPORT == 1)
static VOID DrawBlock(
    ULONG x,
    ULONG y,
    ULONG w,
    ULONG h,
    EFI_UGA_PIXEL *BltBuffer
    )
#else
  VOID DrawBlock(
    ULONG x,
    ULONG y,
    ULONG w,
    ULONG h,
    EFI_UGA_PIXEL *BltBuffer
    )
#endif
//<licm-20210226 VGA and UART Display 16bit Port80 SUPPORT ->
{

  UCHAR *pjFBLinear = (UCHAR*)GetFrameBufferAddress(); //<Lizhq-20170807 Get dynamic MMIO base for fast video frame buffer.(Mantis:15241)>
  ULONG bpp = CP_BPP;
  ULONG xres = CP_COORDINATE_X;
  ULONG PixelPerByte = (bpp+1)/8;
  ULONG ScreenPicth  = xres*PixelPerByte;
  ULONG WriteData;
  ULONG StartAddr,NextRowAddr, DestAddr;
  ULONG i, j;

  
  StartAddr = CP_STARTADDR(x,y);
  WriteData = *(ULONG *)BltBuffer;
  for (i=0; i<w; i++)
  {
    NextRowAddr = StartAddr + PixelPerByte*i;
    for (j=0; j<h; j++)
    {
      DestAddr = NextRowAddr + ScreenPicth*j;
      if      (PixelPerByte == 1) *(UCHAR *)(pjFBLinear+DestAddr)  = (UCHAR) WriteData;
      else if (PixelPerByte == 2) *(USHORT *)(pjFBLinear+DestAddr) = (USHORT) WriteData;
      else                        *(ULONG *)(pjFBLinear+DestAddr)  = WriteData;
    }
  }
}


/**
    function to draw the border of a progress bar

    @param DestX UINTN DestY, UINTN Width, UINTN Height,
    @param BltBuffer UINTN  bw

    @retval void

**/
//<licm-20210226 VGA and UART Display 16bit Port80 SUPPORT +>
#if defined(VGA_UART_DIS16BIT_PORT80_SUPPORT) && (VGA_UART_DIS16BIT_PORT80_SUPPORT == 1)
static VOID DrawProgressBarBorder(
    ULONG x,
    ULONG y,
    ULONG w,
    ULONG h,
    EFI_UGA_PIXEL *BltBuffer,
    ULONG bw
    )
#else
VOID DrawProgressBarBorder(
    ULONG x,
    ULONG y,
    ULONG w,
    ULONG h,
    EFI_UGA_PIXEL *BltBuffer,
    ULONG bw
    )
#endif
//<licm-20210226 VGA and UART Display 16bit Port80 SUPPORT ->
{

  //Top
  DrawBlock(x, y, w, bw, BltBuffer) ;
  //Bottom
  DrawBlock(x, y+h-bw, w, bw, BltBuffer) ;
  //Left
  DrawBlock(x, y, bw, h, BltBuffer) ;
  //Right
  DrawBlock(x+w-bw, y, bw, h, BltBuffer) ;

}
//<licm-20210226 VGA and UART Display 16bit Port80 SUPPORT +>
#if defined(VGA_UART_DIS16BIT_PORT80_SUPPORT) && (VGA_UART_DIS16BIT_PORT80_SUPPORT == 1)
static VOID DrawBltProgressBar(
    ULONG x,
    ULONG y,
    ULONG w,
    ULONG h,
    UCHAR percent
  )
#else
VOID DrawBltProgressBar(
    ULONG x,
    ULONG y,
    ULONG w,
    ULONG h,
    UCHAR percent
  )
#endif
//<licm-20210226 VGA and UART Display 16bit Port80 SUPPORT ->
{

  EFI_UGA_PIXEL backgroundColor = OEMPROGRESSBAR_BACKGROUNDCOLOR;
  //EFI_UGA_PIXEL borderColor = OEMPROGRESSBAR_BORDERCOLOR;
  EFI_UGA_PIXEL fillColor = OEMPROGRESSBAR_FILLCOLOR;
  //ULONG delta = 0;
  ULONG completed;

  if(percent>100)
    percent = 100;

  completed = (w * percent)/100;

  if(completed == 0)DrawBlock(x, y, w, h, &backgroundColor);          //draw background

  if(completed <= w)
  {
    DrawBlock(x, y, completed, h, &fillColor);  //draw progression
    //DrawProgressBarBorder(x, y, w, h, &borderColor, 2) ;     //draw border
  }

}

void DrawTxtProgressBar(
    ULONG             x,
    ULONG             y,
    UCHAR             barlen,
    UCHAR             percent,
    _DEVInfo          *DevInfo, 
    PVBIOS_MODE_INFO  pVGAModeInfo
 )
{
  CHAR16      DisplayBuffer[150]  = {0};
  UCHAR       i=0;

  if(barlen>100) 
    barlen = 100;
  
  if(percent>100) 
    percent = 100;

  DisplayBuffer[0] = '[';
  for(i=1;i<=barlen;++i)DisplayBuffer[i]=i*100<=percent*barlen?'>':' ';
  DisplayBuffer[barlen+1] = ']';
  DisplayBuffer[barlen+2] = 0;
  DevInfo->PCIAddr =  CP_STARTADDR(x, y);
  ShowString(DevInfo, pVGAModeInfo, DisplayBuffer);

}

VOID DrawProgressBar(
    _DEVInfo          *DevInfo, 
    PVBIOS_MODE_INFO  pVGAModeInfo,
    UCHAR             percent,
    BOOLEAN           TxtMode
  )
{
  
  if(TxtMode)
    DrawTxtProgressBar(0,CP_COORDINATE_Y-(FONT_HEIGHT+2),100,percent,DevInfo,pVGAModeInfo);
  else
    DrawBltProgressBar(0,CP_COORDINATE_Y-(FONT_HEIGHT+2),OEMBAR_WIDTH,OEMBAR_HEIGHT,percent);

}
//<lvych001-20170805 Add stop fastvideo checkpoint output function+>
BOOLEAN CheckCpIsStop(UINT8 cp,UINT8 phase)
{
  BOOT_DATA   Bootdata;
  
  
  Bootdata.Uint32 = PcdGet32(PcdBootData);
  if(Bootdata.Bits.CpStop)
    return TRUE;
  
  if((cp == STOP_CPCODE) && (phase == OTHER_CODE)){
    Bootdata.Bits.CpStop = 1;
    PcdSet32 (PcdBootData, Bootdata.Uint32);
  }
  
  return FALSE;

}
//<lvych001-20170805 Add stop fastvideo checkpoint output function->
void DisplayProgressBar(_DEVInfo *DevInfo, PVBIOS_MODE_INFO pVGAModeInfo,UINT8 phase,UINT8 cp, BOOLEAN minorCode)
{
  BOOT_DATA   Bootdata;
  CHAR16      message[100];
  ULONG       DATA32;
  UCHAR       percent;
  CPTOPERCENT_DATA *pCp2Percent = Cp2PercentTable;
  UCHAR	      Index;

  if(minorCode)
    return;
  
  Bootdata.Uint32 = PcdGet32(PcdBootData);
  if(!Bootdata.Bits.BarSupport)
    return;
  
  if(!Bootdata.Bits.InitDone)
    return;
  
  if(cp == Bootdata.Bits.Percp)
    return;

  if((Bootdata.Uint32 &0xFFFFFF) == 0){//Init bar
    DevInfo->PCIAddr =  CP_STARTADDR(0, CP_COORDINATE_Y-(FONT_HEIGHT*2+2));
    Swprintf(message, L"It will take a while to boot up...");
    ShowString(DevInfo, pVGAModeInfo, message);
    DrawProgressBar(DevInfo,pVGAModeInfo,0,FALSE);
  }
  
  if(phase == MRC_CODE){
    if(Bootdata.Bits.Count > MRC_CPCOUNT) 
      Bootdata.Bits.Count = MRC_CPCOUNT;
  }
  
  if(Bootdata.Bits.Count > MAX_CPCOUNT){
    Bootdata.Bits.Count = MAX_CPCOUNT;
  }

  DATA32 = Bootdata.Bits.Count;
  DATA32 = (DATA32*100)/MAX_CPCOUNT;
  percent = DATA32&0xFF;
  
  for(Index=0;Index<sizeof(Cp2PercentTable)/sizeof(CPTOPERCENT_DATA);Index++,pCp2Percent++){
	  if((phase == pCp2Percent->phase) && (cp == pCp2Percent->cp) && (percent < pCp2Percent->percent)){
		  percent = pCp2Percent->percent;
		  DATA32 = (percent*MAX_CPCOUNT)/100;
		  Bootdata.Bits.Count = (DATA32&0xFFFF);
		  break;
	  }
  }

  DrawProgressBar(DevInfo,pVGAModeInfo,percent,FALSE);

  Bootdata.Bits.Percp = cp;
  Bootdata.Bits.Count++;

  
  PcdSet32 (PcdBootData, Bootdata.Uint32);
  return;
}
//<lvych001-20170210 Add display boot progress bar function.->

//<licm-20210226 VGA and UART Display 16bit Port80 SUPPORT +>
#if defined(VGA_UART_DIS16BIT_PORT80_SUPPORT) && (VGA_UART_DIS16BIT_PORT80_SUPPORT == 1)
VOID IdsVGACheckPoint(UINT16 cp) 
{
    _DEVInfo devInfo;
    VBIOS_MODE_INFO vbiosModeInfo;

     _DEVInfo *DevInfo=NULL;
    PVBIOS_MODE_INFO pVGAModeInfo=NULL;
    CHAR16 message[100];

    if (GetFrameBufferAddress() == 0)
      return ;

    DevInfo = &devInfo;
    pVGAModeInfo = &vbiosModeInfo;

    DevInfo->PCIAddr =  CP_STARTADDR(CP_COORDINATE_X-((FONT_WIDTH+4)*4), CP_COORDINATE_Y-(FONT_HEIGHT+2));
    DevInfo->pjFBLinear = (UCHAR*)GetFrameBufferAddress(); //<Lizhq-20170807 Get dynamic MMIO base for fast video frame buffer.(Mantis:15241)>
    pVGAModeInfo->bpp = CP_BPP;
    pVGAModeInfo->xres = CP_COORDINATE_X;

    if (0xFFFFFFFF == *(UINT32*)GetFrameBufferAddress()) //<Lizhq-20170807 Get dynamic MMIO base for fast video frame buffer.(Mantis:15241)>
      return ;

    Swprintf(message, L"  %04X", cp);
    //OEMDEBUG ((DEBUG_INFO, "[OemDbg]AgesaPoint = %04X\n", cp));
    ShowString(DevInfo, pVGAModeInfo, message);
     return;
}
#endif

//<lizhq040-20170120 Auto identify logo resolution->
#if defined(VGA_UART_DIS16BIT_PORT80_SUPPORT) && (VGA_UART_DIS16BIT_PORT80_SUPPORT == 1)
VOID VGACheckPoint(VOID *PeiServices, UINT16 cp) 
#else
VOID VGACheckPoint(VOID *PeiServices, UINT8 cp) 
#endif
{
	_DEVInfo devInfo;
	VBIOS_MODE_INFO vbiosModeInfo;
	UINT16 CheckPoint = (UINT16)cp;

	_DEVInfo *DevInfo=NULL;
	PVBIOS_MODE_INFO pVGAModeInfo=NULL;
	CHAR16 message[100];
	UINT8 DataValue;
	
    if (GetFrameBufferAddress() == 0)
      return ;

	if(CheckCpIsStop((UINT8)(CheckPoint&0xFF),OTHER_CODE)) return;  //<lvych001-20170805 Stop fastvideo checkpoint output when post done>
	DevInfo = &devInfo;
	pVGAModeInfo = &vbiosModeInfo;

	DevInfo->PCIAddr =  CP_STARTADDR(CP_COORDINATE_X-((FONT_WIDTH+4)*4), CP_COORDINATE_Y-(FONT_HEIGHT+2));
	DevInfo->pjFBLinear = (UCHAR*)GetFrameBufferAddress(); //<Lizhq-20170807 Get dynamic MMIO base for fast video frame buffer.(Mantis:15241)>
	pVGAModeInfo->bpp = CP_BPP;
	pVGAModeInfo->xres = CP_COORDINATE_X;
	
	if (0xFFFFFFFF == *(UINT32*)GetFrameBufferAddress()) //<Lizhq-20170807 Get dynamic MMIO base for fast video frame buffer.(Mantis:15241)>
	  return ;
	
    DataValue = PcdGet8(PcdPostCodeDebug);
    if(DataValue==1) {
        Swprintf(message, L"%02X(%02X)", CheckPoint&0xFF,CheckPoint>>8);
    }else{
        Swprintf(message, L"    %02X", CheckPoint & 0xFF);
    }
    //OEMDEBUG ((DEBUG_INFO, "[OemDbg]CheckPoint = %s\n", message));
	ShowString(DevInfo, pVGAModeInfo, message);
	DisplayProgressBar(DevInfo, pVGAModeInfo,OTHER_CODE,CheckPoint&0xFF,FALSE); //<lvych001-20170210 Add display boot progress bar function.>
	
	return;
}
//<licm-20210226 VGA and UART Display 16bit Port80 SUPPORT ->

VOID VGACheckPointMRC(VOID *PeiServices, UINT8 cp, BOOLEAN minorCode) {
	_DEVInfo devInfo;
	VBIOS_MODE_INFO vbiosModeInfo;
	
	_DEVInfo *DevInfo=NULL;
	PVBIOS_MODE_INFO pVGAModeInfo=NULL;
	CHAR16 message[100];
	
  if (GetFrameBufferAddress() == 0)
    return ;

	DevInfo = &devInfo;
	pVGAModeInfo = &vbiosModeInfo;
	
	DevInfo->PCIAddr =  CP_STARTADDR(CP_COORDINATE_X-(FONT_WIDTH*4), CP_COORDINATE_Y-(FONT_HEIGHT+2));
	DevInfo->pjFBLinear = (UCHAR*)GetFrameBufferAddress(); //<Lizhq-20170807 Get dynamic MMIO base for fast video frame buffer.(Mantis:15241)>
	pVGAModeInfo->bpp = CP_BPP;
	pVGAModeInfo->xres = CP_COORDINATE_X;
	
	if (0xFFFFFFFF == *(UINT32*)GetFrameBufferAddress()) //<Lizhq-20170807 Get dynamic MMIO base for fast video frame buffer.(Mantis:15241)>
	  return ;
	if (minorCode) {
	    DevInfo->PCIAddr =  CP_STARTADDR(CP_COORDINATE_X-(FONT_WIDTH*2), CP_COORDINATE_Y-(FONT_HEIGHT+2));
		Swprintf(message, L"%02X", cp);
	} else {
		Swprintf(message, L"%02X00", cp);
	}

	ShowString(DevInfo, pVGAModeInfo, message);
	DisplayProgressBar(DevInfo, pVGAModeInfo,MRC_CODE,cp,minorCode); //<lvych001-20170210 Add display boot progress bar function.>
	
	return;
}

//<lvych001-20170216 Add bios recovery messages. +>
VOID VideoSwPrintf(UINT32 x,UINT32 y, const CHAR16* sFormat, ...)
{
  _DEVInfo devInfo;
  VBIOS_MODE_INFO vbiosModeInfo;
  CHAR16 message[1024];  
  va_list ArgList;

  
  if (GetFrameBufferAddress() == 0)
    return ;

  if (0xFFFFFFFF == *(UINT32*)GetFrameBufferAddress()) //<Lizhq-20170807 Get dynamic MMIO base for fast video frame buffer.(Mantis:15241)>
    return;
  
  devInfo.PCIAddr =  CP_STARTADDR(FONT_WIDTH*x, FONT_HEIGHT*y);
  devInfo.pjFBLinear = (UCHAR*)GetFrameBufferAddress(); //<Lizhq-20170807 Get dynamic MMIO base for fast video frame buffer.(Mantis:15241)>
  vbiosModeInfo.bpp = CP_BPP;
  vbiosModeInfo.xres = CP_COORDINATE_X;

  VA_START (ArgList, sFormat);
  Swprintf_s_va_list(message,sizeof(message),sFormat,ArgList);
  VA_END (ArgList);
  
  ShowString(&devInfo, &vbiosModeInfo, message);
  
}
//<lvych001-20170216 Add bios recovery messages. ->

#if FV_GraphicMode_StringOutput_EN
void ShowChar(_DEVInfo *DevInfo, PVBIOS_MODE_INFO pVGAModeInfo, _FontInfo *FontInfo, ULONG *StartAddr, CHAR16 ch)
{
	UCHAR *FontEntry = FontInfo->FontTable + ((FontInfo->FontWidth+7)/FONT_WIDTH)*FontInfo->FontHeight*ch;
	ULONG PixelPerByte = (pVGAModeInfo->bpp+1)/8;
	ULONG ScreenPicth  = pVGAModeInfo->xres*PixelPerByte;
	ULONG WriteData;
	ULONG NextLineAddr, DestAddr;
	UCHAR i, j, k, l, Temp;
	
	for (i=0; i<FontInfo->FontHeight; i++)
	{
		NextLineAddr = *StartAddr + ScreenPicth*FontInfo->YScale*i;					
		for (j=0; j<FontInfo->YScale; j++)
		{
			DestAddr = NextLineAddr + ScreenPicth*j;
		    Temp = *(UCHAR *)(FontEntry + i);					
	        for (k=0; k<FontInfo->FontWidth; k++)
	        {
		        if (Temp & 0x80)
		            WriteData = FontInfo->FG;
		        else
		            WriteData = FontInfo->BG;
		        Temp <<= 1;    
		        for (l=0; l<FontInfo->XScale; l++)
		        {
			        if      (PixelPerByte == 1) *(UCHAR *)(DevInfo->pjFBLinear+DestAddr)  = (UCHAR) WriteData;
			        else if (PixelPerByte == 2) *(USHORT *)(DevInfo->pjFBLinear+DestAddr) = (USHORT) WriteData;
			        else                        *(ULONG *)(DevInfo->pjFBLinear+DestAddr)  = WriteData;
			        DestAddr += PixelPerByte;   
		        }        
	        }
        }    
    }
} //ShowChar
	
void ShowString(_DEVInfo *DevInfo, PVBIOS_MODE_INFO pVGAModeInfo, CHAR16 *String)
{
    _FontInfo FontInfo;	
    ULONG StrLength = (UINT32)StrLen(String);
    ULONG i, StartAddr, DestAddr;
    CHAR16 ch;
//<bieyb-20180122  Set Font color by logo background color+>
    UINT32      PixelBGColor = 0;
    BOOT_DATA   Bootdata;
    EFI_UGA_PIXEL fillColor = OEMPROGRESSBAR_FILLCOLOR;
    
    Bootdata.Uint32 = PcdGet32(PcdBootData);

    if(Bootdata.Bits.BarSupport)
    {
        CopyMem (&PixelBGColor, &fillColor, sizeof(EFI_UGA_PIXEL));
        PixelBGColor = (~PixelBGColor)&0xffffff;    //get BackGround color
    }
    else
    {
        PixelBGColor= PcdGet32(PcdPostFontBackGround);
    }
//<bieyb-20180122  Set Font color by logo background color->
    
    StartAddr = 0;
    
    //Init Font Table
    FontInfo.FontTable  = (UINT8 *)FONT8X16;
    FontInfo.FontHeight = FONT_HEIGHT;
    FontInfo.FontWidth  = 8;

    FontInfo.XScale     = 1;   
    FontInfo.YScale     = 1;   
//<bieyb-20180122  Set Font color by logo background color+>
    FontInfo.FG         = (~PixelBGColor)&0xffffff;
    FontInfo.BG         = PixelBGColor;
//<bieyb-20180122  Set Font color by logo background color->
    StartAddr           = DevInfo->PCIAddr;
    
    for (i=0; i<StrLength; i++)
    {
	    DestAddr = StartAddr + FontInfo.FontWidth*((pVGAModeInfo->bpp+1)/FONT_WIDTH)*FontInfo.XScale*i;
	    ch = String[i];
	    ShowChar(DevInfo, pVGAModeInfo, &FontInfo, &DestAddr, ch);    
    }    
} //ShowString
#endif
