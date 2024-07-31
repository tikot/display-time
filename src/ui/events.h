#ifndef _EVENTS_H_
#define _EVENTS_H_

#include <lvgl.h>

class EventClass {
 public:
  static void startStopEvent(lv_event_t* e);
  static void resetEvent(lv_event_t* e);
  static void changeTimeToRed(lv_event_t* e);
  static void changeTimeToYellow(lv_event_t* e);
  static void changeTimeToGreen(lv_event_t* e);
};

extern EventClass UiEvent;

#endif
