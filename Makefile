CC = i386-elf-gcc
CFLAGS = -ffreestanding -I.
LD = i386-elf-ld
AS = nasm
EMU = qemu-system-i386
EMUFLAGS = -m 128
KERNEL_OBJECTS =  ./kernel/entry.o ./kernel/kmain.o ./drivers/ports.o ./drivers/display/vga/vga_text.o

all: ./kernel/kernel.bin ./boot/bootloader.bin
	dd if=/dev/null of=./kernel/kernel.bin bs=512 count=0 seek=16 > /dev/null 2> /dev/null # Padding the kernel to 16 sectors
	cat ./boot/bootloader.bin ./kernel/kernel.bin > os.bin
	$(EMU) $(EMUFLAGS) -hda ./os.bin > /dev/null 2> /dev/null

./kernel/kernel.bin: $(KERNEL_OBJECTS)
	$(LD) -o $@ -T ./kernel/linker.ld $^

./boot/bootloader.bin: ./boot/bootloader.asm
	$(AS) -f bin ./boot/bootloader.asm -o ./boot/bootloader.bin

./kernel/entry.o: ./kernel/entry.asm
	$(AS) ./kernel/entry.asm -f elf -o ./kernel/entry.o

./kernel/%.o: ./kernel/%.c
	$(CC) $(CFLAGS) -std=gnu99 -Wall -Wextra -c $^ -o $@

clean:
	rm -rf ./boot/bootloader.bin
	rm -rf ./kernel/*.o
	rm -rf ./kernel/*.bin
	rm -rf ./os.bin
