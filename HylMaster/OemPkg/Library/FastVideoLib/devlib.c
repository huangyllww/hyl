#include "devlib.h"
#include "vgahw.h"

//
// DevInfo
//
BOOL FVGetDevInfo(USHORT usVendorID, USHORT usDeviceID, _DEVInfo *DevInfo)
{
    ULONG PCIAddr;
    
    {
        PCIAddr = FindPCIDevice(usVendorID, usDeviceID); 

        if (PCIAddr)
        {
          DevInfo->PCIAddr = PCIAddr;
          DevInfo->usVendorID = usVendorID;
          DevInfo->usDeviceID = usDeviceID;
          DevInfo->jRevision = (UCHAR) GetPCIReg(PCIAddr+0x08, 0x000000FF);
          
          SetPCIReg (PCIAddr + 0x04, 0xFFFFFFFF, 0x03); //enable MEM & IO
                      
          DevInfo->usRelocateIOBase = (USHORT) GetPCIReg(PCIAddr+0x18, 0x0000FFFC);
             
          DevInfo->ulMMIOBase = GetPCIReg(PCIAddr+0x14, 0xFFFFFFF0);
          DevInfo->ulMMIOSize = 0x20000;      //default:128KB    
          DevInfo->pjMMIOLinear  = (UCHAR *)DevInfo->ulMMIOBase;      
  
          //Get default info
          FVGetVRAMInfo(DevInfo);
          FVGetDRAMInfo(DevInfo);
                      
          DevInfo->ulFBBase = GetPCIReg(PCIAddr+0x10, 0xFFFFFFF0);     
          DevInfo->pjFBLinear =  (UCHAR *)DevInfo->ulFBBase;                   
          return 1;
        }
      
    }   
    
    return 0;   
}
