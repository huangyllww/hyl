#include "devlib.h"
#include "vgahw.h"
#include "vgamode.h"
#include "dp501.h"
#include "Token.h"

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>

//
// Mode List
//
OPT_MODE_STRUCT OptModeTable[] = {
    {"mode3+" ,       720,  400,  1},	
    {"640x480x8" ,    640,  480,  8},
    {"640x480x16",    640,  480, 16},
    {"640x480x32",    640,  480, 32},
    {"800x600x8" ,    800,  600,  8},
    {"800x600x16",    800,  600, 16},
    {"800x600x32",    800,  600, 32},
    {"1024x768x8",   1024,  768,  8},
    {"1024x768x16",  1024,  768, 16},
    {"1024x768x32",  1024,  768, 32},
    {"1280x1024x8",  1280, 1024,  8},
    {"1280x1024x16", 1280, 1024, 16},
    {"1280x1024x32", 1280, 1024, 32},
    {"1600x1200x8",  1600, 1200,  8},
    {"1600x1200x16", 1600, 1200, 16},
    {"1600x1200x32", 1600, 1200, 32},
    {"end",1600,1200,  32},
    	
};	

//
// Mode Parameters Table
//
VBIOS_STDTABLE_STRUCT StdTable[] = {
    /* MD_2_3_400 */
    {
        0x67,						
        {0x00,0x03,0x00,0x02},				
        {0x5f,0x4f,0x50,0x82,0x52,0x9e,0xbf,0x1f,	
         0x00,0x4f,0x0d,0x0e,0x00,0x00,0x00,0x00,
         0x9e,0x80,0x8f,0x28,0x1f,0x96,0xb9,0xa3,
         0xff},
        {0x00,0x01,0x02,0x03,0x04,0x05,0x14,0x07,	
         0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
         0x0c,0x00,0x0f,0x08},
        {0x00,0x00,0x00,0x00,0x00,0x10,0x0e,0x00,	
         0xff}
    },   
    /* Mode12/ExtEGATable */
    {
        0xe3, 						
        {0x01,0x0f,0x00,0x06},				
        {0x5f,0x4f,0x50,0x82,0x55,0x81,0x0b,0x3e,       
         0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,       
         0xe9,0x8b,0xdf,0x28,0x00,0xe7,0x04,0xe3,       
         0xff},                                         
        {0x00,0x01,0x02,0x03,0x04,0x05,0x14,0x07,       
         0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,       
         0x01,0x00,0x0f,0x00},                          
        {0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x0f,       
         0xff}                                          
    },
    /* ExtVGATable */
    {
        0x2f,						
        {0x01,0x0f,0x00,0x0e},                          
        {0x5f,0x4f,0x50,0x82,0x54,0x80,0x0b,0x3e,       
         0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,       
         0xea,0x8c,0xdf,0x28,0x40,0xe7,0x04,0xa3,       
         0xff},                                         
        {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,       
         0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,       
         0x01,0x00,0x00,0x00},                          
        {0x00,0x00,0x00,0x00,0x00,0x40,0x05,0x0f,       
         0xff}
    },
    /* ExtHiCTable */
    {
        0x2f,						
        {0x01,0x0f,0x00,0x0e},                          
        {0x5f,0x4f,0x50,0x82,0x54,0x80,0x0b,0x3e,       
         0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,       
         0xea,0x8c,0xdf,0x28,0x40,0xe7,0x04,0xa3,       
         0xff},                                         
        {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,       
         0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,       
         0x01,0x00,0x00,0x00},                          
        {0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x0f,       
         0xff}
    },     
    /* ExtTrueCTable */
    {
        0x2f,						
        {0x01,0x0f,0x00,0x0e},                          
        {0x5f,0x4f,0x50,0x82,0x54,0x80,0x0b,0x3e,       
         0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,       
         0xea,0x8c,0xdf,0x28,0x40,0xe7,0x04,0xa3,       
         0xff},                                         
        {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,       
         0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,       
         0x01,0x00,0x00,0x00},                          
        {0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x0f,       
         0xff}
    },  
};

VBIOS_ENHTABLE_STRUCT  Res640x480Table[] = {
    { 800, 640, 8, 96, 525, 480, 2, 2, VCLK25_175,	/* 60Hz */
      (SyncNN | HBorder | VBorder | Charx8Dot), 60, 1, 0x2E },
    { 832, 640, 16, 40, 520, 480, 1, 3, VCLK31_5,	/* 72Hz */
      (SyncNN | HBorder | VBorder | Charx8Dot), 72, 2, 0x2E  },
    { 840, 640, 16, 64, 500, 480, 1, 3, VCLK31_5,	/* 75Hz */
      (SyncNN | Charx8Dot) , 75, 3, 0x2E },      
    { 832, 640, 56, 56, 509, 480, 1, 3, VCLK36,		/* 85Hz */
      (SyncNN | Charx8Dot) , 85, 4, 0x2E },
    { 832, 640, 56, 56, 509, 480, 1, 3, VCLK36,		/* end */
      (SyncNN | Charx8Dot) , 0xFF, 4, 0x2E },              	      			
};


VBIOS_ENHTABLE_STRUCT  Res800x600Table[] = {
#if 0	
    {1024, 800, 24, 72, 625, 600, 1, 2, VCLK36,		/* 56Hz */
      (SyncPP | Charx8Dot), 56, 1, 0x30 },	      
#endif      
    {1056, 800, 40, 128, 628, 600, 1, 4, VCLK40,	/* 60Hz */ 
      (SyncPP | Charx8Dot), 60, 2, 0x30 },
    {1040, 800, 56, 120, 666, 600, 37, 6, VCLK50,	/* 72Hz */ 
      (SyncPP | Charx8Dot), 72, 3, 0x30 },            
    {1056, 800, 16, 80, 625, 600, 1, 3, VCLK49_5,	/* 75Hz */ 
      (SyncPP | Charx8Dot), 75, 4, 0x30 },  	  			
    {1048, 800, 32, 64, 631, 600, 1, 3, VCLK56_25,	/* 85Hz */ 
      (SyncPP | Charx8Dot), 85, 5, 0x30 },  
    {1048, 800, 32, 64, 631, 600, 1, 3, VCLK56_25,	/* end */ 
      (SyncPP | Charx8Dot), 0xFF, 5, 0x30 },           
};


VBIOS_ENHTABLE_STRUCT  Res1024x768Table[] = {	
    {1344, 1024, 24, 136, 806, 768, 3, 6, VCLK65,	/* 60Hz */ 
      (SyncNN | Charx8Dot), 60, 1, 0x31 },	
    {1328, 1024, 24, 136, 806, 768, 3, 6, VCLK75,	/* 70Hz */ 
      (SyncNN | Charx8Dot), 70, 2, 0x31 },
    {1312, 1024, 16, 96, 800, 768, 1, 3, VCLK78_75,	/* 75Hz */ 
      (SyncPP | Charx8Dot), 75, 3, 0x31 },      
    {1376, 1024, 48, 96, 808, 768, 1, 3, VCLK94_5,	/* 85Hz */ 
      (SyncPP | Charx8Dot), 85, 4, 0x31 },  
    {1376, 1024, 48, 96, 808, 768, 1, 3, VCLK94_5,	/* end */ 
      (SyncPP | Charx8Dot), 0xFF, 4, 0x31 },             
};

