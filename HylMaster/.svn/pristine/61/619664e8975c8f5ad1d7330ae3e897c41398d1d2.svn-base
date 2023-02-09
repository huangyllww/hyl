//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file
  @brief Ami Debug Print Error Level Service Ppi header file

  The file contains Ami Debug Print Error Level Service Ppi definitions.
**/

#ifndef __OEM_DEBUG_LEVEL_SERVICE_PPI_H__
#define __OEM_DEBUG_LEVEL_SERVICE_PPI_H__

/// AMI Debug Print Error Level Service PPI GUID definition
#define OEM_DEBUG_LEVEL_SERVICE_PPI_GUID \
      {0x2bc7455f, 0x8b91, 0x4dae, {0xa5, 0x54, 0x02, 0xb9, 0x74, 0xf1, 0xd3, 0x8d}}

typedef struct _OEM_DEBUG_LEVEL_LEVEL_SERVICE_PPI OEM_DEBUG_LEVEL_LEVEL_SERVICE_PPI;

/**
  Get configured Debug Print Error Level for a module

  @param  This          Pointer to Ami Debug Print Error Level Service Ppi
  @param  ModuleGuid    Pointer to Module File Guid
  @param  ErrorLevel    Pointer to return current Debug Print Error Level mask for the current module

  @retval EFI_STATUS    Returns Success if module is found in Database, else Not Found
**/
typedef
EFI_STATUS
(EFIAPI *OEM_GET_DEBUG_PRINT_ERROR_LEVEL)(
  IN      OEM_DEBUG_LEVEL_LEVEL_SERVICE_PPI             *This,
  IN      EFI_GUID                                      *ModuleGuid,
  OUT     UINT32                                        *ErrorLevel
  );

/**
  Set/Configure Debug Print Error Level for a module

  @param  This          Pointer to Ami Debug Print Error Level Service Ppi
  @param  ModuleGuid    Pointer to Module File Guid
  @param  ErrorLevel    Debug Print Error Level mask to be Set/Configured for the current module

  @retval EFI_STATUS    Returns Success if module is found in Database, else Not Found
**/
typedef
EFI_STATUS
(EFIAPI *OEM_SET_DEBUG_PRINT_ERROR_LEVEL)(
  IN  OEM_DEBUG_LEVEL_LEVEL_SERVICE_PPI             *This,
  IN  EFI_GUID                                      *ModuleGuid,
  IN  UINT32                                        ErrorLevel
  );

struct _OEM_DEBUG_LEVEL_LEVEL_SERVICE_PPI {
    OEM_GET_DEBUG_PRINT_ERROR_LEVEL     GetPrintErrorLevel;
    OEM_SET_DEBUG_PRINT_ERROR_LEVEL     SetPrintErrorLevel;
};

extern EFI_GUID gOemDebugLevelServicePpiGuid;

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
