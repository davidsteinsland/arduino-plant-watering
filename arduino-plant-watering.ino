#include <avr/wdt.h>

#include <LiquidCrystal.h>
#include <DS3232RTC.h>    //http://github.com/JChristensen/DS3232RTC
#include <Time.h>         //http://www.arduino.cc/playground/Code/Time
#include <Wire.h>         //http://arduino.cc/en/Reference/Wire (included with Arduino IDE)

#include "config.h"
#include "ds3232.h"
#include "buttons.h"
#include "lcd.h"
#include "frames.h"
#include "busy_wait.h"

pump_t pump = {.state = PUMP_OFF, .state_start = 0, .count = 0};

void setup() {
  uint8_t arrow[8] = {
    0b00000,
    0b00100,
    0b00010,
    0b11111,
    0b00010,
    0b00100,
    0b00000,
    0b00000
  };
  lcd.createChar(0, arrow);
  lcd.begin(COLS, ROWS);

  lcd.print("WATER PUMPER 1.0");
  uint16_t _delay = 1000;
  uint16_t _char_delay = _delay / COLS;

  uint8_t i, j = 0;
  for (j = 0; j < COLS; ++j) {
    if (j > 0) {
      busyWait(_char_delay);
    }

    lcd.setCursor(0, 1);
    for (i = 0; i < j; ++i) {
      lcd.print('-');
    }
    lcd.write(byte(0));
   }

  setSyncProvider(RTC.get);
  if (timeStatus() != timeSet) {
     lcd.print("Error: Invalid time status");
  }

  delay(1000);

  /* configure our buttons, according to:

            [analog A0]
                |
  5v --> [1kΩ] -+--> [270Ω] +-> [390Ω] +
                |           |          |
                v           v          v
             [ btn ]     [ btn ]    [ btn ]
                |           |          |
  gnd <---------+-----------+----------+
  */
  button_init(1000, 0);
  button_init(1000, 270);
  button_init(1000, 270 + 390);

  readAlarm(ALARM_2, &alarm2_def);

  // clear alarm flags
  RTC.alarm(ALARM_1);
  RTC.alarm(ALARM_2);

  RTC.squareWave(SQWAVE_NONE);

  current_time = now();

  RTC.setAlarm(ALM1_EVERY_SECOND, 0, 0, 0, 0); // every second
  //RTC.setAlarm(ALM2_MATCH_MINUTES, 0, 0, 0, 0); // every zero-th minute
  RTC.alarmInterrupt(ALARM_1, true);
  //RTC.alarmInterrupt(ALARM_2, true);

  pinMode(RTC_INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RTC_INT_PIN), alarmChange, FALLING);

  pinMode(FLOAT_SWITCH_INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FLOAT_SWITCH_INT_PIN), floatSwitchInterrupt, CHANGE);

  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW);
}

void pump_on() {
  if (digitalRead(FLOAT_SWITCH_INT_PIN) == FLOAT_SWITCH_OFF) {
    return;
  }

  pump.state = PUMP_ON;
  pump.state_start = millis();
  pump.count++;
  digitalWrite(PUMP_PIN, HIGH);

  /* configure watchdog to 1 second */
  watchdog_init(WDTO_1S);
}

void pump_off() {
  wdt_disable();

  digitalWrite(PUMP_PIN, LOW);
  pump.state = PUMP_OFF;
  pump.state_start = millis();
}

uint16_t get_pump_count() {
  return pump.count;
}

/*
  RTC Alarm ISR
 */
void alarmChange() {
  alarmCalled = 1;
}

/*
  Float switch ISR
 */
void floatSwitchInterrupt() {
  if (digitalRead(FLOAT_SWITCH_INT_PIN) == FLOAT_SWITCH_OFF && pump.state == PUMP_ON) {
    pump_off();
  }
}

void watchdog_init(uint8_t mask) {
  /* Clear the reset flag. */
  MCUSR &= ~(1 << WDRF);

  /* In order to change WDE or the prescaler, we need to
  * set WDCE (This will allow updates for 4 clock cycles).
  */
  WDTCSR |= (1<<WDCE) | (1<<WDE);

  /* set new watchdog timeout prescaler value */
  WDTCSR = mask;

  /* Enable the WD interrupt (note no reset). */
  WDTCSR |= _BV(WDIE);
}

/*
  Watchdog ISR
 */
ISR(WDT_vect) {
  if ((millis() - pump.state_start) >= WATER_RUNTIME) {
    pump_off();
  }
}

void loop() {
  if (alarmCalled) {
    alarmCalled = 0;
    if (RTC.alarm(ALARM_1)) {
      handle_alarm1();
    }

    if (RTC.alarm(ALARM_2) && is_alarm_active(ALARM_2)) {
      handle_alarm2();
    }
  }

  switch (get_active_frame()) {
    case FRAME_0:
      /* root frame */
      frame0();
    break;

    case FRAME_1:
      /* menu frame */
      frame1();
    break;

    case FRAME_2:
      /* menu option frame */
      frame2();
      break;
  }

  set_prev_frame(get_active_frame());

  if (get_next_frame() != -1) {
    set_active_frame(get_next_frame());
    set_next_frame(-1);
    set_last_draw();
  }
}
