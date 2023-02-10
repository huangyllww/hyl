.386.model
assume cs:code, ds:data, ss:stack

stack segment use16
    db    128   dup(0)
stack ends

data segment use16
    temp_data           db  20  dup(0)
	temp_startaddress	db	2	dup(0)
	temp_length			db	2	dup(0)
    temp_error          db  'ERROR! Please enter the right number(max 0x00ff)!',0
    temp_string1        db  'Please enter the start address(eg:1100) :',0
    temp_string2        db  'Please enter the length(eg:0000-00ff,eg:0022) :',0
    temp_string3        db  'press Enter to continue    press ESC to exit',0
data ends

;;---------------------------------------------------
;;***************************************************
;
;   ***	主函数	***
;
;;***************************************************
;;---------------------------------------------------

code segment use16
    start:
            mov ax,stack  
            mov ss,ax
            mov sp,128
            mov ax,data
            mov ds,ax
			call clearscreen
            call print_string1          ;Please enter the starting address(eg:0x1234,)
			call KeyScanf_address
			call print_nextline
			call print_string2
			call KeyScanf_length
			call print_data
			call print_nextline
			call print_string3
		
		KeyScanf:
			mov ah,01h			;使用通道01h检测是否有按键按下
			int 16h
			jz	KeyScanf
			
			mov  ah,00h             ;有按键按下，读取按键
			int  16h
			
			cmp al,0dh			;Enter(1c0dh)
			je	start
			cmp al,1bh			;Esc(011bh)
			je	exit
			jne	KeyScanf

    exit:
			call clearscreen
            mov ax,4c00h
            int 21h
;;---------------------------------------------------
;;*************子函数********************************		
;;---------------------------------------------------
;;  ---清屏函数---
;;para:	null
;;ret：	null
;;---------------------------------------------------
    clearscreen:
                mov ah,00h
                mov al,03h
                int 10h
			ret
