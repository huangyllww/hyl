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
MeInterface.h

This is the header file of ME flash function for X64.

*/
#ifndef _MEINTERFACE_H_
#define _MEINTERFACE_H_

//enum ENUM_ME_FLASH_ID 
typedef enum {
	// 0~2 : For ME Ignition Firmware Use
	// 3~5 : For ME Entire Firmware Use
	// 6~7 : For ME Update to use
	eMFD_IGN_INIT = 0,
	eMFD_IGN_START,
	eMFD_IGN_END,
	eMFD_ME_INIT,
	eMFD_ME_START,
	eMFD_ME_END,
	eMFD_ME_ERASE,
	eMFD_ME_WRITE,
	eMFD_ME_READ,
	eMFD_ME_INFO,
	eMFD_ME_UPDATE,
	eMFD_ME_VERSION  // EIP 354005/377144 +
}ENUM_ME_FLASH_ID;

EFI_STATUS	MERF_Check();
EFI_STATUS 	MERF_Flash(UINTN Frequency, CallBackFn_T CallBackFunc);
UINT32      ME_GetFlashCapacity();
BOOLEAN     MER_GetBlkInfo(BYTE Type, DWORD* pStartAddress, DWORD* pBlockSize);  // EIP 377144 +

#endif
