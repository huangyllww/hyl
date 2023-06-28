//****************************************************************************
//****************************************************************************
//**                                                                        **
//**         (C)Copyright 1985-2019, American Megatrends, Inc.              **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**          5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:        NBRes.ASL
//
// Description: PCI Root Bridge resources buffer and CRS Method
// MUST be included from NB.ASL directly using "include" ASL term
// Suppose to be already in ROOT BRIDGE Device ASL Scope.
//
// NOTE: NO Porting needed 
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

                        
//Defining some place holders, we need to updte at runtime.
        Name(CPRB, 0x0)         //BOOLEAN  Compatible Root Bridge if !=0
        Name(LVGA, 0x55)        //BOOLEAN  Legacy Video Present if !=0
        Name(STAV,0x0F)
//Information Provided in RootBridgeIo->Configuration() function.
        Name(BRB, 0x0100)       //Bus Range Base 
        Name(BRL, 0x0100)       //Bus Range Length

        Name(IOB, 0x0100)       //IO Range BASE (16bit resource)
        Name(IOL, 0x0100)       //IO Range LENGTH

        Name(MBB, 0x10000)      //Memory Range Below 4G MIN (32bit resource)
        Name(MBL, 0x10000)      //Memory Range Below 4G LENGTH (32bit resource)

//        Name(MABL, 0x10000)     //Memory Range Above 4G MIN LSDW (32bit resource)
        Name(MABL, 0x100000000)     //Make 64bit per Olek
        Name(MABH, 0x10000)     //Memory Range Above 4G MIN MSDW (32bit resource)
                                             
        Name(MAML, 0x10000)     //Memory Range Above 4G MAX LSDW (32bit resource)
        Name(MAMH, 0x10000)     //Memory Range Above 4G MAX LSDW (32bit resource)
        
