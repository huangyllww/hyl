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
/** @file
  Implementation of the <stdlib.h> functions responsible for communication with
  the environment:
    - abort(void)
    - atexit(void(*handler)(void))
    - exit(int status)
    - _Exit(int status)

  Copyright (c) 2010 - 2011, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials are licensed and made available under
  the terms and conditions of the BSD License that accompanies this distribution.
  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include  <Uefi.h>
#include  <Library/UefiBootServicesTableLib.h>
#include  <Library/BaseLib.h>
#include  <Library/DebugLib.h>
#include  <Library/MemoryAllocationLib.h>

#include  <LibConfig.h>

#include  <errno.h>
#include  <stdlib.h>

/* #################  Public Functions  ################################### */

/** The abort function causes abnormal program termination to occur, unless
    the signal SIGABRT is being caught and the signal handler does not return.

    Open streams with unwritten buffered data are not flushed, open
    streams are not closed, and temporary files are not removed by abort.

**/
void
abort(void)
{
  ASSERT (FALSE);
}

/** The atexit function registers the function pointed to by func, to be
    called without arguments at normal program termination.

    The implementation shall support the registration of
    at least 32 functions.

    @return   The atexit function returns zero if the registration succeeds,
              nonzero if it fails.
**/
int
atexit(void (*handler)(void))
{
  return -1;
}

/** The exit function causes normal program termination to occur. If more than
    one call to the exit function is executed by a program,
    the behavior is undefined.

    First, all functions registered by the atexit function are called, in the
    reverse order of their registration. If, during the call to any such function, a
    call to the longjmp function is made that would terminate the call to the
    registered function, the behavior is undefined.

    Next, all open streams with unwritten buffered data are flushed, all open
    streams are closed, and all files created by the tmpfile function
    are removed.

    The status returned to the host environment is determined in the same way
    as for the _Exit function.
**/
void
exit(int status)
{
  _Exit(status);
}

/** The _Exit function causes normal program termination to occur and control
    to be returned to the host environment.

    No functions registered by the atexit function or signal handlers
    registered by the signal function are called.  Open streams with unwritten
    buffered data are not flushed, open streams are not closed, and temporary
    files are not removed by abort.

    Finally, control is returned to the host environment. If the value of
    status is zero, or EXIT_SUCCESS, status is returned unchanged. If the value
    of status is EXIT_FAILURE, RETURN_ABORTED is returned.
    Otherwise, status is returned unchanged.
**/
void
_Exit(int status)
{
  return;
#ifdef __GNUC__
  __builtin__Exit(status);         /* Keep GCC happy - never reached */
#endif
}

/** If string is a null pointer, the system function determines whether the
    host environment has a command processor. If string is not a null pointer,
    the system function passes the string pointed to by string to that command
    processor to be executed in a manner which the implementation shall
    document; this might then cause the program calling system to behave in a
    non-conforming manner or to terminate.

    @retval   EXIT_FAILURE    EFIerrno will contain the EFI status code
                              indicating the cause of failure.

    @retval   EXIT_SUCCESS    EFIerrno will contain the EFI status returned
                              by the executed command string.
    @retval   0               If string is NULL, 0 means a command processor
                              is not available.
    @retval   1               If string is NULL, 1 means a command processor
                              is available.
**/
int
system(const char *string)
{
  return EXIT_FAILURE;
}

/** The getenv function searches an environment list, provided by the host
    environment, for a string that matches the string pointed to by name.  The
    set of environment names and the method for altering the environment list
    are determined by the underlying UEFI Shell implementation.

    @return   The getenv function returns a pointer to a string associated with
              the matched list member.  The string pointed to shall not be
              modified by the program, but may be overwritten by a subsequent
              call to the getenv function.  If the specified name cannot be
              found, a null pointer is returned.
**/
char   *getenv(const char *name)
{
  return NULL;
}


/**
  Add or update a variable in the environment list

  @param name     Address of a zero terminated name string
  @param value    Address of a zero terminated value string
  @param rewrite  TRUE allows overwriting existing values

  @retval Returns 0 upon success
  @retval Returns -1 upon failure, sets errno with more information

  Errors

  EINVAL - name is NULL or points to a zero length string
  EALREADY - name already set and rewrite set to FALSE
  ENODEV - Unable to set non-volatile version of environment variable
  ENOMEM - Unable to set volatile version of environment variable
  ENOTSUP - Variable storage not supported

**/
int
setenv (
  register const char * name,
  register const char * value,
  int rewrite
  )
{
  return -1;
}

