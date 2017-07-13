all:
	nasm -f bin ./boot/bootloader.asm -o ./boot/bootloader.bin

	i386-elf-gcc -ffreestanding -c ./kernel/main.c -o ./kernel/main.o
	nasm ./kernel/entry.asm -f elf -o ./kernel/entry.o
	i386-elf-gcc -ffreestanding -c ./drivers/ports.c -o ./drivers/ports.o
	i386-elf-ld -o ./kernel/kernel.bin -Ttext 0x1000 ./kernel/entry.o ./kernel/main.o ./drivers/ports.o --oformat binary
	dd if=/dev/null of=./kernel/kernel.bin bs=1 count=0 seek=1024 # Padding the kernel to 1024 bytes
	cat ./boot/bootloader.bin ./kernel/kernel.bin > os.bin
	qemu-system-i386 -m 128 -hda ./os.bin

clean:
	rm ./boot/bootloader.bin
	rm ./kernel/*.o
	rm ./kernel/*.bin
	rm ./os.bin
