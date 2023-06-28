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

//*************************************************************************
/** @file AcpiModeEnable.c
    Provide functions to enable and disable ACPI mode

**/
//*************************************************************************

#include <Token.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>

#include <Protocol/SmmBase2.h>
#include <Protocol/SmmSwDispatch2.h>

#include <Protocol/DevicePath.h>
#include "AcpiModeEnable.h"
#include <Library/ElinkLib.h>
#include <SbElinks.h>
#include <AmiChipsetIoLib.h>
#define AMI_SMM_SW_DISPATCH_PROTOCOL EFI_SMM_SW_DISPATCH2_PROTOCOL
#define AMI_SMM_SW_DISPATCH_CONTEXT  EFI_SMM_SW_REGISTER_CONTEXT
#define SMM_CHILD_DISPATCH_SUCCESS   EFI_SUCCESS

//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
// Variable Declaration(s)

ACPI_DISPATCH_LINK  *gAcpiEnDispatchHead = 0, *gAcpiEnDispatchTail = 0;
ACPI_DISPATCH_LINK  *gAcpiDisDispatchHead = 0, *gAcpiDisDispatchTail = 0;

UINT16          wPM1_SaveState;
UINT32          dGPE_SaveState;
UINT32          SaveSmiTimerState = 0;
UINT32          SaveSmiControl8 = 0;

typedef VOID (ACPI_MODE_CALLBACK) (
    IN EFI_HANDLE                   DispatchHandle,
    IN AMI_SMM_SW_DISPATCH_CONTEXT  *DispatchContext
);

extern ACPI_MODE_CALLBACK ACPI_ENABLE_CALL_BACK_LIST EndOfList;
ACPI_MODE_CALLBACK* AcpiEnableCallbackList[] = { ACPI_ENABLE_CALL_BACK_LIST NULL };
extern ACPI_MODE_CALLBACK ACPI_DISABLE_CALL_BACK_LIST EndOfList;
ACPI_MODE_CALLBACK* AcpiDisableCallbackList[] = { ACPI_DISABLE_CALL_BACK_LIST NULL };

/**
    This function determines if the system is resuming from an S3
    sleep state.

    @param VOID


    @retval TRUE It is an S3 Resume
    @retval FALSE It is not an S3 Resume

**/
BOOLEAN IsS3(VOID)
{
    // Check WAK_STS bit
    if (READ_IO16_PM(ACPI_IOREG_PM1_STS) & 0x8000) {
        // Check the sleep type
        if ((READ_IO16_PM(ACPI_IOREG_PM1_CNTL) & 0x1C00) == 0xC00)
            return TRUE;
    }
    return FALSE;
}

/**
    Create and add link to specified list.

          
    @param Size 
    @param Head 
    @param Tail 

        VOID* 
    @retval Link Return Value is a Pointer.

    @note  Here is the control flow of this function:
              1. Allocate Link in Smm Pool.
              2. Add Link to end.
              3. Return Link address.

**/

VOID * AddLink(
    IN UINT32       Size,
    IN VOID         **Head,
    IN VOID         **Tail)
{
    VOID *Link;

    if (pSmst->SmmAllocatePool(EfiRuntimeServicesData, Size, &Link) != EFI_SUCCESS) return 0;

    ((GENERIC_LINK*)Link)->Link = 0;
    if (!*Head)
    {
        *Head = *Tail = Link;
    }
    else
    {
        ((GENERIC_LINK*)*Tail)->Link = Link;
        *Tail = Link;
    }

    return Link;
}

/**
    Remove link from specified list.

          
    @param Handle EFI Handle
    @param Head 
    @param Tail 

        BOOLEAN
    @retval TRUE if link was removed. FALSE if link not in the list.
        FALSE

    @note  Here is the control flow of this function:
              1. Search link list for Link.
              2. Remove link from list.
              3. Free link.

**/

