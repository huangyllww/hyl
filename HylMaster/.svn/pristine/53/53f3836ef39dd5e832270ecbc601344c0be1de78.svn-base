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
//**********************************************************************

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:     AmiPspStorageLib.c
//
// Description:  AMI PSP Storage library
//
// Notes:    
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#include "Token.h"
#include "FFS.h"
#include "AmiLib.h"
#include "AmiPspStorageLib.h"
#include <Library/AmdPspBaseLibV2.h>
//#include "PspStorageLib.h"
#include <Protocol/FlashProtocol.h>

FLASH_PROTOCOL	        *pFlash;
EFI_PHYSICAL_ADDRESS	PspNvBase;
EFI_FVB_ATTRIBUTES_2    RtFwVolAttr;
UINT8                   Buffer4K[FLASH_BLOCK_SIZE];


//Not used yet   EFI_GUID    gAmiPspStorageFsGuid = AMI_PSP_STORAGE_FS_GUID;


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   GetLbaAddress
//
// Description: Returns physical address for given LBA
//
// Input:       
//  IN        EFI_LBA Lba - Logical block address
//  IN OUT    UINTN   *LbaAddress - pointer to Physical address of LBA
//
// Output:      EFI_STATUS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
GetLbaAddress(
  IN        EFI_LBA Lba,
  IN OUT    UINTN   *LbaAddress
)
{
    // Check if LBA is above the total number of blocks
    if (Lba > ((PSP_NVRAM_SIZE / FLASH_BLOCK_SIZE) - 1)) return  EFI_INVALID_PARAMETER;
    
    *LbaAddress = (UINTN)((UINT8*)PspNvBase + Lba * FLASH_BLOCK_SIZE);
    
    return  EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   GetPspNvBase
//
// Description: Returns base address of PSP NVRAM
//
// Input:       
//  IN OUT EFI_PHYSICAL_ADDRESS	*Address
//
// Output:      EFI_STATUS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
GetPspNvBase(
  IN OUT EFI_PHYSICAL_ADDRESS	*Address
)
{
    UINT32          PspDirBase;
    UINT32          BiosDirBase;
    PSP_DIRECTORY   *PspDir;
    UINTN           i;

    if (GetDirBase (&PspDirBase, &BiosDirBase) != TRUE) return  EFI_NOT_FOUND;
    
    PspDir = (PSP_DIRECTORY *)PspDirBase;
    for (i = 0; i < PspDir->Header.TotalEntries; i++) {
        if (PspDir->PspEntry[i].Type == PSP_S3_NV_DATA) {
            *Address = PspDir->PspEntry[i].Location;
            return  EFI_SUCCESS;
        }
    }
    
    return  EFI_NOT_FOUND;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   PspGetVolumeAttributes
//
// Description: Get Volume attributes
//
// Input:       
//  OUT EFI_FVB_ATTRIBUTES                    *Attributes
//
// Output:      EFI_STATUS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
 
EFI_STATUS
EFIAPI
PspGetVolumeAttributes  (
  OUT   EFI_FVB_ATTRIBUTES                         * Attributes
  )
{
    if (Attributes == NULL)  return EFI_INVALID_PARAMETER;
    
    // Returning attributes from runtime copy
    *Attributes = RtFwVolAttr;
    
    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   PspSetVolumeAttributes
//
// Description: Set Volume attributes
//
// Input:       
//  OUT EFI_FVB_ATTRIBUTES                    *Attributes
//
// Output:      
//  EFI_STATUS
//    EFI_SUCCESS if attributes changed and current state of volume in Attributes
//    EFI_INVALID_PARAMETER if new settings are in conflict with capabilities
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
 
EFI_STATUS
EFIAPI
PspSetVolumeAttributes  (
  OUT   EFI_FVB_ATTRIBUTES                          * Attributes
  )
{
//    EFI_FVB_ATTRIBUTES      CurrStatus;
    EFI_FVB_ATTRIBUTES      NewStatus;
    EFI_FVB_ATTRIBUTES      CurrCap;
    

    if (Attributes == NULL)  return  EFI_INVALID_PARAMETER;
    
    // check STATUS LOCK, if set - no attributes change allowed
    if (RtFwVolAttr & EFI_FVB2_LOCK_STATUS) return EFI_INVALID_PARAMETER;
    
        
//    CurrStatus = RtFwVolAttr & FVB2_ATTR_STATUS_MASK;
    CurrCap = RtFwVolAttr & FVB2_ATTR_CAP_MASK;
    NewStatus = *Attributes & FVB2_ATTR_STATUS_MASK;
    
    // Check if new changes are allowed by capabilities
    // Write enable - cap=0 and new status has write enable
    if ((CurrCap & EFI_FVB2_WRITE_ENABLED_CAP) == 0) {
        if(NewStatus & EFI_FVB2_WRITE_STATUS) return EFI_INVALID_PARAMETER;
    }
    // Read enable - cap=0 and new status has read enable
    if ((CurrCap & EFI_FVB2_READ_ENABLED_CAP) == 0) {
        if(NewStatus & EFI_FVB2_READ_STATUS) return EFI_INVALID_PARAMETER;
    }
    
    // Write disable - cap=0 and new status has write disable
    if ((CurrCap & EFI_FVB2_WRITE_DISABLED_CAP) == 0) {
        if((NewStatus & EFI_FVB2_WRITE_STATUS) == 0) return EFI_INVALID_PARAMETER;
    }
    
    // Read disable - cap=0 and new status has read disable
    if ((CurrCap & EFI_FVB2_READ_DISABLED_CAP) == 0) {
        if((NewStatus & EFI_FVB2_READ_STATUS) == 0) return EFI_INVALID_PARAMETER;
    }
    
    // Lock
    if ((CurrCap & EFI_FVB2_LOCK_CAP) == 0) {
        if(NewStatus & EFI_FVB2_LOCK_STATUS) return EFI_INVALID_PARAMETER;
    }
    
    RtFwVolAttr = (RtFwVolAttr & ~((UINT32)(FVB2_ATTR_STATUS_MASK))) | NewStatus;
    // Update input parameter with current attributes
    *Attributes = RtFwVolAttr; 
    
    return EFI_SUCCESS;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   PspGetPhysicalAddress
//
// Description: Get Volume physical address
//
// Input:       
//  OUT   EFI_PHYSICAL_ADDRESS *Address - on success contains base address of
//                                        the firmware volume.
//
// Output:      
//  EFI_STATUS
//    EFI_SUCCESS - The firmware volume base address returned
//    EFI_DEVICE_ERROR - block device is not functioning correctly
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
 
EFI_STATUS
EFIAPI
PspGetPhysicalAddress (
  OUT   EFI_PHYSICAL_ADDRESS                        * Address
  )
{
    if (PspNvBase == 0) return EFI_DEVICE_ERROR;

    *Address = PspNvBase;
    
    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   PspGetBlockSize
//
// Description: Get Volume block size in bytes
//
// Input:       
//  IN    EFI_LBA                             Lba
//  OUT   UINTN                               *BlockSize
//  OUT   UINTN                               *NumberOfBlocks
//
// Output:      
//  EFI_STATUS
//    EFI_SUCCESS - block size and number of consecutive blocks returned
//    EFI_INVALID_PARAMETER - the requested Lba is out of range
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
 
EFI_STATUS
EFIAPI
PspGetBlockSize (
  IN       EFI_LBA                                      Lba,
     OUT   UINTN                                       *BlockSize,
     OUT   UINTN                                       *NumberOfBlocks
  )
{

    // sanity check
    if ((BlockSize == NULL) || (NumberOfBlocks == NULL)) return EFI_INVALID_PARAMETER;

	if (Lba > ((PSP_NVRAM_SIZE / FLASH_BLOCK_SIZE) - 1)) return EFI_INVALID_PARAMETER;

    *NumberOfBlocks = (PSP_NVRAM_SIZE / FLASH_BLOCK_SIZE)  - Lba;
    *BlockSize = FLASH_BLOCK_SIZE;
    
    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   PspRead
//
// Description: Reads the specified number of bytes into a buffer from 
//              the specified block.
//
// Input:       
//  IN    EFI_LBA  Lba - The starting logical block index from which to read.
//  IN       UINTN Offset - Offset into the block at which to begin reading.
//  IN OUT   UINTN *NumBytes - At entry, *NumBytes contains the total size 
//                              of the buffer. At exit, *NumBytes contains 
//                              the total number of bytes read.
//     OUT   UINT8 *Buffer - Pointer to a caller-allocated buffer that will
//                           be used to hold the data that is read.
//
// Output:      
//  EFI_STATUS
//    EFI_SUCCESS - The firmware volume was read successfully and contents 
//                  are in Buffer.
//    EFI_BAD_BUFFER_SIZE - Read attempted across an LBA boundary. On output,
//                  NumBytes contains the total number of bytes returned in Buffer.
//    EFI_ACCESS_DENIED - The firmware volume is in the ReadDisabled state.
//    EFI_DEVICE_ERROR - The block device is not functioning correctly and 
//                       could not be read.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
 
EFI_STATUS
EFIAPI
PspRead (
  IN       EFI_LBA                                      Lba,
  IN       UINTN                                        Offset,
  IN OUT   UINTN                                        *NumBytes,
     OUT   UINT8                                        *Buffer
  )
{
    EFI_FVB_ATTRIBUTES  Attributes;
    UINTN               BlockSize;
    UINTN               NumberOfBlocks;
    EFI_STATUS          Status;
    BOOLEAN             BadBufferSize = FALSE;
    VOID                *FlashAddr;
    
    // Sanity checks
    if (PspNvBase == 0) return EFI_DEVICE_ERROR;
    if ((NumBytes == NULL) || (Buffer == NULL) || (Offset > FLASH_BLOCK_SIZE)) return EFI_INVALID_PARAMETER;
    
    // Check if Lba is in valid range
    Status = PspGetBlockSize( Lba, &BlockSize, &NumberOfBlocks);
    if (EFI_ERROR(Status))   return EFI_INVALID_PARAMETER;
    
    // Check if read enabled
    PspGetVolumeAttributes( &Attributes);
    if ((Attributes & EFI_FVB2_READ_STATUS) == 0)   return EFI_ACCESS_DENIED;
    
    // Check if read cross the block boundary and adjust accordingly
    if ((Offset + *NumBytes) > BlockSize) {
        *NumBytes = BlockSize - Offset;
        BadBufferSize = TRUE;
    }
    
    FlashAddr = (UINT8*)PspNvBase + Lba*BlockSize + Offset;
    
    Status = pFlash->Read( FlashAddr, *NumBytes, Buffer);

    if (( Status == EFI_SUCCESS) & BadBufferSize) return EFI_BAD_BUFFER_SIZE;
    
    return EFI_SUCCESS;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   PspWrite
//
// Description: Writes the specified number of bytes from the input buffer to the block.
//
// Input:       
//  IN    EFI_LBA   Lba - The starting logical block index to write to.
//  IN       UINTN  Offset - Offset into the block at which to begin writing.
//  IN OUT   UINTN  *NumBytes - Pointer to a UINTN. At entry, *NumBytes contains the 
//                  total size of the buffer. At exit, *NumBytes contains the total 
//                  number of bytes actually written.
//     OUT   UINT8  *Buffer - Pointer to a caller-allocated buffer that contains 
//                            the source for the write.
//
// Output:
//  EFI_STATUS
//      EFI_SUCCESS - The firmware volume was written successfully.
//      EFI_BAD_BUFFER_SIZE - The write was attempted across an LBA boundary.
//      EFI_ACCESS_DENIED - The firmware volume is in the WriteDisabled state.
//      EFI_DEVICE_ERROR - The block device is malfunctioning and could not be written.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
 
EFI_STATUS
EFIAPI
PspWrite (
  IN       EFI_LBA                                      Lba,
  IN       UINTN                                        Offset,
  IN OUT   UINTN                                        *NumBytes,
  IN       UINT8                                        *Buffer
  )
{
    EFI_FVB_ATTRIBUTES  Attributes;
    UINTN               BlockSize;
    UINTN               NumberOfBlocks;
    EFI_STATUS          Status;
    BOOLEAN             BadBufferSize = FALSE;
    VOID                *FlashAddr;

    // Sanity checks
    if (pFlash == NULL) return EFI_DEVICE_ERROR;

    if ((NumBytes == NULL) || (Buffer == NULL) || (Offset > FLASH_BLOCK_SIZE)) return EFI_INVALID_PARAMETER;
    
    // Check if Lba is in valid range
    Status = PspGetBlockSize( Lba, &BlockSize, &NumberOfBlocks);
    if (EFI_ERROR(Status))   return EFI_INVALID_PARAMETER;
    
    // Check if write enabled
    PspGetVolumeAttributes( &Attributes);
    if ((Attributes & EFI_FVB2_WRITE_STATUS) == 0)   return EFI_ACCESS_DENIED;
    
    // Check if write cross the block boundary and adjust accordingly
    if ((Offset + *NumBytes) > BlockSize) {
        *NumBytes = BlockSize - Offset;
        BadBufferSize = TRUE;
    }
    
    FlashAddr = (UINT8*)PspNvBase + Lba*BlockSize + Offset;
    
    Status = pFlash->Write( FlashAddr, *NumBytes, Buffer);

    if (( Status == EFI_SUCCESS) & BadBufferSize) return EFI_BAD_BUFFER_SIZE;
    
    return  Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   PspEraseBlocks
//
// Description: Erase block
//
// Input:       
//  IN    EFI_LBA   Lba - Start LBA of erased region
//  IN      UINTN   NumOfBlocks - number of blocks to erase
//  IN ...  block map, variable list , terminated by 
//              EFI_LBA_LIST_TERMINATOR
//
// Output:      
//  EFI_STATUS
//    EFI_SUCCESS - erase request was successfully completed
//    EFI_ACCESS_DENIED - volume is in Write disabled state
//    EFI_DEVICE_ERROR - block device is not functioning correctly
//    EFI_INVALID_PARAMETER - on or more LBAs in input are out of range
//
//  Note: To avoid endless loop if list is not terminated, limit set to 
//          MAX_ERASE_ARGS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
 
EFI_STATUS
EFIAPI
PspEraseBlocks (
  IN      EFI_LBA Lba,
  IN      UINTN   NumOfBlocks,
  IN      ...
  )
{
    EFI_FVB_ATTRIBUTES  Attributes;
    EFI_STATUS          Status;
    EFI_LBA             MaxLba;
    UINTN               BlockSize;
    va_list             ArgPtr;
    BOOLEAN             IsTerminated = FALSE;
    EFI_LBA             tLba = 0;
    UINTN               tNumBlocks = 0;
    UINTN               TotalBlocks;
    UINTN               i;
    VOID                *FlashAddr;

    // Check if write disabled
    PspGetVolumeAttributes( &Attributes);
    if ((Attributes & EFI_FVB2_WRITE_STATUS) == 0)   return EFI_ACCESS_DENIED;
    
    // Check input parameters
    // a. get valid range
    Status = PspGetBlockSize( 0, &BlockSize, &TotalBlocks);
    if (EFI_ERROR(Status))   return EFI_DEVICE_ERROR;
    MaxLba = TotalBlocks -1;
    
    // b.check first tuple
    if ( ((Lba + NumOfBlocks -1)> MaxLba) || (NumOfBlocks == 0)) return EFI_INVALID_PARAMETER;
    
    // c. verify variable argument list
    va_start(ArgPtr, NumOfBlocks);
    
    for (i = 0; i < MAX_ERASE_ARGS; i++) {
        tLba = va_arg(ArgPtr, EFI_LBA);
        if (tLba == EFI_LBA_LIST_TERMINATOR) {
            IsTerminated = TRUE;
            break;
        }
        tNumBlocks = va_arg(ArgPtr, UINTN);
        if ((tLba == 0) || (tNumBlocks == 0) || ((tLba + tNumBlocks - 1) > MaxLba)) {
            va_end(ArgPtr);
            return  EFI_INVALID_PARAMETER;
        }
    }
    va_end(ArgPtr);
    // check if list was terminated
    if (!IsTerminated)   return EFI_INVALID_PARAMETER;
    
    // Erase requested regions
    FlashAddr = (UINT8*)PspNvBase + Lba*BlockSize;
    Status = pFlash->Erase(FlashAddr, NumOfBlocks*BlockSize);

    if(EFI_ERROR(Status))   return Status;
    
    va_start(ArgPtr, NumOfBlocks);
    for (i = 0; i < MAX_ERASE_ARGS; i++) {
        tLba = va_arg(ArgPtr, EFI_LBA);
        if (tLba == EFI_LBA_LIST_TERMINATOR) break;
    
        tNumBlocks = va_arg(ArgPtr, UINTN);
        FlashAddr = (UINT8*)PspNvBase + tLba*BlockSize;
        Status = pFlash->Erase(FlashAddr, tNumBlocks*BlockSize);

        if (EFI_ERROR(Status)) break;
    }
    
    va_end(ArgPtr);
    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   PspStorageInit
//
// Description: Initialize PSP Storage protocol
//
// Input:       
//  IN       EFI_SYSTEM_TABLE   *SystemTable
//
// Output:      EFI_STATUS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
 
EFI_STATUS
EFIAPI
PspStorageInit (
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
    EFI_STATUS  Status;

	Status = SystemTable->BootServices->LocateProtocol( &gFlashSmmProtocolGuid, \
                                        NULL, \
                                        &pFlash );

    if (EFI_ERROR(Status))   return Status;

    if (pFlash == NULL) return EFI_NOT_READY;

    // Find Address of Volume
    Status = GetPspNvBase(&PspNvBase);

	if (EFI_ERROR(Status)) return Status;
	
	RtFwVolAttr = DEFAULT_FVB2_ATTR;    

    //#### DEBUG - workaround for PSP FW handling NVRAM

    return Status;
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
