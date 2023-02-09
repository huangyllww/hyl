#ifndef _TSE_SETUP_TRANSFER_H_
#define _TSE_SETUP_TRANSFER_H_


//EFI_GUID gSetupLoadDefaultGuid = { 0x5e3980cb, 0x3983, 0x45d1, { 0xb1, 0xf3, 0x62, 0x18, 0x90, 0xf, 0xea, 0x12 } };

#define TSE_TRANSFER_GUID \
    { 0x129b9b2f, 0x6646, 0x441e, 0x92, 0x65, 0x99, 0xe8, 0xfd, 0xe7, 0xac, 0x1c }

//EFI_GUID gTseTransferGuid = { 0x129b9b2f, 0x6646, 0x441e, { 0x92, 0x65, 0x99, 0xe8, 0xfd, 0xe7, 0xac, 0x1c } };


#pragma pack(push)
#pragma	pack(1)

typedef struct _Tse_Setup_transfer	Tse_Setup_transfer;

typedef EFI_STATUS (*EFI_DXE_VarGetBuffer)(
	CHAR16	Name[40],EFI_GUID Guid,	UINT8** buf,UINTN *size);

struct _Tse_Setup_transfer
{
	EFI_DXE_VarGetBuffer	VarGetBuffer;
};

#pragma pack(pop)

#endif