VBIOS_ENHTABLE_STRUCT  Res1280x1024Table[] = {	
    {1688, 1280, 48, 112, 1066, 1024, 1, 3, VCLK108,	/* 60Hz */ 
      (SyncPP | Charx8Dot), 60, 1, 0x32 }, 
    {1688, 1280, 16, 144, 1066, 1024, 1, 3, VCLK135,	/* 75Hz */ 
      (SyncPP | Charx8Dot), 75, 2, 0x32 },  
    {1728, 1280, 64, 160, 1072, 1024, 1, 3, VCLK157_5,	/* 85Hz */ 
      (SyncPP | Charx8Dot), 85, 3, 0x32 },    
    {1728, 1280, 64, 160, 1072, 1024, 1, 3, VCLK157_5,	/* end */ 
      (SyncPP | Charx8Dot), 0xFF, 3, 0x32 },                        
};

VBIOS_ENHTABLE_STRUCT  Res1600x1200Table[] = {	
    {2160, 1600, 64, 192, 1250, 1200, 1, 3, VCLK162,	/* 60Hz */ 
      (SyncPP | Charx8Dot), 60, 1, 0x33 }, 
    {2160, 1600, 64, 192, 1250, 1200, 1, 3, VCLK162,	/* end */ 
      (SyncPP | Charx8Dot), 60, 1, 0x33 },         
};

//AST2500 VCLK Table without SCU0130 Setting
VBIOS_DCLK_INFO DCLKTable_AST2500 [] = {
    {0x40, 0x38, 0x73},							//00: VCLK25_175         
    {0x3A, 0x38, 0x43},							//01: VCLK28_322         
    {0x3E, 0x70, 0x73},							//02: VCLK31_5           
    {0x35, 0x70, 0x43},							//03: VCLK36             
    {0x31, 0x28, 0x73},							//04: VCLK40             
    {0x41, 0x68, 0x73}, 						//05: VCLK49_5           
    {0x31, 0x68, 0x53},             			//06: VCLK50    
    {0x4A, 0x68, 0x73},             			//07: VCLK56_25 
    {0x40, 0x68, 0x53},             			//08: VCLK65	   
    {0x31, 0x60, 0x73},             			//09: VCLK75	   
    {0x3A, 0x28, 0x43},							//0A: VCLK78_75          
    {0x3E, 0x60, 0x73},             			//0B: VCLK94_5  
    {0x35, 0x60, 0x63},             			//0C: VCLK108   
    {0x3D, 0x40, 0x63},             			//0D: VCLK135   
    {0x4E, 0x60, 0x63},             			//0E: VCLK157_5 
    {0x35, 0x60, 0x53},							//0F: VCLK162            
    {0x4C, 0x60, 0x63},							//10: VCLK154            
    {0x4F, 0x48, 0x53},							//11: VCLK83.5           
    {0x46, 0x60, 0x73},							//12: VCLK106.5          
    {0x3C, 0x20, 0x63},							//13: VCLK146.25         
    {0x3D, 0x20, 0x63},							//14: VCLK148.5          
    {0x46, 0x68, 0x53},							//15: VCLK71    
    {0x49, 0x68, 0x43},							//16: VCLK88.75
    {0x4e, 0x60, 0x73},							//17: VCLK119
    {0x38, 0x60, 0x73},							//18: VCLK85_5 
    {0x38, 0x20, 0x73},							//19: VCLK97_75 	     	 
};

VBIOS_DCLK_INFO DCLKTable [] = {
    {0x2C, 0xE7, 0x03},					/* 00: VCLK25_175	*/		
    {0x95, 0x62, 0x03},				        /* 01: VCLK28_322	*/
    {0x67, 0x63, 0x01},				        /* 02: VCLK31_5         */
    {0x76, 0x63, 0x01},				        /* 03: VCLK36         	*/
    {0xEE, 0x67, 0x01},				        /* 04: VCLK40          	*/
    {0x82, 0x62, 0x01}, 			        /* 05: VCLK49_5        	*/
    {0xC6, 0x64, 0x01},                        	        /* 06: VCLK50          	*/
    {0x94, 0x62, 0x01},                        	        /* 07: VCLK56_25       	*/
    {0x80, 0x64, 0x00},                        	        /* 08: VCLK65		*/
    {0x7B, 0x63, 0x00},                        	        /* 09: VCLK75	        */
    {0x67, 0x62, 0x00},				        /* 0A: VCLK78_75       	*/
    {0x7C, 0x62, 0x00},                        	        /* 0B: VCLK94_5        	*/
    {0x8E, 0x62, 0x00},                        	        /* 0C: VCLK108         	*/
    {0x85, 0x24, 0x00},                        	        /* 0D: VCLK135         	*/
    {0x67, 0x22, 0x00},                        	        /* 0E: VCLK157_5       	*/
    {0x6A, 0x22, 0x00},				        /* 0F: VCLK162         	*/
};

VBIOS_DAC_INFO DAC_TEXT[] = {
 { 0x00, 0x00, 0x00 },  { 0x00, 0x00, 0x2a },  { 0x00, 0x2a, 0x00 },  { 0x00, 0x2a, 0x2a }, 
 { 0x2a, 0x00, 0x00 },  { 0x2a, 0x00, 0x2a },  { 0x2a, 0x2a, 0x00 },  { 0x2a, 0x2a, 0x2a }, 
 { 0x00, 0x00, 0x15 },  { 0x00, 0x00, 0x3f },  { 0x00, 0x2a, 0x15 },  { 0x00, 0x2a, 0x3f }, 
 { 0x2a, 0x00, 0x15 },  { 0x2a, 0x00, 0x3f },  { 0x2a, 0x2a, 0x15 },  { 0x2a, 0x2a, 0x3f }, 
 { 0x00, 0x15, 0x00 },  { 0x00, 0x15, 0x2a },  { 0x00, 0x3f, 0x00 },  { 0x00, 0x3f, 0x2a }, 
 { 0x2a, 0x15, 0x00 },  { 0x2a, 0x15, 0x2a },  { 0x2a, 0x3f, 0x00 },  { 0x2a, 0x3f, 0x2a }, 
 { 0x00, 0x15, 0x15 },  { 0x00, 0x15, 0x3f },  { 0x00, 0x3f, 0x15 },  { 0x00, 0x3f, 0x3f }, 
 { 0x2a, 0x15, 0x15 },  { 0x2a, 0x15, 0x3f },  { 0x2a, 0x3f, 0x15 },  { 0x2a, 0x3f, 0x3f }, 
 { 0x15, 0x00, 0x00 },  { 0x15, 0x00, 0x2a },  { 0x15, 0x2a, 0x00 },  { 0x15, 0x2a, 0x2a }, 
 { 0x3f, 0x00, 0x00 },  { 0x3f, 0x00, 0x2a },  { 0x3f, 0x2a, 0x00 },  { 0x3f, 0x2a, 0x2a }, 
 { 0x15, 0x00, 0x15 },  { 0x15, 0x00, 0x3f },  { 0x15, 0x2a, 0x15 },  { 0x15, 0x2a, 0x3f }, 
 { 0x3f, 0x00, 0x15 },  { 0x3f, 0x00, 0x3f },  { 0x3f, 0x2a, 0x15 },  { 0x3f, 0x2a, 0x3f }, 
 { 0x15, 0x15, 0x00 },  { 0x15, 0x15, 0x2a },  { 0x15, 0x3f, 0x00 },  { 0x15, 0x3f, 0x2a }, 
 { 0x3f, 0x15, 0x00 },  { 0x3f, 0x15, 0x2a },  { 0x3f, 0x3f, 0x00 },  { 0x3f, 0x3f, 0x2a }, 
 { 0x15, 0x15, 0x15 },  { 0x15, 0x15, 0x3f },  { 0x15, 0x3f, 0x15 },  { 0x15, 0x3f, 0x3f }, 
 { 0x3f, 0x15, 0x15 },  { 0x3f, 0x15, 0x3f },  { 0x3f, 0x3f, 0x15 },  { 0x3f, 0x3f, 0x3f }, 	
};

