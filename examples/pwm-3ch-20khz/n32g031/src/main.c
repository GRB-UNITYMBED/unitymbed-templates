/* ====================================================================
 * [ UnityMbed ] Triple PWM Control (20kHz on ALL Channels) + WWDG
 * Target MCU: Nations N32G031
 * 
 * Hardware Connections:
 * - PA0 : Potentiometer (Analog Input - ADC Channel 0)
 * - PB0 : LED PWM 1 (TIM3 Channel 3) -> 20 kHz
 * - PB1 : LED PWM 2 (TIM3 Channel 4) -> 20 kHz
 * - PA8 : Oscilloscope PWM (TIM1 Channel 1) -> 20 kHz
 * - PA9 : UART1 TX (Serial Monitor @ 115200 baud)
 * ==================================================================== */

#include "n32g031.h"
#include "n32g031_gpio.h"
#include "n32g031_rcc.h"
#include "n32g031_adc.h" 
#include "n32g031_usart.h"
#include "n32g031_tim.h"
#include <stdio.h>

/* --- Tuning Parameters --- */
#define PWM_FREQ_HZ       20000 // 20 kHz for ALL Channels
#define KNOB_MAX          4095  // Maximum 12-bit ADC value

extern uint32_t SystemCoreClock; 
uint32_t timer1_period = 0; // ARR for TIM1
uint32_t timer3_period = 0; // ARR for TIM3

/* ====================================================================
 * Hardware Initialization Functions
 * ==================================================================== */

void Setup_Knob(void) {
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPH_ADC, ENABLE);
    
    GPIO_InitType GPIO_InitStructure;
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin = GPIO_PIN_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_ANALOG;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);

    ADC_InitType ADC_InitStructure;
    ADC_InitStruct(&ADC_InitStructure);
    ADC_InitStructure.MultiChEn      = DISABLE;
    ADC_InitStructure.ContinueConvEn = DISABLE;
    ADC_InitStructure.ExtTrigSelect  = ADC_EXT_TRIGCONV_NONE;
    ADC_InitStructure.DatAlign       = ADC_DAT_ALIGN_R;
    ADC_InitStructure.ChsNumber      = 1;
    ADC_Init(ADC, &ADC_InitStructure);
    ADC_Enable(ADC, ENABLE);
}

void Setup_UART(void) {
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_USART1, ENABLE);

    GPIO_InitType GPIO_InitStructure;
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin            = GPIO_PIN_9;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF4_USART1;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);

    USART_InitType USART_InitStructure;
    USART_InitStructure.BaudRate            = 115200;
    USART_InitStructure.WordLength          = USART_WL_8B;
    USART_InitStructure.StopBits            = USART_STPB_1;
    USART_InitStructure.Parity              = USART_PE_NO;
    USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
    USART_InitStructure.Mode                = USART_MODE_TX;
    USART_Init(USART1, &USART_InitStructure);
    USART_Enable(USART1, ENABLE);
}

/**
 * @brief Configures PA8 (TIM1_CH1) for 20 kHz PWM
 */
void Setup_PWM_TIM1(void) {
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_TIM1, ENABLE);

    GPIO_InitType GPIO_InitStructure;
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin            = GPIO_PIN_8;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF2_TIM1; 
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);

    timer1_period = (SystemCoreClock / PWM_FREQ_HZ) - 1;

    TIM_TimeBaseInitType TIM_TimeBaseStructure;
    TIM_InitTimBaseStruct(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.Prescaler = 0;
    TIM_TimeBaseStructure.Period    = timer1_period;
    TIM_TimeBaseStructure.CntMode   = TIM_CNT_MODE_UP;
    TIM_InitTimeBase(TIM1, &TIM_TimeBaseStructure);

    OCInitType TIM_OCInitStructure; 
    TIM_InitOcStruct(&TIM_OCInitStructure);
    TIM_OCInitStructure.OcMode      = TIM_OCMODE_PWM1;
    TIM_OCInitStructure.OutputState = TIM_OUTPUT_STATE_ENABLE;
    TIM_OCInitStructure.Pulse       = 0; 
    TIM_OCInitStructure.OcPolarity  = TIM_OC_POLARITY_HIGH;
    TIM_InitOc1(TIM1, &TIM_OCInitStructure);

    TIM_ConfigOc1Preload(TIM1, TIM_OC_PRE_LOAD_ENABLE);
    TIM_ConfigArPreload(TIM1, ENABLE);
    TIM_Enable(TIM1, ENABLE);
    TIM_EnableCtrlPwmOutputs(TIM1, ENABLE);
}

/**
 * @brief Configures PB0 (TIM3_CH3) and PB1 (TIM3_CH4) for 20 kHz PWM
 */
