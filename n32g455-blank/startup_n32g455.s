.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

.section .isr_vector, "a", %progbits
.word _estack
.word Reset_Handler
.word NMI_Handler
.word HardFault_Handler
.rept 13
.word Default_Handler
.endr

.section .text
.thumb_func
.global Reset_Handler
Reset_Handler:
    ldr r0, =_sdata
    ldr r1, =_edata
    ldr r2, =_sidata
copy_data:
    cmp r0, r1
    bge zero_bss
    ldr r3, [r2]
    str r3, [r0]
    adds r0, r0, #4
    adds r2, r2, #4
    b copy_data
zero_bss:
    ldr r0, =_sbss
    ldr r1, =_ebss
    movs r3, #0
bss_loop:
    cmp r0, r1
    bge call_main
    str r3, [r0]
    adds r0, r0, #4
    b bss_loop
call_main:
    bl main
hang:
    b hang

.thumb_func
Default_Handler:
    b Default_Handler

.weak NMI_Handler
.thumb_set NMI_Handler, Default_Handler
.weak HardFault_Handler
.thumb_set HardFault_Handler, Default_Handler
