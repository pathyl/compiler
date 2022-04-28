;IO1NasmLinux32.asm
;
;assemble: nasm -f elf -l IO1Nasm32Linux.lst  IO1Nasm32Linux.asm
;link: gcc -o IO1Nasm32Linux  IO1Nasm32Linux.o
;run: ,/IO1Nasm32Linux
;
;For 64 bit Linux
;nasm -felf64 IO1Nasm32Linux.asm && ld IO1Nasm32Linux.o && ./a.out
;
;for debugging with gdb or DDD try
;nasm -g dwarf2 -f elf64 IO1Nasm32Linux.asm -l IO1Nasm32Linux.lst -o IO1Nasm32Linux.o
;ld -g -o IO1Nasm32Linux IO1Nasm32Linux.o
;./IO1Nasm32Linux
	
sys_exit	equ	1
sys_read	equ	3
sys_write	equ	4
stdin		equ	0 ; default keyboard
stdout		equ	1 ; default terminal screen
stderr		equ	3

section .data		;used to declare constants	
	userMsg		db      'Enter an integer(less than 32,765): '
        myMsg           db      'My Message!'
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

        text db "Not an integer", 10
        len equ $-text
        
;end assembly1.txt

;Begin adding constants
	N	DW	85
	M	DW	7
; Start of user variable area    ----------------------------------------------
;begin assembly2.txt

section	.bss		;used to declare uninitialized variables

	TempChar	RESB	1              ;temp space for use by GetNextChar
	testchar	RESB	1	
	ReadInt		RESW	1              ;Temporary storage GetAnInteger.	
	tempint		RESW	1              ;Used in converting to base ten.
	negflag		RESB	1              ;P=positive, N=negative	
	T1		RESW	1		;reserve space for temporaries
	T2		RESW	1
	T4		RESW	1
	T5		RESW	1
	T6		RESW	1
	T7		RESW	1
	T8		RESW	1
	T9		RESW	1
	global _main
        global main
        global _start
        ascii resb 16
        intMemory resb 100
        intAddress resb 8
				;main program
;first add regular vars here, then section .text follows
;end assembly2.txt

;Begin adding vars
	lit12	RESW	1
	lit7	RESW	1
	lit6	RESW	1
	lit8	RESW	1
	lit3	RESW	1
	lit1	RESW	1
	lit2	RESW	1
	F	RESW	1
	Z	RESW	1
	Y	RESW	1
	X	RESW	1
section .text
_main:
    mov ebp, esp; for correct debugging	nop
main:   nop
_start: nop
	mov ebx,[userMsg]
	call PrintString
	call _getText
        call _toInteger
        mov [ReadInt], rax
	mov eax,[ReadInt]
	mov [F], eax
	mov eax, [F]
	cmp eax, [M]
	jge L1
	mov eax, [F]
	mov ebx, [lit2]
	mul ebx
	mov [T2], eax
	mov eax, [T2]
	mov [F], eax
	mov eax, [lit2]
	mov [Y], eax
	mov eax, [lit1]
	mov [X], eax
	mov eax, [lit3]
	mov [Z], eax
	jmp E1
L1:	nop
	mov eax, [lit3]
	mov [Y], eax
	mov eax, [lit8]
	mov [X], eax
	mov eax, [lit6]
	mov [Z], eax
E1:	nop
W1:	nop
	mov eax, [F]
	cmp eax, [lit7]
	jge E2
	mov eax, [Y]
	add [lit2], eax
	mov [T2], eax
	mov eax, [T2]
	mov [Y], eax
	mov eax, [F]
	add [lit1], eax
	mov [T4], eax
	mov eax, [T4]
	mov [F], eax
	jmp W1
E2:	nop
	mov eax, [Y]
	add [X], eax
	mov [T6], eax
	mov eax, [T6]
	mov [Z], eax
	mov eax, [Y]
	mov ebx, [Z]
	mul ebx
	mov [T8], eax
	mov eax, [M]
	add [T8], eax
	mov [T9], eax
	mov eax, [T9]
	add [lit12], eax
	mov [T1], eax
	mov eax, [T1]
	mov [X], eax
	mov eax, [X]
	mov [F], eax
	mov eax, [N]
	call ConvertIntegerToString ;result in ebx
	call PrintString
;begin assembly3.txt
; exit code
fini:
	mov eax,sys_exit ;terminate, sys_exit = 1
	xor ebx,ebx	;successfully, zero in ebx indicates success
	int 80h

;	ENDP main

;
;       Subroutine to print a string on the display
;
; Input:
;       DS:BX = pointer to the string to print
;
; Output: None
;
; Registers destroyed: none
;
;PrintString	PROC
PrintString:
        nop
	push	ax            ;Save registers;
	push	dx
