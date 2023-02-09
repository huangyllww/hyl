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
AfuDriver.h

This is the header file for X64.

*/
#ifndef _AFU_DRIVER_H_
#define _AFU_DRIVER_H_
#include <AmiDxeLib.h>
#include <Token.h>

#include <Protocol/AmiAfuProtocol.h>

#pragma pack(1)
/**
  pUC2RegBuffer : General Purpose Registers Buffer structure
*/
// General Purpose Registers Buffer
typedef struct UC2_GeneralPurposeRegisterBuffer {
	UINT32		Edi;	// 0
	UINT32		Esi;	// 4
	UINT32		Ebp;	// 8
	UINT32		Ebx;	// 12
	UINT32		Edx;	// 16
	UINT32		Ecx;	// 20
	UINT32		Eax;	// 24
} UC2RegBuffer, *pUC2RegBuffer;
#pragma pack()

typedef struct UC2_SmiMailbox{
    UINT64 Revision;
    UINT64 PhysicalAddress;
    UINT32 Status;
    UINT32 Token;
} UC2SmiMailbox, *pUC2SmiMailbox;

VOID OutPortByte(UINT16  Port, UINT8 Value, pUC2RegBuffer RegBuffer);

#endif	// #ifndef _AFU_DRIVER_H_
