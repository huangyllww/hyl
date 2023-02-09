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
ofbd_cfg.h

This is the header file of ofbd function for X64.

*/
/*----------------------------------------------------------------------------
	OFBD_CFG.H
	On Flash Block Description Common Config Header file.
----------------------------------------------------------------------------*/
#ifndef _OFBD_CFG_H_
#define _OFBD_CFG_H_

#include <basictypes.h>

typedef enum {
	// ---------------------------------------------
	eEID_NO_ERROR = 0,							///< Not an error
	// ---------------------------------------------
	eEID_COMMAND_LINE,							///< 0x01 - Invalid command line arguments
	eEID_NO_BIOS_SUPPORT,						///< 0x02 - BIOS not supporting AFU utilities
	eEID_WRONG_ROM_FILE_SIZE,					///< 0x03 - BIOS ROM file with wrong size
	eEID_WRONG_ROM_FILE_ID,						///< 0x04 - BIOS ROM tag different in file and ROM
	eEID_BOOT_BLOCK	,							///< 0x05 - BOOT BLOCK checksum error
	eEID_NON_CRITICAL_BLOCK	,					///< 0x06 - Unsupported Non-critical block
	eEID_BIOS_CHECKSUM,							///< 0x07 - BIOS modules checksum error
	eEID_INVALID_USER_OPTION,					///< 0x08 - Invalid user options in BIOS flashing
	eEID_NOT_MATCH_ROM_SIZE,					///< 0x09 - Size of ROM file does not match the size of system ROM
	eEID_FAIL_UPDATE_HOLE,						///< 0x0A - Unable to update ROM hole
	eEID_ROMHOLE_NOT_EXIST,                     ///< 0x0B - ROMHOLE not exist
	eEID_UPDATE_CANCEL_BY_USER,                 ///< 0x0C - BIOS update cancelled by user.
	eEID_OFBD_OEM_ERROR,                        ///< 0x0D - BIOS Report Error.

	eEID_DRIVER_NOT_KERNEL_SOURCE,              ///< 0x0E - Error: Kernel source files cannot be found.
	eEID_DRIVER_LOAD = 0x10,					///< 0x10 - Error: loading flash device driver
	eEID_DRIVER_UNLOAD,							///< 0x11 - Error: unloading flash device driver
	eEID_NOT_FOUND_NCB,							///< 0x12 - Invalid Non Critical Block request by user
	eEID_INVALID_NCB,							///< 0x13 - Requested non-critical block not available in ROM file
	eEID_NOT_MATCH_NCB,							///< 0x14 - Non-critical blocks in ROM image file do not match those in the system

	eEID_SECURE_FLASH_SUPPORT,					///< 0X15 - Secure Flash function is not supported on this platform
	eEID_SECURE_FLASH_POLICY_GET,				///< 0x16 - Unable to get Secure Flash policy from BIOS
	eEID_SECURE_FLASH_POLICY_MATCH,				///< 0x17 - Unsupported Secure Flash policy
	eEID_SECURE_FLASH_SESSION_START,			///< 0x18 - Error: Secure Flash Rom Verify fail.
	eEID_SECURE_FLASH_RUNTIME_ERASE,			///< 0x19 - Failed to erase flash chip (at Runtime Secure Flash)
	eEID_SECURE_FLASH_RUNTIME_UPDATE,			///< 0x1A - Failed to update flash chip (at Runtime Secure Flash)
	eEID_SECURE_FLASH_RUNTIME_READ,				///< 0x1B - Failed to read flash chip (at Runtime Secure Flash)
	eEID_SECURE_FLASH_RUNTIME_VERIFY,			///< 0x1C - Failed to verify flash chip (at Runtime Secure Flash)
	eEID_SECURE_FLASH_LOAD_IMAGE,				///< 0x1D - Failed to load image into memory
	eEID_SECURE_FLASH_SUPPORT_FILE,             ///< 0x1E - Secure Flash function is not supported on this file
	eEID_SECURE_FLASH_RESERVE_1,		        ///< 0x1F - Reserved for Secure Flash.

	eEID_MEM_MGR_INIT = 0x20,					///< 0x20 - Error: Unable to initialize memory manager.
	eEID_MEM_MGR_EXIT,							///< 0x21 - Error: Unable to close memory manager.
	eEID_MEM_ALLOC,								///< 0x22 - Error: Problem allocating memory.
	eEID_MEM_FREE,								///< 0x23 - Error: Problem freeing memory.
	eEID_BIOS_BUFFER_ALLOC,						///< 0x24 - Error: Problem allocating BIOS buffer.
	eEID_BIOS_BUFFER_FREE,						///< 0x25 - Error: Problem freeing BIOS buffer.
	eEID_BIOS_MAPPING,							///< 0x26 - Error: Problem freeing mapping BIOS.
	eEID_BIOS_UNMAPPING,						///< 0x27 - Error: Problem freeing unmapping BIOS.
	eEID_BIOS_DATA_MAPPING,						///< 0x28 - Error: Problem mapping BIOS data.
	eEID_BIOS_DATA_UNMAPPING,					///< 0x29 - Error: Problem unmapping BIOS data.

	eEID_OPEN_IN_FILE = 0x30,					///< 0x30 - Error: Problem opening file for reading.
	eEID_READ_FILE,								///< 0x31 - Error: Problem reading file.
	eEID_OPEN_OUT_FILE,							///< 0x32 - Error: Problem opening file to write.
	eEID_WRITE_FILE,							///< 0x33 - Error: Problem writting file.
	eEID_PLATFORM_APTIOIV,                      ///< 0x34 - Error: Using the wrong AFU version, Please use Aptio 4 AFU.
	eEID_PLATFORM_APTIOV,                       ///< 0x35 - Error: Using the wrong AFU version, Please use Aptio 5 AFU.

	eEID_FLASH_INTERFACE_OPEN = 0x40,			///< 0x40 - Error: BIOS is write-protected.
	eEID_FLASH_INTERFACE_CLOSE,					///< 0x41 - Error: Can not close flash interface.
	eEID_READ_FLASH,							///< 0x42 - Error: Problem reading flash.
	eEID_ERASE_FLASH,							///< 0x43 - Error: Problem erasing flash.
	eEID_WRITE_FLASH,							///< 0x44 - Error: Problem writting flash.
	eEID_VERIFY_FLASH,							///< 0x45 - Error: verifying BIOS flash
	eEID_FLASH_GET_INFO,						///< 0x46 - Error: in getting flash information
	eEID_NO_FIRMWAREID,							///< 0x47 - Error: No firmware id.
	eEID_POWER_NOT_CONNECTED,					///< 0x48 - Error: Power cord not connected. Plug in power cord to flash.
	eEID_FLASH_NOT_ALLOWED,						///< 0x49 - Error: A platform condition has prevented flashing.
	eEID_WRITE_ONCE_DATA_ERROR,					///< 0x4A - Error: Platform data is not empty, And data address is not Alignment Block Address.	
	eEID_SLPKEY_DATA_ERROR,						///< 0x4b - Error: SLP key is not empty at all.

// DOS specific errors 0x5X
	eEID_DOS_NO_WIN_EXEC = 0x50,				///< 0x50 - Error: This program must be run in MS-DOS mode.

// WIN specific errors 0x6X
	eEID_WIN_REGISTRY = 0x60,					///< 0x60 - Error: Accessing registry.
	eEID_WINDOWS_TWO_INSTANCES,                 ///< 0x61 - Error: Program already running.

// Linux and BSD specific errors 0x7X
	eEID_BSD_ACCESS_IO = 0x70,					///< 0x70 - Error: BSD access IO.

// comparision errors
	eEID_ROM_SIZE_MISMATCH = 0x80,				        ///< 0x80 - Error: Size of system ROM mismatches size of ROM file
	eEID_ROM_ID_MISMATCH,						        ///< 0x81 - Error: ROM ID mismatch.
	eEID_BOOTCHECKSUM_MISMATCH,					        ///< 0x82 - Error: Bootblock checksum error.
	
	
	eEID_SHUTDOWN_FAIL = 0x90,					        ///< 0x90 - Error: Error to shutdown.
	eEID_REBOOT_FAIL,							        ///< 0x91 - Error: Error to restart...
	eEID_COMMAND_LINE_OLD_STYLE_OPEN_ROMID_FILE,        ///< 0x92 - Error: Can't open ROM ID file.
	eEID_COMMAND_LINE_OLD_STYLE_ROMID_FILE_WRONG_SIZE,  ///< 0x93 - Error: ROM ID file is not a ROM file.
	eEID_COMMAND_LINE_BAD_MAC_ADDRESS,                  ///< 0x94 - Error: Invalid MAC address.
	eEID_INVALID_LOAD_CMOS_OPTION,                      ///< 0x95 - Error: Invalid load current CMOS option.
	eEID_COMMAND_LINE_BAD_RETRY_COUNT,                  ///< 0x96 - Error: Invalid retry count.
	eEID_INVALID_ROMID_DEF_LENGTH,                      ///< 0x97 - Error: Invalid defined ROM ID length.
	eEID_COMMAND_LINE_BAD_SMI_ADDRESS,                  ///< 0x98 - Error: Invalid SMI.
	eEID_INEXIST_ROM_FILE_ID,                           ///< 0x99 - Error: ROM File ID don't exist.
	eEID_INEXIST_SYSTEM_ROM_ID,                         ///< 0x9A - Error: System ROM ID don't exist.
	// Password Check
	eEID_PASSWORD_ERROR,                        ///< 0x9B - Error: Password Retry count exceeded
	//SETUP preserve
	eEID_SETUP_NOT_SUPPORT,                     ///< 0x9C - Error: BIOS don't support SETUP preserve funtion.
	eEID_SETUP_STORE_ERROR,                     ///< 0x9D - Error: Store SETUP setting error.
	eEID_SETUP_RESTORE_ERROR,                   ///< 0x9E - Error: Restore SETUP setting error.
	// ME Firmware Update
	eEID_ANALYZE_ROM_IMG,                       ///< 0x9F - Error: Can not analyze ROM file. ROM file may be corrupted.
	
	
	eEID_ANALYZE_ME_DATA = 0xA0,                ///< 0xA0 - Error: Can not analyze the ME Data. ROM file may be corrupted.
	eEID_ME_STOP,                               ///< 0xA1 - Error: BIOS does not support ME Entire Firmware update
	eEID_ME_IGN_STOP,                           ///< 0xA2 - Error: BIOS does not support ME Ignition Firmware update
	// EC Flash Type
	eEID_EC_FILE_ERROR,                         ///< 0xA3 - Error: Invalid EC ROM file.
	eEID_EC_FILE_CHKSUM_ERROR,                  ///< 0xA4 - Error: EC ROM file checksum error.
	eEID_EC_ENTRY_ERROR,                        ///< 0xA5 - Error: Can't entry EC flash mode.
	eEID_EC_ERASE_ERROR,                        ///< 0xA6 - Error: Erasing EC flash memory fail.
	eEID_EC_INIT_ERROR,						    ///< 0xA7 - Error: Initial EC programming fail.
	eEID_EC_START_ERROR,					    ///< 0xA8 - Error: EC flash data transmit error.
	eEID_EC_WRITE_ERROR,					    ///< 0xA9 - Error: Writting EC flash memory fail.
	eEID_EC_EXIT_ERROR,						    ///< 0xAA - Error: Exit EC programming mode fail.
	eEID_EC_CHIPID_ERROR,					    ///< 0xAB - Error: ROM Chip ID mismatch.
	eEID_EC_TABLE_ERROR,					    ///< 0xAC - Error: Invalid EC Header Table.
	eEID_EC_STOP,          					    ///< 0xAD - Error: EC does not permit BIOS update
	// OEM CMD
	eEID_OEM_CMD_NOT_SUPPORT,				    ///< 0xAE - Error: BIOS doesn't support OEMCMD function.
	// DMI Data preserve
	eEID_DMI_STORE_ERROR,					    ///< 0xAF - Error: Store DMI Data error.
	
	
	eEID_DMI_RESTORE_ERROR = 0xB0, 			    ///< 0xB0 - Error: Restore DMI Data error.
	// Activation preservation check
	eEID_OAK_FILE_ERROR,                        ///< 0xB1 - Error: Invalid Activation Key file.
	eEID_OAK_FILE_SIZE_ERROR,                   ///< 0xB2 - Error: File Size is greater than image activation key length.
	eEID_OAK_KEY_LENGTH_ERROR,                  ///< 0xB3 - Error: Image activation key larger than BIOS activation key
	eEID_OAK_CHECKSUM_ERROR,                    ///< 0xB4 - Error: Activation Key checksum error
	eEID_OAK_NO_SUPPORT_ERROR,                  ///< 0xB5 - Error: No Support Activation Key error
	eEID_OAK_DATA_ERROR,                        ///< 0xB6 - Error: OA key is not NULL at all. And OA Key is no the same as Bin File in system.
	eEID_OAD_NULL_DATA_ERROR,                   ///< 0xB7 - Error: OA key is NULL at all already.
	eEID_OAK_REGION_ERROR,                      ///< 0xB8 - Error: OA key region incorrect.
	// Clear event log
	eEID_CLEAR_EVENT_LOG_NOT_SUPPORT,           ///< 0xB9 - Error: BIOS doesn't support Clear event log function
	eEID_CLEAR_EVENT_LOG_ERROR,                 ///< 0xBA - Error: Clear event log error
	//[EIP 59260] if hole layout change  
	eEID_INVALID_HOLE_LAYOUT,					///< 0xBB - Error: Rom image layout detected RomHole is redesigned.
	//[EIP 61679] BIOS has more than one FFS/OEM Hole's GUID is the same   
	eEID_EXIST_SAME_HOLE_GUID,					///< 0xBC - Error: BIOS have more than one RomHole's GUID is the same. 
	eEID_INVALID_HOLE,                          ///< 0xBD - Error: Requested Rom Hole not available in ROM file.
	eEID_NOT_MATCH_HOLE,    					///< 0xBE - Error: Rom Hole in ROM image file do not match those in the system.
	eEID_OAK_DATA_THE_SAME_ERROR,               ///< 0xBF - Error: OA key is not NULL at all.  And OA Key is the same as Bin File in system.
	//0xCx
	// For EIP 63481 +>>>    
	// New SPS ME Update 
	eEID_ME_INFO_NO_SUPPORT = 0xC0,		        ///< 0xC0 - Error: BIOS doesn't support process ME information
	eEID_ME_PROCESS_ERROR,		                ///< 0xC1 - Error: BIOS return error, when tring to re-flash ME Firmware data
	eEID_ME_IS_PROTECT,			                ///< 0xC2 - Error: FDT Region is write-protected
	// For EIP 63481 <<<+
	eEID_EC_NOT_FOUND = 0xC6,                   ///< 0xC6 - Error: No EC blocks found in system ROM.
	//[EIP 75281] BIOS does not support all ROM flash command (/GAN)
	eEID_AllROM_NOT_SUPPORT,                    ///< 0xC7 - Error: BIOS doesn't support all ROM flashing function
	
	//0xDx
	// New Rule for OA Update
	eEID_NOA_DATA_INVALID = 0xD0,			    ///< 0xD0 - Error: OA Update Error.
	eEID_BIOS_ALREADY_UPDATE_NOA,				///< 0xD1 - Error: BIOS already update OA.
	eEID_BIOS_NOT_ALLOW_UPDATE_NOA,				///< 0xD2 - Error: BIOS do not allow to update OA.
	eEID_OFBD_OA_UPDATE_NOT_SUPPORT,			///< 0xD3 - Error: OFBD doesn't support OA Update
	//For EIP [EIP 66385] <<<+

	eEID_DMI_DATA_STORE_ERROR,                  ///< 0xD4 - Error: The DMI data size of system is greater than File's DMI data length.
	eEID_EC_BATTERY_CHECK_NOT_SUPPORT,          ///< 0xD5 - Error: BIOS doesn't support EC Battery Check function.

	eEID_CUSTOM_ID_1 = 0xF0,
	eEID_CUSTOM_ID_2,
	eEID_CUSTOM_ID_3,
	eEID_CUSTOM_ID_4,
	eEID_CUSTOM_ID_5,
	eEID_CUSTOM_ID_6,
	eEID_CUSTOM_ID_7,
	eEID_CUSTOM_ID_8,
	eEID_CUSTOM_ID_9,
	eEID_CUSTOM_ID_A,
	eEID_CUSTOM_ID_B,
	eEID_CUSTOM_ID_C,
	eEID_CUSTOM_ID_D,
	eEID_CUSTOM_ID_E,
	eEID_CUSTOM_ID_F,
	// ---------------------------------------------
	eEID_MAX									///< MAX - The max enumerator ID.
	// ---------------------------------------------
} ENUM_ERR_ID ;

