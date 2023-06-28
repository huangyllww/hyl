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

/** @file ProgramMmioRange.c
        This file contains helper function to program MMIO range registers!
**/
#include <PiDxe.h>
#include <Protocol/FabricResourceManagerServicesProtocol.h>
#include <Protocol/FabricTopologyServices.h>
#include <Library/ProgramMmioRange.h>
#include <Library/ApcbLib.h>
#include <Library/UefiLib.h>
#include "Nb.h"

VOID 
DoColdResetToAdjustTom(
    UINT32  RawNewBottomIo ) 
{ 
	UINT64  CurrentTom = 0;
    UINT64  AlignedNewBottomIO = 0;
    UINT32    TokenSize = sizeof(UINT8);
    EFI_PEI_HOB_POINTERS    HobList;
    EFI_BOOT_MODE           BootMode;
    
    CurrentTom = ReadMsr(HYGON_MSR_TOP_MEM);
    
    AlignedNewBottomIO = ((UINT64)RawNewBottomIo & 0xF0000000);
	 
	TRACE ((TRACE_ALWAYS, "\nCurrentBottomIO = %lx, RawNewBottomIo = %lx, AlignedNewBottomIO = %x\n", CurrentTom, RawNewBottomIo, AlignedNewBottomIO));
	        
    if(AlignedNewBottomIO < CurrentTom || (AlignedNewBottomIO > CurrentTom + MCT_MIN_RECLAIM_MEM_VAL)) {
        AlignedNewBottomIO = AlignedNewBottomIO >> 24;
    	AmdPspApcbSetCfgParameter(APCB_TOKEN_CONFIG_DF_BOTTOMIO, &TokenSize, &AlignedNewBottomIO);
    	AmdPspWriteBackApcbShadowCopy();
    	
        EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **)&HobList.Raw);
        BootMode = HobList.HandoffInformationTable->BootMode;    
        if (BootMode != BOOT_IN_RECOVERY_MODE) {
            TRACE ((TRACE_ALWAYS, "\nDoing Warm reset to adjust bottomIO!\n"));
            pRS->ResetSystem(EfiResetWarm, EFI_SUCCESS, 0, NULL);
        }
    }
}

