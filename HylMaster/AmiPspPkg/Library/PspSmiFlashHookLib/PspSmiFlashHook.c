//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file PspSmiFlashHook.c
    SW SMI hook. 

**/

#include <AmiDxeLib.h>
#include <Protocol/SmiFlash.h>
#include <Token.h>

//----------------------------------------------------------------------------


/**
    This function is SW SMI hook that sets Flash Block Description
    type for AMI AFU utility.

        
    @param SwSmiNum SW SMI value number
    @param Buffer Flash descriptor address

    @retval VOID

**/

#define FLASH_DEVICE_BASE_ADDRESS             (0xFFFFFFFF - FLASH_SIZE + 1)
#define ROMSIG_BASE                           (0xFFFFFFFF - FLASH_SIZE + FCH_FIRMWARE_OFFSET + 1)
#define HYGON_ROMSIG_BLOCK_ADDRESS            (ROMSIG_BASE - FLASH_DEVICE_BASE_ADDRESS)
#define HYGON_ROMSIG_BLOCK_END                (HYGON_ROMSIG_BLOCK_ADDRESS + ROMSIG_SIZE)
// Not supported.
//#define HYGON_PSP_NVRAM_BLOCK_ADDRESS         (PSP_NVRAM_BASE - FLASH_DEVICE_BASE_ADDRESS)
//#define HYGON_PSP_NVRAM_BLOCK_END             (HYGON_PSP_NVRAM_BLOCK_ADDRESS + PSP_NVRAM_SIZE)
#define HYGON_DHYANA_PSP_FIXED_BLOCK_ADDRESS  PSP_DATA_OFFSET
#define HYGON_DHYANA_PSP_FIXED_BLOCK_END      PSP_APOB_SHARED_OFFSET
#define HYGON_DHYANA_PSP_UPD_BLOCK_ADDRESS    HYGON_DHYANA_PSP_FIXED_BLOCK_END
#define HYGON_DHYANA_PSP_UPD_BLOCK_END        (PSP_DATA_OFFSET + PSP_DATA_SIZE)


