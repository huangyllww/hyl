//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//*************************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//*************************************************************************
/** @file BoradID.c
    This file is used to get board id by GPIO.

    @note  MAKE SURE NO PEI OR DXE SPECIFIC CODE IS NEEDED

**/
//*************************************************************************

//----------------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------------

#include <Efi.h>
#include <Token.h>
//#include <Setup.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Library\Protocol\AmdCpmTableProtocol\AmdCpmTableProtocol.h>
#include <Include\AmdCpmFunction.h>
#include "BoardId.h"

// Produced Protocols

// Consumed Protocols

//----------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//----------------------------------------------------------------------------
// Constant Definition(s)

#define GPIO_SMN_BASE_ADDRESS     0x02D02500

// GPIO23-1         SKU
//    0x1         TC4600 E
//    0x0         TC4600 T


// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)

//----------------------------------------------------------------------------
// Variable and External Declaration(s)
//----------------------------------------------------------------------------
// Variable Declaration(s)

// GUID Definition(s)

// Protocol Definition(s)

// External Declaration(s)

// Function Definition(s)

//----------------------------------------------------------------------------

/**
    This function returns board ID value.
    @param BoardId return the board ID

    @retval EFI_STATUS
**/
EFI_STATUS
GetBoardId (
  OUT UINT8 *BoardId
)
{
    EFI_STATUS                          Status = EFI_SUCCESS;
    AMD_CPM_TABLE_PROTOCOL              *AmdCpmTableProtocolPtr;
    UINT8                               BoardIdGpioVal = 0; 
    EFI_GUID                            gAmdCpmTableProtocolGuid       = AMD_CPM_TABLE_PROTOCOL_GUID;

    Status = pBS->LocateProtocol ( &gAmdCpmTableProtocolGuid,
                                    NULL,
                                    (VOID**)&AmdCpmTableProtocolPtr
                                    );

    if (EFI_ERROR (Status)) {
      return Status;
    }
    //
    // Read Gpio to get the BoardId
    //
    BoardIdGpioVal = AmdCpmTableProtocolPtr->CommonFunction.SmnRead8 (AmdCpmTableProtocolPtr, 0, 1, GPIO_SMN_BASE_ADDRESS, ((23 << 2) + 2));//GPIO23_1

    BoardIdGpioVal = BoardIdGpioVal & 0x01;
    
    DEBUG ((DEBUG_ERROR, "BoardIdGpioVal = %x\n",BoardIdGpioVal));
    
    if(BoardIdGpioVal == 0x1)
        *BoardId = 1;
    else
        *BoardId = 0;
    
    DEBUG ((DEBUG_ERROR, "BoardId  = %x\n",*BoardId));
    
    return EFI_SUCCESS;

}
//<Kangmm-20220224-SGEZT22429 According BoardId to adjust CbsCpuSmtCtrl item +>
EFI_STATUS
BoardIdEntryPoint(
    IN EFI_HANDLE         ImageHandle,
    IN EFI_SYSTEM_TABLE   *SystemTable
    )
{
    EFI_GUID            mAmiTseOemPortingVar1Guid = OEM_PORT_VARIABEL1;
    OEM_NO_DEFAULT_VARIABLES      OEMSetupData;//,OEMSetupData;
    UINTN                         OEMSetupDataSize = sizeof(OEM_NO_DEFAULT_VARIABLES);
    EFI_STATUS          Status;
    UINT8               BoardId;
    UINTN                           VariableSize = sizeof(CBS_CONFIG);
    CBS_CONFIG                      CbsConfig;
    UINT32                          VariableAttribute;
    
    InitAmiLib(ImageHandle, SystemTable);

    Status = GetBoardId(&BoardId);
    
    OEMSetupDataSize = sizeof(OEM_NO_DEFAULT_VARIABLES);
    Status = pRS->GetVariable(L"OemPortVar1", &mAmiTseOemPortingVar1Guid, NULL,
                  &OEMSetupDataSize, &OEMSetupData);
    DEBUG ((DEBUG_ERROR, "OEMSetupData.BoardSku  = %x\n",OEMSetupData.BoardSku));
    DEBUG ((DEBUG_ERROR, "BoardId  = %x\n",BoardId));
    DEBUG ((DEBUG_ERROR, "Status  = %x\n",Status));
    if(!EFI_ERROR(Status))
    {
      if(BoardId != OEMSetupData.BoardSku)
      {
        Status = pRS->GetVariable (L"AmdSetup", &gCbsSystemConfigurationGuid, &VariableAttribute, &VariableSize, &CbsConfig);
        DEBUG ((DEBUG_ERROR, "AmdSetup Status  = %x\n",Status));
        if(!EFI_ERROR(Status))
        {
          if(BoardId == 1)
          {
            CbsConfig.CbsCpuSmtCtrl = 0;                                    // SMT Control
          }else{
            CbsConfig.CbsCpuSmtCtrl = 1;                                    // SMT Control
          }
        }
        Status = pRS->SetVariable(L"AmdSetup", &gCbsSystemConfigurationGuid,VariableAttribute,sizeof(CBS_CONFIG),&CbsConfig);
        OEMSetupData.BoardSku = BoardId;
        DEBUG ((DEBUG_ERROR, "OemSetupData.BoardSku  = %x\n",OEMSetupData.BoardSku));
        pRS->SetVariable( L"OemPortVar1", \
                          &mAmiTseOemPortingVar1Guid, \
                          EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,\
                          sizeof(OEM_NO_DEFAULT_VARIABLES), \
                          &OEMSetupData);
      }
    }else{
      OEMSetupData.BoardSku = BoardId;
      DEBUG ((DEBUG_ERROR, "OEMSetupData.BoardSku  = %x\n",OEMSetupData.BoardSku));
      Status = pRS->GetVariable (L"AmdSetup", &gCbsSystemConfigurationGuid, &VariableAttribute, &VariableSize, &CbsConfig);
      DEBUG ((DEBUG_ERROR, "AmdSetup Status  = %x\n",Status));
      if(!EFI_ERROR(Status))
      {
        if(BoardId == 1)
        {
          CbsConfig.CbsCpuSmtCtrl = 0;                                    // SMT Control
        }else{
          CbsConfig.CbsCpuSmtCtrl = 1;                                    // SMT Control
        }
      }
      Status = pRS->SetVariable(L"AmdSetup", &gCbsSystemConfigurationGuid,VariableAttribute,sizeof(CBS_CONFIG),&CbsConfig);
      pRS->SetVariable( L"OemPortVar1", \
                    &mAmiTseOemPortingVar1Guid, \
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,\
                    sizeof(OEM_NO_DEFAULT_VARIABLES), \
                    &OEMSetupData);
    }
    return EFI_SUCCESS;
}
//<Kangmm-20220224-SGEZT22429 According BoardId to adjust CbsCpuSmtCtrl item ->
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
