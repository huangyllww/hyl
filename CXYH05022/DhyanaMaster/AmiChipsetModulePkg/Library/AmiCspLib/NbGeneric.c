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

/** @file NbGeneric.c
    This file contains generic NB code that is common between
    various components such as NB PEI, DXE etc

**/


// Module specific Includes
#include "Efi.h"
#include "token.h"
#include <AmiLib.h>
#include "Library/NbCspLib.h"
#include <AmiCspLib.h>
#include <AmiDxeLib.h>
#include "Library/NbPolicy.h"
#include <Protocol/PciRootBridgeIo.h>
#include <AmiCpuPkg/CPU/Include/CpuCspLib.h>
// Produced Protocols

// GUID Definitions

// Portable Constants
EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *gPciRootBridgeIo;

// Function Prototypes

// Function Definition

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#if		CSM_SUPPORT
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// Start OF CSM Related Porting Hooks
//----------------------------------------------------------------------------

//
// The following data structure specifies the PCI device/function number of the root
// bridge(s). Number of entries in this table defined by NUMBER_OF_ROOTBRIDGES.
// This table is a missing link between RootBridgeIo and PciIo, which allows to update
// BusNumXlat table with actual bus numbers.
// Each entry in the table is a pair of RootBridge UID (UINT32), provided in RootBridge
// device path, and PCI Dev/Func number (UINT8) that can be used to access Root Bridge on
// PCI bus.
//
ROOT_BRIDGE_MAPPING_ENTRY	RbMap[ROOT_BRIDGE_COUNT] = {
//			RB ID			Device function number
			{ 0,			0xC0}
	};
UINTN						RbCount = ROOT_BRIDGE_COUNT;


/**
    Program 0xc0000 - 0xfffff regions to Lock/Unlock.

    @param pBS 
    @param pRS 
    @param StartAddress 
    @param Length 
    @param Setting 
    @param Granularity 

    @retval EFI_STATUS return EFI status

    @note  Here is the control flow of this function:
				1. Search the structure for the first entry matching 
					the StartAddress.
  			2. If not found, return EFI_INVALID_PARAMETER.
  			3. Find the last entry in structure for the region to program,
     				by adding the lengths of the entries.
				4. If not found, return EFI_INVALID_PARAMETER.
				5. Read/Write each register for the entry to set region.
				6. Return the Granularity for the region.
**/

EFI_STATUS
NBProgramPAMRegisters(
	EFI_BOOT_SERVICES		*pBS,
	EFI_RUNTIME_SERVICES	*pRS,
	UINT32					StartAddress,
	UINT32					Length,
	UINT8					Setting,
	UINT32					*Granularity)
{
	EFI_STATUS		Status = EFI_SUCCESS;


	return Status;
}

/**
    Program 0xc0000 - 0xfffff regions to Lock/Unlock.

        
    @param PeiServices 
    @param StartAddress 
    @param Length 
    @param Setting 
    @param Granularity 

    @retval EFI_STATUS return EFI status

**/

EFI_STATUS NBPeiProgramPAMRegisters (
    IN EFI_PEI_SERVICES **PeiServices,
    UINT32				StartAddress,
    UINT32				Length,
    UINT8				Setting,
    UINT32				*Granularity OPTIONAL )
{
	EFI_STATUS	Status = EFI_SUCCESS;
	return Status;
}


/**
    Writes the final settings of PAM registers to the BOOT Script

    @param BootScriptSave 

    @retval EFI_STATUS return EFI status

    @note  Here is the control flow of this function:
				1. From the Pci register save table, read the pci register to save.
  			2. Write to the boot script the value.
  			3. Repeat 1 & 2 for all table entries.
**/

EFI_STATUS NBPAMWriteBootScript(
#if (PI_SPECIFICATION_VERSION < 0x0001000A) || (BACKWARD_COMPATIBLE_MODE == 1)
	IN EFI_BOOT_SCRIPT_SAVE_PROTOCOL *BootScriptSave
#else
    IN EFI_S3_SAVE_STATE_PROTOCOL    *BootScriptSave
#endif
)
{
	return EFI_SUCCESS;
}

//----------------------------------------------------------------------------
#endif          // END OF CSM Related Porting Hooks
//----------------------------------------------------------------------------

/**

     This function provides runtime interface to enable/disable
              writing in C000-F000 segment

     @param   Attribute   The shadowing attributes of the BIOS range
     @param   Range       The BIOS range which is going to be shadowed

     @retval  EFI_STATUS return EFI status

**/

EFI_STATUS
NbRuntimeShadowRamWriteExt (
    IN SHADOW_ARRTIBUTE   Attribute,
    IN SHADOW_BIOS_RANGE  Range
)
{
    UINT32  StartAddress;
    UINT32  Length;
    UINT8   Setting;

    switch (Attribute) {
    case Disabled:
      Setting = 0x3;
      break;
    case ReadOnly:
      Setting = LEGACY_REGION_LOCK;
      break;
    case WriteOnly:
      Setting = LEGACY_REGION_UNLOCK;
      break;
    case ReadWrite:
      Setting = LEGACY_REGION_UNLOCK;
      break;
    default:
      return EFI_INVALID_PARAMETER;
    }

    //The start address should be 0xC0000 at beginning
    StartAddress = 0xC0000;
    //The Length is 16k as a base except the F0000 is 64k as a base
    Length = 0x4000;
    
    if (Range <= 11)
    {
        StartAddress += (Length * Range);
    }
    else if (Range == 12)
    {
        Length = 0x10000;
        StartAddress = 0xF0000;
    }
    else
        return EFI_INVALID_PARAMETER;

    
    CPUProgramPAMRegisters(NULL, NULL, StartAddress, Length, Setting, NULL);
    
    return EFI_SUCCESS;
}

/**
    HYGON PciConfig read function....Reads Normal and Extended PCI space

        
    @param Address 
    @param ExtAccess use Extended PCI space access if TRUE

         
    @retval UINT32 The value read from the indicated Address.

    @note  
  Supports only 32 bit read only to be used for S3 Feature

**/

UINT32
NbPciConfigRead(
    IN UINT32 Address,
    IN BOOLEAN ExtAccess )
{
    UINT32  PciAddress = ((UINT32)1 << 31) +
        (((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Bus      << 16 )  +
        (((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Device   << 11 )  +
        (((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Function << 8  )  +
        (((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Register       );

    PciAddress = PciAddress & 0xFFFFFFFC; //Dword Align

    if(ExtAccess) //If extended config Access requested then add the extended config address  bit (24 to 27)
        PciAddress = PciAddress | 0x1000000;

    IoWrite32(0xCF8, PciAddress);
    return IoRead32(0xCFC);         //Return the read value
}

/**
    Write a value into the PCI register indicated

        
    @param Address 
    @param Value 
    @param ExtAccess 

         
    @retval VOID

**/

VOID NbPciConfigWrite(
    IN UINT32 Address,
    IN UINT32 Value,
    IN BOOLEAN ExtAccess)
{
    UINT32  PciAddress = ((UINT32)1 << 31) +
        (((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Bus      << 16 )  +
        (((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Device   << 11 )  +
        (((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Function << 8  )  +
        (((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Register       );
    PciAddress = PciAddress & 0xfffffffc;

    //If extended config Access requested then add the extended config address  bit (24 to 27)
    if (ExtAccess)
        PciAddress = PciAddress | 0x1000000;

    IoWrite32(0xcf8, PciAddress);  //Select the register to write
    IoWrite8(0xed, 00);
    IoWrite8(0xed, 00);
    IoWrite32(0xcfc, Value); //Write the Value
}



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
