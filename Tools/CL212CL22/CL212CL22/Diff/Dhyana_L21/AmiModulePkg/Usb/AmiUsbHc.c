//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file AmiUsbHc.c
    USB_HC_PROTOCOL implementation

**/

//#include <Efi.h>
#include "AmiDef.h"
#include "UsbDef.h"
#include "Uhcd.h"
#include "Xhci.h"

#include "Tree.h"
//#include "Uhci.h"
#include <Pci.h>
#include <Protocol/Cpu.h>
#include <Protocol/AmiUsbHcMemRecordProtocol.h>

extern USB_GLOBAL_DATA *gUsbData;

static UINT8 SpeedMap[] = { 0x10, 0x01, 0, 0 };

UINT8	*gUsbBusTempBuffer = NULL;
UINTN	gTempBufferPages = 0;

/**
    Function returns a pointer to HcProtocol2 record of a given protocol

**/

static HC_DXE_RECORD* GetThis(EFI_USB2_HC_PROTOCOL* Protocol)
{
    return (HC_DXE_RECORD*)(
        (char*)Protocol - (UINTN)&((HC_DXE_RECORD*)0)->hcprotocol2 );
}


/**
    Function returns a pointer to HcProtocol record of a given protocol

**/

static HC_DXE_RECORD* GetThis1 (EFI_USB_HC_PROTOCOL* Protocol)
{
    return (HC_DXE_RECORD*)(
        (char*)Protocol - (UINTN)&((HC_DXE_RECORD*)0)->hcprotocol );
}


/**
    Function converts the bitmap of gUsbData->dLastCommandStatusExtended into
    a valid USB error.

**/

static UINT32 GetTransferStatus()
{
    static struct { int BitDst, BitSrc; } ErrorMap[] = {
        EFI_USB_ERR_STALL,      USB_TRSFR_STALLED,
        EFI_USB_ERR_STALL,      USB_BULK_STALLED,
        EFI_USB_ERR_STALL,      USB_CONTROL_STALLED,
        EFI_USB_ERR_BUFFER,     USB_TRSFR_BUFFER_ERROR,
        EFI_USB_ERR_BABBLE,     USB_TRNSFR_BABBLE,
        EFI_USB_ERR_NAK,        USB_TRNSFR_NAK,
        EFI_USB_ERR_CRC,        USB_TRNSFR_CRCERROR,
        EFI_USB_ERR_TIMEOUT,    USB_TRNSFR_TIMEOUT,	//(EIP84790)
        EFI_USB_ERR_BITSTUFF,   USB_TRNSFR_BITSTUFF,
        EFI_USB_ERR_SYSTEM, 0
    };
    UINT32 Err = 0;
    UINT32 Status = gUsbData->dLastCommandStatusExtended;
    UINT8 i;

    for (i = 0; i<COUNTOF(ErrorMap); ++i){
        if( Status & ErrorMap[i].BitSrc ) {
            Err |= ErrorMap[i].BitDst;
        }
    }

    if( Status == 0 ) {
        return EFI_USB_NOERROR;
    } else {
        return Err;
    }
}

/**
    Bridge between DXE code and function in USB Core proc table which is inside
    our SMI code.

    @param 
        Func - function number opUSBCORE_XXX which corresponds to index in Core Proc table.
        Rest of the parameters coresponds the callee interface

    @retval 
        Whatever callee returns

**/

enum {
    opUSBCORE_GetDescriptor,
    opUSBCORE_ReConfigDevice,
    opUSBCORE_ReConfigDevice2,
    opUSBCORE_AllocDevInfo,
    opUSBCORE_PrepareForLegacyOS,
    opUSBCORE_ResetAndReconfigDev,
    opUSBCORE_DevDriverDisconnect,
    opUSBCORE_PeriodicEvent,
    opUSBCORE_Last,
};


int CoreprocStackSize[] = {
    6*sizeof(VOID*), // opUSBSMI_GetDescriptor
    2*sizeof(VOID*), // opUSBCORE_ReConfigDevice
    4*sizeof(VOID*), // opUSBCORE_ReConfigDevice2
    1*sizeof(VOID*), // opUSBCORE_AllocDevInfo
    1*sizeof(VOID*), // opUSBCORE_PrepareForLegacyOS
    2*sizeof(VOID*), // opUSBCORE_ResetAndReconfigDev
    2*sizeof(VOID*), // opUSBCORE_DevDriverDisconnect
    0, // opUSBCORE_PeriodicEvent
};

UINTN UsbSmiCore( UINT8 Func, ...  )
{
	EFI_STATUS	Status;
	UINTN		Index;
	UINTN		*Buffer = NULL;
    URP_STRUC   Params;
    va_list     ArgList;
    ASSERT(Func < COUNTOF(CoreprocStackSize));
//    ASSERT(CoreprocStackSize[Func] > 0);

    va_start(ArgList, Func);

	Status = gBS->AllocatePool(EfiBootServicesData, 
						CoreprocStackSize[Func], (VOID**)&Buffer);
	if (EFI_ERROR(Status)) {
        va_end(ArgList);
		return USB_ERROR;
	}

	for (Index = 0; Index < CoreprocStackSize[Func] / sizeof(VOID*); Index++) {
		Buffer[Index] = va_arg(ArgList, UINTN);
	}
	va_end(ArgList);

    Params.bFuncNumber = USB_API_CORE_PROC;
    Params.bSubFunc = Func;
	Params.bRetValue = USB_ERROR;       //(EIP91840+)
    Params.ApiData.CoreProc.paramBuffer = Buffer;
    Params.ApiData.CoreProc.paramSize = CoreprocStackSize[Func];
    Params.ApiData.CoreProc.retVal = 0; //(EIP91840+)
    USB_DEBUG(DEBUG_INFO, DEBUG_USBHC_LEVEL8,
        "call CORE SMI proc(%d); params: %x\n", Func, Params.ApiData.CoreProc.paramBuffer);

	InvokeUsbApi(&Params);

	gBS->FreePool(Buffer);
//    ASSERT(Params.bRetValue == USB_SUCCESS);

    return Params.ApiData.CoreProc.retVal;
}


/**
    SW SMI to execute GetDescriptor transfer

**/

UINT8*
UsbSmiGetDescriptor (
    HC_STRUC*   HostController,
    DEV_INFO*   Device,
    UINT8*      Buffer,
    UINT16      Length,
    UINT8       DescType,
    UINT8       DescIndex
)
{
	EFI_STATUS	Status;
	UINT8		*DataBuffer = Buffer;
	UINTN		ReturnValue;

	if (RShiftU64((UINTN)Buffer, 32)) {
		if (gUsbBusTempBuffer == NULL || Length > (gTempBufferPages << 12)) {
			if (gUsbBusTempBuffer) {
				gBS->FreePages((EFI_PHYSICAL_ADDRESS)gUsbBusTempBuffer, gTempBufferPages);
				gUsbBusTempBuffer = NULL;
				gTempBufferPages = 0;
			}
			gTempBufferPages = EFI_SIZE_TO_PAGES(Length);
			gUsbBusTempBuffer = (UINT8*)0xFFFFFFFF;
			Status = gBS->AllocatePages(AllocateMaxAddress, EfiBootServicesData,
							gTempBufferPages, (EFI_PHYSICAL_ADDRESS*)&gUsbBusTempBuffer);
			if (!EFI_ERROR(Status)) {
				SetMem (gUsbBusTempBuffer, gTempBufferPages << 12, 0);
			} else {
                gUsbBusTempBuffer = NULL;
                gTempBufferPages = 0;
			}
		}

        if (gUsbBusTempBuffer) {
		    DataBuffer = gUsbBusTempBuffer;
        }
	}
    ReturnValue = UsbSmiCore(opUSBCORE_GetDescriptor,
        HostController, Device, DataBuffer, Length, DescType, DescIndex);
    
    if (ReturnValue == 0) {
        return NULL;
    }
    
	if (DataBuffer != Buffer) {
		CopyMem (Buffer, DataBuffer, Length);
	}

	return Buffer;
}

/**
    Perform the device specific configuration

**/

UINT8
UsbSmiReConfigDevice(
    HC_STRUC*   HostController,
    DEV_INFO*   Device
)
{
    return (UINT8)UsbSmiCore(opUSBCORE_ReConfigDevice, HostController, Device);
}


/**
    Allocate the empty buffer for USB device

**/

DEV_INFO*
UsbAllocDevInfo()
{
    return (DEV_INFO*)UsbSmiCore(opUSBCORE_AllocDevInfo, 1);
}


/**
    Callback on LEGACY_BOOT event

**/

VOID UsbPrepareForLegacyOS()
{
    UsbSmiCore(opUSBCORE_PrepareForLegacyOS, 1);
}

/**


**/

UINT8
UsbResetAndReconfigDev(
    HC_STRUC*   HostController,
    DEV_INFO*   Device
)
{
    return (UINT8)UsbSmiCore(opUSBCORE_ResetAndReconfigDev, HostController, Device);
}

/**

**/

UINT8
UsbDevDriverDisconnect(
    HC_STRUC*   HostController,
    DEV_INFO*   Device
)
{
    return (UINT8)UsbSmiCore(opUSBCORE_DevDriverDisconnect, HostController, Device);
}

/**
    Bridge between DXE code and SMI function in USB HC driver.

    @param 
        Func       - function number opHC_XXX which corresponds to index in HCD_HEADER
        HcType     - type of USB HC controller; selects an HC driver to call
        Rest of the parameters coresponds the callee interface

    @retval 
        Whatever callee returns

**/

int HcprocStackSize[] = {
    sizeof(VOID*) * 1, // opHC_Start
    sizeof(VOID*) * 1, // opHC_Stop
    sizeof(VOID*) * 1, // opHC_EnumeratePorts
    sizeof(VOID*) * 1, // opHC_DisableInterrupts
    sizeof(VOID*) * 1, // opHC_EnableInterrupts
    sizeof(VOID*) * 1, // opHC_ProcessInterrupt
    sizeof(VOID*) * 3, // opHC_GetRootHubStatus
    sizeof(VOID*) * 2, // opHC_DisableRootHub
    sizeof(VOID*) * 2, // opHC_EnableRootHub
    sizeof(VOID*) * 7, // opHC_ControlTransfer
    sizeof(VOID*) * 5, // opHC_BulkTransfer
    sizeof(VOID*) * 6, // opHC_IsocTransfer
    sizeof(VOID*) * 6, // opHC_InterruptTransfer
    sizeof(VOID*) * 2, // opHC_DeactivatePolling
    sizeof(VOID*) * 2, // opHC_ActivatePolling
    sizeof(VOID*) * 3, // opHC_EnableEndpoints
    sizeof(VOID*) * 1, // opHC_DisableKeyRepeat
    sizeof(VOID*) * 1, // opHC_EnableKeyRepeat
    sizeof(VOID*) * 4, // opHC_InitDeviceData
    sizeof(VOID*) * 2, // opHC_DeinitDeviceData
    sizeof(VOID*) * 2, // opHC_ResetRootHub
    sizeof(VOID*) * 3, // opHC_ClearEndpointState
    sizeof(VOID*) * 1, // opHC_GlobalSuspend
    sizeof(VOID*) * 2, // opHC_SmiControl
};

UINTN
UsbSmiHc(
    UINT8 Func,
    UINT8 HcType,
    ...
)
{
	EFI_STATUS	Status;
	UINTN		Index = 0;
	UINTN		*Buffer = NULL;
    URP_STRUC   Params;
    va_list     ArgList;

    ASSERT(Func < sizeof(HcprocStackSize)/sizeof(int));
    ASSERT( HcprocStackSize[Func] > 0);

    va_start(ArgList, HcType);

	Status = gBS->AllocatePool(EfiBootServicesData, 
						HcprocStackSize[Func], (VOID**)&Buffer);
	if (EFI_ERROR(Status)) {
        va_end(ArgList);
		return USB_ERROR;
	}

	for (Index = 0; Index < HcprocStackSize[Func] / sizeof(VOID*); Index++) {
		Buffer[Index] = va_arg(ArgList, UINTN);
	}
	va_end(ArgList);

    Params.bFuncNumber = USB_API_HC_PROC;
    Params.bSubFunc = Func;
	Params.bRetValue = USB_ERROR;       //(EIP91840+)
    Params.ApiData.HcProc.paramBuffer = Buffer;
    Params.ApiData.HcProc.paramSize = HcprocStackSize[Func];
    Params.ApiData.HcProc.bHCType = HcType;
    Params.ApiData.HcProc.retVal = 0;   //(EIP91840+)
    USB_DEBUG(DEBUG_INFO, DEBUG_USBHC_LEVEL8,
        "call HC SMI driver(type:%d;func:%d); params at %x\n",
            HcType, Func, Params.ApiData.HcProc.paramBuffer);

	InvokeUsbApi(&Params);

	gBS->FreePool(Buffer);
//    ASSERT(Params.bRetValue == USB_SUCCESS);

    return Params.ApiData.HcProc.retVal;
}

VOID
UsbSmiPeriodicEvent(VOID)
{
//    UsbSmiCore(opUSBCORE_PeriodicEvent);
}


/**
    Wrappers for calling USB HC driver functions in USBSMI service

**/

UINT32
UsbSmiGetRootHubStatus(
    HC_STRUC    *HcStruc,
    UINT8       PortNum, 
    BOOLEAN     ClearChangeBits
)
{
    return (UINT32)UsbSmiHc(
            opHC_GetRootHubStatus, HcStruc->bHCType, HcStruc, PortNum, ClearChangeBits);
}

UINT8 UsbSmiEnableRootHub(HC_STRUC* HcStruc, UINT8 PortNum)
{
    return (UINT8)UsbSmiHc(
            opHC_EnableRootHub, HcStruc->bHCType, HcStruc, PortNum);
}

UINT8 UsbSmiDisableRootHub(HC_STRUC* HcStruc, UINT8 PortNum)
{
    return (UINT8)UsbSmiHc(
            opHC_DisableRootHub, HcStruc->bHCType, HcStruc, PortNum);
}

UINT8 UsbSmiEnableEndpoints(HC_STRUC* HcStruc, DEV_INFO* DevInfo, UINT8 *Desc)
{
    return (UINT8) UsbSmiHc(
            opHC_EnableEndpoints, HcStruc->bHCType, HcStruc, DevInfo, Desc);
}

/**
    Wrappers for calling USB HC driver USB transfer functions

**/


