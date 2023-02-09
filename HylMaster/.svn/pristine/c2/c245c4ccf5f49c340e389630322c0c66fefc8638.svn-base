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
/** @file SbInterfaceDxe.c
    SB CIMX interface in the Dxe Phase ,it is main change the parameters
    for the SB CIMX code .

**/
//**********************************************************************
// Module specific Includes
#include <PiDxe.h>
#include <Library/PcdLib.h>
#include <Library/PciLib.h>
#include <Library/S3PciLib.h>
#include <AmiChipsetIoLib.h>
#include <SB.H>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <Setup.h>
#include <AmiHobs.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/HygonSbPolicy.h>
#include <Protocol/S3SaveState.h>
#include <Library/SbSetupData.h>
#include <Ppi/SbPpi.h>
#include <FastBoot.h>
#include <Token.h>


//---------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//---------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)
typedef struct {
    UINT64                  Address;
    EFI_BOOT_SCRIPT_WIDTH   Width;
} BOOT_SCRIPT_PCI_REGISTER_SAVE;

// Function Prototype(s)
EFI_STATUS EFIAPI DetecThermalCap(
);

VOID
HookInitEnv (
  IN HYGON_FCH_AMI_POLICY_PROTOCOL    *This,
  IN VOID                           *InitAgesaHook
  );

VOID
HookInitMid (
  IN HYGON_FCH_AMI_POLICY_PROTOCOL    *This,
  IN VOID                           *InitAgesaHook
  );

VOID
HookInitLate (
  IN HYGON_FCH_AMI_POLICY_PROTOCOL    *This,
  IN VOID                           *InitAgesaHook
  );

VOID
HookCustomizeS3Save (
  IN HYGON_FCH_AMI_POLICY_PROTOCOL    *This,
  IN VOID                           *InitAgesaHook
  );

VOID
HookAfterInitEnv (
  IN HYGON_FCH_AMI_POLICY_PROTOCOL    *This,
  IN VOID                           *InitAgesaHook
  );

VOID
HookAfterInitMid (
  IN HYGON_FCH_AMI_POLICY_PROTOCOL    *This,
  IN VOID                           *InitAgesaHook
  );

VOID
HookAfterInitLate (
  IN HYGON_FCH_AMI_POLICY_PROTOCOL    *This,
  IN VOID                           *InitAgesaHook
  );

VOID
HookAgesaFchOemCalloutDxe (
  IN HYGON_FCH_AMI_POLICY_PROTOCOL    *This,
  IN VOID                           *InitAgesaHook
  );

EFI_STATUS
ReserveSbMmioProgrammingDXETable(
  IN OUT    FCH_DATA_BLOCK *FchData
  );

//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
// Variable Declaration(s)

HYGON_FCH_AMI_POLICY_PROTOCOL     *gHygonFchAmiPolicy;
EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL	static	*gPciRootBridgeIo = NULL;
EFI_S3_SAVE_STATE_PROTOCOL     	static	*gBootScriptSave = NULL;
BOOLEAN							static	IsBeforePciRestoreSwSmiDone = FALSE;

BOOT_SCRIPT_PCI_REGISTER_SAVE gPciRegScriptTbl[] = {
    // HDA boot script save
    SATA_REG(0x2C),  EfiBootScriptWidthUint32,
    SMBUS_REG(0x2C),  EfiBootScriptWidthUint32,
    USB1_EHCI_REG(0x10),  EfiBootScriptWidthUint32,
    USB1_EHCI_REG(0x2C),  EfiBootScriptWidthUint32,
    SB_REG(0x2C),  EfiBootScriptWidthUint32,
    PCIBR_REG(0x2C),  EfiBootScriptWidthUint32,
    USB_XHCI0_REG(0x10),  EfiBootScriptWidthUint32,
    USB_XHCI0_REG(0x2C),  EfiBootScriptWidthUint32,
    SD_REG(0x2C),  EfiBootScriptWidthUint32,
};
UINTN gPciRegScriptSize = \
        sizeof(gPciRegScriptTbl) / sizeof(BOOT_SCRIPT_PCI_REGISTER_SAVE);

