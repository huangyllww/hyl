#include "typedef.h"
#include "astiolib.h"
#include "memlib.h"
#include "pcilib.h"

//
// options
//
//#define RelocateIO		1
#define SoftwareTraining			0

//
// define
//
#define SCU_BASE		0x1e6e2000
#define I2C_BASE		0x1e78a000

//
// Struct
//
typedef struct _DP501INFO {
	
	UCHAR I2CAddress;				// DP501 I2C Address
    UCHAR ConnectStatus;			// connect/not connect	
    UCHAR SinkDeviceType;			// DP/DVI
     
} _DP501Info;	

typedef struct _DEVINFO {
	
    ULONG  PCIAddr;	
    USHORT usVendorID;
    USHORT usDeviceID;
    UCHAR  jRevision;

    UCHAR jDRAMType;
    ULONG ulMCLK;
    ULONG ulDRAMBusWidth;
        
    ULONG ulFBSize;
    ULONG ulFBBase;
  
    UCHAR *pjFBLinear;        
    
    ULONG ulMMIOSize;            
    ULONG ulMMIOBase;
     
    UCHAR *pjMMIOLinear;            

    USHORT usRelocateIOBase;

    UCHAR *pjA0000Linear;
    UCHAR *pjB0000Linear;
    UCHAR *pjC0000Linear;                                
} _DEVInfo;

//
// Export
//
/* ProtoType */
VOID udelay(ULONG us);
ULONG ReadDD_SOC3(_DEVInfo *DevInfo, ULONG addr);
VOID WriteDD_SOC3(_DEVInfo *DevInfo, ULONG addr, ULONG data);
USHORT ReadDW_SOC3(_DEVInfo *DevInfo, ULONG addr);
VOID WriteDW_SOC3(_DEVInfo *DevInfo, ULONG addr, USHORT data);
UCHAR ReadDB_SOC3(_DEVInfo *DevInfo, ULONG addr);
VOID WriteDB_SOC3(_DEVInfo *DevInfo, ULONG addr, UCHAR data);
VOID SetI2CReg(_DEVInfo *DevInfo, UCHAR jChannel, UCHAR jDevAddr, UCHAR jIndex, UCHAR jData);
UCHAR GetI2CReg(_DEVInfo *DevInfo, UCHAR jChannel, UCHAR jDevAddr, UCHAR jIndex);
BOOL GetDevInfo(EFI_PEI_SERVICES **PeiServices, USHORT usVendorID, USHORT usDeviceID, _DEVInfo *DevInfo);

