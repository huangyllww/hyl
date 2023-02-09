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


/** @file PspCspLib.h

**/

#ifndef __PSPCSPLIB_H__
#define __PSPCSPLIB_H__

//---------------------------------------------------------------------------


#ifdef __cplusplus
extern "C" {
#endif

typedef union _MBOX_BUFFER MBOX_BUFFER;

#define MSR_MMIO_Cfg_Base           0xC0010058ul    // MMIO Configuration Base Address Register

/// These width descriptors are used by the library function, and others, to specify the data size
typedef enum ACCESS_WIDTH {
  AccessWidthNone = 0,                                      ///< dummy access width
  AccessWidth8 = 1,                                         ///< Access width is 8 bits.
  AccessWidth16,                                            ///< Access width is 16 bits.
  AccessWidth32,                                            ///< Access width is 32 bits.
  AccessWidth64,                                            ///< Access width is 64 bits.

  AccessS3SaveWidth8 = 0x81,                                ///< Save 8 bits data.
  AccessS3SaveWidth16,                                      ///< Save 16 bits data.
  AccessS3SaveWidth32,                                      ///< Save 32 bits data.
  AccessS3SaveWidth64,                                      ///< Save 64 bits data.
} ACCESS_WIDTH;

/// Extended PCI address format
typedef struct {
  IN OUT  UINT32      Register:12;                ///< Register offset
  IN OUT  UINT32      Function:3;                 ///< Function number
  IN OUT  UINT32      Device:5;                   ///< Device number
  IN OUT  UINT32      Bus:8;                      ///< Bus number
  IN OUT  UINT32      Segment:4;                  ///< Segment
} EXT_PCI_ADDR;

/// Union type for PCI address
typedef union _PCI_ADDR {
  IN  UINT32          AddressValue;               ///< Formal address
  IN  EXT_PCI_ADDR    Address;                    ///< Extended address
} PCI_ADDR;

#define MAKE_SBDFO(Seg, Bus, Dev, Fun, Off) ((((UINT32) (Seg)) << 28) | (((UINT32) (Bus)) << 20) | \
                   (((UINT32)(Dev)) << 15) | (((UINT32)(Fun)) << 12) | ((UINT32)(Off)))
#define ILLEGAL_SBDFO 0xFFFFFFFFul

#define NB_CFG      0xC001001Ful

#define IOCF8 0xCF8
#define IOCFC 0xCFC

#define PSP_PCI_SEG        0x00    ///< PSP Seg address
#define PSP_PCI_BUS        0x00    ///< PSP Bus address
#define PSP_PCI_DEV        0x08    ///< PSP Device address
#define PSP_PCI_FN         0x00    ///< PSP Fn address
#define PSP_PCI_BDA        ((PSP_PCI_DEV << 11) + (PSP_PCI_FN << 8))
#define GET_PSP_PCI_ADDR (Offset)    MAKE_SBDFO (PSP_PCI_SEG, PSP_PCI_BUS, PSP_PCI_DEV, PSP_PCI_FN, Offset)

#define PSP_PCI_DEVID_REG           0x00    ///< DevId
#define PSP_PCI_CMD_REG             0x04    ///< CmdReg
#define PSP_PCI_BAR1_REG            0x18    ///< Pci Bar1
#define PSP_PCI_BAR3_REG            0x20    ///< Pci Bar3
#define PSP_PCI_MIRRORCTRL1_REG     0x44    ///< PSP Mirror Reg Ctrl 1
#define PSP_PCI_EXTRAPCIHDR_REG     0x48    ///< Extra PCI Header Ctr
#define PSP_PCI_HTMSICAP_REG        0x5C    ///<  HT MSI Capability

#define D8F0x44_PmNxtPtrW_MASK                                  0xff

#define PSP_MAILBOX_BASE            0x70    ///< Mailbox base offset on PCIe BAR
#define PSP_MAILBOX_STATUS_OFFSET   0x4     ///< Staus Offset

