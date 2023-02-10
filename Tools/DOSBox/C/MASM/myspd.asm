;;---------------------------------------------------
;;***************************************************
;用SMBus访问方式通过汇编写：
;	根据SMBus的Bus、Device、Function号，使用IO访问SPD，并打印128字节
;
; SMBus总线号 bus (8bit)：00h

; SMBus设备号 device (5bit):1Fh

; SMBus功能号 Function (3bit):03h
; 待改进：
		
;;***************************************************
;;---------------------------------------------------
.586p
assume cs:spd_code, ds:spd_data, ss:spd_stack

spd_stack segment use16
    db    128   dup(0)
spd_stack ends

spd_data segment use16
    spd_temp		        db  20  dup(0)	;[0]:spd_Bus,[1]spd_Device,[2]spd_Function
	spd_SBA_data			db	4	dup(0)
	spd_SMBus_Bus			db	00h
	spd_SMBus_Device		db	1fh
	spd_SMBus_Function		db	03h
	spd_SMBus_read_data		db  00h
	spd_SMBus_read_cnt		dw	0000h				;存放SPD读取到的128个bytes
	spd_SMBus_print_cnt		db  00h
	;
	spd_Slave_Device		dw	0000h
	;
	spd_SMBus_note			db	'SMBus: Bus: 00h, Device: 1Fh, Function: 03h',0
	spd_Slave_select		db	'Select the slave device(A0/A4/A8/AA/AC,eg:A0):A',0
	spd_error          		db  'ERROR! Please enter the right number',0
	;
    spd_string_tail        	db  'press Enter to continue    press ESC to Exit',0
spd_data ends
;;---------------------------------------------------
;;	bits:	31~28 27~24 23~20 19~16 15~12 11~8 7~4 3~0
;;	[31]: 1bit,1=enble,0=disable
;;	[30~24]: 7bit,reserved,保留，未使用
;;	[15~11]: 5bits,device number
;;	[10~8]: 3bit,function number
;;	[7~2]: 6bit,register number
;;	[1~0]: 2bit,必须是00
;;  register number:[7~0],最低两位为0	
;;---------------------------------------------------
;;***************************************************
;
;   ***	主函数	***
;
;;***************************************************
;;---------------------------------------------------

spd_code segment use16
    spd_start:
			mov ax,spd_stack
            mov ss,ax
            mov sp,128

            mov ax,spd_data
            mov ds,ax
			
			; mov spd_SMBus_Bus,00h
			; mov spd_SMBus_Device,1fh
			; mov spd_SMBus_Function,03h
			
		
			call spd_clearscreen			;清屏
		;;----打印 SMBus 信息	
			call spd_print_SMBus_note		;打印SMBus的相关信息
			call spd_print_nextline
			call spd_print_Slave_select		;打印slave设备的选择
			call spd_Keyscanf_Slave_select
			call spd_print_nextline			;
		;;----打印 spd 信息	
			call spd_get_SMA				;SMB Base Address(SBA)-Offset 20h
			call spd_print_data				;打印读取到的数据
			call spd_print_nextline			;
			call spd_print_string_tail		;打完数据，打印tail

		;;------------
		spd_KeyScanf:
			mov ah,01h			;使用通道01h检测是否有按键按下
			int 16h
			jz	spd_KeyScanf
			
			mov  ah,00h             ;有按键按下，读取按键
			int  16h
			
			cmp al,0dh			;Enter(1c0dh)
			je	spd_start
			cmp al,1bh			;Esc(011bh)
			je	spd_exit
			jne	spd_KeyScanf

	spd_exit:
			call spd_clearscreen
            mov ax,4c00h
            int 21h
;;---------------------------------------------------
;;*************子函数********************************		
;;---------------------------------------------------
;;  ---清屏函数---
;;para:	null
;;ret：	null
;;---------------------------------------------------
spd_clearscreen:
			mov ah,00h
			mov al,03h
			int 10h
		ret
;;---------------------------------------------------
;;  --- 打印相关函数 ---
;;para:	bl:颜色
;;		si:字符串的偏移地址
;;ret：	null
;;---------------------------------------------------	
spd_print_SMBus_note:
			mov si,offset spd_SMBus_note
			mov bl,00001010b
			call spd_print_string		
		ret
spd_print_Slave_select:
			mov si,offset spd_Slave_select
			mov bl,00001010b
			call spd_print_string
			; call spd_print_nextline	
		ret
spd_print_error_string:
			mov si,offset spd_error
			mov bl,00000100b
			call spd_print_string
		ret

