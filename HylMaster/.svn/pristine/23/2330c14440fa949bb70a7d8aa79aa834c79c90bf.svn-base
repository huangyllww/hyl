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
  Implement the connect API.

  Copyright (c) 2011, Intel Corporation
  All rights reserved. This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <SocketInternals.h>

// AMI PORTING STARTS
static BOOLEAN gTerminateConnectWork;	// Setting this variable means user wants to break the AcceptWork() function.
/** 
  This function helps to stop the function ConnectWork() by setting the variable gTerminateConnectWork to TRUE.
  @param [in] VOID
  
  @param [out] VOID
 **/
VOID ForceExitConnectWork()
{
	gTerminateConnectWork = TRUE;
}
// AMI PORTING ENDS

/**
  Connect to a remote system via the network.

  The connect routine attempts to establish a connection to a
  socket on the local or remote system using the specified address.

  There are three states associated with a connection:
  <ul>
    <li>Not connected</li>
    <li>Connection in progress</li>
    <li>Connected</li>
  </ul>
  In the initial "Not connected" state, calls to connect start the connection
  processing and update the state to "Connection in progress".  During
  the "Connection in progress" state, connect polls for connection completion
  and moves the state to "Connected" after the connection is established.
  Note that these states are only visible when the file descriptor is marked
  with O_NONBLOCK.  Also, the POLLOUT bit is set when the connection
  completes and may be used by poll or select as an indicator to call
  connect again.

  The
  <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/connect.html">POSIX</a>
  documentation is available online.
  
  @param [in] s         Socket file descriptor returned from ::socket.

  @param [in] address   Network address of the remote system

  @param [in] address_len Length of the remote network address

  @return     This routine returns zero if successful and -1 when an error occurs.
              In the case of an error, ::errno contains more details.

 **/
int
connect (
  int s,
  const struct sockaddr * address,
  socklen_t address_len
  )
{
  BOOLEAN bBlocking;
  int Count = 0; // !!! AMI PORTING EIP 195927 !!!
  int ConnectStatus;
  struct __filedes * pDescriptor;
  EFI_SOCKET_PROTOCOL * pSocketProtocol;
  EFI_STATUS Status;
  
  gTerminateConnectWork = FALSE;	// AMI PORTING - To exit this function forcefully.

  //
  //  Locate the context for this socket
  //
  pSocketProtocol = BslFdToSocketProtocol ( s,
                                            &pDescriptor,
                                            &errno );
  if ( NULL != pSocketProtocol ) {
    //
    //  Determine if the operation is blocking
    //
    bBlocking = (BOOLEAN)( 0 == ( pDescriptor->Oflags & O_NONBLOCK ));

    //
    //  Attempt to connect to a remote system
    //
    do {
    	// AMI PORTING STARTS - gTerminateConnectWork will be TRUE when TerminateConnectOperation() is called.
    	// It means user wants to terminate this function.
    	if(TRUE == gTerminateConnectWork)
    	{
			gTerminateConnectWork = FALSE;
    		Status = EFI_ABORTED;
    		errno = ETIMEDOUT;
    		break;
    	}
		// AMI PORTING ENDS - gTerminateConnectWork will be TRUE when TerminateConnectOperation() is called.
    	// It means user wants to terminate this function.
    	
      Count++; // !!! AMI PORTING EIP 195927 !!!
      errno = 0;
      Status = pSocketProtocol->pfnConnect ( pSocketProtocol,
                                             address,
                                             address_len,
                                             &errno );
    } while ( bBlocking && ( EFI_NOT_READY == Status ));
  }

  //
  //  Return the new socket file descriptor
  //
  if ((errno == EISCONN) && (Status == EFI_ALREADY_STARTED) && (Count > 1)) // !!! AMI PORTING EIP 195927!!!
	  return 0; /* !!! AMI PORTING EIP 195927!!!

   	1. In connect function pSocketProtocol->pfnConnect called in "do while" loop until status other than EFI_NOT_READY will be 
   	received.
	2. On first invocation EslSocketConnect goes on SOCKET_STATE_NOT_CONFIGURED path where pSocket->pApi->pfnConnectStart is called. This initiates connect (which can take some time) modifies pSocket->State = SOCKET_STATE_CONNECTING and returns EFI_NOT_READY.
	3. On all next invocation of EslSocketConnect takes SOCKET_STATE_CONNECTING path and calls pSocket->pApi->pfnConnectPoll.
	4. EslTcp4ConnectPoll checks pSocket->bConnected value and and if it is true (which means connecting in progress) returns 
	EFI_NOT_READY or if it is false (already connected) based on pSocket->ConnectStatus returns EFI_SUCCESS or some Error status. 
	It is important that in this time connect is pending and pSocket->bConnected and pSocket->ConnectStatus can be updated at any 
	time by a callback function.

	So somewhere after exiting from EslTcp4ConnectPoll and entering EslSocketConnect pSocket->bConnected and pSocket->ConnectStatus 
	gets updated and EslSocketConnect on its final invocation in "do while" loop (step 1) based on pSocket->ConnectStatus takes 
	SOCKET_STATE_CONNECTED path and returns EFI_ALREADY_STARTED.

	Only possibility when EFI_SUCCESS can be returned from EslSocketConnect is when pSocket->bConnected and pSocket->ConnectStatus 
	will be updated between SOCKET_STATE_CONNECTING path is chosen in EslSocketConnect and entering the 
	pSocket->pApi->pfnConnectPoll which is very unlikely.

	As a solution connect will return Success if EFI_ALREADY_STARTED was returned from EslSocketConnect, but not for the first time in "do while" loop.
   
   */
  ConnectStatus = (0 == errno) ? 0 : -1;
  return ConnectStatus;
}
