//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2018, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************

/** @file AMISetupNVLock.h
    This file contains AMISetupNVLock header

**/

#ifndef _AMISETUPNVLOCK_H_
#define	_AMISETUPNVLOCK_H_

#define TSE_SETUP_VAR_NAME	L"AMITSESetup"
#define SETUPPASS_VAR_NAME L"$SETUPPASSWD"
#define SET_ADMINPASS_VAR_NAME L"$SETADMINPASSWD"
#define SET_USERPASS_VAR_NAME L"$SETUSERPASSWD"

#define NULL_GUID \
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }

#define SETUPPASSWD_GUID \
	{0x5855CE1B, 0xFB8E, 0x47E4, 0xBC, 0x1A, 0x39, 0xEC, 0xAA, 0x0C, 0x96, 0xCF}

// Need new TSE 2.17.xxx.
#ifndef AMITSE_EVENT_AFTER_BOOT_GUID
// {8c12A959-70BC-4362-B437-bB80514a1916e}
#define AMITSE_EVENT_AFTER_BOOT_GUID \
  {0x8c12a959, 0x70bc, 0x4362, 0xb4, 0x37, 0xb8, 0x05, 0x14, 0xa1, 0x91, 0x6e}
#endif

#define	AMI_PASSWORD_NONE		0
#define	AMI_PASSWORD_USER		1
#define	AMI_PASSWORD_ADMIN		2
#define	AMI_INVALID_PASSWORD	3

BOOLEAN IsNVWriteProtected();
UINT32 AuthenticatePassword( CHAR16 *Password,  UINTN Passwordsize);
EFI_STATUS DxeGetVariable(IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid, OUT UINT32 *Attributes OPTIONAL,
                          IN OUT UINTN *DataSize, OUT VOID *Data);
EFI_STATUS DxeSetVariable(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    IN UINT32 Attributes, IN UINTN DataSize, IN VOID *Data
);
VOID PasswordEncode( CHAR16 *Password, UINTN MaxSize);
EFI_STATUS LockNV();
EFI_STATUS UnlockNV();
void ConvertChar16toChar8(CHAR16 *ErrorString, CHAR8 *OutString);
EFI_STATUS SetPassword(CHAR16 *VariableName, VOID *Password, UINTN Passwordsize);
EFI_STATUS ValidateandSetSetupPwd(CHAR16 *VariableName, EFI_GUID *VendorGuid, UINT32 Attributes, UINTN DataSize, VOID *Data);

#endif /* _AMISETUPNVLOCK_H_ */

//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2018, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
