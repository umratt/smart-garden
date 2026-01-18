#ifndef MUX_MANAGER_H
#define MUX_MANAGER_H

#include <Arduino.h>
#include "configApi.h"

// Pin Tanımları
#define MUX_SIG  34   
#define MUX_S3   32   
#define MUX_S2   33   
#define MUX_S1   25   
#define MUX_S0   26   

// Fonksiyon Prototipleri (Bu satırlar 'Not declared' hatasını çözer)
void muxKurulum();
int muxOku(int kanal);

void muxKurulum() {
  pinMode(MUX_S0, OUTPUT);
  pinMode(MUX_S1, OUTPUT);
  pinMode(MUX_S2, OUTPUT);
  pinMode(MUX_S3, OUTPUT);
  pinMode(MUX_SIG, INPUT);
}

int muxOku(int kanal) {
  digitalWrite(MUX_S0, bitRead(kanal, 0));
  digitalWrite(MUX_S1, bitRead(kanal, 1));
  digitalWrite(MUX_S2, bitRead(kanal, 2));
  digitalWrite(MUX_S3, bitRead(kanal, 3));
  delayMicroseconds(50); 
  return analogRead(MUX_SIG);
}

#endif