CC = i386-elf-gcc
CFLAGS = -ffreestanding
LD = i386-elf-ld
AS = nasm
EMU = qemu-system-i386
EMUFLAGS = -m 128
KERNEL_OBJECTS = ./kernel/entry.o ./kernel/kmain.o ./drivers/ports.o

all: ./kernel/kernel.bin ./boot/bootloader.bin
	dd if=/dev/null of=./kernel/kernel.bin bs=1 count=0 seek=1024 # Padding the kernel to 1024 bytes
	cat ./boot/bootloader.bin ./kernel/kernel.bin > os.bin
	$(EMU) $(EMUFLAGS) -hda ./os.bin

./kernel/kernel.bin: $(KERNEL_OBJECTS)
	$(LD) -o $@ -Ttext 0x1000 $^ --oformat binary

./boot/bootloader.bin: ./boot/bootloader.asm
	$(AS) -f bin ./boot/bootloader.asm -o ./boot/bootloader.bin

./kernel/entry.o: ./kernel/entry.asm
	$(AS) ./kernel/entry.asm -f elf -o ./kernel/entry.o

./kernel/%.o: ./kernel/%.c
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -rf ./boot/bootloader.bin
	rm -rf ./kernel/*.o
	rm -rf ./kernel/*.bin
	rm -rf ./os.bin