EFI_STATUS 
AllocateMMIO32ForAllRB(
        IN PCI_HOST_BRG_DATA    *HostBrgData,
        FABRIC_RESOURCE_MANAGER_PROTOCOL *FabricResourceManager)  
{    
    EFI_STATUS Status = EFI_SUCCESS;
    UINT8      Index, Index2 = 0;
    UINT64      RbMmioActualBase = 0xffffffff;
    ASLR_QWORD_ASD              *res;
    FABRIC_TARGET               MmioTarget;
    FABRIC_MMIO_ATTRIBUTE       Attributes;
    UINT64                      Length;
    PCI_ROOT_BRG_DATA           *rbdata = NULL;
    DXE_SERVICES                *dxe;

    UINT64 RbHotplugMmioLength [MAX_ROOT_BRIDGE_COUNT] = {
            MmioHotPlugLength
    };

    NB_TRACE ((TRACE_NB, "==========Allocate MMIO32 start==============\n"));

    Status=LibGetDxeSvcTbl(&dxe);

    for (Index = 0; Index < HostBrgData->RootBridgeCount; Index++)
    {
        //print this RB res data        
        rbdata = HostBrgData->RootBridges[Index];
        if(rbdata->NotPresent == 1) {
            continue;
        }

        NB_TRACE ((TRACE_NB, "RBDATA #%x resource\n",Index));            

        for (Index2 = 0; Index2 < rbdata->ResCount; Index2++){
            res=rbdata->RbRes[Index2];
            if(res->Type==ASLRV_SPC_TYPE_MEM) {
                //Calculate total length
                Length = res->_LEN + (RbHotplugMmioLength[Index] << 25);
                if(!Length) {
                    //Not need resource
                    continue; 
                }

                if(res->_GRA==32){                                                                      
                    //NB_TRACE ((TRACE_NB,"Need MMIO32 MIN=0x%lX MAX=0x%lX LEN=0x%lX\n", res->_MIN, res->_MAX, res->_LEN));
                    //NB_TRACE ((TRACE_NB,"Reserve length 0x%lX for hot plug device \n", (RbHotplugMmioLength[Index] << 25)));
                    //Allocate MMIO32 resource for PCI device
                    
                    NB_TRACE ((TRACE_NB,"res->_GRA = %d, res->TFlags.MEM_FLAGS._MEM = %d \n", res->_GRA, res->TFlags.MEM_FLAGS._MEM));

                    MmioTarget.TgtType = TARGET_PCI_BUS;
                    MmioTarget.SocketNum = 0;
                    MmioTarget.DieNum = 0;
                    MmioTarget.PciBusNum = rbdata->RbSdlData->Bus;
                    Attributes.ReadEnable = 1;
                    Attributes.WriteEnable = 1;
                    Attributes.NonPosted = 0;
                    if(res->TFlags.MEM_FLAGS._MEM == ASLRV_MEM_CEPF)
                        Attributes.MmioType = P_MMIO_BELOW_4G;  // prefechable
                    else
                        Attributes.MmioType = MMIO_BELOW_4G;    // Non prefechable
                        
                    RbMmioActualBase  = 0;
                    Status = FabricResourceManager->FabricAllocateMmio (
                            FabricResourceManager,
                            &RbMmioActualBase, 
                            &Length, 
                            res->_MAX, 
                            MmioTarget, 
                            &Attributes);
                    if(EFI_ERROR(Status)) {
                        NB_TRACE ((TRACE_NB, "FabricAllocateMmio fail (%r) \n", Status));
                        continue;
                    }  
                    NB_TRACE ((TRACE_NB, "Root Bridge %d MMIO32 BaseAddress 0x%lX, length 0x%lX \n", Index, RbMmioActualBase, Length));                
                    res->_MIN = RbMmioActualBase;

                    if (rbdata->AcpiRbRes[raMmio32].Min == 0) {
                        //First allocate MMIO32 resource
                        rbdata->AcpiRbRes[raMmio32].Min = RbMmioActualBase;
                        rbdata->AcpiRbRes[raMmio32].Max = RbMmioActualBase + Length - 1;
                        rbdata->AcpiRbRes[raMmio32].Gra = res->_MAX;
                    } else {
                        //Second allocate MMIO32 resource
                        if (RbMmioActualBase > rbdata->AcpiRbRes[raMmio32].Min) {
                            rbdata->AcpiRbRes[raMmio32].Max = RbMmioActualBase + Length - 1;
                        } else {
                            rbdata->AcpiRbRes[raMmio32].Min = RbMmioActualBase;
                        }         
                        if (res->_MAX > rbdata->AcpiRbRes[raMmio32].Gra) {
                            rbdata->AcpiRbRes[raMmio32].Gra = res->_MAX;
                        }
                    }
                    rbdata->AcpiRbRes[raMmio32].Len=rbdata->AcpiRbRes[raMmio32].Max - rbdata->AcpiRbRes[raMmio32].Min + 1;
                    rbdata->AcpiRbRes[raMmio32].AddrUsed = rbdata->AcpiRbRes[raMmio32].Max+1;
                    rbdata->AcpiRbRes[raMmio32].AllocType=EfiGcdAllocateAddress;

                    Status = dxe->AddMemorySpace(EfiGcdMemoryTypeMemoryMappedIo, RbMmioActualBase, res->_LEN, EFI_MEMORY_UC);
                    Status = dxe->SetMemorySpaceAttributes(RbMmioActualBase, res->_LEN, EFI_MEMORY_UC);
                }
            }
        }
    }

    NB_TRACE ((TRACE_NB, "==========Allocate MMIO32 End==============\n"));
    return Status;
}

