//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file NbPciCsp.c
    This file contains generic NB code that is common between
    various components such as NB PEI, DXE etc

**/

#include <Token.h>

/****** DO NOT WRITE ABOVE THIS LINE *******/
#include <Agesa.h>
#include "PciHostBridge.h"
#include <Acpi20.h>
#include "Hob.h"
#include <Nb.h>
//#include <AmiCspLib.h>
#include <setup.h>
#if CORE_COMBINED_VERSION >= 0x4028b
#include <Protocol\AcpiTable.h>    // this protocol exists only if CORE_COMBINED_VERSION >= 4.6.5.1
#endif
#if (!defined(BACKWARD_COMPATIBLE_MODE))
#include "Protocol\AcpiSupport.h"
#endif
#include <Protocol/FabricResourceManagerServicesProtocol.h>
#include <Protocol/FabricTopologyServices.h>
#include <Library/ProgramMmioRange.h>

//--------------------------------------------------------------------------

#ifdef EFI_DEBUG

  // If EFI_DEBUG is defined, asserts will stop execution at the line of
  // the error. Otherwise, execution is aborted and 'return_error' is 
  // returned. (In many cases, 'retun_error' equals 'status.')
  #define ASSERT_EFI_RETURN(status, return_error) \
    if (EFI_ERROR(status)) {\
      TRACE((-1,"ASSERT in %s on %i: %s\n",__FILE__, __LINE__, #status));\
      EFI_DEADLOOP(); \
    }
#else
  #define ASSERT_EFI_RETURN(status, return_error) \
    if (EFI_ERROR(status)) {\
        return return_error;\
    }
#endif
//Global Vars

VOID                        *mAcpiReg;
UINTN                       mMcfgTblKey = 0;
EFI_EVENT                   mAcpiEvent;

EFI_GUID    gEfiSetupGuid           = SETUP_GUID;

#if CORE_COMBINED_VERSION >= 0x5000a
static  EFI_GUID    gAmiGlobalVariableGuid = AMI_GLOBAL_VARIABLE_GUID;
#else
static  EFI_GUID    gAmiGlobalVariableGuid = EFI_GLOBAL_VARIABLE;
#endif
//--------------------------------------------------------------------------

//WMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWM
//PORTING!!!//PORTING!!!//PORTING!!!//PORTING!!!//PORTING!!!//PORTING!!!//PORTING!!!
//PORTING!!!//PORTING!!!//PORTING!!!//PORTING!!!//PORTING!!!//PORTING!!!//PORTING!!!
//PORTING!!!//PORTING!!!//PORTING!!!//PORTING!!!//PORTING!!!//PORTING!!!//PORTING!!!
//PORTING!!!//PORTING!!!//PORTING!!!//PORTING!!!//PORTING!!!//PORTING!!!//PORTING!!!
//PORTING!!!//PORTING!!!//PORTING!!!//PORTING!!!//PORTING!!!//PORTING!!!//PORTING!!!
//WMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWMWM

#define PCI_IO_REGSET_COUNT 0x01

#define FLASH_DEVICE_BASE_ADDRESS (0xFFFFFFFF - FLASH_SIZE + 1)
#define FWH_FEATURE_SPACE_BASE (FLASH_DEVICE_BASE_ADDRESS & ~(UINTN)0x400000)

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// For HOST AND ROOT BRIDGE Architectural information 
// see chapter 12.1.1 PCI Root Bridge I/O Overview of EFI 1.1 spec
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//===============================================================================================
//This table will provide information on how many Root Bridges 
//particular Host Bridge will produce.
//The table has NB_NUMBER_OF_HOST_BRG entries.
//(Token NB_NUMBER_OF_HOST_BRG is defined in NB.sdl)
//
//For i945 it will be ONE ROOT UNDER ONE HOST.
//
//If system has more than ONE Host Add more Lines
//-----------------------------------------------------------------------------------------------
UINTN   gRbCountTbl[NB_NUMBER_OF_HOST_BRG] =
{
    1,  //Number of root bridges produced by Host #0
        //Number of root bridges produced by Host #1    
        //Number of root bridges produced by Host #2    
        //Number of root bridges produced by Host #3    
};


//===============================================================================================
//This is the table to provide each host allocation attributes  
// The table has NB_NUMBER_OF_HOST_BRG entries.
//(Token NB_NUMBER_OF_HOST_BRG is defined in NB.sdl)
//Accepted values are:
//  EFI_PCI_HOST_BRIDGE_COMBINE_MEM_PMEM
//  EFI_PCI_HOST_BRIDGE_MEM64_DECODE
//These values can be ORed.
//The values are defined and explained in 
//PCI Host Bridge Resource Allocation Protocol Specification V 0.9
//-----------------------------------------------------------------------------------------------
UINT64 gHbAllocAttribTbl[NB_NUMBER_OF_HOST_BRG] =
{
    0,  //Allocation Attributes of Host #0
        //Allocation Attributes of Host #1  
        //Allocation Attributes of Host #2  
        //Allocation Attributes of Host #3  
        // And so on 
};

//===============================================================================================
//This table will provide PCI Buses Decoding Information
//in form of ACPI QWORD resource descriptor. 
//The only valid fields are:
//  ResourceType,  _MIN, _LEN. 
//  Maximum Bus may be calculated as: _MAX=_MIN+_LEN -1
//
//If system has more then one root bridge, add more lines 
//
//i945 chipset is simple chipset with only one root bridge 
//which decodes all bus ranges from 0 to 0xFF
//
//See definition of QWORD ACPI Resource Descriptor in ACPI 2.0 Spec 
//and "Address Space Resource Descriptor Internal definitions" in AcpiRes.h
// Number of table entries depends on values ported in gRbCountTbl 
//-----------------------------------------------------------------------------------------------
//  Name,  Len, Resource Type,     Flags    _GRA,   _MIN,   _MAX,   _TRA,   _LEN
ASLR_QWORD_ASD  gRbBusesTbl[] =
{
//for Root 0 at Host 0
    {0x8A, 0x2B,ASLRV_SPC_TYPE_BUS, 0,0,    0x1,    0x00,   0xFF,   0,      0x100 }//Bus
//for Root 1 at Host 0 if any 

//for Root 0 at Host 1 if any
};

//===============================================================================================
//This is the table for the Capabilities Supported by ROOT BRIDGE
// See EFI 1.1 spec for meaning of the Capabilities bits
//
//if system has more than one root bridge add more lines
// Number of table entries depends on values ported in gRbCountTbl
//-----------------------------------------------------------------------------------------------
UINT64  gRbSupportsTbl[] =
{
//for Root 0 at Host 0
    (EFI_PCI_ATTRIBUTE_IDE_PRIMARY_IO | EFI_PCI_ATTRIBUTE_IDE_SECONDARY_IO| \
     EFI_PCI_ATTRIBUTE_ISA_IO | EFI_PCI_ATTRIBUTE_VGA_PALETTE_IO  | \
     EFI_PCI_ATTRIBUTE_VGA_MEMORY | EFI_PCI_ATTRIBUTE_VGA_IO | \
     EFI_PCI_ATTRIBUTE_ISA_MOTHERBOARD_IO),
//for Root 1 at Host 0 if any

//for Root 0 at Host 1 if any

// and so on...
};


/**
    Csp Function which will return PCI Segment Number for
    Chipsets which capable of decoding multiple PCI segments
    Parameters UINTN  HostNumber, UINTN RootNumber are ZERO based

        
    @param HostBridgeNumber 
    @param RootBridgeNumber 

         
    @retval VOID

    @note  
  THIS FUNCTION MUST BE PORTED FOR MULTI-HOST SYSTEMS

  HOST bridge handle supports:
    - ResourceAllocation Protocol (REQUIRED);
    - RootHotplugControllerInitialization Protocol (OPTIONAL);
    - PciPlatform Protocol (OPTIONAL).
  ROOT bridge handle supports:
    -PciRootBridgeIo Protocol (REQUIRED).

**/

UINTN HbCspGetPciSegment(
    IN UINTN HostBridgeNumber,
    IN UINTN RootBridgeNumber)
{
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//

//Usualy even server chipsets decodes only one PCI segment
//but if  chipsets has more than one SEGMENT we have to specify 
//which HOST/ROOT(s) pare will have SEG=0; SEG=1 and so on...

    return 0; // this is for i945 chipset

//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
}


/**
    CSP Function invoked in PCI Host Bridge Protocol when NotifyPhase function is called
    with phase EfiPciHostBridgeBeginEnumeration

        
    @param ResAllocProtocol 
    @param RbIoProtocolBuffer 
    @param RbCount 

         
    @retval EFI_STATUS return EFI status

    @note  
  THIS FUNCTION MUST BE PORTED FOR MULTI-HOST SYSTEMS

  HOST bridge handle supports:
    - ResourceAllocation Protocol (REQUIRED);
    - RootHotplugControllerInitialization Protocol (OPTIONAL);
    - PciPlatform Protocol (OPTIONAL).
  ROOT bridge handle supports:
    -PciRootBridgeIo Protocol (REQUIRED).

**/

EFI_STATUS HbNotifyCspBeforeEnumeration(
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL    *ResAllocProtocol,
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                     **RbIoProtocolBuffer,
    IN UINTN                                               RbCount)
{
    EFI_STATUS  Status=EFI_SUCCESS;
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//Any Additional Variables goes here


//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
    return Status;
}

/**
    This function will be invoked in PCI Host Bridge Driver
    before converting all Non Existant MMIO into PCI MMIO.
    In order to allow CSP code do aome none standard conversion.

    @param ImageHandle - this image Handle
    @param ControllerHandle - Controller(RB) Handle (Optional).

    @retval EFI_STATUS
            EFI_UNSUPPORTED - means use default MMIO convertion.
            EFI_SUCCESS - CSP code has been converted MMIO itself.
            ANYTHING else - ERROR.
            
    @note  Porting required if needed.
**/
EFI_STATUS HbCspConvertMemoryMapIo (
            IN EFI_HANDLE      ImageHandle,
            IN EFI_HANDLE      ControllerHandle)
{
    EFI_STATUS  Status = EFI_UNSUPPORTED;

    // Any Additional Variables goes here
 //---------------------------------------   
    

    return Status;
}

EFI_STATUS HbCspConvertMemoryMapMmio (
            IN EFI_HANDLE      ImageHandle,
            IN EFI_HANDLE      ControllerHandle)
{
    EFI_STATUS  Status = EFI_UNSUPPORTED;

    // Any Additional Variables goes here
 //---------------------------------------   
    

    return Status;
}

/**
    CSP Function invoked in PCI Host Bridge Protocol when NotifyPhase function is called
    with phase EfiPciHostBridgeBeginBusAllocation

        
    @param ResAllocProtocol 
    @param RbIoProtocolBuffer 
    @param RbCount 

         
    @retval EFI_STATUS return EFI status

    @note  
  THIS FUNCTION MUST BE PORTED FOR MULTI-HOST SYSTEMS

  HOST bridge handle supports:
    - ResourceAllocation Protocol (REQUIRED);
    - RootHotplugControllerInitialization Protocol (OPTIONAL);
    - PciPlatform Protocol (OPTIONAL).
  ROOT bridge handle supports:
    -PciRootBridgeIo Protocol (REQUIRED).

**/

EFI_STATUS HbNotifyCspBeginBusAllocation(
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL    *ResAllocProtocol,
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                     **RbIoProtocolBuffer,
    IN UINTN                                               RbCount)
{
    EFI_STATUS  Status=EFI_SUCCESS;
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//Any Additional Variables goes here


//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
    return Status;
}

/**
    CSP Function invoked in PCI Host Bridge Protocol when NotifyPhase function is called
    with phase EfiPciHostBridgeEndBusAllocation

        
    @param ResAllocProtocol 
    @param RbIoProtocolBuffer 
    @param RbCount 

         
    @retval EFI_STATUS reutrn EFI status

    @note  
  THIS FUNCTION MUST BE PORTED FOR MULTI-HOST SYSTEMS

  HOST bridge handle supports:
    - ResourceAllocation Protocol (REQUIRED);
    - RootHotplugControllerInitialization Protocol (OPTIONAL);
    - PciPlatform Protocol (OPTIONAL).
  ROOT bridge handle supports:
    -PciRootBridgeIo Protocol (REQUIRED).

**/

EFI_STATUS HbNotifyCspEndBusAllocation(
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL    *ResAllocProtocol,
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                     **RbIoProtocolBuffer,
    IN UINTN                                               RbCount)
{
    EFI_STATUS  Status=EFI_SUCCESS;
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//Any Additional Variables goes here


//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
    return Status;
}


/**
    CSP Function invoked in PCI Host Bridge Protocol when NotifyPhase function is called
    with phase EfiPciHostBridgeBeginResourceAllocation

        
    @param ResAllocProtocol 
    @param RbIoProtocolBuffer 
    @param RbCount 

         
    @retval EFI_STATUS return EFI status

    @note  
  THIS FUNCTION MUST BE PORTED FOR MULTI-HOST SYSTEMS

  HOST bridge handle supports:
    - ResourceAllocation Protocol (REQUIRED);
    - RootHotplugControllerInitialization Protocol (OPTIONAL);
    - PciPlatform Protocol (OPTIONAL).
  ROOT bridge handle supports:
    -PciRootBridgeIo Protocol (REQUIRED).

**/

EFI_STATUS HbNotifyCspBeginResourceAllocation(
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL    *ResAllocProtocol,
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                     **RbIoProtocolBuffer,
    IN UINTN                                               RbCount)
{
    EFI_STATUS  Status=EFI_SUCCESS;
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//Any Additional Variables goes here


//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
    return Status;
}


/**
    CSP Function invoked in PCI Host Bridge Protocol when NotifyPhase function is called
    with phase EfiPciHostBridgeAllocateResources

        
    @param ResAllocProtocol 
    @param RbIoProtocolBuffer 
    @param RbCount 

         
    @retval EFI_STATUS return EFI status

    @note  
  THIS FUNCTION MUST BE PORTED FOR MULTI-HOST SYSTEMS

  HOST bridge handle supports:
    - ResourceAllocation Protocol (REQUIRED);
    - RootHotplugControllerInitialization Protocol (OPTIONAL);
    - PciPlatform Protocol (OPTIONAL).
  ROOT bridge handle supports:
    -PciRootBridgeIo Protocol (REQUIRED).

**/

EFI_STATUS HbNotifyCspAllocateResources(
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL    *ResAllocProtocol,
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                     **RbIoProtocolBuffer,
    IN UINTN                                               RbCount)
{
    EFI_STATUS  Status=EFI_SUCCESS;
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//Any Additional Variables goes here


//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
    return Status;
}


/**
    CSP Function invoked in PCI Host Bridge Protocol when NotifyPhase function is called
    with phase EfiPciHostBridgeSetResources

        
    @param ResAllocProtocol 
    @param RbIoProtocolBuffer 
    @param RbCount 

         
    @retval EFI_STATUS return EFI status

    @note  
  THIS FUNCTION MUST BE PORTED FOR MULTI-HOST SYSTEMS

  HOST bridge handle supports:
    - ResourceAllocation Protocol (REQUIRED);
    - RootHotplugControllerInitialization Protocol (OPTIONAL);
    - PciPlatform Protocol (OPTIONAL).
  ROOT bridge handle supports:
    -PciRootBridgeIo Protocol (REQUIRED).

**/

EFI_STATUS HbNotifyCspSetResources(
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL    *ResAllocProtocol,
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                     **RbIoProtocolBuffer,
    IN UINTN                                               RbCount)
{
    EFI_STATUS  Status=EFI_SUCCESS;
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//Any Additional Variables goes here


//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
    return Status;
}

/**
    CSP Function invoked in PCI Host Bridge Protocol when NotifyPhase function is called
    with phase EfiPciHostBridgeEndResourceAllocation

        
    @param ResAllocProtocol 
    @param RbIoProtocolBuffer 
    @param RbCount 

         
    @retval EFI_STATUS return EFI status

    @note  
  THIS FUNCTION MUST BE PORTED FOR MULTI-HOST SYSTEMS

  HOST bridge handle supports:
    - ResourceAllocation Protocol (REQUIRED);
    - RootHotplugControllerInitialization Protocol (OPTIONAL);
    - PciPlatform Protocol (OPTIONAL).
  ROOT bridge handle supports:
    -PciRootBridgeIo Protocol (REQUIRED).

**/

EFI_STATUS HbNotifyCspEndResourceAllocation(
    IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL    *ResAllocProtocol,
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                     **RbIoProtocolBuffer,
    IN UINTN                                               RbCount)
{
    EFI_STATUS  Status=EFI_SUCCESS;
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//Any Additional Variables goes here


//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
    return Status;
}


/**
    CSP Function invoked in PCI Host Bridge Protocol StartBusEnumeration function
    It must prepare initial Bus ACPI Resource

        
    @param HostBrgData 
    @param RootBrgData 
    @param RootBrgIndex 

         
    @retval EFI_STATUS return EFI status

    @note  
  THIS FUNCTION MUST BE PORTED FOR MULTI-HOST SYSTEMS

  HOST bridge handle supports:
    - ResourceAllocation Protocol (REQUIRED);
    - RootHotplugControllerInitialization Protocol (OPTIONAL);
    - PciPlatform Protocol (OPTIONAL).
  ROOT bridge handle supports:
    -PciRootBridgeIo Protocol (REQUIRED).

**/

EFI_STATUS HbCspStartBusEnumeration(
    IN PCI_HOST_BRG_DATA *HostBrgData,
    IN PCI_ROOT_BRG_DATA *RootBrgData,
    IN UINTN RootBrgIndex)
{
    EFI_STATUS  Status=EFI_SUCCESS;

//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//Any Additional Variables goes here

//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
    return Status;
}


/**
    CSP Function invoked in PCI Host Bridge Protocol SubmitBusNumbers function

        
    @param HostBrgData 
    @param RootBrgData 
    @param RootBrgIndex 

         
    @retval EFI_STATUS return EFI status

    @note  
  THIS FUNCTION MUST BE PORTED FOR MULTI-HOST SYSTEMS

  HOST bridge handle supports:
    - ResourceAllocation Protocol (REQUIRED);
    - RootHotplugControllerInitialization Protocol (OPTIONAL);
    - PciPlatform Protocol (OPTIONAL).
  ROOT bridge handle supports:
    -PciRootBridgeIo Protocol (REQUIRED).

**/

EFI_STATUS HbCspSetBusNnumbers(
    IN PCI_HOST_BRG_DATA *HostBrgData,
    IN PCI_ROOT_BRG_DATA *RootBrgData,
    IN UINTN RootBrgIndex)
{
    EFI_STATUS  Status=EFI_SUCCESS;
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//Any Additional Variables goes here


//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
    return Status;
}


/**
    CSP Function invoked in PCI Host Bridge Protocol SubmitResources function

        
    @param HostBrgData 
    @param RootBrgData 
    @param RootBrgIndex 

         
    @retval EFI_STATUS return EFI status

    @note  
  THIS FUNCTION MUST BE PORTED FOR MULTI-HOST SYSTEMS

  HOST bridge handle supports:
    - ResourceAllocation Protocol (REQUIRED);
    - RootHotplugControllerInitialization Protocol (OPTIONAL);
    - PciPlatform Protocol (OPTIONAL).
  ROOT bridge handle supports:
    -PciRootBridgeIo Protocol (REQUIRED).

**/

EFI_STATUS HbCspSubmitResources(
    IN PCI_HOST_BRG_DATA *HostBrgData,
    IN PCI_ROOT_BRG_DATA *RootBrgData,
    IN UINTN RootBrgIndex)
{
    EFI_STATUS  Status=EFI_SUCCESS;
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//Any Additional Variables goes here


//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
    return Status;
}

// <AMI_PHDR_START>
//----------------------------------------------------------------------------
// 
// Name: ParseGcdMmio32
//
// Description:
//      This function parses the GCD descriptors to find the regions of
//      memory associated with the first MMIO hole at 4G. If any of the 
//      parameters are NULL, the execution path will be to only generate
//      debug output.
//
// Input:   
//      OUT EFI_PHYSICAL_ADDRESS *HighestMem32,
//                  -  This parameter will be updated with the highest 
//                     system memory addresss below 4G.
//      OUT EFI_PHYSICAL_ADDRESS *LowestMmio32,
//                  -  This parameter will be updated with the lowest
//                     MMIO address in the GCD map.
//      OUT EFI_PHYSICAL_ADDRESS *LowestAllocatedMmio32,
//                  -  This parameter will be updated with the lowest
//                     MMIO address in the GCD map that has an image handle
//      OUT EFI_PHYSICAL_ADDRESS *HighestAllocatedMmio32
//                  -  This parameter will be updated with the highest
//                     MMIO address in the GCD map that has an image handle
//
// Output:
//      EFI_STATUS (Return Value)
//                  = EFI_SUCCESS if successful
//                  = or valid EFI error code
//
// Modified:    
//      None
//
// Referrals:   
//      (     DXE_SERVICES*)->GetMemorySpaceMap
//      (EFI_BOOT_SERVICES*)->FreePool
//
// Notes:
//      
//-------------------------------------------------------------------------- 
// <AMI_PHDR_END>

EFI_STATUS ParseGcdMmio32(
    OUT EFI_PHYSICAL_ADDRESS   *HighestMem32,
    OUT EFI_PHYSICAL_ADDRESS   *LowestMmio32,
    OUT EFI_PHYSICAL_ADDRESS   *LocalLowestAllocatedMmio32,
    OUT EFI_PHYSICAL_ADDRESS   *HighestAllocatedMmio32 )
{
    EFI_GCD_MEMORY_SPACE_DESCRIPTOR    *MemorySpaceMap;
    EFI_STATUS              Status;
    UINTN                   NumberOfDescriptors;
    UINTN                   Index;
    EFI_PHYSICAL_ADDRESS    Start;
    EFI_PHYSICAL_ADDRESS    End;
    EFI_GCD_MEMORY_SPACE_DESCRIPTOR *Descr;
    BOOLEAN                 UpdateParameters = TRUE;
    DXE_SERVICES                        *dxe;

    //If any of the parameters are NULL, only generate debug output.
    if ( HighestMem32 == NULL || LowestMmio32 == NULL || 
         LocalLowestAllocatedMmio32  == NULL || HighestAllocatedMmio32  == NULL ) 
    {
        //Only generate debug output.
        UpdateParameters = FALSE;
    }
    else {
        *LowestMmio32 = *LocalLowestAllocatedMmio32 = 0xFFFFFFFF; //Initialize to 0xFFFFFFFF guarantee any 32bit MMIO will be lower and replace the value.
        *HighestMem32 = *HighestAllocatedMmio32 = 0; //Initialize to 0x0F guarantee any 32bit MMIO will be higher and replace the value.
    }
    Status = LibGetDxeSvcTbl(&dxe);
    ASSERT_EFI_ERROR(Status);
    if(EFI_ERROR(Status))
        return Status;

    Status = dxe->GetMemorySpaceMap(&NumberOfDescriptors, 
                                          &MemorySpaceMap);
    ASSERT_EFI_ERROR(Status);

    //Find the lowest MMIO and lowest allocated MMIO in GCD.

    PCI_TRACE((-1, "Finding EFI_GCD_MEMORY_SPACE_DESCRIPTORs:\n"));
    PCI_TRACE((-1, "            Base            Length  Attributes/Type (*=Image, #=Device)\n"));
    for(Index = 0; Index < NumberOfDescriptors; ++Index) {
        Descr = &MemorySpaceMap[Index];
        Start = Descr->BaseAddress;
        End = Start + Descr->Length - 1;

        if (UpdateParameters){

            PCI_TRACE((-1, "            0x%x             0x%x  0x%x\n",Start, End, Descr->GcdMemoryType));                  
                  
            // Ignore below 1MB and above 4G
            if (Start < 0x100000 || End > 0xFFFFFFFF) 
                continue;

            // Find highest system below 4GB memory.
            // Treat any non MMIO as system memory. Not all system memory 
            // is reported as system memory, such as SMM.
            if (Descr->GcdMemoryType != EfiGcdMemoryTypeMemoryMappedIo && Start < *LocalLowestAllocatedMmio32 && End > *HighestMem32)
            {
                    *HighestMem32 = End;
                    PCI_TRACE((-1, "New HighestMem32=0x%x\n", End ));

            } // Find Lowest and Highest mmio above system memory.
            else if (Descr->GcdMemoryType == EfiGcdMemoryTypeMemoryMappedIo) {
                if (Start < *LowestMmio32)
                {
                    *LowestMmio32 = Start;
                    PCI_TRACE((-1, "New LowestMmio32=0x%x\n", Start ));
                }
                //If ImageHandle in non-NULL, then the MMIO is allocated.
                if ((Start < *LocalLowestAllocatedMmio32) && Descr->ImageHandle)
                {
                    *LocalLowestAllocatedMmio32 = Start;
                    PCI_TRACE((-1, "New LowestAllocatedMmio32=0x%x\n", Start ));
                }   
                if ((End > *HighestAllocatedMmio32) && Descr->ImageHandle )
                {
                    *HighestAllocatedMmio32 = End;
                    PCI_TRACE((-1, "New HighestAllocatedMmio32=0x%x\n", End ));
                }
            }
        }
    }
    if (UpdateParameters){
        PCI_TRACE((-1, 
            "HighestMem32=0x%X  LowestMmio32=0x%X\n",
            *HighestMem32, *LowestMmio32 ));
        PCI_TRACE((-1, 
            "LowestAllocatedMmio32=0x%X  HighestAllocatedMmio32=0x%X\n",
            *LocalLowestAllocatedMmio32, *HighestAllocatedMmio32 ));
            
        if (*HighestMem32 + 1 != *LowestMmio32) {
            PCI_TRACE((-1, 
                "System Memory and MMIO are not consecutive.\n"));
            PCI_TRACE((-1, 
                "Top of below 4G memory: 0x%X\n   Bottom of MMIO: 0x%lX\n",
                *HighestMem32, *LowestMmio32 ));
        }
    }

    pBS->FreePool(MemorySpaceMap);

    return EFI_SUCCESS;
}

EFI_STATUS DumpGcdIO()
{
	EFI_STATUS  Status = EFI_SUCCESS;
    DXE_SERVICES                *dxe;
	UINTN num_io_descriptors = 0;
	UINTN count = 0;
	EFI_GCD_IO_SPACE_DESCRIPTOR *iodescriptor;
	EFI_GCD_IO_SPACE_DESCRIPTOR *Descr;
	EFI_PHYSICAL_ADDRESS    End;
	
    Status=LibGetDxeSvcTbl(&dxe);
         if(EFI_ERROR(Status)) return Status;
         
	NB_TRACE((TRACE_NB, "DumpGcdIO\n"));		
	dxe->GetIoSpaceMap(&num_io_descriptors,&iodescriptor);
	
	for(count=0;count < num_io_descriptors;count++)
	{
		Descr = &iodescriptor[count];		
		End = Descr->BaseAddress + Descr->Length - 1;
		if(Descr->ImageHandle==0)
			NB_TRACE((TRACE_NB, "Type=0x%X Range=0x%X to 0x%X Free\n", Descr->GcdIoType, Descr->BaseAddress, End));		
		else
			NB_TRACE((TRACE_NB, "Type=0x%X Range=0x%X to 0x%X Used\n", Descr->GcdIoType, Descr->BaseAddress, End));		
		
	}
		
	return Status;
}
		
/**
 EstimatedMmioAndIOResources, If small than available resource, must Reallocate Resource For EachDie
**/
EFI_STATUS EstimatedMmioAndIOResourcesForAllRB (
        IN PCI_HOST_BRG_DATA    *HostBrgData,
        IN FABRIC_RESOURCE_MANAGER_PROTOCOL *FabricResourceManager,
        IN AMD_FABRIC_TOPOLOGY_SERVICES_PROTOCOL *FabricTopology)
{
    EFI_STATUS  Status = EFI_SUCCESS;
    FABRIC_RESOURCE_FOR_EACH_DIE        AvailResource;
    FABRIC_RESOURCE_FOR_EACH_DIE        RequestResource;
    UINT64     MmioLength;
    UINT64     IoLength;
    UINT8      Index, Index2 = 0;
    ASLR_QWORD_ASD              *res;
    BOOLEAN     NeedReallocateMmio32Resource = FALSE;
    BOOLEAN     NeedReallocateMmio64Resource = FALSE;
    BOOLEAN     NeedReallocateIoResource = FALSE;
    PCI_ROOT_BRG_DATA           *rbdata = NULL;
    UINTN       NumberOfInstalledProcessors;
    UINTN       NumberOfDie;
    UINT32      DieNumberOfPerSocket;
    UINT8       SocketNum, DieNum;
    BOOLEAN     FoundIoRes;
    
    UINT64 RbHotplugMmioLength [MAX_ROOT_BRIDGE_COUNT] = {
            MmioHotPlugLength
    };
    UINT32 RbHotplugIOLength [MAX_ROOT_BRIDGE_COUNT] = {
            IOHotPlugLength
    };
    
    pBS->SetMem((void *)&AvailResource, sizeof (FABRIC_RESOURCE_FOR_EACH_DIE), 0);
    pBS->SetMem((void *)&RequestResource, sizeof (FABRIC_RESOURCE_FOR_EACH_DIE), 0);
    
    Status = FabricResourceManager->FabricGetAvailableResource(FabricResourceManager, &AvailResource);
    if (EFI_ERROR (Status)){
        NB_TRACE ((TRACE_NB, "Fabric Get Available Resource fail (%r) \n", Status));
        return EFI_NOT_READY;
    }
    
    FabricTopology->GetSystemInfo (FabricTopology, &NumberOfInstalledProcessors, &NumberOfDie, NULL);
    DieNumberOfPerSocket = (UINT32)(NumberOfDie / NumberOfInstalledProcessors);
    
    NB_TRACE ((TRACE_NB, "-----------------------------------------------------\n"));
    for (Index = 0; Index < HostBrgData->RootBridgeCount; Index++)
    {
        //print this RB res data        
        rbdata = HostBrgData->RootBridges[Index];
        if(rbdata->NotPresent == 1) {
            continue;
        }
        
        SocketNum = Index / (UINT8)DieNumberOfPerSocket;
        DieNum = Index % (UINT8)DieNumberOfPerSocket;
        
        //NB_TRACE ((TRACE_NB,"\nRB #%d : Socket %d die %d has %d Res \n", Index, SocketNum, DieNum, rbdata->ResCount));
        
        FoundIoRes = FALSE;
        
        for (Index2 = 0; Index2 < rbdata->ResCount; Index2++){
            res=rbdata->RbRes[Index2];
            //NB_TRACE ((TRACE_NB,"Current Res type = %d : \n", res->Type));
            if(res->Type==ASLRV_SPC_TYPE_MEM) {
                
                if(!res->_LEN) {
                    continue; 
                }
                
                if(res->_GRA==32){
                   //Need MMIO32
                    NB_TRACE ((TRACE_NB,"Need MMIO32 MIN=0x%lX MAX=0x%lX LEN=0x%lX\n", res->_MIN, res->_MAX, res->_LEN));
                    NB_TRACE ((TRACE_NB,"    Reserve MMIO32 LEN 0x%lX for hot plug device \n", (RbHotplugMmioLength[Index] << 25)));
                    if(res->TFlags.MEM_FLAGS._MEM == ASLRV_MEM_CEPF) {
                        MmioLength = res->_LEN;
                        NB_TRACE ((TRACE_NB,"    Total Request MMIO32 LEN=0x%X , Available Prefetchable MMIO32 LEN=0x%lX\n", MmioLength, AvailResource.NonPrefetchableMmioSizeBelow4G[SocketNum][DieNum].Size));
                        if (MmioLength > AvailResource.PrefetchableMmioSizeBelow4G[SocketNum][DieNum].Size) { 
                            NeedReallocateMmio32Resource = TRUE;

                            if ((SocketNum == 0) && (DieNum == 0) && (MmioLength <= AvailResource.Die0SecondPrefetchableMmioSizeBelow4G.Size)) 
                            {
                                NeedReallocateMmio32Resource = FALSE;
                            }
                        }
                        RequestResource.PrefetchableMmioSizeBelow4G[SocketNum][DieNum].Size = MmioLength;
                        RequestResource.PrefetchableMmioSizeBelow4G[SocketNum][DieNum].Alignment = res->_MAX;
                    } else {
                        MmioLength = res->_LEN + (RbHotplugMmioLength[Index] << 25);
                        NB_TRACE ((TRACE_NB,"    Total Request MMIO32 LEN=0x%X , Available NonPrefetchable MMIO32 LEN=0x%lX\n", MmioLength, AvailResource.NonPrefetchableMmioSizeBelow4G[SocketNum][DieNum].Size));
                        if (MmioLength > AvailResource.NonPrefetchableMmioSizeBelow4G[SocketNum][DieNum].Size) { 
                            NeedReallocateMmio32Resource = TRUE;

                            if ((SocketNum == 0) && (DieNum == 0) && 
                                    (MmioLength <= AvailResource.Die0SecondNonPrefetchableMmioSizeBelow4G.Size)) 
                            {
                                NeedReallocateMmio32Resource = FALSE;
                            }
                        }
                        RequestResource.NonPrefetchableMmioSizeBelow4G[SocketNum][DieNum].Size = MmioLength;
                        RequestResource.NonPrefetchableMmioSizeBelow4G[SocketNum][DieNum].Alignment = res->_MAX;                        
                    }
                } 
                
                if(res->_GRA==64){
                   //Need MMIO64
                    MmioLength = res->_LEN + (RbHotplugMmioLength[Index] << 25);
                    NB_TRACE ((TRACE_NB,"Need MMIO64 MIN=0x%lX MAX=0x%lX LEN=0x%lX\n", res->_MIN, res->_MAX, res->_LEN));
                    NB_TRACE ((TRACE_NB,"    Reserve MMIO64 LEN 0x%lX for hot plug device \n", (RbHotplugMmioLength[Index] << 25)));
                    if(res->TFlags.MEM_FLAGS._MEM == ASLRV_MEM_CEPF) {
                        NB_TRACE ((TRACE_NB,"    Total Request MMIO64 LEN=0x%X , Available Prefetchable MMIO64 LEN=0x%lX\n", MmioLength, AvailResource.PrefetchableMmioSizeAbove4G[SocketNum][DieNum].Size));
                        if (MmioLength > AvailResource.PrefetchableMmioSizeAbove4G[SocketNum][DieNum].Size) {
                            NeedReallocateMmio64Resource = TRUE; 
                        }
                        RequestResource.PrefetchableMmioSizeAbove4G[SocketNum][DieNum].Size = MmioLength;
                        RequestResource.PrefetchableMmioSizeAbove4G[SocketNum][DieNum].Alignment = res->_MAX;
                    } else {
                        NB_TRACE ((TRACE_NB,"    Total Request MMIO64 LEN=0x%X , Available NonPrefetchable MMIO64 LEN=0x%lX\n", MmioLength, AvailResource.NonPrefetchableMmioSizeAbove4G[SocketNum][DieNum].Size));
                        if (MmioLength > AvailResource.NonPrefetchableMmioSizeAbove4G[SocketNum][DieNum].Size) {
                            NeedReallocateMmio64Resource = TRUE; 
                        }
                        RequestResource.NonPrefetchableMmioSizeAbove4G[SocketNum][DieNum].Size = MmioLength;
                        RequestResource.NonPrefetchableMmioSizeAbove4G[SocketNum][DieNum].Alignment = res->_MAX;
                    }
                }
                //------------------------------------------------------------
            } else if (res->Type==ASLRV_SPC_TYPE_IO) {
                //Caluate IO resource total length
                IoLength = res->_LEN + (UINT64)(RbHotplugIOLength[Index] << 12); 
                if(!IoLength) {
                    continue; 
                }
                
                FoundIoRes = TRUE;
                NB_TRACE ((TRACE_NB, "Need IO: _MIN=0x%lX; _MAX=0x%lX; _LEN=0x%lX; _GRA=0x%lX\n",
                        res->_MIN, res->_MAX, res->_LEN, res->_GRA ));
                NB_TRACE ((TRACE_NB,"    Reserve IO LEN 0x%X for hot plug device \n", (RbHotplugIOLength[Index] << 12)));
                NB_TRACE ((TRACE_NB,"    Total Request IO LEN=0x%X , Available IO LEN=0x%X\n", IoLength, AvailResource.IO[SocketNum][DieNum].Size));
                if (IoLength > AvailResource.IO[SocketNum][DieNum].Size) {
                    NeedReallocateIoResource = TRUE;  
                }
                RequestResource.IO[SocketNum][DieNum].Size = IoLength; 
            }
            
            if (Index2 == ((UINT8)rbdata->ResCount -1)) {
                //Current RB not need IO resource, but we must reserve for hot plug devices
                if (!FoundIoRes) {
                    IoLength = RbHotplugIOLength[Index] << 12;
                    if (IoLength > AvailResource.IO[SocketNum][DieNum].Size) {
                        NeedReallocateIoResource = TRUE;  
                    }
                    RequestResource.IO[SocketNum][DieNum].Size = IoLength;
                    NB_TRACE ((TRACE_NB,"  Reserve IO LEN 0x%X for hot plug device, Available IO LEN=0x%X \n", IoLength, AvailResource.IO[SocketNum][DieNum].Size));
                }
            }
        }
    }
    
    NB_TRACE ((TRACE_NB, "-----------------------------------------------------\n"));
    for (SocketNum = 0; SocketNum < NumberOfInstalledProcessors; SocketNum++) {
        for (DieNum = 0; DieNum < DieNumberOfPerSocket; DieNum++) {
            if (NeedReallocateMmio32Resource == FALSE) {
                //Restore MMIO32 default resource assign
                RequestResource.NonPrefetchableMmioSizeBelow4G[SocketNum][DieNum].Size = AvailResource.NonPrefetchableMmioSizeBelow4G[SocketNum][DieNum].Size;
                RequestResource.NonPrefetchableMmioSizeBelow4G[SocketNum][DieNum].Alignment = AvailResource.NonPrefetchableMmioSizeBelow4G[SocketNum][DieNum].Alignment;
                RequestResource.PrefetchableMmioSizeBelow4G[SocketNum][DieNum].Size = AvailResource.PrefetchableMmioSizeBelow4G[SocketNum][DieNum].Size;
                RequestResource.PrefetchableMmioSizeBelow4G[SocketNum][DieNum].Alignment = AvailResource.PrefetchableMmioSizeBelow4G[SocketNum][DieNum].Alignment;
                RequestResource.Die0SecondNonPrefetchableMmioSizeBelow4G.Size = AvailResource.Die0SecondNonPrefetchableMmioSizeBelow4G.Size;
                RequestResource.Die0SecondNonPrefetchableMmioSizeBelow4G.Alignment = AvailResource.Die0SecondNonPrefetchableMmioSizeBelow4G.Alignment;
                RequestResource.Die0SecondPrefetchableMmioSizeBelow4G.Size = AvailResource.Die0SecondPrefetchableMmioSizeBelow4G.Size;
                RequestResource.Die0SecondPrefetchableMmioSizeBelow4G.Alignment = AvailResource.Die0SecondPrefetchableMmioSizeBelow4G.Alignment;
            }
            if (NeedReallocateMmio64Resource == FALSE) {
                //Restore MMIO64 default resource assign
                RequestResource.NonPrefetchableMmioSizeAbove4G[SocketNum][DieNum].Size = AvailResource.NonPrefetchableMmioSizeAbove4G[SocketNum][DieNum].Size;
                RequestResource.NonPrefetchableMmioSizeAbove4G[SocketNum][DieNum].Alignment = AvailResource.NonPrefetchableMmioSizeAbove4G[SocketNum][DieNum].Alignment;
                RequestResource.PrefetchableMmioSizeAbove4G[SocketNum][DieNum].Size = AvailResource.PrefetchableMmioSizeAbove4G[SocketNum][DieNum].Size;
                RequestResource.PrefetchableMmioSizeAbove4G[SocketNum][DieNum].Alignment = AvailResource.PrefetchableMmioSizeAbove4G[SocketNum][DieNum].Alignment;
            }
            if (NeedReallocateIoResource == FALSE) {
                //Restore IO default resource assign
                RequestResource.IO[SocketNum][DieNum].Size = AvailResource.IO[SocketNum][DieNum].Size;
                RequestResource.IO[SocketNum][DieNum].Alignment = AvailResource.IO[SocketNum][DieNum].Alignment;
            }
        }
    }
    
    if ( (NeedReallocateMmio32Resource == TRUE) ||
         (NeedReallocateMmio64Resource == TRUE) || 
         (NeedReallocateIoResource == TRUE) ){
        NB_TRACE ((TRACE_NB, " Fabric Reallocate Resource size For Each Die : \n"));
        Status = FabricResourceManager->FabricReallocateResourceForEachDie(
                                                    FabricResourceManager, 
                                                    &RequestResource, 
                                                    NULL);
        if (EFI_ERROR (Status)){
            NB_TRACE ((TRACE_NB, "Relocate MMIO and IO resource for all RB fail (%r) \n", Status));
        } else {            
            NB_TRACE ((TRACE_NB, "Set relocate MMIO and IO resource size success \nNow do warm reset! \n"));
            pRS->ResetSystem(EfiResetWarm, EFI_SUCCESS, 0, NULL);
        }
    }
    
    NB_TRACE ((TRACE_NB, "-----------------------------------------------------\n"));
    
    return Status;
}
/**
    This function is invoked in PCI Host Bridge Driver when time
    to ask GCD for resources. You can overwrite a default
    algorithm used to allocate resources for the Root Bridge.

    @param HostBrgData Pointer to Host Bridge private structure data.
    @param RootBrgData Pointer to Root Bridge private structure data.
    @param RootBrgIndex Root Bridge index (0 Based).

    @retval EFI_STATUS

    @note  Porting required if needed.
**/
EFI_STATUS HbCspAllocateResources (
                            IN PCI_HOST_BRG_DATA    *HostBrgData,
                            IN PCI_ROOT_BRG_DATA    *RootBrgData,
                            IN UINTN                RootBrgIndex )
{
    EFI_STATUS  Status = EFI_SUCCESS;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL     *RtBrdg;
    FABRIC_RESOURCE_MANAGER_PROTOCOL    *FabricResourceManager;
    AMD_FABRIC_TOPOLOGY_SERVICES_PROTOCOL *FabricTopology;
    UINTN       NumberOfInstalledProcessors;
    UINTN       NumberOfDie;
   
    RtBrdg = &RootBrgData->RbIoProtocol;
    NB_TRACE ((TRACE_NB, "\n RootBrgIndex = %X, Total %d Root Bridge \n", RootBrgIndex, HostBrgData->RootBridgeCount));
    
    Status = pBS->LocateProtocol (
                    &gAmdFabricResourceManagerServicesProtocolGuid,
                    NULL,
                    &FabricResourceManager
                    );
    if (EFI_ERROR (Status)){
        NB_TRACE ((TRACE_NB, "Locate AmdFabricResourceManagerServicesProtocol fail (%r) \n", Status));
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
    
    FabricTopology->GetSystemInfo (FabricTopology, &NumberOfInstalledProcessors, &NumberOfDie, NULL);
        
    if ( RootBrgIndex == (NumberOfDie - 1)) {
        NB_TRACE ((TRACE_NB, "\n*****Estimated Mmio And IO Resources Start********\n")); 
        Status = EstimatedMmioAndIOResourcesForAllRB(HostBrgData, FabricResourceManager, FabricTopology);
        NB_TRACE ((TRACE_NB, "\n*****Estimated Mmio And IO Resources End********\n")); 
        
        NB_TRACE ((TRACE_NB, "\n*****Allocate Mmio And IO Resources Start********\n")); 
        Status = AllocateMMIO32ForAllRB(HostBrgData, FabricResourceManager);
        Status = AllocateMMIO64ForAllRB(HostBrgData, FabricResourceManager);
        Status = AllocateIOForAllRB(HostBrgData, FabricResourceManager);
        NB_TRACE ((TRACE_NB, "\n*****Allocate Mmio And IO Resources End********\n"));
    }
    
    return Status;
}    

/**
    This function adjusts the MMIO overlap.

        
    @param HostBrgData 
    @param RootBrgData 
    @param RootBrgIndex 

         
    @retval EFI_STATUS return EFI status

    @note  
      None

**/
EFI_STATUS
HbCspAdjustMemoryMmioOverlap(
    IN PCI_HOST_BRG_DATA *HostBrgData,
    IN PCI_ROOT_BRG_DATA *RootBrgData,
    IN UINTN RootBrgIndex)
{
	return EFI_SUCCESS;
}


/**
    This function will be invoked when Pci Host Bridge driver runs
    out of resources.

    @param HostBrgData Pointer to Host Bridge private structure data.
    @param RootBrgData 
    @param RootBrgNumber 

    @retval EFI_STATUS return EFI status

    @note  Porting required if needed.
**/
EFI_STATUS HbCspGetProposedResources (
    IN PCI_HOST_BRG_DATA                            *HostBrgData,
    IN PCI_ROOT_BRG_DATA                            *RootBrgData,
    IN UINTN                                        RootBrgNumber )
{
    EFI_STATUS  Status = EFI_SUCCESS;

    // Any Additional Variables goes here

    return Status;
}

/**
    This function adjusts the MMIO overlap.

        
    @param HostBrgData 
    @param RootBrgData 
    @param RootBrgNumber 
    @param PciAddress 
    @param Phase 


         
    @retval EFI_STATUS return EFI status

    @note  
      None

**/

EFI_STATUS
HbCspPreprocessController(
    IN PCI_HOST_BRG_DATA *HostBrgData,
    IN PCI_ROOT_BRG_DATA *RootBrgData,
    IN UINTN RootBrgNumber,
    IN EFI_PCI_CONFIGURATION_ADDRESS PciAddress,
    IN EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE Phase)
{
    EFI_STATUS  Status=EFI_SUCCESS;
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//Any Additional Variables goes here


//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
    return Status;
}


/**
    This function create NB ACPI table.

        
    @param Event 
    @param Context 

         
    @retval VOID

    @note  
      None

**/

VOID
CreateNbAcpiTables(
    IN EFI_EVENT Event,
    IN VOID *Context)
{
#if ACPI_SUPPORT
    EFI_STATUS                  Status = 0;
    MCFG_20                     *mcfg;
    EFI_ACPI_TABLE_PROTOCOL     *AcpiTableProtocol = NULL;
    UINT64                      PcieBaseAddr;

    //--------------------------------
    //it must be only one instance of such protocol
    Status = pBS->LocateProtocol(&gEfiAcpiTableProtocolGuid, NULL, &AcpiTableProtocol);
    PCI_TRACE((-1, "PciHostCSHooks: LocateProtocol(ACPISupport)=%r\n", Status));
    if (EFI_ERROR(Status))  return;

    mcfg = MallocZ(sizeof(MCFG_20));
    ASSERT(mcfg);
    if (!mcfg)  return;

    //Fill Table header;
    mcfg->Header.Signature = MCFG_SIG;
    mcfg->Header.Length = sizeof(MCFG_20);
    mcfg->Header.Revision = 1;
    mcfg->Header.Checksum = 0;
//    mcfg->Header.OemId[0]='A';      //"A M I "
//    mcfg->Header.OemId[1]=0x20;
//    mcfg->Header.OemId[2]='M';
//    mcfg->Header.OemId[3]=0x20;
//    mcfg->Header.OemId[4]='I';
//    mcfg->Header.OemId[5]=0x20;
//    mcfg->Header.OemTblId[0]='A'; 	//"OEMMCFG "
//    mcfg->Header.OemTblId[1]='M';
//    mcfg->Header.OemTblId[2]='D';
//    mcfg->Header.OemTblId[3]='O';
//    mcfg->Header.OemTblId[4]='N';
//    mcfg->Header.OemTblId[5]='T';
//    mcfg->Header.OemTblId[6]='P';
//    mcfg->Header.OemTblId[7]='I';

#ifdef  NB_MCFG_OEM_ID
    // user definition.
    pBS->CopyMem((VOID*)mcfg->Header.OemId, \
        CONVERT_TO_STRING(NB_MCFG_OEM_ID), \
        sizeof(mcfg->Header.OemId) \
    );
#else
  // Use the definition of ACPI.sdl.
  #ifdef T_ACPI_OEM_ID
    pBS->CopyMem((VOID*)mcfg->Header.OemId, \
        CONVERT_TO_STRING(T_ACPI_OEM_ID), \
        sizeof(mcfg->Header.OemId) \
    );
  #endif
#endif

#ifdef NB_MCFG_OEM_TBL_ID
    // user definition.
    pBS->CopyMem((VOID*)mcfg->Header.OemTblId, \
        CONVERT_TO_STRING(NB_MCFG_OEM_TBL_ID), \
        sizeof(mcfg->Header.OemTblId) \
    );
#else
  // Use the definition of ACPI.sdl.
  #ifdef T_ACPI_OEM_TBL_ID
    pBS->CopyMem((VOID*)mcfg->Header.OemTblId, \
        CONVERT_TO_STRING(T_ACPI_OEM_TBL_ID), \
        sizeof(mcfg->Header.OemTblId) \
    );
  #endif
#endif

    mcfg->Header.OemRev = ACPI_OEM_REV;
    mcfg->Header.CreatorId = CREATOR_ID_MS;     // "MSFT" 4D 53 46 54
    mcfg->Header.CreatorRev = CREATOR_REV_MS;   // 0x00010013;

    //fill MCFG Fields 
    PcieBaseAddr = PcdGet64 (PcdPciExpressBaseAddress);
    mcfg->BaseAddr = PcieBaseAddr;              //Base address of 256MB extended config space
    mcfg->PciSeg = 0;                           //Segment # of PCI Bus
    mcfg->StartBus = 0;                         //Start bus number of PCI segment
    mcfg->EndBus = (PCIEX_LENGTH >> 20) - 1;    //End bus number of PCI segment

    //Add table 
    Status = AcpiTableProtocol->InstallAcpiTable(AcpiTableProtocol, mcfg, mcfg->Header.Length, &mMcfgTblKey);
    PCI_TRACE((-1, "PciHostCSHooks: ACPISupport->InstallAcpiTable(MCFG) = %r\n", Status));
    ASSERT_EFI_ERROR(Status);

    //free memory used for table image
    pBS->FreePool(mcfg);
    //Kill the Event
    pBS->CloseEvent(Event);
#endif

    return;
}

/**
    BASIC Chipset initialization function called after Initialization of generic part of the
    Host and Root Bridges. All Handles for PCIHostBrg and PciRootBrg has been created and
    Protocol Intergaces installed
    Need to update this function from Aptio core Version 4.6.2 onwards

        
    @param HostBrg0 

         
    @retval EFI_STATUS return EFI status

    @note  
      None

**/

EFI_STATUS
HbCspBasicChipsetInit(
    IN PCI_HOST_BRG_DATA       *HostBrg0)
{
    EFI_STATUS              Status;
    DXE_SERVICES            *dxe;   

    Status = LibGetDxeSvcTbl(&dxe);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;

    //Now for New Chipset which has PCI Express support we have to build 
    //MCFG Table to inform OS about PCIE Ext cfg space mapping
    Status = RegisterProtocolCallback(&gEfiAcpiTableProtocolGuid, CreateNbAcpiTables, NULL,
                                    &mAcpiEvent, &mAcpiReg);
    ASSERT_EFI_ERROR(Status);

    //If this protocol has been installed we can use it rigth on the way
    CreateNbAcpiTables(mAcpiEvent, NULL);

    return EFI_SUCCESS;
}


/**
    Chipset Specific function to Map Internal Device address
    residing ABOVE 4G to the BELOW 4G address space for DMA.
    MUST BE IMPLEMENTED if CHIPSET supports address space
    decoding ABOVE 4G.

        
    @param Root Bridge private structure
    @param Operation to provide Mapping for
    @param HostAddress of the Device
    @param Number of Byte in Mapped Buffer.
    @param Mapped Device Address.
    @param Mapping Info Structure this function must allocate and fill.

         
    @retval EFI_SUCCESS Successful.
    @retval UINTN Number of Byte in Mapped Buffer.
    @retval EFI_PHYSICAL_ADDRESS Mapped Device Address.
    @retval PCI_ROOT_BRIDGE_MAPPING Mapping Info Structure this function must allocate and fill.

**/

EFI_STATUS
RbCspIoPciMap(
    IN PCI_ROOT_BRG_DATA                            *RbData,
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_OPERATION    Operation,
    IN EFI_PHYSICAL_ADDRESS                         HostAddress,
    IN OUT UINTN                                    *NumberOfBytes,
    OUT EFI_PHYSICAL_ADDRESS                        *DeviceAddress,
    OUT VOID                                        **Mapping)
{
    
    EFI_STATUS  Status=EFI_SUCCESS;
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//Porting is required for chipsets that supports Decoding of the PCI Address Space ABOVE 4G.

//Any Additional Variables goes here

//LakePort DOES NOT support decoding of the PCI resources ABOVE 4G. 
//So return EFI_UNSUPPORTED;
    *Mapping=NULL;
    Status=EFI_UNSUPPORTED;

//for Chipsets which DOES support decoding of the PCI resources ABOVE 4G.
//here must be something like that. 
/*
    PCI_ROOT_BRIDGE_MAPPING *mapping;
//------------------------------
        //Common buffer operations can not be remapped because in such
        // operations the same buffer will be accessed by CPU and PCI hardware
        if(Operation==EfiPciOperationBusMasterCommonBuffer || Operation==EfiPciOperationBusMasterCommonBuffer64)
            return EFI_UNSUPPORTED;

        mapping =Malloc(sizeof(PCI_ROOT_BRIDGE_MAPPING));
        if(mapping==NULL) return EFI_OUT_OF_RESOURCES;
        
        mapping->Signature  = EFI_PCI_RB_MAPPING_SIGNATURE;
        mapping->Resrved    = 0;
        mapping->Operation  = Operation;
        mapping->NumPages   = EFI_SIZE_TO_PAGES(*NumberOfBytes);
        mapping->HostAddr   = HostAddress;
        mapping->DeviceAddr = 0x00000000ffffffff;

        Status = pBS->AllocatePages(AllocateMaxAddress,EfiBootServicesData,mapping->NumPages,&mapping->DeviceAddr);
        if (EFI_ERROR(Status))
        {
            pBS->FreePool(mapping);
            return Status;
        }
        *Mapping=(VOID*)mapping;

        //Here must be a way to copy context of HostDevice buffer to the Mapped one.
        //This code given as example only you might need to do some chipset programming to 
        //access PCI Address Space Above 4G

        if(Operation==EfiPciOperationBusMasterRead||Operation==EfiPciOperationBusMasterRead64)
            pBS->CopyMem( (VOID*)(UINTN)mapping->DeviceAddr,
                        (VOID*)(UINTN)mapping->HostAddr,
                        mapping->NumBytes);

        *DeviceAddress = mapping->DeviceAddr;
*/
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
    return Status;
}

/**
    Chipset Specific function to Unmap previousely Mapped
    buffer for DMA. 
    MUST BE IMPLEMENTED if CHIPSET supports address space 
    decoding ABOVE 4G.

        
    @param Root Bridge private structure
    @param Mapping Info Structure this function must free.

          
    @retval EFI_SUCCESS Successful
    @retval EFI_INVALID_PARAMETER Failure

**/

EFI_STATUS
RbCspIoPciUnmap(
    IN PCI_ROOT_BRG_DATA                            *RbData,
    OUT PCI_ROOT_BRIDGE_MAPPING                     *Mapping)
{
    EFI_STATUS  Status=EFI_SUCCESS;
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//

//Any Additional Variables goes here

//LakePort DOES NOT support decoding of the PCI resources ABOVE 4G. 
//and for corresponded Mapping call we will return NULL as Mapping
//So return EFI_UNSUPPORTED if we get back something different;
    if(Mapping!=NULL) return EFI_INVALID_PARAMETER;
    //for all other conditions we would return EFI_UNSUPPORTED.
    Status=EFI_UNSUPPORTED;

//for Chipsets which DOES support decoding of the PCI resources ABOVE 4G.
//And provides corresponded mapping for the host address 
//here must be something like that. 
/*
    if(Mapping->Signature!=EFI_PCI_RB_MAPPING_SIGNATURE)Status=EFI_INVALID_PARAMERTER;
    
    if(!EFI_ERROR(Status)){ 

        if (Mapping->Operation == EfiPciOperationBusMasterWrite || Mapping->Operation == EfiPciOperationBusMasterWrite64) 

        //Here must be a way to copy context of the Unmapped buffer to HostDevice.
        //This code given as example only you might need to do some chipset programming to 
        //access PCI Address Space Above 4G
        pBS->CopyMem((VOID*)(UINTN)Mapping->HostAddr,(VOID*)(UINTN)Mapping->DeviceAddr,Mapping->NumBytes);
            
        pBS->FreePages(Mapping->DeviceAddr, Mapping->NumPages);
        pBS->FreePool(Mapping);
    }
*/
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
    return Status;
}


//###<AAV> Core5.005 compliance ### #if (PCI_BUS_MAJOR_VER >= 2) && (PCI_BUS_MINOR_VER >=2)
// The following Structures and functions are used for
//  HYGON Northbridge Multi-Rootbridge Legacy PCI Express VGA Support
// Structures declaration starts
typedef struct _CONFIGURATION_MAP_REGISTER
{
    union {
        struct {
            UINT32  ReadEnable  :1;  // [0] RE: read enable
            UINT32  WriteEnable :1;  // [1] WE: write enable
            UINT32  DevCmpEn    :1;  // [2] device number compare mode enable
            UINT32  Reserved1   :1;  // [3] Reserved
            UINT32  DstNode     :3;  // [6:4] Destination node ID bits
            UINT32  Reserved2   :1;  // [7] Reserved
            UINT32  DstLink     :2;  // [9:8] Destination link ID
            UINT32  DstSubLink  :1;  // [10] Destination sublink
            UINT32  Reserved3   :5;  // [15:11] Reserved
            UINT32  BusNumBase  :8;  // [23:16] Bus number base bits
            UINT32  BusNumLimit :8;  // [31:24] bus number limit bits
        } Field;
        UINT32  AllBits;
    };
} CONFIGURATION_MAP_REGISTER;

typedef struct _DEVICE_VENDOR_ID_REGISTER
{
    union {
        struct {
            UINT32  VendorId    :16;    // [15:0] Vendor ID
            UINT32  DeviceId    :16;    // [31:16] Device ID
        } Field;
        UINT32  AllBits;
    };
} DEVICE_VENDOR_ID_REGISTER;

typedef struct _PCI64_ADDRESS
{
    union {
        struct {
            UINT64  Register    :8;     // [7:0] Register
            UINT64  Function    :8;     // [15:8] Function
            UINT64  Device      :8;     // [23:16] Device
            UINT64  Bus         :40;    // [63:24] Bus
        } Field;
        UINT64  AllBits;
    };
}PCI64_ADDRESS;
// Structures declaration ends

/**
    Pci Config Read         

        
    @param Root Bridge private structure
    @param Address 

         
    @retval UINT32 Contains data read from PCI register    
**/

// Functions support HYGON Northbridge Multi-Rootbridge Legacy PCI Express VGA
UINT32 PciConfigRead(
    PCI_ROOT_BRG_DATA       *RbData,
    PCI64_ADDRESS           Address )
{
    UINT32 PciValue;

    RbData->RbIoProtocol.Pci.Read(
            &RbData->RbIoProtocol,
            EfiPciWidthUint32,
            Address.AllBits,
            1,
            &PciValue );
            
    return PciValue;        //Return the read value
}

/**
    Pci Config Write

        
    @param Root Bridge private structure
    @param Address 
    @param PciValue 

         
    @retval VOID
**/

VOID PciConfigWrite(
    PCI_ROOT_BRG_DATA   *RbData,
    PCI64_ADDRESS       Address,
    UINT32              PciValue )
{
    RbData->RbIoProtocol.Pci.Write(
            &RbData->RbIoProtocol,
            EfiPciWidthUint32,
            Address.AllBits,
            1,
            &PciValue );
            
    return;     //Return the read value
}
//###<AAV> Core5.005 compliance ### #endif

/**
    Chipset Specific function to do PCI RB Attributes releated
    programming.

        
    @param Root Bridge private structure
    @param Attributes 
    @param ResourceBase 
    @param ResourceLength 

         
    @retval EFI_SUCCESS Successful.
    @retval UINT64 ResourceBase OPTIONAL,
    @retval UINT64 ResourceLength OPTIONAL

**/

EFI_STATUS
RbCspIoPciAttributes(
    IN PCI_ROOT_BRG_DATA *RbData,
    IN     UINT64   Attributes,
    IN OUT UINT64   *ResourceBase OPTIONAL,
    IN OUT UINT64   *ResourceLength OPTIONAL)
{
    EFI_STATUS  Status = EFI_SUCCESS;

    return Status;
}

/**
    Read Pci Registers into buffer.
    Csp Function which actualy access PCI Config Space
    Chipsets that capable of having PCI EXPRESS Ext Cfg Space transactions
    Must Implement this access routine here

        
    @param RbData Root Bridge private structure
    @param Width Memory Width
    @param Address PCI Address
    @param Count Number of width reads/writes.
    @param Buffer Buffer where read/written.
    @param Write Set if write.

         
    @retval EFI_SUCCESS Successful read.
    @retval VOID Buffer where read/written.

    @note  
    None

**/

EFI_STATUS
RootBridgeIoPciRW(
    IN PCI_ROOT_BRG_DATA                        *RbData,
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    Width,
    IN UINT64                                   Address,
    IN UINTN                                    Count,
    IN OUT VOID                                 *Buffer,
    IN BOOLEAN                                  Write)
{
    EFI_TPL     OldTpl;
    UINT8       IncrementValue      = 1 << (Width & 3); //1st 2 bits currently define width. Other bits define type.
    BOOLEAN     IsPCIe = FALSE;
    UINT64      PcieBaseAddr = PcdGet64 (PcdPciExpressBaseAddress);
    UINTN  PCIeAddress = PcieBaseAddr +
        (((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Bus      << 20 )  +
        (((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Device   << 15 )  +
        (((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Function << 12 )  +
        ((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->ExtendedRegister;
    UINT32  PciAddress = (1 << 31) +
        (((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Bus      << 16 )  +
        (((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Device   << 11 )  +
        (((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Function << 8  )  +
        (((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Register       );
    UINT8   PciDataByte = PciAddress & 3;
    UINT32  PciAligned;
    UINT16  PciDataReg;

    if ((PcieBaseAddr) && 
        (((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->ExtendedRegister) )
            IsPCIe = TRUE;

    // To read 64bit values, reduce Increment Value (by half) and
    // double the count value (by twice)
    if (IncrementValue > 4)
    {
        IncrementValue >>= 1;
        Count <<= 1;
    }

    if (Width >= EfiPciWidthMaximum || IncrementValue > 4) return EFI_INVALID_PARAMETER;

    while (Count--)
    {
        if (!IsPCIe)
        {
            PciAligned = PciAddress & ~3;           //Dword aligned.
            PciDataReg = 0xCFC + PciDataByte;       //0xcfc + {0,1,2,3} = reg + {0,1,2,3}
            OldTpl = pBS->RaiseTPL(TPL_HIGH_LEVEL);
            IoWrite32(0xCF8, PciAligned);
        }
        if (Write)
        {
            switch(IncrementValue)
            {
            case 1:
                if (IsPCIe) *(UINT8*)PCIeAddress = *(UINT8*)Buffer;
                else        IoWrite8(PciDataReg, *(UINT8*)Buffer);
                break;
            case 2:
                if (IsPCIe) *(UINT16*)PCIeAddress = *(UINT16*)Buffer;
                else        IoWrite16(PciDataReg, *(UINT16*)Buffer);
                break;
            default:
                if (IsPCIe) *(UINT32*)PCIeAddress = *(UINT32*)Buffer;
                else        IoWrite32(PciDataReg, *(UINT32*)Buffer);
                break;
            }
        }
        else
        {
            switch(IncrementValue)
            {
            case 1:
                *(UINT8*)Buffer = (IsPCIe) ? *(UINT8*)PCIeAddress : IoRead8(PciDataReg);
                break;
            case 2:
                *(UINT16*)Buffer = (IsPCIe) ? *(UINT16*)PCIeAddress : IoRead16(PciDataReg);
                break;
            default:
                *(UINT32*)Buffer = (IsPCIe) ? *(UINT32*)PCIeAddress : IoRead32(PciDataReg);
                break;
            }
        }

        if (!IsPCIe) pBS->RestoreTPL(OldTpl);

        if (Width <= EfiPciWidthFifoUint64)
        {
             Buffer = ((UINT8 *)Buffer + IncrementValue);
            //Buffer is increased for only EfiPciWidthUintxx and EfiPciWidthFifoUintxx
        }

        // Only increment the PCI address if Width is not a FIFO.
        if ((Width & 4) == 0)
        {
            if (IsPCIe) PCIeAddress += IncrementValue;
            else
            {
                PciAddress += IncrementValue;
                PciDataByte = PciAddress & 3;
            }
        }
    }
    return EFI_SUCCESS;
}

/****** DO NOT WRITE BELOW THIS LINE *******/


//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