ACPI_REG_WRITE Oem_SbMmioProgrammingDXETable[] =
{
    {00, 00, 0xB0, 0xAC}, // Don't delete this line! it is OEM program Signature for CIMX

    // SciMap0 [SMI_Reg: 40h - 57h]: Mapping of GEVENT20 to one of 32 Event_Status.
    // 00000: map event source 20 to the input of Event_Status bit 0
    // 00001: map event source 20 to the input of Event_Status bit 1
    // .
    // 11111: map event source 20 to the input of Event_Status bit 31

//    {SMI_BASE >> 8, FCH_SMI_Gevent0, 0, GEVENT0_MAP_EVENTBIT},
//    {SMI_BASE >> 8, FCH_SMI_Gevent1, 0, GEVENT1_MAP_EVENTBIT},
//    {SMI_BASE >> 8, FCH_SMI_Gevent2, 0, GEVENT2_MAP_EVENTBIT},
//    {SMI_BASE >> 8, FCH_SMI_Gevent3, 0, GEVENT3_MAP_EVENTBIT},        // map EC SCI to _L03 
//    {SMI_BASE >> 8, FCH_SMI_Gevent4, 0, GEVENT4_MAP_EVENTBIT},
//    {SMI_BASE >> 8, FCH_SMI_Gevent5, 0, GEVENT5_MAP_EVENTBIT},        // map PME wakeup to _L05 
//    {SMI_BASE >> 8, FCH_SMI_Gevent6, 0, GEVENT6_MAP_EVENTBIT},
//    {SMI_BASE >> 8, FCH_SMI_Gevent7, 0, GEVENT7_MAP_EVENTBIT},
//    {SMI_BASE >> 8, FCH_SMI_Gevent8, 0, GEVENT8_MAP_EVENTBIT},        // map PCIEExpress hot plug to _L08
//    {SMI_BASE >> 8, FCH_SMI_Gevent9, 0, GEVENT9_MAP_EVENTBIT},        // map RadioButton to _L09
//    {SMI_BASE >> 8, FCH_SMI_Gevent10, 0, GEVENT10_MAP_EVENTBIT},
//    {SMI_BASE >> 8, FCH_SMI_Gevent11, 0, GEVENT11_MAP_EVENTBIT},
//    {SMI_BASE >> 8, FCH_SMI_Gevent12, 0, GEVENT12_MAP_EVENTBIT},
//    {SMI_BASE >> 8, FCH_SMI_Gevent13, 0, GEVENT13_MAP_EVENTBIT},
//    {SMI_BASE >> 8, FCH_SMI_Gevent14, 0, GEVENT14_MAP_EVENTBIT},
//    {SMI_BASE >> 8, FCH_SMI_Gevent15, 0, GEVENT15_MAP_EVENTBIT},   // map ODD event to _L0F
//    {SMI_BASE >> 8, FCH_SMI_Gevent16, 0, GEVENT16_MAP_EVENTBIT},   
//    {SMI_BASE >> 8, FCH_SMI_Gevent17, 0, GEVENT17_MAP_EVENTBIT},   // ODD_DA# event to _L10
//    {SMI_BASE >> 8, FCH_SMI_Gevent18, 0, GEVENT18_MAP_EVENTBIT},
//    {SMI_BASE >> 8, FCH_SMI_Gevent19, 0, GEVENT19_MAP_EVENTBIT},
//    {SMI_BASE >> 8, FCH_SMI_Gevent20, 0, GEVENT20_MAP_EVENTBIT},
//    {SMI_BASE >> 8, FCH_SMI_Gevent21, 0, GEVENT21_MAP_EVENTBIT},
//    {SMI_BASE >> 8, FCH_SMI_Gevent22, 0, GEVENT22_MAP_EVENTBIT},   // map LID to _L16
//    {SMI_BASE >> 8, FCH_SMI_Gevent23, 0, GEVENT23_MAP_EVENTBIT},

    {SMI_BASE >> 8, FCH_SMI_xHC0Pme, 0, XHCI0_PME_MAP_EVENTBIT},                    // map USB device 10 func0 PME to ACPI _L18
    {SMI_BASE >> 8, FCH_SMI_xHC1Pme, 0, XHCI1_PME_MAP_EVENTBIT},                    // map USB device 10 func1 PME to ACPI _L18
    {SMI_BASE >> 8, FCH_SMI_Usbwakup0, 0, USB_WAKUP0_MAP_EVENTBIT},               // map USB device 18 PME to ACPI _L18
    {SMI_BASE >> 8, FCH_SMI_Usbwakup1, 0, USB_WAKUP1_MAP_EVENTBIT},               // map USB device 19 PME to ACPI _L18
    {SMI_BASE >> 8, FCH_SMI_Usbwakup2, 0, USB_WAKUP2_MAP_EVENTBIT},               // map USB device 20 PME to ACPI _L18
    {SMI_BASE >> 8, FCH_SMI_Usbwakup3, 0, USB_WAKUP3_MAP_EVENTBIT},               // map USB device 22 PME to ACPI _L18

    {SMI_BASE >> 8, FCH_SMI_SBGppPme0, 0, SBGPP_PME0_MAP_EVENTBIT},
    {SMI_BASE >> 8, FCH_SMI_SBGppPme1, 0, SBGPP_PME1_MAP_EVENTBIT},
    {SMI_BASE >> 8, FCH_SMI_SBGppPme2, 0, SBGPP_PME2_MAP_EVENTBIT},
    {SMI_BASE >> 8, FCH_SMI_SBGppPme3, 0, SBGPP_PME3_MAP_EVENTBIT},

    {SMI_BASE >> 8, FCH_SMI_SBGppHp0, 0, SBGPP_HP0_MAP_EVENTBIT},
    {SMI_BASE >> 8, FCH_SMI_SBGppHp1, 0, SBGPP_HP1_MAP_EVENTBIT},
    {SMI_BASE >> 8, FCH_SMI_SBGppHp2, 0, SBGPP_HP2_MAP_EVENTBIT},
    {SMI_BASE >> 8, FCH_SMI_SBGppHp3, 0, SBGPP_HP3_MAP_EVENTBIT},
    
    {SMI_BASE >> 8, FCH_SMI_SataGevent0, 0, SATA_GEVENT0_MAP_EVENTBIT},
    {SMI_BASE >> 8, FCH_SMI_SataGevent1, 0, SATA_GEVENT1_MAP_EVENTBIT},
//    {SMI_BASE >> 8, FCH_SMI_GecPme,      0, GEC_PME_MAP_EVENTBIT},      // map GEC PME to ACPI _L1A
    {SMI_BASE >> 8, FCH_SMI_IMCGevent0,  0, IMC_GEVENT0_MAP_EVENTBIT},
    {SMI_BASE >> 8, FCH_SMI_IMCGevent1,  0, IMC_GEVENT1_MAP_EVENTBIT},

    {SMI_BASE >> 8, FCH_SMI_CIRPme,        0, CIR_PME_MAP_EVENTBIT},
    {SMI_BASE >> 8, FCH_SMI_WakePinGevent, 0, WAKEPIN_GEVENT_MAP_EVENTBIT},
    {SMI_BASE >> 8, FCH_SMI_FanThGevent,   0, FANTH_GEVENT_MAP_EVENTBIT},
    {SMI_BASE >> 8, FCH_SMI_ASFMasterIntr, 0, ASF_MASTERINTR_MAP_EVENTBIT},
    {SMI_BASE >> 8, FCH_SMI_ASFSlaveIntr,  0, ASF_SLAVEINTR_MAP_EVENTBIT},
    {SMI_BASE >> 8, FCH_SMI_SMBUS0,        0, SMBUS0_MAP_EVENTBIT},
    {SMI_BASE >> 8, FCH_SMI_TWARN,         0, TWARN_MAP_EVENTBIT},
    {SMI_BASE >> 8, FCH_SMI_TMI,           0, TMI_MAP_EVENTBIT},

    {0xff, 0xff, 0xff, 0xff},
};



