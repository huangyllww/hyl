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
//
// Name:    FlashWrite.c
//
// Description: Flash update routines
//
//<AMI_FHDR_END>
//**********************************************************************
//============================================================================
// Includes
//============================================================================
#include <Uefi.h>
#include <AmiDxeLib.h>
#include "FlashPart.h"
#include "AmiModulePkg/Flash/SPI/SpiFlash.h"
#include "Token.h"
//============================================================================
// Local defines for transaction types
//============================================================================
// ITE8512 SPI register defines.
#if defined(REG80_FIFO_SUPPORT) && (REG80_FIFO_SUPPORT == 1)
#define     SPI_MAX_DATA_TRANSFER       64
#define     FIFO_ADDRESS_0              0x80
#define     FIFO_ADDRESS_1              0x81
#define     FIFO_ADDRESS_2              0x82
#define     FIFO_ADDRESS_3              0x83
#define     FIFO_ADDRESS_4              0x84
#else
#define     SPI_MAX_DATA_TRANSFER       4
#define     FIFO_ADDRESS_0              0x0C
#define     FIFO_ADDRESS_1              0x0C
#define     FIFO_ADDRESS_2              0x0C
#define     FIFO_ADDRESS_3              0x0C
#define     FIFO_ADDRESS_4              0x0C
#endif
#define     SPI_PREFIX_WRITE_S_EN       0x1
#define     SPI_PREFIX_WRITE_EN         0x0
//============================================================================
// Module level global data
//============================================================================
extern UINT16       gFlashId;
extern FLASH_PART   *FlashAPI;
//============================================================================
// Function Externs
//============================================================================
extern
VOID
SpiChipsetVirtualFixup(
    IN  EFI_RUNTIME_SERVICES    *pRS
);
//============================================================================
// Local prototypes
//============================================================================
BOOLEAN
EFIAPI
CommonSpiEraseCommand(
    IN  volatile UINT8  *pBlockAddress
);
EFI_STATUS
EFIAPI
CommonSpiEraseCommandEx(
    IN  volatile UINT8  *pBlockAddress,
    IN  UINT32          Length
);
BOOLEAN
EFIAPI
CommonSpiProgramCommand(
    IN      volatile UINT8  *pByteAddress,
    IN      UINT8           *Byte,
    IN  OUT UINT32          *Length
);
VOID
EFIAPI
CommonSpiReadCommand(
    IN      volatile UINT8  *pByteAddress,
    OUT     UINT8           *Byte,
    IN  OUT UINT32          *Length
);
BOOLEAN
EFIAPI
CommonSpiIsEraseCompleted(
    IN  volatile UINT8  *pBlockAddress,
    OUT BOOLEAN         *pError,
    OUT UINTN           *pStatus
);
BOOLEAN
EFIAPI
CommonSpiIsProgramCompleted(
    IN  volatile UINT8  *pByteAddress,
    IN  UINT8           *Byte,
    IN  UINT32          Length,
    OUT BOOLEAN         *pError,
    OUT UINTN           *pStatus
);
VOID
EFIAPI
CommonSpiBlockWriteEnable(
    IN  volatile UINT8  *pBlockAddress
);
VOID
EFIAPI
CommonSpiBlockWriteDisable(
    IN  volatile UINT8  *pBlockAddress
);
VOID
EFIAPI
CommonSpiDeviceWriteEnable(
    VOID
);
VOID
EFIAPI
CommonSpiDeviceWriteDisable(
    VOID
);
static VOID
EFIAPI
CommonSpiDeviceVirtualFixup(
    IN  EFI_RUNTIME_SERVICES    *pRS
);
//============================================================================
// Local Variables
//============================================================================
FLASH_PART mCommonSpiFlash ={
    CommonSpiReadCommand,
    CommonSpiEraseCommand,
    CommonSpiEraseCommandEx,
    CommonSpiProgramCommand,
    CommonSpiBlockWriteEnable,
    CommonSpiBlockWriteDisable,
    CommonSpiDeviceWriteEnable,
    CommonSpiDeviceWriteDisable,
    CommonSpiDeviceVirtualFixup,
    1,                      // default value, should be changed in Init function
    SECTOR_SIZE_4KB,        // default value, should be changed in Init function
    NULL                    // Flash Part Number Pointer
};
EX_FLASH_PART mExFlashPart = {
      {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},0,0},
      FLASH_SIZE,         // flash size, should be changed in Init function
      0,                  // flash part id, should be changed in Init function
      0                   // flash part string, should be changed in Init function
};
volatile UINT8  *gSPIBASE = (UINT8*)(UINTN)SPI_BASE_ADDRESS;
UINT32          gBiosRegionBase = 0;
UINT8           gbDeviceVirtual = 0;
UINT8           gbDeviceWriteEnabled = 0;
//============================================================================
// Function Definitions
/**
    The function delays for flash operation.

**/
VOID
IoDelay(
    VOID
)
{
    UINT8               bTimeout;
    for ( bTimeout = 0; bTimeout < 33; bTimeout++ ) {
        IoWrite8( 0xEB, 0x55 );
        IoWrite8( 0xEB, 0xAA );
    }
    return ;
}

