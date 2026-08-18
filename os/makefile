BUILD_DIR = ../build/os

OBJ_DIR = $(BUILD_DIR)/obj

SRC_DIR = ./src

INC_DIR = ./include

C_SOURCES = $(shell find $(SRC_DIR) -iname "*.c")

ASM_SOURCES = $(shell find $(SRC_DIR) -iname "*.asm")

C_OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(C_SOURCES))

ASM_OBJECTS := $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%.o, $(ASM_SOURCES))

ALL_OBJECTS := $(C_OBJECTS) $(ASM_OBJECTS)

TARGET = kernel

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@gcc -I$(INC_DIR) -m32 -ffreestanding -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -g -O0 -c $< -o $@
	@echo " [1] CC $< -o $@"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.asm
	@mkdir -p $(dir $@)
	@nasm -f elf32 $< -o $@
	@echo " [2] NASM $< -o $@"

all: $(ALL_OBJECTS)
	@mkdir -p $(BUILD_DIR)
	@echo ""
	@ld -m elf_i386 -T linker.ld $(ALL_OBJECTS) -o $(BUILD_DIR)/$(TARGET).elf
	@echo " [3] LD ALL_OBJECTS"
	objcopy -O binary $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).bin	

clean:
	rm -rf $(BUILD_DIR)
