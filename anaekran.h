#ifndef ANAEKRAN_H
#define ANAEKRAN_H

#include "configApi.h"
#include "time_manager.h"
#include "dhtsensor.h"
#include "susensor.h"
#include "toprak_sensor.h"
#include "isik_sensor.h"
#include "ec_sensor.h" // BU SATIR EKLENDİ (Hatayı çözen kısım)

extern Adafruit_ILI9341 tft;

// --- 1. YARDIMCI FONKSIYONLAR ---
uint16_t uyariRengiBelirle(float deger, float minL, float maxL, uint16_t normalRenk) {
    if (deger <= minL || deger >= maxL) return ACCENT_KIRM; 
    float tolerans = (maxL - minL) * 0.10;
    if (deger <= (minL + tolerans) || deger >= (maxL - tolerans)) return ACCENT_SARI; 
    return normalRenk; 
}

void wifiSinyalCiz(int x, int y) {
    tft.fillRect(x - 12, y, 50, 15, ARKA_PLAN); 
    if (WiFi.status() != WL_CONNECTED) {
        tft.setTextColor(ACCENT_KIRM); tft.setTextSize(1);
        tft.setCursor(x - 10, y + 2); tft.print("No WiFi");
    } else {
        int32_t rssi = WiFi.RSSI();
        int kademe = (rssi > -50) ? 5 : (rssi > -60) ? 4 : (rssi > -70) ? 3 : (rssi > -80) ? 2 : 1;
        for (int i = 1; i <= 5; i++) {
            uint16_t renk = (i <= kademe) ? ACCENT_YESL : ILI9341_DARKGREY;
            tft.fillRect(x + (i * 6), y + (10 - (i * 2)), 3, (i * 2), renk);
        }
    }
}

void headerGuncelle() {
    tft.fillRect(5, 5, 60, 20, ARKA_PLAN);
    tft.setTextColor(YAZI_GRI); tft.setTextSize(1);
    tft.setCursor(10, 10); tft.print(suankiSaatiAl());
    wifiSinyalCiz(270, 5);
}

void headerCiz() {
    tft.fillRect(0, 0, 320, 31, ARKA_PLAN);
    tft.setTextColor(BEYAZ); tft.setTextSize(2);
    tft.setCursor(85, 5); tft.print("SMART GARDEN");
    headerGuncelle();
    tft.drawFastHLine(0, 32, 320, KART_ZEMIN);
}

void ikonCiz(int x, int y, int type, uint16_t renk) {
    if (type == 1 || type == 3) { tft.fillCircle(x+2, y+10, 4, renk); tft.fillRect(x, y, 4, 10, renk); }
    else if (type == 2) { tft.fillTriangle(x, y, x - 5, y + 9, x + 5, y + 9, renk); }
    else if (type == 4) { tft.drawCircle(x, y + 5, 6, renk); tft.fillCircle(x, y + 5, 2, renk); }
    else if (type == 5) { tft.fillCircle(x, y + 8, 5, renk); tft.drawLine(x, y, x, y + 8, renk); }
    else if (type == 6) { tft.drawRect(x - 2, y, 5, 12, renk); }
    else if (type == 7) { tft.drawLine(x + 4, y, x - 4, y + 10, renk); tft.drawLine(x - 4, y + 10, x, y + 10, renk); }
}

void veriKartiCiz(int x, int y, int w, int h, uint16_t renk, String baslik, int ikonTipi) {
    tft.fillRoundRect(x, y, w, h, 4, KART_ZEMIN);
    tft.fillRoundRect(x, y, 4, h, 2, renk); 
    tft.setTextColor(YAZI_GRI); tft.setTextSize(1);
    tft.setCursor(x + 12, y + 8); tft.print(baslik);
    ikonCiz(x + w - 15, y + 5, ikonTipi, renk);
}

