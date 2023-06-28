//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************

/** @file Dbg2Board.c

**/

//---------------------------------------------------------------------------
// Include(s)
//---------------------------------------------------------------------------
#include <AmiDxeLib.h>
#include <DebugPortTable2.h>
#include <Protocol/PciIo.h>
#include <Pci.h>

#if	DBG2_SUPPORT

//Tunnel mountain platform ECHI port0 Name space.
//#define EHCI_USB_NAME_SPACE	"\\_SB.PCI0.USBE.HUBN.PR10.PR30"

EFI_GUID gPciIoProtocolGuid	= EFI_PCI_IO_PROTOCOL_GUID;

#if EHCI_DEV_INFO_SUPPORT
/**
    Get Capability Registers offset off the BAR
		
	@retval	UINT64	Address
**/
UINT64	GetCapRegBar(VOID)
{
	EFI_STATUS	Status;
    UINTN       PciDevCount = 0;
    EFI_HANDLE  *PciDevHB;
    UINTN	Index;
    EFI_PCI_IO_PROTOCOL *PciIo;
    UINTN	Seg, Bus, Dev, Fun; 
    UINTN	EhciBDF[] = EHCI_SEG_BUS_FUN_DEVICE;
    UINT32	Address;
    UINT64	Address64;
    
	Status= pBS->LocateHandleBuffer(ByProtocol,&gPciIoProtocolGuid,NULL, &PciDevCount, &PciDevHB);
	TRACE((-1,"Dbg : Locate PCIIO protocol %r \n",Status));
	if(EFI_ERROR(Status)) return 0;
	TRACE((-1,"Dbg : How many PCI devices? Ans:%d. \n",PciDevCount));

    for(Index=0; Index< PciDevCount; Index++){
        Status=pBS->HandleProtocol(PciDevHB[Index],&gPciIoProtocolGuid,&PciIo);
        if(EFI_ERROR(Status)) {
            pBS->FreePool(PciDevHB);
            return 0;
        }
        Status = PciIo->GetLocation(PciIo, &Seg, &Bus, &Dev, &Fun);
        if(EFI_ERROR(Status)) break;
        if((Seg == EhciBDF[0])&&(Bus == EhciBDF[1])&&\
        		(Dev == EhciBDF[2])&&(Fun == EhciBDF[3])){
            Status = PciIo->Pci.Read(PciIo, 
            			EfiPciIoWidthUint32, PCI_BAR0, 1, &Address);
            if(EFI_ERROR(Status)) break;
            if((Address & (BIT1 |BIT2)) == BIT2){
				Status = PciIo->Pci.Read(PciIo, 
									EfiPciIoWidthUint32, PCI_BAR0, 
									sizeof(VOID*)/sizeof(UINT32), &Address64);
				if(EFI_ERROR(Status)) break;
				Address64 = Address64 & ~(0xb); // Clear attributes		
	            TRACE((-1,"Dbg : 64bit BAR0 [0x%lx] \n",Address64));        
	            return Address64;
            }
            TRACE((-1,"Dbg : BAR0 [0x%lx] \n",Address));        
            return Address;
        }
    }
    return 0;
}
#endif

