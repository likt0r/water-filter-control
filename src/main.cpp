// Wasserfilter-Steuerung: Ozongenerator folgt dem Wasserfluss.
// Board: AZ-Delivery ESP32 DevKit C V4 (WROOM-32)
#include <Arduino.h>
#include "config.h"
#include "flow.h"
#include "persistence.h"
#include "relay_led.h"
#include "webserver.h"

namespace {
uint32_t lastControlMs = 0;
uint32_t lastSaveMs = 0;
uint32_t lastLogMs = 0;
} // namespace

void setup() {
  relay_led::begin(); // zuerst: Relais sicher AUS
  Serial.begin(115200);
  Serial.println("\nWasserfilter-Steuerung startet...");

  double savedLiters = persistence::begin();
  Serial.printf("Gespeicherter Zaehlerstand: %.3f L\n", savedLiters);

  flow::begin(savedLiters);
  webserver::begin();
}

void loop() {
  webserver::handle();

  uint32_t now = millis();
  if (now - lastControlMs < CONTROL_INTERVAL_MS) return;
  lastControlMs = now;

  flow::update(now);
  bool flowing = flow::isFlowing();

  // Blau = Durchfluss-Helligkeit (wenig=gedimmt, viel=hell); Gruen folgt Relais
  relay_led::setFlowLevel(flowing, flow::flowRateLpm());
  relay_led::setRelay(flowing);

  // Persistenz: beim Fluss-Ende sofort, waehrend des Flusses periodisch
  static bool wasFlowing = false;
  bool saveNow = false;
  if (wasFlowing && !flowing) saveNow = true;                      // Fluss endete
  if (flowing && (now - lastSaveMs) >= SAVE_INTERVAL_MS) saveNow = true;
  if (saveNow) {
    persistence::saveTotal(flow::totalLiters());
    lastSaveMs = now;
  }
  wasFlowing = flowing;

  // Fehler-LED: NVS-Fehler oder unplausibler Dauerfluss (moegliches Leck)
  bool longFlow = CONTINUOUS_FLOW_ALERT_MS > 0 && flowing &&
                  (now - flow::flowStartMs()) >= CONTINUOUS_FLOW_ALERT_MS;
  relay_led::setErrorLed(persistence::hasError() || longFlow);

  // Serielles Log alle 2 s bei aktivem Fluss
  if (flowing && (now - lastLogMs) >= 2000) {
    lastLogMs = now;
    Serial.printf("Fluss: %.1f L/min | Gesamt: %.3f L | Relais: %s\n",
                  flow::flowRateLpm(), flow::totalLiters(),
                  relay_led::relayOn() ? "EIN" : "AUS");
  }
}
