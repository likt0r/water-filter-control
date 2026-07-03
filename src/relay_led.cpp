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

  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);

  // Blaue LED an PWM-Kanal (LEDC) fuer dimmbare Helligkeit
  ledcSetup(LEDC_CHANNEL_BLUE, LEDC_FREQ_HZ, LEDC_RES_BITS);
  ledcAttachPin(PIN_LED_BLUE, LEDC_CHANNEL_BLUE);

  // Selbsttest beim Start: alle LEDs 2 s an (Funktionskontrolle)
  ledcWrite(LEDC_CHANNEL_BLUE, FLOW_LED_MAX_DUTY);
  digitalWrite(PIN_LED_GREEN, HIGH);
  digitalWrite(PIN_LED_RED, HIGH);
  delay(LED_SELFTEST_MS);
  ledcWrite(LEDC_CHANNEL_BLUE, 0);
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

void setFlowLevel(bool flowing, float lpm) {
  if (!flowing) {
    ledcWrite(LEDC_CHANNEL_BLUE, 0);
    return;
  }
  float frac = lpm / FLOW_LED_MAX_LPM;
  if (frac < 0.0f) frac = 0.0f;
  if (frac > 1.0f) frac = 1.0f;
  frac = frac * frac; // Gamma ~2: gefuehlt lineare Helligkeit, betont hohen Fluss
  int duty = FLOW_LED_MIN_DUTY + (int)(frac * (FLOW_LED_MAX_DUTY - FLOW_LED_MIN_DUTY));
  ledcWrite(LEDC_CHANNEL_BLUE, duty);
}

void setErrorLed(bool on) { digitalWrite(PIN_LED_RED, on ? HIGH : LOW); }

} // namespace relay_led
