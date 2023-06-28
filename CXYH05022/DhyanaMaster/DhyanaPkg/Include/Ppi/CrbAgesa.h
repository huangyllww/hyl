//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//*************************************************************************
/** @file CrbAgesa.h

**/
//*************************************************************************

#ifndef __CRBAGESA_H__
#define __CRBAGESA_H__

// Type Definition(s)
typedef enum{
  GfxSlot1,
  GfxSlot2,
  GppSlot,
  DummySlot
} PCIE_RST_ID;

#define PCIE_DDI_DETECT_PPI_GUID \
  { 0xad1bf7f8, 0xcd6d, 0x4130, 0x8b, 0x6a, 0x64, 0x84, 0xdd, 0x3b, 0xe1, 0xd1 }

static EFI_GUID gPcieDdiDetectGuid = PCIE_DDI_DETECT_PPI_GUID;

typedef
EFI_STATUS
(EFIAPI *PCIEDDI_ENABLE_COMM) (
  IN  EFI_PEI_SERVICES   **PeiServices
);

typedef
BOOLEAN
(EFIAPI *PCIEDDI_HDMI_DOUNGLE) (
  IN  EFI_PEI_SERVICES    **PeiServices,
  IN  UINTN               DdiNo
);

typedef
EFI_STATUS
(EFIAPI *PCIEDDI_GET_ADAPTER_TYPE) (
  IN  EFI_PEI_SERVICES    **PeiServices,
  OUT UINT8               *AdapterType
);

typedef struct _PCIE_DDI_DETECT_PPI PCIE_DDI_DETECT_PPI;

typedef struct _PCIE_DDI_DETECT_PPI
{
  PCIEDDI_ENABLE_COMM       EnableComm;
  PCIEDDI_HDMI_DOUNGLE      IsHdmiDoungle;
  PCIEDDI_GET_ADAPTER_TYPE  GetAdapterType;
};

#endif


//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