//======================================================================================
//
// Define Mailbox Status field
//
//======================================================================================
//
/// MBox Status MMIO space
///
typedef struct {
  UINT32 MboxInitialized:1;   ///< Target will set this to 1 to indicate it is initialized  (for ex. PSP/TPM ready)
  UINT32 Error:1;             ///< Target in adddtion to Done bit will also set this bit to indicate success/error on last command
  UINT32 Terminated:1;        ///< Target will set this bit if it aborted the command due to abort request
  UINT32 Halt:1;              ///< Target will set this error if there is critical error that require reset etc
  UINT32 Recovery:1;          ///< Target will set this error if some PSP entry point by PSP directory has been corrupted.
  UINT32 Reserved:27;          ///< Reserved
} MBOX_STATUS;

//
// Above defined as bitmap
#define MBOX_STATUS_INITIALIZED       0x00000001ul    ///< Mailbox Status: Initialized
#define MBOX_STATUS_ERROR             0x00000002ul    ///< Mailbox Status: Error
#define MBOX_STATUS_ABORT             0x00000004ul    ///< Mailbox Status: Abort
#define MBOX_STATUS_HALT              0x00000008ul    ///< Mailbox Status: Halt
#define MBOX_STATUS_RECOVERY          0x00000010ul    ///< Mailbox Status: Recovery required

//======================================================================================
//
// Define Mailbox Command
//
//======================================================================================
//
/// Mbox command list. Only one command can be send till target process it; the only exception is Abort command that BIOS may send in case of timeout etc.
///
typedef enum {
  MboxCmdRsvd               = 0x00,   ///< Unused

  MboxBiosCmdDramInfo       = 0x01,   ///< Bios -> PSP: Memory DRAM information (ie. PspBuffer address etc)
  MboxBiosCmdSmmInfo        = 0x02,   ///< Bios -> PSP: Bios will provide SMM inf - SmmBase, PspSmmDataRegion,PspSmmDataRegionLength, SoftSmiValue, SoftSmiPort
  MboxBiosCmdSxInfo         = 0x03,   ///< Bios -> PSP: Sx transition info (S3, S5)
  MboxBiosCmdRsmInfo        = 0x04,   ///< Bios -> PSP: Resume transition info (BiosResume Vector, Size of resume code)
  MboxBiosCmdPspQuery       = 0x05,   ///< Bios -> PSP: Bios Find supported feature
  MboxBiosCmdBootDone       = 0x06,   ///< Bios -> PSP: Bios is done with BIOS POST
  MboxBiosCmdClearS3Sts     = 0x07,   ///< Bios -> PSP: Inform PSP clear S3ExitReset
  MboxBiosS3DataInfo        = 0x08,   ///< Bios -> PSP: Bios will send this command to inform PSP to save the data needed to restore memory during resume from S3
  MboxBiosCmdNop            = 0x09,   ///< Bios -> PSP: Bios will send this NOP command to indicate to PSP that is is done servicing PSP SMM request


  MboxPspCmdSpiGetAttrib    = 0x81,   ///< PSP -> BIOS: Get location of PSP NVRam region
  MboxPspCmdSpiSetAttrib    = 0x82,   ///< PSP -> BIOS: Get location of PSP NVRam region
  MboxPspCmdSpiGetBlockSize = 0x83,   ///< PSP -> BIOS: Get Block size info
  MboxPspCmdSpiReadFV       = 0x84,   ///< PSP -> BIOS: Read PSP NVRAM firmware volume
  MboxPspCmdSpiWriteFV      = 0x85,   ///< PSP -> BIOS: Write PSP NVRAM firmware volume
  MboxPspCmdSpiEraseFV      = 0x86,   ///< PSP -> BIOS: Erase PSP NVRAM firmware volume

  MboxCmdAbort              = 0xfe,   ///< Abort the last command (BIOS to PSP in case of timeout etc)
} MBOX_COMMAND;

///
///
/// standard header structure for additional parameter
///
typedef struct {
  UINT32    TotalSize;                      ///< Total Size of MBOX_BUFFER (including this field)
  UINT32    Status;                         ///< Status value if any:e
  //UINT8   ReqBuffer[x];                   ///< X byte long Request buffer for additional parameter.
} MBOX_BUFFER_HEADER;