UINT16
UsbSmiControlTransfer (
    HC_STRUC    *HcStruc,
    DEV_INFO    *DevInfo,
    UINT16      Request,
    UINT16      Index,
    UINT16      Value,
    UINT8       *Buffer,
    UINT16      Length)
{
	EFI_STATUS	Status;
	UINT8		*DataBuffer = Buffer;
	UINTN		ReturnValue;

	if (RShiftU64((UINTN)Buffer, 32)) {
		if (gUsbBusTempBuffer == NULL || Length > (gTempBufferPages << 12)) {
			if (gUsbBusTempBuffer) {
				gBS->FreePages((EFI_PHYSICAL_ADDRESS)gUsbBusTempBuffer, gTempBufferPages);
				gUsbBusTempBuffer = NULL;
				gTempBufferPages = 0;
			}
			gTempBufferPages = EFI_SIZE_TO_PAGES(Length);
			gUsbBusTempBuffer = (UINT8*)0xFFFFFFFF;
			Status = gBS->AllocatePages(AllocateMaxAddress, EfiBootServicesData,
							gTempBufferPages, (EFI_PHYSICAL_ADDRESS*)&gUsbBusTempBuffer);
			if (!EFI_ERROR(Status)) {
				SetMem (gUsbBusTempBuffer, gTempBufferPages << 12, 0);
			} else {
                gUsbBusTempBuffer = NULL;
                gTempBufferPages = 0;
			}
		}
        if (gUsbBusTempBuffer) {
		    DataBuffer = gUsbBusTempBuffer;
        }
	}

    if (DataBuffer != Buffer) {
        if (!(Request & USB_REQ_TYPE_INPUT)) {
            CopyMem(DataBuffer, Buffer, Length);
        }
    }
    
    ReturnValue = UsbSmiHc(opHC_ControlTransfer,
        HcStruc->bHCType, HcStruc, DevInfo,
        Request, Index, Value, DataBuffer, Length);
    
	if (DataBuffer != Buffer) {
        if (Request & USB_REQ_TYPE_INPUT) {
            CopyMem(Buffer, DataBuffer, Length);
        }
	}

	return (UINT16)ReturnValue;
}

UINT32
UsbSmiBulkTransfer(
    HC_STRUC    *HcStruc,
    DEV_INFO    *DevInfo,
    UINT8       XferDir,
    UINT8       *Buffer,
    UINT32      Length
)
{
	EFI_STATUS	Status;
	UINT8		*DataBuffer = Buffer;
	UINTN		ReturnValue;

	if (RShiftU64((UINTN)Buffer, 32)) {
		if (gUsbBusTempBuffer == NULL || Length > (gTempBufferPages << 12)) {
			if (gUsbBusTempBuffer) {
				gBS->FreePages((EFI_PHYSICAL_ADDRESS)gUsbBusTempBuffer, gTempBufferPages);
				gUsbBusTempBuffer = NULL;
				gTempBufferPages = 0;
			}
			gTempBufferPages = EFI_SIZE_TO_PAGES(Length);
			gUsbBusTempBuffer = (UINT8*)0xFFFFFFFF;
			Status = gBS->AllocatePages(AllocateMaxAddress, EfiBootServicesData,
							gTempBufferPages, (EFI_PHYSICAL_ADDRESS*)&gUsbBusTempBuffer);
			if (!EFI_ERROR(Status)) {
				SetMem (gUsbBusTempBuffer, gTempBufferPages << 12, 0);
			} else {
                gUsbBusTempBuffer = NULL;
                gTempBufferPages = 0;
			}
		}
        if (gUsbBusTempBuffer) {
		    DataBuffer = gUsbBusTempBuffer;
        }
	}

	if (DataBuffer != Buffer) {
        if (!(XferDir & BIT7)) {
		    CopyMem(DataBuffer, Buffer, Length);
        }
	}
	
    ReturnValue = UsbSmiHc(opHC_BulkTransfer,
      HcStruc->bHCType, HcStruc, DevInfo, XferDir, DataBuffer, Length);
    
	if (DataBuffer != Buffer) {
        if (XferDir & BIT7) {
            CopyMem(Buffer, DataBuffer, Length);
        }
	}

	return (UINT32)ReturnValue;
}


UINT32
UsbSmiIsocTransfer(
    HC_STRUC    *HcStruc,
    DEV_INFO    *DevInfo,
    UINT8       XferDir,
    UINT8       *Buffer,
    UINT32      Length,
    UINT8       *Async
)
{
    return (UINT32)UsbSmiHc(opHC_IsocTransfer,
      HcStruc->bHCType, HcStruc, DevInfo, XferDir, Buffer, Length, Async );
/*    EFI_STATUS  Status;
    UINT8       *DataBuffer = Buffer;
    UINTN       ReturnValue;

    if (RShiftU64((UINTN)Buffer, 32)) {
        if (gUsbBusTempBuffer == NULL || Length > (gTempBufferPages << 12)) {
            if (gUsbBusTempBuffer) {
                gBS->FreePages((EFI_PHYSICAL_ADDRESS)gUsbBusTempBuffer, gTempBufferPages);
                gUsbBusTempBuffer = NULL;
                gTempBufferPages = 0;
            }
            gTempBufferPages = EFI_SIZE_TO_PAGES(Length);
            gUsbBusTempBuffer = (UINT8*)0xFFFFFFFF;
            Status = gBS->AllocatePages(AllocateMaxAddress, EfiBootServicesData,
                            gTempBufferPages, (EFI_PHYSICAL_ADDRESS*)&gUsbBusTempBuffer);
            if (!EFI_ERROR(Status)) {
                SetMem (gUsbBusTempBuffer, gTempBufferPages << 12, 0);
            } else {
                gUsbBusTempBuffer = NULL;
                gTempBufferPages = 0;
            }
        }
        if (gUsbBusTempBuffer) {
            DataBuffer = gUsbBusTempBuffer;
        }
    }

    ReturnValue = UsbSmiHc(opHC_IsocTransfer,
      HcStruc->bHCType, HcStruc, DevInfo, XferDir, DataBuffer, Length );
    if (DataBuffer != Buffer) {
        CopyMem (Buffer, DataBuffer, Length);
    }

    return (UINT32)ReturnValue;
	*/
}

UINT16
UsbSmiInterruptTransfer(
    HC_STRUC*   HcStruc,
    DEV_INFO*   DevInfo,
    UINT8       EndpointAddress,
    UINT16      MaxPktSize,
    UINT8       *Buffer,
    UINT16      Length
)
{
	EFI_STATUS	Status;
	UINT8		*DataBuffer = Buffer;
	UINTN		ReturnValue;

	if (RShiftU64((UINTN)Buffer, 32)) {
		if (gUsbBusTempBuffer == NULL || Length > (gTempBufferPages << 12)) {
			if (gUsbBusTempBuffer) {
				gBS->FreePages((EFI_PHYSICAL_ADDRESS)gUsbBusTempBuffer, gTempBufferPages);
				gUsbBusTempBuffer = NULL;
				gTempBufferPages = 0;
			}
			gTempBufferPages = EFI_SIZE_TO_PAGES(Length);
			gUsbBusTempBuffer = (UINT8*)0xFFFFFFFF;
			Status = gBS->AllocatePages(AllocateMaxAddress, EfiBootServicesData,
							gTempBufferPages, (EFI_PHYSICAL_ADDRESS*)&gUsbBusTempBuffer);
			if (!EFI_ERROR(Status)) {
				SetMem (gUsbBusTempBuffer, gTempBufferPages << 12, 0);
			} else {
                gUsbBusTempBuffer = NULL;
                gTempBufferPages = 0;
			}
		}
        if (gUsbBusTempBuffer) {
		    DataBuffer = gUsbBusTempBuffer;
        }
	}

	if (DataBuffer != Buffer) {
        if (!(EndpointAddress & BIT7)) {
		    CopyMem(DataBuffer, Buffer, Length);
        }
	}

    ReturnValue = UsbSmiHc(opHC_InterruptTransfer,
        HcStruc->bHCType, HcStruc, DevInfo, EndpointAddress, MaxPktSize, DataBuffer, Length);
    
	if (DataBuffer != Buffer) {
        if (EndpointAddress & BIT7) {
		    CopyMem(Buffer, DataBuffer, Length);
        }
	}

	return (UINT16)ReturnValue;
}


/**
    Wrappers for calling USB HC driver USB polling functions

**/

UINT8 UsbSmiDeactivatePolling (HC_STRUC* HostController, DEV_INFO* DevInfo )
{
    return (UINT8)UsbSmiHc( opHC_DeactivatePolling,
            HostController->bHCType, HostController, DevInfo );
}

UINT8   UsbSmiActivatePolling (HC_STRUC* HostController, DEV_INFO* DevInfo )
{
    return (UINT8)UsbSmiHc( opHC_ActivatePolling,
            HostController->bHCType, HostController, DevInfo );
}


/**
    Converts one bit-strng to another using a convertion table

    @param 
        Val - intial 32 bit wide bit-string
        BitT- array of bitmaptable_t recodrds
        Cnt - number of records in array BitT

         
    @retval 32 bit wide bit-string - result of conversion applied to Val

**/

struct BITMAPTABLE_T {unsigned int src; unsigned int dst;};

UINT32
ConvertBitmaps(
    UINT32 Val,
    struct BITMAPTABLE_T* BitT,
    UINT32 Cnt
)
{
    UINT32 Res = 0;
    UINT32 i;
    for( i=0; (BitT->src !=0)&& ( i < Cnt); ++i, BitT++ ){
        if( BitT->src & Val )
            Res |= BitT->dst;
    }
    return Res;
}


/**
    Enumerates DEV_INFO structures in aDevInfoTable array of USB data to find
    one that matches the specified USB address and connects to a specified USB
    host controller.

    @param 
        DEV_INFO    Start Pointer to the device info structure from
        where the search begins (if 0 start from first entry)
        DevAddr    Device address
        HcStruc    Pointer to the HCStruc structure

    @retval 
        Pointer to DEV_INFO structure, NULL if device is not found

**/

DEV_INFO*
SearchDevinfoByAdr(
    DEV_INFO*   Start,
    UINT8       DevAddr,
    HC_STRUC*   HcStruc
)
{
    DEV_INFO*   DevInfo;
    DEV_INFO*   Dev = gUsbData->aDevInfoTable + COUNTOF(gUsbData->aDevInfoTable);

    for ( DevInfo = Start!=NULL?Start:&gUsbData->aDevInfoTable[1];
            DevInfo != Dev; ++DevInfo ){
        if ((DevInfo->Flag & (DEV_INFO_VALID_STRUC | DEV_INFO_DEV_DUMMY)) ==
			DEV_INFO_VALID_STRUC) {
            if(( gUsbData->HcTable[DevInfo->bHCNumber - 1] == HcStruc) &&
                ( DevInfo->bDeviceAddress == DevAddr )){
                    return DevInfo;
            }
        }
    }
    return NULL;
}


/**
    Returns a DEV_INFO that corresponds to a device that is connected to a
    specified host controller and has a specified address

    @param 
        Addr    Device address
        Hc      Pointer to the HCStruc structure

    @retval 
        Pointer to DEV_INFO structure, NULL if device is not found

**/

DEV_INFO*
DevAddr2Info(
    UINT8 Addr,
    HC_STRUC* Hc
)
{
    DEV_INFO* Dev = SearchDevinfoByAdr(NULL, Addr, Hc);
    USB_DEBUG(DEBUG_INFO, DEBUG_USBHC_LEVEL8,
        "\tDevAddr2Info %x -> %x(hc:%x;hub:%x;port:%x;if:%x)\n",
            Addr, Dev, Dev->bHCNumber, Dev->bHubDeviceNumber,
            Dev->bHubPortNumber, Dev->bInterfaceNum );
    return Dev;
}


/**
    Searches a DEV_INFO that was used as a temporary structure for the USB transfer.

    @param 
        Device address
        EndPoint   Interrupt Endpoint number that was assigned to temporary structure
        HCStruc    Pointer to the HCStruc structure

    @retval 
        Pointer to DeviceInfo Structure NULL if device is not found

**/

DEV_INFO*
FindOldTransfer(
    UINT8 HubPortNumber,
    UINT8 HubDeviceNumber,
    UINT8 EndPoint,
    HC_STRUC* HcStruc
)
{
    DEV_INFO *Dev;
    DEV_INFO *LastDev = gUsbData->aDevInfoTable + 
                            COUNTOF(gUsbData->aDevInfoTable);

    for ( Dev = &gUsbData->aDevInfoTable[1]; Dev != LastDev; ++Dev ){
        if( ((Dev->Flag & (DEV_INFO_VALID_STRUC|DEV_INFO_DEV_PRESENT) )==
            (DEV_INFO_VALID_STRUC|DEV_INFO_DEV_PRESENT)) &&
            (Dev->bHCNumber == HcStruc->bHCNumber ) &&
            (Dev->bHubPortNumber == HubPortNumber ) &&
            (Dev->bHubDeviceNumber == HubDeviceNumber ) &&
            (Dev->IntInEndpoint == EndPoint )) 
        {
            return Dev;
        }
    }
    return NULL;
}


/**
    Allocates temporary DEV_INFO structure in USB data area for use in USB transfer.

    @param VOID
    @retval 
        Pointer to a DEV_INFO structure

**/

DEV_INFO*
AllocDevInfo()
{
    DEV_INFO* Dev = UsbAllocDevInfo();
    ASSERT(Dev);
    if(Dev){
        Dev->bDeviceType = BIOS_DEV_TYPE_USBBUS_SHADOW;
        Dev->Flag |= DEV_INFO_DEV_DUMMY;
    }
    return Dev;
}


/**
    Marks DEV_INFO structure that it is free for use in consequent operations.

**/

VOID
FreeDevInfo(
    DEV_INFO* Dev
)
{
    ASSERT(Dev);
    if (Dev) {
      Dev->Flag &= ~(DEV_INFO_VALID_STRUC |  DEV_INFO_DEV_PRESENT | DEV_INFO_DEV_DUMMY);
    }
}

UINT8
TranslateInterval(
    UINT8   Speed,
    UINTN   Interval
)
{
    UINT8  BitCount = 0;

    if (Speed == EFI_USB_SPEED_LOW ||
        Speed == EFI_USB_SPEED_FULL) {
        return (UINT8)Interval;
    }

    for (BitCount = 0; Interval != 0; BitCount++) {
        Interval >>= 1;
    }
    return (BitCount + 2);
}

/////////////////////////////////////////////////////////////////////////////
//                  USB Host Controller API functions
/////////////////////////////////////////////////////////////////////////////

/**
    Provides software reset for the USB host controller.

    @param 
        This        A pointer to the EFI_USB_HC_PROTOCOL instance. Type
        EFI_USB_HC_PROTOCOL is defined in Section 14.1.
        Attributes  A bit mask of the reset operation to perform.

    @retval 
        EFI_SUCCESS             The reset operation succeeded.
        EFI_INVALID_PARAMETER   Attributes is not valid.
        EFI_UNSUPPORTED         The type of reset specified by Attributes is
        not currently supported by the host controller
        hardware.
        EFI_DEVICE_ERROR        An error was encountered while attempting to
        perform the reset operation.

**/

