//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
 
/** @file ApeiDxe.c
    APEI helper driver   

**/

#include "Token.h"
#include <AmiDxeLib.h>
#include "Protocol/AmdRasApeiProtocol.h"

AMD_RAS_APEI_PROTOCOL     *AmdRasApeiProtocol;

/**
    ApeiDxe driver entry point
               
    @param ImageHandle 
    @param SystemTable 

    @retval EFI_STATUS Return the EFI  Status
**/

EFI_STATUS
ApeiDxeEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
)
{
    EFI_STATUS  Status;
    
    InitAmiLib(ImageHandle, SystemTable);
    Status = pBS->LocateProtocol(
                    &gAmdRasApeiProtocolGuid,
                    NULL,
                    &AmdRasApeiProtocol
                    );
    DEBUG((-1, "ApeiDxe: Locate AmdRasApeiProtocol - %r\n", Status));
    
    Status = EFI_SUCCESS;
    return Status;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
