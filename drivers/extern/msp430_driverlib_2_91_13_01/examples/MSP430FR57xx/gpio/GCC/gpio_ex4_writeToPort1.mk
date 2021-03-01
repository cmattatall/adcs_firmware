#==============================================================================
#
# Makefile for building MSP430 DriverLib Code examples from command line
# environment using the GCC Open Source Compiler for MSP430
#
#==============================================================================

DEVICE:=MSP430FR5739
EXAMPLE:=gpio_ex4_writeToPort1

###################### Windows OS ######################
ifeq ($(OS),Windows_NT)
	################## GCC Root Variable ###################
	GCC_DIR     ?= $(HOMEDRIVE)$(HOMEPATH)/ti/gcc
	GCC_MSP_INC_DIR ?= $(GCC_DIR)/include
	LDDIR       := $(GCC_MSP_INC_DIR)/$(DEVICE)
	RM          := rd /s /q
################### Linux or Mac OS ####################
else
	################## GCC Root Variable ###################
	GCC_DIR     ?= ~/ti/gcc
	GCC_MSP_INC_DIR ?= $(GCC_DIR)/include
	LDDIR       := $(GCC_MSP_INC_DIR)/$(shell echo $(DEVICE) | tr A-Z a-z)
	RM          := rm -rf
endif

######################################
GCC_BIN_DIR      ?= $(GCC_DIR)/bin
GCC_INC_DIR      ?= $(GCC_DIR)/msp430-elf/include
######################################
CC               := $(GCC_BIN_DIR)/msp430-elf-gcc
GDB	             := $(GCC_BIN_DIR)/msp430-elf-gdb
######################################
CFLAGS           := -O2 -D__$(DEVICE)__ -mmcu=$(DEVICE) -g -ffunction-sections -fdata-sections -DDEPRECATED
LDFLAGS          := -T $(LDDIR).ld -L $(GCC_MSP_INC_DIR) -mmcu=$(DEVICE) -g -Wl,--gc-sections
INCLUDES         := -I $(GCC_MSP_INC_DIR) -I $(GCC_INC_DIR) -I ../../../../driverlib/MSP430FR57xx -I ../..
######################################
DRIVERLIB_SRC    := $(wildcard ../../../../driverlib/MSP430FR57xx/*.c)
EXAMPLE_SRC      := ../$(EXAMPLE).c
SRC              := $(EXAMPLE_SRC) $(DRIVERLIB_SRC)
DRIVERLIB_OBJECTS:= $(patsubst %.c,%.o,$(DRIVERLIB_SRC))
EXOBJECT         := $(EXAMPLE)/$(EXAMPLE).o
EXOUTPUT         := $(EXAMPLE)/$(EXAMPLE).out
######################################

all: $(EXOUTPUT)

$(EXAMPLE):
	@mkdir $(EXAMPLE)

%.o: %.c
	@echo ============================================
	@echo Compiling $<
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

$(EXOBJECT): $(EXAMPLE_SRC)
	@echo ============================================
	@echo Compiling $<
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

$(EXOUTPUT): $(EXAMPLE) $(DRIVERLIB_OBJECTS) $(EXOBJECT)
	@echo ============================================
	@echo Linking objects and generating output binary
	$(CC) $(LDFLAGS) $(EXOBJECT) $(DRIVERLIB_OBJECTS) -o $@

debug: all
	$(GDB) $(EXAMPLE)/$(EXAMPLE).out

clean:
	@$(RM) $(EXAMPLE)
	@$(RM) $(DRIVERLIB_OBJECTS)
