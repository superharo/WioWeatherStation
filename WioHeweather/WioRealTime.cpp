#include <WioRealTime.h>

unsigned long RealTime_Clock::calibreTimeNow(bool needPrint)
{
    unsigned long new_devicetime = getNTPtime();
 
    if (new_devicetime == 0) 
    {
        Serial.println("Failed to get time from network time server.");
        return 0;
    }
 
    if (!rtc.begin()) 
    {
        Serial.println("Couldn't find RTC");
        return 0;
    }
 
    rtc.adjust(DateTime(new_devicetime));
    if (needPrint == true)
    {
        Serial.println("rtc time updated.");
        // get and print the adjusted rtc time
        now = rtc.now();
        
        Serial.print("Adjusted RTC (boot) time is: ");
        Serial.println(now.timestamp(DateTime::TIMESTAMP_FULL));
    }
    return new_devicetime;
}

unsigned long RealTime_Clock::getNTPtime(void)
{
    // module returns a unsigned long time valus as secs since Jan 1, 1970 
    // unix time or 0 if a problem encounted
 
    //only send data when connected
    if (WiFi.status() == WL_CONNECTED) 
    {
        //initializes the UDP state
        //This initializes the transfer buffer
        udp.begin(WiFi.localIP(), localPort);
 
        sendNTPpacket(timeServer); // send an NTP packet to a time server
        // wait to see if a reply is available
        delay(100);

        if (udp.parsePacket()) 
        {
            // Serial.println("udp packet received");
            // We've received a packet, read the data from it
            udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
 
            // the timestamp starts at byte 40 of the received packet and is four bytes,
            // or two words, long. First, extract the two words:
 
            unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
            unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
            // combine the four bytes (two words) into a long integer
            // this is NTP time (seconds since Jan 1 1900):
            unsigned long secsSince1900 = highWord << 16 | lowWord;
            // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
            const unsigned long seventyYears = 2208988800UL;
            // subtract seventy years:
            unsigned long epoch = secsSince1900 - seventyYears;
 
            // adjust time for timezone offset in secs +/- from UTC
            // WA time offset from UTC is +8 hours (28,800 secs)
            // + East of GMT
            // - West of GMT
            return epoch;
        }
        else 
        {
            // were not able to parse the udp packet successfully
            // clear down the udp connection
            udp.stop();
            return 0; // zero indicates a failure
        }
        // not calling ntp time frequently, stop releases resources
        udp.stop();
    }
    else 
    {
        // network not connected
        Serial.println("WiFi is not connected!");
        return 0;
    }
}

// send an NTP request to the time server at the given address
unsigned long RealTime_Clock::sendNTPpacket(const char* address)
{
    // set all bytes in the buffer to 0
    for (int i = 0; i < NTP_PACKET_SIZE; ++i) 
    {
        packetBuffer[i] = 0;
    }

    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    packetBuffer[1] = 0;            // Stratum, or type of clock
    packetBuffer[2] = 6;            // Polling Interval
    packetBuffer[3] = 0xEC;         // Peer Clock Precision

    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;
 
    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
    udp.beginPacket(address, 123); //NTP requests are to port 123
    udp.write(packetBuffer, NTP_PACKET_SIZE);
    udp.endPacket(); 
}
