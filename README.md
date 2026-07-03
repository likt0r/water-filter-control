# Wasserfilter-Ozonsteuerung (ESP32)

Steuerung für einen Wasserfilter: Ein **Wasserfluss-Sensor** erkennt, ob Wasser
fließt. Solange Wasser fließt, schaltet ein **Relais** einen **Ozongenerator**
ein, der Ozon ins Wasser einspeist; stoppt der Fluss, schaltet das Relais wieder
ab. Die durchgeflossene Wassermenge wird aufaddiert, **stromausfallsicher**
gespeichert und über eine kleine **Webseite (WLAN-Hotspot)** angezeigt — inkl.
Knopf zum Zurücksetzen des Zählers.

## Funktionen

- Relais folgt dem Wasserfluss (Ozongenerator an/aus) mit Timeout-Hysterese
- Durchflussmessung + aufaddierte Gesamtmenge (Liter), im NVS-Flash gespeichert
- SoftAP-Hotspot mit Live-Anzeige (Menge, L/min, Ozon-Status) + Reset-Knopf
- Status-LEDs: **Blau** = Wasserfluss, **Grün** = Relais/Ozon, **Rot** = Fehler

## Hardware

| Komponente | Details |
|---|---|
| Board | AZ-Delivery ESP32 DevKit C V4 (WROOM-32) |
| Flow-Sensor | G3/4" Messing-Hall-Sensor (YF-B6-Typ), 1–30 L/min, `F = 6,6 · Q` → ~396 Impulse/L |
| Relais | MakerFactory-Modul mit SRD-05VDC-SL-C (Low-aktiv), 10 A / 250 VAC |
| LEDs | direkt am GPIO: rot 120 Ω (U_f ≈ 2,1 V), blau + grün je 100 Ω (U_f ≈ 3,0–3,2 V @ 20 mA) |
| Spannungsteiler | 10 kΩ + 20 kΩ (Sensor 5 V → 3,3 V für den ESP32) |
| Versorgung | 5 V (USB oder Netzteil an VIN/5V) |

## Verkabelungsdiagramm

```
                  AZ-Delivery ESP32 DevKit C V4 (WROOM-32)
                 +---------------------------------------------+
      5V  ───────┤ 5V/VIN                                  27  ├──< Sensor-Signal (über Teiler)
      GND ──┬────┤ GND.1                                   26  ├──> Relais IN
            │    │                                         33  ├──[100Ω]──▶|── GND   blaue LED (Fluss)
            │    │                                         25  ├──[100Ω]──▶|── GND   grüne LED (Relais/Ozon)
            │    │                                         32  ├──[120Ω]──▶|── GND   rote  LED (Fehler)
            │    +---------------------------------------------+
            └──────────── gemeinsame Masse (GND) ─────────────┘


  Flow-Sensor (G3/4" Hall)                Spannungsteiler 5V → 3,3V:
   ┌──────────────┐
   │ Rot     (VCC)├───── 5V         Sensor Gelb ──[ R1 = 10kΩ ]──┬── GPIO27
   │ Schwarz (GND)├───── GND                                     │
   │ Gelb  (Signal)──► Teiler ─► GPIO27                     [ R2 = 20kΩ ]
   └──────────────┘                                              │
                                                                GND

  LEDs (alle direkt am GPIO):   GPIO ──[R]── LED-Anode ─▶|─ Kathode ── GND
    rot  (GPIO32): R = 120 Ω  → ~10 mA
    blau (GPIO33): R = 100 Ω  → ~3–6 mA (U_f sinkt bei kleinen Strömen unter 3 V)
    grün (GPIO25): R = 100 Ω  → ~3–6 mA
  Hinweis: Blau/Grün leuchten so etwas gedämpfter als mit voller 20-mA-
  Ansteuerung — als Statusanzeige gut sichtbar. Falls zu dunkel: LED über
  BC547-Transistor an 5 V schalten (LED: 5V→┤▶├→100Ω→Kollektor, Emitter→GND,
  Basis über 1 kΩ an den GPIO; BC547 flache Seite zu dir = C·B·E von links).

  Relais-Modul SRD-05VDC-SL-C            220 V-Seite  ⚠️ LEBENSGEFAHR:
   VCC ─── 5V                             COM ─── Netz L (Phase)
   GND ─── GND                            NO  ─── Ozongenerator ─── Netz N
   IN  ─── GPIO26                         NC  ─── frei
                                          (Schutzleiter des Generators beibehalten)
```

