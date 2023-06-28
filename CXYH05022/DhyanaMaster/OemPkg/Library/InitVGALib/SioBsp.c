//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
//
//*************************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//*************************************************************************
// Revision History
// ----------------
// $Log: $
// 
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:  <SioBsp.C>
//
// Description: Oem prting for PEI phase
//
//
//<AMI_FHDR_END>
//*************************************************************************
#include <Pei.h>
#include <Token.h>
//#include <AmiLib.h>
//#include <AmiCspLib.h>
#include <Library\IoLib.h>
#include "Devlib.h" //<Lizhq-20170807 Get dynamic MMIO base for fast video frame buffer.(Mantis:15241)>

#ifndef	_EFI_MMIO_ACCESS_H_
#define	_EFI_MMIO_ACCESS_H_

#define MmioAddress(BaseAddr, Register) \
	( (UINTN)BaseAddr + (UINTN)(Register) )

// 32-bit
#define Mmio32Ptr(BaseAddr, Register) \
	( (volatile UINT32 *)MmioAddress(BaseAddr, Register) )

#define Mmio32(BaseAddr, Register) \
	*Mmio32Ptr(BaseAddr, Register)

// 16-bit
#define Mmio16Ptr(BaseAddr, Register) \
	( (volatile UINT16 *)MmioAddress(BaseAddr, Register) )

#define Mmio16(BaseAddr, Register) \
	*Mmio16Ptr(BaseAddr, Register)

// 8-bit
#define Mmio8Ptr(BaseAddr, Register) \
	( (volatile UINT8 *)MmioAddress(BaseAddr, Register) )

#define Mmio8(BaseAddr, Register) \
	*Mmio8Ptr(BaseAddr, Register)

#endif

VOID      AST_Set_MMIOBase_PEI(OUT UINT32 *ASTBaseAdd);
VOID      AST_Clear_MMIOBase_PEI();
VOID      AST_SET_SUC(IN UINT32 ASTBaseAdd);
//<Lizhq-20170807 Get dynamic MMIO base for fast video frame buffer.(Mantis:15241) +>
VOID      AST_Set_PCIE_BUS_NUMBER(_DEVInfo* pDevInfo);
//<Lizhq-20170807 Get dynamic MMIO base for fast video frame buffer.(Mantis:15241) ->
VOID      AST_Clear_PCIE_BUS_NUMBER();

VOID static WritePciReg32 (
    IN UINT32       dNumOfBusDevFunc,
    IN UINT16       Reg,
    IN UINT32       Value32 );

UINT32 static ReadPciReg32 (
    IN UINT32       dNumOfBusDevFunc,
    IN UINT16       Reg );

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: AST2300_P2BspInit
//
// Description:
//
// This function Step through table and initialize the Logic Device
//
// Input:     None
//
// Output:    EFI_STATUS
//
// Modified:  Nothing
//
// Referrals: None
//
// Note:
//------------------------------------------------------------------------- 
// <AMI_PHDR_END> 
//<Lizhq-20170807 Get dynamic MMIO base for fast video frame buffer.(Mantis:15241) +>
VOID AST2300_PEInit(_DEVInfo* pDevInfo)
{
	UINT32  ASTBaseAdd = pDevInfo->ulFBBase;
//  Init the bridge and PCI device.
    AST_Set_PCIE_BUS_NUMBER(pDevInfo);
//  get the AST base address form AST PCI deivce.
	AST_Set_MMIOBase_PEI(&ASTBaseAdd);
}
//<Lizhq-20170807 Get dynamic MMIO base for fast video frame buffer.(Mantis:15241) ->

