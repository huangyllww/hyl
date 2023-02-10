.386.model   ;eax ,ebx 等寄存器需添加.386.model
; 给出存储空间
data segment
	Vendor 			db 'Vendor ID:',12 dup (0)
	Information		db 16 dup (0)
	Family			db 'Family:',5 dup (0)
	Model			db 'Model:',5 dup (0)
	Step			db 'Stepping:',5 dup(0)
data ends
assume cs:code,ds:data; assume要指定数据
code segment use16
start:
; 末尾加上“use16”以使编译器识别32位寄存器
	call clearscreen    ;清屏
	mov eax,data
	mov ds,eax 
	mov si ,offset Vendor	
	mov al,si
	mov bl,0fh		
	call print_string
 ;读取厂商名称
	mov eax,0
	db 0fh  ;对于不支cpuid的处理器  使用 db 0f db 0a2h 代替cpuid指令
	db 0a2h
	mov bp,offset Information
	mov ds:[bp],ebx
	mov ds:[bp+4],edx
	mov ds:[bp+8],ecx
	mov si,0ch;
s:
	mov al,ds:[bp]
	mov bl ,0fh
	call print_char_color
	dec si
	inc bp; 
	cmp si,0
	jne s
	call print_nextline
	mov si ,offset Family
	mov al,si
	mov bl,0fh
	call print_string
	mov eax,1
	db 0fh  ;对于不支cpuid的处理器  使用 db 0f db 0a2h 代替cpuid指令
	db 0a2h 
	mov edi,eax
	mov cl,8
	shr eax,cl
	add al,30h
	mov bl,0fh
	call print_char_color
	call print_nextline
	mov si ,offset Model
	mov al,si
	call print_string
	mov eax,edi
	and eax,000000f0h
	mov cl,4
	shr eax,cl
	add al,30h
	call print_char_color
	call print_nextline
	mov si ,offset Step
	mov al,si
	call print_string
	mov eax,edi
	and ax,0fh
	add al,30h
	call print_char_color
	call exit
exit:                               ;终止运行退出
	mov ax,04c00h
	int 21h
ret
clearscreen:                         ;清屏
                mov ah,00h
                mov al,03h
                int 10h
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
print_string_ret:               ;ascall和数字转换
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
		print_blank:    ;打印字符空格
			mov al,' '
			mov bl,0fh
			call print_char_color
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
            mov ah,09h       ;在光标位置显示字符
			; mov al,31h     ;字符 ASCII
			; mov bl,0fb     ;颜色：[7]:闪烁、[6,5,4]:背景R,G,B、[3]:高亮、[2,1,0]:前景R,G,B
			mov bh,00h       ;第0页
			mov cx,01h       ;字符重复个数
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
code ends
end start