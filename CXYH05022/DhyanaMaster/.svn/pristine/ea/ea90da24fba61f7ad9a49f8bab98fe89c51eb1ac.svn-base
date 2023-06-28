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
 
/** @file NbDxe.c
    This file contains code for Template Northbridge initialization
    in the DXE stage

**/

// Module specific Includes

#include <Uefi.h>
#include <Library/UefiLib.h>
#include "token.h"
#include <DXE.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <PciNumaMapping.h>
#include "AmiCspLib.h"
#include "NbDxeBoard.h"
#include "Nb.h"
#include "NbS3Save.h"
#include <AmiAcpiS3.h>
#include "Ppi/Nbppi.h"
#include "Library/NbPolicy.h"
#include "Guid/AmdMemoryInfoHob.h"
#include <Protocol/AmiBoardInfo2.h>
#include <Protocol/MpService.h>
#include <Protocol/FabricTopologyServices.h>
#include <Protocol/FabricResourceManagerServicesProtocol.h>
#include <Protocol/FabricNumaServicesProtocol.h>
#if PI_SPECIFICATION_VERSION < 0x0001000A
#include <Protocol/BootScriptSave.h>
#else
#include <Protocol/S3SaveState.h>
#endif
#include <Library/PcdLib.h>

#define  PCI_ADDRESS(bus, dev, func, reg) \
            ((UINT64) ( (((UINT8)bus) << 16) + (((UINT8)dev) << 11) + \
            (((UINT8)func) << 8) + ((UINT8)reg) )) & 0x00000000ffffffff

// Build flag adjustments
#ifndef		SMM_SUPPORT
#define		SMM_SUPPORT			0
#endif


//Start S3 declaration
void SavePciRegisters(gPciRegistersSavelist *gPciRegistersSavelisttbl);
void SavePciIndexRegisters(gPciIndexRegistersSavelist *gPciIndexRegistersSavelisttbl);
VOID CallbackBootScript(IN EFI_EVENT Event, IN VOID *Context);
EFI_STATUS CreateNbMemoryInfo (
    IN EFI_SYSTEM_TABLE *SystemTable );
#if PI_SPECIFICATION_VERSION < 0x0001000A
EFI_BOOT_SCRIPT_SAVE_PROTOCOL       *gBootScriptSave;
#else
EFI_S3_SAVE_STATE_PROTOCOL          *gBootScriptSave;
#endif
EFI_STATUS SaveHygonMemInfoHOB();
EFI_STATUS SetApicId (
        IN  EFI_HANDLE              ImageHandle);
 
EFI_STATUS SetIommuResource (
        IN  EFI_HANDLE              ImageHandle);
EFI_STATUS SetPspBarOnAps (
        IN  EFI_HANDLE              ImageHandle);


//End S3 declaration

PCI_NUMA_NODE_MAPPING_PROTOCOL gPciNumaNodeMappingProtocol = {
        GetPciRbNumaNodeDomain,
        NULL
};

// External data definitions
EFI_GUID gEfiSetupGuid = SETUP_GUID;

static  EFI_GUID    gAmiGlobalVariableGuid = AMI_GLOBAL_VARIABLE_GUID;

VOID SetMemClrFlag(IN EFI_EVENT Event, IN VOID *Context);

VOID
NbDxeAfterPciEnum (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  );

/**
    Read the PCI Register

    @param Address :(UINT32) ( (((UINT32)bus) << 24) + (((UINT32)dev) << 19) \
        + (((UINT32)func) << 16) + ((UINT32)reg) )

    @retval UINT32 Return UINT32 value from the PCI Register.

**/
UINT32 NbReadPCI32(
    IN UINT32 Address
)
{
    UINT32  Value32;
    UINT32  AddrCf8 = ((UINT32)1 << 31) + (Address & 0xFFFF00) + (Address & 0xFC);

    IoWrite32(0xCF8, AddrCf8);
    Value32 = IoRead32(0xCFC);
    return Value32;
}

/**
    Read the PCI Register

    @param Address :(UINT32) ( (((UINT32)bus) << 16) + (((UINT32)dev) << 11) \
        + (((UINT32)func) << 8) + ((UINT32)reg) )
    @param Value to be written

    @retval VOID

**/
VOID NbWritePCI32(
    IN UINT32 Address,
    IN UINT32 Value32
)
{
    UINT32 AddrCf8 = ((UINT32)1 << 31) + (Address & 0xFFFF00) + (Address & 0xFC);

    IoWrite32(0xCF8, AddrCf8);
    IoWrite32(0xCFC, Value32);
}

/**
    Read the PCI Register

    @param Address :(UINT32) ( (((UINT32)bus) << 16) + (((UINT32)dev) << 11) \
        + (((UINT32)func) << 8) + ((UINT32)reg) )
    @param Value to be written

    @retval VOID

**/
VOID NbWritePCIIndex32(
    IN UINT32 Address,
    IN UINT32 Value32
)
{
    UINT32 AddrCf8 = ((UINT32)1 << 31) + ((Address >> 8) & 0xFF0000)
                   + ((Address >> 5) & 0xF800) + (Address & 0x7FF);

    IoWrite32(0xCF8, AddrCf8);
    IoWrite32(0xCFC, Value32);
}

/**
    Read the PCI Express Core Indirect Regs
    Index = PCI Reg 060h,Data  = PCI Reg 064h

    @param Bus : PCIE Bus number
    @param Dev : PCIE Device number
    @param Fun : PCIE Function number
    @param InDReg : index reg for PCIE Indirect regs.

    @retval UINT32 Return the PCIE indirect value.

**/

UINT32 NBROOTPCIEIND_RD(
    IN UINT8 Bus,
    IN UINT8 Dev,
    IN UINT8 Fun,
    IN UINT8 IndReg
)
{
    UINT32 Buffer32 = 0, temp = 0;

    NbWritePCI32(PCI_ADDRESS(Bus, Dev, Fun, 0x60), (UINT32)IndReg);
    Buffer32 = NbReadPCI32(PCI_ADDRESS(Bus, Dev, Fun, 0x64));

    NbWritePCI32(PCI_ADDRESS(Bus, Dev, Fun, 0x60), temp); //Clear Index.

    return Buffer32;
}

/**
    Read the PCI Express Core Indirect Regs
    Index = PCI Reg 0E0h,Data  = PCI Reg 0E4h

    @param Bus : PCIE Bus number
    @param Dev : PCIE Device number
    @param Fun : PCIE Function number
    @param InDReg : index reg for PCIE Indirect regs.

    @retval UINT32 Return the PCIE indirect value.

**/

UINT32 NBD4PCIEIND_RD(
    IN UINT8 Bus,
    IN UINT8 Dev,
    IN UINT8 Fun,
    IN UINT8 IndReg
)
{
    UINT32 Buffer32 = 0, temp = 0;

    NbWritePCI32(PCI_ADDRESS(Bus, Dev, Fun, 0xE0), (UINT32)IndReg);
    Buffer32 = NbReadPCI32(PCI_ADDRESS(Bus, Dev, Fun, 0xE4));

    NbWritePCI32(PCI_ADDRESS(Bus, Dev, Fun, 0xE0), temp); //Clear Index.

    return Buffer32;
}

