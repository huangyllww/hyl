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

//*************************************************************************
/** @file SbDxe.c
    This file contains code for Template Southbridge initialization
    in the DXE stage

**/
//*************************************************************************

#include <PiDxe.h>
#include <Library/PcdLib.h>

// Module specific Includes
#include <token.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Setup.h>
#include <AmiHobs.h>
#include <SB.h>
#include <AmiChipsetIoLib.h>
#include <AmiCspLib.h>
#include <Library/SbSetupData.h>
// Consumed Protocols
#include <Protocol/HygonSbPolicy.h>
#include <Protocol/SbSataProtocol.h>
#include <Protocol/PciIo.h>
#include <Protocol/Cpu.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/S3SaveState.h>
#include <Protocol/AmiBoardInfo2.h>

#if ACPI_SUPPORT

// The initial protocol defined in the UEFI2.1 spec.
#include <Protocol/AcpiTable.h>

#endif


// Produced Protocols
#include <Protocol/RealTimeClock.h>
#include <Protocol/WatchdogTimer.h>
#include <Protocol/Timer.h>
#include <Protocol/Legacy8259.h>

#if defined AMIUSB_SUPPORT && AMIUSB_SUPPORT == 1
#include <Protocol/AmiUsbController.h>              // (EIP53061+)
#include <Protocol/UsbPolicy.h>
#endif

#if defined(CSM_SUPPORT) && (CSM_SUPPORT == 1)
#include <Protocol/LegacyBiosExt.h>
#endif
// Build flag adjustments
#ifndef   SMM_SUPPORT
#define   SMM_SUPPORT     0
#endif

// Constant definitions
#define PCI_CFG_ADDR(bus,dev,func,reg) \
                        ((VOID*)(UINTN) (PcdGet64 (PcdPciExpressBaseAddress) + \
                        ((bus) << 20) + ((dev) << 15) + \
                        ((func) << 12) + reg))

//----------------------------------------------------------------------------
//      Timer Constants
//----------------------------------------------------------------------------
#define SYSTEM_TIMER_IRQ 0
// Now using Legacy 8259 protocol to initialize. Remove this line later.
//#define SYSTEM_TIMER_INTR_VECTOR (MASTER_INTERRUPT_BASE + SYSTEM_TIMER_IRQ)

// Timer Period
#define TIMER_TICK        838 //ns

// default duration is 0xffff ticks
#define DEFAULT_TICK_DURATION ((65535*838+50)/100)
#define MAX_TICK_DURATION DEFAULT_TICK_DURATION

//8259 PIC defines
#define ICW1            0x11    //Slave exists and ICW4 required.
#define ICW3_M          1<<2    //IRQ 2 connects to slave
#define ICW3_S          2       //IRQ 2 connects to master
#define ICW4            1       //Bit 4 Normal Nested Mode
                                //Bit 3 Non-buffered Mode
                                //Bit 2 Unused with non-buffered mode
                                //Bit 1 Set manual EOI instead of automatic
                                //Bit 0 8086/8088 mode

#define OCW1_M          0xff    //Master Mask
#define OCW1_S          0xff    //Slave Mask

#define EOI_COMMAND     0x20    //EOI Command

#define INTERRUPTS_TRIGGER_REG    0x4d0   //Trigger for Interrupts (Edge or Level).
#define INTERRUPTS_EDGE_TRIGGER   0       //Set all interrupts at edge level.
#define HPET_ID 0x1d948201
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//      Variable Declaration
//----------------------------------------------------------------------------
EFI_HANDLE  mTimerProtocolHandle    = NULL;
EFI_HANDLE  mWatchdogHandle       = NULL;
EFI_EVENT   mWatchdogEvent;
EFI_EVENT   SBLegacyBootEvent;
VOID        *OpRomCallbackRegistration = NULL;

// GUID Definitions
EFI_S3_SAVE_STATE_PROTOCOL     	static	*gBootScriptSave = NULL;
extern EFI_GUID gAmiTseEventBeforeBootGuid;
extern EFI_GUID gOpromStartEndProtocolGuid;
//This the number of days in a month - 1. (0 Based)
UINT8   gMasterBase, gSlaveBase;
UINT8   gMode = 1;                        //Initially in protected mode. (0 = Real, 1 = 32 bit protected)
UINT16  gIrqMask[2] = {0xffff,0xffff};   //Initially all Real IRQs masked, protected masked
UINT16  gIrqTrigger[2] = {0,0};          //Initially all Real IRQs Edge, protected Edge.
UINT64  mWatchdogPeriod = 0;
UINT64  mProgrammedTimerValue;
EFI_EVENT       gEvtReadyToBoot;
EFI_EVENT       mEvent;
VOID            *mReg;

SB_SETUP_DATA           *gSbSetupData=NULL ;
#if defined AMIUSB_SUPPORT && AMIUSB_SUPPORT == 1
#if defined CONTROL_ALL_USB_METHOD && CONTROL_ALL_USB_METHOD == 1
BOOLEAN         gDisableAllUsbControllers = FALSE;
#endif
EFI_EVENT       gEvtUsbProtocol  = NULL;
VOID            *gRegUsbProtocol = NULL;

#define ACPI_PATH_MACRO(PciRoot) \
 {{ACPI_DEVICE_PATH,ACPI_DP,ACPI_DEVICE_PATH_LENGTH}, EISA_PNP_ID(0x0A03), PciRoot}

#define PCI_PATH_MACRO(Device,Function) \
 {{HARDWARE_DEVICE_PATH, HW_PCI_DP, HW_PCI_DEVICE_PATH_LENGTH}, (Function),(Device)}

#define END_PATH_MACRO \
 {END_DEVICE_PATH,END_ENTIRE_SUBTYPE,END_DEVICE_PATH_LENGTH}

typedef struct
{
 ACPI_HID_DEVICE_PATH      Acpi;
 PCI_DEVICE_PATH           PciDevPath1;
 PCI_DEVICE_PATH           PciDevPath2;
 EFI_DEVICE_PATH_PROTOCOL  End;
} HYGON_USB_DEVICE_PATH;

HYGON_USB_DEVICE_PATH gXhci1DevicePath[] = 
{
    {
        ACPI_PATH_MACRO(0),
        PCI_PATH_MACRO(0x7, 0x01),
        PCI_PATH_MACRO(0x0, 0x03),
        END_PATH_MACRO
    }
};
HYGON_USB_DEVICE_PATH gXhci2DevicePath[] = 
{
    {
        ACPI_PATH_MACRO(1),
        PCI_PATH_MACRO(0x7, 0x01),
        PCI_PATH_MACRO(0x0, 0x03),
        END_PATH_MACRO
    }
};
HYGON_USB_DEVICE_PATH gXhci3DevicePath[] = 
{
    {
        ACPI_PATH_MACRO(4),
        PCI_PATH_MACRO(0x7, 0x01),
        PCI_PATH_MACRO(0x0, 0x03),
        END_PATH_MACRO
    }
};
HYGON_USB_DEVICE_PATH gXhci4DevicePath[] = 
{
    {
        ACPI_PATH_MACRO(5),
        PCI_PATH_MACRO(0x7, 0x01),
        PCI_PATH_MACRO(0x0, 0x03),
        END_PATH_MACRO
    }
};

AMI_USB_HW_SMI_HC_CONTEXT gXhci1HwSmiContext[] =
{
    USB_HC_XHCI,
    (EFI_DEVICE_PATH_PROTOCOL*)gXhci1DevicePath
};
AMI_USB_HW_SMI_HC_CONTEXT gXhci2HwSmiContext[] =
{
    USB_HC_XHCI,
    (EFI_DEVICE_PATH_PROTOCOL*)gXhci2DevicePath
};
AMI_USB_HW_SMI_HC_CONTEXT gXhci3HwSmiContext[] =
{
    USB_HC_XHCI,
    (EFI_DEVICE_PATH_PROTOCOL*)gXhci3DevicePath
};
AMI_USB_HW_SMI_HC_CONTEXT gXhci4HwSmiContext[] =
{
    USB_HC_XHCI,
    (EFI_DEVICE_PATH_PROTOCOL*)gXhci4DevicePath
};
#endif

EFI_RUNTIME_SERVICES                *gRT;
EFI_BOOT_SERVICES                   *gBS;
EFI_TIMER_NOTIFY                    mNotifyFunction;
EFI_LEGACY_8259_PROTOCOL            *mLegacy8259;
EFI_WATCHDOG_TIMER_NOTIFY           mWatchdogNotifyFunction = NULL;
VOID                                *gCpuArchNotifyReg;
extern EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL     *gPciRootBridgeIo;

extern VOID SaveRestoreRegisters(BOOLEAN Save);

typedef struct _OPTIMUM_FCH_MMIO_STRUCT {
    UINT16 Range0Base;
    UINT16 Range0Limit;
    UINT16 Range1Base;
    UINT16 Range1Limit;
    UINT16 Range2Base;
    UINT16 Range2Limit;
} OPTIMUM_FCH_MMIO_STRUCT;

typedef struct _MMIO_RANGE_STRUCT {
    UINT16 Lpc0Base;
    UINT16 Lpc0Limit;
    UINT16 Lpc1Base;
    UINT16 Lpc1Limit;
    UINT16 SpiBase;
    UINT16 SpiLimit;
    UINT16 TmpBase;
    UINT16 TmpLimit;
    UINT16 HpetBase;
    UINT16 HpetLimit;
    UINT16 BiosRamBase;
    UINT16 BiosRamLimit;
    UINT16 WatchDogBase;
    UINT16 WatchDogLimit;
    UINT16 IoapicBase;
    UINT16 IoapicLimit;
    UINT16 AcpiMmioBase;
    UINT16 AcpiMmioLimit;
} MMIO_RANGE_STRUCT;

typedef struct {
    UINT64                  Address;
    EFI_BOOT_SCRIPT_WIDTH   Width;
} BOOT_SCRIPT_PCI_REGISTER_SAVE;

EFI_STATUS
GeneratePortNumber(	 
	IN    UINT8		DeviceNumber,
	IN	  UINT8		FunctionNumber,
    IN    UINT32   	PrimarySecondary,
    IN    UINT32   	MasterSlave,
    OUT   UINT32   *PortNumber
);

//----------------------------------------------------------------------------

UINT8 PATA_PORT_NUM[2][2] = {
    // SATA port4,5 DID=IDE DID
    {4, 5},
    {5, 4}
};

UINT8 SATA_PORT_NUM[3][2] = {
    {0, 2},
    {1, 3},
    {4, 5}
};

BOOT_SCRIPT_PCI_REGISTER_SAVE gFchPciRegScriptTbl[] = {
    //LPC decoding related registers
    SB_REG(0x74),  EfiBootScriptWidthUint32,
    SB_REG(0x64),  EfiBootScriptWidthUint32,
    SB_REG(0x6c),  EfiBootScriptWidthUint32,
    SB_REG(0x90),  EfiBootScriptWidthUint16,
    SB_REG(0x44),  EfiBootScriptWidthUint32,
    SB_REG(0x48),  EfiBootScriptWidthUint32,
};
UINTN gFchPciRegScriptSize = \
        sizeof(gFchPciRegScriptTbl) / sizeof(BOOT_SCRIPT_PCI_REGISTER_SAVE);

//--------------------------------------------------------------
// Function Prototypes
//--------------------------------------------------------------
EFI_STATUS
SbDxeBoardInit (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  );

VOID SBAfterPciInit(
  IN EFI_EVENT Event,
  IN VOID *Context
  );

VOID SBEndOfDxeCallback(
  IN EFI_EVENT Event,
  IN VOID *Context
  );

VOID USBPrePortDisableCallback(
  IN EFI_EVENT Event,
  IN VOID *Context
  );
VOID UsbLegacyBootCallback(
  IN EFI_EVENT Event,
  IN VOID *Context
  );
VOID LegacyOpRomCallback(
  IN EFI_EVENT Event,
  IN VOID *Context
  );

VOID EFIAPI EnableDisableSBDevices ();

VOID EFIAPI SBAfterPCIInit(
  IN EFI_EVENT    Event,
  IN VOID       *Context
  );

VOID
UpdateSbIoApicIdToRouteTable (
  IN UINT8   ApicId,
  IN UINT32  ApicAddress
  );

extern
EFI_STATUS
CountTime (
  IN  UINTN DelayTime,
  IN  UINT16 BaseAddr
);

EFI_STATUS
WatchdogInit (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
);

EFI_STATUS
Initialize8259(
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
);

#if   SMM_SUPPORT
EFI_STATUS
SBSMM_Init (
  IN EFI_HANDLE     ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable);
#endif

VOID
EFIAPI
WatchdogHandler (
  IN EFI_EVENT    Event,
  IN VOID         *Context
);

