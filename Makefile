

#Some of the package-sets for AVR development are trash.  At least the Arduino
#	IDE includes a useful set of working tools and libraries.
ARDUINO_BASE=/usr/local/lib/arduino-1.6.9
AVR_TOOLS=${ARDUINO_BASE}/hardware/tools/avr
BIN=${AVR_TOOLS}/bin

CC=${BIN}/avr-gcc
STRIP=${BIN}/avr-strip
SIZE=${BIN}/avr-size
OBJCOPY=${BIN}/avr-objcopy
AVRDUDE=${BIN}/avrdude

%.hex: %
	${OBJCOPY} -j .text -j .data -O ihex  $< $@

#Make sure you do ISP or something and mess up the flags before you're 
#entirely done with the chip, you'll need to reprogram it again with a 
#high-voltage programmer.  May as well just set your programmer up for that
#to start out.
AVRDUDEFLAGS=-C ${ADCONFIG} -c ${PROGRAMMER} -p ${PART} -P ${PORT}

INCLUDES=-I. -I${AVR_TOOLS}/include

#For smallest size
#OPTIMIZE=-Os -mcall-prologues
OPTIMIZE=-Os
#Faster execution
#OPTIMIZE=-O2
#Debug
#OPTIMIZE=-g

CFLAGS=-mmcu=${AVR_MTYPE} -DF_CPU=${AVR_FCPU} ${OPTIMIZE} ${INCLUDES}
LDFLAGS=-mmcu=${AVR_MTYPE}


#Machine type for GCC
AVR_MTYPE=attiny2313
#AVRDude configuration:
ADCONFIG=${AVR_TOOLS}/etc/avrdude.conf
PART=${AVR_MTYPE}
PROGRAMMER=dragon_pp
PORT=usb

#Clock at 4Mhz, divider on, which gives us 500Khz.
AVR_FCPU=500000UL

#4Mhz
#AVR_FCPU=4000000UL



all: genesis size strip genesis.hex

genesis.hex: genesis

genesis: genesis.o

.PHONY: size strip clean flash fuses

strip: 
	${STRIP} genesis

size: 
	${SIZE} genesis

flash: genesis.hex 
	${AVRDUDE} ${AVRDUDEFLAGS} -U flash:w:genesis.hex


#Reset disabled, internal clock at 8Mhz, clock divider/8 set on.
#Brown-out detection level= 2.7V
fuses:
#500Khz
	${AVRDUDE} ${AVRDUDEFLAGS} -U lfuse:w:0x42:m -U hfuse:w:0xfa:m -U efuse:w:0xff:m 
#4Mhz
#	${AVRDUDE} ${AVRDUDEFLAGS} -U lfuse:w:0xc2:m -U hfuse:w:0xfb:m -U efuse:w:0xff:m 

clean:
	rm *.o genesis genesis.hex


