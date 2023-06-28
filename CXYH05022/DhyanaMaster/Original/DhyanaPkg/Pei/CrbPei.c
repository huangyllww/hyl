//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file CrbPei.c
    This file contains code for Chipset Reference Board
    Template initialization in the PEI stage

**/

//---------------------------------------------------------------------------
// Include(s)
//---------------------------------------------------------------------------
#include <PiPei.h>
#include <Ppi/CrbInfo.h>
#include <Library/PeiServicesLib.h>
#include <Library/PciLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>

#include <Token.h>
#include <AmiPeiLib.h>
#include <AmiCspLib.h>
#include <Setup.h>
#include <Sb.h>

// Consumed PPIs
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/SbPpi.h>
#include <Ppi/CrbAgesa.h>
#include <Ppi/GnbPpi.h>
#include <Ppi/SmBus2.h>

//---------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//---------------------------------------------------------------------------
// Constant Definition(s)

#ifndef MAX_ROOT_BRIDGE_COUNT
#define MAX_ROOT_BRIDGE_COUNT   8
#endif

// Macro Definition(s)

// Type Definition(s)
typedef struct {
    UINT8   BrNum;
    UINT8   BridgeDev;
    UINT8   BridgeFun;
    UINT8   BrSecBusOffs;
    UINT8   BrSubBusOffs;
    UINT32  BridgeMemRange;
    UINT8   CtrlDevNum;
    UINT8   CtrlFunNum;
    UINT32  CtrlMmioBase;
    UINT8   CtrlBarNum;
} RECOVERY_DEVICE_DATA;

#define SATA_SCKT0_DIE0  {0x00,8,1,0x04,0x04,0xDD10DD00,0,2,0xDD000000,PCI_BAR5}
#define SATA_SCKT0_DIE1  {0x01,8,1,0x04,0x04,0xDC10DC00,0,2,0xDC000000,PCI_BAR5}
#define SATA_SCKT0_DIE2  {0x02,8,1,0x04,0x04,0xDB10DB00,0,2,0xDB000000,PCI_BAR5}
#define SATA_SCKT0_DIE3  {0x03,8,1,0x04,0x04,0xDA10DA00,0,2,0xDA000000,PCI_BAR5}
#define SATA_SCKT1_DIE0  {0x04,8,1,0x04,0x04,0xD910D900,0,2,0xD9000000,PCI_BAR5}
#define SATA_SCKT1_DIE1  {0x05,8,1,0x04,0x04,0xD810D800,0,2,0xD8000000,PCI_BAR5}
#define SATA_SCKT1_DIE2  {0x06,8,1,0x04,0x04,0xD710D700,0,2,0xD7000000,PCI_BAR5}
#define SATA_SCKT1_DIE3  {0x07,8,1,0x04,0x04,0xD610D600,0,2,0xD6000000,PCI_BAR5}
#define XHCI_SCKT0_DIE0  {0x00,7,1,0x03,0x03,0xDD20DD10,0,3,0xDD100000,PCI_BAR0}
#define XHCI_SCKT0_DIE1  {0x01,7,1,0x05,0x05,0xDC20DC10,0,3,0xDC100000,PCI_BAR0}
#define XHCI_SCKT1_DIE0  {0x04,7,1,0x03,0x03,0xD920D910,0,3,0xD9100000,PCI_BAR0}
#define XHCI_SCKT1_DIE1  {0x05,7,1,0x05,0x05,0xD820D810,0,3,0xD8100000,PCI_BAR0}

RECOVERY_DEVICE_DATA SataRecDevList[] =
{   SATA_SCKT0_DIE0,
    SATA_SCKT0_DIE1,
    SATA_SCKT0_DIE2,
    SATA_SCKT0_DIE3,
    SATA_SCKT1_DIE0,
    SATA_SCKT1_DIE1,
    SATA_SCKT1_DIE2,
    SATA_SCKT1_DIE3
    };
    
RECOVERY_DEVICE_DATA XhciRecDevList[] =
{   XHCI_SCKT0_DIE0,
    XHCI_SCKT0_DIE1,
    XHCI_SCKT1_DIE0,
    XHCI_SCKT1_DIE1
    };
    
// Function Prototype(s)

    
UINT8
CrbPeiGetChipsetVendorNo (
  IN  CONST AMI_PEI_CRBINFO_PPI  *This
);

