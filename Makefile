#-------------------------------------------
# Makefile for STM32F4xx targets
# Author: (C) Emil Fresk
#-------------------------------------------

# External high speed crystal frequency
F_HSE = 12000000

# Use Standard Pheriphial Libraries (true = 1)
USE_STD_LIBS = 1

# Optimization
OPTIMIZATION = 2

# Build tools
GCC     = arm-none-eabi-gcc
SIZE    = arm-none-eabi-size
OBJDUMP = arm-none-eabi-objdump
OBJCOPY = arm-none-eabi-objcopy
GDB     = arm-none-eabi-gdb

# Make date
DATE = 20$(shell date +'%y%m%d-%H%M')

# Flags
MCU     = -mcpu=cortex-m4 -mthumb -g -mfpu=fpv4-sp-d16 -mfloat-abi=hard -fsingle-precision-constant
CFLAGS  = $(COMMON) -std=gnu99 -O$(OPTIMIZATION) $(INCLUDE)
AFLAGS  = $(COMMON) $(INCLUDE)
LDFLAGS = $(COMMON) -Tstm32f4x_flash.ld -Wl,--build-id=none,-Map=main.map

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
OBJECTS = $(ASRCS:.s=.o) $(CSRCS:.c=.o)
BINPLACE = -j.isr_vector -j.sw_version -j.text -j.ARM.extab -j.ARM 
BINPLACE += -j.preinit_array -j.init_array -j.fini_array -j.data

CTEST = $(wildcard */*/*.c)

load: clean all
	$(GDB) --command=script

dump: main.elf
	@$(OBJDUMP) -D main.elf > main.dump
	@echo "main.dump created"
	
bin: main.elf
	@$(OBJCOPY) main.elf $(BINPLACE) -O binary main.bin
	@echo "main.bin created"

all: main.elf
	@echo
	@echo "Size:"
	@$(SIZE) main.elf		

main.elf: $(OBJECTS)
	$(GCC) $(LDFLAGS) $(OBJECTS) -lm -lc -o main.elf
	
clean:
	rm -f $(OBJECTS) *.elf *.bin *.dump *.map *.*~
	@echo
	
.c.o:
	$(GCC) $(CFLAGS) -c $< -o $(<:.c=.o)
	@echo

.s.o:
	$(GCC) -c $(AFLAGS) -o $(<:.s=.o) $<
	@echo