//===================//
// Code 8 OFBD Port //
//==================//
#define OFBD_SWSMI_PORT		0x26
#define OFBD_SIG			"OFBD"
#define	LNX_ROMHOLE_BLOCK	65536	// 64KByte

//==============//
// OFBD Verison //
//==============//
//const WORD	OFBD_VER_1	    = 0x0100; //Major
//const WORD	OFBD_VER_2	    = 0x0200; //Major
//const WORD  	OFBD_VER_21 	= 0x0210; //Major
#define OFBD_VER_1	0x0100 //Major
#define OFBD_VER_2	0x0200 //Major
#define OFBD_VER_21 	0x0210 //Major

//=================//
// On Flash Status //
//=================//
#define	OFBD_FS_VC		BIT00
#define	OFBD_FS_BBLK		BIT01
#define	OFBD_FS_NVRAM		BIT02
#define	OFBD_FS_MAIN		BIT03
#define	OFBD_FS_NCB		BIT04
#define	OFBD_FS_ROMH		BIT05

#define	OFBD_FS_SECURE_FLASH	BIT06
#define OFBD_FS_OA		BIT07    //[NewOACheck]
#define OFBD_FS_AFUMFG		BIT08
//OEM_TAG +>>>Path for Compal EC status support
#define OFBD_FS_AC_BAT		BIT16
//OEM_TAG <<<+
#define OFBD_FS_GET_PAT		BIT17

