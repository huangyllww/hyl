/* $NoKeywords:$ */
/**
 * @file
 *
 * JedecNvdimmSmbus.c
 * 
 * 
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  JedecNvdimm
 * @e \$Revision$   @e \$Date$
 *
 */
/*
 ******************************************************************************
 *
 * 
 * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
 * 
 * HYGON is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with HYGON.  This header does *NOT* give you permission to use the Materials
 * or any rights under HYGON's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 * 
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by HYGON shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 * 
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 * 
 * HYGON does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by HYGON, or
 * result from use of the Materials or any related information.
 * 
 * You agree that you will not reverse engineer or decompile the Materials.
 * 
 * NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, HYGON retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 * 
 * AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
 * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 * the right to make the modified version available for use with HYGON's PRODUCT.
 ******************************************************************************
 */

/** @file JedecNvdimmSmbus.c
    JedecNvdimmSmbus Driver Implementation

**/

//---------------------------------------------------------------------------

#include "JedecNvdimm.h"
#include "JedecNvdimmSmbus.h"
#include "JedecNvdimmDsm.h"
#include <AmdBoardId.h>

STATIC SMBUS_INSTANCE        *mSmbus;

#define NUM_DIMM_INFO_ENTRIES   sizeof(DimmSmbus)/sizeof(AMD_DIMM_INFO_SMBUS)

// ---- PORTING REQUIRED ------
// Below is configuration for CRB 

AMD_DIMM_INFO_SMBUS DimmSmbus[] =
{
    //  DIMM_SPD_MAP
    {0, 0, 0xA0},
    {0, 1, 0xA2},
    {1, 0, 0xA4},
    {1, 1, 0xA6},
    {2, 0, 0xA8},
    {2, 1, 0xAA},
    {3, 0, 0xAC},
    {3, 1, 0xAE},
    {4, 0, 0xA0},
    {4, 1, 0xA2},
    {5, 0, 0xA4},
    {5, 1, 0xA6},
    {6, 0, 0xA8},
    {6, 1, 0xAA},
    {7, 0, 0xAC},
    {7, 1, 0xAE},  
};
// ---- END OF PORTING -----

/**
  Init Base Address

  @param    Smbus   A pointer to the SMBUS data structure

  @retval   VOID

**/

VOID
SetDataAndRegister (
  IN SMBUS_INSTANCE             *Smbus
  )
{
    SmbusIoWrite (Smbus, HSTS, HSTS_ALL);
}

/**
  Use the PCI_ROOT_Bridge_IO to do an 8 bits I/O port read

  @param    Private     A pointer to the SMBUS data structure
            Offset      The address to read from

  @retval   UINT8       The read value

**/

UINT8
SmbusIoRead (
  IN SMBUS_INSTANCE     *Private,
  IN UINT8                      Offset
  )
{
    UINT8                                                         FchSmbusData8;
    
    LibAmdIoRead (AccessWidth8, Private->SmbusIoBase + Offset, &FchSmbusData8, NULL);
    
    return FchSmbusData8;
}

/**
  Use the PCI_ROOT_Bridge_IO to do an 8 bits I/O port write

  @param    Private     A pointer to the SMBUS data structure
            Offset      The address to read from
            Data        The write value

  @retval   VOID

**/

VOID
SmbusIoWrite (
  IN SMBUS_INSTANCE     *Private,
  IN UINT8                      Offset,
  IN UINT8                      Data
  )
{
    LibAmdIoWrite (AccessWidth8, Private->SmbusIoBase + Offset, &Data, NULL);
}

/**
  Setting flags for access SMBUS operation

  @param    Operation       
            PecCheck        
            Length          
            Len             
            SlaveAddressReg 
            HostCommandReg  
            SmbusCommand    

  @retval   EFI_STATUS      Input is avaliable or invalid

**/

