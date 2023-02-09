//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2013, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**    5555 Oakbrook Pkwy, Norcross, Suite 200, Georgia 30093   **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//


//**********************************************************************
// $Header: /SandBox/Pldm/MemFile.h 3     9/17/13 5:38p Timothym $
//
// $Revision: 3 $
//
// $Date: 9/17/13 5:38p $
//**********************************************************************
// Revision History
// ----------------
// $Log: /SandBox/Pldm/MemFile.h $
// 
// 3     9/17/13 5:38p Timothym
// Add PHDR and change log
// 
//**********************************************************************
//<AMI_FHDR_START>
//
// Name: MemFile.h
//
// Description:	File-like buffer management
//
//<AMI_FHDR_END>
//**********************************************************************

// avoid global name collisions
#ifndef _LNX
#define MemWrite MemWritePldm
#endif

typedef struct {
  UINT8 *Buffer;
  UINT32 FileSize;       // size of "File"
  UINT32 CurrentOffset;  // file pointer
  UINT32 BuffSize;       // size of buffer
  UINT32 errno;          // capture error state
} MEMFILE;

// not implemented
//UINT32
//MemRead(MEMFILE *Buff, void *ReadBuffer, UINT32 ReadSize);

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: 	MemWrite
//
// Description: Write to memory buffer
//              If realloc fails the error will be latched in errno
//				
// Parameters:  MEMFILE *
//                 [in] Buffer Control structure 
//              void *
//                 [in] Pointer to data to write to buffer
//              UINT32
//                 [in] Number of bytes to write
//
// Return: Number of bytes written to buffer. 
//      
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT32
MemWrite(MEMFILE *Buff, void *WriteData, UINT32 WriteSize);

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: 	MemCreat
//
// Description: Initialize buffer for writing
//				
// Parameters:  MEMFILE *
//                 [in] Buffer Control structure to initialize
//
// Return: 0 on success, -1 if malloc fails
//      
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
int
MemCreat(MEMFILE *Buff);

// not implemented
int
MemOpen(MEMFILE *Buff, void *Data, UINT8 DataSize);

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: 	MemClose
//
// Description: Close memory buffer
//				
// Parameters:  MEMFILE *
//                 [in] Buffer Control structure 
// Return: returns errno element. If write failed at some point this will be
//          non-zero.
//      
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
int
MemClose(MEMFILE *Buff);

//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2013, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**    5555 Oakbrook Pkwy, Norcross, Suite 200, Georgia 30093   **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
