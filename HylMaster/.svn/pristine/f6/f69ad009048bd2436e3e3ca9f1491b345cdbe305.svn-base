//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file CrbLib.c
    This file contains Chipset Reference Board related code that
    is needed for both PEI & DXE stage. 
    To avoid code duplication this file is made as a library and
    linked both in PEI & DXE CRB FFS.


**/

//---------------------------------------------------------------------------
// Include(s)
//---------------------------------------------------------------------------

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/AmiRomLayoutLib.h>

#include <Token.h>
#include <Library/SbSetupData.h>
#include <AmiCspLib.h>
#include <Setup.h>
#include <Protocol/PIDEController.h>

// Produced Protocols

// Consumed Protocols
#include <Protocol/PciIo.h>

//---------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//---------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)

//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
// Variable Declaration(s)

// GUID Definition(s)

// Protocol Definition(s)

// External Declaration(s)

// Function Definition(s)

//---------------------------------------------------------------------------

// All routines in this file will be added into CSP library.

UINT32 CrbHdaVerbTbl0[] = {
    //Realtek HD Audio Codec : ALC288
    //PCI PnP ID : PCI\VEN_1022&DEV_780D&SUBSYS_C7511022
    //HDA Codec PnP ID : HDAUDIO\FUNC_01&VEN_10EC&DEV_0286&SUBSYS_1022C751
    //The number of verb command block : 15
    //    NID 0x12 : 0xB7A60150
    //    NID 0x13 : 0x40000000
    //    NID 0x14 : 0x90170110
    //    NID 0x17 : 0x411111F0
    //    NID 0x18 : 0x04A11040
    //    NID 0x19 : 0x411111F0
    //    NID 0x1A : 0x411111F0
    //    NID 0x1D : 0x4067A52D
    //    NID 0x1E : 0x04451130
    //    NID 0x21 : 0x04211020

    //===== HDA Codec Subsystem ID Verb-table =====
    //HDA Codec Subsystem ID  : 0x1022C751
     0x00172051,
     0x001721C7,
     0x00172222,
     0x00172310,

    //===== Pin Widget Verb-table =====
    //Widget node 0x01 :
     0x0017FF00,
     0x0017FF00,
     0x0017FF00,
     0x0017FF00,
    //Pin widget 0x12 - DMIC
     0x01271C50,
     0x01271D01,
     0x01271EA6,
     0x01271FB7,
    //Pin widget 0x13 - DMIC
     0x01371C00,
     0x01371D00,
     0x01371E00,
     0x01371F40,
    //Pin widget 0x14 - SPEAKER-OUT (Port-D)
     0x01471C10,
     0x01471D01,
     0x01471E17,
     0x01471F90,
    //Pin widget 0x17 - I2S-OUT
     0x01771CF0,
     0x01771D11,
     0x01771E11,
     0x01771F41,
    //Pin widget 0x18 - MIC1 (Port-B)
     0x01871C40,
     0x01871D10,
     0x01871EA1,
     0x01871F04,
    //Pin widget 0x19 - I2S-IN
     0x01971CF0,
     0x01971D11,
     0x01971E11,
     0x01971F41,
    //Pin widget 0x1A - LINE1 (Port-C)
     0x01A71CF0,
     0x01A71D11,
     0x01A71E11,
     0x01A71F41,
    //Pin widget 0x1D - PC-BEEP
     0x01D71C2D,
     0x01D71DA5,
     0x01D71E67,
     0x01D71F40,
    //Pin widget 0x1E - S/PDIF-OUT
     0x01E71C30,
     0x01E71D11,
     0x01E71E45,
     0x01E71F04,
    //Pin widget 0x21 - HP-OUT (Port-A)
     0x02171C20,
     0x02171D10,
     0x02171E21,
     0x02171F04,
    //Widget node 0x20 :
     0x02050071,
     0x02040014,
     0x02050010,
     0x02040C22,
    //Widget node 0x20 - 1 :
     0x0205004F,
     0x0204B029,
     0x0205002B,
     0x02040C50,
    //Widget node 0x20 - 2 :
     0x0205002D,
     0x02041020,
     0x02050020,
     0x02040000,
    //Widget node 0x20 - 3 :
     0x02050019,
     0x02040817,
     0x02050035,
     0x02041AA5,
    //Widget node 0x20 - 4 :
     0x02050063,
     0x02042906,
     0x02050063,
     0x02042906,
     
     0xffffffff   //End of Verb Table flag
};

#if CMOS_MANAGER_SUPPORT
/**
    @param PeiServices 
    @param ReadVariablePpi 
    @retval BOOLEAN TRUE or FALSE
**/
BOOLEAN OemIsResetConfigMode(
    IN EFI_PEI_SERVICES **PeiServices, 
    IN CONST EFI_PEI_READ_ONLY_VARIABLE2_PPI *ReadVariablePpi
  )
{


#ifdef CLEAR_NVRAM_IF_CMOS_BAD
  #if CLEAR_NVRAM_IF_CMOS_BAD
  /*
    IoWrite8(CMOS_ADDR_PORT, CMOS_DIAGNOSTIC_STATUS_REGISTER);
    if (IoRead8(CMOS_DATA_PORT) & (BIT06 + BIT07)) return TRUE; 
    */
    IoWrite8(CMOS_BANK1_INDEX, CMOS_BAD_CHECK_ADDRESS);
    if (IoRead8(CMOS_BANK1_DATA) ==0x55)  return TRUE;
  #endif
#endif
    return FALSE;
}
#endif //CMOS_MANAGER_SUPPORT

