/* NS app: flash 0x40000+, RAM ใช้ 0x20018000+ (0x20010000-0x20016000 = shmem modem) */
MEMORY
{
  FLASH (rx)  : ORIGIN = 0x00040000, LENGTH = 256K
  RAM   (rwx) : ORIGIN = 0x20018000, LENGTH = 96K
}
ENTRY(Reset_Handler)
SECTIONS
{
  .text : { KEEP(*(.vectors)) *(.text*) *(.rodata*) . = ALIGN(4); _sidata = .; } > FLASH
  .data : AT(_sidata) { _sdata = .; *(.data*) . = ALIGN(4); _edata = .; } > RAM
  .bss : { _sbss = .; *(.bss*) *(COMMON) . = ALIGN(4); _ebss = .; PROVIDE(end = .); } > RAM
  /DISCARD/ : { *(.ARM.exidx*) *(.ARM.extab*) }
}
