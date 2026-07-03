#include "flow.h"
#include "config.h"

namespace flow {

namespace {

volatile uint32_t pulseCount = 0; // von der ISR erhöht
portMUX_TYPE pulseMux = portMUX_INITIALIZER_UNLOCKED;

double total = 0.0;          // Liter, rücksetzbar
uint32_t lastActiveMs = 0;   // letzter Zeitpunkt mit Impulsrate über der Schwelle
uint32_t startMs = 0;        // Beginn des aktuellen Flusses
bool flowing = false;
float rateLpm = 0.0f;        // geglättete Rate

// Gleitendes 1-s-Fenster als Rauschfilter (Buckets à CONTROL_INTERVAL_MS)
constexpr int WINDOW_BUCKETS = 1000 / CONTROL_INTERVAL_MS;
uint32_t window[WINDOW_BUCKETS] = {0};
int windowIdx = 0;

void IRAM_ATTR onPulse() {
  portENTER_CRITICAL_ISR(&pulseMux);
  pulseCount++;
  portEXIT_CRITICAL_ISR(&pulseMux);
}

} // namespace

void begin(double initialTotalLiters) {
  total = initialTotalLiters;
  // Hochohmiger Eingang: der externe Teiler definiert den Pegel.
  // KEIN interner Pulldown — er laege parallel zum unteren Teilerwiderstand
  // und drueckt den HIGH-Pegel unter die Schwelle (bei 16k/22k: 2,9V -> 2,4V).
  // Stoerimpulse auf offener Leitung filtert die Software (FLOW_START_MIN_PULSES).
  pinMode(PIN_FLOW_SENSOR, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_FLOW_SENSOR), onPulse, FALLING);
}

void update(uint32_t nowMs) {
  // Impulse seit letztem Aufruf atomar abholen
  portENTER_CRITICAL(&pulseMux);
  uint32_t pulses = pulseCount;
  pulseCount = 0;
  portEXIT_CRITICAL(&pulseMux);

  // Gleitendes 1-s-Fenster fortschreiben
  window[windowIdx] = pulses;
  windowIdx = (windowIdx + 1) % WINDOW_BUCKETS;
  uint32_t windowSum = 0;
  for (int i = 0; i < WINDOW_BUCKETS; i++) windowSum += window[i];

  // Volumen nur bei erkanntem Fluss zaehlen (filtert Stoerimpulse aus)
  if (flowing) {
    total += (double)pulses / PULSES_PER_LITER;
  }

  if (windowSum >= FLOW_START_MIN_PULSES) {
    if (!flowing) {
      flowing = true;
      startMs = nowMs;
      // Impulse der Anlaufphase (letzte Sekunde) nachholen
      total += (double)windowSum / PULSES_PER_LITER;
    }
    lastActiveMs = nowMs;
  } else if (flowing && (nowMs - lastActiveMs) >= FLOW_TIMEOUT_MS) {
    flowing = false;
    startMs = 0;
  }

  // Rate aus dem 1-s-Fenster: Hz -> L/min (Q = f / 6,6), leicht geglättet
  float hz = (float)windowSum * 1000.0f / (float)(WINDOW_BUCKETS * CONTROL_INTERVAL_MS);
  float instantLpm = hz * 60.0f / PULSES_PER_LITER;
  rateLpm = flowing ? (0.7f * rateLpm + 0.3f * instantLpm) : 0.0f;
}

bool isFlowing() { return flowing; }
float flowRateLpm() { return rateLpm; }
double totalLiters() { return total; }
void resetTotal() { total = 0.0; }
uint32_t flowStartMs() { return startMs; }

} // namespace flow