EFI_STATUS
EFIAPI
AmiUsbHcReset(
    IN EFI_USB_HC_PROTOCOL  *This,
    IN UINT16               Attributes
)
{
    PROGRESS_CODE(DXE_USB_RESET);

    if (!(Attributes & (EFI_USB_HC_RESET_GLOBAL | EFI_USB_HC_RESET_HOST_CONTROLLER))) {
        return EFI_INVALID_PARAMETER;
    }

    USB_DEBUG(DEBUG_INFO, DEBUG_USBHC_LEVEL, "AmiUsbHcReset:");

    return EFI_UNSUPPORTED;
}


/**
    Protocol USB HC function that returns Host Controller state.

**/

EFI_STATUS
EFIAPI
AmiUsbHcGetState(
    IN EFI_USB_HC_PROTOCOL  *This,
    OUT EFI_USB_HC_STATE *State
)
{
    return AmiUsb2HcGetState(&GetThis1(This)->hcprotocol2, State);
}


/**
    Protocol USB HC function that sets Host Controller state.

**/

EFI_STATUS
EFIAPI
AmiUsbHcSetState(
    IN EFI_USB_HC_PROTOCOL *This,
    IN EFI_USB_HC_STATE State
)
{
    USB_DEBUG(DEBUG_INFO, DEBUG_USBHC_LEVEL, "USB HC:\t\tsetState, %d\n", State);
    return AmiUsb2HcSetState(&GetThis1(This)->hcprotocol2, State);
}


/**
    Protocol USB HC function that performs USB transfer.

**/

EFI_STATUS
EFIAPI
AmiUsbHcControlTransfer(
    IN EFI_USB_HC_PROTOCOL          *HcProtocol,
    IN     UINT8                    DeviceAddress,
    IN     BOOLEAN                  IsSlowDevice,
    IN     UINT8                    MaximumPacketLength,
    IN     EFI_USB_DEVICE_REQUEST   *Request,
    IN     EFI_USB_DATA_DIRECTION   TransferDirection,
    IN OUT VOID                     *Data,
    IN OUT UINTN                    *DataLength,
    IN     UINTN                    Timeout,
    OUT    UINT32                   *TransferResult
)
{
    return AmiUsb2HcControlTransfer(&GetThis1(HcProtocol)->hcprotocol2,
        DeviceAddress,
        IsSlowDevice?EFI_USB_SPEED_LOW:EFI_USB_SPEED_FULL,
        MaximumPacketLength, Request, TransferDirection, Data,
        DataLength, Timeout, NULL, TransferResult);
}


/**
    Protocol USB HC function that performs USB bulk transfer.

**/

EFI_STATUS 
EFIAPI 
AmiUsbHcBulkTransfer(
    IN EFI_USB_HC_PROTOCOL  *HcProtocol,
    IN  UINT8               DeviceAddress,
    IN  UINT8               EndpointAddress,
    IN  UINT8               MaximumPacketLength,
    IN OUT VOID             *Data,
    IN OUT UINTN            *DataLength,
    IN OUT UINT8            *DataToggle,
    IN  UINTN               Timeout,
    OUT UINT32              *TransferResult
)
{
    //
    // Check for valid maximum packet length is 8, 16, 32 or 64
    //
    if ( MaximumPacketLength != 8 &&
         MaximumPacketLength != 16 &&
         MaximumPacketLength != 32 &&
         MaximumPacketLength != 64 ) return EFI_INVALID_PARAMETER;

    return AmiUsb2HcBulkTransfer(&GetThis1(HcProtocol)->hcprotocol2,
        DeviceAddress, EndpointAddress, EFI_USB_SPEED_FULL,
        MaximumPacketLength, 1, &Data, DataLength,
        DataToggle, Timeout, NULL, TransferResult);
}


/**
    Protocol USB HC function that performs USB async interrupt transfer.

**/

EFI_STATUS
EFIAPI
AmiUsbHcAsyncInterruptTransfer(
  IN EFI_USB_HC_PROTOCOL                  *HcProtocol,
  IN     UINT8                            DeviceAddress,
  IN     UINT8                            EndpointAddress,
  IN     BOOLEAN                          IsSlowDevice,
  IN     UINT8                            MaxPacket,
  IN     BOOLEAN                          IsNewTransfer,
  IN OUT UINT8                            *DataToggle,
  IN     UINTN                            PollingInterval  ,
  IN     UINTN                            DataLength,
  IN     EFI_ASYNC_USB_TRANSFER_CALLBACK  CallbackFunction ,
  IN     VOID                             *Context)
{
    return AmiUsb2HcAsyncInterruptTransfer(
            &GetThis1(HcProtocol)->hcprotocol2,
            DeviceAddress, EndpointAddress,
            IsSlowDevice?EFI_USB_SPEED_LOW:EFI_USB_SPEED_FULL,
            MaxPacket, IsNewTransfer, DataToggle,
            PollingInterval, DataLength, NULL, CallbackFunction, Context);
}


/**
    Protocol USB HC function that performs USB sync interrupt transfer.

**/

EFI_STATUS
EFIAPI
AmiUsbHcSyncInterruptTransfer(
  IN EFI_USB_HC_PROTOCOL    *HcProtocol,
  IN     UINT8              DeviceAddress,
  IN     UINT8              EndpointAddress,
  IN     BOOLEAN            IsSlowDevice,
  IN     UINT8              MaximumPacketLength,
  IN OUT VOID               *Data,
  IN OUT UINTN              *DataLength,
  IN OUT UINT8              *DataToggle,
  IN     UINTN              Timeout,
  OUT    UINT32             *TransferResult
)
{
    return AmiUsb2HcSyncInterruptTransfer(
            &GetThis1(HcProtocol)->hcprotocol2,
            DeviceAddress, EndpointAddress,
            IsSlowDevice?EFI_USB_SPEED_LOW:EFI_USB_SPEED_FULL,
            MaximumPacketLength,
            Data, DataLength, DataToggle, Timeout, NULL, TransferResult );
}


/**
    Protocol USB HC function that performs USB sync isochronous transfer.

**/

EFI_STATUS
EFIAPI
AmiUsbHcIsochronousTransfer(
  IN EFI_USB_HC_PROTOCOL    *This,
  IN     UINT8              DeviceAddress,
  IN     UINT8              EndpointAddress,
  IN     UINT8              MaximumPacketLength,
  IN OUT VOID               *Data,
  IN OUT UINTN              DataLength,
  OUT    UINT32             *TransferResult
)
{
    return EFI_UNSUPPORTED;
}


/**
    Protocol USB HC function that performs USB async isochronous transfer.

**/

EFI_STATUS
EFIAPI
AmiUsbHcAsyncIsochronousTransfer(
  IN EFI_USB_HC_PROTOCOL              *This,
  IN     UINT8                        DeviceAddress,
  IN     UINT8                        EndpointAddress,
  IN     UINT8                        MaximumPacketLength,
  IN OUT VOID                         *Data,
  IN     UINTN                        DataLength,
  IN EFI_ASYNC_USB_TRANSFER_CALLBACK  IsochronousCallback,
  IN VOID                             *Context
  )
{
    return EFI_UNSUPPORTED;
}


/**
    Protocol USB HC function that returns the number of ports of a root hub
    on a given controller.

**/

EFI_STATUS
EFIAPI
AmiUsbHcGetRootHubPortNumber (
    IN EFI_USB_HC_PROTOCOL *HcProtocol,
    OUT UINT8 *PortNumber
)
{
    HC_DXE_RECORD *This = GetThis1(HcProtocol);

    USB_DEBUG(DEBUG_INFO, DEBUG_USBHC_LEVEL,
        "USB HC:\t\tget_roothub_port_number\n");

    if (PortNumber == NULL) return EFI_INVALID_PARAMETER;

    *PortNumber = This->hc_data->bNumPorts;

    return EFI_SUCCESS;
}


/**
    Protocol USB HC function that returns the root port status

**/

EFI_STATUS
EFIAPI
AmiUsbHcGetRootHubPortStatus(
    EFI_USB_HC_PROTOCOL *HcProtocol,
    UINT8               PortNumber,
    EFI_USB_PORT_STATUS *PortStatus
)
{
    HC_DXE_RECORD *This = GetThis1( HcProtocol );

    static struct BITMAPTABLE_T StatusTable[] = {
        {USB_PORT_STAT_DEV_CONNECTED,USB_PORT_STAT_CONNECTION},
        {USB_PORT_STAT_DEV_LOWSPEED,USB_PORT_STAT_LOW_SPEED},
        {USB_PORT_STAT_DEV_SUPERSPEED, USB_PORT_STAT_SUPER_SPEED},
        {USB_PORT_STAT_DEV_SUPERSPEED_PLUS, USB_PORT_STAT_SUPER_SPEED},
        {USB_PORT_STAT_DEV_ENABLED, USB_PORT_STAT_ENABLE},
        {USB_PORT_STAT_DEV_SUSPEND, USB_PORT_STAT_SUSPEND},
        {USB_PORT_STAT_DEV_OVERCURRENT, USB_PORT_STAT_OVERCURRENT},
        {USB_PORT_STAT_DEV_RESET, USB_PORT_STAT_RESET},
        {USB_PORT_STAT_DEV_POWER, USB_PORT_STAT_POWER},
        {USB_PORT_STAT_DEV_OWNER, USB_PORT_STAT_OWNER}
    };
    static struct BITMAPTABLE_T ChangeStatusMapTable[] = {
        {USB_PORT_STAT_DEV_CONNECT_CHANGED,USB_PORT_STAT_C_CONNECTION},
        {USB_PORT_STAT_DEV_ENABLE_CHANGED, USB_PORT_STAT_C_ENABLE},
        {USB_PORT_STAT_DEV_OVERCURRENT_CHANGED, USB_PORT_STAT_C_OVERCURRENT},
        {USB_PORT_STAT_DEV_RESET_CHANGED, USB_PORT_STAT_C_RESET}
    };

    UINT32  PortSts;

    USB_DEBUG(DEBUG_INFO, DEBUG_USBHC_LEVEL, "USB HC:\t\tget_roothub_port_status\n" );

    if (PortStatus == NULL || PortNumber >= This->hc_data->bNumPorts)
        return EFI_INVALID_PARAMETER;

    PortSts = UsbSmiGetRootHubStatus(This->hc_data,(UINT8)PortNumber + 1, FALSE);

    PortStatus->PortStatus = (UINT16)ConvertBitmaps(
            PortSts, StatusTable, COUNTOF(StatusTable));
    PortStatus->PortChangeStatus = (UINT16)ConvertBitmaps(
            PortSts, ChangeStatusMapTable, COUNTOF(ChangeStatusMapTable)) ;

    if (((PortSts & USB_PORT_STAT_DEV_CONNECTED) != 0) &&
        ((PortSts & USB_PORT_STAT_DEV_SPEED_MASK) == 0)) {
        PortStatus->PortStatus |= USB_PORT_STAT_HIGH_SPEED;
    }

    USB_DEBUG(DEBUG_INFO, DEBUG_USBHC_LEVEL, "\t\tStatus=%x, PortStatus=%x, PortChangeStatus=%x\n",
        PortSts, PortStatus->PortStatus, PortStatus->PortChangeStatus );

    return EFI_SUCCESS;
}


/**
    Protocol USB HC function set root hub port feature

**/

EFI_STATUS
EFIAPI
AmiUsbHcSetRootHubPortFeature(
    IN EFI_USB_HC_PROTOCOL  *HcProtocol,
    IN UINT8                PortNumber,
    IN EFI_USB_PORT_FEATURE PortFeature
)
{
    HC_DXE_RECORD *This = GetThis1(HcProtocol);

    USB_DEBUG(DEBUG_INFO, DEBUG_USBHC_LEVEL, "USB HC:\t\tset_roothub_port_feature\n" );

    if ( PortNumber >= This->hc_data->bNumPorts)
        return EFI_INVALID_PARAMETER;

    switch( PortFeature ){
        case EfiUsbPortEnable:
            UsbSmiEnableRootHub(This->hc_data, PortNumber + 1);
            break;
    
        default:
            return EFI_INVALID_PARAMETER;
    }

    return EFI_SUCCESS;
}


/**
    Protocol USB HC function clear root hub port feature

**/

