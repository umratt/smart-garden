#ifndef HAFIZA_MANAGER_H
#define HAFIZA_MANAGER_H

#include <Preferences.h>
#include "configApi.h"

// GLOBAL DEĞİŞKENLER (Diğer dosyalardan erişilebilir)
extern float phMin, phMax, ecMin, ecMax;
extern float oIsiMin, oIsiMax, oNemMin, oNemMax, tNemMin, tNemMax, sIsiMin, sIsiMax;

// YENİ: EC Kalibrasyon Çarpanı (Varsayılan olarak senin son ayarın 0.975)
float ecKalibrasyonCarpan = 0.975; 

void ayarlariKaydet() {
    Preferences p;
    p.begin("limitler", false);
    // Limitler
    p.putFloat("phMin", phMin); p.putFloat("phMax", phMax);
    p.putFloat("ecMin", ecMin); p.putFloat("ecMax", ecMax);
    p.putFloat("oIsiMin", oIsiMin); p.putFloat("oIsiMax", oIsiMax);
    p.putFloat("oNemMin", oNemMin); p.putFloat("oNemMax", oNemMax);
    p.putFloat("sIsiMin", sIsiMin); p.putFloat("sIsiMax", sIsiMax);
    p.putFloat("tNemMin", tNemMin); p.putFloat("tNemMax", tNemMax);
    
    // YENİ: Kalibrasyon çarpanını kaydet
    p.putFloat("ecCarpan", ecKalibrasyonCarpan);
    
    p.end();
    Serial.println("Ayarlar ve Kalibrasyon Hafizaya Kaydedildi.");
}

void ayarlariYukle() {
    Preferences p;
    p.begin("limitler", true);
    // Limitler
    phMin = p.getFloat("phMin", 5.5); phMax = p.getFloat("phMax", 6.5);
    ecMin = p.getFloat("ecMin", 1.0); ecMax = p.getFloat("ecMax", 2.0);
    oIsiMin = p.getFloat("oIsiMin", 20.0); oIsiMax = p.getFloat("oIsiMax", 28.0);
    oNemMin = p.getFloat("oNemMin", 40.0); oNemMax = p.getFloat("oNemMax", 70.0);
    sIsiMin = p.getFloat("sIsiMin", 18.0); sIsiMax = p.getFloat("sIsiMax", 24.0);
    tNemMin = p.getFloat("tNemMin", 30.0); tNemMax = p.getFloat("tNemMax", 80.0);
    
    // YENİ: Kalibrasyon çarpanını yükle (Bulamazsa varsayılan 0.975 döner)
    ecKalibrasyonCarpan = p.getFloat("ecCarpan", 0.975);
    
    p.end();
    Serial.println("Ayarlar Hafizadan Yuklendi.");
}

#endif