spd_get_SMA:
			mov eax,80000000h
			;
			mov ebx,0
			mov bl,00h			;Bus
			mov cl,16
			shl ebx,cl
			add eax,ebx
			;
			mov ebx,0
			mov bl,1fh			;device
			mov cl,11
			shl ebx,cl
			add eax,ebx
			;
			mov ebx,0
			mov bl,03h			;function
			mov cl,8
			shl ebx,cl
			add eax,ebx
			;
			mov ebx,0
			mov bl,20h			;SMB Base Address(SBA)-Offset 20h
			add eax,ebx
			;
			mov dx,0cf8h
			; mov eax,80001000h
			out dx,eax
			;
			mov dx,0cfch
			in  eax,dx
			;
			
			; mov eax,spd_SBA_data			;[31-16]:Reserved,[15-5]:Base Address,[4-1]:Reserved,[0]:IO Space Indicator
			and ax,0fffeh
			mov si,offset spd_SBA_data
			mov ds:[si],ax			;处理好后存放在spd_SBA_data
			
		; ;;------debug			
				; mov bp,ax
				; mov ax,bp
				; mov cl,4
				; shr ah,cl
				; and ah,0fh
				; mov al,ah
				; add al,30h
				; mov bl,0fh
				; call spd_print_char_color
				
				; mov ax,bp
				; and ah,0fh
				; mov al,ah
				; add al,30h
				; mov bl,0fh
				; call spd_print_char_color
				
				
				; mov ax,bp
				; mov cl,4
				; shr al,cl
				; and al,0fh
				
				; add al,30h
				; mov bl,0fh
				; call spd_print_char_color
				
				; mov ax,bp
				; and al,0fh
				
				; add al,30h
				; mov bl,0fh
				; call spd_print_char_color
				
				; call spd_print_nextline
	; ;;------debug	

		ret
		
spd_print_data:
			; call spd_print_nextline			;
			;;;-----标准套路-----
			call spd_delay
			
			mov si,offset spd_SBA_data
			mov dx,ds:[si]					;SMBus Base Address
			add dx,00h						;status register
			mov al,0feh
			out dx,al
			call spd_delay					;延时,留出时间让SMBus写数据
			;
			mov si,offset spd_SBA_data
			mov dx,ds:[si]					;SMBus Base Address
			add dx,04h						;slave address register
			mov si,offset spd_Slave_Device
			mov ax,ds:[si]					;从设备地址：A0/A4/A8/AA/AC
			or  al,01h						;[0]:0=write,1=read
			out dx,al
			call spd_delay					;延时,留出时间让SMBus写数据
			;
			mov si,offset spd_SBA_data
			mov dx,ds:[si]					;SMBus Base Address
			add dx,03h						;command register
			mov si,offset spd_SMBus_read_cnt
			mov al,ds:[si]					;寄存器索引
			out dx,al
			inc al
			mov ds:[si],al
			call spd_delay					;延时,留出时间让SMBus写数据
			;
			mov si,offset spd_SBA_data
			mov dx,ds:[si]					;SMBus Base Address
			add dx,02h						;control register
			mov al,48h						;设置读写模式:字节(48h),字(4ch),块(54h) 
			out dx,al
			call spd_delay
			call spd_delay
			;
			mov si,offset spd_SBA_data
			mov dx,ds:[si]					;SMBus Base Address
			add dx,00h						;control register
			in  al,dx						;回读数据
			cmp al,04h						;读取出错,设备不存在
			jz  spd_read_error
			;
			mov si,offset spd_SBA_data
			mov dx,ds:[si]					;SMBus Base Address
			add dx,05h						;data0 register
			in  al,dx						;回读数据
			;
			mov si,offset spd_SMBus_read_data
			mov ds:[si],al					;暂存al到spd_SMBus_read_data中
			call spd_print_hex
			;
			cmp spd_SMBus_read_cnt,200
			jb  spd_print_data				;继续读取
			;
			mov spd_SMBus_read_cnt,0
			mov si,offset spd_SMBus_print_cnt
			mov cl,0
			mov ds:[si],cl
			
			jmp spd_print_data_ret
	spd_read_error:
			call spd_print_nextline			;输入有误,打印警告信息
			call spd_print_error_string
			call spd_print_nextline
			call spd_print_Slave_select
			call spd_Keyscanf_Slave_select
			call spd_print_nextline
			call spd_get_SMA
			jmp  spd_print_data
	spd_print_data_ret:
		ret