BOOLEAN RemoveLink(
    IN EFI_HANDLE   Handle,
    IN VOID         **Head,
    IN VOID         **Tail)
{
    GENERIC_LINK *PrevLink, *Link;

    PrevLink = *Head;

    // Is link first. Link address is the same as the Handle.
    if (((GENERIC_LINK*)*Head) == Handle)
    {
        if (PrevLink == *Tail) *Tail = 0; // If Tail = Head, then 0.
        *Head = PrevLink->Link;
        pSmst->SmmFreePool(PrevLink);
        return TRUE;
    }

    // Find Link.
    for (Link = PrevLink->Link; Link; PrevLink = Link, Link = Link->Link)
    {
        if (Link == Handle)     // Link address is the same as the Handle.
        {
            if (Link == *Tail) *Tail = PrevLink;
            PrevLink->Link = Link->Link;
            pSmst->SmmFreePool(Link);
            return TRUE;
        }
    }

    return FALSE;
}

/**
    Register a Link on ACPI enable SMI.

          
    @param This 
    @param Function 
    @param Handle 


              
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

    @note  Here is the control flow of this function:
              1. Verify if Context if valid. If invalid,
                 return EFI_INVALID_PARAMETER.
              2. Allocate structure and add to link list.
              3. Fill link.
              4. Enable Smi Source.

**/

EFI_STATUS EfiAcpiEnRegister(
    IN EFI_ACPI_DISPATCH_PROTOCOL   *This,
    IN EFI_ACPI_DISPATCH            Function,
    OUT EFI_HANDLE                  *Handle)
{
    ACPI_DISPATCH_LINK *NewLink;

    NewLink = AddLink(sizeof(ACPI_DISPATCH_LINK), \
                      &gAcpiEnDispatchHead, \
                      &gAcpiEnDispatchTail);
    if (!NewLink) return EFI_OUT_OF_RESOURCES;

    NewLink->Function   = Function;
    *Handle = NewLink;

    return EFI_SUCCESS;
}

/**
    Unregister a Link on ACPI enable SMI.

         
    @param This 
    @param Handle 

              
    @retval EFI_SUCCESS Driver loading successfully
    @retval EFI_ERROR Driver loading failed

    @note  Here is the control flow of this function:
              1. Remove link. If no link, return EFI_INVALID_PARAMETER.
              2. Disable SMI Source if no other handlers using source.
              3. Return EFI_SUCCESS.

**/

EFI_STATUS EfiAcpiEnUnregister(
    IN EFI_ACPI_DISPATCH_PROTOCOL   *This,
    IN EFI_HANDLE                   Handle)
{
    if (!RemoveLink(Handle, &gAcpiEnDispatchHead, &gAcpiEnDispatchTail))
        return EFI_INVALID_PARAMETER;
    return EFI_SUCCESS;
}

/**
    Register a Link on ACPI disable SMI.

         
    @param This 
    @param Function 
    @param Handle 

             
    @retval EFI_SUCCESS Driver loading successfully
    @retval EFI_ERROR Driver loading failed

    @note  Here is the control flow of this function:
              1. Verify if Context if valid. If invalid,
                 return EFI_INVALID_PARAMETER.
              2. Allocate structure and add to link list.
              3. Fill link.
              4. Enable Smi Source.

**/

EFI_STATUS EfiAcpiDisRegister(
    IN EFI_ACPI_DISPATCH_PROTOCOL   *This,
    IN EFI_ACPI_DISPATCH            Function,
    OUT EFI_HANDLE                  *Handle)
{
    ACPI_DISPATCH_LINK *NewLink;

    NewLink = AddLink(sizeof(ACPI_DISPATCH_LINK), \
                      &gAcpiDisDispatchHead, \
                      &gAcpiDisDispatchTail);
    if (!NewLink) return EFI_OUT_OF_RESOURCES;

    NewLink->Function   = Function;
    *Handle = NewLink;

    return EFI_SUCCESS;
}

/**
    Unregister a Link on ACPI Disable SMI.

          
    @param This 
    @param Handle EFI Handle

              
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

    @note  Here is the control flow of this function:
              1. Remove link. If no link, return EFI_INVALID_PARAMETER.
              2. Disable SMI Source if no other handlers using source.
              3. Return EFI_SUCCESS.

**/

EFI_STATUS EfiAcpiDisUnregister(
    IN EFI_ACPI_DISPATCH_PROTOCOL   *This,
    IN EFI_HANDLE                   Handle)
{
    if (!RemoveLink(Handle, &gAcpiDisDispatchHead, &gAcpiDisDispatchTail))
        return EFI_INVALID_PARAMETER;
    return EFI_SUCCESS;
}

