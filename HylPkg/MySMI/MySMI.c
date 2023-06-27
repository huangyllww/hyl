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



#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <MyEvent/MyEvent.h>
EFI_STATUS
MySMIHandler (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context         OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  )
{
 
  DEBUG ((DEBUG_ERROR, "-------Hyl:MyEventInit Enter-------\n"));
  IoWrite8(0x70,0x46);   
  DEBUG ((DEBUG_ERROR, "Hyl:1Cmos offset 0x46: - %x\n", IoRead8(0x71)));
  
  IoWrite8(0x70,0X46); 
  IoWrite8(0x71,0xff);  
  IoWrite8(0x70,0X46); 
  DEBUG ((DEBUG_ERROR, "Hyl:2Cmos offset 0x46: - %x\n", IoRead8(0x71)));
  DEBUG ((DEBUG_ERROR, "-------Hyl:MySMIHandler EXIT-------\n"));
  return EFI_SUCCESS;
  }

EFI_STATUS MySMIInit(
	IN EFI_HANDLE ImageHandle,
	IN EFI_SYSTEM_TABLE *SystemTable
)
{     EFI_STATUS                   Status;
   //   EFI_SMM_CONTROL2_PROTOCOL    *SmmControl;
      EFI_SMM_SW_DISPATCH2_PROTOCOL *mySmmSwDispatch2 = NULL;
      EFI_SMM_SW_REGISTER_CONTEXT   SmmSwDispatchContext;
      EFI_HANDLE                    DispatchHandle;
    //  UINT8                        SmiDataValue;
      DEBUG ((DEBUG_ERROR, "-------Hyl:MySMIInit entry-------\n")); 
   //FCH_PROTOCOL_LIST  -> gEfiSmmSwDispatch2ProtocolGuid->value
      
      Status = gSmst->SmmLocateProtocol (
	                    &gEfiSmmSwDispatch2ProtocolGuid,
	                    NULL,
	                    (VOID**)&mySmmSwDispatch2
	                    );
      ASSERT_EFI_ERROR (Status);
      DEBUG ((DEBUG_ERROR, "-------Hyl:MySMIInit entry11-------\n")); 
      SmmSwDispatchContext.SwSmiInputValue = (UINTN)0x77;
      Status=mySmmSwDispatch2->Register (
	                             mySmmSwDispatch2,
	                             MySMIHandler,
	                             &SmmSwDispatchContext,
	                             &DispatchHandle
	                             );
	ASSERT_EFI_ERROR (Status);
	DEBUG ((DEBUG_ERROR, "-------Hyl:MySMIHandler Register 88  OK-------\n"));
	//IoWrite8 (0xB2, 0x77);
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
