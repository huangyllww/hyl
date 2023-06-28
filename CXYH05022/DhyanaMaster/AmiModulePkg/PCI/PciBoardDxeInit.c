//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
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
// $Header:  $
//
// $Revision: $
//
// $Date: $
//**********************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  PciInitPolicy.c
//
// Description:	
//
//<AMI_FHDR_END>
//**********************************************************************

#include <AmiDxeLib.h>
#include <Token.h>
#include <PciBus.h>
#include <PciHostBridge.h>
#include <Setup.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PciIo.h>
#include <Protocol/AmiBoardInitPolicy.h>
#include <Protocol/FabricResourceManagerServicesProtocol.h>
#include <Library/PciAccessCspLib.h>
#include <Library/AmiPciBusLib.h>
#include <Library/PciLib.h>
#include <AmiPciBusSetupOverrideLib.h>
#include <Library/AmdSocBaseLib.h>
#include <Library/AmiSdlLib.h>
#include <Protocol/S3SaveState.h>
#include "OemPCIeDev.h"
PCI_DEV_NINFO Skip_VGAController_DB[] = {
  { 0x1002,  0x66A1,  0xFFFF,  Speed_Undefined,  L"AMD RADEON INSTINCT MI60"                   },  // 0x1002   AMD GPU Card
  { 0x1d94,  0x51b7,  0xFFFF,  Speed_Undefined,  L"Hygon DCU Z100S"                            },
  { 0x1d94,  0x54b7,  0xFFFF,  Speed_Undefined,  L"Hygon DCU Z100"                             },
  { 0x1d94,  0x55b7,  0xFFFF,  Speed_Undefined,  L"Hygon DCU Z100L"                            },
  { 0xFFFF,  0xFFFF,  0xFFFF,  Speed_Undefined,  L"New Device"                                 },
};
typedef struct _VGA_ENABLE_REGISTER
{
    union {
        struct {
            UINT32  VgaEnable   :1;  // [0] VE: VGA enable
            UINT32  NonPosted   :1;  // [1] NP: non-posted
            UINT32  CpuDis      :1;  // [2] CpuDis: CPU Disable
            UINT32  Reserved    :1;  // [3] Reserved
            UINT32  DstFabricID :8;  // [11:4] Destination Fabric ID
            UINT32  Reserved2   :20;  // [31:11] Reserved
        } Field;
        UINT32  AllBits;
    };
} VGA_ENABLE_REGISTER;

#define NB_SMN_INDEX_EXT_3_BIOS  0x00C0
#define NB_SMN_INDEX_3_BIOS  0x00C4
#define NB_SMN_DATA_3_BIOS   0x00C8

EFI_S3_SAVE_STATE_PROTOCOL          *gS3SaveState = NULL;


EFI_STATUS LocateS3SaveProtocol()
{
    EFI_STATUS Status = EFI_SUCCESS;
    if(gS3SaveState==NULL){
        Status = pBS->LocateProtocol(&gEfiS3SaveStateProtocolGuid, NULL, (VOID**)&gS3SaveState);
        if(EFI_ERROR(Status)) {
            PCI_TRACE((TRACE_PCI,"PciBoardDxeInit: Unable to find EfiBootScriptSave Protocol! Status=%r\n", Status));
            gS3SaveState=NULL;
        }
    }
    return Status;
}

VOID
NbSmnRead2 (
  IN       UINT32              DieNum,
  IN       UINT32              Address,
  IN	   UINT32	       *Value
  )
{
	UINT32 read_value=0;
	
	PciWrite32(NB_SMN_INDEX_EXT_3_BIOS, DieNum);
	PciWrite32(NB_SMN_INDEX_3_BIOS, Address);
	read_value=PciRead32(NB_SMN_DATA_3_BIOS);
	*Value=read_value;
	
	//clear in case other functions don't pay attention to this reg
	PciWrite32(NB_SMN_INDEX_EXT_3_BIOS, 0);
			              	 
}

