QEMU		= qemu-system-i386
QEMU_ARGS	= -drive file=litchi.img,format=raw,if=floppy,index=0 -m 64 -D qemu.log -monitor stdio
QEMU_ARGS_D_A	= -drive file=litchi.img,format=raw,if=floppy,index=0 -m 64 -D qemu.log -S -gdb stdio

run:
	$(QEMU) $(QEMU_ARGS)

debug:
	gdb -ex "target remote | $(QEMU) $(QEMU_ARGS) $(QEMU_ARGS_D_A)"
	