### Pin-Belegung

| Funktion | ESP32-Pin | Anschluss |
|---|---|---|
| Flow-Sensor Signal (gelb) | **GPIO 27** | über Spannungsteiler 10k/20k |
| Relais IN | **GPIO 26** | Relaismodul IN (Low-aktiv) |
| LED blau (Wasserfluss) | **GPIO 33** | → 100 Ω → LED → GND |
| LED grün (Relais/Ozon) | **GPIO 25** | → 100 Ω → LED → GND |
| LED rot (Fehler) | **GPIO 32** | → 120 Ω → LED → GND |
| 5 V | 5V / VIN | Sensor VCC, Relais VCC |
| GND | GND | Sensor, Relais, LED-Kathoden (Sternmasse) |

Die Pins sind in [`src/config.h`](src/config.h) definiert; Relais-Polarität,
K-Faktor, Timeouts und Hotspot-Zugang ebenfalls dort.

### ⚠️ Sicherheitshinweise

- **220 V ist lebensgefährlich.** Netzseite (COM/NO) berührungssicher isolieren,
  in ein Gehäuse mit Zugentlastung. Kleinspannungs- und Netzteil räumlich trennen.
- Erst die **komplette Logik mit LEDs/Kleinspannung** testen (oder in Wokwi),
  die 220 V-Last **zuletzt** anschließen.
- Der Sensor-Ausgang liefert ~5 V — **nie** direkt an einen GPIO, immer über den
  Spannungsteiler (ESP32-Pins sind nur 3,3 V-tolerant).
- Relais-Pin wird beim Boot zuerst auf „Aus" gesetzt, damit das Relais nicht
  kurz anzieht.

## Bauen & Flashen (PlatformIO)

```sh
pio run              # kompilieren
pio run -t upload    # auf den ESP32 flashen (per USB)
pio device monitor   # serielle Ausgabe (115200 Baud)
```

Im **Zed-Editor**: `Strg-⇧-P → task: spawn` → *PIO Build / Upload / Monitor*
(siehe [`.zed/tasks.json`](.zed/tasks.json)).

## Bedienung

1. Nach dem Start öffnet der ESP32 den WLAN-Hotspot **„Wasserfilter"**
   (Passwort `ozon1234`, änderbar in `src/config.h`).
2. Im Browser **`http://192.168.4.1`** öffnen → Gesamtmenge, aktueller
   Durchfluss (L/min), Ozon-Status und der **Reset-Knopf**.

## Simulation ohne Hardware

Das Projekt lässt sich komplett in **Wokwi** testen (Relais-/LED-Logik, Zähler,
Persistenz, Webseite) — der Wasserfluss wird per Custom-Chip simuliert.
Anleitung: [`WOKWI.md`](WOKWI.md).

## Projektstruktur

```
src/config.h          Pins, K-Faktor, Relais-Polarität, Timeouts, Hotspot
src/flow.*            Impulszählung (ISR), Durchflussrate, Litersumme
src/persistence.*     NVS-Speicherung via Preferences (stromausfallsicher)
src/relay_led.*       Relais- und LED-Ansteuerung
src/webserver.*       SoftAP + Webseite (Status + Reset)
src/main.cpp          Steuerlogik (Fluss → Relais/LED, Speichern, Fehler-LED)
diagram.json          Wokwi-Schaltung
chips/flow-pulse.*    Wokwi-Custom-Chip (simulierter Flow-Sensor)
```
