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

/** @file NbDxeBoard.h
    This file declares types, functions, and constants for 
    northbridge board-specific DXE phase initialization.

**/

#ifndef __NBDXEBOARD_H__
#define __NBDXEBOARD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <Protocol/PciRootBridgeIo.h>
#include "Protocol/SMBios.h"
#include <Protocol/SmbusHc.h>
#include <Protocol/AmiSmbios.h>

#ifdef EFI_DEBUG


// return EFI_STATUS on error
#ifndef ABORT_CALL
#define ABORT_CALL(FunctionCall) { \
    EFI_STATUS Status = FunctionCall; \
    if ( EFI_ERROR( (Status) ) ) \
    { \
      TRACE((TRACE_ALWAYS, \
        "ABORT in %s on %i: %s\n",__FILE__, __LINE__, #FunctionCall)); \
      return Status; \
    } } 
#endif

// override EFI_STATUS return value on error
#ifndef ABORT_CALL_STATUS
#define ABORT_CALL_STATUS(Status, FunctionCall) { \
    if ( EFI_ERROR( (FunctionCall) ) ) \
    { \
      TRACE((TRACE_ALWAYS, \
        "ABORT in %s on %i: %s\n",__FILE__, __LINE__, #FunctionCall)); \
      return Status; \
    } } 
#endif

// force hang on error
#ifndef ASSERT_CALL
#define ASSERT_CALL(FunctionCall) ASSERT_EFI_ERROR(FunctionCall)
#endif

#else // EFI_DEBUG is not defined

// return EFI_STATUS on error
#ifndef ABORT_CALL
#define ABORT_CALL(FunctionCall) { \
    EFI_STATUS Status = FunctionCall; \
    if (EFI_ERROR(Status)) \
      return Status; \
    }
#endif

// override EFI_STATUS return value on error
#ifndef ABORT_CALL_STATUS
#define ABORT_CALL_STATUS(Status, FunctionCall) { \
  if (EFI_ERROR(FunctionCall)) \
    return Status; \
  }
#endif

// force hang on error
#ifndef ASSERT_CALL
#define ASSERT_CALL(FunctionCall) { \
  if (EFI_ERROR(FunctionCall)) \
    EFI_DEADLOOP(); \
  }
#endif

#endif // #ifdef EFI_DEBUG

//**********************************************************************//
// If SMBios is present, then its data structures and global constants 
// will be used.  The function CreateMemoryDataVariableCallBack assumes 
// structures conform to the following declarations.  Thus, care must be 
// taken to ensure that SYSTEM_MEM_ARRAY_DYNAMIC_DATA is properly 
// initialized so that the data can be accessed by setup and SMBios.
//**********************************************************************//

EFI_STATUS NBDXE_BoardInit (
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable );

#pragma pack(push,1)
typedef struct {
//  UINT8  SocketId;
  UINT8  ChannelId;
  UINT8  DimmId;
  UINT8  SmbusAddress;
} DIMM_INFO_SMBUS_CFG;
#pragma pack(pop)

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
