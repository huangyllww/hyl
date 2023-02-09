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
/** @file SbGetSetupData.c
    Custom South Bridge setup data behavior implementation

**/
//**********************************************************************
#include <PiPei.h>
#include <Token.h>
#include <Setup.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/ElinkLib.h>
#include <SbElinks.h>

#include <Sb.h>
#include <Library/SbSetupData.h>

#include <AmiLib.h>
#include <AmiDxeLib.h>

//---------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//---------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)

typedef VOID (SB_OEM_SETUP_CALLBACK) (
    IN VOID                 *Services,
    IN OUT SB_SETUP_DATA    *SbSetupData,
    IN SETUP_DATA           *SetupData,
    IN BOOLEAN              Pei
);

// Function Prototype(s)

VOID SbSetupCallbacks (
    IN VOID                 *Services,
    IN OUT SB_SETUP_DATA    *SbSetupData,
    IN SETUP_DATA           *SetupData,
    IN BOOLEAN              Pei
);

// External Declaration(s)

extern SB_OEM_SETUP_CALLBACK SB_OEM_SETUP_CALLBACK_LIST EndOfList;
//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
// Variable Declaration(s)


// GUID Definition(s)

static EFI_GUID gSetupGuid = SETUP_GUID;
// Protocol/Ppi Definition(s)



// Function Definition(s)
extern SB_OEM_SETUP_CALLBACK SB_OEM_SETUP_CALLBACK_LIST EndOfList;
SB_OEM_SETUP_CALLBACK* SbOemSetupCallbackList[] = { SB_OEM_SETUP_CALLBACK_LIST NULL };

//---------------------------------------------------------------------------

/**
    This function calls registered callbacks for OEM/custom setup.

    @param Services Pointer to PeiServices or RuntimeServices
        structure
    @param SbSetupData Pointer to custom setup data to return
    @param SetupData Pointer to system setup data.
    @param Pei Pei flag. If TRUE we are in PEI phase

    @retval VOID
**/

VOID SbOemSetupCallbacks (
    IN VOID              *Services,
    IN OUT SB_SETUP_DATA *SbSetupData,
    IN SETUP_DATA        *SetupData,
    IN BOOLEAN           Pei )
{
  UINTN   				  i;
  UINTN                   ElinkPtr;
  AMI_HOOK_LINK           *AmiHookLink;
  UINT32                  Index;
  SB_OEM_SETUP_CALLBACK   *Elink;

  for (i = 0; SbOemSetupCallbackList[i] != NULL; i++)
    SbOemSetupCallbackList[i]( Services, SbSetupData, SetupData, Pei);

  ElinkPtr = ElinkGet (PcdToken(PcdSbOemSetupElink));

  if (ElinkPtr == 0) {
    return;
  }

  AmiHookLink = (AMI_HOOK_LINK *) ElinkPtr;

  for (Index = 0; Index < ELINK_ARRAY_NUM; Index++) {
    if (AmiHookLink->ElinkArray[Index] == NULL) {
      break;
    }
    Elink = AmiHookLink->ElinkArray[Index];
    Elink(Services, SbSetupData, SetupData, Pei);
  }
}


