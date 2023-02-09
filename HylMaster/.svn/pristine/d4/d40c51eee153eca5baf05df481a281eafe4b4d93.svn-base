//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
/** @file SbGeneric.c
    This file contains south bridge related code that is needed
    for both PEI & DXE stage.  To avoid code duplication this
    file is made as a library and linked both in PEI & DXE
    south bridge FFS.

**/
//**********************************************************************

//----------------------------------------------------------------------------
// Module specific Includes
//----------------------------------------------------------------------------
#include "Efi.h"
#include "token.h"
#include <AmiPeiLib.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <Library/PcdLib.h>
#include <Guid/Rtc.h>
#include <Ppi/ReadOnlyVariable2.h>
extern VOID CPULib_DisableInterrupt();
extern VOID CPULib_EnableInterrupt();
extern BOOLEAN CPULib_GetInterruptState();
//----------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//----------------------------------------------------------------------------
#define GPIO_BANK_BASE 0x1500

typedef struct{
	UINT8 IrqIndex;
	UINT8 IrqDeviceIndex;
} FCH_IRQ_STRUCT;
//-------------------------------------------------------------------
// Variable Declaration
//-------------------------------------------------------------------
GLOBAL_REMOVE_IF_UNREFERENCED UINT64  gSbMMIOBASE = SB_MMIO_BASE;

GLOBAL_REMOVE_IF_UNREFERENCED EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL     *gPciRootBridgeIo = NULL;

FCH_IRQ_STRUCT gSbIrqStruct[] = {
    {0x02,0x30}, // 30h: Dev18 (USB) IntA#-> INTC#
    {0x02,0x34}, // 34h: Dev22 (USB) IntA#-> INTC#
    {0x03,0x41}, // 41h: SATA pciPCI interrupt-> INTD#
};

UINT8 IoMuxSelect[] = {
	// According to Mullins BKDG Rev 0.96
    // GPIO 0: PWR_BTN_L can't select as GPIO
    // Reserved: 39 ~ 63, 74, 75, 78 ~ 83, 86 ,102 ~ 112,
    //           117 ~ 125, 127, 128, 133 ~ 183

    // GPIO Bank 0
    0, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 0, 1, 2, 1, 2,  //  0 ~ 15
    1, 1, 1, 1, 1, 1, 1, 0, 0, 2, 0, 0, 1, 0, 0, 0,  // 16 ~ 31
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 32 ~ 47
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 48 ~ 63

    // GPIO Bank 1
    2, 2, 2, 3, 2, 1, 3, 2, 0, 0, 0, 0, 0, 0, 0, 0,  //  64 ~ 79
    0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 3, 2, 0, 0,  //  80 ~ 95
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //  96 ~ 111
    0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 112 ~ 127

    // GPIO Bank 2
    0, 1, 1, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 128 ~ 143
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 144 ~ 159
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 160 ~ 175
    0, 0, 0, 0, 0, 0, 0, 0,                          // 176 ~ 183
};

struct {
	#define FCH_N_OF_SMI_CTRL_REG 10
	UINT32 Reg;
	UINT32 Mask;
} const SmiControl[FCH_N_OF_SMI_CTRL_REG] = {
	 // BKDG section 3.30.17 SMI Registers
	 // Reg		mask
	 {0xA0,		0},
	 {0xA4,		0},
	 {0xA8,		0xC0F0},
	 {0xAC,		0x3FCC0000},
	 {0xB0,		0xC0FF03},
	 {0xB4,		0xFCC3F000},
	 {0xB8,		0xFFFFFFFF},
	 {0xBC,		0xFFFFFFFF},
	 {0xC0,		0x0FFFF3FF},
	 {0xC4,		0xFCFCFCF0},
};

struct {
	SB_GPE0_TYPE Gpe0Type;
	UINT32 PinNum;
	struct {
		UINT32 Reg;
		UINT32 BitOffset;
	} Sci, Smi;
} const HygonSciMapTbl[] = {
	{TYPE_GPE0_GEVENT0,					126,	{0x40, 0}, 	{0xA0, 0}},
	{TYPE_GPE0_GEVENT1,     			129,	{0x40, 8}, 	{0xA0, 2}},
	{TYPE_GPE0_GEVENT2,     			3,		{0x40, 16},	{0xA0, 4}},
	{TYPE_GPE0_GEVENT3,     			22,		{0x40, 24},	{0xA0, 6}},
	{TYPE_GPE0_GEVENT4,     			4,		{0x44, 0},	{0xA0, 8}},
	{TYPE_GPE0_GEVENT5,     			21,		{0x44, 8},	{0xA0, 10}},
	{TYPE_GPE0_GEVENT6,     			14,		{0x44, 16},	{0xA0, 12}},
	{TYPE_GPE0_GEVENT7,     			5,		{0x44, 24},	{0xA0, 14}},
	{TYPE_GPE0_GEVENT8,     			2,		{0x48, 0},	{0xA0, 16}},
	{TYPE_GPE0_GEVENT9,     			32,		{0x48, 8},	{0xA0, 18}},
	{TYPE_GPE0_GEVENT10,    			6,		{0x48, 16},	{0xA0, 20}},
	{TYPE_GPE0_GEVENT11,    	        7,		{0x48, 24},	{0xA0, 22}},
	{TYPE_GPE0_GEVENT12,    	        16,     {0x4C, 0},	{0xA0, 24}},
	{TYPE_GPE0_GEVENT13,    	        17,     {0x4C, 8},	{0xA0, 26}},
	{TYPE_GPE0_GEVENT14,    	        18,     {0x4C, 16},	{0xA0, 28}},
	{TYPE_GPE0_GEVENT15,    	        24,     {0x4C, 24},	{0xA0, 30}},
	{TYPE_GPE0_GEVENT16,     	        23,     {0x50, 0},	{0xA4, 0}},
	{TYPE_GPE0_GEVENT17,     	        25,     {0x50, 8},	{0xA4, 2}},
	{TYPE_GPE0_GEVENT18,     	        11,     {0x50, 16},	{0xA4, 4}},
	{TYPE_GPE0_GEVENT19,     	        1,      {0x50, 24},	{0xA4, 6}},
	{TYPE_GPE0_GEVENT20,     	        15,     {0x54, 0},	{0xA4, 8}},
	{TYPE_GPE0_GEVENT21,     	        13,     {0x54, 8},	{0xA4, 10}},
	{TYPE_GPE0_GEVENT22,     	        9,      {0x54, 16},	{0xA4, 12}},
	{TYPE_GPE0_GEVENT23,     	        8,      {0x54, 24},	{0xA4, 14}},
	{TYPE_GPE0_USB0_PME,     	        -1,     {0x58, 0}, 	{0xA4, 16}},
	{TYPE_GPE0_USB1_PME,     	        -1,     {0x58, 8}, 	{0xA4, 18}},
	{TYPE_GPE0_USB2_PME,     	        -1,     {0x58, 16},	{0xA4, 22}},
	{TYPE_GPE0_HD_AUDIO_PME, 	        -1,     {0x64, 0},	{0xA8, 8}},
	{TYPE_GPE0_SATA0_PME,    	        -1,     {0x64, 8},	{0xA8, 10}},
	{TYPE_GPE0_SATA1_PME,    	        -1,     {0x64, 16},	{0xA8, 12}},
	{TYPE_GPE0_EC0_PME,			        -1,		{0x68, 0},	{-1, -1}},
	{TYPE_GPE0_EC1_PME,			        -1,		{0x68, 8},	{-1, -1}},
	{TYPE_GPE0_CIR_PME,			        -1,		{0x68, 16}, {0xA8, 20}},
	{TYPE_GPE0_WAKE_PIN,		        -1,		{0x68, 24}, {-1, -1}},
	{TYPE_GPE0_FAN_THERMAL,		        -1,		{0x6C, 0},	{0xA8, 24}},
	{TYPE_GPE0_ASF_MASTER,		        -1,		{0x6C, 8},	{0xA8, 26}},
	{TYPE_GPE0_ASF_SLAVE,		        -1,		{0x6C, 16},	{0xA8, 28}},
	{TYPE_GPE0_SMBUS0,			        -1,		{0x6C, 24},	{0xA8, 30}},
	{TYPE_GPE0_TWARN_PIN,		        -1,		{0x70, 0},  {0xAC, 0}},
	{TYPE_GPE0_INTERNAL_TRAFFIC_MONITOR,-1,		{0x70, 8},  {0xAC, 2}},
	{TYPE_GPE0_LLB_PIN,			        -1, 	{0x70, 16},	{0xAC, 4}},
	{TYPE_GPE0_PWRBTN,			         0,		{0x70, 24},	{0xB0, 20}},
	{TYPE_GPE0_PROHOT_PIN,		        -1, 	{0x74, 0},	{0xB4, 6}},
	{TYPE_GPE0_HW_ASSERTION_MESSAGE, 	-1,		{0x74, 8},	{0xAC, 10}},
	{TYPE_GPE0_SCI_ASSERTION_MESSAGE, 	-1, 	{0x74, 16}, {0xAC, 12}},
	{TYPE_GPE0_RAS_EVENT,		        -1,		{0x74, 24}, {-1, -1}},
	{TYPE_GPE0_XHC0,			        -1,		{0x78, 0},	{0xAC, 16}},
	{TYPE_GPE0_XHC1,			        -1,  	{0x78, 8},	{-1, -1}},
	{TYPE_GPE0_AC_DC_TIMER_EVENT,       -1,		{0x78, 16},	{0xAC, 20}},
	{TYPE_GPE0_TEMP_STATUS5,	  		-1,		{0x7C, 0},	{0xAC, 30}},
	{TYPE_GPE0_HYGON_END, -1, {-1, -1}, {-1, -1}}
};
#if     CSM_SUPPORT
//----------------------------------------------------------------------------
#include <Protocol\LegacyInterrupt.h>

typedef struct {
    UINT64                  Address;
    EFI_BOOT_SCRIPT_WIDTH   Width;
    UINT32                  Mask;
} BOOT_SCRIPT_PCI_REGISTER_SAVE;

GLOBAL_REMOVE_IF_UNREFERENCED BOOT_SCRIPT_PCI_REGISTER_SAVE gSBRegsSaveAfterCSM[] = {
    {LPC_BUS_DEV_FUN,  EfiBootScriptWidthUint16, 0xffff},
};

//----------------------------------------------------------------------------
// Start OF CSM Related Porting Hooks
//----------------------------------------------------------------------------
#define MAX_PIRQS        8

GLOBAL_REMOVE_IF_UNREFERENCED UINT8   bMaxPIRQ        = MAX_PIRQS;     // For CSM
GLOBAL_REMOVE_IF_UNREFERENCED UINT8   bRouterBus      = FCH_LPC_BUS    ;// PORTING REQUIRED (Use appropriate Equate)
GLOBAL_REMOVE_IF_UNREFERENCED UINT8   bRouterDevice   = FCH_LPC_DEV    ;// PORTING REQUIRED (Use appropriate Equate)
GLOBAL_REMOVE_IF_UNREFERENCED UINT8   bRouterFunction = FCH_LPC_FUNC    ;// PORTING REQUIRED (Use appropriate Equate)

GLOBAL_REMOVE_IF_UNREFERENCED UINT8   RRegs[MAX_PIRQS]= {0, 1, 2, 3, 4, 5, 6, 7};   // Porting required

/**
    This function is clears the routing registers to default values

    @param PciRBIo Root bridge IO protocol pointer

    @retval EFI_STATUS Returned from PCI read call

**/
EFI_STATUS SBGen_InitializeRouterRegisters (
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRBIo
)
{
    UINT8       Idx;

    for (Idx = 0; Idx < MAX_PIRQS; Idx++)
        WRITE_IO8_PIRQ((RRegs[Idx] | 0x80), Idx+0x10); //

    gPciRootBridgeIo = PciRBIo;         // Save RB IO value for later use
    return EFI_SUCCESS;
}

/**
    This function returns the 0 based PIRQ index (PIRQ0, 1 etc)
    based on the PIRQ register number specified in the routing table

    @param PIRQRegister Register number of the PIR

        UINT8 
    @retval rrIndx MAX_PIRQ -1)
        0xFF if the register is invalid

    @note  Here is the control flow of this function:
              1. If Invalid PirqNumber, return EFI_INVALID_PARAMETER.
              2. Read into *PriqData from PIRQ register for Pirq requested.
              3. Return EFI_SUCCESS.
**/
UINT8
SBGen_GetPIRQIndex (
        IN UINT8       PIRQRegister)
{
    UINT8       rrIndx = 0;

    //--- while ((rrIndx < MAX_PIRQS) && (RRegs[rrIndx] != PIRQRegister)) rrIndx++;
    // The router is Index/Data register.(0~3 9~C)
    // Because Microsoft IRQ Routing Table test.
    // We can't let the field "Chipset Reg" == 0.(..\Build\oempir.inc)
    // So I set the field == (1~4 A~D). Actually it is (0~3 9~C).
    while ((rrIndx < MAX_PIRQS) && (RRegs[rrIndx] != (PIRQRegister-1))) rrIndx++;

    if (rrIndx == MAX_PIRQS) return 0xFF;

    return rrIndx;
}

/**
    This function reads the IRQ associated with the PIRQ from
    the chipset register

    @param This Pointer to Legacy interrupt protocol
    @param PirqNumber PIRQ number to read
    @param PirqData IRQ programmed for this PIRQ (BIT7 will be
        set if the PIRQ is not programmed)

               
    @retval EFI_SUCCESS On successfull IRQ value return
    @retval EFI_INVALID_PARAMETER If PirqNumber is greater than max PIRQs

    @note  Here is the control flow of this function:
              1. If Invalid PirqNumber, return EFI_INVALID_PARAMETER.
              2. Read into *PriqData from PIRQ register for Pirq requested.
              3. Return EFI_SUCCESS.
**/

EFI_STATUS
SBGen_ReadPirq (
    IN EFI_LEGACY_INTERRUPT_PROTOCOL    *This,
    IN UINT8                            PirqNumber,
    OUT UINT8                           *PirqData
    )
{
    if (PirqNumber > MAX_PIRQS) return EFI_INVALID_PARAMETER;

    *PirqData = READ_IO8_PIRQ(RRegs[PirqNumber]);
    if (*PirqData == 0x1F) *PirqData = 0x00;

    return EFI_SUCCESS;
}

/**
    Program the Chipset the IRQ

    @param IrqIndex : IRQ Index
    @param PirqData : Pirq Data

    @retval VOID

**/
VOID
RthChipsetRoutePciIrqSB800Hook(
    IN UINT8                            IrqIndex,
    IN UINT8                            PirqData
    )
{
    UINT8       PirqValue = PirqData & 0x0F;
    UINT32      StartIdx;

    if (IrqIndex < 4) {
        for (StartIdx = 0; StartIdx < \
            (sizeof(gSbIrqStruct)/sizeof(FCH_IRQ_STRUCT)); StartIdx++ )
        {
            if (IrqIndex == gSbIrqStruct[StartIdx].IrqIndex) {
                // For PCI
                WRITE_IO8_PIRQ(gSbIrqStruct[StartIdx].\
                                IrqDeviceIndex, PirqValue);
                // For APIC
                WRITE_IO8_PIRQ((gSbIrqStruct[StartIdx].\
                                IrqDeviceIndex | 0x80), (IrqIndex+16));
            }
        }
    }
}

/**
    This function writes an IRQ value allocated for the PIRQ by
    programming the chipset register

    @param This Pointer to Legacy interrupt protocol
    @param PirqNumber PIRQ number to read
    @param PirqData IRQ to be programmed

               
    @retval EFI_SUCCESS On successfull IRQ value return
    @retval EFI_INVALID_PARAMETER If PirqNumber is greater than
        max PIRQs or PirqData is greater
        than 15 (MAX IRQ)

    @note  Here is the control flow of this function:
              1. If Invalid PirqNumber, return EFI_INVALID_PARAMETER.
              2. Write PirqData to PIRQ register for Pirq requested.
              3. Return EFI_SUCCESS.
**/

EFI_STATUS
SBGen_WritePirq(
    IN EFI_LEGACY_INTERRUPT_PROTOCOL    *This,
    IN UINT8                            PirqNumber,
    IN UINT8                            PirqData
    )
{
    UINT8       PirqValue = PirqData & 0x0F;

    if (PirqNumber > MAX_PIRQS) return EFI_INVALID_PARAMETER;
    if (PirqData > 15) return EFI_INVALID_PARAMETER;

    RthChipsetRoutePciIrqSB800Hook(RRegs[PirqNumber], PirqValue);

    WRITE_IO8_PIRQ(RRegs[PirqNumber], PirqValue);

    WRITE_IO8_PIRQ((RRegs[PirqNumber] | 0x80), (RRegs[PirqNumber] + 0x10));

    return EFI_SUCCESS;
}

/**
    This writes the PIRQ to boot script before booting.

    @param BootScriptSave pointer to boot script
        protocol

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
EFI_STATUS SBGen_WriteBootScript(
    IN EFI_BOOT_SCRIPT_SAVE_PROTOCOL *BootScriptSave
)
{
    UINTN       i;
    UINT32      Value32;

    for (i = 0; i < sizeof(gSBRegsSaveAfterCSM) /  \
                sizeof(BOOT_SCRIPT_PCI_REGISTER_SAVE); ++i) {
        gPciRootBridgeIo->Pci.Read(
                    gPciRootBridgeIo,
                    gSBRegsSaveAfterCSM[i].Width,
                    gSBRegsSaveAfterCSM[i].Address,
                    1,
                    &Value32);
        Value32 &= gSBRegsSaveAfterCSM[i].Mask;
        BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(
            BootScriptSave,
            gSBRegsSaveAfterCSM[i].Width,
            gSBRegsSaveAfterCSM[i].Address,
            1,
            &Value32);
    }

    return EFI_SUCCESS;
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#endif          // END OF CSM Related Porting Hooks
//----------------------------------------------------------------------------

/**

    @param VOID

    @retval VOID

**/
VOID SBLib_Shutdown(VOID)
{
    SB_Shutdown();
}
/**

    @param VOID

    @retval VOID

**/
VOID SB_Shutdown(VOID)
{
    UINT16          Value16;

    // Set SLP_TYPEn to enable SLP_TYP to do S1/S3/S5
    RW_MEM8 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGBE, BIT5, 0);

    // Enable S state transition
    RW_MEM8 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG08 + 3,BIT0,BIT0+BIT1);

    // Turn off SLP_TYP SMI
    RW_MEM32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB0,0,BIT2 + BIT3);

    // power the system off now
    Value16 = IoRead16(PM_BASE_ADDRESS + 4);
    Value16 &= ~(BIT12 + BIT11 + BIT10);
    Value16 |= 5 << 10;
    IoWrite16(PM_BASE_ADDRESS + 4, (UINT16)Value16);

    Value16 |= BIT13;
    IoWrite16(PM_BASE_ADDRESS + 4, (UINT16)Value16);

    IoWrite8(0x80,0x05);
    EFI_DEADLOOP(); //halt at this,never come here!
}

