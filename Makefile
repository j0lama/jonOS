# Makefile script for generate Raspberry Pi images.
# Author: Jon Larrea
# Date: 08/10/2018

# The toolchain used to compile for the Raspberry Pi
ARM_TOOLCHAIN ?= arm-none-eabi

# OS Name
NAME = kernel

# ELF Name
ELF = $(NAME).elf

# Directory for compiled objects
BUILD_DIRECTORY = build/

# Directory for compiled objects
OBJ = objects/

# Directory for the source code
SOURCE_DIRECTORY = source/

# Directory of the header files
INCLUDE_DIRECTORY = include/

# Name of the output image file
TARGET = $(NAME).img

# Name of the map file
MAP_FILE = $(NAME).map

# Name of the linker script used
LINKER_FILE = linker.ld

# OS Name
UNAME := $(shell uname)

# Flags for compilation
FLAGS_C = -mcpu=arm1176jzf-s -fpic -ffreestanding -std=gnu99 -c -O2 -Wall -Wextra -pedantic
FLAGS_S = -mcpu=arm1176jzf-s -fpic -ffreestanding -c

# Names of the objects that must be generated in the compilation
OBJECTS = 	$(OBJ)boot.o $(OBJ)io.o $(OBJ)uart.o $(OBJ)malloc.o $(OBJ)kernel.o

# Make everything
all: $(TARGET)

# Rule to build the image file
$(TARGET): $(BUILD_DIRECTORY) $(OBJ) $(BUILD_DIRECTORY)$(ELF)
	@echo "Building image file..."
	@$(ARM_TOOLCHAIN)-objcopy $(BUILD_DIRECTORY)$(ELF) -O binary $(TARGET)
	@echo "Compilation done"
	@ls

# Rule to build the elf file
$(BUILD_DIRECTORY)$(ELF): $(OBJECTS)
	@echo "Linking objects..."
	@$(ARM_TOOLCHAIN)-ld --no-undefined $(OBJECTS) -Map $(MAP_FILE) -o $(BUILD_DIRECTORY)$(ELF) -T $(LINKER_FILE)

# Build .c files
$(OBJ)%.o: $(SOURCE_DIRECTORY)%.c
	@echo "Building $@..."
	@$(ARM_TOOLCHAIN)-gcc -I $(INCLUDE_DIRECTORY) $(FLAGS_C) $< -o $@

#Build the assembler files
$(OBJ)%.o: $(SOURCE_DIRECTORY)%.s
	@echo "Building $@..."
	@$(ARM_TOOLCHAIN)-gcc -I $(INCLUDE_DIRECTORY) $(FLAGS_S) $< -o $@

$(BUILD_DIRECTORY):
	@mkdir build

$(OBJ):
	@mkdir objects

# Rule to clean all
clean:
	@rm -f $(TARGET) $(MAP_FILE) jonOS.img
	@rm -rf $(BUILD_DIRECTORY) $(OBJ)
	@echo "Clean done"

# Compile and run in QEMU
run: $(TARGET)
	qemu-system-arm -m 512 -M raspi2 -serial stdio -kernel $(BUILD_DIRECTORY)$(ELF)





