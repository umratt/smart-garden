#ifndef EC_SENSOR_H
#define EC_SENSOR_H

#include "mux_manager.h"
#include "susensor.h"
#include "hafiza_manager.h" // Hafızadaki çarpanı almak için

// --- AYARLAR ---
#define EC_MUX_KANAL 7  // Sensör Mux 7. kanalda
#define VREF 3.3        // ESP32 Voltajı

// NOT: 'kalibrasyonCarpan' artık hafiza_manager.h dosyasından geliyor.

float ecDegeriOku() {
    // 1. Veriyi Oku
    int hamDeger = muxOku(EC_MUX_KANAL);
    
    // 2. Voltaja Çevir
    float voltaj = (float)hamDeger * VREF / 4095.0;
    
    // 3. Sıcaklık Telafisi
    float suT = suSicakligiOku();
    if(suT == -999.0) suT = 25.0; 

    float katsayi = 1.0 + 0.02 * (suT - 25.0);
    float duzeltilmisVoltaj = voltaj / katsayi;

    // 4. Voltaj -> TDS Dönüşümü
    float tdsValue = (133.42 * pow(duzeltilmisVoltaj, 3) - 255.86 * pow(duzeltilmisVoltaj, 2) + 857.39 * duzeltilmisVoltaj) * 0.5;
    
    // 5. EC Dönüşümü ve DİNAMİK KALİBRASYON
    // Hafızadan gelen 'ecKalibrasyonCarpan' değişkenini kullanıyoruz
    float ecValue = (tdsValue / 500.0) * ecKalibrasyonCarpan;

    // Filtre
    if (ecValue < 0.01) ecValue = 0.0;

    return ecValue;
}

float ecOrtalamaOku() {
    float toplam = 0;
    for(int i=0; i<10; i++) {
        toplam += ecDegeriOku();
        delay(10); 
    }
    return toplam / 10.0;
}

#endif