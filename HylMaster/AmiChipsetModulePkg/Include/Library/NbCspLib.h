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

/** @file NbCspLib.h
    This file contains definitions for NB Chipset Library

**/
#ifndef __NBLIB_H__
#define __NBLIB_H__

#include <efi.h>
#include <pei.h>
#include <Protocol\PciHostBridgeResourceAllocation.h>
#include <Protocol\PciRootBridgeIo.h>
#include <PciHostBridge.h>
#include <AmiDxeLib.h>
#include <PciBus.h>
#if PI_SPECIFICATION_VERSION < 0x0001000A || (BACKWARD_COMPATIBLE_MODE == 1)
#include <Protocol\BootScriptSave.h>
#else
#include <Protocol\S3SaveState.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

//
// Shadow Attribute
//
typedef enum {
    Disabled,   // Read/Write are both disabled
    ReadOnly,   // Read Only
    WriteOnly,  // Write Only
    ReadWrite   // Read/Write are both enabled
} SHADOW_ARRTIBUTE;

//
// BIOS Range to shadow
//
typedef enum {
    C0000_16K,  // 0C0000-0C3FFF
    C4000_16K,  // 0C4000-0C7FFF
    C8000_16K,  // 0C8000-0CBFFF
    CC000_16K,  // 0CC000-0CFFFF
    D0000_16K,  // 0D0000-0D3FFF
    D4000_16K,  // 0D4000-0D7FFF
    D8000_16K,  // 0D8000-0DBFFF
    DC000_16K,  // 0DC000-0DFFFF
    E0000_16K,  // 0E0000-0E3FFF
    E4000_16K,  // 0E4000-0E7FFF
    E8000_16K,  // 0E8000-0EBFFF
    EC000_16K,  // 0EC000-0EFFFF
    F0000_64K   // 0F0000-0FFFFF
} SHADOW_BIOS_RANGE;

typedef struct {
    UINT8     ChannelNumber;  //< Zero based channel number.
    UINT8     DimmNumber;     //< Zero based DIMM number.
    UINT8     Rank;           //< Zero based rank number.
    UINT8     Bank;           //< Zero based bank number.
    UINT16    Cas;            //< Zero based CAS number.
    UINT16    Ras;            //< Zero based RAS number.
} NB_ADDRESS_DECODE;

EFI_STATUS
NBProgramPAMRegisters (
	EFI_BOOT_SERVICES		*gBS,
	EFI_RUNTIME_SERVICES	*gRS,
	UINT32					StartAddress, 
	UINT32					Length,
	UINT8					Setting, 
	UINT32					*Granularity OPTIONAL
);

EFI_STATUS NBPeiProgramPAMRegisters(
    IN EFI_PEI_SERVICES **PeiServices,
    UINT32				StartAddress,
    UINT32				Length,
    UINT8				Setting,
    UINT32				*Granularity OPTIONAL
);


EFI_STATUS NBPAMWriteBootScript(
#if PI_SPECIFICATION_VERSION < 0x0001000A || (BACKWARD_COMPATIBLE_MODE == 1)
	IN EFI_BOOT_SCRIPT_SAVE_PROTOCOL *BootScriptSave
#else
    IN EFI_S3_SAVE_STATE_PROTOCOL    *BootScriptSave
#endif
);

EFI_STATUS
NbRuntimeShadowRamWriteExt (
    IN SHADOW_ARRTIBUTE   Attribute,
    IN SHADOW_BIOS_RANGE  Range
);

//To Exclude AMI Native Root Bridge Driver HOOKS from CSP LIB
#include <Token.h>
#if AMI_ROOT_BRIDGE_SUPPORT == 1

//***********************************************************************
//-----------------------------------------------------------------------
//Function Prototypes for PciRootBridgeIo
//-----------------------------------------------------------------------
//***********************************************************************
extern DXE_SERVICES		*gDxeSvcTbl;


EFI_STATUS HbResAllocNotifyPhase(
	IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL 		*This,
	IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PHASE            Phase );

EFI_STATUS HbResAllocGetNextRootBridge(
	IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL  		*This,
	IN OUT EFI_HANDLE                                       	*RootBridgeHandle);

EFI_STATUS HbResAllocGetAllocAttributes(
	IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL  		*This,
	IN EFI_HANDLE	                                        	RootBridgeHandle,
  	OUT UINT64                                              	*Attributes);

EFI_STATUS HbResAllocStartBusEnumeration(
	IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL			*This,
	IN EFI_HANDLE 	                                     		RootBridgeHandle,
	OUT VOID	                                                **Configuration);

EFI_STATUS HbResAllocSetBusNumbers(
		IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL		*This,
  		IN EFI_HANDLE                                           RootBridgeHandle,
  		IN VOID                                                 *Configuration);

EFI_STATUS HbResAllocSubmitResources(
		IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL  		*This,
  		IN EFI_HANDLE                                               RootBridgeHandle,
	  	IN VOID                                                     *Configuration);
