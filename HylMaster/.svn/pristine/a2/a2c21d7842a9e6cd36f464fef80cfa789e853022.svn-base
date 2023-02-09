/**
//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2016, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
*/

#include <Library/DebugLib.h>
#include <AmiDxeLib.h>
#include <Protocol/TerminalAmiSerial.h>
#include <Protocol/DevicePath.h>
#include <Library/PcdLib.h>
#include <Token.h>

#include <Library/MemoryAllocationLib.h>
#include <Protocol/SuperIo.h>
#include <AcpiRes.h>


extern EFI_GUID     gEfiSioProtocolGuid;
EFI_SIO_PROTOCOL	    gEfiSio;

ASLR_FixedIO    AmiSerialBaseAddress;
ASLR_IRQNoFlags AmiSerialIrq;

typedef struct {
  VENDOR_DEVICE_PATH        Guid;
  ACPI_HID_DEVICE_PATH	AcpiDp;
  UART_DEVICE_PATH          Uart;
  EFI_DEVICE_PATH_PROTOCOL  End;
} SIMPLE_TEXT_OUT_DEVICE_PATH;

SIMPLE_TEXT_OUT_DEVICE_PATH mDevicePathUart1 = {
  {
    { HARDWARE_DEVICE_PATH, HW_VENDOR_DP, sizeof (VENDOR_DEVICE_PATH), 0},
    EFI_CALLER_ID_GUID // Use the drivers GUID
  },
  { { ACPI_DEVICE_PATH, ACPI_DP, sizeof(ACPI_HID_DEVICE_PATH)}, EISA_PNP_ID(0x501), 1 },
  {
    { MESSAGING_DEVICE_PATH, MSG_UART_DP, sizeof (UART_DEVICE_PATH), 0},
    0,        // Reserved
    FixedPcdGet64 (PcdUartDefaultBaudRate),   // BaudRate
    FixedPcdGet8 (PcdUartDefaultDataBits),    // DataBits
    FixedPcdGet8 (PcdUartDefaultParity),      // Parity (N)
    FixedPcdGet8 (PcdUartDefaultStopBits)     // StopBits
  },
  { END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE, sizeof (EFI_DEVICE_PATH_PROTOCOL), 0}
};

#if FchUart1LegacyEnableToken == 0
BOOLEAN gUart1LegacyIo   = FALSE;
#else
BOOLEAN gUart1LegacyIo   = TRUE;
#endif

#if FchUart1LegacyEnableToken == 0
	#define FCH_UART1_BASE_ADDRESS_IO 0x0
#elif FchUart1LegacyEnableToken == 1
	#define FCH_UART1_BASE_ADDRESS_IO 0x2E8
#elif FchUart1LegacyEnableToken == 2
	#define FCH_UART1_BASE_ADDRESS_IO 0x2F8
#elif FchUart1LegacyEnableToken == 3
	#define FCH_UART1_BASE_ADDRESS_IO 0x3E8
#elif FchUart1LegacyEnableToken == 4
	#define FCH_UART1_BASE_ADDRESS_IO 0x3F8
#endif

/**
    Gets the UART Clock .
 
    @param This Pointer to AmiSerialProtocol
    @param Clock Pointer to return UART Clock.

    @retval VOID
    
    @note This API added for non-generic serial ports other than PCI serial 
    ports.

**/

VOID
GetUartClock (
    IN  AMI_SERIAL_PROTOCOL *This,
    OUT UINTN              *Clock
)
{

    if(!gUart1LegacyIo){
    *Clock=PCI_UART_INPUT_CLOCK;
    } else {
        *Clock = LEGACY_UART_INPUT_CLOCK;
    }
    return;
}


// <AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: GetBaseAddressUart1
//
// Description:
//  Gets the base address for the Serial Port.
// 
// Input:
//  IN  AMI_SERIAL_PROTOCOL *This - Pointer to AmiSerialProtocol
//  OUT UINT64              *BaseAddress - Pointer to return Base Address  
//
// Output:
//  VOID
//
// Modified:
//
// Referrals:
//
// Notes:
//--------------------------------------------------------------------------- 
// <AMI_PHDR_END>

