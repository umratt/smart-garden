#ifndef BLYNK_MANAGER_H
#define BLYNK_MANAGER_H

#define BLYNK_TEMPLATE_ID   "TMPL6VoXGlcaO"
#define BLYNK_TEMPLATE_NAME "Akilli Sera"
#define BLYNK_AUTH_TOKEN    "jE1ZbxnLhL0lQpmH-CNAmz5af5vyxT8G"



#include <BlynkSimpleEsp32.h>
#include "dhtsensor.h"
#include "toprak_sensor.h"

BlynkTimer bTimer;

// Blynk Paneline Veri Gönderen Fonksiyon
void blynkVeriGonder() {
    if (Blynk.connected()) {
        DHTVeri dht = dhtOrtalamaAl();
        float tNemOrt = toprakNemOrtalamaAl();

        // Ortam Verileri
        Blynk.virtualWrite(V1, dht.sicaklik);
        Blynk.virtualWrite(V2, dht.nem);
        Blynk.virtualWrite(V3, tNemOrt);

        // Toprak Sensörleri (V10 - V15)
        Blynk.virtualWrite(V10, tekliToprakOku(0));
        Blynk.virtualWrite(V11, tekliToprakOku(1));
        Blynk.virtualWrite(V12, tekliToprakOku(2));
        Blynk.virtualWrite(V13, tekliToprakOku(3));
        Blynk.virtualWrite(V14, tekliToprakOku(4));
        Blynk.virtualWrite(V15, tekliToprakOku(5));

        Serial.println("Blynk -> Veriler basariyla gonderildi.");
    }
}

void blynkBaslat() {
    Blynk.config(BLYNK_AUTH_TOKEN);
    // Her 5 saniyede bir verileri gönder (Sunucu kısıtlamasına takılmamak için)
    bTimer.setInterval(5000L, blynkVeriGonder);
}

void blynkYonet() {
    if (WiFi.status() == WL_CONNECTED) {
        Blynk.run();
        bTimer.run();
    }
}

#endif