EFI_STATUS
ProcessOperation (
    IN       EFI_SMBUS_OPERATION        Operation,
    IN       BOOLEAN                    PecCheck,
    IN OUT   UINTN                      *Length,
    IN       UINT8                      Len,
       OUT   UINT8                      *SlaveAddressReg,
       OUT   UINT8                      *HostCommandReg,
       OUT   UINTN                      *SmbusCommand
  )
{
    EFI_STATUS                          Status;
    
    Status = EFI_SUCCESS;
    switch (Operation) {
        case EfiSmbusQuickWrite:
            *SlaveAddressReg &= ~READ_ENABLE;
    
        case EfiSmbusQuickRead:
            if (PecCheck) {
            Status = EFI_UNSUPPORTED;
            }
            break;
            
        case EfiSmbusSendByte:
            *HostCommandReg = Len;
            *SlaveAddressReg &= ~READ_ENABLE;
        
        case EfiSmbusReceiveByte:
            *SmbusCommand = SMB_CMD_BYTE;
            if (*Length < 1) {
                Status = EFI_BUFFER_TOO_SMALL;
            }
            *Length = 1;
            break;
        
        case EfiSmbusWriteByte:
            *SlaveAddressReg &= ~READ_ENABLE;
        
        case EfiSmbusReadByte:
            *SmbusCommand = SMB_CMD_BYTE_DATA;
            if (*Length < 1) {
                Status = EFI_BUFFER_TOO_SMALL;
            }
            *Length = 1;
            break;
        
        case EfiSmbusReadWord:
            *SmbusCommand = SMB_CMD_WORD_DATA;
            if (*Length < 2) {
                Status = EFI_BUFFER_TOO_SMALL;
            }
            *Length = 2;
            break;
        
        case EfiSmbusWriteWord:
            *SmbusCommand = SMB_CMD_WORD_DATA;
            *SlaveAddressReg &= ~READ_ENABLE;
            if (*Length < 2) {
                Status = EFI_BUFFER_TOO_SMALL;
            }
            *Length = 2;
            break;
        
        case EfiSmbusWriteBlock:
            *SlaveAddressReg &= ~READ_ENABLE;
            *SmbusCommand = SMB_CMD_BLOCK;
            break;
        
        case EfiSmbusReadBlock:
            *SmbusCommand = SMB_CMD_BLOCK;
            if ((*Length < 1) || (*Length > 72)) {
                Status = EFI_INVALID_PARAMETER;
            }
            break;
        
        case EfiSmbusBWBRProcessCall:
            Status = EFI_UNSUPPORTED;
            break;
        
        default:
            Status = EFI_INVALID_PARAMETER;
            break;
    }
    return Status;
}

/**
  Check for successful completion

  @param    Operation
            Smbus
            Buf
            Length

  @retval   EFI_STATUS

**/

EFI_STATUS
SuccessfulCompletion (
    IN       EFI_SMBUS_OPERATION        Operation,
    IN       SMBUS_INSTANCE             *Smbus,
    IN       UINT8                      *Buf,
    IN OUT   UINTN                      *Length
  )
{
    EFI_STATUS                          Status;
    UINTN                               Index;
    UINTN                               BlockCount;
    
    Status = EFI_SUCCESS;
    
    switch (Operation) {
        case EfiSmbusReadWord:
        case EfiSmbusProcessCall:
            Buf [1] = Smbus->SmbusIoRead (Smbus, HD1);
    
        case EfiSmbusReadByte:
            Buf [0] = Smbus->SmbusIoRead (Smbus, HD0);
            break;
        
        case EfiSmbusReadBlock:
            BlockCount = Smbus->SmbusIoRead (Smbus, HD0);
            if (*Length < BlockCount) {
                BlockCount = *Length;
            }
            for (Index = 0; Index < BlockCount; ++Index) {
                Buf [Index] = Smbus->SmbusIoRead (Smbus, HBD);
            }
            *Length = BlockCount;
            break;
    
        default:
            break;
    }
    return Status;
}

//---------------------------------------------------------------------------