#define OFBD_FS_ERR_LOG		BIT23
#define	OFBD_FS_OEM_CMD		BIT24
#define	OFBD_FS_SETUP		BIT25
#define	OFBD_FS_MEUD		BIT26
#define	OFBD_FS_EC		BIT27 
#define	OFBD_FS_ORI		BIT28 
#define	OFBD_FS_PWD		BIT29 
#define	OFBD_FS_CFG		BIT30 
#define OFBD_FS_BIOS_CFG_PRESERVE   BIT31 

//===================//
// OFBD Type Code ID //
//===================//
#define	OFBD_EXT_TC_ROMH	 0x50 
#define	OFBD_EXT_TC_CFG		 0x51 
#define	OFBD_EXT_TC_PWD		 0x52 
#define	OFBD_EXT_TC_ORI		 0x53 
#define	OFBD_EXT_TC_EC		 0x54 
#define	OFBD_EXT_TC_MEUD	 0x55 
#define	OFBD_EXT_TC_SETUP	 0x56 
#define	OFBD_EXT_TC_OEM_CMD	 0x57 

#define	OFBD_EXT_TC_CLN_ERR_LOG 		0x58 

#define	OFBD_EXT_TC_OA_HANDLE 			0x5A 
//OEM_TAG +>>>Path for Compal EC status support
#define	OFBD_EXT_TC_AC_BAT	  		0x5B 
//OEM_TAG <<<+
#define	OFBD_EXT_TC_GET_PATTERN			0x5C

