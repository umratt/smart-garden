#ifndef SUSENSOR_H
#define SUSENSOR_H

#include <OneWire.h>
#include <DallasTemperature.h>
#include "configApi.h"

#define DS18B20_PIN 27

OneWire oneWire(DS18B20_PIN);
DallasTemperature suSensörü(&oneWire);

void suSensorBaslat() {
    suSensörü.begin();
}

float suSicakligiOku() {
    suSensörü.requestTemperatures(); 
    float sicaklik = suSensörü.getTempCByIndex(0);
    
    // Sensör bağlı değilse veya hata varsa -127 döner
    if (sicaklik == DEVICE_DISCONNECTED_C) {
        return -999.0;
    }
    return sicaklik;
}

#endif