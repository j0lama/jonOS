#
# Makefile
#
# USPi - An USB driver for Raspberry Pi written in C
# Copyright (C) 2014-2018  R. Stange <rsta2@o2online.de>
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

USPIHOME = ..
RASPPI	?= 1
PREFIX	?= arm-none-eabi-

CC	= $(PREFIX)gcc
AS	= $(CC)
LD	= $(PREFIX)ld
AR	= $(PREFIX)ar

ARCH	?= -march=armv6j -mtune=arm1176jzf-s -mfloat-abi=hard 
TARGET	?= kernel

OPTIMIZE ?= -O2

AFLAGS	+= $(ARCH) -DRASPPI=$(RASPPI)
CFLAGS	+= $(ARCH) -Wall -Wno-psabi -fsigned-char -fno-builtin -nostdinc -nostdlib \
	   -std=gnu99 -undef -DRASPPI=$(RASPPI) -I include $(OPTIMIZE) #-DNDEBUG

OBJS	= uspilibrary.o \
	  dwhcidevice.o dwhciregister.o dwhcixferstagedata.o \
	  usbconfigparser.o usbdevice.o usbdevicefactory.o usbendpoint.o usbrequest.o usbstandardhub.o \
	  devicenameservice.o macaddress.o usbfunction.o smsc951x.o lan7800.o string.o util.o \
	  usbmassdevice.o \
	  dwhciframeschednper.o dwhciframeschedper.o keymap.o usbkeyboard.o \
	  dwhcirootport.o usbmouse.o \
	  dwhciframeschednsplit.o usbgamepad.o synchronize.o usbstring.o usbmidi.o

libuspi.a: $(OBJS)
	@echo "  AR    $@"
	@rm -f $@
	@$(AR) cr $@ $(OBJS)

%.o: %.S
	@echo "  AS    $@"
	@$(AS) $(AFLAGS) -c -o $@ $<

%.o: %.c
	@echo "  CC    $@"
	@$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.a *.elf *.lst *.img *.cir *.map *~ $(EXTRACLEAN)
