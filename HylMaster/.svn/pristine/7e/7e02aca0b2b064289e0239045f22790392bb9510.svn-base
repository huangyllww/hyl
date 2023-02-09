;*************************************************************************
;*************************************************************************
;**                                                                     **
;**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
;**                                                                     **
;**                       All Rights Reserved.                          **
;**                                                                     **
;**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
;**                                                                     **
;**                       Phone: (770)-246-8600                         **
;**                                                                     **
;*************************************************************************
;*************************************************************************

;<AMI_FHDR_START>
;----------------------------------------------------------------------------
; Name:         Startup32.asm
;
; Description:  Switch CPU to protected mode, initialize cache-as-RAM and
;               setup stack for PEI execution
;
;----------------------------------------------------------------------------
;<AMI_FHDR_END>

;----------------------------------------------------------------------------
; Assembler build options
;----------------------------------------------------------------------------
.686P
.XMM
.MODEL  SMALL, C
;----------------------------------------------------------------------------

;----------------------------------------------------------------------------
; Include files
;----------------------------------------------------------------------------
        INCLUDE token.equ
        INCLUDE cpu.equ
        INCLUDE mbiosmac.mac
        INCLUDE AgesaPkg\Include\AmdUefiStack.inc
;----------------------------------------------------------------------------

;----------------------------------------------------------------------------
; Constant defintions
;----------------------------------------------------------------------------


IFNDEF MKF_PI_SPECIFICATION_VERSION
MKF_PI_SPECIFICATION_VERSION EQU 0
ENDIF

;;##
;;## Two individual module need to reserve IDT.
;;##   Debug RX component/Serial Debugger component.
;;##
SEC_RESERVE_IDT_FOR_RX_SERIAL = 0
IFDEF MKF_AMIDEBUG_RX_SUPPORT
IF MKF_AMIDEBUG_RX_SUPPORT
SEC_RESERVE_IDT_FOR_RX_SERIAL = 1
ENDIF
ENDIF

IFDEF MKF_SERIAL_DEBUGGER_SUPPORT
IF MKF_SERIAL_DEBUGGER_SUPPORT
SEC_RESERVE_IDT_FOR_RX_SERIAL = 1
ENDIF
ENDIF

FV_BB_IN_DRAM = 1
IFDEF MKF_FV_BB_IN_ROM
IF MKF_FV_BB_IN_ROM
FV_BB_IN_DRAM = 0
ENDIF
ENDIF

;----------------------------------------------------------------------------
; Externs
;----------------------------------------------------------------------------
EXTERN  SECCoreAtPowerOn:NEAR32
PUBLIC  SECCoreAtPowerOnEnd
PUBLIC  SECCoreAPinitEnd
EXTERN  SECCoreAPinit:NEAR32
EXTERN  BeforeSECEntry:NEAR32
PUBLIC  BeforeSECEntryEnd
EXTERN  BeforeSEC_S3Entry:NEAR32
PUBLIC  BeforeSEC_S3EntryEnd

SecCEntry   PROTO   C

;----------------------------------------------------------------------------

;STARTUP_SEG SEGMENT USE32
;----------------------------------------------------------------------------
;       STARTUP_SEG  S E G M E N T  STARTS
;----------------------------------------------------------------------------
STARTUP_SEG     SEGMENT PARA PUBLIC 'CODE' USE32

_ModuleEntryPoint PROC PUBLIC
    ;; <AAV> this is dummy for compiler
    ;; This function provides dummy function so that SecCore can pass pass build
    ;; Validation in Ia32FamilyCpuPkg. All real platform library instances needs
    ;; to implement the real entry point in assembly.
_ModuleEntryPoint ENDP

;<AMI_PHDR_START>
;----------------------------------------------------------------------------
; Procedure:    BSP_START
;
; Description:  Switch CPU to protected mode, INIT CAR and setup stack.
;
; Input:        CPU registers at CPU reset
;
; Output:       None
;
; Modified:     All, except EBP and DS.
;
; Notes:        1. File contains both 16 bit & 32 bit code
;               2. Since the default code is 32 bit, 16 bit code are left
;                  as binary machine codes
;
;----------------------------------------------------------------------------
;<AMI_PHDR_END>

