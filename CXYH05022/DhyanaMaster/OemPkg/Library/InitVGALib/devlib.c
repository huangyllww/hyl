#include "devlib.h"
#include "vgahw.h"

#if defined (__386__)
#include "dos32.c"
#endif

//
// delay
//
VOID udelay(ULONG us)
{
    ULONG count;
	
    count = (us + 14) / 15;
    do {
        IoRead8(0x61);
        count--;
    } while(count); 
}
	
//
// MEM Access
//
ULONG ReadDD_SOC3(_DEVInfo *DevInfo, ULONG addr)
{
 *(ULONG *) (DevInfo->pjMMIOLinear + 0xF004) = (ULONG) ((addr) & 0xFFFF0000);
 *(ULONG *) (DevInfo->pjMMIOLinear + 0xF000) = (ULONG) (0x00000001);
 return ( *(volatile ULONG *) (DevInfo->pjMMIOLinear + 0x10000 + ((addr) & 0x0000FFFF)) );
}

VOID WriteDD_SOC3(_DEVInfo *DevInfo, ULONG addr, ULONG data)
{						
 *(ULONG *) (DevInfo->pjMMIOLinear + 0xF004) = (ULONG) ((addr) & 0xFFFF0000);	
 *(ULONG *) (DevInfo->pjMMIOLinear + 0xF000) = (ULONG) 0x00000001;	
 *(ULONG *) (DevInfo->pjMMIOLinear + 0x10000 + ((addr) & 0x0000FFFF)) = data;	
}

USHORT ReadDW_SOC3(_DEVInfo *DevInfo, ULONG addr)
{
 *(ULONG *) (DevInfo->pjMMIOLinear + 0xF004) = (ULONG) ((addr) & 0xFFFF0000);
 *(ULONG *) (DevInfo->pjMMIOLinear + 0xF000) = (ULONG) (0x00000001);
 return ( *(volatile USHORT *) (DevInfo->pjMMIOLinear + 0x10000 + ((addr) & 0x0000FFFF)) );
}

VOID WriteDW_SOC3(_DEVInfo *DevInfo, ULONG addr, USHORT data)
{						
 *(ULONG *) (DevInfo->pjMMIOLinear + 0xF004) = (ULONG) ((addr) & 0xFFFF0000);	
 *(ULONG *) (DevInfo->pjMMIOLinear + 0xF000) = (ULONG) 0x00000001;	
 *(USHORT *) (DevInfo->pjMMIOLinear + 0x10000 + ((addr) & 0x0000FFFF)) = data;	
}

UCHAR ReadDB_SOC3(_DEVInfo *DevInfo, ULONG addr)
{
 *(ULONG *) (DevInfo->pjMMIOLinear + 0xF004) = (ULONG) ((addr) & 0xFFFF0000);
 *(ULONG *) (DevInfo->pjMMIOLinear + 0xF000) = (ULONG) (0x00000001);
 return ( *(volatile UCHAR *) (DevInfo->pjMMIOLinear + 0x10000 + ((addr) & 0x0000FFFF)) );
}

VOID WriteDB_SOC3(_DEVInfo *DevInfo, ULONG addr, UCHAR data)
{						
 *(ULONG *) (DevInfo->pjMMIOLinear + 0xF004) = (ULONG) ((addr) & 0xFFFF0000);	
 *(ULONG *) (DevInfo->pjMMIOLinear + 0xF000) = (ULONG) 0x00000001;	
 *(UCHAR *) (DevInfo->pjMMIOLinear + 0x10000 + ((addr) & 0x0000FFFF)) = data;	
}

//
// I2C Access
//
VOID SetI2CReg(_DEVInfo *DevInfo, UCHAR jChannel, UCHAR jDevAddr, UCHAR jIndex, UCHAR jData )
{
    ULONG ulI2CRegBase = I2C_BASE + 0x40 * jChannel;
    ULONG retry = 0;
    ULONG ulData;
    
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x00, 0x0);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x04, 0x77743355);
    //WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x04, 0x77776704);    
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x08, 0x0);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x10, 0xffffffff);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x00, 0x1);
    //WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x0C, 0xAF);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x20, (ULONG) jDevAddr);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x14, 0x03);
    retry = 0;
    do {
        ulData = ReadDD_SOC3(DevInfo, ulI2CRegBase + 0x10);
        udelay(15);
        if (retry++ > 1000)
            goto Exit_SetI2CReg;
    } while (!(ulData & 0x01));
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x10, 0xffffffff);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x20, (ULONG) jIndex);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x14, 0x02);
    do {
        ulData = ReadDD_SOC3(DevInfo, ulI2CRegBase + 0x10);
    } while (!(ulData & 0x01)); 
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x10, 0xffffffff);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x20, (ULONG) jData);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x14, 0x02);
    do {
        ulData = ReadDD_SOC3(DevInfo, ulI2CRegBase + 0x10);
    } while (!(ulData & 0x01)); 
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x10, 0xffffffff);
    //ulData = ReadDD_SOC3(DevInfo, ulI2CRegBase + 0x0C);    
    //WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x0C, ulData | 0x10);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x14, 0x20);
    do {
        ulData = ReadDD_SOC3(DevInfo, ulI2CRegBase + 0x10);
    } while (!(ulData & 0x10));
    //ulData = ReadDD_SOC3(DevInfo, ulI2CRegBase + 0x0C);        
    //WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x0C, ulData & 0xffffffef);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x10, 0xffffffff);
    udelay(10);
    
