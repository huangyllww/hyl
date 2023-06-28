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

;*************************************************************************
; $Header: $
;
; $Revision:  $
;
; $Date:  $
;*************************************************************************
;<AMI_FHDR_START>
;
; Name: SmmExcepHandler.asm
;
; Description:  SMM IDT and Exception handlers
;
;<AMI_FHDR_END>
;*************************************************************************

EXTERNDEF   gIdtStart:QWORD

EXTERN		OldTsegBase:QWORD
EXTERN		OldTsegMask:QWORD

EnableMmioMap	PROTO C
DisableMmioMap	PROTO C

.data
align	16

gIdtStart   LABEL   QWORD
IDT_Start:
REPEAT	32
	dw	0								; Target Offset 0..15
	dw	38h								; Target Selector = Code x64
	db	0								; IST 0..2
	db	8eh								; Present, DPL=0, conforming, readable
	dw	0								; Target Offset 16..31
	dd	0								; Target Offset 32..63
	dd  0								; Reserved
ENDM
IDT_End:

.code

;<AMI_PHDR_START>
;----------------------------------------------------------------------------
; Procedure:	SmmIntHandlers
;
; Description:
;   SMM Interrupt handler, only exception vectors 0..31 used
;
; Input:
;   None
;
; Output:
;   None
;
; Modified:
;
; Referrals:
;
; Notes:
;   
;
;----------------------------------------------------------------------------
;<AMI_PHDR_END>

SmmIntHandlers   PROC
INTNUM  = 0
REPEAT  31
    push    INTNUM
    jmp     defaultEntry
INTNUM = INTNUM + 1
ENDM

    push    31

defaultEntry:
    test    spl, 8                      ; In Long mode Interrupt stack frame aligned to 16 bytes,
                                        ; to make stack frame uniform for exceptions with and without
                                        ; error code, we'll pad it with additional push
    jnz     @F
    push    [rsp]
    
@@:
	push	rax
	push	rcx
	push	rdx
	push	r8
	push	r9
    push    r10
    push    r11
    push    r12
    push    r13
    push    r14
    push    r15    
    
    mov     ax, WORD PTR[rsp + 58h]
    cmp     al, 1                       ; DB exception?
    jz      dbHandle
    cmp     al, 14                      ; PF exception?
    jz      pfHandle
    
excpDeadLoop:
    out     80h, al
    jmp     $
    
dbHandle:
    jmp     exitExcepn
    
pfHandle:
    jmp     excpDeadLoop
    
exitExcepn:
    pop     r15
    pop     r14
    pop     r13
    pop     r12
    pop     r11
    pop     r10
	pop		r9
	pop		r8
	pop		rdx
	pop		rcx
    pop     rax
	add		rsp, 16						; skip error code or padding, and Vector number
	iretq    
SmmIntHandlers   ENDP
    

;<AMI_PHDR_START>
;----------------------------------------------------------------------------
; Procedure:	FillIdt
;
; Description:	Fill IDT table with interrupt handler entries
;
; Input:
;   None
;
; Output:
;   None
;
; Modified:
;
; Referrals:
;
; Notes:
;   used assumption that SMM code is below 4Gb
;
;----------------------------------------------------------------------------
;<AMI_PHDR_END>
    
FillIdt	PROC
    push    rax
    push    rbx
    push    rcx

	lea		rbx, SmmIntHandlers
	lea		rax, IDT_Start
    lea     rdx, IDT_End - 16
    
@start:
	cmp		rax, rdx
	ja		@end
    mov     ecx, ebx
    shr     ecx, 16
    mov     word ptr[eax], bx
    mov     word ptr[eax + 6], cx
	
    add     rbx, 4
	add		rax, 16
	jmp		@start
    
@end:	
    pop     rcx
    pop     rbx
    pop     rax
	ret
FillIdt	ENDP

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