SEC_BSPStart    PROC    PUBLIC

; <<----  Processor is in 16bit real mode  ---->>
; Mask the interrupts
        cli

;        BYTE    0EBh, 0FEh              ; JMP $

; Checkpoint 00h
        BYTE    030h, 0C0h              ; xor al, al
        BYTE    0E6h, 080h              ; out 80h, al

; Save EAX (BIST data in ESP)
        BYTE    66h                     ; Operand size override
        mov     esp, eax

; Save EDX (ResetID data in EBP)
        BYTE    66h                     ; Operand size override
        mov     ebp, edx

; Prepare for protected mode switching

        ;Switch to protected mode
        ;lgdt   fword ptr cs:[GdtDescriptor] ; Relative to 4G.
;       db  66h,2eh,0fh,1,16h
;       dw  offset GdtDescriptor    ;<-------Problem------<

        db   66h
        mov  ebx, offset GdtDescriptor
        ;lgdt fword ptr cs:[bx]
        db  66h, 2eh,0fh,1,17h

        mov     eax, cr0
        or      al, 1           		; Set PE bit
        mov     cr0, eax        		; Turn on Protected Mode

; <<----  Processor is in 16bit protected mode  ---->>
        ; Clear the CPU prefetch queue
        jmp     $ + 2

; Setup segment registers with appropriate descriptor
;;      mov     ax, DATA_SEL
;;      mov     ds, ax
;;      mov     es, ax
;;      mov     ss, ax
;;      mov     fs, ax
;;      mov     gs, ax
        BYTE    66h                     ; Data size
        mov     eax, DATA_SEL
        BYTE    08Eh, 0D8h
        BYTE    08Eh, 0C0h
        BYTE    08Eh, 0D0h
        BYTE    08Eh, 0E0h
        BYTE    08Eh, 0E8h

        ;set cs segment
        ;jmp 10:CHANGE_CS
        db  66h, 0eah
        dd  changeCs                    ; Relative to 4G.
        dw  CODE_SEL
        
changeCs:
; <<----  Processor is in 32 bit protected mode  ---->>
IF FV_BB_IN_DRAM
        mov eax, $
        and eax, (MKF_HIGH_MEMORY_REGION_BASE + 0ffffffh)
        add eax, 0fh
        jmp eax
ENDIF
;Setup MTRR start
        xor     edx, edx
; Use variable MTRRs to set APOB space to WB
        mov     eax, (FW_CODE_AREA_START OR WriteBack)
        mov     ecx, MTRR_PHYS_BASE_7
        wrmsr
; Write the size
        mov     eax, (FW_CODE_AREA_SIZE_MASK OR ValidMask)
        mov     edx, MTRR_VAR_TOP_MASK_VALUE
        inc     ecx                     ; MTRR_PHYS_MASK_7
        wrmsr
;Setup MTRR Ends
; Invoke the SECCore Init hook for other components to hook functions

; INPUT:
;       ESP     BIST information
;       EBP     Reset ID (value of EDX at reset)

        ;Current AGESA max stack size is 256k, soon be increased to 1MB
        mov ecx, MKF_FLASH_SIZE ;cache zone size
        mov edx, 0              ; cache zone base = 0
        AMD_ENABLE_UEFI_STACK2 STACK_AT_BOTTOM, MKF_CAR_TOTAL_SIZE, BSP_STACK_BASE_ADDR
        jmp     SECCoreAtPowerOn

SECCoreAtPowerOnEnd::
        cmp     DWORD PTR [esp + 4], 53535353h
        jz      SECCoreAtPowerOnEnd_S3

        cld
ChkPnt  011h

; Initialize FPU on BSP
;        fninit

; In CR4, set OSFXSR bit 9 and OSXMMEXCPT bit 10
        mov     eax, cr4
        or      ax, (1 SHL 9) + (1 SHL 10)
        mov     cr4, eax

		xor 	edx, edx

 ;Check is BSP then continue
        mov     ecx, MSR_XAPIC_BASE
        rdmsr
        and     eax, 0100h
        cmp     eax, 0100h
        jne     Iam_AP
   
