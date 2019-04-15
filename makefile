nasm = nasm -f elf64
loop0 = /dev/loop62
loop1 = /dev/loop63
all: run
run: os.img
	echo "qemu-system-x86_64 -s -drive format=raw,file=os.img -serial stdio"
os.img: image/boot/kernel.bin image/boot/grub/grub.cfg
	dd if=/dev/zero of=build/os.img bs=512 count=32768
	echo "cpe454" | sudo -S parted build/os.img mklabel msdos
	echo "cpe454" | sudo -S parted build/os.img mkpart primary fat32 2048s 30720s
	echo "cpe454" | sudo -S parted build/os.img set 1 boot on
	# setup loop devices
	echo loop0
	echo cpe454 | sudo -S losetup $(loop0) build/os.img
	echo "cpe454" | sudo -S losetup $(loop1) build/os.img -o 1048576
	# install grub
	echo "cpe454" | sudo -S mkdosfs -F32 -f 2 $(loop1)
	echo "cpe454" | sudo -S mount $(loop1) /mnt/fatgrub
	echo "cpe454" | sudo -S grub-install --root-directory=/mnt/fatgrub \
		--no-floppy --modules="normal part_msdos ext2 multiboot" $(loop0)
	echo "cpe454" | sudo -S cp -r image/* /mnt/fatgrub
	# cleanup
	echo "cpe454" | sudo -S umount /mnt/fatgrub
	echo "cpe454" | sudo -S losetup -d $(loop0)
	echo "cpe454" | sudo -S losetup -d $(loop1)

image/boot/kernel.bin:
	nasm -f elf64 build/multiboot_header.asm
	nasm -f elf64 build/long_mode_init.asm
	nasm -f elf64 build/boot.asm
	ld -n -o image/boot/kernel.bin -T build/linker.ld build/multiboot_header.o \
		build/boot.o build/long_mode_init.o