/**
    This function changes the SMBUS page number.
    A dummy write to the slave address of 6E is made to switch the page address to 1, prior to the SPD reading commands.
    Upon completion, another dummy write to the slave address of 6C is made to restore the original page address

    @param PeiServices PEI Services table pointer
    @param Page        Page number for switch
         
    @retval EFI_STATUS return EFI status

**/
VOID
SmmSmbusPageChange (
  IN  UINT8                             Page
  )
{
    EFI_STATUS                          Status;
    EFI_SMBUS_DEVICE_ADDRESS            SmbusSlaveAddress;
    UINTN                               SmbusLength = 0;
    UINTN                               SmbusOffset = 0;
    UINT8                               Buffer = 0;

    // A dummy write to the slave address of 6E is made to switch the page address to 1, prior to the SPD reading commands.
    // Upon completion, another dummy write to the slave address of 6C is made to restore the original page address
    
    SmbusLength = 1;
    SmbusOffset = 0;
    if (Page == 1)
    {
        SmbusSlaveAddress.SmbusDeviceAddress = (0x6E >> 1);
        Status = mSmbus->SmbusController.Execute (&mSmbus->SmbusController, SmbusSlaveAddress, SmbusOffset,
                                EfiSmbusWriteByte, FALSE, &SmbusLength,
                                &Buffer );
    }
    else
    {
        SmbusSlaveAddress.SmbusDeviceAddress = (0x6C >> 1);
        Status = mSmbus->SmbusController.Execute (&mSmbus->SmbusController, SmbusSlaveAddress, SmbusOffset,
                                EfiSmbusWriteByte, FALSE, &SmbusLength,
                                &Buffer );
    }
    
}

/**
    This function changes the NVDIMM page number

    @param Page        Page number for switch
    
    @retval VOID

**/
VOID
NvdimmPageChange (
  IN  EFI_SMBUS_DEVICE_ADDRESS          SmbusSlaveAddress,
  IN  UINT8                             Page
  )
{
    EFI_STATUS                          Status;
    UINTN                               SmbusLength = 0;
    UINT8                               Buffer = 0;
    UINT8                               Retry = 10;

    SmbusLength = 1;
    Status = mSmbus->SmbusController.Execute (&mSmbus->SmbusController, SmbusSlaveAddress, PAGE0_OPEN_PAGE,
                          EfiSmbusReadByte, FALSE, &SmbusLength,
                          &Buffer);
    
    if (Buffer == Page) return;
    
    Buffer = Page;
    Status = mSmbus->SmbusController.Execute (&mSmbus->SmbusController, SmbusSlaveAddress, PAGE0_OPEN_PAGE,
                              EfiSmbusWriteByte, FALSE, &SmbusLength,
                              &Buffer);
    
    do {
        FixedDelay (1000);
        Status = mSmbus->SmbusController.Execute (&mSmbus->SmbusController, SmbusSlaveAddress, PAGE0_OPEN_PAGE,
                                  EfiSmbusReadByte, FALSE, &SmbusLength,
                                  &Buffer);
        Retry--;
    } while ((Buffer != Page) && (Retry > 0));
    return;
}

/**
  Check for slave status

  @param    Smbus

  @retval   EFI_STATUS

**/

EFI_STATUS
CheckSlaveStatus (
    IN SMBUS_INSTANCE       *Smbus
    )
{
    UINT32        LoopCount;
    UINT32        LoopCount1;
    UINT8         dbSmbusTempVar;
    BOOLEAN   ControllerBusy;
    
    ControllerBusy = TRUE;
    LoopCount1 = 3;
    
    while (LoopCount1 && ControllerBusy) {
        LoopCount = 100;
    
        while (LoopCount && ControllerBusy) {
            Smbus->SmbusIoWrite (Smbus, SMBUS_R_SSTS, SMBUS_B_SSTS_ALL); // out index 00h, 0ffh to clear status.
            FixedDelay (2);
            dbSmbusTempVar = Smbus->SmbusIoRead (Smbus, SMBUS_R_SSTS);
            
            if (dbSmbusTempVar & SMBUS_B_SBSY) {
                // Bit0 = 1, controller is still busy!
                LoopCount--;
            } else {
                ControllerBusy = FALSE;
            }
        } // end of while (loopCount)
    
        if (ControllerBusy) {
            Smbus->SmbusIoWrite (Smbus, SMBUS_R_SSTS, SMBUS_B_SINIT);    // slave init
            LoopCount1--;
        }
    }
    
    if (ControllerBusy) {
        return EFI_TIMEOUT;
    } else {
        return EFI_SUCCESS;
    }
}