EFI_STATUS
EFIAPI
RegisterHandler (
  IN EFI_WATCHDOG_TIMER_ARCH_PROTOCOL  *This,
  IN EFI_WATCHDOG_TIMER_NOTIFY         NotifyFunction
);

EFI_STATUS
EFIAPI
WatchdogSetTimerPeriod (
  IN EFI_WATCHDOG_TIMER_ARCH_PROTOCOL  *This,
  IN UINT64                            TimerPeriod
);

EFI_STATUS
EFIAPI
WatchdogGetTimerPeriod (
  IN EFI_WATCHDOG_TIMER_ARCH_PROTOCOL  *This,
  IN UINT64                            *TimerPeriod
);

EFI_STATUS
EFIAPI
TimerInit (
  IN EFI_EVENT  Event,
  IN VOID   *Context
);

EFI_STATUS
TimerRegisterHandler (
    IN EFI_TIMER_ARCH_PROTOCOL   *This,
    IN EFI_TIMER_NOTIFY          NotifyFunction
);

EFI_STATUS
SetTimerPeriod (
    IN EFI_TIMER_ARCH_PROTOCOL    *This,
    IN UINT64                     TimerPeriod
);

EFI_STATUS
GetTimerPeriod (
    IN EFI_TIMER_ARCH_PROTOCOL    *This,
    IN UINT64                     *TimerPeriod
);

EFI_STATUS
GenerateSoftIntr (
    IN EFI_TIMER_ARCH_PROTOCOL   *This
);


// Architectural Protocol Definitions
EFI_WATCHDOG_TIMER_ARCH_PROTOCOL mWatchdog = {
  RegisterHandler,
  WatchdogSetTimerPeriod,
  WatchdogGetTimerPeriod
};

EFI_TIMER_ARCH_PROTOCOL mTimerProtocol = {
  TimerRegisterHandler,
  SetTimerPeriod,
  GetTimerPeriod,
  GenerateSoftIntr
};

SB_HDD_POLICY_PROTOCOL mSbHddPolicy = {
	GeneratePortNumber
};

EFI_STATUS
InstallSbPolicyProtocol (VOID);

VOID 
FchMmioRangeWrite (VOID);

#if defined AMIUSB_SUPPORT && AMIUSB_SUPPORT == 1
VOID SbUsbProtocolCallback (
    IN EFI_EVENT        Event,
    IN VOID             *Context
);
#endif

VOID
EnableDisableUsbPort (VOID);

// Function Definitions
/**
    This function is called to write a byte to the Memory space.

    @param Address A 64 bit address that points to the location to be written to
    @param Value The value to be written to location specified by Address

    @retval VOID

    @note  None.

**/
VOID MemWrite8(IN VOID *Address,IN UINT8 Value)
{
  *(UINT8*)Address = Value;
  BOOT_SCRIPT_S3_MEM_WRITE_MACRO(
                          gBootScriptSave,
                          EfiBootScriptWidthUint8,
                          Address,
                          1,
                          &Value
                          );
}

/**
    This function is called to write a word to the Memory space.

    @param Address A 64 bit address that points to the location to be written to
    @param Value The value to be written to location specified by Address

    @retval VOID

    @note  None.

**/
VOID MemWrite16(IN VOID *Address,IN UINT16 Value)
{
  *(UINT16*)Address = Value;
  BOOT_SCRIPT_S3_MEM_WRITE_MACRO(gBootScriptSave, EfiBootScriptWidthUint16,
     Address, 1, &Value);
}

/**
    This function is called to write a dword to the Memory space.

    @param Address A 64 bit address that points to the location to be written to
    @param Value The value to be written to location specified by Address

    @retval VOID

    @note  None.

**/
VOID MemWrite32(IN VOID *Address,IN UINT32 Value)
{
  *(UINT32*)Address = Value;
  BOOT_SCRIPT_S3_MEM_WRITE_MACRO(gBootScriptSave, EfiBootScriptWidthUint32,
     Address, 1, &Value);
}

#if defined AMIUSB_SUPPORT && AMIUSB_SUPPORT == 1
/**
    This callback function is called after USB Protocol is 
    installed.

    @param Event Event of callback
    @param Context Context of callback.

    @retval VOID
**/

VOID SbUsbProtocolCallback (
    IN EFI_EVENT                    Event,
    IN VOID                         *Context )
{
    EFI_STATUS    		Status = EFI_SUCCESS;
	EFI_USB_PROTOCOL    *SbAmiUsb = NULL;
#if defined OEM_USB_PER_PORT_DISABLE_SUPPORT && OEM_USB_PER_PORT_DISABLE_SUPPORT == 1
	UINT8               dbPortNum = 0;
#endif
    EFI_USB_POLICY_PROTOCOL *EfiUsbPolicyProtocol;	
	
#if defined CONTROL_ALL_USB_METHOD && CONTROL_ALL_USB_METHOD == 1
    USB_SKIP_LIST SkipAllMassTable[] = { {1, 0, 0xff, 0, 0, 0x8},
                                      {0, 0, 0,    0, 0, 0  }
                                    };
#endif

#if defined OEM_USB_PER_PORT_DISABLE_SUPPORT && OEM_USB_PER_PORT_DISABLE_SUPPORT == 1									  
    USB_SKIP_LIST SkipD18F0MassTable[] = {{0, 0, 0x90, 1, 0, 0x8},// Internal Port 0
                                          {0, 0, 0x90, 2, 0, 0x8},// Internal Port 1
										 };
    USB_SKIP_LIST SkipD18F0HubMassTable[] = {{0, 0, 0x90, 1, 1, 0x8},// Internal Port 0 - HUB 0
                                             {0, 0, 0x90, 1, 2, 0x8},// Internal Port 0 - HUB 1
                                             {0, 0, 0x90, 1, 3, 0x8},// Internal Port 0 - HUB 2
                                             {0, 0, 0x90, 1, 4, 0x8},// Internal Port 0 - HUB 3
										    };


    USB_SKIP_LIST SkipD16F0MassTable[] = {{0, 0, 0x80, 1, 0, 0x8},// XHCI 0 Port 0
										  {0, 0, 0x80, 3, 0, 0x8},// XHCI 0 Port 2
                                          {0, 0, 0x80, 2, 0, 0x8},// XHCI 0 Port 1
										  {0, 0, 0x80, 4, 0, 0x8},// XHCI 0 Port 3
										 };

    USB_SKIP_LIST SkipOemMassTable[21] = {0};
	UINT8 		  IndexPort = 0;
	
#endif

    Status = pBS->LocateProtocol(&gEfiUsbPolicyProtocolGuid,
                                NULL,
                                &EfiUsbPolicyProtocol);

    ASSERT_EFI_ERROR(Status);

    if (!EFI_ERROR(Status)) {
        //Alex - Only DIE0 supports HwSMI
        Status = pBS->AllocatePool(EfiBootServicesData, 1 * sizeof(AMI_USB_HW_SMI_HC_CONTEXT*), 
            (VOID**)&EfiUsbPolicyProtocol->AmiUsbHwSmiHcTable.HcContext);

        EfiUsbPolicyProtocol->AmiUsbHwSmiHcTable.HcContext[0] = gXhci1HwSmiContext;

        EfiUsbPolicyProtocol->AmiUsbHwSmiHcTable.HcCount = 1;
    }

    Status = pBS->LocateProtocol( &gEfiUsbProtocolGuid,
                                  NULL,
                                  &SbAmiUsb );
    if (EFI_ERROR(Status)) return;

#if defined CONTROL_ALL_USB_METHOD && CONTROL_ALL_USB_METHOD == 1
    if (gDisableAllUsbControllers)
    {
        SbAmiUsb->UsbCopySkipTable(SkipAllMassTable, sizeof(SkipAllMassTable)/sizeof (USB_SKIP_LIST));
    }
#endif


#if defined OEM_USB_PER_PORT_DISABLE_SUPPORT && OEM_USB_PER_PORT_DISABLE_SUPPORT == 1
//Report SKIP port table by USB port SETUP item.
	//USB Internal port 0-1
	for (dbPortNum = 0; dbPortNum < 2; dbPortNum++){
		if (gSbSetupData->Usb0PortEn[dbPortNum] == 0){
			SkipOemMassTable[IndexPort++] = SkipD18F0MassTable[dbPortNum];
		}
	}
    if(gSbSetupData->Usb0PortEn[0] != 0){
        //  Internal Port 0 - HUB port 0-3
    	for (dbPortNum = 0; dbPortNum < 4; dbPortNum++){
            if (gSbSetupData->Usb0HubPortEn[dbPortNum] == 0){
    			SkipOemMassTable[IndexPort++] = SkipD18F0HubMassTable[dbPortNum];
    		}
    	}
    } else {
        //  Internal Port 0 - HUB port 0-3
    	for (dbPortNum = 0; dbPortNum < 4; dbPortNum++){
            SkipOemMassTable[IndexPort++] = SkipD18F0HubMassTable[dbPortNum];
        }
    }
	

    if (gSbSetupData->Usb.Xhci0Enable) {
        //XHCI0
		UINT8 i;
		if (gSbSetupData->Xhci0PortEn[0] == 0){
			for (i = 0; i < 4; i++)
				SkipOemMassTable[IndexPort++] = SkipD16F0MassTable[i];
		}	
	}

	SbAmiUsb->UsbCopySkipTable(SkipOemMassTable, sizeof(SkipOemMassTable)/sizeof (USB_SKIP_LIST));
#endif
    if (Event) {
        pBS->CloseEvent(Event);
    }
}
#endif

