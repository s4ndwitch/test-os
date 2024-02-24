#!/bin/bash

as --32 -o $1.o $1.S
ld --oformat binary -Ttext 0x7c00 -o $1.bin -m elf_i386 $1.o
qemu-system-i386 $1.bin
rm $1.o $1.bin
