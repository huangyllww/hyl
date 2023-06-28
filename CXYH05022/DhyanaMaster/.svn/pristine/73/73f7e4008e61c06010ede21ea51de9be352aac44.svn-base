//**********************************************************************
//**********************************************************************
//**                                                     			  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file
  Implement the close API.

  Copyright (c) 2011, Intel Corporation
  All rights reserved. This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <SocketInternals.h>
#include "Token.h"     			// AMI PORTING

// AMI PORTING STARTS
#if STDLIB_SOCKETCLOSE_EVENT_SUPPORT
/**
  Calls EslServiceFreeProtocol function after TCP.Close succeded closing the connection.

  @param [in] Event     The close completion event

  @param [in] pSocketProtocol     Address of an ::EFI_SOCKET_PROTOCOL structure.

**/
VOID
FreeEslServiceinEvent (
  IN EFI_EVENT Event,
  IN EFI_SOCKET_PROTOCOL * pSocketProtocol
  )
{
	EslServiceFreeProtocol ( pSocketProtocol );
	gBS->CloseEvent (Event);
}
#endif
// AMI PORTING ENDS

/**
  Worker routine to close the socket.

  @param[in] pSocketProtocol   Socket protocol structure address

  @param[in] pErrno            Address of the ::errno variable

  @retval EFI_SUCCESS   Successfully closed the socket

**/
EFI_STATUS
BslSocketCloseWork (
  IN EFI_SOCKET_PROTOCOL * pSocketProtocol,
  IN int * pErrno
  )
{
  EFI_STATUS Status;

  //
  //  Start closing the socket
  //
  Status = pSocketProtocol->pfnCloseStart ( pSocketProtocol,
                                            FALSE,
                                            pErrno );
// AMI PORTING STARTS
#if STDLIB_SOCKETCLOSE_EVENT_SUPPORT
//create the event for notifying once close is done
  Status = gBS->CreateEvent (  EVT_NOTIFY_SIGNAL,
		  	  	  	  	  	   TPL_CALLBACK,
                               (EFI_EVENT_NOTIFY)FreeEslServiceinEvent,
                               pSocketProtocol,
                               &(pSocketProtocol->CloseEvent));
#endif
// AMI PORTING ENDS
  //
  //  Wait for the socket to close or an error
  //
#if STDLIB_SOCKETCLOSE_EVENT_SUPPORT == 0	// AMI PORTING
  while ( EFI_NOT_READY == Status ) {
    Status = pSocketProtocol->pfnClosePoll ( pSocketProtocol,
                                             pErrno );
  }
  if ( !EFI_ERROR ( Status )) {
    //
    //  Release the socket resources
    //
    *pErrno = EslServiceFreeProtocol ( pSocketProtocol );
  }
  else {
    DEBUG (( DEBUG_ERROR,
              "ERROR - Failed to close the socket: %r\r\n",
              Status ));
    *pErrno = EIO;
  }  
#endif	// AMI PORTING
  //
  //  Return the close status
  //
  return Status;
}


/**
  Close the socket

  The BslSocketClose routine is called indirectly from the close file
  system routine.  This routine closes the socket and returns the
  status to the caller.

  @param[in] pDescriptor Descriptor address for the file

  @return   This routine returns 0 upon success and -1 upon failure.
            In the case of failure, ::errno contains more information.

**/
int
EFIAPI
BslSocketClose (
  struct __filedes * pDescriptor
  )
{
  int CloseStatus;
  EFI_SOCKET_PROTOCOL * pSocketProtocol;

  //
  //  Locate the socket protocol
  //
  pSocketProtocol = BslValidateSocketFd ( pDescriptor, &errno );
  if ( NULL != pSocketProtocol ) {
    //
    //  Close the socket
    //
    BslSocketCloseWork ( pSocketProtocol, &errno );
  }

  //
  //  Return the close status
  //
  CloseStatus = ( errno == 0 ) ? 0 : -1;
  return CloseStatus;
}