/**
    This function is the entry point for this DXE. This function
    initializes the chipset SB

    @param ImageHandle Image handle
    @param SystemTable Pointer to the system table

             
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/

EFI_STATUS
SbDxeInit (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
    EFI_STATUS              Status;
    EFI_EVENT               Event;
    UINT32                  *pIOAPIC;
    EFI_CPU_ARCH_PROTOCOL   *CpuArch;
    EFI_GUID                CpuInfoHobGuid = AMI_CPUINFO_HOB_GUID;
    EFI_GUID                HobListGuid = HOB_LIST_GUID;
    CPUINFO_HOB             *CpuInfoHob;
    UINTN                   SbSetupSize = sizeof(SB_SETUP_DATA);
    UINT8                   SbIoApicId = 0;
    //UINT8                   StartLocalApicId = 0;
    VOID        	    	*Registration = NULL;
    
    ACPI_HDR                        *dsdt;
    EFI_PHYSICAL_ADDRESS            a;
    UINT32			    FchDeviceEnMap;
    
    InitAmiLib(ImageHandle, SystemTable);


    PROGRESS_CODE(DXE_SB_INIT);

    Status = pBS->AllocatePool(
                            EfiBootServicesData,
                            SbSetupSize,
                            &gSbSetupData);
    ASSERT_EFI_ERROR (Status);

    GetSbSetupData(pRS, gSbSetupData, FALSE);
    
    Status=LibGetDsdt(&a,EFI_ACPI_TABLE_VERSION_ALL);
    if(EFI_ERROR(Status)){
	    ASSERT_EFI_ERROR(Status);
            return Status;
    } else dsdt=(ACPI_HDR*)a;
    
    FchDeviceEnMap = PcdGet32 (FchRTDeviceEnableMap);
    //
    // I2C0
    //
    if ( FchDeviceEnMap & BIT5 ) {
	    UpdateAslNameObject(dsdt, (UINT8*)"ICAE", 0xF);
    } else {
	    UpdateAslNameObject(dsdt, (UINT8*)"ICAE", 0x0);
    }

    //
    // I2C1
    //
    if ( FchDeviceEnMap & BIT6 ) {
	    UpdateAslNameObject(dsdt, (UINT8*)"ICBE", 0xF);
    } else {
	    UpdateAslNameObject(dsdt, (UINT8*)"ICBE", 0x0);
    }

    //
    // I2C2
    //
    if ( FchDeviceEnMap & BIT7 ) {
	    UpdateAslNameObject(dsdt, (UINT8*)"ICCE", 0xF);
    } else {
	    UpdateAslNameObject(dsdt, (UINT8*)"ICCE", 0x0);
    }
    
    //
    // I2C3
    //
    if ( FchDeviceEnMap & BIT8 ) {
	    UpdateAslNameObject(dsdt, (UINT8*)"ICDE", 0xF);
    } else {
	    UpdateAslNameObject(dsdt, (UINT8*)"ICDE", 0x0);
    }
    
    //
    // I2C4
    //
    if ( FchDeviceEnMap & BIT9 ) {
	    UpdateAslNameObject(dsdt, (UINT8*)"ICEE", 0xF);
    } else {
	    UpdateAslNameObject(dsdt, (UINT8*)"ICEE", 0x0);
    }

    //
    // I2C5
    //
    if ( FchDeviceEnMap & BIT10 ) {
	    UpdateAslNameObject(dsdt, (UINT8*)"ICFE", 0xF);
    } else {
	    UpdateAslNameObject(dsdt, (UINT8*)"ICFE", 0x0);
    }    

    //
    // UART0
    if (FchDeviceEnMap & BIT11) {
    	UpdateAslNameObject(dsdt, (UINT8*)"UT0E", 0xF); //ON
    } else {
        UpdateAslNameObject(dsdt, (UINT8*)"UT0E", 0x0);
    }

    //
	// UART1
	if (FchDeviceEnMap & BIT12) {
		UpdateAslNameObject(dsdt, (UINT8*)"UT1E", 0xF); //ON
	} else {
		UpdateAslNameObject(dsdt, (UINT8*)"UT1E", 0x0);
	}

	//
	// UART2
	if (FchDeviceEnMap & BIT16) {
		UpdateAslNameObject(dsdt, (UINT8*)"UT2E", 0xF); //ON
	} else {
		UpdateAslNameObject(dsdt, (UINT8*)"UT2E", 0x0);
	}

	//
	// UART3
	if (FchDeviceEnMap & BIT26) {
		UpdateAslNameObject(dsdt, (UINT8*)"UT3E", 0xF); //ON
	} else {
		UpdateAslNameObject(dsdt, (UINT8*)"UT3E", 0x0);
	}
    
// (EIP46008.1)>
//    RESET_IO16_PM(ACPI_IOREG_PM1_CNTL, 0x1C01);       // Clear the SLP_TYP and SCI_EN of PmControl
// <(EIP46008.1)

    Status = pBS->LocateProtocol(
                            &gEfiPciRootBridgeIoProtocolGuid,
                            NULL,
                            &gPciRootBridgeIo);
    ASSERT_EFI_ERROR(Status);

    Status = pBS->LocateProtocol(
                            &gEfiS3SaveStateProtocolGuid,
                            NULL,
                            &gBootScriptSave);
    ASSERT_EFI_ERROR(Status);

    // The Event after PciIoProtocol
    Status = pBS->CreateEvent (
                                EFI_EVENT_NOTIFY_SIGNAL,
                                TPL_CALLBACK,
                                SBAfterPciInit,
                                NULL,
                                &mEvent);

    if(EFI_ERROR(Status)) return Status;

    Status = pBS->RegisterProtocolNotify (
                                &gEfiPciIoProtocolGuid,
                                mEvent,
                                &mReg);
    if(EFI_ERROR(Status)) return Status;

    // The Event End of Dxe
    Status = gBS->CreateEventEx (
                      EVT_NOTIFY_SIGNAL,
                      TPL_CALLBACK,
                      SBEndOfDxeCallback,
                      NULL,
                      &gEfiEndOfDxeEventGroupGuid,
                      &mEvent
                      );
    if(EFI_ERROR(Status)) return Status;

    Status= RegisterProtocolCallback(
                                    &gAmiTseEventBeforeBootGuid,
                                    USBPrePortDisableCallback,
                                    NULL,
                                    &Event,
                                    &Registration);
    ASSERT_EFI_ERROR(Status);
    
	#if defined(CSM_SUPPORT) && (CSM_SUPPORT == 1)
    Status= RegisterProtocolCallback(
		    &gAmiLegacyBootProtocolGuid,
		    USBPrePortDisableCallback,
		    NULL, &Event, &Registration);
    	ASSERT_EFI_ERROR(Status);

    CreateLegacyBootEvent(TPL_CALLBACK, &USBPrePortDisableCallback, NULL, &Event);

    CreateLegacyBootEvent(TPL_CALLBACK, &UsbLegacyBootCallback, NULL, &Event);

    Status = RegisterProtocolCallback(&gOpromStartEndProtocolGuid, LegacyOpRomCallback, NULL, &Event, &OpRomCallbackRegistration);

	#endif

    pBS->CreateEvent(
        EVT_SIGNAL_EXIT_BOOT_SERVICES,TPL_CALLBACK, 
        &USBPrePortDisableCallback, NULL, &Event
    );
        
    if (gSbSetupData->Ir.IrConfig != 0) {
        CSP_RES_ITEM    IrResource = {0x550, 2, EfiGcdIoTypeIo, -1};   //IMC IR confirgure
        Status = LibAllocCspResource(&IrResource,
                                    1,
                                    ImageHandle,
                                    NULL);
        ASSERT_EFI_ERROR(Status);
    }

    // Put the I/O APIC in Virtual wire mode

    // CHIPSET PROGRAMMING TO ENABLE IOAPIC GOES HERE

    CpuInfoHob=(CPUINFO_HOB*)GetEfiConfigurationTable(pST,&HobListGuid);
    if(CpuInfoHob == NULL) Status=EFI_UNSUPPORTED;
    else Status=FindNextHobByGuid(&CpuInfoHobGuid,(VOID**)&CpuInfoHob);

//    if(!EFI_ERROR(Status)){
//        SbIoApicId = CpuInfoHob->CpuCount;
//    } else {
        SbIoApicId = NCPU;
    
    pIOAPIC = (UINT32 *)APCB;
    // IO APIC index (0)
    MemWrite8(pIOAPIC, 0);
    // New ID (at offset 0x10)
    MemWrite32(pIOAPIC + 4,SbIoApicId << 24);
    //Update the ApicRoutTable and ApicInfoTable
    UpdateSbIoApicIdToRouteTable(SbIoApicId,APCB);

    // Enable Top of 4GB flash ROM access

    // CHIPSET PROGRAMMING TO ENABLE TOP of 4GB ROM ACCESS GOES HERE

    // Install 8259 services
    Initialize8259(ImageHandle, SystemTable);

    // Init UART IRQ mapping for PIC mode
    if(FchRTDeviceEnableMapToken & BIT11) 
    {
        IoWrite8(PIRQ_IO_INDEX, 0x74);
        IoWrite8(PIRQ_IO_DATA, 0x04);
    }
    if(FchRTDeviceEnableMapToken & BIT12)
    {
        IoWrite8(PIRQ_IO_INDEX, 0x75);
        IoWrite8(PIRQ_IO_DATA, 0x03);
    }
    if(FchRTDeviceEnableMapToken & BIT16)
    {
		IoWrite8(PIRQ_IO_INDEX, 0x78);
		IoWrite8(PIRQ_IO_DATA, 0x04);
	}
	if(FchRTDeviceEnableMapToken & BIT26)
	{
		IoWrite8(PIRQ_IO_INDEX, 0x79);
		IoWrite8(PIRQ_IO_DATA, 0x03);
	}
        
    
    // Install watchdog timer services
    WatchdogInit(ImageHandle, SystemTable);

    //Install Legacy timer callback. This is so there is no dependency on
    //Cpu arch protocol.
    // Find the CPU Arch Protocol
    Status = pBS->LocateProtocol (
                                    &gEfiCpuArchProtocolGuid,
                                    NULL,
                                    &CpuArch);
    if (Status == EFI_SUCCESS) {
        TimerInit(NULL, NULL);
    } else {
        Status = RegisterProtocolCallback(
                                        &gEfiCpuArchProtocolGuid,
                                        TimerInit,
                                        NULL,
                                        &Event,
                                        &gCpuArchNotifyReg);
        ASSERT_EFI_ERROR(Status);
    }

#if   SMM_SUPPORT
    // Use HYGON UEFI SMM Drivers.
#endif

    //
    // Enable or Disable the Devices depending upon the Setup.
    //
    EnableDisableSBDevices();
#if defined AMIUSB_SUPPORT && AMIUSB_SUPPORT == 1
#if defined CONTROL_ALL_USB_METHOD && CONTROL_ALL_USB_METHOD == 1
	if (gSbSetupData->UsbAllEnable == 0){
		gDisableAllUsbControllers = TRUE;
	}
#endif

{
    EFI_USB_PROTOCOL    *SbAmiUsb = NULL;
    
    Status = pBS->LocateProtocol(&gEfiUsbProtocolGuid,
                                  NULL,
                                  &SbAmiUsb);

    if (EFI_ERROR(Status)) {
        Status = RegisterProtocolCallback( &gEfiUsbProtocolGuid,\
                                           SbUsbProtocolCallback,\
                                           NULL,\
                                           &gEvtUsbProtocol,\
                                           &gRegUsbProtocol );
    } else {
        SbUsbProtocolCallback(NULL, NULL);
    }
}
#endif


#if defined EMUL6064_SUPPORT && EMUL6064_SUPPORT == 1
//HCEx50HCE IntrEn  
//BIT4 EmulationSmiEn. Read-write. Reset: 0. 1=Gnerate SMI if emulation interrupt happens
     RW_MEM32(HCEx50_IntrEn,EmulationSmiEn, 0); 
#endif

    //
    // Board specific init
    //
    Status = SbDxeBoardInit (ImageHandle, SystemTable);
	InstallSbPolicyProtocol();
	
	// Read CMOS Status Register 0x0C to clear potentially undetermined condition of IRQ8 
	// generation for proper Legacy mode operation, including Legacy OptROM execution
	IoWrite8(0x70, 0x0C);
	IoRead8(0x61); //IoDelay
	IoRead8(0x71);

    return Status;
}

/**
    Update the correct Sb IO APCI ID to Route table for Mp table create.

        
    @param ApicId 
    @param ApicAddress 

    @retval VOID

**/

VOID
UpdateSbIoApicIdToRouteTable (
  IN UINT8   ApicId,
  IN UINT32  ApicAddress
  )
{
//### <AAV> make a generic routine later.
#if defined AmiBoardInfo_SUPPORT &&  AmiBoardInfo_SUPPORT == 1
    AMI_BOARD_INFO2_PROTOCOL	*AmiBoardInfo2Protocol = NULL;
    PCI_IRQ_APIC_ROUTE   		*RouteTable = NULL;
    UINT8           			BuildIoApicId = 1;
    UINT32           			i = 0, j = 0, Count = 0;
    EFI_STATUS              	Status;

    Status = pBS->LocateProtocol(&gAmiBoardInfo2ProtocolGuid, NULL, (VOID**)&AmiBoardInfo2Protocol);
    if (!EFI_ERROR(Status))
    {
        Count = AmiBoardInfo2Protocol->ApicBrdData->IoApicCount;

        // Update Correct APICID to AMI_SDL_IO_APIC_INFO
        //-----------------        
        for(i = 0; i < Count; i++) {
        	// Find the SB - IOAPIC
            if (AmiBoardInfo2Protocol->ApicBrdData->IoApicInfo[i].ApicAddress == ApicAddress) {
                BuildIoApicId = AmiBoardInfo2Protocol->ApicBrdData->IoApicInfo[i].ApicIdBuild;
                AmiBoardInfo2Protocol->ApicBrdData->IoApicInfo[i].ApicIdBuild = ApicId;
            }
        } 

        Count = (UINT32)AmiBoardInfo2Protocol->ApicRoutLength/sizeof(PCI_IRQ_APIC_ROUTE);
        RouteTable = AmiBoardInfo2Protocol->ApicRoutTable;

        // Update Correct APICID to PCI RouteTable
        for(i = 0; i < Count; i++)
        {
            for(j = 0; j < 4; j++)
            {
                if(RouteTable[i].Intn[j].IoApicId == BuildIoApicId)
                    RouteTable[i].Intn[j].IoApicId =ApicId;
            }   
        }
     }
#endif
}


/**
    This function  is a PCIIO call back function ,it will did it
    After PCI

    @param Event 
    @param Context 

    @retval VOID

**/
VOID
SBAfterPciInit(
    IN EFI_EVENT Event,
    IN VOID *Context
)
{

#if FCH_MMIO_RANGE_CONTROL
    FchMmioRangeWrite();
#endif


#if (SB_LPC_PORT80_SUPPORT == 0)
    RW_PCI8_S3((PCIB_BUS_DEV_FUN << 16) + FCH_PCIB_REG04, BIT0, 0);
#endif
    //
    // User Disabling Unused SATA Ports
    //

    // Close Event and don't run it again
    pBS->CloseEvent(Event);
}


