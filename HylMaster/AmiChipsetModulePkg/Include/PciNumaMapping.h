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

/** @file PciNumaMapping.h
    This file defines the PciNumaNodeMappingProtocol for determining
    numa node mappings/proximity domains of PCI root bridges

**/
#ifndef __PCI_NUMA_MAPPING__H__
#define __PCI_NUMA_MAPPING__H__

typedef struct _PCI_NUMA_NODE_MAPPING_PROTOCOL PCI_NUMA_NODE_MAPPING_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *GET_PCI_RB_NUMA_NODE_DOMAIN) (
        IN PCI_NUMA_NODE_MAPPING_PROTOCOL   *This,
        IN UINTN                            BusRangeBase,
        OUT UINTN                           *SocketNumber,
        OUT UINTN                           *DieNumber,
        OUT UINT32                          *Domain
);

typedef struct _PCI_NUMA_MAPPING {
    UINTN   Die;
    UINTN   Socket;
    UINTN   BusRangeBase;
    UINT32  Domain;
} PCI_NUMA_MAPPING;

struct _PCI_NUMA_NODE_MAPPING_PROTOCOL {
    GET_PCI_RB_NUMA_NODE_DOMAIN GetPciRbNumaNodeDomain;
    PCI_NUMA_MAPPING         *PciRbNodeMap;
};



extern EFI_GUID gPciNumaNodeMappingProtocolGuid;


EFI_STATUS GetPciRbNumaNodeDomain(
        IN PCI_NUMA_NODE_MAPPING_PROTOCOL   *This,
        IN UINTN                            BusRangeBase,
        OUT UINTN                           *SocketNumber,
        OUT UINTN                           *DieNumber,
        OUT UINT32                          *Domain);

#endif
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
