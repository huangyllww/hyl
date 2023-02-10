.386
assume cs:code, ds:data, ss:stack
stack segment use16
    db  256 dup(0)
stack ends
data segment use16
  	temp_data           db  20   dup(0)
	temp_startaddress	db	2	dup(0)
	temp_length			db	2	dup(0) 
	str_cpuid    	db '1.cpuid', 0
	str_memorry		db '2.memorry',0
	str_sio			db '3.cmos',0
	str_smbus 		db '4.pcie',0
	str_pcie        db '5.sio',0
	str_exit        db '6.smbus',0
	str_choose      db 'Please choose the option',0
	str_return      db 'enter or esc',20 dup (0)	
;;---------------------------------------------------	
;cpuid  数据段
;;---------------------------------------------------	
	Vendor 			db 'Vendor ID:',12 dup (0)	
	Information		db  16 dup (0)
	Family			db 'Family:',2 dup (0)
	Model			db 'Model:',2 dup (0)
	Step			db 'Stepping:',2 dup(0)
;;---------------------------------------------------	
; memorry  数据段
;;---------------------------------------------------	
    temp_string1        db  'Please enter the start address(eg:1100) :',0
    temp_string2        db  'Please enter the length(eg:0000-02ff,eg:0022) :',0
    temp_string3        db  'press Enter to continue    press ESC to exit',0
;;---------------------------------------------------	
;cmos 	数据段
;;---------------------------------------------------	
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
;;---------------------------------------------------	
;sio  	数据段
;;---------------------------------------------------		
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
;;---------------------------------------------------	
;smbus数据段
;;---------------------------------------------------	
 	busnum dw 0000h  ;总线号0 - 00FFh
    devnum dw 001fh  ;设备号0 001Fh
    funnum dw 0003h  ;功能号0-  - 0003h
    regnum dw 0020h  ;寄存器0 - 00FFh
    sm_base   dw 0000h ;存放smbus 基地址
    sm_devnum dw 0000h   ;a0/a2/a4/a6
    sm_regnum dw 0000h   ;
    bufferlen = 128
    buffer db bufferlen dup(0) ;存放spd  128-byte的信息
    buffer1 db 'smbus base address :',2 dup(0)
    buffer2 db 'end :',2 dup(0)
    smbus_base  db 'smbus base address is:',2 dup(0) 
;;---------------------------------------------------	
;pcie数据段
;;---------------------------------------------------	
	temp_bus	        db	4	dup(0)
	temp_device	        db	4	dup(0)
	temp_function	    db	4	dup(0)
    temp_error          db  'ERROR! Please enter the right number(max 0x00ff)!',0
    pcie_string1        db  'Please enter the bus(0x0000-0x00ff,eg:0008):',0
    pcie_string2        db  'Please enter the device(0x0000-0x00ff,eg:0008) :',0
    pcie_string3        db  'press Enter to continue    press ESC to exit',0
	pcie_string4        db  'Please enter the function(0000-0008,eg:0005) :',0
	pcie_string5        db  'Subsystem ID:',0
	pcie_string6        db  'Subsystemvendor ID:',0
	pcie_string7        db  'pcie address:',0
