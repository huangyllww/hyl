//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **

/** @file BmcUserSettings.h
    Includes for BMC User Settings.

**/

#ifndef _BMC_USER_SETTINGS_H_
#define _BMC_USER_SETTINGS_H_

//----------------------------------------------------------------------

#include <Protocol/IpmiTransportProtocol.h>
#include <Include/IpmiNetFnAppDefinitions.h>
#include <Protocol/AMIPostMgr.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Include/ServerMgmtSetupVariable.h>
#include <ServerMgmtSetup.h>
#include <Include/IpmiBmcCommon.h>
#include <Library/BaseLib.h>
#include <Include/AmiDxeLib.h>
#include <Protocol/AMIPostMgr.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HiiLib.h>

//----------------------------------------------------------------------

//
// Macro definitions
//
#define MAX_BMC_USER_COUNT        64
#define MAX_BMC_USER_NAME_LENGTH  16
#define MAX_BMC_USER_NAME_LENGTH_WITH_NULL_CHARACTER  17

//<liuyb-20170801 add bmc user in multi-channel +>
#define MAX_BMC_CHANNEL_LENGTH    8
#define MAX_BMC_CHANNEL_LENGTH_WITH_NULL_CHARACTER    9
//<liuyb-20170801 add bmc user in multi-channel ->

// <xuyj1-20190115 adjust user privilege +>
//Oem BMC Command define
#define ADD_DELETE_USER_FROM_GROUP  0xC3
#define GET_GROUP_OF_USER           0xC4
#define OEM_IPMI_NET_FUNCTION     0x3A
#define GET_USER_GROUP_COMMAND      0xC7
//AMI BMC Command define
#define SET_USER_SNMP_COMMAND       0x7D
#define SET_USER_PRIVILEGE_COMMAND  0xA3
#define AMI_IMPI_NET_FUNCTION       0x32

#define MAX_BMC_GROUP_COUNT         0x0B
// <xuyj1-20190115 adjust user privilege ->

#define LAN_CHANNEL_TYPE 4
#define MAX_CHANNEL_NUMBER 0x0F

#define MAX_BMC_USER_PASSWORD_LENGTH  BMC_USER_PASSWORD_MAX_SIZE
#define MAX_BMC_USER_PASSWORD_LENGTH_WITH_NULL_CHARACTER  BMC_USER_PASSWORD_MAX_SIZE+1

#define STRING_BUFFER_LENGTH    4000

#pragma pack(1)
typedef struct _BMC_USER_DETAILS_LIST {
  UINT8     UserId;
  CHAR8     UserName[MAX_BMC_USER_NAME_LENGTH_WITH_NULL_CHARACTER];
}BMC_USER_DETAILS_LIST;

typedef struct _BMC_USER_DETAILS {
  UINT8       UserId;
  CHAR8       UserName[MAX_BMC_USER_NAME_LENGTH_WITH_NULL_CHARACTER];
  CHAR8       UserPassword[MAX_BMC_USER_PASSWORD_LENGTH_WITH_NULL_CHARACTER];
}BMC_USER_DETAILS;

// <xuyj1-20190115 adjust user privilege +>
typedef struct _BMC_ADD_DELETE_USER_FROM_GROUP {
    UINT8       UserId; 
    UINT8       GroupId;
    UINT8       Flags;//2- Delete  1-Add
}BMC_ADD_DELETE_USER_FROM_GROUP;

typedef struct _BMC_GET_GROUP_OF_USER_COMMAND{
    UINT8       UserId;
}BMC_GET_GROUP_OF_USER_COMMAND;

typedef struct _BMC_GET_GROUP_OF_USER_RESPONSE{
    UINT8       GroupId;  
}BMC_GET_GROUP_OF_USER_RESPONSE;


typedef struct _BMC_SET_USER_SNMP_COMMAND{
    UINT8       UserId;
    UINT8       SnmpByte; // 0- disable 1 -enable
    UINT8       SnmpAcessType;//1-R 2 -RW
    UINT8       SnmpEncryption1; //1-SHA 2-MD5
    UINT8       SnmpEncryption2;  //1-DES 2-AES
}BMC_SET_USER_SNMP_COMMAND;

typedef struct _BMC_SET_USER_PRIVILEGE_COMMAND{
    UINT8       UserId;
    UINT32      ExtendedPrivilege; // bit 0- KVM  bit1- VMedia
}BMC_SET_USER_PRIVILEGE_COMMAND;

typedef struct _BMC_GET_USER_GROUP_COMMAND{
    UINT8       UserId;
}BMC_GET_USER_GROUP_COMMAND;

typedef struct _BMC_GROUP_INFORMATION{
    UINT8       GroupIndex;
    CHAR8       GroupName[32];
    UINT8       GroupPrivilege;
    UINT8       GroupExtendedPrivilege;//Bit0��0b  Disable KVM 1b  Enable KVM ; Bit1��0b  Disable VMedia 1b  Enable VMedia; Bit2��0b  Disable SNMP 1b  Enable SNMP
}BMC_GROUP_INFORMATION;

typedef struct _BMC_GET_USER_GROUP_RESPOND{
    UINT8       UserGroupId;
    BMC_GROUP_INFORMATION   UserGroupCount[MAX_BMC_GROUP_COUNT];
}BMC_GET_USER_GROUP_RESPOND;
//<xuyj1-20190115 adjust user privilege ->
#pragma pack()
#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
