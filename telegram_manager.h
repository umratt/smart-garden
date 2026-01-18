#ifndef TELEGRAM_MANAGER_H
#define TELEGRAM_MANAGER_H

#include <UniversalTelegramBot.h>
#include <WiFiClientSecure.h>
#include <rom/rtc.h> 
#include <time.h> 
#include "dhtsensor.h"
#include "toprak_sensor.h"
#include "susensor.h"
#include "isik_sensor.h"
#include "ec_sensor.h"    
#include "hafiza_manager.h"
#include "ai_manager.h" 
#include "internet_ota.h" // YENİ: OTA Dosyası Eklendi

// --- TELEGRAM AYARLARI ---
#define BOT_TOKEN "8330062549:AAGjJ9pajwwbJE-2hzWM0dJJrSHXnE_jWIE"
#define CHAT_ID   "-1003344565848"

const String telegram_token = BOT_TOKEN;
const String telegram_chat_id = CHAT_ID;

WiFiClientSecure client;
UniversalTelegramBot bot("", client); 

unsigned long botSonKontrol = 0;
int periyodikDakika = 0; 
int hataTekrarDakika = 5; 
int sonRaporDakika = -1; 
int sonHataDakika = -1;

// --- YARDIMCI FONKSİYONLAR ---
String restartNedeniBul(RESET_REASON reason) {
  switch (reason) {
    case 1 : return "Güç Kaynağı (Power-on)";
    case 3 : return "Yazılımsal (Software)";
    case 4 : return "Watchdog (Donma Koruması)";
    case 14: return "Brownout (Düşük Voltaj)";
    case 12: return "SW CPU Reset (OTA Sonrası)"; // OTA sonrası genelde bu olur
    default: return "Sistem Döngüsü (" + String(reason) + ")";
  }
}

String barCiz(float yuzde) {
    String bar = "";
    int doluBlok = (int)(yuzde / 10.0); 
    if(doluBlok > 10) doluBlok = 10;
    if(doluBlok < 0) doluBlok = 0;
    for (int i = 0; i < 10; i++) {
        if (i < doluBlok) bar += "🟦"; 
        else bar += "⬜"; 
    }
    return bar;
}

// OTA Durumunu Telegrama Göndermek İçin Callback
void otaDurumGonder(String mesaj) {
    bot.sendMessage(telegram_chat_id, mesaj, "");
}

void limitGuncelle(String cid, String isim, float *degisken, String yeniDeger, String birim) {
    float deger = yeniDeger.toFloat();
    *degisken = deger;
    ayarlariKaydet(); 
    bot.sendMessage(cid, "✅ *" + isim + "* `" + String(deger, 1) + birim + "` yapıldı.", "Markdown");
}

void durumRaporuOlustur(String cid, String baslik) {
    DHTVeri d11 = dht1_Oku(); delay(50);
    DHTVeri d22 = dht2_Oku(); 
    DHTVeri ort = dhtOrtalamaAl(); 
    float suT = suSicakligiOku();
    float tNem = toprakNemOrtalamaAl();
    int isik = isikSeviyesiOku();
    float anlikEC = ecOrtalamaOku(); 
    float anlikPH = 6.2;           

    String msg = baslik + "\n";
    msg += "━━━━━━━━━━━━━━━\n";
    msg += "🧪 *PH:* " + String(anlikPH, 1) + " | ⚡ *EC:* " + String(anlikEC, 3) + " mS\n";
    msg += "━━━━━━━━━━━━━━━\n";
    msg += "🌡 *DHT11:* " + (d11.hata ? "⚠️ HATA!" : String(d11.sicaklik, 1) + "°C") + "\n";
    msg += "🌡 *DHT22:* " + (d22.hata ? "⚠️ HATA!" : String(d22.sicaklik, 1) + "°C") + "\n";
    msg += "📊 *ORTALAMA:* " + String(ort.sicaklik, 1) + "°C / %" + String(ort.nem, 0) + "\n";
    msg += "━━━━━━━━━━━━━━━\n";
    msg += "🌱 *Toprak:* %" + String(tNem, 0) + "\n";
    msg += "🌡 *Su Isısı:* " + String(suT, 1) + "°C\n";
    msg += "☀️ *Işık:* %" + String(isik) + "\n";
    msg += "━━━━━━━━━━━━━━━\n";
    msg += "🛰 *Sinyal:* " + String(WiFi.RSSI()) + " dBm\n\n";
    msg += "🤖 *AI ANALİZİ:* \n" + aiDanisman("Kısaca verileri analiz et.");
    
    bot.sendMessage(cid, msg, "Markdown");
}