EFI_STATUS 
AllocateMMIO64ForAllRB(
        IN PCI_HOST_BRG_DATA    *HostBrgData,
        FABRIC_RESOURCE_MANAGER_PROTOCOL *FabricResourceManager)  
{    
    EFI_STATUS Status = EFI_SUCCESS;
    UINT8      Index, Index2 = 0;
    UINT64      RbMmioActualBase = 0xffffffff;
    ASLR_QWORD_ASD              *res;
    FABRIC_TARGET               MmioTarget;
    FABRIC_MMIO_ATTRIBUTE       Attributes;
    UINT64                      Length;
    PCI_ROOT_BRG_DATA           *rbdata = NULL;
    DXE_SERVICES                *dxe;
    
    UINT64 RbHotplugMmioLength [MAX_ROOT_BRIDGE_COUNT] = {
                MmioHotPlugLength
    };

    NB_TRACE ((TRACE_NB, "==========Allocate MMIO64 start==============\n"));

    Status=LibGetDxeSvcTbl(&dxe);

    for (Index = 0; Index < HostBrgData->RootBridgeCount; Index++)
    {
        //print this RB res data
        rbdata = HostBrgData->RootBridges[Index];
        if(rbdata->NotPresent == 1) {
            continue;
        }

        NB_TRACE ((TRACE_NB, "RBDATA #%x resource\n",Index));            

        for (Index2 = 0; Index2 < rbdata->ResCount; Index2++){
            res=rbdata->RbRes[Index2];
            if(res->Type==ASLRV_SPC_TYPE_MEM) {
                Length = res->_LEN + (RbHotplugMmioLength[Index] << 25);
                if(!Length) {
                     //Not need resource
                     continue; 
                }
                
                if(res->_GRA==64){                                                                      
                    NB_TRACE ((TRACE_NB,"res->_GRA = %d, res->TFlags.MEM_FLAGS._MEM = %d \n", res->_GRA, res->TFlags.MEM_FLAGS._MEM));
                    
                    //Allocate MMIO64 resource for PCI device
                    MmioTarget.TgtType = TARGET_PCI_BUS;
                    MmioTarget.SocketNum = 0;
                    MmioTarget.DieNum = 0;
                    MmioTarget.PciBusNum = rbdata->RbSdlData->Bus;
                    Attributes.ReadEnable = 1;
                    Attributes.WriteEnable = 1;
                    Attributes.NonPosted = 0;
                    //Attributes.MmioType = MMIO_ABOVE_4G;
                    if(res->TFlags.MEM_FLAGS._MEM == ASLRV_MEM_CEPF)
                        Attributes.MmioType = P_MMIO_ABOVE_4G;  // prefechable
                    else
                        Attributes.MmioType = MMIO_ABOVE_4G;    // Non prefechable
                    
                    RbMmioActualBase  = 0;
                    Status = FabricResourceManager->FabricAllocateMmio (
                            FabricResourceManager,
                            &RbMmioActualBase, 
                            &Length, 
                            res->_MAX, 
                            MmioTarget, 
                            &Attributes);
                    if(EFI_ERROR(Status)) {
                        NB_TRACE ((TRACE_NB, "FabricAllocateMmio fail (%r) \n", Status));
                        continue;
                    }  
                    NB_TRACE ((TRACE_NB, "Root Bridge %d MMIO64 BaseAddress 0x%lX, length 0x%lX \n", Index, RbMmioActualBase, Length));                
                    res->_MIN = RbMmioActualBase;

                    if (rbdata->AcpiRbRes[raMmio64].Min == 0) {
                        //First allocate MMIO64 resource
                        rbdata->AcpiRbRes[raMmio64].Min = RbMmioActualBase;
                        rbdata->AcpiRbRes[raMmio64].Max = RbMmioActualBase + Length - 1;
                        rbdata->AcpiRbRes[raMmio64].Gra = res->_MAX;
                    } else {
                        //Second allocate MMIO64 resource
                        if (RbMmioActualBase > rbdata->AcpiRbRes[raMmio64].Min) {
                            rbdata->AcpiRbRes[raMmio64].Max = RbMmioActualBase + Length - 1;
                        } else {
                            rbdata->AcpiRbRes[raMmio64].Min = RbMmioActualBase;
                        }         
                        if (res->_MAX > rbdata->AcpiRbRes[raMmio64].Gra) {
                            rbdata->AcpiRbRes[raMmio64].Gra = res->_MAX;
                        }
                    }
                    rbdata->AcpiRbRes[raMmio64].Len=rbdata->AcpiRbRes[raMmio64].Max - rbdata->AcpiRbRes[raMmio64].Min + 1;
                    rbdata->AcpiRbRes[raMmio64].AddrUsed = rbdata->AcpiRbRes[raMmio64].Max+1;
                    rbdata->AcpiRbRes[raMmio64].AllocType=EfiGcdAllocateAddress;

                    Status = dxe->AddMemorySpace(EfiGcdMemoryTypeMemoryMappedIo, RbMmioActualBase, res->_LEN, EFI_MEMORY_UC);
                    Status = dxe->SetMemorySpaceAttributes(RbMmioActualBase, res->_LEN, EFI_MEMORY_UC);
                    
                }
            }
        }
    }

    NB_TRACE ((TRACE_NB, "==========Allocate MMIO64 End==============\n"));
    return Status;
}

