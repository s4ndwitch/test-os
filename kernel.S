
.intel_syntax noprefix

.code32

infloop:

	mov edi, 0xb8000
	mov esi, offset HELLO
	call print

loop:
	jmp loop

.include "print.S"

HELLO:
	.asciz "HELLO FROM KERNEL"
