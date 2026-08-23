# UnityMbed nRF9160 standalone template — bare-metal, NO Zephyr / NO TF-M.
# Two images:
#   build/secure.hex   mini secure boot (~1KB): SystemInit + SPU setup, jumps to NS
#   build/$(PROJECT).hex  the non-secure app at 0x40000 — your code lives in src/
# Flash both with `unitymbed flash` (J-Link backend — DK onboard probe).
CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

PROJECT ?= firmware
TARGET   = $(PROJECT)
OUTDIR   = build

COMMON  = -mcpu=cortex-m33 -mthumb -mfloat-abi=soft -Os -g -ffreestanding -Wall

# ── Non-secure app: src/*.c + sys/glue.c + prebuilt nrfx_ipc.o + libmodem.a ──
NS_SRCS   := $(wildcard src/*.c) sys/glue.c
NS_CFLAGS  = $(COMMON) -Ilib/include -Isys/shim $(CFLAGS_EXTRA)
NS_LDFLAGS = -nostdlib -Wl,-T,linker.ld -Wl,--gc-sections -Wl,-Map=$(OUTDIR)/$(TARGET).map

# ── Secure boot: secure/main.c + mdk SystemInit (errata + FICR trims —
#    modem will NOT boot without it; learned the hard way) ──
SEC_SRCS   = secure/main.c lib/mdk/system_nrf91.c
SEC_CFLAGS = $(COMMON) -DNRF9160_XXAA -Ilib/mdk -Ilib/cmsis
SEC_LDFLAGS = -nostdlib -Wl,-T,secure/secure.ld -Wl,--gc-sections

all: $(OUTDIR)/secure.hex $(OUTDIR)/$(TARGET).elf $(OUTDIR)/$(TARGET).bin $(OUTDIR)/$(TARGET).hex
	$(SIZE) $(OUTDIR)/secure.elf $(OUTDIR)/$(TARGET).elf

$(OUTDIR)/secure.elf: $(SEC_SRCS) secure/secure.ld
	@mkdir -p $(OUTDIR)
	$(CC) $(SEC_CFLAGS) $(SEC_SRCS) $(SEC_LDFLAGS) -o $@

$(OUTDIR)/$(TARGET).elf: $(NS_SRCS) linker.ld lib/nrfx_ipc.o lib/libmodem.a
	@mkdir -p $(OUTDIR)
	$(CC) $(NS_CFLAGS) $(NS_SRCS) lib/nrfx_ipc.o lib/libmodem.a -lc -lnosys -lgcc $(NS_LDFLAGS) -o $@

%.hex: %.elf
	$(OBJCOPY) -O ihex $< $@

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -rf $(OUTDIR)

.PHONY: all clean
