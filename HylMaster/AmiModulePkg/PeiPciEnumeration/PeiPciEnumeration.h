//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file PeiPciEnumeration.h
    This file contains the structures definitions required for PeiPciEnumeration
**/
#ifndef __PEI_PCI_ENUMERATION__H__
#define __PEI_PCI_ENUMERATION__H__
#ifdef __cplusplus
extern "C" {
#endif

#include  <Token.h>
#include  <Base.h>
#include  <Library/BaseLib.h>
#include  <Library/PeiServicesLib.h>
#include  <Library/PcdLib.h>
#include  <Library/HobLib.h>
#include  <Library/DebugLib.h>
#include  <Library/MemoryAllocationLib.h>
#include  <Ppi/PciCfg.h>
#include  <Ppi/AmiPeiPciEnumeration.h>
#include  <PeiPciEnumerationElink.h>
#include  <IndustryStandard/Pci.h>
//<Machong-20170719-try to make it work in CAR+>
#include <Library/PeiServicesLib.h>
#include <Library/PeiServicesTablePointerLib.h>
//<Machong-20170719-try to make it work in CAR->

// PCI 2 PCI Bridge Config
#define PCI_IOBASE                      0x001C  // I/O base Register
#define PCI_IOLIMIT                     0x001D  // I/O Limit Register
#define PCI_MEMBASE                     0x0020  // Memory Base Register
#define PCI_MEMLIMIT                    0x0022  // Memory Limit Register
#define DWORD_BAR_MASK                  0xFFFFFFFF
#define P2P_BRG_MEM_GRA                 0xFFFFF //naturally, P2P Brg MMIO aligned to the 1M boundary
#define P2P_BRG_IO_GRA                  0xFFF   //naturally, P2P Brg IO aligned to the 4k boundary

#ifndef PCIE_CFG_ADDRESS
#define PCIE_CFG_ADDRESS(bus, dev, func, reg) \
    ((UINTN)(PcdGet64(PcdPciExpressBaseAddress) + \
    ((bus) << 20) + ((dev) << 15) + \
    ((func) << 12) + reg))
#endif

#if PEI_PCI_DEBUG_MESSAGES
#define PEI_PCI_DEBUG(Arguments)  DEBUG(Arguments);
#else
#define PEI_PCI_DEBUG(Arguments)
#endif

typedef struct _PCI_DEVICE_CLASSCODE_ENTRY{
    UINT8    BaseClassCode;
    UINT8    SubClassCode;
    UINT8    ProgrammingInterface;
    UINT8    BarIndex;
} PCI_DEVICE_CLASSCODE_ENTRY;

typedef struct _PEI_PCI_BRIDGE_ENTRY{
    UINT8    Bus;
    UINT8    Device;
    UINT8    Function;
} PEI_PCI_BRIDGE_ENTRY;

typedef VOID (*INIT_FUNCTION)(EFI_PHYSICAL_ADDRESS);

typedef struct {
    UINT16                      VendorID;
    UINT16                      DeviceID;
    INIT_FUNCTION               InitFunction;
}PCI_CONTROLLER_INFO;

//<Chendl001-20170925 VGA FrameBuffer Align on 16MB +>
typedef VOID (*ALIGN_INIT_FUNCTION)(UINT64*);
typedef struct {
    UINT16                      VendorID;
    UINT16                      DeviceID;
    ALIGN_INIT_FUNCTION         InitFunction;
}PCI_CONTROLLER_ALIGN_INFO;
//<Chendl001-20170925 VGA FrameBuffer Align on 16MB ->

/**
    This ENUM definition used to identify PCI Device different BAR types

    @note  Don't cange the order! Some PCI Bus internal routines rely on it!

 Fields:        
    Name            Type                    Description
   ------------------------------------------------------------------
    tPciUnknown     ENUM            PCI Bus Driver wasn't able to Identify the Device
    tPciRootBrg     ENUM            Device is a PCI Root Bridge Device (A virtual device the root of PCI tree)
    tPci2PciBrg     ENUM            Device is a PCI to PCI Bridge Device
    tUncompatibleDevice ENUM        Device is an Uncompatible PCI Device
    tPciMaxDevice   ENUM            Last valid value of this ENUM 
**/
typedef enum {
    tPciUnknown=0,
    tPciRootBrg,    //1
    tPci2PciBrg,    //2
    tPciDevice,     //3
    tUncompatibleDevice, //4
    tPciMaxDevice   //5
} PEI_PCI_DEVICE_TYPE;

/**
    This ENUM value used to identify PCI Device different BAR types.

 Fields:        
    Name            Type                    Description
   ------------------------------------------------------------------
    tBarUnused      ENUM    BAR is not used to Specify Device Resources(empty)
    tBarMmio32      ENUM    BAR is used to Specify Device's MMIO 32bit Resources
    tBarMmio64      ENUM    BAR is used to Specify Device's MMIO 64bit Resources
    tBarIo          ENUM    BAR is used to Specify Device's IO Resources.
    tBarMem         ENUM    A superset of PCI_BAR_TYPE tBarMmio32 and tBarMmio64
    tBarMaxType     ENUM    Last valid value of this ENUM 
**/
typedef enum {
    tBarUnused=0,
    tBarIo16,       //1
    tBarIo32,       //2
    tBarMmio32,     //3
    tBarMmio64,     //4
    tBarIo,         //5
    tBarMem,        //6
    tBarMaxType     //7
} PEI_PCI_BAR_TYPE;

#pragma pack(1)
/**
    This Structure used to store information about PCI Device 
    Base Address Register (BAR)

 Fields:        
    Name            Type                    Description
   ------------------------------------------------------------------
    Type            PCI_BAR_TYPE        Type of PCI Device BAR may be converted
    Base            UINT64              Base Address of the Resource assigned to this BAR
    Length          UINT64              Length of the Resource assigned to this BAR in bytes
    Gran            UINT64              Granularity of the Resource assigned to this BAR - a value that was read back after all ONEs was applied to the BAR.
    Offset          UINT8               Offset of the PCI Configuration Space where this bar Located 
**/
typedef struct PEI_PCI_BAR {
    PEI_PCI_BAR_TYPE          Type;
    UINT64                    Base;
    UINT64                    Length;
    UINT64                    Gran;
    UINT8                     Offset; //offset of this bar register in PCI_CFG space 
} PEI_PCI_BAR;

/**
 * This Structure will contain information about the discovered
 * Pci Root Bridge, Pci2Pci Bridge or Pci Devices.
 */
typedef struct _PEI_PCI_DEV_INFO {
    PEI_PCI_DEVICE_TYPE             Type;       ///Identifies the device
    UINT8                           Bus;
    UINT8                           Device;
    UINT8                           Function;            
    BOOLEAN                         SkipDevice;
    struct _PEI_PCI_DEV_INFO        *ParentBridge;
    PEI_PCI_BAR                     Bar;
    BOOLEAN                         IsBarProgrammed;
    AMI_PEI_PCI_CLASSCODE           PciDeviceClass;
} PEI_PCI_DEV_INFO;

/**
 * This Structure will contain Additional or Extended information about the discovered
 * for a PCI Root Bridge, Pci2Pci Bridge. This will be always followed by the corresponding
 * PCI_DEVICE_INFO structure of the PCI Root Bridge or Pci2Pci Bridge.
 */
typedef struct {
    UINT8                  PrimaryBusNumber;
    UINT8                  SecondaryBusNumber;
    UINT8                  SubordinateBusNumber;
    BOOLEAN                BridgeProgrammed;
    UINT8                  IsMMIO;
    UINT32                 IoBase;
    UINT64                 MmioBase;
    UINT32                 IoBaseLength;
    UINT64                 MmioBaseLength;
    UINT32                 IoGranularity;
    UINT64                 MmioGranularity;
    UINT8                  ChildDeviceCount;
} PEI_PCI_BRIDGE_INFO_EXT;

/**
 * This Structure will contain the Private Data for Pei Pci Enumeration.
 * 
 */

typedef struct {
    AMI_PEI_PCI_ENUMERATION_PPI             PeiPciEnumerationPpi;
    BOOLEAN                                 HaveEnumeratedDevices;
    UINTN                                   PciDeviceCount;
    PEI_PCI_DEV_INFO                        *PciDeviceInfo[PEI_MAXIMUM_PCI_DEVICE];
    BOOLEAN                                 DeviceResetDone;
    //<Machong-20170719-Move From Global Variable Definition+>
    UINT64                                  MmioBaseAddress;
    UINT32                                  IoBaseAddress;
    BOOLEAN                                 AssignMmioTopDown;
    UINT8                                   LastUsedBusNumber;
    UINT8                                   SubBusNoForRootBridge;
    //<Machong-20170719-Move From Global Variable Definition->
} PEI_PCI_ENUMERATION_PRIVATE_DATA;

#pragma pack()

EFI_STATUS
EFIAPI
InitializePciDevices (
    IN  AMI_PEI_PCI_ENUMERATION_PPI     *This
);

EFI_STATUS
EFIAPI
GetNextPciDeviceByClassCode (
    IN  AMI_PEI_PCI_ENUMERATION_PPI             *This,
    IN  AMI_PEI_PCI_CLASSCODE                   PciClassCode,
    IN  UINT8                                   DevInstance,
    IN  OUT AMI_PEI_PCI_DEVICE_LOCATION         *PciDevice
);

EFI_STATUS
EFIAPI
ResetPciDeviceResources (
    IN  AMI_PEI_PCI_ENUMERATION_PPI     *This
);

EFI_STATUS 
EnumerateBus(
    PEI_PCI_ENUMERATION_PRIVATE_DATA      *PeiPciPrivateData,
    PEI_PCI_DEV_INFO                      *ParentBridge
);

VOID
PcdGetSetPciBridgeResources(
    PEI_PCI_ENUMERATION_PRIVATE_DATA      *PeiPciPrivateData,    
    BOOLEAN         Flag
);

VOID
FindPciMmioAddress(
    UINT64          *PciMmioStartAddress
);

BOOLEAN
IsAllowedClassCodeDevice (
    IN  AMI_PEI_PCI_CLASSCODE               PciClassCode, 
    IN OUT UINT8                               *BarOffset
);

EFI_STATUS 
QueryPciDeviceResources(
    PEI_PCI_DEV_INFO                    *ParentBridge
);

EFI_STATUS 
CalculateBridgeResources(
    PEI_PCI_ENUMERATION_PRIVATE_DATA    *PeiPciPrivateData,
    PEI_PCI_DEV_INFO                    *ParentBridge
);

EFI_STATUS 
AssignBridgeResources(
    PEI_PCI_ENUMERATION_PRIVATE_DATA    *PeiPciPrivateData,
    PEI_PCI_DEV_INFO                    *ParentBridge
);

EFI_STATUS 
GetNextMaxGranDevUnderBridge(
    PEI_PCI_ENUMERATION_PRIVATE_DATA    *PeiPciPrivateData,
    PEI_PCI_DEV_INFO                    *ParentBridge,
    PEI_PCI_BAR_TYPE                    ResourceType,
    UINT64                              *MaxGranularity,
    PEI_PCI_DEV_INFO                    **MaxGranPciDeviceInfo
); 

EFI_STATUS 
AssignDeviceResources(
    PEI_PCI_ENUMERATION_PRIVATE_DATA    *PeiPciPrivateData,
    PEI_PCI_DEV_INFO                    *ParentBridge,
    PEI_PCI_BAR_TYPE                    ResourceType
);

EFI_STATUS
EFIAPI 
PeiPciNotifyOnEndOfPei (
    IN  EFI_PEI_SERVICES          **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
    IN  VOID                      *InvokePpi
);

VOID 
CheckPreEnumerationInitFunctionHook (
    UINT8     Bus, 
    UINT8     Device, 
    UINT8     Function,
    UINT16    VendorId
);

VOID 
FindChildBridgesLength (
    PEI_PCI_ENUMERATION_PRIVATE_DATA     *PeiPciPrivateData,
    PEI_PCI_DEV_INFO                     *ParentBridge,
    UINT32                               *ChildBridgeIoLength, 
    UINT64                               *ChildBridgeMmioLength
);

//<Chendl001-20170925 VGA FrameBuffer Align on 16MB +>
VOID
VgaMemoryAlignFuncHook(
    UINT8     Bus, 
    UINT8     Device, 
    UINT8     Function,
    UINT64    *MMIOBaseAddress
);
//<Chendl001-20170925 VGA FrameBuffer Align on 16MB ->
     
/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
