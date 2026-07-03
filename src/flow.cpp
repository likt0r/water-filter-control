#include "flow.h"
#include "config.h"

namespace flow {

namespace {

volatile uint32_t pulseCount = 0; // von der ISR erhöht
portMUX_TYPE pulseMux = portMUX_INITIALIZER_UNLOCKED;

double total = 0.0;          // Liter, rücksetzbar
uint32_t lastPulseMs = 0;    // Zeitpunkt des letzten verarbeiteten Impulses
uint32_t startMs = 0;        // Beginn des aktuellen Flusses
bool flowing = false;
float rateLpm = 0.0f;        // geglättete Rate

uint32_t lastUpdateMs = 0;

void IRAM_ATTR onPulse() {
  portENTER_CRITICAL_ISR(&pulseMux);
  pulseCount++;
  portEXIT_CRITICAL_ISR(&pulseMux);
}

} // namespace

void begin(double initialTotalLiters) {
  total = initialTotalLiters;
  pinMode(PIN_FLOW_SENSOR, INPUT); // externer Teiler zieht auf GND
  attachInterrupt(digitalPinToInterrupt(PIN_FLOW_SENSOR), onPulse, FALLING);
}

void update(uint32_t nowMs) {
  // Impulse seit letztem Aufruf atomar abholen
  portENTER_CRITICAL(&pulseMux);
  uint32_t pulses = pulseCount;
  pulseCount = 0;
  portEXIT_CRITICAL(&pulseMux);

  uint32_t dtMs = nowMs - lastUpdateMs;
  lastUpdateMs = nowMs;

  if (pulses > 0) {
    total += (double)pulses / PULSES_PER_LITER;
    lastPulseMs = nowMs;
    if (!flowing) {
      flowing = true;
      startMs = nowMs;
    }
  } else if (flowing && (nowMs - lastPulseMs) >= FLOW_TIMEOUT_MS) {
    flowing = false;
    startMs = 0;
  }

  // Rate: Impulse pro Intervall -> Hz -> L/min (Q = f / 6,6), leicht geglättet
  if (dtMs > 0) {
    float hz = (float)pulses * 1000.0f / (float)dtMs;
    float instantLpm = hz * 60.0f / PULSES_PER_LITER;
    rateLpm = flowing ? (0.7f * rateLpm + 0.3f * instantLpm) : 0.0f;
  }
}

bool isFlowing() { return flowing; }
float flowRateLpm() { return rateLpm; }
double totalLiters() { return total; }
void resetTotal() { total = 0.0; }
uint32_t flowStartMs() { return startMs; }

} // namespace flow
