#include "dht11.h"
#include "utils.h" 

static void DHT11_Mode_Out(void) {
    GPIO_InitType GPIO_InitStructure;
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin = DHT11_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_HIGH;
    GPIO_InitPeripheral(DHT11_PORT, &GPIO_InitStructure);
}

static void DHT11_Mode_In(void) {
    GPIO_InitType GPIO_InitStructure;
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin = DHT11_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT; 
    GPIO_InitStructure.GPIO_Pull = GPIO_PULL_UP;
    GPIO_InitPeripheral(DHT11_PORT, &GPIO_InitStructure);
}

void DHT11_Init(void) {
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
    DHT11_Mode_Out();
    GPIO_SetBits(DHT11_PORT, DHT11_PIN);
    delay_ms(1000); 
}

uint8_t DHT11_Read_Data(uint8_t *temp, uint8_t *humi) {
    uint8_t buf[5] = {0};
    uint8_t i, j;
    uint32_t retry; 

    DHT11_Mode_Out();
    GPIO_ResetBits(DHT11_PORT, DHT11_PIN);
    delay_ms(20); 
    GPIO_SetBits(DHT11_PORT, DHT11_PIN);
    delay_us(30); 

    DHT11_Mode_In();
    
    retry = 0;
    while(GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN) == 1 && retry < 200) { retry++; delay_us(1); }
    if(retry >= 200) return 0;
    
    retry = 0;
    while(GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN) == 0 && retry < 200) { retry++; delay_us(1); }
    if(retry >= 200) return 0;

    for(i = 0; i < 5; i++) {
        for(j = 0; j < 8; j++) {
            retry = 0;
            while(GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN) == 1 && retry < 200) { retry++; delay_us(1); }
            retry = 0;
            while(GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN) == 0 && retry < 200) { retry++; delay_us(1); }
            delay_us(40); 
            buf[i] <<= 1;
            if(GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN) == 1) buf[i] |= 1;
        }
    }

    if((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4]) {
        *humi = buf[0];
        *temp = buf[2];
        return 1; 
    }
    return 0;
}