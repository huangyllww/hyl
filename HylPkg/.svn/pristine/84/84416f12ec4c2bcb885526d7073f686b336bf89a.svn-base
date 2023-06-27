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
#include "MyEvent.h"
EFI_EVENT  MyTriggleEvent;
EFI_EVENT  ChangCmosEvent ;
VOID
EFIAPI
ChangCmos (  EFI_EVENT    Event,
             VOID    *Context)
{    
 
   IoWrite8(0x70,0x66);   
   IoWrite8(0x71,0x66);
   IoWrite8(0x70,0x66);
   DEBUG((DEBUG_ERROR, "Hyl:CMOS 0X66: %x", IoRead8(0x71)));    
 
}

VOID
EFIAPI
Mytrigger (  EFI_EVENT    Event,
             VOID    *Context)
{    
    EFI_STATUS                   Status;
    EFI_SMM_CONTROL2_PROTOCOL    *SmmControl;
    UINT8                        SmiDataValue;  
    DEBUG((DEBUG_ERROR, "Hyl:Mytrigger entry"));    
    Status = gBS->LocateProtocol (
                    &gEfiSmmControl2ProtocolGuid,
                    NULL,
                    (VOID **)&SmmControl
                    );
    
    DEBUG((DEBUG_ERROR, "Hyl:Status = SUCCESS")); 
    ASSERT_EFI_ERROR(Status);
    SmiDataValue=0X77;     //hyl SMI trigger
    if (!EFI_ERROR(Status)){
        SmmControl->Trigger (
                             SmmControl,
                             &SmiDataValue,
                             NULL,
                             0,
                             0
                             );
                }
    
    if (Event != NULL) {
        gBS->CloseEvent (Event);
      }
    DEBUG((DEBUG_ERROR, "Hyl:Mytrigger exit"));   
 
}

EFI_STATUS MyEventInit (
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{   
   // VOID    *gProtocolNotifyRegistration;
    //EFI_GUID    gDXEPCIreadProtocolGuid = EFI_SHELL_PROTOCOL_GUID;s
   // VOID *updateChangCmosEvent
 //   static EFI_EVENT  ChangCmosEvent = NULL;
    
    EFI_STATUS         Status;
    DEBUG((DEBUG_ERROR, "====================================\n" ));
    DEBUG((DEBUG_ERROR, "=============MyEventInit============\n" ));
    
    Status = gBS->CreateEvent( 
                                 EFI_EVENT_NOTIFY_SIGNAL,
                                 TPL_CALLBACK,
                                 ChangCmos,
                                 NULL,
                                 &ChangCmosEvent);

         ASSERT_EFI_ERROR(Status);     
         gBS->SignalEvent(ChangCmosEvent);
         gBS->CloseEvent(ChangCmosEvent);
         Status= gBS->CreateEventEx(
                        EVT_NOTIFY_SIGNAL, TPL_NOTIFY, 
                        Mytrigger,
                        NULL,
                        &gEfiEventReadyToBootGuid,
                        &MyTriggleEvent);
        
         ASSERT_EFI_ERROR(Status);    
         DEBUG((DEBUG_ERROR, "====================================\n" ));
         DEBUG((DEBUG_ERROR, "=============MyEventInit exit============\n" ));
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