//<AMI_PHDR_START>
//----------------------------------------------------------------------
// Procedure:  AST_Set_PCIE_BUS_NUMBER
//
// Description: This function Set AST Device in temp bus
//
// Input:       
//
// Output:      NONE
//
// Note: 
//
//----------------------------------------------------------------------
//<AMI_PHDR_END>
//<Lizhq-20170807 Get dynamic MMIO base for fast video frame buffer.(Mantis:15241) +>
VOID AST_Set_PCIE_BUS_NUMBER (_DEVInfo* pDevInfo)
{
    UINT8       bus,dev,fun;
    UINT32      Buffer32;
    UINT32    NumOfBdf_PEI;
    UINT32      RpMmioBase = (pDevInfo-> ulMMIOBase& 0xFFFF0000) |  (pDevInfo->ulFBBase >> 0x10) ;

    DEBUG((EFI_D_ERROR, "RpMmioBase: 0x%x", RpMmioBase));
    NumOfBdf_PEI = (UINT32)(AST_SB_BUS_NUM << 16) | (AST_SB_DEV_NUM << 11) | (AST_SB_FUNC_NUM<< 8);
    Buffer32 = 0x000C0B00; 
    WritePciReg32(NumOfBdf_PEI, 0x18, Buffer32);//set the ast device to temp bus .
    Buffer32 = RpMmioBase;

    WritePciReg32(NumOfBdf_PEI, 0x20, Buffer32);//set the ast device menory.
    Buffer32 = ReadPciReg32(NumOfBdf_PEI,0x1C);//Set the ast device IO. {
    Buffer32 &= 0xFFFF0000;
// Machong 2013/05/09 Sync with 8SG IO Allocation {    
    Buffer32 |= 0x1010;
// Machong 2013/05/09 Sync with 8SG IO Allocation }    
    WritePciReg32(NumOfBdf_PEI, 0x1C, Buffer32); //Set the ast device IO. }
    
    Buffer32 = 0x00100007; 
    WritePciReg32(NumOfBdf_PEI, 0x04, Buffer32); //Let IO & MMIO can Read/Write(Bit0:IO, Bit1:MMIO)
    
    Buffer32 = ReadPciReg32(NumOfBdf_PEI,0x3C);//Set the ast device VGA Support. {
    Buffer32 &= 0xFFFF;
    Buffer32 |= 0x180000;
    WritePciReg32(NumOfBdf_PEI, 0x3C, Buffer32); //Set the ast device VGA Support. }
    
    //Scan the PCI device and search the Device AST by Dev ID.   
    bus = 0x0B;  
            for (dev=0x00; dev <= 0x1f;dev++){
                for(fun=0x00;fun <= 0x07;fun++){
                    NumOfBdf_PEI = (UINT32)(bus << 16) | (dev << 11) | (fun<< 8);
                    Buffer32 = ReadPciReg32(NumOfBdf_PEI,0x00);
                    if (Buffer32 == 0x11501A03){
                    break;
                    }
                }
                if (Buffer32 == 0x11501A03){
                break;
                }
            }        
    if (Buffer32 != 0x11501A03)
    {
    	;
    }

    Buffer32 = 0x200C0C0B; 
    WritePciReg32(NumOfBdf_PEI, 0x18, Buffer32);//set the ast device to temp bus .
    Buffer32 = RpMmioBase;
    WritePciReg32(NumOfBdf_PEI, 0x20, Buffer32);//set the ast device menory.
    Buffer32 = ReadPciReg32(NumOfBdf_PEI,0x1C);//Set the ast device IO. {
    Buffer32 &= 0xFFFF0000;
    // Machong 2013/05/09 Sync with 8SG IO Allocation {     
    Buffer32 |= 0x1111;
    // Machong 2013/05/09 Sync with 8SG IO Allocation }
    WritePciReg32(NumOfBdf_PEI, 0x1C, Buffer32); //Set the ast device IO. }    
    
    Buffer32 = ReadPciReg32(NumOfBdf_PEI,0xC); //Set the ast device Cacheline {
    Buffer32 &= 0xFFFF0000;
    Buffer32 |= 0x10;
    WritePciReg32(NumOfBdf_PEI, 0xC, Buffer32); //Set the ast device Cacheline }       

    Buffer32 = 0x00100007; 
    WritePciReg32(NumOfBdf_PEI, 0x04, Buffer32); //Let IO & MMIO can Read/Write(Bit0:IO, Bit1:MMIO)
    
    Buffer32 = ReadPciReg32(NumOfBdf_PEI,0x3C);//Set the ast device VGA Support. {
    Buffer32 &= 0xFFFF;
    Buffer32 |= 0x180000;
    WritePciReg32(NumOfBdf_PEI, 0x3C, Buffer32); //Set the ast device VGA Support. }
    
    Buffer32 = ReadPciReg32(NumOfBdf_PEI,0x18);
    bus = 0x0C;

            for (dev=0x00; dev <= 0x1f;dev++){
                for(fun=0x00;fun <= 0x07;fun++){
                    NumOfBdf_PEI = (UINT32)(bus << 16) | (dev << 11) | (fun<< 8);
                    Buffer32 = ReadPciReg32(NumOfBdf_PEI,0x00);
                    if (Buffer32 == 0x20001A03){
                    break;
                    }
                }
                if (Buffer32 == 0x20001A03){
                break;
                }
            } 

    if (Buffer32 != 0x20001A03)
    {
    	;
    }
}
//<Lizhq-20170807 Get dynamic MMIO base for fast video frame buffer.(Mantis:15241) ->

