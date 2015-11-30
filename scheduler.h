#ifndef SCHEDULER_H
#define SCHEDULER_H

typedef struct {
  uint8_t state;
} schedule_t;

void water_schedule_init(schedule_t* schedule) {
  schedule->state = LOW;
}

void water_schedule_start(schedule_t* schedule) {
  schedule->state = HIGH;
}

void water_schedule_stop(schedule_t* schedule) {
  schedule->state = LOW;
}

uint8_t water_schedule_active(schedule_t* schedule) {
  if (schedule->state == HIGH) {
    return 1;
  }

  return 0;
}

#endif
