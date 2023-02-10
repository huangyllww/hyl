.386  ;eax ,ebx 等寄存器需添加.386.model  指令集
; 给出存储空间
data segment use16
	temp_device0    db	'device0 offset 30h,60h,61h,70h,f0h:',2 dup (0)
	temp_device1  	db	'device1 offset 30h,60h,61h,70h,f0h:',2 dup (0)
	temp_device2  	db	'device2 offset 30h,60h,61h,70h,f0h:',2 dup (0)
	temp_device3 	db	'device3 offset 30h,60h,61h,70h,f0h:',2 dup (0)
	temp_device4  	db	'device4 offset 30h,60h,61h,70h,f0h:',2 dup (0)
	temp_device5  	db	'device5 offset 30h,60h,61h,70h,f0h:',2 dup (0)
	temp_device6	db	'device6 offset 30h,60h,61h,70h,f0h:',2 dup (0)
	temp_device7	db	'device7 offset 30h,60h,61h,70h,f0h:',2 dup (0)
	temp_device8	db	'device8 offset 30h,60h,61h,70h,f0h:',2 dup (0)
	temp_device9	db	'device9 offset 30h,60h,61h,70h,f0h:',2 dup (0)
	temp_device10	db	'device10 offset 30h,60h,61h,70h,f0h:',2 dup (0)
	temp_device11	db	'device11 offset 30h,60h,61h,70h,f0h:',2 dup (0)
	temp_device12	db	'device12 offset 30h,60h,61h,70h,f0h:',2 dup (0)
	temp_device13	db	'device13 offset 30h,60h,61h,70h,f0h:',2 dup (0)
	temp_device14	db	'device14 offset 30h,60h,61h,70h,f0h:',2 dup (0)
	temp_device15	db	'device15 offset 30h,60h,61h,70h,f0h:',2 dup (0)
	string_active	db	'active',2 dup (0)
	string_inactive  db	'inactive',2 dup (0)
	string_space  db ' ',2 dup(0)
data ends
assume cs:code,ds:data; assume要指定数据
code segment use16
start:
;末尾加上“use16”以使编译器识别32位寄存器
;读取厂商名称
mov eax,data
mov ds,eax 
call clearscreen
;-----------------------------------------------------
; Enter the Extended Function Mode
;-----------------------------------------------------
mov dx, 04eh
mov al, 0a5h
out dx, al
out dx, al
;----------------------------------------------------
; Configure Logical Device 1 通过 out 4eh,07h  out 4fh,01h 选中device01
;首先通过 7端口选择device ,后面可以同样操作改变对应device 的寄存器配置 ，前30位为全局区域
;-----------------------------------------------------
call print_device0
mov dx,04EH
mov al,07h
out dx,al
mov dx,04fh
mov al,00h   ; 选中device0
out dx,al   
call print_device_0ffset30_f0 ;输出offset30_f0的内容
call print_nextline

call print_device1
mov dx,04EH
mov al,07h
out dx,al
mov dx,04fh
mov al,01h   ; 选中device1
out dx,al   
call print_device_0ffset30_f0 ;输出offset30_f0的内容
call print_nextline

call print_device2
mov dx,04EH
mov al,07h
out dx,al
mov dx,04fh
mov al,02h   ; 选中device2
out dx,al   
call print_device_0ffset30_f0 ;输出offset30_f0的内容
call print_nextline
call print_device3
mov dx,04EH
mov al,07h
out dx,al
mov dx,04fh
mov al,03h   ; 选中device3
out dx,al   
call print_device_0ffset30_f0 ;输出offset30_f0的内容
call print_nextline

call print_device4
mov dx,04EH
mov al,07h
out dx,al
mov dx,04fh
mov al,04h   ; 选中device4
out dx,al   
call print_device_0ffset30_f0 ;输出offset30_f0的内容
call print_nextline

call print_device5
mov dx,04EH
mov al,07h
out dx,al
mov dx,04fh
mov al,05h   ; 选中device5
out dx,al   
call print_device_0ffset30_f0 ;输出offset30_f0的内容
call print_nextline
call print_device6
mov dx,04EH
mov al,07h
out dx,al
mov dx,04fh
mov al,06h   ; 选中device6
out dx,al   
call print_device_0ffset30_f0 ;输出offset30_f0的内容
call print_nextline

call print_device7
mov dx,04EH
mov al,07h
out dx,al
mov dx,04fh
mov al,07h   ; 选中device7
out dx,al   
call print_device_0ffset30_f0 ;输出offset30_f0的内容
call print_nextline
call print_device8
mov dx,04EH
mov al,07h
out dx,al
mov dx,04fh
mov al,08h   ; 选中device8
out dx,al   
call print_device_0ffset30_f0 ;输出offset30_f0的内容
call print_nextline
call print_device9
mov dx,04EH
mov al,07h
out dx,al
mov dx,04fh
mov al,09h   ; 选中device9
out dx,al   
call print_device_0ffset30_f0 ;输出offset30_f0的内容
call print_nextline
call print_device10
mov dx,04EH
mov al,07h
out dx,al
mov dx,04fh
mov al,0ah   ; 选中device10
out dx,al   
call print_device_0ffset30_f0 ;输出offset30_f0的内容
call print_nextline
call print_device11
mov dx,04EH
mov al,07h
out dx,al
mov dx,04fh
mov al,0bh   ; 选中device11
out dx,al   
call print_device_0ffset30_f0 ;输出offset30_f0的内容
call print_nextline
call print_device12
mov dx,04EH
mov al,07h
out dx,al
mov dx,04fh
mov al,0ch   ; 选中device12
out dx,al   
call print_device_0ffset30_f0 ;输出offset30_f0的内容
call print_nextline
call print_device13
mov dx,04EH
mov al,07h
out dx,al
mov dx,04fh
mov al,0dh   ; 选中device13
out dx,al   
call print_device_0ffset30_f0 ;输出offset30_f0的内容
call print_nextline
call print_device14
mov dx,04EH
mov al,07h
out dx,al
mov dx,04fh
mov al,0eh   ; 选中device14
out dx,al   
call print_device_0ffset30_f0 ;输出offset30_f0的内容
call print_nextline
call print_device15
mov dx,04EH
mov al,07h
out dx,al
mov dx,04fh
mov al,0fh   ; 选中device15
out dx,al   
call print_device_0ffset30_f0 ;输出offset30_f0的内容

