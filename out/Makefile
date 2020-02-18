QEMU		= qemu-system-i386
QEMU_ARGS	= -fda litchi.img -m 16
QEMU_ARGS_D	= boot.bin -m 16 -S -gdb stdio

run:
	$(QEMU) $(QEMU_ARGS)

debug:
	gdb -ex "target remote | $(QEMU) $(QEMU_ARGS_D)"
	
