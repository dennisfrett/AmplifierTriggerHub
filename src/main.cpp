#include <Arduino.h>

#include <LowPower.h>
#include <nectransmitter.h>

#define IR_PIN 3
#define RELAY_PIN 4
#define TRIGGER_PIN 2

NECTransmitter necTransmitter(IR_PIN);

//#define DEBUG

volatile int triggerPinState;

void wakeUp() {
#ifdef DEBUG
  Serial.println("Pin changed.");
#endif

  triggerPinState = digitalRead(TRIGGER_PIN);
}

void setup() {
#ifdef DEBUG
  Serial.begin(115200);
#endif

  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(TRIGGER_PIN), wakeUp, CHANGE);

#ifdef DEBUG
  Serial.println("Started.");
#endif

  delay(500);
}

void SwitchOffAmps() {
  digitalWrite(RELAY_PIN, LOW);
  necTransmitter.SendExtendedNEC(0x877C, 0x80);

#ifdef DEBUG
  // Turn off internal LED.
  digitalWrite(LED_BUILTIN, LOW);
#endif
}

void SwitchOnAmps() {
  digitalWrite(RELAY_PIN, HIGH);
  necTransmitter.SendExtendedNEC(0x877C, 0x80);

#ifdef DEBUG
  // Turn on internal LED.
  digitalWrite(LED_BUILTIN, HIGH);
#endif
}

void WaitForMainAmpOn() {
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  if (triggerPinState == LOW) {
    WaitForMainAmpOn();
  }
}

void WaitForMainAmpOff() {
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  if (triggerPinState == HIGH) {
    WaitForMainAmpOff();
  }
}

void loop() {
#ifdef DEBUG
  Serial.println("Main amp off. Waiting for trigger...");
#endif

  WaitForMainAmpOn();
  SwitchOnAmps();

#ifdef DEBUG
  Serial.println("Main amp on. Waiting for turn off.");
#endif

  WaitForMainAmpOff();
  SwitchOffAmps();
}
