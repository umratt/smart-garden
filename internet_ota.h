#ifndef INTERNET_OTA_H
#define INTERNET_OTA_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>

// Güncelleme Durumunu Telegram'a Bildirmek İçin Callback
typedef void (*OtaDurumCallback)(String mesaj);
OtaDurumCallback telegramaBildir = nullptr;

void otaCallbackAyarla(OtaDurumCallback cb) {
    telegramaBildir = cb;
}

void internettenGuncelle(String dosyaUrl) {
    if (WiFi.status() != WL_CONNECTED) {
        if(telegramaBildir) telegramaBildir("⚠️ HATA: İnternet bağlantısı yok!");
        return;
    }

    if(telegramaBildir) telegramaBildir("🚀 Güncelleme Başlıyor...\nLütfen bekleyin, cihaz kapanıp açılacak.");

    // Güvenli bağlantı (HTTPS) için sertifika kontrolünü kapatıyoruz
    WiFiClientSecure client;
    client.setInsecure();

    // Güncelleme Ayarları
    httpUpdate.setLedPin(2); // Varsa üzerindeki LED yanıp sönsün
    
    // Güncelleme İşlemi Başlatılıyor
    t_httpUpdate_return ret = httpUpdate.update(client, dosyaUrl);

    // Sonuç Kontrolü
    switch (ret) {
        case HTTP_UPDATE_FAILED:
            if(telegramaBildir) telegramaBildir("❌ GÜNCELLEME BAŞARISIZ!\nHata Kodu: " + String(httpUpdate.getLastError()) + "\nMesaj: " + httpUpdate.getLastErrorString());
            break;

        case HTTP_UPDATE_NO_UPDATES:
            if(telegramaBildir) telegramaBildir("⚠️ Sunucuda güncelleme dosyası bulunamadı.");
            break;

        case HTTP_UPDATE_OK:
            // Başarılı olursa cihaz zaten restart atar, bu mesaj gitmeyebilir ama yazalım.
            Serial.println("Güncelleme Başarılı!");
            break;
    }
}

#endif