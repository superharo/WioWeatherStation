#include <WioTFTLCDDisplay.h>

const char* icon_table[20][2] = 
{
    {"100", "wi-day-sunny"},
    {"101", "wi-cloudy"},
    {"102", "wi-cloud"},
    {"103", "wi-day-cloudy"},
    {"104", "wi-cloud"},
    {"150", "wi-night-clear"},
    {"154", "wi-cloud"},
    {"305", "wi-rain"},
    {"306", "wi-rain"},
    {"307", "wi-rain"},
    {"399", "wi-rain"},
    {"400", "wi-snow"},
    {"401", "wi-snow"},
    {"402", "wi-snow"},
    {"404", "wi-sleet"},
    {"409", "wi-snow"},
    {"501", "wi-fog"},
    {"502", "wi-hail"},
    {"504", "wi-dust"},
    {"507", "wi-sandstorm"}
};

void LCD_Init(uint16_t background_color)
{
    tft_lcd.begin();
    tft_lcd.setRotation(1);
    tft_lcd.fillScreen(background_color);
    backLight.initialize();

    drawImage<uint16_t>("logo_128x128.bmp", 160-128/2, 120-128/2-20);
    tft_lcd.setTextDatum(MC_DATUM);
    tft_lcd.setTextColor(TFT_BLACK, BACKGROUND_COLOR);
    tft_lcd.loadFont("CourierNewPS-ItalicMT-16");
    tft_lcd.drawString("By KafCoppelia", 160, 190);
}

void InitializingDisplay(void)
{
    tft_lcd.loadFont("TimesNewRomanPSMT-24");
    tft_lcd.drawString("Initializing...", 160, 220);
    tft_lcd.unloadFont();
}

bool RealTimeDisplay(unsigned long devicetime)
{
    tft_lcd.loadFont("TimesNewRomanPSMT-24");
    tft_lcd.setTextDatum(MC_DATUM);
    tft_lcd.setTextColor(TFT_BLACK, BACKGROUND_COLOR);
	tft_lcd.drawString(strTime(devicetime), 150, 20);
    tft_lcd.unloadFont();
    tft_lcd.setFreeFont(FS9);
    tft_lcd.drawFastHLine(20, 40, 280, TFT_RED);
}

