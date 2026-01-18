#ifndef BUTTON_H
#define BUTTON_H

#include "configApi.h"

// Butonların basılı tutulma durumunu takip eden değişkenler
bool btnYukariAktif = false;
bool btnAsagiAktif = false;
bool btnOkAktif = false;
bool btnGeriAktif = false;

void butonKurulum() {
    // Pin modlarını ayarla
    pinMode(BTN_PIN_GERI, INPUT_PULLUP);
    pinMode(BTN_PIN_YUKARI, INPUT_PULLUP);
    pinMode(BTN_PIN_ASAGI, INPUT_PULLUP);
    pinMode(BTN_PIN_OK, INPUT_PULLUP);
}

int butonOku() {
    // --- YUKARI BUTONU ---
    if (digitalRead(BTN_PIN_YUKARI) == LOW) {
        if (!btnYukariAktif) { // Daha önce basılmadıysa işlem yap
            delay(50); // Parazit engelleme (Debounce)
            if (digitalRead(BTN_PIN_YUKARI) == LOW) {
                btnYukariAktif = true; // Basıldı olarak işaretle
                return TUS_YUKARI;
            }
        }
    } else {
        btnYukariAktif = false; // Parmağını çekti, sıfırla
    }

    // --- AŞAĞI BUTONU ---
    if (digitalRead(BTN_PIN_ASAGI) == LOW) {
        if (!btnAsagiAktif) {
            delay(50);
            if (digitalRead(BTN_PIN_ASAGI) == LOW) {
                btnAsagiAktif = true;
                return TUS_ASAGI;
            }
        }
    } else {
        btnAsagiAktif = false;
    }

    // --- OK BUTONU ---
    if (digitalRead(BTN_PIN_OK) == LOW) {
        if (!btnOkAktif) {
            delay(50);
            if (digitalRead(BTN_PIN_OK) == LOW) {
                btnOkAktif = true;
                return TUS_OK;
            }
        }
    } else {
        btnOkAktif = false;
    }

    // --- GERİ BUTONU ---
    if (digitalRead(BTN_PIN_GERI) == LOW) {
        if (!btnGeriAktif) {
            delay(50);
            if (digitalRead(BTN_PIN_GERI) == LOW) {
                btnGeriAktif = true;
                return TUS_GERI;
            }
        }
    } else {
        btnGeriAktif = false;
    }

    return BASILMADI;
}

#endif