// (EIP53061)>
#if CONTROL_ALL_USB_METHOD
/**
    This function is will disable all usb controller

    @param VOID

    @retval VOID
**/
VOID CheckDisableUsbControllers()
{
    EFI_USB_PROTOCOL        *UsbProtocol;
    EFI_STATUS              Status;
    UINT8                   dbVar1 = 0;
    UINT8                   dbVar2 = 0;
    static EFI_GUID UsbProtocolGuid = EFI_USB_PROTOCOL_GUID;

    Status = pBS->LocateProtocol(
                 &UsbProtocolGuid,
                 NULL,
                 &UsbProtocol
             );
    if (EFI_ERROR(Status)) return;

    if (gBootScriptSave == NULL) {
	EFI_STATUS      Status;
        Status = pBS->LocateProtocol(&gEfiS3SaveStateProtocolGuid,NULL, \
                    &gBootScriptSave);
        ASSERT_EFI_ERROR(Status);
    }
    if(gSbSetupData->UsbAllEnable == 1) {

//####           dbVar1 = 0x7F;
//####           dbVar2 =0x03;
//(EIP62170+)>>
        dbVar2=READ_MEM8(ACPI_MMIO_BASE + XHCI_BASE + 0x00);

       if (dbVar2 & 0x03 )
        {
           dbVar1 = 0xCF;
           dbVar2 =0x03;
        } 
        else
        {
         dbVar1 = 0x7F;
        }
//<<(EIP62170+)
    } else {
        dbVar1 = 0; 
        dbVar2 = 0; //XHCI disable
        UsbProtocol->UsbRtShutDownLegacy();
        // clear status of USB Controller #1
        RESET_PCI16_EUSB_S3(gBootScriptSave,0x04, 0xffff);
        RESET_PCI32_EUSB_S3(gBootScriptSave,0x10, 0xffffffff);
        RESET_PCI32_EUSB_S3(gBootScriptSave,0x3C, 0xffffffff);
    }
    // Enable/Disable all OHCI/EHCI Controller
    RW_MEM8_S3 (gBootScriptSave,ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGEF, dbVar1, 0xFF);
    // Enable/Disable all XHCI Controller
    RW_MEM8_S3 (gBootScriptSave,ACPI_MMIO_BASE + XHCI_BASE + 0x00, dbVar2, 0x03);

}
#endif
// <(EIP53061)

/**
    Due to HYGON BTS testing fail, it required to restore FCH registers 
    back while resume from S3.

    @param VOID

    @retval VOID

**/

VOID
RestoreFchS3ForBTS()
{
    UINT32 buffer32 = 0;
    if (gBootScriptSave == NULL) {
	EFI_STATUS      Status;
        Status = pBS->LocateProtocol(&gEfiS3SaveStateProtocolGuid,NULL, \
                    &gBootScriptSave);
        ASSERT_EFI_ERROR(Status);
    }
    //SMIxD0 MemTrapping0
    buffer32 = READ_MEM32(ACPI_MMIO_BASE + SMI_BASE + 0xD0);
    WRITE_MEM32_S3(gBootScriptSave, ACPI_MMIO_BASE + SMI_BASE + 0xD0, buffer32);

    //D14F3xD0
    buffer32 = READ_PCI32_SB(0xD0);
    WRITE_PCI32_SB_S3(gBootScriptSave, 0xD0,buffer32);

    //PMx74
    buffer32 = READ_MEM32(ACPI_MMIO_BASE + PMIO_BASE + 0x74);
    WRITE_MEM32_S3(gBootScriptSave, ACPI_MMIO_BASE + PMIO_BASE + 0x74, buffer32);
    

    //D10F0x4C XHCI OverCurrent
    WRITE_PCI32_XHCI0_S3(gBootScriptSave, FCH_XHCI_REG48, FCH_XHCI_IND_REG04);
    buffer32 = READ_PCI32_XHCI0(FCH_XHCI_REG4C);
    WRITE_PCI32_XHCI0_S3(gBootScriptSave, FCH_XHCI_REG4C, buffer32);
}

/**
    Restore Fch PCI register value for S3 resume.

    @param VOID

    @retval VOID

**/

VOID
RestoreFchPciS3()
{
    EFI_STATUS      Status;
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

    for (i = 0; i < gFchPciRegScriptSize; i++) {
        gPciRootBridgeIo->Pci.Read(
                        gPciRootBridgeIo,
                        gFchPciRegScriptTbl[i].Width,
                        gFchPciRegScriptTbl[i].Address,
                        1,
                        &Buffer);

        BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(
                        gBootScriptSave,
                        gFchPciRegScriptTbl[i].Width,
                        gFchPciRegScriptTbl[i].Address,
                        1,
                        &Buffer);
    }
}

/**
    The callback function executed when the EndOfDxe event group is signaled.
    @param Event 
    @param Context 

    @retval VOID

**/
VOID
SBEndOfDxeCallback(
    IN EFI_EVENT Event,
    IN VOID *Context
)
{

    RestoreFchS3ForBTS();

    RestoreFchPciS3();

    // Close Event and don't run it again
    pBS->CloseEvent(Event);
}

/**
    This function  is a ready to boot event function ,it will did it
    when ready to boot

    @param Event 
    @param Context 

    @retval VOID

**/

VOID
USBPrePortDisableCallback(
    IN EFI_EVENT Event,
    IN VOID *Context
)
{
#if CONTROL_ALL_USB_METHOD  
    CheckDisableUsbControllers();
#endif 

#if defined OEM_USB_PER_PORT_DISABLE_SUPPORT && OEM_USB_PER_PORT_DISABLE_SUPPORT == 1
    EnableDisableUsbPort();
#endif

    // Close Event and don't run it again
    pBS->CloseEvent(Event);
}

/**
    This function called on legacy boot event. If SMI periodical dispatcher
    for Short timer was registered, timer will be started here.

    @param Event 
    @param Context 

    @retval VOID

**/

VOID
UsbLegacyBootCallback(
    IN EFI_EVENT Event,
    IN VOID *Context
)
{	
	// Check if timer was enabled - in dispatcher registration routine
	if(ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGC3) & BIT4) {
        DEBUG((EFI_D_INFO, "UsbLegacyBootCallback: Start SMI periodic timer\n"));
        // Start SMI periodic timer for USB
        // 1. Select Short timer registers
        ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98) &= ~BIT29;
        // 2. Start Timer
        ACPIMMIO16 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG96) |= SMI_TIMER_ENABLE;
    }
    // Close Event and don't run it again
    pBS->CloseEvent(Event);
}

#if ( defined(CSM_SUPPORT) && (CSM_SUPPORT != 0) )
/**
    This function called on legacy OpRom Start/End event. If SMI periodical dispatcher
    for Short timer was registered, timer will be started/stopped here.

    @param Event 
    @param Context 

    @retval VOID

**/

VOID
LegacyOpRomCallback(
    IN EFI_EVENT  Event,
    IN VOID       *Context
)
{   
    static BOOLEAN              PeriodicSmiEnabled;
    EFI_HANDLE                  Handle;
    UINTN                       Size = sizeof(EFI_HANDLE);
    CSM_PLATFORM_POLICY_DATA    *OpRomStartEndProtocol;
    EFI_STATUS                  Status;
    
    // Determine if that is Start or Stop of Legacy ROM execution
    Status = pBS->LocateHandle(ByRegisterNotify, \
                                   NULL,\
                                   OpRomCallbackRegistration, \
                                   &Size,\
                                   &Handle);
    if (EFI_ERROR(Status)) {
        return;
    }
    
    Status = pBS->HandleProtocol(Handle, \
                                     &gOpromStartEndProtocolGuid,\
                                     &OpRomStartEndProtocol);
    if (EFI_ERROR(Status)) {
        return;
    }
    
    if(OpRomStartEndProtocol != NULL) {
        if(OpRomStartEndProtocol->ExecuteThisRom == TRUE){
            // OpRom is starting
			// Skip starting tmer if that is VGA OptROM
            if(OpRomStartEndProtocol->PciIo != NULL){
                UINT32  PciCfgReg32;
                OpRomStartEndProtocol->PciIo->Pci.Read(OpRomStartEndProtocol->PciIo, EfiPciIoWidthUint32,
                        8,  // offset
                        1,  // width
                        &PciCfgReg32);
                if(((PciCfgReg32>>24)& 0x0F) == PCI_CL_DISPLAY) return;
            }
            // Check if timer was enabled - in dispatcher registration routine
            if(ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGC3) & BIT4) {
                DEBUG((EFI_D_ERROR, "LegacyOpRomCallback: Start SMI periodic timer\n"));
                // Start SMI periodic timer for USB
                // 1. Select Short timer registers
                ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98) &= ~BIT29;
                // 2. Start Timer
                ACPIMMIO16 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG96) |= SMI_TIMER_ENABLE;
                PeriodicSmiEnabled = TRUE;
            }
        }
        
    } else {
        // OpRom already executed
        if(PeriodicSmiEnabled == TRUE){
            // Check if timer was enabled - in dispatcher registration routine
            if(ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGC3) & BIT4) {
                DEBUG((EFI_D_ERROR, "LegacyOpRomCallback: Stop SMI periodic timer\n"));
                // Stop SMI periodic timer for USB
                // 1. Select Short timer registers
                ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98) &= ~BIT29;
                // 2. Stop Timer
                ACPIMMIO16 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG96) &= ~SMI_TIMER_ENABLE;
                PeriodicSmiEnabled = FALSE;
            }
        }
    }
}
#endif

/**
    Decompress MISCRAID.BIN to conventional memory
    According to legacy BIOS put MISCRAID.BIN to 0x30000
    SB.ASM ==> DecompressSbRaidUiBin_FAR

    @param MiscGuid 

             
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/

EFI_STATUS DecompressMiscUI( IN EFI_GUID *MiscGuid)
{
    EFI_STATUS                      Status = EFI_SUCCESS;
    EFI_PHYSICAL_ADDRESS            MiscLocation;
    EFI_FIRMWARE_VOLUME2_PROTOCOL   *fv;
    EFI_FV_FILE_ATTRIBUTES          Attributes;
    EFI_FV_FILETYPE                 FileType;
    EFI_HANDLE                      *HandleBuffer;
    UINT32                          FvStatus=0;
    UINTN                           Size,i,NumberOfHandles;
    VOID*                           ImgBuf;

    //Locate protocol.
    Status = pBS->LocateHandleBuffer(ByProtocol,
                &gEfiFirmwareVolume2ProtocolGuid,
                NULL,&NumberOfHandles,&HandleBuffer);
    if(EFI_ERROR (Status)) return Status;

    //Look for FV with MiscUI storage file
    for (i = 0; i < NumberOfHandles; i++) {
        // Get the protocol on this handle
        Status = pBS->HandleProtocol(HandleBuffer[i],
                    &gEfiFirmwareVolume2ProtocolGuid,&fv);
        if(EFI_ERROR(Status))return Status;

        Status=fv->ReadFile(fv,
                MiscGuid,NULL,&Size,&FileType,&Attributes,&FvStatus);
        if (Status == EFI_SUCCESS) break;
    }
    pBS->FreePool(HandleBuffer);

    if (Status == EFI_SUCCESS) {
        Size=0;
        ImgBuf=NULL;
        Status=fv->ReadSection(fv,
                        MiscGuid,
                        EFI_SECTION_RAW,
                        0,
                        (VOID**)&ImgBuf,
                        &Size,&FvStatus);
        if(EFI_ERROR(Status)) return Status;
        //
        // according to CORE8
        // Taishan/SB750 RAID UI in TEMP_DSEG(0x30000)
        //
        MiscLocation=0x30000;
        Status = pBS->AllocatePages(
                    AllocateAddress,
                    EfiReservedMemoryType,
                    Size/4096+1, // Nomber of 4k pages
                    &MiscLocation);
        pBS->CopyMem((VOID*)MiscLocation,ImgBuf,Size);
        TRACE((-1,"MISC Radi file in memory location = %4X \n", MiscLocation));

        if(EFI_ERROR(Status)) {
            PROGRESS_CODE(DXE_SB_ERROR);
            EFI_DEADLOOP();
        }
    }
    return Status;
}

#if HPET_SUPPORT

#include <Acpi20.h>
#include <Protocol\AcpiSupport.h>

//========================================================================================
/**
    This function will create HPET acpi Table when ACPI Support Protocol Will be available
    Context will stand for HPET Enable Setup question

    @param Event event that has been signaled
    @param Context 
        if NULL than HPET feature is disabled  and no table need to be created

    @retval VOID

**/
VOID CreateHpetAcpiTable(IN EFI_EVENT Event, IN VOID *Context) {
#if defined (HPET_BASE_ADDRESS) && (HPET_BASE_ADDRESS != 0)
    EFI_STATUS                  Status=0;
    HPET_20                     *AcpiTbl;
    EFI_ACPI_TABLE_PROTOCOL     *At;
    UINTN                       TableKey;
    UINTN                       HpetBase=HPET_BASE_ADDRESS;
//--------------------------------
        //it must be only one instance of such protocol
        Status = pBS->LocateProtocol( &gEfiAcpiTableProtocolGuid, NULL, &At );
        if(EFI_ERROR(Status)){
            TRACE((-1,"HPET LocateProtocol(ACPISupport)- %r Error\n", Status));
            return;
        }TRACE((-1,"HPET LocateProtocol(ACPISupport)- %r Success\n", Status));

        AcpiTbl=MallocZ(sizeof(HPET_20));
        ASSERT(AcpiTbl);
        if(AcpiTbl){
            //Fill Table header;
            AcpiTbl->Header.Signature=HPET_SIG;
            AcpiTbl->Header.Length=sizeof(HPET_20);
            AcpiTbl->Header.Revision=1;
            AcpiTbl->Header.Checksum=0;
            pBS->CopyMem((VOID*)AcpiTbl->Header.OemId, \
                CONVERT_TO_STRING(SB_HPET_OEM_ID), \
                sizeof(AcpiTbl->Header.OemId) \
            );
            pBS->CopyMem((VOID*)AcpiTbl->Header.OemTblId, \
                CONVERT_TO_STRING(SB_HPET_OEM_TBL_ID), \
                sizeof(AcpiTbl->Header.OemTblId) \
            );
            AcpiTbl->Header.OemRev=ACPI_OEM_REV;
            //---AcpiTbl->Header.CreatorId=0x2E494D41;//"AMI."
            AcpiTbl->Header.CreatorId=CREATOR_ID_AMI;//"AMI"
            AcpiTbl->Header.CreatorRev=CORE_MAJOR_VERSION;

            //fill HPET Fields
            //The GAS structure
            AcpiTbl->BaseAddress.AddrSpcID=GAS_SYS_MEM;
            AcpiTbl->BaseAddress.RegBitWidth=64;
            AcpiTbl->BaseAddress.RegBitOffs=0;
            ////Base address of 1K HPET RegBlock space
            AcpiTbl->BaseAddress.Address=HpetBase;

            // Decode HPET to get ID.
            //RW_MEM32(ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG50, HpetBase | BIT0, 0x7FF);
            AcpiTbl->EvtTmrBlockId.TMR_BLK_ID=HPET_ID;

            AcpiTbl->MinTickPeriod=14318; //approx 1ms

            //Add table
            Status = At->InstallAcpiTable( At, \
                                           AcpiTbl, \
                                           sizeof(HPET_20), \
                                           &TableKey );
            TRACE((-1,"ACPITable.InstallAcpiTable(AcpiTbl) = %r \n", 4));
            ASSERT_EFI_ERROR(Status);

            //free memory used for table image
            pBS->FreePool(AcpiTbl);
        }
#endif

    //Kill the Event
    pBS->CloseEvent(Event);
}

