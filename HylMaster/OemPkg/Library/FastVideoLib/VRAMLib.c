#include <token.h>
#include <Library/FastVideo.h>
#include <Library/IoLib.h>

/*
 * I/O Access
 */
#ifdef RelocateIO
#define AR_PORT_WRITE		(DevInfo->usRelocateIOBase + 0x40)
#define MISC_PORT_WRITE		(DevInfo->usRelocateIOBase + 0x42)
#define VGA_ENABLE		(DevInfo->usRelocateIOBase + 0x43)
#define SEQ_PORT 		(DevInfo->usRelocateIOBase + 0x44)
#define DAC_INDEX_WRITE		(DevInfo->usRelocateIOBase + 0x48)
#define DAC_DATA		(DevInfo->usRelocateIOBase + 0x49)
#define GR_PORT			(DevInfo->usRelocateIOBase + 0x4E)
#define CRTC_PORT 		(DevInfo->usRelocateIOBase + 0x54)
#define INPUT_STATUS1_READ	(DevInfo->usRelocateIOBase + 0x5A)
#else   //Std. VGA IO
#define AR_PORT_WRITE		0x3c0	//(dinfo->usRelocateIOBase + 0x40)
#define MISC_PORT_WRITE		0x3c2	//(dinfo->usRelocateIOBase + 0x42)
#define VGA_ENABLE		0x3c3	//(dinfo->usRelocateIOBase + 0x43)
#define SEQ_PORT 		0x3c4	//(dinfo->usRelocateIOBase + 0x44)
#define DAC_INDEX_WRITE		0x3c8	//(dinfo->usRelocateIOBase + 0x48)
#define DAC_DATA		0x3c9	//(dinfo->usRelocateIOBase + 0x49)
#define GR_PORT			0x3ce	//(dinfo->usRelocateIOBase + 0x4E)
#define CRTC_PORT 		0x3d4	//(dinfo->usRelocateIOBase + 0x54)
#define INPUT_STATUS1_READ	0x3da	//(dinfo->usRelocateIOBase + 0x5A)
#endif

/*
 * DQSI DLL CBR Setting
 */
#define CBR_SIZE0            ((1  << 10) - 1)
#define CBR_SIZE1            ((4  << 10) - 1)
#define CBR_SIZE2            ((64 << 10) - 1)
#define CBR_PASSNUM          5
#define CBR_PASSNUM2         5
#define CBR_THRESHOLD        10
#define CBR_THRESHOLD2       10
#define TIMEOUT              5000000
#define CBR_PATNUM           8

//
// AST2500 DRAM Settings
//
//#define TIMEOUT              50000

#define REGTBL_NUM           17
#define REGIDX_010           0
#define REGIDX_014           1
#define REGIDX_018           2
#define REGIDX_020           3
#define REGIDX_024           4
#define REGIDX_02C           5
#define REGIDX_030           6
#define REGIDX_214           7
#define REGIDX_2E0           8
#define REGIDX_2E4           9
#define REGIDX_2E8           10
#define REGIDX_2EC           11
#define REGIDX_2F0           12
#define REGIDX_2F4           13
#define REGIDX_2F8           14
#define REGIDX_RFC           15
#define REGIDX_PLL           16

ULONG ddr3_1600_timing_table_Ex[REGTBL_NUM] = {
0x64604D38,                  // 0x010
0x29690599,                  // 0x014
0x00000300,                  // 0x018
0x00000000,                  // 0x020
0x00000000,                  // 0x024
0x02181E70,                  // 0x02C
0x00000040,                  // 0x030
0x00000024,                  // 0x214
0x02001300,                  // 0x2E0
0x0E0000A0,                  // 0x2E4
0x000E001B,                  // 0x2E8
0x35B8C105,                  // 0x2EC
0x08090408,                  // 0x2F0
0x9B000800,                  // 0x2F4
0x0E400A00,                  // 0x2F8
0x9971452F,                  // tRFC
0x000071C1};                 // PLL

