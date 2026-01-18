#ifndef ISIK_SENSOR_H
#define ISIK_SENSOR_H

#include "mux_manager.h"

int isikSeviyesiOku() {
    // Mux C6 kanalını oku
    int hamDeger = muxOku(6);
    
    // ESP32 12-bit ADC (0-4095)
    // LDR modülleri genelde ışık arttıkça voltajı düşürür (Ters orantı)
    // Eğer ışıkta değer düşüyorsa map(hamDeger, 4095, 0, 0, 100) kullan
    int isikYuzde = map(hamDeger, 4095, 0, 0, 100);
    
    return constrain(isikYuzde, 0, 100);
}

#endif