#endif

#if defined WDT_SUPPORT && WDT_SUPPORT == 1
//========================================================================================
/**
    This function will create WDRT acpi Table when ACPI Support Protocol Will be available

    @param Event event that has been signaled
    @param Context 
        if NULL than WDRT feature is disabled  and no table need to be created

    @retval VOID

**/
VOID CreateWdrtAcpiTable(IN EFI_EVENT Event, IN VOID *Context) {
    EFI_STATUS                  Status=0;
    
    EFI_ACPI_TABLE_PROTOCOL     *At;
    UINTN                       TableKey;
    UINTN                       volatile WdrtBase = WDT_BASE_ADDRESS;
//--------------------------------
    if( WdrtBase ) {
	WDRT                        *AcpiTbl;
        //it must be only one instance of such protocol
        Status = pBS->LocateProtocol( &gEfiAcpiTableProtocolGuid, NULL, &At );
        if(EFI_ERROR(Status)) {
            TRACE((-1,\
                "WDRT LocateProtocol(ACPISupport)- %r Error\n", Status));
            return;
        }TRACE((-1,\
            "WDRT LocateProtocol(ACPISupport)- %r Success\n", Status));

        AcpiTbl=MallocZ(sizeof(WDRT));
        ASSERT(AcpiTbl);
        if(AcpiTbl) {
            //Fill Table header;
            AcpiTbl->Header.Signature=WDRT_SIG;
            AcpiTbl->Header.Length=sizeof(WDRT);
            AcpiTbl->Header.Revision=1;
            AcpiTbl->Header.Checksum=0;
            pBS->CopyMem((VOID*)AcpiTbl->Header.OemId, \
                CONVERT_TO_STRING(SB_WDRT_OEM_ID), \
                sizeof(AcpiTbl->Header.OemId) \
            );
            pBS->CopyMem((VOID*)AcpiTbl->Header.OemTblId, \
                CONVERT_TO_STRING(SB_WDRT_OEM_TBL_ID), \
                sizeof(AcpiTbl->Header.OemTblId) \
            );
            AcpiTbl->Header.OemRev=ACPI_OEM_REV;
            AcpiTbl->Header.CreatorId=CREATOR_ID_AMI;//"AMI"
            AcpiTbl->Header.CreatorRev=CORE_MAJOR_VERSION;

            //fill WDRT Fields
            //The GAS structure : Control Register
            AcpiTbl->ControlRegAddr.AddrSpcID=GAS_SYS_MEM;
            AcpiTbl->ControlRegAddr.RegBitWidth=32;
            AcpiTbl->ControlRegAddr.RegBitOffs=0;
            AcpiTbl->ControlRegAddr.AccessSize = 0x3;
            //Base address of WDRT Control Register
            AcpiTbl->ControlRegAddr.Address = WdrtBase;

            //The GAS structure : Count Register
            AcpiTbl->CountRegAddr.AddrSpcID=GAS_SYS_MEM;
            AcpiTbl->CountRegAddr.RegBitWidth=32;
            AcpiTbl->CountRegAddr.RegBitOffs=0;
            AcpiTbl->CountRegAddr.AccessSize = 0x3;
            //Base address of WDRT Count Register
            AcpiTbl->CountRegAddr.Address=WdrtBase + 4;

            AcpiTbl->PciDeviceID = 0xFFFF;            
            AcpiTbl->PciVendorID = 0xFFFF;        
            AcpiTbl->PciBusNumber = 0x00;       
            AcpiTbl->PciDeviceNumber = 0x00;      
            AcpiTbl->PciFunctionNumber = 0x00;
            AcpiTbl->PciSegment = 0x00;
            AcpiTbl->MaxCount = 0x03FF;
            AcpiTbl->Units = 0x00;

            //Add table
            Status = At->InstallAcpiTable( At, \
                                           AcpiTbl, \
                                           sizeof(WDRT), \
                                           &TableKey );
            TRACE((-1,"ACPISupport.SetAcpiTable() = %r \n", Status));
            ASSERT_EFI_ERROR(Status);

            //free memory used for table image
            pBS->FreePool(AcpiTbl);
        }
    }

    //Kill the Event
    pBS->CloseEvent(Event);
}
#endif

/**
    This function Enables or Disables the USB Port depending upon
    the Setup Questions.

    @param VOID

    @retval VOID

**/
VOID
EnableDisableUsbPort (VOID)
{
    UINT8                   UsbPortBit;
    UINT8                   dbPortNum;
    UINT8                   X64Reg1;//, X64Reg2, X64Reg3;
    
	UINT32                  UsbBarAddr;
	UINT32                  PortStatusCtrl;


    //Read the Misc control 2 register and disable Register share
    X64Reg1 = READ_PCI8_EUSB(FCH_EHCI_REG64);
                
    if (gBootScriptSave == NULL) {
	EFI_STATUS      Status;
        Status = pBS->LocateProtocol(&gEfiS3SaveStateProtocolGuid,NULL, \
                    &gBootScriptSave);
        ASSERT_EFI_ERROR(Status);
    }
            
            
    WRITE_PCI8_EUSB_S3(gBootScriptSave, FCH_EHCI_REG64, (~BIT1) & X64Reg1);

    //USB Port enable/disable
    UsbPortBit=0;
    for (dbPortNum=0; dbPortNum < 2; dbPortNum++)
        UsbPortBit |= (gSbSetupData->Usb0PortEn[dbPortNum] ? 0 : 1) << dbPortNum;
                        
                        
    if (UsbPortBit != 0) {
        RW_PCI8_EUSB_S3(gBootScriptSave, FCH_EHCI_REG64 + 3, UsbPortBit, 0);         //USB Internal port 0-1

        // if USB port was disabled, HUB behind this port may still work
        // Reset port for fix the issue.
        UsbBarAddr = READ_PCI32_EUSB(FCH_EHCI_REG10);
        for (dbPortNum=0; dbPortNum < 2; dbPortNum++) {
            if(gSbSetupData->Usb0PortEn[dbPortNum] ? 0 : 1) {
                PortStatusCtrl = READ_MEM32(UsbBarAddr + FCH_EHCI_BAR_REG64 + (dbPortNum*4));
                if((PortStatusCtrl & (BIT0 | BIT2)) == (BIT0 | BIT2)) {
                    SET_MEM32(UsbBarAddr + FCH_EHCI_BAR_REG64 + (dbPortNum*4), BIT8);
                    CountTime(10*1000, PM_BASE_ADDRESS);
                    RESET_MEM32(UsbBarAddr + FCH_EHCI_BAR_REG64 + (dbPortNum*4), BIT8);
                }
            }
        }
    }
    
    if ((UsbPortBit & BIT0) != BIT0) {
        UsbPortBit=0;
        for (dbPortNum=0; dbPortNum < 4; dbPortNum++)
            UsbPortBit |= (gSbSetupData->Usb0HubPortEn[dbPortNum] ? 0 : 1) << dbPortNum;
        UsbPortBit <<= 4;
        
        if (UsbPortBit != 0)
            RW_PCI8_EUSB_S3(gBootScriptSave, FCH_EHCI_REG90 + 2, UsbPortBit, 0);     //HUB port 0-3
                            
    }

    if (gSbSetupData->Usb.Xhci0Enable) {
        UINT32 XhciPortBit = 0;
        UINT32 xIndex = 0x0c;  //Port Disable (Write Once)
        UINT32 xData = 0;

        //XHCI0 Program
        for (dbPortNum = 0; dbPortNum < 4; dbPortNum++)
            XhciPortBit |= (gSbSetupData->Xhci0PortEn[dbPortNum] ? 0 : 1) << dbPortNum;
        if (XhciPortBit != 0) {
            WRITE_PCI32_XHCI0_S3(gBootScriptSave, 0x48, xIndex);
            xData = READ_PCI32_XHCI0(0x4C);
            WRITE_PCI32_XHCI0_S3(gBootScriptSave, 0x4C, xData | XhciPortBit);
        }


    } 

    //Restore Misc control 2 register
    WRITE_PCI8_EUSB_S3(gBootScriptSave, FCH_EHCI_REG64, X64Reg1);
}

/**
    This function Enables or Disables the Devices depending upon
    the Setup Questions.

    @param VOID

    @retval VOID

**/
VOID
EnableDisableSBDevices (VOID)
{
//--------------------------------------------------------------------------
//            NO PORTING REQUIRED FOR THIS CHIPSET
//--------------------------------------------------------------------------
	
	
#if HPET_SUPPORT
    BOOLEAN                 HpetEn;

    HpetEn = gSbSetupData->Hpet.HpetEnable ? TRUE : FALSE;
    if (HpetEn) {
        EFI_STATUS  Status = EFI_SUCCESS;
        EFI_EVENT   mAcpiEvent;
        VOID        *mAcpiReg;
        Status=RegisterProtocolCallback(
                                        &gEfiAcpiTableProtocolGuid,
                                        CreateHpetAcpiTable,
                                        NULL,
                                        &mAcpiEvent,
                                        &mAcpiReg);
        ASSERT_EFI_ERROR(Status);
        pBS->SignalEvent(mAcpiEvent);
    }
#endif
    
    if (gBootScriptSave == NULL) {
	EFI_STATUS  Status = EFI_SUCCESS;
        Status = pBS->LocateProtocol(&gEfiS3SaveStateProtocolGuid,NULL, \
	         &gBootScriptSave);
	ASSERT_EFI_ERROR(Status);
	    	    	    	 }
#if defined WDT_SUPPORT && WDT_SUPPORT == 1
    // Create ACPI table - WDRT
    {
        EFI_STATUS  Status = EFI_SUCCESS;
        EFI_EVENT   mWdrtAcpiEvent;
        VOID        *mWdrtAcpiReg;
        Status=RegisterProtocolCallback(
                                        &gEfiAcpiTableProtocolGuid,
                                        CreateWdrtAcpiTable,
                                        NULL,
                                        &mWdrtAcpiEvent,
                                        &mWdrtAcpiReg);
        ASSERT_EFI_ERROR(Status);
        pBS->SignalEvent(mWdrtAcpiEvent);
    }
#else
    // Disabled Watchdog timer memory decoding.
    RW_MEM8_S3 (gBootScriptSave,ACPI_MMIO_BASE + PMIO_BASE, 0, BIT7);
#endif

#if defined OEM_USB_PER_PORT_DISABLE_SUPPORT && OEM_USB_PER_PORT_DISABLE_SUPPORT == 0
	EnableDisableUsbPort();
#endif
    
    if (!gSbSetupData->TpmDecode)
        RESET_PCI8_SB_S3(gBootScriptSave,FCH_LPC_REG7C, BIT0 + BIT2);   // Disable TPM decode if TPM function disabled
    else
        SET_PCI8_SB_S3(gBootScriptSave,FCH_LPC_REG7C, BIT0 + BIT2);     // Enable TPM decode
}


