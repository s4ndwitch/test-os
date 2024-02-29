
print:
	mov al, [esi]
	test al, al
	jz print_end
	mov ah, 0x07
	mov [edi], al
	mov [edi+1], ah
	add edi, 2
	add esi, 1
	jmp print

	print_end:
	ret
