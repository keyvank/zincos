CC = i386-elf-gcc
CFLAGS = -ffreestanding -I.
LD = i386-elf-ld
AS = nasm
EMU = qemu-system-i386
EMUFLAGS = -m 128
C_FILES = $(shell find . -type f -name '*.c')
KERNEL_OBJECTS = kernel/entry.o ${C_FILES:.c=.o } cpu/interrupt.o

all: ./kernel/kernel.bin ./boot/bootloader.bin
	dd if=/dev/null of=./kernel/kernel.bin bs=512 count=0 seek=32 # Padding the kernel to 32 sectors
	cat ./boot/bootloader.bin ./kernel/kernel.bin > os.bin
	$(EMU) $(EMUFLAGS) -hda ./os.bin

./kernel/kernel.bin: $(KERNEL_OBJECTS)
	$(LD) -o $@ -T ./kernel/linker.ld $^

./boot/bootloader.bin: ./boot/bootloader.asm
	$(AS) -f bin ./boot/bootloader.asm -o ./boot/bootloader.bin

%.o: %.c
	$(CC) $(CFLAGS) -std=gnu99 -Wall -Wextra -c $^ -o $@

%.o: %.asm
	$(AS) $^ -f elf -o $@

clean:
	rm -rf ./boot/bootloader.bin
	rm -rf ./kernel/*.o
	rm -rf ./kernel/*.bin
	rm -rf ./os.bin
