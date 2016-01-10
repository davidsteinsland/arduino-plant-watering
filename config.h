#ifndef TIMER_DISPLAY_H
#define TIMER_DISPLAY_H

/*
LiquidCrystal::LiquidCrystal(uint8_t rs, uint8_t enable,
           uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
           uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
*/

#define COLS 16
#define ROWS 2

#define LCD_PIN_RS 13
#define LCD_PIN_E 12
#define LCD_PIN_D0 11
#define LCD_PIN_D1 10
#define LCD_PIN_D2 9
#define LCD_PIN_D3 8
#define LCD_PIN_D4 7
#define LCD_PIN_D5 6
#define LCD_PIN_D6 5
#define LCD_PIN_D7 4

#define PUMP_PIN 8
#define RTC_INT_PIN 2
#define FLOAT_SWITCH_INT_PIN 3
#define BUTTONS_PIN A0

#define FRAME_0 0
#define FRAME_1 1
#define FRAME_2 2
#define FRAME_3 3

#define TIMER_STATE_SCHEDULE 0
#define TIMER_STATE_HOUR 1
#define TIMER_STATE_MINUTE 2
#define TIMER_STATE_SECOND 3
#define TIMER_STATE_CONFIRM 4
#define TIMER_STATE_SET 5

#define PUMP_SET_TIME 0
#define PUMP_TIME_CONFIRM 1

#define FLOAT_SWITCH_ON 0
#define FLOAT_SWITCH_OFF 1

#define WATER_MIN_RUNTIME 1000
#define WATER_MAX_RUNTIME 10000
#define WATER_RUNTIME 5000


#define PUMP_OFF 0
#define PUMP_ON 1

typedef struct {
  uint8_t state;
  unsigned long state_start;
  uint16_t runtime;
  uint16_t count;
} pump_t;

volatile time_t current_time;

typedef struct {
  uint8_t alarm;
  ALARM_TYPES_t type;
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
} alarm_t;

volatile uint8_t alarmCalled = 0;

/*LiquidCrystal lcd(LCD_PIN_RS, LCD_PIN_E, LCD_PIN_D0, LCD_PIN_D1, LCD_PIN_D2,
  LCD_PIN_D3, LCD_PIN_D4, LCD_PIN_D5, LCD_PIN_D6, LCD_PIN_D7);*/

LiquidCrystal lcd(LCD_PIN_RS, LCD_PIN_E, LCD_PIN_D4, LCD_PIN_D5, LCD_PIN_D6, LCD_PIN_D7);

alarm_t alarm2_def;

void pump_on();
void pump_off();
uint16_t get_pump_count();
uint16_t get_pump_runtime();
void set_pump_runtime(uint16_t);


#endif
