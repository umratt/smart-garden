#include "configApi.h"
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <WiFi.h>
#include <Preferences.h>

// 1. NESNELER
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
AsyncWebServer server(80); 
Preferences preferences;

// 2. KENDİ DOSYALARIN
#include "dhtsensor.h"
#include "toprak_sensor.h"
#include "susensor.h"
#include "mux_manager.h"
#include "hafiza_manager.h"
#include "telegram_manager.h"
#include "web_dashboard.h"
#include "wifi_manager.h"
#include "button.h"
#include "menuekran.h" 
#include "time_manager.h"
#include "ec_sensor.h"
#include "internet_ota.h"    // YENİ: İnternetten güncelleme modülü

// 3. GLOBAL DEĞİŞKENLER
float phMin, phMax, ecMin, ecMax, oIsiMin, oIsiMax, oNemMin, oNemMax, sIsiMin, sIsiMax, tNemMin, tNemMax;
MenuYoneticisi menu; 

void setup() {
    Serial.begin(115200);
    ayarlariYukle();
    muxKurulum(); 
    dhtBaslat();
    suSensorBaslat();
    butonKurulum();
    
    menu.baslat(); 
    zamanBaslat();

    // WiFi Bilgilerini Oku
    preferences.begin("wifi_pref", true);
    String ssid = preferences.getString("ssid", "");
    String pass = preferences.getString("pass", "");
    preferences.end();

    // Web Panelini (AP modunu) her zaman başlat (İnternet olmasa da panele girebilmelisin)
    wifiAPBaslat(); 

    if(ssid != "" && ssid != "NULL") {
        Serial.print("WiFi Baglaniyor: "); Serial.println(ssid);
        WiFi.begin(ssid.c_str(), pass.c_str());
        
        // KRİTİK: Bağlantı için kısa bir süre bekle (Maksimum 10 saniye)
        int deneme = 0;
        while (WiFi.status() != WL_CONNECTED && deneme < 20) {
            delay(500);
            Serial.print(".");
            deneme++;
        }

        if(WiFi.status() == WL_CONNECTED) {
            Serial.println("\nWiFi Baglandi!");
            Serial.print("IP Adresi: "); Serial.println(WiFi.localIP());
            telegramBaslat(); // Bağlantı sağlandığında botu selamla!
        } else {
            Serial.println("\nWiFi Baglantisi Basarisiz. Sadece Panel Aktif.");
        }
    }

    Serial.println("Sistem Hazir!");
}

void loop() {
    int tus = butonOku();
    menu.butonIsle(tus);
    menu.ekranYonet();
    
    // Telegram ve Otomatik Kontrol (Limit aşımı vs.)
    if(WiFi.status() == WL_CONNECTED) {
        static unsigned long tgZaman = 0;
        if(millis() - tgZaman > 2000) { 
            telegramYonet(); // Mesajları oku ve cevapla
            tgZaman = millis();
        }
    }
    
    // İnternet olsun olmasın limitleri ve sensörleri arka planda kontrol etmeli
    // Bu fonksiyonu telegram_manager.h içinden loop'a da ekleyebiliriz 
    // ama genellikle telegramYonet() içinde otomatik tetiklenir.
    
    delay(20); 
}