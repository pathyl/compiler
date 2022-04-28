
sys_exit	equ	1
sys_read	equ	3
sys_write	equ	4
stdin		equ	0 ; default keyboard
stdout		equ	1 ; default terminal screen
stderr		equ	3

section .data		;used to declare constants	
	userMsg		db      'Enter an integer(less than 32,765): '
	lenUserMsg	equ	$-userMsg
	displayMsg	db	'You entered: '
	lenDisplayMsg	equ	$-displayMsg
	newline		db	0xA 	; 0xA 0xD is ASCII <LF><CR>

	Ten             DW      10  ;Used in converting to base ten.

	printTempchar	db	'Tempchar = : '
	lenprintTempchar	equ 	$-printTempchar


	Result          db      'Ans = '
	ResultValue	db	'aaaaa'
			db	0xA		
	ResultEnd       equ 	$-Result    ; $=> here, subtract address Result

	num		times 6 db 'ABCDEF'
	numEnd		equ	$-num

;Begin adding constants
	Z	DW	0
	Y	DW	1
; Start of user variable area    ----------------------------------------------

section	.bss		;used to declare uninitialized variables

	TempChar        RESB    1              ;temp space for use by GetNextChar
	testchar        RESB    1	
	ReadInt         RESW    1              ;Temporary storage GetAnInteger.	
	tempint         RESW	1              ;Used in converting to base ten.
	negflag         RESB    1              ;P=positive, N=negative	
	T1		RESW	1		;reserve space for temporaries
	T2		RESW	1
	T3		RESW	1
	T4		RESW	1
	T5		RESW	1
	T6		RESW	1
	T7		RESW	1
	T8		RESW	1
	T9		RESW	1

	
	global _start   ;main program

;Begin adding vars
	B	RESW	1
	A	RESW	1
section .text
_start:	nop
	call PrintString
	call GetAnInteger
	mov ax,[ReadInt]
	mov [A], ax
	call PrintString
	call GetAnInteger
	mov ax,[ReadInt]
	mov [B], ax
	mov ax, [A]
	cmp ax, [B]
	jle _L1
	mov ax, [Y]
	call ConvertIntegerToString
	call PutInteger
	jmp _E1
_L1:	nop
	mov ax, [Z]
	call ConvertIntegerToString
	call PutInteger
_E1:	nop
	mov ax, [A]
	cmp ax, [B]
	jl _L2
	mov ax, [Y]
	call ConvertIntegerToString
	call PutInteger
	jmp _E2
_L2:	nop
	mov ax, [Z]
	call ConvertIntegerToString
	call PutInteger
_E2:	nop
	mov ax, [A]
	cmp ax, [B]
	jne _L3
	mov ax, [Y]
	call ConvertIntegerToString
	call PutInteger
	jmp _E3
_L3:	nop
	mov ax, [Z]
	call ConvertIntegerToString
	call PutInteger
_E3:	nop
	mov ax, [A]
	cmp ax, [B]
	je _L4
	mov ax, [Y]
	call ConvertIntegerToString
	call PutInteger
	jmp _E4
_L4:	nop
	mov ax, [Z]
	call ConvertIntegerToString
	call PutInteger
_E4:	nop
	mov ax, [A]
	cmp ax, [B]
	jge _L5
	mov ax, [Y]
	call ConvertIntegerToString
	call PutInteger
	jmp _E5
_L5:	nop
	mov ax, [Z]
	call ConvertIntegerToString
	call PutInteger
_E5:	nop
	mov ax, [A]
	cmp ax, [B]
	jg _L6
	mov ax, [Y]
	call ConvertIntegerToString
	call PutInteger
	jmp _E6
_L6:	nop
	mov ax, [Z]
	call ConvertIntegerToString
	call PutInteger
_E6:	nop
;**************************
	
; exit code
fini:   nop
	mov eax,sys_exit ;terminate, sys_exit = 1
	xor ebx,ebx	;successfully, zero in ebx indicates success
	int 80h

PrintString:
	push    ax              ;Save registers;
	push    dx
	mov eax, 4		;Linux print device register conventions
	mov ebx, 1		; print default output device
	mov ecx, userMsg	; pointer to string
	mov edx, lenUserMsg	; arg1, where to write, screen
	int	80h		; interrupt 80 hex, call kernel
	pop     dx              ;Restore registers.
	pop     ax
	ret

GetAnInteger:	;Get an integer as a string
	;get response
	mov eax,3	;read
	mov ebx,2	;device
	mov ecx,num	;buffer address
	mov edx,6	;max characters
	int 0x80

	;print number    ;works
	mov edx,eax 	; eax contains the number of character read including <lf>
	mov eax, 4
	mov ebx, 1
	mov ecx, num
	int 80h

ConvertStringToInteger:
	mov ax,0	;hold integer
	mov [ReadInt],ax ;initialize 16 bit number to zero
	mov ecx,num	;pt - 1st or next digit of number as a string 
			;terminated by <lf>.
	mov bx,0	
	mov bl, byte [ecx] ;contains first or next digit
Next:	sub bl,'0'	;convert character to number
	mov ax,[ReadInt]
	mov dx,10
	mul dx		;eax = eax * 10
	add ax,bx
	mov [ReadInt], ax

	mov bx,0
	add ecx,1 	;pt = pt + 1
	mov bl, byte[ecx]

	cmp bl,0xA	;is it a <lf>
	jne Next	; get next digit  
	ret
PutInteger:

	push ax
	push dx
	mov eax, 4	;write
	mov ebx, 1	;print default sys_out
	mov ecx, Result	;start address for print
	mov edx, ResultEnd
	int 80h
	pop ax
	pop dx
	ret

ConvertIntegerToString:
	mov ebx, ResultValue + 4   ;Store the integer as a five
	                    ;digit char string at Result for printing
ConvertLoop:
	sub dx,dx  ; repeatedly divide dx:ax by 10 to obtain last digit of number
	mov cx,10  ; as the remainder in the DX register.  Quotient in AX.
	div cx
	add dl,'0' ; Add '0' to dl to convert from binary to character.
	mov [ebx], dl
	dec ebx
	cmp ebx,ResultValue
	jge ConvertLoop
	ret