//----------------------------------------------------------------------------
//   USUALLY NO PORTING REQUIRED FOR THE FOLLOWING ROUTINES
//----------------------------------------------------------------------------

/**
    This function is called when the watchdog timer event is
    signalled.  It calls the registered handler and then
    resets the system

    @param Event Watchdog event
    @param Context Context pointer

    @retval VOID

**/
VOID EFIAPI WatchdogHandler (
    IN EFI_EVENT    Event,
    IN VOID         *Context
)
{
    // Call the registered handler if there is one
    if (mWatchdogNotifyFunction != NULL) {
        mWatchdogNotifyFunction (mWatchdogPeriod);
    }

    // Reset the system
    gRT->ResetSystem (EfiResetCold, EFI_TIMEOUT, 0, NULL );
}

/**
    This function registers a handler that is called when the Timer
    event has been signalled

    @param This Pointer to the instance
        of the Architectural Protocol
    @param NotifyFunction The function to call when
        the interrupt fires

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
EFI_STATUS EFIAPI RegisterHandler (
    IN EFI_WATCHDOG_TIMER_ARCH_PROTOCOL  *This,
    IN EFI_WATCHDOG_TIMER_NOTIFY         NotifyFunction
)
{
    // only register the handler if it is still NULL
    if (NotifyFunction && mWatchdogNotifyFunction) return EFI_ALREADY_STARTED;
    if (!NotifyFunction && !mWatchdogNotifyFunction) return EFI_INVALID_PARAMETER;

    mWatchdogNotifyFunction = NotifyFunction;

    return EFI_SUCCESS;
}

/**
    This function sets the timer period before the watchdog goes off
    every TimerPeriod number of 100ns intervals, if the period is set to 0 the
    timer event is cancelled

    @param This Pointer to the instance of
        the Architectural Protocol
    @param TimerPeriod The number of 100ns intervals to which the watchdog
        will be programmed.

          
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
EFI_STATUS EFIAPI WatchdogSetTimerPeriod (
    IN EFI_WATCHDOG_TIMER_ARCH_PROTOCOL  *This,
    IN UINT64                            TimerPeriod
)
{
    EFI_TIMER_DELAY     TimerDelayType;

    // store new timer length
    mWatchdogPeriod = TimerPeriod;

    // cancel timer event if Timer Period is 0
    TimerDelayType = (TimerPeriod) ? TimerRelative : TimerCancel;

    // set the timer for the event
    return gBS->SetTimer ( mWatchdogEvent, TimerDelayType, mWatchdogPeriod );
}


/**
    This function returns the current watchdog timer period

    @param This Pointer to the instance of
        the Architectural Protocol
    @param TimerPeriod Pointer to a memory location to load the current Timer
        period into

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/

EFI_STATUS EFIAPI WatchdogGetTimerPeriod (
    IN EFI_WATCHDOG_TIMER_ARCH_PROTOCOL  *This,
    OUT UINT64                           *TimerPeriod
)
{
    // return the current Watchdog period
    *TimerPeriod = mWatchdogPeriod;

    return EFI_SUCCESS;
}

/**
    This function installs the the Watchdog Timer protocol on its handle,
    and initializes the Watchdog timer.

    @param ImageHandle ImageHandle of the loaded driver
    @param SystemTable Pointer to the System Table

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
EFI_STATUS WatchdogInit (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
)
{
    EFI_STATUS  Status;

    gRT = SystemTable->RuntimeServices;
    gBS = SystemTable->BootServices;

    // Use the Timer event to trigger the Watchdog.  No specific hardware
    //  exists for this
    Status = gBS->CreateEvent( EVT_TIMER | EVT_NOTIFY_SIGNAL, TPL_NOTIFY,
                            WatchdogHandler, NULL, &mWatchdogEvent );
    ASSERT_EFI_ERROR(Status);

    // Create a handle for the ArchProtocol and install Watchdog Arch Protocol on the handle
    Status = gBS->InstallProtocolInterface ( &mWatchdogHandle, &gEfiWatchdogTimerArchProtocolGuid,
                                      EFI_NATIVE_INTERFACE, &mWatchdog );

    return Status;
}

/**
    This function registers a handler that is called every time the
    timer interrupt fires

    @param This Pointer to the instance of the
        Architectural Protocol
    @param NotifyFunction The function to call when the interrupt fires

               
    @retval EFI_SUCCESS new handle registered
    @retval EFI_ALREADY_STARTED if Notify function is already defined

**/

EFI_STATUS
TimerRegisterHandler (
  IN  EFI_TIMER_ARCH_PROTOCOL   *This,
  IN  EFI_TIMER_NOTIFY          NotifyFunction
  )
{
    // check to see if the handler has already been installed
  if (NotifyFunction != NULL && mNotifyFunction != NULL) {
    return EFI_ALREADY_STARTED;
  }

    // if not install it
    mNotifyFunction = NotifyFunction;
    return EFI_SUCCESS;
}


/**
    This function sets the timer to create an Intr on IRQ0
    every TimerPeriod number of 100ns intervals

    @param This Pointer to the instance of the Architectural Protocol
    @param TimerPeriod The number of 100ns intervals to which the timer
        will be programmed. This value will be rounded up to
        the nearest timer interval

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
EFI_STATUS SetTimerPeriod (
    IN EFI_TIMER_ARCH_PROTOCOL      *This,
    IN UINT64                       TimerPeriod
)
{
    EFI_CPU_ARCH_PROTOCOL   *CpuArch;
    EFI_STATUS              Status;

    // Find the CPU Architectural Protocol
    Status = pBS->LocateProtocol (&gEfiCpuArchProtocolGuid, NULL, &CpuArch);
    ASSERT_EFI_ERROR(Status);

    // If timer period is 0 then disable the Timer interrupt
    if (TimerPeriod == 0)
    {
        mLegacy8259->DisableIrq(mLegacy8259,SYSTEM_TIMER_IRQ);
        //DisableIrq(SYSTEM_TIMER_IRQ);
    }
    // otherwise change the timer period into number of ticks and set the timer
    else
    {
        UINT32 NumberOfTicks;
        if (TimerPeriod>MAX_TICK_DURATION) TimerPeriod=MAX_TICK_DURATION;
        // NumberOfTicks = TimerPeriod * 100 /TIMER_TICK;
        // since TimerPeriod in 100ns units and TIMER_TICK in ns
        // we have to multiple TimerPeriod by 100
        // To round up result:
        //      NumberOfTicks = TimerPeriod * 100/TIMER_TICK + 0.5 =
        //                      (TimerPeriod*100+TIMER_TICK/2)/TIMER_TICK
        NumberOfTicks = ((UINT32)TimerPeriod*100+TIMER_TICK/2)/TIMER_TICK;

        //write to port 0x43 to setup the timer
        IoWrite8 ( TIMER_CTRL, 0x36 );
        // write to port 0x40 to set the time
        IoWrite8 ( TIMER_0_COUNT, (UINT8)NumberOfTicks);
        IoWrite8 ( TIMER_0_COUNT, *(((UINT8*)&NumberOfTicks)+1) );

        // Now enable the interrupt
        //EnableIrq(SYSTEM_TIMER_IRQ);
        mLegacy8259->EnableIrq(mLegacy8259,SYSTEM_TIMER_IRQ,FALSE);
    }

    mProgrammedTimerValue = TimerPeriod;

    return EFI_SUCCESS;
}

/**
    This function returns the current timer period

    @param This Pointer to the instance of the Architectural Protocol
    @param TimerPeriod pointer to a memory location to load the current
        Timer period into

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
EFI_STATUS GetTimerPeriod(
    IN      EFI_TIMER_ARCH_PROTOCOL *This,
    IN OUT  UINT64                  *TimerPeriod
)
{
    *TimerPeriod = mProgrammedTimerValue;

    return EFI_SUCCESS;
}

/**
    This function generates a soft timer interrupt

    @param This Pointer to the instance of the Architectural Protocol

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
EFI_STATUS GenerateSoftIntr(
    IN EFI_TIMER_ARCH_PROTOCOL      *This
)
{
    return EFI_UNSUPPORTED;
}

/**
    This function is called when the Timer reaches 0.  It raises
    the TPL and then calls the registered notify function

    @param InterruptType Interrupt type
    @param SystemContext System context

    @retval VOID

**/
VOID TimerInterruptHandler (
    IN EFI_EXCEPTION_TYPE   InterruptType,
    IN EFI_SYSTEM_CONTEXT   SystemContext
)
{
    EFI_TPL     OldTpl;

    SaveRestoreRegisters(TRUE);

    OldTpl = pBS->RaiseTPL (TPL_HIGH_LEVEL);

    // clear the interrupt flag
    //SendEoi(SYSTEM_TIMER_IRQ);
    mLegacy8259->EndOfInterrupt(mLegacy8259,SYSTEM_TIMER_IRQ);

    // This checks for the existance of a registered notify function and if it exists
    //  it calls the function with the current programmed Timer Period
    if (mNotifyFunction)
    {
        mNotifyFunction (mProgrammedTimerValue);
    }

    pBS->RestoreTPL (OldTpl);

    SaveRestoreRegisters(FALSE);
}

/**
    This function installs the the timer protocol on its handle,
    and initializes the timer.

    @param Event Event of callback
    @param Context Context of callback.

             
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
EFI_STATUS EFIAPI TimerInit(
  IN EFI_EVENT  Event,
  IN VOID     *Context
)
{
  EFI_STATUS        Status;
  EFI_CPU_ARCH_PROTOCOL *CpuArch;
  // TimerVector must be initialized to 0,
  // since GetVector only modifies the lowest byte,
  // and RegisterInterruptHandler requires TimerVector to be 4 bytes.
  UINT32      TimerVector = 0;

  Status = pBS->LocateProtocol (
                                  &gEfiLegacy8259ProtocolGuid,
                                  NULL,
                                  &mLegacy8259);
  ASSERT_EFI_ERROR(Status);

  // Disable timer, make sure no interrupt will be created
  Status = SetTimerPeriod (&mTimerProtocol, 0);
  ASSERT_EFI_ERROR(Status);

  // Find the CPU Arch Protocol
  Status = pBS->LocateProtocol (&gEfiCpuArchProtocolGuid,
                                NULL,
                                &CpuArch);
  ASSERT_EFI_ERROR(Status);

    Status = mLegacy8259->GetVector (
                                    mLegacy8259,
                                    Efi8259Irq0,
                                    (UINT8 *)&TimerVector);
  ASSERT_EFI_ERROR(Status);

  Status = CpuArch->RegisterInterruptHandler (
                                  CpuArch,
                                  TimerVector,
                                  TimerInterruptHandler);
  ASSERT_EFI_ERROR(Status);

  // Initialize the handle pointer
  mNotifyFunction = NULL;

  // Force the timer to be enabled at its default period
  Status = SetTimerPeriod (&mTimerProtocol, DEFAULT_TICK_DURATION);
  ASSERT_EFI_ERROR (Status);

  IoWrite8(TIMER_CTRL,0x54);
  IoWrite8(TIMER_1_COUNT,0x12);
  // Install the Timer Architectural Protocol onto a new handle
  Status = pBS->InstallProtocolInterface ( &mTimerProtocolHandle,
                    &gEfiTimerArchProtocolGuid,EFI_NATIVE_INTERFACE,
                    &mTimerProtocol);
  ASSERT_EFI_ERROR(Status);

  return Status;
}

/**
    Program the Irq Mask and Trigger.

    @param VOID

    @retval VOID

    @note  Here is the control flow of this function:
        1. Program Master Irq Mask.
        2. Program Slave Irq Mask.
        3. Program Trigger Level.

**/

VOID ProgramIrqMaskTrigger(VOID)
{
    IoWrite8(0x21,(UINT8)gIrqMask[gMode]);
    IoWrite8(0xa1,(UINT8)(gIrqMask[gMode] >> 8));
  //
  // 4d0 can not be accessed as by IoWrite16, we have to split
  //
    IoWrite8(0x4d0,(UINT8)gIrqTrigger[gMode]);
    IoWrite8(0x4d1,(UINT8)(gIrqTrigger[gMode] >> 8));
}

