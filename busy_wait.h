#ifndef BUSY_WAIT_H
#define BUSY_WAIT_H

#include <Arduino.h>

/*
  busyWait(5000) will wait for 5 seconds
*/
void busyWait(unsigned long ms) {
 unsigned long start = millis();

 while ((millis() - start) < ms) {
  /* sleep for 10 ms */
  delay(10);
 }
}

/*
  busyWait(5000, &buttonState, HIGH); will wait for 5 seconds or until buttonState == HIGH
*/
void busyWaitOrCondition(unsigned long ms, volatile int* val, int expected) {
  unsigned long start = millis();

  while ((millis() - start) < ms && *val != expected) {
    delay(10);
  }
}

void busyWaitOrCondition(unsigned long ms, volatile uint8_t* val, uint8_t expected) {
  unsigned long start = millis();

  while ((millis() - start) < ms && *val != expected) {
    delay(10);
  }
}

void waitForCondition(volatile int* val, int expected) {
 while (*val != expected) {
  delay(10);
 }
}

void waitForCondition(volatile uint8_t* val, uint8_t expected) {
 while (*val != expected) {
  delay(10);
 }
}

#endif