;;---------------------------------------------------
;   ***总函数	***
;;---------------------------------------------------
code segment use16
    start:
            mov ax,stack  
            mov ss,ax
            mov ax,data
            mov ds,ax
			call clearscreen
         	call print_str_cpuid         
			call print_nextline
			call print_str_memorry
			call print_nextline
			call print_str_sio
			call print_nextline
			call print_str_smbus
			call print_nextline
			call print_str_pcie
			call print_nextline
			call print_str_exit
			call print_nextline
			call print_str_choose     ;首先输出界面包括6个选项
			call KeyScanf			  ;键盘输入扫描		
			call exit	
		KeyScanf:               ;扫描输入字符进入对应子程序执行
			mov ah,01h			;使用通道01h检测是否有按键按下,返回参数ZF=0时AL=字符
			int 16h;			;ZF=1时，缓冲区为空
			jz	KeyScanf	 	;根据zf 跳转 ，为0跳转接收到键盘输入
			mov  ah,00h         ;有按键按下，读取按键
			int  16h			;读取缓冲区ascall给al
			cmp al,0dh			;Enter(1c0dh)；输入enter 程序重新开始运行
			je	start            
			cmp al,1bh			;Esc(011bh) 输入exit 结束程序
			je	exit
			cmp al,31h			;输入数字1进入子程序cpuid
			je	cpuid_proc  
			cmp al,32h			;输入数字2进入子程序memorry
			je	memorry_proc  
			cmp al,33h			;输入数字3进入子程序cmos
			je	cmos_proc 
			cmp al,34h			;输入数字4进入子程序pcie
			je	pcie_proc  
			cmp al,35h			;输入数字5进入子程序sio
			je	sio_proc  
			cmp al,36h			;输入数字6进入子程序smbus
			je	smbus_proc   
			jne	KeyScanf
	ret
;;---------------------------------------------------
;;  ---清屏函数---
;;---------------------------------------------------
exit:                               ;终止运行退出
	mov ax,04c00h
	int 21h
ret
iodelay proc                        ;延时函数
    push cx
    mov cx,0ffffh
delay:
    loop delay
    mov cx,0ffffh
delay1:
    loop delay1
    pop cx
    ret
iodelay endp
 	   clearscreen:
		        mov ax,00h
                mov al,03h
                int 10h
			ret
;;---------------------------------------------------
;;  ---键盘读取函数---
;;---------------------------------------------------
	KeyScanf_address:
				mov si,offset temp_data
                xor bx,bx    			    ;异或清零加速计算
                mov ds:[si+5],bx            ;计数器清零				
				call KeyScanf_number        ;扫描数字输入
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
				cmp ax,02ffh					;判断长度是否合法
				jna  length_ok				;长度合法，跳转	;长度非法，重新输入 CF=1,ZF=1,跳转 不小于长度
				call print_nextline

				jmp length_scanf			
			length_ok:
				mov si,offset temp_length   ;;; lenth
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
				int  16h				;功能键检测
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
				int 10h				
				mov ah,09h          	;在光标位置显示字符
				mov bl,09h    	;颜色：[7]:闪烁、[6,5,4]:背景R,G,B、[3]:高亮、[2,1,0]:前景R,G,B
				mov bh,00h          	;第0页
				mov cx,01h          	;字符重复个数
				int 10h				
				jmp number_Scanf
			number_0_9:
				mov bl,09h		   		;设置颜色
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
				mov bl,09h				;设置颜色
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
;;  ---打印函数cpuid---
;;---------------------------------------------------					
	print_str_cpuid:                         ;打印提示字符串1.cpuid
				mov si,offset str_cpuid
				call print_string
			ret
	print_str_memorry:   
	           mov si,offset str_memorry      ;打印提示字符串2.memorry				
				call print_string
			ret
	print_str_sio: 
			 mov si,offset str_sio             ;打印提示字符串3.sio				
				call print_string
			ret
	print_str_smbus:                           ;打印提示字符串4.smbus
				mov si,offset str_smbus
				call print_string
			ret
	print_str_pcie:                            ;打印输入错误提示5.pcie
				mov si,offset str_pcie
				call print_string
			ret	
	print_str_exit:                      		;打印输入错误提示6.exit
				mov si,offset str_exit
				call print_string
			ret	
	print_str_choose:
				mov si ,offset str_choose
				call print_string
			ret	
	print_str_return:
				mov si ,offset str_return
				call print_string
			ret		
