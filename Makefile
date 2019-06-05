ARMGNU ?= aarch64-linux-gnu

COPS = -Wall -nostdlib -nostartfiles -ffreestanding -Iinclude 
ASMOPS = -Iinclude

BUILD_DIR = build
SRC_DIR = src

ifeq ($(TYPE),hex)
MAIN_DIR = hex
hex : kernel8.img raspiloader-hex
else
MAIN_DIR = img
all : kernel8.img raspiloader-img
endif

clean:
		rm -rf $(BUILD_DIR) *.img raspiloader-*

$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
		mkdir -p $(@D)
		$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@
		
$(BUILD_DIR)/%_c.o: $(MAIN_DIR)/%.c
		$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
		$(ARMGNU)-gcc $(ASMOPS) -MMD -c $< -o $@

C_FILES = $(wildcard $(SRC_DIR)/*.c)
C_FILES1 = $(wildcard $(MAIN_DIR)/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)/*.S)
OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
OBJ_FILES += $(C_FILES1:$(MAIN_DIR)/%.c=$(BUILD_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)

DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)

kernel8.img: $(SRC_DIR)/linker.ld $(OBJ_FILES)
	$(ARMGNU)-ld -T $(SRC_DIR)/linker.ld -o $(BUILD_DIR)/kernel8.elf  $(OBJ_FILES)
	$(ARMGNU)-objcopy $(BUILD_DIR)/kernel8.elf -O binary kernel8.img

raspiloader-img:img/downloader/*.c
	gcc  img/downloader/*.c -o $@
    
raspiloader-hex:hex/downloader/*.c
	gcc hex/downloader/*.c -o $@