/**
    This function determines if the system should boot with the
    default configuration. 

    @param PeiServices Pointer to the PEI services table
    @param ReadVariablePpi Pointer to the Read 
        Variable#2 PPI
    @param (The pointer can be used to read and enumerate existing NVRAM
        variables)

    @retval TRUE Firmware will boot with default configuration.

    @note  1. If boot with default configuration is detected, default
                 values for NVRAM variables are used.
    @note  2. Normally we have to check RTC power status or CMOS clear
                 jumper status to determine whether the system should boot
                 with the default configuration.
**/

#if CMOS_MANAGER_SUPPORT
/**
    This function determines if the system should boot with the default configuration.
    This function is called by NVRAM code. 
    If boot with default configuration is detected, default values for NVRAM variables are used.

    @param PeiServices pointer to a pointer to the PEI Services Table.
    @param ReadVariablePpi pointer to EFI_PEI_READ_ONLY_VARIABLE2_PPI PPI. The pointer can be used to
        read and enumerate existing NVRAM variables.

               
    @retval TRUE Boot with default configuration is detected
    @retval FALSE Boot with default configuration is not detected

    @note  This routine is called very early, prior to SBPEI and NBPEI.
**/

BOOLEAN SbIsDefaultConfigMode(
    IN EFI_PEI_SERVICES **PeiServices, 
    IN CONST EFI_PEI_READ_ONLY_VARIABLE2_PPI *ReadVariablePpi
){
#ifdef LOAD_DEFAULTS_IF_CMOS_BAD
  #if LOAD_DEFAULTS_IF_CMOS_BAD
	  //  IoWrite8(CMOS_ADDR_PORT, CMOS_DIAGNOSTIC_STATUS_REGISTER);
	   // if (IoRead8(CMOS_DATA_PORT) & (BIT06 + BIT07)) return TRUE; 
	       IoWrite8(CMOS_BANK1_INDEX, CMOS_BAD_CHECK_ADDRESS);
	    if (IoRead8(CMOS_BANK1_DATA) ==0x55)  return TRUE;
  #endif
#endif 
    return FALSE;
}
#endif // CMOS_MANAGER_SUPPORT

/**
    This function delays for the number of micro seconds passed in

    @param DelayTime Number of microseconds(us) to delay
    @param BaseAddr The I/O base address of the ACPI registers

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/

EFI_STATUS
CountTime (
    IN  UINTN   DelayTime,
    IN  UINT16  BaseAddr                  // only needs to be 16 bit for I/O address
    )
{
    // The following code is to generate delay for specified amount
    //    of micro seconds using ACPI timer.
    UINTN       TicksNeeded;
    UINT32      TimerValue, NewTimerValue;
    UINTN       OverFlow;
    UINTN       TheRest, EndValue;


    // Set up timer to point to the ACPI Timer register
    BaseAddr += ACPI_IOREG_PM1_TMR;           // *** PORTING NEEDED

    // There are 3.58 ticks per us, so we have to convert the number of us passed
    //  in to the number of ticks that need to pass before the timer has expired
    // Convert us to Ticks, don't loose significant figures or as few as possible
    //  do integer math in ticks/tens of ns and then divide by 100 to get ticks
    //  per us
    OverFlow = 0;

    TheRest = TicksNeeded = (DelayTime * 358) / 100;

    // 32 bits corresponds to approz 71 mins  no delay should be that long
    // otherwise
    // Get the number of times the counter will have to overflow to delay as long
    // as needed
    if (NUM_BITS_IN_ACPI_TIMER < MAX_ACPI_TIMER_BITS) {
        // compiler error when NUM_BITS_IN_ACPI_TIMER=32
        //#### OverFlow = TicksNeeded / (1 << NUM_BITS_IN_ACPI_TIMER);
        //#### TheRest = TicksNeeded % (1 << NUM_BITS_IN_ACPI_TIMER);
    }
    //Enable ACPI MMIO Space
    //Get ACPI MMIO Base Address
    
    // read ACPI Timer
    TimerValue = MmioRead32 ((UINTN)(UINT32)BaseAddr + SB_MMIO_BASE);

    // need to adjust the values based off of the start time
    EndValue = TheRest + TimerValue;

    // check for overflow on addition.  possibly a problem
    if (EndValue < TimerValue) {
      OverFlow++;
    }
    // here make sure that EndValue is less than the max value
    // of the counter
    else if (NUM_BITS_IN_ACPI_TIMER < MAX_ACPI_TIMER_BITS) {
      // compiler error when NUM_BITS_IN_ACPI_TIMER=32
      //### OverFlow += EndValue / (1 << NUM_BITS_IN_ACPI_TIMER);
      //### EndValue = EndValue % (1 << NUM_BITS_IN_ACPI_TIMER);
    }

    // let the timer wrap around as many times as calculated
    while (OverFlow) {
        // read timer hygon look to see if the new value read is less than
        // the current timer value.  if this happens the timer overflowed
        NewTimerValue = MmioRead32 ((UINTN)(UINT32)BaseAddr + SB_MMIO_BASE);
        
        if (NewTimerValue < TimerValue)
          OverFlow--;

        TimerValue = NewTimerValue;
    }


    // now wait for the correct number of ticks that need to occur after
    //  all the needed overflows
    while (EndValue > TimerValue) {
        NewTimerValue = MmioRead32 ((UINTN)(UINT32)BaseAddr + SB_MMIO_BASE);
        // check to see if the timer overflowed.  if it did then
        // the time has elapsed. Because EndValue should be greater than TimerValue
        if (NewTimerValue < TimerValue)
            break;

        TimerValue = NewTimerValue;
    }

    return EFI_SUCCESS;
}


UINT8   gPowerState=0;
/**
    Disable Power Button SMI when AFU.

    @param VOID

    @retval VOID

    @retval VOID

**/
VOID ChipsetDisablePowerButton(VOID)
{
    // Disable PWR Button SMI
    UINT8 Data;

    Data = ReadSbSmiIo8(FCH_SMI_REGAC);
    gPowerState = Data & (BIT6+BIT7);

    Data &= ~(BIT6+BIT7); // Disable PBtn
    WriteSbSmiIo8 (FCH_SMI_REGAC,Data); ;

}
/**
    Re-Enable PowerButton after flash BIOS

    @param VOID

    @retval VOID

    @retval VOID

**/
VOID ChipsetEnablePowerButton (VOID)
{
    UINT8 Data;

    //Clear the Power button status
    while (ReadSbSmiIo8(FCH_SMI_REG84+2) & BIT03) 
    {
        RWSbSmiIo8(FCH_SMI_REG84+2, 0, BIT03);
    }

    Data = ReadSbSmiIo8(FCH_SMI_REGAC);
    Data |= gPowerState; // Enable PBtn
    WriteSbSmiIo8 (FCH_SMI_REGAC,Data); ;
}

//=============================================================================
// for Flash component
//=============================================================================
EFI_BOOT_SERVICES       *pBS;


/**

    @param VOID

    @retval VOID

**/
VOID
ChipsetFlashDeviceWriteEnable(VOID)
{

}

/**

    @param VOID

    @retval VOID

**/
VOID
ChipsetFlashDeviceWriteDisable(VOID)
{

}


/**
    This function returns SMI state

    @param VOID

               
    @retval TRUE SMI enabled
    @retval FALSE SMI disabled

    @note  This function must work at runtime. Do not use boot time services/protocols

**/
BOOLEAN SbLib_GetSmiState(VOID)
{
    BOOLEAN     IsSmiEn=FALSE;
#if SMM_SUPPORT
    UINT8   Buffer;
   Buffer = UEFI_ReadSbSmiIo8(FCH_SMI_REG98+3);   //EIP36094+
    if (!(Buffer&BIT07)) IsSmiEn=TRUE; // Yes, Global was enabled.
#endif // #if SMM_SUPPORT
    return IsSmiEn;
}

/**
    This function disables SMI

    @param VOID

    @retval VOID

    @note  This function must work at runtime. Do not use boot time services/protocols

**/
VOID SbLib_SmiDisable(VOID)
{
#if SMM_SUPPORT
    // Disable SMICmdPort
    UEFI_RWSbSmiIo8(FCH_SMI_REGB2, BIT06, 0);  //EIP36094+
    // Disable Global SMI
    UEFI_RWSbSmiIo8(FCH_SMI_REG98+3, BIT07, BIT07);  //EIP36094+
#endif
}

/**
    This function enables SMI

    @param VOID

    @retval VOID

    @note  This function must work at runtime. Do not use boot time services/protocols

**/
VOID SbLib_SmiEnable(VOID)
{
#if SMM_SUPPORT
    // Enable SMICmdPort
    UEFI_RWSbSmiIo8(FCH_SMI_REGB2, BIT06, BIT6); //EIP36094+
    // Enable Global SMI
    UEFI_RWSbSmiIo8(FCH_SMI_REG98+3, BIT07, 0); //EIP36094+
#endif // #if SMM_SUPPORT
}

#if CMOS_MANAGER_SUPPORT

#include <CmosAccess.h>

/**
    This function is used to access addresses in the CMOS register range
    (0x80-0xff), for PEI and DXE boot phases.

        
    @param PeiServices PEI Services table pointer
        (NULL in DXE phase)
    @param AccessType ReadType or WriteType to specify
        the type of access
    @param CmosRegister The CMOS register to access
    @param CmosParameterValue Pointer to the data
        variable to be accessed

         
    @retval EFI_SUCCESS the access operation was successfull.
    @retval Otherwise a valid EFI error code is returned.

    @note  
      This function is used when a translation from logical
      address to index port value is required.  For example, the
      ICH10 only allows values 0-7f for port 72/73 (upper bank)
      access, so logical CMOS address 0x80 must be translated to
      index port value 0, 0x81 becomes index port value 1, and
      so on.

**/
EFI_STATUS ReadWriteSecondBankTranslated (
    IN      EFI_PEI_SERVICES      **PeiServices,  // NULL in DXE phase
    IN      CMOS_ACCESS_TYPE      AccessType,
    IN      UINT16                CmosRegister,
    IN OUT  UINT8                 *CmosParameterValue )

{
    if (CmosRegister < 0x80 || CmosRegister > 0xff)
        return EFI_INVALID_PARAMETER;

    IoWrite8( CMOS_BANK1_INDEX, (UINT8)(CmosRegister-0x80));

    if (AccessType == ReadType) {
        *CmosParameterValue = IoRead8( CMOS_BANK1_DATA );
    }
    else {
        IoWrite8( CMOS_BANK1_DATA, *CmosParameterValue );
    }

    return EFI_SUCCESS;
}

#endif // CMOS_MANAGER_SUPPORT


//----------------------------------------------------------------------------
#if SMM_SUPPORT
//----------------------------------------------------------------------------
/*
/**
    This hook is called in the very SMI entry and exit.
    Save/Restore chipset data if needed.

    @param Save - Save


    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

    @note  None

**/

EFI_STATUS
SbSmmSaveRestoreStates(
  IN BOOLEAN Save
  )
{
    UINT32  static dStoreCF8;
    UINT8   static bStoreCMOS,bStoreExtCMOS;
    UINT8   static StoreIoCD6, StoreIoCD8, StoreIoC00;

    //### RTC_IO_INDEX                0x70    // RTC I/O Index Port
    //### PIRQ_IO_INDEX               0xC00   // PCI Interrupt I/O Index Port
    //### CM_INDEX_PORT               0xC50
    //### GPM_PORT                    0xC52
    //### BIOSRAM_INDEX_PORT          0xCD4
    //### PM2_IO_INDEX                0xCD0
    //### PM_IO_INDEX                 0xCD6
    //### ALINK_ACCESS_INDEX          0xCD8
    //### ALINK_ACCESS_DATA           ALINK_ACCESS_INDEX + 4

    if (Save) {
        dStoreCF8 = IoRead32(0xcf8);    // Store CF8 (PCI index)
        bStoreCMOS = IoRead8(RTC_IO_INDEX);     // Save CMOS
        bStoreExtCMOS = IoRead8(0x72);  // Store extended CMOS index
        // Taishan
        StoreIoCD6 = IoRead8(PM_IO_INDEX);
        StoreIoCD8 = IoRead8(ALINK_ACCESS_INDEX);

        StoreIoC00 = IoRead8(0xC00);

    } else {
        IoWrite32(0xcf8, dStoreCF8);    // Restore 0xCF8 (PCI index)
        IoWrite8(RTC_IO_INDEX, bStoreCMOS);     // Restore 0x70(CMOS index)
        IoWrite8(0x72, bStoreExtCMOS);  // Restore 0x72(EXT-CMOS index)
        // Taishan
        IoWrite8(PM_IO_INDEX, StoreIoCD6);
        IoWrite8(ALINK_ACCESS_INDEX, StoreIoCD8);

        IoWrite8(0xC00, StoreIoC00);

    }
    return EFI_SUCCESS;
}

//----------------------------------------------------------------------------
#endif  // END OF SMM Related Porting Hooks
//----------------------------------------------------------------------------


EFI_STATUS AmiSioLibSetLpcDeviceDecoding (
    IN VOID      				*LpcPciIo,
    IN UINT16                   Base,
    IN UINT8                    DevUid,
    IN SIO_DEV_TYPE             Type );

EFI_STATUS AmiSioLibSetLpcGenericDecoding (
    IN VOID      				*LpcPciIo,
    IN UINT16                   Base,
    IN UINT16                   Length,
    IN BOOLEAN                  Enable );

//----------------------------------------------------------------------------
// Backward compatible old decode routine <START>
//----------------------------------------------------------------------------

/**
	This is old routine please use AmiSioLibSetLpcDeviceDecoding instead.
    This function sets LPC Bridge Device Decoding

    @param LpcPciIo Pointer to LPC PCI IO Protocol
    @param Base I/O base address, if Base is 0 means disabled the
        decode of the device
    @param DevUid The device Unique ID
    @param Type Device Type, please refer to AMISIO.h


    @retval EFI_SUCCESS Set successfully.
    @retval EFI_UNSUPPORTED There is not proper Device Decoding
        register for the device UID.
**/

EFI_STATUS SbLib_SetLpcDeviceDecoding (
    IN EFI_PCI_IO_PROTOCOL      *LpcPciIo,
    IN UINT16                   Base,
    IN UINT8                    DevUid,
    IN SIO_DEV_TYPE             Type )
{
	// code the new decoding routine
	return AmiSioLibSetLpcDeviceDecoding(LpcPciIo, Base, DevUid, Type);
}

/**
	This is old routine please use AmiSioLibSetLpcGenericDecoding instead.
    This function set LPC Bridge Generic Decoding.

    @param LpcPciIo Pointer to LPC PCI IO Protocol
    @param Base I/O base address
    @param Length I/O Length
    @param Enable Enable/Disable the generic decode range register

               
    @retval EFI_SUCCESS Set successfully.
    @retval EFI_UNSUPPORTED This function is not implemented or the
        Length more than the maximum supported
        size of generic range decoding.
    @retval EFI_INVALID_PARAMETER the Input parameter is invalid.
    @retval EFI_OUT_OF_RESOURCES There is not available Generic
        Decoding Register.
    @retval EFI_NOT_FOUND the generic decode range will be disabled
        is not found.
**/

EFI_STATUS SbLib_SetLpcGenericDecoding (
    IN EFI_PCI_IO_PROTOCOL      *LpcPciIo,
    IN UINT16                   Base,
    IN UINT16                   Length,
    IN BOOLEAN                  Enable )
{
	// code the new decoding routine
	return AmiSioLibSetLpcGenericDecoding(LpcPciIo, Base, Length, Enable);
}

//----------------------------------------------------------------------------
// Backward compatible old decode routine <END>
//----------------------------------------------------------------------------

/**
    This function is PM Specific function to check and Report to
    the System Status Code - CMOS Battary and Power Supply Power
    loss/failure. Also it responcibe of clearing PM Power Loss
    Statuses

    @param VOID

    @retval VOID

**/
VOID CspLibCheckPowerLoss(VOID)
{
//
//####Porting required
//####    GEN_PMCON2_REG  pm2;
//####    GEN_PMCON3_REG  pm3;
//####    RTC_REG_D       RegD;
//####    BOOLEAN         RtcLostPower = FALSE;
//####//------------------
//####    //b00|d00|f1f|reg 0xa2
//####    pm2.GEN_PMCON2=*((UINT8*)PCIE_CFG_ADDR(0,0x1f,0,0xa2));
//####    //b00|d00|f1f|reg 0xa4
//####    pm3.GEN_PMCON3=*((UINT8*)PCIE_CFG_ADDR(0,0x1f,0,0xa4));
//####
//####    //Read RTC Power Register
//####    RegD.REG_D = ReadRtcIndex(RTC_REG_D_INDEX);
//####
//####//Set the correct Code # here
//####    //Clear Power Failure/Status bits
//####    //-------------------------------------------------------
//####    //this bit means it was a power loss during S0 or S1,
//####    //cleared by writing 1 to it
//####//  if(pm2.PwrOkFail) ERROR_CODE(0x00,  EFI_ERROR_MAJOR);
//####
//####    //-------------------------------------------------------
//####    //this bit means it was a power loss on CPU VRM well
//####    // during S0 or S1, cleared by writing 0 to it
//####    if(pm2.CpuPwrFail){
//####//      ERROR_CODE(0x01,  EFI_ERROR_MAJOR);
//####        pm2.CpuPwrFail=0;
//####    }
//####
//####    //-------------------------------------------------------
//####    //this bit means that S4 signal assertion was not long
//####    //enough, cleared by writing 1 to it
//####//  if(pm2.SlpS4Fail) ERROR_CODE(0x02,  EFI_ERROR_MINOR);
//####
//####    //-------------------------------------------------------
//####    //this bit means that a power was shut down because of
//####    //CPU Power Trip, cleared by writing 1 to it
//####    if(pm2.ThermTripSts) ERROR_CODE(GENERIC_CPU_THERMAL_ERROR, EFI_ERROR_MAJOR);
//####
//####    //-------------------------------------------------------
//####    //this bit means that RESET# was active
//####    //cleared by writing 1 to it
//####//  if(pm2.ResetSts) ERROR_CODE(0x04,  EFI_ERROR_MINOR);
//####
//####    //-------------------------------------------------------
//####    //this bit means that Battary Bad or was removed
//####    //cleared by writing 1 to it
//####//  if(pm3.PwrFail) ERROR_CODE(0x05,  EFI_ERROR_MAJOR);
//####
//####    //-------------------------------------------------------
//####    //this bit means that RTC Battary Bad or was removed
//####    //cleared by writing 0 to it
//####    if(pm3.RtcPwrFail) {
//####    //-------------------------------------------------
//####        pm3.RtcPwrFail=0;
//####        RtcLostPower=TRUE;
//####    }
//####
//####    if(RegD.DataValid==0) RtcLostPower=TRUE;
//####
//####    //Write back Cleared Statuses
//####    *((UINT8*)PCIE_CFG_ADDR(0,0x1f,0,0xa2))=pm2.GEN_PMCON2;
//####    *((UINT8*)PCIE_CFG_ADDR(0,0x1f,0,0xa4))=pm3.GEN_PMCON3;
//####
//####    if(RtcLostPower){
//####        ERROR_CODE(DXE_SB_BAD_BATTERY,  EFI_ERROR_MAJOR);
//####        InitRtc();
//####    }
//####
}

