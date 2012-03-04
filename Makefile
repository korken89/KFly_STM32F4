#-------------------------------------------
# Makefile for STM32F4xx targets
# Author: (C) Emil Fresk
#-------------------------------------------

# External high speed crystal frequency
F_HSE = 8000000

# Use Standard Pherial Librarys (true = 1)
USE_STD_LIBS = 0

# Optimization
OPTIMIZATION = 2

# Include different source files depending on USE_STD_LIBS
ifeq ($(USE_STD_LIBS),1)
	CSRCS   = $(wildcard *.c) $(wildcard */*.c) $(wildcard */*/*.c)
	COMMON = $(MCU) --no-builtin-memset --no-builtin-memcpy -DHSE_VALUE=$(F_HSE) -DUSE_STDPERIPH_DRIVER
else
	CSRCS   = $(wildcard *.c) $(wildcard */*.c)
	COMMON = $(MCU) --no-builtin-memset --no-builtin-memcpy -DHSE_VALUE=$(F_HSE)
endif
ASRCS   = $(wildcard *.s) $(wildcard */*.s)
OBJECTS = $(CSRCS:.c=.o) $(ASRCS:.s=.o)


INCLUDE = -I./include -I./CMSIS -I./STM32F4xx_StdPeriph_Driver/inc
MCU = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -fsingle-precision-constant
CFLAGS = $(COMMON) -std=gnu99 -O$(OPTIMIZATION) $(INCLUDE)
AFLAGS = $(COMMON) $(INCLUDE)
LDFLAGS = $(COMMON) -nostdlib -Tstm32f4x_flash.ld -Wl,--build-id=none

GCC = arm-none-eabi-gcc
SIZE = arm-none-eabi-size
OBJDUMP = arm-none-eabi-objdump
OBJCOPY = arm-none-eabi-objcopy

dump: main.elf
	$(OBJDUMP) -D main.elf

bin: main.elf
	$(OBJCOPY) -O binary main.elf main.bin

all: main.elf
	@echo
	@echo "Size:"
	@$(SIZE) *.elf		

main.elf: $(OBJECTS)
	$(GCC) $(LDFLAGS) $(OBJECTS) -o main.elf
	
clean:
	rm -f $(OBJECTS) main.*

.c.o:
	$(GCC) $(CFLAGS) -c $< -o $(<:.c=.o)

.s.o:
	$(GCC) -c $(AFLAGS) -o $(<:.s=.o) $<
