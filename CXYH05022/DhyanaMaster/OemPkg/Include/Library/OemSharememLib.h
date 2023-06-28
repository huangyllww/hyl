/** @file
  Base Sharemem library header file.

  Copyright (C) 2017 Dawning Information Industry Co.,LTD.All rights reserved.<BR>
  
**/
#ifndef __OEM_SHAREMEM_LIB_H__
#define __OEM_SHAREMEM_LIB_H__

/**
 * This routine is to write specific shared memory region 
 *  
 * @author AY (2017/12/07)
 * 
 * @param[in] AddrOff   Offset address to BMC share mem base 
 *       address
 * @param[in] DataBuffer Pointer to buffer holding input data
 * @param[in] Length     Length
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
EFIAPI
ShareMemRead (
  VOID *AddrOff,
  VOID *DataBuffer,
  UINT32 Length
  );

/**
 * This routine is to write specific shared memory region 
 *  
 * @author AY (2017/12/07)
 * 
 * @param[in] AddrOff   Offset address to BMC share mem base 
 *       address
 * @param[in] DataBuffer Pointer to buffer holding input data
 * @param[in] Length     Length
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
EFIAPI
ShareMemWrite (
  VOID *AddrOff,
  VOID *DataBuffer,
  UINT32 Length
  );

/**
 * This routine is to set specific shared memory region 
 *  
 * @author AY (2017/12/07)
 * 
 * @param[in] AddrOff   Offset address to BMC share mem base 
 *       address
 * @param[in] Length    Data size to set
 * @param[in] Value     Initialization value
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
EFIAPI
ShareMemSet (
  VOID *AddrOff,
  UINT32 Length,
  UINT8  Value
  );

/**
 * This is routine for reading Mailbox register through LPC
 * 
 * @author AY (2017/12/26)
 * 
 * @param[in] RegOff    Register offset, 0-based
 * @param[in/out] Value  Value returned
 * 
 * @return EFI_STATUS
 */
EFI_STATUS
EFIAPI
MailboxRead (
  UINT16 RegOff,
  UINT8 *Value
  );

/**
 * This is the routine for writing specific mailbox register 
 * through LPC 
 * 
 * @author AY (2017/12/26)
 * 
 * @param[in] RegOff  Register offset
 * @param[in] Value   Value to write
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
EFIAPI
MailboxWrite (
  UINT16 RegOff,
  UINT8  Value
  );
#endif
