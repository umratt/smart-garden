#ifndef AYAREKRAN_H
#define AYAREKRAN_H

#include "configApi.h"
#include "anaekran.h"
#include "sensorayarminmax.h"
#include "hafiza_manager.h"
#include "eckalibre.h" // YENİ: Kalibrasyon dosyasını ekledik

enum AyarModu { MOD_ANA_AYARLAR, MOD_SENSOR_LIMITLERI, MOD_EC_KALIBRE };

class AyarYoneticisi {
private:
    AyarModu aktifMod = MOD_ANA_AYARLAR;
    LimitAyarlayici limitEkran; 
    ECKalibrasyonEkrani ecKalibreEkran; // YENİ
    
    int seciliSatir = 0;
    
    const int toplamAnaSecenek = 3;
    const char* anaMenüMetinleri[3] = {"PH Kalibrasyon", "EC Kalibrasyon", "Sensor Ayarlari"};
    const int toplamLimitSecenegi = 6;
    const char* limitMetinleri[6] = { "PH Ayarlari", "EC Ayarlari", "Ortam Isi", "Ortam Nem", "Su Isi", "Toprak Nemi" };

public:
    void ciz() {
        if (limitEkran.aktifMi()) { limitEkran.ekranCiz(); return; }
        
        // YENİ: EC Kalibrasyon ekranı çizimi
        if (ecKalibreEkran.aktif) {
            ecKalibreEkran.degerGuncelle();
            return;
        }

        if (aktifMod == MOD_ANA_AYARLAR) anaAyarCiz("AYARLAR", anaMenüMetinleri, toplamAnaSecenek);
        else if (aktifMod == MOD_SENSOR_LIMITLERI) anaAyarCiz("LIMITLER", limitMetinleri, toplamLimitSecenegi);
    }

    void anaAyarCiz(const char* baslik, const char** liste, int adet) {
        headerCiz();
        tft.fillRect(80, 0, 160, 30, ARKA_PLAN);
        tft.setTextColor(BEYAZ); tft.setTextSize(2);
        tft.setCursor(100, 5); tft.print(baslik);
        for (int i = 0; i < adet; i++) {
            int y = 45 + (i * 32); 
            if (i == seciliSatir) { tft.fillRoundRect(10, y, 300, 28, 4, ACCENT_MAVI); tft.setTextColor(BEYAZ); }
            else { tft.fillRoundRect(10, y, 300, 28, 4, KART_ZEMIN); tft.setTextColor(YAZI_GRI); }
            tft.setTextSize(1); tft.setCursor(25, y + 10); tft.print(liste[i]);
        }
        tft.setTextColor(YAZI_GRI); tft.setTextSize(1);
        tft.setCursor(80, 230); tft.print("[GERI] Butonu ile cikis");
    }

    bool butonIsle(int tus) {
        if (limitEkran.aktifMi()) {
            if (limitEkran.butonIsle(tus, 0.1)) {
                ayarlariKaydet(); 
                tft.fillScreen(ARKA_PLAN); ciz();
            }
            return false;
        }

        // YENİ: EC Kalibrasyon buton işlemleri
        if (ecKalibreEkran.aktif) {
            if (ecKalibreEkran.islemYap(tus)) {
                aktifMod = MOD_ANA_AYARLAR;
                tft.fillScreen(ARKA_PLAN);
                ciz();
            }
            return false;
        }

        if (tus == TUS_GERI) {
            if (aktifMod == MOD_SENSOR_LIMITLERI) {
                aktifMod = MOD_ANA_AYARLAR; seciliSatir = 2; 
                tft.fillScreen(ARKA_PLAN); ciz(); return false; 
            } else return true; 
        }

        int limitCount = (aktifMod == MOD_ANA_AYARLAR) ? toplamAnaSecenek : toplamLimitSecenegi;
        
        if (tus == TUS_ASAGI) { seciliSatir = (seciliSatir + 1) % limitCount; ciz(); } 
        else if (tus == TUS_YUKARI) { seciliSatir = (seciliSatir - 1 + limitCount) % limitCount; ciz(); }
        else if (tus == TUS_OK) {
            if (aktifMod == MOD_ANA_AYARLAR) {
                if (seciliSatir == 0) { } // PH Kalibrasyon boş
                else if (seciliSatir == 1) { 
                    aktifMod = MOD_EC_KALIBRE;
                    ecKalibreEkran.ac();
                }
                else if (seciliSatir == 2) { 
                    aktifMod = MOD_SENSOR_LIMITLERI; seciliSatir = 0; 
                    tft.fillScreen(ARKA_PLAN); ciz(); 
                }
            } else {
                if (seciliSatir == 0)      limitEkran.ac("PH AYARLARI", &phMin, &phMax, "");
                else if (seciliSatir == 1) limitEkran.ac("EC AYARLARI", &ecMin, &ecMax, "mS");
                else if (seciliSatir == 2) limitEkran.ac("ORTAM ISI", &oIsiMin, &oIsiMax, "C");
                else if (seciliSatir == 3) limitEkran.ac("ORTAM NEM", &oNemMin, &oNemMax, "%");
                else if (seciliSatir == 4) limitEkran.ac("SU ISI", &sIsiMin, &sIsiMax, "C");
                else if (seciliSatir == 5) limitEkran.ac("TOPRAK NEM", &tNemMin, &tNemMax, "%");
            }
        }
        return false;
    }
};
#endif