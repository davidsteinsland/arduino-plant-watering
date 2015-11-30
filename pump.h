#ifndef PUMP_H
#define PUMP_H

typedef struct {
  uint8_t pin;
} pump_t;

void pump_init(pump_t* pump, uint8_t pin) {
  pump->pin = pin;
}

void pump_start(pump_t* pump) {
  digitalWrite(pump->pin, HIGH);
}

void pump_stop(pump_t* pump) {
  digitalWrite(pump->pin, LOW);
}

#endif
