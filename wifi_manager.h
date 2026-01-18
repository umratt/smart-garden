#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <ESPAsyncWebServer.h>
#include "web_dashboard.h"

extern AsyncWebServer server;
extern Preferences preferences;

const char landing_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>
<style>body{font-family:sans-serif;text-align:center;background:#000;color:#fff;padding-top:20vh;}
.btn{display:block;padding:25px;margin:20px auto;width:280px;text-decoration:none;border-radius:15px;font-size:18px;font-weight:bold;color:#fff;}
.btn-wifi{background:#1565c0;}.btn-panel{background:#2e7d32;}</style></head>
<body><h1>🌿 Smart Garden PRO</h1>
<a href='/wifi_setup' class='btn btn-wifi'>📶 WiFi Yapılandırma</a>
<a href='/panel' class='btn btn-panel'>🎮 Hızlı Kontrol Paneli</a></body></html>)rawliteral";

const char wifi_setup_html[] PROGMEM = R"rawliteral(
<html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1'>
<style>body{font-family:sans-serif;background:#212431;color:white;text-align:center;padding:20px;}
input{width:90%;padding:12px;margin:10px 0;border-radius:8px;border:none;}</style></head>
<body><h2>📶 WiFi Bağlantısı</h2><form action='/save_wifi' method='POST'>
SSID:<br><input type='text' name='ssid'><br>Şifre:<br><input type='password' name='pass'><br>
<input type='submit' value='Kaydet ve Bağlan' style='background:#2ECC71;color:white;font-weight:bold;'></form>
<br><a href='/' style='color:#aaa;'>Geri Dön</a></body></html>)rawliteral";

void wifiAPBaslat() {
    WiFi.softAP("SmartGarden_PRO");

    // 1. ANA SAYFA
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", landing_html);
    });

    // 2. WIFI AYARLARI
    server.on("/wifi_setup", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", wifi_setup_html);
    });

    // 3. WIFI KAYDET
    server.on("/save_wifi", HTTP_POST, [](AsyncWebServerRequest *request){
        if(request->hasParam("ssid", true) && request->hasParam("pass", true)) {
            String q_ssid = request->getParam("ssid", true)->value();
            String q_pass = request->getParam("pass", true)->value();
            preferences.begin("wifi_pref", false);
            preferences.putString("ssid", q_ssid);
            preferences.putString("pass", q_pass);
            preferences.end();
            request->send(200, "text/html", "Kaydedildi. ESP Yeniden Baslatiliyor...");
            delay(2000); ESP.restart();
        }
    });

    // 4. CANLI VERİ VE LİMİT KÖPRÜSÜ (JSON)
    server.on("/read_sensors", HTTP_GET, [](AsyncWebServerRequest *request){
        String json = "{";
        json += "\"temp\":\"" + String(dhtOrtalamaAl().sicaklik, 1) + "\",";
        json += "\"hum\":\"" + String(dhtOrtalamaAl().nem, 0) + "\",";
        json += "\"wtemp\":\"" + String(suSicakligiOku(), 1) + "\",";
        json += "\"soil\":\"" + String(toprakNemOrtalamaAl(), 0) + "\",";
        json += "\"saksilar\":[";
        for(int i=0; i<6; i++) {
            json += String(tekliToprakOku(i));
            if(i < 5) json += ",";
        }
        json += "],";
        // Limitleri buraya ekledik ki JavaScript kutuları doldurabilsin
        json += "\"lim\":{";
        json += "\"imin\":\"" + String(oIsiMin, 1) + "\",\"imax\":\"" + String(oIsiMax, 1) + "\",";
        json += "\"nmin\":\"" + String(oNemMin, 0) + "\",\"nmax\":\"" + String(oNemMax, 0) + "\",";
        json += "\"pmin\":\"" + String(phMin, 1) + "\",\"pmax\":\"" + String(phMax, 1) + "\",";
        json += "\"emin\":\"" + String(ecMin, 1) + "\",\"emax\":\"" + String(ecMax, 1) + "\",";
        json += "\"tmin\":\"" + String(tNemMin, 0) + "\",\"tmax\":\"" + String(tNemMax, 0) + "\"";
        json += "}}";
        request->send(200, "application/json", json);
    });

    // 5. DASHBOARD PANELİ (Processor kullanılmıyor, JavaScript JSON'dan okuyor)
    server.on("/panel", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", dashboard_html); 
    });

    // 6. LİMİT KAYDETME
    server.on("/save_limits", HTTP_POST, [](AsyncWebServerRequest *request){
        if(request->hasParam("imin", true)) oIsiMin = request->getParam("imin", true)->value().toFloat();
        if(request->hasParam("imax", true)) oIsiMax = request->getParam("imax", true)->value().toFloat();
        if(request->hasParam("nmin", true)) oNemMin = request->getParam("nmin", true)->value().toFloat();
        if(request->hasParam("nmax", true)) oNemMax = request->getParam("nmax", true)->value().toFloat();
        if(request->hasParam("pmin", true)) phMin = request->getParam("pmin", true)->value().toFloat();
        if(request->hasParam("pmax", true)) phMax = request->getParam("pmax", true)->value().toFloat();
        if(request->hasParam("emin", true)) ecMin = request->getParam("emin", true)->value().toFloat();
        if(request->hasParam("emax", true)) ecMax = request->getParam("emax", true)->value().toFloat();
        if(request->hasParam("tmin", true)) tNemMin = request->getParam("tmin", true)->value().toFloat();
        if(request->hasParam("tmax", true)) tNemMax = request->getParam("tmax", true)->value().toFloat();
        ayarlariKaydet();
        request->redirect("/panel");
    });

    server.begin();
}
#endif