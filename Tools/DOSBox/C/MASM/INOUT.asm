
.386p
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
	mov ax,03h
    int 10h
	MOV EAX,125FFDDH
	call	print_eax
	mov ah,4ch
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
INONEstring:
	mov ax,0100h
	int 21h
ret
	
print_eax:
	push eax
	push eax
	push eax
	push eax
	push eax
	push eax
	push eax
	push eax
	and eax,0f0000000h
	
	eax
	shr eax,cl
	call number_conver_ascii
	MOV DL, AL            
    MOV AH, 02H
	int 21h 
	pop eax
	and eax,0f000000h
	mov cl,24
	shr eax,cl
	call number_conver_ascii
	MOV DL, AL            
    MOV AH, 02H
	int 21h 
	pop eax
	and eax,0f00000h
	mov cl,20
	shr eax,cl
	call number_conver_ascii
	MOV DL, AL            
    MOV AH, 02H
	int 21h 
	pop eax
	and eax,0f0000h
	mov cl,16
	shr eax,cl
	call number_conver_ascii
	MOV DL, AL            
    MOV AH, 02H
	int 21h 
	pop eax
	and eax,0f000h
	mov cl,12
	shr eax,cl
	call number_conver_ascii
	MOV DL, AL            
    MOV AH, 02H
	int 21h 
	pop eax
	and eax,0f00h
	mov cl,8
	shr eax,cl
	call number_conver_ascii
	MOV DL, AL            
    MOV AH, 02H
	int 21h 
	pop eax
	and eax,0f0h
	mov cl,4
	shr eax,cl
	call number_conver_ascii
	MOV DL, AL            
    MOV AH, 02H
	int 21h 
	pop eax
	and eax,0fh
	call number_conver_ascii
	MOV DL, AL                           ;输出字符
    MOV AH, 02H
	INT 21H
ret
print_nextline:
			mov ah,03h       ;读取光标信息
			mov bh,00h       ;第0页
			int 10h          ;CH:光标的起始行、CL:光标的终止行、DH:行(Y坐标)，DL:列(X坐标)
			add dh,1         ;DH:行(Y坐标)向下移动一行
			mov dl,0		 ;DL:列(X坐标) 移动到行首
			mov ah,02h       ;置光标
			mov bh,00h       ;第0页
			int 10h
	ret

code ends
end start
