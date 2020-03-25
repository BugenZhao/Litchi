QEMU		= qemu-system-i386
QEMU_ARGS	= -drive file=litchi.img,format=raw,if=floppy,index=0 -m 64 -monitor stdio
QEMU_ARGS_D	= -drive file=litchi.img,format=raw,if=floppy,index=0 -m 64 -S -gdb stdio

GDB			= cgdb

run:
	$(QEMU) $(QEMU_ARGS)

debug:
	$(GDB) -ex "file LITCHI.LIT" -ex "target remote | $(QEMU) $(QEMU_ARGS_D)" -ex "b kernel_entry" -ex "c"
	