#define	OFBD_EXT_TC_SECURE_FLASH	 	0x70 
#define	OFBD_EXT_TC_AFUMFG 			0x71 
#define OFBD_EXT_TC_BIOS_CFG_PRESERVE		0x80

//===================================//
// Type Code General Return Status   //
//===================================//
#define	OFBD_TC_GRS_PROTECT	 0x01 
#define	OFBD_EXT_TBL_END	 0xFFFF55AA 

//===============//
// Return Status //
//===============//
#define	OFBD_RS_SUPPORT			BIT00 
#define	OFBD_RS_PROTECT			BIT01 
#define	OFBD_RS_PROTECT_PART		BIT02 
#define	OFBD_RS_ERR_OUT			BIT03 
#define	OFBD_RS_DIS_OEMSTR		BIT04 

//================//
// OS environment //
//================//
#define OFBD_OS_DOS     BIT00
#define OFBD_OS_WIN     BIT01
#define OFBD_OS_LINUX   BIT02
#define OFBD_OS_FREEBSD BIT03
#define OFBD_OS_EFI     BIT04

//===================================//
// Type Code 50 ROM Hole Description //
//===================================//
#define IIM_ROM_Hole	 BIT00 	// Bit.0 = 1 = It is used for a Rom Hole
#define IIM_NCB_Hole	 BIT01 	// Bit.1 = 1 = It is used for a Non-Critical Block

//============================//
// Type Code 51 Return Status //
//============================//
#define	OFBD_TC_CFG_P			BIT00 
#define	OFBD_TC_CFG_B			BIT01 
#define	OFBD_TC_CFG_N			BIT02 
#define	OFBD_TC_CFG_K			BIT03 
#define	OFBD_TC_CFG_E			BIT04 
#define	OFBD_TC_CFG_Q			BIT05 
#define	OFBD_TC_CFG_REBOOT		BIT06 
#define	OFBD_TC_CFG_X			BIT07 
#define	OFBD_TC_CFG_S			BIT08 
#define	OFBD_TC_CFG_ECUF		BIT09 
#define	OFBD_TC_CFG_SHUTDOWN		BIT10 
#define	OFBD_TC_CFG_SETUPRESERVE	BIT11 
#define	OFBD_TC_CFG_ROMHOLE		BIT12 
#define	OFBD_TC_CFG_NCB			BIT13 
#define OFBD_TC_CFG_CLNEVNLOG		BIT14 
#define OFBD_TC_CFG_R			BIT15         //Patch for EIP 75281
#define OFBD_TC_CFG_O			BIT16         //Patch for EIP 75281
#define OFBD_TC_CFG_GAN			BIT17         //Patch for EIP 75281
#define OFBD_TC_CFG_KN			BIT18         //Lenovo
#define OFBD_TC_CFG_L			BIT19         //Lenovo
#define OFBD_TC_CFG_LN			BIT20         //Lenovo
#define OFBD_TC_CFG_A			BIT21         //
#define OFBD_TC_CFG_OAD			BIT22         //

#define OFBD_TC_CFG_CAPSULE		BIT23         //
#define OFBD_TC_CFG_RECOVERY		BIT24         //
#define OFBD_TC_CFG_CAF			BIT25         //


//=================================//
// Type Code 52 Get & Check Status //
//=================================//
#define	OEM_PC_CK			BIT00 
#define	OEM_PC_PWD_CMP			BIT01 

//============================//
// Type Code 52 Return Status //
//============================//
#define	OEM_RS_PC_REQ			BIT00 
#define	OEM_RS_PC_CK_OK			BIT01 
#define	OEM_RS_PC_TIMES_EXCEEDED	BIT02 

//==============================//
// Type Code 53 Specific Status //
//==============================//
// ORI - Get Check Status. Spec 6.4.1
#define	OFBD_TC_ORI_GET		 BIT00 
#define	OFBD_TC_ORI_CHECK	 BIT01 

// ORI - Return Satus. Spec 6.4.2
#define	OFBD_TC_ORI_REQ		 BIT00 
#define	OFBD_TC_ORI_ROM		 BIT01 
#define	OFBD_TC_ORI_MOD		 BIT02 
#define	OFBD_TC_ORI_GUID	 BIT03 
#define	OFBD_TC_ORI_CK_OK	 BIT04 

//=========================//
// Type Code 54 Set Status //
//=========================//
#define	OFBD_TC_EC_START	 BIT00 
#define	OFBD_TC_EC_FLASH	 BIT01 
#define	OFBD_TC_EC_END		 BIT02 
#define	OFBD_TC_ECX_SIZE	 BIT03 
#define	OFBD_TC_ECX_ERASE	 BIT04 
#define	OFBD_TC_ECX_VERIFY	 BIT05 
#define	OFBD_TC_ECX_RETRY	 BIT06 