VBIOS_DAC_INFO DAC_EGA[] = {
 { 0x00, 0x00, 0x00 },  { 0x00, 0x00, 0x2a },  { 0x00, 0x2a, 0x00 },  { 0x00, 0x2a, 0x2a }, 
 { 0x2a, 0x00, 0x00 },  { 0x2a, 0x00, 0x2a },  { 0x2a, 0x2a, 0x00 },  { 0x2a, 0x2a, 0x2a }, 
 { 0x00, 0x00, 0x15 },  { 0x00, 0x00, 0x3f },  { 0x00, 0x2a, 0x15 },  { 0x00, 0x2a, 0x3f }, 
 { 0x2a, 0x00, 0x15 },  { 0x2a, 0x00, 0x3f },  { 0x2a, 0x2a, 0x15 },  { 0x2a, 0x2a, 0x3f }, 
 { 0x00, 0x15, 0x00 },  { 0x00, 0x15, 0x2a },  { 0x00, 0x3f, 0x00 },  { 0x00, 0x3f, 0x2a }, 
 { 0x2a, 0x15, 0x00 },  { 0x2a, 0x15, 0x2a },  { 0x2a, 0x3f, 0x00 },  { 0x2a, 0x3f, 0x2a }, 
 { 0x00, 0x15, 0x15 },  { 0x00, 0x15, 0x3f },  { 0x00, 0x3f, 0x15 },  { 0x00, 0x3f, 0x3f }, 
 { 0x2a, 0x15, 0x15 },  { 0x2a, 0x15, 0x3f },  { 0x2a, 0x3f, 0x15 },  { 0x2a, 0x3f, 0x3f }, 
 { 0x15, 0x00, 0x00 },  { 0x15, 0x00, 0x2a },  { 0x15, 0x2a, 0x00 },  { 0x15, 0x2a, 0x2a }, 
 { 0x3f, 0x00, 0x00 },  { 0x3f, 0x00, 0x2a },  { 0x3f, 0x2a, 0x00 },  { 0x3f, 0x2a, 0x2a }, 
 { 0x15, 0x00, 0x15 },  { 0x15, 0x00, 0x3f },  { 0x15, 0x2a, 0x15 },  { 0x15, 0x2a, 0x3f }, 
 { 0x3f, 0x00, 0x15 },  { 0x3f, 0x00, 0x3f },  { 0x3f, 0x2a, 0x15 },  { 0x3f, 0x2a, 0x3f }, 
 { 0x15, 0x15, 0x00 },  { 0x15, 0x15, 0x2a },  { 0x15, 0x3f, 0x00 },  { 0x15, 0x3f, 0x2a }, 
 { 0x3f, 0x15, 0x00 },  { 0x3f, 0x15, 0x2a },  { 0x3f, 0x3f, 0x00 },  { 0x3f, 0x3f, 0x2a }, 
 { 0x15, 0x15, 0x15 },  { 0x15, 0x15, 0x3f },  { 0x15, 0x3f, 0x15 },  { 0x15, 0x3f, 0x3f }, 
 { 0x3f, 0x15, 0x15 },  { 0x3f, 0x15, 0x3f },  { 0x3f, 0x3f, 0x15 },  { 0x3f, 0x3f, 0x3f }, 	
};

