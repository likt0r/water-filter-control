#include "webserver.h"
#include <WiFi.h>
#include <WebServer.h>
#include "config.h"
#include "flow.h"
#include "persistence.h"
#include "relay_led.h"

namespace webserver {

namespace {

WebServer server(80);

const char INDEX_HTML[] PROGMEM = R"HTML(<!DOCTYPE html>
<html lang="de">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Wasserfilter</title>
<style>
  body { font-family: system-ui, sans-serif; margin: 0; background: #f0f4f8; color: #1a2733; }
  main { max-width: 26rem; margin: 0 auto; padding: 1.5rem 1rem; }
  h1 { font-size: 1.3rem; margin: 0 0 1rem; }
  .card { background: #fff; border-radius: 12px; padding: 1rem 1.2rem; margin-bottom: 0.8rem;
          box-shadow: 0 1px 3px rgba(0,0,0,.12); }
  .label { font-size: .8rem; color: #5b6b7a; text-transform: uppercase; letter-spacing: .05em; }
  .value { font-size: 2rem; font-weight: 600; margin-top: .2rem; }
  .unit { font-size: 1rem; font-weight: 400; color: #5b6b7a; }
  .status { display: flex; align-items: center; gap: .5rem; font-size: 1.05rem; }
  .dot { width: .8rem; height: .8rem; border-radius: 50%; background: #9aa7b2; }
  .dot.on { background: #2e9e44; }
  button { width: 100%; padding: .9rem; font-size: 1rem; border: 0; border-radius: 10px;
           background: #c62828; color: #fff; cursor: pointer; }
  button:active { background: #a51f1f; }
</style>
</head>
<body>
<main>
  <h1>&#128167; Wasserfilter</h1>
  <div class="card">
    <div class="label">Gesamtmenge</div>
    <div class="value"><span id="total">–</span> <span class="unit">Liter</span></div>
  </div>
  <div class="card">
    <div class="label">Aktueller Durchfluss</div>
    <div class="value"><span id="rate">–</span> <span class="unit">L/min</span></div>
  </div>
  <div class="card">
    <div class="status"><span id="dot" class="dot"></span><span id="ozon">Ozon aus</span></div>
  </div>
  <div class="card">
    <button onclick="resetCounter()">Z&auml;hler auf 0 setzen</button>
  </div>
</main>
<script>
async function refresh() {
  try {
    const r = await fetch('/api/status');
    const s = await r.json();
    document.getElementById('total').textContent = s.totalLiters.toFixed(2);
    document.getElementById('rate').textContent = s.flowRateLpm.toFixed(1);
    document.getElementById('dot').className = 'dot' + (s.relayOn ? ' on' : '');
    document.getElementById('ozon').textContent = s.relayOn ? 'Ozon aktiv' : 'Ozon aus';
  } catch (e) { /* Verbindung weg -> beim naechsten Tick erneut */ }
}
async function resetCounter() {
  if (!confirm('Zähler wirklich auf 0 setzen?')) return;
  await fetch('/api/reset', { method: 'POST' });
  refresh();
}
refresh();
setInterval(refresh, 2000);
</script>
</body>
</html>)HTML";

void handleRoot() {
  server.send_P(200, "text/html", INDEX_HTML);
}

void handleStatus() {
  char buf[128];
  snprintf(buf, sizeof(buf),
           "{\"totalLiters\":%.3f,\"flowRateLpm\":%.2f,\"relayOn\":%s}",
           flow::totalLiters(), flow::flowRateLpm(),
           relay_led::relayOn() ? "true" : "false");
  server.send(200, "application/json", buf);
}

void handleReset() {
  flow::resetTotal();
  persistence::saveTotal(0.0); // sofort sichern
  server.send(200, "application/json", "{\"ok\":true}");
}

} // namespace

void begin() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASSWORD);

  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/reset", HTTP_POST, handleReset);
  server.onNotFound([]() { server.send(404, "text/plain", "Not found"); });
  server.begin();

  Serial.print("SoftAP gestartet: ");
  Serial.print(AP_SSID);
  Serial.print("  IP: ");
  Serial.println(WiFi.softAPIP());
}

void handle() { server.handleClient(); }

} // namespace webserver