;;---------------------------------------------------
;;  ---打印函数---memorry
;;---------------------------------------------------		
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
	print_data:
				call clearscreen		;清屏				
				mov si,offset temp_startaddress
				mov ax,ds:[si]        	;取出startAddress
				mov es,ax
				xor ax,ax          	;ax清0
				xor di,di			;di清0	
				mov si,offset temp_data	
				xor bp,bp			;bp清0
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
;;---------------------------------------------------
;;  ---打印函数cmos---
;;---------------------------------------------------
cmos_CheckSecond:
		mov ah,01h			;使用通道01h检测是否有按键按下
		int 16h
		jz  cmos_key_no_press	;没有按键按下	
		jnz cmos_key_press		;有按键按下	
	cmos_key_press:			;有按键按下，使用通道00h从键盘缓冲区读取一个字符，并将其从缓冲区删除
		mov ah,00h			
		int 16h
		cmp al,1bh
		je exit
		cmp al,0dh
		je start
	
	cmos_key_no_press:		;没有按键按下，无操作
		mov al,00h
		out 70h,al
		in 	al,71h	
		push si
		mov si,offset cmos_variable
		cmp al,[si]
		pop si
		je cmos_CheckSecond		
	MoveCursor:						;移动光标到起始位置
		mov bh,0					;第 0 页
		mov dh,00h            		;DH ＝ 行(X坐标)
		mov dl,00h 					;DL ＝ 列(Y坐标)
		mov ah,02h              	;置光标
		int 10h 		
		mov di,0
		jmp	cmos_printCmos;循环不断读取 时间显示
ret

	
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
		mov ah,0bh
		mov bl,0ah;设定颜色为绿色
		int 10h
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
	cmos_printNextLine:
		mov dx,offset cmos_nextline
		mov ah,09h						;显示字符串，'$'结尾 换行字符串oah odh
		int 21h
	ret
;;---------------------------------------------------
;;  ---打印函数sio---
;;---------------------------------------------------
print_device0:                          ;打印提示字符串
				mov si,offset temp_device0
				call print_string
			ret
print_device1:                          ;打印提示字符串
				mov si,offset temp_device1
				call print_string
			ret
          	
print_device2:                          ;打印提示字符串
				mov si,offset temp_device2
				call print_string
			ret
print_device3:                          ;打印提示字符串
				mov si,offset temp_device3
				call print_string
			ret
print_device4:                          ;打印提示字符串
				mov si,offset temp_device4
				call print_string
			ret
          	
print_device5:                          ;打印提示字符串
				mov si,offset temp_device5
				call print_string
			ret

print_device6:                     ;打印提示字符串
				mov si,offset temp_device6
				call print_string
			ret
          	
print_device7:                          ;打印提示字符串
				mov si,offset temp_device7
				call print_string
			ret
		
print_device8:                          ;打印提示字符串
				mov si,offset temp_device8
				call print_string
			ret
          	
print_device9:                          ;打印提示字符串
				mov si,offset temp_device9
				call print_string
			ret
print_device10:                          ;打印提示字符串
				mov si,offset temp_device10
				call print_string
			ret
          	
print_device11:                          ;打印提示字符串
				mov si,offset temp_device11
				call print_string
			ret

print_device12:                          ;打印提示字符串
				mov si,offset temp_device12
				call print_string
			ret
print_device13:                          ;打印提示字符串
				mov si,offset temp_device13
				call print_string
			ret
          	
print_device14:                          ;打印提示字符串
				mov si,offset temp_device14
				call print_string
			ret
 
print_device15:                          ;打印提示字符串
				mov si,offset temp_device15
				call print_string
			ret
print_active:                          ;打印提示字符串
				mov si,offset string_active
				call print_string
			ret
print_inactive:
				mov si,offset string_inactive
				call print_string
			ret
