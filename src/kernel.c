

#include "string.h"

#define VIDEO_MEMORY_PTR 0xb8000
#define ALPHABET_PTR 0x7c00 + 0x1c2

__asm("jmp kmain;");

char *video_memory;

void clean_screen() {

	__asm(
		"mov ebx, 0xb8000;"
		"xor ax, ax;"
		"xor ecx, ecx;"
		"clean_screen_loop:"
		"mov [ebx], ax;"
		"add ebx, 2;"
		"inc ecx;"
		"cmp ecx, 2000;"
		"jl clean_screen_loop;"
		:
		:
		: "ebx", "ecx", "ax"
	);

}

void kmain() {

	char *alphabet = (char *) ALPHABET_PTR;
	video_memory = (char *) VIDEO_MEMORY_PTR;

	clean_screen();

	char hello[30] = "HELLO FROM C KERNEL B!!!";
	
	for (int i = 0; i < 30; i++) {
		*(video_memory + i * 2) = hello[i];
		*(video_memory + i * 2 + 1) = 0x07;
	}

}
