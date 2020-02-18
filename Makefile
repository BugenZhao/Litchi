IMG=out/litchi.img

all:
	make build
	make run

build:
	nasm src/boot.asm -i src/ -o out/boot.bin -l out/boot.lst 
	nasm src/bootable.asm -o out/bootable.bin
	dd if=/dev/zero of=$(IMG) bs=1024 count=1440
	dd if=out/bootable.bin of=$(IMG) bs=512 count=1 conv=notrunc
	dd if=out/boot.bin of=$(IMG) bs=512 count=1 conv=notrunc

run:
	make -C out/ run

debug:
	make -C out/ debug

clean:
	rm -f out/*.bin out/*.lst

clean_all:
	make clean
	rm -f out/*.img
