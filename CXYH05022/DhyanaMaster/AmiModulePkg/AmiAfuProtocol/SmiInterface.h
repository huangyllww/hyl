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
SmiInterface.h

This is the header file of smi flash function for X64.

*/
#ifndef _SMIINTERFACE_H_
#define _SMIINTERFACE_H_

#include "Afusysrom.h"

#define FLASH_PAGE_SIZE     0x400
#define FLASH_PAGE_COUNT    0x44
#define FILE_BUF_SIZE       4096
#define SmiBufferBlockSize  4096


#if defined (BIOS_CONFIG_PRESERVE_BIN_SUPPORT) && (BIOS_CONFIG_PRESERVE_BIN_SUPPORT == 1)
#include <PldmSceStruct.h>
#include <AmiPldmInterfaces.h>
#include <AfuCommunication.h>
#include <PldmPreserve.h>
extern EFI_GUID gAmiPreservePldmInterfaceProtocolGuid;
#endif


void        SendSmi( UINT8 SMINumber );
int         EnableFlashIfc();
int         DisableFlashIfc();
EFI_STATUS  EraseBlock( UINT32 BlockRegionOffset, UINT32 BlockRegionSize, UINTN Section, UINTN Frequency, CallBackFn_T CallBackFunc );
EFI_STATUS  WriteBlock( UINT32 BlockRegionOffset, UINT32 BlockRegionSize, UINT8 *DataBuffer, UINTN Section, UINTN Frequency, CallBackFn_T CallBackFunc );
EFI_STATUS  ReadBlock( UINT32 BlockRegionOffset, UINT32 BlockRegionSize, UINT8 *DataBuffer, UINTN Section, UINTN Frequency, CallBackFn_T CallBackFunc );
void        GetBiosInfo();
EFI_STATUS  SmiNCBFlash(UINTN RomSection,UINT8 * pFileRomDataBuffer,CallBackFn_T CallBackFunc,UINTN Frequency);
BOOLEAN     RomHoleCheckFlashAddr(BlockInfo_T* pBlockInfo, LPROMHOLEREC lpInputRHREC);
EFI_STATUS  PrePareFlashBuf(UINT8  *pFlashBuffer, UINT8 *pSysRomDataBuffer, UINT8 *pFileRomDataBuffer, UINTN RomSection, UINT32 UpdateFlag);
EFI_STATUS  SmiBiosUpdate( UINTN RomFileBufAddr, UINT32 RomFileSize, UINTN RomSection, UINTN    Selection, CallBackFn_T CallBackFunc, UINTN Frequency, UINT32 UpdateFlag );
EFI_STATUS  SmiOA3Update( UINTN OAFileBufAddr, UINT32 OAFileSize, CallBackFn_T CallBackFunc, UINTN Frequency );
EFI_STATUS  SmiOA3Delete( CallBackFn_T CallBackFunc, UINTN Frequency );

void        SmiAllocateMemory();
void        SmiFreeMemory();
EFI_STATUS  CreatePldmTable(UINTN Selection, UINT8 FlashOpType);
BOOLEAN     IsSecureFlashSupported( UINT8  Policy);
EFI_STATUS  SecureFlashValidate  ( UINT8 *FileBuffer  ,UINT32 FileSize, UINT32 RomSection);
EFI_STATUS  CheckRomSection(UINTN RomSection, BOOLEAN MEEnabled, UINT32 RomFileSize, UINT32 RomSize);

#endif