#define NB_SMN_INDEX_EXT_3_BIOS  0x00C0
#define NB_SMN_INDEX_3_BIOS  0x00C4
#define NB_SMN_DATA_3_BIOS   0x00C8

VOID
NbSmnRead (
  IN       UINT32              DieNum,
  IN       UINT32              Address,
  IN	   UINT32	       *Value
  )
{
	UINT32 read_value=0;
	
	PciWrite32(NB_SMN_INDEX_EXT_3_BIOS, DieNum);
	PciWrite32(NB_SMN_INDEX_3_BIOS, Address);
	read_value=PciRead32(NB_SMN_DATA_3_BIOS);
	*Value=read_value;
	
	//clear in case other functions don't pay attention to this reg
	PciWrite32(NB_SMN_INDEX_EXT_3_BIOS, 0);
			              	 
}

VOID
NbSmnWrite (
  IN       UINT32              DieNum,
  IN       UINT32              Address,
  IN	   UINT32	       *Value
  )
{
	UINT32 write_value=*Value;
	
	PciWrite32(NB_SMN_INDEX_EXT_3_BIOS, DieNum);
    if(gBootScriptSave != NULL) {
        NB_TRACE ((TRACE_NB, "NbDxe: S3 save Die %d SMN 0x%08X Value 0x%X \n", DieNum, Address, write_value));
        BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(gBootScriptSave, EfiPciWidthUint32, NB_SMN_INDEX_EXT_3_BIOS, 1, &DieNum);
    }
    
	PciWrite32(NB_SMN_INDEX_3_BIOS, Address);
    if(gBootScriptSave != NULL) {
        BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(gBootScriptSave, EfiPciWidthUint32, NB_SMN_INDEX_3_BIOS, 1, &Address);
    }
    
	PciWrite32(NB_SMN_DATA_3_BIOS, write_value);
    if(gBootScriptSave != NULL) {
        BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(gBootScriptSave, EfiPciWidthUint32, NB_SMN_DATA_3_BIOS, 1, &write_value);
    }
	
	//clear in case other functions don't pay attention to this reg
	PciWrite32(NB_SMN_INDEX_EXT_3_BIOS, 0);
    if(gBootScriptSave != NULL) {
        write_value = 0;
        BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(gBootScriptSave, EfiPciWidthUint32, NB_SMN_INDEX_EXT_3_BIOS, 1, &write_value);
    }   
}

VOID
NbSmnRW (
  IN       UINT32              DieNum,
  IN       UINT32              Address,
  IN       UINT32              AndMask,
  IN       UINT32              OrMask
  )
{
  UINT32    RegValue;

  NbSmnRead (DieNum, Address, &RegValue);
  RegValue &= AndMask;
  RegValue |= OrMask;
  NbSmnWrite (DieNum, Address, &RegValue);
}


#if CORE_COMBINED_VERSION > 4600
/*
//
//  Name: gNbCspResTable
//
//
//      UEFI References:
//          Platform Initialization Specification (Ver. 1.0)
//              Pre-EFI Initialization Core Interface
//                  Services - DXE Services
//                      Global Coherency Domain Services:
//                          AddMemorySpace()
//                          EFI_GCD_MEMORY_TYPE (also see DXE.h)
//          UEFI Specification (Ver. 2.1)
//              Services - Boot Services
//                  Memory Allocation Services
//                      GetMemoryMap
//                          Memory Attribute Definitions (Also see EFI.h)
//
//
//      Example entry into gNbCspResTable for flash device:
//
//          FLASH_SIZE = 0x100000 => FLASH_DEVICE_BASE_ADDRESS = FFF00000
//                                => FWH_FEATURE_SPACE_BASE    = FFB00000
//
//          CSP_RES_ITEM gNbCspResTable[] =
//          {
//            { 0xFFF00000,                        // ResBase = Flash base
//	            0x100000,                          // ResLength = 1 MB region
//	            EfiGcdMemoryTypeMemoryMappedIo,    // ResType
//	            EFI_MEMORY_UC|EFI_MEMORY_RUNTIME } // Attributes
//          }
//
//      THIS TABLE MUST BE FILLED WITH RESOURCE REGIONS DECODED
//      BY THE NB FOR ITSELF. !!! Use  Attributes field == -1
//      to mark an IO Space Resource. You must provide valid
//      attributes for Memory Mapped IO regions.
//
*/
//============================================================================
//
//----------------------------------------------------------------------------
CSP_RES_ITEM gNbCspResTable[] = {

//==============================================================================//
//{UINT64 ResBase, UINTN ResLength, GCD_MEMORY_TYPE ResType, UINT64 Attributes}	//
//==============================================================================//

//16K of DMI Registers
//{ 0xFED00000, 0x00100000, EfiGcdMemoryTypeMemoryMappedIo, (EFI_MEMORY_UC)},

//CPU's LOCAL APIC dedicated  SPACE
{ APIC_BASE, 0x00100000, EfiGcdMemoryTypeMemoryMappedIo,(EFI_MEMORY_UC)},

//Here Goes Reserved IO Space
//IO Used for PCI Config access
{ 0xCF8, 0x00000008, EfiGcdIoTypeIo, -1},

//============================================================================

};
UINTN	gNbCspResCount = sizeof(gNbCspResTable) / sizeof(CSP_RES_ITEM);
#endif
// Produced Protocols

// GUID Definitions

// Global variable declarations
EFI_EVENT 			gEvent;
EFI_EVENT           gEvtBootScript;
EFI_EVENT           gEvtSetMemClrFlag;

// Portable Constants

// Function Prototypes
#if		SMM_SUPPORT
EFI_STATUS  NBSMM_Init (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable);
#endif

/**
    Hygon PciConfig read function....Reads Normal and Extended PCI space

        
    @param Address 
    @param ExtAccess use Extended PCI space access if TRUE

         
    @retval UINT32 The value read from the indicated Address.

    @note  
  Supports only 32 bit read only to be used for S3 Feature

**/

