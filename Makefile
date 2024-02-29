
.PHONY: run clean all

all: run clean

build:
	mkdir build

dist:
	mkdir dist

build/bootloader.o: build src/bootloader.S
	as --32 -I src src/bootloader.S -o build/bootloader.o

build/bootloader.bin: build/bootloader.o
	ld --oformat binary -Ttext 0x7c00 -o build/bootloader.bin -m elf_i386 build/bootloader.o

build/kernel.o: build src/kernel.c
	gcc -fno-pie -ffreestanding -m32 -masm=intel -c src/kernel.c -o build/kernel.o

build/string.o: build src/string.c
	gcc -fno-pie -ffreestanding -m32 -c src/string.c -o build/string.o

build/kernel.bin: build/kernel.o build/string.o
	ld --oformat binary -Ttext 0x10000 -o build/kernel.bin --entry=kmain -m elf_i386 \
		build/kernel.o build/string.o

dist/os.img: dist build/kernel.bin build/bootloader.bin
	cat build/bootloader.bin build/kernel.bin > dist/os.img

run: dist/os.img
	-qemu-system-i386 dist/os.img

clean:
	-rm -rf build/* dist/*
