#ifndef ECKALIBRE_H
#define ECKALIBRE_H

#include "anaekran.h"
#include "hafiza_manager.h"
#include "ec_sensor.h" // BU SATIR EKLENDİ

class ECKalibrasyonEkrani {
public:
    bool aktif = false;

    void ac() {
        aktif = true;
        ekranCiz();
    }

    void kapat() {
        aktif = false;
    }

    void ekranCiz() {
        tft.fillScreen(ARKA_PLAN);
        
        tft.fillRect(0, 0, 320, 40, ACCENT_TUR);
        tft.setTextColor(BEYAZ); tft.setTextSize(2);
        tft.setCursor(60, 10); tft.print("EC KALIBRASYON");

        tft.setTextColor(YAZI_GRI); tft.setTextSize(1);
        tft.setCursor(10, 60); tft.print("1. Sensoru 1413 sivisina koy.");
        tft.setCursor(10, 80); tft.print("2. Deger durunca OK'a bas.");

        tft.fillRoundRect(40, 110, 240, 30, 4, ILI9341_DARKGREY);
        tft.setTextColor(BEYAZ); tft.setTextSize(2);
        tft.setCursor(65, 117); tft.print("HEDEF: 1.413");

        degerGuncelle();

        tft.setTextColor(YAZI_GRI); tft.setTextSize(1);
        tft.setCursor(60, 220); tft.print("[OK] Kaydet  [GERI] Iptal");
    }

    void degerGuncelle() {
        if(!aktif) return;
        // ARTIK HATA VERMEYECEK
        float anlik = ecOrtalamaOku();
        
        tft.fillRect(40, 150, 240, 40, KART_ZEMIN);
        tft.setTextColor(ACCENT_TUR); tft.setTextSize(3);
        tft.setCursor(70, 158); 
        tft.print(anlik, 3); 
        tft.setTextSize(2); tft.print(" mS");
    }

    bool islemYap(int tus) {
        if (tus == TUS_GERI) {
            kapat(); return true; 
        }
        
        if (tus == TUS_OK) {
            tft.fillRect(20, 190, 280, 40, ARKA_PLAN);
            tft.setTextColor(ACCENT_SARI); tft.setTextSize(2);
            tft.setCursor(60, 200); tft.print("HESAPLANIYOR...");
            
            float okunanDeger = ecOrtalamaOku();
            
            if (okunanDeger < 0.1) {
                tft.setTextColor(ACCENT_KIRM);
                tft.setCursor(40, 200); tft.print("HATA! SIVI YOK");
                delay(2000);
                ekranCiz(); 
                return false;
            }

            // --- KALİBRASYON FORMÜLÜ ---
            float hedef = 1.413;
            float yeniCarpan = ecKalibrasyonCarpan * (hedef / okunanDeger);
            
            ecKalibrasyonCarpan = yeniCarpan;
            ayarlariKaydet();

            tft.fillScreen(ACCENT_YESL);
            tft.setTextColor(BEYAZ); tft.setTextSize(3);
            tft.setCursor(20, 100); tft.print("KALIBRE EDILDI!");
            tft.setTextSize(2);
            tft.setCursor(30, 140); tft.print("Yeni Carpan: " + String(yeniCarpan, 3));
            delay(3000); 
            
            kapat();
            return true; 
        }
        return false;
    }
};
#endif