;;---------------------------------------------------
;;  smbus打印函数--;
;功能: 根据eax中的地址读取pci的配置空间,并存入eax
;入口: busnum、devnum、funnum、regnum
;出口: eax
;;---------------------------------------------------
pci_read proc      ;根据busnum,devnum,funnum计算出configure addresss,使用IO读取offset regnum 的内容
    ;protect register
    push ebx  ;保存数据
    push dx
    ;clear
    xor eax,eax   ;运算最快的清零
    xor ebx,ebx
    ;enable
    add eax,1h
    shl eax,31
    ;bus number
    mov ebx,ds:[00]
    and ebx,0ffh
    shl ebx,16
    add eax,ebx
    ;device number
    xor ebx,ebx
    mov ebx,ds:[02]
    and ebx,0ffh
    shl ebx,11
    add eax,ebx
    ;function number
    xor ebx,ebx
    mov ebx,ds:[04]
    and ebx,0ffh
    shl ebx,8
    add eax,ebx
    ;register
    xor ebx,ebx
    mov ebx,ds:[06]
    and ebx,0ffh
    add eax,ebx
    ;read IO
    mov dx,0cf8h
    out dx,eax
    mov dx,0cfch
    in eax,dx
    ;resume register
    pop dx
    pop ebx
    ret
pci_read endp
;功能: 通过smbus IO registers 读取spd 信息，并存入buffer
;入口:
sm_read proc   ;读取128字节
    push dx
    push ax
    mov ax,data           ;设置目的地址(buffer 的地址)
    mov es,ax             ;段地址为数据段地址
    mov di,offset buffer  ;偏移地址为缓冲区的偏移地址
nextch:
    ;----------smbus 访问规范--------
    call iodelay
    mov dx,sm_base  ;SMbus 的基地址
    add dx,00h      ;status register
    mov al,0feh     ;
    out dx,al
    call iodelay    ;延时
    mov dx,sm_base
    add dx,04h       ;slave address register
    mov ax,sm_devnum ;从设备地址:a0/a2/a4/a6 ，sm_devnum=a0h
    or al,01h        ;末位:1 - 表示读
    out dx,al
    call iodelay
    mov dx,sm_base
    add dx,03h       ;command register
    mov ax,sm_regnum ;寄存器索引
    out dx,al
    call iodelay
    inc sm_regnum
    mov dx,sm_base
    add dx,02h       ;control register
    mov al,48h       ;设置读写模式:字节(48h)、字(4ch)、块(54h)
    out dx,al
    call iodelay
    call iodelay;
    mov dx,sm_base
    add dx,00h
    in al,dx;
    mov dx,sm_base
    add dx,05h      ;data0 register
    in al,dx        ;回读数据
    push ax
	mov bl,0fh
    call print_al
    call print_blank
    call print_blank
    pop ax
    mov es:[di],al  ;数据保存到buffer 中
    inc di
    cmp sm_regnum,07fh
    ja endsm        ;128-byte  读完
    jmp nextch
endsm:                    ;读完结束，打印buffer 后，结束
    pop ax
    pop dx
    call print_nextline
    call print_nextline
    call print_nextline
sm_read endp

print_al:                                ;打印寄存器al中的内容
    push ax
	and eax,0f0h
	mov cl,4
	shr eax,cl
	call number_conver_ascii
	MOV DL, AL 
	mov bl,0fh             
    MOV AH, 02H
	int 21h 
    pop ax
	and al,0fh
	call number_conver_ascii
	MOV DL, AL    
	mov bl,0fh                       ;输出字符
    MOV AH, 02H
 	INT 21H
