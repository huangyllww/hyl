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
#include <Nb.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PciIo.h>
#include <Protocol/AmiBoardInitPolicy.h>
#include <Library/PciAccessCspLib.h>
#include <Library/AmiPciBusLib.h>
#include <Library/PciLib.h>
#include <Protocol/S3SaveState.h>

//-------------------------------------------------------------------------
//!!!!!!!!!!! PORTING REQUIRED !!!!!!!!!!! PORTING REQUIRED !!!!!!!!!!!*
//!!!!!!!!!!!!!!!! must be maintained for PCI devices!!!!!!!!!!!!!!!!!!*
//-------------------------------------------------------------------------

UINT32 Token_HideDieUSBHC [MAX_ROOT_BRIDGE_COUNT] = {
        HideDieUSBHC
};

#define NB_SMN_INDEX_EXT_3_BIOS  0x00C0
#define NB_SMN_INDEX_3_BIOS  0x00C4
#define NB_SMN_DATA_3_BIOS   0x00C8

EFI_S3_SAVE_STATE_PROTOCOL          *mBootScriptSave;

VOID
NbSmnRead (
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

VOID
NbSmnWrite (
  IN       UINT32              DieNum,
  IN       UINT32              Address,
  IN	   UINT32	       *Value
  )
{
    UINT32 write_value=*Value;
    EFI_STATUS Status;
    
    if(mBootScriptSave == NULL) {
        Status = pBS->LocateProtocol(&gEfiS3SaveStateProtocolGuid, NULL, &mBootScriptSave);
        if(EFI_ERROR(Status)) mBootScriptSave = NULL;
        NB_TRACE ((TRACE_NB, "NbPciDxeInitLib: Locate gEfiS3SaveStateProtocolGuid %r \n", Status));
    }
    PciWrite32(NB_SMN_INDEX_EXT_3_BIOS, DieNum);
    if(mBootScriptSave != NULL) {
        NB_TRACE ((TRACE_NB, "NbPciDxeInitLib: S3 save Die %d SMN 0x%08X Value 0x%X \n", DieNum, Address, write_value));
        BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(mBootScriptSave, EfiPciWidthUint32, NB_SMN_INDEX_EXT_3_BIOS, 1, &DieNum);
    }

    PciWrite32(NB_SMN_INDEX_3_BIOS, Address);
    if(mBootScriptSave != NULL) {
        BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(mBootScriptSave, EfiPciWidthUint32, NB_SMN_INDEX_3_BIOS, 1, &Address);
    }

    PciWrite32(NB_SMN_DATA_3_BIOS, write_value);
    if(mBootScriptSave != NULL) {
        BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(mBootScriptSave, EfiPciWidthUint32, NB_SMN_DATA_3_BIOS, 1, &write_value);
    }

    //clear in case other functions don't pay attention to this reg
    PciWrite32(NB_SMN_INDEX_EXT_3_BIOS, 0);
    if(mBootScriptSave != NULL) {
        write_value = 0;
        BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(mBootScriptSave, EfiPciWidthUint32, NB_SMN_INDEX_EXT_3_BIOS, 1, &write_value);
    }

}

VOID
NbSmnRW (
  IN       UINT32              DieNum,
  IN       UINT32              Address,
  IN       UINT32              AndMask,
  IN       UINT32              OrMask
  )
{
  UINT32    RegValue;

  NbSmnRead (DieNum, Address, &RegValue);
  RegValue &= AndMask;
  RegValue |= OrMask;
  NbSmnWrite (DieNum, Address, &RegValue);
}

UINT32
GetNumberOfDiesOnSocket (
  VOID
  )
{
    UINT32          DieNumberPerSocket;
    
    //Get and calculate DieNumberOerSocket
    NbSmnRead (0, 0x1C600, &DieNumberPerSocket);
    DieNumberPerSocket = (DieNumberPerSocket & 0x0000000F) + 1;
    DieNumberPerSocket = (UINT32)LowBitSet32(DieNumberPerSocket);
    return DieNumberPerSocket;
}

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
    UINT32          temp;
    UINT32          DieNumberPerSocket;
	UINT32  		BusNumBase;
	UINT32  		BusNumLimit;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL     *RtBrdg;
    RtBrdg = &node0->RbIoProtocol;
    
    //ASLR_QWORD_ASD	*res=(ASLR_QWORD_ASD*)Args->Param2;
    //ASLR_QWORD_ASD      		*busrd;
    
//---------------------------------
        
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
					//Use direct pci access instead of RBIO protocol which won't work for 1st RB yet
		                  	//D18F1x204 [System Component Count] (SystemComponentCnt)
		                  	//PIECount
 		  		        NbSmnRead (0, 0x1C604, &temp);
		//			RtBrdg->Pci.Write(RtBrdg, EfiPciWidthUint32, EFI_PCI_ADDRESS (0, 0, 0, 0xC0), 1, &temp);
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
  		       // NbSmnRead (0, 0x1C604, &temp);
/*  		        
			temp=0;
                  	PciWrite32(0x000000C0, temp);
                  	//Use direct pci access instead of RBIO protocol which won't work for 1st RB yet
                  	//D18F1x204 [System Component Count] (SystemComponentCnt)
                  	//PIECount
			temp=0x1C604;
                  	PciWrite32(0x000000C4, temp);
                  	temp=PciRead32(0x000000C8);
*/
                  	//temp = temp & 0xF; 
			//PCI_TRACE((TRACE_PCI, "DIE Count = %x\n",temp));    		
			
			//calculate bus size based on # of DIE
			//read DF programmed bus # to determine bus ranges. This is better as total # of bus may not be 256. 
			DieNumberPerSocket = GetNumberOfDiesOnSocket();
			NbSmnRead (0, 0x1C0A0 + dev->RbSdlData->Bus * 4, &temp);
			
			BusNumBase = (temp & 0x00FF0000) >> 16;
			BusNumLimit = (temp & 0xFF000000) >> 24;
			if ((dev->RbSdlData->Bus != 0) && (BusNumBase == 0)) {
			    NbSmnRead (0, 0x1C0A0 + (dev->RbSdlData->Bus+(4-DieNumberPerSocket))*4, &temp);
				BusNumBase = (temp & 0x00FF0000) >> 16;
				BusNumLimit = (temp & 0xFF000000) >> 24;
			}
			 
			//adjust ASLR_QWORD_ASD
			res->_MIN = BusNumBase;
			res->_MAX = BusNumLimit;
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
			
			Status= EFI_SUCCESS;
		break;
    	
        }//switch
    //---------------------------------

    
