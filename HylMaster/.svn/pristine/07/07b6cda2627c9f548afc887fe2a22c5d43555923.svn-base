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

/** @file MemRestorePpi.h

**/
#ifndef _MEM_RESTORE_PPI_H_
#define _MEM_RESTORE_PPI_H_
#ifdef __cplusplus
extern "C" {
#endif

#define MEM_RESTORE_PPI_GUID \
{0xDF74DAAA, 0x4FF1, 0x4778, 0xA8, 0x04, 0x44, 0xAD, 0x7B, 0xD4, 0x39, 0x03}

typedef struct _MEM_RESTORE_PPI		MEM_RESTORE_PPI;

/// Defines function prototype for MemRestoreInit
typedef
EFI_STATUS
(EFIAPI *MEM_RESTORE_INIT) (
  IN      EFI_PEI_SERVICES      **PeiServices,  ///< Pointer to PeiServices
  IN      MEM_RESTORE_PPI       *This,          ///< Pointer to the PPI structure
  IN OUT  UINT8                 *MemData        ///< Memory data structure
  );

/// Defines function prototype for MemRestoreSave
typedef
EFI_STATUS
(EFIAPI *MEM_RESTORE_SAVE) (
  IN      EFI_PEI_SERVICES      **PeiServices,  ///< Pointer to PeiServices
  IN      MEM_RESTORE_PPI       *This,          ///< Pointer to the PPI structure
  IN OUT  UINT8                 *MemData        ///< Memory data structure
  );

#pragma pack (push, 1)
/// MEM_RESTORE_PPI data structure definition.
typedef struct _MEM_RESTORE_PPI {
  MEM_RESTORE_INIT      MemRestoreInit;    ///< Function to be called
  MEM_RESTORE_SAVE      MemRestoreSave;
} MEM_RESTORE_PPI;
#pragma pack (pop)

EFI_STATUS
EFIAPI
MemRestoreInit (
  IN      EFI_PEI_SERVICES      **PeiServices,  ///< Pointer to PeiServices
  IN      MEM_RESTORE_PPI       *This,          ///< Pointer to the PPI structure
  IN OUT  UINT8                 *MemData        ///< Memory data structure
  );

EFI_STATUS
EFIAPI
MemRestoreSave (
  IN      EFI_PEI_SERVICES      **PeiServices,  ///< Pointer to PeiServices
  IN      MEM_RESTORE_PPI       *This,          ///< Pointer to the PPI structure
  IN OUT  UINT8                 *MemData        ///< Memory data structure
  );

extern EFI_GUID gMemRestorePpiGuid;

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
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
