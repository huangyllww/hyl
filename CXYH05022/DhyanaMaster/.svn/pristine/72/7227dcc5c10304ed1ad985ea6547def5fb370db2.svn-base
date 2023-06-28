#include <ASM106xLib.h>
#include <Porting.h>
#include <Uefi/UefiBaseType.h>
#include <Library/PciLib.h>
#include <Library/DebugLib.h>

/**
    Read Byte from Index-Data Pair

    @param PciAddr64 - Bridge Address of the device.
    @param Register - The Register will be read.

    @Output: Read value.

**/
UINT8
ASM1061RIdpReadByte (
    IN UINT64                   PciAddr64,
    IN UINT32                   Register )
{
    UINT32 Index;
    UINT8 bValue;
    UINT8 Offset;
    
    //Index shall be DWORD alignment
    Index = Register & (~0x03);
    Offset = Register % 4;  
    
    DEBUG((EFI_D_ERROR, "[%a][%d]Index:%x Offsert:%x\n\n",__FUNCTION__,__LINE__,Index,Offset));
    //Write Index    
    PciWrite32(PciAddr64 | ASM1061R_MMIO_INDEX,Index);
    
    //Read BYTE Data   
    bValue = PciRead8(PciAddr64 | (ASM1061R_MMIO_DATA + Offset));
    
    DEBUG((EFI_D_ERROR, "[%a][%d]Register %04x  DataRead:%x\n\n",__FUNCTION__,__LINE__,Register,bValue));
    return bValue;

}



/**
    Write Byte from Index-Data Pair
    @param PciAddr64 - Bridge Address of the device.
    @param Register - The Register will be read.
    @param bValue - The value to write to Register.

    @Output: none.

**/
VOID
ASM1061RIdpWriteByte (
    IN UINT64                   PciAddr64,
    IN UINT32                   Register,
    IN UINT8                    bValue  )
{
    
    UINT32 Index;
    UINT8 Offset;
    //Index shall be DWORD alignment
    Index = Register & (~0x03);
    Offset = Register % 4;
    
    DEBUG((EFI_D_ERROR, "[%a][%d]Index:%x Offsert:%x\n\n",__FUNCTION__,__LINE__,Index,Offset));
    //Write Index    
    PciWrite16(PciAddr64 | ASM1061R_MMIO_INDEX,Index);
    
    //Write BYTE Data    
    PciWrite8(PciAddr64 | (ASM1061R_MMIO_DATA + Offset),bValue);
}


/**
    Set the AHCI class code.
    @param PciAddr64 - Bridge Address for setting the Bus Number.

    @retval None.

    @note  None.

**/

VOID
ASM1061RSetAhciClass (
    IN UINT64                   PciAddr64 )
{
    UINT8                       Buffer8;
    //(1)Enable Subclass to be programmed:Set PCI RxF0[0] = 1 (BACKDOOR_CTL enable)
    Buffer8 = PciRead8(PciAddr64 | 0xF0);
    Buffer8 |= 0x01;
    PciWrite8(PciAddr64 | 0xF0,Buffer8);
        
    //(2)Set Subclass Code:Modify PCI Rx0A = 06h and Rx0B = 01h
    PciWrite8(PciAddr64 | 0x0A,0x06);
      
    PciWrite8(PciAddr64 | 0x0B,0x01);
     
    //(3)Disable Subclass to be programmed:Set PCI RxF0[0] = 0        
    Buffer8 &= 0xFE;
    PciWrite8(PciAddr64 | 0xF0,Buffer8);
        
}


/**
    Set the Spin-Up SATA device.

    @param PeiServices - Pointer to the PEI services table.
    @param PciCfg2 - Pointer to the PciCfg2 PPI.
    @param PciAddr64 - Bridge Address for setting the Bus Number.

    @retval None.

    @note  None.

**/

VOID
ASM1061RSpinUp (
    IN UINT64                   PciAddr64 )
{
    UINT8                   bData;
    // Set MMIO Rx0D0A[1:0] = 2:11b
    bData = ASM1061RIdpReadByte(PciAddr64, 0x0D0A);
    // you can delay some time to program bit 1 after bit 0 was programed to supported staggered spin-up
    bData |= 0x03; //bit[1:0] = 3:11b
    ASM1061RIdpWriteByte(PciAddr64, 0x0D0A, bData);
}