/**
    Initializes the interrupt controller and program the Irq
    Master and Slave Vector Base.

    @param This Pointer to protocol instance
    @param MasterBase IRQ base for the master 8259 controller
    @param SlaveBase IRQ base for the slave 8259 controller

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

    @note  Here is the control flow of this function:
              1. If Master base is changed, initialize master 8259 setting
                 the interrupt offset.
              2. If Slave base is changed, initialize slave 8259 setting
                 the interrupt offset.
              3. Return EFI_SUCCESS.
**/
EFI_STATUS SetVectorBase(
    IN EFI_LEGACY_8259_PROTOCOL *This,
    IN UINT8                    MasterBase,
    IN UINT8                    SlaveBase
)
{
    //8259 Master
    if (MasterBase != gMasterBase)
    {
        IoWrite8(0x20,ICW1);        //Start 8259 Master Initialization.
        IoWrite8(0x21,MasterBase);  //Set Interrupt Offset
        IoWrite8(0x21,ICW3_M);      //Set Slave IRQ.
        IoWrite8(0x21,ICW4);        //Set 8259 mode. See ICW4 comments with #define.
        gMasterBase = MasterBase;
    }

    //8259 Slave
    if (SlaveBase != gSlaveBase)
    {
        IoWrite8(0xa0,ICW1);        //Start 8259 Slave  Initialization.
        IoWrite8(0xa1,SlaveBase);   //Set Interrupt Offset
        IoWrite8(0xa1,ICW3_S);      //Set Slave IRQ.
        IoWrite8(0xa1,ICW4);        //Set 8259 mode. See ICW4 comments with #define.
        gSlaveBase = SlaveBase;
    }

    return EFI_SUCCESS;
}

/**
    Get the Master/Slave Irq Mask, Irq Level for Legacy real
    mode and protected mode.

    @param This Pointer to the protocol instance
    @param LegacyMask Legacy mode interrupt mask
    @param LegacyEdgeLevel Legacy mode edge/level trigger value
    @param ProtectedMask Protected mode interrupt mask
    @param ProtectedEdgeLevel Protected mode edge/level trigger value

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

    @note  Here is the control flow of this function:
              1. If *LegacyMask not NULL, get legacy Mask.
              2. If *LegacyEdgeLevel not NULL, get legacy Trigger Level.
              3. If *ProtectedMask not NULL, get protected Mask.
              4. If *ProtectedEdgeLevel not NULL, get protected trigger level.
              5. Return EFI_SUCCESS.

**/
EFI_STATUS GetMask(
    IN EFI_LEGACY_8259_PROTOCOL *This,
    OUT UINT16                  *LegacyMask         OPTIONAL,
    OUT UINT16                  *LegacyEdgeLevel    OPTIONAL,
    OUT UINT16                  *ProtectedMask      OPTIONAL,
    OUT UINT16                  *ProtectedEdgeLevel OPTIONAL
)
{
    if (LegacyMask)         *LegacyMask         = gIrqMask[0];
    if (LegacyEdgeLevel)    *LegacyEdgeLevel    = gIrqTrigger[0];
    if (ProtectedMask)      *ProtectedMask      = gIrqMask[1];
    if (ProtectedEdgeLevel) *ProtectedEdgeLevel = gIrqTrigger[1];

    return EFI_SUCCESS;
}

/**
    Set the Master/Slave Irq Mask, Irq Level for Legacy real mode
    and protected mode.

    @param This Pointer to the protocol instance
    @param LegacyMask Legacy mode interrupt mask
    @param LegacyEdgeLevel Legacy mode edge/level trigger value
    @param ProtectedMask Protected mode interrupt mask
    @param ProtectedEdgeLevel Protected mode edge/level trigger value

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

    @note  Here is the control flow of this function:
              1. If *LegacyMask not NULL, set legacy mask variable.
              2. If *LegacyEdgeLevel not NULL, set legacy Trigger Level variable.
              3. If *ProtectedMask not NULL, set protected mask variable.
              4. If *ProtectedEdgeLevel not NULL, set protected trigger level variable.
              5. Call function to program 8259 with mask/trigger of current mode.
              6. Return EFI_SUCCESS.

**/
EFI_STATUS SetMask(
    IN EFI_LEGACY_8259_PROTOCOL *This,
    IN UINT16                   *LegacyMask         OPTIONAL,
    IN UINT16                   *LegacyEdgeLevel    OPTIONAL,
    IN UINT16                   *ProtectedMask      OPTIONAL,
    IN UINT16                   *ProtectedEdgeLevel OPTIONAL
    )
{
    if (LegacyMask)         gIrqMask[0]     = *LegacyMask;
    if (LegacyEdgeLevel)    gIrqTrigger[0]  = *LegacyEdgeLevel;
    if (ProtectedMask)      gIrqMask[1]     = *ProtectedMask;
    if (ProtectedEdgeLevel) gIrqTrigger[1]  = *ProtectedEdgeLevel;

    ProgramIrqMaskTrigger();

    return EFI_SUCCESS;
}

/**
    Sets the interrupt mode operation to legacy or protected.
    New mask and edge/level status can be provided as input

    @param This Pointer to the protocol instance
    @param Mode Interrupt mode setting
    @param Mask New interrupt mask for this mode
    @param EdgeLevel New edge/level trigger value for this mode

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

    @note  Here is the control flow of this function:
              1. If invalid mode, return EFI_INVALID_PARAMETER.
              2. If *Mask not NULL, set mode mask variable.
              3. If *EdgeLevel not NULL, set mode trigger level variable.
              4. Call function to program 8259 with mask/trigger of current mode.
              5. Return EFI_SUCCESS.

**/
EFI_STATUS SetMode(
    IN EFI_LEGACY_8259_PROTOCOL *This,
    IN EFI_8259_MODE            Mode,
    IN UINT16                   *Mask       OPTIONAL,
    IN UINT16                   *EdgeLevel  OPTIONAL
)
{
    if (Mode >= Efi8259MaxMode) return EFI_INVALID_PARAMETER;

    gMode = Mode;
    if (Mask) gIrqMask[Mode] = *Mask;
    if (EdgeLevel) gIrqTrigger[Mode] = *EdgeLevel;

    ProgramIrqMaskTrigger();

    return EFI_SUCCESS;
}

/**
    Returns the vector number for the requested IRQ

    @param This Legacy8259 protocol pointer
    @param Irq IRQ number for which vector is needed
    @param Vector Vector value is returned in this pointer

               
    @retval EFI_SUCCESS Get Irq Vector for Irq.
    @retval EFI_ERROR Initialization failed (see error for more details)

    @note  Here is the control flow of this function:
              1. If invalid irq, return EFI_INVALID_PARAMETER.
              2. If Set *Vector to Irq base + interrupt offset.
              3. Return EFI_SUCCESS.

**/
EFI_STATUS GetVector(
    IN EFI_LEGACY_8259_PROTOCOL *This,
    IN EFI_8259_IRQ             Irq,
    OUT UINT8                   *Vector
)
{
    if ((UINT8)Irq >= (UINT8)Efi8259IrqMax) return EFI_INVALID_PARAMETER;
    *Vector = (Irq <= Efi8259Irq7) ? gMasterBase + Irq : gSlaveBase + Irq - 8;

    return EFI_SUCCESS;
}

/**
    Enable the Interrupt controllers to respond in a specific IRQ.

    @param This Legacy8259 protocol pointer
    @param Irq IRQ that has to be enabled
    @param LevelTriggered Trigger mechanism (level or edge) for this IRQ

               
    @retval EFI_SUCCESS Interrupt on the interrupt controllers was enabled.
    @retval EFI_INVALID_PARAMETER Interrupt not existent.
        The 8259 master/slave supports IRQ 0-15.

    @note  Here is the control flow of this function:
              1. Check if Irq is valid. If not, return EFI_INVALID_PARAMETER.
              2. Clear interrupt mask bit in variable of current mode.
              3. Set/Clear bit of trigger level variable of current mode.
              4. Program mask/trigger.
              5. Return EFI_SUCCESS.

**/
EFI_STATUS EnableIrq(
    IN EFI_LEGACY_8259_PROTOCOL *This,
    IN EFI_8259_IRQ             Irq,
    IN BOOLEAN                  LevelTriggered
)
{
    if ((UINT8)Irq > (UINT8)15) return EFI_INVALID_PARAMETER;

    gIrqMask[gMode] &= ~(1 << Irq);                     //Clear mask for the Irq.

    gIrqTrigger[gMode] &= ~(1 << Irq);                  //Mask Irq to change.
    if (LevelTriggered) gIrqTrigger[gMode] |= (1 << Irq);   // Change irq bit, 0 = edge, 1 = level.

    ProgramIrqMaskTrigger();
    return EFI_SUCCESS;
}


/**
    Disable the Interrupt controllers to stop responding to
    a specific IRQ.

    @param This Legacy8259 protocol object
    @param Irq IRQ that has to be disabled

               
    @retval EFI_SUCCESS Interrupt on the interrupt controllers was enabled.
    @retval EFI_INVALID_PARAMETER Interrupt not existent.
        The 8259 master/slave supports IRQ 0-15.

    @note  Here is the control flow of this function:
              1. Check if Irq is valid. If not, return EFI_INVALID_PARAMETER.
              2. Set interrupt mask bit in variable of current mode.
              3. Program mask/trigger.
              4. Return EFI_SUCCESS.

**/

EFI_STATUS DisableIrq(
    IN EFI_LEGACY_8259_PROTOCOL *This,
    IN EFI_8259_IRQ             Irq
)
{
    if ((UINT8)Irq > (UINT8)15) return EFI_INVALID_PARAMETER;

    gIrqMask[gMode] |= 1 << Irq;    //Set mask for the Irq.

    ProgramIrqMaskTrigger();
    return EFI_SUCCESS;
}


/**
    Get IRQ vector asigned to Pci card.

    @param This Pointer to the protocol instance
    @param PciHandle PCI handle for this device
    @param Vector Interrupt vector for this device

               
    @retval EFI_SUCCESS Vector returned.
    @retval EFI_INVALID_PARAMETER Invalid PciHandle.

    @note  Here is the control flow of this function:
              1. Get Handle of PciIo installed on PciHandle.
              2. If PciIo not installed, return EFI_INVALID_DEVICE.
              3. Set *vector to Irq vector programmed into card.
              4. Return EFI_SUCCESS.

**/
EFI_STATUS GetInterruptLine(
    IN EFI_LEGACY_8259_PROTOCOL *This,
    IN EFI_HANDLE               PciHandle,
    OUT UINT8                   *Vector
)
{
    EFI_STATUS              Status;
    EFI_PCI_IO_PROTOCOL     *PciIo;
    UINT8                   InterruptLine;

    Status = pBS->HandleProtocol(PciHandle,
        &gEfiPciIoProtocolGuid,
        &PciIo);
    if (EFI_ERROR(Status)) return EFI_INVALID_PARAMETER;

    PciIo->Pci.Read(PciIo,
        EfiPciIoWidthUint8,
        0x3c,
        1,
        &InterruptLine);    //Read vector from card.

    *Vector = InterruptLine;

    return EFI_SUCCESS;
}

/**
    Send end of interrupt command to interrupt controller(s).

    @param This Pointer to the protocol instance
    @param Irq IRQ number for this EOI has to be sent

               
    @retval EFI_SUCCESS EOI command sent to controller(s)
    @retval EFI_INVALID_PARAMETER Interrupt not existent. The 8259 master/slave supports IRQ 0-15.

    @note  Here is the control flow of this function:
              1. Check if Irq is valid. If not, return EFI_INVALID_PARAMETER.
              2. If Irq >= 8, then Send EOI to slave controller.
              3. Send EOI to master controller. (This is for both master/slave Irqs)
              4. Return EFI_SUCCESS.

**/
EFI_STATUS EndOfInterrupt(
    IN EFI_LEGACY_8259_PROTOCOL *This,
    IN EFI_8259_IRQ             Irq
)
{
    if (Irq > 15) return EFI_INVALID_PARAMETER;

    if (Irq >= 8) {                     //If Slave, send EOI to slave first.
        IoWrite8 (0xa0, EOI_COMMAND);   //Send Slave EOI
    }

    IoWrite8 (0x20, EOI_COMMAND);       //Send Master EOI

    return EFI_SUCCESS;
}

EFI_LEGACY_8259_PROTOCOL gLegacy8259Protocol = {
    SetVectorBase,
    GetMask, SetMask,
    SetMode,
    GetVector,
    EnableIrq, DisableIrq,
    GetInterruptLine,
    EndOfInterrupt
};


/**
    Program the Irq Mask and Trigger.

    @param ImageHandle Image handle for this driver
    @param SystemTable Pointer to the sytem table

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

    @note  Here is the control flow of this function:
              1. Initialize Ami Lib.
              2. Install Legacy 8259 Interface. If error, return error.
              3. Initialize the Cpu setting vector bases.
              4. Set Cpu Mode, mask, and trigger level.
              5. Return EFI_SUCCESS.

**/
EFI_STATUS Initialize8259(
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
)
{
    EFI_STATUS  Status;
    UINT16      Mask        = 0xffff;   //Mask all interrupts.
    UINT16      EdgeLevel   = 0x00;     //Set all edge.
    BOOLEAN     IntState = CPULib_GetInterruptState();

    CPULib_DisableInterrupt();
    SetVectorBase(0, MASTER_INTERRUPT_BASE, SLAVE_INTERRUPT_BASE);
    SetMode(0, Efi8259ProtectedMode, &Mask, &EdgeLevel);

    Status = pBS->InstallMultipleProtocolInterfaces(
                    &ImageHandle,
                    &gEfiLegacy8259ProtocolGuid, &gLegacy8259Protocol,
                    NULL
                    );

    if (IntState) CPULib_EnableInterrupt();

    return Status;
}

