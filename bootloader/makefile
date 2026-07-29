BUILD_DIR = ../build/boot

STAGE1=stage1
STAGE2=stage2

all:
	mkdir -p $(BUILD_DIR)
	nasm -f bin $(STAGE2).asm -o $(BUILD_DIR)/$(STAGE2).o
	nasm -f bin $(STAGE1).asm -o $(BUILD_DIR)/$(STAGE1).o

$(BUILD_DIR)/%.o: %.asm
	nasm -f bin $< -o $@

clean:
	rm -rf $(BUILD_DIR)
