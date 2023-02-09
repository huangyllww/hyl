//*************************************************************************
//*************************************************************************
//**                                                                     **
//**          (C)Copyright 1985-2019, American Megatrends, Inc.          **
//**                                                                     **
//**                         All Rights Reserved.                        **
//**                                                                     **
//**        5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                     **
//**                         Phone: (770)-246-8600                       **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file AmiCpuInfo.h
    Contains Structure definition of AMI_CPU_INFO

**/

#ifndef _AMI_CPU_INFO_H_
#define _AMI_CPU_INFO_H_

#ifdef __cplusplus
extern "C" {
#endif


//F109F361-370C-4d9c-B1AB-7CA2D4C8B3FF
#define AMI_CPU_INFO_PROTOCOL_GUID \
	{0xf109f361,0x370c,0x4d9c,0xb1,0xab,0x7c,0xa2,0xd4,0xc8,0xb3,0xff}

typedef struct _AMI_CPU_INFO_PROTOCOL AMI_CPU_INFO_PROTOCOL;

#pragma pack (1)
typedef	struct {
	UINT8	Desc;
	UINT8	Level;
	UINT8	Type;	//0 - Data, 1 - Instruction, 3 - Unified
	UINT16	Size;
	UINT8	Associativity;
} CACHE_DESCRIPTOR_INFO;


typedef struct {
	UINT32	Version;
	UINT64	Features;
	CHAR8	*BrandString;
	UINT8	NumCores;
	UINT8	NumHts;
	UINT32	FSBFreq;			//MHz
	UINT32	IntendedFreq;		//MHz
    UINT32  MaxFreq;            //MHz  (max supported frequency)
	UINT32	ActualFreq;			//MHz
	UINT32	Voltage;			// Volt / 10000.
	UINT32	MicroCodeVers;
	CACHE_DESCRIPTOR_INFO	*CacheInfo;
} AMI_CPU_INFO;
#pragma pack()

typedef EFI_STATUS (*GET_CPU_INFO) (
	IN AMI_CPU_INFO_PROTOCOL	*This,
	IN UINTN					Cpu,
	OUT AMI_CPU_INFO			**Info
);

#pragma pack (1)
struct _AMI_CPU_INFO_PROTOCOL {
	GET_CPU_INFO	GetCpuInfo;
};
#pragma pack()


/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif

#endif

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**          (C)Copyright 1985-2019, American Megatrends, Inc.          **
//**                                                                     **
//**                         All Rights Reserved.                        **
//**                                                                     **
//**        5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                     **
//**                         Phone: (770)-246-8600                       **
//**                                                                     **
//*************************************************************************
//*************************************************************************