VBIOS_DAC_INFO DAC_VGA[] = {
 { 0x00, 0x00, 0x00 },  { 0x00, 0x00, 0x2a },  { 0x00, 0x2a, 0x00 },  { 0x00, 0x2a, 0x2a }, 
 { 0x2a, 0x00, 0x00 },  { 0x2a, 0x00, 0x2a },  { 0x2a, 0x15, 0x00 },  { 0x2a, 0x2a, 0x2a }, 
 { 0x15, 0x15, 0x15 },  { 0x15, 0x15, 0x3f },  { 0x15, 0x3f, 0x15 },  { 0x15, 0x3f, 0x3f }, 
 { 0x3f, 0x15, 0x15 },  { 0x3f, 0x15, 0x3f },  { 0x3f, 0x3f, 0x15 },  { 0x3f, 0x3f, 0x3f }, 
 { 0x00, 0x00, 0x00 },  { 0x05, 0x05, 0x05 },  { 0x08, 0x08, 0x08 },  { 0x0b, 0x0b, 0x0b }, 
 { 0x0e, 0x0e, 0x0e },  { 0x11, 0x11, 0x11 },  { 0x14, 0x14, 0x14 },  { 0x18, 0x18, 0x18 }, 
 { 0x1c, 0x1c, 0x1c },  { 0x20, 0x20, 0x20 },  { 0x24, 0x24, 0x24 },  { 0x28, 0x28, 0x28 }, 
 { 0x2d, 0x2d, 0x2d },  { 0x32, 0x32, 0x32 },  { 0x38, 0x38, 0x38 },  { 0x3f, 0x3f, 0x3f }, 
 { 0x00, 0x00, 0x3f },  { 0x10, 0x00, 0x3f },  { 0x1f, 0x00, 0x3f },  { 0x2f, 0x00, 0x3f }, 
 { 0x3f, 0x00, 0x3f },  { 0x3f, 0x00, 0x2f },  { 0x3f, 0x00, 0x1f },  { 0x3f, 0x00, 0x10 }, 
 { 0x3f, 0x00, 0x00 },  { 0x3f, 0x10, 0x00 },  { 0x3f, 0x1f, 0x00 },  { 0x3f, 0x2f, 0x00 }, 
 { 0x3f, 0x3f, 0x00 },  { 0x2f, 0x3f, 0x00 },  { 0x1f, 0x3f, 0x00 },  { 0x10, 0x3f, 0x00 }, 
 { 0x00, 0x3f, 0x00 },  { 0x00, 0x3f, 0x10 },  { 0x00, 0x3f, 0x1f },  { 0x00, 0x3f, 0x2f }, 
 { 0x00, 0x3f, 0x3f },  { 0x00, 0x2f, 0x3f },  { 0x00, 0x1f, 0x3f },  { 0x00, 0x10, 0x3f }, 
 { 0x1f, 0x1f, 0x3f },  { 0x27, 0x1f, 0x3f },  { 0x2f, 0x1f, 0x3f },  { 0x37, 0x1f, 0x3f }, 
 { 0x3f, 0x1f, 0x3f },  { 0x3f, 0x1f, 0x37 },  { 0x3f, 0x1f, 0x2f },  { 0x3f, 0x1f, 0x27 }, 
 { 0x3f, 0x1f, 0x1f },  { 0x3f, 0x27, 0x1f },  { 0x3f, 0x2f, 0x1f },  { 0x3f, 0x37, 0x1f }, 
 { 0x3f, 0x3f, 0x1f },  { 0x37, 0x3f, 0x1f },  { 0x2f, 0x3f, 0x1f },  { 0x27, 0x3f, 0x1f }, 
 { 0x1f, 0x3f, 0x1f },  { 0x1f, 0x3f, 0x27 },  { 0x1f, 0x3f, 0x2f },  { 0x1f, 0x3f, 0x37 }, 
 { 0x1f, 0x3f, 0x3f },  { 0x1f, 0x37, 0x3f },  { 0x1f, 0x2f, 0x3f },  { 0x1f, 0x27, 0x3f }, 
 { 0x2d, 0x2d, 0x3f },  { 0x31, 0x2d, 0x3f },  { 0x36, 0x2d, 0x3f },  { 0x3a, 0x2d, 0x3f }, 
 { 0x3f, 0x2d, 0x3f },  { 0x3f, 0x2d, 0x3a },  { 0x3f, 0x2d, 0x36 },  { 0x3f, 0x2d, 0x31 }, 
 { 0x3f, 0x2d, 0x2d },  { 0x3f, 0x31, 0x2d },  { 0x3f, 0x36, 0x2d },  { 0x3f, 0x3a, 0x2d }, 
 { 0x3f, 0x3f, 0x2d },  { 0x3a, 0x3f, 0x2d },  { 0x36, 0x3f, 0x2d },  { 0x31, 0x3f, 0x2d }, 
 { 0x2d, 0x3f, 0x2d },  { 0x2d, 0x3f, 0x31 },  { 0x2d, 0x3f, 0x36 },  { 0x2d, 0x3f, 0x3a }, 
 { 0x2d, 0x3f, 0x3f },  { 0x2d, 0x3a, 0x3f },  { 0x2d, 0x36, 0x3f },  { 0x2d, 0x31, 0x3f }, 
 { 0x00, 0x00, 0x1c },  { 0x07, 0x00, 0x1c },  { 0x0e, 0x00, 0x1c },  { 0x15, 0x00, 0x1c }, 
 { 0x1c, 0x00, 0x1c },  { 0x1c, 0x00, 0x15 },  { 0x1c, 0x00, 0x0e },  { 0x1c, 0x00, 0x07 }, 
 { 0x1c, 0x00, 0x00 },  { 0x1c, 0x07, 0x00 },  { 0x1c, 0x0e, 0x00 },  { 0x1c, 0x15, 0x00 }, 
 { 0x1c, 0x1c, 0x00 },  { 0x15, 0x1c, 0x00 },  { 0x0e, 0x1c, 0x00 },  { 0x07, 0x1c, 0x00 }, 
 { 0x00, 0x1c, 0x00 },  { 0x00, 0x1c, 0x07 },  { 0x00, 0x1c, 0x0e },  { 0x00, 0x1c, 0x15 }, 
 { 0x00, 0x1c, 0x1c },  { 0x00, 0x15, 0x1c },  { 0x00, 0x0e, 0x1c },  { 0x00, 0x07, 0x1c }, 
 { 0x0e, 0x0e, 0x1c },  { 0x11, 0x0e, 0x1c },  { 0x15, 0x0e, 0x1c },  { 0x18, 0x0e, 0x1c }, 
 { 0x1c, 0x0e, 0x1c },  { 0x1c, 0x0e, 0x18 },  { 0x1c, 0x0e, 0x15 },  { 0x1c, 0x0e, 0x11 }, 
 { 0x1c, 0x0e, 0x0e },  { 0x1c, 0x11, 0x0e },  { 0x1c, 0x15, 0x0e },  { 0x1c, 0x18, 0x0e }, 
 { 0x1c, 0x1c, 0x0e },  { 0x18, 0x1c, 0x0e },  { 0x15, 0x1c, 0x0e },  { 0x11, 0x1c, 0x0e }, 
 { 0x0e, 0x1c, 0x0e },  { 0x0e, 0x1c, 0x11 },  { 0x0e, 0x1c, 0x15 },  { 0x0e, 0x1c, 0x18 }, 
 { 0x0e, 0x1c, 0x1c },  { 0x0e, 0x18, 0x1c },  { 0x0e, 0x15, 0x1c },  { 0x0e, 0x11, 0x1c }, 
 { 0x14, 0x14, 0x1c },  { 0x16, 0x14, 0x1c },  { 0x18, 0x14, 0x1c },  { 0x1a, 0x14, 0x1c }, 
 { 0x1c, 0x14, 0x1c },  { 0x1c, 0x14, 0x1a },  { 0x1c, 0x14, 0x18 },  { 0x1c, 0x14, 0x16 }, 
 { 0x1c, 0x14, 0x14 },  { 0x1c, 0x16, 0x14 },  { 0x1c, 0x18, 0x14 },  { 0x1c, 0x1a, 0x14 }, 
 { 0x1c, 0x1c, 0x14 },  { 0x1a, 0x1c, 0x14 },  { 0x18, 0x1c, 0x14 },  { 0x16, 0x1c, 0x14 }, 
 { 0x14, 0x1c, 0x14 },  { 0x14, 0x1c, 0x16 },  { 0x14, 0x1c, 0x18 },  { 0x14, 0x1c, 0x1a }, 
 { 0x14, 0x1c, 0x1c },  { 0x14, 0x1a, 0x1c },  { 0x14, 0x18, 0x1c },  { 0x14, 0x16, 0x1c }, 
 { 0x00, 0x00, 0x10 },  { 0x04, 0x00, 0x10 },  { 0x08, 0x00, 0x10 },  { 0x0c, 0x00, 0x10 }, 
 { 0x10, 0x00, 0x10 },  { 0x10, 0x00, 0x0c },  { 0x10, 0x00, 0x08 },  { 0x10, 0x00, 0x04 }, 
 { 0x10, 0x00, 0x00 },  { 0x10, 0x04, 0x00 },  { 0x10, 0x08, 0x00 },  { 0x10, 0x0c, 0x00 }, 
 { 0x10, 0x10, 0x00 },  { 0x0c, 0x10, 0x00 },  { 0x08, 0x10, 0x00 },  { 0x04, 0x10, 0x00 }, 
 { 0x00, 0x10, 0x00 },  { 0x00, 0x10, 0x04 },  { 0x00, 0x10, 0x08 },  { 0x00, 0x10, 0x0c }, 
 { 0x00, 0x10, 0x10 },  { 0x00, 0x0c, 0x10 },  { 0x00, 0x08, 0x10 },  { 0x00, 0x04, 0x10 }, 
 { 0x08, 0x08, 0x10 },  { 0x0a, 0x08, 0x10 },  { 0x0c, 0x08, 0x10 },  { 0x0e, 0x08, 0x10 }, 
 { 0x10, 0x08, 0x10 },  { 0x10, 0x08, 0x0e },  { 0x10, 0x08, 0x0c },  { 0x10, 0x08, 0x0a }, 
 { 0x10, 0x08, 0x08 },  { 0x10, 0x0a, 0x08 },  { 0x10, 0x0c, 0x08 },  { 0x10, 0x0e, 0x08 }, 
 { 0x10, 0x10, 0x08 },  { 0x0e, 0x10, 0x08 },  { 0x0c, 0x10, 0x08 },  { 0x0a, 0x10, 0x08 }, 
 { 0x08, 0x10, 0x08 },  { 0x08, 0x10, 0x0a },  { 0x08, 0x10, 0x0c },  { 0x08, 0x10, 0x0e }, 
 { 0x08, 0x10, 0x10 },  { 0x08, 0x0e, 0x10 },  { 0x08, 0x0c, 0x10 },  { 0x08, 0x0a, 0x10 }, 
 { 0x0b, 0x0b, 0x10 },  { 0x0c, 0x0b, 0x10 },  { 0x0d, 0x0b, 0x10 },  { 0x0f, 0x0b, 0x10 }, 
 { 0x10, 0x0b, 0x10 },  { 0x10, 0x0b, 0x0f },  { 0x10, 0x0b, 0x0d },  { 0x10, 0x0b, 0x0c }, 
 { 0x10, 0x0b, 0x0b },  { 0x10, 0x0c, 0x0b },  { 0x10, 0x0d, 0x0b },  { 0x10, 0x0f, 0x0b }, 
 { 0x10, 0x10, 0x0b },  { 0x0f, 0x10, 0x0b },  { 0x0d, 0x10, 0x0b },  { 0x0c, 0x10, 0x0b }, 
 { 0x0b, 0x10, 0x0b },  { 0x0b, 0x10, 0x0c },  { 0x0b, 0x10, 0x0d },  { 0x0b, 0x10, 0x0f }, 
 { 0x0b, 0x10, 0x10 },  { 0x0b, 0x0f, 0x10 },  { 0x0b, 0x0d, 0x10 },  { 0x0b, 0x0c, 0x10 }, 
 { 0x00, 0x00, 0x00 },  { 0x00, 0x00, 0x00 },  { 0x00, 0x00, 0x00 },  { 0x00, 0x00, 0x00 }, 
 { 0x00, 0x00, 0x00 },  { 0x00, 0x00, 0x00 },  { 0x00, 0x00, 0x00 },  { 0x00, 0x00, 0x00 }, 	
};

