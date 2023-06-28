/*++
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
--*/
//**************************************************************************
//*                                                                        *
//*      Intel Restricted Secret                                           *
//*                                                                        *
//*      CAR Video Initialization Code                                     *
//*                                                                        *
//*      Copyright (c) 2012, Intel Corporation.                            *
//*                                                                        *
//*      This software and associated documentation (if any) is furnished  *
//*      under a license and may only be used or copied in accordance      *
//*      with the terms of the license. Except as permitted by such        *
//*      license, no part of this software or documentation may be         *
//*      reproduced, stored in a retrieval system, or transmitted in any   *
//*      form or by any means without the express written consent of       *
//*      Intel Corporation.                                                *
//*                                                                        *
//*      This program has been developed by Intel Corporation.             *
//*      Licensee has Intel's permission to incorporate this source code   *
//*      into their product, royalty free.  This source code may NOT be    *
//*      redistributed to anyone without Intel's written permission.       *
//*                                                                        *
//*      Intel specifically disclaims all warranties, express or           *
//*      implied, and all liability, including consequential and other     *
//*      indirect damages, for the use of this code, including liability   *
//*      for infringement of any proprietary rights, and including the     *
//*      warranties of merchantability and fitness for a particular        *
//*      purpose.  Intel does not assume any responsibility for any        *
//*      errors which may appear in this code nor any responsibility to    *
//*      update it.                                                        *
//*                                                                        *
//**************************************************************************

//**************************************************************************
//*                                                                        *
//*  PURPOSE:                                                              *
//*                                                                        *
//*      This file contains routines to initialize and use video.          *
//*                                                                        *
//**************************************************************************

#include <token.h>
#include <Library/FastVideo.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include "devlib.h"
#include "FastVideoPlatform.h"

#define FRAMEBUFFER     0xB8000
#define COLS            80
#define ROWS            25
#define BYTES_PER_CHAR  2   //ASCII value and attribute byte

//
// Use memory outside the screen area to hold cursor location and
// current color.
//
#define CURSOR_STORAGE  FRAMEBUFFER + ROWS*COLS*BYTES_PER_CHAR
#define COLOR_STORAGE   CURSOR_STORAGE + 4

#define FONTBUFFER      0xA0000
#define FONT_CHAR_SIZE  0x20
#define FONTS_PER_TABLE 0x100

//
// Defines used by VGA jam table
//
#define WRITE_OP    1
#define PUSH_OP     2
#define POP_OP      3
#define READ_OP     4
#define TERM_OP     0xff

typedef struct {
    U16 IoAddr;
    U8 Op;
    U8 Data;
} VGA_IO_JAM;

//
// defines for print routines.
//
typedef char * va_list;

#define INT32_MAX   0x7fffffffU
#ifndef _INTSIZEOF
#define _INTSIZEOF(n)   ( (sizeof(n) + sizeof(U32) - 1) & ~(sizeof(U32) - 1) )
#endif

#ifndef va_start
#define va_start(ap,v)  ( ap = (va_list)&v + _INTSIZEOF(v) )
#endif
#ifndef va_arg
#define va_arg(ap,t)    ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
#endif
#ifndef va_end
#define va_end(ap)  ( ap = (va_list)0 )
#endif
#define VA_LIST va_list
#ifndef VA_START
#define VA_START va_start
#endif
#ifndef VA_END
#define VA_END  va_end
#endif

#define CHAR_CR         0x0d
#define CHAR_LF         0x0a

#define LEFT_JUSTIFY    0x01
#define PREFIX_SIGN     0x02
#define PREFIX_BLANK    0x04
#define PREFIX_ZERO     0x08

#define INT_SIGNED      0x20
#define COMA_TYPE       0x40


//
// Forward Declarations
//
void FvPciWrite8(U32 CfgBase, U8 bus, U8 dev, U8 func, U16 reg, U8 data);
void FvPciWrite16(U32 CfgBase, U8 bus, U8 dev, U8 func, U16 reg, U16 data);
void FvPciWrite32(U32 CfgBase, U8 bus, U8 dev, U8 func, U16 reg, U32 data);
void FvPciRead8(U32 CfgBase, U8 bus, U8 dev, U8 func, U16 reg, U8 *data);
void FvPciRead16(U32 CfgBase, U8 bus, U8 dev, U8 func, U16 reg, U16 *data);
void FvPciRead32(U32 CfgBase, U8 bus, U8 dev, U8 func, U16 reg, U32 *data);
void FvIoWrite8(U16 addr, U8 value);
void FvIoRead8(U16 addr, U8 *value);

U32 AssignBusNumbers(VIDEO_PARAMETERS parameters, U8 *VideoBus);
U32 EnableVgaDecode(VIDEO_PARAMETERS parameters);
U32 AssignResources(VIDEO_PARAMETERS parameters, U8 VideoBus);
U32 ControllerSpecificInit(VIDEO_PARAMETERS parameters, U8 VideoBus);
U32 VgaRegisterJam(VGA_IO_JAM *JamTable);
U32 VgaInit(VIDEO_PARAMETERS parameters, U8 VideoBus);
U32 FontMapLoad(VIDEO_PARAMETERS parameters);
U32 VGAEarlyInit(VIDEO_PARAMETERS parameters);


U32 ValidateFontInput(FONT_MAP_INFO *FontMap, U8 FontMapCount);
U32 LoadFont(FONT_MAP_INFO FontMapInfo);


U32 ScrollUp();
U32 PrintChar(char c);
U32 VideoPrintWorker(const char *Format, va_list Marker);
void VidUintnToStr (U32 Value, char *Str, U32 Width, U32 Flags, U32 Base);
U32 VidStrToNumber(char **String);
#define PCIE_REG_ADDR(Bus,Device,Function,Offset) \
  (((Offset) & 0xfff) | (((Function) & 0x07) << 12) | (((Device) & 0x1f) << 15) | (((Bus) & 0xff) << 20))

UCHAR jVersion[] = "v.1.01.00";
_PCI_DEVICE Device_List[] = {
    {0x1A03, 0x2000},
    {0xFFFF, 0xFFFF}
};

void DataDump()
{
  U32 MemBase;
  U8 i,data8, data8_1, data8_2;
  
  data8 = 0;
  MemBase = FRAMEBUFFER;
  DEBUG((EFI_D_ERROR, "\nMemDump(0xB8000):0x%x \n", MemBase));
  for(i=0; i<0x10; i++){
    data8 = *(U8 *)(MemBase+i);
    DEBUG((EFI_D_ERROR, "%02x ",data8));
  }
     
  IoRead8(0x3da);
  IoWrite8(0x3c0, 0);
  data8 = IoRead8(0x3c1);
  IoRead8(0x3da);
  IoWrite8(0x3c0, 0x20);
  DEBUG((EFI_D_ERROR, "\nIoDump(0x3c1):%02x",data8));
  
  IoWrite8(0x3c7, 0);
  data8 = IoRead8(0x3c9);
  data8_1 = IoRead8(0x3c9);
  data8_2 = IoRead8(0x3c9);
  DEBUG((EFI_D_ERROR, "\nIoDump(0x3c9):%02x %02x %02x \n",data8,data8_1,data8_2));

  data8 = 0;
  DEBUG((EFI_D_ERROR, "\nIO Dump(0x3c4)\n", MemBase));
  for(i=0; i<0x5; i++){
    IoWrite8(0x3c4, i);
    data8 = IoRead8(0x3c5);
    DEBUG((EFI_D_ERROR, "%02x ",data8));
  }
  
  IoWrite8(0x3ce, 6);
  data8 = IoRead8(0x3cf);
  DEBUG((EFI_D_ERROR, "\nIO Dump(0x3ce:0x06):0x%x\n ",data8));

	
  DEBUG((EFI_D_ERROR, "\nIO Dump(0x3d5):\n"));
  for(i=0; i<0x19; i++){
    IoWrite8(0x3d4, i);
    data8 = IoRead8(0x3d5);
    DEBUG((EFI_D_ERROR, "0x%02x ",data8));
    if((i+1)%0x10 == 0) DEBUG((EFI_D_ERROR, "\n"));
  }
  
  DEBUG((EFI_D_ERROR, "\nIO Dump(0x3d5 Ext):\n",data8));
  for(i=0; i<0x30; i++){
    IoWrite8(0x3d4, 0xA0+i);
    data8 = IoRead8(0x3d5);
    DEBUG((EFI_D_ERROR, "%02x ",data8));
    if((i+1)%0x10 == 0) DEBUG((EFI_D_ERROR, "\n"));
  }
}

