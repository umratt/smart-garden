#ifndef MENUEKRAN_H
#define MENUEKRAN_H

#include "configApi.h"
#include "anaekran.h"
#include "wifi_manager.h"
#include "time_manager.h"
#include "toprak_sensor.h"
#include "ayarekran.h" // Yeni ayar ekranı modülü

enum EkranDurumu { DURUM_ANA_EKRAN, DURUM_MENU, DURUM_WIFI_KURULUM, DURUM_SENSOR_ANALIZ, DURUM_AYARLAR };

class MenuYoneticisi {
  private:
    EkranDurumu suankiDurum = DURUM_ANA_EKRAN;
    AyarYoneticisi ayarYonetici; // Ayar ekranı nesnesi
    int seciliMenuIndex = 0;
    const int toplamMenuSecenegi = 3; 
    const char* menuBasliklari[3] = {"Ayarlar", "Baglanti (Wifi)", "Sensor Analizi"};
    bool ekranGuncelle = false;
    unsigned long sonDinamikGuncelleme = 0;
    unsigned long sonAnalizGuncelleme = 0;

  public:
    void baslat() {
        tft.begin();
        tft.setRotation(3);
        acilisEkraniAnimasyonu();
        anaEkraniKur();
        ekranGuncelle = false;
        sonDinamikGuncelleme = millis();
    }

    void butonIsle(int tus) {
        if (tus == BASILMADI) return;
        ekranGuncelle = true;

        if (suankiDurum == DURUM_ANA_EKRAN) {
            if (tus == TUS_OK) { 
                suankiDurum = DURUM_MENU; 
                tft.fillScreen(ARKA_PLAN); 
            }
        } 
        else if (suankiDurum == DURUM_MENU) {
            if (tus == TUS_GERI) { 
                suankiDurum = DURUM_ANA_EKRAN; 
                tft.fillScreen(ARKA_PLAN); 
            }
            else if (tus == TUS_ASAGI) { 
                seciliMenuIndex = (seciliMenuIndex + 1) % toplamMenuSecenegi; 
            }
            else if (tus == TUS_YUKARI) { 
                seciliMenuIndex = (seciliMenuIndex - 1 + toplamMenuSecenegi) % toplamMenuSecenegi; 
            }
            else if (tus == TUS_OK) {
                if (seciliMenuIndex == 0) { // AYARLAR
                    suankiDurum = DURUM_AYARLAR; 
                    tft.fillScreen(ARKA_PLAN);
                    ayarYonetici.ciz();
                }
                else if (seciliMenuIndex == 1) { // WIFI
                    suankiDurum = DURUM_WIFI_KURULUM; 
                    tft.fillScreen(ARKA_PLAN);
                    wifiAPBaslat(); 
                }
                else if (seciliMenuIndex == 2) { // ANALIZ
                    suankiDurum = DURUM_SENSOR_ANALIZ; 
                    tft.fillScreen(ARKA_PLAN);
                }
            }
        }
        else if (suankiDurum == DURUM_AYARLAR) {
            // Ayar ekranı kendi içindeki geri dönüşü "true" olarak bildirirse ana menüye dön
            if (ayarYonetici.butonIsle(tus)) {
                suankiDurum = DURUM_MENU;
                tft.fillScreen(ARKA_PLAN);
                ekranGuncelle = true;
            }
        }
        else if (suankiDurum == DURUM_SENSOR_ANALIZ) {
            if (tus == TUS_GERI) { 
                suankiDurum = DURUM_MENU; 
                tft.fillScreen(ARKA_PLAN); 
            }
        }
    }

    void ekranYonet() {
        // Ana Ekran Otomatik Güncelleme (10 sn)
        if (suankiDurum == DURUM_ANA_EKRAN && (millis() - sonDinamikGuncelleme > 10000)) {
            anaEkraniGuncelle();
            sonDinamikGuncelleme = millis();
        }

        // Analiz Ekranı Canlı Güncelleme (1 sn)
        if (suankiDurum == DURUM_SENSOR_ANALIZ && (millis() - sonAnalizGuncelleme > 1000)) {
            sensorAnalizCiz();
            sonAnalizGuncelleme = millis();
        }

        if (!ekranGuncelle && suankiDurum != DURUM_WIFI_KURULUM && suankiDurum != DURUM_SENSOR_ANALIZ && suankiDurum != DURUM_AYARLAR) return;

        if (suankiDurum == DURUM_ANA_EKRAN) anaEkraniKur();
        else if (suankiDurum == DURUM_MENU) menuCiz();
        else if (suankiDurum == DURUM_AYARLAR) ayarYonetici.ciz();
        else if (suankiDurum == DURUM_WIFI_KURULUM) {
            wifiEkraniCiz();
            
        }
        ekranGuncelle = false;
    }

  private:
    void headerGuncelleMenu(String baslik) {
        headerCiz(); 
        tft.fillRect(80, 0, 160, 30, ARKA_PLAN);
        tft.setTextColor(BEYAZ); tft.setTextSize(2);
        tft.setCursor(95, 5); tft.print(baslik);
    }

    void menuCiz() {
        headerGuncelleMenu("ANA MENU");
        for (int i=0; i<toplamMenuSecenegi; i++) {
            int y = 50 + (i * 60);
            tft.fillRoundRect(10, y, 300, 45, 6, KART_ZEMIN);
            if (i == seciliMenuIndex) {
                tft.fillRoundRect(10, y, 6, 45, 3, ACCENT_YESL);
                tft.setTextColor(ACCENT_YESL);
            } else {
                tft.setTextColor(YAZI_GRI);
            }
            tft.setTextSize(2); tft.setCursor(35, y + 15); tft.print(menuBasliklari[i]);
        }
    }

    void sensorAnalizCiz() {
        headerGuncelleMenu("ANALIZ");
        for (int i = 0; i < 6; i++) {
            int nem = tekliToprakOku(i);
            int y = 45 + (i * 32);
            
            tft.setTextColor(YAZI_GRI); tft.setTextSize(1);
            tft.setCursor(15, y + 5); tft.printf("S%d", i + 1);

            tft.drawRoundRect(40, y, 220, 15, 3, KART_ZEMIN);
            uint16_t renk = (nem < 30) ? ACCENT_KIRM : (nem < 70) ? ACCENT_SARI : ACCENT_YESL;
            int barW = map(nem, 0, 100, 0, 216);
            if(barW > 0) tft.fillRect(42, y + 2, barW, 11, renk);

            tft.setTextColor(BEYAZ); tft.setCursor(270, y + 5);
            tft.print(String(nem) + "%");
        }
        tft.setTextColor(ACCENT_TUR); tft.setTextSize(1);
        tft.setCursor(100, 230); tft.print("[GERI] Cikis");
    }

    void wifiEkraniCiz() {
        tft.setCursor(10, 20); tft.setTextColor(ACCENT_CYAN, ARKA_PLAN);
        tft.setTextSize(2); tft.println("WIFI KURULUM MODU");
        tft.setTextColor(BEYAZ, ARKA_PLAN); tft.setTextSize(1);
        tft.setCursor(10, 60); tft.println("1. SmartGarden_PRO agina katil.");
        tft.setCursor(10, 80); tft.println("2. 192.168.4.1 adresine gir.");
    }
};
#endif