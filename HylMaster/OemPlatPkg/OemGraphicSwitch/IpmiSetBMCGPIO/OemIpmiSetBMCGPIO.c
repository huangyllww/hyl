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

#include <Library/DebugLib.h>
#include <OemIpmiNetFnDefinitions.h>
#include <Ppi/IpmiTransportPpi.h>
EFI_STATUS EFIAPI OemSetBmcGpio(
    IN        EFI_PEI_FILE_HANDLE     FileHandle,
    IN  CONST EFI_PEI_SERVICES        **PeiServices
)
{
#if 0    
    EFI_STATUS                              Status;
    PEI_IPMI_TRANSPORT_PPI                  *IpmiTransportPpi;
    OEM_GPIO_SET_PARAMS                   SetGpioParams;
    UINT8                                   CommandSize;
    UINT8                                   ResponseData;
    UINT8                                   ResponseSize=0;
    
    return 0;
    
    Status = (*PeiServices)->LocatePpi (
                    PeiServices,
                    &gEfiPeiIpmiTransportPpiGuid,
                    0,
	                NULL,
	                (VOID **)&IpmiTransportPpi
	                );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, " gEfiPeiIpmiTransportPpiGuid Status: %r \n", Status));
    if (EFI_ERROR (Status)) {
        return Status;
    }
    
    CommandSize                   = sizeof(OEM_GPIO_SET_PARAMS);
    ResponseSize                  = 0x01;
    SetGpioParams.GpioNumber      = 63;
    SetGpioParams.GpioFunction    = 00;
    SetGpioParams.Data            = 00;//<xuyj1-20181101-Adjust function of Disable onBoard VGA  when detect offboard VGA +>
    
    Status = IpmiTransportPpi->SendIpmiCommand (
                      IpmiTransportPpi,
                      NETFN_OEM,
                      BMC_LUN,
                      CMD_OEM_SET_GPIO_PARAM,
                      (UINT8*)&SetGpioParams,
                      CommandSize,
                      &ResponseData,
                      &ResponseSize );
    
    if (EFI_ERROR (Status)) {
        return Status;
    }
    

    CommandSize                    = sizeof(OEM_GPIO_SET_PARAMS);
    ResponseSize                   = 0x01;
    SetGpioParams.GpioNumber       = 63;
    SetGpioParams.GpioFunction     = 01;
    SetGpioParams.Data             = 01;
    Status = IpmiTransportPpi->SendIpmiCommand (
                        IpmiTransportPpi,
                        NETFN_OEM,
                        BMC_LUN,
                        CMD_OEM_SET_GPIO_PARAM,
                        (UINT8*)&SetGpioParams,
                        CommandSize,
                        &ResponseData,
                        &ResponseSize );
    
    if (EFI_ERROR (Status)) {
        return Status;
    }
	return Status;
#endif
    return 0;
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
