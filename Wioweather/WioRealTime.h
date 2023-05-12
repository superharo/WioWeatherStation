#ifndef __WIO_REAL_TIME_H__
#define __WIO_REAL_TIME_H__

#include <RTC_SAMD51.h>
#include <DateTime.h>
#include <rpcWiFi.h>
#include <millisDelay.h>
#include <Arduino.h>

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
class RealTime_Clock
{
public:
    unsigned long calibreTimeNow(bool needPrint = false);
    DateTime now;
    RTC_SAMD51 rtc;

private:
    unsigned long getNTPtime(void);
    // send an NTP request to the time server at the given address
    unsigned long sendNTPpacket(const char *address);

    // switch between local and remote time servers
    // comment out to use remote server
    const char *timeServer = "ntp1.aliyun.com"; // extenral NTP server e.g. time.nist.gov
    unsigned int localPort = 2390;              // local port to listen for UDP packets
    // The udp library class
    WiFiUDP udp;
    byte packetBuffer[NTP_PACKET_SIZE]; // buffer to hold incoming and outgoing packets
    millisDelay CalibreTimer;           // the update delay object. used for ntp periodic update.
};

#endif