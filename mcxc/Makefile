# UnityMbed NXP MCX C (MCXC041 / Kinetis KL03 die) template — bare-metal.
CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

PROJECT ?= firmware
TARGET   = $(PROJECT)
OUTDIR   = build
CPU = cortex-m0plus

SRCS   := $(wildcard src/*.c)
CFLAGS  = -mcpu=$(CPU) -mthumb -Os -g -ffreestanding -Wall $(CFLAGS_EXTRA)
LDFLAGS = -nostdlib -Wl,-T,linker.ld -Wl,--gc-sections -Wl,-Map=$(OUTDIR)/$(TARGET).map

all: $(OUTDIR)/$(TARGET).elf $(OUTDIR)/$(TARGET).bin $(OUTDIR)/$(TARGET).hex
	$(SIZE) $(OUTDIR)/$(TARGET).elf

$(OUTDIR)/$(TARGET).elf: $(SRCS) linker.ld
	@mkdir -p $(OUTDIR)
	$(CC) $(CFLAGS) $(SRCS) $(LDFLAGS) -o $@

%.hex: %.elf
	$(OBJCOPY) -O ihex $< $@

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -rf $(OUTDIR)

.PHONY: all clean