/**
    This function retuns the Flash Descriptor Override Pin Strap Status (FDOPSS).

    @retval TRUE The Flash Descriptor Security Override strap is set
    @retval FALSE The Flash Descriptor Security Override strap is not set
**/
BOOLEAN
IsFlashDescriptorOverrided(
    VOID
)
{
    return FALSE;
}

/**
    This function gets the Base and Limit from the specified Flash Region

    @param Type Type of Flash region.   
    @param Base Base Address of region. 
    @param Limit Limit Address of region.

    @retval EFI_SUCCESS Operation Completed
    @retval EFI_UNSUPPORTED Operation not Completed
**/
EFI_STATUS
GetFlashRegion(
    FLASH_REGION_TYPE   Type,
    UINTN               *Base,
    UINTN               *Limit
)
{
    if (Type != FlashRegionBios) return EFI_UNSUPPORTED;
    *Base = 0;
    *Limit = FLASH_SIZE - 1;
    return EFI_SUCCESS;
}

/**
    This function gets the Maximum Limit from the specified Flash Region

    @param Limit Maximum Limit Address of region.

    @retval EFI_SUCCESS Operation Completed
**/
EFI_STATUS 
GetMaxRegionLimit(
    UINTN   *Limit
)
{
    UINTN   i, MaxRegionLimit;
    for (*Limit = MaxRegionLimit = i = 0; i < FlashRegionMax; i++) { 
        if (EFI_ERROR(GetFlashRegion(i, NULL, &MaxRegionLimit))) continue;
        if (MaxRegionLimit > *Limit) *Limit = MaxRegionLimit;
    }
    return EFI_SUCCESS;
}

/**
    This function set Tx and Rx byte count

    @param ByteCount Top byte is Rx, low byte is Tx
**/
VOID
SpiTxRxByteCount(
    UINT16  ByteCount
)
{
#if defined(YANGTZE_SPI) && (YANGTZE_SPI == 1)
    // SpiExtRegIndx [5] - TxByteCount
    *(volatile UINT8*)( gSPIBASE + 0x1E ) = 5;
    *(volatile UINT8*)( gSPIBASE + 0x1F ) = ByteCount & 0xFF;
    // SpiExtRegData [6] - RxByteCount
    *(volatile UINT8*)( gSPIBASE + 0x1E ) = 6;
    *(volatile UINT8*)( gSPIBASE + 0x1F ) = ByteCount >> 8;;
#else
    // SPI Ctrl - BIT[11..8] : TxByteCount, BIT[15..12] - RxByteCount
    *(volatile UINT8*)( gSPIBASE + 1 ) = ByteCount;
#endif    
}

/**
    This routine resets Internal FIFO port.
**/
VOID
ResetInternalFIFOPointer(
    VOID
)
{
    do {
        *(volatile UINT8*)( gSPIBASE + 2 ) |= 0x10;
    } while ( *(volatile UINT8*)( gSPIBASE + 0xD ) & 0x7 );
}

