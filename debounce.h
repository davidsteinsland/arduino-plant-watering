#ifndef DEBOUNCE_H
#define DEBOUNCE_H

typedef struct {
  unsigned long last_micros;
} debounce_t;

void debounce_init(debounce_t* debounce, unsigned long delay) {
  debounce->last_micros = 0;
}

int debounce_toggle_switch(toggle_switch_t* button, debounce_t* debounce) {
  int val = digitalRead(button->pin);

  if (val != button->prev_state) {
    debounce->last_micros = millis();
  }

  return debounce_read(debounce, delay);
}

int debounce_read(debounce_t* debounce, unsigned long delay) {
  if ((millis() - debounce->last_micros) > delay) {
    return 1;
  }
  return 0;
}

#endif
