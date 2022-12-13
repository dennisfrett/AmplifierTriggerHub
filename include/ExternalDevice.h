#include <Arduino.h>

#define DEBOUNCE_TIME_MS 300

class ExternalDevice
{
public:
  ExternalDevice(int digitalPin,
                 void (*interruptHandler)(),
                 void (*powerStateChangedCallback)(int))
    : digitalPin(digitalPin)
    , powerStateChangedCallback(powerStateChangedCallback)
  {
    // Set pin mode + interrupt handler.
    pinMode(digitalPin, INPUT_PULLUP);
    attachInterrupt(
      digitalPinToInterrupt(digitalPin), interruptHandler, CHANGE);

    // Initialize starting state.
    powerState = digitalRead(digitalPin);
  }

  void Handle()
  {
    // Nothing changed, return.
    if (!potentialStateChange) {
      return;
    }

    const auto currentTime = millis();

    // If debounce time hasn't passed yet, return.
    if (currentTime - potentialStateChangeTime < DEBOUNCE_TIME_MS) {
      return;
    }

    // Reset trigger.
    potentialStateChange = false;

    // Check if state actually changed.
    const auto pinState = digitalRead(digitalPin);
    if (pinState == powerState) {

      // False positive
      return;
    }

    // State changed.
    powerState = pinState;

    // Invoke callback.
    powerStateChangedCallback(powerState);
  }

  void Trigger()
  {
    potentialStateChangeTime = millis();
    potentialStateChange = true;
  }

  bool CanGoToSleep() const { return !potentialStateChange; }

private:
  // Power state of the external device (HIGH == ON, LOW == OFF).
  int powerState = LOW;

  // State needed for debouncing.
  bool potentialStateChange = false;
  long potentialStateChangeTime = 0;

  const int digitalPin;

  void (*powerStateChangedCallback)(int);
};