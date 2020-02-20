IMG=out/litchi.img
FLOPPY=/Volumes/BZLITCHIOS/
LDR_BIN=out/LOADER.LIT

OS=
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Linux)
	OS = LINUX
endif
ifeq ($(UNAME_S), Darwin)
	OS = MACOS
endif

all:
	make init
	make build
	make run

init:
	dd if=/dev/zero of=$(IMG) bs=1024 count=1440

build:
	make boot
	make loader
	make kernel
	make image

boot:
	nasm src/boot.asm -i src/ -o out/boot.bin -l out/boot.lst 
	nasm src/bootable.asm -o out/bootable.bin

loader:
	nasm src/loader.asm -i src/ -o out/LOADER.LIT -l out/loader.lst

kernel:
	nasm src/kernel.asm -i src/ -o out/KERNEL.LIT -l out/kernel.lst

image: 
	dd if=out/bootable.bin of=$(IMG) bs=512 count=1 conv=notrunc
	dd if=out/boot.bin of=$(IMG) bs=512 count=1 conv=notrunc
	hdiutil mount $(IMG) 
	cp -v out/*.LIT $(FLOPPY)
	hdiutil detach $(FLOPPY)

run:
	make -C out/ run

debug:
	make -C out/ debug

clean:
	rm -f out/*.bin out/*.lst out/*.LIT
	rm -f out/*.img

ifeq ($(OS), MACOS)
os:
	@echo macOS!
else ifeq 
os:
	@echo GNU/Linux?
endif