/**
    Disable Safety Removal for AHCI Mode.

    @param PeiServices - Pointer to the PEI services table.
    @param PciCfg2 - Pointer to the PciCfg2 PPI.
    @param PciAddr64 - Bridge Address for setting the Bus Number.

    @retval None.

    @note  None.

**/

VOID
ASM1061RDisableSafetyRemoval (
    IN UINT64                   PciAddr64 )
{
    UINT8                   bData;

    // MMIO Rx0D00[5] = 0 Set HBA CAP.SXS = 0
    bData = ASM1061RIdpReadByte(PciAddr64, 0x0D00);
    bData &= ~0x20; //bit 5 = 0
    ASM1061RIdpWriteByte(PciAddr64, 0x0D00, bData);
    // MMIO Rx0DC8[3] = 0 Set Port 0 PxCMD.HPCP = 0
    // MMIO Rx0DC8[6] = 1 Set Port 0 PxCMD.ESP = 1
    bData = ASM1061RIdpReadByte(PciAddr64, 0x0DC8);
    bData &= ~0x08; //bit 3 = 0
    bData |= 0x40; //bit 6 = 1
    ASM1061RIdpWriteByte(PciAddr64, 0x0DC8, bData);
    // MMIO Rx0DC9[3] = 0 Set Port 0 PxCMD.HPCP = 0
    // MMIO Rx0DC9[6] = 1 Set Port 0 PxCMD.ESP = 1
    bData = ASM1061RIdpReadByte(PciAddr64, 0x0DC9);
    bData &= ~0x08; //bit 3 = 0
    bData |= 0x40; //bit 6 = 1
    ASM1061RIdpWriteByte(PciAddr64, 0x0DC9, bData);
}

/**
    Disable SATA Device Sleep.

    @param PeiServices - Pointer to the PEI services table.
    @param PciCfg2 - Pointer to the PciCfg2 PPI.
    @param PciAddr64 - Bridge Address for setting the Bus Number.

    @retval None.

    @note  None.

**/

VOID
ASM1061RDisableDeviceSleep (
    IN UINT64                   PciAddr64 )
{
    UINT8                   bData;
    
    // Set MMIO Rx0D08[4:3] = 2 00b
    bData = ASM1061RIdpReadByte(PciAddr64, 0x0D08);
    bData &= ~0x18; //bit[4:3] = 2 00b
    ASM1061RIdpWriteByte(PciAddr64, 0x0D08, bData);
}


/**
    Set the Bus Number for PCI Bridge.

    @param PeiServices - Pointer to the PEI services table.
    @param PciCfg2 - Pointer to the PciCfg2 PPI.
    @param PciAddr64 - Bridge Address for setting the Bus Number.
    @param PciBusNum - The Bus Number would be set.

    @retval None.

    @note  None.

**/

VOID
SetPciBridgerBusNum (
    IN UINT64                   PciAddr64,
    IN UINT8                    PciBusNum )
{
    PciWrite8(PciAddr64 | 0x19,PciBusNum);
    PciWrite8(PciAddr64 | 0x1A,PciBusNum);
}


/**
    Search and return the Capability ID Address by FindCapNo.

    @param PeiServices - Pointer to the PEI services table.
    @param PciCfg2 - Pointer to the PciCfg2 PPI.
    @param PciAddr64 - Device Address for finding CapNo.
    @param FindCapNo - The Capability ID would be searched.
    @param CapPtr8 - Pointer to the Capability ID Address.

    @retval EFI_STATUS.

    @note  None.

**/

EFI_STATUS
GetLegCapStrucAddr (
    IN UINT64                   PciAddr64,
    IN UINT8                    FindCapNo,
    OUT UINT8                   *CapPtr8 )
{
    UINT8                       Buffer8;

    Buffer8 = PciRead8(PciAddr64 | 0x34);
    *CapPtr8 = Buffer8;
    if ((*CapPtr8 == 0) || (*CapPtr8 == 0xFF)) return EFI_NOT_FOUND;

    Buffer8 = PciRead8(PciAddr64 | *CapPtr8);
    while (Buffer8 != 0) {
        if (Buffer8 == FindCapNo) return EFI_SUCCESS;
        
        Buffer8 = PciRead8(PciAddr64 | (UINT8)(*CapPtr8 + 1));
        *CapPtr8 = Buffer8;
        if (*CapPtr8 == 0) break;
        
        Buffer8 = PciRead8(PciAddr64 | (UINT8)(*CapPtr8));
    }
    return EFI_NOT_FOUND;
}
