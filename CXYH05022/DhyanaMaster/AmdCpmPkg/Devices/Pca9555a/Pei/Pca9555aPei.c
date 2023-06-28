/*****************************************************************************
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

#include <Library/DebugLib.h>
#include <Ppi/I2cMaster.h>
#include <Ppi/Pca9555aPpi.h>
#include <Pca9555aPei.h>

STATIC EFI_PEI_PCA9555A_PPI mPca9555aPpi = {
  PCA9555A_PPI_REVISION,
  Pca9555aSet,
  Pca9555aGet
};

STATIC EFI_PEI_PPI_DESCRIPTOR mPca9555aPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPca9555aPpiGuid,
  &mPca9555aPpi
};


/*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the AMD CPM PCA9555A PEI driver
 *
 * This function registers the function to publish PCA9545A EEPROM access ppi.
 *
 * @param[in]     FileHandle     Pointer to the firmware file system header
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
EFI_STATUS
EFIAPI
Pca9555aPeiEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  // Publish Pca9555a control Ppi
  Status = (*PeiServices)->InstallPpi (
                           PeiServices,
                           &mPca9555aPpiList
                           );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

EFI_STATUS
EFIAPI
Pca9555aSet (
  IN       CONST EFI_PEI_SERVICES  **PeiServices,
  IN       UINTN             BusSelect,
  IN       UINTN             SlaveAddress,
  IN       UINT8             Offset,
  IN       UINT16            Data
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_PEI_I2C_MASTER_PPI  *I2cMaster;
  EFI_I2C_REQUEST_PACKET  RequestPacket;
  EFI_I2C_OPERATION       *Operation;
  UINT8                   Buffer[3];

  Status = (*PeiServices)->LocatePpi (
     PeiServices,
     &gEfiPeiI2cMasterPpiGuid,
     BusSelect,
     NULL,
     &I2cMaster
     );
  if(EFI_ERROR(Status)) return Status;

  //Prepare write data buffer
  Buffer[0] = Offset;
  Buffer[1] = (UINT8) (Data & 0x00FF); //Low byte
  Buffer[2] = (UINT8) (Data >> 8);     //High byte

  //Write Byte Operation
  Operation   = RequestPacket.Operation;
  RequestPacket.OperationCount = 1;
  Operation[0].Flags           = 0;
  Operation[0].LengthInBytes   = 3;
  Operation[0].Buffer          = &Buffer[0];

  Status = I2cMaster->StartRequest (I2cMaster, SlaveAddress, &RequestPacket);

  return Status;
}

EFI_STATUS
EFIAPI
Pca9555aGet (
  IN       CONST EFI_PEI_SERVICES  **PeiServices,
  IN       UINTN             BusSelect,
  IN       UINTN             SlaveAddress,
  IN       UINT8             Offset,
  OUT      UINT16            *Data
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_PEI_I2C_MASTER_PPI  *I2cMaster;
  UINT8                   PacketBuffer[sizeof (EFI_I2C_REQUEST_PACKET) + sizeof (EFI_I2C_OPERATION)];
  EFI_I2C_REQUEST_PACKET  *RequestPacket;
  EFI_I2C_OPERATION       *Operation;

  Status = (*PeiServices)->LocatePpi (
     PeiServices,
     &gEfiPeiI2cMasterPpiGuid,
     BusSelect,
     NULL,
     &I2cMaster
     );
  if(EFI_ERROR(Status)) return Status;

  RequestPacket = (EFI_I2C_REQUEST_PACKET*)PacketBuffer;
  Operation   = RequestPacket->Operation;
  RequestPacket->OperationCount = 2;
  Operation[0].Flags            = 0;
  Operation[0].LengthInBytes    = sizeof (UINT8);
  Operation[0].Buffer           = &Offset;
  Operation[1].Flags            = I2C_FLAG_READ;
  Operation[1].LengthInBytes    = sizeof (UINT16);
  Operation[1].Buffer           = (UINT8*)Data;

  Status = I2cMaster->StartRequest (I2cMaster, SlaveAddress, RequestPacket);

  return Status;
}