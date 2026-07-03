#pragma once
#include <Arduino.h>

// Relais- und LED-Ansteuerung
namespace relay_led {

// Pins konfigurieren; Relais sicher AUS, kurzer LED-Selbsttest
void begin();

// Relais schalten (true = Ozongenerator an) + grüne LED
void setRelay(bool on);
bool relayOn();

// Blaue LED: Wasser fließt
void setFlowLed(bool on);

// Rote LED: Fehlerzustand
void setErrorLed(bool on);

} // namespace relay_led
