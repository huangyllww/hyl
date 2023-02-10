
.386
    assume cs:code,ds:data
    data segment 
    temp_data           db  20  dup(0)
	temp_bus	        db	4	dup(0)
	temp_device	        db	4	dup(0)
	temp_function	    db	4	dup(0)
    temp_error          db  'ERROR! Please enter the right number(max 0x00ff)!',0
    temp_string1        db  'Please enter the start address(eg:1100) :',0
    temp_string2        db  'Please enter the tlength(eg:0000-02ff,eg:0022) :',0
    temp_string3        db  'press Enter to continue    press ESC to exit',0
data ends

code segment use16
    assume cs:code,ds:data
start:
	mov dx,0cf8h
	MOV EAX,0f8000000H
	OUT DX,EAX
	MOV DX,0CFCH
	IN EAX,DX
	MOV ECX,EAX
	and eax,0f0000000h
	mov cl,28
	shr eax,cl
	call number_conver_ascii
	MOV DL, AL            
    MOV AH, 02H
	int 21h 
	mov eax,ecx
	and eax,0f000000h
	mov cl,24
	shr eax,cl
	call number_conver_ascii
	MOV DL, AL            
    MOV AH, 02H
	int 21h 
	mov eax,ecx
	and eax,0f00000h
	mov cl,20
	shr eax,cl
	call number_conver_ascii
	MOV DL, AL            
    MOV AH, 02H
	int 21h 
	mov eax,ecx
	and eax,0f0000h
	mov cl,16
	shr eax,cl
	call number_conver_ascii
	MOV DL, AL            
    MOV AH, 02H
	int 21h 
	mov eax,ecx
	and eax,0f000h
	mov cl,12
	shr eax,cl
	call number_conver_ascii
	MOV DL, AL            
    MOV AH, 02H
	int 21h 
		mov eax,ecx
	and eax,0f00h
	mov cl,8
	shr eax,cl
	call number_conver_ascii
	MOV DL, AL            
    MOV AH, 02H
	int 21h 
	mov eax,ecx
	and al,0f0h
	mov cl,4
	shr al,cl
	call number_conver_ascii
	MOV DL, AL            
    MOV AH, 02H
	int 21h 
	
	mov ecx,eax
	and al,0fh
	call number_conver_ascii
	MOV DL, AL                           ;输出字符
    MOV AH, 02H
 	INT 21H
  mov ax,4c00h
    int 21h
	 number_conver_ascii:
				cmp al,10
				jb  conver0_9
				jnb convera_f
				
			conver0_9:
				add al,30h
				jmp conver_end
			convera_f:
				add al,57h
				jmp conver_end
			conver_end:
	ret
	
print_eax：
	MOV ebp,EAX
	and eax,0f0000000h
	mov cl,28
	shr eax,cl
	call number_conver_ascii
	MOV DL, AL            
    MOV AH, 02H
	int 21h 
	mov eax,ebp
	and eax,0f000000h
	mov cl,24
	shr eax,cl
	call number_conver_ascii
	MOV DL, AL            
    MOV AH, 02H
	int 21h 
	mov eax,ebp
	and eax,0f00000h
	mov cl,20
	shr eax,cl
	call number_conver_ascii
	MOV DL, AL            
    MOV AH, 02H
	int 21h 
	mov eax,ebp
	and eax,0f0000h
	mov cl,16
	shr eax,cl
	call number_conver_ascii
	MOV DL, AL            
    MOV AH, 02H
	int 21h 
	mov eax,ebp
	and eax,0f000h
	mov cl,12
	shr eax,cl
	call number_conver_ascii
	MOV DL, AL            
    MOV AH, 02H
	int 21h 
	mov eax,ebp
	and eax,0f00h
	mov cl,8
	shr eax,cl
	call number_conver_ascii
	MOV DL, AL            
    MOV AH, 02H
	int 21h 
	mov eax,ebp
	and eax,0f0h
	mov cl,4
	shr eax,cl
	call number_conver_ascii
	MOV DL, AL            
    MOV AH, 02H
	int 21h 
	mov eax,ebp
	and eax,0fh
	call number_conver_ascii
	MOV DL, AL                           ;输出字符
    MOV AH, 02H
 	INT 21H
	 ret


code ends
end start