//---------------------------------
    return Status;
}



EFI_STATUS USB3_Init(
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
    //Temp just try DIE1
    UINT32  DieNum;
    UINT32  DieNumberPerSocket;
    UINT32  CfgAddressMap;
	UINT32	DstFabricId;
    UINT32  BusNumBase;
    UINT32  BusNumLimit;
    UINT32  DieIndex;
    UINT32  temp;
//---------------------------------
    //Check if parameters passed are VALID and
    if(Args->Signature != AMI_PCI_PARAM_SIG) return EFI_INVALID_PARAMETER;
    if(InitStep>=isPciMaxStep) return EFI_INVALID_PARAMETER;
    
    switch (InitStep)
    {	
	//-------------------------------------------------------------------------
		case isPciSkipDevice:
			PCI_TRACE((TRACE_PCI,"\ncalling USB3_Init\n" ));
			//We're going to use this as an init step.
			//Assume any USB HC found (not hidden by earlier code) should be taken out of reset
			//PPR info indicates SP3 Die2/3 have no ports connected. The controllers should then be hidden but if not we could
			//skip this code for those DIE in this function. Alternatively could skip this function by removing it from the SDL data for Die2/3.
			
#if 0			
			//calculate Die Number based on BUS Number
			NbSmnRead (0, 0x1C604, &temp);
			temp = temp & 0xF; 			
			//calculate bus size based on # of DIE			
			temp = 256/temp;
#endif		
			
			DieNumberPerSocket = GetNumberOfDiesOnSocket();
			
			CfgAddressMap = 0x1C0A0;
			for (DieIndex = 0; DieIndex < 8; DieIndex++) {
                NbSmnRead (0, CfgAddressMap, &temp);
				DstFabricId = (temp &0x00000FF0) >> 4;
                BusNumBase = (temp & 0x00FF0000) >> 16;
                BusNumLimit = (temp & 0xFF000000) >> 24;
                
                //calculate die # based on bus num
                if(dev->AmiSdlPciDevData->Bus != 0)
                {
                    if ((dev->AmiSdlPciDevData->Bus >= BusNumBase) && (dev->AmiSdlPciDevData->Bus <= BusNumLimit))
                    {
						DieNum = ((DstFabricId & BIT7) >> 7) * DieNumberPerSocket + ((DstFabricId & (BIT5+BIT6)) >> 5);
                        PCI_TRACE((TRACE_PCI, "Calculated die # = %x\n", DieNum));  
                        break;
                    }
                } else {
                    DieNum=0;
                }	
                CfgAddressMap = CfgAddressMap + 4;
			}
			//Platform porting - Hide unimplemented HCs
            if(Token_HideDieUSBHC[DieNum] == 1)
            {
                //Hide the HC
                NbSmnRW (DieNum, 0x10134600, 0xEFFFFFFF, 0);
                //Skip the device
                Status= EFI_SUCCESS;
            } else {
			
                //skip if already out of reset
                NbSmnRead (DieNum, 0x16D80100, &temp);
                if(temp != 0)
                {		
                    PCI_TRACE((TRACE_PCI, "Take out of reset\n"));  
                    //{ volatile unsigned long __iii; __iii = 1; while (__iii); }
                    NbSmnRW (DieNum, 0x16D80100, 0xFFFFFFFE, 0x1);			
                    NbSmnRW (DieNum, 0x16D80100, 0xFFFFFFF9, 0x6);
                      
                    pBS->Stall(100);
                    
                    NbSmnRW (DieNum, 0x16D08200, 0x00, 0x00000039);
                    NbSmnRW (DieNum, 0x16D08600, 0x00, 0x00000039);
                    NbSmnRW (DieNum, 0x16D08A00, 0x00, 0x00000039);
                    NbSmnRW (DieNum, 0x16D08E00, 0x00, 0x00000039);
                    NbSmnRW (DieNum, 0x16D0C000, 0x00, 0x01B3C153);
                    NbSmnRW (DieNum, 0x16D0C400, 0x00, 0x01B3C153);
                    NbSmnRW (DieNum, 0x16D0C800, 0x00, 0x01B3C153);
                    NbSmnRW (DieNum, 0x16D0CC00, 0x00, 0x01B3C153);		       
                      
                    NbSmnRW (DieNum, 0x16D80100, 0xFFFFFFFE, 0);
                      
                    NbSmnRW (DieNum, 0x16D80100, 0xFFFFFFFB, 0);
                      
                    pBS->Stall(200);
                     
                    NbSmnRW (DieNum, 0x16D80100, 0xFFFFFFFD, 0);
                        
                    NbSmnRW (DieNum, 0x16C0C11C, 0x00, 0x2004010A);
                    NbSmnRW (DieNum, 0x16C0C12C, 0x00, 0x0A43C87D);
                    NbSmnRW (DieNum, 0x16C0C2C0, 0x00, 0x010A0302);
                    NbSmnRW (DieNum, 0x16C0C2C4, 0x00, 0x010A0302);
                    NbSmnRW (DieNum, 0x16C0C2C8, 0x00, 0x010A0302);
                    NbSmnRW (DieNum, 0x16C0C2CC, 0x00, 0x010A0302);
        
                    //May need to update code later to handle ECC_DedErrRptEn
                } //end if in reset
            } // end if(Token_HideDieUSBHC[DieNum] == 1)
			
		break;
	//-------------------------------------------------------------------------
	
		
    }//switch

    return Status;
}

