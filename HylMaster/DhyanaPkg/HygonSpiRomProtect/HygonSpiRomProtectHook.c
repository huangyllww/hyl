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
/** @file HygonSpiRomProtectHook.c
    This file contains code for HYGON SPI ROM Enable and Disable Protect hook.

**/
//**********************************************************************
//----------------------------------------------------------------------------
// Includes
// Statements that include other files
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <Token.h>
//----------------------------------------------------------------------------
// Function Externs

//----------------------------------------------------------------------------
// Local prototypes

//----------------------------------------------------------------------------
// Local Variables
extern volatile UINT8 *gSPIBASE;
extern VOID SbLib_SmiEnable(VOID);
extern VOID SbLib_SmiDisable(VOID);
extern BOOLEAN SbLib_GetSmiState(VOID);
BOOLEAN IsWriteEnabled = FALSE;

//----------------------------------------------------------------------------
// Function Definitions
/**
    Spi Rom Protect Enable Hook

    @param VOID
    @retval VOID

**/
VOID HygonSpiRomProtectWriteEnable (VOID)
{
    UINT8           Data, IsSmiEnabled;

    if ((*(gSPIBASE + 9) != 6) || (*(gSPIBASE + 2) & (BIT07 | BIT6))) return;
    IsWriteEnabled = TRUE;    
    IsSmiEnabled = SbLib_GetSmiState ();
    // HYGON Specification - To enable SpiHostAccesRomEn bit, the Global SMI 
    // MUST be enable state.   
    if (!IsSmiEnabled) SbLib_SmiEnable();
    
#if  defined LPC50_5C_ROM_PROTECT && LPC50_5C_ROM_PROTECT == 1
     WRITE_PCI32_SB(0x50,0x0);
     WRITE_PCI32_SB(0x54,0x0);
     WRITE_PCI32_SB(0x58,0x0);
     WRITE_PCI32_SB(0x5C,0x0);
#endif
     IoWrite8(0x80,0x55);              	 
    Data = *(volatile UINT8*)(gSPIBASE + 2) | (BIT07 | BIT6);    
    *(volatile UINT8*)(gSPIBASE + 2) = Data;
    *(gSPIBASE + 9) = 0;    
    if (!IsSmiEnabled) SbLib_SmiDisable();

}
/**
    Spi Rom Protect Disable Hook.

    @param VOID
    @retval VOID

**/
VOID HygonSpiRomProtectWriteDisable (VOID)
{
    UINT8           Data;
#if defined LPC50_5C_ROM_PROTECT && LPC50_5C_ROM_PROTECT == 1
    UINT8           IsSmiEnabled;
#endif    

   // TRACE((-1,"**HygonSpiRomProtectWriteDisable \n"));
    IoWrite8(0x80,0x66);

#if defined LPC50_5C_ROM_PROTECT && LPC50_5C_ROM_PROTECT == 1
    if (!IsWriteEnabled || !(*(gSPIBASE + 2) & (BIT07 | BIT6))) return;
    IsWriteEnabled = FALSE;  
    IsSmiEnabled = SbLib_GetSmiState ();
    if (!IsSmiEnabled) SbLib_SmiEnable();
    *(gSPIBASE + 9) = 6;    
    Data = *(gSPIBASE + 2) & ~(BIT07 | BIT6);    
    *(gSPIBASE + 2) = Data;
    //Data = *(volatile UINT8*)(gSPIBASE + 2);  
    #if (FLASH_SIZE  == 0x800000)    
    WRITE_PCI32_SB(0x50,0xFF80057F);
    #endif
    #if (FLASH_SIZE  == 0x1000000)  
    WRITE_PCI32_SB(0x50,0xFF0005FF);
    #endif 
    if (!IsSmiEnabled) SbLib_SmiDisable();
#else
    if (!IsWriteEnabled || !(*(gSPIBASE + 2) & (BIT07 | BIT6))) return;
    IsWriteEnabled = FALSE;    
    *(gSPIBASE + 9) = 6;    
    Data = *(gSPIBASE + 2) & ~(BIT07 | BIT6);    
    *(gSPIBASE + 2) = Data;
    //Data = *(volatile UINT8*)(gSPIBASE + 2);  

#endif   

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