EFI_STATUS 
AllocateIOForAllRB(
        IN PCI_HOST_BRG_DATA    *HostBrgData,
        IN FABRIC_RESOURCE_MANAGER_PROTOCOL *FabricResourceManager)  
{    
    EFI_STATUS Status = EFI_SUCCESS;
    UINT8      Index, Index2 = 0;
    UINT32     RbIoBase = 0xffff;
    ASLR_QWORD_ASD              *res;
    FABRIC_TARGET               IoTarget;
    UINT32                      Length;
    PCI_ROOT_BRG_DATA           *rbdata = NULL;
    BOOLEAN                     FoundIoRes;

    UINT32 RbHotplugIOLength [MAX_ROOT_BRIDGE_COUNT] = {
            IOHotPlugLength
    };

    NB_TRACE ((TRACE_NB, "==========Allocate IO start==============\n"));

    for (Index = 0; Index < HostBrgData->RootBridgeCount; Index++)
    {
        //print this RB res data
        rbdata = HostBrgData->RootBridges[Index];
        if(rbdata->NotPresent == 1) {
            continue;
        }

        NB_TRACE ((TRACE_NB, "RBDATA #%x resource\n",Index));            

        FoundIoRes = FALSE;
        for (Index2 = 0; Index2 < rbdata->ResCount; Index2++){
            res=rbdata->RbRes[Index2];
            if(res->Type==ASLRV_SPC_TYPE_IO) {
                //Caluate total length
                Length = (UINT32)res->_LEN + (RbHotplugIOLength[Index] << 12);                    
                if(!Length) {
                    //Not need IO resource
                    if (Index == 0) {
                        //Die 0
                        rbdata->AcpiRbRes[raIo].Min=0;
                        rbdata->AcpiRbRes[raIo].Max=X86_LEGACY_IO_SIZE - 1;
                        rbdata->AcpiRbRes[raIo].Len=X86_LEGACY_IO_SIZE;
                        rbdata->AcpiRbRes[raIo].AddrUsed=X86_LEGACY_IO_SIZE;
                        rbdata->AcpiRbRes[raIo].AllocType=EfiGcdAllocateAddress;
                    } else {
                        //Other Die
                        rbdata->AcpiRbRes[raIo].Min=0;
                        rbdata->AcpiRbRes[raIo].Max=0;
                        rbdata->AcpiRbRes[raIo].Len=0;
                        rbdata->AcpiRbRes[raIo].AddrUsed=0;
                        rbdata->AcpiRbRes[raIo].AllocType=0;
                    }
                    continue;
                } 

                FoundIoRes = TRUE;
                
                //Allocate IO resource for PCI device
                IoTarget.TgtType = TARGET_PCI_BUS;
                IoTarget.SocketNum = 0;
                IoTarget.DieNum = 0;
                IoTarget.PciBusNum = rbdata->RbSdlData->Bus;
                RbIoBase  = 0;
                Status = FabricResourceManager->FabricAllocateIo (
                        FabricResourceManager,
                        &RbIoBase, 
                        &Length, 
                        IoTarget);
                if(EFI_ERROR(Status)) {
                    NB_TRACE ((TRACE_NB, "FabricAllocateIo fail (%r) \n", Status));
                    continue;
                }  
                NB_TRACE ((TRACE_NB, "Root Bridge %d IO BaseAddress 0x%X, length 0x%X \n", Index, RbIoBase, Length));

                if (Index == 0)
                    rbdata->AcpiRbRes[raIo].Min = 0;   //Die 0
                else
                    rbdata->AcpiRbRes[raIo].Min = RbIoBase;    //other Die

                rbdata->AcpiRbRes[raIo].Max = RbIoBase + Length - 1;
                rbdata->AcpiRbRes[raIo].Gra = res->_MAX;
                rbdata->AcpiRbRes[raIo].Len=rbdata->AcpiRbRes[raIo].Max - rbdata->AcpiRbRes[raIo].Min + 1;
                rbdata->AcpiRbRes[raIo].AddrUsed = rbdata->AcpiRbRes[raIo].Max+1;
                rbdata->AcpiRbRes[raIo].AllocType=EfiGcdAllocateAddress;

                res->_MIN = RbIoBase;
                res->_MAX = RbIoBase + Length -1;
            }
            
            if ( (Index2 == ((UINT8)rbdata->ResCount - 1)) &&
                 (FoundIoRes == FALSE) &&
                 (RbHotplugIOLength[Index] != 0) ) {
                //Current RB not need IO resource, but we must reserve for hot plug devices
                Length = RbHotplugIOLength[Index] << 12;
                IoTarget.TgtType = TARGET_PCI_BUS;
                IoTarget.SocketNum = 0;
                IoTarget.DieNum = 0;
                IoTarget.PciBusNum = rbdata->RbSdlData->Bus;
                RbIoBase  = 0;
                Status = FabricResourceManager->FabricAllocateIo (
                        FabricResourceManager,
                        &RbIoBase, 
                        &Length, 
                        IoTarget);
                if(EFI_ERROR(Status)) {
                    NB_TRACE ((TRACE_NB, "FabricAllocateIo fail (%r) \n", Status));
                    continue;
                }  
                NB_TRACE ((TRACE_NB, "Root Bridge %d Hot plug IO BaseAddress 0x%X, length 0x%X \n", Index, RbIoBase, Length));
                if (Index == 0)
                    rbdata->AcpiRbRes[raIo].Min = 0;   //Die 0
                else
                    rbdata->AcpiRbRes[raIo].Min = RbIoBase;    //other Die
                
                rbdata->AcpiRbRes[raIo].Max = RbIoBase + Length - 1;
                rbdata->AcpiRbRes[raIo].Gra = 0xFFF;
                rbdata->AcpiRbRes[raIo].Len=rbdata->AcpiRbRes[raIo].Max - rbdata->AcpiRbRes[raIo].Min + 1;
                rbdata->AcpiRbRes[raIo].AddrUsed = rbdata->AcpiRbRes[raIo].Max+1;
                rbdata->AcpiRbRes[raIo].AllocType=EfiGcdAllocateAddress;
            }
        }
    }

    NB_TRACE ((TRACE_NB, "==========Allocate IO End==============\n"));
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