// External Declaration(s)

extern UINT8 FchAcpiGeventMapTbl[];
extern UINTN FchAcpiGeventMapTblSize;

// Function Definition(s)



/**
    Check the device.

               
    @param PciAddr which PCI device
    @param SidReg 

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

    @note  NONE

**/

EFI_STATUS
IsSBDevice (
    IN  UINT64      PciAddr,
    OUT UINT8       *SidReg
    )
{
    UINTN   j = 0;
    AMI_SB_PCI_SSID_TABLE_STRUCT DefaultSIdTbl[] = {SB_PCI_DEVICES_SSID_TABLE};
    AMI_SB_PCI_SSID_TABLE_STRUCT *SsidTblPtr = DefaultSIdTbl;

    while (SsidTblPtr[j].PciAddr != 0xffffffffffffffff) {
        if (SsidTblPtr[j].PciAddr == PciAddr) {
            *SidReg = PCI_SVID; // 0x2c Subsystem VendorID register
            return EFI_SUCCESS;
        }
        j++;
    }

    return EFI_NOT_FOUND;
}

/**
    This function programs SB PCI devices sub-vendor ID and
    sub-system ID.

    @param VOID

    @retval VOID

    @note  This routine only programs the PCI device in SB, hence, we
              have to check the bus/device/function numbers whether they
              are a SB PCI device or not.
**/
VOID ProgramSBSubId ()
{
    EFI_STATUS						Status = EFI_SUCCESS;
    UINTN							i;
    UINT32							PciSsid = 0xffffffff;
	PCI_ADDR        				PciAddress;
    UINT8							PciSsidReg = 0xff;
    AMI_SB_PCI_SSID_TABLE_STRUCT 	DefaultSIdTbl[] = {SB_PCI_DEVICES_SSID_TABLE};
    AMI_SB_PCI_SSID_TABLE_STRUCT 	*SsidTblPtr = DefaultSIdTbl;
    UINT8                           Value;
    
    if (gBootScriptSave == NULL) {
        Status = pBS->LocateProtocol(&gEfiS3SaveStateProtocolGuid,NULL, \
                    &gBootScriptSave);
    }
    
    SsidTblPtr = (AMI_SB_PCI_SSID_TABLE_STRUCT*)PcdGet32(PcdSbSsidTablePtr);
    if (SsidTblPtr == NULL) {
    	SsidTblPtr = DefaultSIdTbl;
    }

    i = 0;
    while (SsidTblPtr[i].PciAddr != 0xffffffffffffffff) {
        if (IsSBDevice(SsidTblPtr[i].PciAddr, &PciSsidReg) == EFI_SUCCESS) {
			if (SsidTblPtr[i].Sid != 0xffffffff) {
				//Transform UINT8 PciAddr to UINT64 PciAddr    
				PciAddress.Address.Segment = 0;
				PciAddress.Address.Bus = 0x00;
				PciAddress.Address.Device = (SsidTblPtr[i].PciAddr & 0xF8) >> 3;
				PciAddress.Address.Function = SsidTblPtr[i].PciAddr & 0x07;
				PciAddress.Address.Register = 0x2c;
				
				if(PciAddress.Address.Device==FCH_SATA1_DEV&&PciAddress.Address.Function==FCH_SATA1_FUNC){
					Value = READ_PCI8(0, PciAddress.Address.Device, PciAddress.Address.Function, 0x40);
					WRITE_PCI8_S3(gBootScriptSave,0, PciAddress.Address.Device, PciAddress.Address.Function, 0x40, Value|BIT0);
					WRITE_PCI32_S3(gBootScriptSave,0, PciAddress.Address.Device, PciAddress.Address.Function, 0x2c,  SsidTblPtr[i].Sid);
					WRITE_PCI8_S3(gBootScriptSave,0, PciAddress.Address.Device, PciAddress.Address.Function, 0x40, Value);
					} else {	
					WRITE_PCI32_S3(gBootScriptSave,0, PciAddress.Address.Device, PciAddress.Address.Function, 0x2c,  SsidTblPtr[i].Sid);
					}
			} else {
				//Transform UINT8 PciAddr to UINT64 PciAddr
				PciAddress.Address.Segment = 0;
				PciAddress.Address.Bus = 0x00;
				PciAddress.Address.Device = (SsidTblPtr[i].PciAddr & 0xF8) >> 3;
				PciAddress.Address.Function = SsidTblPtr[i].PciAddr & 0x07;

				PciSsid = READ_PCI32(0, PciAddress.Address.Device, PciAddress.Address.Function, 0x00);				
			        WRITE_PCI32_S3(gBootScriptSave,0, PciAddress.Address.Device, PciAddress.Address.Function, 0x2c, PciSsid);
				}
			}
        i++;
    }
}

/**
    Entry point of the HYGON SB DXE Interface driver
    Platform support for SB DXE driver

        
    @param ImageHandle Pointer to the firmware file system header
    @param SystemTable Pointer to System table

         
    @retval EFI_SUCCESS Driver loading successfully
    @retval EFI_ERROR Driver loading failed
**/

