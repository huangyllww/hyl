//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************

/** @file 
AfuCommunication.h

Header file for the AFUCommunication

*/

#ifndef _EFI_AFUBIOSPRESERVECOMN_H_
#define _EFI_AFUBIOSPRESERVECOMN_H_

#pragma pack(1)

#define AFU_BCP_SHARING_PROTOCOL_GUID  \
    { 0x6f99815a, 0x5b81, 0x438d, 0x96, 0x4f, 0x97, 0xb1, 0x1c, 0xfe, 0x4a, 0x16 }

#define RECOVERY_FLASH_UPDATE 	0
#define RUNTIME_FLASH_UPDATE 	1
#define CAPSULE_FLASH_UPDATE 	2
#define PLDM_DATA_SHARING 		3

///
/// Type Code 80 Get & Set Status
///

#define OFBD_TC_BCP_SET_CONFIG_PRESERVE_INFO    BIT00
#define OFBD_TC_BCP_LOAD_PLDM_DATA              BIT01

///
/// Type Code 80 Return Status   
///
#define OFBD_TC_BCP_RS_OK                       BIT00
#define OFBD_TC_BCP_RS_NOT_SUPPORTED            BIT01
#define OFBD_TC_BCP_RS_PRESERVE_CFG_FAILED      BIT02
#define OFBD_TC_BCP_RS_FV_SIZE_EXCEED_FAILED    BIT03
#define OFBD_TC_BCP_RS_FV_SUPPORT_ENABLED      	BIT04

///
/// Structures for Get & Set Status
///

typedef struct
{
	UINT8 	Command;  		// BIOS config preserve command
    UINT8 	Status;   		// Return status
} OFBD_TC_80_BCP_STRUCT;
 
typedef struct {
    UINT8 	FlashOpType;   	// Indicates the flash mode
    UINT32 	Length;       	// Size of entire structure
//    UINT8 	*PldmFileName; 	// PLDM file name
} BIOS_CONFIG_PRESERVE_INFO;
 
typedef struct 
{
    UINT32 	Length;  		// Size of PLDM data block
//    UINT8 	*Data;    		// PLDM data block
} BCP_PLDM_DATA_BLOCK;


VOID BiosConfigPreserveInSmm (
	VOID
);

VOID BiosConfigPreserveFnc (
	VOID		*Buffer,
	UINT8		*pOFBDDataHandled
);

typedef struct _AFU_BCP_SHARING_PROTOCOL AFU_BCP_SHARING_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *AFU_BCP_GETVARIABLE) (
	IN CONST AFU_BCP_SHARING_PROTOCOL *This,
	CHAR16					*VarName,
	OUT void    			**Buffer,
	OUT UINT32  			*BufferSize,
	OUT CHAR16 				**CapsuleDataName,
	OUT void    			**RomLocation,
	OUT void    			**FVLocation
	);

typedef struct _AFU_BCP_SHARING_PROTOCOL {
	AFU_BCP_GETVARIABLE		AfuBcpGetVariable;
};

CHAR16 		*BCPGetLastFwCapsuleVariable (void);
EFI_STATUS 	InstallAfuBcpSharingProtocol (void);

#pragma pack()

#endif

//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