//======================================================================================
//
// Below define Request buffer for various commands. This structure is based on Command
//
//======================================================================================
///
/// structure of ReqBuffer for MboxBiosS3DataInfo mailbox command
///
typedef struct {
  UINT64 S3RestoreBufferBase;               ///< PSP reserve memory near TOM
  UINT64 S3RestoreBufferSize;               ///< Size of PSP memory
} S3DATA_REQ_BUFFER;

/// MBOX buffer for S3Info data to bring memory out of self refresh info
typedef struct {
  MBOX_BUFFER_HEADER  Header;               ///< Header
  S3DATA_REQ_BUFFER   Req;                  ///< Req
} MBOX_S3DATA_BUFFER;

#define SMM_TRIGGER_IO                      0
#define SMM_TRIGGER_MEM                     1

#define SMM_TRIGGER_BYTE                    0
#define SMM_TRIGGER_WORD                    1
#define SMM_TRIGGER_DWORD                   2

/// Define structure of SMM_TRIGGER_INFO
typedef struct {
  UINT64  Address;                          ///< Memory or IO address (Memory will be qword, IO will be word)
  UINT32  AddressType;                      ///< SMM trigger typr - Perform write to IO/Memory
  UINT32  ValueWidth;                       ///< Width of value to write (byte write, word write,..)
  UINT32  ValueAndMask;                     ///< AND mask of value after reading from the address
  UINT32  ValueOrMask;                      ///< OR Mask of value to write to this address.
} SMM_TRIGGER_INFO;

///
/// structure of ReqBuffer for MboxBiosCmdSmmInfo mailbox command
///
typedef struct {
  UINT64            SMMBase;                ///< SMM TSeg Base
  UINT64            SMMLength;              ///< Length of SMM area
  UINT64            PSPSmmDataRegion;       ///< PSP region base in Smm space
  UINT64            PspSmmDataLength;       ///< Psp region length in smm space
  SMM_TRIGGER_INFO  SmmTrigInfo;            ///< Information to generate SMM
} SMM_REQ_BUFFER;

/// MBOX buffer for SMM info
typedef struct {
  MBOX_BUFFER_HEADER  Header;               ///< Header
  SMM_REQ_BUFFER      Req;                  ///< Reques buffer
} MBOX_SMM_BUFFER;


///
/// structure of ReqBuffer for MboxBiosCmdSxInfo mailbox command
///
typedef struct {
  UINT8  SleepType;                         ///< Inform which sleep state the system is going to
} SX_REQ_BUFFER;

/// MBOX buffer for Sx info
typedef struct {
  MBOX_BUFFER_HEADER  Header;               ///< Header
  SX_REQ_BUFFER       Req;                  ///< Request buffer
} MBOX_SX_BUFFER;


///
/// structure of ReqBuffer for MboxBiosCmdRsmInfo mailbox command
///
typedef struct {
  UINT64  ResumeVecorAddress;               ///< Address of BIOS resume vector
  UINT64  ResumeVecorLength;                ///< Length of BIOS resume vector
} RSM_REQ_BUFFER;

/// MBOX buffer for RSM info
typedef struct {
  MBOX_BUFFER_HEADER  Header;               ///< Header
  RSM_REQ_BUFFER      Req;                  ///< Req
} MBOX_RSM_BUFFER;

/// CAPS_REQ_BUFFER structure
typedef struct {
  UINT32 Capabilities;                      ///< PSP Writes capabilities into this field when it returns.
} CAPS_REQ_BUFFER;

// Bitmap defining capabilities
#define PSP_CAP_TPM (1 << 0)

/// MBOX buffer for Capabilities Query
typedef struct {
  MBOX_BUFFER_HEADER  Header;               ///< Header
  CAPS_REQ_BUFFER     Req;                  ///< Req
} MBOX_CAPS_BUFFER;

/// MBOX buffer for Exit BIOS info
typedef struct {
  MBOX_BUFFER_HEADER  Header;               ///< Header
} MBOX_DEFAULT_BUFFER;

//
// Define Malbox buffer comming from PSP->BIOS
//

///
/// structure of ReqBuffer for MboxPspCmdSpiGetAddress/MboxPspCmdSpiGetAddress  mailbox command
///
typedef struct {
  UINT64    Attribute;                      ///< Inform attribute of SPI part
} SPI_ATTRIB_REQ;