EFI_STATUS
HygonSbDxeInterfaceInit(
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
    )
{
    EFI_STATUS      Status = EFI_SUCCESS;
    EFI_HANDLE      Handle = NULL;
    SB_SETUP_DATA   SbSetupData;
    FCH_DATA_BLOCK  *FchData;
#if HWM_DISPLAY_SUPPORT
    UINT8           i;
#endif

    InitAmiLib(ImageHandle, SystemTable);

    Status = pBS->AllocatePool(EfiACPIMemoryNVS,
                        sizeof(HYGON_FCH_AMI_POLICY_PROTOCOL),
                        &gHygonFchAmiPolicy \
                        );
    if (!EFI_ERROR(Status)) {
        pBS->SetMem(gHygonFchAmiPolicy,
                sizeof(HYGON_FCH_AMI_POLICY_PROTOCOL), 0 \
                );
    } else {
        return Status;
    }

    gHygonFchAmiPolicy->HookInitEnv = HookInitEnv;
    gHygonFchAmiPolicy->HookInitMid = HookInitMid;
    gHygonFchAmiPolicy->HookInitLate = HookInitLate;
    gHygonFchAmiPolicy->HookCustomizeS3Save = HookCustomizeS3Save;
    gHygonFchAmiPolicy->HookAfterInitEnv = HookAfterInitEnv;
    gHygonFchAmiPolicy->HookAfterInitMid = HookAfterInitMid;
    gHygonFchAmiPolicy->HookAfterInitLate = HookAfterInitLate;
    gHygonFchAmiPolicy->HookAgesaFchOemCalloutDxe = HookAgesaFchOemCalloutDxe;

#ifdef  SB900_EC_SUPPORT
#if SB900_EC_SUPPORT
    DetecThermalCap();
#endif
#endif  // SB900_EC_SUPPORT

    //
    // Get SETUP DATA parameters
    //
    GetSbSetupData(pRS, &SbSetupData, FALSE);

    gHygonFchAmiPolicy->HygonSafeDefault = SbSetupData.AmdSafeDefault;
    if (!gHygonFchAmiPolicy->HygonSafeDefault) {
        //======================================================================
        // assign FCH parameters here.
        FchData = &gHygonFchAmiPolicy->HygonParameters;
        
        PcdSetBool (PcdSpreadSpectrum, SbSetupData.HwAcpi.SpreadSpectrum);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)

        if (!AmdCbsPkg_SUPPORT){
            PcdSet8 (PcdPwrFailShadow, SbSetupData.HwAcpi.PwrFailShadow);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        }
            
        FchData->HwAcpi.MtC1eEnable = SbSetupData.HwAcpi.MtC1eEnable;  // Setup Option (SB.SD) - Used from data struc (TSHwAcpiLateService.c)
#if SB_POWER_SAVING_SUPPORT
        PcdSet8 (PcdAbClockGating,SbSetupData.Ab.AbClockGating);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
#endif
        
        FchData->Sata.SataMode.SataSetMaxGen2 = SbSetupData.Sata.SataMode.SataSetMaxGen2;  // Setup Option (SB.SD) - Used from data struc (TSSataLib.c)
        if (!AmdCbsPkg_SUPPORT){
        	FchData->Sata.SataClass = SbSetupData.Sata.SataClass;  // Used later in this routine
        }
        PcdSet8 (PcdSataClass,SbSetupData.Sata.SataClass);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataEspPort0,SbSetupData.Sata.SataEspPort.Port0);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataEspPort1,SbSetupData.Sata.SataEspPort.Port1);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataEspPort2,SbSetupData.Sata.SataEspPort.Port2);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataEspPort3,SbSetupData.Sata.SataEspPort.Port3);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataEspPort4,SbSetupData.Sata.SataEspPort.Port4);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataEspPort5,SbSetupData.Sata.SataEspPort.Port5);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataEspPort6,SbSetupData.Sata.SataEspPort.Port6);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataEspPort7,SbSetupData.Sata.SataEspPort.Port7);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataPortPowerPort0,SbSetupData.Sata.SataPortPower.Port0);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataPortPowerPort1,SbSetupData.Sata.SataPortPower.Port1);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataPortPowerPort2,SbSetupData.Sata.SataPortPower.Port2);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataPortPowerPort3,SbSetupData.Sata.SataPortPower.Port3);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataPortPowerPort4,SbSetupData.Sata.SataPortPower.Port4);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataPortPowerPort5,SbSetupData.Sata.SataPortPower.Port5);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataPortPowerPort6,SbSetupData.Sata.SataPortPower.Port6);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataPortPowerPort7,SbSetupData.Sata.SataPortPower.Port7);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataModePort0,SbSetupData.Sata.SataPortMd.Port0);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataModePort1,SbSetupData.Sata.SataPortMd.Port1);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataModePort2,SbSetupData.Sata.SataPortMd.Port2);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataModePort3,SbSetupData.Sata.SataPortMd.Port3);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataModePort4,SbSetupData.Sata.SataPortMd.Port4);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataModePort5,SbSetupData.Sata.SataPortMd.Port5);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataModePort6,SbSetupData.Sata.SataPortMd.Port6);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataModePort7,SbSetupData.Sata.SataPortMd.Port7);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataAggrLinkPmCap,SbSetupData.Sata.SataAggrLinkPmCap);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataPortMultCap,SbSetupData.Sata.SataPortMultCap);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataClkAutoOff,SbSetupData.Sata.SataClkAutoOff);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataPscCap,SbSetupData.Sata.SataPscCap);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataFisBasedSwitching,SbSetupData.Sata.SataFisBasedSwitching);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataCccSupport,SbSetupData.Sata.SataCccSupport);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataSscCap,SbSetupData.Sata.SataSscCap);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataTargetSupport8Device,SbSetupData.Sata.SataTargetSupport8Device);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataDisableGenericMode,SbSetupData.Sata.SataDisableGenericMode);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataAhciEnclosureManagement,SbSetupData.Sata.SataAhciEnclosureManagement);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataSgpio0,SbSetupData.Sata.SataSgpio0);   // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdSataPhyPllShutDown,SbSetupData.Sata.SataPhyPllShutDown);  // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSetBool (PcdSataDevSlpPort0,FCH_SATA_DEVSLP_PORT0);  // SDL Token (SB.SDL) - Set by PCD (FchDxe.c)
        PcdSetBool (PcdSataDevSlpPort1,FCH_SATA_DEVSLP_PORT1);  // SDL Token (SB.SDL) - Set by PCD (FchDxe.c)
        if (!AmdCbsPkg_SUPPORT){
        	PcdSet32 (FchRTDeviceEnableMap,SbSetupData.FchDevEnMap.FchDeviceEnableMap);  // Hardcoded to 0 (SbGetSetupData.c) - Set by PCD (FchDxe.c)
        }
        
        FchData->Spi.LpcMsiEnable = SbSetupData.Spi.LpcMsiEnable;  // Setup Option (SB.SD) - FCH data used (TSLpcEnv.c)
        
        if (!AmdCbsPkg_SUPPORT){
        	PcdSet8 (PcdSdConfig,SbSetupData.Sd.SdConfig);   // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        }
        FchData->Sd.SdForce18 = SbSetupData.Sd.SdForce18;  // Setup Option (SB.SD) - FCH data used (TSSdEnvService.c)
        PcdSet8 (PcdSdDbgConfig,SbSetupData.Sd.SdDbgConfig);   // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        
        PcdSetBool (PcdHpetEnable,SbSetupData.Hpet.HpetEnable);   // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSetBool (PcdHpetMsiDis,SbSetupData.Hpet.HpetMsiDis);   // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        
        FchData->Gcpu.AcDcMsg = SbSetupData.Gcpu.AcDcMsg;  // Setup Option (SB.SD) - FCH data used (TSHwAcpiLateService.c)
        PcdSet8 (PcdTimerTickTrack,SbSetupData.Gcpu.TimerTickTrack);   // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        PcdSet8 (PcdClockInterruptTag,SbSetupData.Gcpu.ClockInterruptTag);   // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
        FchData->Gcpu.EhciTrafficHanding = SbSetupData.Gcpu.EhciTrafficHanding;  // Setup Option (SB.SD) - FCH data used (TSHwAcpiLateService.c)
        FchData->Gcpu.GcpuMsgCMultiCore = SbSetupData.Gcpu.GcpuMsgCMultiCore;  // Setup Option (SB.SD) - FCH data used (TSHwAcpiLateService.c)
        FchData->Gcpu.GcpuMsgCStage = SbSetupData.Gcpu.GcpuMsgCStage;  // Setup Option (SB.SD) - FCH data used (TSHwAcpiLateService.c)

        FchData->Misc.NativePcieSupport = SbSetupData.Misc.NativePcieSupport;   // Setup Option (SB.SD) - FCH data used (TSHwAcpiLateService.c)
        PcdSetBool (PcdNoneSioKbcSupport,NONE_SIO_KBC_SUPPORT);   // Setup Option (SB.SD) - Set by PCD (FchDxe.c)
    }

    Status = ReserveSbMmioProgrammingDXETable(FchData);
    if(EFI_ERROR(Status)) return Status;

    gHygonFchAmiPolicy->SataClass = FchData->Sata.SataClass;  // Updated earlier in routine - FCH data used (SbDxe.c)

    Status = pBS->InstallProtocolInterface (
                &Handle,
                &gHygonFchAmiPolicyProtocolGuid,
                EFI_NATIVE_INTERFACE,
                gHygonFchAmiPolicy);
    if(EFI_ERROR(Status)) return Status;

	
    TRACE ((-1, "FchRTDeviceEnableMap=0x%x in %a\n", PcdGet32(FchRTDeviceEnableMap), __FUNCTION__));