//        Name(MALL, 0x10000)     //Memory Range Above 4G LENGTH LSDW (32bit resource)
        Name(MALL, 0x100000000)        //Make 64bit per Olek
        Name(MALH, 0x10000)     //Memory Range Above 4G LENGTH LSDW (32bit resource)

        Name(PRXM, 0x100)


    //Resource buffer to be used for NONE COMPATIBILITY ROOT BRIDGE
    Name(CRS2, ResourceTemplate() {
        WORDBusNumber(          //Bus number resource (0); the bridge produces bus numbers for its subsequent buses
            ResourceProducer,      // bit 0 of general flags is 1
            MinFixed,           // Range is not fixed
            MaxFixed,           // Range is not fixed
            PosDecode,          // PosDecode
            0x0000,     // Granularity
            0x0080,     // Min
            0x00ff,     // Max; half of the available bus numbers are reserved for this root bus
            0x0000,     // Translation
            0x0080,     // Range Length
            ,,
            BUS2
        )

        // For Legacy Video IO (3B0 - 3DF)
        WORDIO(         //Consumed-and-produced resource (Used for legacy video)
            ResourceProducer,   // bit 0 of general flags is 0
            MinFixed,           // Range is fixed
            MaxFixed,       // Range is fixed
            PosDecode,
            EntireRange,
            0x0000,     // Granularity
            0x0000,     // Min
            0x0000,     // Max
            0x0000,     // Translation
            0x0000,     // Range Length
            ,,
            VIO2
        )

        // Other IO used by this root bridge
        WORDIO(         //Consumed-and-produced resource
            ResourceProducer,   // bit 0 of general flags is 0
            MinFixed,           // Range is fixed
            MaxFixed,       // Range is fixed
            PosDecode,
            EntireRange,
            0x0000,     // Granularity
            0x0000,     // Min
            0x0000,     // Max
            0x0000,     // Translation
            0x0000,     // Range Length
            ,,
            IOW3
        )

        // For Legacy Video Memory (A000 - BFFF)
        DWORDMEMORY(            // descriptor for video RAM behind ISA bus
            ResourceProducer,   // bit 0 of general flags is 0
            PosDecode,
            MinFixed,       // Range is fixed
            MaxFixed,       // Range is Fixed
            Cacheable,
            ReadWrite,
            0x00000000,     // Granularity
            0x00000000,     // Min
            0x00000000,     // Max
            0x00000000,     // Translation
            0x00000000,     // Range Length
            ,,
            VGA2
        )

        // Memory < 4GB
        DWORDMEMORY(            // descriptor for video RAM behind ISA bus
            ResourceProducer,   // bit 0 of general flags is 0
            PosDecode,
            MinFixed,       // Range is fixed
            MaxFixed,       // Range is Fixed
            Cacheable,
            ReadWrite,
            0x00000000,     // Granularity
            0x80000000,     // Min
            0xffffffff,     // Max
            0x00000000,     // Translation
            0x80000000,     // Range Length
            ,,
            MEM1
        )

        // Memory > 4GB
        QWORDMEMORY(            // descriptor for MMIO ABOVE 4G
            ResourceProducer,   // bit 0 of general flags is 0
            PosDecode,
            MinFixed,       // Range is fixed
            MaxFixed,       // Range is Fixed
            Cacheable,
            ReadWrite,
            0x00000000,     // Granularity
            0x00000000,     // Min
            0x00000000,     // Max
            0x00000000,     // Translation
            0x00000000,     // Range Length
            ,,
            MEM9
        )

    } ) //End resource template Name CRS2

    Method(_STA, 0){
        Return(STAV)
    }

    Method(_CRS, 0, Serialized) {

            //breakpoint


                //Bus Range
                CreateWordField(CRS2, ^BUS2._MIN, MIN2) // Min
                CreateWordField(CRS2, ^BUS2._MAX, MAX2) // Max
                CreateWordField(CRS2, ^BUS2._LEN, LEN2) // Length

                Store(BRB, MIN2)
                Store(BRL, LEN2)
                Store(LEN2,Local1)
                Add(MIN2, Decrement(Local1), MAX2)

                //Update Upper IO from IOB up to IOB+IOL
                CreateWordField(CRS2, ^IOW3._MIN, MIN4) // Min
                CreateWordField(CRS2, ^IOW3._MAX, MAX4) // Max
                CreateWordField(CRS2, ^IOW3._LEN, LEN4) // Length

                Store(IOB, MIN4)
                Store(IOL, LEN4)
                Store(LEN4,Local1)
                Add(MIN4, Decrement(Local1), MAX4)

                If(LVGA){
                    CreateWordField(CRS2, ^VIO2._MIN, IMN2) // Min
                    CreateWordField(CRS2, ^VIO2._MAX, IMX2) // Max
                    CreateWordField(CRS2, ^VIO2._LEN, ILN2) // Length

                    Store(0x03b0, IMN2)
                    Store(0x03df, IMX2)
                    Store(0x0030, ILN2)

                    CreateDWordField(CRS2, ^VGA2._MIN, VMN2)    // Min
                    CreateDWordField(CRS2, ^VGA2._MAX, VMX2)    // Max
                    CreateDWordField(CRS2, ^VGA2._LEN, VLN2)    // Length

                    Store(0x000a0000, VMN2)
                    Store(0x000bffff, VMX2)
                    Store(0x00020000, VLN2)
                }

                //Update MMIO Below 4G
                CreateDWordField(CRS2, ^MEM1._MIN, MIN5)    // Min
                CreateDWordField(CRS2, ^MEM1._MAX, MAX5)    // Max
                CreateDWordField(CRS2, ^MEM1._LEN, LEN5)    // Length

                Store(MBB, MIN5)
                Store(MBL, LEN5)
                Store(LEN5,Local1)
                Add(MIN5, Decrement(Local1), MAX5)
                
                //Start 64bit
        //Update MMIO Above 4G

                CreateQWordField(CRS2, ^MEM9._MIN, MIN9)    // Min
                CreateQWordField(CRS2, ^MEM9._MAX, MAX9)    // Max
                CreateQWordField(CRS2, ^MEM9._LEN, LEN9)    // Length
        
                Store(MABL, MIN9)
                Store(MALL, LEN9)
                Store(LEN9,Local0)
                Add(MIN9, Decrement(Local0), MAX9)
                
        //END 64bit                
                
                Return(CRS2)
            

    } // end ROOT BRIDGE PCIx._CRS Method
//----------------------------------------------------------------------------

//****************************************************************************
//****************************************************************************
//**                                                                        **
//**         (C)Copyright 1985-2019, American Megatrends, Inc.              **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**          5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
