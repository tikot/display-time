#ifndef _UI_H_
#define _UI_H_

#include <lvgl.h>

LV_FONT_DECLARE(font_nato100);
LV_FONT_DECLARE(font_nato150);

class UiClass {
 public:
  static constexpr int bgColor{0x232125};
  static constexpr int colorRed{0xD50000};
  static constexpr int colorYellow{0xFFD600};
  static constexpr int colorGreen{0x2ECC40};
  static constexpr int colorBlue{0x59ADFF};
  static constexpr int colorGray{0x808080};

  lv_obj_t* screenOne;
  lv_obj_t* screenTwo;

  lv_obj_t* theTimeLabel;
  lv_obj_t* ledIndicator;
  lv_obj_t* timeContainer;
  lv_obj_t* startStopButton;
  lv_obj_t* startStopLabel;
  lv_obj_t* resetButton;
  lv_obj_t* resetLabel;
  lv_obj_t* redButton;
  lv_obj_t* yellowButton;
  lv_obj_t* greenButton;

  UiClass();

  void init(void);

 private:
  void _initScreenOne(void);
  void _initScreenTwo(void);
};

extern UiClass Ui;

#endif
