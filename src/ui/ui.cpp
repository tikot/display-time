#include "ui.h"
#include "../globals.h"
#include "events.h"

static const lv_style_prop_t props[] = {LV_STYLE_BG_COLOR, 0};
static lv_style_transition_dsc_t default_trans;
static lv_style_transition_dsc_t bg_transition;
static lv_style_t style_default;
static lv_style_t style_red_bg;
static lv_style_t style_yellow_bg;
static lv_style_t style_green_bg;

UiClass::UiClass() {}

static void transition() {
  lv_style_transition_dsc_init(&default_trans, props, lv_anim_path_linear, 100,
                               100, NULL);
  lv_style_transition_dsc_init(&bg_transition, props, lv_anim_path_linear, 300,
                               0, NULL);
  lv_style_init(&style_default);
  lv_style_set_bg_color(&style_default, lv_color_hex(Ui.bgColor));
  lv_style_set_transition(&style_default, &default_trans);
  lv_style_init(&style_red_bg);
  lv_style_set_bg_color(&style_red_bg, lv_color_hex(Ui.colorRed));
  lv_style_set_transition(&style_red_bg, &bg_transition);
  lv_style_init(&style_yellow_bg);
  lv_style_set_bg_color(&style_yellow_bg, lv_color_hex(Ui.colorYellow));
  lv_style_set_transition(&style_yellow_bg, &bg_transition);
  lv_style_init(&style_green_bg);
  lv_style_set_bg_color(&style_green_bg, lv_color_hex(Ui.colorGreen));
  lv_style_set_transition(&style_green_bg, &bg_transition);
}

