#include "in.h"
#include <ArduinoBLE.h>
#include <Arduino_GigaDisplay.h>
#include <Scheduler.h>
#include <mbed.h>
#include "ui/events.h"

#ifdef _CONTROL_PANEL
BLEService actionService(BLE_UUID_SERVICE);
BLEByteCharacteristic timerCharacteristic(BLE_UUID_TIMER, BLEWrite);
BLEByteCharacteristic dataCharacteristic(BLE_UUID_DATA, BLERead | BLENotify);
#endif

#ifndef _CONTROL_PANEL
GigaDisplayRGB rgb;
#endif;

constexpr unsigned long timeInterval{1000};
unsigned long updateTime{};
bool isTimerCounting{false};
volatile long start;
volatile long end;

bool ignoreSensor{false};

#ifdef _SENSOR
Sensor sensor;
volatile long wait;
bool updateControl{false};
#endif

/* UI hold timers */
#ifdef _GREEN_30_LIMIT
lv_timer_t* uiTimer;
#endif
lv_timer_t* bgTimer;

void updateElapsedTime() {
  long now = millis();
  if (now > updateTime) {
    long msTime = now - start;
    long seconds = (msTime / 1000) % 60;
    long minutes = (msTime / 60000) % 60;
    long hours = (msTime / 3600000) % 24;

    lv_label_set_text_fmt(Ui.theTimeLabel, "%02d:%02d:%02d", hours, minutes,
                          seconds);

    updateTime = now + timeInterval;
  }
}

void startTime() {
  if (start == 0) {
    start = millis();
  } else {
    start = millis() - (end - start);
  }
  isTimerCounting = true;
}

void stopTime() {
  isTimerCounting = false;
  end = millis();
}

void resetTime() {
  isTimerCounting = false;
  start = 0;
  end = 0;
  lv_label_set_text(Ui.theTimeLabel, "00:00:00");
}

void changeTextColor(lv_color_t textColor) {
  lv_obj_set_style_text_color(Ui.theTimeLabel, textColor,
                              LV_PART_MAIN | LV_STATE_DEFAULT);
}

#ifdef _GREEN_30_LIMIT
/**
 * Show green background for 30 seconds only
 */
void resetGreenTime(lv_timer_t* timer) {
#ifdef _CONTROL_PANEL
  if ((lv_obj_get_state(Ui.greenButton) & LV_STATE_CHECKED)) {
    changeTextColor(lv_color_white());
    lv_obj_clear_state(Ui.greenButton, LV_STATE_CHECKED);
    lv_obj_remove_state(Ui.timeContainer, LV_STATE_USER_3);
  }
#else
  if (lv_obj_has_state(Ui.screenTwo, LV_STATE_USER_3)) {
    changeTextColor(lv_color_white());
    lv_obj_remove_state(Ui.screenTwo, LV_STATE_USER_3);
  }
#endif
}

void createGreenBlink() {
  if (uiTimer) {
    lv_timer_delete(uiTimer);
  }
  uiTimer = lv_timer_create(resetGreenTime, 30000, NULL);
}
#endif

/**
 * Flash the screen with color when STATE_USER_1 is set
 * Also flash time container in control panel.
 */
static void flashBgColor(lv_timer_t* timer) {
#ifdef _CONTROL_PANEL
  if (lv_obj_has_state(Ui.timeContainer, LV_STATE_USER_1)) {
    lv_obj_set_state(Ui.timeContainer, LV_STATE_USER_1, false);
  } else {
    lv_obj_set_state(Ui.timeContainer, LV_STATE_USER_1, true);
  }
#else
  if (lv_obj_has_state(Ui.screenTwo, LV_STATE_USER_1)) {
    lv_obj_set_state(Ui.screenTwo, LV_STATE_USER_1, false);
  } else {
    lv_obj_set_state(Ui.screenTwo, LV_STATE_USER_1, true);
  }
#endif
}

void createRedBlink() {
  if (!bgTimer) {
    bgTimer = lv_timer_create_basic();
    lv_timer_set_cb(bgTimer, flashBgColor);
    lv_timer_set_period(bgTimer, 800);
    lv_timer_set_repeat_count(bgTimer, -1);
  } else {
    lv_timer_resume(bgTimer);
  }
}

void pauseRedBlink() {
  if (bgTimer) {
    lv_timer_pause(bgTimer);
  }
}

