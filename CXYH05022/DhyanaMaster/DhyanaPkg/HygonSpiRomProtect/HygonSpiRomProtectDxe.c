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
/** @file HygonSpiRomProtectDxe.c
    This file contains code for HYGON SPI ROM Protect DXE entrypoint

**/

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

#define BDS_CONNECT_DRIVERS_PROTOCOL_GUID \
{0x3aa83745, 0x9454, 0x4f7a, 0xa7, 0xc0, 0x90, 0xdb, 0xd0, 0x2f, 0xab, 0x8e}
EFI_GUID  gBdsConnectDriversProtocolGuid = BDS_CONNECT_DRIVERS_PROTOCOL_GUID;



EFI_EVENT gEvtSpiProtect; 
EFI_EVENT gRegSpiProtect; 
volatile UINT8  *gSPIBASE = (UINT8*)(UINTN)SPI_BASE_ADDRESS;

EFI_S3_SAVE_STATE_PROTOCOL     	static	*gBootScriptSave = NULL;
//----------------------------------------------------------------------------
// Function Definitions


/**
    Prepare to for Boot ROM Protect event.

        
    @param VOID

         
    @retval VOID

**/
VOID
PrepareToBootRomProtectCallBack (
    IN EFI_EVENT        Event, 
    IN VOID             *Context
)
{
        EFI_STATUS    Status; 
        UINT32        Value;
    volatile UINT8  *SpiBase = (UINT8*)(UINTN)SPI_BASE_ADDRESS;
    *(SpiBase + 0x9) = 0x6;
    *(volatile UINT32*)SpiBase = *(volatile UINT32*)SpiBase & ~(BIT23 + BIT22);

    if(gBootScriptSave == NULL) {
	        Status = pBS->LocateProtocol(&gEfiS3SaveStateProtocolGuid,NULL, \
	                    &gBootScriptSave);
            if(!EFI_ERROR(Status)) {
                (UINT8)Value = *(SpiBase + 0x9);
            BOOT_SCRIPT_S3_MEM_WRITE_MACRO(gBootScriptSave, \
                EfiBootScriptWidthUint8, (UINT32)(SpiBase + 0x9), 1, &Value);
                Value = *(volatile UINT32*)SpiBase;
            BOOT_SCRIPT_S3_MEM_WRITE_MACRO(gBootScriptSave, \
                EfiBootScriptWidthUint32, (UINT32)SpiBase, 1, &Value);
#if    defined LPC50_5C_ROM_PROTECT && LPC50_5C_ROM_PROTECT == 1
        #if (FLASH_SIZE  == 0x800000)    
          WRITE_PCI32_SB_S3(gBootScriptSave,0x50,0xFF80057F);
        #endif
        #if (FLASH_SIZE  == 0x1000000)  
          WRITE_PCI32_SB_S3(gBootScriptSave,0x50,0xFF0005FF);
        #endif                    
#endif   
 
            }
    }


    pBS->CloseEvent(Event);
}


/**
    OFBD ME Firmware Update DXE Function

        
    @param VOID

         
    @retval VOID

**/
EFI_STATUS 
HygonSpiRomProtectEnteyDxePoint(
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
     EFI_STATUS  Status = EFI_SUCCESS;
    InitAmiLib (ImageHandle, SystemTable);

#if  (ROM_PROTECT_PHASE == 0)
    Status = RegisterProtocolCallback ( &gBdsConnectDriversProtocolGuid, \
                                        PrepareToBootRomProtectCallBack, \
                                        NULL, \
                                        &gEvtSpiProtect, \
                                        &gRegSpiProtect );
#endif

#if (ROM_PROTECT_PHASE == 1)
     Status = RegisterProtocolCallback ( &gBdsAllDriversConnectedProtocolGuid, \
                                        PrepareToBootRomProtectCallBack, \
                                        NULL, \
                                        &gEvtSpiProtect, \
                                        &gRegSpiProtect );
#endif

#if (ROM_PROTECT_PHASE == 2)
     Status = CreateReadyToBootEvent(TPL_NOTIFY, PrepareToBootRomProtectCallBack, NULL, &gEvtSpiProtect);

#endif
          
    return Status;
 
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