void vResetReg(_DEVInfo *DevInfo)
{
    ULONG i;
    	
    for (i=0xAC; i<=0xB0; i++)
    {
        SetIndexReg2(CRTC_PORT,(UCHAR) i, 0x00);        	    
    }

    SetIndexReg(CRTC_PORT,0xBB, 0x0F, 0x00);    
}

void vSetStdReg(_DEVInfo *DevInfo, PVBIOS_MODE_INFO pVGAModeInfo)
{
	
    PVBIOS_STDTABLE_STRUCT pStdModePtr;
    ULONG i;
    UCHAR jReg;    
        
    pStdModePtr = pVGAModeInfo->pStdTableEntry;
    
    /* Set Misc */
    jReg = pStdModePtr->MISC;
    jReg |= SyncNN; //Lizhq-001-20161026 Enable horizontal&vertical sync polarity to prevent overclocking for higher resolution.
    IoWrite8(MISC_PORT_WRITE, jReg);

    /* Set Seq */
    SetIndexReg2(SEQ_PORT,0x00, 0x03);
    for (i=0; i<4; i++)
    {
        jReg = pStdModePtr->SEQ[i];
    	if (!i) (jReg |= 0x20);			/* display off */
        SetIndexReg2(SEQ_PORT,(UCHAR) (i+1), jReg);    	
    }
    
    /* Set CRTC */
    SetIndexReg(CRTC_PORT,0x11, 0x7F, 0x00);
    for (i=0; i<25; i++)
    {
        jReg = pStdModePtr->CRTC[i];
        SetIndexReg2(CRTC_PORT,(UCHAR) i, jReg);        
    }
    /* Set AR */
    jReg = GetReg2(INPUT_STATUS1_READ);
    for (i=0; i<20; i++)
    {
        jReg = pStdModePtr->AR[i];
        IoWrite8(AR_PORT_WRITE, (UCHAR) i);         
        IoWrite8(AR_PORT_WRITE, jReg);        
    }    
    IoWrite8(AR_PORT_WRITE, 0x14);         
    IoWrite8(AR_PORT_WRITE, 0x00);  
    
    jReg = GetReg2(INPUT_STATUS1_READ);
    IoWrite8 (AR_PORT_WRITE, 0x20);		/* set POS */
                        
    /* Set GR */
    for (i=0; i<9; i++)
    {
	jReg = pStdModePtr->GR[i];    	
        SetIndexReg2(GR_PORT,(UCHAR) i, jReg);                  	    	
    }    
                	
}