//-------------------------------------------------------------------------
//!!!!!!!!!!! PORTING REQUIRED !!!!!!!!!!! PORTING REQUIRED !!!!!!!!!!!*
//!!!!!!!!!!!!!!!! must be maintained for PCI devices!!!!!!!!!!!!!!!!!!*
//-------------------------------------------------------------------------

/*
//TEMPLATE PCI DEVICE INIT FUNCTION START
// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: DeviceXXXX_Init
//
// Description:
//  This function provide each initial routine in genericsio.c
//
// Input:
//  IN  AMI_SIO_PROTOCOL    *AmiSio - Logical Device's information
//  IN  EFI_PCI_IO_PROTOCOL *PciIo - Read/Write PCI config space
//  IN  SIO_INIT_STEP       InitStep - Initial routine step
//
// Output:
//  EFI_SUCCESS - Initial step sucessfully
//  EFI_INVALID_PARAMETER - not find the initial step
//
// Modified:    Nothing
//
// Referrals:   None
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS PciDevXXX_Init(
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
    }//switch

    return Status;
}
//TEMPLATE PCI DEVICE INIT FUNCTION END
*/


//Some Example functions:
// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: DeviceXXXX_Init
//
// Description:
//  This function provide each initial routine in genericsio.c
//
// Input:
//  IN  AMI_SIO_PROTOCOL    *AmiSio - Logical Device's information
//  IN  EFI_PCI_IO_PROTOCOL *PciIo - Read/Write PCI config space
//  IN  SIO_INIT_STEP       InitStep - Initial routine step
//
// Output:
//  EFI_SUCCESS - Initial step successfully
//  EFI_INVALID_PARAMETER - not find the initial step
//
// Modified:    Nothing
//
// Referrals:   None
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS PcieRootPort4_Init(
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
			{
				//Like to set setup settings for ASPM to AUTO; Default was Disabled
				//dev->PciExpress->Pcie1Setup.AspmMode=0x55;
				//Status=EFI_SUCCESS;
			}
		break;
	//-------------------------------------------------------------------------
		case isPciSkipDevice:
			PCI_TRACE((TRACE_PCI," (isPciSkipDevice); " ));

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
    }//switch

    return Status;
}

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: DeviceXXXX_Init
//
// Description:
//  This function provide each initial routine in genericsio.c
//
// Input:
//  IN  AMI_SIO_PROTOCOL    *AmiSio - Logical Device's information
//  IN  EFI_PCI_IO_PROTOCOL *PciIo - Read/Write PCI config space
//  IN  SIO_INIT_STEP       InitStep - Initial routine step
//
// Output:
//  EFI_SUCCESS - Initial step sucessfully
//  EFI_INVALID_PARAMETER - not find the initial step
//
// Modified:    Nothing
//
// Referrals:   None
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS PcieRootPort6_Init(
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
		case isPcieSetAspm:
/*			PCI_TRACE((TRACE_PCI," (isPcieSetAspm); " ));
			{
				PCI_DEV_INFO *dn=(PCI_DEV_INFO*)Args->Param1;
				PCI_DEV_INFO *up=(PCI_DEV_INFO*)Args->Param2;
				UINT16		 *CalcAspm=(UINT16*)Args->Param3;
			//---------------------------------------
				//Override ASPM Settings for the link.....
				*CalcAspm=PCIE_ASPM_L1_ENABLE;
				Status=EFI_SUCCESS;
			}
*/			
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
    }//switch

    return Status;
}

