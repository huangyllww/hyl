/** @file OemPort80.h
    Header file for string functionalities.

**/

#ifndef _OEM_Port80_H_
#define	_OEM_Port80_H_

#include <AmiStatusCodes.h>  
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <AmiDxeLib.h>
#include <OemPostCodeEx.h>

typedef VOID (CHECKPOINT_FUNCTION)(IN VOID *PeiServices, IN UINT16 Checkpoint);
#define STATUS_CODE_TYPE(Type) ((Type)&EFI_STATUS_CODE_TYPE_MASK)

typedef struct
{
  EFI_STATUS_CODE_VALUE Value;
  UINT8 Byte;
} STATUS_CODE_TO_BYTE_MAP;


STATUS_CODE_TO_BYTE_MAP OemCheckPointStatusCodes[] =
{
//PEI
        { PEI_CORE_STARTED, 0x10 },
        { PEI_CAR_CPU_INIT, 0x11 },
        // reserved for CPU 0x12 - 0x14
        { PEI_CAR_NB_INIT, 0x15 },
        // reserved for NB 0x16 - 0x18
        { PEI_CAR_SB_INIT, 0x19 },
        // reserved for SB 0x1A - 0x1C
        // reserved for OEM use: 0x1D - 0x2A
        { PEI_MEMORY_SPD_READ, 0x2B },
        { PEI_MEMORY_PRESENCE_DETECT, 0x2C },
        { PEI_MEMORY_TIMING, 0x2D},
        { PEI_MEMORY_CONFIGURING, 0x2E },
        { PEI_MEMORY_INIT, 0x2F },
        // reserved for AML use: 0x30
        { PEI_MEMORY_INSTALLED, 0x31 },
        { PEI_CPU_INIT,  0x32 },
        { PEI_CPU_CACHE_INIT, 0x33 },
        { PEI_CPU_AP_INIT, 0x34 },
        { PEI_CPU_BSP_SELECT, 0x35 },
        { PEI_CPU_SMM_INIT, 0x36 },
        { PEI_MEM_NB_INIT, 0x37 },
        // reserved for NB 0x38 - 0x3A
        { PEI_MEM_SB_INIT, 0x3B },
        // reserved for SB 0x3C - 0x3E
        // reserved for OEM use: 0x3F - 0x4E
        { PEI_DXE_IPL_STARTED, 0x4F },
        { DXE_CORE_STARTED, 0x60 },
    //Recovery
        { PEI_RECOVERY_AUTO, 0xF0 },
        { PEI_RECOVERY_USER, 0xF1 },
        { PEI_RECOVERY_STARTED, 0xF2 },
        { PEI_RECOVERY_CAPSULE_FOUND, 0xF3 },
        { PEI_RECOVERY_CAPSULE_LOADED, 0xF4 },
    //S3
        { PEI_S3_STARTED, 0xE0 },
        { PEI_S3_BOOT_SCRIPT, 0xE1 },
        { PEI_S3_VIDEO_REPOST, 0xE2 },
        { PEI_S3_OS_WAKE, 0xE3 },
        { EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEI_PC_OS_WAKE, 0xE3 }, //Signaled by new EDK II S3Resume module

//DXE
        { DXE_CORE_STARTED, 0x60 },
        { DXE_NVRAM_INIT, 0x61 },
        { DXE_SBRUN_INIT, 0x62 },
        { DXE_CPU_INIT, 0x63 },
        //reserved for CPU 0x64 - 0x67
        { DXE_NB_HB_INIT, 0x68 },
        { DXE_NB_INIT, 0x69 },
        { DXE_NB_SMM_INIT, 0x6A },
        //reserved for NB 0x6B - 0x6F
        { DXE_SB_INIT, 0x70 },
        { DXE_SB_SMM_INIT, 0x71 },
        { DXE_SB_DEVICES_INIT, 0x72 },
        //reserved for SB 0x73 - 0x77
        { DXE_ACPI_INIT, 0x78 },
        { DXE_CSM_INIT, 0x79 },
        //reserved for AMI use: 0x7A - 0x7F
        //reserved for OEM use: 0x80 - 0x8F
        { DXE_BDS_STARTED, 0x90 },
        { DXE_BDS_CONNECT_DRIVERS, 0x91 },
        { DXE_PCI_BUS_BEGIN, 0x92 },
        { DXE_PCI_BUS_HPC_INIT, 0x93 },
        { DXE_PCI_BUS_ENUM, 0x94 },
        { DXE_PCI_BUS_REQUEST_RESOURCES, 0x95 },
        { DXE_PCI_BUS_ASSIGN_RESOURCES, 0x96 },
        { DXE_CON_OUT_CONNECT, 0x97 },
        { DXE_CON_IN_CONNECT, 0x98 },
        { DXE_SIO_INIT, 0x99 },
        { DXE_USB_BEGIN, 0x9A },
        { DXE_USB_RESET, 0x9B },
        { DXE_USB_DETECT, 0x9C },
        { DXE_USB_ENABLE, 0x9D },
        //reserved for AMI use: 0x9E - 0x9F
        { DXE_IDE_BEGIN, 0xA0 },
        { DXE_IDE_RESET, 0xA1 },
        { DXE_IDE_DETECT, 0xA2 },
        { DXE_IDE_ENABLE, 0xA3 },
        { DXE_SCSI_BEGIN, 0xA4 },
        { DXE_SCSI_RESET, 0xA5 },
        { DXE_SCSI_DETECT, 0xA6 },
        { DXE_SCSI_ENABLE, 0xA7 },
        { DXE_SETUP_VERIFYING_PASSWORD, 0xA8 },
        { DXE_SETUP_START, 0xA9 },
        //reserved for AML use: 0xAA
        { DXE_SETUP_INPUT_WAIT, 0xAB },
        //reserved for AML use: 0xAC
        { DXE_READY_TO_BOOT, 0xAD },
        { DXE_LEGACY_BOOT, 0xAE },
        { DXE_EXIT_BOOT_SERVICES, 0xAF },
        { RT_SET_VIRTUAL_ADDRESS_MAP_BEGIN, 0xB0 },
        { RT_SET_VIRTUAL_ADDRESS_MAP_END, 0xB1 },
        { DXE_LEGACY_OPROM_INIT, 0xB2 },
        { DXE_RESET_SYSTEM, 0xB3 },
        { DXE_USB_HOTPLUG, 0xB4 },
        { DXE_PCI_BUS_HOTPLUG, 0xB5 },
        { DXE_NVRAM_CLEANUP, 0xB6 },
        { DXE_CONFIGURATION_RESET, 0xB7 },
        
        { SW_SMI_ACPI_ENABLE,0xA0 },
        { SW_SMI_ACPI_DISABLE,0xA1 },
        //reserved for AMI use: 0xB8 - 0xBF
        //reserved for OEM use: 0xC0 - 0xCF
        
        //<licm-20210226 VGA and UART Display 16bit Port80 SUPPORT +>
        { PORT80_OEM_CPLD, 0x5C },
        { PORT80_OEM_DEBUG, 0xCD },
        //<licm-20210226 VGA and UART Display 16bit Port80 SUPPORT ->
//Error Check Point
        //Errors
        //Regular boot
        { PEI_MEMORY_INVALID_TYPE, 0x50 },
        { PEI_MEMORY_INVALID_SPEED, 0x50 },
        { PEI_MEMORY_SPD_FAIL, 0x51 },
        { PEI_MEMORY_INVALID_SIZE, 0x52 },
        { PEI_MEMORY_MISMATCH, 0x52 },
        { PEI_MEMORY_NOT_DETECTED, 0x53 },
        { PEI_MEMORY_NONE_USEFUL, 0x53 },
        { PEI_MEMORY_ERROR, 0x54 },
        { PEI_MEMORY_NOT_INSTALLED, 0x55 },
        { PEI_CPU_INVALID_TYPE, 0x56 },
        { PEI_CPU_INVALID_SPEED, 0x56 },
        { PEI_CPU_MISMATCH, 0x57 },
        { PEI_CPU_SELF_TEST_FAILED, 0x58 },
        { PEI_CPU_CACHE_ERROR, 0x58 },
        { PEI_CPU_MICROCODE_UPDATE_FAILED, 0x59 },
        { PEI_CPU_NO_MICROCODE, 0x59 },
        { PEI_CPU_INTERNAL_ERROR, 0x5A },
        { PEI_CPU_ERROR, 0x5A },
        { PEI_RESET_NOT_AVAILABLE,0x5B },
            //reserved for AMI use: 0x5C - 0x5F
        //Recovery
        { PEI_RECOVERY_PPI_NOT_FOUND, 0xF8 },
        { PEI_RECOVERY_NO_CAPSULE, 0xF9 },
        { PEI_RECOVERY_INVALID_CAPSULE, 0xFA },
            //reserved for AMI use: 0xFB - 0xFF
        //S3 Resume
        { PEI_MEMORY_S3_RESUME_FAILED, 0xE8 },
        { PEI_S3_RESUME_PPI_NOT_FOUND, 0xE9 },
        { PEI_S3_BOOT_SCRIPT_ERROR, 0xEA },
        { PEI_S3_OS_WAKE_ERROR, 0xEB },
        //reserved for AMI use: 0xEC - 0xEF    
        { DXE_CPU_ERROR, 0xD0 },
        { DXE_NB_ERROR, 0xD1 },
        { DXE_SB_ERROR, 0xD2 },
        { DXE_ARCH_PROTOCOL_NOT_AVAILABLE, 0xD3 },
        { DXE_PCI_BUS_OUT_OF_RESOURCES, 0xD4 },
        { DXE_LEGACY_OPROM_NO_SPACE, 0xD5 },
        { DXE_NO_CON_OUT, 0xD6 },
        { DXE_NO_CON_IN, 0xD7 },
        { DXE_INVALID_PASSWORD, 0xD8 },
        { DXE_BOOT_OPTION_LOAD_ERROR, 0xD9 },
        { DXE_BOOT_OPTION_FAILED, 0xDA },
        { DXE_FLASH_UPDATE_FAILED, 0xDB },
        { DXE_RESET_NOT_AVAILABLE, 0xDC },
        //#define EFI_DEBUG_CODE 0x00000003
    
    
};
#endif 