/**
  Execute the SMBUS command by calling the worker functions

  @param    This
            SlaveAddress
            Command
            Operation
            PecCheck
            Length
            Buffer

  @retval   EFI_STATUS

**/

EFI_STATUS
EFIAPI
SmbusExecute (
    IN       CONST EFI_SMBUS_HC_PROTOCOL        *This,
    IN       EFI_SMBUS_DEVICE_ADDRESS           SlaveAddress,
    IN       EFI_SMBUS_DEVICE_COMMAND           Command,
    IN       EFI_SMBUS_OPERATION                Operation,
    IN       BOOLEAN                            PecCheck,
    IN OUT   UINTN                              *Length,
    IN OUT   VOID                               *Buffer
    )
{
    EFI_STATUS                                  Status;
    SMBUS_INSTANCE                              *Smbus;
    UINT8                                       StsReg,
                                                SlaveAddressReg,
                                                HostCommandReg,
                                                *Buf;
    UINTN                                       Count,
                                                Index,
                                                SmbusCommand;
    UINT32                                      LoopCount;
    UINT8                                       dbSmbusTempVar;
    UINT8                                       bValue;
    BOOLEAN                                     IsBusy;
    UINT8                                       retryCount;

    Status = EFI_SUCCESS;
    Buf = (UINT8 *)Buffer;
    Smbus = SMBUS_INSTANCE_FROM_THIS (This);
    SetDataAndRegister (Smbus);
    
    //
    // Host get SMBUS ownership
    //
    LoopCount = 500;
    while (LoopCount) {
        LibAmdIoRead (AccessWidth8,  Smbus->SmbusIoBase + SMB_IOREG08, &dbSmbusTempVar, NULL);
        if (dbSmbusTempVar & BIT6) {
            LoopCount--;
        } else {
            break;
        }
    }

    dbSmbusTempVar |= BIT4;
    LibAmdIoWrite (AccessWidth8,  Smbus->SmbusIoBase + SMB_IOREG08, &dbSmbusTempVar, NULL);

    if (Operation != EfiSmbusQuickRead && Operation != EfiSmbusQuickWrite && (Length == NULL ||  Buffer == NULL)) {
        Status = EFI_INVALID_PARAMETER;
    }
    IsBusy = TRUE;
    retryCount = 3;
    while (retryCount--) {
        LoopCount = 100;
        do {
            dbSmbusTempVar = 0x1f;
            LibAmdIoWrite (AccessWidth8,  Smbus->SmbusIoBase, &dbSmbusTempVar, NULL);
            LibAmdIoRead (AccessWidth8, 0xed, &bValue, NULL);
            LibAmdIoRead (AccessWidth8, 0xed, &bValue, NULL);
            LibAmdIoRead (AccessWidth8,  Smbus->SmbusIoBase, &dbSmbusTempVar, NULL);
            if (!( dbSmbusTempVar & BIT0)) {
                IsBusy = FALSE;         // Bit0 = 0, controller is idle
                break;
            }
        } while (LoopCount--);
        if (!IsBusy) {
            break;
        }
        dbSmbusTempVar = BIT1;
        LibAmdIoWrite (AccessWidth8,  Smbus->SmbusIoBase + 2, &dbSmbusTempVar, NULL);
    }

    if (!EFI_ERROR (Status)) {
        StsReg = Smbus->SmbusIoRead (Smbus, HSTS);
        if ((StsReg & IUS) != 0) {
            Status = EFI_TIMEOUT;
        } else if (StsReg & HBSY) {
            Smbus->SmbusIoWrite (Smbus, HSTS, HSTS_ALL);
            Status = EFI_TIMEOUT;
        } else {
            Status = CheckSlaveStatus (Smbus);
        }

        if (!EFI_ERROR (Status)) {
            for (Count = 0; Count < TRY_TIMES; ++Count) {
                SmbusCommand = SMB_CMD_QUICK;
                SlaveAddressReg = (UINT8) ((SlaveAddress.SmbusDeviceAddress << 1) | READ_ENABLE);
                HostCommandReg = (UINT8)Command;
                Status = ProcessOperation (
                        Operation,
                        PecCheck,
                        Length,
                        *Buf,
                        &SlaveAddressReg,
                        &HostCommandReg,
                        &SmbusCommand
                );

                if (EFI_ERROR (Status)) {
                    break;
                }

                Smbus->SmbusIoWrite (Smbus, HSTS, (HBSY + INTR + DERR + BERR + FAIL));
                Smbus->SmbusIoWrite (Smbus, TSA, SlaveAddressReg);
                Smbus->SmbusIoWrite (Smbus, HCMD, HostCommandReg);
                Smbus->SmbusIoWrite (Smbus, HCTL, (UINT8) (SmbusCommand));
                //
                // Do a read from Reg02 to reset
                // the counter if it going to be a
                // block read/write operation
                //
                StsReg = Smbus->SmbusIoRead (Smbus, HCTL);
                if ((SlaveAddressReg & READ_ENABLE) != 0) {
                    if (SmbusCommand == SMB_CMD_BLOCK) {
                        Smbus->SmbusIoWrite (Smbus, HD0, (UINT8)*Length);
                    }
                } else {
                    if (SmbusCommand == SMB_CMD_BLOCK) {
                        Smbus->SmbusIoWrite (Smbus, HD0, (UINT8)*Length);
                        for (Index = 0; Index < *Length; Index++) {
                            Smbus->SmbusIoWrite (Smbus, HBD, Buf [Index]);
                        }
                    } else if (SmbusCommand == SMB_CMD_WORD_DATA) {
                        Smbus->SmbusIoWrite (Smbus, HD1, Buf [1]);
                        Smbus->SmbusIoWrite (Smbus, HD0, Buf [0]);
                    } else {
                        Smbus->SmbusIoWrite (Smbus, HD0, Buf [0]);
                    }
                }

                Smbus->SmbusIoWrite (Smbus, HCTL, (UINT8) (SmbusCommand + START + ((PecCheck) ? (StsReg | 0xA0) : 0)));

                if (!(Smbus->IoDone (Smbus, &StsReg))) {
                    Status = EFI_TIMEOUT;
                    break;
                } else if (StsReg & DERR) {
                    Status = EFI_DEVICE_ERROR;
                    break;
                } else if (StsReg & BERR) {
                    Status = EFI_DEVICE_ERROR;
                    Smbus->SmbusIoWrite (Smbus, HSTS, BERR);
                    continue;
                }

                Status = SuccessfulCompletion (Operation, Smbus, Buf, Length);
        
                if ((StsReg & BERR) && (Status != EFI_BUFFER_TOO_SMALL)) {
                    Status = EFI_DEVICE_ERROR;
                    Smbus->SmbusIoWrite (Smbus, HSTS, BERR);
                    continue;
                } else {
                    break;
                }
            } // end for

            Smbus->SmbusIoWrite (Smbus, HSTS, (HBSY + INTR + DERR + BERR + FAIL));
        } //end if (!EFI_ERROR (Status))
    } //end if (!EFI_ERROR (Status))

    //
    // release SMBUS ownership
    //
    LibAmdIoRead (AccessWidth8, Smbus->SmbusIoBase + SMB_IOREG08, &dbSmbusTempVar, NULL);
    dbSmbusTempVar |= BIT5;
    LibAmdIoWrite (AccessWidth8, Smbus->SmbusIoBase + SMB_IOREG08, &dbSmbusTempVar, NULL);
    return Status;
}

