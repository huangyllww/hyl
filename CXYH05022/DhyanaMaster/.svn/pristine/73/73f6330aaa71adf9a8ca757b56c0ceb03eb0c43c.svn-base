//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file PspPlatform.c
    PSP Platform driver

**/

#include <Protocol/PspPlatformProtocol.h>
#include "token.h"
#include "Library/DebugLib.h"
#include <Library/PcdLib.h>

EFI_HANDLE              mPspPlatformHandle = NULL;

#define RESUME_BSP_STACK_SIZE CAR_TOTAL_SIZE
#define RESUME_AP_STACK_SIZE  (1024 * 16)


PSP_PLATFORM_PROTOCOL mPspPlatformProtocol = {
  FALSE,                   //Support CPU Context Resume
  PSP_NOTIFY_SMM_SW_SMI,  //SW SMI Command for Build Context
  RESUME_BSP_STACK_SIZE,  //BSP Stack Size
  RESUME_AP_STACK_SIZE,   //AP Stack Size
  NULL                    //Address of PEI Info Structure
};

EFI_BOOT_SERVICES  *gBS;


/**
    PSP Platform driver entry point

               
    @param ImageHandle 
    @param SystemTable 

    @retval EFI_STATUS return EFI status

**/

EFI_STATUS
PspPlatformEntry (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
    UINT64      RsmInfoLocation = 0;
    EFI_STATUS  Status;
    UINT32		volatile *pRom;
    UINT64      SecLabel;

    gBS = SystemTable->BootServices;
	
    //Search the SEC_BSPStart label
    SecLabel = *(UINT64*)0x0FFFFFFF0;
    SecLabel = (SecLabel >> 24) & 0x0000FFF0;   //4 alignment
    SecLabel = SecLabel | 0xFFFF0000;

    pRom = (UINT32*)SecLabel;

    while(pRom < (UINT32*)0xFFFFFF00){
        if(*pRom == 0x54535251){
            RsmInfoLocation = (UINT32)(pRom + 1);
            break;
        }
        pRom++;
    }

    DEBUG((-1,"PSPplatform RsmInfoLocation = %x\n", RsmInfoLocation));
    ASSERT(RsmInfoLocation);
    if(PcdGetBool(PcdAmdAcpiS3Support) == TRUE)
    {
        mPspPlatformProtocol.CpuContextResumeEnable = TRUE;
    }
    mPspPlatformProtocol.RsmHandOffInfo = (RSM_HANDOFF_INFO *)RsmInfoLocation;

    //Install Protocol
    Status = gBS->InstallProtocolInterface (
                              &mPspPlatformHandle,
                              &gPspPlatformProtocolGuid,
                              EFI_NATIVE_INTERFACE,
                              &mPspPlatformProtocol
                                );
    return Status;
}


//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
