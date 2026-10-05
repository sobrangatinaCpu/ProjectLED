#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <time.h>

// =====================================================
// VERSION
// =====================================================

const char* VERSION = "2.0.0";
const int BUILD_NUMBER = 2;

// =====================================================
// WIFI
// =====================================================

const char* ssid = "PLDT FIBR 5G";
const char* password = "cheese_91125";

// =====================================================
// LEDS
// =====================================================

const int LED1_PIN = 2;
const int LED2_PIN = 4;

const unsigned long BLINK_INTERVAL = 500;

// =====================================================
// SERVER
// =====================================================

WebServer server(80);

// =====================================================
// PERMANENT COUNTER
// =====================================================

Preferences preferences;

unsigned long ledOnCount = 0;

// =====================================================
// LED STATE
// =====================================================

bool ledState = false;

unsigned long lastBlinkTime = 0;

// =====================================================
// PHT
// =====================================================

const long GMT_OFFSET_SEC = 8 * 3600;
const int DAYLIGHT_OFFSET_SEC = 0;

// =====================================================
// TIME
// =====================================================

String getDateTime() {

  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {
    return "Time not available";
  }

  char buffer[40];

  strftime(
    buffer,
    sizeof(buffer),
    "%Y-%m-%d %H:%M:%S",
    &timeinfo
  );

  return String(buffer) + " PHT";
}

// =====================================================
// DASHBOARD
// =====================================================

void handleRoot() {

  String html = "";

  html += "<!DOCTYPE html>";
  html += "<html>";
  html += "<head>";

  html += "<meta name='viewport' ";
  html += "content='width=device-width, initial-scale=1'>";

  html += "<meta http-equiv='refresh' content='2'>";

  html += "<title>ESP32 LED Dashboard</title>";

  html += "<style>";

  html += "body{font-family:Arial;text-align:center;";
  html += "background:#f4f4f9;padding:30px;}";

  html += ".card{background:white;padding:30px;";
  html += "border-radius:15px;max-width:800px;";
  html += "margin:auto;box-shadow:0 4px 15px rgba(0,0,0,.1);}";

  html += ".counter{font-size:60px;font-weight:bold;margin:20px;}";

  html += "table{border-collapse:collapse;width:100%;margin-top:20px;}";

  html += "th,td{border:1px solid #ccc;padding:10px;}";

  html += "th{background:#eee;}";

  html += "</style>";

  html += "</head>";

  html += "<body>";

  html += "<div class='card'>";

  html += "<h1>ESP32 LED Dashboard</h1>";

  html += "<p><b>Current Version:</b> ";
  html += VERSION;
  html += "</p>";

  html += "<p><b>Build:</b> ";
  html += BUILD_NUMBER;
  html += "</p>";

  html += "<p><b>LED Count:</b> 2</p>";

  html += "<p><b>LED Pins:</b> GPIO 2, GPIO 4</p>";

  html += "<p><b>Current Date / Time:</b><br>";
  html += getDateTime();
  html += "</p>";

  // ===================================================
  // COUNTER
  // ===================================================

  html += "<p><b>Total LED Activations</b></p>";

  html += "<div class='counter'>";
  html += String(ledOnCount);
  html += "</div>";

  // ===================================================
  // HISTORY
  // ===================================================

  html += "<h2>Version History</h2>";

  html += "<table>";

  html += "<tr>";
  html += "<th>Version</th>";
  html += "<th>Build</th>";
  html += "<th>Date / Time (PHT)</th>";
  html += "<th>Changes</th>";
  html += "</tr>";

  // V1

  html += "<tr>";

  html += "<td>1.0.0</td>";
  html += "<td>1</td>";
  html += "<td>2026-10-05 17:48:09 PHT</td>";

  html += "<td>";
  html += "Initial version - single LED on GPIO 2";
  html += "</td>";

  html += "</tr>";

  // V2

  html += "<tr>";

  html += "<td>2.0.0</td>";
  html += "<td>2</td>";
  html += "<td>2026-10-05 PHT</td>";

  html += "<td>";
  html += "Two LEDs on GPIO 2 and GPIO 4 - simultaneous 500 ms blink";
  html += "</td>";

  html += "</tr>";

  html += "</table>";

  html += "</div>";

  html += "</body>";

  html += "</html>";

  server.send(
    200,
    "text/html",
    html
  );
}

// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);

  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);

  // ===================================================
  // KEEP COUNTER FROM V1
  // ===================================================

  preferences.begin(
    "led-counter",
    false
  );

  ledOnCount =
    preferences.getULong(
      "counter",
      0
    );

  // ===================================================
  // WIFI
  // ===================================================

  WiFi.mode(WIFI_STA);

  WiFi.begin(
    ssid,
    password
  );

  Serial.print(
    "Connecting to Wi-Fi"
  );

  while (
    WiFi.status() != WL_CONNECTED
  ) {

    delay(500);

    Serial.print(".");
  }

  Serial.println();

  Serial.print(
    "IP Address: "
  );

  Serial.println(
    WiFi.localIP()
  );

  // ===================================================
  // PHT
  // ===================================================

  configTime(
    GMT_OFFSET_SEC,
    DAYLIGHT_OFFSET_SEC,
    "pool.ntp.org",
    "time.nist.gov"
  );

  delay(2000);

  Serial.print(
    "PHT: "
  );

  Serial.println(
    getDateTime()
  );

  // ===================================================
  // SERVER
  // ===================================================

  server.on(
    "/",
    handleRoot
  );

  server.begin();

  Serial.println(
    "Version 2 ready."
  );
}

// =====================================================
// LOOP
// =====================================================

void loop() {

  server.handleClient();

  // ===================================================
  // SIMULTANEOUS 500 ms BLINK
  // ===================================================

  if (
    millis() - lastBlinkTime >=
    BLINK_INTERVAL
  ) {

    lastBlinkTime = millis();

    ledState = !ledState;

    digitalWrite(
      LED1_PIN,
      ledState
    );

    digitalWrite(
      LED2_PIN,
      ledState
    );

    // Count one activation per simultaneous LED cycle
    if (ledState == true) {

      ledOnCount++;

      preferences.putULong(
        "counter",
        ledOnCount
      );

      Serial.print(
        "Dual LED ON | Counter: "
      );

      Serial.println(
        ledOnCount
      );
    }
  }
}