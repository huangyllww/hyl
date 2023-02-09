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
/** @file HygonSpiRomProtect.c
    This file contains code for HYGON SPI ROM Protect entrypoint

**/

//----------------------------------------------------------------------------
// Includes
// Statements that include other files
#include <PiSmm.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/DebugLib.h>

#include <Token.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <AmiSmm.h>
#include <Protocol\SmmCpu.h>

#include <Protocol\SmmBase2.h>

#if CPUIOTRAP_FLAG
#include <AmiCpuPkg/CPU/Include/Protocol/CpuIoTrapProtocol.h> 
#endif

#define SMM_ENTRY_CODE_OFFS 0x400
//----------------------------------------------------------------------------
// Function Externs
VOID HygonSpiRomProtectWriteEnable (VOID);
VOID HygonSpiRomProtectWriteDisable (VOID);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Local Variables
#if CPUIOTRAP_FLAG
EFI_EVENT  gEvtCpuIoTrap;
EFI_EVENT  gRegCpuIoTrap;
#endif
volatile UINT8  *gSPIBASE = (UINT8*)(UINTN)SPI_BASE_ADDRESS;

#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x0001000A)
EFI_SMM_BASE2_PROTOCOL			*pSmmBase2;
EFI_SMM_SYSTEM_TABLE2                   *pSmst2;
#endif



#if CPUIOTRAP_FLAG
/**
    CpuIoTrapCallback1 service for if IO 0xCF9 Trap.

        
    @param Event 
    @param Context 
         
    @retval VOID

**/
VOID CpuIoTrapCallback1(
    IN EFI_HANDLE                               DispatchHandle,
    IN EFI_CPU_IO_TRAP_CONTEXT    *DispatchContext
    )
{

//To get correct trap value in OS (SMI may signal with AP.)
	    EFI_STATUS          Status = EFI_SUCCESS;
	static    UINT32        BspSmmBase;
	    UINT32              ApSmmBase;
	    UINT8               NumberOfCpus;
	    UINT32              SmmSaveStateAddr;
	    UINT8               TrapValue=0;
	    UINT16               TrapPort;
	    HYGON_64_SAVE_STATE   *SmmSave;
	    UINT8               CpuNum;

	    BspSmmBase = (UINT32)ReadMsr(HYGON_MSR_SMM_BASE);

	    NumberOfCpus = (UINT8)pSmst2->NumberOfCpus;
	    NumberOfCpus &= 0x0F;
	    for(CpuNum = 0; CpuNum < NumberOfCpus; CpuNum++)
	    {
	           ApSmmBase = BspSmmBase + (CpuNum * SMM_ENTRY_CODE_OFFS);
	           SmmSaveStateAddr = ApSmmBase + 0xFE00;
	           SmmSave = (HYGON_64_SAVE_STATE *)SmmSaveStateAddr;
	           TrapPort = (SmmSave->IOTrap >> 16);
	           //if(LocalSmiStatus) return ApSmmBase;
	           if (TrapPort == 0xCF9) break;  
	    }    

 
   TrapValue = (UINT8)(SmmSave->RAX);
   if (((TrapValue == 0x06) | (TrapValue == 0x04) | (TrapValue == 0x02)) && (TrapPort == 0xCF9)){
     HygonSpiRomProtectWriteEnable ();
     IoWrite8(0xCF9,0x6);
     EFI_DEADLOOP();   
   }
     IoWrite8(TrapPort,TrapValue);

}  // end CpuIoTrapCallback1


/**
    CpuIoTrapCallback2 service for if IO 0x92 Trap.

        
    @param Event 
    @param Context 
         
    @retval VOID

**/
VOID CpuIoTrapCallback2(
    IN EFI_HANDLE                               DispatchHandle,
    IN EFI_CPU_IO_TRAP_CONTEXT    *DispatchContext
    )
{


//To get correct trap value in OS (SMI may signal with AP)
	    EFI_STATUS          Status = EFI_SUCCESS;
	    static    UINT32        BspSmmBase;
	    UINT32              ApSmmBase;
	    UINT8               NumberOfCpus;
	    UINT32              SmmSaveStateAddr;
	    UINT8               TrapValue=0;
	    UINT16               TrapPort;
	    HYGON_64_SAVE_STATE   *SmmSave;
	    UINT8               CpuNum;

	    BspSmmBase = (UINT32)ReadMsr(HYGON_MSR_SMM_BASE);

	    NumberOfCpus = (UINT8)pSmst2->NumberOfCpus;
	    NumberOfCpus &= 0x0F;
	    for(CpuNum = 0; CpuNum < NumberOfCpus; CpuNum++)
	    {
	           ApSmmBase = BspSmmBase + (CpuNum * SMM_ENTRY_CODE_OFFS);
	           SmmSaveStateAddr = ApSmmBase + 0xFE00;
	           SmmSave = (HYGON_64_SAVE_STATE *)SmmSaveStateAddr;
	           TrapPort = (SmmSave->IOTrap >> 16);
	           //if(LocalSmiStatus) return ApSmmBase;
	           if (TrapPort == 0x92) break;  
	    }    
   TrapValue = (UINT8)(SmmSave->RAX);
   if ((TrapValue == 0x01) && (TrapPort == 0x92)){
    HygonSpiRomProtectWriteEnable ();
    IoWrite8(0xCF9,6);
    EFI_DEADLOOP();
   }
   IoWrite8(TrapPort,TrapValue);

}    // end CpuIoTrapCallback2