/**
    To prevent duplicate SCI in each SciMap registers.
    The reason is there are 64 sources can be mapped to the 32
    standard ACPI EVENT bits. Because of ACPI-EVENT bits is 32 but we
    have 64 sources.
    The register is 0 default. So the 0 value is usually used to be a unuse SCI.
    The routine is to check the duplicate SCI-EVENT in the 64 sources.

    @param unavailableSciNum 
    @param duplicateSci 
    @param duplicateSciReg 
    @retval TRUE duplicateSci, duplicateSciReg
    @retval FALSE no duplicate sci

**/
BOOLEAN
CheckSciMap(
  IN     UINT8  unavailableSciNum,
  IN OUT UINT8  *duplicateSci OPTIONAL,
  IN OUT UINT8  *duplicateSciReg OPTIONAL
  )
{
    UINT8   acpiEvent, Register;
    UINT8   acpiEventsMap[32];
    BOOLEAN isDuplicateSci = FALSE;
    // clear table to unavailable sci
    MemSet(acpiEventsMap, 32, 0);

    // record the 0x58~0x79
    for (Register = FCH_SMI_Usbwakup0; Register <= FCH_SMI_xHC1Pme; Register++) {
        acpiEvent = READ_MEM8((UINTN)(gSbMMIOBASE + SMI_BASE + Register));
        acpiEvent &= 0x1F;
        acpiEventsMap[acpiEvent] = Register;
    }
    //
    // check if it is unused we can update it.
    //   otherwise there is a duplicate SCI.
    //
    for (Register = FCH_SMI_Gevent0; Register <= FCH_SMI_Gevent23; Register++) {
        acpiEvent = READ_MEM8((UINTN)(gSbMMIOBASE + SMI_BASE + Register));
        acpiEvent &= 0x1F;
        if (acpiEventsMap[acpiEvent] == 0 || acpiEvent == unavailableSciNum) {
            acpiEventsMap[acpiEvent] = Register;
        } else {
            isDuplicateSci = TRUE;
            break;
        }
    }
    if (isDuplicateSci == TRUE) {
        if (duplicateSci != NULL) *duplicateSci = acpiEvent;
        if (duplicateSciReg != NULL) *duplicateSciReg = Register;
    }
    return isDuplicateSci;
}

/**
    This function checks if TopSwap (A16, A17, A18 address lines
    inversion) is on
              
    @param VOID

    @retval TRUE TopSwap is ON
    @retval FALSE TopSwap is OFF

    @note  Intel Chipsets are porting required
**/
BOOLEAN IsTopSwapOn( VOID )
{
  return FALSE;
/*

  volatile UINT8* Address8;

  Address8 = (UINT8 *)(SB_RCBA + SB_BACKED_UP_CONTROL_REGISTER);

  return ((*Address8) & 1) ? TRUE : FALSE;
*/
}

/**
    This function sets TopSwap (A16, A17, A18 address lines 
    inversion) to ON or OFF.

    @param On if TRUE, set TopSwap to ON, if FALSE - set to OFF

    @retval VOID

    @note  Intel Chipsets are porting required
**/
VOID  SetTopSwap(
    IN BOOLEAN      On )
{
/*
  volatile UINT8* Address8;

  Address8 = (UINT8 *)(SB_RCBA + SB_BACKED_UP_CONTROL_REGISTER);

  if (On) {
    *Address8 |= 1;
  } else {
    *Address8 &= 0xFE;
  }
*/
}

#if SMBIOS_SUPPORT && SB_WAKEUP_TYPE_FN
/**
    Detect and return SMBIOS wake-up type

    @param VOID

    @retval 0x01 = Other
            0x02 = Unknown
            0x03 = APM Timer
            0x04 = Modem Ring
            0x05 = LAN Remote
            0x06 = Power Switch
            0x07 = PCI PME#
            0x08 = AC Power Restored

    @note  Porting required
**/
UINT8
getWakeupTypeForSmbios (VOID)
{
	SB_WAKEUP_TYPE	SbWakeType = PcdGet32(PcdSbWakeType);
	UINT8			WakeupTypeForSmbios = 0x6;

	if (SbWakeType >= SbWakeupTypeSmBiosMax) WakeupTypeForSmbios = 0x1; // Other
	else WakeupTypeForSmbios = (UINT8)SbWakeType;

	return WakeupTypeForSmbios;
}
#endif

//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
//                                                                            //
//                  Below for any access routine                              //
//                                                                            //
//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//

/**
    Sb CspLib Get BootScriptSaveProtocol

    @param VOID

    @retval EFI_BOOT_SCRIPT_SAVE_PROTOCOL return protocol

**/
static
EFI_BOOT_SCRIPT_SAVE_PROTOCOL*
SbCspLibGetBootScriptSaveProtocol(VOID)
{
    EFI_BOOT_SCRIPT_SAVE_PROTOCOL static *SbCspLibProtocol = NULL;
    if (SbCspLibProtocol == NULL) {
        EFI_GUID    TheGuid = EFI_BOOT_SCRIPT_SAVE_GUID;
        EFI_STATUS  Status;

        Status = pBS->LocateProtocol(&TheGuid, NULL, &SbCspLibProtocol);
        if (EFI_ERROR(Status)) {
            //####PROGRESS_CODE(DXE_SB_ERROR); while(1) {}
            IoWrite8(0x80,0xD2); while(1) {}
        }
    }
    return SbCspLibProtocol;
}

/**
    Sb CspLib Get PciRootBridgeIoProtocol

    @param VOID

    @retval EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL return protocol

**/
static
EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL*
SbCspLibGetPciRootBridgeIoProtocol(VOID)
{
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL static *SbCspLibProtocol = NULL;
    if (SbCspLibProtocol == NULL) {
        EFI_GUID    TheGuid = EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_GUID;
        EFI_STATUS  Status;

        Status = pBS->LocateProtocol(&TheGuid, NULL, &SbCspLibProtocol);
        if (EFI_ERROR(Status)) {
            //####PROGRESS_CODE(DXE_SB_ERROR); while(1) {}
            IoWrite8(0x80,0xD2); while(1) {}
        }
    }
    return SbCspLibProtocol;
}

//----------------------------------------------------------------------------
// Standard PCI Access Routines, No Porting Required.
//----------------------------------------------------------------------------

/**
    This function converts the format of EFI PCI address to
    the standard PCI I/O format and write to PCI I/O index port.

    @param PciAddr Pointer to PCI configuration Address

    @retval VOID

    @note  None.

**/

VOID SbCspLibEfiPciAddr2PciCfgAddr (
    IN EFI_PCI_CONFIGURATION_ADDRESS    *PciAddr )
{
    UINT32      PciCfg32 = ( 0x80000000 | (PciAddr->Bus << 16) | \
                             (PciAddr->Device << 11) | \
                             (PciAddr->Function << 8) | \
                             (PciAddr->Register & 0xfc) );
    IoWrite32(0xcf8, PciCfg32);
}

/**
    This function reads a 8bits data from PCI configuration
    space register.

    @param PciBusDevFunReg EFI PCI Configuration Address

        UINT8
    @retval value return value

    @note  None.

**/

UINT8 SbCspLibReadPci8 (
    IN UINT64   PciBusDevFunReg )
{
    SbCspLibEfiPciAddr2PciCfgAddr((EFI_PCI_CONFIGURATION_ADDRESS *)&PciBusDevFunReg);
    return IoRead8(0xcfc | (UINT8)(PciBusDevFunReg & 3));
}

/**
    This function reads a 16bits data from PCI configuration
    space register.

    @param PciBusDevFunReg EFI PCI Configuration Address

        UINT16
    @retval value return value

    @note  None.

**/

UINT16 SbCspLibReadPci16 (
    IN UINT64   PciBusDevFunReg )
{
    SbCspLibEfiPciAddr2PciCfgAddr((EFI_PCI_CONFIGURATION_ADDRESS *)&PciBusDevFunReg);
    return IoRead16(0xcfc | (UINT8)(PciBusDevFunReg & 2));
}

/**
    This function reads a 32bits data from PCI configuration
    space register.

    @param PciBusDevFunReg EFI PCI Configuration Address

        UINT32
    @retval value return value

    @note  None.

**/

UINT32 SbCspLibReadPci32 (
    IN UINT64   PciBusDevFunReg )
{
    SbCspLibEfiPciAddr2PciCfgAddr((EFI_PCI_CONFIGURATION_ADDRESS *)&PciBusDevFunReg);
    return IoRead32(0xcfc);
}

/**
    This function writes a 8bits data to PCI configuration space
    register.

    @param PciBusDevFunReg EFI PCI Configuration Address
    @param WriteValue8 8 Bits data writes to PCI register

    @retval VOID

    @note  None.

**/

VOID SbCspLibWritePci8 (
    IN UINT64   PciBusDevFunReg,
    IN UINT8    WriteValue8 )
{
    SbCspLibEfiPciAddr2PciCfgAddr((EFI_PCI_CONFIGURATION_ADDRESS *)&PciBusDevFunReg);
    IoWrite8(0xcfc | (UINT8)(PciBusDevFunReg & 3), WriteValue8);
}

/**
    This function writes a 16bits data to PCI configuration space
    register.

    @param PciBusDevFunReg EFI PCI Configuration Address
    @param WriteValue1 16 Bits data writes to PCI register

    @retval VOID

    @note  None.

**/

VOID SbCspLibWritePci16 (
    IN UINT64   PciBusDevFunReg,
    IN UINT16   WriteValue16 )
{
    SbCspLibEfiPciAddr2PciCfgAddr((EFI_PCI_CONFIGURATION_ADDRESS *)&PciBusDevFunReg);
    IoWrite16(0xcfc | (UINT8)(PciBusDevFunReg & 2), WriteValue16);
}

/**
    This function writes a 32bits data to PCI configuration space
    register.

    @param PciBusDevFunReg EFI PCI Configuration Address
    @param WriteValue32 32 Bits data writes to PCI register

    @retval VOID

    @note  None.

**/

VOID SbCspLibWritePci32 (
    IN UINT64   PciBusDevFunReg,
    IN UINT32   WriteValue32 )
{
    SbCspLibEfiPciAddr2PciCfgAddr((EFI_PCI_CONFIGURATION_ADDRESS *)&PciBusDevFunReg);
    IoWrite32(0xcfc, WriteValue32);
}

/**
    This function reads a 8bits data from PCI configuration
    space, applies masks, and writes it back.

    @param PciBusDevFunReg EFI PCI Configuration Address
    @param SetBit8 Mask of bits to set (1 = Set)
    @param ResetBit8 Mask of bits to clear  (1 = clear)

        UINT8 
    @retval value The final value writes to the PCI register.

    @note  None.

**/

UINT8 SbCspLibRwPci8 (
    IN UINT64   PciBusDevFunReg,
    IN UINT8    SetBit8,
    IN UINT8    ResetBit8 )
{
    UINT8       Buffer8;
    Buffer8 = SbCspLibReadPci8(PciBusDevFunReg) & ~ResetBit8 | SetBit8;
    SbCspLibWritePci8(PciBusDevFunReg, Buffer8);
    return Buffer8;
}

/**
    This function reads a 16bits data from PCI configuration
    space, applies masks, and writes it back.

    @param PciBusDevFunReg EFI PCI Configuration Address
    @param SetBit16 Mask of bits to set (1 = Set)
    @param ResetBit16 Mask of bits to clear  (1 = clear)

        UINT16 
    @retval value The final value writes to the PCI register.

    @note  None.

**/

UINT16 SbCspLibRwPci16 (
    IN UINT64   PciBusDevFunReg,
    IN UINT16   SetBit16,
    IN UINT16   ResetBit16 )
{
    UINT16      Buffer16;
    Buffer16 = SbCspLibReadPci16(PciBusDevFunReg) & ~ResetBit16 | SetBit16;
    SbCspLibWritePci16(PciBusDevFunReg, Buffer16);
    return Buffer16;
}

/**
    This function reads a 32bits data from PCI configuration
    space, applies masks, and writes it back.

    @param PciBusDevFunReg EFI PCI Configuration Address
    @param SetBit32 Mask of bits to set (1 = Set)
    @param ResetBit32 Mask of bits to clear  (1 = clear)

        UINT32 
    @retval value The final value writes to the PCI register.

    @note  None.

**/

UINT32 SbCspLibRwPci32 (
    IN UINT64   PciBusDevFunReg,
    IN UINT32   SetBit32,
    IN UINT32   ResetBit32 )
{
    UINT32      Buffer32;
    Buffer32 = SbCspLibReadPci32(PciBusDevFunReg) & ~ResetBit32 | SetBit32;
    SbCspLibWritePci32(PciBusDevFunReg, Buffer32);
    return Buffer32;
}

//----------------------------------------------------------------------------
// Standard PCI Access Routines, S3 Boot Script support, No Porting Required.
//----------------------------------------------------------------------------

/**
    This function writes a 8bits data to PCI configuration space
    register and Boot Script.

    @param mBootScriptSave Pointer to Boot Script Save Protocal
    @param BusDevFunReg EFI PCI Configuration Address
    @param WriteValue8 8 Bits data writes to PCI register and
        Boot Script.

    @retval VOID

    @note  None.

**/

VOID SbCspLibWritePci8S3 (
    IN EFI_BOOT_SCRIPT_SAVE_PROTOCOL    *mBootScriptSave,
    IN UINT64                           BusDevFunReg,
    IN UINT8                            WriteValue8 )
{

    SbCspLibWritePci8(BusDevFunReg, WriteValue8);

    BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO( mBootScriptSave, \
                                           EfiBootScriptWidthUint8, \
                                           BusDevFunReg, 1, &WriteValue8 );
}

/**
    This function writes a 16bits data to PCI configuration space
    register and Boot Script.

    @param mBootScriptSave Pointer to Boot Script Save Protocal
    @param BusDevFunReg EFI PCI Configuration Address
    @param WriteValue16 16 Bits data writes to PCI register and
        Boot Script.

    @retval VOID

    @note  None.

**/

VOID SbCspLibWritePci16S3 (
    IN EFI_BOOT_SCRIPT_SAVE_PROTOCOL    *mBootScriptSave,
    IN UINT64                           BusDevFunReg,
    IN UINT16                           WriteValue16 )
{

    SbCspLibWritePci16(BusDevFunReg, WriteValue16);

    BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO( mBootScriptSave, \
                                           EfiBootScriptWidthUint16, \
                                           BusDevFunReg, 1, &WriteValue16 );
}

/**
    This function writes a 32bits data to PCI configuration space
    register and Boot Script.

    @param mBootScriptSave Pointer to Boot Script Save Protocal
    @param BusDevFunReg EFI PCI Configuration Address
    @param WriteValue32 32 Bits data writes to PCI register and
        Boot Script.

    @retval VOID

    @note  None.

**/

VOID SbCspLibWritePci32S3 (
    IN EFI_BOOT_SCRIPT_SAVE_PROTOCOL    *mBootScriptSave,
    IN UINT64                           BusDevFunReg,
    IN UINT32                           WriteValue32 )
{

    SbCspLibWritePci32(BusDevFunReg, WriteValue32);

    BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO( mBootScriptSave, \
                                           EfiBootScriptWidthUint32, \
                                           BusDevFunReg, 1, &WriteValue32 );
}

/**
    This function reads a 8bits data from PCI configuration
    space, applies masks, and writes it back, also writes it to
    Boot Script.

    @param mBootScriptSave Pointer to Boot Script Save Protocal
    @param BusDevFunReg EFI PCI Configuration Address
    @param SetBit8 Mask of bits to set (1 = Set)
    @param ResetBit8 Mask of bits to clear  (1 = clear)

        UINT8 
    @retval value The final value writes to the PCI register.

    @note  None.

**/


UINT8 SbCspLibRwPci8S3 (
    IN EFI_BOOT_SCRIPT_SAVE_PROTOCOL    *mBootScriptSave,
    IN UINT64                           BusDevFunReg,
    IN UINT8                            SetBit8,
    IN UINT8                            ResetBit8 )
{
    ResetBit8 = ~ResetBit8;
    BOOT_SCRIPT_S3_PCI_CONFIG_READ_WRITE_MACRO( mBootScriptSave, \
                                                EfiBootScriptWidthUint8, \
                                                BusDevFunReg, &SetBit8, \
                                                &ResetBit8 );
    return SbCspLibRwPci8(BusDevFunReg, SetBit8, ~ResetBit8);
}

