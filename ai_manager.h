#ifndef AI_MANAGER_H
#define AI_MANAGER_H

#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <WiFiClientSecure.h>
#include "ec_sensor.h" // BU SATIR EKLENDİ (Hatayı çözen kısım)

// --- API SABİTLERİ ---
const String groq_api_key = "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"; 
const String GROQ_URL = "https://api.groq.com/openai/v1/chat/completions";

extern float oIsiMin, oIsiMax, oNemMin, oNemMax, tNemMin, tNemMax, sIsiMin, sIsiMax;
extern float phMin, phMax, ecMin, ecMax;

String aiDanisman(String soru) {
    if (WiFi.status() != WL_CONNECTED) return "⚠️ WiFi yok.";

    Preferences p;
    p.begin("bahce_ai", false); 
    String suAnkiBitki = p.getString("bitki_turu", "Çilek");

    if (soru.indexOf("yetistiriyorum") != -1 || soru.indexOf("yetiştiriyorum") != -1) {
        String yeniBitki = soru;
        yeniBitki.replace("/ai ", "");
        p.putString("bitki_turu", yeniBitki);
        p.end();
        return "✅ Bilgi kaydedildi: `" + yeniBitki + "`.";
    }
    p.end();

    // --- GERÇEK VERİLERİ TOPLA ---
    DHTVeri dht = dhtOrtalamaAl();
    float tNem = toprakNemOrtalamaAl();
    float suT = suSicakligiOku();
    
    // ARTIK HATA VERMEYECEK
    float anlikEC = ecOrtalamaOku(); 
    float anlikPH = 6.2; 

    String teknikVeri = "Bitki:" + suAnkiBitki + "\n";
    teknikVeri += "DURUM: Hava:" + String(dht.sicaklik, 1) + "C, ToprakNem:%" + String(tNem, 0) + ", EC:" + String(anlikEC, 3) + "mS, Su:" + String(suT, 1) + "C\n";
    
    teknikVeri += "LİMİTLERİN: Hava(" + String(oIsiMin,0) + "-" + String(oIsiMax,0) + "C), ";
    teknikVeri += "ToprakNem(%" + String(tNemMin,0) + "-%" + String(tNemMax,0) + "), ";
    teknikVeri += "EC(" + String(ecMin,1) + "-" + String(ecMax,1) + "mS)";

    WiFiClientSecure client_ai;
    client_ai.setInsecure();
    
    HTTPClient http;
    http.begin(client_ai, GROQ_URL);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + groq_api_key); 
    http.setTimeout(10000); 

    String sistemRolu = "Sen teknik bir hidroponik uzmanısın. SADECE sana verilen 'LİMİTLERİN' dışındaki değerleri hata kabul et. ";
    sistemRolu += "Kendi kafandan ideal aralık uydurma. Çok kısa teknik emir ver. Max 15 kelime.";

    JsonDocument doc; 
    doc["model"] = "llama-3.1-8b-instant";
    JsonArray msgs = doc["messages"].to<JsonArray>();
    
    JsonObject sys = msgs.add<JsonObject>();
    sys["role"] = "system";
    sys["content"] = sistemRolu + "\nTeknik Durum:\n" + teknikVeri;
    
    JsonObject usr = msgs.add<JsonObject>();
    usr["role"] = "user";
    usr["content"] = soru;

    String payload;
    serializeJson(doc, payload);

    int httpCode = http.POST(payload);
    String cevap = "";

    if (httpCode == 200) {
        JsonDocument resDoc;
        deserializeJson(resDoc, http.getString());
        cevap = resDoc["choices"][0]["message"]["content"].as<String>();
    } else {
        cevap = "🤖 Hata: " + String(httpCode);
    }
    
    http.end();
    return cevap;
}
#endif