void Setup_PWM_TIM3(void) {
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_TIM3, ENABLE);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE); 

    GPIO_InitType GPIO_InitStructure;
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin            = GPIO_PIN_0 | GPIO_PIN_1; // PB0 and PB1
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF2_TIM3; 
    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);

    // Set Prescaler to 0 to match TIM1 resolution for 20kHz
    timer3_period = (SystemCoreClock / PWM_FREQ_HZ) - 1; 

    TIM_TimeBaseInitType TIM_TimeBaseStructure;
    TIM_InitTimBaseStruct(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.Prescaler = 0;
    TIM_TimeBaseStructure.Period    = timer3_period; 
    TIM_TimeBaseStructure.CntMode   = TIM_CNT_MODE_UP;
    TIM_InitTimeBase(TIM3, &TIM_TimeBaseStructure);

    OCInitType TIM_OCInitStructure; 
    TIM_InitOcStruct(&TIM_OCInitStructure);
    TIM_OCInitStructure.OcMode      = TIM_OCMODE_PWM1;
    TIM_OCInitStructure.OutputState = TIM_OUTPUT_STATE_ENABLE;
    TIM_OCInitStructure.Pulse       = 0; 
    TIM_OCInitStructure.OcPolarity  = TIM_OC_POLARITY_HIGH;
    
    TIM_InitOc3(TIM3, &TIM_OCInitStructure);
    TIM_ConfigOc3Preload(TIM3, TIM_OC_PRE_LOAD_ENABLE);

    TIM_InitOc4(TIM3, &TIM_OCInitStructure);
    TIM_ConfigOc4Preload(TIM3, TIM_OC_PRE_LOAD_ENABLE);

    TIM_ConfigArPreload(TIM3, ENABLE);
    TIM_Enable(TIM3, ENABLE);
}

/**
 * @brief Configures Window Watchdog (WWDG) using Bare-Metal Register Access
 */
void Setup_WWDG(void) {
    RCC->APB1PCLKEN |= (1U << 11);
    WWDG->CFG = (3U << 7) | 0x7F;
    WWDG->CTRL = (1U << 7) | 0x7F;
}

/* ====================================================================
 * Hardware Operation Functions
 * ==================================================================== */

uint32_t Read_Knob(void) {
    ADC_ConfigRegularChannel(ADC, ADC_CH_0, 1, ADC_SAMP_TIME_56CYCLES5);
    ADC_EnableSoftwareStartConv(ADC, ENABLE);
    while(ADC_GetFlagStatus(ADC, ADC_FLAG_ENDC) == RESET); 
    ADC_ClearFlag(ADC, ADC_FLAG_ENDC);
    return ADC_GetDat(ADC);
}

void UART_SendString(const char* str) {
    while(*str) {
        USART_SendData(USART1, (uint8_t)*str++);
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXDE) == RESET);
    }
}

/* ====================================================================
 * Main Application Entry Point
 * ==================================================================== */
int main(void) {
    SystemInit();
    SystemCoreClockUpdate();

    Setup_UART();
    UART_SendString("\r\n[DEBUG] System Starting...\r\n");

    Setup_Knob();
    Setup_PWM_TIM1(); // PA8 @ 20kHz
    Setup_PWM_TIM3(); // PB0 & PB1 @ 20kHz
    
    Setup_WWDG();
    
    UART_SendString("[DEBUG] PA8, PB0, PB1 Ready (All 20kHz). WWDG Enabled!\r\n");

    uint32_t raw_knob = 0;
    uint32_t scope_ccr = 0;
    uint32_t led_ccr = 0;
    uint32_t print_counter = 0; 
    char msg_buffer[128];

    while(1) {
        // 1. Read Potentiometer (0 - 4095)
        raw_knob = Read_Knob(); 
        
        // 2. Update 20kHz PWM for PA8
        scope_ccr = (raw_knob * timer1_period) / KNOB_MAX;
        TIM_SetCmp1(TIM1, scope_ccr);

        // 3. Update 20kHz PWM for PB0, PB1
        led_ccr = (raw_knob * timer3_period) / KNOB_MAX;
        TIM_SetCmp3(TIM3, led_ccr); 
        TIM_SetCmp4(TIM3, led_ccr); 

        // 4. Print Telemetry
        if (++print_counter >= 50000) {
            float duty_pct = ((float)raw_knob / (float)KNOB_MAX) * 100.0f;
            sprintf(msg_buffer, "ADC: %4lu | Duty Cycle: %5.1f %%\r\n", raw_knob, duty_pct);
            UART_SendString(msg_buffer);
            print_counter = 0;
        }
        
        // 5. Feed the Watchdog
        WWDG->CTRL = 0x7F;
    }
}