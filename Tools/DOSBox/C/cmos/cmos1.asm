
; CMOS
;;----------------------------------------------
.386
assume cs:cmos_code,ds:cmos_data,ss:cmos_stack

; public cmos_start

cmos_stack segment use16
	db	64	dup(0)
cmos_stack ends

cmos_data segment use16
	cmos_variable	db	10		dup(0)	
	cmos_Blank		db	'    ',		'$'
	cmos_Slash		db	'/',		'$'
	cmos_Colon		db	':',		'$'
	coms_tail		db	'press ESC to quit','$'
	cmos_nextline	db	0ah,0dh,	'$'
	cmos_Monday		db	'Monday',	'$'
	cmos_Tuesday	db	'Tuesday',	'$'
	cmos_Wednesday	db	'Wednesday','$'
	cmos_Thursday	db	'Thursday',	'$'
	cmos_Friday		db	'Friday',	'$'
	cmos_Saturday	db	'Saturday',	'$'
	cmos_Sunday		db	'Sunday',	'$'
cmos_data ends
cmos_code segment use16
	cmos_start:
		mov ax,cmos_stack
		mov ss,ax
		mov sp,64	
		mov ax,cmos_data
		mov ds,ax	
		mov ax,0b800h
		mov es,ax
		mov di,0
	call cmos_clearScreen		;第一次打印，清屏	
	cmos_printCmos:
		call cmos_printYear			;打印年份
		call cmos_printSlash		;打印'/'
		call cmos_printMonth		;打印月份
		call cmos_printSlash		;打印'/'
		call cmos_printDay			;打印日份
		call cmos_printBlank		;打印空格
		call cmos_printHour			;打印时钟
		call cmos_printColon		;打印':'
		call cmos_printMinute		;打印分钟
		call cmos_printColon		;打印':'
		call cmos_printSecond		;打印秒钟
		call cmos_printBlank		;打印空格
		call cmos_printWeek			;打印星期
		call cmos_printBlank		;打印空格
		call cmos_printNextLine		;打印换行
		;call cmos_printTail			;打印标尾	
	cmos_CheckSecond:
		mov al,00h
		out 70h,al
		in 	al,71h	
		push si
		mov si,offset cmos_variable
		cmp al,[si]
		pop si
		je	cmos_CheckSecond
	MoveCursor:						;移动光标到起始位置
		mov bh,0					;第 0 页
		mov dh,00h            		;DH ＝ 行(X坐标)
		mov dl,00h 					;DL ＝ 列(Y坐标)
		mov ah,02h              	;置光标
		int 10h 		
		mov di,0
		jmp	cmos_printCmos;循环不断读取 时间显示
	cmos_exit:
		call cmos_clearScreen	
		mov ax,4c00h
		int 21h
;;----------------------------------------------		
;;----------------------------------------------
	cmos_clearScreen:  ;清屏 
		mov ah,0
		mov al,3
		int 10h		
	ret
;;----------------------------------------------
	cmos_printBlank:					;打印空格
		mov dx,offset cmos_Blank
		mov ah,09h						;显示字符串，'$'结尾
		int 21h
	ret
;;----------------------------------------------
	cmos_printYear:						;打印年份
		mov al,09h
		out 70h,al
		in 	al,71h
		call cmos_BCD_conver
	ret
;;----------------------------------------------
	cmos_printSlash:					;打印'/'
		mov dx,offset cmos_Slash
		mov ah,09h						;显示字符串，'$'结尾
		int 21h
		
	ret
;;----------------------------------------------
	cmos_printMonth:					;打印月份
		mov al,08h
		out 70h,al
		in 	al,71h
		
		call cmos_BCD_conver
	ret
;;----------------------------------------------
	cmos_printDay:						;打印日份
		mov al,07h
		out 70h,al
		in 	al,71h		
		call cmos_BCD_conver
	ret
;;----------------------------------------------
	cmos_printHour:						;打印时钟
		mov al,04h
		out 70h,al
		in 	al,71h	
		call cmos_BCD_conver
	ret
;;----------------------------------------------
	cmos_printColon:					;打印':'
		mov dx,offset cmos_Colon
		mov ah,09h						;显示字符串，'$'结尾
		int 21h
	ret
