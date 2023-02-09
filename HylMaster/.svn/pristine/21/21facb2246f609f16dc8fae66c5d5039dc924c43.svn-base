//<LiuYB038-20180227 Reset BMC Administrator's Name and Password +>

/** @file OemBmcAdminReset.c

    Reset Administrator's User Name and Password

**/

//----------------------------------------------------------------------

#include <Protocol/IpmiTransportProtocol.h>
#include <Include/IpmiNetFnAppDefinitions.h>
#include <Protocol/AMIPostMgr.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Include/ServerMgmtSetupVariable.h>
#include <ServerMgmtSetup.h>
#include <Library/HiiLib.h>

//----------------------------------------------------------------------

#define STRING_BUFFER_LENGTH    512

/**
    This function reset bmc administrator's name and password

    @param This - IpmiTransportProtocol pointer.

    @return EFI_STATUS Return Status.
**/

EFI_STATUS
BmcAdminReset(
    IN EFI_IPMI_TRANSPORT        *This
    )
{
    // For JD, the administrator's name is "jdroot", password is "JCss%6!8"
    // jdroot   - 0x6a 0x64 0x72 0x6f 0x6f 0x74
    // JCss%6!8 - 0x4a 0x43 0x73 0x73 0x25 0x36 0x21 0x38

    EFI_STATUS  Status      = EFI_SUCCESS;
    EFI_STATUS  StatusError = EFI_SUCCESS;  // record the fist error code
    UINT8  UserName[]     = { 2,    0x6a, 0x64, 0x72, 0x6f, 0x6f, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; // user id = 2, user name 16 char
    UINT8  UserAccess[]   = { 0xFF, 2, 4 }; // channel, user id = 2, administrator
    UINT8  UserPassword[] = { 2, 2, 0x4a, 0x43, 0x73, 0x73, 0x25, 0x36, 0x21, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; // user id = 2, set password = 2, password 16 char
    UINT8  UserEnable[]   = { 2, 1 };  // user id = 2, enable user = 1
    UINT8  OemGroup[]     = { 2, 1, 1 };  // user id = 2, admin group = 1, type flag=1
    UINT8  CommandSize;
    UINT8  ResponseData[30] = { 0 };
    UINT8  ResponseDataSize;
    UINT8  Index;

    CommandSize = sizeof(UserName);
    ResponseDataSize = 0x00;
    Status = This->SendIpmiCommand(
                This,
                0x06, // NETFN_APP
                BMC_LUN,
                0x45,  // set user name
                UserName,
                CommandSize,
                ResponseData,
                &ResponseDataSize
                );
    if (StatusError == EFI_SUCCESS) {
        StatusError = Status;
    }

    for (Index = 0; Index < 16; Index++) {
        UserAccess[0] = Index;
        CommandSize = sizeof(UserAccess);
        ResponseDataSize = 0x00;
        Status = This->SendIpmiCommand(
                    This,
                    0x06, // NETFN_APP
                    BMC_LUN,
                    0x43,  // set user access
                    UserAccess,
                    CommandSize,
                    ResponseData,
                    &ResponseDataSize
                    );
        if (StatusError == EFI_SUCCESS) {
            StatusError = Status;
        }
    }

    CommandSize = sizeof(UserPassword);
    ResponseDataSize = 0x00;
    Status = This->SendIpmiCommand(
                This,
                0x06, // NETFN_APP
                BMC_LUN,
                0x47,  // set user password
                UserPassword,
                CommandSize,
                ResponseData,
                &ResponseDataSize
                );
    if (StatusError == EFI_SUCCESS) {
        StatusError = Status;
    }

    CommandSize = sizeof(UserEnable);
    ResponseDataSize = 0x00;
    Status = This->SendIpmiCommand(
                This,
                0x06, // NETFN_APP
                BMC_LUN,
                0x47,  // enable user
                UserEnable,
                CommandSize,
                ResponseData,
                &ResponseDataSize
                );
    if (StatusError == EFI_SUCCESS) {
        StatusError = Status;
    }

    CommandSize = sizeof(OemGroup);
    ResponseDataSize = 0x00;
    Status = This->SendIpmiCommand(
                This,
                0x3a, // OEM Cmd
                BMC_LUN,
                0xC3,  // user group
                OemGroup,
                CommandSize,
                ResponseData,
                &ResponseDataSize
                );
    if (StatusError == EFI_SUCCESS) {
        StatusError = Status;
    }

    return StatusError;
}

/**
    This setup callback function reset bmc administaror's name and password
    per user choice.

    @param HiiHandle A handle that was previously registered in the
                     HII Database.
    @param Class    Formset Class of the Form Callback Protocol passed in
    @param SubClass Formset sub Class of the Form Callback Protocol passed in
    @param Key Formset Key of the Form Callback Protocol passed in

    @return EFI_STATUS Return Status

**/

EFI_STATUS
BmcAdminResetCallbackFunction(
    IN  EFI_HII_HANDLE     HiiHandle,
    IN  UINT16             Class,
    IN  UINT16             SubClass,
    IN  UINT16             Key
    )
{
    EFI_STATUS                  Status = EFI_SUCCESS;
    CALLBACK_PARAMETERS         *CallbackParameters = NULL;
    AMI_POST_MANAGER_PROTOCOL   *AmiPostMgr = NULL;
    EFI_IPMI_TRANSPORT          *IpmiTransport = NULL;
    UINTN                       BufferLength;
    CHAR16                      TitleStr[STRING_BUFFER_LENGTH];
    EFI_STATUS                  TitleStrStatus = EFI_SUCCESS;
    CHAR16                      OutputStr[STRING_BUFFER_LENGTH];
    EFI_STATUS                  OutputStrStatus = EFI_SUCCESS;
    UINT8                       MsgBoxSel;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "[OemDbg] BmcAdminReset - Key Value : %x\n", Key));

    //
    // Continue only for OEM_BMC_ADMIN_RESET_KEY
    //
    if (Key != OEM_BMC_ADMIN_RESET_KEY) {
        return EFI_UNSUPPORTED;
    }

    //
    // Get the call back parameters and verify the action
    //
    CallbackParameters = GetCallbackParameters();
    if (CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED) {
        SERVER_IPMI_DEBUG ((EFI_D_INFO,"[OemDbg] BmcAdminReset - Action != EFI_BROWSER_ACTION_CHANGED\n"));
        return EFI_SUCCESS;
    }

    //
    // Locate AmiPostMgrProtocol to display the message boxes in setup
    //
    Status = gBS->LocateProtocol(
                &gAmiPostManagerProtocolGuid,
                NULL,
                (VOID **)&AmiPostMgr
                );
    if (EFI_ERROR(Status)) {
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "[OemDbg] BmcAdminReset - locate AmiPostManagerProtocol: %r\n", Status));
        return Status;
    }

    //
    // Locate IpmiTransport protocol
    //
    Status = gBS->LocateProtocol (
                 &gEfiDxeIpmiTransportProtocolGuid,
                 NULL,
                 (VOID **)&IpmiTransport
                 );
    if (EFI_ERROR(Status)) {

        SERVER_IPMI_DEBUG ((EFI_D_INFO, "[OemDbg] BmcAdminReset - locate IpmiTransportProtocol: %r\n", Status));

        BufferLength = STRING_BUFFER_LENGTH;
        TitleStrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_OEM_BMC_ADMIN_RESET_ERROR), TitleStr, &BufferLength, NULL);

        BufferLength = STRING_BUFFER_LENGTH;
        OutputStrStatus = IpmiHiiGetString(HiiHandle, STRING_TOKEN(STR_PROTOCOL_NOT_FOUND), OutputStr, &BufferLength, NULL);

        if ((!EFI_ERROR(OutputStrStatus)) && (!EFI_ERROR(TitleStrStatus))) {
            AmiPostMgr->DisplayMsgBox(TitleStr, OutputStr, MSGBOX_TYPE_OK, &MsgBoxSel);
        }

        return Status;
    }

    //
    // Get confirmation from the user before doing reset
    //
    BufferLength = STRING_BUFFER_LENGTH;
    TitleStrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_OEM_BMC_ADMIN_RESET_INFO), TitleStr, &BufferLength, NULL);
    if (EFI_ERROR(TitleStrStatus)) {
        return TitleStrStatus;
    }

    BufferLength = STRING_BUFFER_LENGTH;
    OutputStrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_OEM_BMC_ADMIN_RESET), OutputStr, &BufferLength, NULL);
    if (EFI_ERROR(OutputStrStatus)) {
        return OutputStrStatus;
    }

    //
    // if user said no, then EFI_SUCCESS
    //
    Status = AmiPostMgr->DisplayMsgBox(TitleStr, OutputStr, MSGBOX_TYPE_YESNO, &MsgBoxSel);
    if((EFI_ERROR(Status)) || MsgBoxSel == MSGBOX_NO) {
        SERVER_IPMI_DEBUG ((EFI_D_INFO, "[OemDbg] BmcAdminReset - DisplayMsgBox: %r, MsgBoxSel: %x\n", Status, MsgBoxSel));
        return EFI_SUCCESS;
    }

    //
    // Send factory default command to BMC
    //
    Status = BmcAdminReset(IpmiTransport);
    
    BufferLength = STRING_BUFFER_LENGTH;
    TitleStrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_OEM_BMC_ADMIN_RESET_INFO), TitleStr, &BufferLength, NULL);

    BufferLength = STRING_BUFFER_LENGTH;
    OutputStrStatus = IpmiHiiGetString (HiiHandle, STRING_TOKEN(STR_OEM_BMC_ADMIN_RESET_DONE), OutputStr, &BufferLength, NULL);

    if ((!EFI_ERROR(OutputStrStatus)) && (!EFI_ERROR(TitleStrStatus))) {
        AmiPostMgr->DisplayMsgBox(TitleStr, OutputStr, MSGBOX_TYPE_OK, &MsgBoxSel);
    }

    return Status;
}
//<LiuYB038-20180227 Reset BMC Administrator's Name and Password ->