bool CurrentDisplay(OW_current *current)
{
    if (!current)
    {
        Serial.println("Current weather data dosen't exist!");
        return false;
    }

    tft_lcd.fillRect(0, 50, 320, 215, BACKGROUND_COLOR);

    int xpos, ypos;
    uint16_t level_color;
    xpos = 160;
    ypos = 50;

    tft_lcd.setTextDatum(TC_DATUM);
	tft_lcd.setFreeFont(FS9);
    tft_lcd.setTextColor(TFT_BLACK, BACKGROUND_COLOR);
    tft_lcd.drawString(">>> Today's Weather <<<", xpos, ypos, GFXFF);

    tft_lcd.setTextDatum(TL_DATUM);
    tft_lcd.setFreeFont(FS9);
    
    ypos += tft_lcd.fontHeight(GFXFF);
    xpos = 20;
    tft_lcd.setTextColor(TFT_BLACK, BACKGROUND_COLOR);
    xpos += tft_lcd.drawString("Weather: ", xpos, ypos, GFXFF);
    tft_lcd.setTextColor(TFT_ORANGE, BACKGROUND_COLOR);
    if (current->text != "")
        tft_lcd.drawString(current->text, xpos, ypos, GFXFF);
    else
        tft_lcd.drawString("Cloudy", xpos, ypos, GFXFF);
   
    xpos = 160;
    tft_lcd.setTextColor(TFT_BLACK, BACKGROUND_COLOR);
    xpos += tft_lcd.drawString("Temp: ", xpos, ypos, GFXFF);
    if ((current->temp).toInt() < 0)
        tft_lcd.setTextColor(TFT_BLUE, BACKGROUND_COLOR);
    else if ((current->temp).toInt() > 25)
        tft_lcd.setTextColor(TFT_RED, BACKGROUND_COLOR);
    else
        tft_lcd.setTextColor(TFT_ORANGE, BACKGROUND_COLOR);

    xpos += tft_lcd.drawString(current->temp, xpos, ypos, GFXFF);
    tft_lcd.setTextColor(TFT_BLACK, BACKGROUND_COLOR);
    tft_lcd.drawString(" degree", xpos, ypos, GFXFF);

    ypos += tft_lcd.fontHeight(GFXFF);  
    xpos = 20;
    tft_lcd.setTextColor(TFT_BLACK, BACKGROUND_COLOR);
    xpos += tft_lcd.drawString("Humidity: ", xpos, ypos, GFXFF);
    tft_lcd.setTextColor(TFT_ORANGE, BACKGROUND_COLOR);
    xpos += tft_lcd.drawString(current->humidity, xpos, ypos, GFXFF);
    tft_lcd.setTextColor(TFT_BLACK, BACKGROUND_COLOR);
    tft_lcd.drawString(" %", xpos, ypos, GFXFF);
    
    xpos = 160;
    tft_lcd.setTextColor(TFT_BLACK, BACKGROUND_COLOR);
    xpos += tft_lcd.drawString("Wind Scale: ", xpos, ypos, GFXFF);
    tft_lcd.setTextColor(TFT_ORANGE, BACKGROUND_COLOR);
    tft_lcd.drawString(current->windscale, xpos, ypos, GFXFF);

    tft_lcd.setTextDatum(TC_DATUM);
    ypos += tft_lcd.fontHeight(GFXFF);
    xpos = 150;
    tft_lcd.setTextColor(TFT_BLACK, BACKGROUND_COLOR);
    tft_lcd.drawString("Updated: " + (current->wea_updateTime).substring((current->wea_updateTime).length()-11, (current->wea_updateTime).length()-6), xpos, ypos, GFXFF);

    tft_lcd.setTextDatum(TC_DATUM);
    tft_lcd.setFreeFont(FS9);

    ypos += tft_lcd.fontHeight(GFXFF)*1.5;
    xpos = 160;
    tft_lcd.setTextColor(TFT_BLACK, BACKGROUND_COLOR);
    tft_lcd.drawString(">>> Air Pollution <<<", xpos, ypos, GFXFF);

    tft_lcd.setTextDatum(TL_DATUM);
    tft_lcd.setFreeFont(FS9);

    ypos += tft_lcd.fontHeight(GFXFF);
    xpos = 20;
    tft_lcd.setTextColor(TFT_BLACK, BACKGROUND_COLOR);
    xpos += tft_lcd.drawString("Level: ", xpos, ypos, GFXFF);

    switch((current->level).toInt())
    {
        case 1: level_color = TFT_GREEN;break;
        case 2: level_color = TFT_YELLOW;break;
        case 3: level_color = TFT_ORANGE;break;
        case 4: level_color = TFT_RED;break;
        case 5: level_color = TFT_PURPLE;break;
        default:level_color = TFT_MAROON;break;
    }
    tft_lcd.setTextColor(level_color, BACKGROUND_COLOR);
    tft_lcd.drawString(current->level, xpos, ypos, GFXFF);

    xpos = 160;
    tft_lcd.setTextColor(TFT_BLACK, BACKGROUND_COLOR);
    xpos += tft_lcd.drawString("Category: ", xpos, ypos, GFXFF);
    tft_lcd.setTextColor(level_color, BACKGROUND_COLOR);
    tft_lcd.drawString(current->category, xpos, ypos, GFXFF);
    
    tft_lcd.setTextDatum(TC_DATUM);
    ypos += tft_lcd.fontHeight(GFXFF);
    xpos = 150;
    tft_lcd.setTextColor(TFT_BLACK, BACKGROUND_COLOR);
    tft_lcd.drawString("Updated: " + (current->pol_updateTime).substring((current->pol_updateTime).length()-11, (current->pol_updateTime).length()-6), xpos, ypos, GFXFF);

    char* icon_pic_name = new char[40];
    int i = 0;
    for (i = 0; i <= 19; i++)
    {
        if (strcmp(icon_table[i][0], (current->icon).c_str()) == 0)
            break; 
        if (i == 19)
        {
            // Serial.println("No icon found!");
            // return false;
            i = 2;  // 没有图标匹配时显示云
            break;
        }
    }
    icon_pic_name = strcpy(icon_pic_name, icon_table[i][1]);
    icon_pic_name = strcat(icon_pic_name, ".bmp");

    drawImage<uint16_t>(icon_pic_name, 320-32, 240-32);
    return true;
}