/**
    This function reads a 16bits data from PCI configuration
    space, applies masks, and writes it back, also writes it to
    Boot Script.

    @param mBootScriptSave Pointer to Boot Script Save Protocal
    @param BusDevFunReg EFI PCI Configuration Address
    @param SetBit16 Mask of bits to set (1 = Set)
    @param ResetBit16 Mask of bits to clear  (1 = clear)

        UINT16 
    @retval value The final value writes to the PCI register.

    @note  None.

**/

UINT16 SbCspLibRwPci16S3 (
    IN EFI_BOOT_SCRIPT_SAVE_PROTOCOL    *mBootScriptSave,
    IN UINT64                           BusDevFunReg,
    IN UINT16                           SetBit16,
    IN UINT16                           ResetBit16 )
{
    ResetBit16 = ~ResetBit16;
    BOOT_SCRIPT_S3_PCI_CONFIG_READ_WRITE_MACRO( mBootScriptSave, \
                                                EfiBootScriptWidthUint16, \
                                                BusDevFunReg, &SetBit16, \
                                                &ResetBit16 );
    return SbCspLibRwPci16(BusDevFunReg, SetBit16, ~ResetBit16);
}

/**
    This function reads a 32bits data from PCI configuration
    space, applies masks, and writes it back, also writes it to
    Boot Script.

    @param mBootScriptSave Pointer to Boot Script Save Protocal
    @param BusDevFunReg EFI PCI Configuration Address
    @param SetBit32 Mask of bits to set (1 = Set)
    @param ResetBit32 Mask of bits to clear  (1 = clear)

        UINT32 
    @retval value The final value writes to the PCI register.

    @note  None.

**/

UINT32 SbCspLibRwPci32S3 (
    IN EFI_BOOT_SCRIPT_SAVE_PROTOCOL    *mBootScriptSave,
    IN UINT64                           BusDevFunReg,
    IN UINT32                           SetBit32,
    IN UINT32                           ResetBit32 )
{
    ResetBit32 = ~ResetBit32;
    BOOT_SCRIPT_S3_PCI_CONFIG_READ_WRITE_MACRO( mBootScriptSave, \
                                                EfiBootScriptWidthUint32, \
                                                BusDevFunReg, &SetBit32, \
                                                &ResetBit32 );
    return SbCspLibRwPci32(BusDevFunReg, SetBit32, ~ResetBit32);
}

//----------------------------------------------------------------------------
// DXE PCI Access Routines, Redirect to Protocol, No Porting Required.
//----------------------------------------------------------------------------

/**
    Read PCI redirect to EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL

    @param Flag 
    @param Address 

        UINTN
    @retval Data return value		

**/

UINTN
SbCspLibDxeReadPci(
  IN UINT8  Flag,
  IN UINT64 Address
  )
{
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *Interface = SbCspLibGetPciRootBridgeIoProtocol();
    UINTN                           Data;
    Interface->Pci.Read(Interface, Flag, (UINT64)Address, 1, &Data);
    return Data;
}

/**
    Write PCI redirect to EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL

    @param Flag 
    @param Address 
    @param Value 
    @retval VOID

**/

VOID
SbCspLibDxeWritePci(
  IN UINT8  Flag,
  IN UINT64 Address,
  IN UINTN  Value
  )
{
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *Interface = SbCspLibGetPciRootBridgeIoProtocol();
    EFI_BOOT_SCRIPT_SAVE_PROTOCOL   *mBootScriptSave;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH                     Width;
    UINTN                           Data;

    mBootScriptSave = SbCspLibGetBootScriptSaveProtocol();
    Width = (EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH)(Flag & ~S3_SAVE);
    Data = Value;

    Interface->Pci.Write(Interface, Width, (UINT64)Address, 1, &Data);
    if (Flag & S3_SAVE) {
        BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(
                mBootScriptSave, Width, Address, 1, &Data);
    }
}

/**
    Read/Write PCI redirect to EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL

    @param Flag 
    @param Address 
    @param SetBit 
    @param ResetBit 
    @retval VOID

**/

VOID
SbCspLibDxeRwPci(
  IN UINT8  Flag,
  IN UINT64 Address,
  IN UINTN  SetBit,
  IN UINTN  ResetBit
  )
{
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH   Width;
    EFI_BOOT_SCRIPT_SAVE_PROTOCOL           *mBootScriptSave;
    UINTN                                   Data;

    mBootScriptSave = SbCspLibGetBootScriptSaveProtocol();
    Width = (EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH)(Flag & ~S3_SAVE);

    Data = SbCspLibDxeReadPci(Width, Address);
    Data = (Data & ~ResetBit) | SetBit;
    SbCspLibDxeWritePci(Width, Address, Data);
    if (Flag & S3_SAVE) {
        UINTN   BitMask = ~ResetBit;
        BOOT_SCRIPT_S3_PCI_CONFIG_READ_WRITE_MACRO(
                mBootScriptSave, Width, Address, &SetBit, &BitMask);
    }
}

//----------------------------------------------------------------------------;
// Standard Memory Macros, No Porting Required.                               ;
//----------------------------------------------------------------------------;

/**
    This function reads a 8bits data from a specific memory
    (or MMIO) address.

    @param Address A 64Bits Memory (or MMIO) address

        UINT8
    @retval Address return value

    @note  None.

**/

UINT8 SbCspLibReadMem8 (
    IN UINT64   Address )
{
    return *(UINT8 volatile *)Address;
}

/**
    This function reads a 16bits data from a specific memory
    (or MMIO) address.

    @param Address A 64Bits Memory (or MMIO) address

        UINT16
    @retval Address return value

    @note  None.

**/

UINT16 SbCspLibReadMem16 (
    IN UINT64   Address )
{
    return *(UINT16 volatile *)Address;
}

/**
    This function reads a 32bits data from a specific memory
    (or MMIO) address.

    @param Address A 64Bits Memory (or MMIO) address

        UINT32
    @retval Address return value

    @note  None.

**/

UINT32 SbCspLibReadMem32 (
    IN UINT64   Address )
{
    return *(UINT32 volatile *)Address;
}

/**
    This function writes a 8bits data to a specific memory
    (or MMIO) address.

    @param Address A 64Bits Memory (or MMIO) address
    @param Value A 8Bits data writes to the address.

    @retval VOID

    @note  None.

**/

VOID SbCspLibWriteMem8 (
    IN UINT64   Address,
    IN UINT8    Value )
{
    *(UINT8 volatile *)Address = Value;
}

/**
    This function writes a 16bits data to a specific memory
    (or MMIO) address.

    @param Address A 64Bits Memory (or MMIO) address
    @param Value A 16Bits data writes to the address.

    @retval VOID

    @note  None.

**/

VOID SbCspLibWriteMem16 (
    IN UINT64   Address,
    IN UINT16   Value )
{
    *(UINT16 volatile *)Address = Value;
}

/**
    This function writes a 32bits data to a specific memory
    (or MMIO) address.

    @param Address A 64Bits Memory (or MMIO) address
    @param Value A 32Bits data writes to the address.

    @retval VOID

    @note  None.

**/

VOID SbCspLibWriteMem32 (
    IN UINT64   Address,
    IN UINT32   Value )
{
    *(UINT32 volatile *)Address = Value;
}

/**
    This function reads a 8bits data from a specific memory
    (or MMIO) address, applies masks, and writes it back.

    @param Address Mask of bits to set (1 = Set)
    @param SetBit8 Mask of bits to clear  (1 = clear)
    @param ResetBit8 

    @retval UINT8 The final value writes to the memory(or MMIO) address

    @note  None.

**/

UINT8 SbCspLibRwMem8 (
    IN UINT64   Address,
    IN UINT8    SetBit8,
    IN UINT8    ResetBit8 )
{
    UINT8       Buffer8;
    Buffer8 = SbCspLibReadMem8(Address) & ~ResetBit8 | SetBit8;
    SbCspLibWriteMem8(Address, Buffer8);
    return Buffer8;
}

/**
    This function reads a 16bits data from a specific memory
    (or MMIO) address, applies masks, and writes it back.

    @param Address Mask of bits to set (1 = Set)
    @param SetBit16 Mask of bits to clear  (1 = clear)
    @param ResetBit16 

        UINT16 
    @retval value The final value writes to the memory(or MMIO) address

    @note  None.

**/

UINT16 SbCspLibRwMem16 (
    IN UINT64   Address,
    IN UINT16   SetBit16,
    IN UINT16   ResetBit16 )
{
    UINT16      Buffer16;
    Buffer16 = SbCspLibReadMem16(Address) & ~ResetBit16 | SetBit16;
    SbCspLibWriteMem16(Address, Buffer16);
    return Buffer16;
}

/**
    This function reads a 32bits data from a specific memory
    (or MMIO) address, applies masks, and writes it back.

    @param Address Input address
    @param SetBit32 Mask of bits to set (1 = Set)
    @param ResetBit32 Mask of bits to clear  (1 = clear)

        UINT32 
    @retval Buffer32 The final value writes to the memory(or MMIO) address

    @note  None.

**/

UINT32 SbCspLibRwMem32 (
    IN UINT64   Address,
    IN UINT32   SetBit32,
    IN UINT32   ResetBit32 )
{
    UINT32      Buffer32;
    Buffer32 = SbCspLibReadMem32(Address) & ~ResetBit32 | SetBit32;
    SbCspLibWriteMem32(Address, Buffer32);
    return Buffer32;
}

//----------------------------------------------------------------------------;
// Standard Memory Macros + S3 Script, No Porting Required.                   ;
//----------------------------------------------------------------------------;
/**
    This function writes a 8bits data to a specific memory
    (or MMIO) address and Boot Script.

    @param Address A 64Bits Memory (or MMIO) address
    @param Value A 8Bits data writes to the address.

    @retval VOID

    @note  None.

**/

VOID SbCspLibWriteMem8S3 (
    IN UINT64                           Address,
    IN UINT8                            Value )
{
    EFI_BOOT_SCRIPT_SAVE_PROTOCOL     *mBootScriptSave;

    mBootScriptSave = SbCspLibGetBootScriptSaveProtocol();

    SbCspLibWriteMem8(Address, Value);

    BOOT_SCRIPT_S3_MEM_WRITE_MACRO( mBootScriptSave, \
                                    EfiBootScriptWidthUint8, \
                                    Address, 1, &Value );
}

/**
    This function writes a 16bits data to a specific memory
    (or MMIO) address and Boot Script.

    @param Address A 64Bits Memory (or MMIO) address
    @param Value A 16Bits data writes to the address.

    @retval VOID

    @note  None.

**/

VOID SbCspLibWriteMem16S3(
    IN UINT64                           Address,
    IN UINT16                           Value )
{
    EFI_BOOT_SCRIPT_SAVE_PROTOCOL     *mBootScriptSave;

    mBootScriptSave = SbCspLibGetBootScriptSaveProtocol();

    SbCspLibWriteMem16(Address, Value);

    BOOT_SCRIPT_S3_MEM_WRITE_MACRO( mBootScriptSave, \
                                    EfiBootScriptWidthUint16, \
                                    Address, 1, &Value );
}

/**
    This function writes a 32bits data to a specific memory
    (or MMIO) address and Boot Script.

    @param Address A 64Bits Memory (or MMIO) address
    @param Value A 32Bits data writes to the address.

    @retval VOID

    @note  None.

**/

VOID SbCspLibWriteMem32S3 (
    IN UINT64                           Address,
    IN UINT32                           Value )
{
    EFI_BOOT_SCRIPT_SAVE_PROTOCOL     *mBootScriptSave;

    mBootScriptSave = SbCspLibGetBootScriptSaveProtocol();

    SbCspLibWriteMem32(Address, Value);

    BOOT_SCRIPT_S3_MEM_WRITE_MACRO( mBootScriptSave, \
                                    EfiBootScriptWidthUint32, \
                                    Address, 1, &Value );
}

/**
    This function reads a 8bits data from a specific memory
    (or MMIO) address, applies masks, and writes it back, also
    writes it to Boot Script.

    @param Address A 64Bits Memory (or MMIO) address
    @param SetBit8 Mask of bits to set (1 = Set)
    @param ResetBit8 Mask of bits to clear  (1 = clear)

        UINT8 
    @retval Buffer The final value writes to the memory(or MMIO) address

    @note  None.

**/

UINT8 SbCspLibRwMem8S3 (
    IN UINT64                           Address,
    IN UINT8                            SetBit8,
    IN UINT8                            ResetBit8 )
{
    EFI_BOOT_SCRIPT_SAVE_PROTOCOL     *mBootScriptSave;

    mBootScriptSave = SbCspLibGetBootScriptSaveProtocol();

    ResetBit8 = ~ResetBit8;
    BOOT_SCRIPT_S3_MEM_READ_WRITE_MACRO( mBootScriptSave, \
                                         EfiBootScriptWidthUint8, \
                                         Address, &SetBit8, &ResetBit8 );
    return SbCspLibRwMem8(Address, SetBit8, ~ResetBit8);
}

/**
    This function reads a 16bits data from a specific memory
    (or MMIO) address, applies masks, and writes it back, also
    writes it to Boot Script.

    @param Address A 64Bits Memory (or MMIO) address
    @param SetBit16 Mask of bits to set (1 = Set)
    @param ResetBit16 Mask of bits to clear  (1 = clear)

        UINT16
    @retval Buffer The final value writes to the memory(or MMIO) address

    @note  None.

**/

UINT16 SbCspLibRwMem16S3 (
    IN UINT64                           Address,
    IN UINT16                           SetBit16,
    IN UINT16                           ResetBit16 )
{
    EFI_BOOT_SCRIPT_SAVE_PROTOCOL     *mBootScriptSave;

    mBootScriptSave = SbCspLibGetBootScriptSaveProtocol();

    ResetBit16 = ~ResetBit16;
    BOOT_SCRIPT_S3_MEM_READ_WRITE_MACRO( mBootScriptSave, \
                                         EfiBootScriptWidthUint16, \
                                         Address, &SetBit16, &ResetBit16 );
    return SbCspLibRwMem16(Address, SetBit16, ~ResetBit16);
}

/**
    This function reads a 32bits data from a specific memory
    (or MMIO) address, applies masks, and writes it back, also
    writes it to Boot Script.

    @param Address A 64Bits Memory (or MMIO) address
    @param SetBit32 Mask of bits to set (1 = Set)
    @param ResetBit32 Mask of bits to clear  (1 = clear)

        UINT32 
    @retval Buffer The final value writes to the memory(or MMIO) address

    @note  None.

**/

UINT32 SbCspLibRwMem32S3 (
    IN UINT64                           Address,
    IN UINT32                           SetBit32,
    IN UINT32                           ResetBit32 )
{
    EFI_BOOT_SCRIPT_SAVE_PROTOCOL     *mBootScriptSave;

    mBootScriptSave = SbCspLibGetBootScriptSaveProtocol();

    ResetBit32 = ~ResetBit32;
    BOOT_SCRIPT_S3_MEM_READ_WRITE_MACRO( mBootScriptSave, \
                                         EfiBootScriptWidthUint32, \
                                         Address, &SetBit32, &ResetBit32 );
    return SbCspLibRwMem32(Address, SetBit32, ~ResetBit32);
}

/**

    @param VOID

        UINT64
    @retval gSbMMIOBASE SB_MMIO_BASE

**/
UINT64
GetHygonSbAcpiMmio (VOID)
{
    return gSbMMIOBASE;
}

/**

    @param VOID

        UINT64
    @retval gSbMMIOBASE SB_MMIO_BASE		

**/
UINT64
GetHygonSbAcpiMmioEx (VOID)
{
	// Kern fix the FCH MMIO.
    return GetHygonSbAcpiMmio();
}

/**

    @param Reg 

        UINT8
    @retval value the content of specific address.

**/
UINT8
ReadSbSmiIo8 (
    IN UINT8    Reg
    )
{
    return SbCspLibReadMem8(GetHygonSbAcpiMmio() + SMI_BASE + Reg);
}

/**
    This function support the UEFI OS called,in the UEFI OS,it will
    use the liner mode to access Virtual memory address



    @param Reg 

        UINT8
    @retval value the content of specific address.

**/
UINT8
UEFI_ReadSbSmiIo8 (
    IN UINT8    Reg
    )
{
    return SbCspLibReadMem8((UINT64)gSbMMIOBASE + SMI_BASE + Reg);
}

/**

    @param Reg 
    @param Data 

    @retval VOID

**/
VOID
WriteSbSmiIo8 (
    IN UINT8    Reg,
    IN UINT8    Data
)
{
    SbCspLibWriteMem8(GetHygonSbAcpiMmio() + SMI_BASE + Reg, Data);
}


/**
    This function support the UEFI OS called in the UEFI OS,it will
    use the liner mode to access Virtual memory address

    @param Reg 
    @param Data 

    @retval VOID

**/
VOID
UEFI_WriteSbSmiIo8 (
    IN UINT8    Reg,
    IN UINT8    Data
)
{
    SbCspLibWriteMem8((UINT64)gSbMMIOBASE + SMI_BASE + Reg, Data);
}


//P111109A>>
/**

    @param Reg 
    @param Data 

    @retval VOID

**/
VOID
WriteSbSmiIo8S3 (
    IN UINT8    Reg,
    IN UINT8    Data
)
{
    SbCspLibWriteMem8S3(GetHygonSbAcpiMmio() + SMI_BASE + Reg, Data);
}

//<<P111109A
//----------------------------------------------------------------------------
/**

    @param Reg 
    @param ResetBit8 
    @param SetBit8 

    @retval VOID

**/
VOID
RWSbSmiIo8 (
    IN UINT8    Reg,
    IN UINT8    ResetBit8,
    IN UINT8    SetBit8
)
{
    UINT8       Buffer8;

    Buffer8 = ReadSbSmiIo8(Reg) & ~ResetBit8 | SetBit8;
    WriteSbSmiIo8(Reg, Buffer8);
}

//----------------------------------------------------------------------------
/**
    This function support the UEFI OS called in the UEFI OS,it will
    use the liner mode to access Virtual memory address

    @param Reg 
    @param ResetBit8 
    @param SetBit8 

    @retval VOID

**/
VOID
UEFI_RWSbSmiIo8 (
    IN UINT8    Reg,
    IN UINT8    ResetBit8,
    IN UINT8    SetBit8
)
{
    UINT8       Buffer8;

    Buffer8 = UEFI_ReadSbSmiIo8(Reg) & ~ResetBit8 | SetBit8;
    UEFI_WriteSbSmiIo8(Reg, Buffer8);
}


