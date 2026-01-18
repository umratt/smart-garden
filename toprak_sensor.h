#ifndef TOPRAK_SENSOR_H
#define TOPRAK_SENSOR_H

#include "mux_manager.h"

// 6 sensörün ortalamasını döner
float toprakNemOrtalamaAl() {
  long toplam = 0;
  for (int i = 0; i < 6; i++) {
    toplam += muxOku(i);
  }
  int ortalamaHam = toplam / 6;
  int nemYuzde = map(ortalamaHam, 4095, 0, 0, 100); 
  return constrain(nemYuzde, 0, 100);
}

// Analiz ekranı için tek bir kanalın yüzdesini döner
int tekliToprakOku(int kanal) {
    int ham = muxOku(kanal);
    int yuzde = map(ham, 4095, 0, 0, 100);
    return constrain(yuzde, 0, 100);
}

#endif