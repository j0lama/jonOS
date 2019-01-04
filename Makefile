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
OBJECT_DIRECTORY = objects/

# Directory for the source code
SOURCE_DIRECTORY = source/

# Directory of the header files
INCLUDE_DIRECTORY = include/

# Directory with sources for ethernet support
EXT_LIB_DIRECTORY = external/

# USPi Library with enviroment
USPi = $(EXT_LIB_DIRECTORY)libuspi.a $(EXT_LIB_DIRECTORY)libuspienv.a

# Name of the output image file
TARGET = $(NAME).img

# Name of the map file
MAP_FILE = $(NAME).map

# Name of the linker script used
LINKER_FILE = linker.ld

# Flags for compilation
FLAGS_C = -mcpu=arm1176jzf-s -fpic -ffreestanding -std=gnu99 -c -g -O0 -Wall -Wextra -pedantic -mfloat-abi=hard
FLAGS_S = -mcpu=arm1176jzf-s -fpic -ffreestanding -c -mfloat-abi=hard

# Listing source files
CFILES = $(wildcard $(SOURCE_DIRECTORY)*.c)
SFILES = $(wildcard $(SOURCE_DIRECTORY)*.s)

# Names of the objects that must be generated in the compilation
# fucntion patsubst to search patterns and replace text
OBJECTS = $(patsubst $(SOURCE_DIRECTORY)%.c, $(OBJECT_DIRECTORY)%.o, $(CFILES)) $(patsubst $(SOURCE_DIRECTORY)%.s, $(OBJECT_DIRECTORY)%.o, $(SFILES))

# Make everything
all: $(TARGET)

# Rule to build the image file
$(TARGET): $(BUILD_DIRECTORY) $(OBJECT_DIRECTORY) $(BUILD_DIRECTORY)$(ELF)
	@echo "Building image file..."
	@$(ARM_TOOLCHAIN)-objcopy $(BUILD_DIRECTORY)$(ELF) -O binary $(TARGET)
	@echo "Compilation done"

# Rule to build the elf file
$(BUILD_DIRECTORY)$(ELF): $(OBJECTS)
	@echo "Linking objects..."
	@$(ARM_TOOLCHAIN)-ld --no-undefined $(OBJECTS) $(USPi) -Map $(MAP_FILE) -o $(BUILD_DIRECTORY)$(ELF) -T $(LINKER_FILE)

# Build .c files
$(OBJECT_DIRECTORY)%.o: $(SOURCE_DIRECTORY)%.c
	@echo "Building $@..."
	@$(ARM_TOOLCHAIN)-gcc -I $(INCLUDE_DIRECTORY) $(FLAGS_C) $< -o $@

#Build the assembler files
$(OBJECT_DIRECTORY)%.o: $(SOURCE_DIRECTORY)%.s
	@echo "Building $@..."
	@$(ARM_TOOLCHAIN)-gcc -I $(INCLUDE_DIRECTORY) $(FLAGS_S) $< -o $@

$(BUILD_DIRECTORY):
	@mkdir build

$(OBJECT_DIRECTORY):
	@mkdir objects


.PHONY: clean

# Rule to clean all
clean:
	@rm -f $(TARGET) $(MAP_FILE) jonOS.img
	@rm -rf $(BUILD_DIRECTORY) $(OBJECT_DIRECTORY)
	@echo "Clean done"

# Run a HTTP Server to send the kernel to the emulator
run: $(TARGET)
	@python3 -m http.server
	# target remote localhost:1234
	# file test.elf