/**

    @param Reg 
    @param ResetBit8 
    @param SetBit8 

    @retval VOID

**/
VOID
RWSbSmiIo8S3 (
    IN UINT8    Reg,
    IN UINT8    ResetBit8,
    IN UINT8    SetBit8
)
{
    UINT8       Buffer8;

    Buffer8 = ReadSbSmiIo8(Reg) & ~ResetBit8 | SetBit8;
    WriteSbSmiIo8S3(Reg, Buffer8);
}

/**

    @param Reg 

        UINT16
    @retval value return value

**/
UINT16
ReadSbSmiIo16 (
    IN UINT8    Reg
    )
{
    return SbCspLibReadMem16(GetHygonSbAcpiMmio() + SMI_BASE + Reg);
}

/**

    @param Reg 
    @param Data 

    @retval VOID

**/
VOID
WriteSbSmiIo16 (
    IN UINT8    Reg,
    IN UINT16    Data
)
{
    SbCspLibWriteMem16(GetHygonSbAcpiMmio() + SMI_BASE + Reg, Data);
}


/**

    @param Reg 
    @param Data 

    @retval VOID

**/
VOID
WriteSbSmiIo16S3 (
    IN UINT8    Reg,
    IN UINT16    Data
)
{
    SbCspLibWriteMem16S3(GetHygonSbAcpiMmio() + SMI_BASE + Reg, Data);
}

//----------------------------------------------------------------------------
/**

    @param Reg 
    @param ResetBit16 
    @param SetBit16 

    @retval VOID

**/
VOID
RWSbSmiIo16 (
    IN UINT8    Reg,
    IN UINT16    ResetBit16,
    IN UINT16    SetBit16
)
{
    UINT16       Buffer16;

    Buffer16 = ReadSbSmiIo16(Reg) & ~ResetBit16 | SetBit16;
    WriteSbSmiIo16(Reg, Buffer16);
}

/**

    @param Reg 
    @param ResetBit16 
    @param SetBit16 

    @retval VOID

**/
VOID
RWSbSmiIo16S3 (
    IN UINT8    Reg,
    IN UINT16    ResetBit16,
    IN UINT16    SetBit16
)
{
    UINT16       Buffer16;

    Buffer16 = ReadSbSmiIo16(Reg) & ~ResetBit16 | SetBit16;
    WriteSbSmiIo16S3(Reg, Buffer16);
}


/**

    @param Reg 

        UINT32
    @retval value return value

**/
UINT32
ReadSbSmiIo32 (
    IN UINT8    Reg
    )
{
    return SbCspLibReadMem32(GetHygonSbAcpiMmio() + SMI_BASE + Reg);
}

/**

    @param Reg 
    @param Data 

    @retval VOID

**/
VOID
WriteSbSmiIo32 (
    IN UINT8    Reg,
    IN UINT32    Data
)
{
    SbCspLibWriteMem32(GetHygonSbAcpiMmio() + SMI_BASE + Reg, Data);
}

/**

    @param Reg 
    @param Data 

    @retval VOID

**/
VOID
WriteSbSmiIo32S3 (
    IN UINT8    Reg,
    IN UINT32    Data
)
{
    SbCspLibWriteMem32S3(GetHygonSbAcpiMmio() + SMI_BASE + Reg, Data);
}


/**

    @param Reg 
    @param ResetBit 
    @param SetBit 

    @retval VOID

**/
VOID
RWSbSmiIo32 (
    IN UINT8    Reg,
    IN UINT32   ResetBit,
    IN UINT32   SetBit
)
{
    UINT32       Buffer;

    Buffer = ReadSbSmiIo32(Reg) & ~ResetBit | SetBit;
    WriteSbSmiIo32(Reg, Buffer);
}


/**

    @param Reg 
    @param ResetBit 
    @param SetBit 

    @retval VOID

**/
VOID
RWSbSmiIo32S3 (
    IN UINT8    Reg,
    IN UINT32   ResetBit,
    IN UINT32   SetBit
)
{
    UINT32       Buffer;

    Buffer = ReadSbSmiIo32(Reg) & ~ResetBit | SetBit;
    WriteSbSmiIo32S3(Reg, Buffer);
}

//<<P071209B
/**

    @param Reg 

        UINT8
    @retval value return value

**/
UINT8
ReadSbPmuIo8 (
    IN UINT8    Reg
)
{
    return SbCspLibReadMem8(GetHygonSbAcpiMmio() + PMIO_BASE + Reg);
}

/**

    @param Reg 
    @param Data 

    @retval VOID

**/
VOID
WriteSbPmuIo8 (
    IN UINT8    Reg,
    IN UINT8    Data
)
{
    SbCspLibWriteMem8(GetHygonSbAcpiMmio() + PMIO_BASE + Reg, Data);
}

/**

    @param Reg 
    @param Data 

    @retval VOID

**/
VOID
WriteSbPmuIo8S3 (
    IN UINT8    Reg,
    IN UINT8    Data
)
{
    SbCspLibWriteMem8S3(GetHygonSbAcpiMmio() + PMIO_BASE + Reg, Data);
}

/**

    @param Reg 
    @param ResetBit8 

    @retval VOID

**/
VOID
RWSbPmuIo8 (
    IN UINT8    Reg,
    IN UINT8    ResetBit8,
    IN UINT8    SetBit8
)
{
    UINT8       Buffer8;

    Buffer8 = ReadSbPmuIo8(Reg) & ~ResetBit8 | SetBit8;
    WriteSbPmuIo8(Reg, Buffer8);
}

/**

    @param Pin 

        UINT8
    @retval value return value

**/
UINT8
ReadSbGpioIo8 (
    IN UINT8    Pin
)
{
    return SbCspLibReadMem8(GetHygonSbAcpiMmio() + GPIO_BANK_BASE + (Pin << 2) + 2);
}

/**

    @param Pin 
    @param Data 

    @retval VOID

**/
VOID
WriteSbGpioIo8 (
    IN UINT8    Pin,
    IN UINT8    Data
)
{
    SbCspLibWriteMem8(GetHygonSbAcpiMmio() + GPIO_BANK_BASE + (Pin << 2) + 2, Data);
}

/**

    @param Reg 
    @param ResetBit8 
    @param SetBit8 

    @retval VOID

**/
VOID
RWSbGpioIo8 (
    IN UINT8    Reg,
    IN UINT8    ResetBit8,
    IN UINT8    SetBit8
)
{
    UINT8       Buffer8;

    Buffer8 = ReadSbGpioIo8(Reg) & ~ResetBit8 | SetBit8;
    WriteSbGpioIo8(Reg, Buffer8);
}


/**

    @param Pin 
    @param Data 

    @retval VOID

**/
VOID
WriteSbGpioIo8S3 (
    IN UINT8    Pin,
    IN UINT8    Data
)
{
    SbCspLibWriteMem8S3(GetHygonSbAcpiMmio() + GPIO_BANK_BASE + (Pin << 2) + 2, Data);
}

/**

    @param Reg 
    @param ResetBit 
    @param SetBit 

    @retval VOID

**/
VOID
RWSbGpioIo8S3 (
    IN UINT8    Reg,
    IN UINT8    ResetBit,
    IN UINT8    SetBit
)
{
    UINT8       Buffer;

    Buffer = ReadSbGpioIo8(Reg) & ~ResetBit | SetBit;
    WriteSbGpioIo8S3(Reg, Buffer);
}


/**

    @param Reg 

        UINT8
    @retval value return value

**/
UINT8
ReadSbMisc8 (
    IN UINT8    Reg
  )
{
    return SbCspLibReadMem8(GetHygonSbAcpiMmioEx() + MISC_BASE + Reg);
}

/**

    @param Reg 

        UINT32
    @retval value return value

**/
UINT32
ReadSbMisc32 (
    IN UINT8    Reg
  )
{
    return SbCspLibReadMem32(GetHygonSbAcpiMmioEx() + MISC_BASE + Reg);
}
/**

    @param Reg 
    @param Data 

    @retval VOID

**/
VOID
WriteSbMisc8 (
    IN UINT8    Reg,
    IN UINT8    Data
  )
{
    SbCspLibWriteMem8(GetHygonSbAcpiMmioEx() + MISC_BASE + Reg, Data);
}

/**

    @param Reg 
    @param Data 

    @retval VOID

**/
VOID
WriteSbMisc32 (
    IN UINT8    Reg,
    IN UINT32   Data
  )
{
    SbCspLibWriteMem32(GetHygonSbAcpiMmioEx() + MISC_BASE + Reg, Data);
}

/**

    @param Reg 
    @param ResetBit 
    @param SetBit 

    @retval VOID

**/
VOID
RWSbMisc8 (
    IN UINT8    Reg,
    IN UINT8    ResetBit,
    IN UINT8    SetBit
  )
{
    UINT8       Buffer;

    Buffer = ReadSbMisc8(Reg) & ~ResetBit | SetBit;
    WriteSbMisc8(Reg, Buffer);
}

/**

    @param Reg 
    @param ResetBit 
    @param SetBit 

    @retval VOID

**/
VOID
RWSbMisc32 (
    IN UINT8    Reg,
    IN UINT32   ResetBit,
    IN UINT32   SetBit
  )
{
    UINT32      Buffer;

    Buffer = ReadSbMisc32(Reg) & ~ResetBit | SetBit;
    WriteSbMisc32(Reg, Buffer);
}

/**

    @param Reg 
    @param Data 

    @retval VOID

**/
VOID
WriteSbMisc8S3 (
    IN UINT8    Reg,
    IN UINT8    Data
  )
{
	SbCspLibWriteMem8S3(GetHygonSbAcpiMmioEx() + MISC_BASE + Reg, Data);
}

/**

    @param Reg 
    @param Data 

    @retval VOID

**/
VOID
WriteSbMisc32S3 (
    IN UINT8    Reg,
    IN UINT32   Data
  )
{
	SbCspLibWriteMem32S3(GetHygonSbAcpiMmioEx() + MISC_BASE + Reg, Data);
}

/**

    @param Reg 
    @param ResetBit 
    @param SetBit 

    @retval VOID

**/
VOID
RWSbMisc8S3 (
    IN UINT8    Reg,
    IN UINT8    ResetBit,
    IN UINT8    SetBit
  )
{
    UINT8       Buffer;

    Buffer = ReadSbMisc8(Reg) & ~ResetBit | SetBit;
    WriteSbMisc8S3(Reg, Buffer);
}

/**

    @param Reg 
    @param ResetBit 
    @param SetBit 

    @retval VOID

**/
VOID
RWSbMisc32S3 (
    IN UINT8    Reg,
    IN UINT32   ResetBit,
    IN UINT32   SetBit
  )
{
    UINT32      Buffer;

    Buffer = ReadSbMisc32(Reg) & ~ResetBit | SetBit;
    WriteSbMisc32S3(Reg, Buffer);
}

/**
    Use RTC index 0x0A UIP bit as a 1 second maker

    @param VOID

    @retval VOID

**/
VOID
WaitOneSecRTC(VOID)
{
    UINT8 NMI = (IoRead8(0x70) & 0x80);   //Read bit 7 (NMI setting).
    IoWrite8(0x70,0x0A | NMI);
    while ((IoRead8(0x71) & 0x80) == 0) {};
    while (IoRead8(0x71) & 0x80) {};

}

#ifdef SB_GENERIC_RUNTIME

/**

        
    @param Event 
    @param Context 

    @retval VOID

**/
VOID SBLibVirtualAddressChange (
    IN EFI_EVENT Event, IN VOID *Context
)
{
	pRS->ConvertPointer(0, (VOID**)&gSbMMIOBASE);
}

/**

    @param ImageHandle 
    @param SystemTable 

         
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
EFI_STATUS
EFIAPI
SBGenericRuntimeLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
	InitAmiRuntimeLib(ImageHandle, SystemTable, NULL, SBLibVirtualAddressChange);
	return EFI_SUCCESS;
}
#endif

//###############################################################
//###############################################################
//####
//#### Following libraries are EIP148016
//####
#if defined(AMI_CPM_LIB_WRAPPER_SUPPORT) && (AMI_CPM_LIB_WRAPPER_SUPPORT == 1)
#include <AmdCpmPkg/Library/Proc/Fch/AmdCpmFch.h>
VOID*
EFIAPI
AmiCpmFchWrapperConstructor (
		VOID
  );
#endif

typedef enum {
	HYGON_FCH_SMI_MMIO,
	HYGON_FCH_PMIO_MMIO,
	//HYGON_FCH_PMIO2_MMIO,
	HYGON_FCH_IOMUX_MMIO,
	HYGON_FCH_GPIO_MMIO,
	//HYGON_FCH_GPIOB0_MMIO,
	//HYGON_FCH_GPIOB1_MMIO,
	//HYGON_FCH_GPIOB2_MMIO,
	HYGON_FCH_MMIO_TYPE_MAX
} HYGON_FCH_MMIO_TYPE;


/**
    Get mmio base by type.

    @param Type - The type of SB mmio.

    @retval UINT32 mmio base

**/
UINT32
EFIAPI
HygonFchMmioBase (
  IN HYGON_FCH_MMIO_TYPE	Type
  )
{
	UINT32 	FchAcpiMmioBase = SB_MMIO_BASE;
	UINT32 	MmioBase, Offset;

	switch (Type) {
	case HYGON_FCH_SMI_MMIO :
		Offset = SMI_BASE;
		break;
	case HYGON_FCH_PMIO_MMIO :
		Offset = PMIO_BASE;
		break;
	case HYGON_FCH_IOMUX_MMIO :
		Offset = IOMUX_BASE;
		break;
	case HYGON_FCH_GPIO_MMIO :
		Offset = GPIO_BANK0_BASE;
		break;
	}
	MmioBase = FchAcpiMmioBase + Offset;
	return MmioBase;
}

/**
    Read bite from mmio.

    @param Type - The type of SB mmio.
    @param Register - register for read

    @retval UINT8 return value

**/
UINT8
EFIAPI
HygonFchMmioRead8 (
  IN HYGON_FCH_MMIO_TYPE	Type,
  IN UINT32		Register
  )
{
	UINT32 	FchMmioBase = HygonFchMmioBase(Type);

	return MmioRead8(FchMmioBase + Register);
}

/**
    Read double word from mmio.

    @param Type - The type of SB mmio.
    @param Register - register for read

    @retval UINT32 return value

**/
UINT32
EFIAPI
HygonFchMmioRead32 (
  IN HYGON_FCH_MMIO_TYPE	Type,
  IN UINT32		Register
  )
{
	UINT32 	FchMmioBase = HygonFchMmioBase(Type);

	return MmioRead32(FchMmioBase + Register);
}

/**
    Write bite to mmio.

    @param Type - The type of SB mmio.
    @param Register - register to write
    @param Data - data for write

    @retval UINT8

**/
UINT8
EFIAPI
HygonFchMmioWrite8 (
  IN HYGON_FCH_MMIO_TYPE	Type,
  IN UINT32		Register,
  IN UINT8		Data
  )
{
	UINT32 	FchMmioBase = HygonFchMmioBase(Type);

	return MmioWrite8(FchMmioBase + Register, Data);
}

/**
    Write double word to mmio.

    @param Type - The type of SB mmio.
    @param Register - register to write
    @param Data - data for write

    @retval UINT8

**/
UINT32
EFIAPI
HygonFchMmioWrite32 (
  IN HYGON_FCH_MMIO_TYPE	Type,
  IN UINT32		Register,
  IN UINT32		Data
  )
{
	UINT32 	FchMmioBase = HygonFchMmioBase(Type);

	return MmioWrite32(FchMmioBase + Register, Data);
}

/**
    Or double word then write to mmio.

    @param Type - The type of SB mmio.
    @param Register - register to write
    @param OrData - or data

    @retval UINT8

**/
UINT32
EFIAPI
HygonFchMmioOr32 (
  IN HYGON_FCH_MMIO_TYPE	Type,
  IN UINT32		Register,
  IN UINT32		OrData
  )
{
	UINT32 	FchMmioBase = HygonFchMmioBase(Type);

	return MmioOr32(FchMmioBase + Register, OrData);
}

/**
    And double word then write to mmio.

    @param Type - The type of SB mmio.
    @param Register - register to write
    @param AndData - and data

    @retval UINT8

**/
UINT32
EFIAPI
HygonFchMmioAnd32 (
  IN HYGON_FCH_MMIO_TYPE	Type,
  IN UINT32		Register,
  IN UINT32		AndData
  )
{
	UINT32 	FchMmioBase = HygonFchMmioBase(Type);

	return MmioAnd32(FchMmioBase + Register, AndData);
}

/**
    And double word then or double word then write to mmio.

    @param Type - The type of SB mmio.
    @param Register - register to write
    @param AndData - and data
    @param OrData - or data

    @retval UINT8

**/
UINT32
EFIAPI
HygonFchMmioAndThenOr32 (
  IN HYGON_FCH_MMIO_TYPE	Type,
  IN UINT32		Register,
  IN UINT32		AndData,
  IN UINT32		OrData
  )
{
	UINT32 	FchMmioBase = HygonFchMmioBase(Type);

	return MmioAndThenOr32(FchMmioBase + Register, AndData, OrData);
}

/**
    And bite then or bite then write to mmio.

    @param Type - The type of SB mmio.
    @param Register - register to write
    @param AndData - and data
    @param OrData - or data

    @retval UINT8

**/
UINT8
EFIAPI
HygonFchMmioAndThenOr8 (
  IN 	  HYGON_FCH_MMIO_TYPE	  		Type,
  IN 	  UINT32					Register,
  IN      UINT8                     AndData,
  IN      UINT8                     OrData
  )
{
	UINT32 	FchMmioBase = HygonFchMmioBase(Type);

	return MmioAndThenOr8(FchMmioBase + Register, AndData , OrData);
}


