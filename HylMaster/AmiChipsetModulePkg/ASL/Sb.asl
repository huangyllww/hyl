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
//<AMI_PHDR_START>
//-----------------------------------------------------------------------
//
// Procedure:    LPC bridge operation
// Description:  METHOD IS CALLED BY OS PRIOR TO ENTER ANY SLEEP STATE
// Input:        Arg0 = Sleep state System about to enter
// Output:       Nothing
//
//-----------------------------------------------------------------------
//<AMI_PHDR_END>
//**********************************************************************
  //  OperationRegion(RMBS, PCI_Config, 0x00, 0x100)
  //  Field(RMBS, ByteAcc, NoLock, Preserve)
  //  {
  //  Offset(0x6c),
  //  LR2S, 16, // LPC ROM Address Range 2 (Start Address)
  //  LR2E, 16, // LPC ROM Address Range 2 (End Address)
   // }

//<AMI_PHDR_START>
//------------------------------------------------------------------------/
//
// Procedure:    SPTS
// Description:  METHOD IS CALLED BY OS PRIOR TO ENTER ANY SLEEP STATE
// Input:        Arg0 = Sleep state System about to enter
// Output:       Nothing
//
//-------------------------------------------------------------------------
//<AMI_PHDR_END>

    Method (SPTS, 1) {
    }

//<AMI_PHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    SWAK
// Description:  METHOD CALLED ON WAKE UP FROM ANY SLEEP STATE
// Input:        Arg0 = Sleep state System is resuming from
// Output:       Nothing
//
//-------------------------------------------------------------------------
//<AMI_PHDR_END>

    Method (SWAK, 1) {
         If(PICM) { \_SB_.IRQC()} //Add PICM check for APIC  OS
    }

//;<AMI_PHDR_START>
//;------------------------------------------------------------------------
//;
//; OperationRegion    SLPS
//;-------------------------------------------------------------------------
//;<AMI_PHDR_END>

    Scope(\_SB) {
        Name(SLPS, 0)
    }

//<AMI_PHDR_START>
//------------------------------------------------------------------------
//
// OperationRegion    SMI0 (SMI SW SMI Port)
//-------------------------------------------------------------------------
//<AMI_PHDR_END>
   OperationRegion (SMI0, SystemIO, SMIO, 1)
   Field (SMI0, ByteAcc,NoLock,Preserve)
    { SMIC, 8,}  //This regision define move it from the subcrb.asl.
                 // it is for build issue when CRB board remove
