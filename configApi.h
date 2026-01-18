#ifndef CONFIG_API_H
#define CONFIG_API_H

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h> // Burayı Async olarak güncelledik
#include <Preferences.h>

// --- ESP32 PINLERI ---
#define TFT_CS   16
#define TFT_RST  17
#define TFT_DC   5
#define TFT_MOSI 23
#define TFT_CLK  18

#define BTN_PIN_GERI   15
#define BTN_PIN_YUKARI 0
#define BTN_PIN_ASAGI  2
#define BTN_PIN_OK     4

// --- DURUMLAR ---
#define BASILMADI 0
#define TUS_GERI   1
#define TUS_YUKARI 2
#define TUS_ASAGI 3
#define TUS_OK     4

// --- RENK PALETİ ---
#define ARKA_PLAN   0x0000 
#define KART_ZEMIN  0x2124 
#define YAZI_GRI    0xBDF7 
#define BEYAZ       0xFFFF 
#define ACCENT_KIRM 0xE2C9 
#define ACCENT_MAVI 0x349F 
#define ACCENT_CYAN 0x05F7 
#define ACCENT_SARI 0xFDC0 
#define ACCENT_YESL 0x2ECC 
#define ACCENT_MOR  0x9B3F 
#define ACCENT_TUR  0xE460 

// --- GLOBAL DEĞİŞKENLER (Sadece Bildirim) ---
extern float phMin, phMax, ecMin, ecMax, oIsiMin, oIsiMax, oNemMin, oNemMax, sIsiMin, sIsiMax, tNemMin, tNemMax;
extern Adafruit_ILI9341 tft;
extern AsyncWebServer server;
extern Preferences preferences;

#endif