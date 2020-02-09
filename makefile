###############################################################################
# Makefile STM32F746-DISCO
###############################################################################

TARGET		= app

CC 		= /usr/bin/arm-none-eabi-gcc
AR 		= /usr/bin/arm-none-eabi-ar
OBJCOPY 	= /usr/bin/arm-none-eabi-objcopy
OBJDUMP 	= /usr/bin/arm-none-eabi-objdump
SIZE 		= /usr/bin/arm-none-eabi-size

BOARD		= ./board/stm32f746-disco
STARTUP 	= $(BOARD)/stm32f746xx_startup.s
LDSCRIPT	= $(BOARD)/stm32f746xx_flash.ld

DEFINES		= STM32F746xx USE_HAL_DRIVER

# -----------------------------------------------------------------------------
# PERIPHERAL LIBRARY
INCLUDES  = $(BOARD) $(BOARD)/bsp/cmsis
INCLUDES += $(BOARD)/bsp/hal $(BOARD)/bsp/hal/inc

devSOURCES  = $(wildcard $(BOARD)/bsp/cmsis/*.c)
devSOURCES += $(wildcard $(BOARD)/bsp/hal/src/*.c)

devOBJS = $(devSOURCES:.c=.o)

# -----------------------------------------------------------------------------
# FreeRTOS LIBRARY
INCLUDES += $(BOARD)/freertos
INCLUDES += $(BOARD)/freertos/Source/include
INCLUDES += $(BOARD)/freertos/Source/CMSIS_RTOS
INCLUDES += $(BOARD)/freertos/Source/portable/GCC/ARM_CM7/r0p1
INCLUDES += $(BOARD)/freertos/Source

osSOURCES += $(wildcard $(BOARD)/freertos/*.c)
osSOURCES += $(wildcard $(BOARD)/freertos/Source/*.c)
osSOURCES += $(wildcard $(BOARD)/freertos/Source/CMSIS_RTOS/*.c)
osSOURCES += $(wildcard $(BOARD)/freertos/Source/portable/MemMang/heap_4.c)
osSOURCES += $(wildcard $(BOARD)/freertos/Source/portable/GCC/ARM_CM7/r0p1/*.c)

osOBJS = $(osSOURCES:.c=.o)

# -----------------------------------------------------------------------------
# BOARD UTILITIES
INCLUDES += $(BOARD)/bsp $(BOARD)/bsp/peripheral/
INCLUDES += $(BOARD)/bsp/drivers/touchscreen/
INCLUDES += $(BOARD)/bsp/drivers/lcd/
INCLUDES += $(BOARD)/lvport/

SOURCES  += $(wildcard $(BOARD)/bsp/peripheral/*.c)
SOURCES  += $(wildcard $(BOARD)/bsp/drivers/touchscreen/*.c)
SOURCES  += $(wildcard $(BOARD)/bsp/drivers/lcd/*.c)
SOURCES  += $(wildcard $(BOARD)/lvport/*.c)

SOURCES  += $(wildcard $(BOARD)/*.c)

INCLUDES += $(BOARD)/osapi
SOURCES  += $(wildcard $(BOARD)/osapi/*.c)

# -----------------------------------------------------------------------------
# APPLICATION
INCLUDES	+= ./utilities/littlegl/
INCLUDES	+= ./application/ ./application/objects/ ./application/terminal/
INCLUDES	+= ./application/gui/

INCLUDES	+= ./utilities/littlegl/lvgl/lv_core/
INCLUDES	+= ./utilities/littlegl/lvgl/lv_draw/
INCLUDES	+= ./utilities/littlegl/lvgl/lv_fonts/
INCLUDES	+= ./utilities/littlegl/lvgl/lv_hal/
INCLUDES	+= ./utilities/littlegl/lvgl/lv_misc/
INCLUDES	+= ./utilities/littlegl/lvgl/lv_objx/
INCLUDES	+= ./utilities/littlegl/lvgl/lv_themes/
INCLUDES	+= ./utilities/littlegl/lvgl/

SOURCES 	+= $(shell find ./application/ -name *.c)
SOURCES 	+= $(shell find ./utilities/littlegl/ -name *.c)
SOURCES 	+= $(shell find ./application/gui/ -name *.c)

SOURCES 	+= $(shell find ./utilities/littlegl/lvgl/lv_core/ -name *.c)
SOURCES 	+= $(shell find ./utilities/littlegl/lvgl/lv_draw/ -name *.c)
SOURCES 	+= $(shell find ./utilities/littlegl/lvgl/lv_fonts/ -name *.c)
SOURCES 	+= $(shell find ./utilities/littlegl/lvgl/lv_hal/ -name *.c)
SOURCES 	+= $(shell find ./utilities/littlegl/lvgl/lv_misc/ -name *.c)
SOURCES 	+= $(shell find ./utilities/littlegl/lvgl/lv_objx/ -name *.c)
SOURCES 	+= $(shell find ./utilities/littlegl/lvgl/lv_themes/ -name *.c)


INCLUDES	+= ./application/gui/apps/settings/
SOURCES 	+= $(shell find ./application/gui/apps/settings/ -name *.c)

INCLUDES	+= ./application/gui/apps/snake/
SOURCES 	+= $(shell find ./application/gui/apps/snake/ -name *.c)

# Add assembly startup template & config files
SOURCES 	+= $(STARTUP)

# -----------------------------------------------------------------------------
# BUILD
CFLAGS  = -Wall -g -std=c99 -Os
CFLAGS += $(addprefix -D ,$(DEFINES))
CFLAGS += -Werror-implicit-function-declaration
CFLAGS += -mlittle-endian -mcpu=cortex-m7 -mthumb -DARM_MATH_CM7 
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += -ffunction-sections -fdata-sections 
CFLAGS += -Wl,--gc-sections -Wl,-Map=build/$(TARGET).map -Wstrict-aliasing
CFLAGS +=  $(addprefix -I ,$(INCLUDES))

# Append board specified libraries
CLIBS 	=  -L ./build -lstm32f7 -lfreertos

BUILD_PRINT = @echo "CC $<"

.SILENT:

.PHONY: all

all: libstm32f7.a libfreertos.a build/$(TARGET).elf

libstm32f7.a: $(devOBJS)
	$(BUILD_PRINT)
	$(AR) -r build/$@ $(devOBJS)
	
libfreertos.a: $(osOBJS)
	$(BUILD_PRINT)
	$(AR) -r build/$@ $(osOBJS)

%.o: %.c
	$(BUILD_PRINT)
	$(CC) -c -o $@ $< $(CFLAGS)

build/$(TARGET).elf: $(SOURCES)
	$(CC) -Os $(CFLAGS) $^ -o $@ $(CLIBS) -T $(LDSCRIPT) -MD
	$(OBJCOPY) -O ihex build/$(TARGET).elf build/$(TARGET).hex
	$(OBJCOPY) -O binary build/$(TARGET).elf build/$(TARGET).bin
	$(OBJDUMP) -St build/$(TARGET).elf >build/$(TARGET).lst
	$(SIZE) build/$(TARGET).elf
	$(BUILD_PRINT)

clean:
	find . -name "*.o" -type f -delete
	rm build/*

run : build/$(TARGET).bin
	st-flash --reset write build/$(TARGET).bin 0x8000000

###############################################################################
# END OF FILE
###############################################################################

