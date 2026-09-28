// UnityMbed — Smooth Servo Sweep Control @ N32G031
#include "n32g031.h"
#include "n32g031_gpio.h"
#include "n32g031_rcc.h"

/* ====================================================================
 * [Hardware Calibration Zone]
 * Pulse width parameters for calibrated angular positions
 * ==================================================================== */
#define PULSE_0_DEG     2250   // 0 degrees (Far Left)
#define PULSE_45_DEG    3325   // 45 degrees (Mid-Left)
#define PULSE_90_DEG    4400   // 90 degrees (Perpendicular / Center)

/* Simple software delay loop */
void Delay_Loop(volatile uint32_t count) {
    while(count--) { __NOP(); }
}

/* ====================================================================
 * Sub-routine: Single-step pulse delivery (PWM bit-banging)
 * ==================================================================== */
void Servo_Step(uint32_t pulse_width) {
    GPIO_SetBits(GPIOA, GPIO_PIN_1);     
    Delay_Loop(pulse_width);             
    GPIO_ResetBits(GPIOA, GPIO_PIN_1);   
    Delay_Loop(100000);                  
}

/* ====================================================================
 * Core Motion Routine: Smooth sweep transition between pulse angles
 * ==================================================================== */
void Smooth_Move(uint32_t start_pulse, uint32_t end_pulse) {
    // If target angle is greater, increment pulse width smoothly
    if (start_pulse < end_pulse) {
        for (uint32_t p = start_pulse; p <= end_pulse; p += 20) {
            Servo_Step(p);
        }
    } 
    // If target angle is lower, decrement pulse width smoothly
    else {
        for (uint32_t p = start_pulse; p >= end_pulse; p -= 20) {
            Servo_Step(p);
        }
    }
}

/* ====================================================================
 * Main Entry Point: Periodic Smooth Sweep Loop
 * ==================================================================== */
int main(void) {
    /* Initialize GPIOA Peripheral Clock and Pin 1 as Push-Pull Output */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
    GPIO_InitType GPIO_InitStructure;
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin = GPIO_PIN_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP; 
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);

    while(1) {
        /* 1. Smoothly sweep arm from 0 to 45 degrees */
        Smooth_Move(PULSE_0_DEG, PULSE_45_DEG);
        Delay_Loop(5000000); // Hold position
        
        /* 2. Smoothly sweep arm from 45 to 90 degrees */
        Smooth_Move(PULSE_45_DEG, PULSE_90_DEG);
        Delay_Loop(5000000); 

        /* 3. Smoothly sweep arm back from 90 to 45 degrees */
        Smooth_Move(PULSE_90_DEG, PULSE_45_DEG);
        Delay_Loop(5000000); 

        /* 4. Smoothly sweep arm back from 45 to 0 degrees to restart cycle */
        Smooth_Move(PULSE_45_DEG, PULSE_0_DEG);
        Delay_Loop(5000000); 
    }
}
