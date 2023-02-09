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

/** @file
    Contains the functions to override the PCI related setup options so
    that it won't be modified or their registers were not programmed
    again.
*/

//---------------------------------------------------------------------------

#include <AmiPciBusSetupOverrideLib.h>
#include <PciSetup.h>

UINT8 gARIEnable;
//---------------------------------------------------------------------------

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: AmiPciGetPciDevSetupDataOverrideHook
//
// Description: 
//  Sets PCI_SETUP_DONT_TOUCH for all the common PCI Dynamic Setup options
//
// Input:
//  IN PciDev                 - Pointer to the General Pci related setup data buffer
//  IN DevSdlInfo             - Pointer to the Pci device information
//  IN DevIdx                 - Index of the Pci device
//  IN Set                    - TRUE or FALSE
//
// Output:
//     EFI_STATUS
//       EFI_SUCCESS         - Success will be returned.
//
// Modified:    Nothing.
//
// Referrals:   None.
//
// Notes:       None.
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS 
AmiPciGetPciDevSetupDataOverrideHook (
  IN PCI_DEVICE_SETUP_DATA     *PciDev,
  IN AMI_SDL_PCI_DEV_INFO      *DevSdlInfo, //OPTIONAL if == NULL get defaults...
  IN UINTN                     DevIdx,
  IN BOOLEAN                   Set
)
{
    if (PciDev != NULL) {
        PciDev->PciLatency = PCI_SETUP_DONT_TOUCH;
        PciDev->PciXLatency = PCI_SETUP_DONT_TOUCH;
        PciDev->VgaPallete = PCI_SETUP_DONT_TOUCH;
        PciDev->PerrEnable = PCI_SETUP_AUTO_VALUE;
        PciDev->SerrEnable = PCI_SETUP_AUTO_VALUE;

        //PciDev->Pcie1Disable = PCI_SETUP_DONT_TOUCH;
        //PciDev->Pcie2Disable = PCI_SETUP_DONT_TOUCH;
    }
    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: AmiPciGetCommonSetupDataOverrideHook
//
// Description: 
//  Sets PCI_SETUP_DONT_TOUCH for all the common PCI Dynamic Setup options
//
// Input:
//  IN PciCommon             - Pointer to the Pci Bus common setup data buffer
//
// Output:
//     EFI_STATUS
//       EFI_SUCCESS         - Success will be returned.
//
// Modified:    Nothing.
//
// Referrals:   None.
//
// Notes:       None.
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
AmiPciGetCommonSetupDataOverrideHook (
  IN PCI_COMMON_SETUP_DATA     *PciCommon
)
{
    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: AmiPciGetPcie1SetupDataOverrideHook
//
// Description: 
//  Sets PCI_SETUP_DONT_TOUCH for all the GEN 1 PCI Dynamic Setup options
//
// Input:
//  IN Pcie1                  - Pointer to the GEN 1 Pci related setup data buffer
//  IN DevSdlInfo             - Pointer to the Pci device information
//  IN DevIdx                 - Index of the Pci device
//  IN Set                    - TRUE or FALSE
//
// Output:
//     EFI_STATUS
//       EFI_SUCCESS        - Success will be returned.
//
// Modified:    Nothing.
//
// Referrals:   None.
//
// Notes:       None.
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
AmiPciGetPcie1SetupDataOverrideHook (
  IN PCI_DEV_INFO              *PciDevInfo,
  IN PCIE1_SETUP_DATA          *Pcie1,
  IN AMI_SDL_PCI_DEV_INFO      *DevSdlInfo, //OPTIONAL if == NULL get defaults...
  IN UINTN                     DevIdx,		 //OPTIONAL if == 0 
  IN BOOLEAN                   Set
)
{
    EFI_GUID            SetupGuid   = SETUP_GUID;
    EFI_STATUS          Status;
    UINTN               sz=sizeof(SETUP_DATA);
    SETUP_DATA          SetupData;

    Status=pRS->GetVariable(L"Setup",&SetupGuid, NULL, &sz, &SetupData);
    
    if (Pcie1 != NULL){
        Pcie1->RelaxedOrdering = PCI_SETUP_DONT_TOUCH;
        Pcie1->ExtTagField = PCI_SETUP_DONT_TOUCH;
        Pcie1->NoSnoop = PCI_SETUP_DONT_TOUCH;
 
#if (PCI_SETUP_USE_APTIO_4_STYLE == 1)
        if ( (PciDevInfo != NULL) && (PciDevInfo->Type==tPci2PciBrg) &&
                (PciDevInfo->PciExpress!=NULL) && (PciDevInfo->PciExpress->Pcie1Setup.MaxPayload!=PCI_SETUP_DONT_TOUCH)) {
            Pcie1->MaxPayload = SetupData.MaxPayload;
        } else {
            Pcie1->MaxPayload = SetupData.MaxPayload;//<hujf1001-20181015 Support PCIE Setup item like MaxPayload >
        }
#else
        Pcie1->MaxPayload = PCI_SETUP_DONT_TOUCH;
#endif

        Pcie1->MaxReadRequest = SetupData.MaxReadRequest;//<hujf1001-20181015 Support PCIE Setup item like MaxPayload >
        //mizl-202012-Import Hygon PCIe V02 Patch +>
		//<Chendl001-20170911 Disable all PCIe device's ASPM +>
        //Pcie1->AspmMode = PCI_SETUP_DONT_TOUCH;
		Pcie1->AspmMode = 0;
        //<Chendl001-20170911 Disable all PCIe device's ASPM ->
		//mizl-202012-Import Hygon PCIe V02 Patch ->
        Pcie1->ExtendedSynch = SetupData.ExtendedSynch;//<hujf1001-20181015 Support PCIE Setup item like MaxPayload >
        Pcie1->ClockPm = PCI_SETUP_DONT_TOUCH;
        Pcie1->LnkTrRetry = SetupData.LnkTrRetry; //<hujf1001-20181015 Support PCIE Setup item like MaxPayload >
        Pcie1->LnkTrTimeout = SetupData.LnkTrTimeout;//<hujf1001-20181015 Support PCIE Setup item like MaxPayload >
        Pcie1->LnkDisable = SetupData.LnkDisable;//<hujf1001-20181015 Support PCIE Setup item like MaxPayload >
        Pcie1->IgnoreCommonClock = PCI_SETUP_DONT_TOUCH;
        
        Pcie1->DevCtrDontTouch = PCI_SETUP_DONT_TOUCH;
        Pcie1->LnkCtrDontTouch = PCI_SETUP_DONT_TOUCH;
        Pcie1->SltCtrDontTouch = PCI_SETUP_DONT_TOUCH;
    }
    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: AmiPciGetPcie2SetupDataOverrideHook
//
// Description: 
//  Sets PCI_SETUP_DONT_TOUCH for all the GEN 2 PCI Dynamic Setup options
//
// Input:
//  IN Pcie2                  - Pointer to the GEN 2 Pci related setup data buffer
//  IN DevSdlInfo             - Pointer to the Pci device information
//  IN DevIdx                 - Index of the Pci device
//  IN Set                    - TRUE or FALSE
//
// Output:
//     EFI_STATUS
//       EFI_SUCCESS        - Success will be returned.
//
// Modified:    Nothing.
//
// Referrals:   None.
//
// Notes:       None.
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
AmiPciGetPcie2SetupDataOverrideHook (
  IN PCIE2_SETUP_DATA          *Pcie2,
  IN AMI_SDL_PCI_DEV_INFO      *DevSdlInfo,//OPTIONAL if == NULL get defaults...
  IN UINTN                     DevIdx,		//OPTIONAL if == 0 
  IN BOOLEAN                   Set
)
{	
	UINTN                   BufferSize;
	PCI_COMMON_SETUP_DATA 	PciCommon;
	EFI_STATUS Status;
	EFI_GUID		PciSetupGuid	= PCI_FORM_SET_GUID;
	
	BufferSize = sizeof(PCI_COMMON_SETUP_DATA);
	Status=pRS->GetVariable(PCI_COMMON_VAR_NAME, &PciSetupGuid, NULL, &BufferSize, &PciCommon);
	if(EFI_ERROR(Status)){
		gARIEnable = PCI_SETUP_DONT_TOUCH;
	} else {
		gARIEnable = PciCommon.AriSupport;
	}
		
    if (Pcie2 != NULL) {
        Pcie2->ComplTimeOut = PCI_SETUP_DONT_TOUCH;
        //Pcie2->AriFwd = PCI_SETUP_DONT_TOUCH;
        Pcie2->AriFwd = gARIEnable;
        Pcie2->AtomOpReq = PCI_SETUP_DONT_TOUCH;
        Pcie2->AtomOpEgressBlk = PCI_SETUP_DONT_TOUCH;
        Pcie2->IDOReq = PCI_SETUP_DONT_TOUCH;
        Pcie2->IDOCompl = PCI_SETUP_DONT_TOUCH;
        Pcie2->LtrReport = PCI_SETUP_DONT_TOUCH;
        Pcie2->E2ETlpPrBlk = PCI_SETUP_DONT_TOUCH;
       // Pcie2->LnkSpeed = PCI_SETUP_DONT_TOUCH;liutz001-20200213 target speed can not set
        Pcie2->ComplSos = PCI_SETUP_DONT_TOUCH;
        
        Pcie2->HwAutoWidth = PCI_SETUP_DONT_TOUCH;
        Pcie2->HwAutoSpeed = PCI_SETUP_DONT_TOUCH;
        
        Pcie2->DevCtr2DontTouch = PCI_SETUP_DONT_TOUCH;
        Pcie2->LnkCtr2DontTouch = PCI_SETUP_DONT_TOUCH;
    }
    return EFI_SUCCESS;
}

#if (PCI_SETUP_USE_APTIO_4_STYLE == 1)
//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: AmiPciGetSetupDataOverrideHook
//
// Description: 
//  Sets PCI_SETUP_DONT_TOUCH for all the PCI Static Setup options
//
// Input:
//  IN PciSetupData         - Pointer to the Pci Bus specific setup data buffer
//  IN PciCommon            - Pointer to the Pci Bus common setup data buffer
//  IN HpSetup              - Pointer to the Pci Bus Hot plug related setup data buffer
//
// Output:
//     EFI_STATUS
//       EFI_SUCCESS        - Success will be returned.
//
// Modified:    Nothing.
//
// Referrals:   None.
//
// Notes:       None.
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
AmiPciGetSetupDataOverrideHook (
  IN PCI_SETUP_DATA            *PciSetupData,
  IN PCI_COMMON_SETUP_DATA     *PciCommon,
  IN PCI_HOTPLUG_SETUP_DATA    *HpSetup
)
{
	UINTN                   BufferSize;
	//PCI_COMMON_SETUP_DATA 	PciCommon;
	EFI_STATUS Status;
	EFI_GUID		PciSetupGuid	= PCI_FORM_SET_GUID;
	
	BufferSize = sizeof(PCI_COMMON_SETUP_DATA);
	Status=pRS->GetVariable(PCI_COMMON_VAR_NAME, &PciSetupGuid, NULL, &BufferSize, PciCommon);
	if(EFI_ERROR(Status)){
		gARIEnable = PCI_SETUP_DONT_TOUCH;
	} else {
		gARIEnable = PciCommon->AriSupport;
	}
		
    if (PciSetupData!=NULL) {
        PciSetupData->PciDevSettings.PciLatency = PCI_SETUP_DONT_TOUCH;
        PciSetupData->PciDevSettings.PciXLatency = PCI_SETUP_DONT_TOUCH;
        PciSetupData->PciDevSettings.VgaPallete = PCI_SETUP_DONT_TOUCH;
        PciSetupData->PciDevSettings.PerrEnable = PCI_SETUP_DONT_TOUCH;
        PciSetupData->PciDevSettings.SerrEnable = PCI_SETUP_DONT_TOUCH;
        
        PciSetupData->Pcie1Settings.RelaxedOrdering = PCI_SETUP_DONT_TOUCH;
        PciSetupData->Pcie1Settings.ExtTagField = PCI_SETUP_DONT_TOUCH;
        PciSetupData->Pcie1Settings.NoSnoop = PCI_SETUP_DONT_TOUCH;
        PciSetupData->Pcie1Settings.MaxPayload = PCI_SETUP_DONT_TOUCH;
        PciSetupData->Pcie1Settings.MaxReadRequest = PCI_SETUP_DONT_TOUCH;
        PciSetupData->Pcie1Settings.AspmMode = PCI_SETUP_DONT_TOUCH;
        PciSetupData->Pcie1Settings.ExtendedSynch = PCI_SETUP_DONT_TOUCH;
        PciSetupData->Pcie1Settings.ClockPm = PCI_SETUP_DONT_TOUCH;
        PciSetupData->Pcie1Settings.LnkTrRetry = PCI_SETUP_DONT_TOUCH;
        PciSetupData->Pcie1Settings.LnkTrTimeout = PCI_SETUP_DONT_TOUCH;
        PciSetupData->Pcie1Settings.LnkDisable = PCI_SETUP_DONT_TOUCH;
        PciSetupData->Pcie1Settings.IgnoreCommonClock = PCI_SETUP_DONT_TOUCH;
        
        PciSetupData->Pcie1Settings.DevCtrDontTouch = PCI_SETUP_DONT_TOUCH;
        PciSetupData->Pcie1Settings.LnkCtrDontTouch = PCI_SETUP_DONT_TOUCH;
        PciSetupData->Pcie1Settings.SltCtrDontTouch = PCI_SETUP_DONT_TOUCH;
        
        PciSetupData->Pcie2Settings.ComplTimeOut = PCI_SETUP_DONT_TOUCH;
        PciSetupData->Pcie2Settings.AriFwd = gARIEnable;
        PciSetupData->Pcie2Settings.AtomOpReq = PCI_SETUP_DONT_TOUCH;
        PciSetupData->Pcie2Settings.AtomOpEgressBlk = PCI_SETUP_DONT_TOUCH;
        PciSetupData->Pcie2Settings.IDOReq = PCI_SETUP_DONT_TOUCH;
        PciSetupData->Pcie2Settings.IDOCompl = PCI_SETUP_DONT_TOUCH;
        PciSetupData->Pcie2Settings.LtrReport = PCI_SETUP_DONT_TOUCH;
        PciSetupData->Pcie2Settings.E2ETlpPrBlk = PCI_SETUP_DONT_TOUCH;
        //PciSetupData->Pcie2Settings.LnkSpeed = PCI_SETUP_DONT_TOUCH;liutz001-20200213 target speed can not set
        PciSetupData->Pcie2Settings.ComplSos = PCI_SETUP_DONT_TOUCH;
        
        PciSetupData->Pcie2Settings.HwAutoWidth = PCI_SETUP_DONT_TOUCH;
        PciSetupData->Pcie2Settings.HwAutoSpeed = PCI_SETUP_DONT_TOUCH;
        
        PciSetupData->Pcie2Settings.DevCtr2DontTouch = PCI_SETUP_DONT_TOUCH;
        PciSetupData->Pcie2Settings.LnkCtr2DontTouch = PCI_SETUP_DONT_TOUCH;
    }
    return EFI_SUCCESS;
}
#else
//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: AmiPciGetSetupDataOverrideHook
//
// Description: 
//  Sets PCI_SETUP_DONT_TOUCH for all the PCI Static Setup options
//
// Input:
//  IN PciCommon              - Pointer to the Pci Bus common setup data buffer
//  IN Pci                    - Pointer to the general PCI related setup data buffer
//  IN Pcie1                  - Pointer to the GEN 1 Pci related setup data buffer
//  IN Pcie2                  - Pointer to the GEN 2 Pci related setup data buffer
//  IN DevSdlInfo             - Pointer to the Pci device information
//  IN DevIdx                 - Index of the Pci device
//
// Output:
//     EFI_STATUS
//       EFI_SUCCESS        - Success will be returned.
//
// Modified:    Nothing.
//
// Referrals:   None.
//
// Notes:       None.
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
AmiPciGetSetupDataOverrideHook (
  PCI_COMMON_SETUP_DATA     *PciCommon,
  PCI_DEVICE_SETUP_DATA     *Pci,
  PCIE1_SETUP_DATA          *Pcie1,
  PCIE2_SETUP_DATA          *Pcie2,
  AMI_SDL_PCI_DEV_INFO      *DevSdlInfo,    //OPTIONAL if ==NULL get defaults...
  UINTN                     DevIdx          //OPTIONAL 
)
{
    return EFI_SUCCESS;
}
#endif 
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
