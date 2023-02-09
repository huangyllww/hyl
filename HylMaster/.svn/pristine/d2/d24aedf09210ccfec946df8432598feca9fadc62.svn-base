#ifndef __ASM_106X_LIB_H__
#define __ASM_106X_LIB_H__

#include <Porting.h>

//#define ASM1061R_TEMP_PCI_BUS     0x15

#define ASM1061R_MMIO_INDEX       0xD0      //PCI offset 0xD0 ~ 0xD3: Index
#define ASM1061R_MMIO_DATA        0xD4     //PCI offset 0xD4 ~ 0xD7: Data



/**
    Set the AHCI class code.
    @param PciAddr64 - Bridge Address for setting the Bus Number.

    @retval None.

    @note  None.

**/

VOID
ASM1061RSetAhciClass (
    IN UINT64                   PciAddr64 );

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
    IN UINT64                   PciAddr64 );

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
    IN UINT64                   PciAddr64 );
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
    IN UINT64                   PciAddr64 );

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
    IN UINT8                    PciBusNum );

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
    OUT UINT8                   *CapPtr8 );
#endif