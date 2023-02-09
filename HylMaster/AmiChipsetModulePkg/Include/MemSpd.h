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

/** @file MemSpd.h
    MemSpd definitions 

**/
#ifndef __MEMSPD__H__
#define __MEMSPD__H__
#ifdef __cplusplus
extern "C" {
#endif

#include "Agesa.h"

extern EFI_GUID gSpdByPassHobGuid;
extern EFI_GUID gMemRestoreSerialHobGuid;

EFI_STATUS
EFIAPI 
PlatformDimmSpdRead (
  IN      EFI_PEI_SERVICES                    **PeiServices,  ///< Pointer to PeiServices
  IN struct _PEI_AMD_PLATFORM_DIMM_SPD_PPI    *This,          ///< Pointer to the PPI structure
  IN OUT  AGESA_READ_SPD_PARAMS               *SpdData        ///< SPD Data Buffer
  );

typedef VOID (AMI_HOOK_BEFORE_READSPD) (EFI_PEI_SERVICES **PeiServices, AGESA_READ_SPD_PARAMS *SpdData, UINT8 **SpdBuffer);

#pragma pack (push, 1)
typedef struct {
//  UINT8  SocketId;
  UINT8  ChannelId;
  UINT8  DimmId;
  UINT8  SmbusAddress;
} DIMM_INFO_SMBUS;
#pragma pack (pop)


#define DIMM_DEVICE_SLOT_COUNT      DIMM_SLOTS_PER_SCKT       // Dimm Slot depends on platform
#define DIMM_SPD_SERIAL_NO_LENGTH   6       // 4   Bytes Serial Number + 2 Byte CRC
#define DIMM_SPD_PART_NO_LENGTH     18      // 18  Bytes Part Number
#define DIMM_SPD_DATA_LENGTH        256     // 256 Bytes SPD Data
#define DIMM_SPD_SERIAL_NO_OFFSET   0x7A    // SPD Data Offset 7Ah
#define DIMM_SPD_PART_NO_OFFSET     0x80    // SPD Data Offset 80h

                                            // 24  Bytes Serial no Data + CRC
#define DIMM_HOB_SERIAL_NO_LENGTH   DIMM_SPD_SERIAL_NO_LENGTH * DIMM_DEVICE_SLOT_COUNT
                                            // 256 Bytes Hob Data
#define DIMM_HOB_DATA_LENGTH        DIMM_SPD_DATA_LENGTH * DIMM_DEVICE_SLOT_COUNT
#pragma pack (push, 1)
typedef struct {
    EFI_HOB_GUID_TYPE       Header;
    UINT8                   SPD_Serial[DIMM_HOB_SERIAL_NO_LENGTH];
    UINT8                   SPD_Data[DIMM_HOB_DATA_LENGTH];
    UINT8                   SPDUpdate;      // a flag to indicate if we need to set variable in DXE
} SPD_BYPASS_HOB;
#pragma pack (pop)


#define TOTAL_SOCKETS NSOCKETS
#define TOTAL_DIMM_SLOTS_IN_SYSTEM DIMM_SLOTS_PER_SCKT * TOTAL_SOCKETS
#define MEMRESTORE_SERIAL_HOB_LENGTH TOTAL_DIMM_SLOTS_IN_SYSTEM * DIMM_SPD_SERIAL_NO_LENGTH

#pragma pack (push, 1)
typedef struct {
    EFI_HOB_GUID_TYPE       Header;
    UINT8                   SPD_Serial[MEMRESTORE_SERIAL_HOB_LENGTH];
    UINT8                   SPDUpdate;      // a flag to indicate if we need to set variable in DXE
} MEMRESTORE_SERIAL_HOB;
#pragma pack (pop)

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
