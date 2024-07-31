#include <Arduino.h>
#include <ArduinoBLE.h>
#include <Arduino_GigaDisplayTouch.h>
#include <Arduino_H7_Video.h>

#include <lvgl.h>
#include "src/in.h"

Arduino_H7_Video Display(800, 480, GigaDisplayShield);
Arduino_GigaDisplayTouch Touch;

void setup() {
#ifdef _DEBUG_PANEL
  Serial.begin(9600);
  while (!Serial)
    ;
#endif

  Display.begin();
  Touch.begin();
  Ui.init();
  setupBlue();
}

void loop() {
  lv_timer_handler();
  handle();
}