//    TRACE ((-1, "FchAl2AhbLegacyUartIoEnable=0x%x in %a\n", PcdGet16(FchAl2AhbLegacyUartIoEnable), __FUNCTION__));

    return Status;
}

/**
    This is the stub function which will be called after Agesa AmdInitLate
    enrty point


    @param This 
    @param InitAgesaHook 

    @retval VOID

**/

VOID
HookAgesaFchOemCalloutDxe (
  IN HYGON_FCH_AMI_POLICY_PROTOCOL    *This,
  IN VOID                           *InitAgesaHook
  )
{
    FCH_DATA_BLOCK              *FchData = InitAgesaHook, *HygonParameters = &This->HygonParameters;
#if HWM_DISPLAY_SUPPORT
    UINT8                       i;
#endif

    //
    // Assign AGESA-FCH parameters
    //
    if (!This->HygonSafeDefault) {
    	
        FchData->HwAcpi.MtC1eEnable = HygonParameters->HwAcpi.MtC1eEnable;
        FchData->Sata.SataMode.SataSetMaxGen2 = HygonParameters->Sata.SataMode.SataSetMaxGen2;
        FchData->Sata.SataClass = HygonParameters->Sata.SataClass;
        FchData->Spi.LpcMsiEnable = HygonParameters->Spi.LpcMsiEnable;
        FchData->Sd.SdForce18 = HygonParameters->Sd.SdForce18;
        FchData->Gcpu.AcDcMsg = HygonParameters->Gcpu.AcDcMsg;
        FchData->Gcpu.EhciTrafficHanding = HygonParameters->Gcpu.EhciTrafficHanding;
        FchData->Gcpu.GcpuMsgCMultiCore = HygonParameters->Gcpu.GcpuMsgCMultiCore;
        FchData->Gcpu.GcpuMsgCStage = HygonParameters->Gcpu.GcpuMsgCStage;
        FchData->Misc.NativePcieSupport = HygonParameters->Misc.NativePcieSupport;
    }
}