//============================//
// Type Code 54 Return Status //
//============================//
#define	OFBD_RS_EC_OK		 BIT00 
#define	OFBD_RS_EC_CHKSUM	 BIT01 
#define	OFBD_RS_EC_ENTRY	 BIT02 
#define	OFBD_RS_EC_ERASE	 BIT03 
#define	OFBD_RS_EC_INIT		 BIT04 
#define	OFBD_RS_EC_START	 BIT05 
#define	OFBD_RS_EC_WRITE	 BIT06 
#define	OFBD_RS_EC_EXIT		 BIT07 
#define	OFBD_RS_EC_CHIPID	 BIT08 

// For EIP 63481 +>>>
//==============================//
// Type Code 55 ME Block Info   //
//==============================//
typedef enum
{
    //Old definition
    FDT_BLK, //Region0 - Descriptor
    PDR_BLK, //Region4 - Platform Data
    GBE_BLK, //Region3 - GbE
    ME_BLK, //Region2 - ME
    ME_OPR_BLK,
    BIOS_BLK, //Region1 - Bios
    DER_BLK, //Region5 - DeviceExpansion1
    //Programming guide definition
    REG7_BLK,
    REG8_BLK, //Region8 - BMC, EC
    REG9_BLK, //Region9 - DeviceExpansion2
    REGA_BLK, //Region10 - IE
    REGB_BLK, //Region11 - 10 GbE
    REGC_BLK, //Region12 - Oprom
    REGD_BLK,
    REGE_BLK,
    REGF_BLK,
    //Confilict with old definition
    BIOS_2ND_BLK, //Region6 - 2nd BIOS, conflict with DER_BLK

    MAX_BLK
} ME_BLOCK_TYPE;

#define	ME_FDT_BLK_IS_PROTECT  0x0001   // Bit 0
#define	ME_PDR_BLK_IS_PROTECT  0x0002   // Bit 1
#define	ME_GBE_BLK_IS_PROTECT  0x0004   // Bit 2
#define	ME_MER_BLK_IS_PROTECT  0x0010   // Bit 3
#define	ME_OPR_BLK_IS_PROTECT  0x0020   // Bit 4

#define	ME_REQUEST_ALLOC_SMM      0x01   // Bit 0
#define	ME_REQUEST_TRANSFER_DATA  0x02   // Bit 1
#define	ME_REQUEST_TRAN_ONLY_ME   0x03   // Only ME (MEUL+Capsule)
#define	ME_REQUEST_UPD_START      0x04   // Bit 2
#define	ME_REQUEST_TRAN_BIOS_ME   0x05   // ME+BIOS (MEUL+Capsule)
#define	ME_REQUEST_CALL_AGAIN     0x08   // Bit 3
#define	ME_REQUEST_FREE_SMM       0x10   // Bit 4
#define	ME_REQUEST_MEUL_UPDATE    0x20   // Bit 5
// For EIP 63481 <<<+


//==============================//
// Type Code 56 Specific Status //
//==============================//
// SETUP - Get Status. Spec 6.4.1
#define	OFBD_TC_SETUP_SUPPORT	 BIT00 
#define	OFBD_TC_SETUP_STORE		 BIT01 
#define	OFBD_TC_SETUP_RESTORE	 BIT02 
#define	OFBD_TC_DMI_STORE		 BIT03 
#define	OFBD_TC_DMI_RESTORE		 BIT04 

// SETUP - Return Satus. Spec 6.4.2
#define	OFBD_TC_SETUP_OK			 BIT00 
#define	OFBD_TC_SETUP_NOT_SUPPORT	 BIT01 
#define	OFBD_TC_SETUP_STORE_ERROR	 BIT02 
#define	OFBD_TC_SETUP_RESTORE_ERROR	 BIT03 
#define	OFBD_TC_DMI_STORE_ERROR		 BIT04 
#define	OFBD_TC_DMI_RESTORE_ERROR	 BIT05 

//====================================//
// Type Code 57 Flash & Return Status //
//====================================//
// OEM CMD - Flash Status.
#define	OFBD_TC_OEM_CMD_BF 		 0x01 
#define	OFBD_TC_OEM_CMD_AF 		 0x02 
#define	OFBD_TC_OEM_CMD_BE 		 0x03 
#define	OFBD_TC_OEM_CMD_SF 		 0x04 
#define	OFBD_TC_OEM_CMD_BUF		 0x05   // For /CMD: + 2010/07/08
#define	OFBD_TC_OEM_CMD_BUF_SF   0x06 
#define	OFBD_TC_OEM_CMD_BUF_AF   0x07 
#define	OFBD_TC_OEM_CMD_BUF_BE   0x08 

// OEM CMD - Return Status.
#define	OFBD_TC_OEM_CMD_OK		BIT00 
#define	OFBD_TC_OEM_CMD_NOT_SUPPORT	BIT01 
#define	OFBD_TC_OEM_CMD_ERROR		BIT02 

//===============================
// Type Code 58 Func Status 
//===============================
#define OFBD_TC_CEL_FUNC_REQ 		BIT00 

//============================
// Type Code 58 Return Status 
//============================
#define OFBD_TC_CEL_OK			BIT00 
#define OFBD_TC_CEL_ERROR  		BIT01 
#define OFBD_TC_CEL_BIOS_NOT_SUPPORT 	BIT02 

//[NewOA] Add for OA Data Handle +>>> 2011/08/01
//===============================
// Type Code 5A Func Status 
//===============================
#define OFBD_TC_OA_UPDATE_NOTIFY 	 0x01 
//============================
// Type Code 5A Return Status 
//============================
#define OFBD_RS_OA_DATA_IS_INVALID 	0x01 
#define OFBD_RS_OA_DATA_IS_MODIFIED	0x02 
#define OFBD_RS_OA_UPDATE_SKIPPED 	0x04 
#define OFBD_RS_OA_UPDATE_DECLINED 	0x08 
#define OFBD_RS_OA_USE_OEM_ERROR_ID	0x8000 
//[NewOA]<++

//OEM_TAG +>>>
//===============================
// Type Code 5B Func Status 
//===============================
#define  OFBD_TC_AC_BAT_INFO_REQ	BIT00 

//============================
// Type Code 5B Return Status 
//============================
#define  OFBD_RS_AC_BAT_OK		BIT00 
#define  OFBD_RS_AC_BAT_ERR		BIT01 
//OEM_TAG <<<+

