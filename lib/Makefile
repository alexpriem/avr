##### Customize the values as indicated below and :
##### make
##### make disasm 
##### make stats 
##### make hex
##### make writeflash
##### make gdbinit
##### or make clean
#####
##### See the http://electrons.psychogenic.com/ 
##### website for detailed instructions


#####              Configuration               #####
#####                                          #####
##### Customize the values in this section for #####
##### your project. MCU, PROJECTNAME and       #####
##### PRJSRC must be setup for all projects,   #####
##### the remaining variables are only         #####
##### relevant to those needing additional     #####
##### include dirs or libraries and those      #####
##### who wish to use the avrdude programmer   #####
# Name of target controller 
# (e.g. 'at90s8515', see the available avr-gcc mmcu 
# options for possible values)
MCU=atmega32

# id to use with programmer
# default: PROGRAMMER_MCU=$(MCU)
# In case the programer used, e.g avrdude, doesn't
# accept the same MCU name as avr-gcc (for example
# for ATmega8s, avr-gcc expects 'atmega8' and 
# avrdude requires 'm8')
PROGRAMMER_MCU=m32

# Name of our project
# (use a single word, e.g. 'myproject')
PROJECTNAME=lib

# Source files
# List C/C++/Assembly source files:
# (list all files to compile, e.g. 'a.c b.cpp as.S'):
# Use .cc, .cpp or .C suffix for C++ files, use .S 
# (NOT .s !!!) for assembly source code files.
#PRJSRC=main.c uart.c delay.c leds.c  bitops.c mcp3202.c mcp4922.c \
#		scpi.c scpi_cmds.c hv_supply.c
PRJSRC=bitops.c cat4016 delay.c keypad.c leds.c \
		mcp3201.c mcp3202.c mcp3204.c mcp4922.c keypad.c  uart.c  
		 
		

		
# additional includes (e.g. -I/path/to/mydir)
INC=-Ie:\avr\include -Ie:\avr\lib

# libraries to link in (e.g. -lmylib)
LIBS=-Le:\avr\lib 

# Optimization level, 
# use s (size opt), 1, 2, 3 or 0 (off)
OPTLEVEL=0


#####      AVR Dude 'writeflash' options       #####
#####  If you are using the avrdude program
#####  (http://www.bsdhome.com/avrdude/) to write
#####  to the MCU, you can set the following config
#####  options and use 'make writeflash' to program
#####  the device.


# programmer id--check the avrdude for complete list
# of available opts.  These should include stk500,
# avr910, avrisp, bsd, pony and more.  Set this to
# one of the valid "-c PROGRAMMER-ID" values 
# described in the avrdude info page.
# 
AVRDUDE_PROGRAMMERID=stk500v2

# port--serial or parallel port to which your 
# hardware programmer is attached
#
#AVRDUDE_PORT=/dev/ttyS0
AVRDUDE_PORT= com4

# PROG is com2


####################################################
#####                Config Done               #####
#####                                          #####
##### You shouldn't need to edit anything      #####
##### below to use the makefile but may wish   #####
##### to override a few of the flags           #####
##### nonetheless                              #####
#####                                          #####
####################################################


##### Flags ####

# HEXFORMAT -- format for .hex file output
HEXFORMAT=ihex

# compiler
CFLAGS=-I. $(INC) -g -mmcu=$(MCU) -O$(OPTLEVEL) \
	-fpack-struct -fshort-enums             \
	-funsigned-bitfields -funsigned-char    \
	-Wall -Wstrict-prototypes               \
	-Wa,-ahlms=$(firstword                  \
	$(filter %.lst, $(<:.c=.lst)))

# c++ specific flags
CPPFLAGS=-fno-exceptions               \
	-Wa,-ahlms=$(firstword         \
	$(filter %.lst, $(<:.cpp=.lst))\
	$(filter %.lst, $(<:.cc=.lst)) \
	$(filter %.lst, $(<:.C=.lst)))

# assembler
ASMFLAGS =-I. $(INC) -mmcu=$(MCU)        \
	-x assembler-with-cpp            \
	-Wa,-gstabs,-ahlms=$(firstword   \
		$(<:.S=.lst) $(<.s=.lst))


# linker
LDFLAGS=-Wl,-Map,$(TRG).map -mmcu=$(MCU) $(LIBS)

##### executables ####
CC=avr-gcc
OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump
SIZE=avr-size
AVRDUDE=avrdude
REMOVE=rm -f

##### automatic target names ####
TRG=$(PROJECTNAME).out
DUMPTRG=$(PROJECTNAME).s

HEXROMTRG=$(PROJECTNAME).hex 
HEXTRG=$(HEXROMTRG) $(PROJECTNAME).ee.hex
GDBINITFILE=gdbinit-$(PROJECTNAME)

# Define all object files.