EFI_STATUS SATA_Init(
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

    UINT32	DieNum;
    UINT32          temp;
//---------------------------------
    //Check if parameters passed are VALID and
    if(Args->Signature != AMI_PCI_PARAM_SIG) return EFI_INVALID_PARAMETER;
    if(InitStep>=isPciMaxStep) return EFI_INVALID_PARAMETER;
    
    switch (InitStep)
    {	
	//-------------------------------------------------------------------------
		case isPciSkipDevice:
			PCI_TRACE((TRACE_PCI,"\ncalling SATA_Init\n" ));
			//We're going to use this as an init step.

#if 0			
			//calculate Die Number based on BUS Number
			NbSmnRead (0, 0x1C604, &temp);
			temp = temp & 0xF; 			
			//calculate bus size based on # of DIE			
			temp = 256/temp;
#endif
			
			NbSmnRead (0, 0x1C0A0, &temp);
			temp = temp & 0xFF000000;
			temp = temp >> 24;
			temp = temp+1;

			//calculate die # based on bus num
			if(dev->AmiSdlPciDevData->Bus != 0)
			{
				temp = dev->AmiSdlPciDevData->Bus/temp;
				PCI_TRACE((TRACE_PCI, "Calculated die # = %x\n",temp));  
				DieNum=temp;
			} else {
				DieNum=0;
			}			
			//This is Platform specific, need to make more generic later. Assume Die0 SATA= FieldCard
			if(DieNum == 0)
			{		
				//PCI_TRACE((TRACE_PCI, "Limit SATA to Gen2\n"));  
				//NbSmnRW (DieNum, 0x3100000, 0xFFFFFFFB, 0x4);		       				  
			}
		break;
	//-------------------------------------------------------------------------
	
		
    }//switch

    return Status;
}