ULONG ddr4_1600_timing_table_Ex[REGTBL_NUM] = {
0x63604E37,                  // 0x010
0xE97AFA99,                  // 0x014
0x00019000,                  // 0x018
0x08000000,                  // 0x020
0x00000400,                  // 0x024
0x00000410,                  // 0x02C
0x00000101,                  // 0x030
0x00000024,                  // 0x214
0x03002900,                  // 0x2E0
0x0E0000A0,                  // 0x2E4
0x000E001C,                  // 0x2E8
0x35B8C106,                  // 0x2EC
0x08080607,                  // 0x2F0
0x9B000900,                  // 0x2F4
0x0E400A00,                  // 0x2F8
0x99714545,                  // tRFC
0x000071C1};                 // PLL

//
// delay
//
VOID udelay_Ex(ULONG us)
{
    ULONG count;
	
    count = (us + 14) / 15;
    do {
        IoRead8(0x61);
        count--;
    } while(count); 
}

UCHAR GetIndexReg_Ex (USHORT usPORT, UCHAR jIndex, UCHAR jMask)
{
     
//    IoWrite16 ((SHORT)usPORT++, (SHORT)jIndex);
        
//    return ( ((UCHAR)inp ((SHORT)usPORT)) & jMask );  
	  IoWrite8(usPORT++, jIndex);
	  return (IoRead8(usPORT) & jMask);		
  
}

UCHAR GetIndexReg2_Ex (USHORT usPORT, UCHAR jIndex)
{
     
//    IoWrite16 ((SHORT)usPORT++, (SHORT)jIndex);
        
//    return ( ((UCHAR)inp ((SHORT)usPORT)) );
	  IoWrite8(usPORT++, jIndex);
	  return (IoRead8(usPORT));	
  
}

/* Init DRAM */
__inline ULONG MIndwm_Ex(UCHAR *mmiobase, ULONG r)
{
    *(ULONG *) (mmiobase + 0xF004) = r & 0xFFFF0000;
    *(ULONG *) (mmiobase + 0xF000) = 0x1;

    return ( *(volatile ULONG *) (mmiobase + 0x10000 + (r & 0x0000FFFF)) );

}

__inline void MOutdwm_Ex(UCHAR *mmiobase, ULONG r, ULONG v)
{

    *(ULONG *) (mmiobase + 0xF004) = r & 0xFFFF0000;
    *(ULONG *) (mmiobase + 0xF000) = 0x1;

    *(volatile ULONG *) (mmiobase + 0x10000 + (r & 0x0000FFFF)) = v;
}

/******************************************************************************
 Check DRAM Size
 1Gb : 0x80000000 ~ 0x87FFFFFF
 2Gb : 0x80000000 ~ 0x8FFFFFFF
 4Gb : 0x80000000 ~ 0x9FFFFFFF
 8Gb : 0x80000000 ~ 0xBFFFFFFF
 *****************************************************************************/
void Check_DRAM_Size_Ex(_DEVInfo *DevInfo, ULONG tRFC)
{
  ULONG reg_04, reg_14;
  UCHAR *mmiobase = DevInfo->pjMMIOLinear;

  //printf("Check DRAM Size...");

  reg_04 = MIndwm_Ex(mmiobase, 0x1E6E0004) & 0xfffffffc;
  reg_14 = MIndwm_Ex(mmiobase, 0x1E6E0014) & 0xffffff00;

  MOutdwm_Ex(mmiobase, 0xA0100000, 0x41424344);
  MOutdwm_Ex(mmiobase, 0x90100000, 0x35363738);
  MOutdwm_Ex(mmiobase, 0x88100000, 0x292A2B2C);
  MOutdwm_Ex(mmiobase, 0x80100000, 0x1D1E1F10);

  // Check 8Gbit
  if(MIndwm_Ex(mmiobase, 0xA0100000) == 0x41424344){
    reg_04 |= 0x03;
    reg_14 |= (tRFC >> 24) & 0xFF;
    //printf("8Gbit\n");
  // Check 4Gbit
  }else if(MIndwm_Ex(mmiobase, 0x90100000) == 0x35363738){
    reg_04 |= 0x02;
    reg_14 |= (tRFC >> 16) & 0xFF;
    //printf("4Gbit\n");
  // Check 2Gbit
  }else if(MIndwm_Ex(mmiobase, 0x88100000) == 0x292A2B2C){
    reg_04 |= 0x01;
    reg_14 |= (tRFC >> 8) & 0xFF;
    //printf("2Gbit\n");
  }else{
    reg_14 |= tRFC & 0xFF;
    //printf("1Gbit\n");
  }
  MOutdwm_Ex(mmiobase, 0x1E6E0004, reg_04);
  MOutdwm_Ex(mmiobase, 0x1E6E0014, reg_14);
}