/**
  Select which SMBUS I2C Mux to use

  @param    I2CMuxAddress
            I2CCommand
            I2CChannelAddress

  @retval   VOID

**/

VOID
SmbusI2CMuxSelection (

  IN  UINT8   I2CMuxAddress,
  IN  UINT8   I2CCommand,
  IN  UINT8   I2CChannelAddress
)
{
    EFI_STATUS                          Status;
    EFI_SMBUS_DEVICE_ADDRESS            SmbusSlaveAddress;
    UINTN                               SmbusLength = 0;

    SmbusLength = 1;
    SmbusSlaveAddress.SmbusDeviceAddress = (I2CMuxAddress >> 1);
    Status = mSmbus->SmbusController.Execute (&mSmbus->SmbusController, SmbusSlaveAddress, 
                                I2CCommand,
                                EfiSmbusWriteByte, 
                                FALSE, 
                                &SmbusLength,
                                &I2CChannelAddress);
}

/**
  Check SMBUS status register

  @param    Private     A pointer to the SMBUS data structure
            StsReg      Test Bit

  @retval   BOOLEAN     Timeout or succesfully

**/

BOOLEAN
IoDone (
    IN SMBUS_INSTANCE             *Private,
    IN UINT8                      *StsReg
    )
{
    BOOLEAN                       Ret;
    UINTN                         StallTries;
    UINTN                         StallIndex;
    
    Ret = FALSE;
    
    for (StallTries = STALL_TIME / Private->StallTime, StallIndex = 0; StallIndex < StallTries; ++StallIndex) {
        *StsReg = Private->SmbusIoRead (Private, HSTS);
        if (*StsReg & (INTR | BYTE_DONE_STS | DERR | BERR)) {
            Ret = TRUE;
            break;
        }
        FixedDelay(Private->StallTime);
    }
    return Ret;
}