EFI_STATUS
EFIAPI
AmiUsbHcClearRootHubPortFeature(
    IN EFI_USB_HC_PROTOCOL    *HcProtocol,
    IN UINT8                  PortNumber,
    IN EFI_USB_PORT_FEATURE   PortFeature
)
{
    HC_DXE_RECORD *This = GetThis1(HcProtocol);

    USB_DEBUG(DEBUG_INFO, DEBUG_USBHC_LEVEL, "USB HC:\t\tclear_roothub_port_feature\n");

    if (PortNumber >= This->hc_data->bNumPorts) return EFI_INVALID_PARAMETER;

    switch (PortFeature ) {
        case EfiUsbPortEnable:
            UsbSmiDisableRootHub(This->hc_data, PortNumber + 1);
            break;
    
        default:
            return EFI_INVALID_PARAMETER;
    }
    return EFI_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////
//                  USB2 Host Controller API functions
/////////////////////////////////////////////////////////////////////////////

/**
    This is USB2HC API to get the host controller capability.

**/

EFI_STATUS
EFIAPI
AmiUsb2HcGetCapability(
  IN  EFI_USB2_HC_PROTOCOL  *This,
  OUT UINT8                 *MaxSpeed,
  OUT UINT8                 *PortNumber,
  OUT UINT8                 *Is64BitCapable
)
{
    HC_DXE_RECORD *Rec = GetThis(This);
    USB3_HOST_CONTROLLER    *Usb3Hc;

    if (MaxSpeed == NULL || PortNumber == NULL || Is64BitCapable == NULL) {
        return EFI_INVALID_PARAMETER;
    }

    switch (Rec->hc_data->bHCType) {
        case USB_HC_OHCI:
        case USB_HC_UHCI:
            *Is64BitCapable = FALSE;
            *MaxSpeed = EFI_USB_SPEED_FULL;
            break;
        case USB_HC_EHCI:
            *Is64BitCapable = FALSE;
            *MaxSpeed = EFI_USB_SPEED_HIGH;
            break;
        case USB_HC_XHCI:
            Usb3Hc = (USB3_HOST_CONTROLLER*)Rec->hc_data->usbbus_data;
            if (Usb3Hc->CapRegs.HccParams1.Ac64) {
                *Is64BitCapable = TRUE;
            } else {
                *Is64BitCapable = FALSE;
            }
            *MaxSpeed = EFI_USB_SPEED_SUPER;
            break;
    }

    *PortNumber = Rec->hc_data->bNumPorts;

    return EFI_SUCCESS;
}


/**
    This is USB2HC API to perform host controller reset.

**/

EFI_STATUS
EFIAPI
AmiUsb2HcReset(
  IN EFI_USB2_HC_PROTOCOL   *This,
  IN UINT16                 Attributes
)
{
    PROGRESS_CODE(DXE_USB_RESET);

    if (Attributes == 0 || (Attributes & ~(EFI_USB_HC_RESET_GLOBAL
        | EFI_USB_HC_RESET_HOST_CONTROLLER | EFI_USB_HC_RESET_GLOBAL_WITH_DEBUG
        | EFI_USB_HC_RESET_HOST_WITH_DEBUG))==0 )
    {
        return EFI_INVALID_PARAMETER;
    }

    return EFI_UNSUPPORTED;
}


/**
    This is USB2HC API to get the host controller state.

**/

EFI_STATUS
EFIAPI
AmiUsb2HcGetState(
  IN  EFI_USB2_HC_PROTOCOL    *This,
  OUT EFI_USB_HC_STATE        *State
)
{

    HC_DXE_RECORD *Rec = GetThis(This);
    
    if (State == NULL) {
        return EFI_INVALID_PARAMETER;
    }

    if (Rec->hc_data->dHCFlag & HC_STATE_RUNNING) {
        *State = EfiUsbHcStateOperational;
        return EFI_SUCCESS;
    }

    if (Rec->hc_data->dHCFlag & HC_STATE_SUSPEND) {
        *State = EfiUsbHcStateSuspend;
        return EFI_SUCCESS;
    }

    *State = EfiUsbHcStateHalt;

    return EFI_SUCCESS;
}


/**
    This is USB2HC API to set the host controller state.
    @param  This    Pointer to EFI_USB2_HC_PROTOCOL
            State   USB host controller state

    @retval EFI_STATUS Status of the operation
**/

EFI_STATUS
EFIAPI
AmiUsb2HcSetState(
  IN EFI_USB2_HC_PROTOCOL    *This,
  IN EFI_USB_HC_STATE        State
)
{
    HC_DXE_RECORD       *Rec = GetThis(This);
    EFI_USB_HC_STATE    CurrentState;
    EFI_STATUS          Status;
    UINT8               UsbStatus = USB_SUCCESS;

    Status = AmiUsb2HcGetState(This, &CurrentState);

    if (EFI_ERROR(Status)) {
        return EFI_DEVICE_ERROR;
    }

    if (CurrentState == State) {
        return Status;
    }

    switch (State) {
        case EfiUsbHcStateHalt:
            UsbStatus = (UINT8)UsbSmiHc(opHC_Stop, Rec->hc_data->bHCType, Rec->hc_data);
            break;
        case EfiUsbHcStateOperational:
            UsbStatus = (UINT8)UsbSmiHc(opHC_Start, Rec->hc_data->bHCType, Rec->hc_data);
            break;
        case EfiUsbHcStateSuspend:
            UsbStatus = (UINT8)UsbSmiHc(opHC_GlobalSuspend, Rec->hc_data->bHCType, Rec->hc_data);
            break;
        default:
            Status = EFI_INVALID_PARAMETER;
            break;
    }

    if (UsbStatus != USB_SUCCESS) {
        Status = EFI_DEVICE_ERROR;
    }
    
    return Status;
}

/**
    This function checks if queue has a new transfer. If yes, calls a
    callback with data from transfer.

**/

VOID
EFIAPI
AsyncInterruptOnTimer (
    EFI_EVENT   Event,
    VOID        *Ctx
)
{
    USBHC_INTERRUPT_DEVNINFO_T *Idi = (USBHC_INTERRUPT_DEVNINFO_T*)Ctx;
    UINT8   Lock;
    VOID    *Data;

    //
    // Check re-entrance
    //
    ATOMIC({Lock = Idi->Lock; Idi->Lock = 1;});
    if (Lock) {
        return; //control is already inside
    }

    while ((UINTN)QueueSize(&Idi->QCompleted) >= Idi->DataLength) {
    ATOMIC(Data = QueueRemoveMsg(&Idi->QCompleted, (int)Idi->DataLength));
        Idi->CallbackFunction(
            Data, Idi->DataLength, Idi->Context, EFI_USB_NOERROR);
    }

    Idi->Lock = 0;
}


/**
    This function performs USB2 HC Bulk Transfer

**/

EFI_STATUS
EFIAPI
AmiUsb2HcBulkTransfer(
    IN EFI_USB2_HC_PROTOCOL *HcProtocol,
    IN  UINT8       DeviceAddress,
    IN  UINT8       EndpointAddress,
    IN  UINT8       DeviceSpeed,
    IN  UINTN       MaximumPacketLength,
    IN  UINT8       DataBuffersNumber,
    IN OUT VOID     *Data[EFI_USB_MAX_BULK_BUFFER_NUM],
    IN OUT UINTN    *DataLength,
    IN OUT UINT8    *DataToggle,
    IN  UINTN       Timeout,
    IN EFI_USB2_HC_TRANSACTION_TRANSLATOR *Translator,
    OUT UINT32      *TransferResult
)
{
    UINT32  SmiRes;
    HC_DXE_RECORD *This = GetThis( HcProtocol );
    DEV_INFO* DevInfo;
    DEV_INFO* DevSrc = DevAddr2Info( DeviceAddress, This->hc_data );
    UINT8 XferDir = 0;
    UINT16 CurrentTimeout;
	UINT8 ToggleBit = (EndpointAddress & 0xF) - 1;

    //
    // Check Params
    //
    if (DevSrc == NULL || Data == NULL || Data[0] == NULL ||
        (*DataToggle != 0 && *DataToggle != 1) ||
        (DeviceSpeed != EFI_USB_SPEED_SUPER && DeviceSpeed != EFI_USB_SPEED_HIGH 
        && DeviceSpeed != EFI_USB_SPEED_FULL) ||
        *DataLength == 0 || TransferResult == NULL ||
        MaximumPacketLength == 0 ) {
        return EFI_INVALID_PARAMETER;
    }

    //
    // Check for valid maximum packet length
    //
    if (DeviceSpeed == EFI_USB_SPEED_SUPER && MaximumPacketLength > 1024) {
        return EFI_INVALID_PARAMETER;
    }
    if (DeviceSpeed == EFI_USB_SPEED_HIGH && MaximumPacketLength > 512) {
        return EFI_INVALID_PARAMETER;
    }
    if (DeviceSpeed == EFI_USB_SPEED_FULL && MaximumPacketLength > 64) {
        return EFI_INVALID_PARAMETER;
    }

    //
    // Alloc DEV_INFO
    //
    DevInfo = AllocDevInfo();

    if (DevInfo == NULL) {
        return EFI_OUT_OF_RESOURCES;
    }

    //
    // Fill DEV_INFO
    //
    DevInfo->bDeviceAddress = DeviceAddress;
    DevInfo->bHCNumber = This->hc_data->bHCNumber;
    DevInfo->bEndpointSpeed = SpeedMap[DeviceSpeed];
    DevInfo->bHubDeviceNumber = DevSrc->bHubDeviceNumber;
    DevInfo->bHubPortNumber = DevSrc->bHubPortNumber;
    DevInfo->DevMiscInfo = DevSrc->DevMiscInfo;	//(EIP84790+)

    if (EndpointAddress & 0x80) {
        XferDir = 0x80;
        DevInfo->bBulkInEndpoint = EndpointAddress & 0xF;
        DevInfo->wBulkInMaxPkt = (UINT16)MaximumPacketLength;
        DevInfo->wDataInSync = (UINT16)(*DataToggle) << ToggleBit;
    } else {
        XferDir = 0x0;
        DevInfo->bBulkOutEndpoint = EndpointAddress & 0xF;
        DevInfo->wBulkOutMaxPkt = (UINT16)MaximumPacketLength;
        DevInfo->wDataOutSync = (UINT16)(*DataToggle) << ToggleBit;
    }

    //
    // Call SMI routine and retrieve last status
    // if any error
    //
    CRITICAL_CODE( EFI_TPL_NOTIFY,
    {
        CurrentTimeout = gUsbData->wTimeOutValue;
        gUsbData->wTimeOutValue = (UINT16)Timeout;
        gUsbData->dLastCommandStatusExtended = 0;
        SmiRes = UsbSmiBulkTransfer(
            This->hc_data, DevInfo,
            XferDir,
            (UINT8*)Data[0],
            (UINT32)*DataLength );
        *TransferResult = GetTransferStatus();
        gUsbData->wTimeOutValue = CurrentTimeout;
    });

    //
    // Update the data length
    //
    *DataLength = (UINTN)SmiRes;

    //
    // Update Toggle bit
    //
    if (XferDir) {
        *DataToggle = (UINT8)(DevInfo->wDataInSync >> ToggleBit) & 0x1;
    } else {
        *DataToggle = (UINT8)(DevInfo->wDataOutSync >> ToggleBit) & 0x1;
    }
    FreeDevInfo(DevInfo);

    if ((*TransferResult) & EFI_USB_ERR_TIMEOUT) {
        return EFI_TIMEOUT;
    }
    return (*TransferResult)? EFI_DEVICE_ERROR:EFI_SUCCESS;
}


/**
    This function performs USB2 HC Async Interrupt Transfer
    @param  EFI_USB2_HC_PROTOCOL    Pointer to EFI_USB2_HC_PROTOCOL
            DeviceAddress           Device address
            EndpointAddress         Address of Endpoint
            DeviceSpeed             Speed of Device
            MaxPacket               Max Packet of Device
            IsNewTransfer           Check is new transfer or not
            DataToggle              Data toggle
            PollingInterval         Polling interval
            DataLength              Data length
            Translator              Point to usb2 hc transaction translator
            CallbackFunction        Interrupt call back function
            Context                 Point to transfer context

    @retval EFI_STATUS Status of the operation    
**/

EFI_STATUS
EFIAPI
AmiUsb2HcAsyncInterruptTransfer(
    IN EFI_USB2_HC_PROTOCOL *HcProtocol,
    IN UINT8                DeviceAddress,
    IN UINT8                EndpointAddress,
    IN UINT8                DeviceSpeed,
    IN UINTN                MaxPacket,
    IN BOOLEAN              IsNewTransfer,
    IN OUT UINT8            *DataToggle,
    IN UINTN                PollingInterval,
    IN UINTN                DataLength,
    IN     EFI_USB2_HC_TRANSACTION_TRANSLATOR *Translator,
    IN EFI_ASYNC_USB_TRANSFER_CALLBACK  CallbackFunction ,
    IN VOID                 *Context
)
{
    HC_DXE_RECORD *This = GetThis( HcProtocol );
    DEV_INFO* DevInfo;
    DEV_INFO* DevInfoSrc = DevAddr2Info( DeviceAddress, This->hc_data );
    USBHC_INTERRUPT_DEVNINFO_T* AsyncTransfer = 0;
    UINT8 SmiStatus = USB_SUCCESS;
    UINT8 ToggleBit = (EndpointAddress & (BIT0 | BIT1 | BIT2 | BIT3))-1;
	EFI_STATUS  Status;

    if (DeviceSpeed != EFI_USB_SPEED_SUPER && 
		DeviceSpeed != EFI_USB_SPEED_HIGH &&
		DeviceSpeed != EFI_USB_SPEED_FULL &&
		DeviceSpeed != EFI_USB_SPEED_LOW){
		return EFI_INVALID_PARAMETER;
    }

    if (!(EndpointAddress & BIT7)) {
		return EFI_INVALID_PARAMETER;
    }

    if (IsNewTransfer){

        if( DevInfoSrc == NULL ) {
            return EFI_INVALID_PARAMETER;
        }

        DevInfo = FindOldTransfer( DevInfoSrc->bHubPortNumber, 
                                   DevInfoSrc->bHubDeviceNumber, 
                                   EndpointAddress, This->hc_data );

		if (DataLength == 0) {
			 return EFI_INVALID_PARAMETER;
		}

	    if (*DataToggle != 0 && *DataToggle != 1) {
	        return EFI_INVALID_PARAMETER;
	    }

		if (PollingInterval < 1 || PollingInterval > 255) {     //Input polling interval should be less than 255
			return EFI_INVALID_PARAMETER;                       //and should be greater than 1, or return
		}                                                       //invalid parameter
	
        if( CallbackFunction == NULL) {
            return EFI_INVALID_PARAMETER;
        }
        if( DevInfo != NULL ){
            USB_DEBUG(DEBUG_ERROR, DEBUG_LEVEL_3,"Stacked AsyncInterrupt request are not supported\n");
            return EFI_INVALID_PARAMETER;
        }
        DevInfo = AllocDevInfo();
        *DevInfo = *DevInfoSrc;
        DevInfo->IntInEndpoint = EndpointAddress;
        DevInfo->bEndpointSpeed = SpeedMap[DeviceSpeed];
        DevInfo->IntInMaxPkt = (UINT16)MaxPacket;    //(EIP84790+)
        DevInfo->bPollInterval = TranslateInterval(DeviceSpeed, PollingInterval);
        DevInfo->PollingLength = (UINT16)DataLength;

        //create new transfer
        gBS->AllocatePool (EfiBootServicesData,
            sizeof(USBHC_INTERRUPT_DEVNINFO_T) + DataLength*INTERRUPTQUEUESIZE,
            (VOID**)&AsyncTransfer );
        SetMem(AsyncTransfer, sizeof(USBHC_INTERRUPT_DEVNINFO_T)+
                    DataLength*INTERRUPTQUEUESIZE, 0);
        DevInfo->pExtra = (UINT8*)AsyncTransfer;
        AsyncTransfer->QCompleted.data = (VOID *volatile *)AsyncTransfer->Data;
        AsyncTransfer->QCompleted.maxsize = (int)DataLength * INTERRUPTQUEUESIZE;
        AsyncTransfer->DataLength = DataLength;
        AsyncTransfer->EndpointAddress = EndpointAddress;

        DevInfo->Flag |= DEV_INFO_DEV_DUMMY;
        
        Status = gBS->CreateEvent(EFI_EVENT_TIMER | EFI_EVENT_NOTIFY_SIGNAL,
            EFI_TPL_CALLBACK, AsyncInterruptOnTimer,
            AsyncTransfer,&AsyncTransfer->Event);
        
        ASSERT_EFI_ERROR(Status);
        
        if (EFI_ERROR(Status)) {
            return Status;
        }

		PollingInterval = PollingInterval < 32 ? 32 : PollingInterval;      //Polling interval for setTimer()
		                                                                    //should be greater than 32
        Status = gBS->SetTimer(AsyncTransfer->Event, TimerPeriodic,
                PollingInterval * MILLISECOND);
                
        ASSERT_EFI_ERROR(Status);
                
        if (EFI_ERROR(Status)) {
            return Status;
        }
        
        AsyncTransfer->CallbackFunction = CallbackFunction;
        AsyncTransfer->Context = Context;
		if(EndpointAddress & BIT7) {
	        DevInfo->wDataInSync = (UINT16)(*DataToggle) << ToggleBit;
		} else {
			DevInfo->wDataOutSync = (UINT16)(*DataToggle) << ToggleBit;
		}

        //
        // Activate transfer
        //
        SmiStatus = UsbSmiActivatePolling(This->hc_data, DevInfo);
        ASSERT(SmiStatus==USB_SUCCESS);
    } else {
        //
        // Find old transfer
        //
        DevInfo = FindOldTransfer(DevInfoSrc->bHubPortNumber, 
                                  DevInfoSrc->bHubDeviceNumber,
                                  EndpointAddress, This->hc_data );
        if( DevInfo == NULL  || DevInfo->pExtra == NULL ){
            USB_DEBUG(DEBUG_ERROR, DEBUG_LEVEL_3, "Canceling bad AsyncInterrupt request\n");
            return EFI_INVALID_PARAMETER;
        }
        AsyncTransfer = (USBHC_INTERRUPT_DEVNINFO_T*)DevInfo->pExtra;
        DevInfo->pExtra = 0;
        // 
        // Deactivate transfer
        //
        SmiStatus = UsbSmiDeactivatePolling (This->hc_data, DevInfo);
        if (DataToggle){
			if(EndpointAddress & BIT7) {
				*DataToggle = (UINT8)(DevInfo->wDataInSync >> ToggleBit) & BIT0;
			} else {
				*DataToggle = (UINT8)(DevInfo->wDataOutSync >> ToggleBit) & BIT0;
			}
        }
        gBS->SetTimer(AsyncTransfer->Event, TimerCancel, 0);
        gBS->CloseEvent(AsyncTransfer->Event);
        gBS->FreePool(AsyncTransfer);
        FreeDevInfo(DevInfo);
    }

    return SmiStatus == USB_SUCCESS? EFI_SUCCESS : EFI_DEVICE_ERROR;
}


/**
    This function performs USB2 HC Sync Interrupt Transfer
    @param  EFI_USB2_HC_PROTOCOL    Pointer to EFI_USB2_HC_PROTOCOL
            DeviceAddress           Device address
            EndpointAddress         Address of Endpoint
            DeviceSpeed             Device speed
            MaxPacket               Max packet of device
            Data                    Point to interrupt transfer data
            DataLength              Point to interrupt transfer data length
            DataToggle              Point to interrupt transfer data toggle
            Timeout                 Time out value
            Translator              Point to usb2 hc transaction translator
            TransferResult          Point to transfer result

    @retval EFI_STATUS Status of the operation
**/

EFI_STATUS
EFIAPI
AmiUsb2HcSyncInterruptTransfer(
    IN EFI_USB2_HC_PROTOCOL    *HcProtocol,
    IN     UINT8                DeviceAddress,
    IN     UINT8                EndpointAddress,
    IN     UINT8                DeviceSpeed,
    IN     UINTN                MaximumPacketLength,
    IN OUT VOID                 *Data,
    IN OUT UINTN                *DataLength,
    IN OUT UINT8                *DataToggle,
    IN     UINTN                Timeout,
    IN     EFI_USB2_HC_TRANSACTION_TRANSLATOR *Translator,
    OUT    UINT32                *TransferResult
)
{
    UINT16      SmiRes;
    HC_DXE_RECORD *This = GetThis( HcProtocol );
    DEV_INFO    *DevInfo;
    DEV_INFO*   DevSrc = DevAddr2Info( DeviceAddress, This->hc_data );
    UINT16      CurrentTimeout;
    UINT8       ToggleBit = (EndpointAddress & (BIT0 | BIT1 | BIT2 | BIT3))-1;
	UINT16		*wDataSync;

    if (DeviceSpeed != EFI_USB_SPEED_SUPER && 
		DeviceSpeed != EFI_USB_SPEED_HIGH && 
		DeviceSpeed != EFI_USB_SPEED_FULL && 
		DeviceSpeed != EFI_USB_SPEED_LOW) {
		return EFI_INVALID_PARAMETER;
    }

#if !defined(EFI_USB_HC_INTERRUPT_OUT_SUPPORT) || !EFI_USB_HC_INTERRUPT_OUT_SUPPORT
    if (!(EndpointAddress & BIT7)) {
		return EFI_INVALID_PARAMETER;
    }
#endif

    if (Data == NULL || DataLength == NULL ||
		*DataLength == 0 || TransferResult == NULL) {
        return EFI_INVALID_PARAMETER;
    }

    if ( DeviceSpeed == EFI_USB_SPEED_LOW  && MaximumPacketLength != 8) {   //Device is low speed and maximum packet
        return EFI_INVALID_PARAMETER;                                       //should be 8, or return invalid parameter
    }

    if (DeviceSpeed == EFI_USB_SPEED_FULL &&            //Device is full speed and maximum packet should be
		(MaximumPacketLength != 8 &&                    // 8/16/32/64 , or return invalid parameter
		MaximumPacketLength != 16 &&
		MaximumPacketLength != 32 &&
		MaximumPacketLength != 64)) {
		return EFI_INVALID_PARAMETER;
    }

    if (DeviceSpeed == EFI_USB_SPEED_HIGH && MaximumPacketLength > 3072 ) { //Device is high speed and maximum packet
        return EFI_INVALID_PARAMETER;                                       //should be less than 3072, or return
    }                                                                       //invalid parameter

	if (*DataToggle != 0 && *DataToggle != 1) return EFI_INVALID_PARAMETER;
	
    if (DevSrc == NULL) {
        return EFI_INVALID_PARAMETER;
    }

    DevInfo = AllocDevInfo();
    DevInfo->bDeviceAddress = DeviceAddress;
    DevInfo->bHCNumber = This->hc_data->bHCNumber;
    DevInfo->bEndpointSpeed = SpeedMap[DeviceSpeed];
    DevInfo->bPollInterval = TranslateInterval(DeviceSpeed, 1);
    DevInfo->bHubDeviceNumber = DevSrc->bHubDeviceNumber;
    DevInfo->bHubPortNumber = DevSrc->bHubPortNumber;
	DevInfo->DevMiscInfo = DevSrc->DevMiscInfo;	//(EIP84790+)

	wDataSync = (EndpointAddress & BIT7) ? &DevInfo->wDataInSync : 
		&DevInfo->wDataOutSync;

	*wDataSync = (UINT16)(*DataToggle) << ToggleBit;

    CRITICAL_CODE( EFI_TPL_NOTIFY,
    {
        CurrentTimeout = gUsbData->wTimeOutValue;
        gUsbData->wTimeOutValue = (UINT16)Timeout;
        gUsbData->dLastCommandStatusExtended = 0;
        SmiRes = UsbSmiInterruptTransfer(
            This->hc_data,
            DevInfo,
            EndpointAddress,
            (UINT16)MaximumPacketLength,
            (UINT8*)Data,
            (UINT16)*DataLength);
        *TransferResult = GetTransferStatus();
        gUsbData->wTimeOutValue = CurrentTimeout;
    });

    *DataLength = (UINTN)SmiRes;
    *DataToggle = (UINT8)(*wDataSync >> ToggleBit) & BIT0;

    FreeDevInfo(DevInfo);
    //
    // Return with error or success
    //
    if ( (*TransferResult) &  EFI_USB_ERR_TIMEOUT ) return EFI_TIMEOUT;
    return (*TransferResult)? EFI_DEVICE_ERROR:EFI_SUCCESS;
}


/**
    This function performs USB2 HC Isochronous Transfer

**/

EFI_STATUS
EFIAPI
AmiUsb2HcIsochronousTransfer(
    IN     EFI_USB2_HC_PROTOCOL               *HcProtocol,
    IN     UINT8                              DeviceAddress,
    IN     UINT8                              EndPointAddress,
    IN     UINT8                              DeviceSpeed,
    IN     UINTN                              MaximumPacketLength,
    IN     UINT8                              DataBuffersNumber,
    IN OUT VOID                               *Data[EFI_USB_MAX_ISO_BUFFER_NUM],
    IN     UINTN                              DataLength,
    IN     EFI_USB2_HC_TRANSACTION_TRANSLATOR *Translator,
    OUT    UINT32                             *TransferResult
)
{
    HC_DXE_RECORD   *This;
    DEV_INFO        *DevInfo;
    UINT32          SmiRes;

    if (Data == NULL || Data[0] == NULL || DataLength == 0 ||
        (DeviceSpeed != EFI_USB_SPEED_SUPER && DeviceSpeed != EFI_USB_SPEED_HIGH 
        && DeviceSpeed != EFI_USB_SPEED_FULL) || TransferResult == NULL) {
        return EFI_INVALID_PARAMETER;
    }
    
    if ((DeviceSpeed == EFI_USB_SPEED_HIGH || DeviceSpeed == EFI_USB_SPEED_SUPER)
        && MaximumPacketLength > 1024) {
        return EFI_INVALID_PARAMETER;
    }
    if (DeviceSpeed == EFI_USB_SPEED_FULL && MaximumPacketLength > 1023) {
        return EFI_INVALID_PARAMETER;
    }

    This = GetThis(HcProtocol);

    // DevInfo = DevAddr2Info( DeviceAddress, This->hc_data );

    // Several DevInfo might have same DeviceAddress, validate it using
    // endpoint address
    for (DevInfo = NULL; ;DevInfo++) {
        DevInfo = SearchDevinfoByAdr(DevInfo, DeviceAddress, This->hc_data);
        if (DevInfo == NULL || DevInfo->IsocDetails.Endpoint == EndPointAddress) break;
    }
    
    ASSERT(DevInfo);
    if (DevInfo == NULL) {
        return EFI_NOT_FOUND;
    }

    CRITICAL_CODE( EFI_TPL_NOTIFY,
    {
        gUsbData->dLastCommandStatusExtended = 0;
        SmiRes = UsbSmiIsocTransfer(
            This->hc_data,
            DevInfo,
            EndPointAddress & 0x80,
            (UINT8*)Data[0],
            (UINT32)DataLength,
            NULL
        );
        *TransferResult = GetTransferStatus();
    });

    //
    // Return with error or success
    //
    if ((*TransferResult) & EFI_USB_ERR_TIMEOUT) {
        return EFI_TIMEOUT;
    }
    return (*TransferResult) ? EFI_DEVICE_ERROR:EFI_SUCCESS;
}


VOID
EFIAPI
AsyncIsochOnTimer (
    EFI_EVENT   Event,
    VOID        *Context
)
{
    USBHC_ASYNC_ISOC_CONTEXT *Ctx = (USBHC_ASYNC_ISOC_CONTEXT*)Context;

    if (Ctx->Complete)
    {
        Ctx->CallbackFunction(Ctx->Data, Ctx->DataLength, Ctx->Context, 0);

        gBS->CloseEvent(Ctx->Event);
        gBS->FreePool(Ctx);
    }
}


/**
    This function performs USB2 HC Async Isochronous Transfer

**/

EFI_STATUS
EFIAPI
AmiUsb2HcAsyncIsochronousTransfer(
    IN     EFI_USB2_HC_PROTOCOL               *HcProtocol,
    IN     UINT8                              DeviceAddress,
    IN     UINT8                              EndPointAddress,
    IN     UINT8                              DeviceSpeed,
    IN     UINTN                              MaximumPacketLength,
    IN     UINT8                              DataBuffersNumber,
    IN OUT VOID                               *Data[EFI_USB_MAX_ISO_BUFFER_NUM],
    IN     UINTN                              DataLength,
    IN     EFI_USB2_HC_TRANSACTION_TRANSLATOR *Translator,
    IN     EFI_ASYNC_USB_TRANSFER_CALLBACK    IsochronousCallBack,
    IN     VOID                               *Context OPTIONAL
)
{
    HC_DXE_RECORD   *This;
    DEV_INFO        *DevInfo;
    USBHC_ASYNC_ISOC_CONTEXT    *AsyncIsocTransfer;

    if (Data == NULL || Data[0] == NULL || DataLength == 0 ||
        (DeviceSpeed != EFI_USB_SPEED_SUPER && DeviceSpeed != EFI_USB_SPEED_HIGH 
        && DeviceSpeed != EFI_USB_SPEED_FULL)) {
        return EFI_INVALID_PARAMETER;
    }

    if ((DeviceSpeed == EFI_USB_SPEED_HIGH || DeviceSpeed == EFI_USB_SPEED_SUPER)
        && MaximumPacketLength > 1024) {
        return EFI_INVALID_PARAMETER;
    }
    if (DeviceSpeed == EFI_USB_SPEED_FULL && MaximumPacketLength > 1023) {
        return EFI_INVALID_PARAMETER;
    }

    This = GetThis(HcProtocol);

    // Several DevInfo might have same DeviceAddress, validate it using
    // endpoint address
    for (DevInfo = NULL; ;DevInfo++) {
        DevInfo = SearchDevinfoByAdr(DevInfo, DeviceAddress, This->hc_data);
        if (DevInfo == NULL || DevInfo->IsocDetails.Endpoint == EndPointAddress) {
            break;
        }
    }
    
    ASSERT(DevInfo);
    if (DevInfo == NULL) {
        return EFI_NOT_FOUND;
    }

    // create new transfer
    gBS->AllocatePool (EfiBootServicesData, sizeof(USBHC_ASYNC_ISOC_CONTEXT), (VOID**)&AsyncIsocTransfer );
    
    AsyncIsocTransfer->CallbackFunction = IsochronousCallBack;
    AsyncIsocTransfer->Context = Context;
    AsyncIsocTransfer->Data = Data[0];
    AsyncIsocTransfer->DataLength = DataLength;
    AsyncIsocTransfer->Complete = 0;

    gBS->CreateEvent(EFI_EVENT_TIMER | EFI_EVENT_NOTIFY_SIGNAL,
            EFI_TPL_CALLBACK, AsyncIsochOnTimer, AsyncIsocTransfer, &AsyncIsocTransfer->Event);
    
    gBS->SetTimer(AsyncIsocTransfer->Event, TimerPeriodic, 55 * MILLISECOND);

//    CRITICAL_CODE( EFI_TPL_NOTIFY,
//    {
        gUsbData->dLastCommandStatusExtended = 0;
        UsbSmiIsocTransfer(
            This->hc_data,
            DevInfo,
            EndPointAddress & 0x80,
            (UINT8*)Data[0],
            (UINT32)DataLength,
            &AsyncIsocTransfer->Complete
        );
//    });

    return EFI_SUCCESS;
}


/**
    This function performs USB2 HC Control Transfer
    @param  EFI_USB2_HC_PROTOCOL    Pointer to EFI_USB2_HC_PROTOCOL
            DeviceAddress           Device address
            DeviceSpeed             Device speed
            MaximumPacketLength     Max packet of device
            Request                 Point to transfer requestion
            TransferDirection       Transfer directioin
            Data                    Point to interrupt transfer data
            DataLength              Point to interrupt transfer data length
            Timeout                 Time out value
            Translator              Point to usb2 hc transaction translator
            TransferResult          Point to transfer result

    @retval EFI_STATUS Status of the operation
**/

EFI_STATUS
EFIAPI
AmiUsb2HcControlTransfer(
    IN EFI_USB2_HC_PROTOCOL           *HcProtocol,
    IN     UINT8                      DeviceAddress,
    IN     UINT8                      DeviceSpeed,
    IN     UINTN                      MaximumPacketLength,
    IN     EFI_USB_DEVICE_REQUEST     *Request,
    IN     EFI_USB_DATA_DIRECTION     TransferDirection,
    IN OUT VOID                       *Data ,
    IN OUT UINTN                      *DataLength,
    IN     UINTN                      Timeout,
    IN     EFI_USB2_HC_TRANSACTION_TRANSLATOR *Translator,
    OUT    UINT32                     *TransferResult
)
{
    UINT16      SmiRes;
    HC_DXE_RECORD *This = GetThis( HcProtocol );
    DEV_INFO    *DevInfo;
    DEV_INFO    *DevSrc = DevAddr2Info( DeviceAddress, This->hc_data );
    UINT16      CurrentTimeout;
    EFI_STATUS  Status;

    if ((DeviceSpeed != EFI_USB_SPEED_SUPER && DeviceSpeed != EFI_USB_SPEED_HIGH
            && DeviceSpeed != EFI_USB_SPEED_FULL && DeviceSpeed != EFI_USB_SPEED_LOW)
            || DevSrc == NULL) {
        return EFI_INVALID_PARAMETER;
    }

    if (TransferDirection < EfiUsbDataIn || TransferDirection > EfiUsbNoData) {
        return EFI_INVALID_PARAMETER;
    }

    if (TransferDirection == EfiUsbNoData && (Data != NULL || *DataLength != 0)) {
        return EFI_INVALID_PARAMETER;
    }

    if (TransferDirection != EfiUsbNoData && (Data == NULL || *DataLength == 0)) {
        return EFI_INVALID_PARAMETER;
    }

    if (Request == NULL || TransferResult == NULL) {
        return EFI_INVALID_PARAMETER;
    }

    if (DeviceSpeed == EFI_USB_SPEED_LOW  && MaximumPacketLength != 8) {        //Device is low speed and maximum
        return EFI_INVALID_PARAMETER;                                           //packet should be 8, or return
    }                                                                           //invalid parameter

    if ((DeviceSpeed == EFI_USB_SPEED_HIGH || DeviceSpeed == EFI_USB_SPEED_FULL) && //Device is high/full speed
            ( MaximumPacketLength != 8 &&                                           //and maximum packet should
             MaximumPacketLength != 16 &&                                           //be 8/16/32/64, or return
             MaximumPacketLength != 32 &&                                           //invalid parameter
             MaximumPacketLength != 64 )) {
        return EFI_INVALID_PARAMETER;
    }

    if (DeviceSpeed == EFI_USB_SPEED_SUPER && MaximumPacketLength != 512) {     //Device is super speed and
        return EFI_INVALID_PARAMETER;                                       //maximum packet should be 512,
    }                                                                       // or return invalid parameter

    DevInfo = AllocDevInfo();
    DevInfo->bDeviceAddress = DeviceAddress;
    DevInfo->bHCNumber = This->hc_data->bHCNumber;
    DevInfo->bEndpointSpeed = SpeedMap[DeviceSpeed];
    DevInfo->wEndp0MaxPacket = (UINT16)MaximumPacketLength;
    DevInfo->bHubDeviceNumber = DevSrc->bHubDeviceNumber;
    DevInfo->bHubPortNumber = DevSrc->bHubPortNumber;
    DevInfo->DevMiscInfo = DevSrc->DevMiscInfo;	//(EIP84790+)

    CRITICAL_CODE( EFI_TPL_NOTIFY,
    {
        CurrentTimeout = gUsbData->wTimeOutValue;
        gUsbData->wTimeOutValue = (UINT16)Timeout;
        gUsbData->dLastCommandStatusExtended = 0;
        SmiRes = UsbSmiControlTransfer(This->hc_data, DevInfo,
            (UINT16)((( TransferDirection == EfiUsbDataIn?1:0) << 7) |
            (((UINT16)Request->RequestType)) |
            (((UINT16)Request->Request)<<8)),
            (UINT16)Request->Index, (UINT16)Request->Value,
            (UINT8*)Data, (UINT16)Request->Length);
        *TransferResult = GetTransferStatus();
        gUsbData->wTimeOutValue = CurrentTimeout;
    });

    Status = EFI_SUCCESS;

    if ((*TransferResult) & EFI_USB_ERR_TIMEOUT) {
        Status = EFI_TIMEOUT;
    }
    if ((*TransferResult) & ~EFI_USB_ERR_TIMEOUT) {
        Status = EFI_DEVICE_ERROR;
    }

    *DataLength = (UINTN)SmiRes;

    FreeDevInfo(DevInfo);

    return Status;
}


/**
    This function returns HC root port status.

**/

EFI_STATUS
EFIAPI
AmiUsb2HcGetRootHubPortStatus (
    EFI_USB2_HC_PROTOCOL *HcProtocol,
    UINT8               PortNumber,
    EFI_USB_PORT_STATUS *PortStatus
)
{
    HC_DXE_RECORD *This = GetThis( HcProtocol );
    return AmiUsbHcGetRootHubPortStatus(
                &This->hcprotocol, PortNumber, PortStatus);
}


/**
    This function set root hub port features.

**/

EFI_STATUS
EFIAPI
AmiUsb2HcSetRootHubPortFeature(
    IN EFI_USB2_HC_PROTOCOL *HcProtocol,
    IN UINT8                PortNumber,
    IN EFI_USB_PORT_FEATURE PortFeature
)
{
    HC_DXE_RECORD *This = GetThis( HcProtocol );
    return AmiUsbHcSetRootHubPortFeature(
                &This->hcprotocol, PortNumber, PortFeature);
}


/**
    This function clears root hub port feature.

**/

EFI_STATUS EFIAPI
AmiUsb2HcClearRootHubPortFeature(
    IN EFI_USB2_HC_PROTOCOL *HcProtocol,
    IN UINT8                PortNumber,
    IN EFI_USB_PORT_FEATURE PortFeature
)
{
    HC_DXE_RECORD *This = GetThis( HcProtocol );
    return AmiUsbHcClearRootHubPortFeature(
            &This->hcprotocol, PortNumber, PortFeature);
}


/**
    Search gUsbData for information about HC linked to an EFI handle

    @param 
        Controller - Host Controller handle

**/

HC_STRUC* FindHcStruc(
    EFI_HANDLE Controller
)
{
    unsigned i;
    for (i = 0; i < gUsbData->HcTableCount; i++) {
        if (gUsbData->HcTable[i] == NULL) {
            continue;
        }
        if (!(gUsbData->HcTable[i]->dHCFlag & HC_STATE_USED)) {
            continue;
        }
        if (gUsbData->HcTable[i]->Controller == Controller )
            return gUsbData->HcTable[i];
    }
    return NULL;
}

/**
    Start the AMI USB driver; Sets USB_FLAG_DRIVER_STARTED

    @param 
        This                - Protocol instance pointer.
        ControllerHandle    - Handle of device to test
        RemainingDevicePath - Not used

         
    @retval EFI_SUCCESS USB HC devices were initialized.
    @retval EFI_UNSUPPORTED pThis device is not supported by USB driver.
    @retval EFI_DEVICE_ERROR pThis driver cannot be started due to device error
        EFI_OUT_OF_RESOURCES

**/

EFI_STATUS
InstallHcProtocols(
    IN EFI_DRIVER_BINDING_PROTOCOL  *This,
    IN EFI_HANDLE                   Controller,
    IN EFI_PCI_IO_PROTOCOL   		*PciIo,
    IN HC_STRUC              		*HcData
)
{
    HC_DXE_RECORD         *Rec;
    USB3_HOST_CONTROLLER *Usb3Hc;
    EFI_STATUS              Status;

    //
    // Create HcDxeRecord
    //

    Status = gBS->AllocatePool(
        EfiBootServicesData,
        sizeof(HC_DXE_RECORD),
        (VOID**)&Rec);
    
    ASSERT_EFI_ERROR(Status);
    
    if (EFI_ERROR(Status)) {
        return Status;
    }
        
    Rec->hc_data = HcData;
    Rec->pciIo = PciIo;
    Rec->AsyncTransfers.pHead = NULL;
    Rec->AsyncTransfers.pTail = NULL;
    Rec->AsyncTransfers.Size = 0;

    Rec->hcprotocol.Reset = AmiUsbHcReset;
    Rec->hcprotocol.GetState = AmiUsbHcGetState;
    Rec->hcprotocol.SetState = AmiUsbHcSetState;
    Rec->hcprotocol.ControlTransfer = AmiUsbHcControlTransfer;
    Rec->hcprotocol.BulkTransfer = AmiUsbHcBulkTransfer;
    Rec->hcprotocol.AsyncInterruptTransfer = AmiUsbHcAsyncInterruptTransfer;
    Rec->hcprotocol.SyncInterruptTransfer = AmiUsbHcSyncInterruptTransfer;
    Rec->hcprotocol.IsochronousTransfer = AmiUsbHcIsochronousTransfer;
    Rec->hcprotocol.AsyncIsochronousTransfer = AmiUsbHcAsyncIsochronousTransfer;
    Rec->hcprotocol.GetRootHubPortNumber = AmiUsbHcGetRootHubPortNumber;
    Rec->hcprotocol.GetRootHubPortStatus = AmiUsbHcGetRootHubPortStatus;
    Rec->hcprotocol.SetRootHubPortFeature = AmiUsbHcSetRootHubPortFeature;
    Rec->hcprotocol.ClearRootHubPortFeature = AmiUsbHcClearRootHubPortFeature;

    //
    // Fill USB Revision fields based on type of HC
    //
    // USB_HC_UHCI USB_HC_OHCI -> 1.1
    // USB_HC_EHCI   -> 2.0
    // USB_HC_XHCI 0.96, 1.0 -> 3.0
    // USB_HC_XHCI 1.1 -> 3.1

    switch (HcData->bHCType) {
        case USB_HC_UHCI:
        case USB_HC_OHCI:
            Rec->hcprotocol.MajorRevision = 1;
            Rec->hcprotocol.MinorRevision = 1;
            break;
        case USB_HC_EHCI:
            Rec->hcprotocol.MajorRevision = 2;
            Rec->hcprotocol.MinorRevision = 0;
            break;
        case USB_HC_XHCI:
            Rec->hcprotocol.MajorRevision = 3;
            Usb3Hc = (USB3_HOST_CONTROLLER*)HcData->usbbus_data;
            if (Usb3Hc->CapRegs.HciVersion <= 0x0100) {
                Rec->hcprotocol.MinorRevision = 0;
            } else {
                Rec->hcprotocol.MinorRevision = 1;
            }
            break;
        default:
            break;
    }

    Rec->hcprotocol2.GetCapability = AmiUsb2HcGetCapability;
    Rec->hcprotocol2.Reset = AmiUsb2HcReset;
    Rec->hcprotocol2.GetState = AmiUsb2HcGetState;
    Rec->hcprotocol2.SetState = AmiUsb2HcSetState;
    Rec->hcprotocol2.ControlTransfer = AmiUsb2HcControlTransfer;
    Rec->hcprotocol2.BulkTransfer = AmiUsb2HcBulkTransfer;
    Rec->hcprotocol2.AsyncInterruptTransfer = AmiUsb2HcAsyncInterruptTransfer;
    Rec->hcprotocol2.SyncInterruptTransfer = AmiUsb2HcSyncInterruptTransfer;
    Rec->hcprotocol2.IsochronousTransfer = AmiUsb2HcIsochronousTransfer;
    Rec->hcprotocol2.AsyncIsochronousTransfer = AmiUsb2HcAsyncIsochronousTransfer;
    Rec->hcprotocol2.GetRootHubPortStatus = AmiUsb2HcGetRootHubPortStatus;
    Rec->hcprotocol2.SetRootHubPortFeature = AmiUsb2HcSetRootHubPortFeature;
    Rec->hcprotocol2.ClearRootHubPortFeature = AmiUsb2HcClearRootHubPortFeature;
    Rec->hcprotocol2.MajorRevision = Rec->hcprotocol.MajorRevision;
    Rec->hcprotocol2.MinorRevision = Rec->hcprotocol.MinorRevision;


    //
    // Instal USB_HC_PROTOCOL
    //
    gBS->InstallProtocolInterface(&Controller,
            &gEfiUsbHcProtocolGuid, EFI_NATIVE_INTERFACE, &Rec->hcprotocol);
    gBS->InstallProtocolInterface(&Controller,
            &gEfiUsb2HcProtocolGuid, EFI_NATIVE_INTERFACE, &Rec->hcprotocol2);

    return EFI_SUCCESS;
}


/**
    This function allocates XHCI scratchpad buffers. Data initialization will
    be done later, in SMI XhciStart function.
    @param  HCStruc Pointer to the HC information structure
            Usb3Hc  Pointer to USB3 host controller

    @retval EFI_STATUS Status of the operation
    
    @note  
  Usb3Hc->DcbaaPtr points to the beginning of memory block first 2048 Bytes
  of which is used for DCBAA.

**/

EFI_STATUS
XhciAllocateScratchpadBuffers (
	IN HC_STRUC				*HcStruct,
    IN USB3_HOST_CONTROLLER *Usb3Hc
)
{
    UINT16  NumBufs =  ((Usb3Hc->CapRegs.HcsParams2.MaxScratchPadBufsHi) << 5) + 
                        Usb3Hc->CapRegs.HcsParams2.MaxScratchPadBufsLo;
    UINT16      Count;
    VOID        *Buffer;
    UINTN       MemSize;
    
    if (NumBufs == 0) {
        return EFI_SUCCESS;
    }

    if (Usb3Hc->ScratchBufEntry == NULL) {
        // Allcate a PAGESIZE boundary for Scratchpad Buffer Array Base Address
        // because bit[0..5] are reserved in  Device Context Base Address Array Element 0.
        MemSize = TRANSFER_RING_OFFSET + Usb3Hc->CapRegs.HcsParams1.MaxSlots * END_POINTS_PADDED * TRB_RING_PADDED_SIZE +
                            RING_SIZE * Usb3Hc->CapRegs.HcsParams1.MaxSlots * END_POINTS_PADDED +
                                (XHCI_DEVICE_CONTEXT_ENTRIES * Usb3Hc->ContextSize) * Usb3Hc->CapRegs.HcsParams1.MaxSlots;
        if(((UINTN)Usb3Hc->DcbaaPtr + MemSize) & (UINTN)EFI_PAGE_MASK){
            Usb3Hc->ScratchBufEntry = (VOID*)(((UINTN)Usb3Hc->DcbaaPtr + MemSize + EFI_PAGE_SIZE) & (~(UINTN)EFI_PAGE_MASK));
        }else{
            Usb3Hc->ScratchBufEntry = (VOID*)((UINTN)Usb3Hc->DcbaaPtr + MemSize);
        }
        
        if (Usb3Hc->ScratchBufEntry == NULL) {
            return EFI_OUT_OF_RESOURCES;
        }

        SetMem(Usb3Hc->ScratchBufEntry, (sizeof(Usb3Hc->ScratchBufEntry) * NumBufs), 0);

        if(((UINTN)Usb3Hc->ScratchBufEntry + (sizeof(Usb3Hc->ScratchBufEntry) * NumBufs)) & (UINTN)EFI_PAGE_MASK){
            Buffer = (UINTN*)(((UINTN)Usb3Hc->ScratchBufEntry + (sizeof(Usb3Hc->ScratchBufEntry) * NumBufs) + EFI_PAGE_SIZE) & (~(UINTN)EFI_PAGE_MASK));
        }else{
            Buffer = (UINTN*)((UINTN)Usb3Hc->ScratchBufEntry + (sizeof(Usb3Hc->ScratchBufEntry) * NumBufs));
        }

        for (Count = 0; Count < NumBufs; Count++) {

            // Allocate scratchpad buffer: PAGESIZE block located on
            // a PAGESIZE boundary. Section 4.20.
            ASSERT(Buffer != NULL); // See if allocation is successful           
            if (Buffer == NULL) {
                return EFI_OUT_OF_RESOURCES;
            }

            // Update *ScratchBufArrayBase
            Usb3Hc->ScratchBufEntry[Count] = (UINTN)Buffer;
            Buffer = (VOID*)((UINTN)Buffer + EFI_PAGES_TO_SIZE(Usb3Hc->PageSize4K));
        }
    }

    // Update scratchpad pointer only if # of scratch buffers >0
    if (NumBufs > 0) {
        *(UINTN*)Usb3Hc->DcbaaPtr = (UINTN)Usb3Hc->ScratchBufEntry;
    }

    return EFI_SUCCESS;
}


/**
    This function allocates XHCI memory buffers. Data initialization will be
    done later, in SMI XhciStart function. These are the memory blocks:
    @param  HCStruc Pointer to the HC information structure
            Usb3Hc  Pointer to USB3 host controller

    @retval EFI_STATUS Status of the operation
    1. DCBAAP + ScrPadBuf pointers + InpCtx + ERST <-- 8KB
    2. CommandRing  <-- 1KB
    3. EventRing    <-- 1KB
    4. XferRings    <-- 1KB*MaxSlots, 1KB = 32EP per slot times 32 (padded size of TRB_RING)
    4. N*sizeof(XHCI_DEVICE_CONTEXT) for device context segment <-- N KB or 2*N KB,
    N is SlotNumber from CONFIG register
  
    5. TransferRings <-- MaxSlots*32*1KB

    @note  
  Scratchpad buffers are optional, they are allocated and initialized separately.

**/

EFI_STATUS
XhciInitMemory (
	IN HC_STRUC				*HcStruct,
    IN USB3_HOST_CONTROLLER *Usb3Hc
)
{
    EFI_STATUS  Status;
    UINTN       MemSize;
    UINTN       DeviceContextSize;
    UINTN       XfrRingsSize;
    UINTN       XfrTrbsSize;
    UINTN       TotalPages;
    UINT16      NumBufs;

    XfrRingsSize = Usb3Hc->CapRegs.HcsParams1.MaxSlots * END_POINTS_PADDED * TRB_RING_PADDED_SIZE;   // 32 endpoints per device, 32 padded size of TRB_RING
    XfrTrbsSize = RING_SIZE * Usb3Hc->CapRegs.HcsParams1.MaxSlots * END_POINTS_PADDED;
    DeviceContextSize = (XHCI_DEVICE_CONTEXT_ENTRIES * Usb3Hc->ContextSize) * Usb3Hc->CapRegs.HcsParams1.MaxSlots;
    NumBufs =  ((Usb3Hc->CapRegs.HcsParams2.MaxScratchPadBufsHi) << 5) + 
                                    Usb3Hc->CapRegs.HcsParams2.MaxScratchPadBufsLo;
//    MemSize = 0x2800 + XfrRingsSize + XfrTrbsSize + DeviceContextSize;
    MemSize = TRANSFER_RING_OFFSET + XfrRingsSize + XfrTrbsSize + DeviceContextSize;
    TotalPages = EFI_SIZE_TO_PAGES(MemSize);
    TotalPages += EFI_SIZE_TO_PAGES(sizeof(Usb3Hc->ScratchBufEntry) * NumBufs);
    TotalPages += (Usb3Hc->PageSize4K * NumBufs);
    if (Usb3Hc->DcbaaPtr == NULL) {
        // InputContext : DcbaaPtr + 0x940
        // ERST         : DcbaaPtr + 0x1200
        // CmdRing      : DcbaaPtr + 0x2000
        // EvtRing      : DcbaaPtr + 0x2400
        // XfrRings     : DcbaaPtr + 0x6400
        // XfrTrbs      : XfrRings + XfrRingsSize
        // DeviceContext: XfrTrbs  + XfrTrbsSize
        // ScratchBuffer: After DeviceContext, start from a new page.
        Usb3Hc->DcbaaPtr = (XHCI_DCBAA*)AllocateHcMemory(HcStruct->PciIo, 
                        TotalPages, USB_PAGE_SIZE_ALIGNMENT);
        if (Usb3Hc->DcbaaPtr == NULL) {
            return EFI_OUT_OF_RESOURCES;
        }
    }
    SetMem(Usb3Hc->DcbaaPtr, MemSize, 0);  // Clear buffer
    USB_DEBUG(DEBUG_INFO, 3, "XHCI: Memory allocation - total %x Bytes:\n 0x6400+XfrRings(%x)+XfrTrbs(%x)+DevCtx(%x)\n",
        MemSize, XfrRingsSize, XfrTrbsSize, DeviceContextSize);

    // Assign DCBAA (Device Context Base Address); program the
    // DCBAA Pointer (DCBAAP) register (5.4.6) with a 64-bit address
    // pointing to where the Device Context Base Address Array is located.
    //
    // DCBAA: size 2048 Bytes, within PAGESIZE, 64 Bytes aligned.
    //
    // These requirements can be met by allocating 1 page using
    // gBS->AllocatePages; the address will be 4K aligned and will
    // not span PAGESIZE boundary.

    Status = XhciAllocateScratchpadBuffers(HcStruct, Usb3Hc);
    ASSERT_EFI_ERROR(Status);

    // Assign Input Context; the size of Input Context is either
    // 0x420 or 0x840 depending on HCPARAMS.Csz
    if (Usb3Hc->InputContext == NULL) {
        Usb3Hc->InputContext = (VOID*)((UINTN)Usb3Hc->DcbaaPtr + INPUT_CONTEXT_SIZE);
    }

    // Initialize Transfer Rings pointer and store it in Usb3Hc; actual
    // xfer ring initialization happens later, when the EP is being enabled
//    Usb3Hc->XfrRings = (TRB_RING*)((UINTN)Usb3Hc->DcbaaPtr + 0x2800);
    if (Usb3Hc->XfrRings == NULL) {
        Usb3Hc->XfrRings = (TRB_RING*)((UINTN)Usb3Hc->DcbaaPtr + TRANSFER_RING_OFFSET);
    }

    // 1024 = 32 bytes is padded sizeof(TRB_RING) times 32 EP per device
    if (Usb3Hc->XfrTrbs == 0) {
        Usb3Hc->XfrTrbs = (UINTN)Usb3Hc->XfrRings + XfrRingsSize;
    }

    // Assign device context memory: Usb3Hc->MaxSlots devices,
    // 1024 (2048 if HCPARAMS.Csz is set) Bytes each
    if (Usb3Hc->DeviceContext == NULL) {
        Usb3Hc->DeviceContext = (VOID*)((UINTN)Usb3Hc->XfrTrbs + XfrTrbsSize);
    }
    
    Status = UsbHcMemoryRecord(Usb3Hc->Controller,(EFI_PHYSICAL_ADDRESS)(UINTN)Usb3Hc->DcbaaPtr,TotalPages);

    return Status;
}

/**

**/

EFI_STATUS
XhciExtCapParser(
    HC_STRUC    *HcStruc,
    IN USB3_HOST_CONTROLLER *Usb3Hc
)
{
	EFI_PCI_IO_PROTOCOL		*PciIo = HcStruc->PciIo;
    UINT32                  CurPtrOffset;
    UINT32                  XhciLegCtrlSts;
    UINT32                  XhciLegSup;
    XHCI_EXT_CAP            *XhciExtCap = (XHCI_EXT_CAP*)&XhciLegSup;
    EFI_STATUS              Status;

    if (Usb3Hc->CapRegs.HccParams1.Xecp == 0) {
        return EFI_SUCCESS;
    }

	// Starts from first capability
	CurPtrOffset = Usb3Hc->CapRegs.HccParams1.Xecp << 2;

    // Traverse all capability structures
	for(;;) {
	    Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint32, 0, CurPtrOffset, 1, XhciExtCap);
        if (EFI_ERROR(Status)) {
           USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Xhci: Extended Capability error: %r at %x\n", Status, CurPtrOffset);
           return Status;
        }
		switch (XhciExtCap->CapId) {
			case XHCI_EXT_CAP_USB_LEGACY:
                
                Usb3Hc->UsbLegSupOffSet = CurPtrOffset;
                
                // Clear HC BIOS and OS Owned Semaphore bit               
                XhciLegSup &= ~(XHCI_BIOS_OWNED_SEMAPHORE | XHCI_OS_OWNED_SEMAPHORE);
                Status = PciIo->Mem.Write(PciIo, EfiPciIoWidthUint32, 0, CurPtrOffset, 1, &XhciLegSup);
                if (EFI_ERROR(Status)) {
                    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Xhci: Clear HC BIOS and OS Owned Semaphore bit error: %r at %x\n",
                              Status, CurPtrOffset);
                    return Status;
                }
                
				// Clear SMI enable bit
                Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint32, 0, 
                    CurPtrOffset + XHCI_LEGACY_CTRL_STS_REG, 1, &XhciLegCtrlSts);
                if (EFI_ERROR(Status)) {
                    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Xhci: Legacy Support Control/Status error: %r at %x\n", Status, 
                            CurPtrOffset + XHCI_LEGACY_CTRL_STS_REG);
                    return Status;
                }

                XhciLegCtrlSts &= ~(XHCI_SMI_ENABLE | XHCI_SMI_OWNERSHIP_CHANGE_ENABLE);
                Status = PciIo->Mem.Write(PciIo, EfiPciIoWidthUint32, 0, 
                    CurPtrOffset + XHCI_LEGACY_CTRL_STS_REG, 1, &XhciLegCtrlSts);
                if (EFI_ERROR(Status)) {
                    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Xhci: Clear SMI enable bit error: %r at %x\n", Status, 
                            CurPtrOffset + XHCI_LEGACY_CTRL_STS_REG);
                    return Status;
                }
				USB_DEBUG(DEBUG_INFO, 3, "XHCI: USB Legacy Support Offset %x\n", Usb3Hc->UsbLegSupOffSet);
				break;

			case XHCI_EXT_CAP_SUPPORTED_PROTOCOL:
			    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "XHCI: CAP supported : MajorRev %x MajorRev %x\n", ((XHCI_EXT_PROTOCOL*)XhciExtCap)->MajorRev, ((XHCI_EXT_PROTOCOL*)XhciExtCap)->MinorRev);
                if (((XHCI_EXT_PROTOCOL*)XhciExtCap)->MajorRev == 0x02) {
                    Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint32, 0, CurPtrOffset, 3, &Usb3Hc->Usb2Protocol);
                    if (EFI_ERROR(Status)) {
                        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Xhci: USB2 Support Protocol error: %r at %x\n", Status, CurPtrOffset);
                        return Status;
                    }
                    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "XHCI: USB2 Support Protocol %x, PortOffset %x PortCount %x\n", 
                        (UINTN*)(HcStruc->BaseAddress + CurPtrOffset), Usb3Hc->Usb2Protocol.PortOffset, Usb3Hc->Usb2Protocol.PortCount);
                } else if (((XHCI_EXT_PROTOCOL*)XhciExtCap)->MajorRev == 0x03) {
                    if (((XHCI_EXT_PROTOCOL*)XhciExtCap)->MinorRev == 0x00) {
                        Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint32, 0, CurPtrOffset, 3, &Usb3Hc->Usb3Protocol);
                        if (EFI_ERROR(Status)) {
                            USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Xhci: USB3.0 Support Protocol error: %r at %x\n", Status, CurPtrOffset);
                            return Status;
                        }
                    } else if (((XHCI_EXT_PROTOCOL*)XhciExtCap)->MinorRev == 0x01) {
                        Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint32, 0, CurPtrOffset, 3, &Usb3Hc->Usb31Protocol);
                        if (EFI_ERROR(Status)) {
                            USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Xhci: USB3.1 Support Protocol error: %r at %x\n", Status, CurPtrOffset);
                            return Status;
                        }
                        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "XHCI: USB3.1 Support Protocol %x, PortOffset %x PortCount %x\n",
                            (UINTN*)(HcStruc->BaseAddress + CurPtrOffset), Usb3Hc->Usb31Protocol.PortOffset, Usb3Hc->Usb31Protocol.PortCount);
                    }else if (((XHCI_EXT_PROTOCOL*)XhciExtCap)->MinorRev == 0x10) {
                        Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint32, 0, CurPtrOffset, 3, &Usb3Hc->Usb31Protocol);
                        if (EFI_ERROR(Status)) {
                            USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Xhci: USB3.1 Support Protocol error: %r at %x\n", Status, CurPtrOffset);
                            return Status;
                        }
                        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "XHCI: USB3.1 Support Protocol %x, PortOffset %x PortCount %x\n",
                            (UINTN*)(HcStruc->BaseAddress + CurPtrOffset), Usb3Hc->Usb31Protocol.PortOffset, Usb3Hc->Usb31Protocol.PortCount);
                    }
                }
				break;

			case XHCI_EXT_CAP_POWERMANAGEMENT:
			case XHCI_EXT_CAP_IO_VIRTUALIZATION:
                break;
			case XHCI_EXT_CAP_USB_DEBUG_PORT:
                Usb3Hc->DbCapOffset = CurPtrOffset;
                USB_DEBUG(DEBUG_INFO, 3, "XHCI: USB Debug Capability Offset %x\n", Usb3Hc->DbCapOffset);
				break;
		}
        if (XhciExtCap->NextCapPtr == 0) {
            break;
        }
	    // Point to next capability
	    CurPtrOffset += XhciExtCap->NextCapPtr << 2;
	}

	return EFI_SUCCESS;
}