//<AMI_PHDR_START>
//------------------------------------------------------------------------
//
// Procedure:    SB900
// Description:  describe resource
//
//-------------------------------------------------------------------------
//<AMI_PHDR_END>
//-- New HID support -->
//-- When NB Root PCI Bridge == PCI Express --> Scope(\_SB){ Scope(PCI0){
Scope(\_SB){ Scope(PCI0){
//Resources Consumed By Taishan
Device(S900) {
    //-- New HID support --> Name(_HID, EISAID("PNP0C01"))       // Hardware Device ID = >System Board Resources
    Name(_HID, EISAID("PNP0C02"))   // System board resources device node ID
    Name(_UID, 0x700)
//---------
    Name(_STA,0x0F)
//---------
    Name(CRS, ResourceTemplate(){

        IO(Decode16,0x10, 0x10, 0,0x10)
        IO(Decode16,0x22, 0x22, 0,0x1e)
//fding-2005-05-26-01 Start
//remove 0x44-0x5f from mother board resource for TPM uses 0x4e-0x4f
//      IO(Decode16,0x44, 0x44, 0,0x1c)
//fding-2005-05-26-01 End
//(P2011217F)>>The Io port 0x62 ,0x66 ,it will report by the EC device
//        IO(Decode16,0x62, 0x62, 0,0x02)
//        IO(Decode16,0x65, 0x65, 0,0x0b)
        IO(Decode16, 0x63, 0x63, 0, 1)
        IO(Decode16, 0x65, 0x65, 0, 1)
        IO(Decode16, 0x67, 0x67, 0, 9)
//<<(P2011217F)
        IO(Decode16,0x72, 0x72, 0,0x0e)
        IO(Decode16,0x80, 0x80, 0,0x01)
        IO(Decode16,0x84, 0x84, 0,0x03)
        IO(Decode16,0x88, 0x88, 0,0x01)
        IO(Decode16,0x8c, 0x8c, 0,0x03)
        IO(Decode16,0x90, 0x90, 0,0x10)
        IO(Decode16,0xa2, 0xa2, 0,0x1e)
        IO(Decode16,0xb1, 0xb1, 0,0x1)
        IO(Decode16,0xe0, 0xe0, 0,0x10)
// Decoded but not used by FDC. Reserved in FDC device node resources
//                IO(Decode16, 0x3f3, 0x3f3, 0, 0x1)
// Reserve  4D0 and 4D1 for IRQ edge/level control port
        IO(Decode16, 0x4d0, 0x4d0, 0, 0x2)
        // Reserve  unknown I/O space - 40Bh and 4D6h
        IO(Decode16, 0x40B, 0x40B, 0, 0x1)
        IO(Decode16, 0x4D6, 0x4D6, 0, 0x1)
        // Reserve  C00 and C01 for PCI IRQ Routing
        IO(Decode16, 0xC00, 0xC00, 0, 0x2)
        IO(Decode16, 0xC14, 0xC14, 0, 0x1)
        IO(Decode16, 0xC50, 0xC50, 0, 0x2)
        IO(Decode16, 0xC52, 0xC52, 0, 0x1)
        IO(Decode16, 0xC6C, 0xC6C, 0, 0x1)
        IO(Decode16, 0xC6F, 0xC6F, 0, 0x1)

        // Reserve  CD0 and CD1 for BIOSRAM Reg Access PORT
        IO(Decode16, 0xCD0, 0xCD0, 0, 0x2)
        // Reserve  CD2 and CD3 for BIOSRAM Reg Access PORT
        IO(Decode16, 0xCD2, 0xCD2, 0, 0x2)

        // Reserve  CD4 and CD5 for BIOSRAM Reg Access PORT
        IO(Decode16, 0xCD4, 0xCD4, 0, 0x2)
        // Reserve  CD6 and CD7 for PM Reg Access PORT
        IO(Decode16, 0xCD6, 0xCD6, 0, 0x2)
        // Reserve  CD8 and CDF for AB Reg Access PORT
        IO(Decode16, 0xCD8, 0xCD8, 0, 0x8)
        // ACPI IO base address allocation
        IO(Decode16, 0, 0, 0, 0, IO0)
        // GP 1 I/O space
        IO(Decode16, 0, 0, 0, 0, IO2)
        // SMBus I/O base0. space if applicable
        IO(Decode16, 0, 0, 0, 0, IO10)
        // SMBus I/O base1. space if applicable
        IO(Decode16, 0, 0, 0, 0, IO11)
        // IST module use
        IO(Decode16, 0x900, 0x900, 0, 0x10)
        IO(Decode16, 0x910, 0x910, 0, 0x10)
        // PM2 use
        IO(Decode16, 0xFE00, 0xFE00, 0, 0xFF)

        IO(Decode16, 0x60, 0x60, 0, 0,KBC0)
        IO(Decode16, 0x64, 0x64, 0, 0,KBC1)
// LPC sst49004b need use this range P122605A
        //---Memory32Fixed(ReadOnly, 0xFFB80000, 0x80000)

        //IO APIC SPACE
        Memory32Fixed(ReadWrite, 0, 0, AP)
        //IO APIC 1 SPACE
        Memory32Fixed(ReadWrite, 0xFEC01000, 0x1000, AP2)
        //FCH AL2AHB Configuration Space
        Memory32Fixed(ReadWrite, 0xFEDC0000, 0x1000, AHB)
        //LOCAL APIC SPACE
        Memory32Fixed(ReadWrite, 0xFEE00000, 0x1000, LA)

    //SB MMIO Base
        Memory32Fixed(ReadWrite, 0xfed80000, 0x10000, SBMO)

    //SB GEC Base
        Memory32Fixed(ReadWrite, 0xfed61000, 0x10000, SGEC)

        // Report SPI space
        Memory32Fixed(ReadWrite, 0, 0, SPIx)

//        // Report HPET space
//        Memory32Fixed(ReadWrite, 0, 0, HxT)
        
        // Report WDT space
        Memory32Fixed(ReadWrite, 0, 0, WDTx)

        // Report BIOS ROM space
        Memory32Fixed(ReadWrite, 0, 0, ROMx)
    })

    Method (_CRS, 0)
    {
        //PMBS,PMLN - ACPI PM I/O Base address and length
        CreateWordField(CRS, ^IO0._MIN, PBB)   //PM base
        CreateWordField(CRS, ^IO0._MAX, PBH)
        CreateByteField(CRS, ^IO0._LEN, PML)   // PM length
        Store(\PMBS, PBB)           // ACPI IO Base address
        Store(\PMBS, PBH)           // ACPI
        Store(\PMLN, PML)           // Region length

        If(SMBB) //SMBUS IO Resources
        {
            CreateWordField(CRS, ^IO11._MIN, SMB1) // SMBUS1. Base
            CreateWordField(CRS, ^IO11._MAX, SMH1)
            CreateByteField(CRS, ^IO11._LEN, SML1) // SMBUS1. Length
            Store(\SMBB, SMB1)   // Min Base address
            Store(\SMBB, SMH1)   // Max Base address
            Store(\SMBL, SML1)   // Region length

            CreateWordField(CRS, ^IO10._MIN, SMBZ) // SMBUS0. base
            CreateWordField(CRS, ^IO10._MAX, SMH0)
            CreateByteField(CRS, ^IO10._LEN, SML0) // SMBUS0. length
            Store(SMB0, SMBZ)   // Min Base address
            Store(SMB0, SMH0)   // Max Base address
            Store(SMBM, SML0)   // Region length
        }
        If(GPBS)
        {
            CreateWordField(CRS, ^IO2._MIN, IGB) // SB Chipset GPIO Base
            CreateWordField(CRS, ^IO2._MAX, IGH)
            CreateByteField(CRS, ^IO2._LEN, IGL) // GPIO length
            Store(\GPBS, IGB)   // Min Base address
            Store(\GPBS, IGH)   // Max Base address
            Store(\GPLN, IGL)   // Region length
        }
        // Reserve IO & Local APIC memory ranges only if IO APIC is decoded/enabled
        // APCB,APCL - on chip I/O APIC Base address & region length
        If(APCB)
        {
            CreateDwordField(CRS, ^AP._BAS, APB)
            CreateDwordField(CRS, ^AP._LEN, APL)
            Store(\APCB, APB)   // Base address I/O APIC
            Store(\APCL, APL)   // Region length
        }
        // Reserve SPI base
        If(SPIB)
        {
            CreateDwordField(CRS, ^SPIx._BAS, SPIb)
            CreateDwordField(CRS, ^SPIx._LEN, SPIl)
            Store(\SPIB, SPIb)   // Base address SPI
            Store(\SPIL, SPIl)   // Region length
        }
//        // Reserve HPET base
//        If(HPTB)
//        {
//            CreateDwordField(CRS, ^HxT._BAS, HxTb)
//            CreateDwordField(CRS, ^HxT._LEN, HxTl)
//            Store(\HPTB, HxTb)   // Base address HPET
//            Store(\HPTL, HxTl)   // Region length
//        }
        // Reserve WDT base
        If(WDTB)
        {
            CreateDwordField(CRS, ^WDTx._BAS, WDTb)
            CreateDwordField(CRS, ^WDTx._LEN, WDTl)
            Store(\WDTB, WDTb)   // Base address WDT
            Store(\WDTL, WDTl)   // Region length
        }
        
        // BIOS ROM Base address & region length
        CreateDwordField(CRS, ^ROMx._BAS, ROMB)
        CreateDwordField(CRS, ^ROMx._LEN, ROML)
	//Reserve Flash resources Region 0xFF000000 as BIOS ROM have been decoded 16M in AgesaPI
        Store(0xFF000000, ROMB)   // Base address WDT
        Store(0x1000000, ROML)   // Region length

        Return(CRS)
    }

} //end device
} //Scope(PCI0)
} //Scope(\_SB
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************