/**
    This function returns custom setup data from system SetupData
    variable

    @param Services Pointer to PeiServices or RuntimeServices
        structure
    @param SbSetupData Pointer to custom setup data to return
    @param Pei Pei flag. If TRUE we are in PEI phase

    @retval VOID

    @note  PORTING REQUIRED
**/
VOID GetSbSetupData (
    IN VOID                 *Services,
    IN OUT SB_SETUP_DATA    *SbSetupData,
    IN BOOLEAN              Pei )
{
    EFI_STATUS                      Status;
    SETUP_DATA                      SetupData, *TempSetupData = NULL;
    EFI_PEI_SERVICES                **PeiServices;
    EFI_RUNTIME_SERVICES            *RunServices;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI  *ReadOnlyVariable = NULL;
    UINTN                           VariableSize = sizeof(SETUP_DATA);


    if (Pei) {
        PeiServices = (EFI_PEI_SERVICES **)Services;
        Status = (*PeiServices)->LocatePpi( PeiServices, \
                                            &gEfiPeiReadOnlyVariable2PpiGuid, \
                                            0, \
                                            NULL, \
                                            &ReadOnlyVariable );

        if (!EFI_ERROR(Status)) {
            Status = ReadOnlyVariable->GetVariable( ReadOnlyVariable, \
                                                    L"Setup", \
                                                    &gSetupGuid, \
                                                    NULL, \
                                                    &VariableSize, \
                                                    &SetupData );

        }
    } else {
        RunServices = (EFI_RUNTIME_SERVICES *)Services;
        Status = RunServices->GetVariable( L"Setup", \
                                           &gSetupGuid, \
                                           NULL, \
                                           &VariableSize, \
                                           &SetupData );
    }

    // Porting Start
    // Update SB_SETUP_DATA according to the default values.
    // To use AMD-CRB safe default if NVRAM invalid.
    SbSetupData->AmdSafeDefault = TRUE;

    //==================================
    // AMD parameters by token
    //==================================
    SbSetupData->LegacyFree = CIM_LEGACY_FREE;

    //==================================
    // AMI definition
    //==================================
#if XHCI_Firmware_Support
    SbSetupData->Usb.Xhci0Enable = 1;
#else
    SbSetupData->Usb.Xhci0Enable = 0;
#endif

    SbSetupData->Usb0PortEn[0] = 1;
    SbSetupData->Usb0PortEn[1] = 1;

    SbSetupData->Usb0HubPortEn[0] = 1;
    SbSetupData->Usb0HubPortEn[1] = 1;
    SbSetupData->Usb0HubPortEn[2] = 1;
    SbSetupData->Usb0HubPortEn[3] = 1;

    SbSetupData->Xhci0PortEn[0] = 1;
    SbSetupData->Xhci0PortEn[1] = 1;
    SbSetupData->Xhci0PortEn[2] = 1;
    SbSetupData->Xhci0PortEn[3] = 1;
    
    SbSetupData->TpmDecode = 0;

    SbSetupData->FchDevEnMap.FchDeviceEnableMap = 0;
    SbSetupData->FchDevD3Map.FchDeviceD3ColdMap = 0x7FFFFFFF;
    
    if (!EFI_ERROR(Status)) {
        TempSetupData = &SetupData;
    }
    SbOemSetupCallbacks( Services, SbSetupData, TempSetupData, Pei );

}

