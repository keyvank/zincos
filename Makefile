AS=nasm
AS_FLAGS=-f bin
EMU=qemu-system-i386
EMU_FLAGS=-m 128

all: ./boot/bootloader.asm
	$(AS) $(AS_FLAGS) ./boot/bootloader.asm -o ./boot/bootloader
	$(EMU) $(EMU_FLAGS) -hda ./boot/bootloader

clean:
	rm ./boot/bootloader
