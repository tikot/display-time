#ifndef _SENSOR_H_
#define _SENSOR_H_

#include <Arduino.h>

class Sensor {
 public:
  Sensor();
  void begin(uint8_t pin, uint8_t threshold);
  void changeState(PinStatus reading);
  uint16_t getDistance();
  int getRawData();
  bool withInThreshold();

 private:
  uint8_t _pin;
  uint8_t _threshold;

  int _state = HIGH;
  int _reading;
  int _previous = LOW;
  unsigned long _time = 0;
  unsigned long _debounce = 200UL;
};

#endif