#if 0
EFI_STATUS RootBrgInit(
AMI_BOARD_INIT_PROTOCOL		*This,
IN UINTN					*Function,
IN OUT VOID					*ParameterBlock
)
{
//Update Standard parameter block
	AMI_BOARD_INIT_PARAMETER_BLOCK	*Args=(AMI_BOARD_INIT_PARAMETER_BLOCK*)ParameterBlock;
	PCI_INIT_STEP        			InitStep=(PCI_INIT_STEP)Args->InitStep;
	PCI_ROOT_BRG_DATA   			*dev=(PCI_ROOT_BRG_DATA*)Args->Param1;
	PCI_ROOT_BRG_DATA   			*node0=dev->Owner->RootBridges[0];
	ASLR_QWORD_ASD                    *res = (ASLR_QWORD_ASD*)Args->Param2;
        EFI_STATUS  					Status=EFI_UNSUPPORTED;
        UINT32          temp ;
        EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL     *RtBrdg;
        RtBrdg = &node0->RbIoProtocol;
        //ASLR_QWORD_ASD	*res=(ASLR_QWORD_ASD*)Args->Param2;
        //ASLR_QWORD_ASD      		*busrd;
        
//---------------------------------
        //Temp skip to test Linux ACPI handling of bus _CRS
        //return Status;
        
    //Skip RBs that aren't there
    switch (InitStep)
        {
    	//-------------------------------------------------------------------------
    		case isRbCheckPresence:
    			PCI_TRACE((TRACE_PCI,"\n (isRbCheckPresence\n); " ));
    			
    			//For now this function only supports 2 combinations (2 socket, 4 die SP3) & (1 socket, 4 die SP3)
    			//Update later to support all POR combinations.
    			
    			PCI_TRACE((TRACE_PCI, "Bus = %x, Device = %x, Function = %x\n", dev->RbSdlData->Bus,
    						    dev->RbSdlData->Device,
    						    dev->RbSdlData->Function));
    				//assume Die0= Bus0 exists
              			//change to look ASL name for better match
    				if(dev->RbSdlData->Bus != 0)
    				{
    					temp=0;
					PciWrite32(0x000000C0, temp);
					//Use direct pci access instead of RBIO protocol which won't work for 1st RB yet
					temp=0x1C604;
					PciWrite32(0x000000C4, temp);
					temp=PciRead32(0x000000C8);
		//			RtBrdg->Pci.Write(RtBrdg, EfiPciWidthUint32, EFI_PCI_ADDRESS (0, 0, 0, 0xC0), 1, &temp);
		//			RtBrdg->Pci.Write(RtBrdg, EfiPciWidthUint32, EFI_PCI_ADDRESS (0, 0, 0, 0xC4), 1, &temp);
		//			RtBrdg->Pci.Read(RtBrdg, EfiPciWidthUint32, EFI_PCI_ADDRESS (0, 0, 0, 0xC8), 1, &temp);
					temp = temp & 0xF; 
					PCI_TRACE((TRACE_PCI, "DIE Count = %x\n",temp));    		
					
					//This is the combination support code. Simple for bringup.
					if(dev->RbSdlData->Bus > temp-1)
					{
						dev->NotPresent = TRUE; //indicates not present. 
						TRACE ((TRACE_ALWAYS, "Root not present.\n"));    			                        
					}

					//calculate bus size based on # of DIE
    					/*			
    					RtBrdg->Pci.Read(RtBrdg, EfiPciWidthUint32, 
    			                                               EFI_PCI_ADDRESS (dev->RbSdlData->Bus, dev->RbSdlData->Device, 
    			                                        		       dev->RbSdlData->Function, 0),
    			                                       1, &temp);
    			                    if(temp != 0x14501022) {
    			                        TRACE ((TRACE_ALWAYS, "Root not present, DevVenId = %x\n",temp));    			                        
      			                        dev->NotPresent = TRUE; //indicates not present. 
      			                        //Test also changing non-existent bus # to 255
      			                        //not sure if necessary
        			                  dev->RbSdlData->Bus=0xFF;
    			                    }*/
    				}
    			                      			
    			Status= EFI_SUCCESS;
    		break;
    		case isRbBusUpdate:
    			//Check system die count to determine bus mapping
    			
    			
                    	
			PCI_TRACE((TRACE_PCI,"\n (isRbBusUpdate)\n" ));
			temp=0;
                  	PciWrite32(0x000000C0, temp);
                  	//Use direct pci access instead of RBIO protocol which won't work for 1st RB yet
			temp=0x1C604;
                  	PciWrite32(0x000000C4, temp);
                  	temp=PciRead32(0x000000C8);
//			RtBrdg->Pci.Write(RtBrdg, EfiPciWidthUint32, EFI_PCI_ADDRESS (0, 0, 0, 0xC0), 1, &temp);
//			RtBrdg->Pci.Write(RtBrdg, EfiPciWidthUint32, EFI_PCI_ADDRESS (0, 0, 0, 0xC4), 1, &temp);
//			RtBrdg->Pci.Read(RtBrdg, EfiPciWidthUint32, EFI_PCI_ADDRESS (0, 0, 0, 0xC8), 1, &temp);
                  	temp = temp & 0xF; 
			PCI_TRACE((TRACE_PCI, "DIE Count = %x\n",temp));    		
			
			//calculate bus size based on # of DIE
			
			temp = 256/temp;
			PCI_TRACE((TRACE_PCI, "Calculated width = %x\n",temp));  
			//Need to adjust MIN also.

			
			  
			  //{ volatile unsigned long __iii; __iii = 1; while (__iii); }

			//adjust ASLR_QWORD_ASD
			res->_MIN = dev->RbSdlData->Bus * temp;  //instance count (bld time) * calculated width
			res->_MAX = res->_MIN + temp - 1;
			res->_LEN = (res->_MAX - res->_MIN) + 1;	
			
			//adjust AMI_SDL_PCI_DEV_INFO here
			PCI_TRACE((TRACE_PCI, "BEFORE Bus = %x, Device = %x, Function = %x\n", dev->RbSdlData->Bus,
			    						    dev->RbSdlData->Device,
			    						    dev->RbSdlData->Function));
			
			dev->RbSdlData->Bus = (UINT8) res->_MIN;
			
			PCI_TRACE((TRACE_PCI, "AFTER Bus = %x, Device = %x, Function = %x\n", dev->RbSdlData->Bus,
			    						    dev->RbSdlData->Device,
			    						    dev->RbSdlData->Function));


			//adjust PCI_ROOT_BRIDGE_MAPPING?
			
			//adjust ACPI_RES_DATA?
			
			//
			
			Status= EFI_SUCCESS;
		break;
    	
        }//switch
    //---------------------------------

    
//---------------------------------
    return Status;
}
#endif

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: DeviceXXXX_Init
//
// Description:
//  This function provide each initial routine in genericsio.c
//
// Input:
//  IN  AMI_SIO_PROTOCOL    *AmiSio - Logical Device's information
//  IN  EFI_PCI_IO_PROTOCOL *PciIo - Read/Write PCI config space
//  IN  SIO_INIT_STEP       InitStep - Initial routine step
//
// Output:
//  EFI_SUCCESS - Initial step sucessfully
//  EFI_INVALID_PARAMETER - not find the initial step
//
// Modified:    Nothing
//
// Referrals:   None
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS OnBoardVgaInit(
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

    //INTEL IGD GOP driver fails if it uses address above 4G.
    //So force IGD to use only Below 4G Address Space...
    if(InitStep==isPciGetSetupConfig){
//    	dev->DevSetup.Decode4gDisable=TRUE;
//    	Status=EFI_SUCCESS;
    }
    
    return Status;	
    
}
 
