/*****************************************************************************
 *
 * 
 * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
 * 
 * HYGON is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with HYGON.  This header does *NOT* give you permission to use the Materials
 * or any rights under HYGON's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 * 
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by HYGON shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 * 
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 * 
 * HYGON does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by HYGON, or
 * result from use of the Materials or any related information.
 * 
 * You agree that you will not reverse engineer or decompile the Materials.
 * 
 * NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, HYGON retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 * 
 * AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
 * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 * the right to make the modified version available for use with HYGON's PRODUCT.
 * ***************************************************************************
 */
//AMD Special CMOS Port72/73
//Offset 0 to 0dh, used by ATI SB Module
//
OperationRegion(\DEB2, SystemIO, 0x80, 0x2)
Field(\DEB2, WordAcc, NoLock, Preserve)
{
     P80H, 16
}

Name(OSTY, Ones)                //BIOS kernel asl code should save the OS type in this variable for the
                                //SB CIM module to do settings based on the current OS
                                //      0 - Legacy or Unknown OS
                                //      1 - Win 98/Se
                                //      2 - Win Me
                                //      3 -
                                //      4 - Win 2000
                                //      5 - Window XP
                                //      6 - Windows Vista
                                //      7 and above - reserved for future OS
                                //      Some of the known issues which require this variable setup properly
                                //      are S3 resume failure(blue screen) and S3 resume time issues with Vista,
                                //      Win2K with sata harddrives.

OperationRegion (ACMS, SystemIO, 0x72, 0x10)
Field (ACMS, ByteAcc, NoLock, Preserve)
{
        ICMS,8,
        DCMS,8,
        Offset(0x0e),
	P80_,8
}

//----------- FCH -----------//
// Original mechanism failure because of....
//  1. AGESA FCH did not allcate runtime memory.
//  2. The PCIE field didn't exist in AMD_CONFIG_PARAMS.
// Below is temporary solution. FCH.
Name (OSCF, 0)
Name (GGN2, 1)
//----------- FCH End   -----------//

/*
IndexField (ICMS, DCMS, ByteAcc, NoLock, Preserve)
{
        Offset(0x00),
        Offset(0x01),
        Offset(0x04),
//	RMBA, 32,
        Offset(0x08),
	BS_A,	32,	// Config Structure in memory
        Offset(0x0C)
}

// Set of global Names 
OperationRegion(CFGS, SystemMemory, BS_A, 0x100)
Field(CFGS, AnyAcc, NoLock, Preserve)
{
	Offset(0x0C),	
	PCIE, 32,	//PCIE base
	Offset(0x20),	
	, 16,	//
	EKBC, 1,	//EC KBC
	Offset(127),	
	,5,
	GGN2,1,		//GPP GEN2 Enabled
	Offset(0x93),	//MiscConfiguration offset 0x93 (* DEC 146+1) 
	,5,
	OSCF,1,		//Show _OSC under PCI0 to support native PCIE

	Offset(166),	
	PRS0,1,		//Present
	DET0,1,		//Detect
	Offset(170),	
	PRS1,1,		//Present
	DET1,1,		//Detect
	Offset(174),	
	PRS2,1,		//Present
	DET2,1,		//Detect
	Offset(178),	
	PRS3,1,		//Present
	DET3,1,		//Detect
	Offset(186),	
	,6,
	ABPS,1,		//AB Power saving
	,3,
	GPPS,1,		//GPP Power saving
}*/

//
//CMIndex 0xC50-0xC52
OperationRegion (CMPT, SystemIO, 0xC50, 0x3)
Field (CMPT, ByteAcc, NoLock, Preserve)
{
        CMID,8,         
	    ,6,
        GPCT,2,
	GP0I,1,          
	GP1I,1,          
	GP2I,1,          
	GP3I,1,          
	GP4I,1,          
	GP5I,1,          
	GP6I,1,          
	GP7I,1,          
}