void Enable_Cache_Ex(_DEVInfo *DevInfo)
{
  ULONG reg_04, data;
  UCHAR *mmiobase = DevInfo->pjMMIOLinear;

  reg_04 = MIndwm_Ex(mmiobase, 0x1E6E0004);
  MOutdwm_Ex(mmiobase, 0x1E6E0004, reg_04 | 0x1000);

  do{
    data = MIndwm_Ex(mmiobase, 0x1E6E0004);
  }while(!(data & 0x80000));
  MOutdwm_Ex(mmiobase, 0x1E6E0004, reg_04 | 0x400);
}


///////////////////////////////////////////////////////////////////////////////
// Set MPLL
///////////////////////////////////////////////////////////////////////////////
void Set_MPLL_Ex(_DEVInfo *DevInfo)
{
  ULONG addr, data, param;
  UCHAR *mmiobase = DevInfo->pjMMIOLinear;

  // Reset MMC
  MOutdwm_Ex(mmiobase, 0x1E6E0000,0xFC600309);
  MOutdwm_Ex(mmiobase, 0x1E6E0034,0x00020080);
  for(addr = 0x1e6e0004;addr < 0x1e6e0090;){
    MOutdwm_Ex(mmiobase, addr, 0x0);
    addr += 4;
  }
  MOutdwm_Ex(mmiobase, 0x1E6E0034,0x00020000);

  MOutdwm_Ex(mmiobase, 0x1E6E2000, 0x1688A8A8);
  data = MIndwm_Ex(mmiobase, 0x1E6E2070) & 0x00800000;
  if(data){                  // CLKIN = 25MHz
    //printf("CLKIN = 25MHz\n");
    param = 0x930023E0;
  }else{
    //printf("CLKIN = 24MHz\n");
    param = 0x93002400;
  }
  MOutdwm_Ex(mmiobase, 0x1E6E2020, param);
  //udelay(100); // delay 3 ms
}

int MMCTestBurst_AST2500_Ex(_DEVInfo *DevInfo, ULONG datagen)
{
  ULONG data, timecnt;
  UCHAR *mmiobase = DevInfo->pjMMIOLinear;

  MOutdwm_Ex(mmiobase, 0x1E6E0070, 0x00000000);
  MOutdwm_Ex(mmiobase, 0x1E6E0070, 0x000000C1 | (datagen << 3));
  timecnt = 0;
  do{
    data = MIndwm_Ex(mmiobase, 0x1E6E0070) & 0x3000;
    if(data & 0x2000){
      return(0);
    }
    if(++timecnt > TIMEOUT){
      //printf("Timeout!!\n");
      MOutdwm_Ex(mmiobase, 0x1E6E0070, 0x00000000);
      return(0);
    }
  }while(!data);
  MOutdwm_Ex(mmiobase, 0x1E6E0070, 0x00000000);
  return(1);
}

int MMCTestSingle_AST2500_Ex(_DEVInfo *DevInfo, ULONG datagen)
{
  ULONG data, timecnt;
  UCHAR *mmiobase = DevInfo->pjMMIOLinear;

  MOutdwm_Ex(mmiobase, 0x1E6E0070, 0x00000000);
  MOutdwm_Ex(mmiobase, 0x1E6E0070, 0x00000085 | (datagen << 3));
  timecnt = 0;
  do{
    data = MIndwm_Ex(mmiobase, 0x1E6E0070) & 0x3000;
    if(data & 0x2000){
      return(0);
    }
    if(++timecnt > TIMEOUT){
      //printf("Timeout!!\n");
      MOutdwm_Ex(mmiobase, 0x1E6E0070, 0x00000000);
      return(0);
    }
  }while(!data);
  MOutdwm_Ex(mmiobase, 0x1E6E0070, 0x00000000);
  return(1);
}

ULONG CBRTest_AST2500_Ex(_DEVInfo *DevInfo)
{
  UCHAR *mmiobase = DevInfo->pjMMIOLinear;
	
  MOutdwm_Ex(mmiobase, 0x1E6E0074, 0x0000FFFF);
  MOutdwm_Ex(mmiobase, 0x1E6E007C, 0xFF00FF00);
  if(!MMCTestBurst_AST2500_Ex(DevInfo, 0)) return(0);
  if(!MMCTestSingle_AST2500_Ex(DevInfo, 0)) return(0);
  return(1);
}

