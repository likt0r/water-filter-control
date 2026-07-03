#pragma once
#include <Arduino.h>

// Stromausfallsichere Speicherung des Zählerstands im NVS (Flash)
namespace persistence {

// NVS öffnen; gibt gespeicherten Zählerstand in Litern zurück (0 beim ersten Start)
double begin();

// Zählerstand speichern; true bei Erfolg
bool saveTotal(double liters);

// true, wenn der letzte Schreib-/Lesevorgang fehlschlug (für rote LED)
bool hasError();

} // namespace persistence
