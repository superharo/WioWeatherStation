#include <Arduino.h>
#line 1 "C:\\002\\arduinoworkspace\\WioWeatherStation\\WioHeweatherDemo\\WioHeweatherDemo.ino"
#include "WioHeweather.h"
#include "WioRealTime.h"
#include "WioTFTLCDDisplay.h"
#include "rpcWiFi.h"
#include "network.h"
#include <Seeed_FS.h>
#include "RawImage.h"

/* ---------- User Config ---------- */
// #define WIFI_SSID     "juxuan"
// #define WIFI_PASSWORD "828901288"
String com_key = "16a9f947813447e7a557ed17287632f6";
String locationID = "101010100";
String language = "en";
String units = "metric";
/* ---------------------------------- */

WiFiClientSecure client;
OW_Weather ow; // Weather forecast library instance
RealTime_Clock wio_rtc;
LCDBackLight backLight;
TFT_eSPI tft_lcd;
int current_page = 1;
int blacklight_auto_adjust = 1;
uint8_t cur_brightness = 100;
unsigned long devicetime;
millisDelay updateDelay;
OW_current *current = new OW_current;
OW_forecast *forecast = new OW_forecast;
void setup()
{
	// GPIO Initialize
	pinMode(WIO_KEY_C, INPUT_PULLUP); // Key C/B/A
	pinMode(WIO_KEY_B, INPUT_PULLUP);
	pinMode(WIO_KEY_A, INPUT_PULLUP);

	pinMode(WIO_5S_LEFT, INPUT_PULLUP);
	pinMode(WIO_5S_RIGHT, INPUT_PULLUP);
	pinMode(WIO_5S_UP, INPUT_PULLUP);
	pinMode(WIO_5S_DOWN, INPUT_PULLUP);
	pinMode(WIO_LIGHT, INPUT);

	// Serial Initialize
	Serial.begin(115200); // Fast to stop it holding up the stream
	delay(3000);
	Serial.println("--------------------start------------\n");

	// SD Initialize
	while (!SD.begin(SDCARD_SS_PIN, SDCARD_SPI))
	{ // Calculate y position to center text
		Serial.println("SD card error!\n");
		delay(500);
	}

	// TFT_LCD Initialize
	LCD_Init(BACKGROUND_COLOR);

	InitializingDisplay();
	// WiFi Initialize
	Network_Init();
	// RTC Initialize
	while ((devicetime = wio_rtc.calibreTimeNow()) == 0)
		;

	// Weather Info Initialize
	Serial.println("Requesting weather information from QWeather...");
	if (ow.getForecast(current, forecast, locationID, units, language, com_key) == true)
	{
		ow.printCurrentWeather();
		ow.printForecastWeather();
	}

	tft_lcd.fillRect(0, 0, 320, 240, BACKGROUND_COLOR);
	RealTimeDisplay(devicetime);
	CurrentDisplay(ow.current);
	PageDisplay(current_page);

	// Set update delay for RTC
	updateDelay.start(1 * 10 * 1000);
}
void loop()
{
	unsigned long temp_devicetime;

	if (updateDelay.justFinished())
	{
		updateDelay.repeat();
		if ((temp_devicetime = wio_rtc.calibreTimeNow()) != 0)
		{
			devicetime = temp_devicetime + 1;
		}
	}
	if (digitalRead(WIO_KEY_C) == LOW)
	{
		ow.getForecast(current, forecast, locationID, units, language, com_key);
		ow.printCurrentWeather();
		ow.printForecastWeather();
	}
	if (digitalRead(WIO_KEY_B) == LOW)
	{
		blacklight_auto_adjust = (blacklight_auto_adjust == 1) ? 0 : 1;
	}
	if (digitalRead(WIO_KEY_A) == LOW)
	{
		if ((temp_devicetime = wio_rtc.calibreTimeNow()) != 0)
		{
			devicetime = temp_devicetime + 1;
		}
	}
	if (digitalRead(WIO_5S_LEFT) == LOW) // <---
	{
		if (current_page == 2)
		{
			current_page = 1;
			CurrentDisplay(ow.current);
			PageDisplay(current_page);
		}
	}
	if (digitalRead(WIO_5S_RIGHT) == LOW) // --->
	{
		if (current_page == 1)
		{
			current_page = 2;
			ForecastDisplay(ow.forecast);
			PageDisplay(current_page);
		}
	}

	if (blacklight_auto_adjust)
	{
		cur_brightness = BacklightAdjust(analogRead(WIO_LIGHT));
	}
	else
	{
		if (digitalRead(WIO_5S_UP) == LOW)
		{
			cur_brightness = cur_brightness + 10;
		}
		if (digitalRead(WIO_5S_DOWN) == LOW)
		{
			cur_brightness = cur_brightness - 10;
		}
		backLight.setBrightness(cur_brightness);
	}
	// Serial.print("Current brightness: "); Serial.println(cur_brightness);

	RealTimeDisplay(devicetime);
	devicetime++;
	delay(1000);
}
