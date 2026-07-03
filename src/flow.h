#pragma once
#include <Arduino.h>

// Impulszählung (ISR) + Durchflussrate + Volumen-Akkumulation
namespace flow {

// Interrupt einrichten; totalLiters mit gespeichertem Wert initialisieren
void begin(double initialTotalLiters);

// Regelmäßig aufrufen (alle CONTROL_INTERVAL_MS). Verarbeitet neue Impulse.
void update(uint32_t nowMs);

// true, solange Impulse innerhalb FLOW_TIMEOUT_MS eintreffen
bool isFlowing();

// Aktuelle Durchflussrate in L/min (geglättet)
float flowRateLpm();

// Aufakkumulierte Gesamtmenge in Litern (rücksetzbar)
double totalLiters();

// Zähler auf 0 setzen (Web-Reset)
void resetTotal();

// Beginn des aktuellen Flusses (ms-Timestamp), 0 wenn kein Fluss
uint32_t flowStartMs();

} // namespace flow