UINT32
HygonPciConfigRead(
    IN UINT32 Address,
    IN BOOLEAN ExtAccess )
{
	UINT32  PciAddress = ((UINT32)1 << 31) +
		(((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Bus      << 16 )	+
		(((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Device   << 11 )	+
		(((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Function << 8  )	+
		(((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Register       );

	PciAddress = PciAddress & 0xFFFFFFFC; //Dword Align

	if(ExtAccess) //If extended config Access requested then add the extended config address  bit (24 to 27)
		PciAddress = PciAddress | 0x1000000;

	IoWrite32(0xCF8, PciAddress);
	return IoRead32(0xCFC);			//Return the read value
}


/**
    Write a value into the PCI register indicated

        
    @param Address 
    @param Value 
    @param ExtAccess 

         
    @retval VOID

**/
VOID HygonPciConfigWrite(
    IN UINT32 Address,
    IN UINT32 Value,
    IN BOOLEAN ExtAccess)
{
	UINT32  PciAddress = ((UINT32)1 << 31) +
        (((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Bus      << 16 )  +
        (((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Device   << 11 )  +
        (((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Function << 8  )  +
        (((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Register       );
    PciAddress = PciAddress & 0xfffffffc;

    //If extended config Access requested then add the extended config address  bit (24 to 27)
    if (ExtAccess)
        PciAddress = PciAddress | 0x1000000;

    IoWrite32(0xcf8, PciAddress);  //Select the register to write
    IoWrite8(0xed, 00);
    IoWrite8(0xed, 00);
    IoWrite32(0xcfc, Value); //Write the Value
}


/**
    Reserve the resource that NB needs.

        
    @param ImgHandle 
    @param CntrHandle 

         
    @retval EFI_STATUS return EFI status

**/

EFI_STATUS ReserveNBResources(IN EFI_HANDLE	ImgHandle, IN EFI_HANDLE	CntrHandle)
{
	EFI_STATUS			Status = EFI_SUCCESS;

#if CORE_COMBINED_VERSION > 4600
	Status = LibAllocCspResource( gNbCspResTable, gNbCspResCount, ImgHandle, CntrHandle);
#endif
    return Status;
}


/**
    NB late init.

        
    @param ImageHandle 
    @param SystemTable 

         
    @retval EFI_STATUS return EFI status

**/

EFI_STATUS DoNBLateInit(
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable )
{
    EFI_STATUS  Status = EFI_SUCCESS;

    return Status;
}

EFI_STATUS GetPciRbNumaNodeDomain(
        IN PCI_NUMA_NODE_MAPPING_PROTOCOL   *This,
        IN UINTN                            BusRangeBase,
        OUT UINTN                           *SocketNumber,
        OUT UINTN                           *DieNumber,
        OUT UINT32                          *Domain)
{
    EFI_STATUS Status;
    UINTN Index;
    UINTN TotalNumberRootBridges;
    AMD_FABRIC_TOPOLOGY_SERVICES_PROTOCOL  *FabricTopology;
    
    
    if(This == NULL) return EFI_INVALID_PARAMETER;
    
    if(This->PciRbNodeMap == NULL) return EFI_INVALID_PARAMETER;
    
    // Locate FabricTopologyServicesProtocol
    Status = pBS->LocateProtocol (&gAmdFabricTopologyServicesProtocolGuid, NULL, (VOID **) &FabricTopology);
    if(EFI_ERROR (Status)) return Status;
    
    // Number of sockets & rb's
    Status = FabricTopology->GetSystemInfo (FabricTopology, NULL, NULL, &TotalNumberRootBridges);
    if(EFI_ERROR (Status)) return Status;
    
    // Find matching BusRangeBase
    for(Index=0; Index < TotalNumberRootBridges; Index++){
        if(This->PciRbNodeMap[Index].BusRangeBase == BusRangeBase){
            if(SocketNumber != NULL) *SocketNumber = This->PciRbNodeMap[Index].Socket;
            if(DieNumber != NULL)    *DieNumber = This->PciRbNodeMap[Index].Die;
            if(Domain != NULL)       *Domain = This->PciRbNodeMap[Index].Domain;
            
            return EFI_SUCCESS;
        }
    }
    
    return EFI_NOT_FOUND;
}

EFI_STATUS InitPciNumaNodeMappingProtocol()
{
    EFI_STATUS  Status;
    UINTN       NumberOfSockets;
    UINTN       SocketLoop;
    UINTN       NumberOfDies;
    UINTN       DieLoop;
    UINTN       NumberOfRootBridges;
    UINTN       RbLoop;
    UINTN       BusNumberBase;
    UINTN       TotalNumberDie;
    UINTN       TotalNumberRootBridges;
    UINTN       RbCounter = 0;
    UINT32      Domain;
    AMD_FABRIC_TOPOLOGY_SERVICES_PROTOCOL  *FabricTopology;
    FABRIC_NUMA_SERVICES_PROTOCOL          *FabricNuma;
    PCI_NUMA_MAPPING                       *NumaPciMap;
    EFI_HANDLE                             PciRbNumaMapHandle = NULL;
    
    // Locate FabricNumaServicesProtocol
    Status = pBS->LocateProtocol (&gAmdFabricNumaServicesProtocolGuid, NULL, (VOID **) &FabricNuma);
    if(EFI_ERROR (Status)) return Status;

    // Locate FabricTopologyServicesProtocol
    Status = pBS->LocateProtocol (&gAmdFabricTopologyServicesProtocolGuid, NULL, (VOID **) &FabricTopology);
    if(EFI_ERROR (Status)) return Status;
    
    // Number of sockets & rb's
    Status = FabricTopology->GetSystemInfo (FabricTopology, &NumberOfSockets, &TotalNumberDie, &TotalNumberRootBridges);
    if(EFI_ERROR (Status)) return Status;
    
    // Allocate pool for NumaPciMap
    Status = pBS->AllocatePool(EfiBootServicesData, sizeof(PCI_NUMA_MAPPING)*TotalNumberRootBridges, &NumaPciMap);
    if(EFI_ERROR (Status)) return Status;
    
    // Setup NumaPciMap
    for (SocketLoop = 0; SocketLoop < NumberOfSockets; SocketLoop++) { //Loop sockets
        Status = FabricTopology->GetProcessorInfo (FabricTopology, SocketLoop, &NumberOfDies, NULL, NULL, NULL);
        if(EFI_ERROR (Status)) return Status;
        
        for (DieLoop = 0; DieLoop < NumberOfDies; DieLoop++) { //Loop dies
            FabricTopology->GetDieInfo(FabricTopology, SocketLoop, DieLoop, &NumberOfRootBridges, NULL, NULL, NULL, NULL);
            if(EFI_ERROR (Status)) return Status;
            for(RbLoop = 0; RbLoop < NumberOfRootBridges; RbLoop++){
                FabricTopology->GetRootBridgeInfo(
                                FabricTopology,
                                SocketLoop,
                                DieLoop,
                                RbLoop,
                                NULL,
                                &BusNumberBase,
                                NULL
                                );
                
                
                Status = FabricNuma->DomainXlat (FabricNuma, SocketLoop, DieLoop, &Domain);
                if(EFI_ERROR (Status)) return Status;
                
                NumaPciMap[RbCounter].Socket = SocketLoop;
                NumaPciMap[RbCounter].Die = DieLoop;
                NumaPciMap[RbCounter].Domain = Domain;
                NumaPciMap[RbCounter].BusRangeBase = BusNumberBase; 
                
                RbCounter++;
            }
        }
    }
    
    gPciNumaNodeMappingProtocol.PciRbNodeMap = NumaPciMap;
    
    Status = pBS->InstallProtocolInterface(
            &PciRbNumaMapHandle,
            &gPciNumaNodeMappingProtocolGuid,
            EFI_NATIVE_INTERFACE,
            &gPciNumaNodeMappingProtocol
            );
    
    return EFI_SUCCESS;
}

/**
    This function is the entry point for this DXE. This function
    initializes the chipset NB

    @param ImageHandle Image handle
    @param SystemTable Pointer to the system table

    @retval EFI_STATUS Return Status based on errors that occurred while waiting for
        time to expire.

**/

EFI_STATUS
NbDxeInit (
    IN  EFI_HANDLE              ImageHandle,
    IN  EFI_SYSTEM_TABLE        *SystemTable
)
{
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL     *PciRootBridgeIo;
    EFI_STATUS                          Status = EFI_SUCCESS;
    //EFI_EVENT                           NBSMBIOSEvent = 0;
    //EFI_EVENT                           NBSSIDEvent = 0;
    //VOID                                *NBSmbiosReg = NULL;
    //VOID                                *NBSsid = NULL;
    UINTN                               NumberOfCpus = 1;
    UINTN                               NumberOfEnabledCpus;
    NB_ACPI_S3_VARIABLE_SET             *NbAcpiS3VariableSet;
    EFI_PHYSICAL_ADDRESS                AcpiMemoryBase;
    EFI_MP_SERVICES_PROTOCOL            *MpServices;
    
    EFI_EVENT                   NbDxeAfterPciEnumEvent;
    VOID                        *Registration;

	InitAmiLib(ImageHandle, SystemTable);
    PROGRESS_CODE(DXE_NB_INIT);
    
    Status = InitPciNumaNodeMappingProtocol();
    ASSERT_EFI_ERROR(Status);
    
	if (PcdGetBool(PcdAmdAcpiS3Support) == TRUE)
	{
	    Status = pBS->LocateProtocol(&gEfiMpServiceProtocolGuid, NULL, &MpServices);
	    ASSERT_EFI_ERROR (Status);
	    if(!EFI_ERROR(Status))
	    {
	        Status = MpServices->GetNumberOfProcessors(MpServices, &NumberOfCpus, &NumberOfEnabledCpus);
	        ASSERT_EFI_ERROR (Status);
        
	        NbAcpiS3VariableSet = (NB_ACPI_S3_VARIABLE_SET *)0xFFFFFFFF;
	        Status = pBS->AllocatePages(
	                AllocateMaxAddress,
	                EfiACPIMemoryNVS,
	                EFI_SIZE_TO_PAGES(sizeof(NB_ACPI_S3_VARIABLE_SET)),
	                (EFI_PHYSICAL_ADDRESS *)&NbAcpiS3VariableSet);
	        ASSERT_EFI_ERROR (Status);
        
	        pBS->SetMem(NbAcpiS3VariableSet, sizeof(NB_ACPI_S3_VARIABLE_SET), 0);
        
	        Status = pBS->AllocatePages(
	                AllocateAnyPages,
	                EfiACPIMemoryNVS,
	                EFI_SIZE_TO_PAGES(S3_BASE_MEMORY_SIZE + S3_MEMORY_SIZE_PER_CPU * NumberOfCpus),
	                &AcpiMemoryBase
	                );
	        ASSERT_EFI_ERROR (Status);
        
	        NbAcpiS3VariableSet->AcpiReservedMemoryBase = (EFI_PHYSICAL_ADDRESS)AcpiMemoryBase;
	        NbAcpiS3VariableSet->AcpiReservedMemorySize = (UINT32)(S3_BASE_MEMORY_SIZE + S3_MEMORY_SIZE_PER_CPU * NumberOfCpus);
        
	        DEBUG((DEBUG_INFO, "AcpiReservedMemoryBase = %lX, AcpiReservedMemorySize=%X\n", NbAcpiS3VariableSet->AcpiReservedMemoryBase, NbAcpiS3VariableSet->AcpiReservedMemorySize));
        
	        Status = pRS->SetVariable(ACPI_GLOBAL_VARIABLE,
	                &gEfiAcpiVariableGuid,
	                EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
	                sizeof(UINT32),
	                &NbAcpiS3VariableSet);
	    }
	}

	Status = pBS->LocateProtocol (&gEfiPciRootBridgeIoProtocolGuid, NULL, &PciRootBridgeIo);
	ASSERT_EFI_ERROR(Status);
	
#if PI_SPECIFICATION_VERSION < 0x0001000A
	Status = pBS->LocateProtocol(&gEfiBootScriptSaveProtocolGuid, NULL, &gBootScriptSave);
#else
	Status = pBS->LocateProtocol(&gEfiS3SaveStateProtocolGuid, NULL, &gBootScriptSave);
#endif
	
	ASSERT_EFI_ERROR(Status);

//    ReserveNBResources(ImageHandle, SystemTable);
	ASSERT_EFI_ERROR(Status);

#if SMM_SUPPORT
#if defined(HygonCpuPkg_SUPPORT) && (HygonCpuPkg_SUPPORT)
// Skip installation of SmmAccess Protocol if HygonCpuPkg used
#else	
    PROGRESS_CODE(DXE_NB_SMM_INIT);
	Status = NBSMM_Init (ImageHandle, SystemTable);
	if (Status != EFI_SUCCESS) {
		TRACE ((TRACE_ALWAYS, "NBSMM_Init() = %r\n", Status));
		return Status;
	}
#endif	
#endif

    Status = CreateNbMemoryInfo (SystemTable);

	Status = CreateReadyToBootEvent(
		TPL_NOTIFY,
		CallbackBootScript,
		NULL,
		&gEvtBootScript
	);
    ASSERT_EFI_ERROR(Status);

    //Create ready to boot event for SetMemClrFlag
	Status = CreateReadyToBootEvent(
		TPL_NOTIFY,
		SetMemClrFlag,
		NULL,
		&gEvtSetMemClrFlag
	);
    ASSERT_EFI_ERROR(Status);

    Status = SaveHygonMemInfoHOB();
//    Status = SetApicId(ImageHandle);

    
    NbDxeAfterPciEnumEvent = EfiCreateProtocolNotifyEvent (
                     &gEfiPciEnumerationCompleteProtocolGuid,
                     TPL_NOTIFY,
                     NbDxeAfterPciEnum,
                     ImageHandle,
                     &Registration
                     );
     ASSERT (NbDxeAfterPciEnumEvent != NULL);

#if NMI_BUTTON_SUPPORT
     //Enable LPC_SMI_L/NMI_SYNC_FLOOD_L/AGPIO86 pin as NMI (NMI button)
     //IOHCMISCx00000128 (IOHC_PIN_CNTL) bit0 NMI_SYNCFLOOD_PIN_MODE
     NbSmnRW (0, 0x13B10128, 0xFFFFFFFE, 0x1);	
     //IOHCRASx0000015C (MISC_RAS_CONTROL)bit2 PIN_NMI_SyncFlood_En
     NbSmnRW (0, 0x13B2015C, 0xFFFFFFFB, 0x4);	
#endif
     
    return EFI_SUCCESS;
}

/**
    This function set the memory clear flag

        
    @param Event 
    @param Context 

         
    @retval VOID

**/

VOID SetMemClrFlag(IN EFI_EVENT Event, IN VOID *Context)
{
    SETUP_DATA      SetupData;
    UINTN           VariableSize = sizeof(SETUP_DATA);
    EFI_STATUS      Status;
    UINT32          Attributes;

    Status = pRS->GetVariable(L"Setup", &gEfiSetupGuid, &Attributes, &VariableSize, &SetupData);
//Maybe we should run this code
//    SetupData.MemClrFlag = 1;   // Set memory clear flag before boot
    pRS->SetVariable(L"Setup", &gEfiSetupGuid,
                     Attributes,
                     VariableSize, &SetupData);
    //Kill the Event
    pBS->CloseEvent(Event);
}


#pragma pack (push, 1)
typedef struct
{
    EFI_HOB_GUID_TYPE       EfiHobGuidType;
    AMD_MEMORY_INFO_HOB     MemInfoHob;
}AMI_HYGON_MEM_INFO_HOB;
#pragma pack (pop)

/**
    Saves HYGON_MEMORY_INFO_HOB into NVRAM variable AmimemInfo.

        
    @param VOID

         
    @retval EFI_SUCCESS HOB found and variable saved
    @retval EFI_NOT_FOUND HOB not found

**/

EFI_STATUS 
SaveHygonMemInfoHOB(VOID)
{
    AMI_HYGON_MEM_INFO_HOB        *tempHOB;
    AMD_MEMORY_INFO_HOB         *MemInfoHob;
    AMD_MEMORY_RANGE_DESCRIPTOR *MemRangDesc;
    AMI_MEM_INFO                AmimemInfo;
    EFI_STATUS                  Status;
    EFI_GUID                    HobListGuid = HOB_LIST_GUID;
    EFI_GUID                    MemInfoHobGuid = gAmdMemoryInfoHobGuid;
	AMI_MEM_INFO                AmimemInfo2;
    UINTN                       VarSize;
    
    tempHOB = (AMI_HYGON_MEM_INFO_HOB *)GetEfiConfigurationTable(pST, &HobListGuid);

    if (tempHOB == NULL)
        Status = EFI_UNSUPPORTED;
    else
        Status = FindNextHobByGuid(&MemInfoHobGuid, &tempHOB);

    if (!EFI_ERROR(Status))
    {
        MemInfoHob = &(tempHOB->MemInfoHob);
        MemRangDesc = &MemInfoHob->Ranges[0];  
  
        AmimemInfo.dLoMemSize = (UINT32)MemRangDesc[0].Size;
        AmimemInfo.dLoMemBase = (UINT32)MemRangDesc[0].Base;
        if (MemInfoHob->NumberOfDescriptor > 0){
            if (MemRangDesc[1].Attribute == AMD_MEMORY_ATTRIBUTE_AVAILABLE) {
                AmimemInfo.dHiMemSize = MemRangDesc[1].Size;
                AmimemInfo.dHiMemBase = MemRangDesc[1].Base;
            } else if (MemRangDesc[1].Attribute & AMD_MEMORY_ATTRIBUTE_UMA) {
                AmimemInfo.dHiMemSize = 0;
                AmimemInfo.UMASize = MemRangDesc[1].Size;
                AmimemInfo.UMABase = MemRangDesc[1].Base;
            }
            
            if(MemInfoHob->NumberOfDescriptor == 2) {
                AmimemInfo.UMASize = MemRangDesc[2].Size;
                AmimemInfo.UMABase = MemRangDesc[2].Base;
            }
        }

        VarSize = sizeof(AMI_MEM_INFO);
        Status = pRS->GetVariable (L"AMIMemInfo",
                &gAmiMemInfoGuid,
                NULL,
                &VarSize,
                &AmimemInfo2);
        if(EFI_ERROR(Status) || MemCmp(&AmimemInfo, &AmimemInfo2, sizeof(AMI_MEM_INFO))) {
            NB_TRACE((TRACE_NB, "SetVariable.AMIMemInfo Status = %r\n", Status));
            Status = pRS->SetVariable(
                        L"AMIMemInfo",
                        &gAmiMemInfoGuid,
                        EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                        sizeof(AMI_MEM_INFO),
                        &AmimemInfo
                        );
            ASSERT_EFI_ERROR(Status);
        }
        return EFI_SUCCESS;
    }
    return EFI_NOT_FOUND;
}

/**
    This function is to get the memory length information(TOM)
    from PEI by HOB. And save it to variable for S3 resume.

        
    @param SystemTable Pointer to the system table

    @retval EFI_STATUS Return Status

**/
EFI_STATUS CreateNbMemoryInfo (
    IN EFI_SYSTEM_TABLE *SystemTable )
{
    EFI_STATUS          Status;
    EFI_GUID            HobListGuid = HOB_LIST_GUID;
    CHAR16              NbMemoryLengthVar[] = NB_MEMORY_LENGTH_VARIABLE;
    NB_MEMORY_LENGTH    NbMemoryLength;
    NB_MEMORY_LENGTH    LastNbMemoryLength;
	VOID				*FirstHob;
	EFI_HOB_RESOURCE_DESCRIPTOR	*ResDescHob;
    UINTN               DataSize = sizeof(NB_MEMORY_LENGTH);


    NbMemoryLength.MemoryLengthBelow4G = 0;
    NbMemoryLength.MemoryLengthAbove4G = 0;

	FirstHob = GetEfiConfigurationTable(SystemTable, &HobListGuid);
	if (!FirstHob) ASSERT_EFI_ERROR(EFI_NOT_FOUND);
	ResDescHob = (EFI_HOB_RESOURCE_DESCRIPTOR*) FirstHob;

    while (!EFI_ERROR(Status = FindNextHobByType(
	                                        EFI_HOB_TYPE_RESOURCE_DESCRIPTOR,
	                                        &ResDescHob)))
    {
		if (ResDescHob->ResourceType == EFI_RESOURCE_SYSTEM_MEMORY)
		{
			if (ResDescHob->PhysicalStart >= 0x100000)
            {
                NbMemoryLength.MemoryLengthAbove4G += ResDescHob->ResourceLength;
                if (ResDescHob->PhysicalStart <= 0xffffffff)
				    NbMemoryLength.MemoryLengthBelow4G += ResDescHob->ResourceLength;
            }
		}
	}
    NbMemoryLength.MemoryLengthAbove4G -= NbMemoryLength.MemoryLengthBelow4G;
	Status = pRS->GetVariable( NbMemoryLengthVar, \
                        	   &gNbMemoryLengthGuid, \
                        	   NULL, \
                        	   &DataSize, \
                        	   &LastNbMemoryLength );

    if (EFI_ERROR(Status) || MemCmp(&NbMemoryLength, &LastNbMemoryLength, sizeof(NB_MEMORY_LENGTH)))
    {
        NB_TRACE((TRACE_NB, "SetVariable.NBMemoryLength Status = %r\n", Status));
        Status = pRS->SetVariable( NbMemoryLengthVar, &gNbMemoryLengthGuid, \
                                   EFI_VARIABLE_NON_VOLATILE | \
                                   EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                   sizeof(NbMemoryLength), &NbMemoryLength );
        ASSERT_EFI_ERROR(Status);
    }
    return EFI_SUCCESS;
}

VOID
NbDxeAfterPciEnum (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS  Status;
  VOID        *Interface;
  
  //
  // Try to locate it because EfiCreateProtocolNotifyEvent will trigger it once when registration.
  // Just return if it is not found.
  //
  Status = pBS->LocateProtocol (
                  &gEfiPciEnumerationCompleteProtocolGuid,
                  NULL,
                  &Interface
                  );
  if (EFI_ERROR (Status)) {
    return ;
  }

  //Make sure the hook ONLY called one time.
  if (Event != NULL) {
    pBS->CloseEvent (Event);
  }
         
  if(!EFI_ERROR(Status))
  {	  
	  SetApicId(Context);
	  SetIommuResource(Context);
	  SetPspBarOnAps(Context);
  }  
}


EFI_STATUS SetApicId (
        IN  EFI_HANDLE              ImageHandle)
{
    EFI_STATUS Status;
    AMI_BOARD_INFO2_PROTOCOL    *AmiBoardInfo2Protocol = NULL;
    AMD_FABRIC_TOPOLOGY_SERVICES_PROTOCOL *FabricTopology;
    FABRIC_RESOURCE_MANAGER_PROTOCOL *FabricResourceManager;
    UINT64 Address;
    UINT32 IoApicId, RegIoApicId;
    UINT32 i = 0, ApicCount = 0,RbNo = 0;
    UINT32              Value;
    DXE_SERVICES        *dxe;
    UINTN               NumberOfInstalledProcessors;
    UINTN               NumberOfDie;
    UINT32              DieNumberOfPerSocket;
    FABRIC_TARGET               MmioTarget;
    FABRIC_MMIO_ATTRIBUTE       Attributes;
    BOOLEAN             ApicBasePresent = 0;
    UINT64              Length;

    NB_TRACE ((TRACE_NB, "\nSetApicId()\n"));

    if (PcdGetBool (PcdCfgIoApicMMIOAddressReservedEnable)) {
        ApicBasePresent = 1;
    }

    Status=LibGetDxeSvcTbl(&dxe);
    if(EFI_ERROR(Status)) {
        return Status;
    }

    Status = pBS->LocateProtocol(
            &gAmiBoardInfo2ProtocolGuid,
            NULL,
            &AmiBoardInfo2Protocol);
    if(EFI_ERROR(Status)) {
        NB_TRACE ((TRACE_NB, "Get AmiBoardInfo2Protocol fail (%r) \n", Status));
        return Status;
    }

    Status = pBS->LocateProtocol(
            &gAmdFabricTopologyServicesProtocolGuid,
            NULL,
            &FabricTopology);
    if(EFI_ERROR(Status)) {
        NB_TRACE ((TRACE_NB, "Get FabricTopology protocol fail (%r) \n", Status));
        return Status;
    }

    Status = pBS->LocateProtocol(
            &gAmdFabricResourceManagerServicesProtocolGuid,
            NULL,
            &FabricResourceManager);
    if(EFI_ERROR(Status)) {
        NB_TRACE ((TRACE_NB, "Get FabricResourceManager protocol fail (%r) \n", Status));
        return Status;
    }

    ApicCount = AmiBoardInfo2Protocol->ApicBrdData->IoApicCount;
    NB_TRACE ((TRACE_NB, "ApicBrdData->IoApicCount=0x%x\n", ApicCount));
    
    FabricTopology->GetSystemInfo (FabricTopology, &NumberOfInstalledProcessors, &NumberOfDie, NULL);
    DieNumberOfPerSocket = (UINT32)(NumberOfDie / NumberOfInstalledProcessors);

    //Init Apic base to some default value
    //-----------------        
    for(i = 0; i < ApicCount; i++) {
        // Find the NB - IOAPIC
        AmiBoardInfo2Protocol->ApicBrdData->IoApicInfo[i].ApicAddress = 0xDEADBEEF;
    } 

    for(RbNo=0; RbNo < (UINT32)NumberOfDie; RbNo++){
        if (ApicBasePresent) {
            //Read IOAPIC Base from registers
            NbSmnRead (RbNo, IOAPIC_BASE_ADDR_HI_REG, &Value);
            Address = (UINT64)(Value & 0xFFFF);
            Address = Address << 32;
            NbSmnRead (RbNo, IOAPIC_BASE_ADDR_LOW_REG, &Value);
            Address = Address + (UINT64)(Value & 0xFFFFFF00);

        } else {
            //Allocate MMIO for IO APIC
            Length = NBIO_IOAPIC_LENGTH;
            MmioTarget.TgtType = TARGET_DIE;
            MmioTarget.SocketNum = RbNo / DieNumberOfPerSocket;
            MmioTarget.DieNum = RbNo % DieNumberOfPerSocket;
            Attributes.ReadEnable = 1;
            Attributes.WriteEnable = 1;
            Attributes.NonPosted = 0;
            Attributes.MmioType = NON_PCI_DEVICE_BELOW_4G;
            Address = 0;
            Status = FabricResourceManager->FabricAllocateMmio (
                    FabricResourceManager,
                    &Address, 
                    &Length, 
                    ALIGN_64K, 
                    MmioTarget, 
                    &Attributes);
            //if(!Address)  continue;
            if(EFI_ERROR(Status)) {
                NB_TRACE ((TRACE_NB, "Fabric Allocate Mmio for Rb %d fail (%r) \n", RbNo, Status));
                continue;
            }
            
            Status = dxe->AddMemorySpace(EfiGcdMemoryTypeMemoryMappedIo, Address, (UINT64)NBIO_IOAPIC_LENGTH, EFI_MEMORY_UC|EFI_MEMORY_RUNTIME);
            Status = dxe->SetMemorySpaceAttributes(Address,(UINT64)NBIO_IOAPIC_LENGTH, EFI_MEMORY_UC|EFI_MEMORY_RUNTIME);
        }

        IoApicId = 0x81 + RbNo;
        NB_TRACE ((TRACE_NB, "DIE %d IOAPIC Address = 0x%lx , ID = 0x%x\n", RbNo, Address, IoApicId));
        RegIoApicId = IoApicId << 24;

        // Update Correct APICID to AMI_SDL_IO_APIC_INFO
        //-----------------        
        for(i = 0; i < ApicCount; i++) {
            // Find the NB - IOAPIC
            if (AmiBoardInfo2Protocol->ApicBrdData->IoApicInfo[i].ApicIdBuild == IoApicId) {   
                AmiBoardInfo2Protocol->ApicBrdData->IoApicInfo[i].ApicAddress = Address;
                break;
            }
        } 

        //program IOAPIC base address
        if (!ApicBasePresent) {
            NB_TRACE ((TRACE_NB, "Program APIC base registers \n"));
            Value = Address >> 32;
            NbSmnWrite (RbNo, IOAPIC_BASE_ADDR_HI_REG, &Value);
            Value = Address & 0xFFFFFF00;
            NbSmnWrite (RbNo, IOAPIC_BASE_ADDR_LOW_REG, &Value);
            Value |= 1;
            NbSmnWrite (RbNo, IOAPIC_BASE_ADDR_LOW_REG, &Value);
        }
        
        //program IOAPIC ID
        NbSmnWrite (RbNo, IOAPIC_ID_REG, &RegIoApicId);
    }

    return EFI_SUCCESS;
}



EFI_STATUS SetIommuResource (
        IN  EFI_HANDLE              ImageHandle)
{
    EFI_STATUS Status;
    UINT64 Address;
    UINT32 RbNo = 0;
    UINT32              Value;
    DXE_SERVICES        *dxe;
    UINTN               NumberOfInstalledProcessors;
    UINTN               NumberOfDie;
    UINT32              DieNumberOfPerSocket;
    FABRIC_TARGET               MmioTarget;
    FABRIC_MMIO_ATTRIBUTE       Attributes;
    AMD_FABRIC_TOPOLOGY_SERVICES_PROTOCOL *FabricTopology;
    FABRIC_RESOURCE_MANAGER_PROTOCOL *FabricResourceManager;
    UINT64                      Length;
    
    if (PcdGetBool (PcdCfgIommuMMIOAddressReservedEnable)) {
        NB_TRACE ((TRACE_NB, "We has reserved IOMMU MMIO space in PEIM ! \n"));
        return EFI_SUCCESS;
    }
    NB_TRACE ((TRACE_NB, "\nSetIommuResource()\n"));
    
    Status=LibGetDxeSvcTbl(&dxe);
    if(EFI_ERROR(Status)) return Status;
    
    Status = pBS->LocateProtocol(
            &gAmdFabricTopologyServicesProtocolGuid,
            NULL,
            &FabricTopology);
    if(EFI_ERROR(Status)) {
        NB_TRACE ((TRACE_NB, "Get FabricTopology protocol fail (%r) \n", Status));
        return Status;
    }
    
    Status = pBS->LocateProtocol(
            &gAmdFabricResourceManagerServicesProtocolGuid,
            NULL,
            &FabricResourceManager);
    if(EFI_ERROR(Status)) {
        NB_TRACE ((TRACE_NB, "Get FabricResourceManager protocol fail (%r) \n", Status));
        return Status;
    }
    FabricTopology->GetSystemInfo (FabricTopology, &NumberOfInstalledProcessors, &NumberOfDie, NULL);
    DieNumberOfPerSocket = (UINT32)(NumberOfDie / NumberOfInstalledProcessors);
             
    for(RbNo=0; RbNo < (UINT32)NumberOfDie; RbNo++){
        Length = NBIO_IOMMU_LENGTH;
        MmioTarget.TgtType = TARGET_DIE;
        MmioTarget.SocketNum = RbNo / DieNumberOfPerSocket;
        MmioTarget.DieNum = RbNo % DieNumberOfPerSocket;
        Attributes.ReadEnable = 1;
        Attributes.WriteEnable = 1;
        Attributes.NonPosted = 0;
        Attributes.MmioType = NON_PCI_DEVICE_BELOW_4G;
        Address = 0;
        Status = FabricResourceManager->FabricAllocateMmio (
                                            FabricResourceManager,
                                            &Address, 
                                            &Length, 
                                            ALIGN_512K, 
                                            MmioTarget, 
                                            &Attributes);
        
        //if(!Address)  continue;
        if(EFI_ERROR(Status)) {
            NB_TRACE ((TRACE_NB, "Fabric Allocate Mmio for Rb %d fail (%r) \n", RbNo, Status));
            continue;
        }      

        NB_TRACE ((TRACE_NB, "DIE %d IOMMU Address = %lx\n", RbNo, Address));
        
        Status = dxe->AddMemorySpace(EfiGcdMemoryTypeMemoryMappedIo, Address, (UINT64)NBIO_IOMMU_LENGTH, EFI_MEMORY_UC|EFI_MEMORY_RUNTIME);
        Status = dxe->SetMemorySpaceAttributes(Address,(UINT64)NBIO_IOMMU_LENGTH, EFI_MEMORY_UC|EFI_MEMORY_RUNTIME);
        
        //program base address
        Value = Address >> 32;
        NbSmnWrite (RbNo, IOMMU_CAP_BASE_HI_ADDRESS, &Value);
        Value = Address & 0xFFF80000;   //BIT[31:19]
        NbSmnWrite (RbNo, IOMMU_CAP_BASE_LO_ADDRESS, &Value);
    }
    
    return EFI_SUCCESS;        
}


VOID
SetPspAddrMsrTask ()
{
	UINT32                      RegEax = 0;
	UINT32                      RegEbx = 0;
	UINT32                      RegEcx = 0;
	UINT32                      RegEdx = 0;
	UINT32						NodeNum = 0;
	UINT64						PspAddr = 0;
	UINT32						temp = 0;
	UINT32                      NodesPerProcessor;
	UINT32                      SocketID;
	UINT32                      DieID;
	
	//Determine node #
	CPULib_CpuID(0x8000001E, &RegEax, &RegEbx, &RegEcx, &RegEdx);
	
	///
	/// 0x8000001E.Ecx: bit 8~10: NodesPerProcessor, bit2: socket id, bit0~1: die id
	/// NodeNum = NodesPerProcessor * SocketID + DieID
	///
	NodeNum           = RegEcx & 0x7FF;
	NodesPerProcessor = ((NodeNum & 0x700) >> 8) + 1;
	SocketID          = (NodeNum & 0x4) >> 2;
	DieID             = NodeNum & 0x3;
	NodeNum           = (NodesPerProcessor * SocketID) + DieID;
	
	 //Read PSP BASE Address in NBMISC
    NbSmnRead (NodeNum, PSP_BASE_ADDR_HI_REG, &temp);
	PspAddr = temp;
	PspAddr = PspAddr << 32;
    NbSmnRead (NodeNum, PSP_BASE_ADDR_LOW_REG, &temp);
    temp = temp & 0xFFF00000;
    PspAddr = PspAddr | temp;
    
    //Write PSP_ADDR
	WriteMsr (HYGON_MSR_PSP_ADDR, PspAddr);
}


EFI_STATUS SetPspBarOnAps (
        IN  EFI_HANDLE              ImageHandle)
{
    EFI_STATUS Status;
    UINT64 Address;
    UINT32 i = 0, RbNo = 0;
    UINT32              Value;
    DXE_SERVICES                *dxe;
    EFI_MP_SERVICES_PROTOCOL    *MpServices;
    UINTN                   NumberOfInstalledProcessors;
    UINTN                   NumberOfDie;
    UINT32                  DieNumberOfPerSocket;
    FABRIC_TARGET               MmioTarget;
    FABRIC_MMIO_ATTRIBUTE       Attributes;
    AMD_FABRIC_TOPOLOGY_SERVICES_PROTOCOL *FabricTopology;
    FABRIC_RESOURCE_MANAGER_PROTOCOL *FabricResourceManager;
    UINT64                      Length;
    
    NB_TRACE ((TRACE_NB, "\nSetPspBarOnAps()\n"));
    
    Status=LibGetDxeSvcTbl(&dxe);
    if(EFI_ERROR(Status) || PcdGetBool(PcdPspBaseSetAtAgesaEnable)) return Status;
    
    Status = pBS->LocateProtocol(
            &gAmdFabricTopologyServicesProtocolGuid,
            NULL,
            &FabricTopology);
    if(EFI_ERROR(Status)) {
        NB_TRACE ((TRACE_NB, "Get FabricTopology protocol fail (%r) \n", Status));
        return Status;
    }
    
    Status = pBS->LocateProtocol(
            &gAmdFabricResourceManagerServicesProtocolGuid,
            NULL,
            &FabricResourceManager);
    if(EFI_ERROR(Status)) {
        NB_TRACE ((TRACE_NB, "Get FabricResourceManager protocol fail (%r) \n", Status));
        return Status;
    }
    
    FabricTopology->GetSystemInfo (FabricTopology, &NumberOfInstalledProcessors, &NumberOfDie, NULL);
    DieNumberOfPerSocket = (UINT32)(NumberOfDie / NumberOfInstalledProcessors);
            
    for(RbNo=1; RbNo < NumberOfDie; RbNo++){
		
        //Address = gRbPciMmio32PspBarBase[RbNo];
        Length = PSP_BASE_LENGTH;
        MmioTarget.TgtType = TARGET_DIE;
        MmioTarget.SocketNum = RbNo / DieNumberOfPerSocket;
        MmioTarget.DieNum = RbNo % DieNumberOfPerSocket;
        Attributes.ReadEnable = 1;
        Attributes.WriteEnable = 1;
        Attributes.NonPosted = 0;
        Attributes.MmioType = NON_PCI_DEVICE_BELOW_4G;
        Address = 0;
        Status = FabricResourceManager->FabricAllocateMmio (
                                            FabricResourceManager,
                                            &Address, 
                                            &Length, 
                                            ALIGN_1M, 
                                            MmioTarget, 
                                            &Attributes);
        
        //if(!Address)  continue;
        if(EFI_ERROR(Status)) {
            NB_TRACE ((TRACE_NB, "Fabric Allocate Mmio for Rb %d fail (%r) \n", RbNo, Status));
            continue;
        }
        
        NB_TRACE ((TRACE_NB, "DIE %d PSP BAR Address = %lx\n", RbNo, Address));
        
        Status = dxe->AddMemorySpace(EfiGcdMemoryTypeMemoryMappedIo, Address, (UINT64)PSP_BASE_LENGTH, EFI_MEMORY_UC|EFI_MEMORY_RUNTIME);
        Status = dxe->SetMemorySpaceAttributes(Address,(UINT64)PSP_BASE_LENGTH, EFI_MEMORY_UC|EFI_MEMORY_RUNTIME);
            
        //Set PSP BASE Address in NBMISC, and enable lock the MMIO
        Value = Address >> 32;
        NbSmnWrite (RbNo, PSP_BASE_ADDR_HI_REG, &Value);
        Value = (Address & 0xFFFFFFFF) | (BIT0 + BIT8);
        NbSmnWrite (RbNo, PSP_BASE_ADDR_LOW_REG, &Value);
    }
    	
	//Set MSR on all cores
	Status = pBS->LocateProtocol (
						&gEfiMpServiceProtocolGuid,
						NULL,
						&MpServices );
	if (Status != EFI_SUCCESS)
		return Status;

	// Execute on all running APs
	Status = MpServices->StartupAllAPs(
		MpServices,                              // EFI_MP_SERVICES_PROTOCOL*
		(EFI_AP_PROCEDURE)
		SetPspAddrMsrTask,         // EFI_AP_PROCEDURE
		TRUE,                                   // BOOLEAN SingleThread
		NULL,                                    // EFI_EVENT WaitEvent
		0,                                    // UINTN Timeout
		NULL,                 // VOID *ProcArguments
		NULL);                                   // UINTN *FailedCPUList 

    return EFI_SUCCESS;
        
}

/**
    This function saves the boot script

        
    @param Event 
    @param Context 

         
    @retval VOID

**/

VOID CallbackBootScript(IN EFI_EVENT Event, IN VOID *Context)
{
	static BOOLEAN BootScriptSaved = FALSE;

    if (BootScriptSaved) return;


	BootScriptSaved = TRUE;

}

/**
    Save the gPciRegisterSaveListtbl pci devices save the reg
    for S3 resume wirte back.

        
    @param gPciRegistersSavelisttbl 

         
    @retval VOID

**/

VOID SavePciRegisters( IN gPciRegistersSavelist *gPciRegistersSavelisttbl)
{
    UINT8   j;
    UINT8   i;
    UINT8   Buffer8;
    UINT32  Buffer32;
    UINT16  Buffer16;
    UINT64  Address64;

    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRootBridgeIo;
    EFI_STATUS  Status;

    Status = pBS->LocateProtocol(
        &gEfiPciRootBridgeIoProtocolGuid,
        NULL,
        &PciRootBridgeIo
        );
    if (EFI_ERROR(Status)) return;

	j  =  0;
    while(gPciRegistersSavelisttbl[j].DevAddr != 0xFF)
    {
        i = 0;
    	while(gPciRegistersSavelisttbl[j].gPciRegisters[i].Address != 0xFF)
        {
            Address64 = (UINT64)(gPciRegistersSavelisttbl[j].DevAddr +
                                gPciRegistersSavelisttbl[j].gPciRegisters[i].Address);

            switch(gPciRegistersSavelisttbl[j].gPciRegisters[i].Width)
            {
                case    EfiBootScriptWidthUint8:
						PciRootBridgeIo->Pci.Read(
        	 			   	PciRootBridgeIo,
        	  			   	gPciRegistersSavelisttbl[j].gPciRegisters[i].Width,
        	  			   	Address64,
        	  			   	1,
        	 			   	&Buffer8
        					);
#if PI_SPECIFICATION_VERSION < 0x0001000A
						WRITE_PCI8_S3(gBootScriptSave, Address64, Buffer8);
#else
                        BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(
                        gBootScriptSave, EfiPciWidthUint8, Address64, 1, &Buffer8);
#endif

					break;
				case	EfiBootScriptWidthUint16:
						PciRootBridgeIo->Pci.Read(
        	 			   	PciRootBridgeIo,
        	  			   	gPciRegistersSavelisttbl[j].gPciRegisters[i].Width,
        	  			   	Address64,
        	  			   	1,
        	 			   	&Buffer16
        					);
#if PI_SPECIFICATION_VERSION < 0x0001000A
        				WRITE_PCI16_S3(gBootScriptSave, Address64, Buffer16);
#else
                        BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(
                        gBootScriptSave, EfiPciWidthUint16, Address64, 1, &Buffer16);
#endif

					break;
				case	EfiBootScriptWidthUint32:
						PciRootBridgeIo->Pci.Read(
        	 			   	PciRootBridgeIo,
        	  			   	gPciRegistersSavelisttbl[j].gPciRegisters[i].Width,
        	  			   	Address64,
        	  			   	1,
        	 			   	&Buffer32
        					);
#if PI_SPECIFICATION_VERSION < 0x0001000A
        				WRITE_PCI32_S3(gBootScriptSave, Address64, Buffer32);
#else
                        BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(
                        gBootScriptSave, EfiPciWidthUint32, Address64, 1, &Buffer32);
#endif

					break;
			}   // end of switch(gPciRegistersSavelisttbl[j].gPciRegisters[i].Width)
			i++;
    	}   // end of while(gPciRegistersSavelisttbl[j].gPciRegisters[i].Address != 0xFF)
		j++;
	}   // end of while(gPciRegistersSavelisttbl[j].DevAddr != 0xFF)
}
//////////////////////////////S3 PCI Context ends /////////////////////////////////////////

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
