#define RELAY_PIN 4
#define OFF_SWITCH 3

#define OFF_SWITCH_LED_PIN 5
#define PUMP_LED_PIN 6
#define WAIT_LED_PIN 7

/* watering schedule */
#define TIME_BETWEEN_WATERING 10000
#define WATER_TIME 5000

volatile int previousButtonState = LOW;
volatile int currentButtonState = HIGH;

volatile int schedulerState = LOW;
volatile int isSchedulerStarted = LOW;

void setup() {
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(OFF_SWITCH_LED_PIN, OUTPUT);
  pinMode(PUMP_LED_PIN, OUTPUT);
  pinMode(WAIT_LED_PIN, OUTPUT);

  /* INPUT_PULLUP */
  pinMode(OFF_SWITCH, INPUT);
  digitalWrite(OFF_SWITCH, HIGH);

  attachInterrupt(digitalPinToInterrupt(OFF_SWITCH), scheduler, CHANGE);

  currentButtonState = digitalRead(OFF_SWITCH);
  previousButtonState = !currentButtonState;
  schedulerState = !currentButtonState;

  if (schedulerState == HIGH && isSchedulerStarted == LOW) {
    startSchedule();
  }

  delay(1000);
}

void scheduler() {
  currentButtonState = digitalRead(OFF_SWITCH);
  previousButtonState = !currentButtonState;
  schedulerState = !currentButtonState;

  if (previousButtonState == HIGH && currentButtonState == LOW) {
    startSchedule();
  } else if (previousButtonState == LOW && currentButtonState == HIGH) {
    stopSchedule();
  }
}

void startSchedule() {
  isSchedulerStarted = HIGH;
  turnLedOn(OFF_SWITCH_LED_PIN);
  Serial.println("Starting schedule");
}

void stopSchedule() {
  isSchedulerStarted = LOW;
  turnLedOff(OFF_SWITCH_LED_PIN);
 Serial.println("Stopping schedule.");
}

void loop() {
  if (isSchedulerStarted == HIGH) {
    Serial.print("[");
    Serial.print(millis());
    Serial.print("] ");
    Serial.println("Starting pump");

    turnPumpOn();
    turnLedOn(PUMP_LED_PIN);

    Serial.print("[");
    Serial.print(millis());
    Serial.print("] ");
    Serial.print("Waiting ");
    Serial.print(WATER_TIME);
    Serial.println(" ms or until button is switched OFF");
    busyWaitOrCondition(WATER_TIME, &isSchedulerStarted, LOW);

    turnPumpOff();
    turnLedOff(PUMP_LED_PIN);

    Serial.print("[");
    Serial.print(millis());
    Serial.print("] ");
    Serial.println("Pumped stopped");

    if (isSchedulerStarted == HIGH) {
      Serial.print("[");
      Serial.print(millis());
      Serial.print("] ");
      Serial.print("Waiting ");
      Serial.print(TIME_BETWEEN_WATERING);
      Serial.println(" ms or until button is switched ON");

      turnLedOn(WAIT_LED_PIN);
      busyWaitOrCondition(TIME_BETWEEN_WATERING, &isSchedulerStarted, LOW);
      turnLedOff(WAIT_LED_PIN);

      Serial.print("[");
      Serial.print(millis());
      Serial.print("] ");
      Serial.println("Done waiting");
    }
  } else {
    Serial.println("Waiting for button to be switched ON");
    waitForCondition(&isSchedulerStarted, HIGH);
  }
}

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

void waitForCondition(volatile int* val, int expected) {
 while (*val != expected) {
  delay(10);
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

void turnPumpOn() {
 if (digitalRead(RELAY_PIN) == LOW) {
   Serial.println("Running water pump");
   digitalWrite(RELAY_PIN, HIGH);
 }
}

void turnPumpOff() {
 if (digitalRead(RELAY_PIN) == HIGH) {
   Serial.println("Turning off pump");
   digitalWrite(RELAY_PIN, LOW);
 }
}