IF FV_BB_IN_DRAM 
        mov eax, $
        and eax, (MKF_HIGH_MEMORY_REGION_BASE + 0ffffffh)
        add eax, 0fh
        jmp eax
ENDIF        
       mov   esp, MKF_CAR_BASE_ADDRESS + MKF_CAR_TOTAL_SIZE

        pushd   027fh
        fldcw   WORD PTR [esp]          ; Set FP Control Word according UEFI
        add     esp, 4

IF SEC_RESERVE_IDT_FOR_RX_SERIAL
        ; reserve IDT space for DebugRx
        sub     esp,320
        cld
        push    edi
        push    ecx
        sub     eax, eax
        mov     edi, esp
        add     di,	 8
        mov     ecx, 328/4
        rep     stosd
        pop     ecx
        pop     edi
ENDIF

        mov     eax, esp
        sub     esp, EFI_PEI_SERVICES_DOUBLE_POINTER_SIZE

        ; IDT Size is 0 because it will be initalized in PEI.  Here, it is only 
        ; used to reserved 8 bytes space for (EFI_PEI_SERVICES**)
        sub     esp, sizeof IDTR32
        mov     (IDTR32 ptr [esp]).BaseAddress, eax
IF SEC_RESERVE_IDT_FOR_RX_SERIAL
        mov     (IDTR32 ptr [esp]).Limit, 320
ELSE
        mov     (IDTR32 ptr [esp]).Limit, 0
ENDIF
        lidt    FWORD PTR [esp]
        add     esp, sizeof IDTR32

  ;; |-------------------|---->
  ;; |Idt Table          |
  ;; |-------------------|
  ;; |PeiService Pointer |    PeiStackSize
  ;; |-------------------|
  ;; |                   |
  ;; |      Stack        |
  ;; |-------------------|----> StackBase (MKF_CAR_BASE_ADDRESS + MKF_CAR_TOTAL_SIZE/2)
  ;; |                   |
  ;; |                   |
  ;; |      Heap         |    PeiTemporaryRamSize
  ;; |                   |
  ;; |                   |
  ;; |-------------------|---->  TempRamBase


; Call SEC C CORE
        mov    ebp, 100000h ;;clear the area to store core count
         xor eax, eax
        mov   [ebp], eax
        mov     eax, MKF_FV_BB_BASE
IF FV_BB_IN_DRAM	
        and eax, (MKF_HIGH_MEMORY_REGION_BASE + 0ffffffh)
ENDIF
        push    eax
        mov     eax, MKF_CAR_BASE_ADDRESS
        push    eax
        mov     eax, MKF_CAR_TOTAL_SIZE
        push    eax
        jmp     BeforeSECEntry

BeforeSECEntryEnd::
        call    SecCEntry
        
Iam_AP:
;jmp $
;;Read AP Launch Address
        jmp     SECCoreAPinit
SECCoreAPinitEnd::
        mov     esi, edi
        add esi, 0400h
        add esi, 04h ;size of AsmNearJump
        
@@:
	cli
	hlt
	jmp @B
        
        jmp     edi
;============================ PSP RESUME ==================================
;typedef struct _PSP_SMM_RSM_MEM_INFO {
;  UINT32                  BspStackSize;           // BSP Stack Size for resume
;  UINT32                  ApStackSize;            // AP Stack Size for resume
;  EFI_PHYSICAL_ADDRESS    StackPtr;              // Point to the base of Stack
;  EFI_PHYSICAL_ADDRESS    RsmHeapPtr;            // Point to the base of Resume Heap
;  UINT32                  HeapSize;               // Reserved Heap Size
;  UINT32                  TempRegionSize;         // Reserved Temp Region Size
;  EFI_PHYSICAL_ADDRESS    TempRegionPtr;         // Point to the base of Temporary Region (used for store the dynamic value during SMM SEC Phase)
;} PSP_SMM_RSM_MEM_INFO;

