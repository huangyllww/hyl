//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
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
// $Header: /Alaska/Tools/template.c 6     1/13/10 2:13p Felixp $
//
// $Revision: 6 $
//
// $Date: 1/13/10 2:13p $
//**********************************************************************
// Revision History
// ----------------
// $Log: /Alaska/Tools/template.c $
//
// 6     1/13/10 2:13p Felixp
//
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  <This File's Name>
//
// Description: <jiangzhy001-20180928 - XHCI port can be enabled or disabled singly +>
//
//<AMI_FHDR_END>
//**********************************************************************

#include <AmiDxeLib.h>
#include <Library\Protocol\AmdCpmTableProtocol\AmdCpmTableProtocol.h>
#include <Include\AmdCpmFunction.h>
#include "token.h"
#include <Include\Setup.h>
#include <Library\DebugLib.h>
#include "OemLib.h"
#include "UsbPortControl.h"

EFI_GUID gEfiSetupGuid = SETUP_GUID;
UINT32 Attributes;
UINTN VariableSize = sizeof(SETUP_DATA);
SETUP_DATA SetupData;

#define USBStringID(Socket, Die, Port,String) 0x0

USB_PORT_CONTROL UsbPortControl[] = {
    {CPU0_DIE0_Port0,   {TRUE, L"NULL",L"NULL",USBStringID(0, 0, 0,_PROMPT),USBStringID(0, 0, 0,_PROMPT_HELP)}},
    {CPU0_DIE0_Port1,   {TRUE, L"NULL",L"NULL",USBStringID(0, 0, 0,_PROMPT),USBStringID(0, 0, 0,_PROMPT_HELP)}},
    {CPU0_DIE1_Port0,   {TRUE, L"NULL",L"NULL",USBStringID(0, 0, 0,_PROMPT),USBStringID(0, 0, 0,_PROMPT_HELP)}},
    {CPU0_DIE1_Port1,   {TRUE, L"NULL",L"NULL",USBStringID(0, 0, 0,_PROMPT),USBStringID(0, 0, 0,_PROMPT_HELP)}},
    {PortTerminate,     {TRUE, L"NULL",L"NULL",USBStringID(0, 0, 0,_PROMPT),USBStringID(0, 0, 0,_PROMPT_HELP)}}};

#define CheckUSBPort0Status(Socket, Die)                                               \
    if (SetupData.USBPortControl[USB_PORT_OFFSET(Socket, Die, 0)] == USB_PORT_DISABLE) \
    {                                                                                  \
        SetupData.USBPortControl[USB_PORT_OFFSET(Socket, Die, 1)] = USB_PORT_DISABLE;  \
        SetupData.USBPortControl[USB_PORT_OFFSET(Socket, Die, 0)] = USB_PORT_DISABLE;  \
        SetupData.USBPortControl[USB_PORT_OFFSET(Socket, Die, 1)] = USB_PORT_DISABLE;  \
    }

