#pragma once

// ===================== Pin-Belegung =====================
// Siehe Schaltplan im Plan. Vermeidet Strapping-/Input-only-/Flash-Pins.
constexpr int PIN_FLOW_SENSOR = 27; // Gelbe Ader, über Spannungsteiler 10k/20k
constexpr int PIN_RELAY       = 26; // Relaismodul IN
constexpr int PIN_LED_BLUE    = 33; // Wasserfluss
constexpr int PIN_LED_GREEN   = 25; // Relais/Ozon aktiv
constexpr int PIN_LED_RED     = 32; // Fehler

// ===================== Flow-Sensor ======================
// G3/4" Messing-Hall-Sensor (YF-B6-Typ): F = 6,6 * Q  (F in Hz, Q in L/min)
// => 6,6 * 60 = 396 Impulse pro Liter. Bei Bedarf nachkalibrieren.
constexpr float PULSES_PER_LITER = 396.0f;

// Kein Impuls für diese Dauer => Wasserfluss gilt als beendet
constexpr uint32_t FLOW_TIMEOUT_MS = 1500;

// Mindestimpulse im 1-s-Fenster, damit "Fluss" erkannt wird (Rauschfilter).
// Echter Fluss: >= 6,6 Hz (1 L/min). Stoerimpulse auf offener Leitung: << 1 Hz.
constexpr uint32_t FLOW_START_MIN_PULSES = 4;

// Abtastintervall der Steuerlogik
constexpr uint32_t CONTROL_INTERVAL_MS = 200;

// ===================== Relais ===========================
// MakerFactory-Modul mit SRD-05VDC-SL-C: dieses Exemplar ist High-aktiv
// (IN HIGH = Relais/Ozon EIN). Am Board verifiziert.
constexpr bool RELAY_ACTIVE_LOW = false;

// ===================== LED-Selbsttest ===================
// Beim Start alle LEDs so lange einschalten (Funktionskontrolle)
constexpr uint32_t LED_SELFTEST_MS = 2000;

// ===================== Blaue LED: Helligkeit ~ Durchfluss =====
// Die blaue LED wird per PWM (LEDC) gedimmt: wenig Durchfluss = schwach,
// ab FLOW_LED_MAX_LPM = volle Helligkeit.
constexpr int      LEDC_CHANNEL_BLUE = 0;
constexpr int      LEDC_FREQ_HZ      = 5000; // flimmerfrei
constexpr int      LEDC_RES_BITS     = 8;    // Duty 0..255
constexpr float    FLOW_LED_MAX_LPM  = 15.0f; // ab hier max. Helligkeit
constexpr int      FLOW_LED_MIN_DUTY = 20;   // Mindesthelligkeit bei Fluss (sichtbar)
constexpr int      FLOW_LED_MAX_DUTY = 255;  // volle Helligkeit

// ===================== Fehler-Erkennung =================
// Dauerfluss länger als diese Zeit => rote LED (mögliches Leck).
// 0 = deaktiviert.
constexpr uint32_t CONTINUOUS_FLOW_ALERT_MS = 60UL * 60UL * 1000UL; // 60 min

// ===================== Persistenz (NVS) =================
// Während aktivem Fluss spätestens alle X ms speichern
constexpr uint32_t SAVE_INTERVAL_MS = 10000;

// ===================== SoftAP / Webseite ================
constexpr char AP_SSID[]     = "Wasserfilter";
constexpr char AP_PASSWORD[] = "ozon1234"; // min. 8 Zeichen (WPA2)
