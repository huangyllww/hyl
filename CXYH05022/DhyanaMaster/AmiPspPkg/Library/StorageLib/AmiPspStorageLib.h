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

//*************************************************************************
//<AMI_FHDR_START>
//
// Name:  AmiPspStorageLib.h
//
//  Description:
//  Provides VOLUME_BLOCK_PROTOCOL for PSP.
//
//<AMI_FHDR_END>
//*************************************************************************
#ifndef _AMI_PSP_STORAGE_LIB_H_
#define _AMI_PSP_STORAGE_LIB_H_
#ifdef __cplusplus
extern "C" {
#endif

//
// Include files
//

#include "Protocol\FirmwareVolumeBlock.h"


// To be  able include HYGON's framework protocol.h file
#ifndef EFI_STRINGIZE
#define EFI_STRINGIZE(a)            #a
#define EFI_PROTOCOL_DEFINITION(a)  EFI_STRINGIZE (Protocol/a.h)
#define EFI_PROTOCOL_CONSUMER(a)    EFI_PROTOCOL_DEFINITION (a)
#endif


// define some dummy Filesystem GUID for PSP NVRAM Volume
#define AMI_PSP_STORAGE_FS_GUID \
 {0xfbb6da33, 0x534c, 0x41e3, 0x80, 0xf8, 0x7f, 0x10, 0xd2, 0xc1, 0x47, 0x3d}

#ifndef EFI_LBA_LIST_TERMINATOR
#define EFI_LBA_LIST_TERMINATOR 0xFFFFFFFFFFFFFFFF
#endif

#define DEFAULT_FVB2_ATTR   \
            EFI_FVB2_READ_ENABLED_CAP | \
            EFI_FVB2_READ_DISABLED_CAP | \
            EFI_FVB2_WRITE_ENABLED_CAP | \
            EFI_FVB2_WRITE_DISABLED_CAP | \
            EFI_FVB2_READ_STATUS | \
            EFI_FVB2_WRITE_STATUS | \
            EFI_FVB2_LOCK_CAP | \
            EFI_FVB2_STICKY_WRITE
            
#define FVB2_ATTR_CAP_MASK   \
            EFI_FVB2_READ_ENABLED_CAP | \
            EFI_FVB2_READ_DISABLED_CAP | \
            EFI_FVB2_WRITE_ENABLED_CAP | \
            EFI_FVB2_WRITE_DISABLED_CAP | \
            EFI_FVB2_LOCK_CAP
            
#define FVB2_ATTR_STATUS_MASK   \
            EFI_FVB2_READ_STATUS | \
            EFI_FVB2_WRITE_STATUS | \
            EFI_FVB2_LOCK_STATUS
            
#define MAX_ERASE_ARGS  64/2        // 32 pairs of variable argument list

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