void DDR_Init_Common_Ex(_DEVInfo *DevInfo)
{
  UCHAR *mmiobase = DevInfo->pjMMIOLinear;
	
  MOutdwm_Ex(mmiobase, 0x1E6E0034,0x00020080);
  MOutdwm_Ex(mmiobase, 0x1E6E0008,0x2003000F);
  MOutdwm_Ex(mmiobase, 0x1E6E0038,0x00000FFF);
  MOutdwm_Ex(mmiobase, 0x1E6E0040,0x88448844);
  MOutdwm_Ex(mmiobase, 0x1E6E0044,0x24422288);
  MOutdwm_Ex(mmiobase, 0x1E6E0048,0x22222222);
  MOutdwm_Ex(mmiobase, 0x1E6E004C,0x22222222);
  MOutdwm_Ex(mmiobase, 0x1E6E0050,0x80000000);
  MOutdwm_Ex(mmiobase, 0x1E6E0208,0x00000000);
  MOutdwm_Ex(mmiobase, 0x1E6E0218,0x00000000);
  MOutdwm_Ex(mmiobase, 0x1E6E0220,0x00000000);
  MOutdwm_Ex(mmiobase, 0x1E6E0228,0x00000000);
  MOutdwm_Ex(mmiobase, 0x1E6E0230,0x00000000);
  MOutdwm_Ex(mmiobase, 0x1E6E02A8,0x00000000);
  MOutdwm_Ex(mmiobase, 0x1E6E02B0,0x00000000);
  MOutdwm_Ex(mmiobase, 0x1E6E0240,0x86000000);
  MOutdwm_Ex(mmiobase, 0x1E6E0244,0x00008600);
  MOutdwm_Ex(mmiobase, 0x1E6E0248,0x80000000);
  MOutdwm_Ex(mmiobase, 0x1E6E024C,0x80808080);
}

void Do_DDRPHY_Init_Ex(_DEVInfo *DevInfo)
{
  ULONG data, pass, timecnt;
  UCHAR *mmiobase = DevInfo->pjMMIOLinear;

  pass = 0;
  MOutdwm_Ex(mmiobase, 0x1E6E0060,0x00000005);
  while(!pass){
    for(timecnt = 0;timecnt < TIMEOUT;timecnt++){
      data = MIndwm_Ex(mmiobase, 0x1E6E0060) & 0x1;
      if(!data){
        break;
      }
    }
    if(timecnt != TIMEOUT){
      data = MIndwm_Ex(mmiobase, 0x1E6E0300) & 0x000A0000;
      if(!data){
        pass = 1;
      }
    }
    if(!pass){
      MOutdwm_Ex(mmiobase, 0x1E6E0060,0x00000000);
      udelay_Ex(10); // delay 10 us
      MOutdwm_Ex(mmiobase, 0x1E6E0060,0x00000005);
    }
  }

  MOutdwm_Ex(mmiobase, 0x1E6E0060,0x00000006);
}

