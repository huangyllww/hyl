//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
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
// $Header: $
//
// $Revision: $
//
// $Date: $
//**********************************************************************

//*************************************************************************
//<AMI_FHDR_START>
//
// Name:        CrbFeature.asl
//
// Description: CRB related feature asl code
//
// Notes:
//
//<AMI_FHDR_END>
//*************************************************************************

Scope (\_GPE)
{
    External(\_SB.ALIB, MethodObj)

    Method(_L0A, 0, NotSerialized) {
        DOCK()
        Store(Not(GE10), E10C)
    }

    Method(_INI, 0, NotSerialized)      {
        // NFIT Health Event Notification
        \_SB.NVDN()
        DOCK()
    }

    Name(DOKB, Buffer(0x03){})
    Method(DOCK, 0, NotSerialized) {
        CreateWordField(DOKB, Zero, BSZE)
        CreateByteField(DOKB,0x02,DKST)
        Store(0x03, BSZE)
        Sleep(0x02)
        If(GE10)
        {
            Store(One, DKST)
            Store(0xF0D1, DBG8)
        }
        Else
        {
            Store(Zero, DKST)
            Store(0xF0D2, DBG8)
        }
        \_SB.ALIB(0x0A,DOKB)
    }

}


//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************