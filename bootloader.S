
.intel_syntax noprefix

.code16

.equiv VIDEO_MEMORY, 0xb8000
.equiv WHITE_ON_BLACK, 0x0f

start:
	mov bp, 0x9000
	mov sp, bp

	mov bx, offset MSG_READ_MODE
	call print_string

	call switch_to_pm

	jmp $

print_string:
	mov al, [bx]
	test al, al
	jz end_print_string

	mov ah, 0x0e
	int 0x10
	addw bx, 1
	jmp print_string

	end_print_string:
	ret

gdt:
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0xff, 0xff, 0x00, 0x00, 0x00, 0x9A, 0xCF, 0x00
	.byte 0xff, 0xff, 0x00, 0x00, 0x00, 0x92, 0xCF, 0x00

gdt_info:
	.word gdt_info - gdt
	.word gdt, 0

switch_to_pm:

	cli
	lgdt gdt_info

	in al, 0x92
	or al, 2
	out 0x92, al

	mov eax, cr0
	or eax, 0x1
	mov cr0, eax

	jmp 0x8:init_pm


.code32
.include "print.S"

init_pm:

	mov ax, 0x10
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	mov ebp, 0x90000
	mov esp, ebp

	call BEGIN_PM

BEGIN_PM:

	mov edi, 0xb8000

	xor cx, cx
tt:
	mov esi, offset SPACE
	call print
	inc cx
	cmp cx, 79
	jl tt

loop:
	hlt
	jmp loop

MSG_READ_MODE:
	.asciz "Started in 16-bit Read Mode\n\r"

MSG_PROT_MODE:
	.asciz "Successfully landed in 32-bit Protected Mode"

SPACE:
	.asciz "F"

.zero 510 - ($ - start)
.word 0xaa55
