//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

#include <Uefi.h>
#include <PiPei.h>
#include "Pi/PiHob.h"
#include <AGESA.h>
#include "Library/AmdCalloutLib.h"
#include "Library/IoLib.h"
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include "token.h"
#include <Library/DebugLib.h>
#include "Ppi/ReadOnlyVariable2.h"
#include "Ppi/Smbus2.h"
#include "Ppi/AmdMemPpi.h"
#include <Ppi/AmiAgesaReadSpdPpi.h>
#include "Library/AmdCalloutLib/AgesaReadSpd.h"
#include "PlatformMemoryConfiguration.h"
#include "AmiAgesaPei.h"
#include "Ppi/AmdMemPpi.h"
#include <Library/AmdSocBaseLib.h>
//
// Identify all static PPI produced by this module
//
PEI_AMI_PLATFORM_DIMM_SPD_PPI  PeiAmiPlatformDimmSpdPpi = {
            //PPI data object;
            PlatformDimmSpdRead
            };


//
// Define the list of all PPI produced by this module
//
EFI_PEI_PPI_DESCRIPTOR PpiList[] = {
  { (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gAmiAgesaReadSpdPpiGuid,
    &PeiAmiPlatformDimmSpdPpi
    }
  };

//#if (PLATFORM_SELECT == 13) || (PLATFORM_SELECT == 21)
// Below is configuration for 52D16
AMI_DIMM_INFO_SMBUS DimmSmbus[] =
{
  //  DIMM_SPD_MAP
  {0xFF, 2, 0, 0xA0},
  {0xFF, 2, 1, 0xA2},
  {0xFF, 3, 0, 0xA4},
  {0xFF, 3, 1, 0xA6},
  {0xFF, 6, 0, 0xA0},
  {0xFF, 6, 1, 0xA2},
  {0xFF, 7, 0, 0xA4},
  {0xFF, 7, 1, 0xA6}, 
};
/*#elif (PLATFORM_SELECT == 27) 
AMI_DIMM_INFO_SMBUS DimmSmbus[] =
{
  //  DIMM_SPD_MAP
  {0, 0, 0, 0xA0},
  {0, 0, 1, 0xA2},
  {0, 1, 0, 0xA4},
  {0, 1, 1, 0xA6},
  {1, 0, 0, 0xA8},
  {1, 0, 1, 0xAA},
  {1, 1, 0, 0xAC},
  {1, 1, 1, 0xAE}, 
};
#else
// ---- PORTING REQUIRED ------
// Below is configuration for CRB 
AMI_DIMM_INFO_SMBUS DimmSmbus[] =
{
  //  DIMM_SPD_MAP
  {0xFF, 0, 0, 0xA0},
  {0xFF, 0, 1, 0xA2},
  {0xFF, 1, 0, 0xA4},
  {0xFF, 1, 1, 0xA6},
  {0xFF, 2, 0, 0xA8},
  {0xFF, 2, 1, 0xAA},
  {0xFF, 3, 0, 0xAC},
  {0xFF, 3, 1, 0xAE},
  {0xFF, 4, 0, 0xA0},
  {0xFF, 4, 1, 0xA2},
  {0xFF, 5, 0, 0xA4},
  {0xFF, 5, 1, 0xA6},
  {0xFF, 6, 0, 0xA8},
  {0xFF, 6, 1, 0xAA},
  {0xFF, 7, 0, 0xAC},
  {0xFF, 7, 1, 0xAE}, 
};
// ---- END OF PORTING -----
#endif*/


#define NUM_DIMM_INFO_ENTRIES   sizeof(DimmSmbus)/sizeof(AMI_DIMM_INFO_SMBUS)

VOID
SmbusI2CMuxSelection (
  IN  CONST EFI_PEI_SERVICES     **PeiServices,
  IN  UINT8   I2CMuxAddress,
  IN  UINT8   I2CCommand,
  IN  UINT8   I2CChannelAddress
)
{
    EFI_STATUS                          Status;
    EFI_PEI_SMBUS2_PPI                  *SmbusPpi = NULL;
    EFI_SMBUS_DEVICE_ADDRESS            SmbusSlaveAddress;
    UINTN                               SmbusLength = 0;
    UINTN                               SmbusOffset = 0;
    UINT8                               Buffer = 0;


    (**PeiServices).LocatePpi(PeiServices, &gEfiPeiSmbus2PpiGuid, 0, NULL, &SmbusPpi);
    SmbusLength = 1;
    SmbusOffset = 0;
    SmbusSlaveAddress.SmbusDeviceAddress = (I2CMuxAddress >> 1);
    Status = SmbusPpi->Execute(SmbusPpi, SmbusSlaveAddress, 
                                I2CCommand,
                                EfiSmbusWriteByte, 
                                FALSE, 
                                &SmbusLength,
                                &I2CChannelAddress);
}   

/**
    This function changes the SMBUS page number.
    A dummy write to the slave address of 6E is made to switch the page address to 1, prior to the SPD reading commands.
    Upon completion, another dummy write to the slave address of 6C is made to restore the original page address

    @param PeiServices PEI Services table pointer
    @param Page        Page number for switch
         
    @retval EFI_STATUS return EFI status

**/

VOID
SmbusPageChange (
  IN  CONST EFI_PEI_SERVICES     **PeiServices,
  IN  UINT8                      Page
  )
{
    EFI_STATUS                          Status;
    EFI_PEI_SMBUS2_PPI                  *SmbusPpi = NULL;
    EFI_SMBUS_DEVICE_ADDRESS            SmbusSlaveAddress;
    UINTN                               SmbusLength = 0;
    UINTN                               SmbusOffset = 0;
    UINT8                               Buffer = 0;


    (**PeiServices).LocatePpi(PeiServices, &gEfiPeiSmbus2PpiGuid, 0, NULL, &SmbusPpi);
    //A dummy write to the slave address of 6E is made to switch the page address to 1, prior to the SPD reading commands.
    //Upon completion, another dummy write to the slave address of 6C is made to restore the original page address

    SmbusLength = 1;
    SmbusOffset = 0;
    if (Page == 1)
    {
        SmbusSlaveAddress.SmbusDeviceAddress = (0x6E >> 1);
        Status = SmbusPpi->Execute(SmbusPpi, SmbusSlaveAddress, SmbusOffset,
                                EfiSmbusWriteByte, FALSE, &SmbusLength,
                                &Buffer );
        //DEBUG((EFI_D_ERROR, "\n\n Change SMBUS to page 1 \n"));
    }
    else
    {
        SmbusSlaveAddress.SmbusDeviceAddress = (0x6C >> 1);
        Status = SmbusPpi->Execute(SmbusPpi, SmbusSlaveAddress, SmbusOffset,
                                EfiSmbusWriteByte, FALSE, &SmbusLength,
                                &Buffer );
        //DEBUG((EFI_D_ERROR, "\n\n Change SMBUS to page 0 \n"));
    }
    
}


/**
    This function reads whole SPD to provided buffer.

    @param PeiServices PEI Services table pointer
    @param SaveAddr SMBus address of SPD
    @param SpdData Pointer to read SPD parameters structure
         
    @retval EFI_STATUS return EFI status

**/

EFI_STATUS
MemInitReadSpd (
  IN  EFI_PEI_SERVICES   **PeiServices,
  IN  UINT8              SlaveAddr,
  IN OUT  AGESA_READ_SPD_PARAMS *SpdData
  )
{
    EFI_STATUS                          Status;
    EFI_SMBUS_DEVICE_ADDRESS            SmbusSlaveAddress;
    UINTN                               SmbusLength = 0;
    UINTN                               SmbusOffset = 0;
    EFI_PEI_SMBUS2_PPI                  *SmbusPpi = NULL;
    UINTN                               Index = 0;
    UINT8                               *Buffer2 = NULL;
    UINT8                               DimmSelect = 0;             // Dimm Number selection
    UINTN                               VariableSize = 0;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI     *ReadOnlyVariable = NULL;      // ptr for read Variavle PPI
    UINT8                               DramType = 0;   // B is DDR3, C is DDR4

    // Locate Smbus Device
    Status = (**PeiServices).LocatePpi( PeiServices, &gEfiPeiSmbus2PpiGuid, 0, NULL, &SmbusPpi );
    if (EFI_ERROR(Status))  return Status;

    SmbusSlaveAddress.SmbusDeviceAddress = (SlaveAddr >> 1);
    SmbusLength = 1;
    SmbusOffset = 2;    // Key Byte / DRAM Device Type    
    Status = SmbusPpi->Execute( SmbusPpi, SmbusSlaveAddress, SmbusOffset,
                          EfiSmbusReadByte, FALSE, &SmbusLength,
                          &DramType );
    //if SPD data not available at location 2, then we return status
    if (EFI_DEVICE_ERROR == Status)
        return Status;


        Status = EFI_SUCCESS;
        SmbusLength = 1;
        SmbusOffset = 0;
        Buffer2 = SpdData->Buffer;

        //It's DDR4
        //The 256 bytes in Page 0
        for ( Index = 0; Index < DIMM_SPD_DATA_256; Index++ )
        {
            Status = SmbusPpi->Execute( SmbusPpi, SmbusSlaveAddress, SmbusOffset,
                                  EfiSmbusReadByte, FALSE, &SmbusLength,
                                  Buffer2 );
            SmbusOffset += 1;
            Buffer2++;
        }

        //Change to page 1
        SmbusPageChange(PeiServices, 1);
        //The 256 bytes in Page 1
        for ( Index = 0; Index < DIMM_SPD_DATA_256; Index++ )
        {
            Status = SmbusPpi->Execute( SmbusPpi, SmbusSlaveAddress, SmbusOffset,
                                  EfiSmbusReadByte, FALSE, &SmbusLength,
                                  Buffer2 );
            SmbusOffset += 1;
            Buffer2++;
        }

        //Change to Page 0
        SmbusPageChange(PeiServices, 0);


    return Status;
}

/**
    This function reads provides SPD data if DIMM present.

           
    @param PeiServices PEI Services table pointer
    @param This Pointer to 
    @param SpdData Pointer to read SPD parameters
        structure

         
    @retval EFI_STATUS return EFI status

**/

EFI_STATUS
EFIAPI
PlatformDimmSpdRead (
  IN      EFI_PEI_SERVICES      **PeiServices,
  IN      PEI_AMI_PLATFORM_DIMM_SPD_PPI      *This,
  IN OUT  AGESA_READ_SPD_PARAMS *SpdData
  )
{
    UINTN       Index;
    UINT8       SmbusAddress;
    EFI_STATUS  Status;

    IoWrite8(0x80, 0x2B);       // PEI_PROGRESS_CODE(PeiServices, PEI_MEMORY_SPD_READ);
    SmbusAddress = 0;
    Status = EFI_INVALID_PARAMETER;

    for (Index = 0; Index < NUM_DIMM_INFO_ENTRIES; Index++)
    {
        if ((DimmSmbus[Index].SocketId == 0xFF) || (SpdData->SocketId == DimmSmbus[Index].SocketId))
        {
          if ((SpdData->MemChannelId == DimmSmbus[Index].ChannelId)
            && (SpdData->DimmId == DimmSmbus[Index].DimmId))
          {
              SmbusAddress = DimmSmbus[Index].SmbusAddress;
              DEBUG ((EFI_D_ERROR,"SMBUS address found = %x\n",SmbusAddress));
              Status = EFI_SUCCESS;
              break;
          }
        }
    }

    if (SpdData->SocketId > (NSOCKETS - 1)) return Status;

    if (Status == EFI_INVALID_PARAMETER)
    {
        return Status;
    }  

    //
    // Switch the channel based off following information before initiating SPD read.
    //
#if (PLATFORM_SELECT == 4) || (PLATFORM_SELECT == 5) || (PLATFORM_SELECT == 13)   || \
    (PLATFORM_SELECT == 16) || (PLATFORM_SELECT == 17) || (PLATFORM_SELECT == 19) || \
    (PLATFORM_SELECT == 21) || (PLATFORM_SELECT == 23) || (PLATFORM_SELECT == 25) || \
    (PLATFORM_SELECT == 28) || (PLATFORM_SELECT == 29) || (PLATFORM_SELECT == 18) || \
    (PLATFORM_SELECT == 30) 
    if (SpdData->SocketId == 0) {
      if ((SpdData->MemChannelId >= 0) && (SpdData->MemChannelId < 4)) {
        SmbusI2CMuxSelection (PeiServices, 0xE0, 0xff, 0x01);
      }
      if ((SpdData->MemChannelId >= 4) && (SpdData->MemChannelId < 8)) {
        SmbusI2CMuxSelection (PeiServices, 0xE0, 0xff, 0x02);
      }
    }
    
   if (SpdData->SocketId == 1) {
      if ((SpdData->MemChannelId >= 0) && (SpdData->MemChannelId < 4)) {
        SmbusI2CMuxSelection (PeiServices, 0xE0, 0xff, 0x04);
      }
      if ((SpdData->MemChannelId >= 4) && (SpdData->MemChannelId < 8)) {
        SmbusI2CMuxSelection (PeiServices, 0xE0, 0xff, 0x08);
      }
  }
#else
    if (SpdData->SocketId == 0) {
      if ((SpdData->MemChannelId >= 0) && (SpdData->MemChannelId < 4)) {
        SmbusI2CMuxSelection (PeiServices, 0x94, 0x03, 0x80);
      }
      if ((SpdData->MemChannelId >= 4) && (SpdData->MemChannelId < 8)) {
        SmbusI2CMuxSelection (PeiServices, 0x94, 0x03, 0x40);
      }
    }
    
   if (SpdData->SocketId == 1) {
      if ((SpdData->MemChannelId >= 0) && (SpdData->MemChannelId < 4)) {
        SmbusI2CMuxSelection (PeiServices, 0x94, 0x03, 0x20);
      }
      if ((SpdData->MemChannelId >= 4) && (SpdData->MemChannelId < 8)) {
        SmbusI2CMuxSelection (PeiServices, 0x94, 0x03, 0x10);
      }
    }   
#endif
    Status = MemInitReadSpd(PeiServices, SmbusAddress, SpdData);

    DEBUG((EFI_D_ERROR, "\n --Sckt-Ch-Dimm=[%x-%x-%x]--Dimm Addr %X  ---SPD Read Sts %r \n", \
                SpdData->SocketId,SpdData->MemChannelId,SpdData->DimmId,SmbusAddress, Status));
    return Status;
}

/**
    This function reads the serial number of SPD

    @param PeiServices PEI Services table pointer
    @param UINT8       SMBUS slave address to read
    @param Buffer      the data buffer to be sent out
    @param BufferIndex the buffer index

    @retval EFI_STATUS Status of result

**/
EFI_STATUS ReadSaveDimmSerial(
 IN   CONST EFI_PEI_SERVICES    **PeiServices,
 UINT8                          SlaveAddr,
 UINT8                          *Buffer,
 UINT8                          BufferIndex)
{

    UINT8                       Buffer2, Index;
    EFI_STATUS                  Status;
    EFI_SMBUS_DEVICE_ADDRESS    SmbusSlaveAddress;
    EFI_PEI_SMBUS2_PPI          *SmbusPpi = NULL;
    UINTN                       SmbusLength = 0;
    UINTN                       SmbusOffset = 0;
    UINT8                       DramType = 0;

    //Check if device Present by reading offset 2
    //if not present return
    //if device present, read the device serial
    //Update Buffer 
    //Returns error if cannot locate smbus ppi
    SmbusLength = 1;
    SmbusOffset = 2;    // Key Byte / DRAM Device Type

    DEBUG((EFI_D_ERROR, "\n ReadSaveDimmSerial entry \n"));

    Status = (**PeiServices).LocatePpi( PeiServices,
                                        &gEfiPeiSmbus2PpiGuid,
                                        0, NULL, &SmbusPpi );

    if (EFI_ERROR(Status))  return  EFI_NOT_FOUND;

    DEBUG((EFI_D_ERROR, "\n SlaveAddr = %x \n", SlaveAddr));
    DEBUG((EFI_D_ERROR, "\n BufferIndex = %x \n", BufferIndex));

    SmbusSlaveAddress.SmbusDeviceAddress = (SlaveAddr >> 1);

    Status = SmbusPpi->Execute( SmbusPpi, SmbusSlaveAddress, SmbusOffset,
                          EfiSmbusReadByte, FALSE, &SmbusLength,
                          &DramType );
    //if SPD data is not available at offset 2, then we return status
    if (EFI_DEVICE_ERROR == Status)
        return Status;

    Status = EFI_SUCCESS;
    SmbusSlaveAddress.SmbusDeviceAddress = (SlaveAddr >> 1);
    SmbusLength = 1;

    //It's DDR4
    DEBUG((EFI_D_ERROR, "\n DDR4 Serial number = \n"));

    //Change to page 1
    SmbusPageChange(PeiServices, 1);

    SmbusOffset = DIMM_SPD_SERIAL_NO_OFFSET_DDR4;
    for ( Index = 0; Index < DIMM_SPD_SERIAL_NO_LENGTH; Index++ )
    {
        Status = SmbusPpi->Execute( SmbusPpi, SmbusSlaveAddress, SmbusOffset,
                              EfiSmbusReadByte, FALSE, &SmbusLength,
                              &Buffer2 );
        DEBUG((EFI_D_ERROR, " %x ", Buffer2));
        SmbusOffset += 1;
        Buffer[BufferIndex * DIMM_SPD_SERIAL_NO_LENGTH + Index] = Buffer2;
    }
        
    //Change to Page 0
    SmbusPageChange(PeiServices, 0);



    return EFI_SUCCESS;
}


/**
 *  AmiAgesaPeiEntry
 *
 *  Description:
 *    This function will prepare the platform configuarion for AGESA use.
 *
 *  Parameters:
 *    @param[in]        **PeiServices
 *                      FileHandle
 *
 *
 *    @retval         VOID
 *
 **/
EFI_STATUS
EFIAPI
AmiAgesaPeiEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
    EFI_STATUS                                  Status = EFI_SUCCESS;
    UINTN                                       VariableSize = sizeof(UINT64);

    DEBUG ((EFI_D_ERROR, "    AmiAgesaPeiEntry Entry\n"));

    Status = (*PeiServices)->InstallPpi(PeiServices, &PpiList[0]);



    DEBUG ((EFI_D_ERROR, "    AmiAgesaPeiEntry Exit\n"));

    return  Status;
}






//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