void
vSetCRTCReg(EFI_PEI_SERVICES **PeiServices, _DEVInfo *DevInfo, PVBIOS_MODE_INFO pVGAModeInfo)
{
    PVBIOS_ENHTABLE_STRUCT pEnhModePtr;
    ULONG ulHBorder=0, ulVBorder=0, DisableLineCompare=0;        
    USHORT usTemp;	
    UCHAR jReg05, jReg07, jReg09, jRegAC, jRegAD, jRegAE;
    
    jReg05 = jReg07 = jReg09 = jRegAC = jRegAD = jRegAE = 0;

    pEnhModePtr = pVGAModeInfo->pEnhTableEntry;
    if (pEnhModePtr->Flags & HBorder) ulHBorder = 8;
    if (pEnhModePtr->Flags & VBorder) ulVBorder = 8;
    if (pEnhModePtr->Flags & LineCompareOff) DisableLineCompare = 1;
    
    /* unlock CRTC */
    SetIndexReg(CRTC_PORT,0x11, 0x7F, 0x00);

    /* Horizontal Timing Programming */
    usTemp = (USHORT)((pEnhModePtr->HT >> 3) - 5);
    if (usTemp & 0x100) jRegAC |= 0x01;			/* HT D[8] */
    SetIndexReg(CRTC_PORT,0x00, 0x00, (UCHAR) usTemp);
    usTemp = (USHORT)((pEnhModePtr->HDE >> 3) - 1);
    if (usTemp & 0x100) jRegAC |= 0x04;			/* HDE D[8] */
    SetIndexReg(CRTC_PORT,0x01, 0x00, (UCHAR) usTemp);   
    usTemp = (USHORT)(((pEnhModePtr->HDE + ulHBorder) >> 3) - 1);
    if (usTemp & 0x100) jRegAC |= 0x10;			/* HBS D[8] */
    SetIndexReg(CRTC_PORT,0x02, 0x00, (UCHAR) usTemp);      	
    usTemp = (((pEnhModePtr->HT - ulHBorder) >> 3) - 1) & 0x7F;
    if (usTemp & 0x20) jReg05 |= 0x80;			/* HBE D[5] */
    if (usTemp & 0x40) jRegAD |= 0x01;			/* HBE D[6] */    
    SetIndexReg(CRTC_PORT,0x03, 0xE0, (UCHAR) (usTemp & 0x1F));
    usTemp = (USHORT)((pEnhModePtr->HDE + pEnhModePtr->HFP + ulHBorder - 1) >> 3 );
    if (usTemp & 0x100) jRegAC |= 0x40;			/* HRS D[5] */    
    SetIndexReg(CRTC_PORT,0x04, 0x00, (UCHAR) (usTemp)); 
    usTemp = ((pEnhModePtr->HDE + pEnhModePtr->HFP + pEnhModePtr->HSYNC + ulHBorder - 1) >> 3 ) & 0x3F;
    if (usTemp & 0x20) jRegAD |= 0x04;			/* HRE D[5] */    
    SetIndexReg(CRTC_PORT,0x05, 0x60, (UCHAR) ((usTemp & 0x1F) | jReg05));
    SetIndexReg(CRTC_PORT,0xAC, 0x00, (UCHAR) jRegAC);        	
    SetIndexReg(CRTC_PORT,0xAD, 0x00, (UCHAR) jRegAD);  
    /* Vetical Timing Programming */

    usTemp = (USHORT)((pEnhModePtr->VT) - 2);
    if (usTemp & 0x100) jReg07 |= 0x01;			/* VT D[8] */
    if (usTemp & 0x200) jReg07 |= 0x20;			
    if (usTemp & 0x400) jRegAE |= 0x01;			/* VT D[10] */    
    SetIndexReg(CRTC_PORT,0x06, 0x00, (UCHAR) usTemp);  
    usTemp = (USHORT)((pEnhModePtr->VDE + pEnhModePtr->VFP + ulVBorder - 1));
    if (usTemp & 0x100) jReg07 |= 0x04;			/* VRS D[8] */
    if (usTemp & 0x200) jReg07 |= 0x80;			/* VRS D[9] */
    if (usTemp & 0x400) jRegAE |= 0x08;			/* VRS D[10] */    
    SetIndexReg(CRTC_PORT,0x10, 0x00, (UCHAR) usTemp); 
    usTemp = (pEnhModePtr->VDE + pEnhModePtr->VFP + pEnhModePtr->VSYNC + ulVBorder - 1) & 0x3F;
    if (usTemp & 0x10) jRegAE |= 0x20;			/* VRE D[4] */
    if (usTemp & 0x20) jRegAE |= 0x40;			/* VRE D[5] */
    SetIndexReg(CRTC_PORT,0x11, 0x70, (UCHAR) (usTemp & 0x0F)); 
    usTemp = (USHORT)(pEnhModePtr->VDE - 1);
    if (usTemp & 0x100) jReg07 |= 0x02;			/* VDE D[8] */
    if (usTemp & 0x200) jReg07 |= 0x40;			/* VDE D[9] */
    if (usTemp & 0x400) jRegAE |= 0x02;			/* VDE D[10] */  
    SetIndexReg(CRTC_PORT,0x12, 0x00, (UCHAR) usTemp); 
    usTemp = (USHORT)(pEnhModePtr->VDE + ulVBorder - 1);
    if (usTemp & 0x100) jReg07 |= 0x08;			/* VBS D[8] */
    if (usTemp & 0x200) jReg09 |= 0x20;			/* VBS D[9] */
    if (usTemp & 0x400) jRegAE |= 0x04;			/* VBS D[10] */  
    SetIndexReg(CRTC_PORT,0x15, 0x00, (UCHAR) usTemp);     
    usTemp = (USHORT)(pEnhModePtr->VT - ulVBorder - 1);
    if (usTemp & 0x100) jRegAE |= 0x10;			/* VBE D[8] */
    SetIndexReg(CRTC_PORT,0x16, 0x00, (UCHAR) usTemp);                                

    SetIndexReg(CRTC_PORT,0x07, 0x00, (UCHAR) jReg07);                                
    SetIndexReg(CRTC_PORT,0x09, 0xDF, (UCHAR) jReg09);                                
    SetIndexReg(CRTC_PORT,0xAE, 0x00, (UCHAR) (jRegAE | 0x80));	/* disable line compare */
    
    /* lock CRTC */
    SetIndexReg(CRTC_PORT,0x11, 0x7F, 0x80);                                    
                   	
}

void vSetOffsetReg(_DEVInfo *DevInfo, PVBIOS_MODE_INFO pVGAModeInfo)
{
    USHORT usOffset;	

    usOffset = 	(USHORT)((pVGAModeInfo->xres * ((pVGAModeInfo->bpp + 7)/8)) >> 3);		/* Unit: char */
        
    SetIndexReg2(CRTC_PORT,0x13, (UCHAR) (usOffset & 0xFF));         	
    SetIndexReg2(CRTC_PORT,0xB0, (UCHAR) ((usOffset >> 8) & 0x3F));         	
    
}

void vSetDCLKReg(_DEVInfo *DevInfo, PVBIOS_MODE_INFO pVGAModeInfo)
{
    PVBIOS_ENHTABLE_STRUCT pEnhModePtr;
    PVBIOS_DCLK_INFO pDCLKPtr;
	
    /* Formal Setting */
    pEnhModePtr = pVGAModeInfo->pEnhTableEntry;
    if (DevInfo->jRevision == 0x40)	//ast2500a0
    	pDCLKPtr = &DCLKTable_AST2500[pEnhModePtr->DCLKIndex];    
	else    
    	pDCLKPtr = &DCLKTable[pEnhModePtr->DCLKIndex];

    SetIndexReg(CRTC_PORT,0xC0, 0x00,  pDCLKPtr->Param1); 
    SetIndexReg(CRTC_PORT,0xC1, 0x00,  pDCLKPtr->Param2);
    if (DevInfo->jRevision == 0x40)	//ast2500a0
    	SetIndexReg(CRTC_PORT,0xBB, 0x0F,  (pDCLKPtr->Param3 & 0xF0));                                                                                                           
    else                
    	SetIndexReg(CRTC_PORT,0xBB, 0x0F,  ((pDCLKPtr->Param3 & 0xC0) | ((pDCLKPtr->Param3 & 0x03) << 4)) );                                                                                                       
    	
}

