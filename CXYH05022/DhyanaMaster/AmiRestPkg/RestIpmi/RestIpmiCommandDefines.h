//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file RestIpmiCommandDefinitions.h
	This file contains definitions required for IPMI OEM commands to support RESTful exchange between BIOS and BMC.

**/


#ifndef __REST_IPMI_COMMAND_DEFINITIONS_H__
#define __REST_IPMI_COMMAND_DEFINITIONS_H__

#define ERR_REST_IPMI_SUCCESS				0x00		/* Success */
#define ERR_REST_IPMI_OUT_OF_MEMORY			0x82		/* BMC out of memory */
#define ERR_REST_IPMI_INV_TRANS_ID			0x83		/* Invalid transaction ID */
#define ERR_REST_IPMI_INV_CHECK_SUM			0x84		/* Invalid check sum */
#define ERR_REST_IPMI_NO_REDFISH_RESPONSE	0x85		/* No Redfish response on BMC side */
#define ERR_REST_IPMI_BMC_TRANS_INTERRUPT	0x86		/* Interruption on BMC side */
#define ERR_REST_IPMI_BMC_MAX_COMMAND_SIZE	0x87		/* Exceeds maximum command size configured in BMC */
#define ERR_REST_IPMI_INVALID_COMMAND		0xC1		/* Invalid command */
#define ERR_REST_IPMI_TIME_OUT				0xC3		/* Time-out during a processing the command */
#define ERR_REST_IPMI_INV_REQUEST_SIZE		0xC7		/* Invalid request size */
#define ERR_REST_IPMI_INV_REQUEST_BYTES		0xCC		/* Invalid request bytes */

#define MAX_IPMI_PACKET_SIZE 16384

#define REST_NET_FUNCTION 0x32
#define REST_LUN 0x0
#define REST_COMMAND 0x5C
  
  
#pragma pack(1)
typedef struct {
  UINT8    StartPacketBit:1;
  UINT8    EndPacketBit:1;
  UINT8    ResendPacketBit:1;
  UINT8    EndTransactionBit:1;
  UINT8    InterruptTransactionBit:1;
  UINT8    Reserved:3;
} REST_IPMI_PACKET_ID_BITS;

typedef union {
  UINT8        RawData;
  REST_IPMI_PACKET_ID_BITS   PacketIDBits;
} REST_IPMI_PACKET_ID;

typedef struct {
  //
  // Byte 1
  //
  REST_IPMI_PACKET_ID PacketID;
  //
  // Byte 2
  //
  UINT8    TransactionID;
  //
  // Byte 3-6
  //
  UINT32    TotalSize;
  //
  // Data 7-8
  //
  UINT16    PacketSize;
  //
  // Byte 9-12 
  //
  UINT32    CheckSum;
} REST_IPMI_REQUEST_PARAMS;

typedef struct {
  //
  // Byte 1-12
  //
  REST_IPMI_REQUEST_PARAMS RestIpmiRequestParams;
  //
  // Byte 13-N
  //
  UINT8    *RestData;
} REST_IPMI_REQUEST_DATA;




typedef struct {
  //
  // Byte 1
  //
  REST_IPMI_PACKET_ID PacketID;
  //
  // Byte 2
  //
  UINT8    TransactionID;
  //
  // Byte 3-6
  //
  UINT32    TotalSize;
  //
  // Data 7-8
  //
  UINT16    PacketSize;
  //
  // Byte 9-12 
  //
  UINT32    CheckSum;
} REST_IPMI_RESPONSE_PARAMS;




typedef struct {
  //
  // Byte 1-12
  //
  REST_IPMI_RESPONSE_PARAMS RestIpmiResponseParams;
  //
  // Byte 13-N
  //
  UINT8    *RestData;
} REST_IPMI_RESPONSE_DATA;
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
