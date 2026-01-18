#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include "configApi.h"
#include <time.h>

const long  gmtOffset_sec = 3 * 3600;
const int   daylightOffset_sec = 0;
const char* ntpServer = "pool.ntp.org";

void zamanBaslat() {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer, "time.google.com");
}

String suankiSaatiAl() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        unsigned long saniye = millis() / 1000;
        char buf[10];
        sprintf(buf, "%02d:%02d", (int)((saniye / 60) % 60), (int)(saniye % 60));
        return String(buf);
    }
    char timeStringBuff[10];
    strftime(timeStringBuff, sizeof(timeStringBuff), "%H:%M", &timeinfo);
    return String(timeStringBuff);
}

#endif