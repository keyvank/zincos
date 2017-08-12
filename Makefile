CC = i386-elf-gcc
CFLAGS = -ffreestanding -I. -std=gnu99 -Wall -Wextra -Werror
CPPC = i386-elf-c++
CPPFLAGS = -ffreestanding -I. -std=c++11 -Wall -Wextra -Werror -fno-exceptions -fno-rtti
LD = i386-elf-ld
AS = nasm
EMU = qemu-system-i386
EMUFLAGS = -m 1024
C_FILES = $(shell find . -type f -name '*.c' -not -path "./user/*")
CPP_FILES = $(shell find . -type f -name '*.cpp' -not -path "./user/*")
KERNEL_OBJECTS = kernel/entry.o ${CPP_FILES:.cpp=.o } ${C_FILES:.c=.o } cpu/interrupt.o cpu/asmutil.o drivers/diskasm.o

all: ./kernel/kernel.bin ./boot/bootloader.bin ./boot/loader.bin ./user/apps.bin
	dd if=/dev/null of=./kernel/kernel.bin bs=512 count=0 seek=112 # Padding the kernel to 112 (0x70) sectors (56 KB)
	dd if=/dev/null of=./user/apps.bin bs=512 count=0 seek=64 # Padding the apps to 64 sectors
	cat ./boot/bootloader.bin ./boot/loader.bin ./kernel/kernel.bin ./user/apps.bin > os.bin
	$(EMU) $(EMUFLAGS) -hda ./os.bin

./user/apps.bin: ./user/entry.o ./user/api.o ./user/hello_world.o ./user/shell.o
	$(LD) -o ./user/hello_world.bin -Ttext 0x800000 --oformat binary ./user/entry.o ./user/api.o ./user/hello_world.o
	dd if=/dev/null of=./user/hello_world.bin bs=512 count=0 seek=1 # Padding to 1 sectors
	$(LD) -o ./user/shell.bin -Ttext 0x800000 --oformat binary ./user/entry.o ./user/api.o ./user/shell.o
	dd if=/dev/null of=./user/shell.bin bs=512 count=0 seek=1 # Padding to 1 sectors
	cat ./user/hello_world.bin ./user/shell.bin > ./user/apps.bin

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
