#include <Arduino.h>

#define IRSND_IR_FREQUENCY 38000

#define IRSND_OUTPUT_PIN 5

// Needs to be included before `irsnd.hpp`.
#include <irsndSelectMain15Protocols.h>

#include <irsnd.hpp>

#include <ExternalDevice.h>
#include <LowPower.h>
#include <State.h>
#include <nectransmitter.h>

#define NAD_IR_PIN 6
#define RELAY_PIN 4
#define MAIN_AMP_TRIGGER_PIN 2
#define SHIELD_TRIGGER_PIN 3

NECTransmitter necTransmitter(NAD_IR_PIN);

//#define DEBUG

void
SwitchOffAmps()
{
  digitalWrite(RELAY_PIN, LOW);
  necTransmitter.SendExtendedNEC(0x877C, 0x80);

#ifdef DEBUG
  // Turn off internal LED.
  digitalWrite(LED_BUILTIN, LOW);
#endif
}

void
SwitchOnAmps()
{
  digitalWrite(RELAY_PIN, HIGH);
  necTransmitter.SendExtendedNEC(0x877C, 0x80);

#ifdef DEBUG
  // Turn on internal LED.
  digitalWrite(LED_BUILTIN, HIGH);
#endif
}

void
denonAVRTriggered();

void
shieldTriggered();

void
denonPowerStateChanged(int newState)
{
  if (newState == HIGH) {
    SwitchOnAmps();
  } else {
    SwitchOffAmps();
  }
}

void
shieldPowerStateChanged(int)
{
  IRMP_DATA irsnd_data = { IRMP_NEC_PROTOCOL, 0xFF00, 0x40 };
  irsnd_send_data(&irsnd_data, true);
}

ExternalDevice denonAVR(MAIN_AMP_TRIGGER_PIN,
                        &denonAVRTriggered,
                        &denonPowerStateChanged);

ExternalDevice shield(SHIELD_TRIGGER_PIN,
                      &shieldTriggered,
                      &shieldPowerStateChanged);

void
denonAVRTriggered()
{
  denonAVR.Trigger();
}

void
shieldTriggered()
{
  shield.Trigger();
}

void
setup()
{
#ifdef DEBUG
  Serial.begin(115200);
#endif

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  irsnd_init();

#ifdef DEBUG
  Serial.println("Started.");
  delay(500);
#endif

  // Go to sleep.
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
}

void
loop()
{
  denonAVR.Handle();
  shield.Handle();

  if (denonAVR.CanGoToSleep() && shield.CanGoToSleep()) {
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  }
}
