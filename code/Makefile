# Name: Makefile
# Author: Randy Glenn <randy@surrealitylabs.com
# Copyright: 2018. All rights reserved.
# License: GPL 3.0

# This is a prototype Makefile. Modify it according to your needs.
# You should at least check the settings for
# DEVICE ....... The AVR device you compile for
# CLOCK ........ Target AVR clock rate in Hertz
# OBJECTS ...... The object files created from your source files. This list is
#                usually the same as the list of source files with suffix ".o".
# PROGRAMMER ... Options to avrdude which define the hardware you use for
#                uploading to the AVR and the interface where this hardware
#                is connected.
# FUSES ........ Parameters for avrdude to flash the fuses appropriately.

DEVICE     = attiny4313
CLOCK      = 8000000
PROGRAMMER = -c avrisp2
OBJECTS    = main.o ui.o demo.o game.o highscore.o tone.o
FUSES	   = -U lfuse:w:0xe4:m -U hfuse:w:0xdf:m -U efuse:w:0x01:m

AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE) -B 10 -F

COMPILE = avr-gcc -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)

# symbolic targets:
all:	binarygame.hex	size

main.o:

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

.c.s:
	$(COMPILE) -S $< -o $@

flash:	binarygame.hex  size
	$(AVRDUDE) -U flash:w:binarygame.hex:i

fuse:
	$(AVRDUDE) $(FUSES)

size:	binarygame.elf
	avr-size --mcu=$(DEVICE) --format=avr binarygame.elf

ident:
	$(AVRDUDE)

clean:
	rm -f $(OBJECTS)
	rm -f binarygame.hex binarygame.elf binarygame.map

# file targets:
binarygame.elf: $(OBJECTS)
	$(COMPILE) -o binarygame.elf -Wl,-Map,binarygame.map $(OBJECTS)

binarygame.hex: binarygame.elf
	rm -f binarygame.hex
	avr-objcopy -j .text -j .data -O ihex binarygame.elf binarygame.hex
