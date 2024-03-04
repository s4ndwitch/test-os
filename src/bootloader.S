
.intel_syntax noprefix

.code16

start:

	mov ax, cs
	mov ds, ax
	mov ss, ax
	mov sp, start

get_character:

	mov bx, offset ABC
	call puts

	mov ah, 0x0
	int 0x16

	cmp ah, 0x1c
	je load_kernel

	cmp al, 'a'
	jl get_character

	cmp al, 'z'
	jg get_character

	mov bx, offset ABC

	sub al, 'a'
	mov ah, 0x0
	add bx, ax

	mov ah, [bx]
	cmp ah, '_'
	jne set_lowline

	mov ah, al
	add ah, 'a'
	mov [bx], ah

	jmp set_lowline_done

set_lowline:

	mov ah, '_'
	mov [bx], ah

set_lowline_done:

	jmp get_character

load_kernel:

	mov [BOOT_DRIVE], dl

	mov bx, 0x1000
	mov es, bx
	mov bx, 0x0
	mov dh, 30
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

puts:

	mov al, [bx]
	test al, al
	jz end_puts

	mov ah, 0x0e
	int 0x10

	add bx, 1

	jmp puts
	
	end_puts:
	
	ret

.code32

protected_mode:

	mov ax, 0x10
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

running_kernel:

	call 0x10000

	mov edx, 0xb8000
	mov al, 'T'
	mov ah, 0x07
	mov [edx], ax
	
	jmp shutdown

.code16
shutdown:

	mov eax, cr0
	xor al, al
	mov cr0, eax

	int 0x80

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

.zero 450 - ($ - start)

ABC:
	.asciz "abcdefghijklmnopqrstuvwxyz\n\r"

.zero 510 - ($ - start)

.word 0xaa55
