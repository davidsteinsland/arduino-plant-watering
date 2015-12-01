#include "toggle_switch.h"
#include "debounce.h"
#include "busy_wait.h"

#define BIT_IS_SET(x, n) ((x) & (1<<(n)))
#define BIT_SET(x, n)    ((x) |= (1<<(n)))
#define BIT_UNSET(x, n)  ((x) &= ~(1<<(n)))
#define BIT_TOGGLE(x, n) ((x) ^= (1<<(n)))

#define RELAY_BIT_POS 0
#define TOGGLE_SWITCH_BIT_POS 1
#define FLOAT_SWITCH_BIT_POS 2

/* Input/Output pins */
#define RELAY_PIN 4
#define OFF_SWITCH 3
#define FLOAT_SWITCH_PIN 2

/* LEDs */
#define OFF_SWITCH_LED_PIN 5
#define FLOAT_SWITCH_LED_PIN 6
#define PUMP_LED_PIN 7

/* watering schedule */
#define TIME_BETWEEN_WATERING 10000
#define WATER_TIME 5000

#define DEBUG 1

volatile uint8_t state_mask;

volatile toggle_switch_t button;
volatile toggle_switch_t float_switch;
debounce_t button_debounce;
debounce_t float_switch_debounce;

unsigned long pump_state_msec;

void setup() {
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(OFF_SWITCH_LED_PIN, OUTPUT);
  pinMode(PUMP_LED_PIN, OUTPUT);
  pinMode(FLOAT_SWITCH_LED_PIN, OUTPUT);

  /* pullup the toggle switch to 10k internal resistor */
  pinMode(OFF_SWITCH, INPUT_PULLUP);
  /* pullup the float switch to 10k internal resistor */
  pinMode(FLOAT_SWITCH_PIN, INPUT_PULLUP);

  /* because of INPUT_PULLUP, we need to take action when circuit goes from HIGH -> LOW or LOW -> HIGH */
  attachInterrupt(digitalPinToInterrupt(OFF_SWITCH), handleToggleButton, CHANGE);
  attachInterrupt(digitalPinToInterrupt(FLOAT_SWITCH_PIN), handleFloatSwitch, CHANGE);

  debounce_init(&button_debounce);
  debounce_init(&float_switch_debounce);
  toggle_switch_init((toggle_switch_t*)&button, OFF_SWITCH, 1);
  toggle_switch_init((toggle_switch_t*)&float_switch, FLOAT_SWITCH_PIN);

  pump_state_msec = 0;

  /* if toggle switch is in ON position at startup ... */
  if (toggle_switch_on(&button)) {
    BIT_SET(state_mask, TOGGLE_SWITCH_BIT_POS);
  }

  if (toggle_switch_on(&float_switch)) {
    BIT_SET(state_mask, FLOAT_SWITCH_BIT_POS);
  }

  delay(1000);
}

void handleToggleButton() {
  if (debounce_toggle_switch(&button, &button_debounce, 2)) {
    toggle_switch_read_state(&button);

    if (toggle_switch_on(&button)) {
      BIT_SET(state_mask, TOGGLE_SWITCH_BIT_POS);
    } else {
      BIT_UNSET(state_mask, TOGGLE_SWITCH_BIT_POS);
    }
  }
}

void handleFloatSwitch() {
  if (debounce_toggle_switch(&float_switch, &float_switch_debounce, 2)) {
    toggle_switch_read_state(&float_switch);

    if (toggle_switch_on(&float_switch)) {
      BIT_SET(state_mask, FLOAT_SWITCH_BIT_POS);
    } else {
      BIT_UNSET(state_mask, FLOAT_SWITCH_BIT_POS);
    }
  }
}

void loop() {
  writeStateMaskToLeds();

  if (!BIT_IS_SET(state_mask, TOGGLE_SWITCH_BIT_POS) || !BIT_IS_SET(state_mask, FLOAT_SWITCH_BIT_POS)) {
    if (BIT_IS_SET(state_mask, RELAY_BIT_POS)) {
      digitalWrite(RELAY_PIN, LOW);
      pump_state_msec = 0;
      BIT_UNSET(state_mask, RELAY_BIT_POS);
    }
    return;
  }

  if (!BIT_IS_SET(state_mask, RELAY_BIT_POS)) {
    /* scheduler is active but pump is not running */

    if (pump_state_msec == 0 || ((millis() - pump_state_msec) > TIME_BETWEEN_WATERING)) {
      digitalWrite(RELAY_PIN, HIGH);
      pump_state_msec = millis();
      BIT_SET(state_mask, RELAY_BIT_POS);
    }
    return;
  }

  /* scheduler is active and pump is running */

  if ((millis() - pump_state_msec) > WATER_TIME) {
    digitalWrite(RELAY_PIN, LOW);
    pump_state_msec = millis();
    BIT_UNSET(state_mask, RELAY_BIT_POS);
  }
}

void turnLedOn(int pin) {
 if (digitalRead(pin) == LOW) {
  digitalWrite(pin, HIGH);
 }
}

void turnLedOff(int pin) {
 if (digitalRead(pin) == HIGH) {
  digitalWrite(pin, LOW);
 }
}

void writeStateMaskToLeds() {
  if (BIT_IS_SET(state_mask, TOGGLE_SWITCH_BIT_POS)) {
    turnLedOn(OFF_SWITCH_LED_PIN);
  } else {
    turnLedOff(OFF_SWITCH_LED_PIN);
  }

  if (BIT_IS_SET(state_mask, FLOAT_SWITCH_BIT_POS)) {
    turnLedOn(FLOAT_SWITCH_LED_PIN);
  } else {
    turnLedOff(FLOAT_SWITCH_LED_PIN);
  }

  if (BIT_IS_SET(state_mask, RELAY_BIT_POS)) {
    turnLedOn(PUMP_LED_PIN);
  } else {
    turnLedOff(PUMP_LED_PIN);
  }
}
