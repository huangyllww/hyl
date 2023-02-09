//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************

/** @file
    LIB driver for Resetting the Video Controller Programming done in the PEI 

**/

#include <Include/AmiLib.h>
#include <Library/DebugLib.h>
#include <Protocol/AmiDxeTextOut.h>
#include <AmiDxeLib.h>
#include <Token.h>

typedef struct {
    UINT8   Bus;
    UINT8   Dev;
    UINT8   Func;
} DEV_PATH;

// For AST video: PCIe Root Port<->PCIe-to-PCI<->VGA
DEV_PATH mVideoVidPath [] = {
    {VGA_ROOT_PORT_BUS,0x1,VGA_ROOT_PORT_FUN},
    {VGA_ROOT_PORT_BUS + 1,0x0,0x0},   //Video Bridge
    {VGA_ROOT_PORT_BUS + 2,0x0,0x0}    //Video Controller
};

extern BOOLEAN  VideoDeviceInUse;
VOID            *gPciProtocolNotifyReg;

#define PCIE_REG_ADDR(Bus,Device,Function,Offset) \
  (((Offset) & 0xfff) | (((Function) & 0x07) << 12) | (((Device) & 0x1f) << 15) | (((Bus) & 0xff) << 20))

VOID
PvcrPciWrite16(
  IN UINT32 CfgBase, 
  IN UINT8 bus, 
  IN UINT8 dev, 
  IN UINT8 func, 
  IN UINT16 reg, 
  IN UINT16 data) 
{
    UINT8 *regAddr;

    regAddr = (UINT8 *)(CfgBase | PCIE_REG_ADDR(bus, dev, func, reg));
    // cppcheck-suppress unreadVariable
    *(volatile UINT16 *)regAddr = data;
}

VOID
PvcrPciRead16(
  IN UINT32 CfgBase, 
  IN UINT8 bus, 
  IN UINT8 dev, 
  IN UINT8 func, 
  IN UINT16 reg, 
  IN OUT UINT16 *data
)
{
    UINT8 *regAddr;

    regAddr = (UINT8 *)(CfgBase | PCIE_REG_ADDR(bus, dev, func, reg));
    *data = *(volatile UINT16 *)regAddr;
}

/**
    Disables legacy VGA decode down to the video controller
 
 **/
VOID
DisableVgaDecode(
  VOID
) 
{
    UINT8     bus, dev, func, devPathIdx;
    UINT16    data16 = 0;
    UINT32    base;
    DEV_PATH  *DevPath;
    UINT8     DevPathEntries;

    DevPath = mVideoVidPath;
    base = (UINT32) PcdGet64(PcdPciExpressBaseAddress);
    bus = DevPath[0].Bus;
    // Check if bus need to be adjusted - 1P/2P case, assuming 0x10 Buses per Die for 2P
    // Read Base bus number for Die1
    PvcrPciRead16(base, 0, 0x18, 0, 0xA6, &data16);
    if((UINT8)(data16 & 0x00FF) != 0x10){
        bus = (UINT8)(data16 & 0x00FF) * (bus/0x10);
    }
    
    DevPathEntries = sizeof(mVideoVidPath)/sizeof(DEV_PATH);
    //
    // Iterate through all the bridges in the dev path and disable VGA decode.
    // it is Aspeed specific, where Vga device is behind PCIe-to-PCI bridge
    for (devPathIdx = 0; devPathIdx < DevPathEntries - 1; devPathIdx++) {
        
        dev = DevPath[devPathIdx].Dev;
        func = DevPath[devPathIdx].Func;
        //
        //Disable VGA decode
        //
        PvcrPciRead16(base, bus, dev, func, 0x3E, &data16);
        data16 &= 0xF7;
        PvcrPciWrite16(base, bus, dev, func, 0x3E, data16);
        
        bus++;
    } 
}

/**
    Notification function for Pci Protocol 
    This function stops the video protocol after PCI enumeration 

    @param Event - Event which caused this handler
    @param Context - Context passed during Event Handler registration

    @return VOID

**/

VOID
PciProtocolCallBack (
  IN EFI_EVENT        Event,
  IN VOID             *TextOutContext 
)
{
    EFI_STATUS        Status = EFI_SUCCESS;
    AMI_DXE_TEXT_OUT  *TextOut = NULL;
    
    DEBUG((EFI_D_INFO,"[Early Video] \n"__FUNCTION__));

    //Disable VGA Decode in pci bridge devices
    DisableVgaDecode();   
    
    VideoDeviceInUse = TRUE;   
    
    // Kill the Event
    pBS->CloseEvent(Event);
}

/**
    Install the Callback to Reset the Video Controller Programming done in the PEI Phase. 

    @param   None 

    @retval  EFI_NOT_FOUND
**/
EFI_STATUS 
AmiVideoControllerReset(
    VOID
)
{
    EFI_STATUS  Status = EFI_NOT_FOUND;
    EFI_EVENT   PciProtocolEvent;

    // Install the Callback function to Reset the Video Controller programming. 
    // Porting Required.
    //
    DEBUG((EFI_D_INFO,"[Platform Early Video] Platform Controller Reset"));
    //Create notification for PciEnumerationComplete
    
    Status = pBS->CreateEvent (EVT_NOTIFY_SIGNAL, 
                                    TPL_CALLBACK,
                                    PciProtocolCallBack, 
                                    NULL, 
                                    &PciProtocolEvent);
    ASSERT_EFI_ERROR(Status);
    
    if (!EFI_ERROR(Status)) {
        Status = pBS->RegisterProtocolNotify (
          &gBdsConnectDriversProtocolGuid,
          PciProtocolEvent, 
          &gPciProtocolNotifyReg
          );
    }
    return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