/**
    This routine execute SPI OpCode.
**/
VOID
ExecuteSpiCommand(
    VOID
)
{
    *(volatile UINT8*)( gSPIBASE + 2 ) |= 1;
}

/**
    This routine wait SPI cycle to be done.
**/
VOID
WaitForSpiCycleDone(
    VOID
)
{
    UINT8   bStatus;
    UINT16  wTimeout;

    for ( wTimeout = 0; wTimeout < 0xFFFF; wTimeout++ ) {
        bStatus = *(volatile UINT8*)( gSPIBASE + 2);
        if (!( bStatus & BIT00 )) break;
    }
}

/**
    This routine send write enable command.
**/
VOID
SpiWriteEnable(
    VOID
)
{
    UINT8   bCommand;

    bCommand = mExFlashPart.FlashCommandMenu.WriteEnable.Opcode;
    *(volatile UINT8*)gSPIBASE = bCommand;
    // TxByte = 0, RxByte = 0
    SpiTxRxByteCount (0);
    ExecuteSpiCommand ();
    WaitForSpiCycleDone ();
    return;
}

/**
    This routine send write enable command.

    @retval UINT8 SPI flash status
**/
UINT8
CommonSpiReadStatus(
    VOID
)
{
    UINT8   bCommand;

    bCommand = mExFlashPart.FlashCommandMenu.ReadStatus.Opcode;
    *(volatile UINT8*)gSPIBASE = bCommand;
    // TxByte = 1, RxByte = 2
    SpiTxRxByteCount ((0x02 << 8) + 0x01);
    ResetInternalFIFOPointer ();
    *(volatile UINT8*)( gSPIBASE + FIFO_ADDRESS_0 ) = 0;
    ResetInternalFIFOPointer ();
    ExecuteSpiCommand ();
    WaitForSpiCycleDone ();
    ResetInternalFIFOPointer ();
    bCommand = *(volatile UINT8*)( gSPIBASE + FIFO_ADDRESS_1 );
    bCommand = *(volatile UINT8*)( gSPIBASE + FIFO_ADDRESS_2 );
    return bCommand;
}

/**
    This routine wait write command is completed.
**/
VOID
WaitForWriteOperationCompleted(
    VOID
)
{
    UINT16  wWaitStsRetry;
    UINT8   bStatus;

    for( wWaitStsRetry = 0; wWaitStsRetry < 0xFFFF; wWaitStsRetry++ ) {
        // read flash status register.
        bStatus = CommonSpiReadStatus();
        // Is operation busy ?
        if( !( bStatus & 0x1 ) ) break;
    }
}

/**
    Routine for Write SPI Status Register.

    @param bWriteData Status to be written to SPI.
    @param bOpcodeIndex Write status command. 
    @param bIsDataPhase
    @param bPrefixOp Command be sent before write status.
    @param dSectorAddress
**/
VOID
CommonSpiWriteStatus(
    IN  UINT8   bWriteData,
    IN  UINT8   bOpcodeIndex,
    IN  UINT8   bIsDataPhase,
    IN  UINT8   bPrefixOp,
    IN  UINT32  dSectorAddress
)
{
    UINT8   bCommand;

    if ( bPrefixOp ) {
        bCommand = mExFlashPart.FlashCommandMenu.WriteStatusEnable.Opcode;
        *(volatile UINT8*)gSPIBASE = bCommand;
        // TxByte = 0, RxByte = 0
        SpiTxRxByteCount (0);
        ExecuteSpiCommand ();
        WaitForSpiCycleDone ();
    } else SpiWriteEnable ();
    *(volatile UINT8*)gSPIBASE = bOpcodeIndex;
    // TxByte = 1, RxByte = 0
    SpiTxRxByteCount (1);
    ResetInternalFIFOPointer ();
    if ( bIsDataPhase ) *(volatile UINT8*)( gSPIBASE + FIFO_ADDRESS_0 ) = bWriteData;
    ResetInternalFIFOPointer ();
    ExecuteSpiCommand ();
    WaitForSpiCycleDone ();
    if ( bIsDataPhase ) WaitForWriteOperationCompleted ();
}

