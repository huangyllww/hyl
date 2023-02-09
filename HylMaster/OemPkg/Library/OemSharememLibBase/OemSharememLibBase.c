/** @file
  Base Sharemem library instance.

  Copyright (C) 2017 Dawning Information Industry Co.,LTD.All rights reserved.<BR>
  
**/

#include "Token.h"
#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>		//MmioRead32()
#include <Library/DebugLib.h>
#include <Library/OemSharememLib.h>
#include <Library/PciLib.h>

/**
 * This routine is to read shared memory 
 *  
 * @author AY (2017/12/07) 
 * @param AddrOff     Offset address to BMC share mem base 
 *                    address
 * @param DataBuffer  Pointer to data buffer holding return data
 * @param Length      Length
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
EFIAPI
ShareMemRead (
  VOID *AddrOff,
  VOID *DataBuffer,
  UINT32 Length
  )
{
  UINT8 Data8;
  UINT8 Offset;
  UINT32 i;
  UINT8 *Buffer;
  UINT32 PageStart;
  UINT32 PageCount;
  UINT32 PageNum;
  UINT32 OffInStartPage;
  UINT32 LeftByteInStart;
  UINT32 LeftByteInEnd;
  UINT32 StartAddr;
  UINT32 ShareMemBaseConfigAddr = 0xffffffff;
  UINT32 ShareMemPciBarAddr = 0xffffffff;
  UINT32 ShareMemWriteBase;
  RETURN_STATUS  Status = EFI_SUCCESS;
  BOOLEAN CrossPage = TRUE;
  UINT32 Addr = (UINT32) AddrOff;
  
  if ((Addr > SHARE_MEM_SIZE) ||((Addr + Length) > SHARE_MEM_SIZE) || (DataBuffer == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  
  Buffer = (UINT8*) DataBuffer;
  ShareMemPciBarAddr = PcdGet32 (PcdBmcP2ABarAddr);
  if (!SHARE_MEM_LPC_ALWAYS && (ShareMemPciBarAddr != 0xffffffff)) { //Bmc P2A Bar assigned with resource, use MMIO way
    ShareMemWriteBase = ShareMemPciBarAddr + SHARE_MEM_BLOCK_SIZE;
    ShareMemBaseConfigAddr = ShareMemPciBarAddr + BMC_P2A_CONFIG_OFFSET;
	PageStart = (UINT32) AddrOff / SHARE_MEM_BLOCK_SIZE;
    OffInStartPage = (UINT32) AddrOff % SHARE_MEM_BLOCK_SIZE;
    LeftByteInStart = SHARE_MEM_BLOCK_SIZE - OffInStartPage;

    if (Length <= LeftByteInStart) {
      PageCount = 0;
      LeftByteInEnd = 0;
      CrossPage = FALSE;
    } else {
      PageCount = (Length - LeftByteInStart) / SHARE_MEM_BLOCK_SIZE;
      LeftByteInEnd = (Length - LeftByteInStart) % SHARE_MEM_BLOCK_SIZE;
      CrossPage = TRUE;
    }

    //
    // Set start page in BAR
    // 0xEF00F004 is share memory configuration address
    // 0xEF010000 is mapping address
    // Note that 32M region from that address is reserved by BMC as share memory
    // 0x9C000000 is page 0 from a BMC perspective
    // 0x9C010000 is page 1 from a BMC perspective
    //
    
    MmioWrite32 (ShareMemBaseConfigAddr, SHARE_MEM_BASE_BMC_ADDR + PageStart*SHARE_MEM_BLOCK_SIZE);
    //
    // Read first page
    //
    if (!CrossPage) {
      CopyMem ((VOID*)Buffer, (VOID*)(ShareMemWriteBase + OffInStartPage), Length);
      Status = EFI_SUCCESS;
    } else {
      CopyMem ((VOID*)Buffer, (VOID*)(ShareMemWriteBase + OffInStartPage), LeftByteInStart);
      Buffer = Buffer + LeftByteInStart;

      //
      // Read remaining pages
      //
      StartAddr = SHARE_MEM_BASE_BMC_ADDR + (PageStart + 1)*SHARE_MEM_BLOCK_SIZE;
      for (PageNum = 1; PageNum < PageCount; PageNum++) {
        //
        // Change page by configuring 0xEF00F004
        //
        MmioWrite32 (ShareMemBaseConfigAddr, StartAddr);
        CopyMem ((VOID*) Buffer, (VOID*)ShareMemWriteBase, SHARE_MEM_BLOCK_SIZE);
        StartAddr = StartAddr + PageNum*SHARE_MEM_BLOCK_SIZE;
        Buffer = Buffer + SHARE_MEM_BLOCK_SIZE;
      }
      MmioWrite32 (ShareMemBaseConfigAddr, StartAddr);
      CopyMem ((VOID*)Buffer, (VOID*)ShareMemWriteBase, LeftByteInEnd);
      Status = EFI_SUCCESS;
    }
  } else { 
    Addr += SHARE_MEM_BASE_BMC_ADDR;
    //
    // Enable iLPC2AHB: Device 0xD
    //
    IoWrite8 (AST2500_CONFIG_INDEX, 0xA5);
    IoWrite8 (AST2500_CONFIG_INDEX, 0xA5);
    IoWrite8 (AST2500_CONFIG_INDEX, 0x07);
    IoWrite8 (AST2500_CONFIG_DATA, 0x0D);
    IoWrite8 (AST2500_CONFIG_INDEX, 0x30);
    Data8 = IoRead8 (AST2500_CONFIG_DATA);
    Data8 |= BIT0;
    IoWrite8 (AST2500_CONFIG_INDEX, 0x30);
    IoWrite8 (AST2500_CONFIG_DATA, Data8);
  
    for (i = 0; i < Length; i++) {
      IoWrite8 (AST2500_CONFIG_INDEX, 0xF0);
      IoWrite8 (0xED, 0);//short delay.
      IoWrite8 (AST2500_CONFIG_DATA, (UINT8) ((Addr >> 24)& 0xff)); // Address [31:24] 
      IoWrite8 (0xED, 0);//short delay.
      IoWrite8 (AST2500_CONFIG_INDEX, 0xF1);
      IoWrite8 (0xED, 0);//short delay.
      IoWrite8 (AST2500_CONFIG_DATA, (UINT8) ((Addr >> 16)& 0xff)); // Address[23:16] 
      IoWrite8 (0xED, 0);//short delay.
      IoWrite8 (AST2500_CONFIG_INDEX, 0xF2);
      IoWrite8 (0xED, 0);//short delay.
      IoWrite8 (AST2500_CONFIG_DATA, (UINT8) ((Addr >> 8)& 0xff));  // Address[15:8] 
      IoWrite8 (0xED, 0);//short delay.
      IoWrite8 (AST2500_CONFIG_INDEX, 0xF3);
      IoWrite8 (0xED, 0);//short delay.
      IoWrite8 (AST2500_CONFIG_DATA, (UINT8) (Addr& 0xff));          // Address[7:0] 
  
      //
      // AST2500 LPC2AHB interface support only 4-byte alighment read/write
      //
      Offset = 0xF7 - (Addr % 4);
      //
      // Trigger read command by reading offset 0xFE
      //
      IoWrite8 (AST2500_CONFIG_INDEX, 0xFE);
      Data8 = IoRead8 (AST2500_CONFIG_DATA);
      IoWrite8 (AST2500_CONFIG_INDEX, Offset);
      Data8 = IoRead8 (AST2500_CONFIG_DATA);
      CopyMem ((VOID*)Buffer, &Data8, 1);
      Buffer++;
      Addr++;
    }
  }
  return Status;
}

/**
 * This routine is to write specific shared memory region 
 *  
 * @author AY (2017/12/07)
 * 
 * @param[in] AddrOff   Offset address to BMC share mem base 
 *       address
 * @param[in] DataBuffer Pointer to buffer holding input data
 * @param[in] Length     Length
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
EFIAPI
ShareMemWrite (
  VOID *AddrOff,
  VOID *DataBuffer,
  UINT32 Length
  )
{
  UINT8 Data8;
  UINT8 Offset;
  UINTN i;
  UINT8 *Buffer;
  UINT32 PageStart;
  UINT32 PageCount;
  UINT32 PageNum;
  UINT32 OffInStartPage;
  UINT32 LeftByteInStart;
  UINT32 LeftByteInEnd;
  UINT32 StartAddr;
  EFI_STATUS  Status = EFI_SUCCESS;
  BOOLEAN	CrossPage;
  UINT32 ShareMemBaseConfigAddr = 0xffffffff;
  UINT32 ShareMemPciBarAddr = 0xffffffff;
  UINT32 ShareMemWriteBase;
  UINT32 Addr = (UINT32) AddrOff;
  
  if ((Addr > SHARE_MEM_SIZE) ||((Addr + Length) > SHARE_MEM_SIZE) || (DataBuffer == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  
  Buffer = (UINT8*) DataBuffer;
  ShareMemPciBarAddr = PcdGet32 (PcdBmcP2ABarAddr);
  if (!SHARE_MEM_LPC_ALWAYS && (ShareMemPciBarAddr != 0xffffffff)) { //Bmc P2A Bar assigned with resource, use MMIO way
    DEBUG ((EFI_D_ERROR, "OemDbg:Bmc P2A Bar allocated, go with MMIO.\n"));
    ShareMemWriteBase = ShareMemPciBarAddr + SHARE_MEM_BLOCK_SIZE;
    ShareMemBaseConfigAddr = ShareMemPciBarAddr + BMC_P2A_CONFIG_OFFSET;
    PageStart = (UINT32)AddrOff / SHARE_MEM_BLOCK_SIZE;
    OffInStartPage = (UINT32)AddrOff % SHARE_MEM_BLOCK_SIZE;
    LeftByteInStart = SHARE_MEM_BLOCK_SIZE - OffInStartPage;

    if (Length <= LeftByteInStart) {
      PageCount = 0;
      LeftByteInEnd = 0;
      CrossPage = FALSE;
    } else {
      PageCount = (Length - LeftByteInStart) / SHARE_MEM_BLOCK_SIZE;
      LeftByteInEnd = (Length - LeftByteInStart) % SHARE_MEM_BLOCK_SIZE;
      CrossPage = TRUE;
    }
	
    //
    // Set start page in BAR
    // 0xEF00F004 is share memory configuration address
    // 0xEF010000 is mapping address
    // Note that 32M region from that address is reserved by BMC as share memory
    // 0x9C000000 is page 0 from a BMC perspective
    // 0x9C010000 is page 1 from a BMC perspective
    //

    MmioWrite32 (ShareMemBaseConfigAddr, SHARE_MEM_BASE_BMC_ADDR + PageStart*SHARE_MEM_BLOCK_SIZE);

    //
    // Write first page
    //
    if (!CrossPage) {
      CopyMem ((VOID*)(ShareMemWriteBase + OffInStartPage), (VOID*)Buffer, Length);
      Status = EFI_SUCCESS;
    } else {
      CopyMem ((VOID*)(ShareMemWriteBase + OffInStartPage), (VOID*)Buffer, LeftByteInStart);
      Buffer = Buffer + LeftByteInStart;

      //
      // Write left pages
      //
      StartAddr = SHARE_MEM_BASE_BMC_ADDR + (PageStart + 1)*SHARE_MEM_BLOCK_SIZE;
      for (PageNum = 1; PageNum <= PageCount; PageNum++) {
        //
        // Change page by configuring 0xEF00F004
        //
        MmioWrite32 (ShareMemBaseConfigAddr, StartAddr);
        CopyMem ((VOID*)ShareMemWriteBase, (VOID*)Buffer, SHARE_MEM_BLOCK_SIZE);
        StartAddr = StartAddr + PageNum*SHARE_MEM_BLOCK_SIZE;
        Buffer = Buffer + SHARE_MEM_BLOCK_SIZE;
      }
      MmioWrite32 (ShareMemBaseConfigAddr, StartAddr);
      CopyMem ((VOID*)ShareMemWriteBase, (VOID*)Buffer, LeftByteInEnd);
      Status = EFI_SUCCESS;
    }
  } else {// Use LPC way
    DEBUG ((EFI_D_ERROR, "OemDbg:Bmc P2A Bar not yet allocated, go with LPC.\n"));
    Addr += SHARE_MEM_BASE_BMC_ADDR;
    
    //
    // Enable iLPC2AHB
    //
    IoWrite8 (AST2500_CONFIG_INDEX, 0xA5);
    IoWrite8 (AST2500_CONFIG_INDEX, 0xA5);
    IoWrite8 (AST2500_CONFIG_INDEX, 0x07);
    IoWrite8 (AST2500_CONFIG_DATA, 0x0D);
    IoWrite8 (AST2500_CONFIG_INDEX, 0x30);
    Data8 = IoRead8 (AST2500_CONFIG_DATA);
    Data8 |= BIT0;
    IoWrite8 (AST2500_CONFIG_INDEX, 0x30);
    IoWrite8 (AST2500_CONFIG_DATA, Data8);
  
    for (i=0; i< Length; i++) {
      IoWrite8 (AST2500_CONFIG_INDEX, 0xF0);
      IoWrite8 (0xED, 0);//short delay.
      IoWrite8 (AST2500_CONFIG_DATA, (UINT8) ((Addr >> 24)& 0xff));
      IoWrite8 (0xED, 0);//short delay.
      IoWrite8 (AST2500_CONFIG_INDEX, 0xF1);
      IoWrite8 (0xED, 0);//short delay.
      IoWrite8 (AST2500_CONFIG_DATA, (UINT8) ((Addr >> 16)& 0xff));
      IoWrite8 (0xED, 0);//short delay.
      IoWrite8 (AST2500_CONFIG_INDEX, 0xF2);
      IoWrite8 (0xED, 0);//short delay.
      IoWrite8 (AST2500_CONFIG_DATA, (UINT8) ((Addr >> 8)& 0xff));
      IoWrite8 (0xED, 0);//short delay.
      IoWrite8 (AST2500_CONFIG_INDEX, 0xF3);
      IoWrite8 (0xED, 0);//short delay.
      IoWrite8 (AST2500_CONFIG_DATA, (UINT8) (Addr& 0xff));
      IoWrite8 (0xED, 0);//short delay.
  
      //
      // AST2500 LPC2AHB interface support only 4-byte alighment read/write
      //
      Offset = 0xF7 - (Addr % 4);
      IoWrite8 (AST2500_CONFIG_INDEX, Offset);
      IoWrite8 (AST2500_CONFIG_DATA, *Buffer);
      IoWrite8 (0xED, 0);//short delay.
      //
      // Trigger write command by writing 0xCF to offset 0xFE
      //
      IoWrite8 (AST2500_CONFIG_INDEX, 0xFE);
      IoWrite8 (AST2500_CONFIG_DATA, 0xCF);
      Addr++;
      Buffer++;
    }
  }
  return Status;
}

/**
 * This routine is to set specific shared memory region 
 *  
 * @author AY (2017/12/07)
 * 
 * @param[in] AddrOff   Offset address to BMC share mem base 
 *       address
 * @param[in] Length    Data size to set
 * @param[in] Value     Initialization value
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
EFIAPI
ShareMemSet (
  VOID *AddrOff,
  UINT32 Length,
  UINT8  Value
  )
{
  UINT8 Data8;
  UINT8 Offset;
  UINT32 i;
  UINT32 PageStart;
  UINT32 PageCount;
  UINT32 PageNum;
  UINT32 OffInStartPage;
  UINT32 LeftByteInStart;
  UINT32 LeftByteInEnd;
  UINT32 StartAddr;
  UINT32 ShareMemBaseConfigAddr = 0xffffffff;
  UINT32 ShareMemPciBarAddr = 0xffffffff;
  UINT32 ShareMemWriteBase;
  BOOLEAN  CrossPage = TRUE;
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT32 Addr = (UINT32) AddrOff;
  
  if ((Addr > SHARE_MEM_SIZE) ||((Addr + Length) > SHARE_MEM_SIZE)) {
    return EFI_INVALID_PARAMETER;
  }
  
  ShareMemPciBarAddr = PcdGet32 (PcdBmcP2ABarAddr);
  if (!SHARE_MEM_LPC_ALWAYS && (ShareMemPciBarAddr != 0xffffffff)) { //Bmc P2A Bar assigned with resource, use MMIO way
    DEBUG ((EFI_D_ERROR, "OemDbg:Bmc P2A Bar allocated, go with MMIO.\n"));
    ShareMemWriteBase = ShareMemPciBarAddr + SHARE_MEM_BLOCK_SIZE;
	  ShareMemBaseConfigAddr = ShareMemPciBarAddr + BMC_P2A_CONFIG_OFFSET;
	  
	  PageStart = (UINT32)AddrOff / SHARE_MEM_BLOCK_SIZE;
	  OffInStartPage = (UINT32)AddrOff % SHARE_MEM_BLOCK_SIZE;
	  LeftByteInStart = SHARE_MEM_BLOCK_SIZE - OffInStartPage;

    if (Length <= LeftByteInStart) {
      PageCount = 0;
      LeftByteInEnd = 0;
      CrossPage = FALSE;
    } else {
      PageCount = (Length - LeftByteInStart) / SHARE_MEM_BLOCK_SIZE;
      LeftByteInEnd = (Length - LeftByteInStart) % SHARE_MEM_BLOCK_SIZE;
      CrossPage = TRUE;
    }

    //
    // Set start page in BAR
    // 0xEF00F004 is share memory configuration address
    // 0xEF010000 is mapping address
    // Note that 32M region from that address is reserved by BMC as share memory
    // 0x9C000000 is page 0 from a BMC perspective
    // 0x9C010000 is page 1 from a BMC perspective
    //
    
    MmioWrite32 (ShareMemBaseConfigAddr, SHARE_MEM_BASE_BMC_ADDR + PageStart*SHARE_MEM_BLOCK_SIZE);
    //
    // Write first page
    //
    if (!CrossPage) {
      SetMem ((VOID*)(ShareMemWriteBase + OffInStartPage), Length, Value);
      Status = EFI_SUCCESS;
    } else {
      SetMem ((VOID*)(ShareMemWriteBase + OffInStartPage), LeftByteInStart, Value);

      //
      // Write remaining pages
      //
      StartAddr = SHARE_MEM_BASE_BMC_ADDR + (PageStart + 1)*SHARE_MEM_BLOCK_SIZE;
      for (PageNum = 1; PageNum <= PageCount; PageNum++) {
        //
        // Change page by configuring 0xEF00F004
        //
        MmioWrite32 (ShareMemBaseConfigAddr, StartAddr);
        SetMem ((VOID*)ShareMemWriteBase, SHARE_MEM_BLOCK_SIZE, Value);
        StartAddr += PageNum*SHARE_MEM_BLOCK_SIZE;
      }
      MmioWrite32 (ShareMemBaseConfigAddr, StartAddr);
      SetMem ((VOID*)ShareMemWriteBase, LeftByteInEnd, Value);
      Status = EFI_SUCCESS;
    }
  } else {
	  DEBUG ((EFI_D_ERROR, "OemDbg:Bmc P2A Bar not yet allocated, go with LPC.\n"));
    Addr += SHARE_MEM_BASE_BMC_ADDR;
    //
    // Enable iLPC2AHB
    //
    IoWrite8 (AST2500_CONFIG_INDEX, 0xA5);
    IoWrite8 (AST2500_CONFIG_INDEX, 0xA5);
    IoWrite8 (AST2500_CONFIG_INDEX, 0x07);
    IoWrite8 (AST2500_CONFIG_DATA, 0x0D);
    IoWrite8 (AST2500_CONFIG_INDEX, 0x30);
    Data8 = IoRead8 (AST2500_CONFIG_DATA);
    Data8 |= BIT0;
    IoWrite8 (AST2500_CONFIG_INDEX, 0x30);
    IoWrite8 (AST2500_CONFIG_DATA, Data8);
    
    for (i=0; i< Length; i++) {
      IoWrite8 (AST2500_CONFIG_INDEX, 0xF0);
      IoWrite8 (0xED, 0);//short delay.
      IoWrite8 (AST2500_CONFIG_DATA, (UINT8) ((Addr >> 24)& 0xff));
      IoWrite8 (0xED, 0);//short delay.
      IoWrite8 (AST2500_CONFIG_INDEX, 0xF1);
      IoWrite8 (0xED, 0);//short delay.
      IoWrite8 (AST2500_CONFIG_DATA, (UINT8) ((Addr >> 16)& 0xff));
      IoWrite8 (0xED, 0);//short delay.
      IoWrite8 (AST2500_CONFIG_INDEX, 0xF2);
      IoWrite8 (0xED, 0);//short delay.
      IoWrite8 (AST2500_CONFIG_DATA, (UINT8) ((Addr >> 8)& 0xff));
      IoWrite8 (0xED, 0);//short delay.
      IoWrite8 (AST2500_CONFIG_INDEX, 0xF3);
      IoWrite8 (0xED, 0);//short delay.
      IoWrite8 (AST2500_CONFIG_DATA, (UINT8) (Addr& 0xff));
      IoWrite8 (0xED, 0);//short delay.
  
      //
      // AST2500 LPC2AHB interface support only 4-byte alighment read/write
      //
      Offset = 0xF7 - (Addr % 4);
      IoWrite8 (AST2500_CONFIG_INDEX, Offset);
      IoWrite8 (AST2500_CONFIG_DATA, Value);
      IoWrite8 (0xED, 0);//short delay.
      //
      // Trigger write command by writing 0xCF to offset 0xFE
      //
      IoWrite8 (AST2500_CONFIG_INDEX, 0xFE);
      IoWrite8 (AST2500_CONFIG_DATA, 0xCF);
      Addr++;
    }
  }
  return Status;
}

/**
 * This is routine for reading Mailbox register through LPC
 * 
 * @author AY (2017/12/26)
 * 
 * @param[in] RegOff    Register offset, 0-based
 * @param[in/out] Value  Value returned
 * 
 * @return EFI_STATUS
 */
