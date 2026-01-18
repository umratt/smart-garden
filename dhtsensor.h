#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include <DHT.h>
#include "configApi.h"

// PIN KONTROLÜ: ESP32 kullanıyorsan 13 ve 22 uygundur ancak 13 bazen dahili LED veya 
// boot sırasında sinyal gönderen bir pindir. Eğer sorun devam ederse 13 yerine 14 veya 27 dene.
#define DHTPIN      13  // DHT22 Pin
#define DHT11PIN    22  // DHT11 Pin

DHT dht22(DHTPIN, DHT22);
DHT dht11(DHT11PIN, DHT11);

struct DHTVeri {
    float sicaklik;
    float nem;
    bool hata;
};

void dhtBaslat() {
    dht11.begin();
    dht22.begin();
    Serial.println("DHT Sensörleri başlatıldı...");
}

DHTVeri dht1_Oku() {
    DHTVeri v;
    // Sensörün toparlanması için çok kısa bir süre tanıyoruz
    v.nem = dht11.readHumidity();
    v.sicaklik = dht11.readTemperature();
    
    // DHT11 için mantıksız değer filtresi (Örn: -0.6 derece ev ortamı için imkansızdır)
    v.hata = isnan(v.nem) || isnan(v.sicaklik) || (v.sicaklik < 0 && v.sicaklik > -5) || (v.nem < 1);
    
    if(v.hata) { Serial.println("DHT11 Okuma Hatası veya Mantıksız Veri!"); }
    return v;
}

DHTVeri dht2_Oku() {
    DHTVeri v;
    v.nem = dht22.readHumidity();
    v.sicaklik = dht22.readTemperature();
    
    v.hata = isnan(v.nem) || isnan(v.sicaklik) || (v.nem < 1);
    
    if(v.hata) { Serial.println("DHT22 Okuma Hatası!"); }
    return v;
}

DHTVeri dhtOrtalamaAl() {
    DHTVeri d1 = dht1_Oku();
    delay(50); // İki sensör okuması arasına kısa bir nefes payı
    DHTVeri d2 = dht2_Oku();
    DHTVeri sonuc = {0, 0, false};

    if (!d1.hata && !d2.hata) {
        sonuc.sicaklik = (d1.sicaklik + d2.sicaklik) / 2.0;
        sonuc.nem = (d1.nem + d2.nem) / 2.0;
    } else if (!d1.hata) {
        sonuc = d1;
    } else if (!d2.hata) {
        sonuc = d2;
    } else {
        sonuc.sicaklik = 0;
        sonuc.nem = 0;
        sonuc.hata = true;
    }
    return sonuc;
}
#endif