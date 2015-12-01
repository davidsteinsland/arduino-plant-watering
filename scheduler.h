#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <Arduino.h>

typedef struct {
  uint8_t state;
} schedule_t;

void water_schedule_init(schedule_t* schedule) {
  schedule->state = LOW;
}

void water_schedule_start(volatile schedule_t* schedule) {
  schedule->state = HIGH;
}

void water_schedule_stop(volatile schedule_t* schedule) {
  schedule->state = LOW;
}

uint8_t water_schedule_active(volatile schedule_t* schedule) {
  if (schedule->state == HIGH) {
    return 1;
  }

  return 0;
}

#endif