void veriGuncelle(int x, int y, int w, String deger, String birim, uint16_t renk) {
    int textX = x + (w < 110 ? 10 : 15);
    int textY = y + (w < 110 ? 28 : 25);
    tft.fillRect(textX, textY, w - 25, 25, KART_ZEMIN);
    tft.setTextColor(BEYAZ); tft.setTextSize(w < 110 ? 2 : 3);
    tft.setCursor(textX, textY); tft.print(deger);
    tft.setTextSize(1); tft.setTextColor(renk); tft.print(" " + birim);
}

void anaEkraniGuncelle() {
    headerGuncelle();
    DHTVeri dht = dhtOrtalamaAl();
    float suT = suSicakligiOku();
    float tNem = toprakNemOrtalamaAl();
    int isikYuzde = isikSeviyesiOku();
    
    // ARTIK HATA VERMEYECEK
    float anlikEC = ecOrtalamaOku(); 
    float anlikPH = 6.2; 

    uint16_t phR   = uyariRengiBelirle(anlikPH, phMin, phMax, ACCENT_MOR);
    uint16_t ecR   = uyariRengiBelirle(anlikEC, ecMin, ecMax, ACCENT_TUR);
    uint16_t oIsiR = uyariRengiBelirle(dht.sicaklik, oIsiMin, oIsiMax, ACCENT_KIRM);
    uint16_t oNemR = uyariRengiBelirle(dht.nem, oNemMin, oNemMax, ACCENT_CYAN);
    uint16_t sIsiR = uyariRengiBelirle(suT, sIsiMin, sIsiMax, ACCENT_MAVI);
    uint16_t tNemR = uyariRengiBelirle(tNem, tNemMin, tNemMax, ACCENT_YESL);

    veriGuncelle(5, 40, 152, String(anlikPH, 1), "", phR);        
    veriGuncelle(163, 40, 152, String(anlikEC, 3), "mS", ecR);    
    veriGuncelle(5, 115, 152, (dht.hata ? "--" : String(dht.sicaklik, 1)), "C", oIsiR);      
    veriGuncelle(163, 115, 152, (dht.hata ? "--" : String(dht.nem, 0)), "%", oNemR);    
    veriGuncelle(5, 190, 100, (suT == -999.0 ? "--" : String(suT, 1)), "C", sIsiR);     
    veriGuncelle(110, 190, 100, String(tNem, 0), "%", tNemR); 
    veriGuncelle(215, 190, 100, String(isikYuzde), "%", ACCENT_SARI);  
}

void anaEkraniKur() {
    tft.fillRect(0, 33, 320, 207, ARKA_PLAN); 
    headerCiz();
    veriKartiCiz(5, 40, 152, 65, ACCENT_MOR, "PH", 6);
    veriKartiCiz(163, 40, 152, 65, ACCENT_TUR, "EC", 7);
    veriKartiCiz(5, 115, 152, 65, ACCENT_KIRM, "ORTAM ISI", 1);
    veriKartiCiz(163, 115, 152, 65, ACCENT_CYAN, "ORTAM NEM", 2);
    veriKartiCiz(5, 190, 100, 45, ACCENT_MAVI, "SU ISI", 3);
    veriKartiCiz(110, 190, 100, 45, ACCENT_YESL, "TOPRAK", 5);
    veriKartiCiz(215, 190, 100, 45, ACCENT_SARI, "ISIK", 4);
    anaEkraniGuncelle(); 
}

void acilisEkraniAnimasyonu() {
    tft.fillScreen(ARKA_PLAN);
    tft.setTextColor(BEYAZ); tft.setTextSize(3);
    tft.setCursor(35, 90); tft.print("Hidroponik Pro");
    tft.drawRect(60, 150, 200, 15, YAZI_GRI);
    for(int i=0; i<=100; i++) {
        tft.fillRect(62, 152, map(i, 0, 100, 0, 196), 11, ACCENT_CYAN);
        delay(10);
    }
}
void anaEkraniCiz() { anaEkraniKur(); }
#endif