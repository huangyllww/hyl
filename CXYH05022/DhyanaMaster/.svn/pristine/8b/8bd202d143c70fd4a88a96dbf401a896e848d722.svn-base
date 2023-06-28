#include "typedef.h"
#include "pcilib.h"

#define AST2300_VGA_BAR0		0xDE000008;
#define AST2300_VGA_BAR1		0xDFB00000;
#define AST2300_VGA_BAR2		0x5001;			//IO

#define AST2300_BRG_RES0		0xDFB0DFB0;
#define AST2300_BRG_RES1		0xDEF1DE01;		//MMIO
#define AST2300_BRG_RES2		0x5151

//
// Basic API
//
ULONG FindPCIDevice (EFI_PEI_SERVICES **PeiServices, USHORT usVendorID, USHORT usDeviceID)
{
    ULONG i;
//Tangxj2_20190123 fix issue that can not get full pci config in Rome platform ,add fun number checking in loop ++   
//    for (i=0x0; i<256*32*0x1000; i+=0x1000)
    for (i=0x0; i<256*32*8*0x1000; i+=0x1000)
//Tangxj2_20190123 fix issue that can not get full pci config in Rome platform ,add fun number checking in loop  --	
    {
    	if (((USHORT) GetPCIReg(i, 0x0000FFFF) == usVendorID) && (((USHORT) (GetPCIReg(i, 0xFFFF0000) >> 16)) == usDeviceID))
    	{
#if 0
    	    {
    	    	ULONG j;
    			DEBUG((0xffffffff, "Dumping PCIE Configuration\n"));
    	    	for(j = 0; j<256; j+=4)
    	    	{
    	    		if(j%16 == 0)
    	    			DEBUG((0xffffffff, "\n"));
    					DEBUG((0xffffffff, "%08x ", GetPCIReg(i + j, 0xFFFFFFFF)));
    	    	}
    	    }
#endif
    	    return i;
    	}
    }
    
    return (0);
}

VOID SetPCIReg (ULONG ulPCIAddr, ULONG ulMask, ULONG ulData)
{
	ULONG ulTemp;
	
	ulTemp = PciRead32(ulPCIAddr);
	ulTemp = ulTemp & ulMask | ulData;
	PciWrite32(ulPCIAddr, ulTemp);
}


ULONG GetPCIReg (ULONG ulPCIAddr, ULONG ulMask)
{
	ULONG ulTemp;
	ulTemp = PciRead32(ulPCIAddr);
	ulTemp &= ulMask;
	return ulTemp;
}
