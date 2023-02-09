//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2009, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
//
//*************************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:  <UART1.ASL>
//
// Description: Define ACPI method or namespce For Super IO
//
//<AMI_FHDR_END>
//*************************************************************************
// UART1 //
// Category # :0x00
//Device(UAR1) {
//  Name(_HID, EISAID("PNP0501"))    //PnP Device ID 16550 Type
//  Name(_UID, 1)   //Generic ID for COMA

// Use the following if not using SI1P or only have 1 SIO   
//  Method(_HID, 0) {Return(^^SIO1.UHID(0))}    //PnP Device ID
    Name(_DDN, "COM1")
    Method(_STA, 0) {Return(^^SIO1.DSTA(0))}    //Get UART status
    Method(_DIS, 0) {^^SIO1.DCNT(0, 0)}         //Disable UART
    Method(_CRS, 0) {Return(^^SIO1.DCRS(0, 0))} //Get UART current resources
    Method(_SRS, 1) {^^SIO1.DSRS(Arg0, 0)}      //Set UART recources


//-----------------------------------------------------------------------
// UART1 Possible Resources
//-----------------------------------------------------------------------
//NOTE: _PRS MUST be the NAME not a METHOD object 
//to have GENERICSIO.C working right! 
//-----------------------------------------------------------------------
    Name(_PRS, ResourceTemplate() {

        StartDependentFn(0, 0) {
      IO(Decode16, 0x220, 0x220, 1, 8)    //#<Cancon-duanjun-20181225-com1 yellow mark +>
            IRQNoFlags() {4}              //<xuyj1-20210413 Mantis:134131 window os device manager is abnormal +>
            DMA(Compatibility, NotBusMaster, Transfer8) {}
        }
        EndDependentFn()
    })
//} // End Of UAR1
//-----------------------------------------------------------------------

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2008, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
