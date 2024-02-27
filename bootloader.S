
.intel_syntax noprefix

.code16

start:

	mov ax, cs
	mov ds, ax
	mov ss, ax
	mov sp, start

load_kernel:

	mov [BOOT_DRIVE], dl

	mov bx, 0x1000
	mov dh, 15
	mov dl, [BOOT_DRIVE]

	push dx

	mov ah, 0x02
	mov al, dh
	mov ch, 0x00
	mov dh, 0x00
	mov cl, 0x02

	int 0x13

entering:

	cli
	lgdt gdt_info

	in al, 0x92
	or al, 2
	out 0x92, al

	mov eax, cr0
	or eax, 0x1
	mov cr0, eax

	jmp 0x8:protected_mode


.code32

protected_mode:

	mov ax, 0x10
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	mov edi, 0xb8000
	mov esi, offset MSG_PROT_MODE
	call print

	call 0x1000

loop:
	hlt
	jmp loop

.include "print.S"


MSG_PROT_MODE:
	.asciz "Successfully landed in 32-bit Protected Mode"

BOOT_DRIVE:
	.byte 0x0

gdt:
	.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.byte 0xff, 0xff, 0x00, 0x00, 0x00, 0x9A, 0xCF, 0x00
	.byte 0xff, 0xff, 0x00, 0x00, 0x00, 0x92, 0xCF, 0x00

gdt_info:
	.word gdt_info - gdt
	.word gdt, 0

.zero 510 - ($ - start)
.word 0xaa55
