# Wokwi-Simulation

Damit lässt sich die komplette Steuerlogik **ohne echte Hardware** testen:
Relais- und LED-Schaltung, Zähler/Persistenz und die SoftAP-Webseite. Der
Wasserfluss wird über einen Custom-Chip (`chips/flow-pulse`) simuliert, der
Impulse mit `F = 6,6 · Q` erzeugt — ein Regler stellt den Durchfluss in L/min ein.

Dateien:
- `diagram.json` — Schaltung (ESP32 + Relais + 3 LEDs + Flow-Chip)
- `wokwi.toml` — verweist auf die PlatformIO-Firmware + den Custom-Chip
- `chips/flow-pulse.chip.c` / `.chip.json` — der simulierte Flow-Sensor

Vorher immer einmal die Firmware bauen:
```
pio run
```

## Variante A — Wokwi Web-IDE (am einfachsten, kein lokaler Compiler)

1. Auf <https://wokwi.com> einloggen → **New Project** → ESP32.
2. Inhalte von `diagram.json` einfügen.
3. Custom-Chip anlegen: im Editor **+ → New file** `flow-pulse.chip.c` und
   `flow-pulse.chip.json` mit dem Inhalt aus `chips/` — Wokwi kompiliert die
   `.wasm` automatisch in der Cloud (kein clang nötig).
4. Firmware hochladen: `.pio/build/az-delivery-devkit-v4/firmware.bin`
   (Web-IDE: Firmware-Upload bzw. die Datei ins Projekt ziehen).
5. **Play** drücken. Über den Regler am Flow-Chip den Durchfluss ändern →
   Relais/grüne LED schalten, blaue LED zeigt Fluss, der Zähler steigt.
6. **Webseite testen:** Wokwi WiFi-Gateway aktivieren (Doku:
   <https://docs.wokwi.com/guides/esp32-wifi>), dann die vom ESP32 gehostete
   Seite im Browser öffnen.

## Variante B — Lokal mit `wokwi-cli` (in Zed/Terminal)

1. CLI installieren: <https://docs.wokwi.com/wokwi-cli/getting-started>
   (kostenloser `WOKWI_CLI_TOKEN` erforderlich).
2. **Custom-Chip zu WASM bauen** (einmalig) — braucht clang mit wasm32-Target
   und `lld`:
   ```
   cd chips
   curl -O https://raw.githubusercontent.com/wokwi/wokwi-chip-c-template/main/wokwi-api.h
   make
   cd ..
   ```
   > Fedora: `sudo dnf install clang lld`. Ohne wasm-Toolchain stattdessen
   > Variante A nutzen.
3. Simulation starten:
   ```
   pio run
   wokwi-cli .
   ```

## Grenzen der Simulation
- ISR-Timing, Kontaktprellen und echte 5-V-Pegel werden **nicht** exakt
  abgebildet — der finale Test am echten Board + Relais bleibt nötig.
- Der Spannungsteiler (10k/20k) entfällt in der Sim: der Chip gibt bereits
  einen sauberen Logikpegel auf GPIO27.
