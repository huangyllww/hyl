//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//**********************************************************************
//
// $Header: /Alaska/Projects/AMD/AgesaV5/CarrizoLitePI/Gardenia_1ASGR/CRB/CRBUSB.ASL 2     3/28/14 5:35a Artieliu $
//
// $Revision: 2 $
//
// $Date: 3/28/14 5:35a $
//
//**********************************************************************
// Revision History
// ----------------
// $Log: /Alaska/Projects/AMD/AgesaV5/CarrizoLitePI/Gardenia_1ASGR/CRB/CRBUSB.ASL $
// 
// 2     3/28/14 5:35a Artieliu
// 
// 1     3/27/14 3:58a Artieliu
// 
// 1     3/19/14 3:18a Artieliu
// 
//
//**********************************************************************
//;<AMI_PHDR_START>
//;------------------------------------------------------------------------/
//;
//; Procedure:    CRB USB asl code
//;
//;-------------------------------------------------------------------------
//;<AMI_PHDR_END>

//Sync with AMD demo BIOS
    Name(XUPC, Package(4) {0xFF, 0xFF, Zero, Zero})
    Name(XUP1, Package(4) {Zero, 0xFF, Zero, Zero})
    Name(XPLD, Buffer(0x10)
    {
        0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    })
    Name(NPLD, Zero)
    Name(WPLD, Package(1)
    {
        Buffer(0x14)
        {
            0x82, 0x00, 0x00, 0x00, 0x05, 0x00, 0x05, 0x00, 
            0x25, 0x09, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
            0x2C, 0x01, 0xBE, 0x00
        }
    })
    Name(DPLD, Package(1)
    {
        Buffer(0x10)
        {
            0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
            0xB0, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        }
    })
    Name(BPLD, Package(1)
    {
        Buffer(0x14)
        {
            0x82, 0x00, 0x00, 0x00, 0x64, 0x00, 0x64, 0x00, 
            0x24, 0x41, 0x80, 0x01, 0x00, 0x00, 0x04, 0x00, 
            0x00, 0x03, 0xAB, 0x02
        }
    })
    Name(CPLD, Package(1)
    {
        Buffer(0x14)
        {
            0x82, 0x00, 0x00, 0x00, 0x05, 0x00, 0x05, 0x00, 
            0x25, 0x1D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
            0xFF, 0xFF, 0xFF, 0xFF
        }
    })


