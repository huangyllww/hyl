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
#include "FieldCardPcieWorkaround.h"
#include <Library/TimerLib.h>

#if 0
SATA_WA_PLATFORM SataWaPlatformList[] = {
    {{HonghaiSL1SLT      , 0, REVISION_A, 0}, 0x0F00},
    {{HonghaiSL1SLT      , 0, REVISION_B, 0}, 0x0F00},
    {{HonghaiSL1SLT_DEBUG, 0, REVISION_A, 0}, 0x0F00},
    {{HonghaiSL1SLT_SLT  , 0, REVISION_A, 0}, 0x0F00},
    {{HonghaiSL1SLT_SLT  , 0, REVISION_B, 0}, 0x0F00},
    {{HonghaiSL1SLT_DAP  , 0, REVISION_A, 0}, 0x0F00},
    {{HonghaiSL1SLT_DAP  , 0, REVISION_B, 0}, 0x0F00},
    {{OCTANE      , 0, REVISION_A, 0}, 0x0F00},
    {{OCTANE_DAP  , 0, REVISION_A, 0}, 0x0F00}
};

#define PLATFORM_LIST_COUNT  (sizeof (SataWaPlatformList) / sizeof (SATA_WA_PLATFORM))

EFI_STATUS
PlatformSearch (
    IN       AMD_PLATFORM_ID     *AmdPlatformId,
    OUT      UINT16              *BitMap
  )
{
  UINT32    Index;
  AMD_PLATFORM_ID *SataWaPlatformId;
  DEBUG((EFI_D_ERROR, "AmdPlatformId = 0x%08x\n", AmdPlatformId));
  for (Index = 0; Index < PLATFORM_LIST_COUNT; Index++) {
    SataWaPlatformId = &SataWaPlatformList[Index].PlatformID;
    if (AmdPlatformId->BoardId == SataWaPlatformId->BoardId) {
        if (AmdPlatformId->RevisionId == SataWaPlatformId->RevisionId) {
            *BitMap = SataWaPlatformList[Index].SataInstanceBitMap;
            break;
        }
    }
  }
  if (Index == PLATFORM_LIST_COUNT) {
      DEBUG((EFI_D_ERROR, "Platform not found !!!\n"));
    return RETURN_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
SataWrokaround (
    IN      UINT16    BitMap
  )
{
  EFI_STATUS Status = EFI_SUCCESS;


  return Status;
}
#endif
/*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the FieldCard Pcie Slot Workaround Init PEIM driver
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
FieldCardPcieWorkaroundPeiInit (
    IN       EFI_PEI_FILE_HANDLE  FileHandle,
    IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS              Status = EFI_SUCCESS;
  EFI_PEI_PCA9545A_PPI    *Pca9545aPpi;
  EFI_PEI_DS125BR401A_PPI *Ds125Br401aPpi;
  UINT8                   i = 0, j = 0;
  
  DEBUG((EFI_D_ERROR, "Pcie workaround Required !!!!\n"));

  Status = (*PeiServices)->LocatePpi (
     PeiServices,
     &gPca9545aPpiGuid,
     0,
     NULL,
     &Pca9545aPpi
     );
  
  if (Status != EFI_SUCCESS) {
      DEBUG((EFI_D_ERROR, "Can not find Pca9545aPpi !!!!\n"));
      return Status; 
  }

  Status = (*PeiServices)->LocatePpi (
     PeiServices,
     &gDs125Br401aPpiGuid,
     0,
     NULL,
     &Ds125Br401aPpi
     );
  
  if (Status != EFI_SUCCESS) {
      DEBUG((EFI_D_ERROR, "Can not find Ds125Br401aPpi !!!!\n"));
      return Status; 
  }
  //enable first field card I2C MUX channel 1
//  ControlByte = 0x01; //enable channel 1 

  Pca9545aPpi->Set(PeiServices, 0, 0x70, 0x01);
 // MicroSecondDelay(5000000); //Delay 5s

  for (i=0; i<=1; i++) {
      Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, 0x06, 0x18); //<Ds125Br401a>Enable SMBus Register Control
      //Channel B 0-3 EQ
      Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, 0x04, 0);       //original code, do not know why
      Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, 0x08, 0x04);    // Override PIN control bit[2]= Override MODE  
      Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, 0x0E, 0x08);    //CH0-CHB0 IDLE,RXDET bit[3:2]=RXDET 10:Auto RX-Detect
      
      for (j=0; j<4; j++) {
          //Set Channel B 0-3 EQ/VOD/DEM
          Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, (0x0F + 7*j + 0), 0x02);     //CH[j]-B[j]-EQ
          Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, (0x0F + 7*j + 1), 0xAA);     //CH[j]-B[j]-VOD  
          Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, (0x0F + 7*j + 2), 0x02);     //CH[j]-B[j]-DEM. -3.5dB
          
          //Set Channel A 4-7 EQ/VOD/DEM
          Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, (0x2C + 7*j + 0), 0x02);     //CH[j]-A[j]-EQ
          Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, (0x2C + 7*j + 1), 0xAA);     //CH[j]-A[j]-VOD 
          Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, (0x2C + 7*j + 2), 0x02);     //CH[j]-A[j]-DEM. -3.5dB          
      } //end for (j=0; j<4; j++)
      
  } //end for (i=0; i<=1; i++)

  for (i=0; i<=1; i++) {      
      //Enalbe filed Card1's Redriver 0-7 signal detect function
      Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, 0x0D, 0x02); //CH0-CHB0 Signal Detect bit[1]= SD Preset  1:Force Signal detect "on" 0:Force Signal detect "off"
      Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, 0x14, 0x02);
      Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, 0x1B, 0x02);
      Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, 0x22, 0x02);
      Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, 0x2A, 0x02);
      Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, 0x31, 0x02);
      Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, 0x38, 0x02);
      Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, 0x3F, 0x02);
  }
  Pca9545aPpi->Set(PeiServices, 0, 0x70, 0x00);
  //MicroSecondDelay(5000000); //Delay 5s
  
  Pca9545aPpi->Set(PeiServices, 0, 0x71, 0x01);
 // MicroSecondDelay(5000000); //Delay 5s

  for (i=0; i<=1; i++) {
      Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, 0x06, 0x18); //<Ds125Br401a>Enable SMBus Register Control
      //Channel B 0-3 EQ
      Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, 0x04, 0);       //original code, do not know why
      Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, 0x08, 0x04);    // Override PIN control bit[2]= Override MODE  
      Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, 0x0E, 0x08);    //CH0-CHB0 IDLE,RXDET bit[3:2]=RXDET 10:Auto RX-Detect
      
      for (j=0; j<4; j++) {
          //Set Channel B 0-3 EQ/VOD/DEM
          Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, (0x0F + 7*j + 0), 0x02);     //CH[j]-B[j]-EQ
          Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, (0x0F + 7*j + 1), 0xAA);     //CH[j]-B[j]-VOD  
          Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, (0x0F + 7*j + 2), 0x02);     //CH[j]-B[j]-DEM. -3.5dB
          
          //Set Channel A 4-7 EQ/VOD/DEM
          Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, (0x2C + 7*j + 0), 0x02);     //CH[j]-A[j]-EQ
          Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, (0x2C + 7*j + 1), 0xAA);     //CH[j]-A[j]-VOD 
          Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, (0x2C + 7*j + 2), 0x02);     //CH[j]-A[j]-DEM. -3.5dB          
      } //end for (j=0; j<4; j++)
      
  } //end for (i=0; i<=1; i++)

  for (i=0; i<=1; i++) {      
      //Enalbe filed Card1's Redriver 0-7 signal detect function
      Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, 0x0D, 0x02); //CH0-CHB0 Signal Detect bit[1]= SD Preset  1:Force Signal detect "on" 0:Force Signal detect "off"
      Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, 0x14, 0x02);
      Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, 0x1B, 0x02);
      Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, 0x22, 0x02);
      Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, 0x2A, 0x02);
      Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, 0x31, 0x02);
      Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, 0x38, 0x02);
      Ds125Br401aPpi->Set(PeiServices, 0, (0xB2+2*i) >> 1, 0x3F, 0x02);
  }
  Pca9545aPpi->Set(PeiServices, 0, 0x71, 0x00);
  
  DEBUG((EFI_D_ERROR, "Get outof Setting FieldCard!!!\n"));
  
  return EFI_SUCCESS;
}