void EventClass::startStopEvent(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t* obj = lv_event_get_current_target_obj(e);
  if (code == LV_EVENT_VALUE_CHANGED) {
    if ((lv_obj_get_state(obj) & LV_STATE_CHECKED)) {
      lv_label_set_text(Ui.startStopLabel, "Stop");
      lv_obj_add_state(Ui.resetButton, LV_STATE_DISABLED);
      startTime();
#ifdef _CONTROL_PANEL
      if (dataCharacteristic.subscribed()) {
        dataCharacteristic.writeValue(TIMER_START);
      }
#endif
    } else {
      lv_label_set_text(Ui.startStopLabel, "Resume");
      lv_obj_remove_state(Ui.resetButton, LV_STATE_DISABLED);
      stopTime();
#ifdef _CONTROL_PANEL
      if (dataCharacteristic.subscribed()) {
        dataCharacteristic.writeValue(TIMER_STOP);
      }
#endif
    }
  }
}

void EventClass::resetEvent(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_CLICKED) {
    lv_label_set_text(Ui.startStopLabel, "Start");
    lv_obj_remove_state(Ui.startStopButton, LV_STATE_CHECKED);
    lv_obj_add_state(Ui.resetButton, LV_STATE_DISABLED);
    resetTime();
#ifdef _CONTROL_PANEL
    if (dataCharacteristic.subscribed()) {
      dataCharacteristic.writeValue(TIMER_RESET);
    }
#endif
  }
}

/**
 * Click event to change time container background color to Red.
 */
void EventClass::changeTimeToRed(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t* obj = lv_event_get_current_target_obj(e);
  if (code == LV_EVENT_VALUE_CHANGED) {
    if ((lv_obj_get_state(obj) & LV_STATE_CHECKED)) {
      lv_obj_clear_state(Ui.yellowButton, LV_STATE_CHECKED);
      lv_obj_clear_state(Ui.greenButton, LV_STATE_CHECKED);
      lv_obj_add_state(Ui.timeContainer, LV_STATE_USER_1);
      lv_obj_remove_state(Ui.timeContainer, LV_STATE_USER_2);
      lv_obj_remove_state(Ui.timeContainer, LV_STATE_USER_3);
      changeTextColor(lv_color_white());
      createRedBlink();
#ifdef _CONTROL_PANEL
      if (dataCharacteristic.subscribed()) {
        dataCharacteristic.writeValue(COLOR_RED);
      }
#endif
    } else {
      lv_obj_remove_state(Ui.timeContainer, LV_STATE_USER_1);
      changeTextColor(lv_color_white());
      pauseRedBlink();
#ifdef _CONTROL_PANEL
      if (dataCharacteristic.subscribed()) {
        dataCharacteristic.writeValue(COLOR_RESET);
      }
#endif
    }
  }
}

/**
 * Click event to change time container background color to Yellow.
 */
void EventClass::changeTimeToYellow(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t* obj = lv_event_get_current_target_obj(e);
  if (code == LV_EVENT_VALUE_CHANGED) {
    if ((lv_obj_get_state(obj) & LV_STATE_CHECKED)) {
      lv_obj_clear_state(Ui.redButton, LV_STATE_CHECKED);
      lv_obj_clear_state(Ui.greenButton, LV_STATE_CHECKED);
      lv_obj_add_state(Ui.timeContainer, LV_STATE_USER_2);
      lv_obj_remove_state(Ui.timeContainer, LV_STATE_USER_1);
      lv_obj_remove_state(Ui.timeContainer, LV_STATE_USER_3);
      changeTextColor(lv_color_hex(0x1B1806));
#ifdef _CONTROL_PANEL
      if (dataCharacteristic.subscribed()) {
        dataCharacteristic.writeValue(COLOR_YELLOW);
      }
#endif
    } else {
      lv_obj_remove_state(Ui.timeContainer, LV_STATE_USER_2);
      changeTextColor(lv_color_white());
      pauseRedBlink();
#ifdef _CONTROL_PANEL
      if (dataCharacteristic.subscribed()) {
        dataCharacteristic.writeValue(COLOR_RESET);
      }
#endif
    }
  }
}

/**
 * Click event to change time container background color to Green.
 */
void EventClass::changeTimeToGreen(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t* obj = lv_event_get_current_target_obj(e);
  if (code == LV_EVENT_VALUE_CHANGED) {
    if ((lv_obj_get_state(obj) & LV_STATE_CHECKED)) {
      lv_obj_clear_state(Ui.redButton, LV_STATE_CHECKED);
      lv_obj_clear_state(Ui.yellowButton, LV_STATE_CHECKED);
      lv_obj_add_state(Ui.timeContainer, LV_STATE_USER_3);
      lv_obj_remove_state(Ui.timeContainer, LV_STATE_USER_1);
      lv_obj_remove_state(Ui.timeContainer, LV_STATE_USER_2);
      changeTextColor(lv_color_hex(0x0C3423));
#ifdef _GREEN_30_LIMIT
      createGreenBlink();
#endif
#ifdef _CONTROL_PANEL
      if (dataCharacteristic.subscribed()) {
        dataCharacteristic.writeValue(COLOR_GREEN);
      }
#endif
    } else {
      lv_obj_remove_state(Ui.timeContainer, LV_STATE_USER_3);
      changeTextColor(lv_color_white());
      pauseRedBlink();
#ifdef _CONTROL_PANEL
      if (dataCharacteristic.subscribed()) {
        dataCharacteristic.writeValue(COLOR_RESET);
      }
#endif
    }
  }
}