void toprakAnaliziGonder(String cid) {
    String analiz = "📊 *TOPRAK NEM ANALİZİ*\n━━━━━━━━━━━━━━━\n";
    float toplam = 0;
    for (int i = 0; i < 6; i++) {
        float deger = tekliToprakOku(i); 
        toplam += deger;
        analiz += "S" + String(i + 1) + ": " + barCiz(deger) + " %" + String(deger, 0) + "\n";
    }
    analiz += "━━━━━━━━━━━━━━━\n📈 *Ortalama:* %" + String(toplam/6.0, 0);
    bot.sendMessage(cid, analiz, "Markdown");
}

void otomatikKontrol() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) return; 
    int suankiDakika = timeinfo.tm_min;

    if (periyodikDakika > 0 && (suankiDakika % periyodikDakika == 0) && suankiDakika != sonRaporDakika) {
        durumRaporuOlustur(telegram_chat_id, "🕒 *OTOMATİK DURUM RAPORU*");
        sonRaporDakika = suankiDakika;
    }

    if ((suankiDakika % hataTekrarDakika == 0) && suankiDakika != sonHataDakika) {
        DHTVeri ort = dhtOrtalamaAl();
        float tNem = toprakNemOrtalamaAl();
        float suT = suSicakligiOku();
        float anlikEC = ecOrtalamaOku();
        float anlikPH = 6.2; 
        String h = "";
        bool acilHata = false;

        if (anlikPH < phMin || anlikPH > phMax) { h += "🧪 pH SINIR DIŞI: " + String(anlikPH, 1) + "\n"; acilHata = true; }
        if (anlikEC < ecMin || anlikEC > ecMax) { h += "⚡ EC SINIR DIŞI: " + String(anlikEC, 3) + " mS\n"; acilHata = true; }
        if (ort.sicaklik > oIsiMax || ort.sicaklik < oIsiMin) h += "🌡 Isı Sınır Dışı: " + String(ort.sicaklik, 1) + "°\n";
        if (tNem < tNemMin || tNem > tNemMax) h += "🥀 Toprak Nem Sınır Dışı: %" + String(tNem, 0) + "\n";
        if (suT > sIsiMax || suT < sIsiMin) h += "❄️ Su Isısı Uygun Değil!\n";

        if (h != "") {
            String baslik = acilHata ? "🚨 *ACİL TEKNİK MÜDAHALE GEREKLİ*\n" : "⚠️ *LİMİT AŞIMI UYARISI*\n";
            String uyarimsj = baslik + "━━━━━━━━━━━━━━━\n" + h + "━━━━━━━━━━━━━━━\n";
            uyarimsj += "🤖 *AI TAVSİYE:* " + aiDanisman("Limit aşımı var: " + h);
            bot.sendMessage(telegram_chat_id, uyarimsj, "Markdown");
            sonHataDakika = suankiDakika;
        } else {
            sonHataDakika = -1;
        }
    }
}

