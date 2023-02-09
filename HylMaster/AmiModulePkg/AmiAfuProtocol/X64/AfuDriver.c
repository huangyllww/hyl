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
/** @file 
AfuDriver.c

This is the driver file for AmiAfuProtocol module.

*/
#include  "AfuDriver.h"
#include "basictypes.h"

extern UINT32 gSmiMailBoxAddress;
extern UC2SmiMailbox gSmiMailBox;

VOID PortWriteByte(UINT16  Port, UINT8 Value, pUC2RegBuffer RegBuffer);

VOID OutPortByte(UINT16  Port, UINT8 Value, pUC2RegBuffer RegBuffer)
{
    UC2SmiMailbox * pMailBox = (UC2SmiMailbox *)NULL; 

    if(gSmiMailBoxAddress)
    {
        if(Value >= 0x20 && Value <= 0x26)
        {
            pMailBox = (UC2SmiMailbox *)((PBYTE)gSmiMailBoxAddress);
            pMailBox->Token = gSmiMailBox.Token;    
        }
    }
    
	PortWriteByte(Port, Value, RegBuffer);
}
