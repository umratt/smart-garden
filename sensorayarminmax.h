#ifndef SENSORAYARMINMAX_H
#define SENSORAYARMINMAX_H

#include "configApi.h"

class LimitAyarlayici {
private:
    float *hedefMin, *hedefMax;
    String birim, baslik;
    bool ayarAktif = false;
    bool minSecili = true; // Hangi değerin ayarlandığını tutar

public:
    void ac(String _baslik, float* _min, float* _max, String _birim) {
        baslik = _baslik;
        hedefMin = _min;
        hedefMax = _max;
        birim = _birim;
        ayarAktif = true;
        minSecili = true;
        ekranCiz();
    }

    void ekranCiz() {
        tft.fillScreen(ARKA_PLAN);
        headerCiz();
        
        tft.setTextColor(BEYAZ); tft.setTextSize(2);
        tft.setCursor(40, 50); tft.print(baslik);

        // --- MIN KUTUSU ---
        uint16_t minRenk = minSecili ? ACCENT_CYAN : KART_ZEMIN;
        tft.fillRoundRect(20, 90, 135, 70, 8, minRenk);
        if(!minSecili) tft.drawRoundRect(20, 90, 135, 70, 8, YAZI_GRI);
        
        tft.setTextColor(minSecili ? ARKA_PLAN : BEYAZ); tft.setTextSize(1);
        tft.setCursor(30, 100); tft.print("MIN DEGER");
        tft.setTextSize(3); tft.setCursor(35, 120);
        tft.print(String(*hedefMin, 1));

        // --- MAX KUTUSU ---
        uint16_t maxRenk = !minSecili ? ACCENT_CYAN : KART_ZEMIN;
        tft.fillRoundRect(165, 90, 135, 70, 8, maxRenk);
        if(minSecili) tft.drawRoundRect(165, 90, 135, 70, 8, YAZI_GRI);
        
        tft.setTextColor(!minSecili ? ARKA_PLAN : BEYAZ); tft.setTextSize(1);
        tft.setCursor(175, 100); tft.print("MAX DEGER");
        tft.setTextSize(3); tft.setCursor(180, 120);
        tft.print(String(*hedefMax, 1));

        // Bilgi Notu
        tft.setTextColor(YAZI_GRI); tft.setTextSize(1);
        tft.setCursor(40, 180); tft.print("[OK] Min/Max Arasi Gecis");
        tft.setCursor(40, 200); tft.print("[+/-] Degeri Degistir");
        tft.setCursor(40, 220); tft.print("[GERI] Kaydet ve Cik");
    }

    bool butonIsle(int tus, float adim) {
        float* suankiDeger = minSecili ? hedefMin : hedefMax;

        if (tus == TUS_YUKARI) {
            *suankiDeger += adim;
            ekranCiz();
        } else if (tus == TUS_ASAGI) {
            if (*suankiDeger - adim >= 0) *suankiDeger -= adim;
            ekranCiz();
        } else if (tus == TUS_OK) {
            minSecili = !minSecili; // Min ve Max arası geçiş yap
            ekranCiz();
        } else if (tus == TUS_GERI) {
            ayarAktif = false; // Çıkış
            return true;
        }
        return false;
    }

    bool aktifMi() { return ayarAktif; }
};

#endif