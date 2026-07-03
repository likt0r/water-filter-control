// Wokwi Custom Chip: simuliert einen Wasserfluss-Sensor (G3/4" Hall, YF-B6).
// Erzeugt am Pin OUT ein Rechtecksignal mit F = 6,6 * Q  (Q = L/min, Regler "lpm").
// => Firmware zaehlt 396 Impulse/Liter und liest die Durchflussrate korrekt.
#include "wokwi-api.h"

typedef struct {
  pin_t    pin_out;
  uint32_t lpm_attr; // Regler: Durchfluss in L/min
  timer_t  timer;
  uint8_t  level;
} chip_state_t;

// Einzelinstanz statisch (kein malloc -> keine libc noetig)
static chip_state_t chip_state;

static void on_timer(void *user_data) {
  chip_state_t *chip = (chip_state_t *)user_data;
  float lpm = attr_read_float(chip->lpm_attr);
  float freq = 6.6f * lpm; // volle Perioden pro Sekunde

  if (freq < 0.01f) {
    // Kein Fluss: OUT auf LOW halten, spaeter erneut pruefen
    chip->level = 0;
    pin_write(chip->pin_out, LOW);
    timer_start(chip->timer, 100000, false); // 100 ms
    return;
  }

  chip->level = !chip->level;
  pin_write(chip->pin_out, chip->level ? HIGH : LOW);

  uint32_t half_us = (uint32_t)(500000.0f / freq); // halbe Periode in us
  if (half_us < 1) half_us = 1;
  timer_start(chip->timer, half_us, false);
}

void chip_init(void) {
  chip_state_t *chip = &chip_state;
  chip->level = 0;
  chip->pin_out = pin_init("OUT", OUTPUT);
  chip->lpm_attr = attr_init_float("lpm", 10.0f);

  const timer_config_t cfg = {
    .callback = on_timer,
    .user_data = chip,
  };
  chip->timer = timer_init(&cfg);

  pin_write(chip->pin_out, LOW);
  timer_start(chip->timer, 100000, false);
}
