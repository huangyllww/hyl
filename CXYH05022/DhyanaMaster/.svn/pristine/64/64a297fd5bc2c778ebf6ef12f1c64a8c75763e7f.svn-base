//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************

/** @file 
AmiAfuProtocol.h

Header file for the AmiAfuProtocol module

*/
#ifndef _AMI_AFU_Protocol_H_
#define _AMI_AFU_Protocol_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <Protocol/SecSmiFlash.h>

///
/// Miscellaneous
///
#ifndef EFI_FVB_ERASE_POLARITY
#define EFI_FVB_ERASE_POLARITY      0x00000800
#endif
#define MaxBlkSize 65536
#define PAGE_SIZE   1024
#define FWID_SIG_LENGTH 4
#define FWID_LENGTH 9
#define FWROMPROJECTID_LENGTH   5
#define FFS_FIXED_CHECKSUM_1_0 0xAA

///
/// ROM Section
///
#define FLASH_MAIN_BLOCK 0x00000001
#define FLASH_BOOT_BLOCK 0x00000002
#define FLASH_NVRAM 0x00000004
#define FLASH_FULL_ME 0x00000008
#define FLASH_OA3_BLOCK 0x00000010
#define FLASH_ALL_BLOCK FLASH_MAIN_BLOCK | FLASH_BOOT_BLOCK | FLASH_NVRAM

#define FLASH_EC_BLOCK 0x00000020
#define FLASH_NEWER_EC_BLOCK 0x00000040
#define FLASH_NON_CRITICAL_BLOCK 0x00000080 //all non-critical block

#define FLASH_NON_CRITICAL_BLOCK_0  0x00000100
#define FLASH_NON_CRITICAL_BLOCK_1  0x00000200
#define FLASH_NON_CRITICAL_BLOCK_2  0x00000400
#define FLASH_NON_CRITICAL_BLOCK_3  0x00000800
#define FLASH_NON_CRITICAL_BLOCK_4  0x00001000
#define FLASH_NON_CRITICAL_BLOCK_5  0x00002000
#define FLASH_NON_CRITICAL_BLOCK_6  0x00004000
#define FLASH_NON_CRITICAL_BLOCK_7  0x00008000
#define FLASH_NON_CRITICAL_BLOCK_8  0x00010000
#define FLASH_NON_CRITICAL_BLOCK_9  0x00020000
#define FLASH_NON_CRITICAL_BLOCK_10 0x00040000
#define FLASH_NON_CRITICAL_BLOCK_11 0x00080000
#define FLASH_NON_CRITICAL_BLOCK_12 0x00100000
#define FLASH_NON_CRITICAL_BLOCK_13 0x00200000
#define FLASH_NON_CRITICAL_BLOCK_14 0x00400000
#define FLASH_NON_CRITICAL_BLOCK_15 0x00800000


#define FLASH_ROM_HOLE_0    0x000001000000
#define FLASH_ROM_HOLE_1    0x000002000000
#define FLASH_ROM_HOLE_2    0x000004000000
#define FLASH_ROM_HOLE_3    0x000008000000
#define FLASH_ROM_HOLE_4    0x000010000000
#define FLASH_ROM_HOLE_5    0x000020000000
#define FLASH_ROM_HOLE_6    0x000040000000
#define FLASH_ROM_HOLE_7    0x000080000000
#define FLASH_ROM_HOLE_8    0x000100000000
#define FLASH_ROM_HOLE_9    0x000200000000
#define FLASH_ROM_HOLE_10   0x000400000000
#define FLASH_ROM_HOLE_11   0x000800000000
#define FLASH_ROM_HOLE_12   0x001000000000
#define FLASH_ROM_HOLE_13   0x002000000000
#define FLASH_ROM_HOLE_14   0x004000000000
#define FLASH_ROM_HOLE_15   0x008000000000

#define FLASH_ROM_HOLE      0x010000000000 //all rom hole

#define ERASE_BLOCK     0x1000000000000000//The highest nibble used for identifying the action
#define READ_BLOCK      0x2000000000000000
#define WRITE_BLOCK     0x4000000000000000
#define VERIFY_BLOCK    0x8000000000000000

///
///Selection
///
#define BCPALL  0x00000001
#define CLRCFG  0x00000002