/**
    This is the stub function lock or unlock the XHCI Vid/DID


               
    @param This 
    @param TRUE:lock, FALSE:Unlock 

    @retval VOID

**/

VOID HygonXhciVidDidLock (BOOLEAN Lock)
{
    UINT32 Buffer,i;
    
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS	CONST XhciDevice[2] = {
            {0, 0, 0x10, 0, 0},
            {0, 1, 0x10, 0, 0}
    };
    
    for (i = 0; i < sizeof(XhciDevice)/sizeof(EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS); i++) {

        Buffer = PciRead32(PCI_LIB_ADDRESS(XhciDevice[i].Bus, XhciDevice[i].Device, XhciDevice[i].Function, 0));
        if (Buffer != -1) {
            PciWrite32(PCI_LIB_ADDRESS(XhciDevice[i].Bus, XhciDevice[i].Device, XhciDevice[i].Function, 0x48), 0x4);
            Buffer = PciRead32(PCI_LIB_ADDRESS(XhciDevice[i].Bus, XhciDevice[i].Device, XhciDevice[i].Function, 0x4C));
            Buffer &= ~BIT8;
            Buffer |= (UINT32)(Lock << 8);
            PciWrite32(PCI_LIB_ADDRESS(XhciDevice[i].Bus, XhciDevice[i].Device, XhciDevice[i].Function, 0x48), 0x4);
            PciWrite32(PCI_LIB_ADDRESS(XhciDevice[i].Bus, XhciDevice[i].Device, XhciDevice[i].Function, 0x4C), Buffer);
        }
    }
}


/**
 * Program SpreadSpectrum  ,it need clear boot fail flag.
 *    
 * @param[in]   Void
 *
 */
#if defined(POST_TIME_TUNING_SUPPORT) && (POST_TIME_TUNING_SUPPORT == 1)
#if defined(LAST_BOOT_FAIL_MECHANISM) && (LAST_BOOT_FAIL_MECHANISM == 1)
VOID 
SpreadSpectrumClearBootFailFlag (
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINT32 LastBootFailed;
    UINTN Size = sizeof(UINT32);
    EFI_GUID FastBootVariableGuid = FAST_BOOT_VARIABLE_GUID;

    SB_SETUP_DATA   SbSetupData;
    
    GetSbSetupData(pRS, &SbSetupData, FALSE);
        
    if (SbSetupData.HwAcpi.SpreadSpectrum) {
        if (( ACPIMMIO32 (FCH_MISCx40_MiscClkCntl1) & FCH_MISCx40_MiscClkCntl1_CG1PLL_FBDIV_Test ) == 0) {
            Status = pRS->GetVariable(L"LastBootFailed", &FastBootVariableGuid, NULL, &Size, &LastBootFailed);
            if (!EFI_ERROR(Status)) {
                pRS->SetVariable(L"LastBootFailed", 
                                          &FastBootVariableGuid, 
                                          EFI_VARIABLE_NON_VOLATILE,
                                          0,
                                          &LastBootFailed);
            }
        }
    }
}
#endif //defined(LAST_BOOT_FAIL_MECHANISM) && (LAST_BOOT_FAIL_MECHANISM == 1)
#endif // defined(POST_TIME_TUNING_SUPPORT) && (POST_TIME_TUNING_SUPPORT == 1)
/**
    This is the stub function will call the host environment through the binary block
    interface (call-out port) to provide a user hook opportunity


               
    @param This 
    @param InitAgesaHook 

    @retval VOID

**/

VOID
HookInitEnv (
  IN HYGON_FCH_AMI_POLICY_PROTOCOL    *This,
  IN VOID                           *InitAgesaHook
  )
{
    EFI_STATUS      Status;
    UINT8           SwSmi;
    UINT32          SetBits, MaskBits;
    UINT64          Address;
	
    SetBits = BIT11;
    MaskBits = -1;
    Address = ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG88;
	
    if (gBootScriptSave == NULL) {
        Status = pBS->LocateProtocol(&gEfiS3SaveStateProtocolGuid,NULL, \
                    &gBootScriptSave);
    }

	IsBeforePciRestoreSwSmiDone = FALSE;
    // if BootScript is not ready save it after BootScript gets ready.
    if (gBootScriptSave != NULL) {
		BOOT_SCRIPT_S3_MEM_READ_WRITE_MACRO( gBootScriptSave, \
											 EfiBootScriptWidthUint32, \
											 Address, &SetBits, &MaskBits
											 );

		//
		// Enable CmdPort SMI
		//
		//###<AAV> ###SetBits = BIT06;
		//###<AAV> ###ResetBits = BIT07 + BIT06;
		//###<AAV> ###Address = ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB2;
		//<AAV> We aligned it 32bit. Maybe base library is bad. : find out root cause.
		SetBits = BIT22;
		MaskBits = ~(BIT23 + BIT22);
		Address = ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB0;
		BOOT_SCRIPT_S3_MEM_READ_WRITE_MACRO( gBootScriptSave, \
											 EfiBootScriptWidthUint32, \
											 Address, &SetBits, &MaskBits
											 );

		//
		// Clear SmiEnB to enable SMI function
		//
		SetBits = 0;
		MaskBits = ~BIT31;
		Address = ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98;
		BOOT_SCRIPT_S3_MEM_READ_WRITE_MACRO( gBootScriptSave, \
											 EfiBootScriptWidthUint32, \
											 Address, &SetBits, &MaskBits
											 );

		//
		// Set the EOS Bit
		//
		SetBits = BIT28;
		MaskBits = -1;
		Address = ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98;
		BOOT_SCRIPT_S3_MEM_READ_WRITE_MACRO( gBootScriptSave, \
											 EfiBootScriptWidthUint32, \
											 Address, &SetBits, &MaskBits
											 );
		// issue the Fch S3 resume.
		SwSmi = SB_BEFORE_PCI_RESTORE_SWSMI;
		BOOT_SCRIPT_S3_IO_WRITE_MACRO(gBootScriptSave, \
				EfiBootScriptWidthUint8, \
				SW_SMI_IO_ADDRESS, 1, &SwSmi \
				);
		IsBeforePciRestoreSwSmiDone = TRUE;
    }
    
    #if defined(POST_TIME_TUNING_SUPPORT) && (POST_TIME_TUNING_SUPPORT == 1)
    #if defined(LAST_BOOT_FAIL_MECHANISM) && (LAST_BOOT_FAIL_MECHANISM == 1)
    SpreadSpectrumClearBootFailFlag();
    #endif
    #endif
}

