#include <WioHeweather.h>
#include <ArduinoJson.h>
#include <Time.h>

bool OW_Weather::getForecast(OW_current *current, OW_forecast *forecast, String locationID, String units, String language, String api_key)
{
	this -> current = current;
	this -> forecast = forecast;

	String cur_weather_url = "https://api.qweather.com/v7/weather/now?location=" + locationID + "&lang=" + language + "&key=" + api_key + "&gzip=n";
	bool result1 = parseRequest(cur_weather_url, cur_weather);

	String cur_pollution_url = "https://api.qweather.com/v7/air/now?location=" + locationID + "&lang=" + language + "&key=" + api_key + "&gzip=n";
	bool result2 = parseRequest(cur_pollution_url, cur_pollution);

	String fore_weather_url = "https://api.qweather.com/v7/weather/7d?location=" + locationID + "&lang=" + language + "&key=" + api_key + "&gzip=n";
	bool result3 = parseRequest(fore_weather_url, fore_weather);

	String fore_pollution_url = "https://api.qweather.com/v7/air/5d?location=" + locationID + "&lang=" + language + "&key=" + api_key + "&gzip=n";
	bool result4 = parseRequest(fore_pollution_url, fore_pollution);
	
    return result1 & result2 & result3 & result4;
}

bool OW_Weather::parseRequest(String url, url_type type) 
{
    uint32_t dt = millis();
	const char* host = "api.qweather.com";

    if (!client.connect(host, 443))
    {
    	Serial.println("Connection failed!");
    	return false;
    }
    uint32_t timeout = millis();

    Serial.print("Sending GET request to "); Serial.print(host); Serial.println("...");
    client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

    while (client.available() || client.connected())
    {
		String line = client.readStringUntil('\n');
		if (line == "\r") 
		{
			// Serial.println("Header end found.");
			break;
    	}   	

		if ((millis() - timeout) > 5000UL)
		{
			Serial.println("HTTP header timeout!");
			client.stop();
			return false;
		}
    }

    while (client.available())
    {
		stream_data += client.readStringUntil('\r');
		// Serial.println(stream_data);
		break;
    }

	bool result;
	if (type == cur_weather)
	{
		result = parseCurWeatherData(stream_data);
	}
	else if (type == fore_weather)
	{
		result = parseForeWeatherData(stream_data);
	}
	else if (type == cur_pollution)
	{
		result = parseCurPollutionData(stream_data);
	}
	else if (type == fore_pollution)
	{
		result = parseForePollutionData(stream_data);
	}
	else
	{
		Serial.println("Type of data error!");
		return false;
	}

    Serial.print("Done in "); Serial.print(millis()-dt); Serial.println(" ms");
	stream_data = "";
    client.stop();

    return result;
}

bool OW_Weather::parseCurWeatherData(String str)
{
	// String input;
	StaticJsonDocument<128> filter;
	filter["code"] = true;
	filter["updateTime"] = true;

	JsonObject filter_now = filter.createNestedObject("now");
	filter_now["temp"] = true;
	filter_now["icon"] = true;
	filter_now["text"] = true;
	filter_now["windScale"] = true;
	filter_now["humidity"] = true;

	StaticJsonDocument<256> doc;

	DeserializationError error = deserializeJson(doc, str, DeserializationOption::Filter(filter));

	if (error) 
	{
		Serial.print("deserializeJson() failed: ");
		Serial.println(error.c_str());
		return false;
	}

	const char* code = doc["code"]; // "200"
	const char* updateTime = doc["updateTime"]; // "2021-12-16T11:57+08:00"

	if (strcmp(code, "200") != 0)
	{
		Serial.println("Current weather request failed!");
		return false;
	}

	JsonObject now = doc["now"];
	const char* now_temp = now["temp"]; // "3"
	const char* now_icon = now["icon"]; // "100"
	const char* now_text = now["text"]; // "Sunny"
	const char* now_windScale = now["windScale"]; // "5"
	const char* now_humidity = now["humidity"]; // "20"

	current -> wea_updateTime = updateTime;
	current -> temp = now_temp;
	current -> icon = now_icon;
	current -> text = now_text;
	current -> windscale = now_windScale;
	current -> humidity = now_humidity;
	return true;
}

