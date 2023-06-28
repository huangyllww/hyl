#include<Library/IoLib.h>
#include<Library/DebugLib.h>
#include"XR28v382PlatformHookSerialPortLib.h"
UINT8
ReadXr28v382Reg(
    UINT8    RegOffset
)
{
    UINT8    RegValue;

    IoWrite8(XR28V382_LPC_INDEX,RegOffset);
    RegValue = IoRead8(XR28V382_LPC_DATA);

    return RegValue;
}

VOID
WriteXr28v382Reg(
    UINT8    RegOffset,
    UINT8    Data
)
{
    IoWrite8(XR28V382_LPC_INDEX,RegOffset);
    IoWrite8(XR28V382_LPC_DATA,Data);
}

VOID
EnXr28v382Uart(void)
{
	/// Enter Conf-mode
	/// write entry key (0xA0) twice to configuration port
	/// enable access to the configuration registers
        IoWrite8(XR28V382_LPC_INDEX,XR28V382_LPC_CRA0);
        IoWrite8(XR28V382_LPC_INDEX,XR28V382_LPC_CRA0);
        
	/// select the input clock frequency 24MHz
        WriteXr28v382Reg(XR28V382_LPC_CR25, 0x0);

	/// select the UART Channel A
        WriteXr28v382Reg(XR28V382_LPC_CR07, 0x0);

	/// Set the UART Channel A base address high byte as 0x3
        WriteXr28v382Reg(XR28V382_LPC_CR60, 0x3);

	/// Set the UART Channel A base address low byte as 0xF8
        WriteXr28v382Reg(XR28V382_LPC_CR61, 0xF8);

	/// Set the FIFO size 16 bytes,  RX trigger level 1, 4, 8, 14 and no delay for THR empty interrupt
        WriteXr28v382Reg(XR28V382_LPC_CRF6, 0x0);

	/// Enable the UART Channel A
        WriteXr28v382Reg(XR28V382_LPC_CR30, 0x1);

	/// Disable access to the configuration registers
        IoWrite8(XR28V382_LPC_INDEX,XR28V382_LPC_CRAA);

}
/**
  Performs platform specific initialization required for the CPU to access
  the hardware associated with a SerialPortLib instance.  This function does
  not intiailzie the serial port hardware itself.  Instead, it initializes
  hardware devices that are required for the CPU to access the serial port
  hardware.  This function may be called more than once.

  @retval  RETURN_SUCCESS    The platform specific initialization succeeded.
  @retval  Others            The platform specific initialization could not be completed.
**/
RETURN_STATUS
EFIAPI
PlatformHookSerialPortInitialize (VOID)
{

    EnXr28v382Uart();

    return  RETURN_SUCCESS;
}
