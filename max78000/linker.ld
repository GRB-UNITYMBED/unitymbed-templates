/* MAX78000 — 512K flash at 0x10000000, 128K SRAM at 0x20000000.
 *
 * Flash does not start at 0 or 0x08000000 the way most Cortex-M parts do.
 * The 0x20020000 stack top is not a calculation either: it is the value the
 * factory firmware carries in its own vector table, read back over SWD. */
MEMORY {
  FLASH (rx)  : ORIGIN = 0x10000000, LENGTH = 512K
  RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
}
_estack = ORIGIN(RAM) + LENGTH(RAM);
SECTIONS {
  .text : { KEEP(*(.vectors)) *(.text*) *(.rodata*) . = ALIGN(4); _sidata = .; } > FLASH
  .data : AT(_sidata) { _sdata = .; *(.data*) . = ALIGN(4); _edata = .; } > RAM
  .bss  : { _sbss = .; *(.bss*) *(COMMON) . = ALIGN(4); _ebss = .; } > RAM
}