void UiClass::_initScreenOne() {
  screenOne = lv_obj_create(NULL);
  lv_obj_clear_flag(screenOne, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_bg_color(screenOne, lv_color_hex(bgColor),
                            LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(screenOne, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

  ///// Timer /////
  timeContainer = lv_obj_create(screenOne);
  lv_obj_remove_style_all(timeContainer);
  lv_obj_set_width(timeContainer, 522);
  lv_obj_set_height(timeContainer, 249);
  lv_obj_set_x(timeContainer, 38);
  lv_obj_set_y(timeContainer, 40);
  lv_obj_clear_flag(timeContainer,
                    LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
  // lv_obj_set_style_bg_color(timeContainer, lv_color_hex(bgColor),
  //                           LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(timeContainer, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_radius(timeContainer, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_color(timeContainer, lv_color_white(),
                                LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_opa(timeContainer, 255,
                              LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(timeContainer, 1,
                                LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_add_style(timeContainer, &style_default, LV_STATE_DEFAULT);
  lv_obj_add_style(timeContainer, &style_red_bg, LV_STATE_USER_1);
  lv_obj_add_style(timeContainer, &style_yellow_bg, LV_STATE_USER_2);
  lv_obj_add_style(timeContainer, &style_green_bg, LV_STATE_USER_3);

  theTimeLabel = lv_label_create(timeContainer);
  lv_obj_set_width(theTimeLabel, LV_SIZE_CONTENT);
  lv_obj_set_height(theTimeLabel, LV_SIZE_CONTENT);
  lv_obj_set_align(theTimeLabel, LV_ALIGN_CENTER);
  lv_label_set_text(theTimeLabel, "00:00:00");
  lv_obj_set_style_text_color(theTimeLabel, lv_color_white(),
                              LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_opa(theTimeLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(theTimeLabel, &font_nato100,
                             LV_PART_MAIN | LV_STATE_DEFAULT);

  ///// Start & Stop Button /////
  startStopButton = lv_btn_create(screenOne);
  lv_obj_set_width(startStopButton, 250);
  lv_obj_set_height(startStopButton, 120);
  lv_obj_set_x(startStopButton, 38);
  lv_obj_set_y(startStopButton, 320);
  lv_obj_add_flag(startStopButton, LV_OBJ_FLAG_CHECKABLE);
  lv_obj_set_style_radius(startStopButton, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(startStopButton, lv_color_hex(colorBlue),
                            LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(startStopButton, 255,
                          LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(startStopButton, lv_color_white(),
                              LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_opa(startStopButton, 255,
                            LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_add_event_cb(startStopButton, UiEvent.startStopEvent, LV_EVENT_ALL,
                      NULL);

  startStopLabel = lv_label_create(startStopButton);
  lv_label_set_text(startStopLabel, "Start");
  lv_obj_center(startStopLabel);
  lv_obj_set_style_text_color(startStopLabel, lv_color_white(),
                              LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_opa(startStopLabel, 255,
                            LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(startStopLabel, &lv_font_montserrat_48,
                             LV_PART_MAIN | LV_STATE_DEFAULT);

  ///// Reset Button /////
  resetButton = lv_btn_create(screenOne);
  lv_obj_set_width(resetButton, 250);
  lv_obj_set_height(resetButton, 120);
  lv_obj_set_x(resetButton, 310);
  lv_obj_set_y(resetButton, 320);
  lv_obj_add_flag(resetButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
  lv_obj_clear_flag(resetButton, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_remove_flag(resetButton, LV_OBJ_FLAG_PRESS_LOCK);
  lv_obj_add_state(resetButton, LV_STATE_DISABLED);
  lv_obj_set_style_radius(resetButton, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(resetButton, lv_color_hex(colorGray),
                            LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(resetButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(resetButton, lv_color_white(),
                              LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_opa(resetButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_add_event_cb(resetButton, UiEvent.resetEvent, LV_EVENT_CLICKED, NULL);

  resetLabel = lv_label_create(resetButton);
  lv_obj_set_width(resetLabel, LV_SIZE_CONTENT);
  lv_obj_set_height(resetLabel, LV_SIZE_CONTENT);
  lv_obj_set_align(resetLabel, LV_ALIGN_CENTER);
  lv_label_set_text(resetLabel, "Reset");
  lv_obj_set_style_text_color(resetLabel, lv_color_white(),
                              LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_opa(resetLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(resetLabel, &lv_font_montserrat_48,
                             LV_PART_MAIN | LV_STATE_DEFAULT);

  ///// Red Button /////
  redButton = lv_btn_create(screenOne);
  lv_obj_set_width(redButton, 146);
  lv_obj_set_height(redButton, 115);
  lv_obj_set_x(redButton, 619);
  lv_obj_set_y(redButton, 40);
  lv_obj_add_flag(redButton, LV_OBJ_FLAG_CHECKABLE);
  lv_obj_set_style_radius(redButton, 11, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(redButton, lv_color_hex(colorRed),
                            LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(redButton, lv_color_hex(colorRed),
                            LV_STATE_CHECKED);
  lv_obj_set_style_bg_opa(redButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_add_event_cb(redButton, UiEvent.changeTimeToRed, LV_EVENT_ALL, NULL);

  ///// Yellow Button /////
  yellowButton = lv_btn_create(screenOne);
  lv_obj_set_width(yellowButton, 146);
  lv_obj_set_height(yellowButton, 115);
  lv_obj_set_x(yellowButton, 619);
  lv_obj_set_y(yellowButton, 182);
  lv_obj_add_flag(yellowButton, LV_OBJ_FLAG_CHECKABLE);
  lv_obj_set_style_radius(yellowButton, 11, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(yellowButton, lv_color_hex(colorYellow),
                            LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(yellowButton, lv_color_hex(colorYellow),
                            LV_STATE_CHECKED);
  lv_obj_set_style_bg_opa(yellowButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_add_event_cb(yellowButton, UiEvent.changeTimeToYellow, LV_EVENT_ALL,
                      NULL);

  ///// Greed Button /////
  greenButton = lv_btn_create(screenOne);
  lv_obj_set_width(greenButton, 146);
  lv_obj_set_height(greenButton, 115);
  lv_obj_set_x(greenButton, 619);
  lv_obj_set_y(greenButton, 325);
  lv_obj_add_flag(greenButton, LV_OBJ_FLAG_CHECKABLE);
  lv_obj_set_style_radius(greenButton, 11, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(greenButton, lv_color_hex(colorGreen),
                            LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(greenButton, lv_color_hex(colorGreen),
                            LV_STATE_CHECKED);
  lv_obj_set_style_bg_opa(greenButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_add_event_cb(greenButton, UiEvent.changeTimeToGreen, LV_EVENT_ALL,
                      NULL);
}

void UiClass::_initScreenTwo() {
  screenTwo = lv_obj_create(NULL);
  lv_obj_clear_flag(screenTwo, LV_OBJ_FLAG_SCROLLABLE);
  // lv_obj_set_style_bg_color(screenTwo, lv_color_hex(0x0000FF),
  //                           LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(screenTwo, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_add_style(screenTwo, &style_default, LV_STATE_DEFAULT);
  lv_obj_add_style(screenTwo, &style_red_bg, LV_STATE_USER_1);
  lv_obj_add_style(screenTwo, &style_yellow_bg, LV_STATE_USER_2);
  lv_obj_add_style(screenTwo, &style_green_bg, LV_STATE_USER_3);

  theTimeLabel = lv_label_create(screenTwo);
  lv_obj_set_width(theTimeLabel, LV_SIZE_CONTENT);
  lv_obj_set_height(theTimeLabel, LV_SIZE_CONTENT);
  lv_obj_set_align(theTimeLabel, LV_ALIGN_CENTER);
  lv_label_set_text(theTimeLabel, "00:00:00");
  lv_obj_set_style_text_color(theTimeLabel, lv_color_white(),
                              LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_opa(theTimeLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(theTimeLabel, &font_nato150,
                             LV_PART_MAIN | LV_STATE_DEFAULT);

  ledIndicator = lv_led_create(screenTwo);
  lv_obj_set_size(ledIndicator, 5, 5);
  lv_obj_align(ledIndicator, LV_ALIGN_BOTTOM_LEFT, 4, -4);
  lv_led_set_brightness(ledIndicator, 150);
  lv_led_set_color(ledIndicator, lv_palette_main(LV_PALETTE_LIGHT_BLUE));
}

void UiClass::init() {
  lv_disp_t* dispDefault = lv_disp_get_default();
  lv_theme_t* theme = lv_theme_default_init(
      dispDefault, lv_palette_main(LV_PALETTE_BLUE),
      lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
  lv_disp_set_theme(dispDefault, theme);

  transition();

#ifdef _CONTROL_PANEL
  _initScreenOne();
  lv_disp_load_scr(screenOne);
#else
  _initScreenTwo();
  lv_disp_load_scr(screenTwo);
#endif
}

UiClass Ui;
