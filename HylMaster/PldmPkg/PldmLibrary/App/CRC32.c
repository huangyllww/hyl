#ifdef _EFI
#include "os_hook.h"
#endif
#define _EFI 1

#include "CRC32.h"
#if defined(_LNX) || defined(_BSD)
#include "os_hook.h"
#endif

static BOOLEAN gTableInitialized = FALSE;
static ULONG Table[0x100];

static ULONG Reflect(ULONG ref, char ch)
{
    ULONG value = 0;
    int i;
    // Swap bit 0 for bit 7

    // bit 1 for bit 6, etc.

    for (i = 1; i < (ch + 1); i++)
    {
        if (ref & 1)
            value |= 1 << (ch - i);
        ref >>= 1;
    }
    return value;
}

static void InitializeCRC32Table()
{
    // This is the official polynomial used by CRC-32

    // in PKZip, WinZip and Ethernet.

    ULONG ulPolynomial = 0x04C11DB7;
    int i;
    int j;

    // 256 values representing ASCII character codes.

    for(i = 0; i <= 0xFF; i++)
    {
        Table[i] = Reflect(i, 8) << 24;
        for (j = 0; j < 8; j++)
            Table[i] = (Table[i] << 1) ^ (Table[i] & (1 << 31) ? ulPolynomial : 0);
        Table[i] = Reflect(Table[i],  32);
    }
}

void CalculateCRC_32(const LPBYTE Buffer, UINT Size, UINT32 *CRC)
{   // calculate the CRC

    LPBYTE pbyte = Buffer;
    UINT32 crc = 0xFFFFFFFF;
    UINT size = Size;

    if(!gTableInitialized)
    {
      InitializeCRC32Table();
      gTableInitialized = TRUE;
    }

    while(size--)
        crc = (crc >> 8) ^ Table[(crc & 0xFF) ^ *pbyte++];
    *CRC = crc ^ 0xFFFFFFFF;
}
