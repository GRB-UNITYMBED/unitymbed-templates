/* ====================================================================
 * [ UnityMbed ] Real-time Servo Control with Potentiometer (ADC)
 * Target MCU: Nations N32G031
 * 
 * Hardware Connections:
 * - PA0 : Potentiometer (Analog Input - ADC Channel 0)
 * - PA1 : Servo Motor Signal (Digital Output - Software PWM)
 * - PA9 : UART1 TX (Serial Monitor @ 115200 baud)
 * ==================================================================== */

#include "n32g031.h"
#include "n32g031_gpio.h"
#include "n32g031_rcc.h"
#include "n32g031_adc.h" 
#include "n32g031_usart.h"
#include <stdio.h>

/* --- Tuning Parameters --- */
#define PULSE_MIN     2250  // Minimum pulse width for Servo (approx 0 degrees)
#define PULSE_MAX     5500  // Maximum pulse width for Servo (approx 180 degrees)
#define KNOB_MAX      4095  // Maximum 12-bit ADC value (2^12 - 1)

/* ====================================================================
 * Utility & Math Functions
 * ==================================================================== */

/**
 * @brief Simple software delay loop used for generating PWM pulses.
 *        Blocks the CPU for a specific number of cycles.
 */
void Delay_Loop(volatile uint32_t count) {
    while(count--) { __NOP(); }
}

/**
 * @brief 1D Kalman Filter for smoothing noisy analog sensor readings.
 *        It predicts the true value by balancing the new measurement against past data.
 * @param measurement The raw ADC value just read.
 * @param state       Pointer to the current estimated true value.
 * @param pc          Pointer to the error covariance (uncertainty).
 * @return The filtered (smoothed) value.
 */
float kalman_update(float measurement, float* state, float* pc) {
    // R = 0.1f (Measurement Noise): How much we distrust the sensor.
    // Q = 0.01f (Process Noise): How fast we expect the actual value to change.
    float k_gain = *pc / (*pc + 0.1f);     
    *pc = (1.0f - k_gain) * (*pc) + 0.01f; 
    *state = *state + k_gain * (measurement - *state);
    return *state;
}

/* ====================================================================
 * Hardware Initialization Functions
 * ==================================================================== */

/**
 * @brief Configures PA0 as an Analog Input and initializes the ADC peripheral.
 */
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

/**
 * @brief Configures PA9 as USART1 TX for sending data to the Serial Monitor.
 */
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

/* ====================================================================
 * Hardware Operation Functions
 * ==================================================================== */

/**
 * @brief Triggers an ADC conversion and waits for it to finish.
 * @return 12-bit raw ADC value (0 to 4095).
 */
uint32_t Read_Knob(void) {
    ADC_ConfigRegularChannel(ADC, ADC_CH_0, 1, ADC_SAMP_TIME_56CYCLES5);
    ADC_EnableSoftwareStartConv(ADC, ENABLE);
    while(ADC_GetFlagStatus(ADC, ADC_FLAG_ENDC) == RESET); // Wait for conversion
    ADC_ClearFlag(ADC, ADC_FLAG_ENDC);
    return ADC_GetDat(ADC);
}

/**
 * @brief Sends a string character by character over UART.
 *        Waits for the Transmit Data Register Empty (TXDE) flag before sending the next char.
 */
void UART_SendString(const char* str) {
    while(*str) {
        USART_SendData(USART1, (uint8_t)*str++);
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXDE) == RESET);
    }
}

/**
 * @brief Generates a single PWM pulse to control the servo motor position.
 *        A servo expects a HIGH pulse of ~1ms to 2ms, followed by a LOW period.
 */
void Servo_Step(uint32_t pulse_width) {
    GPIO_SetBits(GPIOA, GPIO_PIN_1);     // Set PA1 HIGH
    Delay_Loop(pulse_width);             // Keep HIGH for 'pulse_width' duration
    GPIO_ResetBits(GPIOA, GPIO_PIN_1);   // Set PA1 LOW
    Delay_Loop(100000);                  // Keep LOW for the rest of the cycle (~20ms)
}

/* ====================================================================
 * Main Application Entry Point
 * ==================================================================== */
int main(void) {
    /* --- 1. System Initialization --- */
    Setup_UART();
    UART_SendString("\r\n[DEBUG] UART Initialized! Wiring is correct.\r\n");

    Setup_Knob();
    UART_SendString("[DEBUG] ADC Initialized. Reading baseline...\r\n");
    
    // Initialize Servo Motor Control Pin (PA1) as Push-Pull Output
    GPIO_InitType GPIO_InitStructure;
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin = GPIO_PIN_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP; 
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);

    /* --- 2. Variable Setup --- */
    uint32_t raw_knob = 0;
    uint32_t target_pulse = PULSE_MIN;
    
    // Read initial baseline to prevent the servo from jerking on startup
    float k_state = (float)Read_Knob(); 
    float k_pc = 0.0f;                  

    char msg_buffer[64];
    uint32_t print_counter = 0; // Used to limit UART output frequency

    UART_SendString("System Initialized. Starting Servo Control...\r\n");

    /* --- 3. Main Control Loop --- */
    while(1) {
        // Step A: Read raw analog value from the potentiometer
        raw_knob = Read_Knob(); 
        
        // Step B: Apply Kalman Filter to remove electrical noise/jitter
        kalman_update((float)raw_knob, &k_state, &k_pc);
        
        // Step C: Map the filtered ADC value (0-4095) to Servo Pulse Width (2250-5500)
        // Note: The math is inverted here (PULSE_MAX - ...) to match the physical knob direction
        target_pulse = PULSE_MAX - (((uint32_t)k_state * (PULSE_MAX - PULSE_MIN)) / KNOB_MAX);
        
        // Step D: Send the calculated pulse to physically move the servo
        Servo_Step(target_pulse);

        // Step E: Print telemetry data to Serial Monitor (Only once every 50 loops to avoid spam)
        if (++print_counter >= 50) {
            sprintf(msg_buffer, "Raw ADC: %4lu | Filtered: %4d | Pulse: %4lu\r\n", 
                    raw_knob, (int)k_state, target_pulse);
            UART_SendString(msg_buffer);
            print_counter = 0; // Reset counter
        }
    }
}