// loop for BLE controls
#ifdef _CONTROL_PANEL
void loop2() {
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print(F("Connected to central: "));
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {
      // if the remote device wrote to the characteristic,
      // use the value to control the LED:
      if (timerCharacteristic.written()) {
        Serial.print(F("Char written: "));
        Serial.println(timerCharacteristic.value());
        switch (timerCharacteristic.value()) {
          case TIMER_STOP:
            Serial.println(F("Time stop"));
            if (isTimerCounting) {
              lv_label_set_text(Ui.startStopLabel, "Resume");
              lv_obj_remove_state(Ui.startStopButton, LV_STATE_CHECKED);
              lv_obj_remove_state(Ui.resetButton, LV_STATE_DISABLED);
              stopTime();
            }
            break;
          case TIMER_START:
            Serial.println(F("Time start"));
            if (!isTimerCounting) {
              lv_label_set_text(Ui.startStopLabel, "Stop");
              lv_obj_set_state(Ui.startStopButton, LV_STATE_CHECKED, true);
              lv_obj_add_state(Ui.resetButton, LV_STATE_DISABLED);
              startTime();
            }
            break;
          case TIMER_RESET:
            Serial.println(F("Time reset"));
            if (!lv_obj_has_state(Ui.resetButton, LV_STATE_DISABLED)) {
              lv_label_set_text(Ui.startStopLabel, "Start");
              lv_obj_remove_state(Ui.startStopButton, LV_STATE_CHECKED);
              lv_obj_add_state(Ui.resetButton, LV_STATE_DISABLED);
              resetTime();
            }
            break;
          default:
            Serial.println(F("Unsupported action value"));
        }
      }
    }

    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }

  delay(1000);
  yield();
}
#endif

// loop for BLE view
#ifndef _CONTROL_PANEL
void setScreenColorRed(void*) {
  lv_obj_add_state(Ui.screenTwo, LV_STATE_USER_1);
  lv_obj_remove_state(Ui.screenTwo, LV_STATE_USER_2);
  lv_obj_remove_state(Ui.screenTwo, LV_STATE_USER_3);
  changeTextColor(lv_color_white());
  createRedBlink();
}

void setScreenColorYellow(void*) {
  lv_obj_add_state(Ui.screenTwo, LV_STATE_USER_2);
  lv_obj_remove_state(Ui.screenTwo, LV_STATE_USER_1);
  lv_obj_remove_state(Ui.screenTwo, LV_STATE_USER_3);
  changeTextColor(lv_color_hex(0x1B1806));
}

void setScreenColorGreen(void*) {
  lv_obj_add_state(Ui.screenTwo, LV_STATE_USER_3);
  lv_obj_remove_state(Ui.screenTwo, LV_STATE_USER_1);
  lv_obj_remove_state(Ui.screenTwo, LV_STATE_USER_2);
  changeTextColor(lv_color_hex(0x0C3423));
#ifdef _GREEN_30_LIMIT
  createGreenBlink();
#endif
}

void resetScreenColor(void*) {
  lv_obj_remove_state(Ui.screenTwo, LV_STATE_USER_1);
  lv_obj_remove_state(Ui.screenTwo, LV_STATE_USER_2);
  lv_obj_remove_state(Ui.screenTwo, LV_STATE_USER_3);
  changeTextColor(lv_color_white());
  pauseRedBlink();
}