VOID VgaRoute(
IN PCI_DEV_INFO		*dev
)
{
	UINT8	OnBoard;
	EFI_STATUS Status;
	FABRIC_RESOURCE_MANAGER_PROTOCOL *FabricResourceManager;
	FABRIC_TARGET   FabricTarget;
	
	PCI_TRACE((TRACE_PCI, "VgaRoute: B/D/F = %02X/%X/%d\n", dev->Address.Addr.Bus,dev->Address.Addr.Device,dev->Address.Addr.Function));
	
    Status = pBS->LocateProtocol(
            &gAmdFabricResourceManagerServicesProtocolGuid,
            NULL,
            &FabricResourceManager);
    if(EFI_ERROR(Status)) {
        PCI_TRACE ((TRACE_PCI, "VgaRoute : Get FabricResourceManager protocol fail (%r) \n", Status));
        return;
    }	
    
	PCI_TRACE((TRACE_PCI,"Limiting %x:%x:%x Gfx device to 32bit MMIO.\n",dev->Address.Addr.Bus,dev->Address.Addr.Device,dev->Address.Addr.Function));
	
	dev->DevSetup.Decode4gDisable=TRUE;

	FabricTarget.TgtType = TARGET_PCI_BUS;
	FabricTarget.SocketNum = 0;
	FabricTarget.DieNum = 0;
	FabricTarget.PciBusNum = dev->Address.Addr.Bus; //dev->AmiSdlPciDevData->Bus;
    Status = FabricResourceManager->FabricEnableVgaMmio (FabricResourceManager, FabricTarget);
    PCI_TRACE ((TRACE_PCI, "VgaRoute : Enable Fabric Vga status = %r \n", Status));

	if(dev->Attrib == EFI_PCI_IO_ATTRIBUTE_EMBEDDED_DEVICE)
	{
		OnBoard = 1;
	} else {
		OnBoard = 0;
	}
	
	PcdSet32 (PcdLegacyVgaBDF, PCI_LIB_ADDRESS(dev->Address.Addr.Bus,dev->Address.Addr.Device,dev->Address.Addr.Function,OnBoard));
	return;
}