Scope(\_SB.PCI0.XHC0)
{
    Method (RHRS, 0, NotSerialized)
    {
        Name (RBUF, ResourceTemplate ()
        {
            GpioInt (Level, ActiveHigh, Exclusive, PullNone, 0x0000,
                "\\_SB.GPIO", 0x00, ResourceConsumer, ,
                )
                {   // Pin list
                    0x003A
                }
        })
        Return (RBUF)
    }

    Device (RHUB)
    {
        Name (_ADR, Zero)  // _ADR: Address
        Device (PRT1)
        {
            Name (_ADR, One)  // _ADR: Address
            Name (_UPC, Package (0x04)  // _UPC: USB Port Capabilities
            {
                0xFF, 
                0x03, 
                Zero, 
                Zero
            })
            Name (_PLD, Package (0x01)  // _PLD: Physical Location of Device
            {
                Buffer (0x14)
                {
                    /* 0000 */   0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    /* 0008 */   0x11, 0x0C, 0x80, 0x00, 0x01, 0x00, 0x00, 0x00,
                    /* 0010 */   0xFF, 0xFF, 0xFF, 0xFF
                    /*           Revision : 02     */
                    /*        IgnoreColor : 01     */
                    /*              Color : 000000 */
                    /*              Width : 0000   */
                    /*             Height : 0000   */
                    /*        UserVisible : 01     */
                    /*               Dock : 00     */
                    /*                Lid : 00     */
                    /*              Panel : 02     */
                    /*   VerticalPosition : 00     */
                    /* HorizontalPosition : 00     */
                    /*              Shape : 03     */
                    /*   GroupOrientation : 00     */
                    /*         GroupToken : 00     */
                    /*      GroupPosition : 01     */
                    /*                Bay : 00     */
                    /*          Ejectable : 01     */
                    /*  OspmEjectRequired : 00     */
                    /*      CabinetNumber : 00     */
                    /*     CardCageNumber : 00     */
                    /*          Reference : 00     */
                    /*           Rotation : 00     */
                    /*              Order : 00     */
                    /*     VerticalOffset : FFFF   */
                    /*   HorizontalOffset : FFFF   */
                }
            })
        }

        Device (PRT2)
        {
            Name (_ADR, 0x02)  // _ADR: Address
            Name (_UPC, Package (0x04)  // _UPC: USB Port Capabilities
            {
                0xFF, 
                0x03, 
                Zero, 
                Zero
            })
            Name (_PLD, Package (0x01)  // _PLD: Physical Location of Device
            {
                Buffer (0x14)
                {
                    /* 0000 */   0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    /* 0008 */   0x91, 0x0C, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
                    /* 0010 */   0xFF, 0xFF, 0xFF, 0xFF
                    /*           Revision : 02     */
                    /*        IgnoreColor : 01     */
                    /*              Color : 000000 */
                    /*              Width : 0000   */
                    /*             Height : 0000   */
                    /*        UserVisible : 01     */
                    /*               Dock : 00     */
                    /*                Lid : 00     */
                    /*              Panel : 02     */
                    /*   VerticalPosition : 02     */
                    /* HorizontalPosition : 00     */
                    /*              Shape : 03     */
                    /*   GroupOrientation : 00     */
                    /*         GroupToken : 00     */
                    /*      GroupPosition : 02     */
                    /*                Bay : 00     */
                    /*          Ejectable : 01     */
                    /*  OspmEjectRequired : 00     */
                    /*      CabinetNumber : 00     */
                    /*     CardCageNumber : 00     */
                    /*          Reference : 00     */
                    /*           Rotation : 00     */
                    /*              Order : 00     */
                    /*     VerticalOffset : FFFF   */
                    /*   HorizontalOffset : FFFF   */
                }
            })
        }

        Device (PRT3)
        {
            Name (_ADR, 0x03)  // _ADR: Address
            Name (_UPC, Package (0x04)  // _UPC: USB Port Capabilities
            {
                0xFF, 
                0x03, 
                Zero, 
                Zero
            })
            Name (_PLD, Package (0x01)  // _PLD: Physical Location of Device
            {
                Buffer (0x14)
                {
                    /* 0000 */   0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    /* 0008 */   0x91, 0x1C, 0x80, 0x01, 0x01, 0x00, 0x00, 0x00,
                    /* 0010 */   0xFF, 0xFF, 0xFF, 0xFF
                    /*           Revision : 02     */
                    /*        IgnoreColor : 01     */
                    /*              Color : 000000 */
                    /*              Width : 0000   */
                    /*             Height : 0000   */
                    /*        UserVisible : 01     */
                    /*               Dock : 00     */
                    /*                Lid : 00     */
                    /*              Panel : 02     */
                    /*   VerticalPosition : 02     */
                    /* HorizontalPosition : 00     */
                    /*              Shape : 07     */
                    /*   GroupOrientation : 00     */
                    /*         GroupToken : 00     */
                    /*      GroupPosition : 03     */
                    /*                Bay : 00     */
                    /*          Ejectable : 01     */
                    /*  OspmEjectRequired : 00     */
                    /*      CabinetNumber : 00     */
                    /*     CardCageNumber : 00     */
                    /*          Reference : 00     */
                    /*           Rotation : 00     */
                    /*              Order : 00     */
                    /*     VerticalOffset : FFFF   */
                    /*   HorizontalOffset : FFFF   */
                }
            })
        }

        Device (PRT4)
        {
            Name (_ADR, 0x04)  // _ADR: Address
            Name (_UPC, Package (0x04)  // _UPC: USB Port Capabilities
            {
                0xFF, 
                0xFF, 
                Zero, 
                Zero
            })
            Name (_PLD, Package (0x01)  // _PLD: Physical Location of Device
            {
                Buffer (0x14)
                {
                    /* 0000 */   0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    /* 0008 */   0x24, 0x1D, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
                    /* 0010 */   0xFF, 0xFF, 0xFF, 0xFF
                    /*           Revision : 02     */
                    /*        IgnoreColor : 01     */
                    /*              Color : 000000 */
                    /*              Width : 0000   */
                    /*             Height : 0000   */
                    /*        UserVisible : 00     */
                    /*               Dock : 00     */
                    /*                Lid : 01     */
                    /*              Panel : 04     */
                    /*   VerticalPosition : 00     */
                    /* HorizontalPosition : 01     */
                    /*              Shape : 07     */
                    /*   GroupOrientation : 00     */
                    /*         GroupToken : 00     */
                    /*      GroupPosition : 04     */
                    /*                Bay : 00     */
                    /*          Ejectable : 00     */
                    /*  OspmEjectRequired : 00     */
                    /*      CabinetNumber : 00     */
                    /*     CardCageNumber : 00     */
                    /*          Reference : 00     */
                    /*           Rotation : 00     */
                    /*              Order : 00     */
                    /*     VerticalOffset : FFFF   */
                    /*   HorizontalOffset : FFFF   */
                }
            })
        }

        Device (PRT5)
        {
            Name (_ADR, 0x05)  // _ADR: Address
            Name (_UPC, Package (0x04)  // _UPC: USB Port Capabilities
            {
                0xFF, 
                0x03, 
                Zero, 
                Zero
            })
            Name (_PLD, Package (0x01)  // _PLD: Physical Location of Device
            {
                Buffer (0x14)
                {
                    /* 0000 */   0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    /* 0008 */   0x11, 0x0C, 0x80, 0x00, 0x01, 0x00, 0x00, 0x00,
                    /* 0010 */   0xFF, 0xFF, 0xFF, 0xFF
                    /*           Revision : 02     */
                    /*        IgnoreColor : 01     */
                    /*              Color : 000000 */
                    /*              Width : 0000   */
                    /*             Height : 0000   */
                    /*        UserVisible : 01     */
                    /*               Dock : 00     */
                    /*                Lid : 00     */
                    /*              Panel : 02     */
                    /*   VerticalPosition : 00     */
                    /* HorizontalPosition : 00     */
                    /*              Shape : 03     */
                    /*   GroupOrientation : 00     */
                    /*         GroupToken : 00     */
                    /*      GroupPosition : 01     */
                    /*                Bay : 00     */
                    /*          Ejectable : 01     */
                    /*  OspmEjectRequired : 00     */
                    /*      CabinetNumber : 00     */
                    /*     CardCageNumber : 00     */
                    /*          Reference : 00     */
                    /*           Rotation : 00     */
                    /*              Order : 00     */
                    /*     VerticalOffset : FFFF   */
                    /*   HorizontalOffset : FFFF   */
                }
            })
        }

        Device (PRT6)
        {
            Name (_ADR, 0x06)  // _ADR: Address
            Name (_UPC, Package (0x04)  // _UPC: USB Port Capabilities
            {
                0xFF, 
                0x03, 
                Zero, 
                Zero
            })
            Name (_PLD, Package (0x01)  // _PLD: Physical Location of Device
            {
                Buffer (0x14)
                {
                    /* 0000 */   0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    /* 0008 */   0x91, 0x0C, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
                    /* 0010 */   0xFF, 0xFF, 0xFF, 0xFF
                    /*           Revision : 02     */
                    /*        IgnoreColor : 01     */
                    /*              Color : 000000 */
                    /*              Width : 0000   */
                    /*             Height : 0000   */
                    /*        UserVisible : 01     */
                    /*               Dock : 00     */
                    /*                Lid : 00     */
                    /*              Panel : 02     */
                    /*   VerticalPosition : 02     */
                    /* HorizontalPosition : 00     */
                    /*              Shape : 03     */
                    /*   GroupOrientation : 00     */
                    /*         GroupToken : 00     */
                    /*      GroupPosition : 02     */
                    /*                Bay : 00     */
                    /*          Ejectable : 01     */
                    /*  OspmEjectRequired : 00     */
                    /*      CabinetNumber : 00     */
                    /*     CardCageNumber : 00     */
                    /*          Reference : 00     */
                    /*           Rotation : 00     */
                    /*              Order : 00     */
                    /*     VerticalOffset : FFFF   */
                    /*   HorizontalOffset : FFFF   */
                }
            })
        }

        Device (PRT7)
        {
            Name (_ADR, 0x07)  // _ADR: Address
            Name (_UPC, Package (0x04)  // _UPC: USB Port Capabilities
            {
                0xFF, 
                0x03, 
                Zero, 
                Zero
            })
            Name (_PLD, Package (0x01)  // _PLD: Physical Location of Device
            {
                Buffer (0x14)
                {
                    /* 0000 */   0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    /* 0008 */   0x91, 0x1C, 0x80, 0x01, 0x01, 0x00, 0x00, 0x00,
                    /* 0010 */   0xFF, 0xFF, 0xFF, 0xFF
                    /*           Revision : 02     */
                    /*        IgnoreColor : 01     */
                    /*              Color : 000000 */
                    /*              Width : 0000   */
                    /*             Height : 0000   */
                    /*        UserVisible : 01     */
                    /*               Dock : 00     */
                    /*                Lid : 00     */
                    /*              Panel : 02     */
                    /*   VerticalPosition : 02     */
                    /* HorizontalPosition : 00     */
                    /*              Shape : 07     */
                    /*   GroupOrientation : 00     */
                    /*         GroupToken : 00     */
                    /*      GroupPosition : 03     */
                    /*                Bay : 00     */
                    /*          Ejectable : 01     */
                    /*  OspmEjectRequired : 00     */
                    /*      CabinetNumber : 00     */
                    /*     CardCageNumber : 00     */
                    /*          Reference : 00     */
                    /*           Rotation : 00     */
                    /*              Order : 00     */
                    /*     VerticalOffset : FFFF   */
                    /*   HorizontalOffset : FFFF   */
                }
            })
        }

        Device (PRT8)
        {
            Name (_ADR, 0x08)  // _ADR: Address
            Name (_UPC, Package (0x04)  // _UPC: USB Port Capabilities
            {
                Zero, 
                0xFF, 
                Zero, 
                Zero
            })
        }
    }
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
