//----------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------
#include <Token.h>
#include <AmiPeiLib.h>
#include <Library/AmiSioPeiLib.h>

#define	IO_BANDA	0x00
#define	IO_BANDB	0x01
#define	IO_BANDC	0x02
#define	IO_BANDD	0x03
#define	IO_BANDE	0x04
#define	IO_BANDF	0x05
#define	IO_BANDG	0x06
#define	IO_BANDH	0x07
#define	IO_BANDI	0x08
#define	IO_BANDJ	0x09
#define	IO_BANDK	0x0A
#define	IO_BANDL	0x0B
#define	IO_BANDM	0x0C
#define	IO_BANDN	0x0D
#define	IO_BANDO	0x0E
#define	IO_BANDP	0x0F
#define	IO_BANDQ	0x10
#define	IO_BANDR	0x11
#define	IO_BANDS	0x12
#define	IO_BANDT	0x13
#define	IO_BANDU	0x14
#define	IO_BANDV	0x15
#define	IO_BANDW	0x16
#define	IO_BANDX	0x17
#define	IO_BANDY	0x18
#define	IO_BANDZ	0x19
#define	IO_BANDAA	0x1A
#define	IO_BANDAB	0x1B

#define IO_BAND_x(x) (x)

#if IO_BAND_x(OEM_AST2500_PORT80_GPIO_BAND) > 0x1B
#error "IO Band for AST2500 Port80 GPIO limit to 0x00(GPIOA)-0x1B(GPIOAB), Please check token OEM_AST2500_PORT80_GPIO_BAND"
#endif

#pragma pack(push, 1)
typedef struct 
{
    UINT16  Direction;
    UINT16  CmdSrc0;
}GPIO_REG_INFO;
#pragma pack(pop)

GPIO_REG_INFO GpioReg[7] = 
{
    {0x04,   0x60},     //A/B/C/D
    {0x24,   0x68},     //E/F/G/H
    {0x74,   0x90},     //I/J/K/L
    {0x7C,   0xE0},     //M/N/O/P
    {0x84,   0x110},    //Q/R/S/T
    {0x8C,   0x140},    //U/V/W/X
    {0x1E4,  0x170},    //Y/Z/AA/AB
};

SIO_DEVICE_INIT_DATA  AST2500Port80SioInitTable[]= {
    // -----------------------------
    //|  Addr | DataMask  | DataValue |
    // -----------------------------

    //------------------------------------------------------------------
    // Enter Configuration Mode.
    //------------------------------------------------------------------
    {AST2500_CONFIG_INDEX, 0x00, AST2500_CONFIG_MODE_ENTER_VALUE},
    {AST2500_CONFIG_INDEX, 0x00, AST2500_CONFIG_MODE_ENTER_VALUE},

    {AST2500_CONFIG_INDEX, 0x00, AST2500_LDN_SEL_REGISTER},
    {AST2500_CONFIG_DATA,  0x00, AST2500_LDN_GPIO},
    
    {AST2500_CONFIG_INDEX, 0x00, AST2500_ACTIVATE_REGISTER},
    {AST2500_CONFIG_DATA,  0x7F, BIT7}, //BIT7 - Enable port 80h GPIO
    
    {AST2500_CONFIG_INDEX, 0x00, 0x38},//bit7:enable inverse polarity of port 80h GPIO, BIt[0-4]: IO band selection
    {AST2500_CONFIG_DATA,  0x00, 0x00
#if defined(OEM_AST2500_PORT80_GPIO_INVERSE) && (OEM_AST2500_PORT80_GPIO_INVERSE)
       | BIT7
#endif
#if defined(OEM_AST2500_PORT80_GPIO_BAND)
       | ((OEM_AST2500_PORT80_GPIO_BAND) & 0x1F)  //BIT[4:0], IO band selection, 0x0 - GPIOA, 0x1 - GPIOB, ...., 0x1B - GPIOAB
#endif
    }, //GPIOM 

    {AST2500_CONFIG_INDEX, 0x00, AST2500_CONFIG_MODE_EXIT_VALUE},
};
UINT8 AST2500Port80SioInitTableCount = sizeof(AST2500Port80SioInitTable)/sizeof(SIO_DEVICE_INIT_DATA);