OperationRegion(PCFG, SystemMemory, PEBS, 0x02000000)		//Reserve 32 bus number
Field(PCFG, AnyAcc, NoLock, Preserve)
{
	Offset(0x00020078),		//PCIE P2P Device 4 
	, 16,
	PMSA, 1,			//PME Status
	Offset(0x00028078),		//PCIE P2P Device 5 
	, 16,
	PMSB, 1,			//PME Status
	Offset(0x00048078),		//PCIE P2P Device 9 
	, 16,
	PMSC, 1,			//PME Status
	Offset(0x0008800A),		//SATA class ID
	STCL, 16,
//	Offset(0x00088024),		//SATA BAR5
//	STB5, 32,
	Offset(0x000880FC),		//Reserved memory bar for SATA controller is moved to 0ffset 04 of ioport 0x72/0x73
	RMBA, 32,
	Offset(0x000A0004),		//SMBUS	BUS 0 DEV 14h FUN 0
	SMIE, 1,			//SMbus Io Enable
	SMME, 1,			//SMbus Memory Enable
	Offset(0x000A0008),		//SMBUS	BUS 0 DEV 14h FUN 0
	RVID, 8,			//SMbus Revision ID
	Offset(0x000A0014),		//SMBUS	BUS 0 DEV 14h FUN 0
	SMB1, 32,			//SMbus Bar 1
	Offset(0x000A0082),
	,2,
	G31O,1,				//Output control of GPIO31
	Offset(0x000A00E1),
	,6,
	ACIR,1,			//ACPI Interrupt
	Offset(0x000A4004),
	PCMD, 2,
	Offset(0x000A807A),		//SB GPP Device 21 Fun 0
	PMS0, 1,			//PME Status
	Offset(0x000A8088),		//SB GPP Device 21 Fun 0
	TLS0, 4,			//TARGET_LINK_SPEED
	Offset(0x000A907A),		//SB GPP Device 21 Fun 1
	PMS1, 1,			//PME Status
	Offset(0x000A9088),		//SB GPP Device 21 Fun 1
	TLS1, 4,			//TARGET_LINK_SPEED
	Offset(0x000AA07A),		//SB GPP Device 21 Fun 2
	PMS2, 1,			//PME Status
	Offset(0x000AA088),		//SB GPP Device 21 Fun 2
	TLS2, 4,			//TARGET_LINK_SPEED
	Offset(0x000AB07A),		//SB GPP Device 21 Fun 3
	PMS3, 1,			//PME Status
	Offset(0x000AB088),		//SB GPP Device 21 Fun 3
	TLS3, 4,			//TARGET_LINK_SPEED
}

// Aptio Porting >> Aptio doesn't need that now. wrong RMBA will cause OS fail.
//###OperationRegion(RMEM, SystemMemory, RMBA, 0x0200)		//Reserve 32 bus number
//###Field(RMEM, AnyAcc, NoLock, Preserve)
//###{
//###	Offset(0x00000004),		//Reserved memory size for SATA controller
//###	RMLN, 32,
//###}
// Aptio Porting <<

OperationRegion (PMIO, SystemIO, 0xCD6, 0x2)
Field (PMIO, ByteAcc, NoLock, Preserve)
{
        INPM,8,
        DAPM,8
}

IndexField (INPM, DAPM, ByteAcc, NoLock, Preserve)	//R07
{
	, 6,
	HPEN, 1,
	Offset(0x60),		//AcpiPm1EvgBlk
	P1EB, 16,
	Offset(0xC8),	//Offset(200),
	, 2,
	SPRE, 1,
	TPDE, 1,
	Offset(0xF0),	//Offset(240),
	, 3,
	RSTU, 1,
}               //end of indexed field

