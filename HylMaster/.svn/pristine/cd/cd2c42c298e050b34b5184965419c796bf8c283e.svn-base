//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093   	  **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file
    Implementation of fopen as declared in <stdio.h>.

    Copyright (c) 2010 - 2011, Intel Corporation. All rights reserved.<BR>
    This program and the accompanying materials are licensed and made available
    under the terms and conditions of the BSD License that accompanies this
    distribution.  The full text of the license may be found at
    http://opensource.org/licenses/bsd-license.php.

    THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
    WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

    Copyright (c) 1990, 1993
    The Regents of the University of California.  All rights reserved.

    This code is derived from software contributed to Berkeley by
    Chris Torek.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
      - Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      - Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      - Neither the name of the University nor the names of its contributors
        may be used to endorse or promote products derived from this software
        without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

    NetBSD: fopen.c,v 1.12 2003/08/07 16:43:24 agc Exp
    fopen.c 8.1 (Berkeley) 6/4/93"
**/
#include  <LibConfig.h>
#include  <sys/EfiCdefs.h>

#include  <sys/types.h>
#include  <sys/stat.h>
#include  <assert.h>
#include  <fcntl.h>
#include  <stdio.h>
#include  <errno.h>
#include  <unistd.h>
#include  "reentrant.h"
#include  "local.h"
// AMI PORTING STARTS
#include  "Token.h"        
#include  "AMIFileXRef.h"
#define Read_Flag 1156
// AMI PORTING ENDS
FILE *
fopen(const char *file, const char *mode)
{
  FILE *fp;
// AMI PORTING STARTS
#if(STDLIB_READ_FROM_FV)
  int f = INVALID_FILE_DESC;
#else
// AMI PORTING ENDS
  int f;
#endif	// AMI PORTING - STDLIB_READ_FROM_FV

  int flags, oflags;

// AMI PORTING STARTS
#if(STDLIB_READ_FROM_FV)  // Token for supporting Firmware volume file operations
  BOOLEAN IsAFwFile = FALSE;
  BOOLEAN IsValidMode = FALSE;	

  // if gFWDescriptors is not initialized, initialize the array with FREE_TO_USE
	if(!gFWDescriptorsInitialized)
		InitializeFWDescriptors(); 
#endif  //STDLIB_READ_FROM_FV
// AMI PORTING ENDS
  _DIAGASSERT(file != NULL);  

// AMI PORTING STARTS
#if(STDLIB_READ_FROM_FV  &&  (STDLIB_FV_OR_FS_PRIORITY == READ_FROM_FV_ONLY || STDLIB_FV_OR_FS_PRIORITY == PREFER_FV_THEN_FS))
  // check if the file is a firmware volume file.
  IsAFwFile = IsAFirmwareFile(((char*)file));
  if(IsAFwFile)
  {  
	  // if it's a firmware volume file, check the mode to open is safe/acceptable.
	  IsValidMode = IsAValidFWMode(((char*)mode));
	  // when a FV file is tried to open with invalid mode, return NULL.
	  if(!IsValidMode && (STDLIB_FV_OR_FS_PRIORITY == READ_FROM_FV_ONLY))
		  return (fp = NULL);
  }
#endif	// STDLIB_READ_FROM_FV
// AMI PORTING ENDS
  if ((flags = __sflags(mode, &oflags)) == 0)
    return (NULL);
  if ((fp = __sfp()) == NULL)
    return (NULL);
// AMI PORTING STARTS 
#if(STDLIB_READ_FROM_FV)  // Token for supporting Firmware volume file operations
#if(STDLIB_FV_OR_FS_PRIORITY == READ_FROM_FV_ONLY || STDLIB_FV_OR_FS_PRIORITY == PREFER_FV_THEN_FS)
  
  if (IsAFwFile)
	 f = FVfopen(fp,((char*)file));
	 // if file has to be read from FV only but opening/reading fails, return NULL.
	 if(STDLIB_FV_OR_FS_PRIORITY == READ_FROM_FV_ONLY && f < 0)
		 return (fp = NULL);

  // if user prefers FV, but not found, then go for FS.
	 if (STDLIB_FV_OR_FS_PRIORITY == PREFER_FV_THEN_FS && f < 0)
	 {
		 // check if Shell Protocol is initialized.
		 // if YES open the file else return NULL.
		 if(!IsShellProtocolAvailable())
	 		  return (fp = NULL);
		 else
			 IsAFwFile = 0; //the FV read is failed so making IsAFwFile value to false for file system support
	 }
#else if(STDLIB_FV_OR_FS_PRIORITY == READ_FROM_FS_ONLY || STDLIB_FV_OR_FS_PRIORITY == PREFER_FS_THEN_FV)
	 // here, user wants to open file from FS only, if Shell Protocol is initialized open the requested file else return NULL.
  if( (STDLIB_FV_OR_FS_PRIORITY == READ_FROM_FS_ONLY) && (!IsShellProtocolAvailable()))
	  return (fp = NULL);
  // user wants to open file from FV if the required file is not present in FS.
  if(STDLIB_FV_OR_FS_PRIORITY == PREFER_FS_THEN_FV)
  ;// TODO :: Not supported now
#endif	// STDLIB_FV_OR_FS_PRIORITY == READ_FROM_FV_ONLY || STDLIB_FV_OR_FS_PRIORITY == PREFER_FV_THEN_FS
  
if(!IsAFwFile)
{
#endif	// STDLIB_READ_FROM_FV
// AMI PORTING ENDS	
	if ((f = open(file, oflags, DEFFILEMODE)) < 0)
    goto release;
  if (oflags & O_NONBLOCK) {
    struct stat st;
    if (fstat(f, &st) == -1) {
      int sverrno = errno;
      (void)close(f);
      errno = sverrno;
      goto release;
    }
    if (!S_ISREG(st.st_mode)) {
      (void)close(f);
      errno = EFTYPE;
      goto release;
    }
  }
// AMI PORTING STARTS
#if(STDLIB_READ_FROM_FV)   // Token for supporting Firmware volume file operations
}
#endif	// STDLIB_READ_FROM_FV
// AMI PORTING ENDS	 
  fp->_file = (short)f;
// AMI PORTING STARTS
#if(STDLIB_READ_FROM_FV)  // Token for supporting Firmware volume file operations
  if(IsAFirmwareDescriptor(fp->_file))  // Checking Firmware volume file Descriptor status.
  fp->_flags = (unsigned short)Read_Flag; 
  else
#endif  // STDLIB_READ_FROM_FV
// AMI PORTING ENDS
  fp->_flags = (unsigned short)flags;
  fp->_cookie = fp;
  fp->_read = __sread;
  fp->_write = __swrite;
  fp->_seek = __sseek;
  fp->_close = __sclose;

  /*
   * When opening in append mode, even though we use O_APPEND,
   * we need to seek to the end so that ftell() gets the right
   * answer.  If the user then alters the seek pointer, or
   * the file extends, this will fail, but there is not much
   * we can do about this.  (We could set __SAPP and check in
   * fseek and ftell.)
   */
  if (oflags & O_APPEND)
    (void) __sseek((void *)fp, (fpos_t)0, SEEK_END);
  return (fp);
release:
  fp->_flags = 0;     /* release */
  return (NULL);
}
