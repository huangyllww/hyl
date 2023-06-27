//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **

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

//----------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------

#include <MyHob.h>

//<AMI_PHDR_END>
EFI_STATUS EFIAPI PEICreatMyHob (
    IN EFI_FFS_FILE_HEADER      *FfsHeader,
    IN EFI_PEI_SERVICES         **PeiServices
)

{
    EFI_STATUS         Status;
    SMP_HOB           *MyNewHob=NULL;   
    EFI_GUID   MyPEI_DXE_HobGuid=PEI_DXE_MYHOB_GUID;
    // Create Recovery Hob
    Status = (*PeiServices)->CreateHob(
        PeiServices, EFI_HOB_TYPE_GUID_EXTENSION, 
        sizeof(SMP_HOB), &MyNewHob);
    if(!EFI_ERROR(Status))
    {
        MyNewHob->EfiHobGuidType.Name=MyPEI_DXE_HobGuid;
        MyNewHob->SmpHob_U32=0x12345678;
        MyNewHob->SmpHob_U16=0xabcd;
        MyNewHob->MyBoardId=0x11;
    }
  
    DEBUG((DEBUG_ERROR, "Hyl:HOB entry" )); 
//<Wangyia003-20170331 Add password clear pin function ->

    //TODO: Put your code here end
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