/**
      This function allocates memory for EHCI iTD list.
    @param  Handle  Handle number of HC
            HCStruc Pointer to the HC information structure

    @retval EFI_STATUS Status of the operation
**/

EFI_STATUS PreInitEhci(
    EFI_HANDLE  Handle,
    HC_STRUC    *HcStruc
)
{
    EFI_STATUS Status = EFI_SUCCESS;
#if EHCI_SUPPORT && USB_ISOCTRANSFER_SUPPORT
    EFI_PHYSICAL_ADDRESS    Address;
    UINTN                   NumPages;
    
    if (HcStruc->IsocTds == NULL) {

        Address = 0xFFFFFFFF;
        NumPages = EFI_SIZE_TO_PAGES(EHCI_FRAMELISTSIZE * sizeof(EHCI_ITD));
        
        Status = gBS->AllocatePages(AllocateMaxAddress, EfiRuntimeServicesData,
                        NumPages, &Address);
        
        //ASSERT_EFI_ERROR(Status);
        
        if (EFI_ERROR(Status)) {
            return Status;
        }
        HcStruc->IsocTds = (VOID*)(UINTN)Address;
    }
    
    SetMem(HcStruc->IsocTds, EHCI_FRAMELISTSIZE * sizeof(EHCI_ITD), 0);
    

    USB_DEBUG(DEBUG_INFO, 3, "PreInitEhci: ITD buffer address: %x\n", (UINTN)HcStruc->IsocTds);
#endif

    return Status;
}

