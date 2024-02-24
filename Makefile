.PHONY: all assemble link run

all: assemble link run

assemble: bootloader.S
	as --32 -o bootloader.o bootloader.S

link: bootloader.o
	ld -Ttext 0x7c00 --oformat binary -m elf_i386 -o bootloader.bin bootloader.o

run: bootloader.bin
	qemu-system-i386 -fda bootloader.bin