/**
    Read byte from NVDIMM

    @param    Socket
    @param    Channel
    @param    Dimm
    @param    Page
    @param    Register
    @param    *Data
    @param    IsSpd

    @retval   EFI_SUCCESS

**/

EFI_STATUS
ReadNvdimm (
    UINT8    Socket,
    UINT8    Channel,
    UINT8    Dimm,
    UINT8    Page,
    UINT8    Register,
    UINT8    *Data,
    BOOLEAN  IsSpd    
)
{
    EFI_SMBUS_DEVICE_ADDRESS      SmbusSlaveAddress;
    UINTN                         Index;
    UINT8                         SmbusAddress=0;
    EFI_STATUS                    Status;
    UINTN                         SmbusLength;
    HYGON_BOARD_ID                PlatformSelect;
    PlatformSelect = PcdGet8(PcdPlatformSelect);
    
    for (Index = 0; Index < NUM_DIMM_INFO_ENTRIES; Index++)
    {
        if ((Channel == DimmSmbus[Index].ChannelId)
           && (Dimm == DimmSmbus[Index].DimmId))
        {
            SmbusAddress = DimmSmbus[Index].SmbusAddress;            
            Status = EFI_SUCCESS;
            break;
        }
    }
    
    //
    // Switch the channel based off following information before initiating SPD read.
    //
    
    if ((PlatformSelect == HYGON_35N16) || (PlatformSelect == HYGON_65N32) || (PlatformSelect == HYGON_52D16) ||
        (PlatformSelect == HYGON_35N16SL1R2) || (PlatformSelect == HYGON_65N32SL1R2)) {
        if (Socket == 0) {
            if ((Channel >= 0) && (Channel < 4)) {
                SmbusI2CMuxSelection (0xE0, 0xff, 0x01);
            }
            if ((Channel >= 4) && (Channel < 8)) {
                SmbusI2CMuxSelection (0xE0, 0xff, 0x02);
            }
        }
        if (Socket == 1) {
            if ((Channel >= 0) && (Channel < 4)) {
                SmbusI2CMuxSelection (0xE0, 0xff, 0x04);
            }
            if ((Channel >= 4) && (Channel < 8)) {
                SmbusI2CMuxSelection (0xE0, 0xff, 0x08);
            }
        }
    } else {
        if (Socket == 0) {
            if ((Channel >= 0) && (Channel < 4)) {
                SmbusI2CMuxSelection (0x94, 0x03, 0x80);
            }
            if ((Channel >= 4) && (Channel < 8)) {
                SmbusI2CMuxSelection (0x94, 0x03, 0x40);
            }
        }    
        if (Socket == 1) {
            if ((Channel >= 0) && (Channel < 4)) {
                SmbusI2CMuxSelection (0x94, 0x03, 0x20);
            }
            if ((Channel >= 4) && (Channel < 8)) {
                SmbusI2CMuxSelection (0x94, 0x03, 0x10);
            }
        }
    }
    SmbusSlaveAddress.SmbusDeviceAddress = (SmbusAddress >> 1);
    
    // Change to input page number
    if (IsSpd) {
        if (Page != 0) SmmSmbusPageChange (Page);
    }
    else {
        SmbusAddress -= 0x20;
        SmbusSlaveAddress.SmbusDeviceAddress = (SmbusAddress >> 1);
        NvdimmPageChange(SmbusSlaveAddress, Page);
    }
    
    SmbusLength = 1;    
    Status = mSmbus->SmbusController.Execute (&mSmbus->SmbusController, SmbusSlaveAddress, Register,
                          EfiSmbusReadByte, FALSE, &SmbusLength,
                          Data);
    
    // Restore page number back to 0 as in PEI
    if (IsSpd) {
        if (Page != 0) SmmSmbusPageChange (0);
    }
    
    return Status;
}