bool ForecastDisplay(OW_forecast *forecast)
{
    if (!forecast)
    {
        Serial.println("Forcast info dosen't exist!");
        return false;
    }

    tft_lcd.fillRect(0, 25, 320, 215, BACKGROUND_COLOR);

    int xpos, ypos;
    int i;
    uint16_t level_color[3];
    
    xpos = 160;
    ypos = 50;
    tft_lcd.setTextDatum(TC_DATUM);
	tft_lcd.setFreeFont(FS9);
    tft_lcd.setTextColor(TFT_BLACK, BACKGROUND_COLOR);
    tft_lcd.drawString(">>> 3-day Forecast <<<", xpos, ypos, GFXFF);

    tft_lcd.setTextDatum(TL_DATUM);
    ypos += tft_lcd.fontHeight(GFXFF);
    xpos = 20;
    tft_lcd.setTextColor(TFT_BLACK, BACKGROUND_COLOR);
    tft_lcd.drawString("Date: ", xpos, ypos, GFXFF);

    tft_lcd.setTextDatum(TC_DATUM);
    xpos = 100;
    tft_lcd.setTextColor(TFT_BLACK, BACKGROUND_COLOR);
    tft_lcd.drawString((forecast->fxDate[0]).substring((forecast->fxDate[0]).length()-5, (forecast->fxDate[0]).length()), xpos, ypos, GFXFF);
    xpos = 180;
    tft_lcd.drawString((forecast->fxDate[1]).substring((forecast->fxDate[1]).length()-5, (forecast->fxDate[1]).length()), xpos, ypos, GFXFF);
    xpos = 260;
    tft_lcd.drawString((forecast->fxDate[2]).substring((forecast->fxDate[2]).length()-5, (forecast->fxDate[2]).length()), xpos, ypos, GFXFF);
    
    tft_lcd.setTextDatum(TL_DATUM);
    ypos += tft_lcd.fontHeight(GFXFF);
    xpos = 20;
    tft_lcd.setTextColor(TFT_BLACK, BACKGROUND_COLOR);
    tft_lcd.drawString("Main: ", xpos, ypos, GFXFF);

    tft_lcd.setTextDatum(TC_DATUM);
    xpos = 100;
    tft_lcd.setTextColor(TFT_ORANGE, BACKGROUND_COLOR);
    tft_lcd.drawString(forecast->textDay[0], xpos, ypos, GFXFF);
    xpos = 180;
    tft_lcd.setTextColor(TFT_ORANGE, BACKGROUND_COLOR);
    tft_lcd.drawString(forecast->textDay[1], xpos, ypos, GFXFF);
    xpos = 260;
    tft_lcd.setTextColor(TFT_ORANGE, BACKGROUND_COLOR);
    tft_lcd.drawString(forecast->textDay[2], xpos, ypos, GFXFF);

    tft_lcd.setTextDatum(TL_DATUM);
    ypos += tft_lcd.fontHeight(GFXFF);
    xpos = 20;
    tft_lcd.setTextColor(TFT_BLACK, BACKGROUND_COLOR);
    tft_lcd.drawString("Temp: ", xpos, ypos, GFXFF);

    tft_lcd.setTextDatum(TC_DATUM);
    xpos = 90;
    tft_lcd.setTextColor(TFT_BLUE, BACKGROUND_COLOR);
    xpos += tft_lcd.drawString(forecast->tempMin[0], xpos, ypos, GFXFF);
    tft_lcd.setTextColor(TFT_BLACK, BACKGROUND_COLOR);
    xpos += tft_lcd.drawString("~", xpos, ypos, GFXFF);
    tft_lcd.setTextColor(TFT_RED, BACKGROUND_COLOR);
    xpos += tft_lcd.drawString(forecast->tempMax[0], xpos, ypos, GFXFF);

    xpos = 170;
    tft_lcd.setTextColor(TFT_BLUE, BACKGROUND_COLOR);
    xpos += tft_lcd.drawString(forecast->tempMin[1], xpos, ypos, GFXFF);
    tft_lcd.setTextColor(TFT_BLACK, BACKGROUND_COLOR);
    xpos += tft_lcd.drawString("~", xpos, ypos, GFXFF);
    tft_lcd.setTextColor(TFT_RED, BACKGROUND_COLOR);
    xpos += tft_lcd.drawString(forecast->tempMax[1], xpos, ypos, GFXFF);

    xpos = 250;
    tft_lcd.setTextColor(TFT_BLUE, BACKGROUND_COLOR);
    xpos += tft_lcd.drawString(forecast->tempMin[2], xpos, ypos, GFXFF);
    tft_lcd.setTextColor(TFT_BLACK, BACKGROUND_COLOR);
    xpos += tft_lcd.drawString("~", xpos, ypos, GFXFF);
    tft_lcd.setTextColor(TFT_RED, BACKGROUND_COLOR);
    xpos += tft_lcd.drawString(forecast->tempMax[2], xpos, ypos, GFXFF);

    tft_lcd.setTextDatum(TC_DATUM);
    ypos += tft_lcd.fontHeight(GFXFF);
    xpos = 150;
    tft_lcd.setTextColor(TFT_BLACK, BACKGROUND_COLOR);
    tft_lcd.drawString("Updated: " + (forecast->wea_updateTime).substring((forecast->wea_updateTime).length()-11, (forecast->pol_updateTime).length()-6), xpos, ypos, GFXFF);

    tft_lcd.setTextDatum(TL_DATUM);
    ypos += tft_lcd.fontHeight(GFXFF);
    xpos = 20;
    tft_lcd.setTextColor(TFT_BLACK, BACKGROUND_COLOR);
    tft_lcd.drawString("Level: ", xpos, ypos, GFXFF);

    tft_lcd.setTextDatum(TC_DATUM);
    xpos = 100;
    switch((forecast->level[0]).toInt())
    {
        case 1: level_color[0] = TFT_GREEN;break;
        case 2: level_color[0] = TFT_YELLOW;break;
        case 3: level_color[0] = TFT_ORANGE;break;
        case 4: level_color[0]= TFT_RED;break;
        case 5: level_color[0] = TFT_PURPLE;break;
        default:level_color[0] = TFT_MAROON;break;
    } 

    tft_lcd.setTextColor(level_color[0], BACKGROUND_COLOR);
    tft_lcd.drawString(forecast->level[0], xpos, ypos, GFXFF);
    xpos = 180;
    switch((forecast->level[1]).toInt())
    {
        case 1: level_color[1] = TFT_GREEN;break;
        case 2: level_color[1] = TFT_YELLOW;break;
        case 3: level_color[1] = TFT_ORANGE;break;
        case 4: level_color[1] = TFT_RED;break;
        case 5: level_color[1] = TFT_PURPLE;break;
        default:level_color[1] = TFT_MAROON;break;
    } 

    tft_lcd.setTextColor(level_color[1], BACKGROUND_COLOR);
    tft_lcd.drawString(forecast->level[1], xpos, ypos, GFXFF);
    xpos = 260;
    switch((forecast->level[2]).toInt())
    {
        case 1: level_color[2] = TFT_GREEN;break;
        case 2: level_color[2] = TFT_YELLOW;break;
        case 3: level_color[2] = TFT_ORANGE;break;
        case 4: level_color[2] = TFT_RED;break;
        case 5: level_color[2] = TFT_PURPLE;break;
        default:level_color[2] = TFT_MAROON;break;
    } 

    tft_lcd.setTextColor(level_color[2], BACKGROUND_COLOR);
    tft_lcd.drawString(forecast->level[2], xpos, ypos, GFXFF);

    tft_lcd.setTextDatum(TL_DATUM);
    ypos += tft_lcd.fontHeight(GFXFF);
    xpos = 20;
    tft_lcd.setTextColor(TFT_BLACK, BACKGROUND_COLOR);
    tft_lcd.drawString("Cate: ", xpos, ypos, GFXFF);
    
    tft_lcd.setTextDatum(TC_DATUM);
    xpos = 100;
    tft_lcd.setTextColor(level_color[0], BACKGROUND_COLOR);
    tft_lcd.drawString(forecast->category[0], xpos, ypos, GFXFF);
    xpos = 180;
    tft_lcd.setTextColor(level_color[1], BACKGROUND_COLOR);
    tft_lcd.drawString(forecast->category[1], xpos, ypos, GFXFF);
    xpos = 260;
    tft_lcd.setTextColor(level_color[2], BACKGROUND_COLOR);
    tft_lcd.drawString(forecast->category[2], xpos, ypos, GFXFF);

    tft_lcd.setTextDatum(TC_DATUM);
    ypos += tft_lcd.fontHeight(GFXFF);
    xpos = 150;
    tft_lcd.setTextColor(TFT_BLACK, BACKGROUND_COLOR);
    tft_lcd.drawString("Updated: " + (forecast->pol_updateTime).substring((forecast->pol_updateTime).length()-11, (forecast->pol_updateTime).length()-6), xpos, ypos, GFXFF);

    return true;
}

