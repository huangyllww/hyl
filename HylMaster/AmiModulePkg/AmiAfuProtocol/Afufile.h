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
Afufile.h

This is the header file of AFU generic functions .

*///**********************************************************************
#ifndef _AFUFILE_H_
#define _AFUFILE_H_

#include "basictypes.h"

#define BOOTBLOCK   0x0
#define MAINBIOS    0x1
#define SLP_NUMBER          2

BOOLEAN     AFU_RomFile_SearchNCB(PBYTE lpFilebuff);
UINT8       AFU_RomFile_GetRomHoleNumber();
BOOLEAN     AFU_RomFile_HaveSLP20();

#endif
