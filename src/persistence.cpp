#include "persistence.h"
#include <Preferences.h>

namespace persistence {

namespace {
Preferences prefs;
bool error = false;
double lastSaved = -1.0;
constexpr char NVS_NAMESPACE[] = "waterfilter";
constexpr char KEY_TOTAL[] = "totalL";
} // namespace

double begin() {
  if (!prefs.begin(NVS_NAMESPACE, /*readOnly=*/false)) {
    error = true;
    return 0.0;
  }
  double liters = prefs.getDouble(KEY_TOTAL, 0.0);
  lastSaved = liters;
  return liters;
}

bool saveTotal(double liters) {
  if (liters == lastSaved) return true; // unnötige Flash-Schreibzyklen vermeiden
  size_t written = prefs.putDouble(KEY_TOTAL, liters);
  error = (written == 0);
  if (!error) lastSaved = liters;
  return !error;
}

bool hasError() { return error; }

} // namespace persistence