void vSetExtReg(_DEVInfo *DevInfo, PVBIOS_MODE_INFO pVGAModeInfo, ULONG OutputOption)
{
    ULONG ulModeID, ulColorIndex = 0, ulRefreshRateIndex = 0;	
    UCHAR jRegA0, jRegA3, jRegA8;
    	
    jRegA0=jRegA3=jRegA8=0;	
    /* Mode Type Setting */	
    switch (pVGAModeInfo->bpp) {
    case 1:
	    ulColorIndex = 0;
    
        jRegA0 = 0x00;
        jRegA3 = 0x00;
        jRegA8 = 0x00;        
        break;
        	    
    case 8:
	ulColorIndex = VGAModeIndex-1;       
    
        jRegA0 = 0x70;
        jRegA3 = 0x01;
        jRegA8 = 0x00;        
        break;
    case 15:        
    case 16:
	ulColorIndex = HiCModeIndex-1;                
    
        jRegA0 = 0x70; 
        jRegA3 = 0x04;
        jRegA8 = 0x00;                   
        break;    
    case 32:
	ulColorIndex = TrueCModeIndex-1;                
    
        jRegA0 = 0x70; 
        jRegA3 = 0x08;
        jRegA8 = 0x00;                   
        break;
    }
    if (OutputOption & Init_DVO)
        jRegA3 |= 0x80;           	
    SetIndexReg(CRTC_PORT,0xA0, 0x8F, (UCHAR) jRegA0);                                
    SetIndexReg(CRTC_PORT,0xA3, 0x00, (UCHAR) jRegA3);                                
    SetIndexReg(CRTC_PORT,0xA8, 0xFD, (UCHAR) jRegA8);                                

    /* Set Threshold */
    if (DevInfo->jRevision >= 0x20)
    {	   
        SetIndexReg2(CRTC_PORT,0xA7, 0x78);                                
        SetIndexReg2(CRTC_PORT,0xA6, 0x60);                                
    }    
    else
    {	   
        SetIndexReg2(CRTC_PORT,0xA7, 0x3F);                                
        SetIndexReg2(CRTC_PORT,0xA6, 0x2F);                                
    }        

    /* Write mode info to scratch */
    if (pVGAModeInfo->bpp > 1)
    {
        ulRefreshRateIndex = pVGAModeInfo->pEnhTableEntry->ulRefreshRateIndex;
        ulModeID = pVGAModeInfo->pEnhTableEntry->ulModeID;

        SetIndexReg2(CRTC_PORT, 0x8C, (UCHAR) ((ulColorIndex & 0x0F) << 4));         	    
        SetIndexReg2(CRTC_PORT, 0x8D, (UCHAR) (ulRefreshRateIndex & 0xFF));         	    
        SetIndexReg2(CRTC_PORT, 0x8E, (UCHAR) (ulModeID & 0xFF));         	
    }    
       	
}

#define VGA_LOAD_PALETTE_INDEX(index, red, green, blue) \
{ \
   UCHAR __junk;				\
   IoWrite8(DAC_INDEX_WRITE,(UCHAR)(index));	\
   __junk = GetReg2(SEQ_PORT);			\
   IoWrite8(DAC_DATA,(UCHAR)(red));		\
   __junk = GetReg2(SEQ_PORT);			\
   IoWrite8(DAC_DATA,(UCHAR)(green));		\
   __junk = GetReg2(SEQ_PORT);			\
   IoWrite8(DAC_DATA,(UCHAR)(blue));		\
   __junk = GetReg2(SEQ_PORT);      		\
}

int bSetDACReg(_DEVInfo *DevInfo, PVBIOS_MODE_INFO pVGAModeInfo, VOID *Palette)
{
    PVBIOS_DAC_INFO pDACPtr;
    ULONG i, ulDACNumber;
    UCHAR DACR, DACG, DACB;

    switch (pVGAModeInfo->bpp)
    {
	case 1: /* text */
         ulDACNumber = DAC_NUM_TEXT;
         pDACPtr = (PVBIOS_DAC_INFO) &DAC_TEXT[0];
         break;	        
    case 8:
         ulDACNumber = DAC_NUM_VGA;
         pDACPtr = (PVBIOS_DAC_INFO)Palette ;
         break;
    default:
         return 1;
    }

    for (i=0; i<ulDACNumber; i++)
    {
    	DACR = pDACPtr->DACR;
    	DACG = pDACPtr->DACG;
    	DACB = pDACPtr->DACB;
    	
    	DACR >>= 2;
    	DACG >>= 2;
    	DACB >>= 2;

        VGA_LOAD_PALETTE_INDEX (i, DACR, DACG, DACB);
        
        pDACPtr++;
    }

    return 0;

}

int VgaStrcmp( char *string1, char *string2 ) {
	while(*string1 && *string1==*string2) {*string1++;*string2++;}
	return *string1 - *string2;
}
#if FV_GraphicMode_StringOutput_EN
static UCHAR *SetMem (UCHAR *Buffer, ULONG Length, UCHAR Value)
{
    UCHAR                    *Pointer;

    Pointer = (UCHAR*)Buffer;
    while (Length-- > 0) {
        *(Pointer++) = Value;
    }
    return Buffer;
}

void vOpenCharMap(_DEVInfo *DevInfo)
{
    SetIndexReg2(SEQ_PORT,0x02, 0x04);                                
    SetIndexReg2(SEQ_PORT,0x04, 0x07);                                
    SetIndexReg2(GR_PORT,0x04, 0x02);                                
    SetIndexReg2(GR_PORT,0x05, 0x00);                                
    SetIndexReg2(GR_PORT,0x06, 0x04);                                
}
	
void vCloseCharMap(_DEVInfo *DevInfo)
{
    SetIndexReg2(SEQ_PORT,0x02, 0x03);                                
    SetIndexReg2(SEQ_PORT,0x04, 0x03);                                
    SetIndexReg2(GR_PORT,0x04, 0x00);                                
    SetIndexReg2(GR_PORT,0x05, 0x10);                                
    SetIndexReg2(GR_PORT,0x06, 0x0E);                                		
}

void vLoadTextFont(_DEVInfo *DevInfo)
{
    UCHAR *WritePtr, *FontPtr;
    ULONG i, j;
    	
    vOpenCharMap(DevInfo);
    
    //
    //load font
    //
    WritePtr = DevInfo->pjA0000Linear;
    for (i=0; i<0x100; i++)
    {
	    FontPtr = &FONT8X16[i][0];
	    for (j=0; j<0x10; j++)
	        *(UCHAR *)(WritePtr + j) = *(UCHAR *)(FontPtr++);
	    WritePtr += 0x20;    
    }      
    
    vCloseCharMap(DevInfo);
}
#endif	
BOOL SearchOPTModeInfo(EFI_PEI_SERVICES **PeiServices, char *mode, VBIOS_MODE_INFO *pVGAModeInfo)
{
    OPT_MODE_STRUCT *OptMode;
  
    OptMode = OptModeTable;
    while (VgaStrcmp(OptMode->ModeString, "end"))
    {  	
        if (!VgaStrcmp(OptMode->ModeString, mode))
        {
            pVGAModeInfo->xres = OptMode->xres;
            pVGAModeInfo->yres = OptMode->yres;
            pVGAModeInfo->bpp  = OptMode->bits_per_pixel; 
            
            /* Get std. mode entry */     
            switch (pVGAModeInfo->bpp)
            {
	        case 1: /* text mode */
                pVGAModeInfo->pStdTableEntry = (PVBIOS_STDTABLE_STRUCT) &StdTable[TextModeIndex]; 
                break;   	            
            case 8:
                pVGAModeInfo->pStdTableEntry = (PVBIOS_STDTABLE_STRUCT) &StdTable[VGAModeIndex]; 
                break;   
            case 16:
                pVGAModeInfo->pStdTableEntry = (PVBIOS_STDTABLE_STRUCT) &StdTable[HiCModeIndex];
                break;
            case 24:         
            case 32:
                pVGAModeInfo->pStdTableEntry = (PVBIOS_STDTABLE_STRUCT) &StdTable[TrueCModeIndex]; 
	        break;    
            default:
                return FALSE;
            }

            /* get ext. mode entry */
            switch (pVGAModeInfo->xres)
            {
            case 640:
            case 720:
                pVGAModeInfo->pEnhTableEntry = (PVBIOS_ENHTABLE_STRUCT) Res640x480Table;
                break;
            case 800:
                pVGAModeInfo->pEnhTableEntry = (PVBIOS_ENHTABLE_STRUCT) Res800x600Table;
                break;         
            case 1024:
                pVGAModeInfo->pEnhTableEntry = (PVBIOS_ENHTABLE_STRUCT) Res1024x768Table;
                break;
            case 1280:
                pVGAModeInfo->pEnhTableEntry = (PVBIOS_ENHTABLE_STRUCT) Res1280x1024Table;
                break;
            case 1600:
                pVGAModeInfo->pEnhTableEntry = (PVBIOS_ENHTABLE_STRUCT) Res1600x1200Table;
                break;                  
            default:
                return FALSE;     
            }
                      	
            return TRUE;	
        } //	
        OptMode++;
    } //while	
	
    return FALSE;
}	

