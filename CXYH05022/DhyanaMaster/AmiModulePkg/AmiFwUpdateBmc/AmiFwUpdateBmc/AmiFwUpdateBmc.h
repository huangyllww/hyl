
#ifndef __AMIFWUPDATEBMC_H
#define __AMIFWUPDATEBMC_H

#include "Token.h"

EFI_STATUS AMIFWUpdateHiiInitializeProtocol(VOID);

typedef enum _FLASH_OPTION
{
    FLASH_OP_MAIN_BLOCK,
    FLASH_OP_BOOT_BLOCK,
    FLASH_OP_NVRAM,
    FLASH_OP_ME,
    FLASH_OP_NCB,
    FLASH_OP_NCBN,
    FLASH_OP_ROM_HOLES,
    FLASH_OP_ROM_HOLESN,
    FLASH_OP_PRESERVE_SMBIOS,
    FLASH_OP_PRESERVE_SETUP,
    FLASH_OP_NO_PRESERVE_BCP,
    FLASH_OP_PRESERVE_BCP_ALL,
    FLASH_OP_REBOOT,
    FLASH_OP_FILENAME,
    FLASH_OP_CAPSULE,
    FLASH_OP_RECOVERY,
    FLASH_OP_BIOS,
    FLASH_OP_OTH,
    FLASH_OP_DRIVE_PATH,
    FLASH_OP_UNKNOWN,
}FLASH_OPTION;

typedef CHAR16* COMMAND_ARG_LIST;

#define FLASH_MAIN_BLOCK 	0x00000001
#define FLASH_BOOT_BLOCK 	0x00000002
#define FLASH_NVRAM 		0x00000004
#define FLASH_FULL_ME 		0x00000008

#define PRESERVE_SMBIOS 	0x00000001
#define PRESERVE_SETUP 		0x00000002

#define FLASHING_REGION     0x00000000000000FF
#define FLASHING_OPERATION  0xF000000000000000

void InitializingString();
void FreeingHIIString();

EFI_STATUS Initialise_AFU_Protocol();

void UpdatingHandle() ;
extern EFI_HANDLE gHiiHandle;
CHAR16 *FW_HiiGetString( VOID* handle, UINT16 token );

typedef enum _FW_UPDATE_BMC_WAIT_STATUS 
{
	ABORT,
	START,
	WAIT
}FW_UPDATE_BMC_WAIT_STATUS;
#define WAIT_TIME_INTERVAL   		   10000000

#endif	//__AMIFWUPDATEBMC_H