/**
    And bite then or bite then write to mmio.

    @param Gpe0Type	SB_GPE0_TYPE, like TYPE_HOT_PLUG .. etc.
    @param PinNum - gpio pin

    @retval UINT32 GPE0 bit number

**/
UINT32
HygonFchGpioGetGpe0BitNum (
//###  IN VOID* This,
  IN SB_GPE0_TYPE Gpe0Type,
  IN UINT32 PinNum
  )
{
	UINT32 i;
	UINT32 Gpe0BitNum;

	// Find the GPE0 bit number.
	Gpe0BitNum = -1;
	i = 0;
	do {
		if (PinNum != -1 && PinNum == HygonSciMapTbl[i].PinNum) {
			Gpe0BitNum = (HygonFchMmioRead32(HYGON_FCH_SMI_MMIO, HygonSciMapTbl[i].Sci.Reg) >> HygonSciMapTbl[i].Sci.BitOffset) & 0x1F;
			break;
		}
		if (Gpe0Type != TYPE_GPE0_HYGON_END && Gpe0Type == HygonSciMapTbl[i].Gpe0Type) {
			Gpe0BitNum = (HygonFchMmioRead32(HYGON_FCH_SMI_MMIO, HygonSciMapTbl[i].Sci.Reg) >> HygonSciMapTbl[i].Sci.BitOffset) & 0x1F;
			break;
		}
	} while (HygonSciMapTbl[i].Gpe0Type == TYPE_GPE0_HYGON_END);

	return Gpe0BitNum;
}

//###############################################################
//###############################################################
//####
//#### The following libraries are EIP148016
//####

/**
    Enable or disable SB common functions.

    @param SbDevType - The type of SB device, refer to SB_DEV_TYPE
    @param Enable - TRUE: Enable / FALSE: Disable

    @retval VOID

**/
VOID SbEnableDisableFunctions (
    IN SB_DEV_TYPE  SbDevType,
    IN BOOLEAN      Enable
)
{
	switch (SbDevType) {
	case SB_DEV_FUNCTION_0:
		// HYGON not support.
		break;
	case SB_DEV_ADSP:
		// HYGON not support.
		break;
	case SB_DEV_SATA1:
		//###RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGDA, AccessWidth8, 0, FchSataMode); FchSataMode |= 0x01;
		if (Enable) {
			//###MmioOr8 (FchMmio + PMIO_BASE + FCH_PMIOA_REGDA, BIT0);
			HygonFchMmioOr32 (HYGON_FCH_PMIO_MMIO, FCH_PMIOA_REGDA, BIT0);
		} else {
			//###MmioAnd8 (FchMmio + PMIO_BASE + FCH_PMIOA_REGDA, ~BIT0);
			HygonFchMmioAnd32 (HYGON_FCH_PMIO_MMIO, FCH_PMIOA_REGDA, ~BIT0);
		}
		break;
	case SB_DEV_SMBUS:
		// HYGON not support.
		break;
		
	case SB_DEV_EHCI1:
		//###RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGEF, AccessWidth8, 0, UsbModeReg); UsbModeReg |= 0x02;
		if (Enable) {
			//###MmioOr8 (FchMmio + PMIO_BASE + FCH_PMIOA_REGEF, BIT1);
			HygonFchMmioOr32 (HYGON_FCH_PMIO_MMIO, FCH_PMIOA_REGEF, BIT1);
		} else {
			//###MmioAnd8 (FchMmio + PMIO_BASE + FCH_PMIOA_REGEF, ~BIT1);
			HygonFchMmioAnd32 (HYGON_FCH_PMIO_MMIO, FCH_PMIOA_REGEF, ~BIT1);
		}
		break;
	case SB_DEV_EHCI2:
		//###RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGEF, AccessWidth8, 0, UsbModeReg); UsbModeReg |= 0x08;
		if (Enable) {
			//###MmioOr8 (FchMmio + PMIO_BASE + FCH_PMIOA_REGEF, BIT3);
			HygonFchMmioOr32 (HYGON_FCH_PMIO_MMIO, FCH_PMIOA_REGEF, BIT3);
		} else {
			//###MmioAnd8 (FchMmio + PMIO_BASE + FCH_PMIOA_REGEF, ~BIT3);
			HygonFchMmioAnd32 (HYGON_FCH_PMIO_MMIO, FCH_PMIOA_REGEF, ~BIT3);
		}
		break;
	case SB_DEV_EHCI3:
		//###RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGEF, AccessWidth8, 0, UsbModeReg); UsbModeReg |= 0x20;
		if (Enable) {
			//###MmioOr8 (FchMmio + PMIO_BASE + FCH_PMIOA_REGEF, BIT5);
			HygonFchMmioOr32 (HYGON_FCH_PMIO_MMIO, FCH_PMIOA_REGEF, BIT5);
		} else {
			//###MmioAnd8 (FchMmio + PMIO_BASE + FCH_PMIOA_REGEF, ~BIT5);
			HygonFchMmioAnd32 (HYGON_FCH_PMIO_MMIO, FCH_PMIOA_REGEF, ~BIT5);
		}
		break;
	case SB_DEV_LPC_BRIDGE:
		break;
	case SB_DEV_PCI_EX_PORT1:
		// HYGON not support.
		break;
	case SB_DEV_PCI_EX_PORT2:
		// HYGON not support.
		break;
	case SB_DEV_PCI_EX_PORT3:
		// HYGON not support.
		break;
	case SB_DEV_PCI_EX_PORT4:
		// HYGON not support.
		break;
	case SB_DEV_PCI_EX_PORT5:
		// HYGON not support.
		break;
	case SB_DEV_PCI_EX_PORT6:
		// HYGON not support.
		break;
	case SB_DEV_PCI_EX_PORT7:
		// HYGON not support.
		break;
	case SB_DEV_PCI_EX_PORT8:
		// HYGON not support.
		break;
	case SB_DEV_THERMAL:
		//### <AAV> : check HYGON.
		break;
	case SB_DEV_SATA2:
		// HYGON not support.
		break;
	case SB_DEV_XHCI:
		//### <AAV> : check HYGON.
		break;
	case SB_DEV_LAN:
		// HYGON not support.
		break;
	default:
		break;
	}
}

/**
    Save or Restore All Hardware SMIs

    @param Save TRUE  : Save all HW SMIs
                FALSE : Restore saved HW SMIs

    @retval VOID

**/
VOID SbSaveRestoreAllHwSmi (
    IN BOOLEAN      Save
)
{
  UINT32 i = 0;
  UINT32 static SaveData[FCH_N_OF_SMI_CTRL_REG];

  if (Save) {
    for (i = 0; i < FCH_N_OF_SMI_CTRL_REG; i++) {
    	if (SmiControl[i].Mask == 0xFFFFFFFF) continue; // all reserved
    	SaveData[i] = HygonFchMmioRead32(HYGON_FCH_SMI_MMIO, SmiControl[i].Reg) & ~SmiControl[i].Mask;
    }
  } else {
    for (i = 0; i < FCH_N_OF_SMI_CTRL_REG; i++) {
    	if (SmiControl[i].Mask == 0xFFFFFFFF) continue; // all reserved
    	HygonFchMmioWrite32(HYGON_FCH_SMI_MMIO,SmiControl[i].Reg,
    			(HygonFchMmioRead32(HYGON_FCH_SMI_MMIO, SmiControl[i].Reg) & SmiControl[i].Mask) | SaveData[i]
    			);
    }
  }
}

/**
    Disable all HW SMIs

    @param VOID

    @retval VOID

**/
VOID SbDisableAllHwSmi (
    VOID
)
{
	UINT32 i = 0;

    for (i = 0; i < FCH_N_OF_SMI_CTRL_REG; i++) {
    	if (SmiControl[i].Mask == 0xFFFFFFFF) continue; // all reserved
    	HygonFchMmioWrite32(HYGON_FCH_SMI_MMIO,SmiControl[i].Reg,
    			(HygonFchMmioRead32(HYGON_FCH_SMI_MMIO, SmiControl[i].Reg) & SmiControl[i].Mask)
    			);
    }
}

/**
    Check NMI bit

    @param VOID

    @retval TRUE  : NMI is enabled.
            FALSE : NMI is disabled.
**/
BOOLEAN SbCheckNmiStatus (
    VOID
)
{
  if (IoRead8(0x70) & RTC_NMI_MASK) {
    // NMI is disabled.
    return FALSE;
  } else {
    // NMI is Enabled.
    return TRUE;
  }
}

/**
    Enable / Disable NMI Bit

    @param Enable - TRUE  : Enable NMI
                    FALSE : Disable NMI

    @retval TRUE  : NMI is enabled
            FALSE : NMI is disabled
**/
BOOLEAN SbEnableDisableNmi (
    IN BOOLEAN      Enable
)
{
  BOOLEAN IntState = CPULib_GetInterruptState();
  static UINT8   Data8;

  CPULib_DisableInterrupt();

  Data8 = IoRead8 (CMOS_ADDR_PORT);

  if (Enable)
    Data8 &= ~RTC_NMI_MASK;
  else {
    Data8 |= RTC_NMI_MASK;
  }
  IoWrite8 (CMOS_ADDR_PORT, Data8);

  if (IntState) CPULib_EnableInterrupt();

  return SbCheckNmiStatus();
}

/**
    Save / Restore NMI bit

    @param Save TRUE  : Save NMI
                FALSE : Restore NMI

    @retval VOID

**/
VOID SbSaveRestoreNmi (
    IN BOOLEAN      Save
)
{
  static  UINT8   NmiMask;
          BOOLEAN IntState = CPULib_GetInterruptState();
          UINT8   Data8;

  CPULib_DisableInterrupt();

  if (Save)
    // Save current NMI_EN.
    NmiMask = IoRead8 (CMOS_ADDR_PORT) & RTC_NMI_MASK;
  else {
    Data8 = IoRead8 (CMOS_ADDR_PORT) & ~(RTC_NMI_MASK);
    IoWrite8 (CMOS_ADDR_PORT, Data8 | NmiMask);
  }
  if (IntState) CPULib_EnableInterrupt();
}

/**
    Trigger SW SMI and adding extra I/O instructions around
    the SMI port write reduces the failure rate.

    @param SwSmi - The number of Software SMI

    @retval VOID

**/
VOID SbSwSmiTrigger (
    IN UINT8        SwSmi
)
{
  IoWrite8 (0x81, 0xEF);
  IoWrite8 (0x81, 0xEF);
  IoWrite8 (SW_SMI_IO_ADDRESS, SwSmi);
  IoWrite8 (0x81, 0xEF);
  IoWrite8 (0x81, 0xEF);
}

/**
    Trigger SW SMI and return the S/W SMI data from the S/W SMI data port.

    @param SwSmi The number of Software SMI
    @param Data Pointer to the data to be written to the S/W SMI data port.

    @return Data Pointer to the data read from the S/W SMI data port.

**/
VOID SbSwSmiIo (
    IN     UINT8    SwSmi,
    IN OUT UINT8    *Data
)
{
  IoWrite8 (SW_SMI_IO_ADDRESS + 1, *Data);
  SbSwSmiTrigger (SwSmi);
  *Data = IoRead8 (SW_SMI_IO_ADDRESS + 1);
}

/**
    Get data from S/W SMI I/O port and return the address of S/W SMI 
    I/O port.

    @param DataValue Pointer to the data buffer for receiving the S/W SMI
                     number read from S/W SMI I/O port.

    @return DataValue Pointer to the data buffer contained the S/W SMI
                      number read from S/W SMI I/O port.

**/
UINT16 SbGetSwSmi (
    IN OUT UINT32   *DataValue
)
{
  *DataValue = (UINT32)(IoRead8(SW_SMI_IO_ADDRESS));
  return SW_SMI_IO_ADDRESS;
}

/**
    Get Intel TCO2 Status address.(Intel Chipset Only)

    @param AcpiBaseAddr ACPI Base address, it is defined by PM_BASE_ADDRESS
                        normally.

    @return The address of TCO2 status register.
            0xFFFF - not supported.

**/
UINT16 SbGetTco2StsAddress (
    IN UINT16       AcpiBaseAddr
)
{
  // <AAV>  HYGON not support.
  return 0xFFFF;
}

/**
    Get Intel TCO2 Control address. (Intel Chipset Only)

    @param AcpiBaseAddr ACPI Base address, it is defined by PM_BASE_ADDRESS
                        normally.

    @retval The address of TCO2 control register.
            0xFFFF - not supported.

**/
UINT16 SbGetTco2CntAddress (
    IN UINT16       AcpiBaseAddr
)
{
  // <AAV>  HYGON not support.
  return 0xFFFF;
}

/**
    Set After G3 bit.

    @param Set
           TRUE  = Enable G3 Bit
           FALSE = Disable G3 Bit

    @retval VOID

**/
VOID SetAfterG3Bit (
    IN BOOLEAN      Set
)
{
	// Reference code : AcLossControl : AmdAgesaPkg/Proc/Fch/Common/FchLib.c
	if (Set) SET_IO8_PMIO(FCH_PMIOA_REG5B, BIT2);
	else RESET_IO8_PMIO(FCH_PMIOA_REG5B, BIT2);
}

/**
    Check After G3 bit.

    @param VOID

    @retval TRUE:  G3 bit is set
            FALSE: G3 bit is not set

**/
BOOLEAN SbCheckAfterG3 (
    VOID
)
{
    if (READ_IO8_PMIO(FCH_PMIOA_REG5B) & 0x0F)
        return FALSE;
    else
        return TRUE;
}

/**
    Disable LPC Super I/O Devices

    @param SioType - The type of SIO device, refer to SB_LPC_SIO_TYPE.

    @retval VOID

**/
VOID SbDisableLpcDevices (
    IN SB_LPC_SIO_TYPE  SioType
)
{
  UINT32 Data32;

  Data32 = READ_PCI32_SB (FCH_LPC_REG44);
  switch (SioType) {
    case SbComA:
      Data32 &= ~BIT6;
      break;
    case SbComB:
      Data32 &= ~BIT7;
      break;
    case SbLpt:
      Data32 &= ~BIT0;
      break;
    case SbFdd:
      Data32 &= ~BIT26;
      break;
    case SbGameL:
      Data32 &= ~BIT28;
      break;
    case SbGameH:
      //### <AAV> HYGON not support ###Data32 &= ~B_PCH_LPC_ENABLES_GAMEH_EN;
      break;
    case SbKbc:
      Data32 &= ~BIT29;
      break;
    case SbMc:
      Data32 &= ~BIT30;
      break;
    case SbCnf1:
      RESET_PCI32_SB(FCH_LPC_REG48, BIT0);
      break;
    case SbCnf2:
      RESET_PCI32_SB(FCH_LPC_REG48, BIT1);
      break;
    default:
      break;
  }
  WRITE_PCI32_SB (FCH_LPC_REG44, Data32);
}

/**
    Enable EHCI USB SMI

    @param VOID

    @retval VOID

**/
VOID SbEnableEhciSmi (
    VOID
)
{
	//
	// Reference code :
	// ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB0) |= BIT24;
	// ACPIMMIO32 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGED) |= (BIT4);
	HygonFchMmioOr32(HYGON_FCH_SMI_MMIO, FCH_SMI_REGB0, BIT24);
	HygonFchMmioOr32(HYGON_FCH_PMIO_MMIO, FCH_PMIOA_REGED, BIT4);
}

/**
    Disable EHCI USB SMI

    @param VOID

    @retval VOID

**/
VOID SbDisableEhciSmi (
    VOID
)
{
  /** PORTING REQUIRED **
  UINT16 PmBase;

  PmBase    = PcdGet16 (PcdAcpiIoPortBaseAddress);
  IoAnd32 (PmBase + R_PCH_SMI_EN, ~B_PCH_SMI_EN_LEGACY_USB2);
  IoWrite32 (PmBase + R_PCH_SMI_STS, B_PCH_SMI_STS_LEGACY_USB2);
  **/
	HygonFchMmioAndThenOr32(HYGON_FCH_SMI_MMIO, FCH_SMI_REGB0, ~BIT24, 0);
	HygonFchMmioAndThenOr32(HYGON_FCH_PMIO_MMIO, FCH_PMIOA_REGED, ~BIT4, 0);
}

