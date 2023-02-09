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
;-----------------------------------------------------------------------
;
; Name:		CPUSupport.asm
;
; Description:	This is the CPU support file that contains assembly
;		language routines for the DXE CPU Module.  This file
;		also contains the default interrupt handlers.
;
;-----------------------------------------------------------------------
;<AMI_FHDR_END>

ifndef EFIx64

.686p
.model  flat        
.code

extern _gC1EStackSave:DWORD
extern _gC1ESupport:BYTE

;<AMI_PHDR_START>
;---------------------------------------------------------------------------
;
; Procedure: EnableC1E
;
; Description: Enable C1E. It is done in assembly because some CPUs genenerate
;   a GPF when enabling C1E if unsupported. The interrupt handler will return
;   to this routine. The stack must be set properly.
;
; Input: None
;
; Output: None
;---------------------------------------------------------------------------
;<AMI_PHDR_END>

EnableC1E       PROC    C
	;Some CPUs may generate a GPF if C1E is not supported.
	; This routine and the temp C1E GPF handler takes care of this.
	; Enable Enhanced Halt State C1E. Note: Need setup question?

	;This routine is not reentrant.
	pushad
	push 	EnableC1Eerror			;If gpf occurs, C1E gpf handler will give control to this.
	mov		_gC1EStackSave, esp		;Save esp for handler.

	mov		ecx, 1a0h		;IA32_MISC_ENABLE
	rdmsr
	or		eax, 1 SHL 25	;enable C1E
	wrmsr
	rdmsr
	bt		eax, 25
	jc		@f				;Cary set if enabled
	mov		_gC1ESupport, 0
    
@@:
	add		esp, 4			;pop from stack.
EnableC1Eerror	equ $
	popad
	ret
EnableC1E ENDP

;<AMI_PHDR_START>
;---------------------------------------------------------------------------
;
; Procedure:	TempGPInterruptHandler
;
; Description: This routine returns from a GPF generated on some 
;   CPUs enabling C1E if unsupported. Only a way detecting C1E is
;   to enable it.
;
; Input:
;      IN EFI_EXCEPTION_TYPE   Exception
;      IN EFI_SYSTEM_CONTEXT   Context
;
; Output:
;      None
;---------------------------------------------------------------------------
;<AMI_PHDR_END>

_TempGPInterruptHandler PROC
	mov _gC1ESupport, 0
	mov esp, _gC1EStackSave
	ret
_TempGPInterruptHandler ENDP


public _MachineCheckHandlerSize

;<AMI_PHDR_START>
;---------------------------------------------------------------------------
;
; Procedure: MachineCheckHandler
;
; Description:
; Halt loop to be copied as the Machine check handler.
; MachineCheckHandler will be copied to runtime.
;
; Input: None
;
; Output: None
;---------------------------------------------------------------------------
;<AMI_PHDR_END>

MachineCheckHandler     PROC C

@@:
        cli
        hlt
        jmp     @b
        ret
MachineCheckHandler     ENDP
MachineCheckHandlerEnd equ $
_MachineCheckHandlerSize dd offset MachineCheckHandlerEnd - MachineCheckHandler

else
.code
include Token.equ
extern gC1EStackSave:QWORD
extern gC1ESupport:BYTE

;<AMI_PHDR_START>
;---------------------------------------------------------------------------
;
; Procedure: EnableC1E
;
; Description:
; Enable C1E. It is done in assembly because some CPUs genenerate
; a GPF when enabling C1E if unsupported. The interrupt handler will return
; to this routine. The stack must be set properly.
;
; Input: None
;
; Output: None
;---------------------------------------------------------------------------
;<AMI_PHDR_END>

EnableC1E       PROC
	;Some CPUs may generate a GPF if C1E is not supported.
	; This routine and the temp C1E GPF handler takes care of this.
	;Enable Enhanced Halt State C1E. Note: Need setup question?

	;This routine is not reentrant.
;	pushad
;   markw this next instruction is generating an error.
;	push 	qword ptr EnableC1Eerror			;If gpf occurs, C1E gpf handler will give control to this.
    mov		gC1EStackSave, rsp		;Save esp for handler.

	mov		ecx, 1a0h		;IA32_MISC_ENABLE
	rdmsr
	or		eax, 1 SHL 25	;enable C1E
	wrmsr
	rdmsr
	bt		eax, 25
	jc		@f				;Cary set if enabled
	mov		gC1ESupport, 0
    
@@:
	add		rsp, 4			;pop from stack.
EnableC1Eerror	equ $
;	popad
	ret
EnableC1E       ENDP

;<AMI_PHDR_START>
;---------------------------------------------------------------------------
;
; Procedure:	TempGPInterruptHandler
;
; Description:
; This routine returns from a GPF generated on some
; CPUs enabling C1E if unsupported. Only a way detecting C1E is
; to enable it.
;
; Input:
;      IN EFI_EXCEPTION_TYPE   Exception
;      IN EFI_SYSTEM_CONTEXT   Context
;
; Output:
;      None
;---------------------------------------------------------------------------
;<AMI_PHDR_END>

TempGPInterruptHandler  PROC
	mov gC1ESupport, 0
	mov rsp, gC1EStackSave
	ret
TempGPInterruptHandler  ENDP


public MachineCheckHandlerSize

;<AMI_PHDR_START>
;---------------------------------------------------------------------------
;
; Procedure: MachineCheckHandler
;
; Description:
; Halt loop to be copied as the Machine check handler.
; MachineCheckHandler will be copied to runtime.
;
; Input: None
;
; Output: None
;---------------------------------------------------------------------------
;<AMI_PHDR_END>

MachineCheckHandler     PROC

@@:
        cli
        hlt
        jmp     @b
        ret
MachineCheckHandler     ENDP
MachineCheckHandlerEnd equ $
MachineCheckHandlerSize dd offset MachineCheckHandlerEnd - MachineCheckHandler

;<AMI_PHDR_START>
;---------------------------------------------------------------------------
;
; Procedure: SaveC1EContext
;
; Description:
; This saves register information that is lost if an GPF occurs.
;
; Input: None
;
; Output: None
;---------------------------------------------------------------------------
;<AMI_PHDR_END>

SaveC1EContext  PROC
        pop     rax     ; Get RIP
        push    rbx
        push    rcx
        push    rdx
        push    r8
        push    r9
        push    r10
        push    r11
        push    rax     ; Restore RIP
        ret
SaveC1EContext  ENDP

;<AMI_PHDR_START>
;---------------------------------------------------------------------------
;
; Procedure: RestoreC1EContext
;
; Description:
; This restores register information that is lost if an GPF occurs.
;
; Input: None
;
; Output: None
;---------------------------------------------------------------------------
;<AMI_PHDR_END>

RestoreC1EContext       PROC
        pop     rax     ; Get rip
        pop     r11
        pop     r10
        pop     r9
        pop     r8
        pop     rdx
        pop     rcx
        pop     rbx
        push    rax     ; Restore rip
        ret
RestoreC1EContext       ENDP

AsmIret PROC
        iretq
AsmIret ENDP

AsmEoiIret PROC
        push    rax
        push    rbx
        xor     eax, eax
        mov     rbx, MKF_APIC_BASE + MKF_APIC_EOI_REGISTER
        ; Indicate End-Of-Interrupt to APIC
        mov     [rbx], eax
        pop     rbx
        pop     rax
        iretq
AsmEoiIret ENDP

endif

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