/**
    This is the stub function will call the host environment through the binary block
    interface (call-out port) to provide a user hook opportunity


               
    @param This 
    @param InitAgesaHook 

    @retval VOID

**/

VOID
HookInitMid (
  IN HYGON_FCH_AMI_POLICY_PROTOCOL    *This,
  IN VOID                           *InitAgesaHook
  )
{
    // unlock VID/DID.
    //   We lock it in the HookAfterInitMid.
    //   Because of the EIP#107491 do a dummy write while PciBus Start.
    //   So unlock it here maybe HYGON reference need to program the VID/DID.
   HygonXhciVidDidLock(FALSE);
   
}
/**
    This is the stub function will call the host environment through the binary block
    interface (call-out port) to provide a user hook opportunity


               
    @param This 
    @param InitAgesaHook 

    @retval VOID

**/

VOID
HookInitLate (
  IN HYGON_FCH_AMI_POLICY_PROTOCOL    *This,
  IN VOID                           *InitAgesaHook
  )
{

}

/**
    This is the stub function will call the host environment through the binary block
    interface (call-out port) to provide a user hook opportunity


               
    @param This 
    @param InitAgesaHook 

    @retval VOID

**/

VOID
HookCustomizeS3Save (
  IN HYGON_FCH_AMI_POLICY_PROTOCOL    *This,
  IN VOID                           *InitAgesaHook
  )
{
    EFI_STATUS      Status;
    UINT8           SwSmi;
    UINT32          SetBits, MaskBits;
    UINT64          Address;
    UINTN           Buffer,i;

    if (gPciRootBridgeIo == NULL) {
        Status = pBS->LocateProtocol(&gEfiPciRootBridgeIoProtocolGuid, NULL, \
                    &gPciRootBridgeIo);
        ASSERT_EFI_ERROR(Status);
    }

    if (gBootScriptSave == NULL) {
        Status = pBS->LocateProtocol(&gEfiS3SaveStateProtocolGuid,NULL, \
                    &gBootScriptSave);
        ASSERT_EFI_ERROR(Status);
    }
    if (gBootScriptSave == NULL && gPciRootBridgeIo == NULL) {
        PROGRESS_CODE(DXE_SB_ERROR);
        EFI_DEADLOOP();
    }

    for (i = 0; i < gPciRegScriptSize; i++) {
        gPciRootBridgeIo->Pci.Read(
                        gPciRootBridgeIo,
                        gPciRegScriptTbl[i].Width,
                        gPciRegScriptTbl[i].Address,
                        1,
                        &Buffer);

        BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(
                        gBootScriptSave,
                        gPciRegScriptTbl[i].Width,
                        gPciRegScriptTbl[i].Address,
                        1,
                        &Buffer);
    }

    if (!IsBeforePciRestoreSwSmiDone) {
		//
		// Clear SmiCmdPort Status Bit
		//
		SetBits = BIT11;
		MaskBits = -1;
		Address = ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG88;
		BOOT_SCRIPT_S3_MEM_READ_WRITE_MACRO( gBootScriptSave, \
											 EfiBootScriptWidthUint32, \
											 Address, &SetBits, &MaskBits
											 );

		//
		// Enable CmdPort SMI
		//
		//###<AAV> ###SetBits = BIT06;
		//###<AAV> ###ResetBits = BIT07 + BIT06;
		//###<AAV> ###Address = ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB2;
		//<AAV> We aligned it 32bit. Maybe base library is bad. : find out root cause.
		SetBits = BIT22;
		MaskBits = ~(BIT23 + BIT22);
		Address = ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB0;
		BOOT_SCRIPT_S3_MEM_READ_WRITE_MACRO( gBootScriptSave, \
											 EfiBootScriptWidthUint32, \
											 Address, &SetBits, &MaskBits
											 );

		//
		// Clear SmiEnB to enable SMI function
		//
		SetBits = 0;
		MaskBits = ~BIT31;
		Address = ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98;
		BOOT_SCRIPT_S3_MEM_READ_WRITE_MACRO( gBootScriptSave, \
											 EfiBootScriptWidthUint32, \
											 Address, &SetBits, &MaskBits
											 );

		//
		// Set the EOS Bit
		//
		SetBits = BIT28;
		MaskBits = -1;
		Address = ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98;
		BOOT_SCRIPT_S3_MEM_READ_WRITE_MACRO( gBootScriptSave, \
											 EfiBootScriptWidthUint32, \
											 Address, &SetBits, &MaskBits
											 );
		// issue the Fch S3 resume.
		SwSmi = SB_BEFORE_PCI_RESTORE_SWSMI;
		BOOT_SCRIPT_S3_IO_WRITE_MACRO(gBootScriptSave, \
				EfiBootScriptWidthUint8, \
				SW_SMI_IO_ADDRESS, 1, &SwSmi \
				);
    }

    // issue the Fch S3 resume.
    SwSmi = SB_AFTER_PCI_RESTORE_SWSMI;
    BOOT_SCRIPT_S3_IO_WRITE_MACRO(gBootScriptSave, \
            EfiBootScriptWidthUint8, \
            SW_SMI_IO_ADDRESS, 1, &SwSmi \
            );
}