// OEM_TAG Terry +>>>
//===============================
// Type Code 5C Func Status 
//===============================
#define  OFBD_TC_GET_PAT_REQ		BIT00 
#define  OFBD_TC_GET_PAT_CHK       	BIT01 
#define  OFBD_TC_GET_PAT_NOT_FOUND 	BIT02 
//[EIP250565]
#define  OFBD_TC_GET_PAT_LENGTH_CHK     BIT07 

//============================
// Type Code 5C Return Status 
//============================
#define  OFBD_RS_GET_PAT_OK        	BIT00 
#define  OFBD_RS_GET_PAT_ERROR         	BIT01 
// OEM_TAG Terry <<<+

//====================================//
// Type Code 70 Flash & Return Status //
//====================================//
// SFU - Flash Status.
#define	OFBD_TC_SFU_GET_FLASH_INFO			BIT00 
#define	OFBD_TC_SFU_LOAD_FIRMWARE_IMAGE			BIT01 
#define	OFBD_TC_SFU_GET_FLASH_UPDATE_POLICY		BIT02 
#define	OFBD_TC_SFU_SET_FLASH_UPDATE_METHOD		BIT03 
#define	OFBD_TC_SFU_RUNTIME_FLASH			BIT04 

// SFU - Return Status.
#define	OFBD_TC_SFU_OK					BIT00 
#define	OFBD_TC_SFU_NOT_SUPPORTED			BIT01 
#define	OFBD_TC_SFU_FW_AUTH_FAILED			BIT02 
#define	OFBD_TC_SFU_INVALID_FW_CAPSULE_HDR		BIT03 
#define	OFBD_TC_SFU_SIGNING_KEYS_DONT_MATCH		BIT04 
#define	OFBD_TC_SFU_UNSUPPORTED_CERTIFICATE_HDR_FORMAT	BIT05 

//============================
// Type Code 71 Return Status 
//============================
#define OFBD_TC_AFUMFG_OK			BIT00 
#define OFBD_TC_AFUMFG_ERROR			BIT01 

//========================================//
// Type Code 80 Functions & Return Status //
//========================================//
// Preserve Config - Function commands.
#define OFBD_TC_BCP_SET_BIOS_CFG_PRESERVE_INFO BIT00 
#define OFBD_TC_BCP_LOAD_PLDM_DATA             BIT01 

// Preserve Config - Return Status.
#define OFBD_TC_BCP_RS_OK                      BIT00 
#define OFBD_TC_BCP_RS_NOT_SUPPORTED           BIT01 
#define OFBD_TC_BCP_RS_PRESERVE_CFG_FAILED     BIT02 

//
// Enumerated Flash update methods
//
// FLASH_OP
//
#define	OFBD_SFU_FLASH_OP_DISABLED	BIT00 
#define	OFBD_SFU_FLASH_OP_RECOVERY	BIT01 	// power can be lost, FW Capsule image read from file on the recovery media    
#define	OFBD_SFU_FLASH_OP_CAPSULE	BIT02 	// S3/SW reset with memory intact. FW Capsule resides in Memory
#define	OFBD_SFU_FLASH_OP_RUNTIME	BIT03 	// runtime update using secured SMM Flash protocol

#pragma pack(1)

// OFBD_Descriptor Version 2.0
typedef struct OFBD_Descriptor_Struct_V2 {
	DWORD	ddSignature;
	WORD	dwVersion;
	WORD	dwSize;
	DWORD	ddFlashStatus;
	WORD	dwRetuenStatus;
	WORD	dwHdrSize;
	WORD	dwOS;
} OFBD_STRUCT_V2, *POFBD_STRUCT_V2;

// OFBD_TypeCode Version 2.0
typedef struct OFBD_TypeCode_Struct_V2 {
	BYTE	dbCode;
	BYTE	dbNumHole;
	WORD	dwTCStructSize;
} OFBD_TYPECODE_V2, *POFBD_TYPECODE_V2;

typedef struct OFBD_ExtHeader_End {
	DWORD	ddHdrEnd;
} OFBD_EXTHDR_END, *POFBD_EXTHDR_END;

// OFBD_TC_50_RH_STRUCT
// OFBD_ROMHOLE Version 2.0
typedef struct OFBD_TC_50_RH_STRUCT_V2 {
	UINT32	ddExtCfg;
	UINT32	Data1;
	UINT16	Data2;
	UINT16	Data3;
	UINT8	Data4[8];
} OFBD_ROMHOLE_V2, *POFBD_ROMHOLE_V2;

// OFBD_TC_51_DC_STRUCT
typedef struct OFBD_TC_51_DC_STRUCT {
	DWORD	ddExtCfg;
	DWORD	ddRetSts;
} OFBD_DEFSET, *POFBD_DEFSET;

// DC Command Data struct
typedef struct OFBD_DC_EXDATA_STRUCT {
    DWORD   ddCmdIndex;
    WORD	dwExecuteFlag;      // Use Kn or Ln if you need Execute 
    WORD	dwCancelFlag;       // Use Kn or Ln if you need cancel
} OFBD_DC_EXDATA, *POFBD_DC_EXDATA;

// OFBD_TC_52_PC_STRUCT
typedef struct OFBD_TC_52_PC_STRUCT {
	WORD	dwPwdLen;
	BYTE	dbGetCkSts;
	BYTE	dbRetSts;
} OFBD_PWD, *POFBD_PWD;

// OFBD OEM ROM ID Body Structure
typedef struct OFBD_TC_53_ORI_Struct {
	BYTE	dbGetCkSts;
	BYTE	dbRetSts;
	DWORD	ddStartAddressLow;	// ROM Address Low DWORD in ROM/ Module ID/GUID (EFI)
	DWORD	ddStartAddressHigh;	// ROM Address High DWORD in ROM/ Module ID/GUID (EFI)
	DWORD	ddSize;
} OFBD_ORI_BD_Struct, *POFBD_ORI_BD_Struct;

//OEM ROM ID Checking Structure
/*
typedef struct OFBD_ORI_Struct {
	OFBD_TYPECODE		ExtHdr;
	OFBD_ORI_BD_Struct	ORI_Body;
} OFBD_ORI_Struct, *POFBD_ORI_Struct;
*/