/**
    Routine for reading byte data from spi.

    @param dByteAddress Address to be read from.

    @retval UINT8 Byte data.
**/
UINT8
CommonSpiReadByte(
    IN  UINTN    dByteAddress
)
{
    return ( *(volatile UINT8*)dByteAddress );
}

/**
    Convert spi address

    @param pAddress Address to be converted.

    @retval UINT32 Address.
**/
UINT32
CommonConvertSpiAddress(
    IN  volatile UINT8  *pAddress
)
{
    if ( gbDeviceVirtual ) {
        // pAddress - offset from Flash Device Base.
        pAddress -= FlashDeviceBase;
        // pAddress - 32bit memory mapping address.
        pAddress += (0xFFFFFFFF - FLASH_SIZE) + 1;
    }
    return ((UINT32)pAddress);
}

/**
    Initialize spi
**/
VOID
InitializeSpiEnvironment(
    IN  FLASH_INFO  *FlashInfo
)
{
    return;
}

/**
    Re-initialize spi
**/
VOID
ReinitializeSpiEnvironment(
    IN  FLASH_INFO  *FlashInfo
)
{
    return;
}

/**
    Read flash part's ID

    @param FlashInfo Pointer of FLASH_INFO structure.
    @param dFlashId Pointer to return flash part's id.

    @retval TRUE Read id successfully.
**/
BOOLEAN
CommonSpiReadId(
    IN  FLASH_INFO  *FlashInfo,
    OUT UINT32      *dFlashId
)
{
    UINT8   bCommand;

    CommonSpiDeviceWriteEnable();
    InitializeSpiEnvironment( FlashInfo );
    bCommand = FlashInfo->ReadId.Opcode;
    *(volatile UINT8*)gSPIBASE = bCommand;
    if ( bCommand != 0x9F ) {
        // TxByte = 3, RxByte = 4
        SpiTxRxByteCount ( (0x04 << 8) + 0x03 );
        ResetInternalFIFOPointer ();
        *(volatile UINT8*)( gSPIBASE + FIFO_ADDRESS_0 ) = 0;
        *(volatile UINT8*)( gSPIBASE + FIFO_ADDRESS_1 ) = 0;
        *(volatile UINT8*)( gSPIBASE + FIFO_ADDRESS_2 ) = 0;
    } else {
        // TxByte = 0, RxByte = 3
        SpiTxRxByteCount ( (0x03 << 8) + 0 );
    }
    ResetInternalFIFOPointer ();
    ExecuteSpiCommand ();
    WaitForSpiCycleDone ();
    ResetInternalFIFOPointer ();
    if ( bCommand != 0x9F ) {
        *(UINT8*)dFlashId = *(volatile UINT8*)( gSPIBASE + FIFO_ADDRESS_3 );
        *(UINT8*)dFlashId = *(volatile UINT8*)( gSPIBASE + FIFO_ADDRESS_4 );
    } else {
        *(UINT8*)dFlashId = *(volatile UINT8*)( gSPIBASE + FIFO_ADDRESS_0 );
        *((UINT8*)dFlashId+1) = *(volatile UINT8*)( gSPIBASE + FIFO_ADDRESS_1 );
    }
    CommonSpiDeviceWriteDisable();
    return TRUE;
}

/**
     This function flushes a range of addresses from the processor cache.
     This is needed because the SPI controller erase and program functions
     change flash memory through a back door, and the processor cache doesn't
     get updated. If the flash range just erased or programmed is present in
     the cache, reading the flash will return the data as it was before erase
     or program. Flushing the range just erased or programmed eliminates this
     problem.
     
     @param start Start address that need to be flush
     @param count Data size	
 **/
