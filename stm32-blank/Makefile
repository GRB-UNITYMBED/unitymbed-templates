# UnityMbed STM32 generic template.
# `unitymbed init` auto-detects the connected part over ST-Link and rewrites
# CPU (and linker.ld sizes) to match the real silicon.
CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

PROJECT ?= firmware
TARGET   = $(PROJECT)
OUTDIR   = build
CPU = cortex-m4

SRCS   := $(wildcard src/*.c)
CFLAGS  = -mcpu=$(CPU) -mthumb -Os -g -ffreestanding -Wall $(CFLAGS_EXTRA)
LDFLAGS = -nostdlib -Wl,-T,linker.ld -Wl,--gc-sections -Wl,-Map=$(OUTDIR)/$(TARGET).map

all: $(OUTDIR)/$(TARGET).elf $(OUTDIR)/$(TARGET).bin $(OUTDIR)/$(TARGET).hex

$(OUTDIR)/$(TARGET).elf: $(SRCS) linker.ld
	@mkdir -p $(OUTDIR)
	$(CC) $(CFLAGS) $(SRCS) $(LDFLAGS) -o $@
	$(SIZE) $@

$(OUTDIR)/$(TARGET).bin: $(OUTDIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(OUTDIR)/$(TARGET).hex: $(OUTDIR)/$(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

clean:
	rm -rf $(OUTDIR)

.PHONY: all clean
