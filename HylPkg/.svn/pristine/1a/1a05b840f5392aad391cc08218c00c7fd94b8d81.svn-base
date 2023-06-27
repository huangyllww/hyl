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
//???pci??32?
#include <SetupText.h>
/*
UINT32 MyPCIRead (
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
*/


VOID InitSetupText(EFI_HII_HANDLE HiiHandle, UINT16 Class)
{ EFI_HANDLE                  *MyHandleBuffer;
  EFI_STATUS  Status;
  PCI_DEV_INFO    *Dev;
  UINT16 VID=0;
  UINT16 DID=0;
  UINT8 bus_num=0;
  UINT8 device_num=0;
  UINT8 function_num=0;
  UINTN i,MyNumberOfHandles;
  Status = pBS->LocateHandleBuffer(ByProtocol,
                                             &gEfiPciIoProtocolGuid,
                                             NULL,
                                             &MyNumberOfHandles,
                                             &MyHandleBuffer);
          
            if (EFI_ERROR(Status)) return;
            DEBUG((DEBUG_ERROR, "Hyl:MyNumberOfHandles:%x ", MyNumberOfHandles));
            for(i=0;i<MyNumberOfHandles;i++)
            {
                UINT8 PciClass[4];
                EFI_PCI_IO_PROTOCOL *PciIO;
              Status = pBS->HandleProtocol(MyHandleBuffer[i],
                                             &gEfiPciIoProtocolGuid,
                                             (VOID**)&PciIO);
            if(EFI_ERROR(Status)) continue;
            DEBUG((DEBUG_ERROR, "----------%x--------- ", i));
            Dev=(PCI_DEV_INFO*)PciIO;
            Status=PciIO->Pci.Read(PciIO, EfiPciIoWidthUint32, 0x08, 1, &PciClass);
            DEBUG((DEBUG_ERROR, "Hyl:MyNumberOfHandles:%x ", PciClass[3]));
            if (PciClass[3] == 0x02) {
                bus_num=   Dev->AmiSdlPciDevData->Bus;
                device_num =Dev->AmiSdlPciDevData->Device;
                function_num=Dev->AmiSdlPciDevData->Function;
                Status=PciIO->Pci.Read(PciIO, EfiPciIoWidthUint16, 0x00, 1, &VID);
                Status=PciIO->Pci.Read(PciIO, EfiPciIoWidthUint16, 0x02, 1, &DID);
                DEBUG((DEBUG_ERROR, "Hyl:VIDs:%x ", VID));
                DEBUG((DEBUG_ERROR, "Hyl:DID:%x ", DID));
                InitString( HiiHandle,     //show bus dev fuc 
                                                  STRING_TOKEN(STR_PCI_DEVICE_VALUE),
                                                  L"PCI BUS_NUM:%d DEVICE_NUM:%d FUNCTION_NUM:%d VID:%x DID:%x" ,bus_num,device_num,function_num,VID,DID);
                                 
            pBS->FreePool(MyHandleBuffer);
             return;    
            }
                
            }     
          pBS->FreePool(MyHandleBuffer);
          InitString(HiiHandle,
          STRING_TOKEN(STR_PCI_DEVICE_VALUE),
          L"Not Found ");


}
  /* 
    UINT32 bus_num=0;
    UINT32 device_num=0;
    UINT32 function_num=0;
    UINT32 VID=0;
    UINT32 DID=0;
    UINT32 result;
    DEBUG((DEBUG_ERROR, "====================================\n" ));
    DEBUG((DEBUG_ERROR, "Hyl:InitSetupText entry\n "));

    for(bus_num=0;bus_num<256;bus_num++ )
    {
        for(device_num=0;device_num<32;device_num++ )
          {
            for(function_num=0;function_num<8;function_num++ )
              {  
              result= MyPCIRead(bus_num,device_num,function_num,0x09) ;
              result=result&0xff000000;
              if(result==0x02000000)                     //�ж϶�ȡ�Ƿ�Ϊ�����豸
              {    result= MyPCIRead(bus_num,device_num,function_num,0X0) ;  
                   DID=(result&0xffff0000)>>16;
                   VID=result&0x0000ffff; 
                  DEBUG((DEBUG_ERROR, "Hyl:bus_num:%x ", bus_num));
                  DEBUG((DEBUG_ERROR, "Hyl:device_num:%x ", device_num));
                  DEBUG((DEBUG_ERROR, "Hyl:function_num:%x ", function_num));                
                  DEBUG((DEBUG_ERROR, "Hyl:NetWork controller --Ethernet controller\n "));
                  InitString( HiiHandle,     //?????bus dev fuc ??setup????
                              STRING_TOKEN(STR_PCI_DEVICE_VALUE),
                              L"PCI BUS_NUM:%d DEVICE_NUM:%d FUNCTION_NUM:%d VID:%x DID:%x" ,bus_num,device_num,function_num,VID,DID);
                  return;
              }
              
              
                     
            }
           
      
         }
    InitString(HiiHandle,
		STRING_TOKEN(STR_PCI_DEVICE_VALUE),
		L"Not Found ");

    } 
*/



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