static VOID 
FlushMemoryRange (
    IN VOID *start, 
	IN UINTN count
)
{
	UINT8 *clFirst, *clLast, *cl;
	UINTN address = (UINTN)start;
	INTN clSize = 64;
	
	clFirst = (void *)(address & -clSize);
	clLast  = (void *)((address + count - 1) & -clSize);
	for (cl = clFirst; cl <= clLast; cl += clSize)
	 AsmFlushCacheLine (cl);
 
}
 	
/**
    This API function erases a block in the flash. Flash model specific
    code will branch out from this routine.

    @param pBlockAddress Block that need to be erased.
    @param Length Number of bytes that need to be erased

    @retval EFI_SUCCESS erase complete
    @retval EFI_DEVICE_ERROR erase not completed
**/
EFI_STATUS
EFIAPI
CommonSpiEraseCommandEx(
    IN  volatile UINT8  *pBlockAddress,
    IN  UINT32          Length
)
{
    volatile UINTN  dSectorAddr, dPhysicalAddr;
    UINT32          dNByte;
    UINT16          i, wEraseRetry, wNumSectors;
    UINT8           bCommand;
    EFI_STATUS      Status;

    // These parts only erase in 64K sectors
    InitializeSpiEnvironment( &mExFlashPart.FlashCommandMenu );
    if( Length % FlashAPI->FlashSectorSize ) return EFI_INVALID_PARAMETER;
    wNumSectors = ( Length / FlashAPI->FlashSectorSize );
    for ( i = 0; i < wNumSectors ; i++ ) {
        dSectorAddr = (UINTN)(pBlockAddress + (i * FlashAPI->FlashSectorSize));
        dPhysicalAddr = CommonConvertSpiAddress( (volatile UINT8*)dSectorAddr );
        for ( dNByte = 0; dNByte < FlashAPI->FlashSectorSize; dNByte++ ) {
            if ( 0xFF != CommonSpiReadByte( dSectorAddr + dNByte ) ) break;
        }
        if  ( dNByte == FlashAPI->FlashSectorSize )   continue;
        SpiWriteEnable ();
        bCommand = mExFlashPart.FlashCommandMenu.Erase.Opcode;
        for ( wEraseRetry = 0; wEraseRetry < FLASH_RETRIES; wEraseRetry++ ) {
            *(volatile UINT8*)gSPIBASE = bCommand;
            // TxByte = 3, RxByte = 0
            SpiTxRxByteCount (3);
            ResetInternalFIFOPointer ();
            *(volatile UINT8*)( gSPIBASE + FIFO_ADDRESS_0 ) = \
                                (UINT8)(( dPhysicalAddr >> 16 ) & 0xFF );//A2;
            *(volatile UINT8*)( gSPIBASE + FIFO_ADDRESS_1 ) = \
                                (UINT8)(( dPhysicalAddr >> 8 ) & 0xFF );//A1;
            *(volatile UINT8*)( gSPIBASE + FIFO_ADDRESS_2 ) = \
                                (UINT8)( dPhysicalAddr & 0xFF );//A0;
            ResetInternalFIFOPointer ();
            ExecuteSpiCommand ();
            WaitForSpiCycleDone ();
            WaitForWriteOperationCompleted ();
#if ENABLE_ROM_CACHE_IN_DXE			
			// Flush the memory erased so that it will be read back correctly
            FlushMemoryRange (pBlockAddress, FlashBlockSize);
#endif			
            // write operation appeared to succeed, now read back byte and
            // compare.
            CommonSpiIsEraseCompleted( (volatile UINT8*)dSectorAddr, NULL, &Status );
            if( !EFI_ERROR(Status) ) break;
        }
        if( EFI_ERROR(Status) ) return Status;
    }
    return Status;
}

/**
    This API function erases a block in the flash. Flash model specific code
    will branch out from this routine

    @param pBlockAddress Block that need to be erased                 
**/
BOOLEAN
EFIAPI
CommonSpiEraseCommand(
    IN  volatile UINT8  *pBlockAddress
)
{
    return (EFI_ERROR(CommonSpiEraseCommandEx( pBlockAddress, FlashBlockSize ))) ? FALSE : TRUE;
}

