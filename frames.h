#ifndef TIMER_FRAMES_H
#define TIMER_FRAMES_H

#include "config.h"
#include "lcd.h"

#define TIMER_DISABLE 0
#define TIMER_ENABLE 1
#define TIMER_CONTROL_SCHEDULE 0
#define TIMER_SET_SCHEDULE 1

uint8_t _alarm1_called = 0,
  _alarm2_called = 0;

uint8_t alarm1_called() {
  if (_alarm1_called) {
    _alarm1_called = 0;
    return 1;
  }

  return 0;
}

uint8_t alarm2_called() {
  if (_alarm2_called) {
    _alarm2_called = 0;
    return 1;
  }

  return 0;
}

/* frame 1 stuff */
uint8_t frame1_alarm_state = 0,
  frame1_state = 0;

/* frame 2 stuff */
alarm_t user_timer;
uint8_t frame2_state = 0;
unsigned long frame2_blink_time = 0;
uint8_t frame2_blink_state = 0;

void frame0_draw_alarm1() {
  lcd_clear_row(ROWS - 1, 8);
  lcd_print_two_digit(hour(current_time));
  lcd.print(':');
  lcd_print_two_digit(minute(current_time));
  lcd.print(':');
  lcd_print_two_digit(second(current_time));
}

/* gets called as soon as the alarm occurs, no matter
  which frame is active */
void handle_alarm1() {
  current_time = now();
  _alarm1_called = 1;
}

/* gets called as soon as the alarm occurs, no matter
  which frame is active */
void handle_alarm2() {
  _alarm2_called = 1;
}

void frame0_draw_alarm2() {
  lcd_clear_row(ROWS - 1, 8, 8);
  lcd.print(" PMP: ");
  lcd.print(get_pump_count());
}

void frame0_draw() {
  lcd_clear_row(0);
  lcd.print("WATER PUMPER 1.0");
  lcd_clear_row(1);

  frame0_draw_alarm1();
  frame0_draw_alarm2();
}

void frame0() {
  if (frame_needs_redraw()) {
    frame0_draw();
  }

  if (alarm1_called()) {
    frame0_draw_alarm1();
  }

  if (alarm2_called()) {
    /* this would be the place we we'd start the pump */
    pump_on();

    frame0_draw_alarm2();
  }

  int8_t btn = button_handle();

  if (btn == 0) {
    switch_frame(0);
  } else if (btn == 1) {
    switch_frame(FRAME_1);
  } else if (btn == 2) {
    switch_frame(FRAME_2);
  }
}

void frame1_draw() {
  lcd_clear_row(0);

  if (is_alarm_active(ALARM_2)) {
    lcd.print("Timer enabled.");
    frame1_alarm_state = TIMER_DISABLE;
  } else {
    lcd.print("Timer disabled.");
    frame1_alarm_state = TIMER_ENABLE;
  }

  lcd_clear_row(1);

  frame1_state = TIMER_CONTROL_SCHEDULE;
  frame2_blink_time = 0;
  frame2_blink_state = 0;
}

void frame1() {
  if (frame_needs_redraw()) {
    frame1_draw();
  }

  switch (frame1_state) {
    case TIMER_CONTROL_SCHEDULE:
      {
        char* types[] = {
          "Disable timer.",
          "Enable timer."
        };

        frame_blink_text(&frame2_blink_time, &frame2_blink_state, types[frame1_alarm_state], 0, 1);
        frame_button_handle(&frame1_alarm_state, TIMER_DISABLE, TIMER_ENABLE, 0, 1, &frame1_state, TIMER_SET_SCHEDULE);
      }
    break;

    case TIMER_SET_SCHEDULE:
      RTC.alarmInterrupt(ALARM_2, frame1_alarm_state);
      switch_frame(FRAME_0);
      break;
  }
}

void frame2_draw() {
  lcd_clear_row(0);
  lcd.print("Set timer");
  lcd_clear_row(1);

  user_timer = alarm2_def;

  frame2_state = TIMER_STATE_SCHEDULE;
  frame2_blink_time = 0;
  frame2_blink_state = 0;
}

void frame2() {
  if (frame_needs_redraw()) {
    frame2_draw();
  }

  if ((millis() - get_last_button_activity()) > 10000) {
    switch_frame(0);
    return;
  }

  /* 0 => select hour, 1 => select minute, 2 => select second, 3 => cancel */
  switch (frame2_state) {
    case TIMER_STATE_SCHEDULE:
      {
        frame_blink_text(&frame2_blink_time, &frame2_blink_state, alarm2s(user_timer.type), 0, 1);

        #define TYPES_SIZE 3
        uint8_t types[TYPES_SIZE] = {
          (uint8_t)ALM2_EVERY_MINUTE,
          (uint8_t)ALM2_MATCH_MINUTES,
          (uint8_t)ALM2_MATCH_HOURS
        };

        if (frame_button_handle_enum((uint8_t*)&user_timer.type, types, TYPES_SIZE, 0, 1, &frame2_state, TIMER_STATE_HOUR)) {
          switch (user_timer.type) {
            case ALM2_MATCH_HOURS:
              {
                lcd_print_two_digit(user_timer.hours);
                lcd.print(':');
                lcd_print_two_digit(user_timer.minutes);
                lcd.print(':');
                lcd_print_two_digit(user_timer.seconds);
              }
            break;

            case ALM2_EVERY_MINUTE:
              {
                frame2_state = TIMER_STATE_CONFIRM;
                lcd_clear_row(0);
                lcd.print("Confirm time");
              }
            break;
            case ALM2_MATCH_MINUTES:
              {
                lcd.print("   ");
                lcd_print_two_digit(user_timer.minutes);
                lcd.print(':');
                lcd_print_two_digit(user_timer.seconds);
                frame2_state = TIMER_STATE_MINUTE;
              }
            break;
          }
        }
      }
    break;

   case TIMER_STATE_HOUR:
    frame_blink_text(&frame2_blink_time, &frame2_blink_state, user_timer.hours, 0, 1);
    frame_button_handle(&user_timer.hours, 0, 23, 0, 1, &frame2_state, TIMER_STATE_MINUTE);
    break;

    case TIMER_STATE_MINUTE:
      frame_blink_text(&frame2_blink_time, &frame2_blink_state, user_timer.minutes, 3, 1);
      frame_button_handle(&user_timer.minutes, 0, 59, 3, 1, &frame2_state, TIMER_STATE_SECOND);
    break;

    case TIMER_STATE_SECOND:
      frame_blink_text(&frame2_blink_time, &frame2_blink_state, user_timer.seconds, 6, 1);
      if (frame_button_handle(&user_timer.seconds, 0, 59, 6, 1, &frame2_state, TIMER_STATE_CONFIRM)) {
        lcd_clear_row(0);
        lcd.print("Confirm time");
      }
    break;

    case TIMER_STATE_CONFIRM:
      {
        int8_t btn = button_handle();
        // CANCEL
        if (btn == 0 || btn == 1) {
          switch_frame(FRAME_0);
          return;
        } else if (btn == 2) {
          // CONFIRM
          lcd_clear_row(1);
          lcd.print("Time set.");
          frame2_state = TIMER_STATE_SET;

          setAlarm(ALARM_2, &user_timer);
          alarm2_def = user_timer;
        }
      }
    break;

    case TIMER_STATE_SET:
      {
        int8_t btn = button_handle();
        if (btn == 2) {
          switch_frame(FRAME_0);
        }
      }
    break;
  }
}

#endif
