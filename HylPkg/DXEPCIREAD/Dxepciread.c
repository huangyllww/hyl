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

//********

#include<Dxepciread.h>

//#define DXE_GUID  {0xae257f7c, 0xf0e6, 0x4960, {0x85, 0xe7, 0x5e, 0xf4, 0x38, 0xbc, 0xda, 0x93} }
UINT32 DXEPCIRead (
	IN UINT32        bus_num,
	IN UINT32        device_num,
	IN UINT32        function_num,
	IN UINT32        offset)
{    
       
       UINT32  PciAddress = 0x80000000+(bus_num << 16 ) 
        +( device_num   << 11 )  +(function_num << 8 ) 
        +offset;
        IoWrite32(0xcf8,PciAddress);   
        return IoRead32(0xcfc);	    
}
void DXEPCIWrite (
	IN UINT32        bus_num,
	IN UINT32        device_num,
	IN UINT32       function_num,
	IN UINT32        offset,
    IN UINT32       data)
{     
    UINT32  PciAddress = 0x80000000+(bus_num << 16) 
           +( device_num   << 11 )  +(function_num << 8 ) 
           +offset;
           IoWrite32(0xcf8,PciAddress);   
           IoWrite32(0xcfc,data);          
}  


EFI_PCI_ROOT_DXE_RW_PROTOCOL myProtocol={DXEPCIRead,DXEPCIWrite };   

EFI_STATUS EFIAPI 
DXEPCIReadWrite(
IN EFI_HANDLE ImageHandle,
IN EFI_SYSTEM_TABLE *SystemTable
)
{
  EFI_STATUS                    Status;
  InitAmiLib(ImageHandle, SystemTable);
  // initialize functions 
  // Install protocol
     
  DEBUG((DEBUG_ERROR, "Hyl:DXEPCIReadWriteb entry" ));
   Status = pBS->InstallProtocolInterface(&ImageHandle,
           &gDXEPCIreadProtocolGuid,EFI_NATIVE_INTERFACE, &myProtocol);

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