void telegramYonet() {
    otomatikKontrol(); 

    if (millis() - botSonKontrol > 2000) {
        int yeniMesaj = bot.getUpdates(bot.last_message_received + 1);
        while (yeniMesaj) {
            for (int i = 0; i < yeniMesaj; i++) {
                String text = bot.messages[i].text;
                text.trim(); 
                String from_id = bot.messages[i].chat_id;

                if (text == "/durum") durumRaporuOlustur(from_id, "🌿 *ANLIK DURUM RAPORU*");
                else if (text == "/analiz") toprakAnaliziGonder(from_id);
                else if (text == "/limitler") {
                    String L = "⚙️ *AKTİF LİMİTLER*\n━━━━━━━━━━━━━━━\n";
                    L += "🧪 PH: " + String(phMin,1) + " - " + String(phMax,1) + "\n";
                    L += "⚡ EC: " + String(ecMin,1) + " - " + String(ecMax,1) + "\n";
                    L += "🌡 ISI: " + String(oIsiMin,1) + " - " + String(oIsiMax,1) + "\n";
                    L += "💧 NEM: %" + String(oNemMin,0) + " - %" + String(oNemMax,0) + "\n";
                    L += "🌱 TOPRAK: %" + String(tNemMin,0) + " - %" + String(tNemMax,0) + "\n";
                    L += "💧 SU ISI: " + String(sIsiMin,1) + " - " + String(sIsiMax,1) + "\n";
                    L += "━━━━━━━━━━━━━━━";
                    bot.sendMessage(from_id, L, "Markdown");
                }
                
                // --- YENİ: İNTERNETTEN GÜNCELLEME KOMUTU ---
                else if (text.startsWith("/guncelle ")) {
                    String url = text.substring(10); // "/guncelle " sonrasını al
                    url.trim();
                    if(url.length() > 5) {
                        internettenGuncelle(url);
                    } else {
                        bot.sendMessage(from_id, "⚠️ Hata: Link girmedin.\nÖrnek: /guncelle https://site.com/kod.bin");
                    }
                }
                
                else if (text.startsWith("/phmin "))   limitGuncelle(from_id, "pH Alt", &phMin, text.substring(7), "");
                else if (text.startsWith("/phmax "))   limitGuncelle(from_id, "pH Üst", &phMax, text.substring(7), "");
                else if (text.startsWith("/ecmin "))   limitGuncelle(from_id, "EC Alt", &ecMin, text.substring(7), " mS");
                else if (text.startsWith("/ecmax "))   limitGuncelle(from_id, "EC Üst", &ecMax, text.substring(7), " mS");
                else if (text.startsWith("/isimin "))  limitGuncelle(from_id, "Isı Alt", &oIsiMin, text.substring(8), "°C");
                else if (text.startsWith("/isimax "))  limitGuncelle(from_id, "Isı Üst", &oIsiMax, text.substring(8), "°C");
                else if (text.startsWith("/nemmin "))  limitGuncelle(from_id, "Nem Alt", &oNemMin, text.substring(8), "%");
                else if (text.startsWith("/nemmax "))  limitGuncelle(from_id, "Nem Üst", &oNemMax, text.substring(8), "%");
                else if (text.startsWith("/tnemmin ")) limitGuncelle(from_id, "Toprak Alt", &tNemMin, text.substring(9), "%");
                else if (text.startsWith("/tnemmax ")) limitGuncelle(from_id, "Toprak Üst", &tNemMax, text.substring(9), "%");
                else if (text.startsWith("/sisimin ")) limitGuncelle(from_id, "Su Alt", &sIsiMin, text.substring(9), "°C");
                else if (text.startsWith("/sisimax ")) limitGuncelle(from_id, "Su Üst", &sIsiMax, text.substring(9), "°C");
                else if (text.startsWith("/hata ")) {
                    hataTekrarDakika = text.substring(6).toInt();
                    bot.sendMessage(from_id, "⚠️ Hata sıklığı `" + String(hataTekrarDakika) + "` dk yapıldı.");
                }
                else if (text.startsWith("/sure ")) {
                    periyodikDakika = text.substring(6).toInt();
                    bot.sendMessage(from_id, "🕒 Rapor aralığı `" + String(periyodikDakika) + "` dk yapıldı.");
                }
                else if (text == "/restart") {
                    bot.sendMessage(from_id, "🔄 Yeniden başlatılıyor...");
                    delay(1000); ESP.restart();
                }
                else if (!text.startsWith("/")) { 
                    bot.sendChatAction(from_id, "typing"); 
                    String cevap = aiDanisman(text);
                    bot.sendMessage(from_id, "🤖 " + cevap, "Markdown");
                }
            }
            yeniMesaj = bot.getUpdates(bot.last_message_received + 1);
        }
        botSonKontrol = millis(); 
    }
}

void telegramBaslat() {
    client.setInsecure();
    bot.updateToken(telegram_token);
    configTime(10800, 0, "pool.ntp.org", "time.nist.gov");
    
    // OTA Callback Ayarı
    otaCallbackAyarla(otaDurumGonder);

    String sebep = restartNedeniBul(rtc_get_reset_reason(0));
    String fmsg = "⚡ *HİDROPONİK SİSTEM ÇEVRİMİÇİ* ⚡\n━━━━━━━━━━━━━━━━━━━━\n";
    fmsg += "📝 *Restart Nedeni:* `" + sebep + "`\n📡 *IP:* " + WiFi.localIP().toString() + "\n━━━━━━━━━━━━━━━━━━━━";
    bot.sendMessage(telegram_chat_id, fmsg, "Markdown");
}

#endif