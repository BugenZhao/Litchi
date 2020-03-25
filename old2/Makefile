#IMG    =./out/litchi.img
#TMPIMG = /tmp/litchi.img
#LITBIN =./out/LITCHI.LIT
#FLOPPY = /mnt/litchi

#CC  = gcc
#LD  = ld
#ASM = nasm
#
#C_FLAGS   = -c -Wall -m32 -ggdb -gstabs+ -nostdinc -fno-pic -fno-builtin -fno-stack-protector -I src/include
#LD_FLAGS  = -T src/scripts/kernel.ld -m elf_i386 -nostdlib
#ASM_FLAGS = -f elf -g -F stabs
#
#C_SOURCES = $(shell find src/ -name "*.c")
#C_OBJECTS = $(patsubst %.c, %.o, $(C_SOURCES))
#S_SOURCES = $(shell find src/ -name "*.s")
#S_OBJECTS = $(patsubst %.s, %.o, $(S_SOURCES))
# S_LISTFLS = $(patsubst %.asm, %.lst, $(S_SOURCES))

#all:
#	make build
#	make image
#
#build: $(S_OBJECTS) $(C_OBJECTS) link
#
#.s.o:
#	$(ASM) $(ASM_FLAGS) $<
#.c.o:
#	$(CC) $(C_FLAGS) $< -o $@
#link:
#	$(LD) $(LD_FLAGS) $(S_OBJECTS) $(C_OBJECTS) -o $(LITBIN)

# loader:
# 	$(ASM) $(ASM_FLAGS) src/nloader.asm -o out/nloader.bin -l out/nloader.lst
# 	$(CC) $(C_FLAGS) src/ckernel.c -o out/ckernel.o
# 	$(CC) $(C_FLAGS) src/print.c -o out/print.o
# 	$(LD) $(LD_FLAGS) out/nloader.bin out/ckernel.o out/print.o -o out/KERNEL.LIT

#image:
#	cp -v grub.img $(TMPIMG)
#	sudo mkdir -p $(FLOPPY)
#	sudo mount -o loop $(TMPIMG) $(FLOPPY)
#	sudo cp -v out/*.LIT $(FLOPPY)
#	sudo umount $(FLOPPY)
#	cp -v $(TMPIMG) out

run:
	make -C out/ run

debug:
	make -C out/ debug

#clean:
#	rm -f $(C_OBJECTS) $(S_OBJECTS) $(LITBIN) out/*.log
#
#clean_all:
#	make clean
#	rm -f $(IMG)
