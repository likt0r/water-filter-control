#pragma once
#include <Arduino.h>

// SoftAP + Webseite (Statusanzeige, Zähler-Reset)
namespace webserver {

// SoftAP starten und HTTP-Routen registrieren
void begin();

// Im Loop aufrufen (bedient anstehende HTTP-Anfragen)
void handle();

} // namespace webserver