SECCore_PspResEntry::
        mov     esi, eax                ; Save EAX - pointer to PSP_SMM_RSM_MEM_INFO
        push    53535353h
        push    eax

;Clear Long Mode Enable 
        mov     ecx, 0c0000080h         ; EFER MSR number. 
        rdmsr                           
        btr     eax, 8                  ; Set LME=0 
        wrmsr

        xor     edx, edx
;Setup MTRR Start >>>
; Use variable MTRRs to set ROM space to WP
        mov     eax, (FW_CODE_AREA_BASE OR WriteProtect)
        mov     ecx, MTRR_PHYS_BASE_7
        wrmsr
        
; Write the size
        mov     eax, (FW_CODE_AREA_SIZE_MASK OR ValidMask)
        mov     edx, MTRR_VAR_TOP_MASK_VALUE
        inc     ecx                     ; MTRR_PHYS_MASK_7
        wrmsr
;Setup MTRR Ends <<<

        
        mov     ecx, MSR_XAPIC_BASE
        rdmsr
        and     eax, 0100h
        cmp     eax, 0100h
        je      @f

        ; AP related code
        lea     eax, [esp + 8]
        push    eax                     ; pointer to the top of stack
        lea     eax, [esp]

        push    eax
        
        xor     eax, eax
        push    eax
        lidt    FWORD PTR [esp+2]
;        pop     eax
;        pop     eax

        jmp     Iam_AP

ALIGN 4
        db      51h,52h,53h,54h
        dd      OFFSET GDT_BASE         ; GDT offset
        dw      0010h                   ; CODE selector
        dw      0008h                   ; DATA selector
        dd      OFFSET SECCore_PspResEntry ; RSM Entry Point
        dd      00000EDFh               ; RSM EDX signature
@@:
IF FV_BB_IN_DRAM 
        mov eax, $
        and eax, (MKF_HIGH_MEMORY_REGION_BASE + 0ffffffh)
        add eax, 0fh
        jmp eax
ENDIF
        push    edx
        push    eax
        mov     dx, 0cd6h
        mov     al, 4                   ; PMx04
        out     dx, al
        inc     dx                      ; CD7
        in      al, dx
        or      al, 2                   ; MMIO enable
        out     dx, al
        dec     dx                      ; cd6
        xor     ax,ax                   ; PMx00
        out     dx, al
        inc     dx
        in      al, dx
        or      al, 11h                 ; enable SMBUS decode, legacyIO Enable
        out     dx, al

        ;---  Adding MMIO Base and Limit - Early setup needed for system boot with real Hw

        mov     dx, 0cf8h
        mov     eax, 8000c184h
        out     dx, eax
        add     dx, 4
        mov     eax, 00FED880h
        out     dx, eax
        mov     dx, 0cf8h
        mov     eax, 8000c180h
        out     dx, eax
        add     dx, 4
        mov     eax, 00FED803h
        out     dx, eax

        pop     eax
        pop     edx

        ; re-programm PM_BASE
        push    ebx
        mov     ebx, 0FED80362h         ; Pm1CtlBlock Base
        mov     ax, MKF_PM_BASE_ADDRESS + 04h
        mov     WORD PTR [ebx], ax
        pop     ebx

        jmp     SECCoreAtPowerOn

SECCoreAtPowerOnEnd_S3::
IF SEC_RESERVE_IDT_FOR_RX_SERIAL
        ; reserve IDT space for DebugRx
        sub     esp,320
        cld
;        push    edi
;        push    ecx
        sub     eax, eax
        mov     edi, esp
;        add     di,  8
        mov     ecx, 320/4
        rep     stosd
;        pop     ecx
;        pop     edi
ENDIF
        mov     eax, esp
        sub     esp, EFI_PEI_SERVICES_DOUBLE_POINTER_SIZE

        ; IDT Size is 0 because it will be initalized in PEI.  Here, it is only 
        ; used to reserved 8 bytes space for (EFI_PEI_SERVICES**)
        sub     esp, sizeof IDTR32
        mov     (IDTR32 ptr [esp]).BaseAddress, eax
IF SEC_RESERVE_IDT_FOR_RX_SERIAL
        mov     (IDTR32 ptr [esp]).Limit, 320