//Mizl-20200616-Mantis 0040303: USB 2.0 Control Item Works Abnormal ->
EFI_STATUS EnableOrDisableEachUSBPort()
{

    EFI_STATUS Status;
    AMD_CPM_TABLE_PROTOCOL *AmdCpmTableProtocolPtr;
    // UINT32                      EnableUsbPort = 0;
    EFI_GUID gAmdCpmTableProtocolGuid = AMD_CPM_TABLE_PROTOCOL_GUID;
    // //Mizl-20200616-Mantis 0040303: USB 2.0 Control Item Works Abnormal +>
    UINT32 USBPortDisableRead = 0;
    // //Mizl-20200616-Mantis 0040303: USB 2.0 Control Item Works Abnormal ->
    UINT8 iUSBPort;

    UINT8 Socket = 0, Die = 0, Port = 0;

    MODULE_START

    Status = pBS->LocateProtocol(&gAmdCpmTableProtocolGuid,
                                 NULL,
                                 (VOID **)&AmdCpmTableProtocolPtr);
    if (EFI_ERROR(Status))
        MODULE_END(Status)

    /**
    Note:Check USB Port0 Control Flag
    **/
    CheckUSBPort0Status(0, 0);
    CheckUSBPort0Status(0, 1);

    for (iUSBPort = 0; iUSBPort < TOTAL_USB_COUNT; iUSBPort++)
        DEBUG((EFI_D_INFO, "Port:%x USBPortPresent:%x USBPortControl:%x\r\n", iUSBPort, SetupData.USBPortPresent[iUSBPort], SetupData.USBPortControl[iUSBPort]));

    /**
    Note:Disable USB Port
    **/
    for (iUSBPort = 0; UsbPortControl[iUSBPort].CPUUsbPortLocation != PortTerminate; iUSBPort++)
    {
        /* code */
        DEBUG((EFI_D_INFO, "UsbPortControl - Port:%x  USBPortControl:%x\r\n", UsbPortControl[iUSBPort].CPUUsbPortLocation, SetupData.USBPortControl[UsbPortControl[iUSBPort].CPUUsbPortLocation]));

        if (SetupData.USBPortControl[UsbPortControl[iUSBPort].CPUUsbPortLocation] != USB_PORT_DISABLE)
            continue;

        Socket = gaCPUUsbMap[UsbPortControl[iUSBPort].CPUUsbPortLocation].Socket;
        Die = gaCPUUsbMap[UsbPortControl[iUSBPort].CPUUsbPortLocation].Die;
        Port = gaCPUUsbMap[UsbPortControl[iUSBPort].CPUUsbPortLocation].Port;

        USBPortDisableRead = AmdCpmTableProtocolPtr->CommonFunction.SmnRead32(AmdCpmTableProtocolPtr, Socket, Die, FCH_TS_USB_PORT_DISABLE0, 0);
        DEBUG((EFI_D_INFO, "[%a][%d]Port %s Current Setting:%x\n\n", __FUNCTION__, __LINE__, UsbPortControl[iUSBPort].UsbPort[0].PortDescription, USBPortDisableRead));

        USBPortDisableRead = USBPortDisableRead | (1 << Port) | (1 << (Port + 16));
        DEBUG((EFI_D_INFO, "[%a][%d]Port %s Atfer Setting:%x\n\n", __FUNCTION__, __LINE__, UsbPortControl[iUSBPort].UsbPort[0].PortDescription, USBPortDisableRead));
        AmdCpmTableProtocolPtr->CommonFunction.SmnWrite32(AmdCpmTableProtocolPtr, Socket, Die, FCH_TS_USB_PORT_DISABLE0, 0, USBPortDisableRead);
    }
    MODULE_END(Status = EFI_SUCCESS)
}

EFI_STATUS UsbPortControlEntryPoint(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS Status;

    UINT8 iPort;

    InitAmiLib(ImageHandle, SystemTable);

    MODULE_START
    //Sync USB Port Implement Info
    Status = pRS->GetVariable(L"Setup", &gEfiSetupGuid, &Attributes, &VariableSize, &SetupData);
    if (EFI_ERROR(Status))
        MODULE_END(Status)
    /**
    Note:Disable all Port as disabled in default
    **/
    for (iPort = 0; iPort < TOTAL_USB_COUNT; iPort++)
        SetupData.USBPortPresent[iPort] = USB_PORT_DISABLE;

    /**
   Note:Enable USB Port By Porting Table
   **/
    for (iPort = 0; UsbPortControl[iPort].CPUUsbPortLocation != PortTerminate; iPort++)
        SetupData.USBPortPresent[UsbPortControl[iPort].CPUUsbPortLocation] = 1;

    for (iPort = 0; iPort < TOTAL_USB_COUNT; iPort++)
        DEBUG((EFI_D_INFO, "Port:%x USBPortPresent:%x USBPortControl:%x\r\n", iPort, SetupData.USBPortPresent[iPort], SetupData.USBPortControl[iPort]));

    Status = EnableOrDisableEachUSBPort();
    if (EFI_ERROR(Status))
        MODULE_END(Status)

    /**
    Note:Sync Setup Variable
    **/
    Status = pRS->SetVariable(L"Setup", &gEfiSetupGuid, Attributes, VariableSize, &SetupData);
    if (EFI_ERROR(Status))
        MODULE_END(Status)

    MODULE_END(Status = EFI_SUCCESS)
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