/**
    Setting the DBG2 device information data.
	@param	pDbg2Table	Point to Default DBG2 header
	
	@retval	VOID*	Point to new DBG2 table address.
**/
VOID* SetupDebugDevideInformationData(IN DBG_PORT_2_TABLE *pDbg2Table)
{
	EFI_STATUS			Status = EFI_SUCCESS;
	VOID				*IntegrateDebugPort2Table = NULL;
	UINT16 				IntegrateDebugPort2TableSize = 0;
	UINT16  			TotalDbgDevInfoSize = 0;
	UINT16				SerialDbgDevInfoSize = 0;
	UINT16				EhciPort0DbgDevInfoSize = 0;
	UINT8   			*pDefaultNameSpace = NULL;
	UINT8				*pEhciPort0NameSpace = NULL;
	UINT16				SizeForDefaultNameSpace = 0;
	UINT16				SizeForEhciPort0NameSpace = 0;
	DBG_DEV_INFO_STRUCT	*pSerialDevInfo = NULL;
	DBG_DEV_INFO_STRUCT	*pEhciPort0DevInfo = NULL;
	GAS_30				*pSerialDBaseAddrReg = NULL;
	GAS_30				*pEhciPort0BaseAddrReg = NULL;
	UINT32				*pSerialAddrSize = NULL;
	UINT32				*pEchiPort0AddrSize = NULL;
	UINT8				*pSerialNameSpaceStr = NULL;
	UINT8				*pEchiPort0NameSpaceStr = NULL;
	
	//
	// 1. Initialize the number of device and Offset debug device Info for DBG2
	//
	pDbg2Table->NumberDbgDeviceInfo = NUMBER_DEBUG_DEVICE_INFO;
	if(pDbg2Table->NumberDbgDeviceInfo == 0)
		return (VOID*)pDbg2Table;
	
	pDbg2Table->OffsetDbgDeviceInfo = sizeof(DBG_PORT_2_TABLE);
	
	//
	// 2. Initialize the DBG2 debug device's name space string and size
	//
#if	SERIAL_DEV_INFO_SUPPORT
	//pDefaultNameSpace = DBG2_DEV_INFO_DEFAULT_NAME_SPACE;
	pDefaultNameSpace = CONVERT_TO_STRING(SERIAL_NAME_SPACE);
	SizeForDefaultNameSpace = (UINT16)AsciiStrSize(pDefaultNameSpace);
#endif
	
#if EHCI_DEV_INFO_SUPPORT	
	pEhciPort0NameSpace = CONVERT_TO_STRING(EHCI_USB_NAME_SPACE);
	SizeForEhciPort0NameSpace = (UINT16)AsciiStrSize(pEhciPort0NameSpace);
#endif
	//
	// 3. Initialize the DBG2 total size, each device info size.
	//
#if	SERIAL_DEV_INFO_SUPPORT	
	SerialDbgDevInfoSize = (UINT16)(
								sizeof(DBG_DEV_INFO_STRUCT) +	//Debug Device Information Structure
								sizeof(GAS_30) +				//Base Address Registe - Assume 1 BAR
								4 +								//Address Size
								SizeForDefaultNameSpace +		//Name Space String
								0								//Oem Data - Assume none
							);
#endif
	
#if EHCI_DEV_INFO_SUPPORT
	EhciPort0DbgDevInfoSize = (UINT16)(
								sizeof(DBG_DEV_INFO_STRUCT) +	//Debug Device Information Structure
								sizeof(GAS_30) +				//Base Address Registe - Assume 1 BAR
								4 +								//Address Size
								SizeForEhciPort0NameSpace +		//Name Space String
								0								//Oem Data - Assume none
							);
#endif	
	
	TotalDbgDevInfoSize = SerialDbgDevInfoSize + EhciPort0DbgDevInfoSize;
	
	IntegrateDebugPort2TableSize = sizeof(DBG_PORT_2_TABLE) + TotalDbgDevInfoSize;
	
	//
	// 4. Allocate memory for DBG2
	//
	Status = pBS->AllocatePool (EfiBootServicesData, IntegrateDebugPort2TableSize, &IntegrateDebugPort2Table);
	ASSERT_EFI_ERROR (Status);
	if(EFI_ERROR(Status)) return NULL;

	pBS->SetMem (IntegrateDebugPort2Table, IntegrateDebugPort2TableSize, 0);
	
	//
	// 5. Update total length
	//
	pDbg2Table->AcpiHeader.Length = IntegrateDebugPort2TableSize;
	
	//
	// 6. Copy pDbg2Table to New table pointer
	//
	pBS->CopyMem((VOID*)IntegrateDebugPort2Table,(VOID*)pDbg2Table,sizeof(DBG_PORT_2_TABLE));
	
	//  
	// 7. Set pointer to DBG2 Device Info Structure Offset
	//
	pSerialDevInfo = (DBG_DEV_INFO_STRUCT*)((UINT8*)IntegrateDebugPort2Table + pDbg2Table->OffsetDbgDeviceInfo);
	pEhciPort0DevInfo = (DBG_DEV_INFO_STRUCT*)((UINT8*)pSerialDevInfo + SerialDbgDevInfoSize);

	//
	// 8. Populate DBG2 Device Info Structure
	//
#if	SERIAL_DEV_INFO_SUPPORT
	//Serial
	pSerialDevInfo->Revision						= 0x00;
	pSerialDevInfo->Length							= SerialDbgDevInfoSize;
	pSerialDevInfo->NumberofGenericAddressRegisters = 1;
	pSerialDevInfo->NameSpaceStringLength			= SizeForDefaultNameSpace;
	pSerialDevInfo->NameSpaceStringOffset			= sizeof(DBG_DEV_INFO_STRUCT) + sizeof(GAS_30) + 4;
	pSerialDevInfo->OemDataLength                   = 0;
	pSerialDevInfo->OemDataOffset                   = 0;
	pSerialDevInfo->PortType						= DBG2_TYPE_SERIAL;
	pSerialDevInfo->PortSubtype						= DBG2_SUBTYPE_SERIAL_FULL_16550;
	pSerialDevInfo->Reserved[0]						= 0;
	pSerialDevInfo->Reserved[1]						= 0;
	pSerialDevInfo->BaseAddressRegisterOffset		= sizeof(DBG_DEV_INFO_STRUCT);
	pSerialDevInfo->AddressSizeOffset				= sizeof(DBG_DEV_INFO_STRUCT) + sizeof(GAS_30);
#endif
	
#if EHCI_DEV_INFO_SUPPORT
	//EHCI
	pEhciPort0DevInfo->Revision							= 0x00;
	pEhciPort0DevInfo->Length							= EhciPort0DbgDevInfoSize;
	pEhciPort0DevInfo->NumberofGenericAddressRegisters	= 1;
	pEhciPort0DevInfo->NameSpaceStringOffset			= sizeof(DBG_DEV_INFO_STRUCT) + sizeof(GAS_30) + 4;
	pEhciPort0DevInfo->OemDataLength					= 0;
	pEhciPort0DevInfo->OemDataOffset					= 0;
	pEhciPort0DevInfo->PortType							= DBG2_TYPE_USB;
	pEhciPort0DevInfo->PortSubtype						= DBG2_SUBTYPE_USB_EHCI;
	pEhciPort0DevInfo->Reserved[0]						= 0;
	pEhciPort0DevInfo->Reserved[1]						= 0;
	pEhciPort0DevInfo->BaseAddressRegisterOffset		= sizeof(DBG_DEV_INFO_STRUCT);
	pEhciPort0DevInfo->AddressSizeOffset				= sizeof(DBG_DEV_INFO_STRUCT) + sizeof(GAS_30);
#endif	
	//  
	// 9. Set pointer to DBG2 Device Base Address Offset and populate value
	//
	
	//Point to First Device information data and fill data
#if	SERIAL_DEV_INFO_SUPPORT
	pSerialDBaseAddrReg		= (GAS_30*)((UINT8*)pSerialDevInfo + pSerialDevInfo->BaseAddressRegisterOffset);
#endif

#if EHCI_DEV_INFO_SUPPORT	
	pEhciPort0BaseAddrReg	= (GAS_30*)((UINT8*)pEhciPort0DevInfo + pEhciPort0DevInfo->BaseAddressRegisterOffset);
#endif

#if	SERIAL_DEV_INFO_SUPPORT
	//Serial
	pSerialDBaseAddrReg->AddrSpcID		= SERIAL_DBG2_ADDR_SPC_ID;
	pSerialDBaseAddrReg->RegBitWidth	= SERIAL_DBG2_ADDR_BIT_WIDTH;
	pSerialDBaseAddrReg->RegBitOffs		= SERIAL_DBG2_ADDR_BIT_OFFSET;
	pSerialDBaseAddrReg->AccessSize		= SERIAL_DBG2_ACCESS_SIZE;
	pSerialDBaseAddrReg->Address		= PcdGet64 (PcdSerialRegisterBase);
#endif

#if EHCI_DEV_INFO_SUPPORT
	//ECHI
	pEhciPort0BaseAddrReg->AddrSpcID	= EHCIPORT0_DBG2_ADDR_SPC_ID;
	pEhciPort0BaseAddrReg->RegBitWidth	= EHCIPORT0_DBG2_ADDR_BIT_WIDTH;
	pEhciPort0BaseAddrReg->RegBitOffs	= EHCIPORT0_DBG2_ADDR_BIT_OFFSET;
	pEhciPort0BaseAddrReg->AccessSize	= EHCIPORT0_DBG2_ACCESS_SIZE;
	
	//Get Capability Registers offset off the BAR
	pEhciPort0BaseAddrReg->Address		= GetCapRegBar();
#endif
	
	//
	// 10. Set pointer to DBG2 Device Address Size Offset and populate value
	//	
#if	SERIAL_DEV_INFO_SUPPORT
	pSerialAddrSize = (UINT32*)((UINT8*)pSerialDevInfo + pSerialDevInfo->AddressSizeOffset);
	*pSerialAddrSize = 32;
#endif

#if EHCI_DEV_INFO_SUPPORT
	pEchiPort0AddrSize = (UINT32*)((UINT8*)pEhciPort0DevInfo + pEhciPort0DevInfo->AddressSizeOffset);
	*pEchiPort0AddrSize = 32;
#endif
	
	//
	// 11. Set pointer to DBG2 Device Name Space Offset and populate value
	//
#if	SERIAL_DEV_INFO_SUPPORT
	pSerialNameSpaceStr = (UINT8 *)((UINT8 *)pSerialDevInfo + pSerialDevInfo->NameSpaceStringOffset);
	pBS->CopyMem(pSerialNameSpaceStr, pDefaultNameSpace, SizeForDefaultNameSpace);
#endif

#if EHCI_DEV_INFO_SUPPORT	
	pEchiPort0NameSpaceStr = (UINT8 *)((UINT8 *)pEhciPort0DevInfo + pEhciPort0DevInfo->NameSpaceStringOffset);
	pBS->CopyMem(pEchiPort0NameSpaceStr, pEhciPort0NameSpace, SizeForEhciPort0NameSpace);
#endif
	
	return IntegrateDebugPort2Table;

}
#endif
//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************