OperationRegion(ERMM, SystemMemory, 0xFED80000, 0x001800)		//
Field(ERMM, AnyAcc, NoLock, Preserve)
{
    // GPIO Bank 0 Control Register
    //------------------------------ 
    // Bits  | Description
    //------------------------------
    // 3:0   | DebounceTmrOut
    // 4     | DebounceTmrOutUnit
    // 6:5   | DebounceCntrl
    // 7     | Reserved
    // 8     | LevelTrig
    // 10:9  | ActiveLevel
    // 12:11 | InterruptEnable
    // 15:13 | WakeCntrl
    // 16    | PinSts
    // 18:17 | DrvStrengthSel
    // 19    | PullUpSel
    // 20    | PullUpEnable
    // 21    | PullDownEnable
    // 22    | OutputValue
    // 23    | OutputEnable
    // 25:24 | Reserved
    // 27:26 | Reserved
    // 28    | InterruptSts
    // 29    | WakeSts
    // 31:30 | Reserved


//SMI base
    Offset(0x200),		
        ,9,
    GESA,1,
    offset(0x208),
        ,5,
    E05C,1,
    offset(0x209),
    E08C,1,
    E09C,1,
    E10C,1,
    offset(0x20B),
    ,5,
    E1DC,1,
    
    Offset(0x288),	//Offset(648),
    , 1,
    CLPS, 1,
//EIP218890+
    offset(0x296),
    ,7,
    TMSE,1,                             //Timer SMI En
//EIP218890+
// AMI porting <<
	Offset(0x0002B0),		//Slp_Type
	,2,
	SLPS,2,				// SMI control of Slp_Type
	Offset(0x000377),		// PCIeNative
	EPNM,1,				// Block PCIe GPP PME message and HotPlug message from generating SCI.
	DPPF,1,				// disable PCIE_WAK_STS and PCIE_WAK_DIS function.
	Offset(0x0003BB),		// WakePinEnable
	,6,
	PWDE,1,				// enable wakeup from WAKE# pin.
	Offset(0x0003BE),		//Allow Slp_Type
	,5,
	ALLS,1,				// Set to 1 to allow SLP_TYP to do S1, S3, and S5
	Offset(0x0003E4),	        //Offset(996),
	BLNK, 2,
	Offset(0x0003F0),		// USB Control
	PHYD,1,
	,1,
	,1,
	US5R,1,
	Offset(0x0003F6),		// UGEC Control
	GECD,1,
// AMI porting >>
//Io mux 
    Offset(0x0D06),
    G06M,8,
    Offset(0x0D07),
    G07M,8,
    Offset(0x0D0B),
    G0BM,8,
    Offset(0x0D0E),
    G0EM,8,
    Offset(0x0D14),
    G14M,8,
    Offset(0x0D1C),
    G1CM,8,
    Offset(0x0D20),
    G20M,8,
    Offset(0x0D23),
    G23M,8,
    Offset(0x0D2D),
    G2DM,8,
    Offset(0x0D37),
    G37M,8,
    Offset(0x0D70),
    G70M,8,
    Offset(0x0D76),
    G76M,8,
    Offset(0x0DAF),
    GAFM,8,
    Offset(0x0DB0),
    GB0M,8,

//Misc base
// AMI porting <<
	Offset(0x000E80),		//StrapStatus - R 8/16/32 bits - [Misc_Reg: 80h]
	,2,
    IMCS,1,
//	ECEN,1,				//EcEnableStrap 2 0b Enable Embedded Controller (EC)

// AMI porting >>
// GPIO
    Offset(0x1518),         // GEVENT 10
    ,16,
    GE10, 1,

    offset(0x1524),         // GEVENT 22
    ,16,
    GE22,1,
    Offset(0x1538),       // GEVENT6 - GPIO102
    ,16,
    G66I,1,                 // GPI
    ,5,
    G66O,1,                 // GP0
    G66E,1,                 // GpioIutEnB
    Offset(0x1560),       // GEVENT15 - GPIO111
    ,16,
    G6FI,1,                 // GPI
    ,5,
    G6FO,1,                 // GP0
    G6FE,1,                 // GpioIutEnB                        
    Offset(0x1604),        // GPIO50
    ,16,
    G50I,1,                 // GPI
    ,5,
    G50O,1,                 // GP0
    G50E,1,                 // GpioIutEnB
    Offset(0x1608),       // GPIO51
    ,16,
    G51I,1,                 // GPI 
    ,5,       
    G51O,1,                 // GP0
    G51E,1,                 // GpioIutEnB
    Offset(0x1610),       // GPIO57
    ,16,
    G57I,1,                 // GpioIn 
    ,5,
    G57O,1,                 // GpioOut
    G57E,1,                 // GpioOutEnB - GPIO57
    Offset(0x1618),       // GPIO59
    ,16,
    G59I,1,                 // GpioIn 
    ,5,
    G59O,1,                 // GpioOut
    G59E,1,                 // GpioOutEnB - GPIO57      
// AMI porting <<
}