/**
    Write byte to NVDIMM

    @param    Socket
    @param    Channel
    @param    Dimm
    @param    Page
    @param    Register
    @param    *Data
    @param    IsSpd

    @retval   EFI_SUCCESS

**/

EFI_STATUS
WriteNvdimm (
    UINT8    Socket,
    UINT8    Channel,
    UINT8    Dimm,
    UINT8    Page,
    UINT8    Register,
    UINT8    *Data,
    BOOLEAN  IsSpd   
)
{
    EFI_SMBUS_DEVICE_ADDRESS      SmbusSlaveAddress;
    UINTN                         Index;
    UINT8                         SmbusAddress=0;
    EFI_STATUS                    Status;
    UINTN                         SmbusLength;
    HYGON_BOARD_ID                PlatformSelect;
    PlatformSelect = PcdGet8(PcdPlatformSelect);
    
    for (Index = 0; Index < NUM_DIMM_INFO_ENTRIES; Index++)
    {
        if ((Channel == DimmSmbus[Index].ChannelId)
           && (Dimm == DimmSmbus[Index].DimmId))
        {
            SmbusAddress = DimmSmbus[Index].SmbusAddress;            
            Status = EFI_SUCCESS;
            break;
        }
    }
    
    //
    // Switch the channel based off following information before initiating SPD read.
    //
    if ((PlatformSelect == HYGON_35N16) || (PlatformSelect == HYGON_65N32) || (PlatformSelect == HYGON_52D16) ||
        (PlatformSelect == HYGON_35N16SL1R2) || (PlatformSelect == HYGON_65N32SL1R2)) {
        if (Socket == 0) {
            if ((Channel >= 0) && (Channel < 4)) {
                SmbusI2CMuxSelection (0xE0, 0xff, 0x01);
            }
            if ((Channel >= 4) && (Channel < 8)) {
                SmbusI2CMuxSelection (0xE0, 0xff, 0x02);
            }
        }    
        if (Socket == 1) {
            if ((Channel >= 0) && (Channel < 4)) {
                SmbusI2CMuxSelection (0xE0, 0xff, 0x04);
            }
            if ((Channel >= 4) && (Channel < 8)) {
                SmbusI2CMuxSelection (0xE0, 0xff, 0x08);
            }
        }
    } else {
        if (Socket == 0) {
            if ((Channel >= 0) && (Channel < 4)) {
                SmbusI2CMuxSelection (0x94, 0x03, 0x80);
            }
            if ((Channel >= 4) && (Channel < 8)) {
                SmbusI2CMuxSelection (0x94, 0x03, 0x40);
            }
        }    
        if (Socket == 1) {
            if ((Channel >= 0) && (Channel < 4)) {
                SmbusI2CMuxSelection (0x94, 0x03, 0x20);
            }
            if ((Channel >= 4) && (Channel < 8)) {
                SmbusI2CMuxSelection (0x94, 0x03, 0x10);
            }
        }
    }
    SmbusSlaveAddress.SmbusDeviceAddress = (SmbusAddress >> 1);
    
    // Change to input page number
    if (IsSpd) {
        if (Page != 0) SmmSmbusPageChange (Page);
    }
    else {
        SmbusAddress -= 0x20;
        SmbusSlaveAddress.SmbusDeviceAddress = (SmbusAddress >> 1);
        NvdimmPageChange(SmbusSlaveAddress, Page);
    }
    
    SmbusLength = 1;    
    Status = mSmbus->SmbusController.Execute (&mSmbus->SmbusController, SmbusSlaveAddress, Register,
                          EfiSmbusWriteByte, FALSE, &SmbusLength,
                          Data);
    
    // Restore page number back to 0 as in PEI
    if (IsSpd) {
        if (Page != 0) SmmSmbusPageChange (0);
    }
    
    return Status;
}

