nasm = nasm -f elf64
loop_devices= loop0 loop1
all: run
run: os.img
	echo "qemu-system-x86_64 -s -drive format=raw,file=os.img -serial stdio"
os.img: image/boot/kernel.bin image/boot/grub/grub.cfg
	dd if=/dev/zero of=build/os.img bs=512 count=32768
	echo "cpe454" | sudo -S parted build/os.img mklabel msdos
	echo "cpe454" | sudo -S parted build/os.img mkpart primary fat32 2048s 30720s
	echo "cpe454" | sudo -S parted build/os.img set 1 boot on
	# setup loop devices
	loop0 =$(shell losetup -f)
	echo loop0
	echo cpe454 | sudo -S losetup $(loop0) build/os.img
	loop1=losetup -f
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
