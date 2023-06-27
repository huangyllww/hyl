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
#include <DXEPCIREAD/Dxepciread.h>

#include <Setup.h>
#include <Library/AmiSetupLibrary.h>


EFI_STATUS EFIAPI 
DXEPCIReadLocate(
IN EFI_HANDLE ImageHandle,
IN EFI_SYSTEM_TABLE *SystemTable

)
{   
  EFI_STATUS                    Status;
  // initialize functions 
  // Install protocol
  EFI_PCI_ROOT_DXE_RW_PROTOCOL  *PCIRW;
  DEBUG((DEBUG_ERROR, "Hyl:PCI====66" ));
  InitAmiLib(ImageHandle, SystemTable);
  Status = pBS->LocateProtocol(&gDXEPCIreadProtocolGuid,NULL,&PCIRW);
  PCIRW->DXEPCIRead(0x00,0x04,0x00,0x00);    
  PCIRW->DXEPCIRead(0x00,0x04,0x00,0x01);      
  DEBUG((DEBUG_ERROR, "Hyl:PCI 0 4 0 Offset00: %x", PCIRW->DXEPCIRead(0x00,0x04,0x00,0x00)));
  DEBUG((DEBUG_ERROR, "Hyl:PCI 0 4 0 Offset01: %x", PCIRW->DXEPCIRead(0x00,0x04,0x00,0x01)));
  DEBUG((DEBUG_ERROR, "Hyl:PCI 0 4 0 Offset28: 66" ));
return EFI_SUCCESS;
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