OperationRegion (ABIO, SystemIO, 0xCD8, 0x8)
Field (ABIO, DWORDAcc, NoLock, Preserve)
{
        INAB,32,
        DAAB,32
}

Method (RDAB,1)	//
{
	store(arg0, INAB)
	return (DAAB)
}               //

Method (WTAB,2)	//
{
	store(arg0, INAB)
	store(arg1, DAAB)
}               //

Method (RWAB,3)	//
{
	and(RDAB(arg0),arg1,Local0)
	or(Local0,arg2,Local1)
	WTAB(arg0, Local1)
}               //

Method (CABR,3)	// Caculate AB register
{
	Shiftleft(arg0, 5, Local0)
	add(Local0, arg1, Local1)
	ShiftLeft(Local1, 24, Local2)
	add(Local2, arg2, Local3)
	return(Local3)
}               //

Method (GHPS,2)	// GPP HOT PLUG SERVICE //arg0:0 in;1 out
					//arg1:gpp number
{
//breakpoint
//	if (LAND(ABPS,GPPS)) {
		if (LEQUAL(arg0,0)){
			RWAB(CABR(6,0,0xc0),0xffffbfff,0x00000000)
			RWAB(CABR(1,0,0x65),0xfffffbfb,0x00000000)
			stall(200)
		}
		if (LEQUAL(arg0,1)){
			RWAB(CABR(6,0,0xc0),0xffffbfff,0x00004000)
			RWAB(CABR(1,0,0x65),0xfffffbfb,0x00000404)
			stall(200)
		}
//	}
	if (GGN2) {
	if (LEqual(arg0,0))
	{
//breakpoint
		//try GEN2
		GEN2()
		store(RDAB(CABR(3,2,0xa5)),Local0)
		and(Local0,0xff,Local0)
		store(500,Local1)
		while (LAnd(LGreater(Local1,0),LNotEqual(Local0,0x10)))
		{
			store(RDAB(CABR(3,2,0xa5)),Local0)
			and(Local0,0xff,Local0)
			Decrement(Local1)
			stall(200)
			stall(200)
		}
		if (LNotEqual(Local0, 0x10)) {GEN1()}
	}
	}
}               //

Method (GEN2,0)	//
{
	store(2, TLS2)
	RWAB(CABR(3,2,0xa4),0xfffffffe,0x00000001)
	RWAB(CABR(3,2,0xa2),0xffffdfff,0x00002000)
	RWAB(CABR(3,2,0xc0),0xffff7fff,0x00008000)
	RWAB(CABR(3,2,0xa4),0xdfffffff,0x20000000)
	stall(200)
	stall(200)
}

Method (GEN1,0)	//
{
	store(1, TLS2)
	RWAB(CABR(3,2,0xa4),0xfffffffe,0x00000000)
	RWAB(CABR(3,2,0xa2),0xffffdfff,0x00002000)
	stall(200)
	stall(200)
}
OperationRegion (P1E0, SystemIO, P1EB, 0x04)
Field (P1E0, ByteAcc, Nolock, Preserve) {
	, 14,
	PEWS,	1,
	WSTA,	1,
        Offset(0x02),
	, 14,
	PEWD,	1,
}


