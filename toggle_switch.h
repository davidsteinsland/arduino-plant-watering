#ifndef TOGGLE_SWITCH_H
#define TOGGLE_SWITCH_H

#include <Arduino.h>

typedef struct {
  uint8_t pin;
  uint8_t state;
  uint8_t prev_state;
  uint8_t inverted;
} toggle_switch_t;

void toggle_switch_init(toggle_switch_t* button, uint8_t pin, uint8_t inverted) {
  button->pin = pin;
  button->inverted = inverted;
  button->state = digitalRead(button->pin);
  button->prev_state = !button->state;
}

void toggle_switch_init(toggle_switch_t* button, uint8_t pin) {
  toggle_switch_init(button, pin, 0);
}

void toggle_switch_read_state(volatile toggle_switch_t* button) {
  button->prev_state = button->state;
  button->state = digitalRead(button->pin);
}

uint8_t toggle_switch_on(volatile toggle_switch_t* button) {
  if (button->inverted) {
    return button->state == LOW;
  }
  
  return button->state == HIGH;
}

uint8_t toggle_switch_off(volatile toggle_switch_t* button) {
  if (button->inverted) {
    return button->state == HIGH;
  }
  
  return button->state == LOW;
}

uint8_t toggle_switch_was_on(volatile toggle_switch_t* button) {
  if (button->inverted) {
    return button->prev_state == LOW;
  }
  
  return button->prev_state == HIGH;
}

uint8_t toggle_switch_was_off(volatile toggle_switch_t* button) {
  if (button->inverted) {
    return button->prev_state == HIGH;
  }
  
  return button->prev_state == LOW;
}

#endif
