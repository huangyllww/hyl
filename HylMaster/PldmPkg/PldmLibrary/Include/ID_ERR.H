#ifndef __ERR_ID__
#define __ERR_ID__
#define ERROR_GET_NVRAM_VARIABLE         2L
/* Command ID Enumerator
    this should move global */
typedef enum {
    // ---------------------------------------------
    eEID_NO_ERROR =0,          // start of error
    // ---------------------------------------------
	eEID_COMMAND_LINE,							//0x01  //Invalid command line arguments
	eEID_NO_BIOS_SUPPORT,						//0x02	// BIOS not supporting AFU utilities
	eEID_WRONG_ROM_FILE_SIZE,					//0x03	// BIOS ROM file with wrong size
	eEID_WRONG_ROM_FILE_ID,						//0x04	// BIOS ROM tag different in file and ROM
	eEID_BOOT_BLOCK	,							//0x05	// BOOT BLOCK checksum error
	eEID_NON_CRITICAL_BLOCK	,					//0x06	// Unsupported Non-critical block
	eEID_BIOS_CHECKSUM,							//0x07	// BIOS modules checksum error
	eEID_INVALID_USER_OPTION,					//0x08	//Invalid user options in BIOS flashing
	eEID_NOT_MATCH_ROM_SIZE,					//0x09	//Size of ROM file does not match the size of system ROM
	eEID_FAIL_UPDATE_HOLE,						//0x0A	//Unable to update ROM hole

	eEID_DRIVER_LOAD = 0x10,					//0x10	// Error loading flash device driver
	eEID_DRIVER_UNLOAD,							//0x11	// Error unloading flash device driver
	eEID_NOT_FOUND_NCB,							//0x12	// Invalid Non Critical Block request by user
	eEID_INVALID_NCB,							//0x13	//Requested non-critical block not available in ROM file
	eEID_NOT_MATCH_NCB,							//0x14	//Non-critical blocks in ROM image file do not match those in the system
	
	eEID_MEM_MGR_INIT = 0x20,					//0x20	// Error initializing memory manager
	eEID_MEM_MGR_EXIT,							//0x21	// Error closing memory manager
	eEID_MEM_ALLOC,								//0x22	// Error allocating memory
	eEID_MEM_FREE,								//0x23	// Error releasing memory
	eEID_BIOS_BUFFER_ALLOC,						//0x24	// Error allocating memory for BIOS interface
	eEID_BIOS_BUFFER_FREE,						//0x25	// Error releasing memory for BIOS interface
	eEID_BIOS_MAPPING,							//0x26	// Error mapping BIOS code
	eEID_BIOS_UNMAPPING,						//0x27	// Error unmapping BIOS code
	eEID_BIOS_DATA_MAPPING,						//0x28	// Error mapping BIOS data
	eEID_BIOS_DATA_UNMAPPING,					//0x29	// Error unmapping BIOS data

	eEID_OPEN_IN_FILE = 0x30,					//0x30	// Error opening input file
	eEID_READ_FILE,								//0x31	// Error reading input file
	eEID_OPEN_OUT_FILE,							//0x32	// Error opening output file
	eEID_WRITE_FILE,							//0x33	// Error writing output file

	eEID_FLASH_INTERFACE_OPEN = 0x40,			//0x40	// Error opening BIOS flashing interface
	eEID_FLASH_INTERFACE_CLOSE,					//0x41	// Error closing BIOS flashing interface
	eEID_READ_FLASH,							//0x42	// Error reading BIOS flash
	eEID_ERASE_FLASH,							//0x43	// Error erasing BIOS flash
	eEID_WRITE_FLASH,							//0x44	// Error writing BIOS flash
	eEID_VERIFY_FLASH,							//0x45	// Error verifying BIOS flash
	eEID_FLASH_GET_INFO,						//0x46	// Error in getting flash information
	eEID_NO_FIRMWAREID,							//0x47	// Error in getting flash information
	eEID_POWER_NOT_CONNECTED,					//0x48	// Error power cord not plugged in
	eEID_FLASH_NOT_ALLOWED,						//0x49	// Firmware has prevented flashing

// DOS specific errors 0x5X
	eEID_DOS_NO_WIN_EXEC = 0x50,				//0x50	// DOS executable cannot run under Windows

// WIN specific errors 0x6X
	eEID_WIN_REGISTRY = 0x60,					//0x60	// Error accessing Windows registry
	eEID_WINDOWS_TWO_INSTANCES,

// Linux and BSD specific errors 0x7X
	eEID_BSD_ACCESS_IO = 0x70,					//0x70	// Error accesing IO driver in BSD

//comparision errors
	eEID_ROM_SIZE_MISMATCH = 0x80,				//0x80
	eEID_ROM_ID_MISMATCH,						//0x81
	eEID_BOOTCHECKSUM_MISMATCH,					//0x82
	eEID_SHUTDOWN_FAIL = 0x90,					//0x90
	eEID_REBOOT_FAIL,							//0x91
	eEID_COMMAND_LINE_OLD_STYLE_OPEN_ROMID_FILE,
    eEID_COMMAND_LINE_OLD_STYLE_ROMID_FILE_WRONG_SIZE,
	eEID_COMMAND_LINE_BAD_MAC_ADDRESS,
	eEID_INVALID_LOAD_CMOS_OPTION,
	eEID_COMMAND_LINE_BAD_RETRY_COUNT,
	eEID_INVALID_ROMID_DEF_LENGTH,
	eEID_COMMAND_LINE_BAD_SMI_ADDRESS,
	eEID_INEXIST_ROM_FILE_ID,
	eEID_INEXIST_SYSTEM_ROM_ID,
	// Password Check
    eEID_PASSWORD_ERROR,
	//SETUP preserve
	eEID_SETUP_NOT_SUPPORT,
	eEID_SETUP_STORE_ERROR,
	eEID_SETUP_RESTORE_ERROR,
    // ME Firmware Update
	eEID_ANALYZE_ROM_IMG,
    eEID_ANALYZE_ME_DATA,                                  
    eEID_ME_STOP,
    eEID_ME_IGN_STOP,
// EC Flash Type 
    eEID_EC_FILE_ERROR,
    eEID_EC_FILE_CHKSUM_ERROR,
    eEID_EC_ENTRY_ERROR,
    eEID_EC_ERASE_ERROR,
    eEID_EC_INIT_ERROR,
    eEID_EC_START_ERROR,
    eEID_EC_WRITE_ERROR,
    eEID_EC_EXIT_ERROR,
    eEID_EC_CHIPID_ERROR,
    eEID_EC_TABLE_ERROR,
    eEID_EC_STOP,                                           

    // ---------------------------------------------
    eEID_MAX          // THIS MUST BE THE LAST!!
    // ---------------------------------------------
} ENUM_ERR_ID ;
#endif

