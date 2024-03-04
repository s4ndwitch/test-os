

#include "string.h"

#define VIDEO_MEMORY_PTR 0xb8000
#define VIDEO_MEMORY_OVERBUF 0xb8000 + 4000
#define ALPHABET_PTR 0x7c00 + 0x1c2
#define IDT_TYPE_INTR 0x0e
#define IDT_TYPE_TRAP 0x0f
#define GDT_CS 0x8
#define PIC1_PORT 0x20
#define NULL 0x0

__asm("jmp kmain;");

int input_counter;
char input_buffer[41];

char scancodes[128];

void scancodes_init() {

	for (int i = 0; i < 128; i++)
		scancodes[i] = NULL;

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
	scancodes[0x1E] = 'a';
	scancodes[0x30] = 'b';
	scancodes[0x2E] = 'c';
	scancodes[0x20] = 'd';
	scancodes[0x12] = 'e';
	scancodes[0x21] = 'f';
	scancodes[0x22] = 'g';
	scancodes[0x23] = 'h';
	scancodes[0x17] = 'i';
	scancodes[0x24] = 'j';
	scancodes[0x25] = 'k';
	scancodes[0x26] = 'l';
	scancodes[0x32] = 'm';
	scancodes[0x31] = 'n';
	scancodes[0x18] = 'o';
	scancodes[0x19] = 'p';
	scancodes[0x10] = 'q';
	scancodes[0x13] = 'r';
	scancodes[0x1F] = 's';
	scancodes[0x14] = 't';
	scancodes[0x16] = 'u';
	scancodes[0x2F] = 'v';
	scancodes[0x11] = 'w';
	scancodes[0x2D] = 'x';
	scancodes[0x15] = 'y';
	scancodes[0x2C] = 'z';
	scancodes[0x39] = ' ';
	scancodes[0x4E] = '+';
	scancodes[0x4A] = '-';
	scancodes[0x35] = '/';
	scancodes[0x37] = '*';
	scancodes[0x1C] = '\n';
	scancodes[0x0E] = 0x8;
	
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


char *cursor;
struct idt_entry g_idt[256];
struct idt_ptr g_idtp;

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

	cursor = (char *) VIDEO_MEMORY_PTR;

}

void put_symbol(const char symbol) {

	if (cursor == (char *) VIDEO_MEMORY_OVERBUF)
		clean_screen();

	switch (symbol) {
		case '\n':
			cursor = (char *) VIDEO_MEMORY_PTR + ((cursor - (char *) VIDEO_MEMORY_PTR) / 160 + 1) * 160;
			break;
		case 0x08:
			*(cursor--) = 0x0;
			*(cursor--) = 0x0;
			break;
		default:
			*(cursor++) = symbol;
			*(cursor++) = 0x07;
			break;
	}

}

void puts(const char *string) {

	if (string == NULL)
		return;

	while (*string != 0) {

		put_symbol(*string);

		string += 1;
	}

}

char *dictionary[52][2];
int dictionary_count;

