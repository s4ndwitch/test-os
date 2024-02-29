

#include "string.h"

#define VIDEO_MEMORY_PTR 0xb8000
#define ALPHABET_PTR 0x7c00 + 0x1c2
#define IDT_TYPE_INTR 0x0e
#define IDT_TYPE_TRAP 0x0f
#define GDT_CS 0x8
#define PIC1_PORT 0x20
#define NULL 0x0

__asm("jmp kmain;");

char scancodes[128];

void scancodes_init() {

	scancodes[0x00] = NULL;
	scancodes[0x01] = NULL;
	scancodes[0x02] = '1';
	scancodes[0x03] = '2';
	scancodes[0x04] = '3';
	scancodes[0x05] = '4';
	scancodes[0x06] = '5';
	scancodes[0x07] = '6';
	scancodes[0x08] = '7';
	scancodes[0x09] = '8';
	scancodes[0x0A] = '9';
	scancodes[0x0B] = '0';
	
}

struct idt_entry {

	unsigned short base_lo;
	unsigned short segm_sel;
	unsigned char always0;
	unsigned char flags;
	unsigned short base_hi;
} __attribute__((packed));

struct idt_ptr {
	unsigned short limit;
	unsigned int base;
} __attribute__((packed));


char *video_memory;
struct idt_entry g_idt[256];
struct idt_ptr g_idtp;

void default_intr_handler() {
	__asm("pusha;");
	// tt
	__asm("popa; leave; iret;");
}

typedef void (*intr_handler) ();
void intr_reg_handler(int num, unsigned short segm_sel,
		unsigned short flags, intr_handler hndlr) {

	unsigned int hndlr_addr = (unsigned int) hndlr;

	g_idt[num].base_lo = (unsigned short) (hndlr_addr & 0xFFFF);
	g_idt[num].segm_sel = segm_sel;
	g_idt[num].always0 = 0;
	g_idt[num].flags = flags;
	g_idt[num].base_hi = (unsigned short) (hndlr_addr >> 16);

}

void intr_init() {
	int i;
	int idt_count = sizeof(g_idt) / sizeof(g_idt[0]);

	for (i = 0; i < idt_count; i++)
		intr_reg_handler(i, GDT_CS, 0x80 | IDT_TYPE_INTR,
			default_intr_handler);
}

void intr_start() {
	int idt_count = sizeof(g_idt) / sizeof(g_idt[0]);

	g_idtp.base = (unsigned int) (&g_idt[0]);
	g_idtp.limit = (sizeof (struct idt_entry) * idt_count) - 1;

	__asm( "lidt %0;" : : "m" (g_idtp) );
}

void intr_enable() {
	__asm("sti;");
}

void intr_disable() {
	__asm("cli;");
}

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

static inline unsigned char inb (unsigned short port) {
	unsigned char data;
	__asm volatile ("inb %b0, %w1" : "=a" (data) : "Nd" (port));
	return data;
}

static inline void outb (unsigned short port, unsigned char data) {
	__asm volatile ("outb %w1, %b0" : : "a" (data), "Nd" (port));
}

void on_key(unsigned char scan_code) {

	video_memory = (char *) VIDEO_MEMORY_PTR;

	char key = scancodes[scan_code];

	if (key == NULL)
		return;

	*(video_memory) = key;
	*(video_memory + 1) = 0x07;
	
}

void keyb_process_keys() {
	if (inb(0x64) & 0x01) {
		unsigned char scan_code;

		scan_code = inb(0x60);

		if (scan_code < 128)
			on_key(scan_code);
	}
}

void keyb_handler() {
	__asm("pusha");

	keyb_process_keys();

	outb(PIC1_PORT, 0x20);
	__asm("popa; leave; iret;");
}

void keyb_init() {
	intr_reg_handler(0x09, GDT_CS, 0x80 | IDT_TYPE_INTR, keyb_handler);
	outb(PIC1_PORT + 1, 0xff ^ 0x02);
}

void kmain() {

	intr_init();
	keyb_init();
	scancodes_init();
	
	intr_start();
	intr_enable();

	video_memory = (char *) VIDEO_MEMORY_PTR;

	clean_screen();

	char hello[30] = "HELLO FROM C KERNEL B!!!";
	
	for (int i = 0; i < 30; i++) {
		*(video_memory + i * 2) = hello[i];
		*(video_memory + i * 2 + 1) = 0x07;
	}

}