U32 InitVGA2 (void *ptr, void *ptr2)
{
	U32 Status;
  _PCI_DEVICE *pDeviceList;
  _DEVInfo DevInfo;
  VIDEO_PARAMETERS tParameters = {
    0x0,            //RootBus
    mVidPath,       //*DevPath
    3,              //DevPathEntries
    0x80000000,     //PciExpressCfgBase
    0,     			//MemBase
    0x2000000,      //MemSizeMax
    0x1000,         //IoBase
    0x4000,         //IoSizeMax
    &mFontMapInfo,  //*FontMap
    1               //FontMapCount
  };
  
  DEBUG((0xffffffff, "ASPEED Graphics Init Utility %s \n", jVersion));
  ZeroMem((VOID *)&DevInfo, sizeof(_DEVInfo));
  
  pDeviceList  = Device_List;
  if (FVGetDevInfo (pDeviceList->usVendorID, pDeviceList->usDeviceID, &DevInfo) == 0)
  {
    DEBUG ((0xffffffff, "[ERROR] Can't Find the Device\n"));
    return VIDEO_SUCCESS;
  }
  
  //Re-initialize tParameters
  tParameters.MemBase = DevInfo.ulMMIOBase;
  tParameters.MemSizeMax = DevInfo.ulMMIOSize;
  tParameters.IoBase = DevInfo.usRelocateIOBase;
  
   Status = VideoInit(tParameters, &DevInfo);  
//   gVgaInitStatus = Status;
   if (Status != VIDEO_SUCCESS){
	   DEBUG((EFI_D_ERROR, "VideoInit Error:%02x\n",Status));
	   return Status;
   }

	VideoClear(); 
	VideoSetColor((VIDEO_COLOR_BLACK << 4) | VIDEO_COLOR_WHITE);
#if DEBUG_MODE
	VideoGotoXY(0,ROW_NUMBER_FOR_MODULE_INFO);
	VideoPrint("System Initializing..."); //<bieyb-20171115 Revise output string for different platforms>
#endif
	return VIDEO_SUCCESS;
    
}


//
// Main API functions - see headers in FastVideo.h for more info
//

U32 VideoInit(VIDEO_PARAMETERS parameters, _DEVInfo *DevInfo) 
{
    U8 vBus = 0;
    U32 Status;

    DEBUG((EFI_D_ERROR, "VideoInit Entry \n"));
    
    Status = EnableVgaDecode(parameters);
//    DEBUG((EFI_D_ERROR, "EnableVgaDecode Status %02x: \n",Status ));
    if (Status != VIDEO_SUCCESS) return Status;   

    Status = VGAEarlyInit(parameters);
//    DEBUG((EFI_D_ERROR, "VGAEarlyInit Status %02x: \n",Status ));
    if (Status != VIDEO_SUCCESS) return Status;
    
//    UCHAR *mmiobase = DevInfo->pjMMIOLinear;                        //N_Phenix
//    MOutdwmm(mmiobase, 0x1e6e2000, 0x1688a8a8);                     //N_Phenix
//    DEBUG ((DEBUG_ERROR, "%x\n", MIndwmm(mmiobase, 0x1e6e2000)));   //N_Phenix       
    
    vInitAST2500DRAMReg_Ex(DevInfo); //<lvych00120161009>
	
    Status = VgaInit(parameters, vBus);
//    DEBUG((EFI_D_ERROR, "VgaInit Status %02x: \n",Status ));
    if (Status != VIDEO_SUCCESS) return Status;

    Status = FontMapLoad(parameters);
//    DEBUG((EFI_D_ERROR, "FontMapLoad Status %02x: \n",Status ));
    if (Status != VIDEO_SUCCESS) return Status;

//    DataDump();
    
//    IoWrite8(0x3c3, 0x00);
//    IoWrite8(0x3c2, 0xE3);
    
    FvPciWrite8(parameters.PciExpressCfgBase, 0, parameters.DevPath[0].Dev, parameters.DevPath[0].Func, 0x19, 0);
    FvPciWrite8(parameters.PciExpressCfgBase, 0, parameters.DevPath[0].Dev, parameters.DevPath[0].Func, 0x1A, 0);

    return VIDEO_SUCCESS;
}

U32 VideoClear() {
    U8 *framebuffer = (U8 *)FRAMEBUFFER;
    U32 *cursor = (U32 *)CURSOR_STORAGE;
    U8 *color = (U8 *)COLOR_STORAGE;
    U32 fIdx = 0;

    for (fIdx = 0; fIdx < ROWS*COLS*BYTES_PER_CHAR; fIdx+=2) {
        framebuffer[fIdx] = ' ';
        framebuffer[fIdx+1] = 7;
    }

    *cursor = 0;
    *color = VIDEO_COLOR_WHITE;
    
    return VIDEO_SUCCESS;
}

U32 VideoClearRow(U8 y) {
    U8 *framebuffer = (U8 *)FRAMEBUFFER;
    U32 *cursor = (U32 *)CURSOR_STORAGE;
    U32 fIdx;
    
    *cursor = y*COLS*BYTES_PER_CHAR;

    for (fIdx = *cursor; fIdx < *cursor + COLS*BYTES_PER_CHAR; fIdx+=2) {
        framebuffer[fIdx] = ' ';
        framebuffer[fIdx+1] = 7;
    }
    return VIDEO_SUCCESS;
}

U32 VideoPrintXY(U8 x, U8 y, const char* Format, ...) {
    va_list Args;

    VideoGotoXY(x, y);
    VA_START(Args, Format);
    VideoPrintWorker(Format, Args);
    VA_END(Args);

    return VIDEO_SUCCESS;
}

U32 VideoPrint(const char* Format, ...) {
    va_list Args;

    VA_START (Args, Format);
    VideoPrintWorker(Format, Args);
    VA_END (Args);

    return VIDEO_SUCCESS;
}

U32 VideoSetColor(U8 c) {
    U8 *color = (U8 *)COLOR_STORAGE;
    *color = c;

    return VIDEO_SUCCESS;
}

U32 VideoGotoXY(U8 x, U8 y) {
    U32 *cursor = (U32 *)CURSOR_STORAGE;
    
    if (x >= COLS || y >= ROWS) return VIDEO_ERR_BAD_PARM;

    *cursor = y*COLS*BYTES_PER_CHAR + x*BYTES_PER_CHAR;

    return VIDEO_SUCCESS;
}



//
// Private functions
//

void FvPciWrite8(U32 CfgBase, U8 bus, U8 dev, U8 func, U16 reg, U8 data) {
    U8 *regAddr;
    
    regAddr = (U8 *)(CfgBase | PCIE_REG_ADDR(bus, dev, func, reg));
    
    *(volatile U8 *)regAddr = data;
        
}

void FvPciWrite16(U32 CfgBase, U8 bus, U8 dev, U8 func, U16 reg, U16 data) {
    U8 *regAddr;
    
    regAddr = (U8 *)(CfgBase | PCIE_REG_ADDR(bus, dev, func, reg));
    
    *(volatile U16 *)regAddr = data;

}

void FvPciWrite32(U32 CfgBase, U8 bus, U8 dev, U8 func, U16 reg, U32 data) {
    U8 *regAddr;
    
    regAddr = (U8 *)(CfgBase | PCIE_REG_ADDR(bus, dev, func, reg));
    
    *(volatile U32 *)regAddr = data;
}

void FvPciRead8(U32 CfgBase, U8 bus, U8 dev, U8 func, U16 reg, U8 *data) {    
    U8 *regAddr;
    
    regAddr = (U8 *)(CfgBase | PCIE_REG_ADDR(bus, dev, func, reg));

    *data = *(volatile U8 *)regAddr;
}

void FvPciRead16(U32 CfgBase, U8 bus, U8 dev, U8 func, U16 reg, U16 *data) {
    U8 *regAddr;
    
    regAddr = (U8 *)(CfgBase | PCIE_REG_ADDR(bus, dev, func, reg));
    *data = *(volatile U16 *)regAddr;

}

void FvPciRead32(U32 CfgBase, U8 bus, U8 dev, U8 func, U16 reg, U32 *data) {
    U8 *regAddr;
    
    regAddr = (U8 *)(CfgBase | PCIE_REG_ADDR(bus, dev, func, reg));
    *data = *(volatile U32 *)regAddr;
}

void FvIoWrite8(U16 addr, U8 value) {
/*    _asm {
        mov dx, addr
        mov al, value
        out dx, al
    }
    */
  IoWrite8(addr, value);
}

void FvIoRead8(U16 addr, U8 *value) {
    U8 data8;
    /*
    _asm {
        mov dx, addr
        in  al, dx
        mov data8, al
    }*/
    data8 = IoRead8(addr);
    *value = data8;
}

/*
 * AssignBusNumbers- Assigns bus numbers along the device path from the root bus to the video controller.
 * 
 *
 * Input:
 *    Parameters - VIDEO_PARAMETERS structure specifying parameters for video initialization
 *   *VideoBus - pointer to a U8 which will be updated with the bus number assigned to the video controller.
 *
 * Output:
 *   VIDEO_SUCCESS if everything worked, one of the video error codes if something did not work or bad input was provided.
 */
U32 AssignBusNumbers(VIDEO_PARAMETERS parameters, U8 *VideoBus) {
    U8 bus, dev, func, devPathIdx,data8;
    U32 base;

    bus = parameters.RootBus;
    base = parameters.PciExpressCfgBase;

    //
    // Iterate through the bridges in the device path and assign bus numbers
    //
    for (devPathIdx = 0; devPathIdx < parameters.DevPathEntries-1; devPathIdx++) {
        dev = parameters.DevPath[devPathIdx].Dev;
        func = parameters.DevPath[devPathIdx].Func;
        //
        // Read header type and validate that this is a bridge
        //
        FvPciRead8(base, bus, dev, func, 0x0E, &data8);
        if ( (data8 & 0x7f) != 0x01) {
            return VIDEO_ERR_BAD_DEVPATH;
        }
        
        
        //Assign primary bus number
        FvPciWrite8(base, bus, dev, func, 0x18, bus);
        //Assign secondary and subordinate bus numbers
        FvPciWrite8(base, bus, dev, func, 0x19, bus+1); 
        FvPciWrite8(base, bus, dev, func, 0x1A, bus+2); 
        bus++;
    }

    //
    // Read header type and validate that this is an endpoint device
    //
    dev = parameters.DevPath[devPathIdx].Dev;
    func = parameters.DevPath[devPathIdx].Func;
    FvPciRead8(base, bus, dev, func, 0x0E, &data8);
    if ( (data8 & 0x7f) != 0x00) {
        return VIDEO_ERR_BAD_DEVPATH;
    }
    
    *VideoBus = bus;

    return VIDEO_SUCCESS;
}

