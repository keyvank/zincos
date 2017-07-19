CC = i386-elf-gcc
CFLAGS = -ffreestanding -I. -std=gnu99 -Wall -Wextra -Werror
CPPC = i386-elf-c++
CPPFLAGS = -ffreestanding -I. -std=c++11 -Wall -Wextra -Werror -fno-exceptions -fno-rtti
LD = i386-elf-ld
AS = nasm
EMU = qemu-system-i386
EMUFLAGS = -m 1024
C_FILES = $(shell find . -type f -name '*.c')
CPP_FILES = $(shell find . -type f -name '*.cpp')
KERNEL_OBJECTS = kernel/entry.o ${CPP_FILES:.cpp=.o } ${C_FILES:.c=.o } cpu/interrupt.o cpu/asmutil.o

all: ./kernel/kernel.bin ./boot/bootloader.bin ./boot/loader.bin
	dd if=/dev/null of=./kernel/kernel.bin bs=512 count=0 seek=32 # Padding the kernel to 32 sectors
	cat ./boot/bootloader.bin ./boot/loader.bin ./kernel/kernel.bin > os.bin
	$(EMU) $(EMUFLAGS) -hda ./os.bin

./kernel/kernel.bin: $(KERNEL_OBJECTS)
	$(LD) -o $@ -T ./kernel/linker.ld $^

./boot/bootloader.bin: ./boot/bootloader.asm
	$(AS) -f bin ./boot/bootloader.asm -o ./boot/bootloader.bin

./boot/loader.bin: ./boot/loader.asm
	$(AS) -f bin ./boot/loader.asm -o ./boot/loader.bin

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

%.o: %.cpp
	$(CPPC) $(CPPFLAGS) -c $^ -o $@

%.o: %.asm
	$(AS) $^ -f elf -o $@

clean:
	rm -rf $(shell find . -type f -name '*.o')
	rm -rf $(shell find . -type f -name '*.bin')