spd_print_string_tail:
			mov si,offset spd_string_tail
			mov bl,00000111b
			call spd_print_string
		ret
			
spd_print_hex:
			mov si,offset spd_SMBus_read_data
			mov al,ds:[si]
			mov cl,4							;先打印高四位
			shr al,cl
			and al,0fh
			call spd_number_conver_ascii 		;数值转换成ASCII码
			mov bl,0fh
			call spd_print_char_color
			;
			mov al,ds:[si]						;再打印低4位
			and al,0fh
			call spd_number_conver_ascii		;数值转换成ASCII码
			mov bl,0fh
			call spd_print_char_color
			;
			call spd_print_blank
			;
			mov si,offset spd_SMBus_print_cnt
			mov cl,ds:[si]
			inc cl
			mov ds:[si],cl
			cmp cl,16
			jb  spd_print_hex_ret
			mov cl,0
			mov ds:[si],cl
			call spd_print_nextline
	spd_print_hex_ret:		
		ret
spd_number_conver_ascii:
			cmp al,10
			jb  spd_conver0_9
			jnb spd_convera_f
			
		spd_conver0_9:
			add al,30h
			jmp spd_conver_end
		spd_convera_f:
			; add al,57h					;10~15 → a~f
			add al,37h						;10~15 → A~F
			jmp spd_conver_end
		spd_conver_end:
		ret
; ;--------------------------------------		
	spd_print_string:
                mov cl,ds:[si]
                mov ch,0
                jcxz spd_print_string_ret
                
                mov al,ds:[si]
                ; mov bl,00001111b    ;颜色：[7]:闪烁、[6,5,4]:背景R,G,B、[3]:高亮、[2,1,0]:前景R,G,B
                call spd_print_char_color

                inc si
                jmp spd_print_string
        spd_print_string_ret:
			ret	
	spd_print_blank:
			mov al,' '
			mov bl,0fh
			call spd_print_char_color
		ret	
	spd_print_colon:
			mov al,':'
			mov bl,0fh
			call spd_print_char_color
		ret