void init_dictionary() {

	char *alphabet = (char *) ALPHABET_PTR;

	if (alphabet[0] != '_') {

		dictionary[dictionary_count][0] = "air";
		dictionary[dictionary_count++][1] = "aire";

		dictionary[dictionary_count][0] = "alphabet";
		dictionary[dictionary_count++][1] = "alfabeto";

	}

	if (alphabet[1] != '_') {

		dictionary[dictionary_count][0] = "belligerent";
		dictionary[dictionary_count++][1] = "beligerante";

		dictionary[dictionary_count][0] = "bestial";
		dictionary[dictionary_count++][1] = "bestial";

	}

	if (alphabet[2] != '_') {

		dictionary[dictionary_count][0] = "campus";
		dictionary[dictionary_count++][1] = "campus";
		
		dictionary[dictionary_count][0] = "candidate";
		dictionary[dictionary_count++][1] = "candidato";

	}

	if (alphabet[3] != '_') {

		dictionary[dictionary_count][0] = "dictate";
		dictionary[dictionary_count++][1] = "dictar";

		dictionary[dictionary_count][0] = "dominate";
		dictionary[dictionary_count++][1] = "dominar";

	}

	if (alphabet[4] != '_') {

		dictionary[dictionary_count][0] = "effect";
		dictionary[dictionary_count++][1] = "efecto";

		dictionary[dictionary_count][0] = "emperor";
		dictionary[dictionary_count++][1] = "emperador";

	}

	if (alphabet[5] != '_') {

		dictionary[dictionary_count][0] = "feminine";
		dictionary[dictionary_count++][1] = "femenino";

		dictionary[dictionary_count][0] = "figure";
		dictionary[dictionary_count++][1] = "figura";

	}

	if (alphabet[6] != '_') {

		dictionary[dictionary_count][0] = "generate";
		dictionary[dictionary_count++][1] = "generar";

		dictionary[dictionary_count][0] = "gladiator";
		dictionary[dictionary_count++][1] = "gladiador";

	}

	if (alphabet[7] != '_') {

		dictionary[dictionary_count][0] = "herb";
		dictionary[dictionary_count++][1] = "hierba";
		
		dictionary[dictionary_count][0] = "hour";
		dictionary[dictionary_count++][1] = "hora";

	}

	if (alphabet[8] != '_') {

		dictionary[dictionary_count][0] = "illuminate";
		dictionary[dictionary_count++][1] = "iluminar";

		dictionary[dictionary_count][0] = "initial";
		dictionary[dictionary_count++][1] = "inicial";

	}

	if (alphabet[9] != '_') {

		dictionary[dictionary_count][0] = "judicial";
		dictionary[dictionary_count++][1] = "judicial";

		dictionary[dictionary_count][0] = "juvenile";
		dictionary[dictionary_count++][1] = "juvenil";

	}

	if (alphabet[10] != '_') {

		dictionary[dictionary_count][0] = "keep";
		dictionary[dictionary_count++][1] = "mantener";

		dictionary[dictionary_count][0] = "key";
		dictionary[dictionary_count++][1] = "clave";

	}

	if (alphabet[11] != '_') {

		dictionary[dictionary_count][0] = "legal";
		dictionary[dictionary_count++][1] = "legal";

		dictionary[dictionary_count][0] = "latitude";
		dictionary[dictionary_count++][1] = "latitud";

	}

	if (alphabet[12] != '_') {

		dictionary[dictionary_count][0] = "major";
		dictionary[dictionary_count++][1] = "mayor";
		
		dictionary[dictionary_count][0] = "marine";
		dictionary[dictionary_count++][1] = "marina";

	}

	if (alphabet[13] != '_') {

		dictionary[dictionary_count][0] = "nominal";
		dictionary[dictionary_count++][1] = "nominal";

		dictionary[dictionary_count][0] = "note";
		dictionary[dictionary_count++][1] = "nota";

	}

	if (alphabet[14] != '_') {

		dictionary[dictionary_count][0] = "ocular";
		dictionary[dictionary_count++][1] = "ocular";

		dictionary[dictionary_count][0] = "ovine";
		dictionary[dictionary_count++][1] = "ovino";

	}

	if (alphabet[15] != '_') {

		dictionary[dictionary_count][0] = "perfect";
		dictionary[dictionary_count++][1] = "perfecto";
		
		dictionary[dictionary_count][0] = "popular";
		dictionary[dictionary_count++][1] = "popular";

	}

	if (alphabet[16] != '_') {

		dictionary[dictionary_count][0] = "quant";
		dictionary[dictionary_count++][1] = "cuant";

		dictionary[dictionary_count][0] = "quarantine";
		dictionary[dictionary_count++][1] = "cuarentena";

	}

	if (alphabet[17] != '_') {

		dictionary[dictionary_count][0] = "resist";
		dictionary[dictionary_count++][1] = "resistir";

		dictionary[dictionary_count][0] = "rational";
		dictionary[dictionary_count++][1] = "racional";

	}

	if (alphabet[18] != '_') {

		dictionary[dictionary_count][0] = "sacrament";
		dictionary[dictionary_count++][1] = "sacramento";

		dictionary[dictionary_count][0] = "sapient";
		dictionary[dictionary_count++][1] = "sapiente";

	}

	if (alphabet[19] != '_') {

		dictionary[dictionary_count][0] = "tempo";
		dictionary[dictionary_count++][1] = "tempo";

		dictionary[dictionary_count][0] = "terrain";
		dictionary[dictionary_count++][1] = "terreno";

	}

	if (alphabet[20] != '_') {

		dictionary[dictionary_count][0] = "unite";
		dictionary[dictionary_count++][1] = "unir";
		
		dictionary[dictionary_count][0] = "urban";
		dictionary[dictionary_count++][1] = "urbano";

	}

	if (alphabet[21] != '_') {

		dictionary[dictionary_count][0] = "vast";
		dictionary[dictionary_count++][1] = "vasto";

		dictionary[dictionary_count][0] = "verbal";
		dictionary[dictionary_count++][1] = "verbal";

	}

	if (alphabet[22] != '_') {

		dictionary[dictionary_count][0] = "word";
		dictionary[dictionary_count++][1] = "palabra";

		dictionary[dictionary_count][0] = "world";
		dictionary[dictionary_count++][1] = "mundo";

	}

	if (alphabet[23] != '_') {

		dictionary[dictionary_count][0] = "xerox";
		dictionary[dictionary_count++][1] = "xerox";

		dictionary[dictionary_count][0] = "xylene";
		dictionary[dictionary_count++][1] = "xileno";

	}

	if (alphabet[24] != '_') {

		dictionary[dictionary_count][0] = "yay";
		dictionary[dictionary_count++][1] = "yay";

		dictionary[dictionary_count][0] = "yesterday";
		dictionary[dictionary_count++][1] = "ayer";

	}

	if (alphabet[25] != '_') {

		dictionary[dictionary_count][0] = "zero";
		dictionary[dictionary_count++][1] = "cero";
		
		dictionary[dictionary_count][0] = "zoom";
		dictionary[dictionary_count++][1] = "zoom";

	}

}

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