EFI_STATUS BMC_Init(
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

  	        PCI_TRACE((TRACE_PCI,"\nPciInit: BMC - Disabling PCIe Init...\n" ));
//			    if(PcieCheckPcieCompatible(dev)){
			         dev->DevSetup.Pcie1Disable=TRUE;
			         dev->DevSetup.Pcie2Disable=TRUE; //if you think that issues is in GEN2 int use this line only. 
 		  	        PCI_TRACE((TRACE_PCI,"\nPciInit: BMC - Disabling Gen1&2...\n" ));
	//		      }
			    Status=EFI_SUCCESS;			    
			
		break;
	//-------------------------------------------------------------------------
		case isPciSkipDevice:
			PCI_TRACE((TRACE_PCI," (isPciSkipDevice); " ));
			//PCI_TRACE((TRACE_PCI,"\ncalling BMC_Init\n" ));
			//PCI_TRACE((TRACE_PCI,"skip BMC VGA\n" ));
			//Status= EFI_SUCCESS;
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

EFI_STATUS NTB_Init(
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

  	        PCI_TRACE((TRACE_PCI,"\nPciInit: NTB - Disabling PCIe Init...\n" ));
  	        		//For Tiger NTB
			        // dev->DevSetup.Pcie1Disable=TRUE;
			        // dev->DevSetup.Pcie2Disable=TRUE; //if you think that issues is in GEN2 int use this line only. 
 		  	        PCI_TRACE((TRACE_PCI,"\nPciInit: NTB - Disabling Gen1&2...\n" ));

			    //Status=EFI_SUCCESS;			    

		break;
	//-------------------------------------------------------------------------
		case isPciSkipDevice:
			PCI_TRACE((TRACE_PCI," (isPciSkipDevice); " ));
			PCI_TRACE((TRACE_PCI,"\ncalling NTB_Init\n" ));

			//{ volatile unsigned long __iii; __iii = 1; while (__iii); }
		//	Status= EFI_SUCCESS;
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
