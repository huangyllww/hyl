#include "devlib.h"
#include "vgahw.h"
#include "dp501.h"
#include <Library/IoLib.h>
#include <Library/FastVideo.h>


/* init VGA */
ast_ExtRegInfo astExtReg[] = {
    {0xA0, 0x0F},
    {0xA1, 0x04},
    {0xA2, 0x1C},
    {0xB6, 0x04},
    {0xB7, 0x00},
    {0xFF, 0xFF},
};

ast_ExtRegInfo astExtReg_AST2300[] = {
    {0xA0, 0x0F},
    {0xA1, 0x04},
    {0xA2, 0x1F},
    {0xB6, 0x24},
    {0xB7, 0x00},
    {0xFF, 0xFF},
};

/*
 * common API
 */
void enable_vga(_DEVInfo *DevInfo)
{
    if (!(GetReg2(VGA_ENABLE) & 0x01))
    {
        SetReg2(VGA_ENABLE, 0x01);
        IoWrite8(MISC_PORT_WRITE, 0xE3);
    }
}

void open_key(_DEVInfo *DevInfo)
{
   SetIndexReg2(CRTC_PORT,0x80, 0xA8);
}

void DisplayOFF(_DEVInfo *DevInfo)
{   	
   
   SetIndexReg(SEQ_PORT,0x01, 0xDF, 0x20);		/* screen off */   
   
}

void DisplayON(_DEVInfo *DevInfo)
{   	
   
   SetIndexReg(SEQ_PORT,0x01, 0xDF, 0x00);		/* screen on */   
   
}

void SetDPMSOFF(_DEVInfo *DevInfo)
{   	
   open_key(DevInfo);
   SetIndexReg(CRTC_PORT,0xB6, 0xFC, 0x03);		/* dpms off */      
}

void SetDPMSON(_DEVInfo *DevInfo)
{   	 
   open_key(DevInfo);	  
   SetIndexReg(CRTC_PORT,0xB6, 0xFC, 0x00);		/* dpms on */   
}

/* Init DRAM */
__inline ULONG MIndwm(UCHAR *mmiobase, ULONG r)
{
    *(ULONG *) (mmiobase + 0xF004) = r & 0xFFFF0000;
    *(ULONG *) (mmiobase + 0xF000) = 0x1;

    return ( *(volatile ULONG *) (mmiobase + 0x10000 + (r & 0x0000FFFF)) );

}

__inline void MOutdwm(UCHAR *mmiobase, ULONG r, ULONG v)
{

    *(ULONG *) (mmiobase + 0xF004) = r & 0xFFFF0000;
    *(ULONG *) (mmiobase + 0xF000) = 0x1;

    *(volatile ULONG *) (mmiobase + 0x10000 + (r & 0x0000FFFF)) = v;
}

/*
 * Export
 */
void FVGetVRAMInfo(_DEVInfo *DevInfo)
{
    UCHAR jReg;

    /* Get Framebuffer Size */
    jReg = GetIndexReg(CRTC_PORT, 0xAA, 0xFF);
    switch (jReg & 0x03)
    {
    case 0x03:
        DevInfo->ulFBSize = VRAM_SIZE_064M;
        break;
    case 0x02:
        DevInfo->ulFBSize = VRAM_SIZE_032M;
        break;
    case 0x01:
        DevInfo->ulFBSize = VRAM_SIZE_016M;
        break;
    default:
        DevInfo->ulFBSize = VRAM_SIZE_008M;
        break;
    }
} /* GetVRAMInfo */

void FVGetDRAMInfo(_DEVInfo *DevInfo)
{
    ULONG ulData;

    if (DevInfo->jRevision >= 0x20)
    {
      DevInfo->jDRAMType      = DRAMTYPE_1Gx16;
      DevInfo->ulMCLK         = 396;
        DevInfo->ulDRAMBusWidth = 16;

        //Marks for auto-sizing, ycchen@030614
#if 0        
        ulData = MIndwm(DevInfo->pjMMIOLinear, 0x1e6e2070);
      switch (ulData & 0x18000000)  //D[28:27]
      {
      case 0x00000000:
          DevInfo->jDRAMType = DRAMTYPE_512Mx16;
          break;
      case 0x08000000:
          DevInfo->jDRAMType = DRAMTYPE_1Gx16;
          break;
      case 0x10000000:
          DevInfo->jDRAMType = DRAMTYPE_2Gx16;
          break;
      case 0x18000000:
          DevInfo->jDRAMType = DRAMTYPE_4Gx16;
          break;
      }
#endif      
    }
    else if (DevInfo->jRevision >= 0x10)
    {
      DevInfo->jDRAMType      = DRAMTYPE_1Gx16;
      DevInfo->ulMCLK         = 266;
        DevInfo->ulDRAMBusWidth = 32;
        ulData = MIndwm(DevInfo->pjMMIOLinear, 0x1e6e207c);
        if ((ulData & 0x0300) == 0x0200)  /* AST1100/2050/2150 */
        {
          DevInfo->ulMCLK         = 200;
            DevInfo->ulDRAMBusWidth = 16;
        }
    }
    else  /* AST2000 */
    {
      DevInfo->jDRAMType      = DRAMTYPE_512Mx32;
      DevInfo->ulMCLK         = 166;
        DevInfo->ulDRAMBusWidth = 32;
    }

} /* GetDRAMInfo */