/*
 * EnableVgaDecode- Enables legacy VGA decode down to the video controller
 * 
 *
 * Input:
 *    Parameters - VIDEO_PARAMETERS structure specifying parameters for video initialization
 *
 * Output:
 *   VIDEO_SUCCESS if everything worked, one of the video error codes if something did not work or bad input was provided.
 */
U32 EnableVgaDecode(VIDEO_PARAMETERS parameters) {
    U8  bus, dev, func, devPathIdx;
    U16 data16;
    U32 base;
    U8 data8=0;

    base = parameters.PciExpressCfgBase;
    bus = parameters.RootBus;

    //
    // Iterate through all the bridges in the dev path and enable Mem/IO decode and VGA decode.
    //
    for (devPathIdx = 0; devPathIdx < parameters.DevPathEntries-1; devPathIdx++) {
        dev = parameters.DevPath[devPathIdx].Dev;
        func = parameters.DevPath[devPathIdx].Func;
        //
        //Enable VGA decode
        //
        FvPciRead8(base, bus, dev, func, 0x0E, &data8);
//        DEBUG((EFI_D_ERROR, "Enable VGA decode BUS:0x%x Dev:0x%x Fun:0x%x Data0e:0x%x\n",bus, dev,func,data8));
        if ( (data8 & 0x7f) == 0x01) {
          FvPciRead16(base, bus, dev, func, 0x3E, &data16);
          data16 |= 0x08;
          FvPciWrite16(base, bus, dev, func, 0x3E, data16);
        }
        
        //
        //Enable Mem & IO decode 
        //
        FvPciRead16(base, bus, dev, func, 0x04, &data16);
        data16 |= 0x03;
        FvPciWrite16(base, bus, dev, func, 0x04, data16);
        
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

    FvPciRead16(base, bus, dev, func, 0x04, &data16);
    data16 |= 0x03;
    FvPciWrite16(base, bus, dev, func, 0x04, data16);
    
    return VIDEO_SUCCESS;
}

/*
 * ControllerSpecificInit- Do any controller specific initialization required
 * 
 *
 * Input:
 *    Parameters - VIDEO_PARAMETERS structure specifying parameters for video initialization
 *    vBus - Bus Number of the vide controller
 *
 * Output:
 *   VIDEO_SUCCESS if everything worked, one of the video error codes if something did not work or bad input was provided.
 */
U32 ControllerSpecificInit(VIDEO_PARAMETERS parameters, U8 vBus) {
  U8 vDev, vFunc;
  U32 base;
  
  vDev = parameters.DevPath[parameters.DevPathEntries-1].Dev;
  vFunc = parameters.DevPath[parameters.DevPathEntries-1].Func;
  base = parameters.PciExpressCfgBase;
  
  //
  // Matrox G200-specific init.
  //
  FvIoWrite8(0x3c8, 0x1e);
  FvPciWrite16(base, vBus, vDev, vFunc, 0x44, 0x3c0a);
  FvPciWrite8(base, vBus, vDev, vFunc, 0x4a, 0x11);


  //
  // Pilot-II series specific init.
  //
  FvIoWrite8(0x3c8, 0x18);
  FvPciWrite16(base, vBus, vDev, vFunc, 0x44, 0x3c0a);
  FvPciWrite8(base, vBus, vDev, vFunc, 0x4a, 0x3);

  FvIoWrite8(0x3c8, 0x1a);
  FvPciWrite16(base, vBus, vDev, vFunc, 0x44, 0x3c0a);
  FvPciWrite8(base, vBus, vDev, vFunc, 0x4a, 0x1);

  FvIoWrite8(0x3c8, 0x1d);
  FvPciWrite16(base, vBus, vDev, vFunc, 0x44, 0x3c0a);
  FvPciWrite8(base, vBus, vDev, vFunc, 0x4a, 0x20);
  
  return VIDEO_SUCCESS;
}




/*
 * VgaRegisterJam- Process VGA register jam table.
 * 
 *
 * Input:
 *    JamTable - pointer to the VGA jam table providing the operations to perform.
 *
 * Output:
 *   VIDEO_SUCCESS if everything worked, one of the video error codes if something did not work or bad input was provided.
 */ 
#define MAX_JAM_STACK 10 //allow stack depth up to 10.
U32 VgaRegisterJam(VGA_IO_JAM *JamTable) {
  U8 JamStack[MAX_JAM_STACK];
  U8 JamStackPtr = 0;
  U32 JamIdx;
  U8 Data8;

  //
  // Iterate through the Jam Table operations
  //
  
  for (JamIdx = 0; JamTable[JamIdx].Op != TERM_OP; JamIdx++) {
    switch (JamTable[JamIdx].Op) {
        case WRITE_OP:
            //
            // Write the data straight to the port
            //
            FvIoWrite8(JamTable[JamIdx].IoAddr, JamTable[JamIdx].Data);
            break;
        case PUSH_OP:
             //
             // Read the data from the Port and store it on the JAM stack
             //
             if (JamStackPtr == MAX_JAM_STACK) {
                return VIDEO_ERR_BAD_PARM;
             }
             FvIoRead8(JamTable[JamIdx].IoAddr, &Data8);
             JamStack[JamStackPtr] = Data8;
             JamStackPtr++;
             break;
        case POP_OP:
             //
             // Read the data from the JAM stack and write it to the port.
             //
             if (JamStackPtr == 0) {
                return VIDEO_ERR_BAD_PARM;
             }
             JamStackPtr--;
             FvIoWrite8(JamTable[JamIdx].IoAddr, JamStack[JamStackPtr]);
             break;
        case READ_OP:
             //
             //Read the data straight from the port.
             // 
             FvIoRead8(JamTable[JamIdx].IoAddr, &Data8);
             break;
        default:
            return VIDEO_ERR_BAD_PARM;
    }
  }
  return VIDEO_SUCCESS;
}


VGA_IO_JAM mGenericRegs[];
VGA_IO_JAM mSequencerRegs[];
VGA_IO_JAM mCrtRegs[];
VGA_IO_JAM mGraphicsRegs[];
VGA_IO_JAM mAttribRegs[];
VGA_IO_JAM mDacRegs[];
VGA_IO_JAM CRTC_ExtReg[];


void EnableVGA(VIDEO_PARAMETERS parameters){

  IoWrite8(0x3c3, 0x01);
  IoWrite8(0x3c2, 0xE3);
  
//  return;
}

void OpenKey(VIDEO_PARAMETERS parameters){
  
  IoWrite8(0x3d4, 0x80);
  IoWrite8(0x3d5, 0xA8);
  
//  return;
}

U32 VGAEarlyInit(VIDEO_PARAMETERS parameters){

  U8 i;
  U32 Status;
  
  EnableVGA(parameters);
  OpenKey(parameters);
  
  for(i=0x81; i<0x9E; i++){
    IoWrite8(0x3d4, i);
    IoWrite8(0x3d5, 0x00);    
  }
  
  Status = VgaRegisterJam(CRTC_ExtReg);
  if (Status != VIDEO_SUCCESS) return Status;
  
  
  return VIDEO_SUCCESS;
}

/*
 * VgaInit- Initialize the Video controller itself
 * 
 *
 * Input:
 *    Parameters - VIDEO_PARAMETERS structure specifying parameters for video initialization
 *    vBus - Bus Number of the vide controller
 *
 * Output:
 *   VIDEO_SUCCESS if everything worked, one of the video error codes if something did not work or bad input was provided.
 */
U32 VgaInit(VIDEO_PARAMETERS parameters, U8 vBus) {
    U32 Status;

//    Status = ControllerSpecificInit(parameters, vBus);
//    if (Status != VIDEO_SUCCESS) return Status;

    Status = VgaRegisterJam(mGenericRegs);
    if (Status != VIDEO_SUCCESS) return Status;

    Status = VgaRegisterJam(mSequencerRegs);
    if (Status != VIDEO_SUCCESS) return Status;

    Status = VgaRegisterJam(mCrtRegs);
    if (Status != VIDEO_SUCCESS) return Status;
	
    Status = VgaRegisterJam(mAttribRegs);
    if (Status != VIDEO_SUCCESS) return Status;

    Status = VgaRegisterJam(mGraphicsRegs);
    if (Status != VIDEO_SUCCESS) return Status;

    Status = VgaRegisterJam(mDacRegs);
    if (Status != VIDEO_SUCCESS) return Status;
 
    return VIDEO_SUCCESS;
}


//
// Jam tables required for Font Map load.
//

VGA_IO_JAM mVideoOff[] = {
  {0x3da, READ_OP,  0x00},
  {0x3c0, WRITE_OP, 0x00}, //Attr Idx 0x00 -> turn off video
  {0xff,TERM_OP,0xff}               //table terminator  
};

VGA_IO_JAM mVideoOn[] = {
  {0x3da, READ_OP,  0x00},
  {0x3c0, WRITE_OP, 0x20}, //Attr Idx 0x00 -> turn on video
  {0xff,TERM_OP,0xff}               //table terminator
};

VGA_IO_JAM mEnablePlane2RW[] = {
  {0x3c4, WRITE_OP, 0x00}, {0x3c5, WRITE_OP, 0x01}, //Seq Idx 0 -> Sync Reset
  {0x3c4, WRITE_OP, 0x02}, {0x3c5, WRITE_OP, 0x04}, //Seq Idx 2 -> Enable Write to Plane-2
  {0x3c4, WRITE_OP, 0x04}, {0x3c5, WRITE_OP, 0x07}, //Seq Idx 4 -> Enable sequential access
  {0x3c4, WRITE_OP, 0x00}, {0x3c5, WRITE_OP, 0x00}, //Seq Idx 0 -> deassert sync reset for normal operation
  
  {0x3ce, WRITE_OP, 0x04}, {0x3cf, WRITE_OP, 0x02}, //GC idx = 4 -> select only plane 2 for read.
  {0x3ce, WRITE_OP, 0x05}, {0x3cf, WRITE_OP, 0x00}, //GC idx = 5 -> disable odd/even addressing.
  {0x3ce, WRITE_OP, 0x06}, {0x3cf, WRITE_OP, 0x04}, //GC idx = 6 -> Select Text, disable odd/even addressing, 64K memory at A0000h  
  
  {0xff,TERM_OP,0xff}               //table terminator
};

VGA_IO_JAM mEnablePlane01RW[] = {
  {0x3c4, WRITE_OP, 0x00}, {0x3c5, WRITE_OP, 0x01}, //Seq Idx 0 -> Sync Reset
  {0x3c4, WRITE_OP, 0x02}, {0x3c5, WRITE_OP, 0x03}, //Seq Idx 2 -> Enable write to Plane-0, Plane-1
  {0x3c4, WRITE_OP, 0x04}, {0x3c5, WRITE_OP, 0x03}, //Seq Idx 4 -> Enable odd/even addressing in text mode.
  {0x3c4, WRITE_OP, 0x00}, {0x3c5, WRITE_OP, 0x03}, //Seq Idx 0->Normal Operation

  {0x3ce, WRITE_OP, 0x04}, {0x3cf, WRITE_OP, 0x00}, //GC idx = 4 -> Select plane-0, plane-1 for read.
  {0x3ce, WRITE_OP, 0x05}, {0x3cf, WRITE_OP, 0x10}, //GC idx = 5 -> enable odd/even addressing
  {0x3ce, WRITE_OP, 0x06}, {0x3cf, WRITE_OP, 0x0E}, //GC idx = 6 -> Select Text, Enable Odd/Even addressing, 32K memory at B8000h.
  
  {0xff,TERM_OP,0xff}               //table terminator
};

/*
 * FontMapLoad- Load the Font Map
 * 
 *
 * Input:
 *    Parameters - VIDEO_PARAMETERS structure specifying parameters for video initialization
 *
 * Output:
 *   VIDEO_SUCCESS if everything worked, one of the video error codes if something did not work or bad input was provided.
 */
U32 FontMapLoad(VIDEO_PARAMETERS parameters) {
    U32 Status;
    U8 FontMapIndex;


    Status = ValidateFontInput(parameters.FontMap, parameters.FontMapCount);
    if (Status != VIDEO_SUCCESS) return Status;

    Status = VgaRegisterJam(mVideoOff);
    if (Status != VIDEO_SUCCESS) return Status;

    Status = VgaRegisterJam(mEnablePlane2RW);
    if (Status != VIDEO_SUCCESS) return Status;

    for (FontMapIndex = 0; FontMapIndex < parameters.FontMapCount; FontMapIndex++) {
        Status = LoadFont(parameters.FontMap[FontMapIndex]);
        if (Status != VIDEO_SUCCESS) return Status;
    }

    Status = VgaRegisterJam(mEnablePlane01RW);
    if (Status != VIDEO_SUCCESS) return Status;

    Status = VgaRegisterJam(mVideoOn);
    if (Status != VIDEO_SUCCESS) return Status;


    return VIDEO_SUCCESS;
}

/*
 * ValidateFontInput- Validate that the font map is sane
 * 
 *
 * Input:
 *    Parameters - VIDEO_PARAMETERS structure specifying parameters for video initialization
 *
 * Output:
 *   VIDEO_SUCCESS if everything worked, one of the video error codes if something did not work or bad input was provided.
 */
U32 ValidateFontInput(FONT_MAP_INFO *FontMap, U8 FontMapCount) {
    U8 FontMapIndex;

    for (FontMapIndex = 0; FontMapIndex < FontMapCount; FontMapIndex++) {
        if (FontMap[FontMapIndex].CharCount == 0) { 
            return VIDEO_ERR_BAD_PARM;
        }
        if ( ((U16)FontMap[FontMapIndex].StartChar) + ((U16)FontMap[FontMapIndex].CharCount) >= 0x100) {
            return VIDEO_ERR_BAD_PARM;
        }
        if (FontMap[FontMapIndex].CharSize >= 0x20) {
            return VIDEO_ERR_BAD_PARM;
        }
        if (FontMap[FontMapIndex].TargetTable != ALL_TABLES && FontMap[FontMapIndex].TargetTable >= 8) {
            return VIDEO_ERR_BAD_PARM;
        }
    }
    return VIDEO_SUCCESS;
}

/*
 * LoadFont - Load the font table into video RAM
 * 
 *
 * Input:
 *    Parameters - VIDEO_PARAMETERS structure specifying parameters for video initialization
 *
 * Output:
 *   VIDEO_SUCCESS if everything worked, one of the video error codes if something did not work or bad input was provided.
 */
U32 LoadFont(FONT_MAP_INFO FontMapInfo) {
    U8 *fontbuffer;
    U8 tblIdx, fontChar, fontByteIdx;
    if (FontMapInfo.TargetTable != ALL_TABLES) {
        fontbuffer = (U8 *) (FONTBUFFER); 
        fontbuffer += FontMapInfo.TargetTable * FONTS_PER_TABLE * FONT_CHAR_SIZE;
        fontbuffer += FontMapInfo.StartChar * FONT_CHAR_SIZE;

        for (fontChar = 0; fontChar < FontMapInfo.CharCount;  fontChar++) {
          for (fontByteIdx = 0; fontByteIdx < FontMapInfo.CharSize; fontByteIdx++) {
                fontbuffer[fontChar * FONT_CHAR_SIZE + fontByteIdx] = 
                    FontMapInfo.FontMap[fontChar * FontMapInfo.CharSize + fontByteIdx];
            }
            for (fontByteIdx = FontMapInfo.CharSize; fontByteIdx < FONT_CHAR_SIZE; fontByteIdx++) {
                fontbuffer[fontChar * FONT_CHAR_SIZE + fontByteIdx] = 0;
            }
        }
    } else {
        for (tblIdx = 0; tblIdx < 8; tblIdx++) {
            FontMapInfo.TargetTable = tblIdx;
            LoadFont(FontMapInfo);
        }
    }
    return VIDEO_SUCCESS;
}


/*
 * ScrollUp - Scroll the screen up by one line
 * 
 *
 * Input:
 *    None.
 *
 * Output:
 *   VIDEO_SUCCESS if everything worked, one of the video error codes if something did not work or bad input was provided.
 */
U32 ScrollUp() {
    U8 *framebuffer = (U8 *) FRAMEBUFFER;
    U32 fIdx;

    for (fIdx = 0; fIdx < (ROWS-1)*COLS*BYTES_PER_CHAR; fIdx++) {
      framebuffer[fIdx] = framebuffer[fIdx + COLS*BYTES_PER_CHAR];
    }
    for (fIdx = (ROWS-1)*COLS*BYTES_PER_CHAR; fIdx < ROWS*COLS*BYTES_PER_CHAR; fIdx+=2) {
      framebuffer[fIdx] = ' ';
    }
    
    return VIDEO_SUCCESS;
}

/*
 * PrintChar-Print a character to video
 * 
 *
 * Input:
 *    c - Character to print
 *
 * Output:
 *   VIDEO_SUCCESS if everything worked, one of the video error codes if something did not work or bad input was provided.
 */
U32 PrintChar(char c) {
    U8 *framebuffer = (U8 *) FRAMEBUFFER;
    U32 *cursor = (U32 *)CURSOR_STORAGE;
    U8 *color = (U8 *)COLOR_STORAGE;

    if (c == '\n' || c == 0xa || c == 0xd) {
      *cursor = *cursor + (COLS*BYTES_PER_CHAR);
	  *cursor = *cursor / (COLS*BYTES_PER_CHAR);
	  *cursor = *cursor * (COLS*BYTES_PER_CHAR);
	} else {
      framebuffer[*cursor] = c;
      *cursor= *cursor + 1;
      framebuffer[*cursor] = *color;
      *cursor= *cursor + 1;
    }
    if (*cursor >= ROWS*COLS*BYTES_PER_CHAR) {
      ScrollUp();
      *cursor = (ROWS-1)*COLS*BYTES_PER_CHAR;
    } 
    
    return VIDEO_SUCCESS;
}

//
// Macros for digit matching
//
#ifndef isdigit
#define isdigit(_c) ( ((_c) >= '0') && ((_c) <= '9'))
#endif
#ifndef ishexdigit
#define ishexdigit(_c) ( ((_c) >= 'a') && ((_c) <= 'f'))
#endif
/*
 * VideoPrintWorker- worker for processing format string for print to video
 * 
 *
 * Input:
 *    Format - Format String
 *    Marker - variables for format string
 *
 * Output:
 *   VIDEO_SUCCESS if everything worked, one of the video error codes if something did not work or bad input was provided.
 */
U32 VideoPrintWorker(const char *Format, va_list Marker) {
    char *p;
    U32 Width;
    U32 Flags;
    char Str[160];
    char *String;
    U32 CharCount;

    for (p = (char *)Format, CharCount = 0; *p; p++) {
      if ( *p != '%' ) {
         PrintChar(*p);
      } else {
         p++;
         // Check for flags
         Flags = 0;
         if ( *p == '-') {
            Flags |= LEFT_JUSTIFY;
         } else if ( *p == '+' ) {
            Flags |= PREFIX_SIGN;
         } else if ( *p == ' ' ) {
            Flags |= PREFIX_BLANK;
         }
         if (Flags != 0) {
            p++;
         }

         // Check for width
         if ( isdigit(*p) ) {
            if ( *p == '0' ) {
               Flags |= PREFIX_ZERO;
            }
            Width = VidStrToNumber(&p);
         } else if ( *p == '*' ) {
            Width = va_arg( Marker, int );
            p++;
         } else {
            Width = 0;
         }

         if ( *p == ',' ) {
            Flags |= COMA_TYPE;
            p++;
         }

         // get type
         String = Str;
         switch (*p) {
         case 'd':
         case 'i':
                // always print as UINTN will need extra code to print different widths
            VidUintnToStr ((U32)va_arg( Marker, U32 *),Str , Width, Flags | INT_SIGNED, 10);
         break;
         case 'u':
                // always print as UINTN will need extra code to print different widths
            VidUintnToStr ((U32)va_arg( Marker, U32 *),Str , Width, Flags, 10);
         break;
         case 'x':
         case 'X':
                // always print as UINTN will need extra code to print different widths
            VidUintnToStr ((U32)va_arg( Marker, U32 *),Str , Width, Flags, 16);
         break;
         case 'c':
            PrintChar (va_arg (Marker, char));
            Str[0] = '\0';
         break;
         case 's':
            String = (char *)va_arg (Marker, char *);
         break;
         case 'p':
            VidUintnToStr ((U32)va_arg( Marker, U32 *),Str , Width, Flags, 16);
         break;
         }
         while (*String != '\0') {
            PrintChar (*String++);
         }
      }
    }

    return VIDEO_SUCCESS;
}

//-----------------------------------------------------------------------------
void VidUintnToStr (U32 Value, char *Str, U32 Width, U32 Flags, U32 Base)
{
   U32 Negative;
   U32 Int;
   char *Ptr;
   char Prefix;
   char c;
   U32 i;
   const char Hex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

   Ptr = Str;
   if ( (Value > INT32_MAX) && (Flags & INT_SIGNED) ) {
      Int = ~Value + 1;  /* -Value */
      Negative = 1;
   } else {
      Int = Value;
      Negative = 0;
   }

   i = 0;
   do {           /* generate digits in reverse order */
      i++;
      *Ptr++ = Hex[Int % Base];
      if ( Flags & COMA_TYPE ) {
         if ( Base == 16 ) {
            if ( i % 4 == 0 ) {
               *Ptr++ = ',';
            }
         } else if ( Base == 10 ) {
            if ( i % 3 == 0 ) {
               *Ptr++ = ',';
            }
         }
      }
   } while ((Int /= Base) > 0);
   if ( *(Ptr-1) == ',') {
      Ptr--;
   }
   if (Negative) {
      *Ptr++ = '-';
   } else if ( Flags & PREFIX_SIGN ) {
      *Ptr++ = '+';
   }
   if ( Flags & PREFIX_ZERO ) {
      Prefix = '0';
   } else if ( !(Flags & LEFT_JUSTIFY)) {
      Prefix = ' ';
   } else {
      Prefix = 0x00;
   }
   if (Prefix != 0x00) {
      for (i = (int)(Ptr - Str); i < Width; i++) {
         *Ptr++ = Prefix;
      }
   }
   *Ptr = '\0';

   /* reverse string */
   while (Str < --Ptr) {
      c = *Str;
      *Str++ = *Ptr;
      *Ptr = c;
   }
}

//-----------------------------------------------------------------------------
U32 VidStrToNumber(char **String)
{
   U32  Sum;
   char *Str;

   Str = *String;
   if (*Str == '0') {
      Str++;
   }
   Sum = 0;
   while (isdigit(*Str)) {
      Sum = Sum*10 + (*Str++ - '0');
   }
   *String = Str;
   return (Sum);
}


//
// Jam tables for VGA init
//

VGA_IO_JAM mGenericRegs[] = {
  {0x3c4, WRITE_OP, 0x00}, //Set Sequencer Idx = 0
  {0x3c5, PUSH_OP,  0x00}, //Save Sequencer Reset value
  {0x3c5, WRITE_OP, 0x00}, //set reset
  {0x3c2, WRITE_OP, 0x67}, // Set Misc Output Value
  {0x3c5, POP_OP,   0x00}, //Restore Sequencer Reset value
  {0x3da, WRITE_OP, 0x00}, //Set Feature Control Reg
  {0x3c3, WRITE_OP, 0xff}, //Set Video Subsystem Enable Reg
  {0xff,TERM_OP,0xff}               //table terminator
};

VGA_IO_JAM mSequencerRegs[] = {
  {0x3c4, WRITE_OP, 0x00}, //Seq Idx: 0x00
  {0x3c5, WRITE_OP, 0x03},
  {0x3c4, WRITE_OP, 0x00}, //Seq Idx:0 - Reset 
  {0x3c5, PUSH_OP,  0x00}, //Save Sequencer Reset value
  {0x3c5, WRITE_OP, 0x00}, //set reset
  {0x3c4, WRITE_OP, 0x01}, //Seq Idx:1 - Clocking Mode
  {0x3c5, WRITE_OP, 0x00},
  {0x3c4, WRITE_OP, 0x00}, //Seq Idx:0 - Reset
  {0x3c5, POP_OP,   0x00}, //Restore Sequencer Reset value
  {0x3c4, WRITE_OP, 0x02}, //Seq Idx: 0x02
  {0x3c5, WRITE_OP, 0x03},
  {0x3c4, WRITE_OP, 0x03}, //Seq Idx: 0x03
  {0x3c5, WRITE_OP, 0x00},
  {0x3c4, WRITE_OP, 0x04}, //Seq Idx: 0x04
  {0x3c5, WRITE_OP, 0x02},
  {0xff,TERM_OP,0xff}         //table terminator
};

VGA_IO_JAM mCrtRegs[] = {
  {0x3d4, WRITE_OP, 0x00}, //CRT Idx: 0x00
  {0x3d5, WRITE_OP, 0x5f},
  {0x3d4, WRITE_OP, 0x01}, //CRT Idx: 0x01
  {0x3d5, WRITE_OP, 0x4f},
  {0x3d4, WRITE_OP, 0x02}, //CRT Idx: 0x02
  {0x3d5, WRITE_OP, 0x50},
  {0x3d4, WRITE_OP, 0x03}, //CRT Idx: 0x03
  {0x3d5, WRITE_OP, 0x82},
  {0x3d4, WRITE_OP, 0x04}, //CRT Idx: 0x04
  {0x3d5, WRITE_OP, 0x52},
  {0x3d4, WRITE_OP, 0x05}, //CRT Idx: 0x05
  {0x3d5, WRITE_OP, 0x9e},
  {0x3d4, WRITE_OP, 0x06}, //CRT Idx: 0x06
  {0x3d5, WRITE_OP, 0xbf},
  {0x3d4, WRITE_OP, 0x07}, //CRT Idx: 0x07
  {0x3d5, WRITE_OP, 0x1f},
  {0x3d4, WRITE_OP, 0x08}, //CRT Idx: 0x08
  {0x3d5, WRITE_OP, 0x00},
  {0x3d4, WRITE_OP, 0x09}, //CRT Idx: 0x09
  {0x3d5, WRITE_OP, 0x4f},
  {0x3d4, WRITE_OP, 0x0a}, //CRT Idx: 0x0a
  {0x3d5, WRITE_OP, 0x1d},
  {0x3d4, WRITE_OP, 0x0b}, //CRT Idx: 0x0b
  {0x3d5, WRITE_OP, 0x0e},
  {0x3d4, WRITE_OP, 0x0c}, //CRT Idx: 0x0c
  {0x3d5, WRITE_OP, 0x00},
  {0x3d4, WRITE_OP, 0x0d}, //CRT Idx: 0x0d
  {0x3d5, WRITE_OP, 0x00},
  {0x3d4, WRITE_OP, 0x0e}, //CRT Idx: 0x0e
  {0x3d5, WRITE_OP, 0x00},
  {0x3d4, WRITE_OP, 0x0f}, //CRT Idx: 0x0f
  {0x3d5, WRITE_OP, 0x00},
  {0x3d4, WRITE_OP, 0x10}, //CRT Idx: 0x10
  {0x3d5, WRITE_OP, 0x9d},
  {0x3d4, WRITE_OP, 0x11}, //CRT Idx: 0x11
  {0x3d5, WRITE_OP, 0x80},
  {0x3d4, WRITE_OP, 0x12}, //CRT Idx: 0x12
  {0x3d5, WRITE_OP, 0x8f},
  {0x3d4, WRITE_OP, 0x13}, //CRT Idx: 0x13
  {0x3d5, WRITE_OP, 0x28},
  {0x3d4, WRITE_OP, 0x14}, //CRT Idx: 0x14
  {0x3d5, WRITE_OP, 0x1f},
  {0x3d4, WRITE_OP, 0x15}, //CRT Idx: 0x15
  {0x3d5, WRITE_OP, 0x96},
  {0x3d4, WRITE_OP, 0x16}, //CRT Idx: 0x16
  {0x3d5, WRITE_OP, 0xb9},
  {0x3d4, WRITE_OP, 0x17}, //CRT Idx: 0x17
  {0x3d5, WRITE_OP, 0xa3},
  {0x3d4, WRITE_OP, 0x18}, //CRT Idx: 0x18
  {0x3d5, WRITE_OP, 0xff},
  {0xff,TERM_OP,0xff}      //table terminator  
};


VGA_IO_JAM CRTC_ExtReg[] = {
  //Exe. REG
  {0x3d4, WRITE_OP, 0xA0}, //CRT Idx: 0xA0
  {0x3d5, WRITE_OP, 0x0F},
  {0x3d4, WRITE_OP, 0xA1}, //CRT Idx: 0xA1
  {0x3d5, WRITE_OP, 0x04},
  {0x3d4, WRITE_OP, 0xA2}, //CRT Idx: 0xA2
  {0x3d5, WRITE_OP, 0x1F},
  
  {0x3d4, WRITE_OP, 0xA3}, //CRT Idx: 0xA3
  {0x3d5, WRITE_OP, 0x00},
  {0x3d4, WRITE_OP, 0xA8}, //CRT Idx: 0xA8
  {0x3d5, WRITE_OP, 0x00},
  
  {0x3d4, WRITE_OP, 0xA6}, //CRT Idx: 0xA6
  {0x3d5, WRITE_OP, 0x60},//0x78},
  {0x3d4, WRITE_OP, 0xA7}, //CRT Idx: 0xA7
  {0x3d5, WRITE_OP, 0x78},//0x60},
  
  {0x3d4, WRITE_OP, 0xB6}, //CRT Idx: 0xB6
  {0x3d5, WRITE_OP, 0x24}, 
  {0x3d4, WRITE_OP, 0xB7}, //CRT Idx: 0xB7
  {0x3d5, WRITE_OP, 0xF0}, 

  {0xff,TERM_OP,0xff}
};

VGA_IO_JAM mGraphicsRegs[] = {
  {0x3ce, WRITE_OP, 0x00}, //Graphics Idx 0x00
  {0x3cf, WRITE_OP, 0x00},
  {0x3ce, WRITE_OP, 0x01}, //Graphics Idx 0x01
  {0x3cf, WRITE_OP, 0x00},
  {0x3ce, WRITE_OP, 0x02}, //Graphics Idx 0x02
  {0x3cf, WRITE_OP, 0x00},
  {0x3ce, WRITE_OP, 0x03}, //Graphics Idx 0x03
  {0x3cf, WRITE_OP, 0x00},
  {0x3ce, WRITE_OP, 0x04}, //Graphics Idx 0x04
  {0x3cf, WRITE_OP, 0x00},
  {0x3ce, WRITE_OP, 0x05}, //Graphics Idx 0x05
  {0x3cf, WRITE_OP, 0x10},
  {0x3ce, WRITE_OP, 0x06}, //Graphics Idx 0x06
  {0x3cf, WRITE_OP, 0x0e},
  {0x3ce, WRITE_OP, 0x07}, //Graphics Idx 0x07
  {0x3cf, WRITE_OP, 0x00},
  {0x3ce, WRITE_OP, 0x08}, //Graphics Idx 0x08
  {0x3cf, WRITE_OP, 0xff},
  {0xff,TERM_OP,0xff}      //table terminator  
};

VGA_IO_JAM mAttribRegs[] = {
  {0x3da, READ_OP,  0x00},
  {0x3c0, WRITE_OP, 0x00}, //Attr Idx 0x00
  {0x3c0, WRITE_OP, 0x00},
  {0x3da, READ_OP,  0x00},
  {0x3c0, WRITE_OP, 0x01}, //Attr Idx 0x01
  {0x3c0, WRITE_OP, 0x01},
  {0x3da, READ_OP,  0x00},
  {0x3c0, WRITE_OP, 0x02}, //Attr Idx 0x02
  {0x3c0, WRITE_OP, 0x02},
  {0x3da, READ_OP,  0x00},
  {0x3c0, WRITE_OP, 0x03}, //Attr Idx 0x03
  {0x3c0, WRITE_OP, 0x03},
  {0x3da, READ_OP,  0x00},
  {0x3c0, WRITE_OP, 0x04}, //Attr Idx 0x04
  {0x3c0, WRITE_OP, 0x04},
  {0x3da, READ_OP,  0x00},
  {0x3c0, WRITE_OP, 0x05}, //Attr Idx 0x05
  {0x3c0, WRITE_OP, 0x05},
  {0x3da, READ_OP,  0x00},
  {0x3c0, WRITE_OP, 0x06}, //Attr Idx 0x06
  {0x3c0, WRITE_OP, 0x14},
  {0x3da, READ_OP,  0x00},
  {0x3c0, WRITE_OP, 0x07}, //Attr Idx 0x07
  {0x3c0, WRITE_OP, 0x07},
  {0x3da, READ_OP,  0x00},
  {0x3c0, WRITE_OP, 0x08}, //Attr Idx 0x08
  {0x3c0, WRITE_OP, 0x38},
  {0x3da, READ_OP,  0x00},
  {0x3c0, WRITE_OP, 0x09}, //Attr Idx 0x09
  {0x3c0, WRITE_OP, 0x39},
  {0x3da, READ_OP,  0x00},
  {0x3c0, WRITE_OP, 0x0a}, //Attr Idx 0x0a
  {0x3c0, WRITE_OP, 0x3a},
  {0x3da, READ_OP,  0x00},
  {0x3c0, WRITE_OP, 0x0b}, //Attr Idx 0x0b
  {0x3c0, WRITE_OP, 0x3b},
  {0x3da, READ_OP,  0x00},
  {0x3c0, WRITE_OP, 0x0c}, //Attr Idx 0x0c
  {0x3c0, WRITE_OP, 0x3c},
  {0x3da, READ_OP,  0x00},
  {0x3c0, WRITE_OP, 0x0d}, //Attr Idx 0x0d
  {0x3c0, WRITE_OP, 0x3d},
  {0x3da, READ_OP,  0x00},
  {0x3c0, WRITE_OP, 0x0e}, //Attr Idx 0x0e
  {0x3c0, WRITE_OP, 0x3e},
  {0x3da, READ_OP,  0x00},
  {0x3c0, WRITE_OP, 0x0f}, //Attr Idx 0x0f
  {0x3c0, WRITE_OP, 0x3f},
  {0x3da, READ_OP,  0x00},
  {0x3c0, WRITE_OP, 0x10}, //Attr Idx 0x10
  {0x3c0, WRITE_OP, 0x0c},
  {0x3da, READ_OP,  0x00},
  {0x3c0, WRITE_OP, 0x11}, //Attr Idx 0x11
  {0x3c0, WRITE_OP, 0x00},
  {0x3da, READ_OP,  0x00},
  {0x3c0, WRITE_OP, 0x12}, //Attr Idx 0x12
  {0x3c0, WRITE_OP, 0x0f},
  {0x3da, READ_OP,  0x00},
  {0x3c0, WRITE_OP, 0x13}, //Attr Idx 0x13
  {0x3c0, WRITE_OP, 0x08},
  {0x3da, READ_OP,  0x00},
  {0x3c0, WRITE_OP, 0x14}, //Attr Idx 0x14
  {0x3c0, WRITE_OP, 0x00},
  {0x3da, READ_OP,  0x00},
  {0x3c0, WRITE_OP, 0x20},
  {0xff,TERM_OP,0xff}         //table terminator  
};


VGA_IO_JAM mDacRegs[] = {
  {0x3c7, WRITE_OP, 0x00}, //set DAC index to 0x00
  //Pallet Reg 0x00
  {0x3c8, WRITE_OP, 0x00},
  {0x3c9, WRITE_OP, 0x00},
  {0x3c9, WRITE_OP, 0x00},
  {0x3c9, WRITE_OP, 0x00},
  //Pallet Reg 0x01
  {0x3c8, WRITE_OP, 0x01},
  {0x3c9, WRITE_OP, 0x00},
  {0x3c9, WRITE_OP, 0x00},
  {0x3c9, WRITE_OP, 0x2a},
  //Pallet Reg 0x02
  {0x3c8, WRITE_OP, 0x02},
  {0x3c9, WRITE_OP, 0x00},
  {0x3c9, WRITE_OP, 0x2a},
  {0x3c9, WRITE_OP, 0x00},
  //Pallet Reg 0x03
  {0x3c8, WRITE_OP, 0x03},
  {0x3c9, WRITE_OP, 0x00},
  {0x3c9, WRITE_OP, 0x2a},
  {0x3c9, WRITE_OP, 0x2a},
  //Pallet Reg 0x04
  {0x3c8, WRITE_OP, 0x04},
  {0x3c9, WRITE_OP, 0x2a},
  {0x3c9, WRITE_OP, 0x00},
  {0x3c9, WRITE_OP, 0x00},
  //Pallet Reg 0x05
  {0x3c8, WRITE_OP, 0x05},
  {0x3c9, WRITE_OP, 0x2a},
  {0x3c9, WRITE_OP, 0x00},
  {0x3c9, WRITE_OP, 0x2a},
  //Pallet Reg 0x06
  {0x3c8, WRITE_OP, 0x06},
  {0x3c9, WRITE_OP, 0x2a},
  {0x3c9, WRITE_OP, 0x2a},
  {0x3c9, WRITE_OP, 0x00},
  //Pallet Reg 0x07
  {0x3c8, WRITE_OP, 0x07},
  {0x3c9, WRITE_OP, 0x2a},
  {0x3c9, WRITE_OP, 0x2a},
  {0x3c9, WRITE_OP, 0x2a},
  //Pallet Reg 0x08
  {0x3c8, WRITE_OP, 0x08},
  {0x3c9, WRITE_OP, 0x00},
  {0x3c9, WRITE_OP, 0x00},
  {0x3c9, WRITE_OP, 0x15},
  //Pallet Reg 0x09
  {0x3c8, WRITE_OP, 0x09},
  {0x3c9, WRITE_OP, 0x00},
  {0x3c9, WRITE_OP, 0x00},
  {0x3c9, WRITE_OP, 0x3f},
  //Pallet Reg 0x0a
  {0x3c8, WRITE_OP, 0x0a},
  {0x3c9, WRITE_OP, 0x00},
  {0x3c9, WRITE_OP, 0x2a},
  {0x3c9, WRITE_OP, 0x15},
  //Pallet Reg 0x0b
  {0x3c8, WRITE_OP, 0x0b},
  {0x3c9, WRITE_OP, 0x00},
  {0x3c9, WRITE_OP, 0x2a},
  {0x3c9, WRITE_OP, 0x3f},
  //Pallet Reg 0x0c
  {0x3c8, WRITE_OP, 0x0c},
  {0x3c9, WRITE_OP, 0x2a},
  {0x3c9, WRITE_OP, 0x00},
  {0x3c9, WRITE_OP, 0x15},
  //Pallet Reg 0x0d
  {0x3c8, WRITE_OP, 0x0d},
  {0x3c9, WRITE_OP, 0x2a},
  {0x3c9, WRITE_OP, 0x00},
  {0x3c9, WRITE_OP, 0x3f},
  //Pallet Reg 0x0e
  {0x3c8, WRITE_OP, 0x0e},
  {0x3c9, WRITE_OP, 0x2a},
  {0x3c9, WRITE_OP, 0x2a},
  {0x3c9, WRITE_OP, 0x15},
  //Pallet Reg 0x0f
  {0x3c8, WRITE_OP, 0x0f},
  {0x3c9, WRITE_OP, 0x2a},
  {0x3c9, WRITE_OP, 0x2a},
  {0x3c9, WRITE_OP, 0x3f},
  //Pallet Reg 0x10
  {0x3c8, WRITE_OP, 0x10},
  {0x3c9, WRITE_OP, 0x00},
  {0x3c9, WRITE_OP, 0x15},
  {0x3c9, WRITE_OP, 0x00},
  //Pallet Reg 0x11
  {0x3c8, WRITE_OP, 0x11},
  {0x3c9, WRITE_OP, 0x00},
  {0x3c9, WRITE_OP, 0x15},
  {0x3c9, WRITE_OP, 0x2a},
  //Pallet Reg 0x12
  {0x3c8, WRITE_OP, 0x12},
  {0x3c9, WRITE_OP, 0x00},
  {0x3c9, WRITE_OP, 0x3f},
  {0x3c9, WRITE_OP, 0x00},
  //Pallet Reg 0x13
  {0x3c8, WRITE_OP, 0x13},
  {0x3c9, WRITE_OP, 0x00},
  {0x3c9, WRITE_OP, 0x3f},
  {0x3c9, WRITE_OP, 0x2a},
  //Pallet Reg 0x14
  {0x3c8, WRITE_OP, 0x14},
  {0x3c9, WRITE_OP, 0x2a},
  {0x3c9, WRITE_OP, 0x15},
  {0x3c9, WRITE_OP, 0x00},
  //Pallet Reg 0x15
  {0x3c8, WRITE_OP, 0x15},
  {0x3c9, WRITE_OP, 0x2a},
  {0x3c9, WRITE_OP, 0x15},
  {0x3c9, WRITE_OP, 0x2a},
  //Pallet Reg 0x16
  {0x3c8, WRITE_OP, 0x16},
  {0x3c9, WRITE_OP, 0x2a},
  {0x3c9, WRITE_OP, 0x3f},
  {0x3c9, WRITE_OP, 0x00},
  //Pallet Reg 0x17
  {0x3c8, WRITE_OP, 0x17},
  {0x3c9, WRITE_OP, 0x2a},
  {0x3c9, WRITE_OP, 0x3f},
  {0x3c9, WRITE_OP, 0x2a},
  //Pallet Reg 0x18
  {0x3c8, WRITE_OP, 0x18},
  {0x3c9, WRITE_OP, 0x00},
  {0x3c9, WRITE_OP, 0x15},
  {0x3c9, WRITE_OP, 0x15},
  //Pallet Reg 0x19
  {0x3c8, WRITE_OP, 0x19},
  {0x3c9, WRITE_OP, 0x00},
  {0x3c9, WRITE_OP, 0x15},
  {0x3c9, WRITE_OP, 0x3f},
  //Pallet Reg 0x1a
  {0x3c8, WRITE_OP, 0x1a},
  {0x3c9, WRITE_OP, 0x00},
  {0x3c9, WRITE_OP, 0x3f},
  {0x3c9, WRITE_OP, 0x15},
  //Pallet Reg 0x1b
  {0x3c8, WRITE_OP, 0x1b},
  {0x3c9, WRITE_OP, 0x00},
  {0x3c9, WRITE_OP, 0x3f},
  {0x3c9, WRITE_OP, 0x3f},
  //Pallet Reg 0x1c
  {0x3c8, WRITE_OP, 0x1c},
  {0x3c9, WRITE_OP, 0x2a},
  {0x3c9, WRITE_OP, 0x15},
  {0x3c9, WRITE_OP, 0x15},
  //Pallet Reg 0x1d
  {0x3c8, WRITE_OP, 0x1d},
  {0x3c9, WRITE_OP, 0x2a},
  {0x3c9, WRITE_OP, 0x15},
  {0x3c9, WRITE_OP, 0x3f},
  //Pallet Reg 0x1e
  {0x3c8, WRITE_OP, 0x1e},
  {0x3c9, WRITE_OP, 0x2a},
  {0x3c9, WRITE_OP, 0x3f},
  {0x3c9, WRITE_OP, 0x15},
  //Pallet Reg 0x1f
  {0x3c8, WRITE_OP, 0x1f},
  {0x3c9, WRITE_OP, 0x2a},
  {0x3c9, WRITE_OP, 0x3f},
  {0x3c9, WRITE_OP, 0x3f},
  //Pallet Reg 0x20
  {0x3c8, WRITE_OP, 0x20},
  {0x3c9, WRITE_OP, 0x15},
  {0x3c9, WRITE_OP, 0x00},
  {0x3c9, WRITE_OP, 0x00},
  //Pallet Reg 0x21
  {0x3c8, WRITE_OP, 0x21},
  {0x3c9, WRITE_OP, 0x15},
  {0x3c9, WRITE_OP, 0x00},
  {0x3c9, WRITE_OP, 0x2a},
  //Pallet Reg 0x22
  {0x3c8, WRITE_OP, 0x22},
  {0x3c9, WRITE_OP, 0x15},
  {0x3c9, WRITE_OP, 0x2a},
  {0x3c9, WRITE_OP, 0x00},
  //Pallet Reg 0x23
  {0x3c8, WRITE_OP, 0x23},
  {0x3c9, WRITE_OP, 0x15},
  {0x3c9, WRITE_OP, 0x2a},
  {0x3c9, WRITE_OP, 0x2a},
  //Pallet Reg 0x24
  {0x3c8, WRITE_OP, 0x24},
  {0x3c9, WRITE_OP, 0x3f},
  {0x3c9, WRITE_OP, 0x00},
  {0x3c9, WRITE_OP, 0x00},
  //Pallet Reg 0x25
  {0x3c8, WRITE_OP, 0x25},
  {0x3c9, WRITE_OP, 0x3f},
  {0x3c9, WRITE_OP, 0x00},
  {0x3c9, WRITE_OP, 0x2a},
  //Pallet Reg 0x26
  {0x3c8, WRITE_OP, 0x26},
  {0x3c9, WRITE_OP, 0x3f},
  {0x3c9, WRITE_OP, 0x2a},
  {0x3c9, WRITE_OP, 0x00},
  //Pallet Reg 0x27
  {0x3c8, WRITE_OP, 0x27},
  {0x3c9, WRITE_OP, 0x3f},
  {0x3c9, WRITE_OP, 0x2a},
  {0x3c9, WRITE_OP, 0x2a},
  //Pallet Reg 0x28
  {0x3c8, WRITE_OP, 0x28},
  {0x3c9, WRITE_OP, 0x15},
  {0x3c9, WRITE_OP, 0x00},
  {0x3c9, WRITE_OP, 0x15},
  //Pallet Reg 0x29
  {0x3c8, WRITE_OP, 0x29},
  {0x3c9, WRITE_OP, 0x15},
  {0x3c9, WRITE_OP, 0x00},
  {0x3c9, WRITE_OP, 0x3f},
  //Pallet Reg 0x2a
  {0x3c8, WRITE_OP, 0x2a},
  {0x3c9, WRITE_OP, 0x15},
  {0x3c9, WRITE_OP, 0x2a},
  {0x3c9, WRITE_OP, 0x15},
  //Pallet Reg 0x2b
  {0x3c8, WRITE_OP, 0x2b},
  {0x3c9, WRITE_OP, 0x15},
  {0x3c9, WRITE_OP, 0x2a},
  {0x3c9, WRITE_OP, 0x3f},
  //Pallet Reg 0x2c
  {0x3c8, WRITE_OP, 0x2c},
  {0x3c9, WRITE_OP, 0x3f},
  {0x3c9, WRITE_OP, 0x00},
  {0x3c9, WRITE_OP, 0x15},
  //Pallet Reg 0x2d
  {0x3c8, WRITE_OP, 0x2d},
  {0x3c9, WRITE_OP, 0x3f},
  {0x3c9, WRITE_OP, 0x00},
  {0x3c9, WRITE_OP, 0x3f},
  //Pallet Reg 0x2e
  {0x3c8, WRITE_OP, 0x2e},
  {0x3c9, WRITE_OP, 0x3f},
  {0x3c9, WRITE_OP, 0x2a},
  {0x3c9, WRITE_OP, 0x15},
  //Pallet Reg 0x2f
  {0x3c8, WRITE_OP, 0x2f},
  {0x3c9, WRITE_OP, 0x3f},
  {0x3c9, WRITE_OP, 0x2a},
  {0x3c9, WRITE_OP, 0x3f},
  //Pallet Reg 0x30
  {0x3c8, WRITE_OP, 0x30},
  {0x3c9, WRITE_OP, 0x15},
  {0x3c9, WRITE_OP, 0x15},
  {0x3c9, WRITE_OP, 0x00},
  //Pallet Reg 0x31
  {0x3c8, WRITE_OP, 0x31},
  {0x3c9, WRITE_OP, 0x15},
  {0x3c9, WRITE_OP, 0x15},
  {0x3c9, WRITE_OP, 0x2a},
  //Pallet Reg 0x32
  {0x3c8, WRITE_OP, 0x32},
  {0x3c9, WRITE_OP, 0x15},
  {0x3c9, WRITE_OP, 0x3f},
  {0x3c9, WRITE_OP, 0x00},
  //Pallet Reg 0x33
  {0x3c8, WRITE_OP, 0x33},
  {0x3c9, WRITE_OP, 0x15},
  {0x3c9, WRITE_OP, 0x3f},
  {0x3c9, WRITE_OP, 0x2a},
  //Pallet Reg 0x34
  {0x3c8, WRITE_OP, 0x34},
  {0x3c9, WRITE_OP, 0x3f},
  {0x3c9, WRITE_OP, 0x15},
  {0x3c9, WRITE_OP, 0x00},
  //Pallet Reg 0x35
  {0x3c8, WRITE_OP, 0x35},
  {0x3c9, WRITE_OP, 0x3f},
  {0x3c9, WRITE_OP, 0x15},
  {0x3c9, WRITE_OP, 0x2a},
  //Pallet Reg 0x36
  {0x3c8, WRITE_OP, 0x36},
  {0x3c9, WRITE_OP, 0x3f},
  {0x3c9, WRITE_OP, 0x3f},
  {0x3c9, WRITE_OP, 0x00},
  //Pallet Reg 0x37
  {0x3c8, WRITE_OP, 0x37},
  {0x3c9, WRITE_OP, 0x3f},
  {0x3c9, WRITE_OP, 0x3f},
  {0x3c9, WRITE_OP, 0x2a},
  //Pallet Reg 0x38
  {0x3c8, WRITE_OP, 0x38},
  {0x3c9, WRITE_OP, 0x15},
  {0x3c9, WRITE_OP, 0x15},
  {0x3c9, WRITE_OP, 0x15},
  //Pallet Reg 0x39
  {0x3c8, WRITE_OP, 0x39},
  {0x3c9, WRITE_OP, 0x15},
  {0x3c9, WRITE_OP, 0x15},
  {0x3c9, WRITE_OP, 0x3f},
  //Pallet Reg 0x3a
  {0x3c8, WRITE_OP, 0x3a},
  {0x3c9, WRITE_OP, 0x15},
  {0x3c9, WRITE_OP, 0x3f},
  {0x3c9, WRITE_OP, 0x15},
  //Pallet Reg 0x3b
  {0x3c8, WRITE_OP, 0x3b},
  {0x3c9, WRITE_OP, 0x15},
  {0x3c9, WRITE_OP, 0x3f},
  {0x3c9, WRITE_OP, 0x3f},
  //Pallet Reg 0x3c
  {0x3c8, WRITE_OP, 0x3c},
  {0x3c9, WRITE_OP, 0x3f},
  {0x3c9, WRITE_OP, 0x15},
  {0x3c9, WRITE_OP, 0x15},
  //Pallet Reg 0x3d
  {0x3c8, WRITE_OP, 0x3d},
  {0x3c9, WRITE_OP, 0x3f},
  {0x3c9, WRITE_OP, 0x15},
  {0x3c9, WRITE_OP, 0x3f},
  //Pallet Reg 0x3e
  {0x3c8, WRITE_OP, 0x3e},
  {0x3c9, WRITE_OP, 0x3f},
  {0x3c9, WRITE_OP, 0x3f},
  {0x3c9, WRITE_OP, 0x15},
  //Pallet Reg 0x3f
  {0x3c8, WRITE_OP, 0x3f},
  {0x3c9, WRITE_OP, 0x3f},
  {0x3c9, WRITE_OP, 0x3f},
  {0x3c9, WRITE_OP, 0x3f},

  {0xff,TERM_OP,0xff}               //table terminator  
};

VOID FVCheckpoint (
    IN VOID *Dummy,
    IN UINT8 Checkpoint
){

  if (0xFFFF == *(U16*)FRAMEBUFFER)
        return ;

  VideoClearRow(ROW_NUMBER_FOR_CHECKPOINT);
  VideoSetColor((VIDEO_COLOR_BLACK << 4) | VIDEO_COLOR_WHITE);
  VideoGotoXY(0,ROW_NUMBER_FOR_CHECKPOINT);
  VideoPrint("Checkpoint:%02X ", Checkpoint);
}

VOID FVCheckpointMinor (
    IN VOID *Dummy,
    IN UINT8 Checkpoint
){
  if (0xFFFF == *(U16*)FRAMEBUFFER)
      return ;
  
  VideoPrint("Minor:%02X", Checkpoint);
//  DEBUG ((EFI_D_ERROR, "Fast Video Check Point Minor:%02X\n", Checkpoint));
}

U32 VideoPrint_ModuleInfo(BOOT_MODULE_TYPE type, const char* Format, ...) {
//#if DEBUG_MODE
  va_list Args;
  
  if (0xFFFF == *(U16*)FRAMEBUFFER)
      return VIDEO_ERR_VGA_NOT_READY;
  
  VideoSetColor((VIDEO_COLOR_BLACK << 4) | VIDEO_COLOR_WHITE);
  VideoClearRow(ROW_NUMBER_FOR_MODULE_INFO);
  VideoGotoXY(0, ROW_NUMBER_FOR_MODULE_INFO); 
  
  switch(type){
    case BOOT_MODULE_PEIM:
      VideoPrint("PEI Module: ");
      break;
    case BOOT_MODULE_DXE_CORE:
      VideoPrint("DXE-Core Entry");
      break;
    case BOOT_MODULE_DXE_DRIVER:
      VideoPrint("DXE Driver Load: ");
      break;
    case BOOT_MODULE_DRIVER_CONNECTED:
      VideoPrint("DXE Driver Connected: ");
      break;
    case BOOT_MODULE_SMM_DRIVER:
      VideoPrint("SMM Driver Load: ");
      break;     
            
    default:
      break;
  }
  
  VA_START (Args, Format);
  VideoPrintWorker(Format, Args);
  VA_END (Args);
//#endif
  return VIDEO_SUCCESS;
}
