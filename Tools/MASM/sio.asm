.386  ;eax ,ebx 等寄存器需添加.386.model
; 给出存储空间
data segment use16
	temp_30	db	'the offset 30:',2 dup (0)
	temp_60	db	'the offset 60:',2 dup (0)
	temp_61	db	'the offset 61:',2 dup (0)
    temp_70	db	'the offset 70:',2 dup (0)
    temp_str db	'the offset f0:',2 dup (0)	
data ends
assume cs:code,ds:data; assume要指定数据
code segment use16
start:
末尾加上“use16”以使编译器识别32位寄存器
;读取厂商名称
mov eax,data
mov ds,eax 
call clearscreen
call print_temp_30
MOV DX, 04EH
MOV AL, 087H
OUT DX, AL
mov al,01h
out dx,al
mov al,055h
out dx,al
mov al,055h
out dx,al   ; ENTER PNP MOD
MOV AL,07H
OUT DX,AL
MOV DX,04FH
MOV AL,01H
OUT DX,AL
MOV DX,04EH
MOV AL,30h ; 读取offset 30 al的内容
OUT DX,AL
MOV DX,04FH
MOV AL,089H
IN AL,DX
call show_al
call print_nextline
call print_temp_60
MOV DX,04EH
MOV AL,60h ; 读取 0ffset60的内容
OUT DX,AL
MOV DX,04FH
MOV AL,089H
IN AL,DX
call show_al
call print_nextline
call print_temp_61
MOV DX,04EH
MOV AL,61h ; 读取 0ffset61的内容
OUT DX,AL
MOV DX,04FH
MOV AL,089H
IN AL,DX
call show_al
call print_nextline
call print_temp_70
MOV DX,04EH
MOV AL,70h ; 读取 0ffset70的内容
OUT DX,AL
MOV DX,04FH
MOV AL,089H
IN AL,DX
call show_al
call print_nextline
call print_temp_str
MOV DX,04EH
MOV AL,0f0h ; 读取 0ffsetf0的内容
OUT DX,AL
MOV DX,04FH
MOV AL,089H
IN AL,DX
call show_al
MOV DX, 4EH
MOV AL, 0AAH
OUT DX, AL

mov ax,4c00h  ;exit  退出 
int 21h

show_al:
   mov bl,al  ;high to display
   and al,0f0h 
   mov cl,4
   shr al,cl 
   call number_conver_ascii
   mov dl,al
   mov ah,02h
   int 21h
   ;low to display
   and bl,0fh  
   mov al,bl
   call number_conver_ascii
   mov dl,al
   mov ah,02h
   int 21h
  ret
print_temp_30:                          ;打印提示字符串
				mov si,offset temp_30
				mov bl,0fh
				call print_string
			ret
print_temp_60:                          ;打印提示字符串
				mov si,offset temp_60
				mov bl,0fh
				call print_string
			ret
          	
print_temp_61:                          ;打印提示字符串
				mov si,offset temp_61
                mov bl,0fh
				call print_string
			ret
print_temp_70:                          ;打印提示字符串
				mov si,offset temp_70
				mov bl,0fh
				call print_string
			ret
print_temp_str:                          ;打印提示字符
				mov si,offset temp_str
				mov bl,0fh
				call print_string
			ret
print_string:
                mov cl,ds:[si]
                mov ch,0
                jcxz print_string_ret   ;条件转移 当cx=0 跳转             
                mov al,ds:[si]
                mov bl,0fh    ;颜色：[7]:闪烁、[6,5,4]:背景R,G,B、[3]:高亮、[2,1,0]:前景R,G,B
                call print_char_color
                inc si
                jmp print_string
print_string_ret:
			ret	
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

;;---------------------------------------------------
;;  ---打印一个彩色字符---
;;para：al:输出字符，bl：输出字符颜色
;;ret：DH:行(Y坐标)，DL:列(X坐标)
;;---------------------------------------------------
    print_char_color:
            mov ah,03h       ;读取光标信息
            mov bh,00h       ;第0页
            int 10h          ;CH:光标的起始行、CL:光标的终止行、DH:行(Y坐标)，DL:列(X坐标)
            mov ah,09h          ;在光标位置显示字符
			mov bh,00h          ;第0页
			mov cx,01h          ;字符重复个数
			int 10h
			mov ah,03h       ;读取光标信息
			mov bh,00h       ;第0页
			int 10h          ;CH:光标的起始行、CL:光标的终止行、DH:行(Y坐标)，DL:列(X坐标)
			add dl,1         ;将光标向后移动一列
			mov ah,02h       ;置光标
			mov bh,00h       ;第0页
			int 10h
        ret
;;---------------------------------------------------
;;  ---换行---
;;para:	null
;;ret：	null
;;---------------------------------------------------		
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
    clearscreen:
                mov ah,00h
                mov al,03h
                int 10h
			ret
code ends
end start





