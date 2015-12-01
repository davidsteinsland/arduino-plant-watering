#include "pump.h"
#include "toggle_switch.h"
#include "scheduler.h"
#include "debounce.h"
#include "busy_wait.h"

#define RELAY_PIN 4
#define OFF_SWITCH 3
#define FLOAT_SWITCH_PIN 2

#define OFF_SWITCH_LED_PIN 5
#define PUMP_LED_PIN 6
#define WAIT_LED_PIN 7

/* watering schedule */
#define TIME_BETWEEN_WATERING 10000
#define WATER_TIME 5000

#define DEBUG 1

volatile schedule_t scheduler;
volatile toggle_switch_t button;
volatile toggle_switch_t float_switch;
debounce_t button_debounce;
debounce_t float_switch_debounce;
pump_t pump;

void setup() {
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(OFF_SWITCH_LED_PIN, OUTPUT);
  pinMode(PUMP_LED_PIN, OUTPUT);
  pinMode(WAIT_LED_PIN, OUTPUT);

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

  pump_init(&pump, RELAY_PIN);
  water_schedule_init((schedule_t*)&scheduler);

  /* if toggle switch is in ON position at startup ... */
  if (toggle_switch_on(&button) && toggle_switch_on(&float_switch)) {
    startSchedule();
  }

  delay(1000);
}

void handleToggleButton() {
#ifdef DEBUG
  Serial.print("[");
  Serial.print(micros());
  Serial.print("] ");
  Serial.println("handleToggleButton");
#endif
  if (debounce_toggle_switch(&button, &button_debounce, 10)) {
#ifdef DEBUG
    Serial.print("[");
    Serial.print(micros());
    Serial.print("] ");
    Serial.println("handleToggleButton debounced ok");
#endif
    toggle_switch_read_state(&button);

    if (toggle_switch_on(&button)) {
#ifdef DEBUG
    Serial.print("[");
    Serial.print(millis());
    Serial.print("] ");
    Serial.println("handleToggleButton is ON");
#endif
      if (toggle_switch_on(&float_switch)) {
        startSchedule();
      }
    } else {
#ifdef DEBUG
    Serial.print("[");
    Serial.print(millis());
    Serial.print("] ");
    Serial.println("handleToggleButton is OFF");
#endif
      stopSchedule();
    }
  } else {
#ifdef DEBUG
    Serial.print("[");
    Serial.print(micros());
    Serial.print("] ");
    Serial.println("handleToggleButton debounced NOT ok");
#endif
  }
}

void handleFloatSwitch() {

#ifdef DEBUG
  Serial.print("[");
  Serial.print(micros());
  Serial.print("] ");
  Serial.println("handleFloatSwitch");
#endif

  if (debounce_toggle_switch(&float_switch, &float_switch_debounce, 10)) {
#ifdef DEBUG
    Serial.print("[");
    Serial.print(micros());
    Serial.print("] ");
    Serial.println("handleFloatSwitch debounced ok");
#endif
    toggle_switch_read_state(&float_switch);

    if (toggle_switch_on(&float_switch)) {
#ifdef DEBUG
      Serial.print("[");
      Serial.print(millis());
      Serial.print("] ");
      Serial.println("handleFloatSwitch is ON");
#endif
      if (toggle_switch_on(&button)) {
        startSchedule();
      }
    } else {
#ifdef DEBUG
      Serial.print("[");
      Serial.print(millis());
      Serial.print("] ");
      Serial.println("handleFloatSwitch is OFF");
#endif
      stopSchedule();
    }
  } else {
#ifdef DEBUG
    Serial.print("[");
    Serial.print(micros());
    Serial.print("] ");
    Serial.println("handleFloatSwitch debounced NOT ok");
#endif
  }
}

void startSchedule() {
  water_schedule_start(&scheduler);
  turnLedOn(OFF_SWITCH_LED_PIN);
  Serial.println("Starting schedule");
}

void stopSchedule() {
  water_schedule_stop(&scheduler);
  turnLedOff(OFF_SWITCH_LED_PIN);
  Serial.println("Stopping schedule.");
}

void loop() {

  if (!water_schedule_active(&scheduler)) {
#ifdef DEBUG
    Serial.print("[");
    Serial.print(millis());
    Serial.print("] ");
    Serial.println("Waiting for scheduler to become active");
#endif
    waitForCondition(&(scheduler.state), HIGH);
  }

#ifdef DEBUG
  Serial.print("[");
  Serial.print(millis());
  Serial.print("] ");
  Serial.println("Starting pump");
#endif

  pump_start(&pump);
  turnLedOn(PUMP_LED_PIN);

#ifdef DEBUG
  Serial.print("[");
  Serial.print(millis());
  Serial.print("] ");
  Serial.print("Waiting ");
  Serial.print(WATER_TIME);
  Serial.println(" ms or until button is switched OFF");
#endif

  busyWaitOrCondition(WATER_TIME, &(scheduler.state), LOW);

  pump_stop(&pump);
  turnLedOff(PUMP_LED_PIN);

#ifdef DEBUG
  Serial.print("[");
  Serial.print(millis());
  Serial.print("] ");
  Serial.println("Pumped stopped");
#endif

  /* check state once again, as it may have been turned off by the switch */
  if (!water_schedule_active(&scheduler)) {
    return;
  }

#ifdef DEBUG
  Serial.print("[");
  Serial.print(millis());
  Serial.print("] ");
  Serial.print("Waiting ");
  Serial.print(TIME_BETWEEN_WATERING);
  Serial.println(" ms or until button is switched ON");
#endif

  turnLedOn(WAIT_LED_PIN);
  busyWaitOrCondition(TIME_BETWEEN_WATERING, &(scheduler.state), LOW);
  turnLedOff(WAIT_LED_PIN);

#ifdef DEBUG
  Serial.print("[");
  Serial.print(millis());
  Serial.print("] ");
  Serial.println("Done waiting");
#endif
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
