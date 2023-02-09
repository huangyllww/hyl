/**
//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2017, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
*/

/** @file RasWrapperSetup.c
 * This file Contains Source for RasWrapper.
 * 
 *      This file can be used for updating variables for APCB.
 *      
 */
  #include <AmiLib.h>
  #include <AmiDxeLib.h>
  #include <Library/DebugLib.h>
  #include <Setup.h>
  #include <Library/ApcbLib.h>

EFI_GUID gSetupGuid = SETUP_GUID;

EFI_STATUS
RasWrapperSetupSavedConfigChanges (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS            Status;
  SETUP_DATA            SetupData;
  UINT32                SetupDataAttributes = 0;
  UINTN                 SetupDataSize = sizeof(SETUP_DATA);
  UINT32    			TokenSize = sizeof(UINT8);
  UINT64  				new_value = 0;
  
  InitAmiLib (ImageHandle, SystemTable);
  Status = pRS->GetVariable(L"Setup", &gSetupGuid, &SetupDataAttributes,
                                  &SetupDataSize, &SetupData);

  new_value = (UINT64) SetupData.CbsDbgMemRcdParityDdr4;
  AmdPspApcbSetCbsDbgParameter(APCB_TOKEN_CBS_DBG_MEM_RCD_PARITY_DDR4, &TokenSize, &new_value);  
  
  new_value = (UINT64) SetupData.CbsDbgMemAddrCmdParityRetryDdr4;
  AmdPspApcbSetCbsDbgParameter(APCB_TOKEN_CBS_DBG_MEM_ADDR_CMD_PARITY_RETRY_DDR4, &TokenSize, &new_value);  
    
  new_value = (UINT64) SetupData.CbsDbgMemAddrCmdParityErrorMaxReplayDdr4;
  AmdPspApcbSetCbsDbgParameter(APCB_TOKEN_CBS_DBG_MEM_ADDR_CMD_PARITY_ERROR_MAX_REPLAY_DDR4, &TokenSize, &new_value);  
    
  new_value = (UINT64) SetupData.CbsDbgMemCtrllerWrCrcEnDdr4;
  AmdPspApcbSetCbsDbgParameter(APCB_TOKEN_CBS_DBG_MEM_CTRLLER_WR_CRC_EN_DDR4, &TokenSize, &new_value);
  
  new_value = (UINT64) SetupData.CbsDbgMemWriteCrcRetryDdr4;
  AmdPspApcbSetCbsDbgParameter(APCB_TOKEN_CBS_DBG_MEM_WRITE_CRC_RETRY_DDR4, &TokenSize, &new_value);
	
  new_value = (UINT64) SetupData.CbsDbgMemWriteCrcErrorMaxReplayDdr4;
  AmdPspApcbSetCbsDbgParameter(APCB_TOKEN_CBS_DBG_MEM_WRITE_CRC_ERROR_MAX_REPLAY_DDR4, &TokenSize, &new_value);
   
  AmdPspWriteBackApcbShadowCopy();

  return Status;
}

/**
//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2017, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
*/
