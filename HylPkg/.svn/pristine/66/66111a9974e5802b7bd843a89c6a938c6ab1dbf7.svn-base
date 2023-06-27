//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header: /Alaska/Tools/template.c 6     1/13/10 2:13p Felixp $
//
// $Revision: 6 $
//
// $Date: 1/13/10 2:13p $
//**********************************************************************
// Revision History
// ----------------
// $Log: /Alaska/Tools/template.c $
// 
// 6     1/13/10 2:13p Felixp
// 
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  <This File's Name>
//
// Description:	
//
//<AMI_FHDR_END>
//**********************************************************************
// PPI Structure
#include <cmosreadinstall/cmosread.h>
// GUID Define
// function code define
// ppi code define

EFI_STATUS PEI_SHOW_CMOS( IN EFI_FFS_FILE_HEADER *FfsHeader,
        IN CONST EFI_PEI_SERVICES **PeiServices)
{
      // PPI Structure
CMOS_READ_WRITE_PPI *readwritecmosppi;
    // Invoke PPI
    EFI_STATUS Status= EFI_SUCCESS; 
    Status = (*PeiServices)->LocatePpi(
                                        PeiServices,
                                        &gAMICMOSREADWRITEGuid,
                                        0,
                                        NULL,
                                        &readwritecmosppi
                                       ) ;  
    
      readwritecmosppi->peiwritecmos8(0x39,0x66);
 if( (readwritecmosppi->peireadcmos8(0x46))==0)
 {
     readwritecmosppi->peiwritecmos8(0x48,0x22);
     readwritecmosppi->peiwritecmos8(0x46,0x12);
 }
 else
 {
     readwritecmosppi->peiwritecmos8(0x3A,0x66);
     readwritecmosppi->peiwritecmos8(0x3B,0x66);
     readwritecmosppi->peiwritecmos8(0x3C, readwritecmosppi->peireadcmos8(0x46));
     DEBUG((DEBUG_ERROR, "Hyl:CMOS 0X3A: %x",readwritecmosppi->peireadcmos8(0x3A)));
     DEBUG((DEBUG_ERROR, "Hyl:CMOS 0X39: %x",readwritecmosppi->peireadcmos8(0x39)));
     DEBUG((DEBUG_ERROR, "Hyl:CMOS 0X3B: %x",readwritecmosppi->peireadcmos8(0x3B)));
    
   
 }
 
    return Status;
  
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