/**
    This function programs a byte data to the specified location

    @param pByteAddress Location where the data to be written.
    @param Byte Data to be written.
    @param Length Number of bytes to write
**/
BOOLEAN
EFIAPI
CommonSpiProgramCommand(
    IN      volatile UINT8  *pByteAddress,
    IN      UINT8           *Byte,
    IN  OUT UINT32          *Length
)
{
    UINT8   bFlashRetry = 0, bCommand;
    UINT16  wProgBytes = 0, wNumBytes = 0;
    UINT16  wSpiCmd = 0, wRetry = 0, wMaxNumBytes = 0;
    UINT32  dPhyAddress = 0;

    InitializeSpiEnvironment( &mExFlashPart.FlashCommandMenu );
    wProgBytes = mCommonSpiFlash.FlashProgramSize;
    if ( mCommonSpiFlash.FlashProgramSize != 1 ) {
        // Limit the max transfer to the number of bytes the chipset can
        // transfer per cycle
        if ( *Length >= SPI_MAX_DATA_TRANSFER )
            wProgBytes = SPI_MAX_DATA_TRANSFER;
        else wProgBytes = *Length;
        // this is currently for the WINBOND parts only
        // mask off lowest 8 bits of address so that we can determine how
        // many bytes we can write before we hit the end of a page
        wMaxNumBytes = 0x100 - ((UINT8)pByteAddress & 0xFF);
        if ( (UINT32)pByteAddress & 0x1 )   wProgBytes = 1;
        else if ( (UINT16)wProgBytes > wMaxNumBytes )
                wProgBytes = wMaxNumBytes;
    }
    for ( bFlashRetry = 0; bFlashRetry < FLASH_RETRIES; bFlashRetry++ ) {
        // check if do the data need to be programmed ?
        for ( wNumBytes = 0; wNumBytes < wProgBytes; wNumBytes++ ) {
            if ( *( Byte + wNumBytes ) != 0xFF )    break;
        }

        // The data is empty and don't need to be programmed.
        if ( wNumBytes == wProgBytes )  break;
        SpiWriteEnable ();
        bCommand = mExFlashPart.FlashCommandMenu.Write.Opcode;
        dPhyAddress = CommonConvertSpiAddress ( pByteAddress );
        *(volatile UINT8*)gSPIBASE = bCommand;
        // TxByte = Program Bytes + Address Bytes, RxByte = 0
        SpiTxRxByteCount (wProgBytes + 3);
        ResetInternalFIFOPointer ();
        *(volatile UINT8*)( gSPIBASE + FIFO_ADDRESS_0 ) = \
                            (UINT8)(( dPhyAddress >> 16 ) & 0xFF );//A2;
        *(volatile UINT8*)( gSPIBASE + FIFO_ADDRESS_1 ) = \
                            (UINT8)(( dPhyAddress >> 8 ) & 0xFF );//A1;
        *(volatile UINT8*)( gSPIBASE + FIFO_ADDRESS_2 ) = \
                            (UINT8)( dPhyAddress & 0xFF );//A0;
        for ( wNumBytes = 0; wNumBytes < wProgBytes; wNumBytes++ )
#if defined(REG80_FIFO_SUPPORT) && (REG80_FIFO_SUPPORT == 1)
            *(volatile UINT8*)( gSPIBASE + FIFO_ADDRESS_3 + wNumBytes ) = *( Byte + wNumBytes );
#else
            *(volatile UINT8*)( gSPIBASE + FIFO_ADDRESS_3 ) = *( Byte + wNumBytes );
#endif
        ResetInternalFIFOPointer ();
        ExecuteSpiCommand ();
        // wait for chipset SPI flash operation completed.
        WaitForSpiCycleDone ();
        WaitForWriteOperationCompleted ();
#if ENABLE_ROM_CACHE_IN_DXE		
		// Flush the memory erased so that it will be read back correctly
        FlushMemoryRange (pByteAddress, *Length);
#endif		
        // write operation appeared to succeed, now read back byte and compare
        // set control for 1-byte data read, no prefix
        for ( wNumBytes = 0; wNumBytes < wProgBytes; wNumBytes++ ) {
            if (*( Byte + wNumBytes ) != \
                   CommonSpiReadByte ((UINTN)( pByteAddress + wNumBytes )))
                break;
        }
        if ( wNumBytes == wProgBytes )  break;
    }
    // Don't forget to return the number of bytes not written
    *Length = *Length - (UINT32)wProgBytes;
    return TRUE;
}

