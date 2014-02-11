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
OBJDIR = ./build/obj
ELFDIR = ./build

ifdef ComSpec
$(shell md $(subst /,\\,$(OBJDIR)) 2>NUL)
REMOVE = del
else
$(shell mkdir -p $(OBJDIR) 2>/dev/null)
REMOVE = rm -f
endif

# External high speed crystal frequency
F_HSE = 12000000

# Use Standard Pheriphial Libraries (true = 1)
USE_STD_LIBS = 1

# Optimization
OPTIMIZATION = 1

# Make date
DATE = 20$(shell date +'%y%m%d-%H%M')

# StdLibs to use if wanted
STDLIBDIR = ./Libraries/STM32F4xx_StdPeriph_Driver/src/
CSTD = $(STDLIBDIR)stm32f4xx_gpio.c
CSTD += $(STDLIBDIR)stm32f4xx_exti.c
CSTD += $(STDLIBDIR)stm32f4xx_rcc.c
CSTD += $(STDLIBDIR)stm32f4xx_syscfg.c
CSTD += $(STDLIBDIR)stm32f4xx_tim.c
CSTD += $(STDLIBDIR)stm32f4xx_i2c.c
CSTD += $(STDLIBDIR)stm32f4xx_usart.c
CSTD += $(STDLIBDIR)stm32f4xx_flash.c
CSTD += $(STDLIBDIR)misc.c


# Create include paths and find sources in all the modules
MODULES = $(wildcard ./Modules/*)
$(foreach dir, $(MODULES) , $(eval MODULES_INC += -I$(dir)/include))
$(foreach dir, $(MODULES), $(eval MODULES_CSRCS += $(wildcard $(dir)/source/*.c)))
$(foreach dir, $(MODULES), $(eval MODULES_ASRCS += $(wildcard $(dir)/source/*.s)))

# Sources
INCLUDE = $(MODULES_INC) 
INCLUDE += -I./CMSIS -I./Libraries/STM32F4xx_StdPeriph_Driver/inc
CSRCS = $(wildcard ./CMSIS/*.c) $(MODULES_CSRCS)


# FreeRTOS includes and source files
RTOS_DIR = ./FreeRTOS/
RTOS_PORT = $(RTOS_DIR)portable/GCC/ARM_CM4F/
RTOS_MEM = $(RTOS_DIR)portable/MemMang/
INCLUDE += -I$(RTOS_PORT) -I$(RTOS_DIR)include/
CSRCS += $(RTOS_DIR)list.c $(RTOS_DIR)tasks.c $(RTOS_DIR)queue.c $(RTOS_PORT)port.c $(RTOS_MEM)heap_1.c


# USB Libraries
USBLIB = $(wildcard ./Libraries/STM32_USB_Device_Library/Class/cdc/src/*.c)
USBLIB += $(wildcard ./Libraries/STM32_USB_Device_Library/Core/src/*.c)
USBLIB += $(wildcard ./Libraries/STM32_USB_OTG_Driver/src/*.c)

INCLUDE += -I./Libraries/STM32_USB_Device_Library/Class/cdc/inc
INCLUDE += -I./Libraries/STM32_USB_Device_Library/Core/inc
INCLUDE += -I./Libraries/STM32_USB_OTG_Driver/inc
CSTD    += $(USBLIB)

# Include different source files depending on USE_STD_LIBS

ifeq ($(USE_STD_LIBS),1)
	CSRCS += $(CSTD)
	COMMON = -DHSE_VALUE=$(F_HSE) -DUSE_STDPERIPH_DRIVER -DUSE_USB_OTG_FS -DDATE="\"$(DATE)\""
else
	COMMON = -DHSE_VALUE=$(F_HSE)
endif


ASRCS   = $(wildcard ./CMSIS/*.s) $(MODULES_ASRCS) 
ALLSRC = $(ASRCS) $(CSRCS)
ALLSRCBASE = $(notdir $(basename $(ALLSRC)))
ALLOBJECTS = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(ALLSRCBASE)))




# Include defenitions
include make/defs.mk



all: build

build: elf bin hex lss sym dump size

# Link: Create elf output file from object files.
$(eval $(call LINK_TEMPLATE, $(ELFDIR)/$(TARGET).elf, $(ALLOBJECTS)))

# Assemble: Create object files from assembler source files.
$(foreach src, $(ASRCS), $(eval $(call ASSEMBLE_TEMPLATE, $(src))))

# Compile: Create object files from C source files.
$(foreach src, $(CSRCS), $(eval $(call COMPILE_C_TEMPLATE, $(src))))

elf: $(ELFDIR)/$(TARGET).elf
lss: $(ELFDIR)/$(TARGET).lss
sym: $(ELFDIR)/$(TARGET).sym
hex: $(ELFDIR)/$(TARGET).hex
bin: $(ELFDIR)/$(TARGET).bin

size: $(ELFDIR)/$(TARGET).elf
	$(V0) echo $(MSG_SIZE) $(TARGET).elf
	$(V0) $(SIZE) -A $(ELFDIR)/$(TARGET).elf	

dump: $(ELFDIR)/$(TARGET).elf
	$(V0) echo $(MSG_DUMP) $(TARGET).elf
	$(V0) $(OBJDUMP) -D $(ELFDIR)/$(TARGET).elf > $(ELFDIR)/$(TARGET)_dump.txt
	

clean: clean_list
	
clean_list:
	$(V0) echo $(MSG_CLEANING)
	$(V0) $(REMOVE) $(ALLOBJECTS)
	$(V0) $(REMOVE) $(ELFDIR)/$(TARGET).map
	$(V0) $(REMOVE) $(ELFDIR)/$(TARGET).elf
	$(V0) $(REMOVE) $(ELFDIR)/$(TARGET).hex
	$(V0) $(REMOVE) $(ELFDIR)/$(TARGET).bin
	$(V0) $(REMOVE) $(ELFDIR)/$(TARGET).sym
	$(V0) $(REMOVE) $(ELFDIR)/$(TARGET).lss
	