; subpgm:	nop
	; prompt user	
	mov eax, 4		;Linux print device register conventions
	mov ebx, 1		; print default output device
	mov ecx, userMsg	; pointer to string
	mov edx, lenUserMsg	; arg1, where to write, screen
	int	80h		; interrupt 80 hex, call kernel
	pop	dx              ;Restore registers.
	pop	ax
	ret
;PrintString     ENDP

;%NEWPAGE

;

; Subroutine to get an integer (character string) from the keyboard buffer
;    and convert it to a 16 bit binary number.
;
; Input: none
;
; Output: The integer is returned in the AX register as well as the global
;         variable ReadInt .
;
; Registers Destroyed: AX, BX, CX, DX, SI
;
; Globals Destroyed: ReadInt, TempChar, tempint, negflag
;
;GetAnInteger	PROC

GetAnInteger:	nop	;Get an integer as a string
	;get response
	mov rax,3	;read
	mov rbx,2	;device
	mov rcx,[num]	;buffer address
	mov rdx,6	;max characters
	int 0x80

	;print number    ;works
	mov rdx,rax 	; eax contains the number of character read including <lf>
	mov rax, 4
	mov rbx, 1
	mov rcx, [num]
	int 80h
_getText:
    mov rax, 0
    mov rdi, 0
    mov rsi, ascii
    mov rdx, 16
    syscall
    mov byte [ascii-1+rax], 0
    ret
    
_toInteger:
    mov rbx,10      ; for decimal scaling
    xor rax, rax    ; initializing result
    mov rcx, ascii  ; preparing for working with input

    .LL1:           ; loops the bytes
    movzx rdx, byte [rcx]   ; getting current byte (digit)

    test rdx, rdx   ; RDX == 0?
    jz .done        ; Yes: break

    inc rcx         ; for the next digit

    cmp rdx, '0'    ; if it's less than '0' is not a digit
    jb _invalid

    cmp rdx, '9'    ; if it's greater than '9' is not a digit
    ja _invalid

    sub rdx, '0'    ; getting decimal value

    ; mul rbx         ; rax = rax*10
    add rax, rax
    lea rax, [rax + rax * 4]

    add rax, rdx    ; rax = rax + rdx

    ;jmp _toInteger  ; repeat
    jmp .LL1  ; repeat

    .done:
    ret
    
_invalid:
        mov rax, 1
        mov rdi, 1
        mov rsi, text
        mov rdx, len
        syscall
        ret

_appendEOL:
        ; getting EOL
        mov rcx, intMemory
        mov rbx, 10 ; EOL
        mov [rcx], rbx
        inc rcx
        mov [intAddress], rcx

_loopDigit:
        xor rdx, rdx
        mov rbx, 10
        div rbx
        push rax
        add rdx, '0'
        mov rcx, [intAddress]
        mov [rcx], dl
        inc rcx
        mov [intAddress], rcx
        pop rax
        cmp rax, 0
        jne _loopDigit

ConvertStringToInteger:	nop
	mov rax,0  ;hold integer
	mov [ReadInt],rax ;initialize 16 bit number to zero
	mov rcx, [num]	;pt - 1st or next digit of number as a string 
			;terminated by <lf>.
	mov rbx,0	
	mov R15, rcx ;contains first or next digit
Next:	sub R15, 48	;convert character to number
	mov rax,[ReadInt]
	mov rdx,10
	mul rdx		;eax = eax * 10
	add rax,rbx
	mov [ReadInt], rax
	mov rbx,0
	add rcx,1 	;pt = pt + 1
	movzx rcx, byte[rcx]
	cmp bl,0	;is it a <lf>
	jne Next	; get next digit  
	ret
;	ENDP GetAnInteger



;%NEWPAGE
;
; Subroutine to convert a 16 bit integer to a text string
;
; input:
;       AX = number to convert
;       DS:BX = pointer to end of string to store text
;       CX = number of digits to convert
;
; output: none
;
; Registers destroyed: AX, BX, CX, DX, SI
; Globals destroyed negflag 
;
;ConvertIntegerToString PROC

ConvertIntegerToString:	nop
	mov ebx, ResultValue + 4   ;Store the integer as a five
	                    ;digit char string at Result for printing
        ret

ConvertLoop:	nop
	sub edx,edx  ; repeatedly divide dx:ax by 10 to obtain last digit of number
	mov ecx,10  ; as the remainder in the DX register.  Quotient in AX.
	div ecx
	add dl,48 ; Add '0' to dl to convert from binary to character.
	mov [ebx], dl
	dec ebx
	cmp ebx,ResultValue
	jge ConvertLoop
	ret

;ConvertIntegerToString  ENDP
;end assembly3.txt
