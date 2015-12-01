#ifndef DEBOUNCE_H
#define DEBOUNCE_H

#include <Arduino.h>

typedef struct {
  unsigned long last_micros;
} debounce_t;

void debounce_init(debounce_t* debounce) {
  debounce->last_micros = 0;
}

int debounce_read(debounce_t* debounce, unsigned long us) {
  if ((micros() - debounce->last_micros) > us) {
    return 1;
  }
  return 0;
}

int debounce_toggle_switch(volatile toggle_switch_t* button, debounce_t* debounce, unsigned long us) {
  int val = digitalRead(button->pin);

  if (val != button->prev_state) {
    debounce->last_micros = micros();
  }

  return debounce_read(debounce, us);
}

#endif
