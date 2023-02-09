Scope(\_SB)
{
   Method(IQI0, 1) {
      Store(ARG0, I2C0)
      Store(ARG0, AIC0)
   }
   Method(IQI1, 1) {
      Store(ARG0, I2C1)
      Store(ARG0, AIC1)
   }
   Method(IQI2, 1) {
      Store(ARG0, I2C2)
      Store(ARG0, AIC2)
   }
   Method(IQI3, 1) {
      Store(ARG0, I2C3)
      Store(ARG0, AIC3)
   }
   Method(IQI4, 1) {
      Store(ARG0, I2C4)
      Store(ARG0, AIC4)
   }
   Method(IQI5, 1) {
      Store(ARG0, I2C5)
      Store(ARG0, AIC5)
   }
  Device(I2CA) {
    Name(_HID,"HYGO0010")            // Hardware Device ID
    Name(_UID,0x0)
    Name(_CRS, ResourceTemplate() {
      IRQ(Edge,ActiveLow,Shared) {10}
      Memory32Fixed(ReadWrite, 0xFEDC2000, 0x1000)
    })

    Method(_STA, 0, NotSerialized) {
      //Return (0x0F) //Always return yes
      IQI0(10)
      Return (ICAE)
    }
  } // End Device I2CA
  
  Device(I2CB)
  {
    Name(_HID,"HYGO0010")            // Hardware Device ID
    Name(_UID,0x1)
    Name(_CRS, ResourceTemplate() {
      IRQ(Edge,ActiveLow,Shared) {11}
      Memory32Fixed(ReadWrite, 0xFEDC3000, 0x1000)
    })
    Method(_STA, 0, NotSerialized) {
      //Return (0x0F) //Always return yes
      IQI1(11)
      Return (ICBE)
    }
  } // End Device I2CB  
 
  Device(I2CC) {
    Name(_HID,"HYGO0010")            // Hardware Device ID
    Name(_UID,0x2)
    Name(_CRS, ResourceTemplate() {
      IRQ(Edge,ActiveLow,Shared) {4}
      Memory32Fixed(ReadWrite, 0xFEDC4000, 0x1000)
    })
    Method(_STA, 0, NotSerialized) {
      //Return (0x0F) //Always return yes
      IQI2(4)
      Return (ICCE)
    }
  } // End Device I2CC   
 
  Device(I2CD) {
    Name(_HID,"HYGO0010")            // Hardware Device ID
    Name(_UID,0x3)
    Name(_CRS, ResourceTemplate() {
      IRQ(Edge, ActiveHigh, Exclusive) {6}
      Memory32Fixed(ReadWrite, 0xFEDC5000, 0x1000)
    })
    Method(_STA, 0, NotSerialized) {
      //Return (0x0F) //Always return yes
      IQI3(6)
      Return (ICDE)
    }
  } // End Device I2CD 
 
  Device(I2CE) {
    Name(_HID,"HYGO0010")            // Hardware Device ID
    Name(_UID,0x4)
    Name(_CRS, ResourceTemplate() {
      IRQ(Edge,ActiveLow,Shared) {14}
      Memory32Fixed(ReadWrite, 0xFEDC6000, 0x1000)
    })
    Method(_STA, 0, NotSerialized) {
      //Return (0x0F) //Always return yes
      IQI4(14)
      Return (ICEE)
    }
  } // End Device I2CE 

  Device(I2CF) {
    Name(_HID,"HYGO0010")            // Hardware Device ID
    Name(_UID,0x5)
    Name(_CRS, ResourceTemplate() {
      IRQ(Edge,ActiveLow,Shared) {15}
      Memory32Fixed(ReadWrite, 0xFEDCB000, 0x1000)
    })
    Method(_STA, 0, NotSerialized) {
      //Return (0x0F) //Always return yes
      IQI5(15)
      Return (ICFE)
    }
  } // End Device I2CF 

}// end SB scope