EFI_STATUS
EFIAPI
MailboxRead (
  UINT16 RegOff,
  UINT8 *Value
  )
{
  UINT8 Data8;
  UINT16 IoAddr = RegOff + MAILBOX_IO_BASE;
  if (RegOff > 24 || Value == NULL) { // Mailbox totally has 24 registers
    return EFI_INVALID_PARAMETER;
  }

  //
  // Enable iLPC2Mailbox, mailbox device is 0xE
  //
  IoWrite8 (AST2500_CONFIG_INDEX, 0xA5);
  IoWrite8 (AST2500_CONFIG_INDEX, 0xA5);
  IoWrite8 (AST2500_CONFIG_INDEX, 0x07);
  IoWrite8 (AST2500_CONFIG_DATA, 0x0E);
  IoWrite8 (AST2500_CONFIG_INDEX, 0x30);
  Data8 = IoRead8 (AST2500_CONFIG_DATA);
  if (!(Data8 & BIT0)) {
    Data8 |= BIT0;
    IoWrite8 (AST2500_CONFIG_INDEX, 0x30);
    IoWrite8 (AST2500_CONFIG_DATA, Data8);
  }

  //
  // Configure MailboxIoBase, this can be done outside this API once so to make code more efficient
  //
  IoWrite8 (AST2500_CONFIG_INDEX, 0x60);
  IoWrite8 (0xED, 0);//short delay.
  IoWrite8 (AST2500_CONFIG_DATA, (UINT8) ((MAILBOX_IO_BASE >> 8)& 0xff));
  IoWrite8 (0xED, 0);//short delay.
  IoWrite8 (AST2500_CONFIG_INDEX, 0x61);
  IoWrite8 (0xED, 0);//short delay.
  IoWrite8 (AST2500_CONFIG_DATA, (UINT8) (MAILBOX_IO_BASE& 0xff));

  *Value = IoRead8 (IoAddr);
  return EFI_SUCCESS;
}

