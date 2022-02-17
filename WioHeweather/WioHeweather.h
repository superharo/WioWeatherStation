#ifndef __WIO_HEWEATHER_H__
#define __WIO_HEWEATHER_H__

#include <Arduino.h>
#include <rpcWiFiClientSecure.h>

#define FORECAST_DAYS 3
#define TIME_OFFSET 8UL * 3600UL // UTC + 8 hour

extern WiFiClientSecure client;
extern void InitializingDisplay(char cnt);
/***************************************************************************************
** Description:   Structure for current weather
***************************************************************************************/
typedef struct OW_current
{
	String wea_updateTime;
	String temp;
	String icon;
	String text;
	String windscale;
	String humidity;

	/* Air pollution */
	String pol_updateTime;
	String level;
	String category;
	String pm10;
	String pm2p5;

} OW_current;

/***************************************************************************************
** Description:   Structure for forecast weather
***************************************************************************************/
typedef struct OW_forecast 
{
	String wea_updateTime;

	String fxDate[FORECAST_DAYS];
	String tempMax[FORECAST_DAYS];
	String tempMin[FORECAST_DAYS];
	String iconDay[FORECAST_DAYS];
	String textDay[FORECAST_DAYS];
	String windscale[FORECAST_DAYS];
	String humidity[FORECAST_DAYS];

	/* Air pollution */
	String pol_updateTime;
	String aqi[FORECAST_DAYS];
	String level[FORECAST_DAYS];
	String category[FORECAST_DAYS];

} OW_forecast;

typedef enum 
{
	location_id = 0,
	cur_weather = 1,
	fore_weather = 2, 
	cur_pollution = 3, 
	fore_pollution = 4
} url_type;

String strTime(time_t unixTime);

class OW_Weather
{
	public:
    bool getForecast(OW_current *current, OW_forecast *forecast, String locationID, String units, String language, String api_key);
	void printCurrentWeather();
	void printForecastWeather();

	OW_current *current;	// pointer provided by sketch to the OW_current struct
    OW_forecast *forecast;	// pointer provided by sketch to the OW_forecast struct

	private: // Streaming parser callback functions, allow tracking and decisions
	bool parseRequest(String url, url_type type);
	bool parseCurWeatherData(String str);
	bool parseForeWeatherData(String str);
	bool parseCurPollutionData(String str);
	bool parseForePollutionData(String str);

	private: // Variables used internal to library
    // The value storage structures are created and deleted by the sketch and
    // a pointer passed via the library getForecast() call the value() function
    // is then used to populate the structs with values
	String stream_data;		//	Get the data of current weather or forecast weather int he next 5 days
};

#endif