VOID GetBaseAddressUart1 (
    IN  AMI_SERIAL_PROTOCOL *This,
    OUT UINT64              *BaseAddress
) 
{
    if ((FchRTDeviceEnableMapToken & BIT12) != 0) {
	    if(!gUart1LegacyIo){
		    *BaseAddress = FCH_UART1_BASE_ADDRESS;
	    } else {
		    *BaseAddress = FCH_UART1_BASE_ADDRESS_IO;
	    }
    } else {
	    *BaseAddress = 0;
    }
    return;
}

// <AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: CheckPciMmio
//
// Description:
//  Gives information whether it is a pci serial device, if yes,
//  tells whether it is a MMIO device.
// 
// Input:
//  IN  AMI_SERIAL_PROTOCOL *This - Pointer to AmiSerialProtocol
//  OUT BOOLEAN             *Pci  -  BOOLEAN to fill whether it is PCI device 
//  OUT BOOLEAN             *Mmio - BOOLEAN to fill whether it is MMIO access
//
// Output:
//  VOID
//
// Modified:
//
// Referrals:
//
// Notes:
//--------------------------------------------------------------------------- 
// <AMI_PHDR_END>

VOID CheckPciMmio(
    IN  AMI_SERIAL_PROTOCOL *This,
    OUT BOOLEAN             *Pci,
    OUT BOOLEAN             *Mmio,
    OUT UINT8               *MmioWidth
)
{
    *Pci = TRUE;
    if(!gUart1LegacyIo){
    *Mmio = TRUE;
    *MmioWidth = PCI_SERIAL_MMIO_WIDTH;
    } else {
        *Mmio = FALSE;
        *MmioWidth = 0;
    }
    return;
}

// <AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: GetPciLocation
//
// Description:
//  This Function will give the Bus/Dev/func if it is a PCI Serial Device.
// 
// Input:
//  IN  AMI_SERIAL_PROTOCOL *This - Pointer to AmiSerialProtocol
//  OUT UINTN               *Bus - Pointer to return Bus No.
//  OUT UINTN               *Dev - Pointer to return Device No.
//  OUT UINTN               *Func- Pointer to return Function No.
//  OUT UINT8               *Port- Pointer to return Port No.
//
// Output:
//  VOID
//
// Modified:
//
// Referrals:
//
// Notes:
//--------------------------------------------------------------------------- 
// <AMI_PHDR_END>

VOID GetPciLocation(
    IN  AMI_SERIAL_PROTOCOL *This,
    OUT UINTN               *Bus,
    OUT UINTN               *Dev,
    OUT UINTN               *Func,
    OUT UINT8               *Port
)
{

    *Bus = 0x0;
    *Dev = 0x0;
    *Func = 0x0;
    *Port = 0x1;

    return;
}

// <AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: GetSerialIRQ
//
// Description:
//  Gets the IRQ number for the Serial Port.
// 
// Input:
//  IN  AMI_SERIAL_PROTOCOL *This - Pointer to AmiSerialProtocol
//  OUT UINT8               *SerialIRQ - Pointer to return IRQ number.
//
// Output:
//  VOID
//
// Modified:
//
// Referrals:
//
// Notes:
//--------------------------------------------------------------------------- 
// <AMI_PHDR_END>

VOID GetSerialIRQ(
    IN  AMI_SERIAL_PROTOCOL *This,
	OUT UINT8             *SerialIRQ
)
{
    *SerialIRQ = 0x03; 
    return;
}

VOID GetUid (
    IN  AMI_SERIAL_PROTOCOL *This,
    OUT UINT8               *Uid
) 
{
    *Uid = 1;
    return;
}


EFI_HANDLE	 gHandle=NULL;

AMI_SERIAL_PROTOCOL gAmiSerialProtocolUart1 = {
  GetBaseAddressUart1,
  CheckPciMmio,
  GetPciLocation,
  GetSerialIRQ,
  GetUid,
  GetUartClock
};


//
// EFI SIO protocol API definition
//

