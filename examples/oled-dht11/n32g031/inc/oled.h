#ifndef __OLED_H
#define __OLED_H
#include "n32g031.h"

#define OLED_SCL_PORT  GPIOB
#define OLED_SCL_PIN   GPIO_PIN_6
#define OLED_SDA_PORT  GPIOB
#define OLED_SDA_PIN   GPIO_PIN_7

#define OLED_CMD   0
#define OLED_DATA  1

void OLED_Init(void);
void OLED_Clear(void);
void OLED_Set_Pos(uint8_t x, uint8_t y);
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size);
void OLED_ShowString(uint8_t x, uint8_t y, char *chr, uint8_t size);

void OLED_DrawBitmap(uint8_t x, uint8_t page, const unsigned char *bmp, uint8_t w, uint8_t pages);
void OLED_ShowBigDigit(uint8_t x, uint8_t page, uint8_t digit);

#endif