ret
print_ax:                                ;打印寄存器ax中的内容
    mov ebp,eax
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
    print_smbase:
        mov si,offset buffer1
        mov bl ,0f0h
        call print_string
    ret
      print_end:
        mov si,offset buffer2
        mov bl ,0f0h
        call print_string
    ret
	print_eax:							;输出eax内容
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
;;---------------------------------------------------
;;  pcie打印函数--;
;;---------------------------------------------------

	print_pcie_string1:                          ;打印提示字符串1
				mov si,offset pcie_string1
				call print_string
			ret
	print_pcie_string2:                          ;打印提示字符串2
				mov si,offset pcie_string2
				call print_string
			ret
	print_pcie_string3:                           ;打印提示字符串3
				mov si,offset pcie_string3
				call print_string
			ret
	print_pcie_string4:                           ;打印提示字符串4
				mov si,offset pcie_string4	
				call print_string
			ret
	print_pcie_string5:
				mov si ,offset pcie_string5 		;打印提示字符串5
				call print_string
			ret
	print_pcie_string6:                           ;打印提示字符串6 pcie address
				mov si,offset pcie_string6
				call print_string
			ret
	print_pcie_string7:                           ;打印提示字符串7 pcie address
				mov si,offset pcie_string7
				call print_string
			ret
	print_stringError:                       ;打印输入错误提示
				mov si,offset temp_error
				mov bl,0fh
				call print_string
			ret		
	print_pcie_data:
		mov eax,080000000H
		mov si ,offset temp_bus
		mov ebx,0
		mov ebx,ds:[si]
		mov cl ,16
		shl ebx,cl
		add eax,ebx
		mov ebx,0
		mov ebx,ds:[si+1]
		mov cl ,16
		shl ebx,cl
		add eax,ebx

		mov si ,offset temp_device
		mov ebx,0
		mov ebx,ds:[si]
		mov cl ,11
		shl ebx,cl
		add eax,ebx
		mov ebx,0
		mov ebx,ds:[si+1]
		mov cl ,11
		shl ebx,cl
		add eax,ebx
		mov si ,offset temp_function
		mov ebx,0
		mov ebx,ds:[si]
		mov cl ,8
		shl ebx,cl
		add eax,ebx
		mov ebx,0
		mov ebx,ds:[si+1]
		mov cl ,8
		shl ebx,cl
		add eax,ebx
		add eax,02ch  ;加寄存器
		push eax      ;将地址入栈 显示后出栈保持不变后面使用该地址通过io口读取配置空间
		call print_eax
		pop eax
		mov dx,0cf8h    ;cf8h 和cfch 读取寄存器2c的内容
		OUT DX,EAX
		MOV DX,0CFCH
		IN EAX,DX
		MOV edi,EAX
		call print_pcie_string5
		and eax,0f0000000h
		mov cl,28
		shr eax,cl
		call number_conver_ascii
		MOV DL, AL            
		MOV AH, 02H
		int 21h 
		mov eax,edi
		and eax,0f000000h
		mov cl,24
		shr eax,cl
		call number_conver_ascii
		MOV DL, AL            
		MOV AH, 02H
		int 21h 
		mov eax,edi
		and eax,0f00000h
		mov cl,20
		shr eax,cl
		call number_conver_ascii
		MOV DL, AL            
		MOV AH, 02H
		int 21h 
		mov eax,edi
		and eax,0f0000h
		mov cl,16
		shr eax,cl

		call number_conver_ascii
		MOV DL, AL            
		MOV AH, 02H
		int 21h 
		call print_pcie_string6

		mov eax,edi
		and eax,0f000h
		mov cl,12
		shr eax,cl
		call number_conver_ascii
		MOV DL, AL            
		MOV AH, 02H
		int 21h 
		mov eax,edi
		and eax,0f00h
		mov cl,8
		shr eax,cl
		call number_conver_ascii
		MOV DL, AL            
		MOV AH, 02H
		int 21h 
		mov eax,edi
		and eax,0f0h
		mov cl,4
		shr eax,cl
		call number_conver_ascii
		MOV DL, AL            
		MOV AH, 02H
		int 21h 
		
		mov eax,edi
		and eax,0fh
		call number_conver_ascii
		MOV DL, AL                           ;输出字符
		MOV AH, 02H
		INT 21H
		mov eax,edi
		ret
	