;;---------------------------------------------------
;;		按键相关函数
;;
;;---------------------------------------------------
	spd_Keyscanf_Slave_select:
				mov si,offset spd_temp
                mov bx,0
                mov ds:[si+5],bx        	;计数器清零
				;
				call spd_KeyScanf_number
				mov si,offset spd_temp		;取出处理好的键值
				mov ax,ds:[si+16]
				;
				cmp ax,00h
				je spd_Keyscanf_ok
				cmp ax,04h
				je spd_Keyscanf_ok
				cmp ax,08h
				je spd_Keyscanf_ok
				cmp ax,0ah
				je spd_Keyscanf_ok
				cmp ax,0ch
				je spd_Keyscanf_ok
				;
				call spd_print_nextline			;输入有误,打印警告信息
				call spd_print_error_string
				call spd_print_nextline
				call spd_print_Slave_select
				;
				jmp  spd_Keyscanf_Slave_select	;重新输入
			spd_Keyscanf_ok:
				add ax,0a0h
				mov si,offset spd_Slave_Device
				mov ds:[si],ax
			ret
				
	
	spd_KeyScanf_number:
				mov ah,01h				;使用通道01h检测是否有按键按下
				int 16h
				jz	spd_KeyScanf_number
				
				mov  ah,00h             ;有按键按下，读取按键
				int  16h
				
				;功能键检测
				cmp al,1bh				;Esc(011bh)
				je	spd_exit
				cmp al,0dh				;Enter
				je  spd_Key_Enter
                ;;数字按键检测、判断输入是否为合法数字（0~9、a~f）
                cmp al,30h						;数字0
                jb  spd_KeyScanf_number	    	;AL<0,错误,重输
                cmp al,39h              		;数字9
                jna spd_number_0_9  			;0<=AL<=9,正确
				cmp al,41h						;数字A
				jb  spd_KeyScanf_number			;9<AL<A,错误,重输
				cmp al,46h						;数字F
				jna spd_number_a_f				;A<=AL<=F,正确
                cmp al,61h              		;数字a
                jb  spd_KeyScanf_number        	;F<AL<a,错误,重输
                cmp al,66h              		;数字f
                jna spd_number_a_f   			;a<=AL<=f,正确
                ja  spd_KeyScanf_number        	;f<=AL,错误,重输		
	;;-------------------
			spd_Key_Enter:
				jmp spd_number_handle
	;;-------------------
			spd_number_0_9:
				mov bl,00001010b		;设置颜色为：高亮黑底绿色
				call spd_print_char_color   ;回显,将AL中的字符打印在屏幕上

                mov si,offset spd_temp
                mov bx,ds:[si+5]        ;获取计数器

                sub al,30h              ;将按键值的ASCII码的0~9，转为数字0~9
				jmp spd_Number_end
	;;-------------------		
			spd_number_a_f:
				mov bl,00001010b		;设置颜色为：高亮黑底绿色
				call spd_print_char_color   ;回显,将AL中的字符打印在屏幕上

                mov si,offset spd_temp
                mov bx,ds:[si+5]        ;获取计数器
				
				cmp al,61h				;数字a，用来区分大小写
				jb  spd_capital_A_F
				
				sub al,57h              ;将按键值的ASCII码的a~f，转为数字a~f
				jmp spd_Number_end
				
			spd_capital_A_F:
				sub al,37h              ;将按键值的ASCII码的A~F，转为数字A~F
				
            spd_Number_end:    
                mov ds:[si+bx+12],al    ;键值依次存入[12]~[15]
				add bx,1				;计数器加一
				mov ds:[si+5],bx        ;存储当前计数器的值
			    cmp bx,2                ;2个数字已经输入完了
                je  spd_number_handle       ;转去处理

				jmp spd_KeyScanf_number		;继续扫描数字
	;;-------------------		
			spd_number_handle:
				mov si,offset spd_temp
                mov bp,ds:[si+5]        ;取计数器，获取输入字符个数
				mov ax,4
				sub ax,bp				;计算缺几个字符（max 4）
				mov bp,ax				;

                mov ax,0
            spd_num_shl:
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
			
			spd_num_shr:	
				cmp bp,0
				je spd_num_shr_end
				
				mov cl,4
				shr ax,cl
				dec bp
				jmp spd_num_shr
				
			spd_num_shr_end:
				mov ds:[si+5],bp		;此时bp=0，清空计数器
				mov ds:[si+16],ax		;将处理好的数据存放到[16~17]
				
	; ;;------debug			
				; mov al,ds:[si+12]
				; add al,30h
				; mov bl,07h
				; call spd_print_char_color
				; mov al,ds:[si+13]
				; add al,30h
				; mov bl,07h
				; call spd_print_char_color
				; mov al,ds:[si+14]
				; add al,30h
				; mov bl,07h
				; call spd_print_char_color
				; mov al,ds:[si+15]
				; add al,30h
				; mov bl,07h
				; call spd_print_char_color
	; ;;------debug			
				; mov bp,ds:[si+16]
				; mov ax,bp
				; mov cl,4
				; shr ah,cl
				; and ah,0fh
				; mov al,ah
				; add al,30h
				; mov bl,0fh
				; call spd_print_char_color
				
				; mov ax,bp
				; and ah,0fh
				; mov al,ah
				; add al,30h
				; mov bl,0fh
				; call spd_print_char_color
				
				
				; mov ax,bp
				; mov cl,4
				; shr al,cl
				; and al,0fh
				
				; add al,30h
				; mov bl,0fh
				; call spd_print_char_color
				
				; mov ax,bp
				; and al,0fh
				
				; add al,30h
				; mov bl,0fh
				; call spd_print_char_color
	; ;;------debug			

			ret    
;;---------------------------------------------------
;;  ---打印一个彩色字符---
;;para：al:输出字符，bl：输出字符颜色
;;ret：DH:行(Y坐标)，DL:列(X坐标)
;;---------------------------------------------------
    spd_print_char_color:
            mov ah,03h       ;读取光标信息
            mov bh,00h       ;第0页
            int 10h          ;CH:光标的起始行、CL:光标的终止行、DH:行(Y坐标)，DL:列(X坐标)

            mov ah,09h          ;在光标位置显示字符
			; mov al,31h          ;字符 ASCII
			; mov bl,00000100b    ;颜色：[7]:闪烁、[6,5,4]:背景R,G,B、[3]:高亮、[2,1,0]:前景R,G,B
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
	spd_print_nextline:
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
;;  ---延时---
;;para:	null
;;ret：	null
;;---------------------------------------------------	
spd_delay:
			push cx
			mov ecx,0ffffh	;cpu主频2.3GHz, 2^28
			;
		spd_delay_loop:
			nop
			dec ecx
			cmp ecx,0
			ja  spd_delay_loop
			;
			pop cx
		ret
;;---------------------------------------------------
spd_code ends
    end spd_start