bool OW_Weather::parseForeWeatherData(String str)
{
	// String input;
	StaticJsonDocument<176> filter;
	filter["code"] = true;
	filter["updateTime"] = true;

	JsonObject filter_daily_0 = filter["daily"].createNestedObject();
	filter_daily_0["fxDate"] = true;
	filter_daily_0["tempMax"] = true;
	filter_daily_0["tempMin"] = true;
	filter_daily_0["iconDay"] = true;
	filter_daily_0["textDay"] = true;
	filter_daily_0["windScaleDay"] = true;
	filter_daily_0["humidity"] = true;

	DynamicJsonDocument doc(1536);

	DeserializationError error = deserializeJson(doc, str, DeserializationOption::Filter(filter));

	if (error) 
	{
		Serial.print("deserializeJson() failed: ");
		Serial.println(error.c_str());
		return false;
	}

	const char* code = doc["code"]; // "200"
	const char* updateTime = doc["updateTime"]; // "2021-12-18T10:35+08:00"
	if (strcmp(code, "200") != 0)
	{
		Serial.println("Forecast weather request failed!");
		return false;
	}

	forecast -> wea_updateTime = updateTime;

	int i = 0;
	for (JsonObject daily_item : doc["daily"].as<JsonArray>()) 
	{

		const char* daily_item_fxDate = daily_item["fxDate"]; // "2021-12-18", "2021-12-19", "2021-12-20", ...
		const char* daily_item_tempMax = daily_item["tempMax"]; // "5", "7", "11", "10", "6", "5", "0"
		const char* daily_item_tempMin = daily_item["tempMin"]; // "-7", "-5", "-4", "-5", "-4", "-5", "-7"
		const char* daily_item_iconDay = daily_item["iconDay"]; // "100", "100", "100", "100", "100", "100", ...
		const char* daily_item_textDay = daily_item["textDay"]; // "Sunny", "Sunny", "Sunny", "Sunny", "Sunny", ...
		const char* daily_item_windScaleDay = daily_item["windScaleDay"]; // "1-2", "1-2", "1-2", "1-2", "1-2", ...
		const char* daily_item_humidity = daily_item["humidity"]; // "28", "36", "36", "54", "80", "65", "63"
			
		if (i != 0)
		{
			forecast -> fxDate[i-1] = daily_item_fxDate;
			forecast -> tempMax[i-1] = daily_item_tempMax;
			forecast -> tempMin[i-1] = daily_item_tempMin;
			forecast -> iconDay[i-1] = daily_item_iconDay;
			forecast -> textDay[i-1] = daily_item_textDay;
			forecast -> windscale[i-1] = daily_item_windScaleDay;
			forecast -> humidity[i-1] = daily_item_humidity;
		}
		if (i == FORECAST_DAYS)
			break;
		else
			i++;
	}

	return true;
}

bool OW_Weather::parseCurPollutionData(String str)
{
	// String input;
	StaticJsonDocument<112> filter;
	filter["code"] = true;
	filter["updateTime"] = true;

	JsonObject filter_now = filter.createNestedObject("now");
	filter_now["level"] = true;
	filter_now["category"] = true;
	filter_now["pm10"] = true;
	filter_now["pm2p5"] = true;

	StaticJsonDocument<256> doc;

	DeserializationError error = deserializeJson(doc, str, DeserializationOption::Filter(filter));

	if (error) 
	{
		Serial.print("deserializeJson() failed: ");
		Serial.println(error.c_str());
		return false;
	}

	const char* code = doc["code"]; // "200"
	const char* updateTime = doc["updateTime"]; // "2021-12-15T21:59+08:00"

	if (strcmp(code, "200") != 0)
	{
		Serial.println("Current air pollution request failed!");
		return false;
	}

	JsonObject now = doc["now"];
	const char* now_level = now["level"]; // "3"
	const char* now_category = now["category"]; // "Lightly"
	const char* now_pm10 = now["pm10"]; // "133"
	const char* now_pm2p5 = now["pm2p5"]; // "114"

	current -> pol_updateTime = updateTime;
	current -> level = now_level;
	current -> category = now_category;
	current -> pm10 = now_pm10;
	current -> pm2p5 = now_pm2p5;

	return true;
}

