//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************


/** @file CrbPciBoardDxeInitLib.c

**/

#include <AmiDxeLib.h>
#include <Token.h>
#include <PciBus.h>
#include <PciHostBridge.h>
#include <Setup.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PciIo.h>
#include <Library/PciLib.h>
#include <Protocol/AmiBoardInitPolicy.h>
#include <Library/AmiPciBusLib.h>
#if defined(AmiNetworkPkg_SUPPORT)&&(AmiNetworkPkg_SUPPORT==1)
#include <Guid/NetworkStackSetup.h>
#endif

#include <Protocol/LegacyBiosPlatform.h>


#if ( defined(CSM_SUPPORT) && (CSM_SUPPORT != 0) )


EFI_STATUS
CRBGetPlatformVgaHandle (
  IN  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL     *This,
  IN  EFI_GET_PLATFORM_HANDLE_MODE          Mode,
  IN  UINT16                                Type,
  OUT EFI_HANDLE                            **HandleBuffer,
  OUT UINTN                                 *HandleCount,
  OUT VOID                                  OPTIONAL **AdditionalData
  )
{
    EFI_HANDLE              *hBuffer = NULL;
    EFI_HANDLE              hVga = NULL;
    EFI_HANDLE              hOffboardVideo = NULL;
    EFI_STATUS              Status;
    UINT8                   dData[4];
    UINT32                  BARAddrReg;
    UINT8                   BarIndex;
    UINT8                   Offset=0x10;
    BOOLEAN                 IsBARNonZero=FALSE;
	BOOLEAN                 Is64bit=FALSE;
    EFI_PCI_IO_PROTOCOL     *PciIo;
    EFI_STATUS              VgaStatus = EFI_NOT_FOUND;
    UINTN                   n, Count, Seg, Bus, Dev, Fun;
    UINT64                  PciAttributes;
    
	SETUP_DATA  SetupData;
    UINTN                       BufferSize;
    EFI_GUID    EfiSetupGuid = SETUP_GUID;
    UINT32			LegacyVgaBDF;
     
    if (Mode == EfiGetPlatformVgaHandle) {
        BufferSize = sizeof(SETUP_DATA);
		pRS->GetVariable(L"Setup", &EfiSetupGuid, NULL, &BufferSize, &SetupData);
				     
    
        //
        // Locate all PciIo handles
        //
        Status = pBS->LocateHandleBuffer (
                        ByProtocol,
                        &gEfiPciIoProtocolGuid,
                        NULL,
                        &Count,
                        &hBuffer);
        ASSERT_EFI_ERROR(Status);
        if (EFI_ERROR (Status)) {
            return Status;
        }
        
        for (n = 0; n < Count; n++) {
            Status = pBS->HandleProtocol (
                            hBuffer[n],
                            &gEfiPciIoProtocolGuid,
                            &PciIo);           // Get PciIo protocol
            ASSERT_EFI_ERROR (Status);
            if (EFI_ERROR(Status)) break;
            Status = PciIo->Pci.Read(
                                PciIo,
                                EfiPciIoWidthUint32,
                                8,  // offset
                                1,  // width
                                dData);
            
            ASSERT_EFI_ERROR(Status);
            if (EFI_ERROR(Status)) break;
            //
            // Check for VGA-compatible Display controller
            // Base Class: 03h (Display Controller), Sub-Class: 00h (VGA-compatible controller)
            //
            if ((dData[3] == PCI_CL_DISPLAY && dData[2] == PCI_CL_DISPLAY_SCL_VGA) ||
            		(dData[3] == PCI_CL_DISPLAY && dData[2] == PCI_CL_DISPLAY_SCL_OTHER) ||
            		(dData[3] == PCI_CL_MMEDIA && dData[2] == PCI_CL_MMEDIA_SCL_VIDEO)) {
            	
            	Is64bit = FALSE;
            	
                    for(BarIndex=0, IsBARNonZero=FALSE, Offset=0x10; BarIndex<6; BarIndex++,Offset+=4){
                        //Get Device BAR Address Values
                        Status = PciIo->Pci.Read(
                                            PciIo,
                                            EfiPciIoWidthUint32,
                                            Offset,  // offset
                                            1,  // width
                                            &BARAddrReg);
                        if(BARAddrReg & BIT0){
                            // Io Space: Mask BIT1:BIT0 and check BAR is Zero
                            if(BARAddrReg & 0xFFFFFFFC){
                                IsBARNonZero = TRUE;
                            }
                        }else{
                            //Memory Space: Mask BIT3:BIT0 and Check if BAR is ZERO
                            if(BARAddrReg & 0xFFFFFFF0){
                                IsBARNonZero = TRUE;
                            }
                            //Memory Space: Mask BIT2:BIT1 and Check if 64-Bit BAR
                            if((BARAddrReg & (BIT2 | BIT1)) == BIT2){
                                BarIndex++;
                                Offset+=4;
                                //Get Device BAR Address Values
                                Status = PciIo->Pci.Read(
                                                    PciIo,
                                                    EfiPciIoWidthUint32,
                                                    Offset,  // offset
                                                    1,  // width
                                                    &BARAddrReg);
                                //Memory Space: Check if upper 32-bit of BAR is not ZERO
                                if(BARAddrReg){
                                    Is64bit = TRUE;
                                }
                            }
                        }
                    }
                    if((BarIndex == 6) && !IsBARNonZero){
                        DEBUG((EFI_D_INFO, "VGA Detected with No Resources allocated\n"));
                        continue;
                    }
					if((BarIndex == 6) && Is64bit){
                        DEBUG((EFI_D_INFO, "Not the VGA handle we are looking for\n"));
                        continue;
                    }

                Status = PciIo->Attributes(PciIo, EfiPciIoAttributeOperationGet, 0, &PciAttributes);
                if (EFI_ERROR(Status))
                    return Status;
                                							
                LegacyVgaBDF = PcdGet32 (PcdLegacyVgaBDF);
                LegacyVgaBDF = LegacyVgaBDF & 0xFFFFFFFE;	//clear bit0 (onboard bit)                
                PciIo->GetLocation(PciIo, &Seg, &Bus, &Dev, &Fun);                
                
                if(LegacyVgaBDF==PCI_LIB_ADDRESS(Bus, Dev, Fun, 0))
                {
                	hVga = hBuffer[n]; 
                	VgaStatus = EFI_SUCCESS;
                }  
                
            } // if PCI_CL_DISPLAY
        } // end of for loop
        
        pBS->FreePool(hBuffer);
        
        if(VgaStatus != EFI_SUCCESS){
            **HandleBuffer = 0;
            *HandleCount = 0;
            return VgaStatus;
        }
                        
        Status = pBS->HandleProtocol (
                        hVga,
                        &gEfiPciIoProtocolGuid,
                        &PciIo);           // Get PciIo protocol
        ASSERT_EFI_ERROR (Status);
        if (EFI_ERROR(Status))
            return Status;

        Status = PciIo->GetLocation(PciIo, &Seg, &Bus, &Dev, &Fun);
        ASSERT_EFI_ERROR (Status);
        if (EFI_ERROR(Status))
            return Status;
			
        **HandleBuffer = hVga;
        *HandleCount = 1;        
        
        return VgaStatus;
   }
    else {
        return EFI_UNSUPPORTED;
    }

}