///
/// Specify options
///
#define NUMBER_01   0x0001
#define NUMBER_02   0x0002
#define NUMBER_03   0x0004
#define NUMBER_04   0x0008
#define NUMBER_05   0x0010
#define NUMBER_06   0x0020
#define NUMBER_07   0x0040
#define NUMBER_08   0x0080
#define NUMBER_09   0x0100
#define NUMBER_10   0x0200
#define NUMBER_11   0x0400
#define NUMBER_12   0x0800
#define NUMBER_13   0x1000
#define NUMBER_14   0x2000
#define NUMBER_15   0x4000
#define NUMBER_16   0x8000
#define SELECTED_ALL    0xFFFF

///
/// EFI_STATUS of AFU Protocol
///
#define ERROR_FILE_SIZE                 EFIERR(1)
#define ERROR_ROM_ID                    EFIERR(2)
#define ERROR_FILE_CHECKSUM             EFIERR(3)
#define ERROR_BUFFER_ALLOC              EFIERR(4)
#define ERROR_ME_FILE_SIZE              EFIERR(5)

#define ERROR_FLASH_ME                  EFIERR(10)
#define ERROR_FLASH_BOOTBLOCK           EFIERR(11)
#define ERROR_FLASH_MAINBIOS            EFIERR(12)
#define ERROR_FLASH_NVRAM               EFIERR(13)
#define ERROR_FLASH_NCB                 EFIERR(14)
#define ERROR_FLASH_RH                  EFIERR(15)
#define ERROR_FLASH_EC                  EFIERR(16)
#define ERROR_FLASH_OA                  EFIERR(17)

#define ERROR_EC_UNSUPPORT              EFIERR(20)
#define ERROR_NCB_UNSUPPORT             EFIERR(21)
#define ERROR_RH_UNSUPPORT              EFIERR(22)
#define ERROR_ME_UNSUPPORT              EFIERR(23)

#define ERROR_BOOTBLOCK_UNSUPPORT       EFIERR(24)
#define ERROR_MAINBIOS_UNSUPPORT        EFIERR(25)
#define ERROR_NVRAM_UNSUPPORT           EFIERR(26)

#define ERROR_PLDM_UNSUPPORT            EFIERR(27)
#define ERROR_PLDM_LOCATE_PROTOCOL      EFIERR(28)
#define ERROR_PLDM_CREATE_TABLE         EFIERR(29)
#define ERROR_PLDM_EXCEEDS_PLDM_SIZE    EFIERR(30)
#define ERROR_PLDM_PRESERVE_CFG_FAILED  EFIERR(31)
#define ERROR_PLDM_NO_MEDIA             EFIERR(32)
#define ERROR_PLDM_VOLUME_FULL          EFIERR(33)

#define ERROR_SETUP_UNSUPPORT           EFIERR(34)//Error: BIOS doesn't support SETUP preserve function.
#define ERROR_SETUP_STORE               EFIERR(35)//Error: Store SETUP setting error.
#define ERROR_SETUP_RESTORE             EFIERR(36)//Error: Restore SETUP setting error.
#define ERROR_DMI_DATA_STORE            EFIERR(37)//Error: The DMI data size of system is greater than File's DMI data length.
#define ERROR_DMI_STORE                 EFIERR(38)//Error: Store DMI Data error.
#define ERROR_DMI_RESTORE               EFIERR(39)//Error: Restore DMI Data error.

///
/// CallBackExecuteFrequency:
/// Default callback frequency is 125 ms, if caller provides the 0 value.
///
#define CALLBACK_TIMER_Default 125

///
/// // Update flag
///
#define FLAG_DEFAULT 0x00000000
#define PRESERVE_SMBIOS 0x00000001
#define PRESERVE_SETUP 0x00000002

#define AMI_AFU_PROTOCOL_GUID \
    {0xaff347ca,0xaea4,0x4447,0x96,0x8a,0xe6,0xfe,0x75,0x1c,0x24,0xc6}

typedef void (*CallBackFn_T) (UINT64,UINT64,UINT64,UINTN);

/**
 *   The VALIDATE_ROM_FILE() is a API in AfuProtocol Prototype Define
 *
 *  @param RomFileBufAddr   -   Buffer address
 *  @param RomFileSize  -   Buffer size
 *  
 *  @return EFI_STATUS
 */
typedef 
EFI_STATUS
(EFIAPI *VALIDATE_ROM_FILE) (
    IN  UINTN   RomFileBufAddr, 
    IN  UINT32  RomFileSize         
);