#if defined SB_SETUP_SUPPORT && SB_SETUP_SUPPORT == 1
/**
    This function returns SB Chipset setup data from system SetupData
    variable

    @param Services Pointer to PeiServices or RuntimeServices
    @param SbSetupData Pointer to custom setup data to return
    @param SetupData Pointer to system setup data.
    @param Pei Pei flag. If TRUE we are in PEI phase

    @retval VOID
**/
VOID SbSetupCallbacks (
  IN VOID              *Services,
  IN OUT SB_SETUP_DATA *SbSetupData,
  IN SETUP_DATA        *SetupData,
  IN BOOLEAN           Pei )
{

    if (SetupData != NULL) {

        SbSetupData->AmdSafeDefault = FALSE;
        // Porting Start
        // Update SB_SETUP_DATA according to the setup datas.

        
        SbSetupData->CbsPresent = SetupData->AmdCbsPresent;
        
        //==================================
        //Chipset-> South Bridge-> SATA Configuration
        //==================================
        SbSetupData->Sata.SataMode.SataEnable = SetupData->SataController;
        SbSetupData->Sata.SataClass = SetupData->SataClass;

        //==================================
        //Chipset-> South Bridge-> USB Configuration
        //==================================
    #if (!CONTROL_ALL_USB_METHOD)                                 // (EIP53061+)

    #if XHCI_Firmware_Support
        SbSetupData->Usb.Xhci0Enable = SetupData->Xhci0Enable;
    #endif

        SbSetupData->Usb0PortEn[0] = SetupData->Usb0PortEn0;
        SbSetupData->Usb0PortEn[1] = SetupData->Usb0PortEn1;

        SbSetupData->Usb0HubPortEn[0] = SetupData->Usb0HubPortEn0;
        SbSetupData->Usb0HubPortEn[1] = SetupData->Usb0HubPortEn1;
        SbSetupData->Usb0HubPortEn[2] = SetupData->Usb0HubPortEn2;
        SbSetupData->Usb0HubPortEn[3] = SetupData->Usb0HubPortEn3;

        SbSetupData->Xhci0PortEn[0] = SetupData->Xhci0PortEn0;
        SbSetupData->Xhci0PortEn[1] = SetupData->Xhci0PortEn1;
        SbSetupData->Xhci0PortEn[2] = SetupData->Xhci0PortEn2;
        SbSetupData->Xhci0PortEn[3] = SetupData->Xhci0PortEn3;   

    #else
        SbSetupData->UsbAllEnable = SetupData->UsbAllEnable;
    #endif

        //==================================
        //Chipset-> South Bridge-> IR Configuration
        //==================================
        SbSetupData->Ir.IrConfig = SetupData->IrConfig;

        //==================================
        //Chipset-> South Bridge->  SD(Secure Digital) Configuration
        //==================================
        SbSetupData->Sd.SdConfig = SetupData->SdConfig;
        SbSetupData->Sd.SdForce18 = SetupData->SdForce18;
        SbSetupData->Sd.SdDbgConfig = SetupData->SdDbgConfig;

        
        if (SetupData->GpioAcpiEnabled ==0) {
            SbSetupData->FchDevEnMap.FchDeviceEnableMap = 0;
        } else {
        
        SbSetupData->FchDevEnMap.GIOE = SetupData->GpioAcpiEnabled;
        //==================================
        //Chipset-> South Bridge->  I2C Configuration
        //==================================
        SbSetupData->FchDevEnMap.IC0E = SetupData->I2C0Enable;
        SbSetupData->FchDevEnMap.IC1E = SetupData->I2C1Enable;
        SbSetupData->FchDevEnMap.IC2E = SetupData->I2C2Enable;
        SbSetupData->FchDevEnMap.IC3E = SetupData->I2C3Enable;

        //==================================
        //Chipset-> South Bridge->  UART Configuration
        //==================================
        SbSetupData->FchDevEnMap.UT0E = SetupData->Uart0Enable;
        SbSetupData->FchDevEnMap.UT1E = SetupData->Uart1Enable;
        }
        //==================================
        //Chipset-> South Bridge-> Power Saving
        //==================================
#if SB_POWER_SAVING_SUPPORT
        SbSetupData->Ab.AbClockGating = SetupData->AbClockGating;
#endif

        //==================================
        //Chipset-> South Bridge-> SATA Debug Configuration
        //==================================

        //###SbSetupData->Sata.SataEspPort.SataPortReg = 0;
        SbSetupData->Sata.SataEspPort.Port0 = SetupData->SataEspPort0;
        SbSetupData->Sata.SataEspPort.Port1 = SetupData->SataEspPort1;
        SbSetupData->Sata.SataEspPort.Port2 = SetupData->SataEspPort2;
        SbSetupData->Sata.SataEspPort.Port3 = SetupData->SataEspPort3;
        SbSetupData->Sata.SataEspPort.Port4 = SetupData->SataEspPort4;
        SbSetupData->Sata.SataEspPort.Port5 = SetupData->SataEspPort5;
        SbSetupData->Sata.SataEspPort.Port6 = SetupData->SataEspPort6;
        SbSetupData->Sata.SataEspPort.Port7 = SetupData->SataEspPort7;

        //####SbSetupData->Sata.SataPortPower.SataPortReg = 0;
        SbSetupData->Sata.SataPortPower.Port0 = SetupData->SataPower0;
        SbSetupData->Sata.SataPortPower.Port1 = SetupData->SataPower1;
        SbSetupData->Sata.SataPortPower.Port2 = SetupData->SataPower2;
        SbSetupData->Sata.SataPortPower.Port3 = SetupData->SataPower3;
        SbSetupData->Sata.SataPortPower.Port4 = SetupData->SataPower4;
        SbSetupData->Sata.SataPortPower.Port5 = SetupData->SataPower5;
        SbSetupData->Sata.SataPortPower.Port6 = SetupData->SataPower6;
        SbSetupData->Sata.SataPortPower.Port7 = SetupData->SataPower7;

        //####SbSetupData->Sata.SataPortMd.SataPortMode = 0;
        SbSetupData->Sata.SataPortMd.Port0 = SetupData->SataPortMode0;
        SbSetupData->Sata.SataPortMd.Port1 = SetupData->SataPortMode1;
        SbSetupData->Sata.SataPortMd.Port2 = SetupData->SataPortMode2;
        SbSetupData->Sata.SataPortMd.Port3 = SetupData->SataPortMode3;
        SbSetupData->Sata.SataPortMd.Port4 = SetupData->SataPortMode4;
        SbSetupData->Sata.SataPortMd.Port5 = SetupData->SataPortMode5;
        SbSetupData->Sata.SataPortMd.Port6 = SetupData->SataPortMode6;
        SbSetupData->Sata.SataPortMd.Port7 = SetupData->SataPortMode7;

        SbSetupData->Sata.SataMode.SataSetMaxGen2 = SetupData->SataMaxGen2Cap; //  AMD default FALSE ?????
        SbSetupData->Sata.SataMode.SataClkMode = SetupData->SataClkMode;

        SbSetupData->Sata.SataAggrLinkPmCap = SetupData->SataAggrLinkPmCap; //  AMD default 0 ?????
        SbSetupData->Sata.SataPortMultCap = SetupData->SataPortMultCap; //  AMD default 0 ?????
        SbSetupData->Sata.SataClkAutoOff = SetupData->SataClkAutoOff;
        SbSetupData->Sata.SataPscCap = SetupData->SataPscCap; //  AMD default 0 ?????
        SbSetupData->Sata.SataFisBasedSwitching = SetupData->SataFisBasedSwitching;
        SbSetupData->Sata.SataCccSupport = SetupData->SataCccSupport;
        SbSetupData->Sata.SataSscCap = SetupData->SataSscCap; //  AMD default 0 ?????

        SbSetupData->Sata.SataTargetSupport8Device = SetupData->SataTargetSupport8Device;
        SbSetupData->Sata.SataDisableGenericMode = SetupData->SataDisableGenericMode;
        SbSetupData->Sata.SataAhciEnclosureManagement = SetupData->SataAhciEnclosureManagement;
        SbSetupData->Sata.SataSgpio0 = SetupData->SataSgpio0;

        SbSetupData->Sata.SataPhyPllShutDown = SetupData->SataPhyPllShutDown;

        //==================================
        //Chipset-> South Bridge->SB  FUSION Debug Configuration
        //==================================
        SbSetupData->Gcpu.AcDcMsg = SetupData->AcdcChangeMessageDelivery;
        SbSetupData->Gcpu.TimerTickTrack = SetupData->TimerTickTracking; //  AMD default 0 ?????
        SbSetupData->Gcpu.ClockInterruptTag = SetupData->ClockInterruptTag; //  AMD default 0 ?????
        SbSetupData->Gcpu.EhciTrafficHanding = SetupData->EhciTrafficHanding;
        SbSetupData->Gcpu.GcpuMsgCMultiCore = SetupData->FusionMessageCMultiCore;
        SbSetupData->Gcpu.GcpuMsgCStage = SetupData->FusionMessageCState;

        //==================================
        //Chipset-> South Bridge->SB  SPI Debug Configuration
        //==================================
        SbSetupData->Spi.AutoMode = SetupData->SpiAutoMode;
        if(SbSetupData->Spi.AutoMode == 0){
            SbSetupData->Spi.SpiMode = SetupData->SpiMode;
            SbSetupData->Spi.SPI100_Enable = SetupData->SPI100_Enable;
            SbSetupData->Spi.SpiSpeed = SetupData->SpiSpeed;
            SbSetupData->Spi.SpiFastSpeed = SetupData->SpiFastSpeed;
            SbSetupData->Spi.SpiBurstWrite = SetupData->SpiBurstWrite;
        }

        //==================================
        //Chipset-> South Bridge->SB  MSIC Debug Configuration
        //==================================
        SbSetupData->HwAcpi.SpreadSpectrum = SetupData->SpreadSpectrum;

#if HPET_SUPPORT
        SbSetupData->Hpet.HpetEnable = SetupData->HpetTimer;
        SbSetupData->Hpet.HpetMsiDis = SetupData->HpetMsiDis;
#else
        SbSetupData->Hpet.HpetEnable = 0;
#endif

        SbSetupData->Misc.NativePcieSupport = SetupData->NativePcieSupport; //  AMD default 0 ?????

        SbSetupData->Spi.LpcMsiEnable = SetupData->LpcMsi;
        SbSetupData->HwAcpi.PwrFailShadow = SetupData->SBPowerState;
        SbSetupData->HwAcpi.MtC1eEnable = SetupData->SbMtC1eEnable;

        //==================================
        // Other modules
        //==================================
#if (( defined(TCG_SUPPORT) && (TCG_SUPPORT!=0) ) || ( defined(TCG2Support) && (TCG2Support!=0) ))
    SbSetupData->TpmDecode = SetupData->TpmSupport;
    // Porting End
#endif
    }
}

#endif

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
