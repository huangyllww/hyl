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

/** @file AmiAgesaReadSpdPpi.h
    PPI definitions

**/

#ifndef _AMI_AGESA_READSPD_PPI_H_
#define _AMI_AGESA_READSPD_PPI_H_
#include "Library/AmdCalloutLib.h"
//----------------------------------------------------------------------

//----------------------------------------------------------------------

//----------------------------------------------------------------------

//----------------------------------------------------
//
// PEI_AMD_PLATFORM_DIMM_SPD_PPI data structure definition.
//
//-------------------------------------------------------
typedef struct _PEI_AMI_PLATFORM_DIMM_SPD_PPI PEI_AMI_PLATFORM_DIMM_SPD_PPI;

//----------------------------------------------------
//
// Defines function prototype for PlatformDimmSpdRead
//
//-------------------------------------------------------
typedef
EFI_STATUS
(EFIAPI *PEI_AMI_DIMM_SPD_READ) (
  IN      EFI_PEI_SERVICES                    **PeiServices,  ///< Pointer to PeiServices
  IN struct _PEI_AMI_PLATFORM_DIMM_SPD_PPI    *This,          ///< Pointer to the PPI structure
  IN OUT  AGESA_READ_SPD_PARAMS               *SpdData        ///< SPD Data Buffer
  );

//----------------------------------------------------
//
// PEI_AMD_PLATFORM_DIMM_SPD_PPI data structure definition.
//
//-------------------------------------------------------
typedef struct _PEI_AMI_PLATFORM_DIMM_SPD_PPI {          // See the Forward Declaration above
  PEI_AMI_DIMM_SPD_READ                       PlatformDimmSpdRead;    ///< Function to be called
};

extern EFI_GUID gAmiAgesaReadSpdPpiGuid;

//----------------------------------------------------------------------

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
