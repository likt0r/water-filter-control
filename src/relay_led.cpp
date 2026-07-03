#include "relay_led.h"
#include "config.h"

namespace relay_led {

namespace {
bool relayState = false;

void writeRelay(bool on) {
  // Low-aktives Modul: LOW = Relais zieht an
  bool level = RELAY_ACTIVE_LOW ? !on : on;
  digitalWrite(PIN_RELAY, level ? HIGH : LOW);
}
} // namespace

void begin() {
  // Relais-Pin zuerst in den Aus-Pegel bringen, DANN als Ausgang schalten,
  // damit das Relais beim Boot nicht kurz anzieht.
  writeRelay(false);
  pinMode(PIN_RELAY, OUTPUT);
  writeRelay(false);

  pinMode(PIN_LED_BLUE, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);

  // Kurzer Selbsttest: alle LEDs 300 ms an
  digitalWrite(PIN_LED_BLUE, HIGH);
  digitalWrite(PIN_LED_GREEN, HIGH);
  digitalWrite(PIN_LED_RED, HIGH);
  delay(300);
  digitalWrite(PIN_LED_BLUE, LOW);
  digitalWrite(PIN_LED_GREEN, LOW);
  digitalWrite(PIN_LED_RED, LOW);
}

void setRelay(bool on) {
  if (on == relayState) return;
  relayState = on;
  writeRelay(on);
  digitalWrite(PIN_LED_GREEN, on ? HIGH : LOW);
}

bool relayOn() { return relayState; }

void setFlowLed(bool on) { digitalWrite(PIN_LED_BLUE, on ? HIGH : LOW); }

void setErrorLed(bool on) { digitalWrite(PIN_LED_RED, on ? HIGH : LOW); }

} // namespace relay_led