EFI_STATUS HbResAllocGetProposedResources(
  		IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL			*This,
		IN EFI_HANDLE                                              	RootBridgeHandle,
		OUT VOID                                                    **Configuration);

EFI_STATUS HbResAllocPreprocessController(
		IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL  		*This,
		IN EFI_HANDLE                                               RootBridgeHandle,
  		IN EFI_PCI_CONFIGURATION_ADDRESS              				PciAddress,
		IN EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE             Phase);

EFI_STATUS HbCspAllocateResources( PCI_HOST_BRG_DATA *HostBrgData,
                                PCI_ROOT_BRG_DATA *RootBrgData,
                                UINTN RootBrgIndex);

//***********************************************************************
//Chipset hooks function Prototypes

//***********************************************************************
//  This hooks will be called from Notify Phase Host Bridge Resource 
//  Allocation Protocol member function 
//***********************************************************************
//-----------------------------------------------------------------------
//CSP Function invoked in PCI Host Bridge Protocol when NotifyPhase function is called
//with phase EfiPciHostBridgeBeginEnumeration
EFI_STATUS HbNotifyCspBeforeEnumeration(
		EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL	*ResAllocProtocol,
		EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL						**RbIoProtocolBuffer,
		UINTN												RbCount	);

//-----------------------------------------------------------------------
//CSP Function invoked in PCI Host Bridge Protocol when NotifyPhase function is called
//with phase EfiPciHostBridgeBeginBusAllocation
EFI_STATUS HbNotifyCspBeginBusAllocation(
		EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL	*ResAllocProtocol,
		EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL						**RbIoProtocolBuffer,
		UINTN												RbCount	);

//-----------------------------------------------------------------------
//CSP Function invoked in PCI Host Bridge Protocol when NotifyPhase function is called
//with phase EfiPciHostBridgeEndBusAllocation
EFI_STATUS HbNotifyCspEndBusAllocation(
		EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL	*ResAllocProtocol,
		EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL						**RbIoProtocolBuffer,
		UINTN												RbCount	);

//-----------------------------------------------------------------------
//CSP Function invoked in PCI Host Bridge Protocol when NotifyPhase function is called
//with phase EfiPciHostBridgeBeginResourceAllocation
EFI_STATUS HbNotifyCspBeginResourceAllocation(
		EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL	*ResAllocProtocol,
		EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL						**RbIoProtocolBuffer,
		UINTN												RbCount	);

//-----------------------------------------------------------------------
//CSP Function invoked in PCI Host Bridge Protocol when NotifyPhase function is called
//with phase EfiPciHostBridgeAllocateResources
EFI_STATUS HbNotifyCspAllocateResources(
		EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL	*ResAllocProtocol,
		EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL						**RbIoProtocolBuffer,
		UINTN												RbCount	);

//-----------------------------------------------------------------------
//CSP Function invoked in PCI Host Bridge Protocol when NotifyPhase function is called
//with phase EfiPciHostBridgeSetResources
EFI_STATUS HbNotifyCspSetResources(
		EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL	*ResAllocProtocol,
		EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL						**RbIoProtocolBuffer,
		UINTN												RbCount	);

//-----------------------------------------------------------------------
//CSP Function invoked in PCI Host Bridge Protocol when NotifyPhase function is called
//with phase EfiPciHostBridgeEndResourceAllocation
EFI_STATUS HbNotifyCspEndResourceAllocation(
		EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL	*ResAllocProtocol,
		EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL						**RbIoProtocolBuffer,
		UINTN												RbCount	);

//***********************************************************************
//  This hooks will be called from Other Host Bridge Resource 
//  Allocation Protocol member functions
//***********************************************************************

//-----------------------------------------------------------------------
//CSP Function invoked in PCI Host Bridge Protocol StartBusEnumeration function
EFI_STATUS HbCspStartBusEnumeration(
		PCI_HOST_BRG_DATA *HostBrgData,PCI_ROOT_BRG_DATA *RootBrgData,UINTN RootBrgIndex);

//-----------------------------------------------------------------------
//CSP Function invoked in PCI Host Bridge Protocol SetBusNumbers function
EFI_STATUS HbCspSetBusNnumbers(
		PCI_HOST_BRG_DATA *HostBrgData,PCI_ROOT_BRG_DATA *RootBrgData,UINTN RootBrgIndex);

//-----------------------------------------------------------------------
//CSP Function invoked in PCI Host Bridge Protocol SubmitResources function
EFI_STATUS HbCspSubmitResources(
		PCI_HOST_BRG_DATA *HostBrgData,PCI_ROOT_BRG_DATA *RootBrgData,UINTN RootBrgIndex);

//-----------------------------------------------------------------------
//CSP Function invoked in PCI Host Bridge Protocol SubmitResources function
EFI_STATUS HbCspAdjustMemoryMmioOverlap(
		PCI_HOST_BRG_DATA *HostBrgData,PCI_ROOT_BRG_DATA *RootBrgData,UINTN RootBrgIndex);

