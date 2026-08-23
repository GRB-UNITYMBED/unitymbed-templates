/* XMC1100-xx64: flash 64K @ 0x10001000 (XMC1000 user flash ไม่ได้อยู่ 0x0!),
 * SRAM 16K @ 0x20000000 */
MEMORY {
  FLASH (rx) : ORIGIN = 0x10001000, LENGTH = 64K
  RAM  (rwx) : ORIGIN = 0x20000000, LENGTH = 16K
}
_estack = 0x20004000;
ENTRY(Reset_Handler)
SECTIONS {
  .text : { KEEP(*(.vectors)) *(.text*) *(.rodata*) . = ALIGN(4); _sidata = .; } > FLASH
  .data : AT(_sidata) { _sdata = .; *(.data*) . = ALIGN(4); _edata = .; } > RAM
  .bss : { _sbss = .; *(.bss*) *(COMMON) . = ALIGN(4); _ebss = .; } > RAM
  /DISCARD/ : { *(.ARM.exidx*) *(.ARM.extab*) }
}