;;---------------------------------------------------
;;  公用打印函数--
;;---------------------------------------------------
	print_startaddress:							;print32
				mov si,offset temp_startaddress
				mov ax,ds:[si]
				mov cl,4
				shr ah,cl
				and ah,0fh
				mov al,ah
				mov bl,0fh
				call number_conver_ascii		;数值转换成ASCII码
				call print_char_color				
				mov ax,ds:[si]
				and ah,0fh
				mov al,ah
				call number_conver_ascii		;数值转换成ASCII码
				call print_char_color				
				mov ax,ds:[si]
				mov cl,4
				shr al,cl
				and al,0fh
				call number_conver_ascii		;数值转换成ASCII码
				call print_char_color				
				mov ax,ds:[si]
				and al,0fh
				call number_conver_ascii		;数值转换成ASCII码			
				call print_char_color	
				call print_colon				;打印':'					
				mov si,offset temp_data			;打印行数
				mov dx,ds:[si+7]
				and dh,0fh
				mov al,dh
				call number_conver_ascii		;数值转换成ASCII码				
				call print_char_color		
				mov al,'0'
				call print_char_color
				call print_blank
			ret
;;---------------------------------------------------
;;  ---打印一个字符串---
;;para: si:字符串的首地址,
;;		bl：输出字符颜色 
;;---------------------------------------------------
	print_string:
                mov cl,ds:[si]
                mov ch,0
                jcxz print_string_ret  		 ;条件转移 当cx=0 跳转             
                mov al,ds:[si]
                mov bl,0fh   			 ;颜色：[7]:闪烁、[6,5,4]:背景R,G,B、[3]:高亮、[2,1,0]:前景R,G,B
                call print_char_color
                inc si
                jmp print_string
        print_string_ret:
			ret
	print8:
				mov si,offset temp_data		   ;先打印高4位
				mov al,ds:[si]
				mov cl,4
				shr al,cl
				and al,0fh
				call number_conver_ascii 	    ;数值转换成ASCII码		
				call print_char_color				
				mov al,ds:[si]					;再打印低4位
				and al,0fh
				call number_conver_ascii		;数值转换成ASCII码
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
            mov ah,09h       ;在光标位置显示字符
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
;;--------------------------------------------------
;;换行
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
;;---------------------------------------------------
;;cpuid prco 输出cpuid相关数据
;;---------------------------------------------------
cpuid_proc:
	call clearscreen    ;清屏
	mov ax,data
	mov ds,ax 
	mov si ,offset Vendor	
	mov ax,si
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
	mov bl ,0dh
	call print_char_color
	dec si
	inc bp; 
	cmp si,0
	jne s
	call print_nextline
	mov si ,offset Family
	mov ax,si
	mov bl,0bh
	call print_string
	mov eax,1
	db 0fh  ;对于不支cpuid的处理器  使用 db 0f db 0a2h 代替cpuid指令
	db 0a2h 
	push eax
	mov cl,8
	shr eax,cl
	add al,30h
	mov bl,0ah
	call print_char_color
	call print_nextline
	mov si ,offset Model
	mov ax,si
	call print_string
	pop eax
	and eax,000000f0h
	mov cl,4
	shr eax,cl
	add al,30h
	mov bl,0ch
	call print_char_color
	call print_nextline
	mov si ,offset Step
	mov ax,si
	call print_string
	mov eax,edi
	and ax,0fh
	add al,30h
	mov bl,09h
	call print_char_color
	call print_nextline
	call print_str_return
	call KeyScanf
ret
;;---------------------------------------------------
;;memorry prco 输出memorry相关数据
;;--
;-------------------------------------------------
memorry_proc:
			call clearscreen
            call print_string1          ;Please enter the starting address(eg:0x1234,)
			call KeyScanf_address
			call print_nextline
			call print_string2
			call KeyScanf_length
			call print_data
			call print_nextline
			call print_str_return
			call KeyScanf
	ret
	;;---------------------------------------------------
	;;sio_proc 输出memorry相关数据
	;-------------------------------------------------
	sio_proc:
	mov ax,data
	mov ds,eax 
	call clearscreen
	;-----------------------------------------------------
	; Enter the Extended Function Mode
	;-----------------------------------------------------
	mov dx, 4eh
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
	call print_nextline
	MOV DX, 04EH  ;exit the Extended function mode  退出扩展模式 
	MOV AL, 0AAH
	OUT DX, AL
	call print_str_return
	call KeyScanf