/**

  This function frees the memory previously allocated for iTDs.

**/

EFI_STATUS PostStopEhci(
    EFI_HANDLE  Handle,
    HC_STRUC    *HcStruc
)
{
    EFI_STATUS Status = EFI_SUCCESS;
#if EHCI_SUPPORT && USB_ISOCTRANSFER_SUPPORT
    //Status = gBS->FreePages((EFI_PHYSICAL_ADDRESS)(HcStruc->IsocTds),
    //                    EFI_SIZE_TO_PAGES(EHCI_FRAMELISTSIZE*sizeof(EHCI_ITD)));
    //ASSERT_EFI_ERROR(Status);
#endif
    return Status;
}


/**
    This function initializes XHCI data structures, allocates HC memory and
    updates the relevant fields in HcStruc. At this point the controller's
    resources are assigned and accessible.

**/

EFI_STATUS
PreInitXhci(
    EFI_HANDLE  Handle,
    HC_STRUC    *HcStruc
)
{
#if XHCI_SUPPORT
    UINT8   MaxSlots;
    EFI_STATUS Status;
	EFI_PCI_IO_PROTOCOL		*PciIo = HcStruc->PciIo;
    USB3_HOST_CONTROLLER    *Usb3Hc = NULL;

    if (HcStruc->usbbus_data == NULL) {
        Status = gBS->AllocatePool(EfiRuntimeServicesData, 
            sizeof(USB3_HOST_CONTROLLER), (VOID**)&Usb3Hc);
        if (EFI_ERROR(Status)) {
            return Status;
        }
        SetMem(Usb3Hc, sizeof(USB3_HOST_CONTROLLER), 0);
    } else {
        Usb3Hc = HcStruc->usbbus_data;
    }

    Usb3Hc->Controller = Handle;

    // Get Capability Registers offset off the BAR
                                        //(EIP101226)>
    Status = PciIo->Pci.Read(PciIo, 
    			EfiPciIoWidthUint32, PCI_BAR0, 1, &HcStruc->BaseAddress);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) {
		return Status;
    }
    if (((UINT8)HcStruc->BaseAddress & (BIT1 |BIT2)) == BIT2) {
        Status = PciIo->Pci.Read(PciIo, 
            		EfiPciIoWidthUint32, PCI_BAR0, 
            		sizeof(VOID*)/sizeof(UINT32), &HcStruc->BaseAddress);
    }


                                        //<(EIP101226)
    //clear all attributes before use
	HcStruc->BaseAddress &= ~(0x7F); // Clear attributes

	Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint16, PCI_VID, 1, &Usb3Hc->Vid);
    if (EFI_ERROR(Status)) {
        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Xhci: Vendor ID error: %r at %x\n", Status, PCI_VID);
        return Status;
    }
    
    Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint16, PCI_DID, 1, &Usb3Hc->Did);
    if (EFI_ERROR(Status)) {
        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Xhci: Device ID error: %r at %x\n", Status, PCI_DID);
        return Status;
    }
    
    Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint8, 0, XHCI_CAPLENGTH_OFFSET, 1, &Usb3Hc->CapRegs.CapLength);
    if (EFI_ERROR(Status)) {
        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Xhci: Capability Register length error: %r at %x\n", Status, XHCI_CAPLENGTH_OFFSET);
        return Status;
    }
    
    Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint16, 0, XHCI_HCIVERSION_OFFSET, 1, &Usb3Hc->CapRegs.HciVersion);
    if (EFI_ERROR(Status)) {
        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Xhci: Interface Version Number error: %r at %x\n", Status, XHCI_HCIVERSION_OFFSET);
        return Status;
    }
    
    Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint32, 0, XHCI_HCSPARAMS1_OFFSET, 1, &Usb3Hc->CapRegs.HcsParams1);
    if (EFI_ERROR(Status)) {
        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Xhci: Structural Parameters 1 error: %r at %x\n", Status, XHCI_HCSPARAMS1_OFFSET);
        return Status;
    }
    
    Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint32, 0, XHCI_HCSPARAMS2_OFFSET, 1, &Usb3Hc->CapRegs.HcsParams2);
    if (EFI_ERROR(Status)) {
        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Xhci: Structural Parameters 2 error: %r at %x\n", Status, XHCI_HCSPARAMS2_OFFSET);
        return Status;
    }
    
    Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint32, 0, XHCI_HCSPARAMS3_OFFSET, 1, &Usb3Hc->CapRegs.HcsParams3);
    if (EFI_ERROR(Status)) {
        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Xhci: Structural Parameters 3 error: %r at %x\n", Status, XHCI_HCSPARAMS3_OFFSET);
        return Status;
    }
    
    Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint32, 0, XHCI_HCCPARAMS1_OFFSET, 1, &Usb3Hc->CapRegs.HccParams1);
    if (EFI_ERROR(Status)) {
        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Xhci: Capability Parameters 1 error: %r at %x\n", Status, XHCI_HCCPARAMS1_OFFSET);
        return Status;
    }
    
    Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint32, 0, XHCI_DBOFF_OFFSET, 1, &Usb3Hc->CapRegs.DbOff);
    if (EFI_ERROR(Status)) {
        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Xhci: Doorbell Offset error: %r at %x\n", Status, XHCI_DBOFF_OFFSET);
        return Status;
    }
    
    Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint32, 0, XHCI_RTSOFF_OFFSET, 1, &Usb3Hc->CapRegs.RtsOff);
    if (EFI_ERROR(Status)) {
        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Xhci: Runtime Register Space Offset error: %r at %x\n", Status, XHCI_RTSOFF_OFFSET);
        return Status;
    }
    
    Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint32, 0, XHCI_HCCPARAMS2_OFFSET, 1, &Usb3Hc->CapRegs.HccParams2);
    if (EFI_ERROR(Status)) {
        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Xhci: Capability Parameters 2 error: %r at %x\n", Status, XHCI_HCCPARAMS2_OFFSET);
        return Status;
    }

    HcStruc->bOpRegOffset = Usb3Hc->CapRegs.CapLength;
    HcStruc->bNumPorts = Usb3Hc->CapRegs.HcsParams1.MaxPorts;
 
    Usb3Hc->OpRegs = (XHCI_HC_OP_REGS*)(HcStruc->BaseAddress + Usb3Hc->CapRegs.CapLength);

    Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint32, 0, 
            Usb3Hc->CapRegs.CapLength + XHCI_PAGESIZE_OFFSET, 1, &Usb3Hc->PageSize4K);
    if (EFI_ERROR(Status)) {
        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Xhci: Page Size Register error: %r at %x\n", 
                  Status, Usb3Hc->CapRegs.CapLength + XHCI_PAGESIZE_OFFSET);
        return Status;
    }
    
	Usb3Hc->ContextSize = 0x20 << Usb3Hc->CapRegs.HccParams1.Csz;

