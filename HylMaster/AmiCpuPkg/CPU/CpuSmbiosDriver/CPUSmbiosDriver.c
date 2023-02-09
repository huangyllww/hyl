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

/** @file CPUSmbiosDriver.c
    Build and publish CPU smbios tables.( Type 4 and Type 7)

**/

#include "Efi.h"
#include "token.h"
#include <DXE.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Protocol/AmiSmbios.h>
#include <Protocol/PciIO.h>
#include <Protocol/PciRootBridgeIo.h>
#include "CpuSmbiosDriver.h"
#include "cpu.h"
#include "amicsplib.h"
#include "MemDmi.h"
#include "Nb.h"

#define CPU_SOCKET_DESIG_LEN 6
#define CACHE_SOCKET_DESIG_LEN 11
#define PROCESSOR_VENDOR_LEN 13

UINT8    CpuSocketDesig[CPU_SOCKET_DESIG_LEN] = CONVERT_TO_STRING(CPU_SOCKET_DESIGNATION);
UINT8    L1SocketDesig[CACHE_SOCKET_DESIG_LEN] = CONVERT_TO_STRING(L1_SOCKET_DESIGNATION); 
UINT8    L2SocketDesig[CACHE_SOCKET_DESIG_LEN] = CONVERT_TO_STRING(L2_SOCKET_DESIGNATION); 
UINT8    L3SocketDesig[CACHE_SOCKET_DESIG_LEN] = CONVERT_TO_STRING(L3_SOCKET_DESIGNATION); 

AMI_SMBIOS_PROTOCOL     *gSmbiosProtocol;
DMI_INFO                *AgesaBufferPtr;

//Create CPU SMBIOS Table
VOID CreateCpuSmbiosTable(VOID);

/**
    This function is the entry point for the driver and 
    publishes cpu smbios tables.

    @param ImageHandle Image handle for this driver
    @param SystemTable Pointer to the sytem table

    @retval EFI_STATUS return the EFI status

**/

EFI_STATUS
EFIAPI
CpuSmbiosDriverEntry (
    IN  EFI_HANDLE              ImageHandle,
    IN  EFI_SYSTEM_TABLE        *SystemTable
)
{
    EFI_STATUS  Status = EFI_SUCCESS;

    InitAmiLib(ImageHandle, SystemTable);
    //Locate SMBIOS Protocol, Assert if cant locate it.
	Status = pBS->LocateProtocol(&gAmiSmbiosProtocolGuid, NULL, &gSmbiosProtocol);
	ASSERT_EFI_ERROR(Status);
    //Create CPU SMBIOS Table
    CreateCpuSmbiosTable();    

    return Status;
}

VOID CreateCpuSmbiosTable()
{
    
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
//*************************************************************************`