# Start by splitting source files by type
#  C++
CPPFILES=$(filter %.cpp, $(PRJSRC))
CCFILES=$(filter %.cc, $(PRJSRC))
BIGCFILES=$(filter %.C, $(PRJSRC))
#  C
CFILES=$(filter %.c, $(PRJSRC))
#  Assembly
ASMFILES=$(filter %.S, $(PRJSRC))


# List all object files we need to create
OBJDEPS=$(CFILES:.c=.o)    \
	$(CPPFILES:.cpp=.o)\
	$(BIGCFILES:.C=.o) \
	$(CCFILES:.cc=.o)  \
	$(ASMFILES:.S=.o)
	
LIBDEPS=$(CFILES:.c=.a) 

# Define all lst files.
LST=$(filter %.lst, $(OBJDEPS:.o=.lst))

# All the possible generated assembly 
# files (.s files)
GENASMFILES=$(filter %.s, $(OBJDEPS:.o=.s)) 


.SUFFIXES : .c .cc .cpp .C .o .out .s .S \
	.hex .ee.hex .h .hh .hpp


.PHONY: writeflash clean stats gdbinit stats

# Make targets:
# all, disasm, stats, hex, writeflash/install, clean

#lib: uart.a uart.o
	

all: bitops cat4016 delay hc595 hc595p keypad leds mcp3201 mcp3202 mcp3204 mcp4922  uart \
	lcd_serial lcd_4_40x4 rot_encoder

#disasm: $(DUMPTRG) stats

stats: $(TRG)
	$(OBJDUMP) -h $(TRG)
	$(SIZE) $(TRG) 

bitops: bitops.o
	avr-ar rcs libbitops.a bitops.o

cat4016: cat4016.o
	avr-ar rcs libcat4016.a cat4016.o
	
delay: delay.o
	avr-ar rcs libdelay.a delay.o
	
hc595:hc595.o
	avr-ar rcs libhc595.a hc595.o

hc595p:hc595p.o
	avr-ar rcs libhc595p.a hc595p.o
	
keypad: keypad.o
	avr-ar rcs libkeypad.a keypad.o
	
lcd: lcd.o
	avr-ar rcs liblcd.a lcd.o

lcd_serial: lcd_serial.o
	avr-ar rcs liblcd_serial.a lcd_serial.o

lcd_4_40x4: lcd_4_40x4.o
	avr-ar rcs liblcd_4_40x4.a lcd_4_40x4.o
	
leds: leds.o
	avr-ar rcs libleds.a leds.o

mcp3201: mcp3201.o
	avr-ar rcs libmcp3201.a mcp3201.o

mcp3202: mcp3202.o
	avr-ar rcs libmcp3202.a mcp3202.o

mcp3204: mcp3204.o
	avr-ar rcs libmcp3204.a mcp3204.o

mcp4922: mcp4922.o
	avr-ar rcs libmcp4922.a mcp4922.o

rot_encoder: rot_encoder.o
	avr-ar rcs librot_encoder.a rot_encoder.o
	
uart: uart.o
	avr-ar rcs libuart.a uart.o

	


$(DUMPTRG): $(TRG) 
	$(OBJDUMP) -S  $< > $@


$(TRG): $(OBJDEPS) 
	$(CC) $(LDFLAGS) -o $(TRG) $(OBJDEPS)
	$(AR) rcs $(LIBDEPS) $(OBJDEPS)


#### Generating assembly ####
# asm from C
%.s: %.c
	$(CC) -S $(CFLAGS) $< -o $@

# asm from (hand coded) asm
%.s: %.S
	$(CC) -S $(ASMFLAGS) $< > $@


# asm from C++
.cpp.s .cc.s .C.s :
	$(CC) -S $(CFLAGS) $< -o $@



#### Generating object files ####
# object from C
.c.o: 
	$(CC) $(CFLAGS) -c $< -o $@


# object from asm
.S.o :
	$(CC) $(ASMFLAGS) -c $< -o $@


#####  Generating a gdb initialisation file    #####
##### Use by launching simulavr and avr-gdb:   #####
#####   avr-gdb -x gdbinit-myproject           #####
gdbinit: $(GDBINITFILE)

$(GDBINITFILE): $(TRG)
	@echo "file $(TRG)" > $(GDBINITFILE)
	
	@echo "target remote localhost:1212" \
		                >> $(GDBINITFILE)
	
	@echo "load"        >> $(GDBINITFILE) 
	@echo "break main"  >> $(GDBINITFILE)
	@echo "continue"    >> $(GDBINITFILE)
	@echo
	@echo "Use 'avr-gdb -x $(GDBINITFILE)'"


#### Cleanup ####
clean:
#$(REMOVE) $(TRG) $(TRG).map $(DUMPTRG)
	$(REMOVE) $(OBJDEPS)
	$(REMOVE) $(LST) $(GDBINITFILE)
	$(REMOVE) $(GENASMFILES)
	


#####                    EOF                   #####