/// MBOX buffer for Spi Get/Set attribute info
typedef struct {
  MBOX_BUFFER_HEADER      Header;           ///< Header
  SPI_ATTRIB_REQ          Req;              ///< Req
} MBOX_SPI_ATTRIB_BUFFER;


///
/// structure of ReqBuffer for MboxPspCmdSpiGetBlockSize mailbox command
///
typedef struct {
  UINT64  Lba;                              ///< starting LBA
  UINT64  BlockSize;                        ///< Block size of each Lba
  UINT64  NumberOfBlocks;                   ///< Total number of blocks
} SPI_INFO_REQ;

/// MBOX buffer for Spi read block attribute
typedef struct {
  MBOX_BUFFER_HEADER    Header;             ///< Header
  SPI_INFO_REQ          Req;                ///< Req
} MBOX_SPI_INFO_BUFFER;


///
/// structure of ReqBuffer for MboxPspCmdSpiRead/Write mailbox command
///
typedef struct {
  UINT64  Lba;                              ///< starting LBA
  UINT64  Offset;                           ///< Offset in LBA
  UINT64  NumByte;                          ///< Total byte to read
  UINT8   Buffer[1];                        ///< Buffer to read the data
} SPI_RW_REQ;

/// MBOX buffer for Spi read block attribute
typedef struct {
  MBOX_BUFFER_HEADER    Header;             ///< Header
  SPI_RW_REQ            Req;                ///< Req
} MBOX_SPI_RW_BUFFER;


///
/// structure of ReqBuffer for MboxPspCmdSpiErase mailbox command
///
typedef struct {
  UINT64  Lba;                              ///< starting LBA
  UINT64  NumberOfBlocks;                   ///< Total number of blocks
} SPI_ERASE_REQ;

/// MBOX buffer for Spi read block attribute
typedef struct {
  MBOX_BUFFER_HEADER    Header;             ///< Header
  SPI_ERASE_REQ         Req;                ///< Req
} MBOX_SPI_ERASE_BUFFER;

/// Union of various structure
typedef union _MBOX_BUFFER {
  MBOX_DEFAULT_BUFFER       Dflt;           ///< Default

  MBOX_SMM_BUFFER           Smm;            ///< Smm
  MBOX_SX_BUFFER            Sx;             ///< Sx
  MBOX_RSM_BUFFER           Rsm;            ///< Rsm
  MBOX_CAPS_BUFFER          Cap;            ///< Cap

  MBOX_SPI_ATTRIB_BUFFER    SpiAttrib;      ///< SpiAttrib
  MBOX_SPI_INFO_BUFFER      SpiInfo;        ///< SpiInfo
  MBOX_SPI_RW_BUFFER        SpiRw;          ///< SpiRw
  MBOX_SPI_ERASE_BUFFER     SpiErase;       ///< SpiErase
  UINT8                     Rsvd[32];       ///< To Keep the minimal size 32 bytes
} MBOX_BUFFER;

///
/// X86 to PSP Buffer
/// Each MMIO Block will have Command, Status and Buffer pointer entries.
/// The 8 dword wide MMIO mailbox will be part of PSP-CPU MMIO space
///
typedef struct {
  volatile MBOX_COMMAND     MboxCmd;        ///< Mbox Command 32 bit wide
  volatile MBOX_STATUS      MboxSts;        ///< Mbox status  32 bit wide
  MBOX_BUFFER               *Buffer;        ///< 64 bit Ponter to memory with additional parameter.
} PSP_MBOX;

/// 2x Sized Mailbox Buffer for alignment
typedef struct {
  MBOX_BUFFER   Chunk[2];                   ///< Add additional one for alignment
} UNALIGNED_MBOX_BUFFER;

///
/// Buffer is 2X size of aligned structure of size at least 32 bytes long
/// To align in go to next offset past the buffer size & from there back
/// to 32 byte aligned address
#define BALIGN32(p)  ((VOID *) (((UINTN)(VOID*)(p) + 32) & ~0x1F))

BOOLEAN AgesaIsPspEn(VOID);

VOID Psp_S3WarmResetCallback(VOID);

#ifdef __cplusplus
}
#endif
#endif


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
