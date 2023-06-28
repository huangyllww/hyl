//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2015, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**    5555 Oakbrook Pkwy, Norcross, Suite 200, Georgia 30093   **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//

//****************************************************************************
// $Header: $
//
// $Revision: $
//
// $Date:  $
//****************************************************************************
// Revision History
// ----------------
// $Log: $
//
//
//
//****************************************************************************

//<AMI_FHDR_START>
//---------------------------------------------------------------------------
//
// Name: IpmiInterface.c
//
// Description: 
//
//---------------------------------------------------------------------------
//<AMI_FHDR_END>

#include <AmiDxeLib.h>
#include <Token.h>
#include "Library/BaseMemoryLib.h"
#include "Library/MemoryAllocationLib.h"
#include "AmiPldmInterfaces.h"
#include "Protocol/IpmiUsbTransportProtocol.h"

extern EFI_BOOT_SERVICES *pBS;

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:   ExecuteBMCOemIpmiCommand
//
// Description: This procedure sends the OEM IPMI Command and provide the
//              output.
//
// Parameter:  UINT8 NetFunction,UINT8 Lun,UINT8 Command,UINT8 CommandDataSize,
//              UINT8 *CommandData,UINT8 *ResponseData,UINT8 *ResponseDataSize, UINT8 *CommandCompletionCode
//
// Return Value: EFI_STATUS
//--------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS ExecuteBMCOemIpmiCommand(
    UINT8 NetFunction, UINT8 Lun, UINT8 Command,
    UINT8 *CommandData, UINT32 CommandDataSize,
    UINT8 **ResponseData, UINT32 *ResponseDataSize, UINT8 *CommandCompletionCode
)
{
    EFI_GUID EfiIpmiUsbGuid = EFI_IPMI_USB_TRANSPORT_PROTOCOL_GUID;
    IPMI_USB_TRANSPORT      *pIpmiUsbTransportService;
    EFI_STATUS  Status = EFI_SUCCESS;
    UINT8 *pTempResponse;
    UINT32 TempSize;

    if(ResponseDataSize == NULL)
        TempSize = 0;
    else
        TempSize = *ResponseDataSize;

    //Locate the IPMI USB Protocol
    Status = pBS->LocateProtocol (&EfiIpmiUsbGuid, NULL, &pIpmiUsbTransportService);
    DEBUG((DEBUG_ERROR, "\n ExecuteBMCOemIpmiCommand LocateProtocol Status: %x", Status));

    if (EFI_SUCCESS == Status)
    {
        DEBUG((DEBUG_ERROR, "\n ExecuteBMCOemIpmiCommand TempSize: %x", TempSize));
        pTempResponse = (UINT8*)AllocatePool(TempSize);

        if (NULL == pTempResponse)
        {
            DEBUG((DEBUG_ERROR, "\n ExecuteBMCOemIpmiCommand pTempResponse EFI_OUT_OF_RESOURCES"));
            return EFI_BUFFER_TOO_SMALL;
        }
        else
        {
            Status = pIpmiUsbTransportService->SendOemIpmiCommand(pIpmiUsbTransportService, NetFunction, Lun, Command,
                     CommandData, CommandDataSize, pTempResponse, &TempSize);
            DEBUG((DEBUG_ERROR, "\n ExecuteBMCOemIpmiCommand(): SendOemIpmiCommand Status: %x, CC: %x", Status, pIpmiUsbTransportService->CommandCompletionCode));
            *CommandCompletionCode = pIpmiUsbTransportService->CommandCompletionCode;

            if ( EFI_BUFFER_TOO_SMALL == Status)
            {
                pTempResponse = (UINT8*)AllocatePool(TempSize);

                if (NULL == pTempResponse)
                {
                    DEBUG((DEBUG_ERROR, "\n ExecuteBMCOemIpmiCommand pTempResponse EFI_OUT_OF_RESOURCES"));
                    Status = EFI_BUFFER_TOO_SMALL;
                }
                else
                {
                    Status = pIpmiUsbTransportService->SendOemIpmiCommand(pIpmiUsbTransportService, NetFunction, Lun, Command,
                             CommandData, CommandDataSize, pTempResponse, &TempSize);
                    DEBUG((DEBUG_ERROR, "\n ExecuteBMCOemIpmiCommand(): SendOemIpmiCommand Status 1: %x, CC: %x", Status, pIpmiUsbTransportService->CommandCompletionCode));
                    *CommandCompletionCode = pIpmiUsbTransportService->CommandCompletionCode;
                }
            }

            if ( EFI_SUCCESS == Status )
            {
                if (0 != TempSize)
                {
                    *ResponseDataSize = TempSize;
                    *ResponseData = (UINT8*)AllocatePool(TempSize);

                    if (NULL == *ResponseData)
                    {
                        DEBUG((DEBUG_ERROR, "\n ExecuteBMCOemIpmiCommand EFI_OUT_OF_RESOURCES"));
                        Status = EFI_BUFFER_TOO_SMALL;
                    }
                    else
                    {
                        CopyMem(*ResponseData, pTempResponse, *ResponseDataSize);
                    }
                }
                else
                {
                    *ResponseDataSize = 0;
                }
            }

            if (pTempResponse)
            {
                FreePool(pTempResponse);
            }
        }
    }

    return Status;
}

//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2015, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**    5555 Oakbrook Pkwy, Norcross, Suite 200, Georgia 30093   **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