UINT32
CrbPeiGetCpuId (
  IN  CONST AMI_PEI_CRBINFO_PPI  	*This
);

UINT32
CrbPeiGetNorthBridgePciId (
  IN  CONST AMI_PEI_CRBINFO_PPI  *This
);

UINT32
CrbPeiGetSouthBridgePciId (
  IN  CONST AMI_PEI_CRBINFO_PPI  *This
);

CHAR8*
CrbPeiGetProjectId (
  IN  CONST AMI_PEI_CRBINFO_PPI  *This
);

UINT16
CrbPeiGetBiosRevision (
  IN  CONST AMI_PEI_CRBINFO_PPI  *This
);

EFI_STATUS
CrbPeiCallBack(
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
    IN  VOID                            *InvokePpi
);

EFI_STATUS
CrbRecoveryCallback(
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
    IN  VOID                            *InvokePpi
);

VOID
EnableDFMmioDecode(
    IN EFI_PEI_SERVICES **PeiServices
);



// GUID Definition(s)

//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
// Variable Declaration(s)
CHAR8 CONST ProjectTag[10] = CONVERT_TO_STRING(CRB_PROJECT_TAG);
AMI_GPIO_INIT_TABLE_STRUCT Crb1GpioTable [] = {
    //using CPM's Gpio table
    {0xffff, 0xffff}, // End of the table.
};

AMI_GPIO_INIT_TABLE_STRUCT Crb2GpioTable [] = {
    {0xffff, 0xffff}, // End of the table.
};

// PPI Definition(s)
AMI_PEI_CRBINFO_PPI	gAmiCrbInforPpi = {
	CrbPeiGetChipsetVendorNo,
	CrbPeiGetCpuId,
	CrbPeiGetNorthBridgePciId,
	CrbPeiGetSouthBridgePciId,
	CrbPeiGetProjectId,
	CrbPeiGetBiosRevision,
};