//    USB_DEBUG(DEBUG_INFO, 3, "XHCI: Cap %x, OpRegs: %x Ver %x,\n      MaxPorts 0x%x, PageSize %x*4K\n",
//        Usb3Hc->CapRegs, Usb3Hc->OpRegs, Usb3Hc->HciVersion, Usb3Hc->MaxPorts, Usb3Hc->PageSize4K);

    ASSERT(Usb3Hc->PageSize4K < 0x8000);   // Maximum possible page size is 128MB

    Status = PciIo->Pci.Read(PciIo, 
				EfiPciIoWidthUint8, XHCI_PCI_SBRN, 1, &Usb3Hc->SBRN);
    ASSERT_EFI_ERROR(Status);
    USB_DEBUG(DEBUG_INFO, 3, "XHCI: Serial Bus Release Number is %x\n", Usb3Hc->SBRN);

    // OEM might change the default number of MaxSlots
    MaxSlots = Usb3Hc->CapRegs.HcsParams1.MaxSlots;
    Status = Usb3OemGetMaxDeviceSlots(HcStruc, &MaxSlots);

    if (!EFI_ERROR(Status)) {
    	// Validate the porting function output
    	ASSERT(MaxSlots > 0 && MaxSlots <= Usb3Hc->CapRegs.HcsParams1.MaxSlots);
    	if (MaxSlots < Usb3Hc->CapRegs.HcsParams1.MaxSlots){
    		Usb3Hc->CapRegs.HcsParams1.MaxSlots = MaxSlots;
    	}
    }

    USB_DEBUG(DEBUG_INFO, 3, "XHCI: MaxSlots %x, MaxIntrs %x, Doorbell Offset %x\n", 
            Usb3Hc->CapRegs.HcsParams1.MaxSlots, 
            Usb3Hc->CapRegs.HcsParams1.MaxIntrs,
            Usb3Hc->CapRegs.DbOff);

	// Parse all capability structures
	XhciExtCapParser(HcStruc, Usb3Hc);

	if (gUsbData->UsbXhciSupport == 0) {
        gBS->FreePool(Usb3Hc);
		return EFI_UNSUPPORTED;
	}

    // Allocate and initialize memory blocks
    Status = XhciInitMemory(HcStruc, Usb3Hc);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) {
		gBS->FreePool(Usb3Hc);
		return Status;
    }

    HcStruc->usbbus_data = (VOID*)Usb3Hc;