MOV DX, 04EH  ;exit the Extended function mode  退出扩展模式 
MOV AL, 0AAH
OUT DX, AL
mov ax,4c00h  ;exit  退出程序
int 21h


print_device_0ffset30_f0:
	mov dx,04eh
	mov al,30h     
	out dx,al  ;选取30 寄存器
	MOV DX,04fH
	MOV AL,30h ; 读取offset 30 al的内容
	IN AL,DX
	call show_al
	call print_space
	mov dx,04eh
	mov al,60h     
	out dx,al  ;选取60 寄存器
	MOV DX,04fH
	MOV AL,60h ; 读取offset 60 al的内容
	IN AL,DX
	call show_al
	call print_space
	mov dx,04eh
	mov al,61h     
	out dx,al  ;选取61 寄存器
	MOV DX,04fH
	MOV AL,61h ; 读取offset 61 al的内容
	IN AL,DX
	call show_al
	call print_space
	mov dx,04eh
	mov al,70h     
	out dx,al  ;选取70 寄存器
	MOV DX,04fH
	MOV AL,70h ; 读取offset 70 al的内容
	IN AL,DX
	call show_al
	call print_space
	mov dx,04eh
	mov al,0f0h     
	out dx,al  ;选取f0 寄存器
	MOV DX,04fH
	MOV AL,0f0h ; 读取offset f0 al的内容
	IN AL,DX
	call show_al
	call print_space

	mov dx,04eh
	mov al,30h     
	out dx,al  ;选取30 寄存器
	MOV DX,04fH
	MOV AL,30h ; 读取offset 30 al的内容
	IN AL,DX
	cmp al,0ffh
	je show_inactive 
	jne cmp_al_00
	cmp_al_00:
	cmp al,00h
	je show_inactive 
	jne show_active
	show_active:
	call print_active
	jmp cmp_end
	show_inactive:
	call print_inactive
	jmp cmp_end
	cmp_end:
	ret
show_al:
   mov bl,al  ;high to display
   and al,0f0h 
   mov cl,4
   shr al,cl 
   call number_conver_ascii
   mov dl,al
   mov ah,02h
   int 21h
   and bl,0fh  
   mov al,bl
   call number_conver_ascii
   mov dl,al
   mov ah,02h
   int 21h
  ret
print_device0:                          ;打印提示字符串
				mov si,offset temp_device0
				mov bl,0fh
				call print_string
			ret
print_device1:                          ;打印提示字符串
				mov si,offset temp_device1
				mov bl,0fh
				call print_string
			ret
          	
print_device2:                          ;打印提示字符串
				mov si,offset temp_device2
                mov bl,0fh
				call print_string
			ret
print_device3:                          ;打印提示字符串
				mov si,offset temp_device3
				mov bl,0fh
				call print_string
			ret
print_device4:                          ;打印提示字符串
				mov si,offset temp_device4
				mov bl,0fh
				call print_string
			ret
          	
print_device5:                          ;打印提示字符串
				mov si,offset temp_device5
                mov bl,0fh
				call print_string
			ret

print_device6:                     ;打印提示字符串
				mov si,offset temp_device6
				mov bl,0fh
				call print_string
			ret
          	
print_device7:                          ;打印提示字符串
				mov si,offset temp_device7
                mov bl,0fh
				call print_string
			ret
		
print_device8:                          ;打印提示字符串
				mov si,offset temp_device8
				mov bl,0fh
				call print_string
			ret
          	
print_device9:                          ;打印提示字符串
				mov si,offset temp_device9
				mov bl,0fh
				call print_string
			ret
print_device10:                          ;打印提示字符串
				mov si,offset temp_device10
				mov bl,0fh
				call print_string
			ret
          	
print_device11:                          ;打印提示字符串
				mov si,offset temp_device11
                mov bl,0fh
				call print_string
			ret

print_device12:                          ;打印提示字符串
				mov si,offset temp_device12
				mov bl,0fh
				call print_string
			ret
print_device13:                          ;打印提示字符串
				mov si,offset temp_device13
				mov bl,0fh
				call print_string
			ret
          	
print_device14:                          ;打印提示字符串
				mov si,offset temp_device14
                mov bl,0fh
				call print_string
			ret
 
print_device15:                          ;打印提示字符串
				mov si,offset temp_device15
                mov bl,0fh
				call print_string
			ret
print_active:                          ;打印提示字符串
				mov si,offset string_active
                mov bl,0fh
				call print_string
			ret
print_inactive:
				mov si,offset string_inactive
                mov bl,0fh
				call print_string
			ret
print_space:
				mov si,offset string_space
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





