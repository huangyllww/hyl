#include <OemIpmiNetFnDefinitions.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/IPMITransportProtocol.h>

EFI_IPMI_TRANSPORT    *mIpmiTransportProtocol = NULL;

/**
     OEM IPMI command send Interface Implementation

    @param  IpmiCmd -  OEM command number, defined in OemIpmiNetFnDefinitions.h
    @param  DataBuffer - Data buffer to be used for SET&GET IPMI command, it MUST NOT to be NULL pointer.
    @param  BufferSize - Allocated buffer size, especial for GET command, we will check whether the DataBuffer have enough memory space.

    @return EFI_STATUS Status returned.

**/
EFI_STATUS
OemIpmiCmdSend(
    IN UINT8 	IpmiCmd,
	IN UINT8    *CommandData,
	IN UINT8    CommandDataSize,
	OUT UINT8   *ResponseData,
	OUT UINT8   *ResponseDataSize
    ) 
{
  EFI_STATUS  Status = EFI_UNSUPPORTED;
  BOOLEAN   IsGetCmd = FALSE;
  UINT8     RetryCounter = 0;
  
  DEBUG((EFI_D_ERROR, __FUNCTION__" Entered.  \n IpmiProtocol=0x%x \n", mIpmiTransportProtocol));
  
  if (CommandData == NULL) 
    return Status;
  
  if (mIpmiTransportProtocol == NULL) {
    Status = gBS->LocateProtocol (&gEfiDxeIpmiTransportProtocolGuid, NULL, &mIpmiTransportProtocol);
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_ERROR, "Locate IPMI protocol Status:%r\n", Status));
      return Status;
    }
  }
  
  //Need return data if it is IPMI GET command.
  switch (IpmiCmd) {
  case CMD_OEM_GET_MEM_ECC_THRESHOLD:
  case CMD_OEM_GET_MEM_ECC_INFO:
  case CMD_OEM_GET_FAN_POLICY:
  case CMD_OEM_GET_FAN_SPEED:
  case CMD_OEM_GET_MEM_MRC_INFO:
  case CMD_OEM_GET_MEM_MRC_ALL_INFO:
  case CMD_OEM_GET_POWER:
  case CMD_OEM_GET_AGGREGATETHERMAL_INFO:
  case CMD_OEM_GET_HOTPLUG_FLAG:
  case CMD_OEM_GET_BIOS_VERSION_INFO:
  case CMD_OEM_GET_POWER_STANDBY_MODE:
  case CMD_OEM_GET_SPEC_VERSION:
  case CMD_OEM_GET_SETUP_BOOT_OPTIONS:
  case CMD_OEM_GET_FW_BUILD_TIME:
  case CMD_OEM_GET_SERVER_REPOSITORY_INFO:
  case CMD_OEM_GET_MODULE_STATE:
  case CMD_OEM_GET_GPIO_PARAM:
  case CMD_OEM_GET_IOM_Type:
    IsGetCmd = TRUE;
    break;
    
  default:
    //return Status;
    break;
  }
  
  DEBUG((EFI_D_ERROR, "CommandDataSize: %d\n", CommandDataSize));
  if (IsGetCmd) {
	  if (ResponseData == NULL)
		  return EFI_INVALID_PARAMETER;
  }
  
  do {
    RetryCounter++;
    Status = mIpmiTransportProtocol->SendIpmiCommand (
        mIpmiTransportProtocol,
        NETFN_OEM,
        BMC_LUN,
        IpmiCmd,
        (UINT8 *)CommandData,
        CommandDataSize,
        ResponseData,
        ResponseDataSize
      );
    gBS->Stall(10*1000); //10msec delay
  } while(EFI_ERROR(Status) && (RetryCounter<3));

  DEBUG((EFI_D_ERROR, "IPMI CMD:0x%x, ResponseDataSize:%d, Status:%r RetryCounter:%d\n", IpmiCmd, *ResponseDataSize, Status, RetryCounter));
  DEBUG((EFI_D_ERROR, __FUNCTION__" Exit.\n"));
  return Status;
}
