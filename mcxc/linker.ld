/* MCXC041 (KL03 die): 32KB flash, 2KB SRAM (0x1FFFFE00-0x200005FF) */
MEMORY {
  FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 32K
  RAM  (rwx) : ORIGIN = 0x1FFFFE00, LENGTH = 2K
}
_estack = 0x20000600;
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