void peripheralActions(BLEDevice device) {
  Serial.println(F("Connecting ..."));
  // BUG returns false but connected
  if (device.connect()) {
    Serial.println(F("Connected"));
  } else {
    Serial.println(F("Failed to connect!"));
    // TODO fix return when BUG fixed because on connection returns false.
    //  return;
  }

  if (!device.discoverService(BLE_UUID_SERVICE)) {
    Serial.println(F("Attribute discovery failed."));
    device.disconnect();
    rgb.on(255, 0, 0);
    return;
  }

  BLECharacteristic timerChar = device.characteristic(BLE_UUID_TIMER);
  BLECharacteristic dataChar = device.characteristic(BLE_UUID_DATA);

  if (!dataChar) {
    Serial.println(F("No data key characteristic found!"));
    device.disconnect();
    rgb.on(255, 0, 0);
    return;
  } else if (!dataChar.canSubscribe()) {
    Serial.println(F("Data key characteristic is not subscribable!"));
    device.disconnect();
    rgb.on(255, 0, 0);
    return;
  } else if (!dataChar.subscribe()) {
    Serial.println(F("Subscription failed!"));
    device.disconnect();
    rgb.on(255, 0, 0);
    return;
  } else {
    Serial.println(F("Subscribed to data"));
    rgb.off();
    lv_led_toggle(Ui.ledIndicator);
  }

  while (device.connected()) {
    if (dataChar.valueUpdated()) {
      byte value = 0;
      dataChar.readValue(value);
      Serial.print(F("dataChar value: "));
      Serial.println(value);
      switch (value) {
        case TIMER_STOP:
          stopTime();
          break;
        case TIMER_START:
          startTime();
          ignoreSensor = true;
          break;
        case TIMER_RESET:
          resetTime();
          ignoreSensor = false;
          break;
        case COLOR_RED:
          lv_async_call(setScreenColorRed, NULL);
          break;
        case COLOR_YELLOW:
          lv_async_call(setScreenColorYellow, NULL);
          break;
        case COLOR_GREEN:
          lv_async_call(setScreenColorGreen, NULL);
          break;
        case COLOR_RESET:
          lv_async_call(resetScreenColor, NULL);
          break;
        default:
          Serial.println(F("Unsupported timer value"));
      }
    }

#ifdef _SENSOR
    if (updateControl) {
      if (isTimerCounting) {
        timerChar.writeValue(TIMER_START);
        updateControl = false;
      } else {
        timerChar.writeValue(TIMER_STOP);
        delay(500);
        timerChar.writeValue(TIMER_RESET);
        updateControl = false;
      }
    }
#endif
  }
}

void loop3() {
  BLEDevice peripheral = BLE.available();

  if (peripheral) {
    if (peripheral.localName() != CONTROL_LOCAL_NAME) {
      return;
    }
    Serial.println(F("Peripheral available!"));
    BLE.stopScan();

    peripheralActions(peripheral);

    BLE.scan();
  } else {
    BLE.scanForUuid(BLE_UUID_SERVICE);
    Serial.println(F("Try scan again"));
  }

  delay(500);
  yield();
}

#endif

#ifdef _SENSOR
void loopSensor() {
  if (ignoreSensor) {
    return;
  }

  // TODO remove when sensor gets built used for testing
  sensor.changeState(digitalRead(PC_13));

  if (isTimerCounting) {
    // check if sensor with in threshold
    if (!sensor.withInThreshold()) {
      if (!wait) {
        wait = millis() + 20000;
      } else if (millis() > wait) {
        wait = 0;
        stopTime();
        resetTime();
        updateControl = true;
      }
    } else {
      if (wait != 0) {
        wait = 0;
      }
    }
  } else {
    if (sensor.withInThreshold()) {
      if (!wait) {
        wait = millis() + 30000;
      } else if (millis() > wait) {
        wait = 0;
        startTime();
        updateControl = true;
      }
    }
  }

  delay(1000);
  yield();
}

#endif

/**
 * Setup all the BLE configurations
 */
void setupBlue(void) {
  if (!BLE.begin()) {
    Serial.println(F("starting Bluetooth® Low Energy module failed!"));
    while (1)
      ;
  }

#ifdef _CONTROL_PANEL
  BLE.setLocalName(CONTROL_LOCAL_NAME);
  BLE.setAdvertisedService(actionService);

  actionService.addCharacteristic(timerCharacteristic);
  actionService.addCharacteristic(dataCharacteristic);
  BLE.addService(actionService);

  BLE.advertise();
  Serial.println(F("BLE Time CTL"));

  // second loop for BLE
  Scheduler.startLoop(loop2);
#else
  // setup indicator
  rgb.begin();
  rgb.on(0, 255, 0);

  int status = BLE.scanForUuid(BLE_UUID_SERVICE);
  Serial.print(F("Found uuid service: "));
  Serial.println(status);

  Scheduler.startLoop(loop3);
#endif

#ifdef _SENSOR
  Serial.println("setup sensor");
  sensor.begin(A0, 40);
  Scheduler.startLoop(loopSensor);
#endif
}

/**
 * Run in a loop for a timer to update ui
 */
void handle(void) {
  if (isTimerCounting) {
    updateElapsedTime();
  }
}
