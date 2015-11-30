#ifndef TOGGLE_SWITCH_H
#define TOGGLE_SWITCH_H

typedef struct {
  uint8_t pin;
  uint8_t state;
  uint8_t prev_state;
} toggle_switch_t;

void toggle_switch_init(toggle_switch_t* button, uint8_t pin) {
  button->pin = pin;
  button->state = digitalRead(button->pin);
  button->prev_state = !button->state;
}

void toggle_switch_read_state(toggle_switch_t* button) {
  button->prev_state = button->state;
  button->state = digitalRead(button->pin);
}

uint8_t toggle_switch_on(toggle_switch_t* button) {
  if (button->state == LOW) {
    return 1;
  }

  return 0;
}

uint8_t toggle_switch_off(toggle_switch_t* button) {
  if (button->state == HIGH) {
    return 1;
  }

  return 0;
}

uint8_t toggle_switch_was_on(toggle_switch_t* button) {
  if (button->prev_state == LOW) {
    return 1;
  }

  return 0;
}

uint8_t toggle_switch_was_off(toggle_switch_t* button) {
  if (button->prev_state == HIGH) {
    return 1;
  }

  return 0;
}

#endif
