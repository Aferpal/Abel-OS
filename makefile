
.PHONY: all os boot clean

BUILD_DIR=build

BOOTLOADER_DIR=bootloader

BOOTLOADER1_BIN=$(BUILD_DIR)/boot/stage1.o

BOOTLOADER2_BIN=$(BUILD_DIR)/boot/stage2.o

OS_DIR=os

OS_BIN=$(BUILD_DIR)/os/kernel.bin

TARGET=disk.img

all: bootdisk

os:
	$(MAKE) -C $(OS_DIR)

boot:
	$(MAKE) -C $(BOOTLOADER_DIR)

bootdisk: os boot
	cat $(BOOTLOADER1_BIN) $(BOOTLOADER2_BIN) $(OS_BIN) > $(TARGET)

qemu:
	qemu-system-x86_64 -m 1G -drive format=raw,file=$(TARGET)
debug:
	qemu-system-i386 -drive format=raw,file=$(TARGET) -S -s
vqemu:
	qemu-system-i386 -d int,cpu_reset -drive format=raw,file=$(TARGET)
clean:
	$(MAKE) -C os/ clean
	$(MAKE) -C bootloader/ clean