#endif

    return EFI_SUCCESS;
}

/**
    This function frees the HC memory and clears XHCI data structures.

**/

EFI_STATUS
PostStopXhci(
    EFI_HANDLE  Handle,
    HC_STRUC    *HcStruc
)
{
#if XHCI_SUPPORT
/*
	USB3_HOST_CONTROLLER *Usb3Hc = (USB3_HOST_CONTROLLER*)HcStruc->usbbus_data;
    UINT16  NumBufs =  ((Usb3Hc->CapRegs.HcsParams2.MaxScratchPadBufsHi) << 5) + 
                        Usb3Hc->CapRegs.HcsParams2.MaxScratchPadBufsLo;
    UINT16  Count;
    UINTN   MemSize;
    UINTN   DeviceContextSize;
    UINTN   XfrRingsSize;
    UINTN   XfrTrbsSize;

	for (Count = 0; Count < NumBufs; Count++) {
		gBS->FreePages((EFI_PHYSICAL_ADDRESS)(Usb3Hc->ScratchBufEntry[Count]), Usb3Hc->PageSize4K);
	}

    XfrRingsSize = Usb3Hc->CapRegs.HcsParams1.MaxSlots * 32 * 32;   // 32 endpoints per device, 32 padded size of TRB_RING
    XfrTrbsSize = RING_SIZE * Usb3Hc->CapRegs.HcsParams1.MaxSlots *32;
    DeviceContextSize = (XHCI_DEVICE_CONTEXT_ENTRIES * Usb3Hc->ContextSize) * Usb3Hc->CapRegs.HcsParams1.MaxSlots;

//    MemSize = 0x2800 + XfrRingsSize + XfrTrbsSize + DeviceContextSize;
    MemSize = 0x6400 + XfrRingsSize + XfrTrbsSize + DeviceContextSize;

	gBS->FreePages((EFI_PHYSICAL_ADDRESS)Usb3Hc->DcbaaPtr, EFI_SIZE_TO_PAGES(MemSize));
	if (Usb3Hc->ScratchBufEntry) {
	    gBS->FreePages((EFI_PHYSICAL_ADDRESS)Usb3Hc->ScratchBufEntry, EFI_SIZE_TO_PAGES((sizeof(UINT64) * NumBufs)));
	}
	//gBS->FreePool(Usb3Hc);
*/
#endif
	return EFI_SUCCESS;
}

/**
    This function record USB host controller memory start address 
    and total memory pages used
    @param  Handle              Host controller handle number
            MemoryStartAddress  Host controller memory start address
            MemoryTotalPages    Host controller total memory used

    @retval EFI_STATUS Status of the operation

**/
EFI_STATUS
EFIAPI
UsbHcMemoryRecord(
    EFI_HANDLE              Handle,
    EFI_PHYSICAL_ADDRESS    MemoryStartAddress,
    UINTN                   MemoryTotalPages
)
{
    EFI_STATUS  Status;
    EFI_GUID    AmiUsbHcMemRecordProtocolGuid = AMI_USB_HC_MEM_RECORD_PROTOCOL_GUID;
    AMI_USB_HC_MEM_RECORD_PROTOCOL  *AmiUsbHcMemRecord = NULL;
    
    Status = gBS->HandleProtocol(
            Handle,
            &AmiUsbHcMemRecordProtocolGuid,
            (VOID **) &AmiUsbHcMemRecord);

    if(EFI_ERROR(Status)){
        Status = gBS->AllocatePool(EfiBootServicesData,
                sizeof(AMI_USB_HC_MEM_RECORD_PROTOCOL),
                (VOID**)&AmiUsbHcMemRecord);
        if (EFI_ERROR(Status)) {
            return Status;
        }
        
        SetMem(AmiUsbHcMemRecord, sizeof(AMI_USB_HC_MEM_RECORD_PROTOCOL), 0);
        AmiUsbHcMemRecord->HcMemStart = MemoryStartAddress;
        AmiUsbHcMemRecord->MemTotalPages = MemoryTotalPages;
        Status = gBS->InstallProtocolInterface(
                &Handle,
                &AmiUsbHcMemRecordProtocolGuid,
                EFI_NATIVE_INTERFACE,
                AmiUsbHcMemRecord);
        if (EFI_ERROR(Status)) {
            gBS->FreePool(AmiUsbHcMemRecord);
            return Status;
        }
    }
    return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