VOID PspUpdateBlockTypeId (
    IN UINT8  SwSmiNum,
    IN UINT64 Buffer )
{
#if(PSP_USE_HW_AUTH == 1 ) || (PSP_CRISIS_RECOVERY_SUPPORT == 1)
    BLOCK_DESC *BlockDesc;
    UINTN  i;

    // return if SW SMI value is not "Get Flash Info"
    if (SwSmiNum != SMIFLASH_GET_FLASH_INFO) 
        return;
    
    BlockDesc = (BLOCK_DESC*)&((INFO_BLOCK*)Buffer)->Blocks;
#endif
    
#if PSP_USE_HW_AUTH == 1
#if PSP_CRISIS_RECOVERY_SUPPORT == 1
    #error  "PSP Crisis recovery is not supported in PSB mode. Please turn off token PSP_CRISIS_RECOVERY_SUPPORT."
#endif
    // assign Type = BOOT_BLOCK to PSP area 
    // and make it updated in sync with Boot Block
    for (i = 0; i < ((INFO_BLOCK*)Buffer)->TotalBlocks; i++) {
    
        TRACE((TRACE_ALWAYS,"PspUpdateBlockTypeId(ROMSIG): %08X(%08X), Block %08X\n",HYGON_ROMSIG_BLOCK_ADDRESS,HYGON_ROMSIG_BLOCK_END,BlockDesc[i].StartAddress));
        
        if (BlockDesc[i].StartAddress < HYGON_ROMSIG_BLOCK_ADDRESS) 
            continue;
        
        if (BlockDesc[i].StartAddress >= HYGON_ROMSIG_BLOCK_END) 
            continue;
        
        TRACE((TRACE_ALWAYS,"PspUpdateBlockTypeId: Found Blocks %08X\n",BlockDesc[i].StartAddress));
        
        BlockDesc[i].Type = BOOT_BLOCK;
    }    
    
    for (i = 0; i < ((INFO_BLOCK*)Buffer)->TotalBlocks; i++) {
    
        TRACE((TRACE_ALWAYS,"PspUpdateBlockTypeId(PSP_UPD): %08X(%08X), Block %08X\n",HYGON_DHYANA_PSP_FIXED_BLOCK_ADDRESS, HYGON_DHYANA_PSP_UPD_BLOCK_END, BlockDesc[i].StartAddress));
        
        if (BlockDesc[i].StartAddress < HYGON_DHYANA_PSP_FIXED_BLOCK_ADDRESS) 
            continue;
        
        if (BlockDesc[i].StartAddress >= HYGON_DHYANA_PSP_UPD_BLOCK_END) 
            continue;
        
        TRACE((TRACE_ALWAYS,"PspUpdateBlockTypeId: Found Blocks %08X\n",BlockDesc[i].StartAddress));
        
        BlockDesc[i].Type = BOOT_BLOCK;
    }
#endif

#if PSP_CRISIS_RECOVERY_SUPPORT == 1  
    // Lock ROM Sig area
    for (i = 0; i < ((INFO_BLOCK*)Buffer)->TotalBlocks; i++) {
    
        TRACE((TRACE_ALWAYS,"PspUpdateBlockTypeId(ROMSIG): %08X(%08X), Block %08X\n",HYGON_ROMSIG_BLOCK_ADDRESS,HYGON_ROMSIG_BLOCK_END,BlockDesc[i].StartAddress));
        
        if (BlockDesc[i].StartAddress < HYGON_ROMSIG_BLOCK_ADDRESS) 
            continue;
        
        if (BlockDesc[i].StartAddress >= HYGON_ROMSIG_BLOCK_END) 
            continue;
        
        TRACE((TRACE_ALWAYS,"PspUpdateBlockTypeId: Found Blocks %08X\n",BlockDesc[i].StartAddress));
        
        BlockDesc[i].Type = PSP_FLASH_BLOCK_DESC_TYPE;
    }

// There is no PSP NVRAM used currently for SP3
/*
    for (i = 0; i < ((INFO_BLOCK*)Buffer)->TotalBlocks; i++) {
    
        TRACE((TRACE_ALWAYS,"PspUpdateBlockTypeId(PSP): %08X(%08X), Block %08X\n",HYGON_PSP_NVRAM_BLOCK_ADDRESS,HYGON_PSP_NVRAM_BLOCK_END,BlockDesc[i].StartAddress));
        
        if (BlockDesc[i].StartAddress < HYGON_PSP_NVRAM_BLOCK_ADDRESS) 
            continue;
        
        if (BlockDesc[i].StartAddress >= HYGON_PSP_NVRAM_BLOCK_END) 
            continue;
        
        TRACE((TRACE_ALWAYS,"PspUpdateBlockTypeId: Found Blocks %08X\n",BlockDesc[i].StartAddress));
        
        BlockDesc[i].Type = PSP_FLASH_BLOCK_DESC_TYPE;
    }
*/ 
    
// Protect PSP fixed AREA
        for (i = 0; i < ((INFO_BLOCK*)Buffer)->TotalBlocks; i++) {
    
        TRACE((TRACE_ALWAYS,"PspUpdateBlockTypeId(PSP FIXED): %08X(%08X), Block %08X\n",HYGON_DHYANA_PSP_FIXED_BLOCK_ADDRESS,HYGON_DHYANA_PSP_FIXED_BLOCK_END,BlockDesc[i].StartAddress));
        
        if (BlockDesc[i].StartAddress < HYGON_DHYANA_PSP_FIXED_BLOCK_ADDRESS) 
            continue;
        
        if (BlockDesc[i].StartAddress >= HYGON_DHYANA_PSP_FIXED_BLOCK_END)          
            continue;
        
        TRACE((TRACE_ALWAYS,"PspUpdateBlockTypeId: Found Blocks %08X\n",BlockDesc[i].StartAddress));
        
        BlockDesc[i].Type = PSP_FLASH_BLOCK_DESC_TYPE;
    }
#endif
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
