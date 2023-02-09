//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file sha1.c
    Sha1 functions

**/

#include <Efi.h>
#include <AmiLib.h>
#include "Sha1.h"

PEIM_SHA PeimSha ={
        {0x55555555,0x55555555,0x55555555, 0x55555555,0x55555555},
        0x31313131,
        0x68686868,                     // "hhhh"
        0x32323232
};



static CONST SHA1_DIGEST mSha1DigestInit = {
    {
        0x01, 0x23, 0x45, 0x67,
        0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98,
        0x76, 0x54, 0x32, 0x10,
        0xf0, 0xe1, 0xd2, 0xc3
    }
};

extern  PEIM_SHA PeimSha;

extern
VOID
__stdcall SHA1_transform (
    IN OUT UINT8 *Digest,
    IN UINT8     *M );
 
VOID
ShaWrap(
    VOID
);


VOID
ShaWrap(
    VOID
)
{
	UINTN		BufferSize;
	UINT8		*BufferStart;
	SHA1_CTX 	Sha1Ctx;
	SHA1_DIGEST	Digest;
	SHA1_DIGEST	*pDigest = &Digest;
	UINT32      Index;
	

    BufferStart = (UINT8*)(PeimSha.Start);
    BufferSize = PeimSha.Length;
    
	SHA1_init( &Sha1Ctx);
	
	SHA1_update( &Sha1Ctx, BufferStart, BufferSize);
	
	SHA1_final(&Sha1Ctx, &pDigest);
	
	for(Index=0; Index < 5; Index++){
	    if(PeimSha.Hash[Index] != ((UINT32*)pDigest)[Index]){
	        break;
	    }
	}
	
	if(Index != 5) {
	    // Hash is different. Stop.
	    _asm{
	        mov ax, 0xEEEE
	        out 0x80, ax
	    }
	    while(1);
	}
  
}    

UINT32
__stdcall Bswap32(
    IN UINT32 Val )
{
    _asm{
        mov     eax, Val
        bswap   eax
    }
}



EFI_STATUS
__stdcall SHA1_init(
    IN SHA1_CTX *Sha1Ctx )
{
    Sha1Ctx->Status = EFI_SUCCESS;
    Sha1Ctx->Length = 0;
    Sha1Ctx->Digest = mSha1DigestInit;
    Sha1Ctx->Count  = 0;
    return EFI_SUCCESS;
}

EFI_STATUS
__stdcall SHA1_update(
    IN SHA1_CTX *Sha1Ctx,
    IN VOID     *Data,
    IN UINTN    DataLen )
{
    UINTN l;
    UINT8 *D;

    Sha1Ctx->Length += DataLen;
    D = (UINT8*)Data;

    if ( Sha1Ctx->Count > 0 )
    {
        l = 64 - Sha1Ctx->Count;

        if ( l <= DataLen )
        {
            MemCpy( Sha1Ctx->M + Sha1Ctx->Count, D, l );
            SHA1_transform( Sha1Ctx->Digest.digest, Sha1Ctx->M );
            D       += l;
            DataLen -= l;
        }
        else {
            MemCpy(Sha1Ctx->M + Sha1Ctx->Count, D, DataLen );
            Sha1Ctx->Count += DataLen;
            return EFI_SUCCESS;
        }
    }

    Sha1Ctx->Count = DataLen & 63;

    if ( Sha1Ctx->Count > 0 )
    {
        DataLen -= Sha1Ctx->Count;
            MemCpy( Sha1Ctx->M, D + DataLen, Sha1Ctx->Count );
    }

    if ((DataLen >>= 6) == 0 )
    {
        return EFI_SUCCESS;
    }

    do
    {
        SHA1_transform( Sha1Ctx->Digest.digest, D );
        D += 64;
    } while ( --DataLen > 0 );
    return EFI_SUCCESS;
}

EFI_STATUS
__stdcall SHA1_final(
    IN SHA1_CTX    *Sha1Ctx,
    OUT SHA1_DIGEST **HashVal
    )
{
    UINTN PadLen, i;

    union
    {
        UINT32 Dwords[2];
        UINT64 Qword;
    }                                 LenInBits;
    UINT8 PadByte;

    if ( Sha1Ctx->Status == EFI_ALREADY_STARTED )
    {
        *HashVal = &Sha1Ctx->Digest;
        return EFI_SUCCESS;
    }

    if ( EFI_ERROR( Sha1Ctx->Status ))
    {
        return Sha1Ctx->Status;
    }

    PadLen  = sizeof (Sha1Ctx->M) - sizeof (Sha1Ctx->Length);
    PadLen -= Sha1Ctx->Count;

    if ((INTN)PadLen <= 0 )
    {
        PadLen += sizeof (Sha1Ctx->M);
    }

    LenInBits.Qword     = Shl64( Sha1Ctx->Length, 3 );
    LenInBits.Dwords[0] = Bswap32( LenInBits.Dwords[0] );
    LenInBits.Dwords[1] = Bswap32( LenInBits.Dwords[1] );

    PadByte = 0x80;
   
    SHA1_update( Sha1Ctx, &PadByte, sizeof (PadByte));
    PadByte = 0;

    for ( PadLen--; PadLen > 0; PadLen-- )
    {
        SHA1_update( Sha1Ctx, &PadByte, sizeof (PadByte));
    }

    SHA1_update( Sha1Ctx, &LenInBits.Dwords[1], sizeof (*LenInBits.Dwords));

    SHA1_update(Sha1Ctx, &LenInBits.Dwords[0], sizeof (*LenInBits.Dwords));

    i = sizeof (Sha1Ctx->Digest) / sizeof (UINT32);

    while ( i > 0 )
    {
        i--;
        ((UINT32*)Sha1Ctx->Digest.digest)[i] = 
        Bswap32(((UINT32*)Sha1Ctx->Digest.digest)[i]);
    }

    Sha1Ctx->Status = EFI_ALREADY_STARTED;
    Sha1Ctx->Length = 0;
    i               = sizeof (Sha1Ctx->M);

    while ( i > 0 )
    {
        Sha1Ctx->M[--i] = 0;
    }

    return SHA1_final( Sha1Ctx, HashVal );
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