//<AMI_PHDR_START>
//============================================================================
// Procedure:   AmiSerialEfiSioRegisterAccess
//
// Description: Get the Serial Device Register details
//
// Input:
//
//          IN EFI_SIO_PROTOCOL *This,
//          IN BOOLEAN          Write,
//          IN BOOLEAN          ExitCfgMode,
//          IN UINT8            Register,
//          IN OUT UINT8        *Value
//
// Output:
//          EFI_STATUS
//          This functionality is not supported for Serial device
//
// Referrals:
//
//============================================================================
//<AMI_PHDR_END>
EFI_STATUS
EFIAPI
AmiSerialEfiSioRegisterAccess(
  IN   CONST  EFI_SIO_PROTOCOL  *This,
  IN          BOOLEAN           Write,
  IN          BOOLEAN           ExitCfgMode,
  IN          UINT8             Register,
  IN OUT      UINT8             *Value
)
{
    return EFI_UNSUPPORTED;
}

//<AMI_PHDR_START>
//============================================================================
// Procedure:   AmiSerialEfiSioGetResources
//
// Description: Get the Serial Device current resource
//
// Input:
//
//          IN  CONST EFI_SIO_PROTOCOL            *This,
//          OUT       ACPI_RESOURCE_HEADER_PTR    *EfiResourceList
//
// Output:
//          EFI_STATUS
//          Returns the serial device list of the current resources
//
// Referrals:
//
//============================================================================
//<AMI_PHDR_END>
EFI_STATUS
EFIAPI
AmiSerialEfiSioGetResources(
  IN  CONST EFI_SIO_PROTOCOL            *This,
  OUT       ACPI_RESOURCE_HEADER_PTR    *EfiResourceList
)