void ClearBuffer(EFI_PEI_SERVICES **PeiServices, _DEVInfo *DevInfo, PVBIOS_MODE_INFO pVGAModeInfo)
{
    ULONG size;
    	
    size = pVGAModeInfo->xres * ((pVGAModeInfo->bpp+7)/8) * pVGAModeInfo->yres;
    SetMem(DevInfo->pjFBLinear,size ,0x00);
	DEBUG ((0xffffffff,	"size = %x \n bpp = 0x%x", size, pVGAModeInfo->bpp));
}	
void ClearTextBuffer(_DEVInfo *DevInfo, PVBIOS_MODE_INFO pVGAModeInfo)
{
    UCHAR *WritePtr = DevInfo->pjB0000Linear + 0x8000; 	
    ULONG size;
    	
    size = 4000;
    SetMem(WritePtr, size, 0);
}	
#if FV_GraphicMode_StringOutput_EN

void ShowChar(_DEVInfo *DevInfo, PVBIOS_MODE_INFO pVGAModeInfo, _FontInfo *FontInfo, ULONG *StartAddr, CHAR16 ch)
{
	UCHAR *FontEntry = FontInfo->FontTable + ((FontInfo->FontWidth+7)/8)*FontInfo->FontHeight*ch;
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
			        if      (PixelPerByte == 1) *(UCHAR *)(DevInfo->pjFBLinear+DestAddr)  = (UCHAR) WriteData /*& 0x0F*/;
			        else if (PixelPerByte == 2) *(USHORT *)(DevInfo->pjFBLinear+DestAddr) = (USHORT) WriteData;
			        else                        *(ULONG *)(DevInfo->pjFBLinear+DestAddr)  = WriteData;
			        DestAddr += PixelPerByte;   
		        }        
	        }
        }    
    }
} //ShowChar

#if defined (DEBUG_MODE) && DEBUG_MODE == 1
ULONG strlen (UCHAR *String)
{
  ULONG                             Length;

  ASSERT (String != NULL);

  for (Length = 0; *String != '\0'; String++, Length++);
  
  return Length;
}
#endif
	
void ShowString(_DEVInfo *DevInfo, PVBIOS_MODE_INFO pVGAModeInfo, CHAR16 *String)
{
    _FontInfo FontInfo;	
    ULONG StrLength = (UINT32)StrLen(String);
    ULONG i, StartAddr, DestAddr;
    CHAR16 ch;
    
    //Init variables
    StartAddr = 0;
    
    //Init Font Table
    FontInfo.FontTable  = (UINT8 *)FONT8X16;
    FontInfo.FontWidth  = 8;
    FontInfo.FontHeight = 16;
    FontInfo.XScale     = 1;   
    FontInfo.YScale     = 1;   
    FontInfo.FG         = 0xFFFFFFFF;
    FontInfo.BG         = 0x00000000;
    StartAddr           = 0;
    //DEBUG ((0xffffffff, "pjFBLinear = %x bpp = 0x%x xres:0x%x \n", DevInfo->pjFBLinear, pVGAModeInfo->bpp, pVGAModeInfo->xres));    
    for (i=0; i<StrLength; i++)
    {
	    DestAddr = StartAddr + FontInfo.FontWidth*((pVGAModeInfo->bpp+1)/8)*FontInfo.XScale*i;
	    ch = String[i];
	    ShowChar(DevInfo, pVGAModeInfo, &FontInfo, &DestAddr, ch);    
    }    
} //ShowString

void WriteText(_DEVInfo *DevInfo, UCHAR *str)
{
    int i, len = (ULONG)strlen(str);
    UCHAR *WritePtr = DevInfo->pjB0000Linear + 0x8000; 
    
    //hide cursor
    SetIndexReg2(CRTC_PORT, 0x0A, 0x20);
    
    //write text
    for (i=0; i<len; i++)
    {
        *(UCHAR *)(WritePtr++) = *(UCHAR *)(str + i);
	    *(UCHAR *)(WritePtr++) = 0x07;    
    }            	
}	            
#endif  
/*
 * export functions
 */
BOOL SetMode(EFI_PEI_SERVICES **PeiServices, _DEVInfo *DevInfo, UCHAR *ModeName, ULONG OutputOption, VOID *Palette)
{
    VBIOS_MODE_INFO VGAModeInfo; 
	
    if (SearchOPTModeInfo(PeiServices , ModeName, &VGAModeInfo))
    {
	    DisplayOFF(DevInfo);
	    
        switch (VGAModeInfo.bpp)
        {
	    case 1:
            vResetReg(DevInfo);
            vSetStdReg(DevInfo,  &VGAModeInfo);
            vSetExtReg(DevInfo, &VGAModeInfo, OutputOption);            
            bSetDACReg(DevInfo, &VGAModeInfo, Palette);
#if FV_TextMode_StringOutput_EN 
            vLoadTextFont(DevInfo);
            ClearTextBuffer(DevInfo, &VGAModeInfo);
            
            //output sample
            //WriteText(DevInfo, "ASPEED Text Mode Sample!!");            
#endif
	        break;
	        
	    default:
            vResetReg(DevInfo);	    
            vSetStdReg(DevInfo,  &VGAModeInfo);	    
            vSetCRTCReg(PeiServices, DevInfo,  &VGAModeInfo);
            vSetOffsetReg(DevInfo,  &VGAModeInfo);    
            vSetDCLKReg(DevInfo,  &VGAModeInfo);
            vSetExtReg(DevInfo, &VGAModeInfo, OutputOption);
            bSetDACReg(DevInfo, &VGAModeInfo, Palette);      	        
            ClearBuffer(PeiServices, DevInfo, &VGAModeInfo);
            
            //output sample
#if FV_GraphicMode_StringOutput_EN        
            //ShowString(DevInfo, &VGAModeInfo, L"!!!!!!Fast Video Graphic Mode STRING test!!!!!!");
#endif
            
        }
        
	    DisplayON(DevInfo);
                   	    
        return TRUE;
    }	    
        
    return FALSE;
} /* SetMode */
	