typedef struct OFBD_ORI_Struct_V2 {
	OFBD_TYPECODE_V2	ExtHdr;
	OFBD_ORI_BD_Struct	ORI_Body;
} OFBD_ORI_Struct_V2, *POFBD_ORI_Struct_V2;

// OFBD_TC_54_EC_STRUCT
typedef struct OFBD_TC_54_EC_STRUCT {
	BYTE	dbFlaSts;	// EC Set & Check Status
	WORD	dwRetSts;	// EC Return Status
	DWORD	ddChkSum;	// EC Data CheckSum		// For ECX (OFBD_TC_ECX_SIZE): EC Block Size
	DWORD	ddSize;		// EC Data Size			// For ECX (OFBD_TC_ECX_SIZE): EC Firmware Size
} OFBD_EC, *POFBD_EC;

// EC Header Table Struct
typedef struct OFBD_EC_HDR_STRUCT {
	BYTE	ECSig[7];
	BYTE	ChipHeadFlag;
	WORD	Chip_ID;
	WORD	CheckSum;
	BYTE	CoreVer;
	BYTE	OEMVer;
	BYTE	OEMSubVer;
	BYTE	Reserve_1;
} OFBD_EC_HDR, *POFBD_EC_HDR;

// OFBD_TC_55_MEUD_STRUCT
typedef struct OFBD_TC_55_MEUD_STRUCT {
	BYTE	bReturnStatus;
	BYTE	bSubFunction;
	DWORD	dMERuntimeBase;
	DWORD	dMERuntimeLength;
	DWORD	dMEBiosRegionBase;
	DWORD	dMEBiosRegionLength;
} OFBD_MEUD, *POFBD_MEUD;

typedef struct OFBD_TC_55_MEUD_STRUCT_NEW
{
    BYTE	bReturnStatus;
    BYTE	bSubFunction;
    DWORD	dMERuntimeBase;
    DWORD	dMERuntimeLength;
    DWORD	dMEBiosRegionBase;
    DWORD	dMEBiosRegionLength;
    DWORD	ddFlashBufOffset;
    DWORD	ddBlockAddr;
    DWORD	ddBlockSize;
} OFBD_MEUD_NEW, *POFBD_MEUD_NEW; 

// For EIP 63481 +>>>
typedef struct sMEBlock
{
	BYTE    Command[4];						// Signature - FPR or MFSB or FTPR or MFS or OPR1 or OPR2
	BYTE    Description[64];				// Command's Description
	DWORD	StartAddress;					// starting address of ME block in image
	DWORD	BlockSize;						// size of block
	BYTE	Type;							// ME_BLOCK_TYPE
	BYTE	Lock;							// current block is lock or not
} ME_BLOCK_T;

typedef struct OFBD_TC_55_ME_INFO_STRUCT
{
	BYTE	bReturnStatus;
	BYTE	bSubFunction;       			// 0x09
	WORD    dwTotalBlocks;					// Total no. of blocks in ME image
	ME_BLOCK_T tBlockInfo[MAX_BLK];		    // pointer to ME block info
} OFBD_ME_INFO, *POFBD_ME_INFO; 

typedef struct 	sMEBlkInfo 
{
	DWORD       METotalSize;	// Total Blocks Size
	WORD	    TotalBlocks;	// Total no. of blocks in ME image
	ME_BLOCK_T  BlockInfo;		// pointer to block info
}MEBlkInfo, *PMEBlkInfo;

typedef struct OFBD_TC_55_ME_UPDATE_BY_BIOS_STRUCT
{
	BYTE    bReturnStatus;
	BYTE    bSubFunction;       // SubFunction : 0x0A (For SPS firmware data handle)
	DWORD   TotalBlocks;       	// Total blocks of Update data
	DWORD	BlockIndex;			// Block index of TotalBlocks (4KB unit)
    
	BYTE    bHandleRequest;		// For SubFunction using : 
				// Bit 0: Notify the BIOS to allocate SMM memory
				// Bit 1: Transfer ME data into the BIOS
				// Bit 2: Notify the BIOS start to Update ME region of SPS firmware
				// Bit 3: Respond to BIOS requested to call again
				// Bit 4: Notify the BIOS to free the SMM memory
				// Bit 5~7: Reserved
	
	BYTE	bBlockType;			// ME_BLOCK_TYPE
	
	WORD	UpdateResult;		// Bit 0: Update Error
							// Bit 1: AFU need to call again (If BIOS needs AFU to try again, it will set this Bit is true)
				// Note: AFU will set the Bit 3 of bHanleRequest is true, when BIOS requested to call again
				// Bit 2: Tell the AFU to display the process message (If BIOS need to display any message for user to know)
				// Bit 3~15: Reserved
	
	DWORD	ddMEDataBuffer;		// AFU will use this field to send ME data
	DWORD	ddMEDataSize;		// ddMEDataBuffer's Size
	DWORD	ddMEDataChecksum;	// ddMEDataBuffer's checksum 
	DWORD	ddMessageBuffer;	// For BIOS output the process message
} OFBD_ME_UPDATE, *POFBD_ME_UPDATE;
// For EIP 63481 <<<+

// EIP 354005/377144 +>>>
typedef struct OFBD_TC_55_ME_VERSION_STRUCT
{
    BYTE    bReturnStatus;      // IN:0xff OUT:0x1
    BYTE    bSubFunction;       // SubFunction : 0x0B
    UINT8   FWType;             // SPS or ?
    UINT8   MEVersion;          // 
    UINT8   MEStructVersion;    // 01
    UINT8   AFUVersion;         // 5  5.09.03
    UINT8   AFUSubVersion;      // 9
    UINT8   AFUBuildVersion;    // 3
    UINT32  Reserved[4];    
}OFBD_ME_VERSION, *POFBD_ME_VERSION;
// EIP 354005/377144 <<<+

// OFBD_TC_56_PC_STRUCT
typedef struct OFBD_TC_56_SETUP_STRUCT {
	BYTE	dbGetSts;
	BYTE	dbRetSts;
} OFBD_SETUP, *POFBD_SETUP;