EFI_ACPI_DISPATCH_PROTOCOL gEfiAcpiEnDispatchProtocol = \
        {EfiAcpiEnRegister, EfiAcpiEnUnregister};

EFI_ACPI_DISPATCH_PROTOCOL gEfiAcpiDisDispatchProtocol = \
        {EfiAcpiDisRegister, EfiAcpiDisUnregister};


/**
    This function calls registered ELINKs for EnableAcpiModeElink.

    @param DispatchHandle SMI dispatcher handle
    @param DispatchContext Points to an optional S/W SMI context

    @retval VOID
**/

VOID EnableAcpiModeCallbacks (
    IN EFI_HANDLE                   DispatchHandle,
    IN AMI_SMM_SW_DISPATCH_CONTEXT  *DispatchContext )
{
  UINTN                   ElinkPtr;
  AMI_HOOK_LINK           *AmiHookLink;
  UINT32                  Index;
  ACPI_MODE_CALLBACK      *Elink;

  ElinkPtr = ElinkGet (PcdToken(PcdEnableAcpiModeElink));

  if (ElinkPtr == 0) {
    return;
  }

  AmiHookLink = (AMI_HOOK_LINK *) ElinkPtr;

  for (Index = 0; Index < ELINK_ARRAY_NUM; Index++) {
    if (AmiHookLink->ElinkArray[Index] == NULL) {
      break;
    }
    Elink = AmiHookLink->ElinkArray[Index];
    Elink(DispatchHandle, DispatchContext);
  }
}

/**
    This function enable ACPI mode by clearing all SMI and
    enabling SCI generation
    This routine is also called on a S3 resume for special ACPI
    programming.
    Status should not be cleared on S3 resume. Enables are
    already taken care of.

               
    @param DispatchHandle SMI dispatcher handle
    @param DispatchContext OPTIONAL - Points to an optional S/W SMI context
    @param CommBuffer OPTIONAL      - Points to the optional communication
        buffer
    @param CommBufferSize OPTIONAL  - Points to the size of the optional
        communication buffer

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/

EFI_STATUS EnableAcpiMode (
	IN EFI_HANDLE       DispatchHandle,
	IN CONST VOID       *DispatchContext OPTIONAL,
	IN OUT VOID         *CommBuffer OPTIONAL,
	IN OUT UINTN        *CommBufferSize OPTIONAL )
{
    ACPI_DISPATCH_LINK      *Link;
    UINTN					Index;
    if (!IsS3())
    {
        // Enalbe SCI in SB for PIC
        WRITE_IO8_PIRQ(0x10, ACPI_SCI_INT);
        // Enalbe SCI in SB for APIC
        WRITE_IO8_PIRQ(0x90, ACPI_SCI_INT);
        //###WRITE_IO8_PIRQ(4, ACPI_SCI_INT);

        // Set ACPI IRQ Level Trigger
        if (ACPI_SCI_INT > 7)
            SET_IO8(LEGACY_8259_EDGE_LEVEL_TRIGGERED_REGISTER_SLAVE, (UINT8) (1 << (ACPI_SCI_INT-8)) ); //P20100610C
        else
            SET_IO8(LEGACY_8259_EDGE_LEVEL_TRIGGERED_REGISTER_MASTER, (UINT8)(1 << ACPI_SCI_INT) ); //P20100610C
        //Disable SMI Sources

        //Disable and Clear PM1 Sources except power button
        wPM1_SaveState = READ_IO16_PM(ACPI_IOREG_PM1_EN); // 0x02
        WRITE_IO16_PM(ACPI_IOREG_PM1_STS, 0xFFFF); // 0x00
        WRITE_IO16_PM(ACPI_IOREG_PM1_EN, 0x100); // 0x02

        //Disable and Clear GPE0 Sources
        dGPE_SaveState = READ_IO32_PM(ACPI_IOREG_GPE0_EN); // 0x24
        WRITE_IO32_PM(ACPI_IOREG_GPE0_STS, 0xFFFFFFFF); // 0x20
        WRITE_IO32_PM(ACPI_IOREG_GPE0_EN, 0); // 0x24

        //Set day of month alarm invalid - ACPI 1.0 section 4.7.2.4
        WRITE_IO8(RTC_IO_INDEX, 0x8D);
        WRITE_IO8(RTC_IO_DATA, 0);
    }

    // Reference : SB800 CIMx , SBMAIN.C sbSmmAcpiOn()
    // Disable Power Button SMI
// (P20100422A.1)>
//####    RWSbSmiIo8(0xB2, (BIT04 + BIT05), 0);
    RWSbSmiIo8(0xAC, (BIT06 + BIT07), 0);
// <(P20100422A.1)

    //Enable Sci
    SET_IO16_PM(ACPI_IOREG_PM1_CNTL, 1); // 0x04

    //Disable SMI Timer
    // Select ShortTimer for register access
    RESET_MEM32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98, BIT29);
    SaveSmiTimerState = READ_MEM32(ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG96) & BIT15;
    RESET_MEM32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG96, BIT15);

    //Clear SmiControl8 Bit[31:30] Control for Long timer and Bit[29:28] for Control for Short timer 
    //SaveSmiControl8 = READ_MEM32(ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGC0) & (BIT28 | BIT29 | BIT30 | BIT31);
    //RESET_MEM32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGC0, (BIT28 | BIT29 | BIT30 | BIT31));
    // ONLY for ShortTimer
    SaveSmiControl8 = READ_MEM32(ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGC0) & (BIT28 | BIT29 );
    RESET_MEM32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGC0, (BIT28 | BIT29 ));

    
    for (Link = gAcpiEnDispatchHead; Link; Link = Link->Link)
    {
        Link->Function(Link);
    }
    for (Index = 0; AcpiEnableCallbackList[Index] != NULL; Index++)
    {
    	AcpiEnableCallbackList[Index](DispatchHandle, DispatchContext);
    }

    EnableAcpiModeCallbacks (DispatchHandle, DispatchContext);

    IoWrite8(0x80, SW_SMI_ACPI_ENABLE);

    return SMM_CHILD_DISPATCH_SUCCESS;
}

/**
    This function calls registered ELINKs for DisableAcpiModeElink.

    @param DispatchHandle SMI dispatcher handle
    @param DispatchContext Points to an optional S/W SMI context

    @retval VOID
**/