print_device_0ffset30_f0:
	mov dx,4eh
	mov al,30h     
	out dx,al  ;选取30 寄存器
	MOV DX,04fH
	MOV AL,30h ; 读取offset 30 al的内容
	IN AL,DX
	call show_al
	call print_blank
	mov dx,4eh
	mov al,60h     
	out dx,al  ;选取60 寄存器
	MOV DX,04fH
	MOV AL,60h ; 读取offset 60 al的内容
	IN AL,DX
	call show_al
	call print_blank
	mov dx,4eh
	mov al,61h     
	out dx,al  ;选取61 寄存器
	MOV DX,04fH
	MOV AL,61h ; 读取offset 61 al的内容
	IN AL,DX
	call show_al
	call print_blank
	mov dx,4eh
	mov al,70h     
	out dx,al  ;选取70 寄存器
	MOV DX,04fH
	MOV AL,70h ; 读取offset 70 al的内容
	IN AL,DX
	call show_al
	call print_blank
	mov dx,4eh
	mov al,0f0h     
	out dx,al  ;选取f0 寄存器
	MOV DX,04fH
	MOV AL,0f0h ; 读取offset f0 al的内容
	IN AL,DX
	call show_al
	call print_blank
	mov dx,4eh
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
ret

;;---------------------------------------------------
;cmos proc 输出cmos相关数据
;-------------------------------------------------
cmos_proc:		
		mov ax,stack
		mov ss,ax
		mov sp,64	
		mov ax,data
		mov ds,ax	
		mov ax,0b800h
		mov es,ax
		mov di,0
		call cmos_clearScreen		;第一次打印，清屏
		call cmos_printCmos	
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
		call print_str_return
		call cmos_CheckSecond
		ret

;;---------------------------------------------------
;;pcie_proc 输出pcie相关数据
;-------------------------------------------------
pcie_proc:
			mov ax,stack  
            mov ss,ax
            mov sp,128
            mov ax,data
            mov ds,ax
			call clearscreen
            call print_pcie_string1        ;Please enter the bus(0x0000-0x0f0f,eg:0008) :',0
			call KeyScanf_address
			mov si,offset temp_bus
			mov ds:[si],ax
			call print_nextline
			call print_pcie_string2        ;Please enter the temp_device
			call KeyScanf_address
			mov si,offset temp_device
			mov ds:[si],ax
			call print_nextline
			call print_pcie_string4         ;Please enter the temp_function
			call KeyScanf_address
			mov si,offset temp_function
			mov ds:[si],ax		
			call print_nextline
			call print_pcie_string7
			call print_pcie_data
			call print_nextline			
			call print_str_return
			call KeyScanf
ret
;;---------------------------------------------------
;;smbus_proc 输出smbus相关数据
;-------------------------------------------------
smbus_proc:
 	mov ax,data
    mov ds,ax
    ;----------------------扫描PCI ---------------------
    mov sm_devnum,00a0h   ;设备a0
    mov ax,0003h
    int 10h
    call print_smbase;输出提示字符 sm_base addres:
    call pci_read;读取smbase 到eax
    push ax
    call print_ax;显示sm_base addres
    pop ax
    and ax,0fffeh   ;1111_1111_1111_1110,最后一位1 :表示端口方式
    mov sm_base,ax  ;把基地址保存到sm_base
    call print_nextline
    call sm_read;配置 从设备，偏移地址， 读取格式等
	call print_nextline
	call print_str_return
	call KeyScanf
ret
code ends
    end start