VOID FindSlot (
PCI_DEV_INFO	*dev,
UINT32		*Slot
)
{	
	if (dev->SdlMatchFound != 1)
	{
		FindSlot(dev->ParentBrg, Slot);
	} else {
		*Slot = dev->AmiSdlPciDevData->Slot;	
	}	
}
BOOLEAN
FilterVgaDevice(PCI_DEV_INFO *VGADevice)
{
	UINT8					iSkip_VGAController_DB=0;

	for (iSkip_VGAController_DB = 0; Skip_VGAController_DB[iSkip_VGAController_DB].VendorId != 0xFFFF; iSkip_VGAController_DB++)
		if((Skip_VGAController_DB[iSkip_VGAController_DB].VendorId == VGADevice->DevVenId.VenId) &&\
			(Skip_VGAController_DB[iSkip_VGAController_DB].DeviceId == VGADevice->DevVenId.DevId))
			return FALSE;
	return TRUE;
}
// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: DeviceXXXX_Init
//
// Description:
//  This function provide each initial routine in genericsio.c
//
// Input:
//  IN  AMI_SIO_PROTOCOL    *AmiSio - Logical Device's information
//  IN  EFI_PCI_IO_PROTOCOL *PciIo - Read/Write PCI config space
//  IN  SIO_INIT_STEP       InitStep - Initial routine step
//
// Output:
//  EFI_SUCCESS - Initial step sucessfully
//  EFI_INVALID_PARAMETER - not find the initial step
//
// Modified:    Nothing
//
// Referrals:   None
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS PciDevXXX_Init(
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
    
#if ( defined(CSM_SUPPORT) && (CSM_SUPPORT != 0) )
    
    SETUP_DATA  SetupData;
    UINTN                       BufferSize;
    EFI_GUID    EfiSetupGuid = SETUP_GUID;
    
	UINT32	Slot; 
	UINT32	LegacyVgaBDF;
#endif        
	PCI_CFG_ADDR        addr;

//---------------------------------
#if 1
	EFI_PHYSICAL_ADDRESS            DsdtAddress;
	ACPI_HDR                        *Dsdt;
    AMI_SDL_PCI_DEV_INFO            **SdlBuffer;
    AMI_SDL_PCI_DEV_INFO            *SdlDev;
    UINTN                           SdlCount;
    UINTN                           SdlIndex;
    UINTN                           SdlChildIndex;
    
    Dsdt = NULL;
    Status = LibGetDsdt(&DsdtAddress, EFI_ACPI_TABLE_VERSION_ALL);
    if (!EFI_ERROR(Status)) {
        Dsdt = (ACPI_HDR*)DsdtAddress;
    }
    Status = LocateS3SaveProtocol();
    
    if (dev->Type == tPci2PciBrg && dev->PciExpress->PcieCap.PortType == PCIE_TYPE_ROOTPORT && dev->PciExpress->PcieCap.SlotImpl) {
        Status = AmiSdlFindRecordIndex(dev->AmiSdlPciDevData, &SdlIndex);
        if (!EFI_ERROR(Status)) {
            SdlBuffer = NULL;
            Status = AmiSdlPciGetChildsOfParentIndex(&SdlBuffer, &SdlCount, SdlIndex);
            if (!EFI_ERROR(Status)) {
                for (SdlChildIndex = 0; SdlChildIndex < SdlCount; SdlChildIndex++) {
                    SdlDev = SdlBuffer[SdlChildIndex];
                    if (SdlDev->Slot != 0) {
                        // Update PhisSlotNum
                        addr.ADDR = dev->Address.ADDR;
                        addr.Addr.Register = dev->PciExpress->PcieOffs+PCIE_SLT_CAP_OFFSET;
                        dev->PciExpress->SlotCap.PhisSlotNum = SdlDev->Slot;
                        Status = PciCfg32(dev->RbIo, addr, TRUE, &dev->PciExpress->SlotCap.SLT_CAP);      
                        if(gS3SaveState != NULL) {
                            BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(
                                    gS3SaveState,           //This
                                    EfiBootScriptWidthUint32,   //Width
                                    addr.ADDR,1,&dev->PciExpress->SlotCap.SLT_CAP);            //Address, Data
                        } else {
                            PCI_TRACE((TRACE_PCI,"ERROR: gS3SaveState invalid \n"));
                        }
                        PCI_TRACE((TRACE_PCI," (Slot %d) ", SdlDev->Slot ));
                        if (Dsdt != NULL) {
                            Status = UpdateAslNameOfDevice(Dsdt, SdlDev->AslName, "_SUN", (UINT64)SdlDev->Slot);
                        }
                        break;
                    }
                }
                if (SdlChildIndex == SdlCount) {
                    // Clear SlotImplement bit
                    addr.ADDR = dev->Address.ADDR;
                    addr.Addr.Register = dev->PciExpress->PcieOffs+PCIE_CAP_OFFSET;
                    Status=PciCfg16(dev->RbIo,addr,FALSE,&dev->PciExpress->PcieCap.PCIE_CAP);          
                    dev->PciExpress->PcieCap.SlotImpl = 0;
                    Status=PciCfg16(dev->RbIo,addr,TRUE,&dev->PciExpress->PcieCap.PCIE_CAP);          
                    PCI_TRACE((TRACE_PCI," (Onboard) "));
                    if(gS3SaveState != NULL) {
                        BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(
                                gS3SaveState,           //This
                                EfiBootScriptWidthUint16,   //Width
                                addr.ADDR,1,&dev->PciExpress->PcieCap.PCIE_CAP);            //Address, Data
                    } else {
                        PCI_TRACE((TRACE_PCI,"ERROR: gS3SaveState invalid \n"));
                    }
                }
                pBS->FreePool(SdlBuffer);
            }
        }
    }
#else
    Status = LocateS3SaveProtocol();
    if (dev->ParentBrg->Type == tPci2PciBrg || dev->ParentBrg->Type == tPci2CrdBrg) {
	    if (dev->ParentBrg->PciExpress->PcieCap.SlotImpl && (dev->ParentBrg->PciExpress->SlotCap.PhisSlotNum != dev->AmiSdlPciDevData->Slot)) {
		    addr.ADDR = dev->ParentBrg->Address.ADDR;
		    addr.Addr.Register = dev->ParentBrg->PciExpress->PcieOffs+PCIE_SLT_CAP_OFFSET;
		    dev->ParentBrg->PciExpress->SlotCap.PhisSlotNum = dev->AmiSdlPciDevData->Slot;
		    Status=PciCfg32(dev->RbIo,addr,TRUE,&dev->ParentBrg->PciExpress->SlotCap.SLT_CAP);		
		    if(gS3SaveState != NULL) {
		        BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(
		                gS3SaveState,           //This
		                EfiBootScriptWidthUint32,   //Width
		                addr.ADDR,1,&dev->PciExpress->SlotCap.SLT_CAP);            //Address, Data
		    } else {
		        PCI_TRACE((TRACE_PCI,"ERROR: gS3SaveState invalid \n"));
		    }
	    }
    }
#endif
    //Check if parameters passed are VALID and
   if(Args->Signature != AMI_PCI_PARAM_SIG) return EFI_INVALID_PARAMETER;
   if(InitStep>=isPciMaxStep) return EFI_INVALID_PARAMETER;
    switch (InitStep)
    {
	//-------------------------------------------------------------------------
		case isPciGetSetupConfig:
			PCI_TRACE((TRACE_PCI," (isPciGetSetupConfig); " ));
										
			// If CSM is enabled && CSM video is set to legacy then limit to 32bit MMIO.
			// Ideally only 1st gfx device found with legacy opt rom would have this done.
			
#if ( defined(CSM_SUPPORT) && (CSM_SUPPORT != 0) )    
			BufferSize = sizeof(SETUP_DATA);
			pRS->GetVariable(L"Setup", &EfiSetupGuid, NULL, &BufferSize, &SetupData);
									 
			if (SetupData.VideoOpRom == 2 && SetupData.CsmSupport == 1) {   
				 if (((dev->Class.BaseClassCode == 0x03 && dev->Class.SubClassCode == 0x00) ||
					(dev->Class.BaseClassCode == 0x03 && dev->Class.SubClassCode == 0x80) ||
					(dev->Class.BaseClassCode == 0x04 && dev->Class.SubClassCode == 0x00)) &&\
					FilterVgaDevice(dev))//Mizl-20200610- Some GPU's ClassCode is 0380, Those Devices Should Be Ignored >
				  {
						DEBUG((EFI_D_ERROR, "[%a][%d]\n\n",__FUNCTION__,__LINE__));
	
						 	
						FindSlot(dev, &Slot);						
													
						LegacyVgaBDF = PcdGet32 (PcdLegacyVgaBDF);
						if(LegacyVgaBDF == 0)
						{
							VgaRoute(dev);	  
						} else {							
								//If current device is onboard
								if(dev->Attrib == EFI_PCI_IO_ATTRIBUTE_EMBEDDED_DEVICE)
								{
									//Only situation where onboard gets priority over existing device if SETUP->Onboard and existing is EXT
									if((SetupData.OnExtVgaSelect == 1) && ((LegacyVgaBDF & 1) == 0))
									{
										VgaRoute(dev);
									}
								} else {
								//If current device is external.
									//if AUTO and existing device is INT then replace
									if((SetupData.OnExtVgaSelect == 0) && ((LegacyVgaBDF & 1) == 1))
									{
										VgaRoute(dev);
									}
									if(SetupData.OnExtVgaSelect == 2)
									{
										//if(((LegacyVgaBDF & 1) == 1) || (((LegacyVgaBDF & 1) == 0) && (Slot == SetupData.VgaSlotNum)))
										if(Slot == SetupData.VgaSlotNum)
										{
											VgaRoute(dev);
										}
									}									
								}							
						} // else existing device
				  } //end if VGA device
			}// end if legacy video in CSM

#endif
			// APTIOV_SERVER_OVERRIDE_START : AmiPciBus Setup Override changes.
            if (dev->PciExpress != NULL) {
               if (dev->PciExpress->Pcie2 != NULL) {
                   Status = AmiPciGetPcie2SetupDataOverrideHook (&dev->PciExpress->Pcie2->Pcie2Setup, dev->AmiSdlPciDevData, dev->SdlDevIndex, FALSE);
               }
               Status = AmiPciGetPcie1SetupDataOverrideHook (dev, &dev->PciExpress->Pcie1Setup, dev->AmiSdlPciDevData, dev->SdlDevIndex, FALSE);
            }
            Status = AmiPciGetPciDevSetupDataOverrideHook (&dev->DevSetup, dev->AmiSdlPciDevData, dev->SdlDevIndex, FALSE);
           // APTIOV_SERVER_OVERRIDE_END : AmiPciBus Setup Override changes.        			

		break;
	//-------------------------------------------------------------------------
		case isPciSkipDevice:
			PCI_TRACE((TRACE_PCI," (isPciSkipDevice); " ));

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
    }//switch

    return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