static inline unsigned char inb (unsigned short port) {
	unsigned char data;
	__asm volatile ("inb %b0, %w1" : "=a" (data) : "Nd" (port));
	return data;
}

static inline void outb (unsigned short port, unsigned char data) {
	__asm volatile ("outb %w1, %b0" : : "a" (data), "Nd" (port));
}

static inline void outw (unsigned short port, unsigned short data) {
	__asm volatile ("outw %w1, %w0" : : "a" (data), "Nd" (port));
}

void put_number(int number) {

	if (number == 0) {
		put_symbol('0');
		return;
	}

	int temp = number;
	int count = 0;

	while (temp != 0) {
		temp /= 10;
		count += 1;
	}

	int array[count];
	temp = count;

	while (count--) {
		array[count] = number % 10;
		number /= 10;
	}

	for (int i = 0; i < temp; i++)
		put_symbol(array[i] + '0');

}

int strlen(char *a) {

	int result = 0;

	while (*(a++))
		result += 1;

	return result;

}

int starts_with(char *a, char *b) {

	int length = strlen(a);
	int compare = strlen(b);

	if (length > compare)
		return -1;

	char copy[length + 1];
	copy[length] = 0;
	for (int i = 0; i < length; i++)
		copy[i] = b[i];

	return strcmp(a, copy);

}

void on_key(unsigned char scan_code) {

	char key = scancodes[scan_code];

	if (key == NULL)
		return;

	if (key == 0x08 && input_counter == 0)
		return;

	if (key != 0x08 && key != '\n' && input_counter == 40)
		return;

	if (key == 0x08) {
		input_buffer[input_counter] = 0;
		input_counter -= 1;
	}
	
	if (key != 0x08 && key != '\n') {
		input_buffer[input_counter] = key;
		input_counter += 1;
	}

	put_symbol(key);

	if (key == '\n') {

		input_buffer[input_counter] = 0;

		if (!strcmp("info", input_buffer)) {
			puts("DictOS: final edition (hopefully).\nDeveloper: Daniel Bialokon, " \
					"5131001/20002, SpbPU, 2024\nCompilers: bootloader: as, kernel: gcc\n"\
					"Bootloader parameters: ");
			for (int i = 0; i < 26; i++) {
				put_symbol(*((char *) ALPHABET_PTR + i));
			}
			puts("\n");
		} else if (!strcmp("shutdown", input_buffer)) {
			outw(0xb004, 0x2000);
			outw(0x604, 0x2000);
		} else if (!strcmp("dictinfo", input_buffer)) {
			puts("Dictionary: en -> es\nNumber of words: ");
			put_number(52);
			puts("\nNumber of loaded words: ");
			put_number(dictionary_count);
			puts("\n");
		} else if (!starts_with("wordstat ", input_buffer)) {
			if (strlen(input_buffer) == 10) {
				int result = 0;
				for (int i = 0; i < dictionary_count; i++)
					if (dictionary[i][0][0] == input_buffer[9])
						result += 1;
				puts("Letter \'");
				put_symbol(input_buffer[9]);
				puts("\': ");
				put_number(result);
				puts("\n");
			}
		} else if (!starts_with("translate ", input_buffer)) {
			int length = strlen(input_buffer) - 10;
			char word[length + 1], found = 0;

			for (int i = 0; i < length; i++)
				word[i] = input_buffer[i + 10];

			word[length] = 0;
			
			int left = 0, mid, result;
			int right = dictionary_count - 1;
			while (left <= right) {
				mid = (left + right) / 2;
				result = strcmp(dictionary[mid][0], word);
				if (result < 0)
					left = mid + 1;
				else if (result > 0)
					right = mid - 1;
				else {
					found = 1;
					break;
				}

			}
			if (found) {
				puts(dictionary[mid][1]);
				puts("\n");
			} else
				puts("This word is not found :(\n");
		} else
			puts("No command was entered or there was a mistake in one\n");

		input_counter = 0;

		puts("> ");

	}
	
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

	input_counter = 0;
	input_buffer[40] = 0;
	input_buffer[0] = 0;

	dictionary_count = 0;
	init_dictionary();

	intr_init();
	keyb_init();
	scancodes_init();
	
	intr_start();
	intr_enable();

	cursor = (char *) VIDEO_MEMORY_PTR;

	outb(0x3D4, 0x0F);
	outb(0x3D4 + 1, (unsigned char)((26 * 80) & 0xFF));
	outb(0x3d4, 0x0E);
	outb(0x3d4 + 1, (unsigned char)(((26 * 80) >> 8) & 0xFF));

	clean_screen();

	char hello[30] = "HELLO FROM C KERNEL B!!!\n";
	
	puts(hello);

	puts("> ");

	while (1);

}
