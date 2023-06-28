#include "Token.h"
#include <Library/IoLib.h>
#include <Library/OemDebugAlwaysLib.h>

//----------------------------------------------------------------------
// Function Definitions
//----------------------------------------------------------------------

VOID SetDebugFlag(UINT8 DebugValue,UINT8 Mask8)
{
  UINT8                               Nmi;
  UINT8                               Save8;
  UINT8                               Data8;

  Save8 = IoRead8 (CMOS_INDEX_BANK);
  Nmi = IoRead8 (CMOS_BANK0_INDEX) & 0x80;
  IoWrite8 (CMOS_BANK0_INDEX, CMOS_DEBUG_MODE_OFFSET | Nmi);
  Data8 = IoRead8 (CMOS_BANK0_DATA);
  DebugValue = ((Data8 & ~Mask8 ) | (DebugValue & Mask8));
  if(Data8 != DebugValue){ 
      IoWrite8 (CMOS_BANK0_DATA, DebugValue);
  }
  IoWrite8 (CMOS_BANK0_INDEX, Save8 | Nmi); 
}


UINT8 GetDebugFlagEx()
{
    UINT8           Nmi;
    UINT8           save8;
    UINT8           Data8;

    save8 = IoRead8 (CMOS_INDEX_BANK);//get current index
    Nmi = IoRead8 (CMOS_BANK0_INDEX) & 0x80;
    IoWrite8 (CMOS_BANK0_INDEX, CMOS_DEBUG_MODE_OFFSET | Nmi);
    Data8 = IoRead8 (CMOS_BANK0_DATA);
    IoWrite8 (CMOS_BANK0_INDEX, save8 | Nmi);//restore index
    
    return Data8;
}

UINT8 GetDebugFlag(){
    UINT8           Nmi;
    UINT8           save8;
    UINT8           Data8;
    static BOOLEAN      DebugInProgress = FALSE;
    static UINT8 DebugMaskValue = DEBUG_MODE_VALUE_UPDATE_FLAG;
    
    Data8 = DebugMaskValue;
    
    if((Data8 & DEBUG_MODE_VALUE_UPDATE_FLAG)==0) 
        return Data8;
    
    if (DebugInProgress) {
        return Data8;
    }
    
    DebugInProgress = TRUE;
    
    save8 = IoRead8 (CMOS_INDEX_BANK);//get current index
    Nmi = IoRead8 (CMOS_BANK0_INDEX) & 0x80;
    IoWrite8 (CMOS_BANK0_INDEX, CMOS_DEBUG_MODE_OFFSET | Nmi);
    Data8 = IoRead8 (CMOS_BANK0_DATA);
    Data8 &= ~DEBUG_MODE_VALUE_UPDATE_FLAG;
    IoWrite8 (CMOS_BANK0_INDEX, save8 | Nmi);//restore index
    DebugMaskValue = Data8;
    
    DebugInProgress = FALSE;
    return Data8;
}

BOOLEAN
EFIAPI
DebugPrintEnabledEx (
  UINT8 MsgType
  )
{
  
  if ((GetDebugFlag() & (DEBUG_MODE_PROPERTY_DEBUG_PRINT_ENABLED)) == (DEBUG_MODE_PROPERTY_DEBUG_PRINT_ENABLED)) {
    return TRUE;
  }
  if ((GetDebugFlag() & RTS_DEBUG_ENABLED) == RTS_DEBUG_ENABLED) {  //Mizl-20220222-SGPZT#22027 Add Debug Level Lib Support->
    if (MsgType == DebugMsg) {
      return FALSE;
    } else if (MsgType == RcMsg) {
      return TRUE;
    }
  }
  if ((GetDebugFlag() & PEI_DEBUG_ENABLED) == PEI_DEBUG_ENABLED) {
    if ((GetDebugFlag() & DXE_PHASE_FLAG) == DXE_PHASE_FLAG){
      return FALSE;
    } else {
      return TRUE;
    }
  }
  if ((GetDebugFlag() & DXE_DEBUG_ENABLED) == DXE_DEBUG_ENABLED) {
    if ((GetDebugFlag() & DXE_PHASE_FLAG) != DXE_PHASE_FLAG){
          return FALSE;
        } else {
          return TRUE;
    }
  }
  return FALSE;  
}