#endif

#if defined(CRB_PXE_LOADER_SUPPORT) && (CRB_PXE_LOADER_SUPPORT == 0)
/**
  dispatch UEFI PXE depend on the setup item of Network.

  @param[in]  Device  The pointer to PCI_DEV_INFO which is from PciBus driver.


  @return EFI_SUCCESS               No error
  @return Others Errors             No NetworkStackVar support.

**/
EFI_STATUS
AmiCrbPciePxeLoader (
  IN PCI_DEV_INFO *Device
  )
{

    EFI_STATUS  	Status = EFI_SUCCESS;
    PCI_DEV_INFO	*dev = Device;

#if defined(AmiNetworkPkg_SUPPORT) && (AmiNetworkPkg_SUPPORT == 1)
    UINTN    		Size;
    NETWORK_STACK	mNetworkStackData;

    if (dev->DevVenId.DevId == CRB_PXE_DID && dev->DevVenId.VenId == CRB_PXE_VID) {
		Size = sizeof(NETWORK_STACK);
		Status = pRS->GetVariable(L"NetworkStackVar",&gEfiNetworkStackSetupGuid, NULL, &Size, &mNetworkStackData);
		if (Status == EFI_SUCCESS) {
			 if (mNetworkStackData.Enable == 0 ) {
				if(dev->EmbRomCnt > 0) {
					EMB_ROM_INFO	*embrom;
					UINTN 			i;

					for(i = 0; i < dev->EmbRomCnt; i++) {
						embrom = dev->EmbRoms[i];
						if(embrom->UefiDriver) {
							// Remove PXE UEFI driver.
							embrom->UefiDriver = FALSE;
							embrom->EmbRomDevVenId.DEV_VEN_ID = -1;
						}
					}
				}
			 }
		}
    }
#endif

    return Status;
}

#endif


/**
    SB Pci init routine

        AMI_BOARD_INIT_PROTOCOL		*This,
    @param Function 
    @param ParameterBlock 

    @retval EFI_STATUS Status

**/
EFI_STATUS	CrbPciInitRoutine (
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
			// PXE dispatch.
		#if defined(CRB_PXE_LOADER_SUPPORT) && (CRB_PXE_LOADER_SUPPORT == 0)
			Status = AmiCrbPciePxeLoader(dev);
		#endif

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
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
