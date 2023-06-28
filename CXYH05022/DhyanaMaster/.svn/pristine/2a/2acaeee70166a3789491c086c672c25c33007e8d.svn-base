//**********************************************************************
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

//**********************************************************************
/** 
    @file AmiBoardInfo2.c
        Stores AMI Porting information (PCI; SIO; APIC) and Publishes AMI Board Info Protocol
**/
//**********************************************************************
#include <Token.h>
#include <AmiDxeLib.h>
#include <Protocol/AmiBoardInfo2.h>
#include <Library/UefiBootServicesTableLib.h>
//#include <Library/DebugLib.h>
#include <Library/AmiSdlLib.h>
#include <Library/PciLib.h>  //<duanjun-20200427-add function to query current cpu +>
//----------------------------------------------------------------------------
//Global GUIDs Definitions goes here
EFI_GUID    gAmiBoardInfo2Guid=AMI_BOARD_INFO2_PROTOCOL_GUID;
//<duanjun-20200427-support 5xx cpu && 7xx cpu +>
EFI_GUID    gAmiZpBoardInfoFileGuid = {0xA28DEA00, 0xE48E, 0x4FC8, {0x8E, 0x5C, 0x68, 0xC0, 0x72, 0x07, 0x6F, 0xD9}};
EFI_GUID    gAmiZpBoardInfoSectionGuid = {0x0A2AF255, 0xFA3C, 0x4DC8, {0x81, 0xF9, 0xF4, 0x07, 0xA9, 0x5b, 0xB0, 0xD6}};
EFI_GUID    gAmiZpAslDsdtFileGuid = {0xB3AC628B, 0x4707, 0x47E1, {0x83, 0x26, 0x24, 0xA7, 0x45, 0x7D, 0x14, 0x9F}};
//<duanjun-20200427-support 5xx cpu && 7xx cpu ->
extern UINT8 AmlCode[];

/**
    Ami Board Info 2 Protocol Instance.

**/
AMI_BOARD_INFO2_PROTOCOL gAmiBoard2Protocol = {
	///UINTN					BoardInfoVersion
	5,						//APTIO 5 version
	///BOOLEAN                 DataValid;      ///Signifies if Board Specific IRQ data was updated.
    FALSE,
    ///UINT8                   Reserved[7];
    {0,0,0,0,0,0,0},
    ///AMI_SDL_TBL_HEADER      *PciBrdData;
	NULL,
    ///AMI_SDL_TBL_HEADER      *SioBrdData;
	NULL,
    ///AMI_APIC_INFO           *ApicBrdData;
	NULL,
    ///VOID                    *BrdAcpiInfo;
    NULL,
	///UINTN                   BrdAcpiLength;
    0,
    ///VOID					*BrdAcpiIrqInfo; //Optional
    NULL,
    ///VOID					*BrdSetupInfo;
    NULL,
    ///VOID                    *BoardOemInfo;
    NULL,
    ///PCI_IRQ_PIC_ROUTE       *PicRoutTable;
    NULL,
    ///UINTN                   PicRoutLength;
    0,
    ///PCI_IRQ_APIC_ROUTE      *ApicRoutTable;
    NULL,
    ///UINTN                   ApicRoutLength;
    0
};

UINT8 *gSdlDataStart=NULL;
UINTN  gSdlDataLen=0;
UINT8 *gDsdtDataStart=NULL;
UINTN  gDsdtDataLen=0;

/**
    Ami Board Info Protocol Handle.

**/
EFI_HANDLE gAmiBoardInfo2Handle=NULL;


//This GUIDs coming from Autogen.c in build folder generated based on data from AmiModulePkg.dec and this *.inf
extern EFI_GUID gAmiBoardInfoFileGuid;      // = AMI_BOARD_INFO_OUT_FILE_GUID;
extern EFI_GUID gAmiBoardInfoSectionGuid;   // = AMI_BOARD_INFO_OUT_SECTION_GUID;
extern EFI_GUID gAmiAslDsdtFileGuid;        // = AMI_ASL_DSDT_OUT_FILE_GUID;



