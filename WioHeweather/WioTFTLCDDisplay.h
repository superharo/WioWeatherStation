#ifndef __WIO_TFT_LCD_DISPLAY_H__
#define __WIO_TFT_LCD_DISPLAY_H__

#include <TFT_eSPI.h>
#include <Free_Fonts.h>
#include <Arduino.h>
#include <WioHeweather.h>
#include "RawImage.h"
#include "lcd_backlight.hpp"
#include <cstdint>

#define BACKGROUND_COLOR TFT_WHITE

extern LCDBackLight backLight;
extern TFT_eSPI tft_lcd;
extern unsigned long devicetime;

void LCD_Init(uint16_t background_color);
void InitializingDisplay(void);
bool RealTimeDisplay(unsigned long devicetime);
bool CurrentDisplay(OW_current *current);
bool ForecastDisplay(OW_forecast *forecast);
bool PageDisplay(int current_page);
uint8_t BacklightAdjust(int light_value);

#endif