/**
    This function read a byte data to the specified location

    @param pByteAddress Location where the data to be read.
    @param Byte Data to be read.
    @param Length Number of bytes to read
**/
VOID
EFIAPI
CommonSpiReadCommand(
    IN      volatile UINT8  *pByteAddress,
    OUT     UINT8           *Byte,
    IN  OUT UINT32          *Length
)
{
    UINTN   dReadAddress = 0, dNumBytes = 0;

    dReadAddress = (UINTN)pByteAddress;
    for ( dNumBytes = 0; dNumBytes < *Length ; dNumBytes++ )
        *( Byte + dNumBytes ) = CommonSpiReadByte( dReadAddress + dNumBytes );
    *Length = 0;
}

/**
    This function verifies whether the block erase command is completed
    and returns the status of the command.

    @param pByteAddress Location of the block erase.
    @param pError True on error and false on success.
    @param pStatus Error status code (0 - Success).

    @retval TRUE If operation completed successfully.
    @retval FALSE If operation failed.
**/
BOOLEAN
EFIAPI
CommonSpiIsEraseCompleted(
    IN  volatile UINT8  *pBlockAddress,
    OUT BOOLEAN         *pError,
    OUT UINTN           *pStatus
)
{
    UINT32  dNumBytes;

    for ( dNumBytes = 0; dNumBytes < FlashAPI->FlashSectorSize; dNumBytes++ ) {
        if ( *(volatile UINT8*)( pBlockAddress + dNumBytes ) != 0xFF ) {
            if ( pStatus ) *pStatus = EFI_NOT_READY;
            if ( pError ) *pError = TRUE;
            return FALSE;
        }
    }
    if ( pError ) *pError = FALSE;
    if ( pStatus ) *pStatus = EFI_SUCCESS;
    return TRUE;
}

/**
    This function verifies whether the program (page or byte) command is
    completed and returns the status of the command.

    @param pByteAddress Location of the program command.
    @param Byte Last data byte written.
    @param Length
    @param pError True on error and false on success.
    @param pStatus Error status code (0 - Success).

    @retval TRUE If operation completed successfully.
**/
BOOLEAN
EFIAPI
CommonSpiIsProgramCompleted(
    IN  volatile UINT8  *pByteAddress,
    IN  UINT8           *Byte,
    IN  UINT32          Length,
    OUT BOOLEAN         *pError,
    OUT UINTN           *pStatus
)
{
    UINT32  dNumBytes;
    UINT8   bByte;

    for ( dNumBytes = 0; dNumBytes < Length; dNumBytes++ ) {
        bByte = * ( Byte + dNumBytes );
        if ( bByte != *(volatile UINT8*)( pByteAddress + dNumBytes ) ) {
            if ( pStatus ) *pStatus = EFI_NOT_READY;
            if ( pError ) *pError = TRUE;
            return FALSE;
        }
    }
    if ( pError ) *pError = FALSE;
    if ( pStatus ) *pStatus = EFI_SUCCESS;
    return TRUE;
}