//-------------------------------------------------------------------------
// global funtion declare ; these funtions are used for other files.
//-------------------------------------------------------------------------
//*************************************************************************
// belows are functions defined
VOID lpc_readwrite (
        IN      UINT32 ulAddress,
        IN      UINT32 AndData,
        IN      UINT32 OrData
);

//----------------------------------------------------------------------
// global function declare ; these functions are used for other files.
//----------------------------------------------------------------------


EFI_STATUS AST2500Port80InitConstructor(
    IN       EFI_PEI_FILE_HANDLE  FileHandle,
    IN CONST EFI_PEI_SERVICES     **PeiServices
)
{
//<Wangbn002-20190524 Remove temporary checkpoint of AST2500Port80>    IoWrite8(0x80, 0x90);
    //1. Decode
    AmiSioLibSetLpcDeviceDecoding(NULL, AST2500_CONFIG_INDEX, 2, 0xFF);
//<Wangbn002-20190524 Remove temporary checkpoint of AST2500Port80>    IoWrite8(0x80, 0x91);
    //2. Configure LPC snoop function
//<Kangmm-20211207 Modify AST2500 LPC setting for 16bits check point+>
#if defined(VGA_UART_DIS16BIT_PORT80_SUPPORT) && (VGA_UART_DIS16BIT_PORT80_SUPPORT == 1)
    lpc_readwrite(0x1E789090, 0xFFFF0000 , 0x00810080); //SNPWADR[15:0]="0x0080": Set snooping address #0 to 80h
    lpc_readwrite(0x1E789080, 0xFFFFFFFE , 0x00000005); //HICR5[0]="1": Enable snooping address #0.
#else
    lpc_readwrite(0x1E789090, 0xFFFF0000 , 0x00000080); //SNPWADR[15:0]="0x0080": Set snooping address #0 to 80h
    lpc_readwrite(0x1E789080, 0xFFFFFFFE , 0x00000001); //HICR5[0]="1": Enable snooping address #0.
#endif
//<Kangmm-20211207 Modify AST2500 LPC setting for 16bits check point->

//<Wangbn003-Fix Port80 debug LED can't work at the beganing of the SEC startup phase+>
//must set command source to LPC then the GPIO direction can changed by BIOS.
/*
    //3 Configure GPIOx[7:0] as output pin
    lpc_readwrite(0x1E780000 + GpioReg[OEM_AST2500_PORT80_GPIO_BAND >> 2].Direction,
        0xFFFFFFFF,
        ((UINT32)0xFF << ((OEM_AST2500_PORT80_GPIO_BAND & 0x03) << 3))
    );
*/
//<Wangbn003-Fix Port80 debug LED can't work at the beganing of the SEC startup phase->
    //4. Configure GPIO x command sourc to LPC.
    lpc_readwrite(0x1E780000 + GpioReg[OEM_AST2500_PORT80_GPIO_BAND >> 2].CmdSrc0,
        0xFFFFFFFF,
        ((UINT32)0x01 << ((OEM_AST2500_PORT80_GPIO_BAND & 0x03) << 3))
    );  //set GPIOx command source #0

    lpc_readwrite(0x1E780000 + GpioReg[OEM_AST2500_PORT80_GPIO_BAND >> 2].CmdSrc0 + 4,
        ~((UINT32)0x01 << ((OEM_AST2500_PORT80_GPIO_BAND & 0x03) << 3)),
        0x00000000
    );  //set GPIOx command source #1
//<Wangbn003-Fix Port80 debug LED can't work at the beganing of the SEC startup phase+>
    //3 Configure GPIOx[7:0] as output pin
    lpc_readwrite(0x1E780000 + GpioReg[OEM_AST2500_PORT80_GPIO_BAND >> 2].Direction,
        0xFFFFFFFF,
        ((UINT32)0xFF << ((OEM_AST2500_PORT80_GPIO_BAND & 0x03) << 3))
    );
//<Wangbn003-Fix Port80 debug LED can't work at the beganing of the SEC startup phase->
    //5. Set Super IO
    ProgramRtRegisterTable(0x00, AST2500Port80SioInitTable, AST2500Port80SioInitTableCount);
//<Wangbn002-20190524 Remove temporary checkpoint of AST2500Port80>    IoWrite8(0x80, 0x92);
    return  RETURN_SUCCESS;
}

