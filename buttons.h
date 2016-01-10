#ifndef TIMER_ANALOG_BTNS
#define TIMER_ANALOG_BTNS

#include "config.h"

#define MAX_BUTTONS 5
#define BUTTON_THRESHOLD 50

uint16_t btns[MAX_BUTTONS];
uint8_t btns_count = 0;
uint16_t btns_max = 0;

volatile uint8_t debounce_start = 0;
uint8_t prev_reading = 0;
uint8_t readings_num = 0;
unsigned long last_activity = 0;

void button_init(uint16_t r1, uint16_t rl) {
  /* when button is pressed, the amounts of volts that will be
    measured on the PIN equals: */
  float analog_value = (1024.0 * rl) / (r1 + rl);

  /* ... which analogRead() will return as: */
  uint16_t val = (uint16_t)analog_value;

  if (val < BUTTON_THRESHOLD) {
    val = BUTTON_THRESHOLD;
  }

  /* round it down */
  btns[btns_count++] = val;

  if (val > btns_max) {
    btns_max = val;
  }
}

unsigned long get_last_button_activity() {
  return last_activity;
}

int8_t button_find(uint16_t btn) {
  uint8_t i;
  for (i = 0; i < btns_count; ++i) {
   if ((btn >= (btns[i] - BUTTON_THRESHOLD)) && (btn <= (btns[i] + BUTTON_THRESHOLD))) {
    return i;
   }
  }

  return -1;
}

void button_start_bounce() {
  // we are not bouncing anything, so we are listening for when the
   // pin goes from HIGH to a lower voltage
   uint16_t val = analogRead(BUTTONS_PIN);

   /* simple check to see if we get a voltage that may be a button press */
   if ((btns_max + BUTTON_THRESHOLD) > val) {
    // start bouncing
     debounce_start = 1;
   }
}

int8_t button_end_bounce() {
  // a button is currently being pressed, so we are listening
  // for when the voltages goes from low to HIGH
  uint16_t val = analogRead(BUTTONS_PIN);

  /* simple check to see if we get a voltage higher than any of our buttons produce */
  if (val > (btns_max + BUTTON_THRESHOLD)) {
    debounce_start = 0;

    if (readings_num > 0) {
      last_activity = millis();
      int8_t btn = prev_reading;

      readings_num = 0;
      prev_reading = 0;

      return btn;
    }

    return -1;
  }

  if (readings_num > 10) {
   // we have enough, let's just wait until the button is released
    return -1;
  }

  int8_t btn = button_find(val);
  if (btn == -1) {
    readings_num = 0;
  } else {
   if (readings_num == 0) {
    prev_reading = btn;
    readings_num++;
   } else if (btn == prev_reading) {
     // same as before, OK
     readings_num++;
   }
  }

  return -1;
}

int8_t button_handle() {
  if (debounce_start == 0) {
   button_start_bounce();
   return -1;
  }

  return button_end_bounce();
}

#endif