/**
    Check power button pressed.

    @param VOID

    @retval TRUE:  Pressed
            FALSE: No Pressed

**/
BOOLEAN IsPowerButtonPressed (
    VOID
)
{
  UINT16 PmBase;

  PmBase    = PcdGet16 (PcdAcpiIoPortBaseAddress);

  if (IoRead16 (PmBase + ACPI_IOREG_PM1_STS) & BIT8) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
    Enable PCI PME bit.

    @param PciAddress The address got from PCI_LIB_ADDRESS macro in
                      PciLib.h
        #define PCI_LIB_ADDRESS(Bus,Device,Function,Register) \
                (((Register) & 0xfff) | (((Function) & 0x07) << 12) | \
                (((Device) & 0x1f) << 15) | (((Bus) & 0xff) << 20))


    @retval VOID

**/
VOID SbEnablePciPme (
    IN UINTN        PciAddress
)
{
  UINT16 CapPtr;
  UINTN  PcieAddress;

  PcieAddress = CSP_PCIE_CFG_ADDRESS(
                  (PciAddress >> 20) & 0xFF,
                  (PciAddress >> 15) & 0x1F,
                  (PciAddress >> 12) & 0x07,
                  0);  
  CapPtr = FindCapPtr (PcieAddress, 0x01);
  // CapPtr is found
  if (CapPtr != 0) {
    // Set PMEE.
    MmioOr16 (PcieAddress + CapPtr + 4, BIT08);
  }
}

/**
    Disable PCI PME bit.

    @param PciAddress The address got from PCI_LIB_ADDRESS macro in
                      PciLib.h
        #define PCI_LIB_ADDRESS(Bus,Device,Function,Register) \
                (((Register) & 0xfff) | (((Function) & 0x07) << 12) | \
                (((Device) & 0x1f) << 15) | (((Bus) & 0xff) << 20))

    @retval VOID

**/
VOID SbDisablePciPme (
    IN UINTN        PciAddress
)
{
  UINT16 CapPtr;
  UINTN  PcieAddress;

  PcieAddress = CSP_PCIE_CFG_ADDRESS(
                  (PciAddress >> 20) & 0xFF,
                  (PciAddress >> 15) & 0x1F,
                  (PciAddress >> 12) & 0x07,
                  0);  
  CapPtr = FindCapPtr (PcieAddress, 0x01);
  // CapPtr is found
  if (CapPtr != 0) {
    // Disable PMEE.
    MmioAnd16 (PcieAddress + CapPtr + 4, ~BIT08);
    // Clear PMES
    MmioWrite16 (PcieAddress + CapPtr + 4, BIT15);
  }
}

/**
    Program the related registers for WOL enabled.

    @param VOID

    @retval VOID

**/
VOID SbEnableWolPmConfg (
    VOID
)
{
	// <AAV> TBD : no on chip LAN in Mullins-FCH.
	//			The function depend on board layout.
}

/**
    Program the related registers for WOL disabled.

    @param VOID

    @retval VOID

**/
VOID SbDisableWolPmConfg (
    VOID
)
{
	// <AAV> TBD : no on chip LAN in Mullins-FCH.
	//			The function depend on board layout.
}

/**
    Get I/O Trap Info

    @param TrappedIoData Pointer to a 32bit data buffer for receiving the
                         data of I/O trap information.

    @return TrappedIoData Pointer to a 32bit data buffer contained the
                          data of I/O trap information.
            I/O Trap address

**/
UINT16 SbGetIoTrapInfo (
    IN OUT UINT32   *TrappedIoData
)
{
	// <AAV>
	//
	// HYGON South bridge doesn't support the register to get the date from trapped IO.
	//	Instead to get information from SMM data area.
	// REFERENCE :
	//		Preliminary BIOS and Kernel Developer's Guide (BKDG) for AMD Family 16h Models 30h-3Fh Processors
	// Section :
	//		2.4.8.2.5 SMM Save State
	// 		SMMFEC0 SMM IO Trap Offset
	//
	return  0xFFFF;
}

/**
    Get ACPI Base Address

    @param VOID

    @return The address of ACPI Base.

**/
UINT16 SbGetAcpiBaseAddress (
    VOID
)
{
  return PcdGet16 (PcdAcpiIoPortBaseAddress);
}

/**
    Get PM1 Control Register Offset

    @param VOID

    @return The offset of PM1 Control Register

**/
UINT16 SbGetPm1CntOffset (
    VOID
)
{
  return SbGetAcpiBaseAddress() + ACPI_IOREG_PM1_CNTL;
}

/**
    Get Intel RCBB Address (Intel Chipset Only)

    @param VOID

    @return The address of RCRB

**/
UINT32 SbGetRcrbAddress (
    VOID
)
{
  return 0;//###<AAV> HYGON not support ##PcdGet32 (PcdRcrbBaseAddress);
}

/**
    Check RTC power status

    @param VOID

    @retval TRUE:  Valid
            FALSE: Invalid

**/
BOOLEAN SbIsRtcPwrValid (
    VOID
)
{
	BOOLEAN		IsRtcPwrValid = TRUE;
	// PM_Reg 59: VRT_T2 set to 4ms x 0x10 value(0x10) AMD recommend.
	WRITE_MEM8 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG59, 0x10);

	IoWrite8(0x70, 0xD);
	if (!(IoRead8(0x71) & BIT7)) {
		IsRtcPwrValid = FALSE;
	}

	// PM_Reg 59: VRT_T2 set to default value(0xff)
	WRITE_MEM8 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG59, 0xFF);

	return IsRtcPwrValid;
}

/**
    Get the enable bit setting in GPE0 per Gpe0Type

    @param Gpe0Type	SB_GPE0_TYPE, like TYPE_HOT_PLUG .. etc.

    @retval TRUE = Enabled / FALSE = Disabled

**/
BOOLEAN SbGetGpe0En (
    IN SB_GPE0_TYPE Gpe0Type
)
{
	UINT32 	Gpe0BitNum;
	BOOLEAN	IsGpe0En;

	IsGpe0En = FALSE;

	//
	// To get which GPE0 bit that the event use for.
	//
	Gpe0BitNum = HygonFchGpioGetGpe0BitNum(Gpe0Type, -1);

	if (Gpe0BitNum != -1) {
		// Check the corresponding GPE0 bit.
		if ((HygonFchMmioRead32(HYGON_FCH_SMI_MMIO, FCH_SMI_REG04) >> Gpe0BitNum) & 1) {
			IsGpe0En = TRUE;
		}
	}

	return IsGpe0En;
}

/**
    Set the enable bit in GPE0 per Gpe0Type

    @param Gpe0Type	SB_GPE0_TYPE, like TYPE_HOT_PLUG .. etc.

    @retval VOID

**/
VOID SbSetGpe0En (
    IN SB_GPE0_TYPE Gpe0Type
)
{
	// <AAV> TBD :
	//		The Gpe0Type of HYGON GPE0 field is dynamic. Customer should assign the GPE0 number for each type.
	//
	// The following porting :
	//   We assume customer has assigned the GPE0 number for each type.
	UINT32 	Gpe0BitNum;

	//
	// To get which GPE0 bit that the event use for.
	//
	Gpe0BitNum = HygonFchGpioGetGpe0BitNum(Gpe0Type, -1);

	// Set the corresponding GPE0 bit.
	if (Gpe0BitNum != -1) HygonFchMmioOr32(HYGON_FCH_SMI_MMIO, FCH_SMI_REG04, 1 << Gpe0BitNum);
}

/**
    Reset the enable bit in GPE0 per Gpe0Type

    @param Gpe0Type	SB_GPE0_TYPE, like TYPE_HOT_PLUG .. etc.

    @retval VOID

**/
VOID SbResetGpe0En (
    IN SB_GPE0_TYPE Gpe0Type
)
{
	// <AAV> TBD :
	//		The Gpe0Type of HYGON GPE0 field is dynamic. Customer should assign the GPE0 number for each type.
	//
	// The following porting :
	//   We assume customer has assigned the GPE0 number for each type.
	UINT32 	Gpe0BitNum;

	//
	// To get which GPE0 bit that the event use for.
	//
	Gpe0BitNum = HygonFchGpioGetGpe0BitNum(Gpe0Type, -1);

	// Clear the corresponding GPE0 bit.
	if (Gpe0BitNum != -1) HygonFchMmioAnd32(HYGON_FCH_SMI_MMIO, FCH_SMI_REG04, ~(UINT32)(1 << Gpe0BitNum));
}

/**
    Save/Restore/ClearAll GPE0 register per Operation

    @param Operation	GPE0_EN_OP. GPE0_EN_OP_SAVE, GPE0_EN_OP_RESTORE,
                      GPE0_EN_OP_CLEAR_ALL.

    @retval VOID

**/
VOID SbGpe0Operation (
    IN GPE0_EN_OP   Operation
)
{
	// <AAV> TBD :
	//		The Gpe0Type of HYGON GPE0 field is dynamic. Customer should assign the GPE0 number for each type.
	//
	// The following porting :
	//   We assume customer has assigned the GPE0 number for each type.
	UINT32 static Gpe0Save = 0;

	switch (Operation) {
	case GPE0_EN_OP_SAVE:
		Gpe0Save = READ_IO32_PM (ACPI_IOREG_GPE0_EN);
		break;
	case GPE0_EN_OP_RESTORE:
		WRITE_IO32_PM (ACPI_IOREG_GPE0_EN, Gpe0Save);
		break;
	case GPE0_EN_OP_CLEAR_ALL:
		WRITE_IO32_PM (ACPI_IOREG_GPE0_EN, 0);
		break;
	}
}

/**
    Get the status bit setting in GPE0 per GPE0 Type

    @param Gpe0Type	SB_GPE0_TYPE, like TYPE_HOT_PLUG .. etc.

    @retval TRUE = Set / FALSE= Clear

**/
BOOLEAN SbGetGpe0Sts (
    IN SB_GPE0_TYPE Gpe0Type
)
{

	// <AAV> TBD :
	//		The Gpe0Type of HYGON GPE0 field is dynamic. Customer should assign the GPE0 number for each type.
	//
	// The following porting :
	//   We assume customer has assigned the GPE0 number for each type.
	UINT32 	Gpe0BitNum;
	BOOLEAN	IsGpe0En;

	IsGpe0En = FALSE;

	//
	// To get which GPE0 bit that the event use for.
	//
	Gpe0BitNum = HygonFchGpioGetGpe0BitNum(Gpe0Type, -1);

	if (Gpe0BitNum != -1) {
		// Get the corresponding GPE0 status bit.
		if ((HygonFchMmioRead32(HYGON_FCH_SMI_MMIO,FCH_SMI_REG00) >> Gpe0BitNum) & 1) {
			IsGpe0En = TRUE;
		}
	}

	return IsGpe0En;
}

/**
    Clear the status bit in GPE0 per GPE0 Type

    @param Gpe0Type	SB_GPE0_TYPE, like TYPE_HOT_PLUG .. etc.

    @retval VOID 

**/
VOID SbClearGpe0Sts (
    IN SB_GPE0_TYPE Gpe0Type
)
{
	// <AAV> TBD :
	//		The Gpe0Type of HYGON GPE0 field is dynamic. Customer should assign the GPE0 number for each type.
	//
	// The following porting :
	//   We assume customer has assigned the GPE0 number for each type.
	UINT32 	Gpe0BitNum;

	//
	// To get which GPE0 bit that the event use for.
	//
	Gpe0BitNum = HygonFchGpioGetGpe0BitNum(Gpe0Type, -1);

	// Clear the corresponding GPE0 status bit.
	if (Gpe0BitNum != -1) HygonFchMmioAnd32(HYGON_FCH_SMI_MMIO, FCH_SMI_REG00, ~(UINT32)(1 << Gpe0BitNum));
}

/**
    Set GPI Pin Enable bit in Gpe0

    @param Gpio - Define the group and Pin# of the GPIO

    @retval VOID

**/
VOID SbSetGpe0GpinEn (
    IN AMI_OEM_GPIO Gpio
)
{
	UINT8	PinNumber = Gpio.PinNum;
	// <AAV> TBD :
	//		The Gpe0Type of HYGON GPE0 field is dynamic. Customer should assign the GPE0 number for each type.
	//
	// The following porting :
	//   customer need to know the PinNum of (GEVENT0~GEVENT23)
	UINT8	IoMuxEn = IoMuxSelect[PinNumber];

	// IoMux select GPIO.
	HygonFchMmioAndThenOr8(HYGON_FCH_IOMUX_MMIO, PinNumber, 0xFC , IoMuxEn);

	// Clear Bit23 OutputEnable
	HygonFchMmioAndThenOr32 (HYGON_FCH_GPIO_MMIO, (PinNumber << 2), ~BIT23, 0);
}

/**
    Get GPI Pin Enable bit setting in Gpe0

    @param Gpio - Define the group and Pin# of the GPIO

    @retval TRUE = Enabled / FALSE= Disabled

**/
BOOLEAN SbGetGpe0GpinEn (
    IN AMI_OEM_GPIO Gpio
)
{
	UINT8	PinNumber = Gpio.PinNum;
	// <AAV> TBD :
	//		The Gpe0Type of HYGON GPE0 field is dynamic. Customer should assign the GPE0 number for each type.
	//
	// The following porting :
	//   customer need to know the PinNum of (GEVENT0~GEVENT23)
	//UINT8	IoMuxEn = IoMuxSelect[PinNumber];
	BOOLEAN IsGpi = FALSE;

	// IoMux select GPIO.
	//####MmioAndThenOr8(IoMuxMmio + Pin, 0xFC , IoMuxEn);
	// Check Bit23 OutputEnable
	if ((HygonFchMmioRead32 (HYGON_FCH_GPIO_MMIO, (PinNumber << 2)) & BIT23) == 0) IsGpi = TRUE;

	return IsGpi;
}

/**
    Reset GPI Pin Enable bit in Gpe0

    @param Gpio - Define the group and Pin# of the GPIO

    @retval VOID

**/
VOID SbResetGpe0GpinEn (
    IN AMI_OEM_GPIO Gpio
)
{
	UINT8	PinNumber = Gpio.PinNum;
	// <AAV> TBD :
	//		The Gpe0Type of HYGON GPE0 field is dynamic. Customer should assign the GPE0 number for each type.
	//
	// The following porting :
	//   customer need to know the PinNum of (GEVENT0~GEVENT23)
	//
	//   HYGON doesn't Gpi enable bit. we turn on GPO instead.
	UINT8	IoMuxEn = IoMuxSelect[PinNumber];

	// IoMux select GPIO.
	HygonFchMmioAndThenOr8(HYGON_FCH_IOMUX_MMIO, PinNumber, 0xFC , IoMuxEn);
	// Clear Bit23 OutputEnable
	HygonFchMmioAndThenOr32 (HYGON_FCH_GPIO_MMIO, (PinNumber << 2), ~BIT23, 1);
}

/**
    Get GPI Pin Status bit setting in Gpe0

    @param Gpio - Define the group and Pin# of the GPIO

    @retval VOID

**/
BOOLEAN SbGetGpe0GpinSts (
    IN AMI_OEM_GPIO Gpio
)
{
	UINT8	PinNumber = Gpio.PinNum;
	// <AAV> TBD :
	//		The Gpe0Type of HYGON GPE0 field is dynamic. Customer should assign the GPE0 number for each type.
	//
	// The following porting :
	//   Customer need to know corresponding PinNum for GEVENT. (GEVENT0~GEVENT23)
	//
	UINT32 	Gpe0BitNum;
	BOOLEAN	IsGpe0En;

	IsGpe0En = FALSE;

	// Find the GPE0 bit number.
	Gpe0BitNum = HygonFchGpioGetGpe0BitNum(TYPE_GPE0_HYGON_END, PinNumber);

	if (Gpe0BitNum != -1) {
		// Get the corresponding GPE0 status bit.
		if ((HygonFchMmioRead32(HYGON_FCH_SMI_MMIO,FCH_SMI_REG00) >> Gpe0BitNum) & 1) {
			IsGpe0En = TRUE;
		}
	}

	return IsGpe0En;
}

/**
    Clear GPI Pin Status bit in Gpe0

    @param Gpio - Define the group and Pin# of the GPIO

    @retval VOID

**/
VOID SbClearGpe0GpinSts (
    IN AMI_OEM_GPIO Gpio
)
{
	UINT8	PinNumber = Gpio.PinNum;
	// <AAV> TBD :
	//		The Gpe0Type of HYGON GPE0 field is dynamic. Customer should assign the GPE0 number for each type.
	//
	// The following porting :
	//   Customer need to know corresponding PinNum for GEVENT. (GEVENT0~GEVENT23)
	//
	UINT32 	Gpe0BitNum;
	//BOOLEAN	IsGpe0En;

	//IsGpe0En = FALSE;

	// Find the GPE0 bit number.
	Gpe0BitNum = HygonFchGpioGetGpe0BitNum(TYPE_GPE0_HYGON_END, PinNumber);

	if (Gpe0BitNum != -1) {
		// Get the corresponding GPE0 status bit.
		HygonFchMmioOr32(HYGON_FCH_SMI_MMIO,FCH_SMI_REG00, 1 << Gpe0BitNum);
	}
}

/**
    Get GPIO Use Setting

    @param Gpio - Define the group and Pin# of the GPIO

    @retval TRUE = GPIO MODE / FALSE= NATIVE MODE

**/
BOOLEAN SbGetGpioUseSel (
    IN AMI_OEM_GPIO Gpio
)
{
	UINT8	PinNumber = Gpio.PinNum;
	BOOLEAN IsGpio = FALSE;
	UINT8	IoMux;

	IoMux = HygonFchMmioRead8(HYGON_FCH_IOMUX_MMIO, PinNumber) & 3; // Bit[0:1]

	if (IoMux == IoMuxSelect[PinNumber]) {
		IsGpio = TRUE;
	}

	return IsGpio;
}

/**
    Program GPIO Use Setting

    @param Gpio - Define the group and Pin# of the GPIO
    @param GpioMode	- TRUE = GPIO MODE / FALSE= NATIVE MODE
    @param MulFunc - While this pin is a multi-function pin and GpioMode
                     is Native Mode, BIOS will refer to MulFunc to
                     determine which native function this pin will be
                     used.

    @retval VOID

**/
VOID SbProgGpioUseSel (
    IN AMI_OEM_GPIO Gpio,
    IN BOOLEAN      GpioMode,
    IN UINT8		MulFunc
)
{
	UINT8	PinNumber = Gpio.PinNum;
	if (GpioMode) {
		HygonFchMmioWrite8(HYGON_FCH_IOMUX_MMIO, PinNumber, IoMuxSelect[PinNumber]);
	} else {
		HygonFchMmioWrite8(HYGON_FCH_IOMUX_MMIO, PinNumber, MulFunc);
	}
}

/**
    Get GPIO I/O Setting

    @param Gpio - Define the group and Pin# of the GPIO

    @retval TRUE : Input Mode / FALSE : Output Mode

**/
BOOLEAN SbGetGpioIoSel (
    IN AMI_OEM_GPIO Gpio
)
{
	UINT8	PinNumber = Gpio.PinNum;
	BOOLEAN IsInputMode = FALSE;

	// Bit23 OutputEnable
	if (!(HygonFchMmioRead32 (HYGON_FCH_GPIO_MMIO, (PinNumber << 2)) & BIT23)) {
		IsInputMode = TRUE;
	}

	return IsInputMode;
}

/**
    Set GPIO I/O Setting

    @param Gpio - Define the group and Pin# of the GPIO
    @param InputMode - TRUE : Input Mode / FALSE : Output Mode

    @retval VOID

**/
VOID SbSetGpioIoSel (
    IN AMI_OEM_GPIO Gpio,
    IN BOOLEAN      InputMode
)
{
	UINT8	PinNumber = Gpio.PinNum;
	if (InputMode) {
		HygonFchMmioAnd32 (HYGON_FCH_GPIO_MMIO, (PinNumber << 2), ~BIT23);
	} else {
		HygonFchMmioOr32 (HYGON_FCH_GPIO_MMIO, (PinNumber << 2), BIT23);
	}
}

/**
    Get GPIO Level Setting

    @param Gpio - Define the group and Pin# of the GPIO

    @retval TRUE: High level / FALSE: Low level

**/
BOOLEAN SbGetGpioLvlSel (
    IN AMI_OEM_GPIO Gpio
)
{
	UINT8	PinNumber = Gpio.PinNum;
	BOOLEAN IsLevelHigh = FALSE;
	UINT32	GpioContext = HygonFchMmioRead32 (HYGON_FCH_GPIO_MMIO, (PinNumber << 2));
	UINT8	Level;

	if (GpioContext & BIT23) {
		// if GPO return ActiveLevel.

		// Bits Definition   	Bits Definition
		// 00b Active high 		10b Active on both of edges if LevelTrig=0
		// 01b Active Low 		11b Reserved
		Level = (GpioContext >> 9) & 3;
	} else {
		// if GPI return PinSts.
		Level = (GpioContext >> 16) & 1;
	}

	if (Level == 0) {
		IsLevelHigh = TRUE;
	} else if (Level == 1) {
		IsLevelHigh = FALSE;
	}
	// else if <AAV> How about edge trigger.

	return IsLevelHigh;
}