/**
 *   The FIRMWARE_UPDATE() is a API in AfuProtocol Prototype Define
 *
 *  @param BufAddr  -   Buffer address
 *  @param FileSize -   Buffer size
 *  @param Section  -    ROM options
 *  @param Selection    -    RomHole / NCB options
 *  @param CallBackFunc -   CallBack function
 *  @param Frequency    -    Frequency
 *  @param UpdateFlag   -    Preserve data
 *  @param ReturnCode   -    Return code
 *  
 *  @return EFI_STATUS
 */
typedef 
EFI_STATUS
(EFIAPI *FIRMWARE_UPDATE) (
    IN  UINTN  FileBufAddr, 
    IN  UINT32  FileSize,           
    IN  UINTN   Section,            
    IN  UINTN   Selection,                  
    IN  CallBackFn_T CallBackFunc,
    IN  UINTN   Frequency,              
    IN  UINT32  UpdateFlag
);

/**
 *   The OEM_ACTIVATION_UPDATE() is a API in AfuProtocol Prototype Define
 *
 *  @param RomFileBufAddr   -   Buffer address
 *  @param RomFileSize  -   Buffer size
 *  @param CallBackFunc -   CallBack function
 *  @param Frequency    -    Frequency
 *  @param ReturnCode   -    Return code
 *  
 *  @return EFI_STATUS
 */
typedef
EFI_STATUS
(EFIAPI *OEM_ACTIVATION_UPDATE) (
    IN  UINTN  OA3FileBufAddr,  
    IN  UINT32  OA3FileSize,            
    IN  CallBackFn_T CallBackFunc,  
    IN  UINTN   Frequency
);

/**
 *   The OEM_ACTIVATION_DELETE() is a API in AfuProtocol Prototype Define
 *
 *  @param CallBackFunc -   CallBack function
 *  @param Frequency    -    Frequency
 *  @param ReturnCode   -    Return code
 *  
 *  @return EFI_STATUS
 */
typedef
EFI_STATUS
(EFIAPI *OEM_ACTIVATION_DELETE) (            
    IN  CallBackFn_T CallBackFunc,  
    IN  UINTN   Frequency
);

/**
 *   The GET_SECURE_FLASH_POLICY() is a API in AfuProtocol Prototype Define
 *
 *  @param FLASH_POLICY_INFO_BLOCK
 *  
 *  @return EFI_STATUS
 */
typedef
EFI_STATUS
(EFIAPI *GET_SECURE_FLASH_POLICY) (
    IN  FLASH_POLICY_INFO_BLOCK         *pFlashPolicy
);

/**
 *   The LOAD_FIRMWARE_IMAGE() is a API in AfuProtocol Prototype Define
 *
 *  @param FUNC_BLOCK
 *  
 *  @return EFI_STATUS
 */
typedef
EFI_STATUS
(EFIAPI *LOAD_FIRMWARE_IMAGE) (
    IN  FUNC_BLOCK          *pFuncBlock         
);

/**
 *   The SET_SECURE_FLASH_METHOD() is a API in AfuProtocol Prototype Define
 *
 *  @param FUNC_FLASH_SESSION_BLOCK
 *  
 *  @return EFI_STATUS
 */
typedef
EFI_STATUS
(EFIAPI *SET_SECURE_FLASH_METHOD)(
    IN  FUNC_FLASH_SESSION_BLOCK            *pFlashSessionBlock
);

/**
*  AMI_AFU_PROTOCOL : AFU Protocol  structure
*/
typedef struct  {
    VALIDATE_ROM_FILE   ValidateRomFile ;   ///< Validate ROM file
    FIRMWARE_UPDATE     BiosUpdate;     ///< Flash BIOS
    OEM_ACTIVATION_UPDATE   OA3Update;      ///< Flash OA3 Block
    OEM_ACTIVATION_DELETE   OA3Delete;      ///< Delete OA3 Block
    GET_SECURE_FLASH_POLICY GetFlashPolicy;     ///< Secure Flash Update
    LOAD_FIRMWARE_IMAGE LoadFirmwareImage;  ///< Secure Flash Update
    SET_SECURE_FLASH_METHOD SetFlashMethod;     ///< Secure Flash Update
}AMI_AFU_PROTOCOL;

 /****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************
