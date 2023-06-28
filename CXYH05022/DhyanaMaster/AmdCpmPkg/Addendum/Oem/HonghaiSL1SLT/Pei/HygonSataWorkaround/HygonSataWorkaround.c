#include <AmdCpmPei.h>
#include <Library/DebugLib.h>
#include <Ppi/AmdBoardIdPpi.h>
#include <Ppi/Pca9545aPpi.h>
#include <Ppi/Pca9535aPpi.h>
#include <Ppi/Ds125Br401aPpi.h>
#include "HygonSataWorkaround.h"

SATA_WA_PLATFORM SataWaPlatformList[] = {
    {{DIESEL      , 0, REVISION_A, 0}, 0x0F00},
    {{DIESEL      , 0, REVISION_B, 0}, 0x0F00},
    {{DIESEL_DEBUG, 0, REVISION_A, 0}, 0x0F00},
    {{DIESEL_SLT  , 0, REVISION_A, 0}, 0x0F00},
    {{DIESEL_SLT  , 0, REVISION_B, 0}, 0x0F00},
    {{DIESEL_DAP  , 0, REVISION_A, 0}, 0x0F00},
    {{DIESEL_DAP  , 0, REVISION_B, 0}, 0x0F00},
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
    DEBUG ((EFI_D_ERROR, "Platform not found !!!\n"));
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
  EFI_PEI_PCA9545A_PPI   *Pca9545aPpi;
  EFI_PEI_DS125BR401A_PPI *Ds125Br401aPpi;
  UINT8    i;
  UINT8    CardSelect=1;
  DEBUG ((EFI_D_ERROR, "SATA workaround Required !!!\n"));

  Status = (*PeiServices)->LocatePpi (
     PeiServices,
     &gPca9545aPpiGuid,
     0,
     NULL,
     &Pca9545aPpi
     );

  Status = (*PeiServices)->LocatePpi (
     PeiServices,
     &gDs125Br401aPpiGuid,
     0,
     NULL,
     &Ds125Br401aPpi
     );

  for(i=0;i<4;i++){
     
    Pca9545aPpi->Set(PeiServices, 0, 0xE0, CardSelect);
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
      
    //  Ds125Br401aPpi->Get(PeiServices, 0, 0xB0 >> 1, 0x2E, &data);
     // DEBUG((EFI_D_ERROR, "SATA Ch A DEM data   : %04x\n", data));
      
      Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x0D, 0x02);
      Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x14, 0x02);
      Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x1B, 0x02);
      Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x22, 0x02);
      
      //Channel B 0-3 EQ
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
       Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x11, 1);
       Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x18, 0);
       Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x1F, 0);
       Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x26, 1);

       //Channel A 0-3 DEM
       Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x2E, 1);
       Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x35, 1);
       Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x3C, 1);
       Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x43, 1);

       Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x2A, 0x02);
       Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x31, 0x02);
       Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x38, 0x02);
       Ds125Br401aPpi->Set(PeiServices, 0, 0xB0 >> 1, 0x3F, 0x02);
       
       Ds125Br401aPpi->Set(PeiServices, 0, 0xB2 >> 1, 0x06, 0x18);
       //Channel B 0-3 EQ
         Ds125Br401aPpi->Set(PeiServices, 0, 0xB2 >> 1, 0x0F, 0);
         Ds125Br401aPpi->Set(PeiServices, 0, 0xB2 >> 1, 0x16, 0);
         Ds125Br401aPpi->Set(PeiServices, 0, 0xB2 >> 1, 0x1D, 0);
         Ds125Br401aPpi->Set(PeiServices, 0, 0xB2 >> 1, 0x24, 0);


         
         //Channel A 0-3 EQ
         Ds125Br401aPpi->Set(PeiServices, 0, 0xB2 >> 1, 0x2C, 0);
         Ds125Br401aPpi->Set(PeiServices, 0, 0xB2 >> 1, 0x33, 0);
         Ds125Br401aPpi->Set(PeiServices, 0, 0xB2 >> 1, 0x3A, 0);
         Ds125Br401aPpi->Set(PeiServices, 0, 0xB2 >> 1, 0x41, 0);

         
         //Channel B 0-3 VOD
         Ds125Br401aPpi->Set(PeiServices, 0, 0xB2 >> 1, 0x10, 0x0);
         Ds125Br401aPpi->Set(PeiServices, 0, 0xB2 >> 1, 0x17, 0x0);
         Ds125Br401aPpi->Set(PeiServices, 0, 0xB2 >> 1, 0x1E, 0x0);
         Ds125Br401aPpi->Set(PeiServices, 0, 0xB2 >> 1, 0x25, 0x0);

         //Channel A 0-3 VOD
         Ds125Br401aPpi->Set(PeiServices, 0, 0xB2 >> 1, 0x2D, 0x0);
         Ds125Br401aPpi->Set(PeiServices, 0, 0xB2 >> 1, 0x34, 0x0);
         Ds125Br401aPpi->Set(PeiServices, 0, 0xB2 >> 1, 0x3B, 0x0);
         Ds125Br401aPpi->Set(PeiServices, 0, 0xB2 >> 1, 0x42, 0x0);

         //Channel B 0-3 DEM
         Ds125Br401aPpi->Set(PeiServices, 0, 0xB2 >> 1, 0x11, 1);
         Ds125Br401aPpi->Set(PeiServices, 0, 0xB2 >> 1, 0x18, 0);
         Ds125Br401aPpi->Set(PeiServices, 0, 0xB2 >> 1, 0x1F, 0);
         Ds125Br401aPpi->Set(PeiServices, 0, 0xB2 >> 1, 0x26, 1);

         //Channel A 0-3 DEM
         Ds125Br401aPpi->Set(PeiServices, 0, 0xB2 >> 1, 0x2E, 1);
         Ds125Br401aPpi->Set(PeiServices, 0, 0xB2 >> 1, 0x35, 1);
         Ds125Br401aPpi->Set(PeiServices, 0, 0xB2 >> 1, 0x3C, 1);
         Ds125Br401aPpi->Set(PeiServices, 0, 0xB2 >> 1, 0x43, 1);

         Ds125Br401aPpi->Set(PeiServices, 0, 0xB2 >> 1, 0x2A, 0x02);
         Ds125Br401aPpi->Set(PeiServices, 0, 0xB2 >> 1, 0x31, 0x02);
         Ds125Br401aPpi->Set(PeiServices, 0, 0xB2 >> 1, 0x38, 0x02);
         Ds125Br401aPpi->Set(PeiServices, 0, 0xB2 >> 1, 0x3F, 0x02);
    CardSelect<<=1;
  }
  return Status;
}