/**
    CpuIoTrapCallback3 service for if IO 0x64 Trap.

        
    @param Event 
    @param Context 
         
    @retval VOID

**/
VOID CpuIoTrapCallback3(
    IN EFI_HANDLE                               DispatchHandle,
    IN EFI_CPU_IO_TRAP_CONTEXT    *DispatchContext
    )
{

//To get correct trap value in OS (SMI may signal with AP.)
	    EFI_STATUS          Status = EFI_SUCCESS;
	    static    UINT32        BspSmmBase;
	    UINT32              ApSmmBase;
	    UINT8               NumberOfCpus;
	    UINT32              SmmSaveStateAddr;
	    UINT8               TrapValue=0;
	    UINT16               TrapPort;
	    HYGON_64_SAVE_STATE   *SmmSave;
	    UINT8               CpuNum;

	    BspSmmBase = (UINT32)ReadMsr(HYGON_MSR_SMM_BASE);

	    NumberOfCpus = (UINT8)pSmst2->NumberOfCpus;
	    NumberOfCpus &= 0x0F;
	    for(CpuNum = 0; CpuNum < NumberOfCpus; CpuNum++)
	    {
	           ApSmmBase = BspSmmBase + (CpuNum * SMM_ENTRY_CODE_OFFS);
	           SmmSaveStateAddr = ApSmmBase + 0xFE00;
	           SmmSave = (HYGON_64_SAVE_STATE *)SmmSaveStateAddr;
	           TrapPort = (SmmSave->IOTrap >> 16);
	           //if(LocalSmiStatus) return ApSmmBase;
	           if (TrapPort == 0x64) break;  
	    }     
	    
   TrapValue = (UINT8)(SmmSave->RAX);
   if ((TrapValue == 0xFE) && (TrapPort == 0x64)){
     HygonSpiRomProtectWriteEnable ();
     IoWrite8(0xCF9,6);
     EFI_DEADLOOP();
   }
   IoWrite8(TrapPort,TrapValue);

}  // end CpuIoTrapCallback3




VOID
CpuIoTrapDispatchProtocolCallBack (
    IN EFI_EVENT        Event, 
    IN VOID             *Context
)
{
	    EFI_CPU_IO_TRAP_DISPATCH_PROTOCOL   *gHygonCpuIoTrapProtocol;
	    EFI_CPU_IO_TRAP_REGISTER_CONTEXT    CpuIoTrapContext;
	    EFI_STATUS                          Status;
	    EFI_HANDLE                          Handle;

	    
	    Status = pBS->LocateProtocol ( &gCpuIoTrapDispatchProtocolGuid,
	                                    NULL,
	                                   &gHygonCpuIoTrapProtocol);
	 
	    if (EFI_ERROR(Status)) return;


	        CpuIoTrapContext.Address = 0xcf9;
		CpuIoTrapContext.Length = 1;
	    	CpuIoTrapContext.Type = WriteCpuIoTrap;
	        Status = gHygonCpuIoTrapProtocol->Register ( gHygonCpuIoTrapProtocol,
	                                                   CpuIoTrapCallback1,
	                                                  &CpuIoTrapContext,
	                                                  &Handle );


	        CpuIoTrapContext.Address = 0x92;
		CpuIoTrapContext.Length = 1;
	    	CpuIoTrapContext.Type = WriteCpuIoTrap;
	        Status = gHygonCpuIoTrapProtocol->Register ( gHygonCpuIoTrapProtocol,
	                                                   CpuIoTrapCallback2,
	                                                  &CpuIoTrapContext,
	                                                  &Handle );

	        CpuIoTrapContext.Address = 0x64;
		CpuIoTrapContext.Length = 1;
	    	CpuIoTrapContext.Type = WriteCpuIoTrap;
	        Status = gHygonCpuIoTrapProtocol->Register (gHygonCpuIoTrapProtocol,
	                                                  CpuIoTrapCallback3,
	                                                  &CpuIoTrapContext,
	                                                  &Handle );


	    pBS->CloseEvent(Event);
}

#endif


/**
    This function initializes Child dispatcher in SMM mode

        
    @param ImageHandle Image handle
    @param SystemTable pointer to system table
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
EFI_STATUS 
InSmmFunction(
    IN EFI_HANDLE          ImageHandle,
    IN EFI_SYSTEM_TABLE    *SystemTable
)
{
	 EFI_STATUS  Status = EFI_SUCCESS;
	 
	     Status = InitAmiSmmLib( ImageHandle, SystemTable );
	     if (EFI_ERROR(Status)) return Status;
#if  CPUIOTRAP_FLAG
	     Status = RegisterProtocolCallback ( &gCpuIoTrapDispatchProtocolGuid,
	                                         CpuIoTrapDispatchProtocolCallBack,
	                                         NULL,
	                                         &gEvtCpuIoTrap,
	                                         &gRegCpuIoTrap );
	     CpuIoTrapDispatchProtocolCallBack (gEvtCpuIoTrap, NULL);
#endif	


    return Status;
}

/**
    OFBD ME Firmware Update InSmm Function

        
    @param ImageHandle Image handle
    @param SystemTable pointer to system table
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
EFI_STATUS 
HygonSpiRomProtectEnteyPoint(
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{

    InitAmiLib (ImageHandle, SystemTable);
    return InitSmmHandler (ImageHandle, SystemTable, \
                                    InSmmFunction, NULL);
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
