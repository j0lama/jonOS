# Makefile script for generate Raspberry Pi images.
# Author: Jon Larrea
# Date: 08/10/2018

# The toolchain used to compile for the Raspberry Pi
ARM_TOOLCHAIN ?= arm-none-eabi

# Directory of the header files
INCLUDE_DIRECTORY = ../include/

# Flags for compilation
FLAGS_C = -mcpu=arm1176jzf-s -fpic -ffreestanding -std=gnu99 -c -O2 -Wall -Wextra -pedantic -r

all:
	@echo "Building binary..."
	@$(ARM_TOOLCHAIN)-gcc -I $(INCLUDE_DIRECTORY) $(FLAGS_C) -c function.c -o function.o

# Rule to clean all
clean:
	@rm -f function.o
	@echo "Clean done"