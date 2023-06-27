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
#include "cmosread.h"

#define CMOS_GUID  {0xdcc32309, 0x3d1e, 0x4253, {0x93, 0xf9, 0x6c, 0xf1, 0xdc, 0xa4, 0x35, 0x96}} 
void peiwritecmos8(UINT8 OFFSET,UINT8 DATA)
   {    
        IoWrite8(0x70,OFFSET);   
        IoWrite8(0x71,DATA);
   }


UINT8 peireadcmos8( UINT8 OFFSET_READ)
   {
        IoWrite8(0x70,OFFSET_READ);
        return IoRead8(0x71);
   }
static CMOS_READ_WRITE_PPI CMOSPpi ={
        peiwritecmos8,
        peireadcmos8
};
EFI_GUID  gAMICMOSREADWRITEGuid = CMOS_GUID ;



static EFI_PEI_PPI_DESCRIPTOR CMOS_PPI_WRITE_READ[]= {
{ EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
        &gAMICMOSREADWRITEGuid, &CMOSPpi}
};

EFI_STATUS EFIAPI CMOS_SHOW_PEI_TRACE(
        IN EFI_FFS_FILE_HEADER *FfsHeader,
        IN CONST EFI_PEI_SERVICES **PeiServices)
{  
    EFI_STATUS Status= EFI_SUCCESS; 
 

    
    Status = (*PeiServices)->InstallPpi(PeiServices, CMOS_PPI_WRITE_READ); //Install PPI
   
    
    
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