bool OW_Weather::parseForePollutionData(String str)
{
	// String input;
	StaticJsonDocument<112> filter;
	filter["code"] = true;
	filter["updateTime"] = true;

	JsonObject filter_daily_0 = filter["daily"].createNestedObject();
	filter_daily_0["aqi"] = true;
	filter_daily_0["level"] = true;
	filter_daily_0["category"] = true;

	StaticJsonDocument<512> doc;

	DeserializationError error = deserializeJson(doc, str, DeserializationOption::Filter(filter));

	if (error) 
	{
		Serial.print("deserializeJson() failed: ");
		Serial.println(error.c_str());
		return false;
	}

	const char* code = doc["code"]; // "200"
	const char* updateTime = doc["updateTime"]; // "2021-12-16T16:42+08:00"

	if (strcmp(code, "200") != 0)
	{
		Serial.println("Air pollution forecast request failed!");
		return false;
	}

	forecast -> pol_updateTime = updateTime;

	int i = 0;
	for (JsonObject daily_item : doc["daily"].as<JsonArray>()) 
	{
		const char* daily_item_aqi = daily_item["aqi"]; // "49", "50", "59", "79", "89"
		const char* daily_item_level = daily_item["level"]; // "1", "1", "2", "2", "2"
		const char* daily_item_category = daily_item["category"]; // "Excellent", "Excellent", "Good", "Good", ...
		
		if (i != 0)
		{
			forecast -> aqi[i-1] = daily_item_aqi;
			forecast -> level[i-1] = daily_item_level;
			forecast -> category[i-1] = daily_item_category;
		}
		if (i == FORECAST_DAYS)
			break;
		else
			i++;
	}
	
	return true;
}

void OW_Weather::printCurrentWeather()
{
	Serial.println("############### Current Weather ###############");
	Serial.print("Update Time      : "); Serial.println(this->current->wea_updateTime);
	Serial.print("Text             : "); Serial.println(this->current->text);
	Serial.print("Temp             : "); Serial.println(this->current->temp);
	Serial.print("Humidity         : "); Serial.println(this->current->humidity);
	Serial.print("WindScale        : "); Serial.println(this->current->windscale);
	Serial.print("Icon             : "); Serial.println(this->current->icon);
	Serial.println("############ Current Air Pollution ############");
	Serial.print("Update Time      : "); Serial.println(this->current->pol_updateTime);
	Serial.print("Level            : "); Serial.println(this->current->level);
	Serial.print("Category         : "); Serial.println(this->current->category);
	Serial.print("PM2.5            : "); Serial.println(this->current->pm2p5);
	Serial.print("PM10             : "); Serial.println(this->current->pm10);
	Serial.println();
}

void OW_Weather::printForecastWeather()
{
	int i = 0;

	Serial.println("############### Forecast Weather ###############");
	Serial.print("Update Time      : "); Serial.println(this->forecast->wea_updateTime);
	for(i = 0; i < FORECAST_DAYS; i++)
	{
		Serial.print("Forecast Date    : "); Serial.println(this->forecast->fxDate[i]);
		Serial.print("Text             : "); Serial.println(this->forecast->textDay[i]);
		Serial.print("TempMax          : "); Serial.println(this->forecast->tempMax[i]);
		Serial.print("TempMin          : "); Serial.println(this->forecast->tempMin[i]);
		Serial.print("WindScale        : "); Serial.println(this->forecast->windscale[i]);
		Serial.print("Humidity         : "); Serial.println(this->forecast->humidity[i]);
		Serial.print("Icon             : "); Serial.println(this->forecast->iconDay[i]);
		Serial.println();
	}
	Serial.println("############ Forecast Air Pollution ############");
	Serial.print("Update Time      : "); Serial.println(this->forecast->pol_updateTime);
	for(i = 0; i < FORECAST_DAYS; i++)
	{	
		Serial.print("AQI              : "); Serial.println(this->forecast->aqi[i]);
		Serial.print("Level            : "); Serial.println(this->forecast->level[i]);
		Serial.print("Category         : "); Serial.println(this->forecast->category[i]);
		Serial.println();
	}
}

/***************************************************************************************
**                          Convert unix time to a time string
***************************************************************************************/
String strTime(time_t unixTime)
{
	unixTime += TIME_OFFSET;
	return ctime(&unixTime);
}