/** @file
  Implement the constructor and destructor for the EFI socket library

  Copyright (c) 2011, Intel Corporation
  All rights reserved. This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "Socket.h"
#include "Token.h"	// AMI PORTING.

//
// AMI PORTING STARTS
//
#if (SINGLETON_ESLCONSTRUCTOR_SUPPORT == 1)
extern	EFI_BOOT_SERVICES 		*gBS;
extern	EFI_RUNTIME_SERVICES 	*gRT;

/**
	Callback to execute constructor functionality based on Tcp4 protocol availability
**/
VOID	Tcp4NotifyCallback( EFI_EVENT 	Event, VOID      *Context)
{
	gBS->CloseEvent(Event);
	
	EslConstructor ();
}
#endif	// SINGLETON_ESLCONSTRUCTOR_SUPPORT
//
// AMI PORTING ENDS
//

/**
  EFI Socket Library Constructor

  This routine supports an implementation dependent constructor
  depending upon whether the library is linked to a socket
  application or the SocketDxe driver.  The following modules
  declare the redirection for the constructor in ::mpfnEslConstructor:
  <ul>
    <li>StdLib/EfiSocketLib/UseSocketLib.c - Application links against EfiSocketLib</li>
    <li>StdLib/SocketDxe/EntryUnload.c - SocketDxe links against EfiSocketLib</li>
  </ul>

  The EfiSocketLib.inf file lists ::EslConstructor as the CONSTRUCTOR
  in the [Defines] section.  As a result, this routine is called by
  the ProcessLibraryConstructorList routine of the AutoGen.c module
  in the Build directory associated with the socket application or
  the SocketDxe driver.

  @retval EFI_SUCCESS       The socket layer initialization was successful

 **/
EFI_STATUS
EFIAPI
EslConstructor (
  VOID
  )
{
  EFI_STATUS Status;

//
// AMI PORTING STARTS
//
#if (SINGLETON_ESLCONSTRUCTOR_SUPPORT == 1)

  void 			*TcpBuffer;
  EFI_EVENT 	Tcp4RegEvent;
  VOID          *Tcp4NotifyRegistration;

  Status = gBS->LocateProtocol (&gEfiTcp4ProtocolGuid, NULL, (VOID**)&TcpBuffer);
  if (EFI_ERROR (Status))
  {
		Status = gBS->CreateEvent( EVT_NOTIFY_SIGNAL,TPL_CALLBACK, 
				Tcp4NotifyCallback, NULL, &Tcp4RegEvent);
		if (!EFI_ERROR(Status)) 
			Status = gBS->RegisterProtocolNotify(&gEfiTcp4ProtocolGuid, Tcp4RegEvent, &Tcp4NotifyRegistration);	
		
    return EFI_SUCCESS;
  }
#endif	// SINGLETON_ESLCONSTRUCTOR_SUPPORT
//
// AMI PORTING ENDS
//
  
  DBG_ENTER ( );

  //
  //  Assume success
  //
  Status = EFI_SUCCESS;

  //
  //  Call the image dependent constructor if available
  //
  if ( NULL != mpfnEslConstructor ) {
    Status = mpfnEslConstructor ( );
  }

  //
  //  Return the constructor status
  //
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  EFI Socket Library Destructor

  This routine supports an implementation dependent destructor
  depending upon whether the library is linked to a socket
  application or the SocketDxe driver.  The following modules
  declare the redirection for the destructor in ::mpfnEslDestructor:
  <ul>
    <li>StdLib/EfiSocketLib/UseSocketLib.c - Application links against EfiSocketLib</li>
    <li>StdLib/SocketDxe/EntryUnload.c - SocketDxe links against EfiSocketLib</li>
  </ul>

  The EfiSocketLib.inf file lists ::EslDestructor as the DESTRUCTOR
  in the [Defines] section.  As a result, this routine is called by
  the ProcessLibraryDestructorList routine of the AutoGen.c module
  in the Build directory associated with the socket application or
  the SocketDxe driver.

  @retval EFI_SUCCESS       The socket layer shutdown was successful

 **/
EFI_STATUS
EFIAPI
EslDestructor (
  VOID
  )
{
  EFI_STATUS Status;

  DBG_ENTER ( );

  //
  //  Assume success
  //
  Status = EFI_SUCCESS;

  //
  //  Call the image dependent destructor if available
  //
  if ( NULL != mpfnEslDestructor ) {
    Status = mpfnEslDestructor ( );
  }

  //
  //  Return the constructor status
  //
  DBG_EXIT_STATUS ( Status );
  return Status;
}