;;---------------------------------------------------
;;  ---键盘读取函数---
;;para:	null
;;ret：	null
;;---------------------------------------------------
	KeyScanf_address:
				mov si,offset temp_data
                mov bx,0
                mov ds:[si+5],bx        ;计数器清零				
				call KeyScanf_number
				mov si,offset temp_data		;取出处理好的地址数据
				mov ax,ds:[si+16]				
				mov si,offset temp_startaddress
				mov ds:[si],ax
			ret
	KeyScanf_length:
			length_scanf:
				mov si,offset temp_data
                mov bx,0
                mov ds:[si+5],bx        ;计数器清零				
				call KeyScanf_number
				mov si,offset temp_data		;取出处理好的长度数据
				mov ax,ds:[si+16]			
				; mov bp,ax				
				cmp ax,0ffh					;判断长度是否合法
				jna  length_ok				;长度合法，跳转	;长度非法，重新输入 CF=1,ZF=1,跳转 不小于长度
				call print_nextline
				call print_stringError
				call print_nextline
				call print_string2
				jmp length_scanf			
			length_ok:
				mov si,offset temp_length
				mov ds:[si],ax
			ret
	KeyScanf_number:   	                ;键盘输入数据
			number_Scanf:
				mov ah,01h				;检测是否有按键按下
										;中断16 的1号功能功能：用来查询键盘缓冲区，对键盘扫描但不
										;等待，并设置ZF标志。若有按键操作（即键盘缓冲区不空）
										;则ZF＝0，AL中存放的是入的ASCII码，AH中存放输入字符的扩展码。若无
										;键按下，则标志位ZF＝1。
				int 16h                 ;16号中断
				jz	number_Scanf		
				mov  ah,00h             ;有按键按下，读取按键
				int  16h
				;功能键检测
				cmp al,08h            	;ASCII 08H 对应键盘 backspace,删除
				je  Key_Backspace		;跳转到删除字符串
				cmp al,1bh				;Esc(011bh)
				je	exit                ;跳转退出
                ;;数字按键检测、判断输入是否为合法数字（0~9、a~f）
                cmp al,30h				;数字0
                jb  number_Scanf	    ;AL<0,错误,重输
                cmp al,39h              ;数字9
                jna number_0_9  		;0<=AL<=9,正确
                cmp al,61h              ;数字a
                jb  number_Scanf        ;9<AL<a,错误,重输
                cmp al,66h              ;数字f
                jna number_a_f   		;a<=AL<=f,正确
                ja  number_Scanf        ;f<=AL,错误,重输				
			Key_Backspace:
				mov si,offset temp_data
                mov bx,ds:[si+5]        ;获取计数器
                cmp bx,0                ;判断是否已经输入按键
                jna number_Scanf       	;无按键				
				mov al,0		    	;NULL
                dec bx                  ;计数器减一
                mov ds:[si+bx+12],al    ;将上次存入的数据清零
                mov ds:[si+5],bx        ;存储当前计数器的值
                mov ah,03h              ;读取光标信息
                mov bh,00h              ;第0页
                int 10h                 ;CH:光标的起始行、CL:光标的终止行、DH:行(Y坐标)，DL:列(X坐标)
                dec dl                  ;列（X坐标）向前一位				
				mov ah,02h       		;置光标
				mov bh,00h       		;第0页
				; mov dh,00h       		;dh:行号
				; mov dl,00h       		;dl:列号
				int 10h				
				mov ah,09h          	;在光标位置显示字符
				; mov al,31h          	;字符 ASCII
				mov bl,00000111b    	;颜色：[7]:闪烁、[6,5,4]:背景R,G,B、[3]:高亮、[2,1,0]:前景R,G,B
				mov bh,00h          	;第0页
				mov cx,01h          	;字符重复个数
				int 10h				
				jmp number_Scanf
			number_0_9:
				mov bl,0fh		   		;设置颜色
				call print_char_color   ;回显,将AL中的字符打印在屏幕上
                mov si,offset temp_data
                mov bx,ds:[si+5]        ;获取计数器
                sub al,30h              ;将按键值的ASCII码的0~9，转为数字0~9
                mov ds:[si+bx+12],al    ;键值依次存入[12]~[15]
				add bx,1				;计数器加一
				cmp bx,4                ;4个数字已经输入完了
                je  number_handle       ;转去处理
                mov ds:[si+5],bx        ;存储当前计数器的值				
				jmp number_Scanf		;继续扫描数字
			number_a_f:
				mov bl,0fh				;设置颜色
				call print_char_color   ;回显,将AL中的字符打印在屏幕上
                mov si,offset temp_data
                mov bx,ds:[si+5]        ;获取计数器
                sub al,57h              ;将按键值的ASCII码的a~f，转为数字a~f
                mov ds:[si+bx+12],al    ;键值依次存入[12]~[15]
				add bx,1				;计数器加一
			    cmp bx,4                ;4个数字已经输入完了
                je  number_handle       ;转去处理
                mov ds:[si+5],bx        ;存储当前计数器的值				
				jmp number_Scanf		;继续扫描数字			
			number_handle:
				mov si,offset temp_data
                mov bp,0
                mov ds:[si+5],bp        ;计数器清零
                mov ax,0
            num_shl:
                mov ah,ds:[si+12]    	;依次取数据放入Dl中，<<4、再相加
                and ah,0fh              ;0~f,将高4位清零
				mov cl,4				
				shl ah,cl				;左移4位，低位补0
			
				mov dh,ds:[si+13]
                and dh,0fh              ;将高4位清零
                add ah,dh               ;将DX中的值放入AX，AX用来存放处理的数值
                
				mov al,ds:[si+14]    	;依次取数据放入Dl中，<<4、再相加
                and al,0fh              ;0~f,将高4位清零
				mov cl,4				
				shl al,cl				;左移4位，低位补0
				
				mov dl,ds:[si+15]
                and dl,0fh              ;将高4位清零
                add al,dl 
				
				mov ds:[si+16],ax		;将处理好的数据存放到[16~17]
			ret
		
		
