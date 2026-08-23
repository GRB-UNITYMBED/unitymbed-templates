/* MCXC444 (KL42/43 die): 256KB flash, 32KB SRAM (0x1FFF8000-0x20005FFF) */
MEMORY {
  FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 256K
  RAM  (rwx) : ORIGIN = 0x1FFF8000, LENGTH = 32K
}
_estack = 0x20006000;
ENTRY(Reset_Handler)
SECTIONS {
  .text : {
    KEEP(*(.vectors))
    . = 0x400; KEEP(*(.fcf))
    *(.text*) *(.rodata*) . = ALIGN(4); _sidata = .;
  } > FLASH
  .data : AT(_sidata) { _sdata = .; *(.data*) . = ALIGN(4); _edata = .; } > RAM
  .bss : { _sbss = .; *(.bss*) *(COMMON) . = ALIGN(4); _ebss = .; } > RAM
  /DISCARD/ : { *(.ARM.exidx*) *(.ARM.extab*) }
}
