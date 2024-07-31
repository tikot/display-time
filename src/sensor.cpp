#include "sensor.h"
#include <Arduino.h>

Sensor::Sensor() {}

void Sensor::begin(uint8_t pin, uint8_t threshold) {
  _pin = pin;
  _threshold = threshold;
  pinMode(pin, INPUT);

  pinMode(LEDB, OUTPUT);
}

uint16_t Sensor::getDistance() {
  return 0;
}

int Sensor::getRawData() {
  return analogRead(_pin);
}

bool Sensor::withInThreshold() {
  return _state ? false : true;
}

void Sensor::changeState(PinStatus reading) {
  if (reading == HIGH && _previous == LOW && millis() - _time > _debounce) {
    if (_state == HIGH) {
      _state = LOW;
    } else {
      _state = HIGH;
    }

    _time = millis();
  }

  digitalWrite(LEDB, _state);

  _previous = reading;
}
