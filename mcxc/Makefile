# UnityMbed NXP MCX C (MCXC041 / Kinetis KL03 die) template — bare-metal.
CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

PROJECT ?= firmware
TARGET   = $(PROJECT)
OUTDIR   = build
CPU = cortex-m0plus

# SoC select: mcxc041 (KL03 die, 32K/2K) หรือ mcxc444 (KL42 die, 256K/32K)
# `unitymbed init` auto-detect จากบอร์ดแล้ว patch บรรทัดนี้เอง
SOC ?= mcxc041
ifeq ($(SOC),mcxc444)
  LDSCRIPT = linker-mcxc444.ld
  BOARD_DEF = UM_BOARD_MCXC444
else
  LDSCRIPT = linker.ld
  BOARD_DEF = UM_BOARD_MCXC041
endif

SRCS   := $(wildcard src/*.c)
CFLAGS  = -mcpu=$(CPU) -mthumb -Os -g -ffreestanding -Wall -D$(BOARD_DEF) $(CFLAGS_EXTRA)
LDFLAGS = -nostdlib -Wl,-T,$(LDSCRIPT) -Wl,--gc-sections -Wl,-Map=$(OUTDIR)/$(TARGET).map

all: $(OUTDIR)/$(TARGET).elf $(OUTDIR)/$(TARGET).bin $(OUTDIR)/$(TARGET).hex
	$(SIZE) $(OUTDIR)/$(TARGET).elf

$(OUTDIR)/$(TARGET).elf: $(SRCS) $(LDSCRIPT)
	@mkdir -p $(OUTDIR)
	$(CC) $(CFLAGS) $(SRCS) $(LDFLAGS) -o $@

%.hex: %.elf
	$(OBJCOPY) -O ihex $< $@

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -rf $(OUTDIR)

.PHONY: all clean
