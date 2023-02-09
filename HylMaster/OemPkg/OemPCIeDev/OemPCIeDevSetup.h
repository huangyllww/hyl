#ifndef __PCIE_DEV_SETUP_H__
#define __PCIE_DEV_SETUP_H__

#include <Efi.h>
#include <UefiHii.h>
#include <AmiDxeLib.h>
#include <Setup.h> //AMI Variable
#include <Token.h> //Token

#include <Library/HiiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Protocol/DiskInfo.h>
#include "Nvme/NvmeBus.h"
#include <Protocol/HiiString.h>
#include <PciBus.h>
#include <Guid/MdeModuleHii.h>


#ifndef EFI_HANDLE_SIGNATURE
#define EFI_HANDLE_SIGNATURE            SIGNATURE_32('h','n','d','l')
///
/// IHANDLE - contains a list of protocol handles
///
typedef struct {
  UINTN               Signature;
  /// All handles list of IHANDLE
  LIST_ENTRY          AllHandles;
  /// List of PROTOCOL_INTERFACE's for this handle
  LIST_ENTRY          Protocols;
  UINTN               LocateRequest;
  /// The Handle Database Key value when this handle was last created or modified
  UINT64              Key;
} IHANDLE;

#define PROTOCOL_ENTRY_SIGNATURE        SIGNATURE_32('p','r','t','e')

///
/// PROTOCOL_ENTRY - each different protocol has 1 entry in the protocol
/// database.  Each handler that supports this protocol is listed, along
/// with a list of registered notifies.
///
typedef struct {
  UINTN               Signature;
  /// Link Entry inserted to mProtocolDatabase
  LIST_ENTRY          AllEntries;
  /// ID of the protocol
  EFI_GUID            ProtocolID;
  /// All protocol interfaces
  LIST_ENTRY          Protocols;
  /// Registerd notification handlers
  LIST_ENTRY          Notify;
} PROTOCOL_ENTRY;

#define PROTOCOL_INTERFACE_SIGNATURE  SIGNATURE_32('p','i','f','c')

///
/// PROTOCOL_INTERFACE - each protocol installed on a handle is tracked
/// with a protocol interface structure
///
typedef struct {
  UINTN                       Signature;
  /// Link on IHANDLE.Protocols
  LIST_ENTRY                  Link;
  /// Back pointer
  IHANDLE                     *Handle;
  /// Link on PROTOCOL_ENTRY.Protocols
  LIST_ENTRY                  ByProtocol;
  /// The protocol ID
  PROTOCOL_ENTRY              *Protocol;
  /// The interface value
  VOID                        *Interface;
  /// OPEN_PROTOCOL_DATA list
  LIST_ENTRY                  OpenList;
  UINTN                       OpenListCount;

} PROTOCOL_INTERFACE;

#define OPEN_PROTOCOL_DATA_SIGNATURE  SIGNATURE_32('p','o','d','l')

typedef struct {
  UINTN                       Signature;
  ///Link on PROTOCOL_INTERFACE.OpenList
  LIST_ENTRY                  Link;      

  EFI_HANDLE                  AgentHandle;
  EFI_HANDLE                  ControllerHandle;
  UINT32                      Attributes;
  UINT32                      OpenCount;
} OPEN_PROTOCOL_DATA;
#endif

#define PCI_DEV_DYNAMIC_ITEM_BASE_KEY (PCI_DEV_DYNAMIC_ITEM_AUTO_BASE_KEY+0x100)

typedef struct _OPTION_ITEM_VALUE {
    EFI_STRING_ID  StrId;
    UINT8          Value;
} OPTION_ITEM_VALUE;

#endif