;;---------------------------------------------------
;;  ---打印函数---
;;para:	null
;;ret：	null
;;---------------------------------------------------		
	
	print_data:
				call clearscreen		;清屏
				
				mov si,offset temp_startaddress
				mov ax,ds:[si]        	;取出startAddress
				mov es,ax
				;mov ax,ds:[si]        	;取出startAddress
				mov ax,0000h          	;
				mov di,ax
				
				mov si,offset temp_data	
				mov bp,0
				mov ds:[si+5],bp		;计数器初始化
				mov ds:[si+7],bp		;借助BP,清空行计数和列计数
				
				call print_startaddress	;打印第一行的startaddress:行数
		show_loop:
				mov al,es:[di+bp]
				mov si,offset temp_data	
				mov ds:[si],al
				call print8
				call print_blank
				
				inc bp
				mov ds:[si+5],bp
				
				mov si,offset temp_length	;判断是否已打印完
				mov ax,ds:[si]				
				cmp bp,ax
				ja  show_end				;打印完
				
				mov si,offset temp_data		;
				mov dx,ds:[si+7]			;行计数，列计数
				inc dl
				mov ds:[si+7],dx

				cmp dl,15					;列计数，16个一行
				ja show_nextline			;换行打印
				
				jmp show_loop				;当前行继续打印
				
		show_nextline:
				call print_nextline
				
				mov si,offset temp_data	
				mov dx,ds:[si+7]
				mov dl,0
				add dh,1
				mov ds:[si+7],dx
				
				call print_startaddress
				jmp show_loop
				
		show_end:
				mov si,offset temp_data		;计数器清空
				mov dx,0
				mov ds:[si+5],dx
				mov ds:[si+7],dx

			ret
			
	print_string1:                          ;打印提示字符串1
				mov si,offset temp_string1
				mov bl,0fh
				call print_string
			ret
	print_string2:                          ;打印提示字符串2
				mov si,offset temp_string2
				mov bl,0fh
				call print_string
			ret
	print_string3:                           ;打印提示字符串3
				mov si,offset temp_string3
				mov bl,0fh
				call print_string
			ret
	print_stringE4\
				mov si,offset temp_error
				mov bl,0fh
				call print_string
			ret	
	print_startaddress:						;print32
				mov si,offset temp_startaddress
				mov ax,ds:[si]
				mov cl,4
				shr ah,cl
				and ah,0fh
				mov al,ah
				call number_conver_ascii		;数值转换成ASCII码
				mov bl,07h
				call print_char_color
				
				mov ax,ds:[si]
				and ah,0fh
				mov al,ah
				call number_conver_ascii		;数值转换成ASCII码
				mov bl,07h
				call print_char_color
				
				mov ax,ds:[si]
				mov cl,4
				shr al,cl
				and al,0fh
				call number_conver_ascii		;数值转换成ASCII码
				mov bl,07h
				call print_char_color
				
				mov ax,ds:[si]
				and al,0fh
				call number_conver_ascii		;数值转换成ASCII码
				mov bl,07h
				call print_char_color
				
				call print_colon				;打印':'
				
				mov si,offset temp_data			;打印行数
				mov dx,ds:[si+7]
				and dh,0fh
				mov al,dh
				call number_conver_ascii		;数值转换成ASCII码
				mov bl,0fh
				call print_char_color
				
				mov al,'0'
				mov bl,0fh
				call print_char_color
				call print_blank

			ret
;;---------------------------------------------------
;;  ---打印一个字符串---
;;para: si:字符串的首地址,
;;		bl：输出字符颜色 
;;ret：
;;---------------------------------------------------
	print_string:
                mov cl,ds:[si]
                mov ch,0
                jcxz print_string_ret   ;条件转移 当cx=0 跳转             
                mov al,ds:[si]
                ; mov bl,00001111b    ;颜色：[7]:闪烁、[6,5,4]:背景R,G,B、[3]:高亮、[2,1,0]:前景R,G,B
                call print_char_color
                inc si
                jmp print_string
        print_string_ret:
			ret
	print8:
				mov si,offset temp_data			;先打印高4位
				mov al,ds:[si]
				mov cl,4
				shr al,cl
				and al,0fh
				call number_conver_ascii 		;数值转换成ASCII码
				mov bl,0fh
				call print_char_color
				
				mov al,ds:[si]					;再打印低4位
				and al,0fh
				call number_conver_ascii		;数值转换成ASCII码
				mov bl,0fh
				call print_char_color
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
	; print16:
				; mov si,offset temp_data
				; mov al,ds:[si]
				; mov bl,0fh
				; call print_char_color
			; ret
	print_blank:
			mov al,' '
			mov bl,0fh
			call print_char_color
		ret	
	print_colon:
			mov al,':'
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

            mov ah,09h          ;在光标位置显示字符
			; mov al,31h          ;字符 ASCII
			; mov bl,0fb    ;颜色：[7]:闪烁、[6,5,4]:背景R,G,B、[3]:高亮、[2,1,0]:前景R,G,B
			mov bh,00h          ;第0页
			mov cx,01h          ;字符重复个数
			int 10h

			mov ah,03h       ;读取光标信息
			mov bh,00h       ;第0页
			int 10h          ;CH:光标的起始行、CL:光标的终止行、DH:行(Y坐标)，DL:列(X坐标)

			add dl,1         ;将光标向后移动一列

			mov ah,02h       ;置光标
			mov bh,00h       ;第0页
			; mov dh,00h       ;dh:行号
			; mov dl,00h       ;dl:列号
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
			; mov dh,00h       ;dh:行号
			; mov dl,00h       ;dl:列号
			int 10h
		ret
;;---------------------------------------------------
code ends
    end start
