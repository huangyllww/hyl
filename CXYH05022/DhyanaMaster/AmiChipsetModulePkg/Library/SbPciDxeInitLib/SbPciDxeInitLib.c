//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.         **
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
/** @file SbPciDxeInitLib.c
	Sb Pci library link to PciBus.
**/
//**********************************************************************

#include <AmiDxeLib.h>
#include <Token.h>
#include <PciBus.h>
#include <PciHostBridge.h>
#include <Setup.h>
#include <Protocol\PciRootBridgeIo.h>
#include <Protocol\PciIo.h>
#include <Protocol\AmiBoardInitPolicy.h>
#include <Library\AmiPciBusLib.h>

/**
    To skip device enumeration for HYGON Device 14h Function 0 (SMBus).
    The reason is whole the BAR of this device are unavailable.
    AMI PciBus will clear command register it will cause HYGON BTS test tools failure.
    (EIP178628 HyperTransport Failed)

    @param Device the device information.


    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
EFI_STATUS
HygonFchSkipDevice (
  IN PCI_DEV_INFO *Device
  )
{
	EFI_STATUS          			Status = EFI_UNSUPPORTED; //  Unsupported means don't skip it.
	PCI_DEV_INFO        			*dev = Device;
	UINT32							i;
	EFI_PCI_CONFIGURATION_ADDRESS	CONST SkipDevice[1] = {
			// porting : append more device if needed.
			//		.....
			// 	Register	Function	Device	Bus	ExtendedRegister
			{	0, 			0, 			0x14, 	0, 	0}		//SMBUS
//			{	0, 			6, 			0x14, 	0, 	0}		//SD
	};

	for (i = 0; i < sizeof(SkipDevice)/sizeof(PCI_CFG_ADDR); i++) {
		if (dev->Address.Addr.Bus == SkipDevice[i].Bus &&
				dev->Address.Addr.Device == SkipDevice[i].Device &&
				dev->Address.Addr.Function == SkipDevice[i].Function)
		{
			Status = EFI_SUCCESS; // To skip this device.
			break;
		}
	}

	return Status;
}

/**
    SB Pci init routine

    @param This 
    @param Function 
    @param ParameterBlock 

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
EFI_STATUS	CspSbPciInitRoutine (
		AMI_BOARD_INIT_PROTOCOL		*This,
		IN UINTN					*Function,
		IN OUT VOID					*ParameterBlock
)
{
//Update Standard parameter block
	AMI_BOARD_INIT_PARAMETER_BLOCK	*Args=(AMI_BOARD_INIT_PARAMETER_BLOCK*)ParameterBlock;
	PCI_INIT_STEP        			InitStep=(PCI_INIT_STEP)Args->InitStep;
    PCI_DEV_INFO   					*dev=(PCI_DEV_INFO*)Args->Param1;
    EFI_STATUS  					Status=EFI_UNSUPPORTED;

//---------------------------------
    //Check if parameters passed are VALID and
    if(Args->Signature != AMI_PCI_PARAM_SIG) return EFI_INVALID_PARAMETER;
    if(InitStep>=isPciMaxStep) return EFI_INVALID_PARAMETER;

    switch (InitStep)
    {
	//-------------------------------------------------------------------------
		case isPciGetSetupConfig:
			PCI_TRACE((TRACE_PCI," (isPciGetSetupConfig); " ));

		break;
	//-------------------------------------------------------------------------
		case isPciSkipDevice:
			PCI_TRACE((TRACE_PCI," (isPciSkipDevice); " ));

			Status = HygonFchSkipDevice(dev);

		break;
	//-------------------------------------------------------------------------
		case isPciSetAttributes:
			PCI_TRACE((TRACE_PCI," (isPciSetAttributes); " ));

		break;
	//-------------------------------------------------------------------------
		case isPciProgramDevice:
			PCI_TRACE((TRACE_PCI," (isPciProgramDevice); " ));

		break;
	//-------------------------------------------------------------------------
		case isPcieInitLink:
			PCI_TRACE((TRACE_PCI," (isPcieInitLink); " ));

		break;
	//-------------------------------------------------------------------------
		case isPcieSetAspm:
			PCI_TRACE((TRACE_PCI," (isPcieSetAspm); " ));

		break;
	//-------------------------------------------------------------------------
		case isPcieSetLinkSpeed:
			PCI_TRACE((TRACE_PCI," (isPcieSetLinkSpeed); " ));

		break;
	//-------------------------------------------------------------------------
		case isPciGetOptionRom:
			PCI_TRACE((TRACE_PCI," (isPciGetOptionRom); " ));

		break;
	//-------------------------------------------------------------------------
		case isPciOutOfResourcesCheck:
			PCI_TRACE((TRACE_PCI," (isPciOutOfResourcesCheck); " ));

		break;
	//-------------------------------------------------------------------------
		case isPciReadyToBoot:
			PCI_TRACE((TRACE_PCI," (isPciReadyToBoot); " ));

		break;
	//-------------------------------------------------------------------------
		case isPciQueryDevice:
			PCI_TRACE((TRACE_PCI," (isPciQueryDevice); " ));

		break;
	//-------------------------------------------------------------------------
		case isHbBasicInit:
			PCI_TRACE((TRACE_PCI," (isHbBasicInit); " ));

		break;
	//-------------------------------------------------------------------------
		case isRbCheckPresence:
			PCI_TRACE((TRACE_PCI," (isRbCheckPresence); " ));

		break;
	//-------------------------------------------------------------------------
		case isRbBusUpdate:
			PCI_TRACE((TRACE_PCI," (isRbBusUpdate); " ));

		break;
	//-------------------------------------------------------------------------
		default:
			Status=EFI_INVALID_PARAMETER;
			PCI_TRACE((TRACE_PCI," (!!!isPciMaxStep!!!); " ));
    }//switch

    return Status;
}

EFI_STATUS	SD_Init (
		AMI_BOARD_INIT_PROTOCOL		*This,
		IN UINTN					*Function,
		IN OUT VOID					*ParameterBlock
)
{
//Update Standard parameter block
	AMI_BOARD_INIT_PARAMETER_BLOCK	*Args=(AMI_BOARD_INIT_PARAMETER_BLOCK*)ParameterBlock;
	PCI_INIT_STEP        			InitStep=(PCI_INIT_STEP)Args->InitStep;
    PCI_DEV_INFO   					*dev=(PCI_DEV_INFO*)Args->Param1;
    EFI_STATUS  					Status=EFI_UNSUPPORTED;

	UINT64		Address;
	UINT32      Data;

//---------------------------------
    //Check if parameters passed are VALID and
    if(Args->Signature != AMI_PCI_PARAM_SIG) return EFI_INVALID_PARAMETER;
    if(InitStep>=isPciMaxStep) return EFI_INVALID_PARAMETER;

    switch (InitStep)
    {
	//-------------------------------------------------------------------------
		case isPciGetSetupConfig:
			PCI_TRACE((TRACE_PCI," (isPciGetSetupConfig); " ));

		break;
	//-------------------------------------------------------------------------
		case isPciSkipDevice:
			PCI_TRACE((TRACE_PCI," (isPciSkipDevice); " ));
			PCI_TRACE((TRACE_PCI,"\n Hide SD\n " ));
			//Hide - If already hidden by CBS won't even get here.
//			Clear 0xfed8_1e72[3] will make SD controller invisible for Die0. 
			Address=0xFED81E72;
			Data=MmioRead32(Address);
			Data=Data&0xFFFFFFF7;
			MmioWrite32(Address,Data );
			//Skip
			Status = EFI_SUCCESS; // To skip this device.

		break;
	//-------------------------------------------------------------------------
		case isPciSetAttributes:
			PCI_TRACE((TRACE_PCI," (isPciSetAttributes); " ));

		break;
	//-------------------------------------------------------------------------
		case isPciProgramDevice:
			PCI_TRACE((TRACE_PCI," (isPciProgramDevice); " ));

		break;
	//-------------------------------------------------------------------------
		case isPcieInitLink:
			PCI_TRACE((TRACE_PCI," (isPcieInitLink); " ));

		break;
	//-------------------------------------------------------------------------
		case isPcieSetAspm:
			PCI_TRACE((TRACE_PCI," (isPcieSetAspm); " ));

		break;
	//-------------------------------------------------------------------------
		case isPcieSetLinkSpeed:
			PCI_TRACE((TRACE_PCI," (isPcieSetLinkSpeed); " ));

		break;
	//-------------------------------------------------------------------------
		case isPciGetOptionRom:
			PCI_TRACE((TRACE_PCI," (isPciGetOptionRom); " ));

		break;
	//-------------------------------------------------------------------------
		case isPciOutOfResourcesCheck:
			PCI_TRACE((TRACE_PCI," (isPciOutOfResourcesCheck); " ));

		break;
	//-------------------------------------------------------------------------
		case isPciReadyToBoot:
			PCI_TRACE((TRACE_PCI," (isPciReadyToBoot); " ));

		break;
	//-------------------------------------------------------------------------
		case isPciQueryDevice:
			PCI_TRACE((TRACE_PCI," (isPciQueryDevice); " ));

		break;
	//-------------------------------------------------------------------------
		case isHbBasicInit:
			PCI_TRACE((TRACE_PCI," (isHbBasicInit); " ));

		break;
	//-------------------------------------------------------------------------
		case isRbCheckPresence:
			PCI_TRACE((TRACE_PCI," (isRbCheckPresence); " ));

		break;
	//-------------------------------------------------------------------------
		case isRbBusUpdate:
			PCI_TRACE((TRACE_PCI," (isRbBusUpdate); " ));

		break;
	//-------------------------------------------------------------------------
		default:
			Status=EFI_INVALID_PARAMETER;
			PCI_TRACE((TRACE_PCI," (!!!isPciMaxStep!!!); " ));
    }//switch

    return Status;
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
