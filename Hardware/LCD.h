#ifndef __LCD_H
#define __LCD_H

#include "main.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_gpio.h"
#include "stm32l4xx_ll_spi.h"
#include "stm32l4xx_ll_dma.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx.h"
#include "SystemClock.h"

// 屏幕尺寸定义
#define ILI9341_WIDTH  240
#define ILI9341_HEIGHT 320

// 引脚定义（根据实际连接修改）
#define LCD_SPI        SPI1
#define LCD_CS_PORT    GPIOA
#define LCD_CS_PIN     LL_GPIO_PIN_4
#define LCD_DC_PORT    GPIOA
#define LCD_DC_PIN     LL_GPIO_PIN_3
#define LCD_RESET_PORT GPIOA
#define LCD_RESET_PIN  LL_GPIO_PIN_2

// 颜色定义
#define COLOR_BLACK   0x0000
#define COLOR_WHITE   0xFFFF
#define COLOR_RED     0xF800
#define COLOR_GREEN   0x07E0
#define COLOR_BLUE    0x001F

void ILI9341_Init(void);
void ILI9341_FillScreen(uint16_t color);
void WriteCommand(uint8_t cmd);
void WriteData(uint8_t data);
void SPI_Transmit(uint8_t data);
void ILI9341_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void SPI_Transmit_DMA(uint8_t *data, uint32_t size);

#endif