Method (SPTS, 1) {
	Store(1, PCMD)
	Store(Arg0, P80_)

	//Set Blink LED according to sleep state, for S4/S5 leave it ON always.
// 	if (LEqual(Arg0,1)){Store(0x02,\BLNK)}	// for S1, 10: 2sec on, 2sec off, repeating
// 	if (LEqual(Arg0,2)){Store(0x00,\BLNK)}	// for S2, 00: Blink LED off
// 	if (LEqual(Arg0,3)){Store(0x01,\BLNK)}	// for S3, 1sec on, 3sec off, repeating

  if (IMCS) {
    IBSE()
  }

	Store(1, CLPS)	// Clear SLP_SMI Status
	Store(1, SLPS)	// Enable SLP_SMI
	Store(0, ALLS)  // Set NOT to allow any S state (For SLEEP trap)
	Store(1, PHYD)  // A11 bug
	CPMS()
	CPMS()
//
// Clear GPP9 NIC PME Status for ACPI S1 cannot wake while the system goes to sleep state second time.
//
//		Store(5, Local0)
//		While(Local0)
//		{
//			Store(1, PMSA)
//			Store(1, PMSB)
//			Store(1, PMSC)
//			Decrement(Local0)
//		}
// This procedure is workaround for Hudson-2 A11 only.
// Please removed it after A12 chip come back.
		Store(1, PEWS)			// Clear PciExpWakeStatus
//		Store(0x66,DBG8)

//Hudson-2 comment out 	if (LLessEqual(\RVID, 0x13)) {Store(zero, \PWDE)}	// Clear PciExpWakeDisEn to workaround PCIE LAN wakeup hang issue
// 	if (LEqual(Arg0,4)){Store(zero,US5R)}
}

Method (SWAK, 1) {
	If (PICM)
	{
		\_SB_.IRQC()
	}
//	Store(One, \PMS7)
//EIP218890+
if (TMSE){
  Store(0, TMSE)
}
//EIP218890+
// 	Store(one, HECO)	// Enable HPET ECO
// 	if (LEqual(Arg0,3)){Store(one,RSTU)}	//set RSTU to make pci_rst reset USB in S3
// 	Store(\PEWS, \PEWS)	// Clear PciExpWakeStatus
// 	Store(0x03, \BLNK)	// Turn on Blink LED for S0 stateb

	CPMS()
	CPMS()
//
// Clear GPP9 NIC PME Status for ACPI S1 cannot wake while the system goes to sleep state second time.
//
//		Store(5, Local0)
//		While(Local0)
//		{
//			Store(1, PMSA)
//			Store(1, PMSB)
//			Store(1, PMSC)
//			Decrement(Local0)
//		}
//
// This procedure is workaround for Hudson-2 A11 only.
// Please removed it after A12 chip come back.
//
		Store(1, PEWS)			// Clear PciExpWakeStatus
    if (IMCS) {
      IBWE()
    }

    If (LEqual(Arg0, 3)) {
        Notify(\_SB.PWRB, 0x02)
    }
//		Store(0x77,DBG8)
// 	if (LEqual(Arg0,4)){Store(one,US5R)}
}

Method(TRMD, 1) {
//Hudson-2 comment out 		Store(Arg0, SPRE)
//Hudson-2 comment out 		Store(Arg0, TPDE)
}

Method(CPMS, 0) //clear PCIE PME statuse
{
	if (LEqual(EPNM,0))
	{
		Store(1,PMSA)
		Store(1,PMSB)
		Store(1,PMSC)
		Store(1,PMS0)
		Store(1,PMS1)
		Store(1,PMS2)
		Store(1,PMS3)
	}
}

Scope (\_GPE)
{
//	Method(_L1F, 0) {store(0x1f,P80_)}
//	Method(_L19, 0) {store(0x19,P80_)}
//	Method(_L06, 0) {store(0x06,P80_)}
//	Method(_L00, 0) {store(0x00,P80_)}
}