{	
    VOID                 *EfiSioRes = NULL;
    UINT8                *TempRes = NULL;
    UINT32               Size = 0;
    
    if (!This || !EfiResourceList)
        return EFI_INVALID_PARAMETER;

    // Serial Device Base address Resource
    AmiSerialBaseAddress.Hdr.Length = sizeof(ASLR_FixedIO) - sizeof(ASLRF_S_HDR);
    AmiSerialBaseAddress.Hdr.Name   = ASLV_RT_FixedIO;
    AmiSerialBaseAddress.Hdr.Type   = ASLV_SMALL_RES;
    AmiSerialBaseAddress._BAS       = FCH_UART1_BASE_ADDRESS_IO;
    AmiSerialBaseAddress._LEN       = 0x8;
    Size += sizeof(ASLR_FixedIO);
    
    // Device IRQ details
    AmiSerialIrq.Hdr.Length = sizeof(ASLR_IRQNoFlags) - sizeof(ASLRF_S_HDR);
    AmiSerialIrq.Hdr.Name   = ASLV_RT_IRQ;
    AmiSerialIrq.Hdr.Type   = ASLV_SMALL_RES;
    AmiSerialIrq._INT       = 0x03;
    Size += sizeof(ASLR_IRQNoFlags);
    
    TempRes = AllocateZeroPool(Size + sizeof(ASLR_EndTag));
    
    EfiSioRes = TempRes;
    
    if(!EfiSioRes)
        return EFI_OUT_OF_RESOURCES;

    MemCpy(TempRes, &AmiSerialBaseAddress, sizeof(ASLR_FixedIO));
    TempRes += sizeof(ASLR_FixedIO);

    MemCpy(TempRes, &AmiSerialIrq, sizeof(ASLR_IRQNoFlags));
    TempRes += sizeof(ASLR_IRQNoFlags);

    ((ASLR_EndTag*)TempRes)->Hdr.HDR = ASLV_END_TAG_HDR;
    ((ASLR_EndTag*)TempRes)->Chsum = 0;

    EfiResourceList->SmallHeader = EfiSioRes;
    
    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//============================================================================
// Procedure:   AmiSerialEfiSioSetResources
//
// Description: Sets the Serial Device current resource
//
// Input:
//
//          IN CONST  EFI_SIO_PROTOCOL        *This,
//          IN        ACPI_RESOURCE_HEADER_PTR ResourceList
//
// Output:
//          EFI_STATUS
//          This functionality is not supported for Serial device
//
// Referrals:
//
//============================================================================
//<AMI_PHDR_END>
EFI_STATUS
EFIAPI
AmiSerialEfiSioSetResources(
  IN CONST  EFI_SIO_PROTOCOL        *This,
  IN        ACPI_RESOURCE_HEADER_PTR ResourceList
)
{
    return EFI_UNSUPPORTED;
}

//<AMI_PHDR_START>
//============================================================================
// Procedure:   AmiSerialEfiSioPossibleResources
//
// Description: Provides combination of resources that can be used by the device
//
// Input:
//
//          IN CONST  EFI_SIO_PROTOCOL        *This,
//          OUT       ACPI_RESOURCE_HEADER_PTR ResourceCollection
//
// Output:
//          EFI_STATUS
//          This functionality is not supported for Serial device
//
// Referrals:
//
//============================================================================
//<AMI_PHDR_END>
EFI_STATUS
EFIAPI
AmiSerialEfiSioPossibleResources(
  IN  CONST EFI_SIO_PROTOCOL         *This,
  OUT       ACPI_RESOURCE_HEADER_PTR *ResourceCollection
)
{
    return EFI_UNSUPPORTED;
}

//<AMI_PHDR_START>
//============================================================================
// Procedure:   AmiSerialEfiSioModify
//
// Description: Interface for multiple registers programming with single call
//
// Input:
//
//          IN CONST EFI_SIO_PROTOCOL         *This,
//          IN CONST EFI_SIO_REGISTER_MODIFY  *Command,
//          IN       UINTN                    NumberOfCommands
//
// Output:
//          EFI_STATUS
//          This functionality is not supported for Serial device
//
// Referrals:
//
//============================================================================
//<AMI_PHDR_END>
EFI_STATUS
EFIAPI
AmiSerialEfiSioModify(
  IN CONST EFI_SIO_PROTOCOL         *This,
  IN CONST EFI_SIO_REGISTER_MODIFY  *Command,
  IN       UINTN                    NumberOfCommands
)
{
    return EFI_UNSUPPORTED;
}


// <AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: AmiSerialEntryPoint
//
// Description:
//  Entry point for AMI Serial Driver.It installs AMI Serial DriverBinding
//  Protocol.
//
// Input:
//  IN EFI_HANDLE ImageHandle - The image handle.
//  IN EFI_SYSTEM_TABLE *SystemTable - A pointer to the EFI system table.
//
// Output:
//  EFI_STATUS   
//
// Modified:
//
// Referrals:
//  
//
// Notes:
//
//--------------------------------------------------------------------------- 
// <AMI_PHDR_END>

EFI_STATUS EFIAPI
AmiSerialEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{

    EFI_STATUS              Status;
    
#if SioSerial_SUPPORT
    //
    // Install AmiSerial protocol interfaces for the serial device.
    //    
    Status = SystemTable->BootServices->InstallMultipleProtocolInterfaces (
		    &gHandle, 
		    &gAmiSerialProtocolGuid,   &gAmiSerialProtocolUart1,
		    &gEfiDevicePathProtocolGuid, &mDevicePathUart1, 
		    NULL
	      	      );

    ASSERT_EFI_ERROR (Status);
#else
    gEfiSio.RegisterAccess     = AmiSerialEfiSioRegisterAccess;
    gEfiSio.GetResources       = AmiSerialEfiSioGetResources;
    gEfiSio.SetResources       = AmiSerialEfiSioSetResources;
    gEfiSio.PossibleResources  = AmiSerialEfiSioPossibleResources;
    gEfiSio.Modify             = AmiSerialEfiSioModify;
    
    //
    // Install AmiSerial protocol interfaces for the serial device.
    //
 	Status = SystemTable->BootServices->InstallMultipleProtocolInterfaces (
	         	&gHandle, 
	         	&gAmiSerialProtocolGuid,   &gAmiSerialProtocolUart1,
                &gEfiSioProtocolGuid, &gEfiSio,
	         	&gEfiDevicePathProtocolGuid, &mDevicePathUart1, 
	        	NULL
	         	);

	ASSERT_EFI_ERROR (Status);
#endif
    
	return Status;
}

/**
//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2016, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
*/