///////////////////////////////////////////////////////////////////////////////
void DDR4_Init_AST2500_Ex(_DEVInfo *DevInfo, ULONG *ddr_table)
{
  ULONG data, data2, pass;
  ULONG ddr_vref, phy_vref;
  ULONG min_ddr_vref, min_phy_vref;
  ULONG max_ddr_vref, max_phy_vref;
  UCHAR *mmiobase = DevInfo->pjMMIOLinear;
  
  min_phy_vref = 0;//Machong 2016/1/5 Solve compiling issue

  //printf("DDR4 Init.....\n");
  MOutdwm_Ex(mmiobase, 0x1E6E0004,0x00000313);
  MOutdwm_Ex(mmiobase, 0x1E6E0010,ddr_table[REGIDX_010]);
  MOutdwm_Ex(mmiobase, 0x1E6E0014,ddr_table[REGIDX_014]);
  MOutdwm_Ex(mmiobase, 0x1E6E0018,ddr_table[REGIDX_018]);
  MOutdwm_Ex(mmiobase, 0x1E6E0020,ddr_table[REGIDX_020]);          // MODEREG4/6
  MOutdwm_Ex(mmiobase, 0x1E6E0024,ddr_table[REGIDX_024]);          // MODEREG5
  MOutdwm_Ex(mmiobase, 0x1E6E002C,ddr_table[REGIDX_02C] | 0x100);  // MODEREG0/2
  MOutdwm_Ex(mmiobase, 0x1E6E0030,ddr_table[REGIDX_030]);          // MODEREG1/3

  // DDR PHY Setting
  //printf("Config DDRPHY\n");
  MOutdwm_Ex(mmiobase, 0x1E6E0200,0x42492AAE);
  MOutdwm_Ex(mmiobase, 0x1E6E0204,0x09002000);
  MOutdwm_Ex(mmiobase, 0x1E6E020C,0x55E00B0B);
  MOutdwm_Ex(mmiobase, 0x1E6E0210,0x20000000);
  MOutdwm_Ex(mmiobase, 0x1E6E0214,ddr_table[REGIDX_214]);
  MOutdwm_Ex(mmiobase, 0x1E6E02E0,ddr_table[REGIDX_2E0]);
  MOutdwm_Ex(mmiobase, 0x1E6E02E4,ddr_table[REGIDX_2E4]);
  MOutdwm_Ex(mmiobase, 0x1E6E02E8,ddr_table[REGIDX_2E8]);
  MOutdwm_Ex(mmiobase, 0x1E6E02EC,ddr_table[REGIDX_2EC]);
  MOutdwm_Ex(mmiobase, 0x1E6E02F0,ddr_table[REGIDX_2F0]);
  MOutdwm_Ex(mmiobase, 0x1E6E02F4,ddr_table[REGIDX_2F4]);
  MOutdwm_Ex(mmiobase, 0x1E6E02F8,ddr_table[REGIDX_2F8]);
  MOutdwm_Ex(mmiobase, 0x1E6E0290,0x00100008);
  MOutdwm_Ex(mmiobase, 0x1E6E02C4,0x3C183C3C);
  MOutdwm_Ex(mmiobase, 0x1E6E02C8,0x00631E0E);

  // Controller Setting
  MOutdwm_Ex(mmiobase, 0x1E6E0034,0x0001A991);

  // Train PHY Vref first
  max_phy_vref = 0x0;
  pass = 0;
  MOutdwm_Ex(mmiobase, 0x1E6E02C0,0x00001C06);
  for(phy_vref = 0x40;phy_vref < 0x80;phy_vref++){
    MOutdwm_Ex(mmiobase, 0x1E6E000C,0x00000000);
    MOutdwm_Ex(mmiobase, 0x1E6E0060,0x00000000);
    MOutdwm_Ex(mmiobase, 0x1E6E02CC,phy_vref | (phy_vref << 8));
    // Fire DFI Init
    Do_DDRPHY_Init_Ex(DevInfo);
    MOutdwm_Ex(mmiobase, 0x1E6E000C,0x00005C01);
    if(CBRTest_AST2500_Ex(DevInfo)){
      pass++;
      data = MIndwm_Ex(mmiobase, 0x1E6E03D0);
      data2 = data >> 8;
      data  = data & 0xff;
      if(data > data2){
        data = data2;
      }

      if(max_phy_vref < data){
        max_phy_vref = data;
        min_phy_vref = phy_vref;
      }
    }else if(pass > 0){
      break;
    }
  }
  MOutdwm_Ex(mmiobase, 0x1E6E02CC,min_phy_vref | (min_phy_vref << 8));
  //printf("Target PHY Vref = %x\n",min_phy_vref);

  // Train DDR Vref next
  min_ddr_vref = 0xFF;
  max_ddr_vref = 0x0;
  pass = 0;
  for(ddr_vref = 0x00;ddr_vref < 0x40;ddr_vref++){
    MOutdwm_Ex(mmiobase, 0x1E6E000C,0x00000000);
    MOutdwm_Ex(mmiobase, 0x1E6E0060,0x00000000);
    MOutdwm_Ex(mmiobase, 0x1E6E02C0,0x00000006 | (ddr_vref << 8));
    // Fire DFI Init
    Do_DDRPHY_Init_Ex(DevInfo);
    MOutdwm_Ex(mmiobase, 0x1E6E000C,0x00005C01);
    if(CBRTest_AST2500_Ex(DevInfo)){
      pass++;
      if(min_ddr_vref > ddr_vref){
        min_ddr_vref = ddr_vref;
      }
      if(max_ddr_vref < ddr_vref){
        max_ddr_vref = ddr_vref;
      }
    }else if(pass != 0){
      break;
    }
  }
  MOutdwm_Ex(mmiobase, 0x1E6E000C,0x00000000);
  MOutdwm_Ex(mmiobase, 0x1E6E0060,0x00000000);
  ddr_vref = (min_ddr_vref + max_ddr_vref + 1) >> 1;
  MOutdwm_Ex(mmiobase, 0x1E6E02C0,0x00000006 | (ddr_vref << 8));
  //printf("Target DDR Vref = %x\n",ddr_vref);

  // Wait DDR PHY init done
  Do_DDRPHY_Init_Ex(DevInfo);

  MOutdwm_Ex(mmiobase, 0x1E6E0120,ddr_table[REGIDX_PLL]);
  MOutdwm_Ex(mmiobase, 0x1E6E000C,0x42AA5C81);
  MOutdwm_Ex(mmiobase, 0x1E6E0034,0x0001AF93);

  Check_DRAM_Size_Ex(DevInfo, ddr_table[REGIDX_RFC]);
  Enable_Cache_Ex(DevInfo);
  MOutdwm_Ex(mmiobase, 0x1E6E001C,0x00000008);
  MOutdwm_Ex(mmiobase, 0x1E6E0038,0xFFFFFF00);
}

