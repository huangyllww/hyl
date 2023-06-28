//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file RestHttpImpl.h
    This file contains definitions and includes for Rest over Http driver.

**/

#ifndef __UEFI_REST_NETWORK_IMPL_H__
#define __UEFI_REST_NETWORK_IMPL_H__

//----------------------------------------------------------------------

#include "Efi.h"
#include "Token.h"
#include <Protocol/Rest.h>
#include <Library/BaseMemoryLib.h>
#include <Library/TimerLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/HttpLib.h>

//----------------------------------------------------------------------

#define HTTP_BOOT_BLOCK_SIZE                 1500

typedef enum {
  //
  // Part of entity data.
  // Length of entity body in Data.
  //
  RestBodyParseEventOnData,
  //
  // End of message body.
  // Length is 0 and Data points to next byte after the end of the message.
  //
  RestBodyParseEventOnComplete
} REST_HTTP_BODY_PARSE_EVENT;

//
// Function Prototypes
//

EFI_STATUS
SendRecieveRestHttpCommand (
  IN  EFI_REST_PROTOCOL        *This,
  IN EFI_HTTP_MESSAGE *RequestMessage,
  OUT EFI_HTTP_MESSAGE *ResponseMessage);

EFI_STATUS RestParserCallback (
  IN HTTP_BODY_PARSE_EVENT EventType,
  IN CHAR8                      *Data,
  IN UINTN                      Length,
  IN VOID                       *Context
);
#endif  //__UEFI_REST_NETWORK_IMPL_H__

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