VOID lpc_readwrite (
        IN      UINT32 ulAddress,
        IN      UINT32 AndData,
        IN      UINT32 OrData
)
{
    UINT8     jtemp;

    IoWrite8 (AST2500_CONFIG_INDEX, AST2500_CONFIG_MODE_ENTER_VALUE);
    IoWrite8 (AST2500_CONFIG_INDEX, AST2500_CONFIG_MODE_ENTER_VALUE);

    IoWrite8 (AST2500_CONFIG_INDEX, AST2500_LDN_SEL_REGISTER);
    IoWrite8 (AST2500_CONFIG_DATA, AST2500_LDN_ILPC2AHB);

    IoWrite8 (AST2500_CONFIG_INDEX, AST2500_ACTIVATE_REGISTER);
    jtemp = IoRead8 (AST2500_CONFIG_DATA); //
    IoWrite8 (AST2500_CONFIG_DATA, (jtemp | 0x01));//Active
 
//Write Address
    IoWrite8 (AST2500_CONFIG_INDEX, 0xF0);
    IoWrite8 (AST2500_CONFIG_DATA, ((ulAddress & 0xFF000000) >> 24));
    IoWrite8 (AST2500_CONFIG_INDEX, 0xF1);
    IoWrite8 (AST2500_CONFIG_DATA, ((ulAddress & 0x00FF0000) >> 16));
    IoWrite8 (AST2500_CONFIG_INDEX, 0xF2);
    IoWrite8 (AST2500_CONFIG_DATA, ((ulAddress & 0x0000FF00) >> 8));
    IoWrite8 (AST2500_CONFIG_INDEX, 0xF3);
    IoWrite8 (AST2500_CONFIG_DATA, (ulAddress & 0x000000ff));
//Write Mode
    IoWrite8 (AST2500_CONFIG_INDEX, 0xF8);
    jtemp = IoRead8 (AST2500_CONFIG_DATA);
    IoWrite8 (AST2500_CONFIG_DATA, (jtemp & 0xFC) | 0x02);
//Fire
    IoWrite8 (AST2500_CONFIG_INDEX, 0xFE);
    jtemp = IoRead8 (AST2500_CONFIG_DATA);
//Get Data

    for(jtemp = 0xF7; jtemp >= 0xF4; jtemp--)
    {
        IoWrite8 (AST2500_CONFIG_INDEX, jtemp);
        IoWrite8 (AST2500_CONFIG_DATA, (IoRead8 (AST2500_CONFIG_DATA) & (UINT8)AndData) | (UINT8)OrData );
        AndData >>=8;
        OrData >>= 8;
    }

//Write Mode
    IoWrite8 (AST2500_CONFIG_INDEX, 0xF8);
    jtemp = IoRead8 (AST2500_CONFIG_DATA);
    IoWrite8 (AST2500_CONFIG_DATA, (jtemp & 0xFC) | 0x02);
//Fire
    IoWrite8 (AST2500_CONFIG_INDEX, 0xFE);
    IoWrite8 (AST2500_CONFIG_DATA, 0xCF);

    IoWrite8 (AST2500_CONFIG_INDEX, AST2500_CONFIG_MODE_EXIT_VALUE);
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

