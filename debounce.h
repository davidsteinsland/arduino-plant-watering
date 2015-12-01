#ifndef DEBOUNCE_H
#define DEBOUNCE_H

#include <Arduino.h>

typedef struct {
  unsigned long last_micros;
} debounce_t;

void debounce_init(debounce_t* debounce) {
  debounce->last_micros = 0;
}

int debounce_read(debounce_t* debounce, unsigned long ms) {
  if ((millis() - debounce->last_micros) > ms) {
    return 1;
  }
  return 0;
}

int debounce_toggle_switch(volatile toggle_switch_t* button, debounce_t* debounce, unsigned long ms) {
  int val = digitalRead(button->pin);

  if (val != button->prev_state) {
    debounce->last_micros = millis();
  }

  return debounce_read(debounce, ms);
}

#endif