bool PageDisplay(int current_page)
{
    int xpos, ypos;
    xpos = tft_lcd.width() / 2;
    ypos = tft_lcd.height() - 15;
    tft_lcd.setTextDatum(MC_DATUM);

    if (current_page == 1)
    {
        tft_lcd.fillCircle(xpos - 6, ypos, 4, TFT_BLACK);
        tft_lcd.drawCircle(xpos + 6, ypos, 4, TFT_BLACK);
    }
    else if (current_page == 2)
    {
        tft_lcd.drawCircle(xpos - 6, ypos, 4, TFT_BLACK);
        tft_lcd.fillCircle(xpos + 6, ypos, 4, TFT_BLACK);
    }
    else
        return false;

    return true;
}

uint8_t BacklightAdjust(int light_value)
{
    uint8_t cur_brightness;

    if (light_value <= 150)
    {
        cur_brightness = (uint8_t)15;
    }
    else if (light_value <= 300)
    {
        cur_brightness = (uint8_t)light_value / 10;
    }
    else if(light_value <= 600)
    {
        cur_brightness = (uint8_t)light_value / 10 + 10;
    }
    else
    {
        cur_brightness = backLight.getMaxBrightness();
    }
    backLight.setBrightness(cur_brightness);
    return cur_brightness;
}