///////////////////////////////////////////////////////////////////////////////
void DDR3_Init_AST2500_Ex(_DEVInfo *DevInfo, ULONG *ddr_table)
{
  UCHAR *mmiobase = DevInfo->pjMMIOLinear;

  //printf("DDR3 Init.....\n");
  MOutdwm_Ex(mmiobase, 0x1E6E0004,0x00000303);
  MOutdwm_Ex(mmiobase, 0x1E6E0010,ddr_table[REGIDX_010]);
  MOutdwm_Ex(mmiobase, 0x1E6E0014,ddr_table[REGIDX_014]);
  MOutdwm_Ex(mmiobase, 0x1E6E0018,ddr_table[REGIDX_018]);
  MOutdwm_Ex(mmiobase, 0x1E6E0020,ddr_table[REGIDX_020]);          // MODEREG4/6
  MOutdwm_Ex(mmiobase, 0x1E6E0024,ddr_table[REGIDX_024]);          // MODEREG5
  MOutdwm_Ex(mmiobase, 0x1E6E002C,ddr_table[REGIDX_02C] | 0x100);  // MODEREG0/2
  MOutdwm_Ex(mmiobase, 0x1E6E0030,ddr_table[REGIDX_030]);          // MODEREG1/3

  // DDR PHY Setting
  //printf("Config DDRPHY\n");
  MOutdwm_Ex(mmiobase, 0x1E6E0200,0x02492AAE);
  MOutdwm_Ex(mmiobase, 0x1E6E0204,0x00001001);
  MOutdwm_Ex(mmiobase, 0x1E6E020C,0x55E00B0B);
  MOutdwm_Ex(mmiobase, 0x1E6E0210,0x20000000);
  MOutdwm_Ex(mmiobase, 0x1E6E0214,ddr_table[REGIDX_214]);
  MOutdwm_Ex(mmiobase, 0x1E6E02E0,ddr_table[REGIDX_2E0]);
  MOutdwm_Ex(mmiobase, 0x1E6E02E4,ddr_table[REGIDX_2E4]);
  MOutdwm_Ex(mmiobase, 0x1E6E02E8,ddr_table[REGIDX_2E8]);
  MOutdwm_Ex(mmiobase, 0x1E6E02EC,ddr_table[REGIDX_2EC]);
  MOutdwm_Ex(mmiobase, 0x1E6E02F0,ddr_table[REGIDX_2F0]);
  MOutdwm_Ex(mmiobase, 0x1E6E02F4,ddr_table[REGIDX_2F4]);
  MOutdwm_Ex(mmiobase, 0x1E6E02F8,ddr_table[REGIDX_2F8]);
  MOutdwm_Ex(mmiobase, 0x1E6E0290,0x00100008);
  MOutdwm_Ex(mmiobase, 0x1E6E02C0,0x00000006);

  // Controller Setting
  MOutdwm_Ex(mmiobase, 0x1E6E0034,0x00020091);

  // Wait DDR PHY init done
  Do_DDRPHY_Init_Ex(DevInfo);

  MOutdwm_Ex(mmiobase, 0x1E6E0120,ddr_table[REGIDX_PLL]);
  MOutdwm_Ex(mmiobase, 0x1E6E000C,0x42AA5C81);
  MOutdwm_Ex(mmiobase, 0x1E6E0034,0x0001AF93);

  Check_DRAM_Size_Ex(DevInfo, ddr_table[REGIDX_RFC]);
  Enable_Cache_Ex(DevInfo);
  MOutdwm_Ex(mmiobase, 0x1E6E001C,0x00000008);
  MOutdwm_Ex(mmiobase, 0x1E6E0038,0xFFFFFF00);
}