/**
    Initialize SMBUS access routines

    @retval   EFI_SUCCESS

**/

EFI_STATUS
JedecNvdimmSmbusInit (
    VOID
    )
{
    EFI_STATUS                    Status = EFI_SUCCESS;
    UINT16                        SmbusIoBase;
    EFI_METRONOME_ARCH_PROTOCOL   *Metronome;
    
    DEBUG ((EFI_D_ERROR, "JedecNvdimmSmbusInit Begin\n"));  

    Status = gSmst->SmmAllocatePool (EfiRuntimeServicesData, (sizeof (SMBUS_INSTANCE)) * 2, &mSmbus);
    if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "SmmAllocatePool for NVDIMM Smbus is failed\n"));      
        return Status;
    }
    
    Status = gBS->LocateProtocol (&gEfiMetronomeArchProtocolGuid, NULL, &Metronome);
    if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "gEfiMetronomeArchProtocolGuid cannot be located\n"));      
        return Status;
    }
    
    // Read SMBus base address from HW
    LibAmdMemRead (AccessWidth16, (UINT64) (ACPI_MMIO_BASE + PMIO_BASE + FCH_ML_PMIOA_REG00), &SmbusIoBase, NULL);
    
    SmbusIoBase &= 0xFF00;

    // Init data - SMBus
    mSmbus->Signature                 = SMBUS_INSTANCE_SIGNATURE;
    mSmbus->SmbusIoBase               = SmbusIoBase;
    mSmbus->StallTime                 = Metronome->TickPeriod;
    mSmbus->IoDone                    = IoDone;
    mSmbus->SmbusIoRead               = SmbusIoRead;
    mSmbus->SmbusIoWrite              = SmbusIoWrite;
    mSmbus->SmbusController.Execute   = SmbusExecute;   
    
    SetDataAndRegister (mSmbus);
    InitializeListHead (&mSmbus->NotifyFunctionList);
    
    DEBUG ((EFI_D_ERROR, "JedecNvdimmSmbusInit End\n"));
    
    return Status;
}

