# UnityMbed Makefile — N32G455 (Cortex-M4F)
PROJECT := $(shell node -e "console.log(require('./unitymbed.json').name)" 2>/dev/null || echo app)
MCU_FLAGS := -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard

CC      := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
SIZE    := arm-none-eabi-size

CFLAGS  := $(MCU_FLAGS) -O2 -g3 -Wall -Wextra -ffunction-sections -fdata-sections -Iinclude
LDFLAGS := $(MCU_FLAGS) -Tlinker.ld -nostartfiles -Wl,--gc-sections -Wl,-Map=build/$(PROJECT).map

SRC_C := $(wildcard src/*.c) $(wildcard hal/*.c)
SRC_S := startup_n32g455.s
OBJ   := $(patsubst %.c,build/%.o,$(SRC_C)) $(patsubst %.s,build/%.o,$(SRC_S))

all: build/$(PROJECT).elf build/$(PROJECT).bin build/$(PROJECT).hex
	@$(SIZE) build/$(PROJECT).elf

build/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

build/%.o: %.s
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

build/$(PROJECT).elf: $(OBJ)
	@mkdir -p build
	$(CC) $(OBJ) $(LDFLAGS) -o $@

build/%.bin: build/%.elf
	$(OBJCOPY) -O binary $< $@

build/%.hex: build/%.elf
	$(OBJCOPY) -O ihex $< $@

clean:
	rm -rf build

.PHONY: all clean