//<AMI_PHDR_START>
//----------------------------------------------------------------------
// Procedure:  AST_Set_MMIOBase_PEI
//
// Description: This function Config AST chip PCI configreation to temp value.
//
// Input:       
//
// Output:      NONE
//
// Note: 
//
//----------------------------------------------------------------------
//<AMI_PHDR_END>
VOID AST_Set_MMIOBase_PEI(OUT UINT32 *ASTBaseAdd)
{
    UINT32  VIDDID,temp, Buffer32;
    UINT32    NumOfBdf_PEI;
    UINT8       bus,dev,fun;
    bus = 0x0B;
    NumOfBdf_PEI = 0;
    Buffer32 = 0;
    
    bus = 0x0C;
	for (dev=0x00; dev <= 0x1f;dev++){
		for(fun=0x00;fun <= 0x07;fun++){
			NumOfBdf_PEI = (UINT32)(bus << 16) | (dev << 11) | (fun<< 8);
			Buffer32 = ReadPciReg32(NumOfBdf_PEI,0x00);
			if (Buffer32 == 0x20001A03){
			break;
			}
		}
		if (Buffer32 == 0x20001A03){
		break;
		}
	} 
    
    VIDDID = ReadPciReg32(NumOfBdf_PEI,0x00);

    if (VIDDID != 0x20001A03){

    return;
    }
    //init the PCI device.
    //<Lizhq-20170807 Get dynamic MMIO base for fast video frame buffer.(Mantis:15241) +>
    Buffer32 = *ASTBaseAdd;
    WritePciReg32(NumOfBdf_PEI, 0x10, Buffer32);//set the ast device menory1.
    Buffer32 = *ASTBaseAdd+SIZE_4MB; //Reserved 4MB for BAR0
    //<Lizhq-20170807 Get dynamic MMIO base for fast video frame buffer.(Mantis:15241) ->
    WritePciReg32(NumOfBdf_PEI, 0x14, Buffer32);//set the ast device menory2.
    Buffer32 = 0x0000010B; 
    WritePciReg32(NumOfBdf_PEI, 0x3C, Buffer32);//set the ast Int Line.
    
    Buffer32 = ReadPciReg32(NumOfBdf_PEI,0x18);//Set the ast device IO. {
    Buffer32 &= 0xFFFF0000;
    // Machong 2013/05/09 Sync with 8SG IO Allocation { 
    Buffer32 |= 0x1001;
    // Machong 2013/05/09 Sync with 8SG IO Allocation }
    WritePciReg32(NumOfBdf_PEI, 0x18, Buffer32); //Set the ast device IO. } 
    
    Buffer32 = 0x02100003; 
    WritePciReg32(NumOfBdf_PEI, 0x04, Buffer32); //Let IO & MMIO can Read/Write(Bit0:IO, Bit1:MMIO)

    //Get mmio base
    *ASTBaseAdd = ReadPciReg32(NumOfBdf_PEI,0x14);

    //Set PCI Comment
    temp = ReadPciReg32(NumOfBdf_PEI,0x04);
    temp |= 0x3;
    WritePciReg32(NumOfBdf_PEI, 0x4, temp);   //Let IO & MMIO can Read/Write(Bit0:IO, Bit1:MMIO)
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------
// Procedure:  AST_SetBMCReg_PEI
//
// Description: This function set the AST VUART register
//
// Input:       ASTBaseAdd 	: MMIO
//		VUARReg		: VUART Config register
//		DATA		: Date set to register
// Output:      NONE
//
// Note: 
//
//----------------------------------------------------------------------
//<AMI_PHDR_END>
VOID AST_SetBMCReg_PEI(IN UINT32 ASTBaseAdd, IN UINT8 VUARReg, IN UINT8 DATA)
{

    //Write DATA
    Mmio8(ASTBaseAdd, 0x10000 + 0x7000 + VUARReg) = DATA;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------
// Procedure:  AST_GetBMCReg_PEI
//
// Description: This function set the AST VUART register
//
// Input:       ASTBaseAdd 	: MMIO
//		VUARReg		: VUART Config register
//		
// Output:      DATA		: Date set to register
//
// Note: 
//
//----------------------------------------------------------------------
//<AMI_PHDR_END>
UINT32 AST_GetBMCReg_PEI(IN UINT32 ASTBaseAdd, IN UINT8 VUARReg)
{
    UINT8 DATA;
    //Write DATA
    DATA = Mmio8(ASTBaseAdd, 0x10000 + 0x7000 + VUARReg);

    return DATA;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------
// Procedure:  AST_Clear_MMIOBase_PEI
//
// Description: This function Clear AST Device in temp bus
//
// Input:       
//
// Output:      NONE
//
// Note: 
//
//----------------------------------------------------------------------
//<AMI_PHDR_END>
VOID AST_Clear_MMIOBase_PEI()
{
    UINT32  VIDDID,temp, Buffer32;
    UINT32    NumOfBdf_PEI;
    UINT8       bus,dev,fun;
    bus = 0x0B;
    Buffer32=0;
    NumOfBdf_PEI=0;
    
	for (dev=0x00; dev <= 0x1f;dev++){
		for(fun=0x00;fun <= 0x07;fun++){
			NumOfBdf_PEI = (UINT32)(bus << 16) | (dev << 11) | (fun<< 8);
			Buffer32 = ReadPciReg32(NumOfBdf_PEI,0x00);
			if (Buffer32 == 0x11501A03){
			break;
			}
		}
		if (Buffer32 == 0x11501A03){
		break;
		}
	} 
	
    VIDDID = ReadPciReg32(NumOfBdf_PEI,0x00);

    if (VIDDID != 0x20001A03){
    return;
    }
    //Clear PCI Comment
    temp = ReadPciReg32(NumOfBdf_PEI,0x04);
    temp &= 0x00;
    WritePciReg32(NumOfBdf_PEI, 0x4, temp);   //
    //Clear the PCI device.
    Buffer32 = 0x00000000; 
    WritePciReg32(NumOfBdf_PEI, 0x10, Buffer32);//set the ast device menory1.
    Buffer32 = 0x00000000; 
    WritePciReg32(NumOfBdf_PEI, 0x14, Buffer32);//set the ast device menory2.
    Buffer32 = 0x00000000; 
    WritePciReg32(NumOfBdf_PEI, 0x3C, Buffer32);//set the ast Int Line.
    Buffer32 = 0x00000000; 
    WritePciReg32(NumOfBdf_PEI, 0x04, Buffer32); //


}
//<AMI_PHDR_START>
//----------------------------------------------------------------------
// Procedure:  AST_Clear_PCIE_BUS_NUMBER
//
// Description: This function Clear AST Device in temp bus
//
// Input:       
//
// Output:      NONE
//
// Note: 
//
//----------------------------------------------------------------------
//<AMI_PHDR_END>
VOID AST_Clear_PCIE_BUS_NUMBER ()
{
    UINT8   bus,dev,fun;
    UINT32  Buffer32;
    UINT32    NumOfBdf_PEI;
    bus = 0x04;  
    NumOfBdf_PEI =0;
    Buffer32 = 0;
            for (dev=0x00; dev <= 0x1f;dev++){
                for(fun=0x00;fun <= 0x07;fun++){
                    NumOfBdf_PEI = (UINT32)(bus << 16) | (dev << 11) | (fun<< 8);
                    Buffer32 = ReadPciReg32(NumOfBdf_PEI,0x00);
                    if (Buffer32 == 0x11501A03){
                    break;
                    }
                }
                if (Buffer32 == 0x11501A03){
                break;
                }
            } 
    Buffer32 = 0x00000000; 
    WritePciReg32(NumOfBdf_PEI, 0x04, Buffer32); //clear IO & MMIO can Read/Write(Bit0:IO, Bit1:MMIO)
    Buffer32 = 0x00000000; 
    WritePciReg32(NumOfBdf_PEI, 0x18, Buffer32);//clear the ast device to temp bus .
    Buffer32 = 0x00000000; 
    WritePciReg32(NumOfBdf_PEI, 0x20, Buffer32);//clear the ast device menory.


    NumOfBdf_PEI = (UINT32)(AST_SB_BUS_NUM << 16) | (AST_SB_DEV_NUM << 11) | (AST_SB_FUNC_NUM<< 8);
    Buffer32 = 0x00000000; 
    WritePciReg32(NumOfBdf_PEI, 0x04, Buffer32); //clear IO & MMIO can Read/Write(Bit0:IO, Bit1:MMIO)
    Buffer32 = 0x00000000; 
    WritePciReg32(NumOfBdf_PEI, 0x18, Buffer32);//clear the ast device to temp bus 05.
    Buffer32 = 0x00000000;  
    WritePciReg32(NumOfBdf_PEI, 0x20, Buffer32);//clear the ast device menory.
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------
// Procedure:  WritePciReg32
//
// Description: This function write PCI 32bit register 
//
// Input:       UINT32    dNumOfBusDevFunc - Content the BUS DEV and FUNC number to be used.
//              UINT16    Reg       
//              UINT32    Value32
//
// Output:      NONE
//
// Note: 
//
//----------------------------------------------------------------------
//<AMI_PHDR_END>
static VOID WritePciReg32 (
    IN UINT32       dNumOfBusDevFunc,
    IN UINT16       Reg,
    IN UINT32       Value32 )
{
	//BIT31 | (SIO_SB_BUS_NUM << 16) | (SIO_SB_DEV_NUM << 11) | (SIO_SB_FUNC_NUM << 8) | (Reg & 0xfc);
    	dNumOfBusDevFunc |= 0x80000000;
    	dNumOfBusDevFunc &= (UINT32)(~0xff);
    	dNumOfBusDevFunc |= (UINT32)(Reg & ~3);
        IoWrite32(0xcf8, dNumOfBusDevFunc);
        IoWrite32(0xcfc, Value32);
}
//<AMI_PHDR_START>
//----------------------------------------------------------------------
// Procedure:  ReadPciReg32
//
// Description: This function read PCI 32bit register 
//
// Input:       UINT32    dNumOfBusDevFunc - Content the BUS DEV and FUNC number to be used.
//              UINT16    Reg       
//
// Output:      NONE
//
// Note: 
//
//----------------------------------------------------------------------
//<AMI_PHDR_END>
static UINT32 ReadPciReg32 (
    IN UINT32       dNumOfBusDevFunc,
    IN UINT16       Reg )
{
	//BIT31 | (SIO_SB_BUS_NUM << 16) | (SIO_SB_DEV_NUM << 11) | (SIO_SB_FUNC_NUM << 8) | (Reg & 0xfc);
    	dNumOfBusDevFunc |= 0x80000000;
    	dNumOfBusDevFunc &= (UINT32)(~0xff);
    	dNumOfBusDevFunc |= (UINT32)(Reg & ~3);

        IoWrite32(0xcf8, dNumOfBusDevFunc);
        return IoRead32(0xcfc);
}
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
