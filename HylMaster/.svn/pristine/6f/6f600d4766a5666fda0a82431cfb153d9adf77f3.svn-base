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
// $Header: /SandBox/Pldm/MemFile.cpp 3     9/17/13 5:38p Timothym $
//
// $Revision: 3 $
//
// $Date: 9/17/13 5:38p $
//**********************************************************************
// Revision History
// ----------------
// $Log: /SandBox/Pldm/MemFile.cpp $
// 
// 3     9/17/13 5:38p Timothym
// Add PHDR and change log
// 
//**********************************************************************
//<AMI_FHDR_START>
//
// Name: MemFile.c*
//
// Description:	File-like buffer management
//
//<AMI_FHDR_END>
//**********************************************************************


#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "MemFile.h"

#ifdef TESTME   // not used 
UINT32
MemRead(MEMFILE *Buff, void *ReadBuffer, UINT32 ReadSize)
{
    if (Buff->CurrentOffset <= Buff->FileSize)
    {
        UINT32 Size;
        if (Buff->CurrentOffset + ReadSize <= Buff->FileSize)
            Size = ReadSize;    // all requested data available
        else
            Size = Buff->FileSize - Buff->CurrentOffset;
        memcpy(ReadBuffer, Buff->Buffer + Buff->CurrentOffset, Size);
        Buff->CurrentOffset += Size;
        return Size;
    }
    // set error flag here
    return 0;
}
#endif

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
MemWrite(MEMFILE *Buff, void *WriteData, UINT32 WriteSize)
{
    if (Buff->Buffer == 0)
        return 0;
    if (Buff->CurrentOffset + WriteSize > Buff->BuffSize)
    {   // need more buffer capacity
        int Need;
        Need = Buff->CurrentOffset + WriteSize;
        Need = (Need * 3) / 2;
        Buff->Buffer = (UINT8 *)realloc(Buff->Buffer, Need);
        if (Buff->Buffer == 0)
        {
            Buff->errno = 2;
            return 0;
        }
        Buff->BuffSize = Need;
    }
    memcpy(Buff->Buffer + Buff->CurrentOffset, WriteData, WriteSize);
    Buff->CurrentOffset += WriteSize;
    if (Buff->CurrentOffset > Buff->FileSize)
        Buff->FileSize = Buff->CurrentOffset;
    return WriteSize;
}

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
// 
int
MemCreat(MEMFILE *Buff)
{
    // start off with 20 bytes to reduce number of reallocs
    Buff->Buffer = (UINT8 *)malloc(20);
    Buff->BuffSize = 20;
    Buff->CurrentOffset = 0;
    Buff->FileSize = 0;
    Buff->errno = 0;
    if (Buff->Buffer)
        return 0;
    Buff->errno = 1;
    return -1;
}

#ifdef TESTME
int
MemOpen(MEMFILE *Buff, void *Data, UINT8 DataSize)
{
    Buff->Buffer = (UINT8 *)Data;
    Buff->BuffSize = DataSize;
    Buff->CurrentOffset = 0;
    Buff->FileSize = DataSize;
    Buff->errno = 0;
    if (0 == Buff->Buffer)
        return -1;
    return 0;
} 
#endif

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
MemClose(MEMFILE *Buff)
{
    free(Buff->Buffer);
    return Buff->errno; // will return non-zero on error (memory allocation)
}

#ifdef TESTME
main()
{
    MEMBUFF rbuff;
    MEMBUFF wbuff;

    UINT8 Data[] = "1234567890abcdef";
    UINT8 rData[50];
    int nread;
    MemCreat(&wbuff);
    MemWrite(&wbuff, Data, sizeof(Data) - 1);
    MemWrite(&wbuff, Data, sizeof(Data) - 2);
    MemWrite(&wbuff, Data, sizeof(Data) - 3);
    MemOpen(&rbuff, wbuff.Buffer, wbuff.CurrentOffset);
    for (nread = 0; ; )
    {
        memset(rData, 0, sizeof(rData));
        nread = MemRead(&rbuff, rData, 10); 
        if (nread)
            printf("%d read %s\n", nread, rData);
        else
        {
            printf("Endof Data\n");
            break;
        }
    }
}
#endif
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