static EFI_PEI_PPI_DESCRIPTOR gCrbPpiList[] =  {
		{
        (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
		&gAmiPeiCrbInfoPpiGuid,
		&gAmiCrbInforPpi
		}
	};
AMI_GPIO_INIT_PPI Crb1GpioInitPpi = {
    0,          //GPIO_BASE_ADDRESS,
    Crb1GpioTable,
    TRUE        //InitDefaultGpioSetting
};

AMI_GPIO_INIT_PPI Crb2GpioInitPpi = {
    0,          //GPIO_BASE_ADDRESS,
    Crb2GpioTable,
    FALSE       //InitDefaultGpioSetting
};

/*
AMI_SB_PCI_SSID_TABLE_STRUCT Crb1SbSsidTable[] = { 
    CRB1_SB_PCI_DEVICES_SSID_TABLE
};

AMI_GNB_PCI_SSID_TABLE_STRUCT Crb1GnbSsidTable[] = { 
    CRB1_GNB_PCI_DEVICES_SSID_TABLE
};

AMI_SB_PCI_SSID_TABLE_STRUCT Crb2SbSsidTable[] = { 
    CRB2_SB_PCI_DEVICES_SSID_TABLE
};

AMI_GNB_PCI_SSID_TABLE_STRUCT Crb2GnbSsidTable[] = { 
    CRB2_GNB_PCI_DEVICES_SSID_TABLE
};

static AMI_PEI_SB_CUSTOM_PPI Crb1SbCustomPpi = {
    &Crb1GpioInitPpi,
    Crb1SbSsidTable
};

static AMI_PEI_GNB_CUSTOM_PPI Crb1GnbCustomPpi = {
    Crb1GnbSsidTable
};

static AMI_PEI_SB_CUSTOM_PPI Crb2SbCustomPpi = {
    &Crb2GpioInitPpi,
    Crb2SbSsidTable
};

static AMI_PEI_GNB_CUSTOM_PPI Crb2GnbCustomPpi = {
    Crb2GnbSsidTable
};

// PPI that are installed

static EFI_PEI_PPI_DESCRIPTOR Crb1CustomPpi[] = {
    { EFI_PEI_PPI_DESCRIPTOR_PPI, \
      &gAmiPeiGnbCustomPpiGuid, &Crb1GnbCustomPpi },
    { EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST, \
      &gAmiPeiSbCustomPpiGuid, &Crb1SbCustomPpi },
};

static EFI_PEI_PPI_DESCRIPTOR Crb2CustomPpi[] = {
    { EFI_PEI_PPI_DESCRIPTOR_PPI, \
      &gAmiPeiGnbCustomPpiGuid, &Crb2GnbCustomPpi },
    { EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST, \
      &gAmiPeiSbCustomPpiGuid, &Crb2SbCustomPpi },
};
*/

EFI_STATUS
CrbPeiCallback(
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
    IN  VOID                            *InvokePpi
);

EFI_STATUS
CrbPeiZpPcieDoneCallback(
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
    IN  VOID                            *InvokePpi
);

static EFI_PEI_NOTIFY_DESCRIPTOR  CrbNotifyList01[] =
{
    {(EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK
        | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gAmdNbioPcieTrainingDonePpiGuid,
    CrbPeiZpPcieDoneCallback}
};

EFI_STATUS
CrbPeiPspRecoveryCallback(
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
    IN  VOID                            *InvokePpi
);

static EFI_PEI_NOTIFY_DESCRIPTOR  CrbNotifyList02[] =
{
    {(EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK
        | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gAmdPspRecoveryDetectPpiGuid,
    CrbPeiPspRecoveryCallback}
};

static EFI_PEI_PPI_DESCRIPTOR RecoveryModePpi = {
        EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
        &gEfiPeiBootInRecoveryModePpiGuid, NULL
};

// PPI that are notified
static EFI_PEI_NOTIFY_DESCRIPTOR  CrbNotifyList[] =
{
    {(EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK),
    &gEfiPeiBootInRecoveryModePpiGuid,
    CrbRecoveryCallback},
    {(EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK
        | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gAmiPeiSbInitPolicyGuid,
    CrbPeiCallback}
};

// External Declaration(s)

// Function Definition(s)

//---------------------------------------------------------------------------

/**
    Provide the chipset vendor number.

    @param This 

    @retval 0 Unknown.
    @retval 1 Intel.
    @retval 2 Hygon.

    @note  .
**/
UINT8
CrbPeiGetChipsetVendorNo (
  IN  CONST AMI_PEI_CRBINFO_PPI  *This
  )
{
	UINT32      Index;
	UINT32      RegisterEax;
	UINT32      RegisterEbx;
	UINT32      RegisterEcx;
	UINT32      RegisterEdx;
	// Intel
	// EBX 756E6547 "uneG"
	// ECX 6C65746E "letn"
	// EDX 49656e69 "Ieni"
	// Hygon
	//EBX 68747541 "ogyH"
	//ECX 444D4163 "eniu"
	//EDX 69746E65 "neGn"
	CHAR8 CONST Intel[5] = "letnI";
	CHAR8 CONST Hygon[5] = "nogyH";
	CHAR8 MyString[5];
	UINT8 ChipsetVendorNo = 0;

	Index = 0;
	Index = AsmCpuid(Index, &RegisterEax, &RegisterEbx, &RegisterEcx, &RegisterEdx);
	MyString[4] = ((CHAR8*)&RegisterEdx)[3];
	MyString[3] = ((CHAR8*)&RegisterEcx)[0];
	MyString[2] = ((CHAR8*)&RegisterEcx)[1];
	MyString[1] = ((CHAR8*)&RegisterEcx)[2];
	MyString[0] = ((CHAR8*)&RegisterEcx)[3];

	if (CompareMem(MyString, Intel, 5) == 0) {
		ChipsetVendorNo = 1;
	} else if (CompareMem(MyString, Hygon, 5) == 0) {
		ChipsetVendorNo = 2;
	}

	return ChipsetVendorNo;
}

/**
    Provide the CPU ID.

        
    @param This 

    @retval UINT32 EAX : Version Information (Type, Family, Model, and Stepping ID)

    @note  .
**/
UINT32
CrbPeiGetCpuId (
  IN  CONST AMI_PEI_CRBINFO_PPI  	*This
  )
{
	UINT32	Index = 1;
	UINT32	RegisterEax = -1;
	// EAX : Version Information (Type, Family, Model, and Stepping ID)
	Index = AsmCpuid(Index, &RegisterEax, NULL, NULL, NULL);
	return RegisterEax;
}

/**
    Provide the PCI DID/VID of the north bridge.

    @param This 

        -1 - Undefined.
    @retval others PCI DID/VID.

    @note  .
**/
UINT32
CrbPeiGetNorthBridgePciId (
  IN  CONST AMI_PEI_CRBINFO_PPI  *This
  )
{
	UINT32 PciId = -1;

	if (This->GetChipsetVendorNo(This) == 1) {
		PciId = PciRead32(PCI_LIB_ADDRESS(0, 0, 0, 0));
	} else if (This->GetChipsetVendorNo(This) == 2){
		PciId = PciRead32(PCI_LIB_ADDRESS(0, 0, 0, 0));
	}

	return PciId;
}

/**
    Provide the PCI DID/VID of the south bridge.

    @param This 

        -1 - Undefined.
    @retval others PCI DID/VID.

    @note  .
**/
UINT32
CrbPeiGetSouthBridgePciId (
  IN  CONST AMI_PEI_CRBINFO_PPI  *This
  )
{
	UINT32 PciId = -1;

	if (This->GetChipsetVendorNo(This) == 1) {
		PciId = PciRead32(PCI_LIB_ADDRESS(0, 31, 0, 0));
	} else if (This->GetChipsetVendorNo(This) == 2){
		PciId = PciRead32(PCI_LIB_ADDRESS(0, 20, 0, 0));
	}

	return PciId;
}

/**
    Provide the project ID.

    @param This 

    @retval NULL Undefined / error.
    @retval others Project ID.

    @note  .
**/
CHAR8*
CrbPeiGetProjectId (
  IN  CONST AMI_PEI_CRBINFO_PPI  *This
  )
{
	return ProjectTag;
}

/**
    Provide the BIOS revision.

    @param This 

        -1 - Undefined / error.
    @retval others BIOS revision.

    @note  .
**/
UINT16
CrbPeiGetBiosRevision (
  IN  CONST AMI_PEI_CRBINFO_PPI  *This
  )
{
	return (CRB_PROJECT_MAJOR_VERSION << 8) | CRB_PROJECT_MINOR_VERSION;
}


/**
On Board TPM related decode
**/
VOID
CrbOnBoardSpiTpmInit ( 
		VOID
  )
{
	UINT32 Addr  = 0;		
	    	
	// Set AGPIO76 As SPI_TPM_CS_L
    RW_MEM8(ACPI_MMIO_BASE + IOMUX_BASE + FCH_GPIO_REG76, 0x01,0); //SPI_TPM_CS_L
    // Set RouteTpm2Spi.
    RW_PCI32_SB(FCH_LPC_REGA0,BIT3,0);
    // Set TPM Decode
    RW_PCI32_SB(FCH_LPC_REG7C,BIT0 + BIT2 +BIT7,0);
    Addr = READ_PCI32_SB(FCH_LPC_REGA0);
    Addr &= 0xFFFFFF00;
    // Set 33MHz
    RW_MEM8(Addr + FCH_SPI_MMIO_REG22, BIT0,0xF);
}

/**
    This function is the entry point for CRB PEIM.
    It initializes the chipset CRB in PEI phase.

    @param FileHandle Pointer to the FFS file header.
    @param PeiServices Pointer to the PEI services table.

    @retval EFI_STATUS EFI_SUCCESS

    @note  This routine is called very early, prior to SBPEI and NBPEI.
**/
EFI_STATUS
EFIAPI
CrbPeiInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
    EFI_STATUS                  Status = EFI_SUCCESS;
    
    SETUP_DATA                          SetupData;
    UINTN                               VariableSize;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI     *ReadOnlyVariable;
    EFI_GUID                            SetupGuid= SETUP_GUID;
    EFI_GUID      PciSetupGuid    = PCI_FORM_SET_GUID;
    PCI_COMMON_SETUP_DATA     PciCommon;
    
    
    Status = (**PeiServices).NotifyPpi (PeiServices, &CrbNotifyList01[0]);
    Status = (**PeiServices).NotifyPpi (PeiServices, &CrbNotifyList02[0]);

    EnableDFMmioDecode(PeiServices);
        
         
    Status = (*PeiServices)->LocatePpi(
        PeiServices,
        &gEfiPeiReadOnlyVariable2PpiGuid,
        0, NULL,
        &ReadOnlyVariable
    );
    ASSERT_PEI_ERROR(PeiServices, Status);
    
    VariableSize = sizeof(SETUP_DATA);
    Status = ReadOnlyVariable->GetVariable(ReadOnlyVariable, L"Setup", &SetupGuid,
                                           NULL, &VariableSize, &SetupData);
    
    if(EFI_ERROR(Status)){
        PEI_TRACE((-1, PeiServices, "Skip setting SMT: %r\n", Status));
        return Status;
    }
    PcdSet8(PcdPcieLinkTrainingType, (UINT8) SetupData.PcieLinkTrainingType);
    PcdSet8(PcdAmdSmtMode, (UINT8) SetupData.SmtMode);
#if (( defined(TCG_SUPPORT) && (TCG_SUPPORT!=0) ) || ( defined(TCG2Support) && (TCG2Support!=0) ))
    
    if (SetupData.TpmSupport == 1 ) {    	
    	//disable PrefetchEnSpiFromHost - Errata 1042 when dTPM (SPI or LPC) enabled
  	    RW_PCI32_SB(FCH_LPC_REGB8, 0, BIT24); 

#if SPI_TPM_SUPPORT
  	  CrbOnBoardSpiTpmInit();
#endif //SPI_TPM_SUPPORT
  	    
    }         
#endif   //(( defined(TCG_SUPPORT) && (TCG_SUPPORT!=0) ) || ( defined(TCG2Support) && (TCG2Support!=0) ))     
   

//AriSupport
    VariableSize = sizeof(PCI_COMMON_SETUP_DATA);
    Status = ReadOnlyVariable->GetVariable(ReadOnlyVariable, L"PCI_COMMON", &PciSetupGuid,
                                           NULL, &VariableSize, &PciCommon); 
    if(EFI_ERROR(Status)){
            PEI_TRACE((-1, PeiServices, "Skip setting ARI: %r\n", Status));
            return Status;
        } 
    PcdSetBool (PcdCfgPcieAriSupport, PciCommon.AriSupport);
        
    return Status;
}

/**
    This function is called at end of PEI phase. It disables the
    ROM cache to avoid the problem with flash.


    @param PeiServices Pointer to PeiServices
    @param NotifyDesc Pointer to NotifyDesc
    @param InvokePpi 

    @retval EFI_STATUS EFI_SUCCESS

**/
EFI_STATUS
CrbPeiCallback(
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
    IN  VOID                            *InvokePpi
)
{

/*
    EFI_STATUS                         	Status;
    SETUP_DATA                         	SetupData;
    UINTN                              	VariableSize;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI     *ReadOnlyVariable;
    EFI_GUID                           	SetupGuid= SETUP_GUID;

    Status = (*PeiServices)->LocatePpi(
        PeiServices,
        &gEfiPeiReadOnlyVariable2PpiGuid,
        0, NULL,
        &ReadOnlyVariable
    );
    ASSERT_PEI_ERROR(PeiServices, Status);

    VariableSize = sizeof(SETUP_DATA);
    Status = ReadOnlyVariable->GetVariable(ReadOnlyVariable, L"Setup", &SetupGuid,
                                           NULL, &VariableSize, &SetupData);*/

    //porting feature enable/disable here.
    // smi_reg hard coding
/*
    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0x02, 0x11);
    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0x08, 0x04);
    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0x09, 0x40);
    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0x0a, 0x20);
    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0x0d, 0x88);
    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0x0e, 0x09);

    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0x3c, 0x3c);

    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0x47, 0x07);
    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0x49, 0x09);
    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0x4a, 0x0a);
    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0x4b, 0x0b);
    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0x4c, 0x0c);
    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0x4d, 0x0d);
    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0x4e, 0x0e);
    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0x4f, 0x0f);

    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0x50, 0x10);
    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0x51, 0x11);
    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0x52, 0x12);
    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0x53, 0x13);
    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0x54, 0x14);
    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0x55, 0x15);
    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0x56, 0x16);
    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0x57, 0x17);
    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0x58, 0x18);
    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0x59, 0x18);
    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0x5a, 0x18);
    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0x5b, 0x18);

    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0x67, 0x1a);

    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0x70, 0x02);
    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0x78, 0x18);
    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0x79, 0x18);

    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0x98, 0xdb);
    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0x99, 0x77);

    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0xc5, 0x00);
    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0xc9, 0x00);

    WRITE_MEM8(SB_MMIO_BASE + SMI_BASE + 0xf0, 0x00);
*/

    return  EFI_SUCCESS;
}

EFI_STATUS
CrbRecoveryCallback(
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
    IN  VOID                            *InvokePpi
)
{
    EFI_STATUS                  Status = EFI_SUCCESS;
#if CRB_XHCI_RECOVERY_WORKAROUND 
    EFI_PEI_PCI_CFG2_PPI       	*PciCfg;
    EFI_PEI_CPU_IO_PPI          *CpuIo;
    UINT32 VidDid =0; //VerdID DeviceId
        
    // Get pointer to the PCI config PPI
    PciCfg = (*PeiServices)->PciCfg;
    CpuIo = (*PeiServices)->CpuIo;
    

//This workaround only for the Gardenia Rev B
//use afudos xx.rom /p /b /n /recovery . the usb3.0 key image can't be loaded
   VidDid = READ_PCI32_XHCI0(0x00);
   if (VidDid !=0xFFFFFFFF) {
      IoWrite8(0x80,0x06);//checkpoint
      IoWrite8(0xcf9,0x06);// have a cf9 reset
      EFI_DEADLOOP();
   }
#endif //CRB_XHCI_RECOVERY_WORKAROUND
    return  EFI_SUCCESS;
}

/**
    This function program MMIO ranges to access SATA and USB 
    controllers at PEI.
    Fixed memory regions:
    0xED00_0000..0xFFFF_FFFF    RB0
    0xEC00_0000..0xECFF_FFFF    RB1
    0xEB00_0000..0xEBFF_FFFF    RB2
    0xEA00_0000..0xEAFF_FFFF    RB3
    0xE900_0000..0xE9FF_FFFF    RB4
    0xE800_0000..0xE8FF_FFFF    RB5
    0xE700_0000..0xE7FF_FFFF    RB6
    0xE600_0000..0xE6FF_FFFF    RB7
        
    @param  PeiServices     Pointer to Pei Services
    @param  SystemDieCount  Number of Dies in system

    @retval VOID
*/
VOID
EnableDFMmioDecode(
    IN EFI_PEI_SERVICES **PeiServices
    )
{
    EFI_PEI_PCI_CFG2_PPI    *PciCfg;
    UINT32  Reg32;
    UINT32  MmioBase;
    UINT32  temp;
    
    UINT8   SocketNumber;
    UINT8   SocketIndex;
    UINT8   DieNumberPerSocket;
    UINT8   DieIndex;
    UINT8   MMIOIndex;
    UINT8   DFDeviceNumber;
    UINT8   TotalDieNumber;
    
    PciCfg = (*PeiServices)->PciCfg;
    
    //Read Die0 DF function 1 0x200 SystemCfg_PIE Register
    PciCfg->Read(PeiServices, \
            PciCfg, \
            EfiPciWidthUint32, 
            EFI_PEI_PCI_CFG_ADDRESS(0, HYGON_K8_NODE0_DEV, 1, 0x200), \
            &temp);
    
    //get OtherSocketPresent
    if (temp & BIT5){    
        SocketNumber = 2;
    } else {
        SocketNumber = 1;
    }
    
    //get LocalDiePresent
    temp &= 0xF; 
    switch (temp) {
        case 1:
            DieNumberPerSocket = 1;
            break;
        case 3:
            DieNumberPerSocket = 2;
            break;
        case 0xF:
            DieNumberPerSocket = 4;
            break;
    }
    
    TotalDieNumber = DieNumberPerSocket * SocketNumber;
    
    for (SocketIndex = 0; SocketIndex < SocketNumber; SocketIndex++) {
        for (DieIndex = 0; DieIndex < DieNumberPerSocket; DieIndex++) { 
            
            DFDeviceNumber = HYGON_K8_NODE0_DEV + SocketIndex * 4 + DieIndex;
            MmioBase = 0x0000DD00;
            //Configure all MMIO range for this die
            for (MMIOIndex = 0; MMIOIndex < TotalDieNumber; MMIOIndex++)
            {
                // MMIO BASE
                Reg32 = MmioBase;
                PciCfg->Write ( PeiServices, \
                     PciCfg, \
                     EfiPeiPciCfgWidthUint32, \
                     EFI_PEI_PCI_CFG_ADDRESS (0x00, DFDeviceNumber, 0x00, (0x200 + MMIOIndex * 0x10)), \
                     &Reg32);
            
                // MMIO limit
                if (MMIOIndex == 0)
                    Reg32 = 0x0000FFFF;
                else
                    Reg32 = MmioBase + 0x000000FF;
            
                PciCfg->Write ( PeiServices, \
                     PciCfg, \
                     EfiPeiPciCfgWidthUint32, \
                     EFI_PEI_PCI_CFG_ADDRESS (0x00, DFDeviceNumber, 0x00, (0x204 + MMIOIndex * 0x10)), \
                     &Reg32);
            
                // MMIO CTRL, IOMS Fabric ID
                if (MMIOIndex >= DieNumberPerSocket)
                    Reg32 = 0x00000043 + ((MMIOIndex - DieNumberPerSocket) << 9) + ((MMIOIndex/DieNumberPerSocket) << 11);
                else
                    Reg32 = 0x00000043 + (MMIOIndex << 9) + ((MMIOIndex/DieNumberPerSocket) << 11); 
                
                PciCfg->Write ( PeiServices, \
                     PciCfg, \
                     EfiPeiPciCfgWidthUint32, \
                     EFI_PEI_PCI_CFG_ADDRESS (0x00, DFDeviceNumber, 0x00, (0x208 + MMIOIndex * 0x10)), \
                     &Reg32);
            
                MmioBase -= 0x00000100;
            }
        }
    }
}

/**
    This function program Bridges and controllers behind it.
        
    @param  PeiServices Pointer to Pei Services
    @param  RecDevList  Pointer to array of RECOVERY_DEVICE_DATA records
    @param  NumDevices  Number of devices to program

    @retval VOID
*/

VOID
InitRecoveryDevices(
    IN EFI_PEI_SERVICES     **PeiServices,
    IN RECOVERY_DEVICE_DATA *RecDevList,
    IN UINT32               NumDevices
    )
{
    EFI_PEI_PCI_CFG2_PPI    *PciCfg;
    UINT32                  Reg32;
    UINT32                  i;
    UINT8                   Reg8;
    UINT8                   Bus;
    
    PciCfg = (*PeiServices)->PciCfg;
    for(i=0; i < NumDevices; i++){
        // Bridge Subordinate Bus
        // Get Bus range for Bridge0 Die0, this delta be used for remaining bridges
        PciCfg->Read ( PeiServices, \
                             PciCfg, \
                             EfiPeiPciCfgWidthUint8, \
                             PEI_PCI_CFG_ADDRESS (0, 0x18 + RecDevList[i].BrNum, 0, 0x84),\
                             &Bus);
    
        Reg8 = RecDevList[i].BrSubBusOffs + Bus;
        PciCfg->Write ( PeiServices, \
                         PciCfg, \
                         EfiPeiPciCfgWidthUint8, \
                         PEI_PCI_CFG_ADDRESS (Bus, RecDevList[i].BridgeDev, RecDevList[i].BridgeFun, PCI_SUBUS),\
                         &Reg8);
        // Bridge Secondary Bus
        Reg8 = RecDevList[i].BrSecBusOffs + Bus;
        PciCfg->Write ( PeiServices, \
                         PciCfg, \
                         EfiPeiPciCfgWidthUint8, \
                         PEI_PCI_CFG_ADDRESS (Bus, RecDevList[i].BridgeDev, RecDevList[i].BridgeFun, PCI_SBUS),\
                         &Reg8);
        // Bridge Memory range
        Reg32 = RecDevList[i].BridgeMemRange;
        PciCfg->Write ( PeiServices, \
                         PciCfg, \
                         EfiPeiPciCfgWidthUint32, \
                         PEI_PCI_CFG_ADDRESS (Bus, RecDevList[i].BridgeDev, RecDevList[i].BridgeFun, PCI_MEMBASE),\
                         &Reg32);
        // Bridge command
        Reg8 = 0x06;
        PciCfg->Write ( PeiServices, \
                         PciCfg, \
                         EfiPeiPciCfgWidthUint8, \
                         PEI_PCI_CFG_ADDRESS (Bus, RecDevList[i].BridgeDev, RecDevList[i].BridgeFun, PCI_CMD),\
                         &Reg8);
        // Controller MMIO
        Bus += RecDevList[i].BrSecBusOffs;
        Reg32 = RecDevList[i].CtrlMmioBase;
        PciCfg->Write ( PeiServices, \
                         PciCfg, \
                         EfiPeiPciCfgWidthUint32, \
                         PEI_PCI_CFG_ADDRESS (Bus, RecDevList[i].CtrlDevNum, RecDevList[i].CtrlFunNum, RecDevList[i].CtrlBarNum),\
                         &Reg32);
        // Controller CMD
        Reg8 = 0x06;
        PciCfg->Write ( PeiServices, \
                         PciCfg, \
                         EfiPeiPciCfgWidthUint8, \
                         PEI_PCI_CFG_ADDRESS (Bus, RecDevList[i].CtrlDevNum, RecDevList[i].CtrlFunNum, PCI_CMD),\
                         &Reg8);
    }
}

/**
  This function called when gAmdNbioPcieTrainingDonePpi signaled,
  to prepare Bridges, Sata, and USB controllers for recovery.
  If boot mode is not recovery - simple return.
 
  @param     PeiServices       Pointer to Pei Services
  @param     NotifyDesc        The descriptor for the notification event
  @param     InvokePpi         Pointer to the PPI in question.
 
  @retval    EFI_SUCCESS       Module initialized successfully
 */

EFI_STATUS
CrbPeiZpPcieDoneCallback(
    IN  EFI_PEI_SERVICES            **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDesc,
    IN  VOID                        *InvokePpi
)
{
    EFI_BOOT_MODE   BootMode;
    UINT32          NumDevices;
    UINT8           SystemDieCount;
    
    
    (*PeiServices)->PciCfg->Read(PeiServices,(*PeiServices)->PciCfg,
                    EfiPeiPciCfgWidthUint8, EFI_PEI_PCI_CFG_ADDRESS(0, 0x18, 1, 0x204),&SystemDieCount);
    if(SystemDieCount > MAX_ROOT_BRIDGE_COUNT) SystemDieCount = MAX_ROOT_BRIDGE_COUNT;
    
   
    (*PeiServices)->GetBootMode (PeiServices, &BootMode);
    if(BootMode != BOOT_IN_RECOVERY_MODE) return EFI_SUCCESS;

    NumDevices = sizeof(SataRecDevList)/sizeof(RECOVERY_DEVICE_DATA);
    if(NumDevices > SystemDieCount) NumDevices = SystemDieCount;
    InitRecoveryDevices(PeiServices, SataRecDevList, NumDevices);    
    
    NumDevices = sizeof(XhciRecDevList)/sizeof(RECOVERY_DEVICE_DATA);
    // Check if SP3 single socket - 4 dies , 2 XHCI
    if(SystemDieCount < 5)  NumDevices = 2;
    InitRecoveryDevices(PeiServices, XhciRecDevList, NumDevices);    

    return  EFI_SUCCESS;
}


/**
  This function called when gAmdPspRecoveryDetectPpiGuid signaled,
  to change boot mode for recovery.
 
  @param     PeiServices       Pointer to Pei Services
  @param     NotifyDesc        The descriptor for the notification event
  @param     InvokePpi         Pointer to the PPI in question.
 
  @retval    EFI_SUCCESS       Module initialized successfully
 */

EFI_STATUS
CrbPeiPspRecoveryCallback(
    IN  EFI_PEI_SERVICES            **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDesc,
    IN  VOID                        *InvokePpi
)
{
    EFI_BOOT_MODE   BootMode;
    
    (*PeiServices)->GetBootMode (PeiServices, &BootMode);
    // Main recovery already in place 
    if(BootMode == BOOT_IN_RECOVERY_MODE) return EFI_SUCCESS;
    
    BootMode = BOOT_IN_RECOVERY_MODE;
    (*PeiServices)->SetBootMode (PeiServices, BootMode);
    (*PeiServices)->InstallPpi(PeiServices, &RecoveryModePpi);
    
    return  EFI_SUCCESS;
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