ELSE
        mov     (IDTR32 ptr [esp]).Limit, 0
ENDIF
        lidt    FWORD PTR [esp]
        add     esp, sizeof IDTR32

  ;; |-------------------|---->
  ;; |Idt Table          |
  ;; |-------------------|
  ;; |PeiService Pointer |    PeiStackSize
  ;; |-------------------|
  ;; |                   |
  ;; |      Stack        |
  ;; |-------------------|----> StackBase (MKF_CAR_BASE_ADDRESS + MKF_CAR_TOTAL_SIZE/2)
  ;; |                   |
  ;; |                   |
  ;; |      Heap         |    PeiTemporaryRamSize
  ;; |                   |
  ;; |                   |
  ;; |-------------------|---->  TempRamBase


; Call SEC C CORE
        mov     eax, MKF_FV_BB_BASE
IF FV_BB_IN_DRAM
        and eax, (MKF_HIGH_MEMORY_REGION_BASE + 0ffffffh)
ENDIF	
        push    eax
        mov     eax, dword ptr[esi + 8]    ; MKF_CAR_BASE_ADDRESS
        push    eax
        mov     eax, dword ptr[esi]    ; MKF_CAR_TOTAL_SIZE
        push    eax
        jmp     BeforeSEC_S3Entry

BeforeSEC_S3EntryEnd::
        call    SecCEntry
;==========================================================================
SEC_BSPStart    ENDP


;----------------------------------------------
align 16
GDT_BASE:
NULL_SEL        equ     $ - GDT_BASE   ;NULL Selector 0
        dd  0, 0

DATA_SEL        equ     $ - GDT_BASE   ; Selector 8, Data 0-ffffffff 32 bit
        dd 0000ffffh
        dd 00cf9300h

CODE_SEL        equ     $ - GDT_BASE   ; Selector 10h, CODE 0-ffffffff 32 bit
        dd 0000ffffh
        dd 00cf9b00h

; We only need this because Intel DebugSupport driver
; (RegisterPeriodicCallback function) assumes that selector 0x20 is valid
; The funciton sets 0x20 as a code selector in IDT
;
; To switch to 16 bit, Selectors SYS16_CODE_SEL and SYS16_DATA_SEL are used.
;
; System data segment descriptor
;
SYS_DATA_SEL    equ     $ - GDT_BASE    ; Selector [0x18]
        dd 0000FFFFh                    ; 0 - f_ffff
        dd 00cf9300h                    ; data, expand-up, notwritable, 32-bit

; System code segment descriptor
SYS_CODE_SEL    equ     $ - GDT_BASE    ; Selector [0x20]
        dd 0000FFFFh                    ; 0 - f_ffff
        dd 00cf9b00h                    ; data, expand-up, writable, 32-bit
SPARE3_SEL      equ     $ - GDT_BASE    ; Selector [0x28]
        dd 0, 0
SYS_DATA64_SEL  equ     $ - GDT_BASE    ; Selector [0x30]
        dd 0000FFFFh
        dd 00cf9300h
SYS_CODE64_SEL  equ     $ - GDT_BASE    ; Selector [0x38]
        dd 0000FFFFh
        dd 00af9b00h
SPARE4_SEL      equ     $ - GDT_BASE    ; Selector [0x40]
        dd 0, 0
GDT_SIZE        equ     $ - GDT_BASE    ; Size of Descriptor Table

GdtDescriptor:
        dw  GDT_SIZE - 1                ; GDT limit
        dd  offset GDT_BASE             ; GDT base  Relative to 4G.

;----------------------------------------------------------------------------
;       STARTUP_SEG  S E G M E N T  ENDS
;----------------------------------------------------------------------------
STARTUP_SEG     ENDS

END

;*************************************************************************
;*************************************************************************
;**                                                                     **
;**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
;**                                                                     **
;**                       All Rights Reserved.                          **
;**                                                                     **
;**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
;**                                                                     **
;**                       Phone: (770)-246-8600                         **
;**                                                                     **
;*************************************************************************
;*************************************************************************
