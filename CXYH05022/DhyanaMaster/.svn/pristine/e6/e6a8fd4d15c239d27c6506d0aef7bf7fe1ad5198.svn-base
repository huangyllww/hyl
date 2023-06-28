//
// Struct
//
typedef struct {
    USHORT usVendorID;
    USHORT usDeviceID;	
} _PCI_DEVICE;

//
// Basic API
//
ULONG FindPCIDevice (EFI_PEI_SERVICES **PeiServices, USHORT, USHORT);
VOID SetPCIReg (ULONG, ULONG, ULONG);
ULONG GetPCIReg (ULONG, ULONG);