/**
    This Parses SDL Data Table Buffer and returnd pointer to the data table 
    with given "Signature"

         
    @param DataPtr Pointer to the SDL Data Buffer. 
    @param DataLen Total Data Buffer Length in bytes.
    @param Signature Total Data Buffer Length in bytes.
    @param TblPtr Pointer to the data table with "Signature". 
          
    @retval EFI_SUCCESS Function executed successfully,
    @retval EFI_ALREADY_STARTED driver already started
    @retval EFI_OUT_OF_RESOURCES not enough memory to perform operation

**/
EFI_STATUS ParseAmiSdlData(UINT8 *DataPtr, UINTN DataLen, UINT64 Signature, OUT VOID **TblPtr){
    UINT8   *start=DataPtr;
    UINT64  *sig;
    UINTN   i;
//-----------------
    if(start==NULL || TblPtr==NULL || DataLen==0 ) return EFI_INVALID_PARAMETER;

    for(i=0; i<DataLen-sizeof(UINT64); i++){
        sig=(UINT64*)(&start[i]);
        if(*sig == Signature) {
            *TblPtr=sig;
            return EFI_SUCCESS;
        }
    }
    return EFI_NOT_FOUND;
}

//<duanjun-20200427-add function to query current cpu +>
BOOLEAN Is2diecpu()
{
    BOOLEAN Is2die = TRUE;
    UINT32  VidDid = 0xffffffff;
    VidDid = PciRead32(PCI_LIB_ADDRESS(0, 0x1a, 0, 0x00));
    
    if(VidDid != 0xffffffff) Is2die = FALSE;
    
    DEBUG((EFI_D_ERROR, "Is2diecpu is %d\n",Is2die));
    
    return Is2die;
}
//<duanjun-20200427-add function to query current cpu ->
/**
    This function is AMI Board Info driver entry point

         
    @param ImageHandle Image handle
    @param SystemTable pointer to system table

          
    @retval EFI_SUCCESS Function executed successfully,
    @retval EFI_ALREADY_STARTED driver already started
    @retval EFI_OUT_OF_RESOURCES not enough memory to perform operation

**/
EFI_STATUS EFIAPI AmiBoardEntryPoint(
	IN EFI_HANDLE ImageHandle,
	IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS                  Status;
    AMI_BOARD_INFO2_PROTOCOL    *amibrd=NULL;
    VOID                        *DataPtr=NULL;
//<duanjun-20200427-support 5xx cpu && 7xx cpu +>	
    BOOLEAN                   Is5xx = TRUE;
//<duanjun-20200427-support 5xx cpu && 7xx cpu ->
//--------------------------------

	InitAmiLib(ImageHandle,SystemTable);

	//First try to check if we are running on MultiPlatfortm Enabled BIOS
    Status=gBS->LocateProtocol(&gAmiBoardInfo2Guid, NULL, &amibrd);
    if(!EFI_ERROR(Status)){
        TRACE((-1,"AmiBrdInfo: Multi-Platform BrdInfo present Status=%r Exiting",Status));
        return Status;
    }

    amibrd=&gAmiBoard2Protocol;
//<duanjun-20200427-support 5xx cpu && 7xx cpu +>    
    Is5xx = Is2diecpu();
    if(Is5xx){
        Status=AmiSdlReadFfsSdlData(&gSdlDataStart, &gSdlDataLen, &gAmiBoardInfoFileGuid, &gAmiBoardInfoSectionGuid);
    } else {
        Status=AmiSdlReadFfsSdlData(&gSdlDataStart, &gSdlDataLen, &gAmiZpBoardInfoFileGuid, &gAmiZpBoardInfoSectionGuid);
    }
//<duanjun-20200427-support 5xx cpu && 7xx cpu ->    
    ASSERT_EFI_ERROR(Status);
    if(EFI_ERROR(Status))return Status;


    //Init Ami Platform Info Protocol Instance...

    //1. First get PCI Data pointer AMI_PCI_DATA_SIG $PCIDATA
    Status=ParseAmiSdlData(gSdlDataStart,gSdlDataLen,AMI_PCI_DATA_SIG,&DataPtr);
    //It must be there! if it is not - ASSERT HERE.
    ASSERT_EFI_ERROR(Status);
    if(EFI_ERROR(Status)) return Status;
    amibrd->PciBrdData=(AMI_SDL_PCI_DATA*)DataPtr;


    //2. Then Get Dsdt It mus be there
    //Take care if ACPI_SUPPORT IS OFF we will not have a DSDT table in ROM
//<duanjun-20200427-support 5xx cpu && 7xx cpu +>     
    if(Is5xx){
        Status=AmiSdlReadFfsSdlData(&gDsdtDataStart, &gDsdtDataLen, &gAmiAslDsdtFileGuid, NULL);
    } else {
        Status=AmiSdlReadFfsSdlData(&gDsdtDataStart, &gDsdtDataLen, &gAmiZpAslDsdtFileGuid, NULL);
    }
  
//<duanjun-20200427-support 5xx cpu && 7xx cpu ->     
    //ASSERT_EFI_ERROR(Status);
    if(EFI_ERROR(Status)){
        TRACE((-1,"\n======================================================================================\n"));
        TRACE((-1,"AmiBrdInfo: !!!WARNING!!! Can't find DSDT Table in BIOS FV - %r.\n            !!!WARNING!!! Check your project ACPI settings...\n",Status));
        TRACE((-1,"======================================================================================\n\n"));
	} else {
    	amibrd->BrdAcpiInfo=gDsdtDataStart;
	    amibrd->BrdAcpiLength=gDsdtDataLen;
	}

    //3. Than get SIO Data Pointer...AMI_SIO_DATA_SIG $SIODATA
    DataPtr=NULL;
    Status=ParseAmiSdlData(gSdlDataStart,gSdlDataLen,AMI_SIO_DATA_SIG,&DataPtr);

    //Not A BIG deal if we can't find SIO Device. System Might be a LEGACY FREE.
    //not need to assert here //ASSERT_EFI_ERROR(Status);
    if(EFI_ERROR(Status)){
        TRACE((-1,"\n======================================================================================\n"));
        TRACE((-1,"AmiBrdInfo: !!!WARNING!!! Can't find SIO Data Table in BIOS FV - %r.\n            !!!WARNING!!! Check your project SIO settings...\n",Status));
        TRACE((-1,"======================================================================================\n\n"));
	} else 
		amibrd->SioBrdData=(AMI_SDL_SIO_DATA*)DataPtr;

    //3. Get IoApic Data Pointer...AMI_APIC_DATA_SIG  $APIDATA
    DataPtr=NULL;
    Status=ParseAmiSdlData(gSdlDataStart,gSdlDataLen,AMI_APIC_DATA_SIG,&DataPtr);
    //Not A BIG deal if we can't find SIO Device. System Might be a LEGACY FREE.
    //not need to assert here //ASSERT_EFI_ERROR(Status);
        if(EFI_ERROR(Status)){
        TRACE((-1,"\n======================================================================================\n"));
        TRACE((-1,"AmiBrdInfo: !!!WARNING!!! Can't find IOAPIC Data Table in BIOS FV - %r.\n            !!!WARNING!!! Check your project APIC and ROUTER settings...\n",Status));
        TRACE((-1,"======================================================================================\n\n"));
	} else 
		amibrd->ApicBrdData=(AMI_SDL_APIC_DATA*)DataPtr;


    //Now install AMI BOARD INFO 2 Protocol...
    Status=gBS->InstallMultipleProtocolInterfaces(
        &gAmiBoardInfo2Handle,
        &gAmiBoardInfo2Guid, &gAmiBoard2Protocol, NULL
    );

	return Status;
}

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