// OFBD_TC_57_OEM_CMD_STRUCT
typedef struct OFBD_TC_57_OEM_CMD_STRUCT {
	WORD	dwFlaSts;	// 0: Before Flash, 1: After Flash, 2: Before End
	WORD	dwRetSts;	// 0: Failure, 1: Successfully
	DWORD	ddCMD;		// Bit 0~15: OEM CMD, For /CMD: Command String Offset
	DWORD	ddReserve;	// For /CMD: Command String Length
} OFBD_OEM_CMD, *POFBD_OEM_CMD;

//=======================================
// Type Code 58 Struct 
//=======================================
typedef struct OFBD_TC_58_CEL_STRUCT
{
	WORD 	dwFunSts;		// 0: Do nothing 1: Clean Event Log
	WORD 	dwRetSts;		// 0: Failure, 1: Successfully
} OFBD_CEL, *POFBD_CEL;

//[NewOA] Add for OA Data Handle +>>> 2011/08/01
//=======================================
// Type Code 5A Struct 
//=======================================
typedef struct OFBD_TC_5A_OA_HANDLE_STRUCT
{
	UINT8 	dbOASts;		// Bit 0: Notification BIOS, utility is now ready to update the OA data.                                - OFBD_TC_OA_UPDATE_NOTIFY
			    // Bit 1~7: Reserved
			    
	UINT8 	dbErrorID;		// 0~255 : Filled by O.E.M.
	
	UINT16 	dwRetSts;		// Bit 0: OA data is invalid, tell the utility stop the flash procedure                                 - OFBD_RS_OA_DATA_IS_INVALID
			    // Bit 1: OA data has been modified, tell the utility use the new data to update                        - OFBD_RS_OA_DATA_IS_MODIFIED
			    // Bit 2: BIOS has updated the OA, so tell the utility doesn't to update                                - OFBD_RS_OA_UPDATE_SKIPPED
			    // Bit 3: BIOS doesn't allow the OA update, tell the utility stop the flash procedure                   - OFBD_RS_OA_UPDATE_DECLINED
			    // Bit 4~14: Reserved
			    // Bit 15: Use dbErrorID field for utility return OEM specific error code, when this Bit is set to 1.   - OFBD_RS_OA_USE_OEM_ERROR_ID
	UINT32  ddOABlockAddr;  // OA Block Address of BIOS ROM (For NCB module to use)
	UINT64  ddOADataBuffer; // OA Data Buffer
	UINT32  dwOADataSize;   // OA Data Buffer Size
} OFBD_NOA, *POFBD_NOA;

//OEM_TAG +>>>Path for Compal EC status support
//=======================================
// Type Code 5B Struct 
//=======================================
typedef struct OFBD_TC_5B_AC_BATTERY_STRUCT
{
	UINT8   dbGetSts;         // Get Status
	UINT8   dbRetSts;         // Return Status

} OFBD_ECB, *POFBD_ECB;

//OEM_TAG <<<+

// OEM_TAG Terry +>>>
//=======================================
// Type Code 5C Struct 
//=======================================
typedef struct OFBD_TC_5C_GET_PATTERN_STRUCT
{
	UINT8	FunSts;         		// Function Status
	UINT8	RetSts;         		// Return Status
	UINT16	SignatureDataSize;		// The specific Signature data size range that you want the AFU to capture.
    								// # 1. If the BIOS does not fill size range, that AFU default will get 1K data buffer size
									// # 2. Maximum size is 32 KB
									// # 3. BIOS can get data buffer pointer from OFBD_EXT_TBL_END
	UINT8	PatternSignature[64];	// The specific Signature value that you want the AFU to search entire Rom File
} OFBD_GETPAT, *POFBD_GETPAT;
// OEM_TAG Terry <<<+

// OFBD_TC_70_SECURE_FLASH_STRUCT
//=======================================
// Type Code 70 Struct 
//=======================================
typedef struct OFBD_TC_70_SFU_STRUCT {
	UINT8	Command;
	UINT8	Status;
} OFBD_SECURE_FLASH, *POFBD_SECURE_FLASH;

// OFBD_TC_71_AFUMFG_STRUCT
//=======================================
// Type Code 70 Struct 
//=======================================
typedef struct _AFUMFG_COMMAND_T{
	UINT8		Id;
	CHAR		Command[16];
	CHAR		Description[256];
}AFUMFG_COMMAND_T;

typedef struct OFBD_TC_71_AFUMFG_STRUCT{
	UINT8		Version;
	UINT8       Size;
	UINT8		bReturnStatus;
	UINT16		TotalCommands;
	AFUMFG_COMMAND_T*  pCommandInfo;
}OFBD_AFUMFG, *POFBD_AFUMFG;

// OFBD_TC_80_BIOS_CONFIG_PRESERVE_STRUCT
//=======================================
// Type Code 80 Struct
//=======================================
typedef struct OFBD_TC_80_BCP_STRUCT {
	UINT8    Command;     // BIOS config preserve command
	UINT8    Status;      // Return status
} OFBD_PRESERVE_SETUP_CONFIG, *POFBD_PRESERVE_SETUP_CONFIG;

#pragma pack()

void Ofbd_Desc_Header(PBYTE pMemBuf, DWORD ddFS);
//void Ofbd_Ext_Header(PBYTE pMemBuf, BYTE dbTypeID, BYTE dbNumHole = 1);
//void Ofbd_TC_End_Header(PBYTE pMemBuf, BYTE dbTypeID, BYTE dbNumHole = 1);
void Ofbd_Ext_Header(PBYTE pMemBuf, BYTE dbTypeID, BYTE dbNumHole);
void Ofbd_TC_End_Header(PBYTE pMemBuf, BYTE dbTypeID, BYTE dbNumHole);

ENUM_ERR_ID	Ofbd_VersionCheck();
WORD		Ofbd_GetVer();
PCHAR		Ofbd_GetOEMString();
BOOL            Ofbd_GetRS_ERR_OUT();
BOOL            Ofbd_RS_DIS_OEMSTR();
VOID            Ofbd_SMI_26();

PBYTE 		AllocMem(DWORD Size);
BOOL 		FreeMem(PBYTE MemBlk);

#endif	// _OFBD_CFG_H_