///////////////////////////////////////////////////////////////////////////////
int DRAM_Init_AST2500_Ex(_DEVInfo *DevInfo)
{
  ULONG data;
  UCHAR *mmiobase = DevInfo->pjMMIOLinear;
  
#if 0  
  data = MIndwm_Ex(mmiobase, 0x1E6E2070);
  // Check if ARM/ColdfireV1 disabled, if not then delay 300 ms
  if(!(data & 0x01)){
    Delay(300);
  }
  data = MIndwm_Ex(mmiobase, 0x1E6E2040);
  // Check if BMC had initialized the DRAM
  if(data & 0x80){
    return(0);
  }

  // Disable watchdog
  MOutdwm_Ex(mmiobase, 0x1E78502C,0x00000000);
  MOutdwm_Ex(mmiobase, 0x1E78504C,0x00000000);
#endif
  Set_MPLL_Ex(DevInfo);
  DDR_Init_Common_Ex(DevInfo);
  data = MIndwm_Ex(mmiobase, 0x1E6E2070);
  if(data & 0x01000000){
    DDR4_Init_AST2500_Ex(DevInfo, ddr4_1600_timing_table_Ex);
  }else{
    DDR3_Init_AST2500_Ex(DevInfo, ddr3_1600_timing_table_Ex);
  }
  MOutdwm_Ex(mmiobase, 0x1E6E2040, MIndwm_Ex(mmiobase, 0x1E6E2040) | 0x41);
  // Patch code
  data = MIndwm_Ex(mmiobase, 0x1E6E200C) & 0xF9FFFFFF;
  MOutdwm_Ex(mmiobase, 0x1E6E200C, data | 0x10000000);
  return(1);
}

void vInitAST2500DRAMReg_Ex(_DEVInfo *DevInfo)
{
    ULONG ulTemp;
    UCHAR jReg;
    UCHAR *mmiobase = DevInfo->pjMMIOLinear;
    jReg = GetIndexReg_Ex(CRTC_PORT, 0xD0, 0xFF);
    if ((jReg & 0x80) == 0)			/* VGA only */
    {
        MOutdwm_Ex(mmiobase, 0x1e6e2000, 0x1688a8a8);
	    do {
	    ulTemp = MIndwm_Ex(mmiobase, 0x1e6e2000);
        } while	 (ulTemp != 0x01);
        MOutdwm_Ex(mmiobase, 0x1e6e0000, 0xfc600309);
	    do {
	    ulTemp = MIndwm_Ex(mmiobase, 0x1e6e0000);
        } while	 (ulTemp != 0x01);
    	/* Slow down CPU/AHB CLK in VGA only mode */
	    ulTemp = MIndwm_Ex(mmiobase, 0x1e6e2008);
        ulTemp |= 0x73;
        MOutdwm_Ex(mmiobase, 0x1e6e2008, ulTemp);
		DRAM_Init_AST2500_Ex(DevInfo);
        ulTemp  = MIndwm_Ex(mmiobase, 0x1E6E2040);
//      MOutdwm_Ex(mmiobase, 0x1E6E2040, ulTemp | 0x40);
        MOutdwm_Ex(mmiobase, 0x1E6E2040, 0xC0);
    }
    /* wait ready */
    do {
        jReg = GetIndexReg_Ex(CRTC_PORT, 0xD0, 0xFF);
    } while ((jReg & 0x40) == 0);
} /* vInitAST2500DRAMReg */
