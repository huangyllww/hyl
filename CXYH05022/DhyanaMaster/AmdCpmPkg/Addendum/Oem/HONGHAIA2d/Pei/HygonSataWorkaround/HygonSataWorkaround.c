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

#include <AmdCpmPei.h>
#include <Library/DebugLib.h>
#include <Ppi/AmdBoardIdPpi.h>
#include <Ppi/Pca9545aPpi.h>
#include <Ppi/Pca9535aPpi.h>
#include <Ppi/Ds125Br401aPpi.h>
#include "HygonSataWorkaround.h"


EFI_STATUS
CheckPlatform (
    IN       AMD_CPM_TABLE_PPI    *CpmTablePpiPtr
  )
{
  EFI_STATUS Status = EFI_UNSUPPORTED;
  UINT8               Gpio92, Gpio115, Gpio116;
  UINT8               GpioMatrix;
  UINT8               socket, die;

  socket = 1;
  die = 0;
  Gpio92 = CpmTablePpiPtr->CommonFunction.GetGpio (CpmTablePpiPtr,  (socket << 14) + (die << 10) + 92);
  Gpio115 = CpmTablePpiPtr->CommonFunction.GetGpio (CpmTablePpiPtr,  (socket << 14) + (die << 10) + 115);
  Gpio116 = CpmTablePpiPtr->CommonFunction.GetGpio (CpmTablePpiPtr,  (socket << 14) + (die << 10) + 116);

  GpioMatrix = (Gpio92 << 2) + (Gpio115 << 1) + Gpio116;
  DEBUG((EFI_D_ERROR, "GpioMatrix: %x\n", GpioMatrix));
  if (GpioMatrix == 0){
    Status = EFI_SUCCESS;
  }

  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the AMD Sata Workaround Init PEIM driver
 *
 * This function defines CPM OEM definition tables and installs AmdCpmOemTablePpi.
 * It also defines callback function to update these definition table on run time.
 *
 * @param[in]     FileHandle     Pointer to the firmware file system header
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
EFI_STATUS
EFIAPI
AmdSataWorkaroundPeiInit (
    IN       EFI_PEI_FILE_HANDLE  FileHandle,
    IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS              Status = EFI_SUCCESS;
  EFI_PEI_DS125BR401A_PPI *Ds125Br401aPpi;
	AMD_CPM_TABLE_PPI        *CpmTablePpiPtr;
  UINT8                    Id;

	Status = (*PeiServices)->LocatePpi(
		(CPM_PEI_SERVICES**)PeiServices,
		&gAmdCpmTablePpiGuid,
		0,
		NULL,
		(VOID**)&CpmTablePpiPtr
		); 
  if (Status != EFI_SUCCESS) {
      DEBUG((EFI_D_ERROR, "Error: AmdCpmTalbePpi not found. \n"));
      return EFI_SUCCESS;
  }

  Status = CheckPlatform(CpmTablePpiPtr);
  if (Status != EFI_SUCCESS) {
      DEBUG((EFI_D_INFO, "No need to enable sata workaround \n"));
      return EFI_SUCCESS;
  }

  Status = (*PeiServices)->LocatePpi (
     PeiServices,
     &gDs125Br401aPpiGuid,
     0,
     NULL,
     &Ds125Br401aPpi
     );
  if (Status != EFI_SUCCESS) {
      DEBUG((EFI_D_ERROR, "Error: DS125BR401A Pei service not found.\n"));
      return EFI_SUCCESS;
  }

  //Read redriver ID:
  Ds125Br401aPpi->Get(PeiServices, 0, 0xB0 >> 1, 0x51, &Id);
  DEBUG((EFI_D_ERROR, "DS125BR401A ID: %x\n", Id));

  //Config SATA redriver: 

    Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x06, 0x18);
    
    //Channel B 0-3 EQ
    Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x04, 0);
    Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x0F, 0);  
    Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x16, 0);
    Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x1D, 0);
    Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x24, 0);
    
    //Channel A 0-3 EQ
    Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x2C, 0);
    Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x33, 0);
    Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x3A, 0);
    Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x41, 0);

    //Channel B 0-3 VOD
    Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x10, 0x0);
    Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x17, 0x0);
    Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x1E, 0x0);
    Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x25, 0x0);
    
    //Channel A 0-3 VOD
    Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x2D, 0x0);
    Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x34, 0x0);
    Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x3B, 0x0);
    Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x42, 0x0);

    //Channel B 0-3 DEM
    Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x11, 0);
    Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x18, 0);
    Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x1F, 0);
    Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x26, 0);

    //Channel A 0-3 DEM
    Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x2E, 0);
    Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x35, 0);
    Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x3C, 0);
    Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x43, 0);
    
    //Channel B 0-3 signal detect on
    Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x0D, 0x02);
    Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x14, 0x02);
    Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x1B, 0x02);
    Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x22, 0x02);
    
    //Channel A 0-3 signal detect on
    Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x2A, 0x02);
    Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x31, 0x02);
    Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x38, 0x02);
    Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x3F, 0x02);
 

  return Status;
}
