MEMORY { FLASH (rx): ORIGIN = 0x00000000, LENGTH = 256K
         RAM (rwx): ORIGIN = 0x20000000, LENGTH = 64K }
ENTRY(Reset_Handler)
SECTIONS {
  .text : { KEEP(*(.vectors)) *(.text*) *(.rodata*) . = ALIGN(4); _sidata = .; } > FLASH
  .data : AT(_sidata) { _sdata = .; *(.data*) . = ALIGN(4); _edata = .; } > RAM
  .bss : { _sbss = .; *(.bss*) *(COMMON) . = ALIGN(4); _ebss = .; } > RAM
}
