#include "oled.h"
#include "fonts.h"
#include "utils.h"

#define OLED_SCL_Clr() GPIO_ResetBits(OLED_SCL_PORT, OLED_SCL_PIN)
#define OLED_SCL_Set() GPIO_SetBits(OLED_SCL_PORT, OLED_SCL_PIN)
#define OLED_SDA_Clr() GPIO_ResetBits(OLED_SDA_PORT, OLED_SDA_PIN)
#define OLED_SDA_Set() GPIO_SetBits(OLED_SDA_PORT, OLED_SDA_PIN)

static void OLED_I2C_Init(void) {
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
    GPIO_InitType g;
    GPIO_InitStruct(&g);
    g.Pin        = OLED_SCL_PIN | OLED_SDA_PIN;
    g.GPIO_Mode  = GPIO_MODE_OUTPUT_OD;
    g.GPIO_Speed = GPIO_SPEED_HIGH;
    GPIO_InitPeripheral(GPIOB, &g);
    OLED_SCL_Set();
    OLED_SDA_Set();
}

static void I2C_Start(void) {
    OLED_SDA_Set(); OLED_SCL_Set(); delay_us(1);
    OLED_SDA_Clr(); delay_us(1);
    OLED_SCL_Clr(); delay_us(1);
}

static void I2C_Stop(void) {
    OLED_SDA_Clr(); OLED_SCL_Set(); delay_us(1);
    OLED_SDA_Set(); delay_us(1);
}

static void I2C_WaitAck(void) {
    OLED_SDA_Set(); delay_us(1);
    OLED_SCL_Set(); delay_us(1);
    OLED_SCL_Clr(); delay_us(1);
}

static void Send_Byte(uint8_t dat) {
    uint8_t i;
    for(i = 0; i < 8; i++) {
        if(dat & 0x80) OLED_SDA_Set(); else OLED_SDA_Clr();
        delay_us(1);
        OLED_SCL_Set(); delay_us(1);
        OLED_SCL_Clr(); delay_us(1);
        dat <<= 1;
    }
    I2C_WaitAck();
}

static void OLED_WR_Byte(uint8_t dat, uint8_t cmd) {
    I2C_Start();
    Send_Byte(0x78);
    Send_Byte(cmd ? 0x40 : 0x00);
    Send_Byte(dat);
    I2C_Stop();
}

void OLED_Set_Pos(uint8_t x, uint8_t y) {
    OLED_WR_Byte(0xb0 + (y / 8), OLED_CMD);
    OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
    OLED_WR_Byte(x & 0x0f, OLED_CMD);
}

static void SetPage(uint8_t x, uint8_t page) {
    OLED_WR_Byte(0xb0 + page, OLED_CMD);
    OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
    OLED_WR_Byte(x & 0x0f, OLED_CMD);
}

void OLED_Clear(void) {
    uint8_t i, n;
    for(i = 0; i < 8; i++) {
        OLED_WR_Byte(0xb0 + i, OLED_CMD);
        OLED_WR_Byte(0x00, OLED_CMD);
        OLED_WR_Byte(0x10, OLED_CMD);
        for(n = 0; n < 128; n++) OLED_WR_Byte(0, OLED_DATA);
    }
}

void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size) {
    uint8_t c = chr - ' ', i;
    if(x > 127) { x = 0; y += 2; }
    if(size == 16) {
        OLED_Set_Pos(x, y);
        for(i = 0; i < 8; i++) OLED_WR_Byte(F8X16[c * 16 + i], OLED_DATA);
        OLED_Set_Pos(x, y + 8);
        for(i = 0; i < 8; i++) OLED_WR_Byte(F8X16[c * 16 + i + 8], OLED_DATA);
    } else {
        OLED_Set_Pos(x, y);
        for(i = 0; i < 6; i++) OLED_WR_Byte(F6x8[c][i], OLED_DATA);
    }
}

void OLED_ShowString(uint8_t x, uint8_t y, char *chr, uint8_t size) {
    uint8_t j = 0;
    while(chr[j] != '\0') {
        OLED_ShowChar(x, y, chr[j], size);
        x += (size == 16) ? 8 : 6;
        if(x > 120) { x = 0; y += (size == 16) ? 16 : 8; }
        j++;
    }
}

void OLED_DrawBitmap(uint8_t x, uint8_t page, const unsigned char *bmp, uint8_t w, uint8_t pages) {
    uint8_t p, i;
    for(p = 0; p < pages; p++) {
        SetPage(x, page + p);
        for(i = 0; i < w; i++) {
            OLED_WR_Byte(bmp[p * w + i], OLED_DATA);
        }
    }
}

void OLED_ShowBigDigit(uint8_t x, uint8_t page, uint8_t digit) {
    if(digit > 9) return;
    OLED_DrawBitmap(x, page, BigNum16x32[digit], 16, 4);
}

void OLED_Init(void) {
    OLED_I2C_Init();
    delay_ms(100);
    OLED_WR_Byte(0xAE, OLED_CMD); 
    OLED_WR_Byte(0x20, OLED_CMD); 
    OLED_WR_Byte(0x10, OLED_CMD); 
    OLED_WR_Byte(0xB0, OLED_CMD); 
    OLED_WR_Byte(0xC8, OLED_CMD); 
    OLED_WR_Byte(0x00, OLED_CMD); 
    OLED_WR_Byte(0x10, OLED_CMD); 
    OLED_WR_Byte(0x40, OLED_CMD); 
    OLED_WR_Byte(0x81, OLED_CMD); 
    OLED_WR_Byte(0xDF, OLED_CMD);
    OLED_WR_Byte(0xA1, OLED_CMD); 
    OLED_WR_Byte(0xA6, OLED_CMD); 
    OLED_WR_Byte(0xA8, OLED_CMD); 
    OLED_WR_Byte(0x3F, OLED_CMD);
    OLED_WR_Byte(0xA4, OLED_CMD); 
    OLED_WR_Byte(0xD3, OLED_CMD); 
    OLED_WR_Byte(0x00, OLED_CMD);
    OLED_WR_Byte(0xD5, OLED_CMD); 
    OLED_WR_Byte(0xF0, OLED_CMD);
    OLED_WR_Byte(0xD9, OLED_CMD); 
    OLED_WR_Byte(0x22, OLED_CMD);
    OLED_WR_Byte(0xDA, OLED_CMD); 
    OLED_WR_Byte(0x12, OLED_CMD);
    OLED_WR_Byte(0xDB, OLED_CMD); 
    OLED_WR_Byte(0x20, OLED_CMD);
    OLED_WR_Byte(0x8D, OLED_CMD); 
    OLED_WR_Byte(0x14, OLED_CMD);
    OLED_WR_Byte(0xAF, OLED_CMD); 
}