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
#include <PEICreatHob/MyHob.h>
#include "Token.h"
#include <Setup.h>
#include <Guid/HobList.h>
#include <Library/AmdCbsVariable.h>
#define CBS_SYSTEM_CONFIGURATION_NAME L"AmdSetup"
extern  EFI_RUNTIME_SERVICES *gRT;//PS-674
static EFI_GUID HobListGuid	= HOB_LIST_GUID;
EFI_STATUS DXEGetMyHob (
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{   UINT32                     Attributes;
    UINTN                      VarSize;
    CBS_CONFIG                 CbsConfig;
    EFI_STATUS          Status;
    SMP_HOB           *MyGetHob=NULL;
    EFI_GUID gCbsSystemConfigurationGuid = { 0x3A997502, 0x647A, 0x4c82, {0x99, 0x8E, 0x52, 0xEF, 0x94, 0x86, 0xA2, 0x47} };
    EFI_GUID           MyPEI_DXE_HobGuid = PEI_DXE_MYHOB_GUID;
    InitAmiLib(ImageHandle,SystemTable);
    MyGetHob=GetEfiConfigurationTable( SystemTable,&HobListGuid);
    //MyGetHob=  GetEfiConfigurationTable(SystemTable, &gEfiHobListGuid);
       if (MyGetHob!=NULL) {
           Status = FindNextHobByGuid(&MyPEI_DXE_HobGuid, (void **)&MyGetHob);
           if(!EFI_ERROR(Status)) { 
              // DEBUG((DEBUG_ERROR, "Hyl:SmpHob->SmpHob_U32 %x", MyGetHob->SmpHob_U32));
             //  DEBUG((DEBUG_ERROR, "Hyl:SmpHob->SmpHob_U32 %x", MyGetHob->SmpHob_U16));
               DEBUG((DEBUG_ERROR, "Hyl:SmpHob->MyBoardId %x\n", MyGetHob->MyBoardId));
           }
       }
  VarSize = sizeof(CBS_CONFIG);
  Status = gRT->GetVariable (
          CBS_SYSTEM_CONFIGURATION_NAME,
          &gCbsSystemConfigurationGuid,
          &Attributes,
          &VarSize,
          &CbsConfig
          ); 
  if (!EFI_ERROR(Status)) {
     if ((MyGetHob->MyBoardId) ==0x11)
     {       
        DEBUG((DEBUG_ERROR, "Hyl CbsConfig.CbsCmnGnbNbIOMMU=%x \n", CbsConfig.CbsCmnGnbNbIOMMU));//read CbsCmnGnbNbIOMMU
        CbsConfig.CbsCmnGnbNbIOMMU = 0;  //set IOMMU=0
        VarSize = sizeof(CBS_CONFIG);
        Status = gRT->SetVariable(
              CBS_SYSTEM_CONFIGURATION_NAME,
              &gCbsSystemConfigurationGuid,
              EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,  // Attributes,
              VarSize,
              &CbsConfig
              );
     }
     if(CbsConfig.CbsCmnGnbNbIOMMU==0 )
     {
         DEBUG((DEBUG_ERROR, "Hyl:Set IOMMU disabled\n" )); 
     }
   
  }                 
  DEBUG((DEBUG_ERROR, "Hyl:HOBEXIT\n" ));                
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