#if CMOS_MANAGER_SUPPORT
/**
    This function determines if the system should boot with the default configuration.
    This function is called by NVRAM code. 
    If boot with default configuration is detected, default values for NVRAM variables are used.

    @param PeiServices pointer to a pointer to the PEI Services Table.
    @param ReadVariablePpi pointer to EFI_PEI_READ_ONLY_VARIABLE2_PPI PPI. The pointer can be used to
        read and enumerate existing NVRAM variables.

               
    @retval TRUE Boot with default configuration is detected
    @retval FALSE Boot with default configuration is not detected

    @note  This routine is called very early, prior to SBPEI and NBPEI.
**/

BOOLEAN OemIsDefaultConfigMode(
    IN EFI_PEI_SERVICES **PeiServices, 
    IN CONST EFI_PEI_READ_ONLY_VARIABLE2_PPI *ReadVariablePpi
){

#ifdef LOAD_DEFAULTS_IF_CMOS_BAD
  #if LOAD_DEFAULTS_IF_CMOS_BAD
  //  IoWrite8(CMOS_ADDR_PORT, CMOS_DIAGNOSTIC_STATUS_REGISTER);
   // if (IoRead8(CMOS_DATA_PORT) & (BIT06 + BIT07)) return TRUE; 
       IoWrite8(CMOS_BANK1_INDEX, CMOS_BAD_CHECK_ADDRESS);
    if (IoRead8(CMOS_BANK1_DATA) ==0x55)  return TRUE;
    
  #endif
#endif 

    return FALSE;
}
#endif // CMOS_MANAGER_SUPPORT

//CRB Board for Sourth bridge.
//---------------------------------------------------------------------------
/**
        
    @param Controller 
    @param ControllerInfo 
    @param SetupData 
         
        IN OUT CONTROLLER_INFO  *ControllerInfo
    @retval VOID
**/
VOID
OEM_AcousticManagement(
    IN EFI_HANDLE           Controller,
    IN OUT CONTROLLER_INFO  *ControllerInfo,
    IN SETUP_DATA           *SetupData
  )
{
}

/**
    This function could override the Sb setup data

    @param Services 
    @param SbSetupData 
    @param SetupData 
    @param Pei 

        IN OUT SB_SETUP_DATA    *SbSetupData
    @retval VOID

    @note  This routine is called very early, prior to SBPEI and NBPEI.
**/
VOID
CrbSbSetupOverride (
    IN VOID                 *Services,
    IN OUT SB_SETUP_DATA    *SbSetupData,
    IN SETUP_DATA           *SetupData,
    IN BOOLEAN              Pei
 )
{
    //porting here
}

/**
    According to the AMD reference code we should limit Max Payload
    in 128 for on-board LAN Atheros of Larne.

    @param Device 

    @retval Status EFI_SUCCESS

    @note  call before PciIo install.
**/
EFI_STATUS
LarneAtherosWorkaround (
  IN PCI_DEV_INFO *Device
  )
{
    EFI_STATUS          Status = EFI_SUCCESS;
    PCI_DEV_INFO        *dev=Device;
    EFI_PCI_IO_PROTOCOL *PciIo = &Device->PciIo;
    PCIE_DEV_CNT_REG    devc;

    Status = PciIo->Pci.Read(PciIo,
                    EfiPciIoWidthUint16,
                    (dev->PciExpress->PcieOffs+PCIE_DEV_CNT_OFFSET),
                    1,
                    &devc.DEV_CNT
                    );
    devc.MaxPayload = 0; // 128
    Status = PciIo->Pci.Write(PciIo,
                    EfiPciIoWidthUint16,
                    (dev->PciExpress->PcieOffs+PCIE_DEV_CNT_OFFSET),
                    1,
                    &devc.DEV_CNT
                    );

    return Status;
}

/**
    CRBRuntimeResetOverride

    @param ResetType 

    @retval EFI_STATUS EFI_SUCCESS

**/
EFI_STATUS
CRBRuntimeResetOverride (
  IN  EFI_RESET_TYPE  ResetType
 )
{
      //CRB board changes
      EFI_STATUS                      Status=EFI_SUCCESS;

      IoWrite8(0x80, ResetType);
      return (Status); 
}


BOOLEAN
CrbPeiProcessFvBeforePublishing(
  IN CONST EFI_PEI_SERVICES **PeiServices, 
  IN OUT AMI_ROM_AREA *Area, 
  IN UINT32 FvType
  )

{
#if !FV_BB_IN_ROM == 1
	
    if((Area->Address >= FV_BB_BASE))
        Area->Address = Area->Address & (HIGH_MEMORY_REGION_BASE | 0xFFFFFF);    
    
#endif    
    
    return TRUE;
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
