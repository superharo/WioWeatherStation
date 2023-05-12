#include "network.h"
#include "wifiInfo.h"

void Network_Init()
{
    Serial.println("WiFi init...");
    uint8_t retry = 0;
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    WiFi.begin(WiFi_SSID, WiFi_PASSWORD);
    while ((WiFi.status() != WL_CONNECTED) && (retry < 10))
    {
        retry++;
        delay(500);
        Serial.println("Connecting to WiFi..");
        WiFi.begin(WiFi_SSID, WiFi_PASSWORD);
    }
    if (retry == 10)
    {
        Serial.println("Failed to connect to WiFi");
    }
    else
    {
        Serial.println("Connected to the WiFi network");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP()); // prints out the device's IP address
    }
}