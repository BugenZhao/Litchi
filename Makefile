IMG=out/litchi.img
FLOPPY=/Volumes/BZLITCHIOS/
LDR_BIN=out/LOADER.LIT

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Linux)
	OS = LINUX
endif
ifeq ($(UNAME_S), Darwin)
	OS = MACOS
endif

CC = gcc
LD = ld
ASM = nasm

C_FLAGS = -c -Wall -m32 -ggdb -gstabs+ -nostdinc -fno-pic -fno-builtin -fno-stack-protector
LD_FLAGS = -T kernel.ld -m elf_i386 -nostdlib
ASM_FLAGS = -f elf -g -F stabs

all:
	make s1
	make s2
	make run

s1:
	ssh bz-parallels "cd /media/psf/Litchi/Litchi; make build"

s2:
	make image

init:
	dd if=/dev/zero of=$(IMG) bs=1024 count=1440

build:
	make boot
	make loader
	make kernel

boot:
	nasm  -i src/ src/boot.asm -o out/boot.bin -l out/boot.lst 
	nasm  -i src/ src/bootable.asm -o out/bootable.bin

loader:
	nasm  -i src/ src/loader.asm -o out/LOADER.LIT -l out/loader.lst

kernel:
	nasm $(ASM_FLAGS) src/kernel.asm -i src/ -o out/kernel.bin -l out/kernel.lst
	gcc $(C_FLAGS) -o out/ckernel.o src/ckernel.c
	ld $(LD_FLAGS) -o out/KERNEL.LIT out/kernel.bin out/ckernel.o


ifeq ($(UNAME_S), Linux)
image:
	@echo Sorry
	# mkfs.msdos $(IMG)
	# dd if=out/bootable.bin of=$(IMG) bs=512 count=1 conv=notrunc
	# dd if=out/boot.bin of=$(IMG) bs=512 count=1 conv=notrunc

	# sudo mount -o loop out/litchi.img floppy
	# sudo cp -v out/*.LIT floppy
	# sudo umount floppy
endif
ifeq ($(UNAME_S), Darwin)
image:
	make init
	dd if=out/bootable.bin of=$(IMG) bs=512 count=1 conv=notrunc
	dd if=out/boot.bin of=$(IMG) bs=512 count=1 conv=notrunc
	hdiutil mount $(IMG) 
	cp -v out/*.LIT $(FLOPPY)
	hdiutil detach $(FLOPPY)
endif


run:
	make -C out/ run

debug:
	make -C out/ debug

clean:
	rm -f out/*.bin out/*.lst out/*.LIT out/*.o out/*.log
	rm -f out/*.img