/**
    This is the stub function which will be called after Agesa AmdInitEnv
    enrty point


               
    @param This 
    @param InitAgesaHook 

    @retval VOID

**/

VOID
HookAfterInitEnv (
  IN HYGON_FCH_AMI_POLICY_PROTOCOL    *This,
  IN VOID                           *InitAgesaHook
  )
{
    ProgramSBSubId();

    // To patch XHCI disappear. (EIP#117779)
    // lock VID/DID because of the EIP#107491 do a dummy write
    // and the HYGON XHCI VID/DID is writable this moment.
    HygonXhciVidDidLock(TRUE);
    
}

/**
    This is the stub function which will be called after Agesa AmdInitMid
    enrty point


               
    @param This 
    @param InitAgesaHook 

    @retval VOID

**/

VOID
HookAfterInitMid (
  IN HYGON_FCH_AMI_POLICY_PROTOCOL    *This,
  IN VOID                           *InitAgesaHook
  )
{

    // Lock VID/DID 
    // Result in we unlcok it in HookInitMid for HYGON refernce program Vid/DID
    HygonXhciVidDidLock(TRUE);
}

/**
    This is the stub function which will be called after Agesa AmdInitLate
    enrty point


               
    @param This 
    @param InitAgesaHook 

    @retval VOID

**/

VOID
HookAfterInitLate (
  IN HYGON_FCH_AMI_POLICY_PROTOCOL    *This,
  IN VOID                           *InitAgesaHook
  )
{

}

/**
    Reserved SbMmioProgrammingDXETable

        
    @param pConfig Southbridge configuration structure pointer

         
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
EFI_STATUS
ReserveSbMmioProgrammingDXETable(
  IN OUT    FCH_DATA_BLOCK *FchData
  )
{
	EFI_STATUS		Status;
	VOID			*ReserveSbMmioProgrammingDXEPtr = NULL;

    Status = pBS->AllocatePool(
                            EfiACPIMemoryNVS,
                            sizeof (Oem_SbMmioProgrammingDXETable),
                            &ReserveSbMmioProgrammingDXEPtr);
    if(EFI_ERROR(Status)) return Status;

    pBS->CopyMem (
             ReserveSbMmioProgrammingDXEPtr,
             Oem_SbMmioProgrammingDXETable,
             sizeof (Oem_SbMmioProgrammingDXETable)
             );

    FchData->HwAcpi.OemProgrammingTablePtr = (VOID*)ReserveSbMmioProgrammingDXEPtr;

    return EFI_SUCCESS;
}

#ifdef  SB900_EC_SUPPORT
#if SB900_EC_SUPPORT
/**

    @param VOID

             
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
EFI_STATUS EFIAPI DetecThermalCap(
)
{
    EFI_STATUS  Status;
    UINT32      Value32;
    BOOLEAN     ThermalTripStatus;
    BOOLEAN     HtcCapable;

    UINTN       VariableSize    = sizeof(SETUP_DATA);
    UINT32      Attributes      = 0;
    EFI_GUID    gSetupGuid      = SETUP_GUID;
    SETUP_DATA  SetupData;

    Status = pRS->GetVariable( L"Setup", &gSetupGuid, &Attributes, &VariableSize, &SetupData );
    if (EFI_ERROR( Status )){
        return Status;
    }

    Value32 = READ_PCI32(0x01, 0x18, 0x03, 0xE4);
    ThermalTripStatus = ((Value32 & (UINT32)(1 << 5)) != 0);

    Value32 = READ_PCI32(0x01, 0x18, 0x03, 0xE8);
    HtcCapable = ((Value32 & (UINT32)(1 << 10)) != 0);

    SetupData.ThermalCap = (BOOLEAN)(ThermalTripStatus && HtcCapable);

    Status = pRS->SetVariable( L"Setup", &gSetupGuid, Attributes, VariableSize, &SetupData );
    ASSERT( !EFI_ERROR( Status )); 

    return  Status;

}
#endif
#endif  // SB900_EC_SUPPORT

#ifdef Richland_SUPPORT
#if Richland_SUPPORT
/**
    Detect Trinity-Hudson/Richland-Bolton platform,
    then pass the right xhci rom address.

    @param VOID

        UINT32
    @retval XHCI_BIN_TN_BASE Xhci rom address
    @retval XHCI_BIN_RL_BASE Xhci rom address

**/
UINT32 GetXhciRomAddr(
)
{
    UINT8   RevisionID;

    //read the SB revision ID
    RevisionID = READ_PCI8_SMBUS(0x8);

    //detect Trinity-Hudson/Richland-Bolton platform, and pass the right Xhci rom address.
    if (RevisionID < 0x15){
        return XHCI_BIN_TN_BASE;
    }
    else if (RevisionID >= 0x15){
        return XHCI_BIN_RL_BASE;
    }
    //can't detect, using the xhci address in rom sig.
    else {
        return NULL;
    }
}
#endif
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
