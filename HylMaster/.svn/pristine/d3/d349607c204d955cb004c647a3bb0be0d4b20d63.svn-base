#include <Efi.h>
#include <token.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Dxe.h>
#include "Library\UefiLib.h"

#include "Fch.h"

VOID OemPostCompleteGpioDrive (VOID)
{
  
  //get current gpio value
  UINT8                   Value;

  Value = MmioRead8 (ACPI_MMIO_BASE + GPIO_BANK0_BASE + (BIOS_POST_COMPLT_PIN << 2) + 2);
  DEBUG ((DEBUG_INFO, "Value = %x\n",Value));
                          
  //the highest two bits are reserved
  Value = Value & 0xC7; 
                          
  //pull up
  Value |= 0x10; 
                          
  //output enable
  Value |= 0x80;
                          
  //output state
  Value &= 0xBF;
                          
  MmioWrite8 (ACPI_MMIO_BASE + GPIO_BANK0_BASE + (BIOS_POST_COMPLT_PIN << 2) + 2, Value);
                          
  //Set Multi-function IO pin function select of GPIO
  MmioWrite8 (ACPI_MMIO_BASE + IOMUX_BASE + BIOS_POST_COMPLT_PIN, 0x02);
                          
  Value = MmioRead8 (ACPI_MMIO_BASE + GPIO_BANK0_BASE + (BIOS_POST_COMPLT_PIN << 2) + 2);
  DEBUG ((DEBUG_INFO, "Value = %x\n",Value));
                          
  DEBUG ((DEBUG_INFO, "BIOS POST Complete !!\n"));

}

VOID OemReadyToBootNotify(
  IN EFI_EVENT Event, 
  IN VOID *Context)
{ 
  //<Yaosr002-20200819 Add the start time of each page. +>
#if defined(REPORT_POST_START_TIME) && (REPORT_POST_START_TIME == 1)
  UINT16  Year;
  UINT8   Month, Day, Hour, Minute, Second;
  
  Year = ReadRtcIndex(0x09);
  Month = ReadRtcIndex (0x08);
  Day = ReadRtcIndex (0x07);
  Hour = ReadRtcIndex (0x04);
  Minute = ReadRtcIndex (0x02);
  Second = ReadRtcIndex (0x00);

  DEBUG ((EFI_D_ERROR, "OemDbg:\nBIOS Post Complete: 20%.2d/%.2d/%.2d %.2d:%.2d:%.2d\n", Year, Month, Day, Hour, Minute, Second));
#endif
  //<Yaosr002-20200819 Add the start time of each page. ->
  OemPostCompleteGpioDrive();
}

EFI_STATUS
OemPostComplete(
  IN  EFI_SYSTEM_TABLE *SystemTable
)
{
  EFI_STATUS    Status=EFI_SUCCESS;

  Status = EfiNamedEventListen (
               &gAmiLegacyBootProtocolGuid,
               TPL_CALLBACK,
               OemReadyToBootNotify,
               NULL,
               NULL );
  if(EFI_ERROR (Status))
    return Status;

  Status = EfiNamedEventListen (
               &gAmiTseEventBeforeBootGuid,
               TPL_CALLBACK,
               OemReadyToBootNotify,
               NULL,
               NULL );
  
  return Status;
}