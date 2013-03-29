#-------------------------------------------
# Makefile for STM32F4xx targets
# Author: (C) Emil Fresk
#-------------------------------------------

WHEREAMI := $(dir $(lastword $(MAKEFILE_LIST)))
TOP      := $(realpath $(WHEREAMI)/../../../)

# Target same	
TARGET ?= kfly

# Verbose
V0 = @
V1 =

# Where the build will be located and create the folder
OBJDIR = ./build
ifdef ComSpec
$(shell md $(subst /,\\,$(OBJDIR)) 2>NUL)
else
$(shell mkdir -p $(OBJDIR) 2>/dev/null)
endif

# External high speed crystal frequency
F_HSE = 12000000

# Use Standard Pheriphial Libraries (true = 1)
USE_STD_LIBS = 1

# Optimization
OPTIMIZATION = 2

# Make date
DATE = 20$(shell date +'%y%m%d-%H%M')

# StdLibs to use if wanted
STDLIBDIR = Libraries/STM32F4xx_StdPeriph_Driver/src/
CSTD = $(STDLIBDIR)stm32f4xx_gpio.c
CSTD += $(STDLIBDIR)stm32f4xx_exti.c
CSTD += $(STDLIBDIR)stm32f4xx_rcc.c
CSTD += $(STDLIBDIR)stm32f4xx_syscfg.c
CSTD += $(STDLIBDIR)stm32f4xx_tim.c
CSTD += $(STDLIBDIR)stm32f4xx_i2c.c
CSTD += $(STDLIBDIR)stm32f4xx_flash.c
CSTD += $(STDLIBDIR)misc.c


# Sources
INCLUDE = -I./include -I./include/drivers -I./include/math 
INCLUDE += -I./CMSIS -I./Libraries/STM32F4xx_StdPeriph_Driver/inc
CSRCS = $(wildcard CMSIS/*.c) $(wildcard source/*.c) $(wildcard source/drivers/*.c) 
CSRCS += $(wildcard source/math/*.c) 

# FreeRTOS includes and source files
RTOS_DIR = FreeRTOS/
RTOS_PORT = $(RTOS_DIR)portable/GCC/ARM_CM4F/
RTOS_MEM = $(RTOS_DIR)portable/MemMang/
INCLUDE += -I$(RTOS_PORT) -I$(RTOS_DIR)include/
CSRCS += $(RTOS_DIR)list.c $(RTOS_DIR)tasks.c $(RTOS_DIR)queue.c $(RTOS_PORT)port.c $(RTOS_MEM)heap_1.c


# USB Libraries
USBLIB = $(wildcard Libraries/STM32_USB_Device_Library/Class/cdc/src/*.c)
USBLIB += $(wildcard Libraries/STM32_USB_Device_Library/Core/src/*.c)
USBLIB += $(wildcard Libraries/STM32_USB_OTG_Driver/src/*.c)

INCLUDE += -I./Libraries/STM32_USB_Device_Library/Class/cdc/inc
INCLUDE += -I./Libraries/STM32_USB_Device_Library/Core/inc
INCLUDE += -I./Libraries/STM32_USB_OTG_Driver/inc
CSTD    += $(USBLIB)

# Include different source files depending on USE_STD_LIBS

ifeq ($(USE_STD_LIBS),1)
	CSRCS += $(CSTD)
	COMMON = $(MCU) -DHSE_VALUE=$(F_HSE) -DUSE_STDPERIPH_DRIVER -DUSE_USB_OTG_FS -DDATE="\"$(DATE)\""
else
	COMMON = $(MCU) -DHSE_VALUE=$(F_HSE)
endif


ASRCS   = $(wildcard CMSIS/*.s) $(wildcard source/*.s) $(wildcard source/drivers/*.s) 
ALLSRC = $(ASRCS) $(CSRCS)
ALLSRCBASE = $(notdir $(basename $(ALLSRC)))
ALLOBJECTS = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(ALLSRCBASE)))
BINPLACE = -j.isr_vector -j.sw_version -j.text -j.ARM.extab -j.ARM 
BINPLACE += -j.preinit_array -j.init_array -j.fini_array -j.data

# Include defenitions
include make/defs.mk

all: build

build: elf bin hex lss sym

# Link: create ELF output file from object files.
$(eval $(call LINK_TEMPLATE, $(OBJDIR)/$(TARGET).elf, $(ALLOBJECTS)))

# Assemble: create object files from assembler source files.
$(foreach src, $(ASRCS), $(eval $(call ASSEMBLE_TEMPLATE, $(src))))

# Compile: create object files from C source files.
$(foreach src, $(CSRCS), $(eval $(call COMPILE_C_TEMPLATE, $(src))))

elf: $(OBJDIR)/$(TARGET).elf
lss: $(OBJDIR)/$(TARGET).lss
sym: $(OBJDIR)/$(TARGET).sym
hex: $(OBJDIR)/$(TARGET).hex
bin: $(OBJDIR)/$(TARGET).bin

size: $(TARGET).elf
	$(call SIZE_MSG, $(TARGET).elf)
	$(V0) $(SIZE) -A $(TARGET).elf		

	
clean: clean_list
	
clean_list:
	$(V0) echo $(MSG_CLEANING)
	$(V0) rm -f $(ALLOBJECTS)
	$(V0) rm -f $(OBJDIR)/$(TARGET).map
	$(V0) rm -f $(OBJDIR)/$(TARGET).elf
	$(V0) rm -f $(OBJDIR)/$(TARGET).hex
	$(V0) rm -f $(OBJDIR)/$(TARGET).bin
	$(V0) rm -f $(OBJDIR)/$(TARGET).sym
	$(V0) rm -f $(OBJDIR)/$(TARGET).lss
	