Exit_SetI2CReg:
    ;
}

UCHAR GetI2CReg(_DEVInfo *DevInfo, UCHAR jChannel, UCHAR jDevAddr, UCHAR jIndex)
{
    ULONG ulI2CRegBase = I2C_BASE + 0x40 * jChannel;
    ULONG retry = 0;
    ULONG ulData;
    UCHAR jData;
    
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x00, 0x0);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x04, 0x77743355);
    //WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x04, 0x77776704);        
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x08, 0x0);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x10, 0xffffffff);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x00, 0x1);
    //WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x0C, 0xAF);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x20, (ULONG) jDevAddr);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x14, 0x03);
    retry = 0;
    do {
        ulData = ReadDD_SOC3(DevInfo, ulI2CRegBase + 0x10);
        udelay(15);
        if (retry++ > 1000)
            return 0;
    } while (!(ulData & 0x01));
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x10, 0xffffffff);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x20, (ULONG) jIndex);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x14, 0x02);
    do {
        ulData = ReadDD_SOC3(DevInfo, ulI2CRegBase + 0x10);
    } while (!(ulData & 0x01));  
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x10, 0xffffffff);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x20, (ULONG) jDevAddr + 1);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x14, 0x1B);
    do {
        ulData = ReadDD_SOC3(DevInfo, ulI2CRegBase + 0x10);
    } while (!(ulData & 0x04));     
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x10, 0xffffffff);
    //ulData = ReadDD_SOC3(DevInfo, ulI2CRegBase + 0x0C);    
    //WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x0C, ulData | 0x10);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x14, 0x20);
    do {
        ulData = ReadDD_SOC3(DevInfo, ulI2CRegBase + 0x10);
    } while (!(ulData & 0x10));
    //ulData = ReadDD_SOC3(DevInfo, ulI2CRegBase + 0x0C);        
    //WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x0C, ulData & 0xffffffef);
    WriteDD_SOC3(DevInfo, ulI2CRegBase + 0x10, 0xffffffff);
    ulData = ReadDD_SOC3(DevInfo, ulI2CRegBase + 0x20);        
    jData = (UCHAR) ((ulData & 0xFF00) >> 8);
    
    return (jData);	
}

//
// DevInfo
//
BOOL GetDevInfo(EFI_PEI_SERVICES **PeiServices, USHORT usVendorID, USHORT usDeviceID, _DEVInfo *DevInfo)
{
    ULONG PCIAddr;
    
    {
        PCIAddr = FindPCIDevice(PeiServices, usVendorID, usDeviceID);	

        if (PCIAddr)
        {
    	    DevInfo->PCIAddr = PCIAddr;
            DevInfo->usVendorID = usVendorID;
            DevInfo->usDeviceID = usDeviceID;
            DevInfo->jRevision = (UCHAR) GetPCIReg(PCIAddr+0x08, 0x000000FF);
            
            SetPCIReg (PCIAddr + 0x04, 0xFFFFFFFF, 0x03);	//enable MEM & IO
                        
            DevInfo->usRelocateIOBase = (USHORT) GetPCIReg(PCIAddr+0x18, 0x0000FFFC);
               
            DevInfo->ulMMIOBase = GetPCIReg(PCIAddr+0x14, 0xFFFFFFF0);
            DevInfo->ulMMIOSize = 0x20000;			//default:128KB
     
//            DevInfo->pjMMIOLinear  = (UCHAR *) MapPhysicalToLinear (DevInfo->ulMMIOBase,DevInfo->ulMMIOSize);     
			DevInfo->pjMMIOLinear  = (UCHAR *)DevInfo->ulMMIOBase; 			

            //Get default info
            InitVGA_HW(PeiServices, DevInfo, Init_IO);		//enable vga io access
            GetVRAMInfo(DevInfo);
            GetDRAMInfo(DevInfo);
                        
            DevInfo->ulFBBase = GetPCIReg(PCIAddr+0x10, 0xFFFFFFF0);
            //DevInfo->ulFBSize = 8 * 1024 * 1024;		//default:8MB
     
//            DevInfo->pjFBLinear  = (UCHAR *) MapPhysicalToLinear (DevInfo->ulFBBase,DevInfo->ulFBSize);      
            DevInfo->pjFBLinear =  (UCHAR *)DevInfo->ulFBBase;                   
            return 1;
        }
    	
    }  	
    
    return 0;		
}