;;----------------------------------------------
	cmos_printMinute:					;打印分钟
		mov al,02h
		out 70h,al
		in 	al,71h		
		call cmos_BCD_conver
	ret
;;----------------------------------------------
	cmos_printSecond:					;打印秒钟
		mov al,00h
		out 70h,al
		in 	al,71h	
		; mov ch,al						;ch存储此次秒钟
		push si
		mov si,offset cmos_variable
		mov [si],al
		pop si	
		call cmos_BCD_conver
	ret
;;----------------------------------------------
	cmos_printWeek:						;打印星期
		mov al,06h
		out 70h,al
		in 	al,71h	
		cmp al,2
		je	cmos_printMonday	
		cmp al,3
		je 	cmos_printTuesday	
		cmp al,4
		je	cmos_printWednesday	
		cmp al,5
		je	cmos_printThursday	
		cmp al,6
		je	cmos_printFriday	
		cmp al,7
		je	cmos_printSaturday	
		cmp al,8
		je	cmos_printSunday 	
	cmos_printMonday:
		mov dx,offset cmos_Monday
		mov ah,09h						;显示字符串，'$'结尾
		int 21h
	ret
	cmos_printTuesday:
		mov dx,offset cmos_Tuesday
		mov ah,09h						;显示字符串，'$'结尾
		int 21h
	ret
	cmos_printWednesday:
		mov dx,offset cmos_Wednesday
		mov ah,09h						;显示字符串，'$'结尾
		int 21h
	ret
	cmos_printThursday:
		mov dx,offset cmos_Thursday
		mov ah,09h						;显示字符串，'$'结尾
		int 21h
	ret
	cmos_printFriday:
		mov dx,offset cmos_Friday
		mov ah,09h						;显示字符串，'$'结尾
		int 21h
	ret
	cmos_printSaturday:
		mov dx,offset cmos_Saturday
		mov ah,09h						;显示字符串，'$'结尾
		int 21h
	ret
	cmos_printSunday:
		mov dx,offset cmos_Sunday
		mov ah,09h						;显示字符串，'$'结尾
		int 21h
	ret
;;----------------------------------------------
	cmos_BCD_conver:	
		mov ah,al			;读取的al 在ax中显示为 0021变为2121
		mov cl,4     	 
		shr ah,cl	 		;高位左移四位 相当于取十位数 02 01
		and al,0fh 	 		;低位和of相与取年份个位
		add ah,30h			;AX=3231H  0X32 代表字符2 0x31为0 "21"
		add al,30h
		mov si,offset cmos_variable
		mov [si+2],ax   ;  2 1
		mov si,offset cmos_variable 
		mov al,[si+3]   ;1
		call print_char_Color
		mov si,offset cmos_variable
		mov al,[si+2]   ;2
		call print_char_Color
		
	ret
;;----------------------------------------------
	cmos_printTail:
		mov dx,offset coms_tail
		mov ah,09h						;显示字符串，'$'结尾
		int 21h
		 
	ret
;;----------------------------------------------
	cmos_printNextLine:
		mov dx,offset cmos_nextline
		mov ah,09h						;显示字符串，'$'结尾 换行字符串oah odh
		int 21h
	ret
;;----------------------------------------------
	print_char_Color:
		mov ah,09h          ;在光标位置显示字符
		mov bl,0FH          ;颜色：[7]:闪烁、[6,5,4]:背景R,G,B、[3]:高亮、[2,1,0]:前景R,G,B
		mov bh,00h          ;第0页
		mov cx,01h          ;字符重复个数
		int 10h
		mov ah,03h       ;读取光标信息
		mov bh,00h       ;第0页
		int 10h          ;CH:光标的起始行、CL:光标的终止行、DH:行(Y坐标)，DL:列(X坐标)
		add dl,1         ;将光标向后移动一列
		mov ah,02h       ;置光标
		mov bh,00h       ;第0页
		mov dh,00h       ;dh:行号
		; mov dl,00h     ;dl:列号
		int 10h	
	ret
;;----------------------------------------------

cmos_code ends
end cmos_start