/**
    This Function Installs the Sb Hdd Policy Protocol
        

    @param VOID
           
               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details) 

**/
EFI_STATUS
InstallSbPolicyProtocol()
{
	EFI_STATUS  Status = EFI_SUCCESS;
	EFI_HANDLE  Handle = NULL;

    Status = pBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gAmiSbHddPolicyProtocolGuid,
                  &mSbHddPolicy,
                  NULL
                  );

    return Status;
}

/**
    This function returns the Port Number to which the Hard Disk 
    is connected.        

    @param ControllerNumber 
    @param PrimarySecondary 
    @param MasterSlave 
    @param PortNumber 
             
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)   

**/
EFI_STATUS
GeneratePortNumber(	 
    IN    UINT8    DeviceNumber,
    IN    UINT8    FunctionNumber,
    IN    UINT32   IdeChannel,
    IN    UINT32   IdeDevice,
    OUT   UINT32   *PortNumber
)
{
	EFI_STATUS                  Status;
	HYGON_FCH_AMI_POLICY_PROTOCOL	static *HygonSbPolicy = NULL;

    //
    // Locate HYGON-FCH AMI Policy
    //
    if (HygonSbPolicy == NULL) {
		Status = pBS->LocateProtocol(&gHygonFchAmiPolicyProtocolGuid, NULL, &HygonSbPolicy);
		ASSERT_EFI_ERROR(Status);
    }

    // Is Pata or Sata controller ?
    if (DeviceNumber == FCH_IDE_DEV) {
        if (HygonSbPolicy->SataIdeCombMdPriSecOpt == 0) {
            // SATA as primary
            *PortNumber = PATA_PORT_NUM[IdeChannel][IdeDevice];
        }else{
            // SATA as secondary
            *PortNumber = PATA_PORT_NUM[IdeChannel ^ 1][IdeDevice];
        }
    } else  {
        if (HygonSbPolicy->SataClass == 1 || HygonSbPolicy->SataClass == 2 || HygonSbPolicy->SataClass == 5) {
            // RAID / AHCI / AHCI as 7804
            *PortNumber = IdeChannel;
        } else {
            // others
            *PortNumber = SATA_PORT_NUM[IdeChannel][IdeDevice];
        }
    }

    return  EFI_SUCCESS;
}

/**
    This function set the FCH MMIO as non-posted and rearange the      
    MMIO range

    @param VOID
           
    @retval VOID

**/
VOID
FchMmioRangeWrite (
  VOID
  )
{
    MMIO_RANGE_STRUCT       FchTemp;
    OPTIMUM_FCH_MMIO_STRUCT TempRange;
    UINT32                  TempReg;
    UINT16                  MMIORange0L, MMIORange0H;
    UINT16                  NPostRangeL, NPostRangeH;
   // BOOLEAN                 Lpc0Used, Lpc1Used, TpmUsed, WatchDogUsed;

    UINT16 HpetBase;
    //UINT16 WatchDogBase;
    UINT16 IoapicBase;
    UINT16 AcpiMmioBase;

    //Lpc0Used = FALSE;
    //Lpc1Used = TRUE;
    //TpmUsed = FALSE;
    //WatchDogUsed = FALSE;
    
    FchTemp.Lpc0Base = 0;
    FchTemp.Lpc0Limit = 0;
    FchTemp.Lpc1Base = 0;
    FchTemp.Lpc1Limit = 0;

#ifdef HPET_BASE_ADDRESS
    HpetBase = (HPET_BASE_ADDRESS >> 16);
#else
    HpetBase = 0;
#endif
//#ifdef WDT_BASE_ADDRESS
//    WatchDogBase = (WDT_BASE_ADDRESS >> 16);
//#else
//    WatchDogBase = 0;
//#endif
#ifdef APCB
    IoapicBase = (APCB >> 16);
#else
    IoapicBase = 0;
#endif
#ifdef SB_MMIO_BASE
    AcpiMmioBase = (SB_MMIO_BASE >> 16);
#else
    AcpiMmioBase = 0;
#endif
    
    // Fill all FCH Mmio range
    //if (Lpc0Used) {
    //    // Lpc ROM 1 Base read from FCH
    //    FchTemp.Lpc0Base = READ_PCI16(0, 0x14, 3, 0x68);
    //    // Lpc ROM 1 Limit read from FCH
    //    FchTemp.Lpc0Limit = READ_PCI16(0, 0x14, 3, 0x6A);
    //}

    //if (Lpc1Used) {
        // Lpc ROM 2 Base read from FCH
        FchTemp.Lpc1Base = READ_PCI16(0, 0x14, 3, 0x6C);
        // Lpc ROM 2 Limit read from FCH
        FchTemp.Lpc1Limit = READ_PCI16(0, 0x14, 3, 0x6E);
    //}

    // Spi Base Address read from FCH
    FchTemp.SpiBase = READ_PCI16(0, 0x14, 3, 0xA2);
    // Spi base address limit is less then 64K
    
    //if (TpmUsed) {
    //    // Tpm Base Address read from FCH
    //    FchTemp.TmpBase = READ_PCI16(0, 0x14, 3, 0x86);
    //    // Tpm Limit Address read from FCH
    //    FchTemp.TmpLimit = READ_PCI16(0, 0x14, 3, 0x8A);
    //}

    // HPET Base Address is FED0_0000h ~ FED0_01FFh
    FchTemp.HpetBase = HpetBase;
    // HPET base address limit is less then 64K
    
    // BIOS RAM base Address is FED1_0000h(256 bytes)
    FchTemp.BiosRamBase  = 0xFED1;
    // BIOS RAM address limit is less then 64K
    
    //if (WatchDogUsed) {
    //    // WatchDog base address is FEB0_0000h ~ FEB0_000Fh
    //    FchTemp.WatchDogBase = WatchDogBase;
    //    // WatchDog address limit is less then 64K
    //}    
    
    // IoApic base Address is FEC0_0000h ~ FEC0_007Fh
    FchTemp.IoapicBase = IoapicBase;
    // IoApic address limit is less then 64K
    
    // ACPI Mmio base address is FED8_0000h
    FchTemp.AcpiMmioBase = AcpiMmioBase;
    // ACPI Mmio address limit is less then 64K
    
    // Intent all other filed (except LPC) combine to one big MMIO range.
    NPostRangeL = FchTemp.SpiBase;
    NPostRangeH = FchTemp.SpiBase;

    if (FchTemp.HpetBase < NPostRangeL) {
        NPostRangeL = FchTemp.HpetBase;
    }
    if (FchTemp.HpetBase > NPostRangeH) {
        NPostRangeH = FchTemp.HpetBase;
    }

    if (FchTemp.BiosRamBase < NPostRangeL) {
        NPostRangeL = FchTemp.BiosRamBase;
    }
    if (FchTemp.BiosRamBase > NPostRangeH) {
        NPostRangeH = FchTemp.BiosRamBase;
    }

    if (FchTemp.IoapicBase < NPostRangeL) {
        NPostRangeL = FchTemp.IoapicBase;
    }
    if (FchTemp.IoapicBase > NPostRangeH) {
        NPostRangeH = FchTemp.IoapicBase;
    }

    if (FchTemp.AcpiMmioBase < NPostRangeL) {
        NPostRangeL = FchTemp.AcpiMmioBase;
    }
    if (FchTemp.AcpiMmioBase > NPostRangeH) {
        NPostRangeH = FchTemp.AcpiMmioBase;
    }

    //if (Lpc0Used) {
    //    if (FchTemp.Lpc0Base < NPostRangeL) {
    //        NPostRangeL = FchTemp.Lpc0Base;
    //    }
    //    if (FchTemp.Lpc0Limit > NPostRangeH){
    //        NPostRangeH = FchTemp.Lpc0Limit;
    //    }
    //}

    //if (Lpc1Used) {
        if (FchTemp.Lpc1Base < NPostRangeL) {
            NPostRangeL = FchTemp.Lpc1Base;
        }
        if (FchTemp.Lpc1Limit > NPostRangeH){
            NPostRangeH = FchTemp.Lpc1Limit;
        }
    //}

    //if (TpmUsed) {
    //    if (FchTemp.TmpBase < NPostRangeL) {
    //        NPostRangeL = FchTemp.TmpBase;
    //    }
    //    if (FchTemp.TmpLimit > NPostRangeH){
    //        NPostRangeH = FchTemp.TmpLimit;
    //    }
    //}

    //if (WatchDogUsed) {
    //    if (FchTemp.WatchDogBase < NPostRangeL) {
    //        NPostRangeL = FchTemp.WatchDogBase;
    //    }
    //    if (FchTemp.WatchDogBase > NPostRangeH){
    //        NPostRangeH = FchTemp.WatchDogBase;
    //    }
    //}


    // Rearange the MMIO range to avoid overlap
    TempRange.Range0Base = 0;
    TempRange.Range0Limit = 0;
    TempRange.Range1Base = 0;
    TempRange.Range1Limit = 0;
    TempRange.Range2Base = 0;
    TempRange.Range2Limit = 0;

    TempReg = READ_PCI32(0, 0x18, 1, 0x80);
    MMIORange0L = (UINT16)(TempReg >> 8);
    TempReg = READ_PCI32(0, 0x18, 1, 0x84);
    MMIORange0H = (UINT16)(TempReg >> 8);


    if ((MMIORange0L >= NPostRangeL) && (MMIORange0H <= NPostRangeH)) {
        TempRange.Range1Base = NPostRangeL;
        TempRange.Range1Limit = NPostRangeH;
    }

    if (MMIORange0L < NPostRangeL) {
        if (MMIORange0H < NPostRangeL) {
            TempRange.Range0Base = MMIORange0L;
            TempRange.Range0Limit = MMIORange0H;
            TempRange.Range1Base = NPostRangeL;
            TempRange.Range1Limit = NPostRangeH;
        }
        if ((MMIORange0H >= NPostRangeL) && (MMIORange0H <= NPostRangeH)) {
            TempRange.Range0Base = MMIORange0L;
            TempRange.Range0Limit = NPostRangeL - 1;
            TempRange.Range1Base = NPostRangeL;
            TempRange.Range1Limit = NPostRangeH;
        }
    }

    if (MMIORange0H > NPostRangeH) {
        if (MMIORange0L > NPostRangeH) {
            TempRange.Range0Base = NPostRangeH;
            TempRange.Range0Limit = MMIORange0H;
            TempRange.Range1Base = NPostRangeL;
            TempRange.Range1Limit = NPostRangeH;
        }
        if ((MMIORange0L >= NPostRangeL) && (MMIORange0L <= NPostRangeH)) {
            TempRange.Range0Base = NPostRangeH + 1;
            TempRange.Range0Limit = MMIORange0H;
            TempRange.Range1Base = NPostRangeL;
            TempRange.Range1Limit = NPostRangeH;
        }
    }

    if ((MMIORange0L < NPostRangeL) && (MMIORange0H > NPostRangeH)) {
        TempRange.Range0Base = MMIORange0L;
        TempRange.Range0Limit = NPostRangeL - 1;
        TempRange.Range1Base = NPostRangeL;
        TempRange.Range1Limit = NPostRangeH;
        TempRange.Range2Base = NPostRangeH + 1;
        TempRange.Range2Limit = MMIORange0H;
    } 
    
    if (gBootScriptSave == NULL) {
	EFI_STATUS      Status;
        Status = pBS->LocateProtocol(&gEfiS3SaveStateProtocolGuid,NULL, \
                    &gBootScriptSave);
        ASSERT_EFI_ERROR(Status);
    }

    WRITE_PCI32_S3(gBootScriptSave, 0, 0x18, 1, 0x80, (TempRange.Range0Base << 8) + 0x03 );
    WRITE_PCI32_S3(gBootScriptSave, 0, 0x18, 1, 0x84, (TempRange.Range0Limit << 8));
    
    if ((TempRange.Range1Limit) != 0) {
        WRITE_PCI32_S3(gBootScriptSave, 0, 0x18, 1, 0x88, (TempRange.Range1Base << 8) + 0x03 );
        WRITE_PCI32_S3(gBootScriptSave, 0, 0x18, 1, 0x8c, (TempRange.Range1Limit << 8) + 0x80);
    }

    if ((TempRange.Range2Limit) != 0) {
        WRITE_PCI32_S3(gBootScriptSave, 0, 0x18, 1, 0x90, (TempRange.Range2Base << 8) + 0x03 );
        WRITE_PCI32_S3(gBootScriptSave, 0, 0x18, 1, 0x94, (TempRange.Range2Limit << 8));
    }
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
