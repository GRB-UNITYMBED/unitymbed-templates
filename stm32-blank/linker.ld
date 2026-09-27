/* Sizes are rewritten by `unitymbed init` from the detected part.
   Defaults are the smallest common STM32 footprint, so an undetected
   build still links and runs on anything. */
MEMORY
{
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 32K
  RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 8K
}
ENTRY(Reset_Handler)
SECTIONS
{
  .text : { KEEP(*(.vectors)) *(.text*) *(.rodata*) . = ALIGN(4); _sidata = .; } > FLASH
  .data : AT(_sidata) { _sdata = .; *(.data*) . = ALIGN(4); _edata = .; } > RAM
  .bss  : { _sbss = .; *(.bss*) *(COMMON) . = ALIGN(4); _ebss = .; _estack = ORIGIN(RAM) + LENGTH(RAM); } > RAM
}
