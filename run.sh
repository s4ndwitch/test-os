#!/bin/bash

as --32 bootloader.S -o bootloader.o
ld --oformat binary -Ttext 0x7c00 -o bootloader.bin -m elf_i386 bootloader.o

#as --32 kernel.S -o kernel.o
#ld --oformat binary -Ttext 0x1000 -o kernel.bin -m elf_i386 kernel.o

gcc -fno-pie -ffreestanding -m32 -c kernel.c -o kernel.o
ld --oformat binary -Ttext 0x10000 -o kernel.bin --entry=kmain -m elf_i386 kernel.o

cat bootloader.bin kernel.bin > os-image
qemu-system-i386 os-image
rm bootloader.o bootloader.bin kernel.o kernel.bin os-image