/**
    Set GPIO Level

    @param Gpio - Define the group and Pin# of the GPIO
    @param High - TRUE: Set to High level / FALSE: Set to Low level

    @retval VOID

**/
VOID SbSetGpioLvlSel (
    IN AMI_OEM_GPIO Gpio,
    IN BOOLEAN      High
)
{
	UINT8	PinNumber = Gpio.PinNum;
	//BOOLEAN IsLevelHigh = FALSE;
	UINT8	Level;

	// Bits Definition   	Bits Definition
	// 00b Active high 		10b Active on both of edges if LevelTrig=0
	// 01b Active Low 		11b Reserved
	if (High) {
		Level = 0;
	} else {
		Level = 1;
	}

	HygonFchMmioAndThenOr32 (HYGON_FCH_GPIO_MMIO, (PinNumber << 2), ~(BIT9 + BIT10), Level << 9);
}

/**
    Read Alternate GPI SMI Status Register

    @param VOID

    @return The value read from Alternate GPI SMI Status Register

**/
UINT32 SbReadAltGpiSmiSts ( // <AAV> extend the type to UINT32 for HYGON
    VOID
)
{
	return HygonFchMmioRead32(HYGON_FCH_SMI_MMIO, FCH_SMI_REG10);
}

/**
    Clear Alternate GPI SMI Status

    @param Gpio - Define the group and Pin# of the GPIO

    @retval VOID

**/
VOID SbClearAltGpiSmiSts (
    IN AMI_OEM_GPIO Gpio
)
{
	UINT8	PinNumber = Gpio.PinNum;
	// The following porting :
	//   Currently, We only support HYGON-GEVENT as Alt Gpi Smi.
	//   Customer need to know corresponding PinNum for GEVENT. (GEVENT0~GEVENT23)
	UINT32 Gpe0BitNum;

	// Find the GPE0 bit number.
	Gpe0BitNum = HygonFchGpioGetGpe0BitNum(TYPE_GPE0_HYGON_END, PinNumber);

	if (Gpe0BitNum != -1) {
		HygonFchMmioOr32(HYGON_FCH_SMI_MMIO,FCH_SMI_REG10, 1 << Gpe0BitNum);
	}
}

/**
    Program Alternate GPI SMI Register

    @param Gpio - Define the group and Pin# of the GPIO
    @param Set - TRUE: Set / FALSE: Clear corresponding Alternate GPI SMI
                 Enable bit.

    @retval VOID

**/
VOID SbProgramAltGpiSmi (
    IN AMI_OEM_GPIO Gpio,
    IN BOOLEAN      Set
)
{
	UINT8	PinNumber = Gpio.PinNum;
	// The following porting :
	//   Currently, We only support HYGON-GEVENT as Alt Gpi Smi.
	//   Customer need to know corresponding PinNum for GEVENT. (GEVENT0~GEVENT23)
	UINT32 Gpe0BitNum;

	// Find the GPE0 bit number.
	Gpe0BitNum = HygonFchGpioGetGpe0BitNum(TYPE_GPE0_HYGON_END, PinNumber);

	if (Gpe0BitNum != -1) {
		if (Set) HygonFchMmioOr32(HYGON_FCH_SMI_MMIO,FCH_SMI_REG14, 1 << Gpe0BitNum);
		else HygonFchMmioOr32(HYGON_FCH_SMI_MMIO,FCH_SMI_REG14, ~(1 << Gpe0BitNum));
	}
}

/**
    Program GPIO Rout

    @param Gpio - Define the group and Pin# of the GPIO
    @param Mode - NO_EFFECT/SMI_MODE/SCI_MODE/NMI_MODE

    @retval VOID

**/
VOID SbProgramGpioRout (
    IN AMI_OEM_GPIO Gpio,
    IN UINT8        Mode
)
{
	UINT8	PinNumber = Gpio.PinNum;
	// The following porting :
	//   Currently, We only support HYGON-GEVENT as Alt Gpi Smi.
	//   Customer need to know corresponding PinNum for GEVENT. (GEVENT0~GEVENT23)
	UINT32 SmiCtrlMode;
	UINT32 i;

	switch (Mode) {
	case 0:
		SmiCtrlMode = 0;
		break;
	case 1:
		SmiCtrlMode = 1;
		break;
	case 2:
		// <AAV>
		//	 No SCI_MODE support. The Gpe0Type of HYGON GPE0 field is dynamic. Customer should assign the GPE0 number in SciMap for each Pin.
		SmiCtrlMode = 0; // disable : means the SCI depend on SciMap register.
		//##return;
		break;
	case 3:
		SmiCtrlMode = 2;
		break;
	}

	i = 0;
	do {
		if (HygonSciMapTbl[i].PinNum == PinNumber) {
			HygonFchMmioAndThenOr32(HYGON_FCH_SMI_MMIO, HygonSciMapTbl[i].Smi.Reg, ~(3 << HygonSciMapTbl[i].Smi.BitOffset),  SmiCtrlMode);
		}
	} while (HygonSciMapTbl[i].Gpe0Type != TYPE_GPE0_HYGON_END);

}


/**
    Program GPIO Register

    @param Gpio - Define the group and Pin# of the GPIO
    @param RegType - The register type which is going to read
    @param AndData - The value to AND with read value from the GPIO register.
    					(HYGON chip)if HYGON CPM exist the first word be determined as AMD_CPM_GPIO_SETTING.
    @param OrData - The value to OR with the result of the AND operation.
    					(HYGON chip)if HYGON CPM doesn't exist this double word should be ignored.

    @retval VOID

**/
VOID SbProgramGpioRegister (
    IN AMI_OEM_GPIO   Gpio,
    IN GPIO_REG_TYPE  RegType,
    IN UINT32         AndData,
    IN UINT32         OrData
)
{
	UINT8	PinNumber = Gpio.PinNum;
	UINT32	Data = 0;

	// <AAV> Check int and reserve pin later.
	//###if ((PinNum << 2) >= 0xB8 || (PinNum << 2) <= 0xF8) return 0xFFFFFFFF;

#if defined(AMI_CPM_LIB_WRAPPER_SUPPORT) && (AMI_CPM_LIB_WRAPPER_SUPPORT == 1)
	Data = (UINT16)AndData; // the UIN16 refer to AMD_CPM_GPIO_SETTING.
	CpmSetGpio(AmiCpmFchWrapperConstructor(), (UINT16)PinNumber, (UINT16)Data);
#else
	switch (RegType) {
	case HYGON_GPIO:
		HygonFchMmioAndThenOr32 (HYGON_FCH_GPIO_MMIO, (PinNumber << 2), AndData, OrData);
		break;
	case HYGON_IOMUX:
		HygonFchMmioAndThenOr8(HYGON_FCH_IOMUX_MMIO, PinNumber, (UINT8)AndData, (UINT8)OrData);
		break;
	default:
		// EFI_UNSUPPORTED
		break;
	}
#endif
}

/**
    Read GPIO Register

    @param Gpio - Define the group and Pin# of the GPIO
    @param RegType - The register type which is going to read

    @retval 0xFFFFFFFF - Failed to read the register.
    @retval Others - The data read from the register. (HYGON chip) if CPM exist it is UINT8.

**/
UINT32 SbReadGpioRegister (
    IN AMI_OEM_GPIO   Gpio,
    IN GPIO_REG_TYPE  RegType
)
{
	UINT8	PinNumber = Gpio.PinNum;
	UINT32	Data = 0xFFFFFFFF;

	// <AAV> Check int and reserve pin later.
	//###if ((PinNum << 2) >= 0xB8 || (PinNum << 2) <= 0xF8) return 0xFFFFFFFF;

#if defined(AMI_CPM_LIB_WRAPPER_SUPPORT) && (AMI_CPM_LIB_WRAPPER_SUPPORT == 1)
	Data = (UINT32)CpmGetGpio(AmiCpmFchWrapperConstructor(), (UINT16)PinNumber);
#else
	switch (RegType) {
	case HYGON_GPIO:
		Data = HygonFchMmioRead32 (HYGON_FCH_GPIO_MMIO, (PinNumber << 2));
		break;
	case HYGON_IOMUX:
		Data = HygonFchMmioRead32 (HYGON_FCH_IOMUX_MMIO, PinNumber);
		break;
	default:
		// EFI_UNSUPPORTED
		break;
	}
#endif

  return Data;
}

/**
    Get South Bridge Miscellaneous BIT Status

    @param SbMiscType   Please check SB_MISC_TYPE for the details
    @param BitStatus    The value of the Miscellaneous BIT

    @retval EFI_UNSUPPORTED   This Miscellaneous BIT is not supported
    @retval EFI_SUCCESS       Success to get the value of the Miscellaneous BIT

**/
EFI_STATUS SbGetMiscBitStatus(
    IN SB_MISC_TYPE         SbMiscType,
    IN OUT UINT8            *BitStatus
)
{
	EFI_STATUS	Status = EFI_UNSUPPORTED;
    // Please check the datasheet of the chipset for the register to know
    // how to program the value of the Miscellaneous BIT indicated by SbMiscType.
    // If the type is not supported by the chip, please return EFI_UNSUPPORTED.
    switch (SbMiscType) {
      case CPU_THERMAL_TRIP_STATUS:
    	  break;

      case AFTERG3_EN:
    	  if (READ_IO8_PMIO(FCH_PMIOA_REG5B) & BIT2) {
    		  *BitStatus = 1;
    	  } else {
    		  *BitStatus = 0;
    	  }
    	  Status = EFI_SUCCESS;

      case PWR_FLR:
		  #ifndef FCH_PMxC0_ResetStatus_Mask // Next HYGON AGESA reference code will provide this token. define it here for the moment.
				#define FCH_PMxC0_ResetStatus_Mask       0x3fff0000ul // Next HYGON AGESA reference code will provide this token. define it here for the moment.
		  #endif
    	  // Refer to HYGON-CZ reference code (FchInitResetPowerG3Reset). It might be rolled in CZL.
    	  if (HygonFchMmioRead32(HYGON_FCH_PMIO_MMIO, 0xC0) & FCH_PMxC0_ResetStatus_Mask) {
    		  *BitStatus = 1;
    	  } else {
    		  *BitStatus = 0;
    	  }
    	  Status = EFI_SUCCESS;
    	  break;

      default: 
    	  break;
    }

    return Status;
}

/**
    Program South Bridge Miscellaneous BIT

    @param SbMiscType   Please check SB_MISC_TYPE for the details
    @param Set          Set/Clear the value of the Miscellaneous BIT

    @retval EFI_UNSUPPORTED   This Miscellaneous BIT is not supported
    @retval EFI_SUCCESS       Success to Set/Clear the value of the Miscellaneous BIT

**/
EFI_STATUS SbProgramMiscBit(
    IN SB_MISC_TYPE         SbMiscType,
    IN BOOLEAN              Set
)
{
	EFI_STATUS	Status = EFI_UNSUPPORTED;
    // Please check the datasheet of the chipset for the register to know
    // how to program the value of the Miscellaneous BIT indicated by SbMiscType.
    // If the type is not supported by the chip, please return EFI_UNSUPPORTED.
    switch (SbMiscType) {
      case CPU_THERMAL_TRIP_STATUS:
    	  break;

      case AFTERG3_EN:
    	  SetAfterG3Bit(Set);
    	  Status = EFI_SUCCESS;

      case PWR_FLR:
    	  // Clear whole status (Set to Clear). Refer to HYGON-CZ reference code (FchInitResetPowerG3Reset). It might be rolled in CZL.
    	  if (Set) {
    		  HygonFchMmioOr32(HYGON_FCH_PMIO_MMIO, 0xC0, FCH_PMxC0_ResetStatus_Mask);
    	  }
    	  Status = EFI_SUCCESS;
    	  break;

      default: 
    	  break;
    }

    return Status;
}

/**
    Check if any USB port is routed to XHCI controller

    @param VOID

    @return TRUE  There is
    @return FALSE There is not

**/
BOOLEAN SbIsXhciRouting ( 
    VOID 
)
{
	BOOLEAN	IsXhciRouting = FALSE;
	UINT32	Value;

	Value = READ_PCI32_XHCI0(0);
	//if (Value == ((FCH_USB_XHCI_DID << 16) | AMD_FCH_VID)) {
	if ( (Value == ((FCH_USB_XHCI_DID << 16) | AMD_VID)) || 
		 (Value == ((FCH_USB_XHCI_DID << 16) | HYGON_VID))){

		IsXhciRouting = TRUE;
	}

    return  IsXhciRouting;
}

/**
    Get the Mac Address of the LAN inside the chipset

    @param  MacAddress        Pointer to the buffer used to store the Mac Address

    @retval EFI_UNSUPPORTED   This function is not supported
    @retval EFI_SUCCESS       Success to get the Mac Address

**/
EFI_STATUS SbGetChipLanMacAddress ( 
    IN OUT UINT8            *MacAddress
)
{
/**** PORTING REQUIRED ****
    Please check the datasheet of the chipset for the register to know
    how to get the Mac Address of the LAN inside the chipset.
    For Intel chip, you also can get MAC address from Gbe Region of 
    the flash.
    If it is not supported by the chip or there is no LAN inside the 
    chip, please return EFI_UNSUPPORTED.
**** PORTING REQUIRED ****/
    return EFI_UNSUPPORTED;
}

/**
    Read SPI Descriptor

    @param  SectionSelect     Data written to Flash Descriptor Section Select bits
    @param  SectionIndex      Data written to Flash Descriptor Section Index bits
    @param  *SectionData      Pointer to Flash Descriptor Section Data

    @retval EFI_UNSUPPORTED   This function is not supported
    @retval EFI_SUCCESS       Success to read SPI Descriptor
    @retval EFI_DEVICE_ERROR  Failed to read SPI Descriptor
    @retval EFI_NOT_FOUND     Invalid Flash Valid Signature

**/
EFI_STATUS ReadSpiDescriptor (
    IN UINT8                SectionSelect,
    IN UINT8                SectionIndex,
    OUT UINT32              *SectionData
)
{
// HYGON SPI controller didn't provide SPI Descriptor and just return EFI_UNSUPPORTED for this routine. 
    return EFI_UNSUPPORTED;
}

#if defined(AMI_COMPATIBILITY_PKG_VERSION)&&(AMI_COMPATIBILITY_PKG_VERSION > 18)
#if KBC_SUPPORT
/**
    This function detects if the Ctrl + Home key press has been made.
    Is function is only included if KBC_SUPPORT is enabled.  
    It enables the keyboard checks for the Ctrl + Home key press and
     then disables the keyboard

    @param PeiServices pointer to a pointer to the PEI Services Table.

    @retval TRUE - Ctrl + Home was pressed
    @retval FALSE  - Ctrl + Home not detected

    @note  Please make sure the following items have been done before
           calling IsCtrlHomePressed.
        - The decode of Port 60/64 has been enabled.
        - KBC controller has been enabled in SIO/EC device.
**/

BOOLEAN IsCtrlHomePressed (
    EFI_PEI_SERVICES **PeiServices
)
{
  UINT8               Index;
  UINT8               KeyValue = 0;
  UINT8               KeyScan = 0;
  UINT8               LoopCount = CTRL_HOME_CHECK_LOOP;
  UINT8               KbcSts = 0;
  UINT8               KbcSts1 = 0;
  UINT8               KbcSts2 = 0;
  //BOOLEAN             KbcOBF = TRUE;
  BOOLEAN             KbcIBF = TRUE;
  BOOLEAN             KbcIBF1 = TRUE;
  UINT8               KbcRes = 0;

  //
  // Enable KBD
  //
  KbcSts = IoRead8 (0x64);
  //
  // Check KeyBoard Presence
  //
  if (KbcSts != 0xFF) {
    //
    // Save the Sys Bit for later use
    //
    KbcRes = KbcSts & 0x04;
    IoWrite8 (0x64, 0x60);  
    //
    // Wait for Input Buffer Empty
    //
    while(KbcIBF) {
      KbcIBF = FALSE;
      KbcSts1 = IoRead8 (0x64);
      if ((KbcSts1 & 0x02) == 0x02) {
        KbcIBF = TRUE;
      }
    }
    //
    // Keyboard enable, Mouse disable
    //
    IoWrite8 (0x60, KbcRes | 0x61);          
    //  
    // Wait for Input Buffer Empty
    //
    while(KbcIBF1) {
      KbcIBF1 = FALSE;
      KbcSts2 = IoRead8 (0x64);
      if ((KbcSts2 & 0x02) == 0x02) {
        KbcIBF1 = TRUE;
      }
    } 
  }  
  //
  // Loop for Key scan
  //
  for (Index = 0 ; Index < LoopCount ; Index++) {
    //
    // Wait at least for 6ms
    //
//debug_ram disable for simnow    CountTime(6000, PM_BASE_ADDRESS);

    //
    // Read Key stroke
    //
    if (IoRead8 (0x64) & 0x01) {
      KeyValue = IoRead8 (0x60);
    }
    //
    // Check for valid key
    //
    if (KeyValue == 0x1d) {
      //
      // Set BIT0 if it is CTRL key
      //
      KeyScan |= (1 << 0);
    }
    
    if (KeyValue == 0x47) {
      //
      // Set BIT1 if it is HOME key
      //
      KeyScan |= (1 << 1);
    }
    //
    // Check for CTRL_HOME Combination
    //
    if ((KeyScan & 0x03) == 0x03) {
      //
      // Disable KeyBoard Controller
      //
      IoWrite8 (0x64, 0x60);
      IoWrite8 (0x60, 0x30);
      return TRUE;
    }
  } // for (Index = 0 ; Index < LoopCount ; Index++)
  //
  // Disable KeyBoard Controller
  //
  IoWrite8 (0x64, 0x60);
  IoWrite8 (0x60, 0x30);
  //
  // No valid key is pressed
  //
  return FALSE;
}
#endif        // KBC_SUPPORT
#endif        // AMI_COMPATIBILITY_PKG_VERSION

//####
//#### Above libraries are EIP148016
//####
//###############################################################
//###############################################################

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