VOID DisableAcpiModeCallbacks (
    IN EFI_HANDLE                   DispatchHandle,
    IN AMI_SMM_SW_DISPATCH_CONTEXT  *DispatchContext )
{
  UINTN                   ElinkPtr;
  AMI_HOOK_LINK           *AmiHookLink;
  UINT32                  Index;
  ACPI_MODE_CALLBACK      *Elink;

  ElinkPtr = ElinkGet (PcdToken(PcdDisableAcpiModeElink));

  if (ElinkPtr == 0) {
    return;
  }

  AmiHookLink = (AMI_HOOK_LINK *) ElinkPtr;

  for (Index = 0; Index < ELINK_ARRAY_NUM; Index++) {
    if (AmiHookLink->ElinkArray[Index] == NULL) {
      break;
    }
    Elink = AmiHookLink->ElinkArray[Index];
    Elink(DispatchHandle, DispatchContext);
  }
}

/**
    This function disables ACPI mode by enabling SMI generation

              
    @param DispatchHandle SMI dispatcher handle
    @param DispatchContext OPTIONAL - Points to an optional S/W SMI context
    @param CommBuffer OPTIONAL      - Points to the optional communication
        buffer
    @param CommBufferSize OPTIONAL  - Points to the size of the optional
        communication buffer

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/

EFI_STATUS DisableAcpiMode (
	IN EFI_HANDLE       DispatchHandle,
	IN CONST VOID       *DispatchContext OPTIONAL,
	IN OUT VOID         *CommBuffer OPTIONAL,
	IN OUT UINTN        *CommBufferSize OPTIONAL )
{
    ACPI_DISPATCH_LINK      *Link;
    UINTN					Index;

    //Clear PM Sources and set Enables
    WRITE_IO16_PM(ACPI_IOREG_PM1_STS, 0xFFFF); // 0x00
    WRITE_IO16_PM(ACPI_IOREG_PM1_EN, wPM1_SaveState); // 0x02

    //Clear GPE0 Sources and set Enables
    WRITE_IO32_PM(ACPI_IOREG_GPE0_STS, 0xFFFFFFFF); // 0x20
    WRITE_IO32_PM(ACPI_IOREG_GPE0_EN, dGPE_SaveState); // 0x24

    //Disable SCI
    RESET_IO16_PM(ACPI_IOREG_PM1_CNTL, 1); // 0x04

    //Restore SMI Timer
    // Select ShortTimer for register access
    RESET_MEM32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98, BIT29);
    if(SaveSmiTimerState)
        SET_MEM32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG96, BIT15);
    else
        RESET_MEM32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG96, BIT15);

    //Restore SmiControl8 Bit[31:30] Control for Long timer and Bit[29:28] for Control for Short timer 
    if(SaveSmiControl8)
        SET_MEM32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGC0, SaveSmiControl8);
    else
        //RESET_MEM32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGC0, (BIT28 | BIT29 | BIT30 | BIT31));
        // ONLY for ShortTimer
        RESET_MEM32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGC0, (BIT28 | BIT29 ));

    for (Link = gAcpiDisDispatchHead; Link; Link = Link->Link)
    {
        Link->Function(Link);
    }
    for (Index = 0; AcpiDisableCallbackList[Index] != NULL; Index++)
    {
    	AcpiDisableCallbackList[Index](DispatchHandle, DispatchContext);
    }

    DisableAcpiModeCallbacks (DispatchHandle, DispatchContext);

    IoWrite8(0x80, SW_SMI_ACPI_DISABLE);

    return SMM_CHILD_DISPATCH_SUCCESS;
}

/**
    This function is part of the ACPI mode enable/disable
    driver and invoked during SMM initialization.  As the name
    suggests this function is called from SMM

    @param ImageHandle Handle for this FFS image
    @param SystemTable Pointer to the system table

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
EFI_STATUS AcpiModeEnableInSmmFunction(
    IN  EFI_HANDLE          ImageHandle, 
    IN  EFI_SYSTEM_TABLE    *SystemTable
)
{
    EFI_STATUS                   Status;
    EFI_HANDLE                   Handle = NULL;
    AMI_SMM_SW_DISPATCH_PROTOCOL *SwDispatch = NULL;
    EFI_HANDLE                   DummyHandle = NULL;
    AMI_SMM_SW_DISPATCH_CONTEXT  AcpiEnableContext = {SW_SMI_ACPI_ENABLE};
    AMI_SMM_SW_DISPATCH_CONTEXT  AcpiDisableContext = {SW_SMI_ACPI_DISABLE};

    Status = InitAmiSmmLib( ImageHandle, SystemTable );
    if (EFI_ERROR(Status)) return Status;

    Status = pSmst->SmmLocateProtocol( &gEfiSmmSwDispatch2ProtocolGuid , \
                                       NULL, \
                                       &SwDispatch );
    if (EFI_ERROR(Status)) return Status;

    Status = SwDispatch->Register( SwDispatch, \
                                   EnableAcpiMode, \
                                   &AcpiEnableContext, \
                                   &Handle );
    if (EFI_ERROR(Status)) return Status;   

    Status = pSmst->SmmInstallProtocolInterface (
                      &DummyHandle,
                      &gEfiAcpiEnDispatchProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      &gEfiAcpiEnDispatchProtocol
                      );
    if (EFI_ERROR(Status)) return Status;

    Status = SwDispatch->Register( SwDispatch, \
                                   DisableAcpiMode, \
                                   &AcpiDisableContext,\
                                   &Handle );
    if (EFI_ERROR(Status)) return Status;   

    Status = pSmst->SmmInstallProtocolInterface (
                      &DummyHandle,
                      &gEfiAcpiDisDispatchProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      &gEfiAcpiDisDispatchProtocol
                      );

    return Status;
}

/**
    This function is the entry point for the ACPI mode enable/disable
    driver.  This function is called twice: first time by the
    DXE dispatcher and the next time when it is loaded into the
    SMRAM

    @param ImageHandle Handle for this FFS image
    @param SystemTable Pointer to the system table

              
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/

EFI_STATUS 
AcpiModeEnableInit(
    IN  EFI_HANDLE              ImageHandle,
    IN  EFI_SYSTEM_TABLE        *SystemTable
)
{
    InitAmiLib(ImageHandle,SystemTable);

    return InitSmmHandler(ImageHandle, 
                          SystemTable, 
                          AcpiModeEnableInSmmFunction, 
                          NULL);
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