/**
    This function contains any flash specific code need to enable a
    particular flash block write.

    @param pBlockAddress Address within the block to write enable.
**/
VOID
EFIAPI
CommonSpiBlockWriteEnable(
    IN  volatile UINT8  *pBlockAddress
)
{
    UINT8   bStatusReg = 0, bPrefixOp, bDataPhase = 1;
    UINT8   bOpmenuIndex;

    bStatusReg = CommonSpiReadStatus();
    bOpmenuIndex = mExFlashPart.FlashCommandMenu.WriteStatus.Opcode;
    switch ( (UINT8)mExFlashPart.FlashVenDevId ) {
        // if SST flash, prefix 1 w/o address
        case 0xBF :
            bStatusReg &= 0x1C;
            bPrefixOp = SPI_PREFIX_WRITE_S_EN;
            break;
        // if ATMEL flash, prefix 0 w/ address
        case 0x1F :
            bStatusReg &= 0xC;
            bPrefixOp = SPI_PREFIX_WRITE_EN;
            break;
        default :
        // default flash, prefix 0 w/o address
            bStatusReg &= 0x1C;
            bPrefixOp = SPI_PREFIX_WRITE_EN;
    }
    if ( bStatusReg )   CommonSpiWriteStatus (  0, \
                                                bOpmenuIndex, \
                                                bDataPhase, \
                                                bPrefixOp, \
                                                0 );
}
/**
    This function contains any flash specific code need to disable
    a particular flash block write.

    @param pBlockAddress Address within the block to write disable.

**/
VOID
EFIAPI
CommonSpiBlockWriteDisable(
    IN  volatile UINT8  *pBlockAddress
)
{
#if defined(BLOCK_PROTECT_ENABLE) && (BLOCK_PROTECT_ENABLE == 1)
    UINT8   bStatusReg = 0, bPrefixOp, bDataPhase = 1;
    UINT8   bOpmenuIndex, bBlockLock;

    bStatusReg = CommonSpiReadStatus();
    bOpmenuIndex = mExFlashPart.FlashCommandMenu.WriteStatus.Opcode;
    switch ( (UINT8)mExFlashPart.FlashVenDevId ) {
        case 0xBF :
            // if SST flash, prefix 1 w/o address
            bBlockLock = (BIT02 + BIT03 + BIT04);
            bStatusReg &= bBlockLock;
            bPrefixOp = SPI_PREFIX_WRITE_S_EN;
            break;
        case 0x1F :
            // if ATMEL flash, prefix 0 w/ address
            bBlockLock = (BIT02 + BIT03);
            bStatusReg &= bBlockLock;
            bPrefixOp = SPI_PREFIX_WRITE_EN;
            break;
        default :
            // default flash, prefix 0 w/o address
            bBlockLock = (BIT02 + BIT03 + BIT04);
            bStatusReg &= bBlockLock;
            bPrefixOp = SPI_PREFIX_WRITE_EN;
    }
    if ( !bStatusReg )  CommonSpiWriteStatus (  bBlockLock, \
                                                bOpmenuIndex, \
                                                bDataPhase, \
                                                bPrefixOp, \
                                                0 );
#endif
}

/**
    This function contains any flash specific code need to enable flash write.
**/
VOID
EFIAPI
CommonSpiDeviceWriteEnable(
    VOID
)
{
    // check is DeviceWrite enabled, if yes, don't enable it again,
    // else, enable it.
    if ( !gbDeviceWriteEnabled ) {
        gbDeviceWriteEnabled = 1;
    }
}

/**
    This function contains any flash specific code need to disable flash write.
**/
VOID
EFIAPI
CommonSpiDeviceWriteDisable(
    VOID
)
{
   // check is DeviceWrite enabled, if yes, disable it, if no, don't disable it.
   if ( gbDeviceWriteEnabled ) {
        gbDeviceWriteEnabled = 0;
    }
}

/**
    This function will be invoked by the core to convert runtime
    pointers to virtual address.

    @param pRS Pointer to runtime services.
**/
static
VOID
EFIAPI
CommonSpiDeviceVirtualFixup(
    IN  EFI_RUNTIME_SERVICES    *pRS
)
{

//  // Following is an example code for virtual address conversion
//  pRS->ConvertPointer(0, (VOID**)&FlashDeviceBase);

//-    SpiChipsetVirtualFixup(pRS);
    pRS->ConvertPointer(0, (VOID **)&gSPIBASE);
    gbDeviceVirtual = 1;

    return;
}
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2009, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
