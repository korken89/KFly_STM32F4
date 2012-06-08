#-------------------------------------------
# Makefile for STM32F4xx targets
# Author: (C) Emil Fresk
#-------------------------------------------

# External high speed crystal frequency
F_HSE = 12000000

# Use Standard Pherial Librarys (true = 1)
USE_STD_LIBS = 1

# Optimization
OPTIMIZATION = 1

# StdLibs to use if wanted
STDLIBDIR = Libraries/STM32F4xx_StdPeriph_Driver/src/
CSTD = $(STDLIBDIR)stm32f4xx_gpio.c
CSTD += $(STDLIBDIR)stm32f4xx_exti.c
CSTD += $(STDLIBDIR)stm32f4xx_rcc.c
CSTD += $(STDLIBDIR)stm32f4xx_syscfg.c
CSTD += $(STDLIBDIR)misc.c

# Include different source files depending on USE_STD_LIBS
ifeq ($(USE_STD_LIBS),1)
	CSRCS = $(wildcard CMSIS/*.c) $(wildcard source/*.c) $(wildcard source/drivers/*.c) 
	CSRCS += $(CSTD)
	COMMON = $(MCU) -DHSE_VALUE=$(F_HSE) -DUSE_STDPERIPH_DRIVER -DUSE_USB_OTG_FS
else
	CSRCS = $(wildcard CMSIS/*.c) $(wildcard source/*.c)
	COMMON = $(MCU) -DHSE_VALUE=$(F_HSE)
endif
INCLUDE = -I./include -I./include/drivers -I./CMSIS -I./Libraries/STM32F4xx_StdPeriph_Driver/inc

# USB Libraries
USBLIB = $(wildcard Libraries/STM32_USB_Device_Library/Class/cdc/src/*.c)
USBLIB += $(wildcard Libraries/STM32_USB_Device_Library/Core/src/*.c)
USBLIB += $(wildcard Libraries/STM32_USB_OTG_Driver/src/*.c)

INCLUDE += -I./Libraries/STM32_USB_Device_Library/Class/cdc/inc
INCLUDE += -I./Libraries/STM32_USB_Device_Library/Core/inc
INCLUDE += -I./Libraries/STM32_USB_OTG_Driver/inc
CSRCS   += $(USBLIB)

ASRCS   = $(wildcard CMSIS/*.s)
OBJECTS = $(CSRCS:.c=.o) $(ASRCS:.s=.o)

MCU     = -mcpu=cortex-m4 -mthumb -g -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -fsingle-precision-constant
CFLAGS  = $(COMMON) -std=gnu99 -O$(OPTIMIZATION) $(INCLUDE)
AFLAGS  = $(COMMON) $(INCLUDE)
LDFLAGS = $(COMMON) -nostdlib -Tstm32f4x_flash.ld -Wl,--build-id=none

GCC     = arm-none-eabi-gcc
SIZE    = arm-none-eabi-size
OBJDUMP = arm-none-eabi-objdump
OBJCOPY = arm-none-eabi-objcopy

test:
	@echo $(USBLIB)
	@echo $(CSRCS)

dump: main.elf
	$(OBJDUMP) -D main.elf > main.dump

bin: main.elf
	$(OBJCOPY) -O binary main.elf main.bin

all: main.elf
	@echo
	@echo "Size:"
	@$(SIZE) *.elf		

main.elf: $(OBJECTS)
	$(GCC) $(LDFLAGS) $(OBJECTS) -o main.elf
	
clean:
	rm -f $(OBJECTS) *.elf *.bin main.dump
	@echo
	
.c.o:
	$(GCC) $(CFLAGS) -c $< -o $(<:.c=.o)
	@echo

.s.o:
	$(GCC) -c $(AFLAGS) -o $(<:.s=.o) $<
	@echo