/**
 * This is the routine for writing specific mailbox register 
 * through LPC 
 * 
 * @author AY (2017/12/26)
 * 
 * @param[in] RegOff  Register offset
 * @param[in] Value   Value to write
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
EFIAPI
MailboxWrite (
  UINT16 RegOff,
  UINT8  Value
  )
{
  UINT8  Data8;
  UINT16 IoAddr = RegOff + MAILBOX_IO_BASE;
  if (RegOff > 24) { // Mailbox totally has 24 registers
    return EFI_INVALID_PARAMETER;
  }

  //
  // Enable iLPC2Mailbox, mailbox device is 0xE
  //
  IoWrite8 (AST2500_CONFIG_INDEX, 0xA5);
  IoWrite8 (AST2500_CONFIG_INDEX, 0xA5);
  IoWrite8 (AST2500_CONFIG_INDEX, 0x07);
  IoWrite8 (AST2500_CONFIG_DATA, 0x0E);
  IoWrite8 (AST2500_CONFIG_INDEX, 0x30);
  Data8 = IoRead8 (AST2500_CONFIG_DATA);
  if (!(Data8 & BIT0)) {
    Data8 |= BIT0;
    IoWrite8 (AST2500_CONFIG_INDEX, 0x30);
    IoWrite8 (AST2500_CONFIG_DATA, Data8);
  }

  //
  // Configure MailboxIoBase, this can be done outside this API once so to make code more efficient
  //
  IoWrite8 (AST2500_CONFIG_INDEX, 0x60);
  IoWrite8 (0xED, 0);//short delay.
  IoWrite8 (AST2500_CONFIG_DATA, (UINT8) ((MAILBOX_IO_BASE >> 8)& 0xff));
  IoWrite8 (0xED, 0);//short delay.
  IoWrite8 (AST2500_CONFIG_INDEX, 0x61);
  IoWrite8 (0xED, 0);//short delay.
  IoWrite8 (AST2500_CONFIG_DATA, (UINT8) (MAILBOX_IO_BASE& 0xff));

  IoWrite8 (IoAddr, Value);
  return EFI_SUCCESS;
}