//-----------------------------------------------------------------------
//BASIC Chipset initialization function calle after Initialization of generic part of the 
//Host and Root Bridges. All Handles for PCIHostBrg and PciRootBrg has been created and 
//Protocol Intergaces installed
//Need to update this function from Aptio core Version 4.6.2 onwards
//##<AAV>###if ((CORE_MINOR_VERSION >=6) && (CORE_REVISION > 1))
#if (CORE_COMBINED_VERSION > 0x4028B)
EFI_STATUS HbCspBasicChipsetInit(PCI_HOST_BRG_DATA *HostBrg0);
#else
EFI_STATUS HbCspBasicChipsetInit( IN  EFI_HANDLE              ImageHandle,PCI_HOST_BRG_DATA *HostBrg0);
#endif

//-----------------------------------------------------------------------
//Csp Function which will return PCI Segment Number for 
//Chipsets that capable of having multiple PCI segments
//Parameters UINTN	HostNumber, UINTN RootNumber is ZERO based
UINTN HbCspGetPciSegment(UINTN	HostBridgeNumber, UINTN RootBridgeNumber);

//-----------------------------------------------------------------------
//Csp Function which will be called each time PciBus Enumerator finds Pci Device
//on the bus. This Function will allow to program particular device registers before
//resource requirements are collected from that device.
EFI_STATUS HbCspPreprocessController(PCI_HOST_BRG_DATA *HostBrgData, PCI_ROOT_BRG_DATA *RootBrgData, 
								UINTN RootBrgNumber, EFI_PCI_CONFIGURATION_ADDRESS PciAddress, 
								EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE Phase);



//-----------------------------------------------------------------------
//Chipset Specific function to Map Internal Device address 
//residing ABOVE 4G to the BELOW 4G address space for DMA. 
//MUST BE IMPLEMENTED if CHIPSET supports address space 
//decoding ABOVE 4G.
EFI_STATUS RbCspIoPciMap (
	IN PCI_ROOT_BRG_DATA							*RbData,
	IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_OPERATION	Operation,
	IN EFI_PHYSICAL_ADDRESS							HostAddress,
	IN OUT UINTN									*NumberOfBytes,
	OUT EFI_PHYSICAL_ADDRESS						*DeviceAddress,
	OUT VOID										**Mapping);

//-----------------------------------------------------------------------
//Chipset Specific function to UnMap result of previouse PciRbIo->Map
//operation
EFI_STATUS RbCspIoPciUnmap (
	IN PCI_ROOT_BRG_DATA							*RbData,
	IN PCI_ROOT_BRIDGE_MAPPING						*Mapping);

//-----------------------------------------------------------------------
//Chipset Specific function to Sets attributes for a resource range 
//on a PCI root bridge.
EFI_STATUS RbCspIoPciAttributes(PCI_ROOT_BRG_DATA *RbData,
								IN     UINT64	Attributes,
								IN OUT UINT64	*ResourceBase OPTIONAL,
								IN OUT UINT64	*ResourceLength OPTIONAL);

//-----------------------------------------------------------------------
//Csp Function which actualy access PCI Config Space
//Chipsets that capable of having PCI EXPRESS Ext Cfg Space transactions 
//Must Implement this access routine here
EFI_STATUS RootBridgeIoPciRW (
	IN PCI_ROOT_BRG_DATA						*RbData,
	IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH	Width,
	IN UINT64									Address,
	IN UINTN									Count,
	IN OUT VOID									*Buffer,
	IN BOOLEAN									Write);


//Pci RB CSP function prototypes
EFI_STATUS RbCspIoPciUnmap (
	IN	PCI_ROOT_BRG_DATA 							*RbData, 
	OUT PCI_ROOT_BRIDGE_MAPPING 					*Mapping);

EFI_STATUS RbCspIoPciMap (
	IN PCI_ROOT_BRG_DATA 							*RbData,
	IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_OPERATION	Operation,
	IN EFI_PHYSICAL_ADDRESS							HostAddress,
	IN OUT UINTN									*NumberOfBytes,
	OUT EFI_PHYSICAL_ADDRESS						*DeviceAddress,
	OUT VOID										**Mapping);


//##<AAV>##
EFI_STATUS PcieComSkipPciDevice (
    IN PCI_DEV_INFO *PciDevice
);

//--------------------------------------
//#if AMI_ROOT_BRIDGE_SUPPORT == 1
#endif 

EFI_STATUS NbProgramPciDevice (
    IN PCI_DEV_INFO                     *PciDevice
);

EFI_STATUS NbUpdatePciDeviceAttributes (
    IN PCI_DEV_INFO                     *PciDevice,
    IN OUT UINT64                       *Attributes,
    IN UINT64                           Capabilities,
    IN BOOLEAN                          Set
);

#ifdef __cplusplus
}
#endif
#endif


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
