#pragma once
#include <Arduino.h>

// Relais- und LED-Ansteuerung
namespace relay_led {

// Pins konfigurieren; Relais sicher AUS, kurzer LED-Selbsttest
void begin();

// Relais schalten (true = Ozongenerator an) + grüne LED
void setRelay(bool on);
bool relayOn();

// Blaue LED: Helligkeit spiegelt den Durchfluss (per PWM).
// flowing=false -> aus; sonst Helligkeit proportional zu lpm (mit Floor).
void setFlowLevel(bool flowing, float lpm);

// Rote LED: Fehlerzustand
void setErrorLed(bool on);

} // namespace relay_led
