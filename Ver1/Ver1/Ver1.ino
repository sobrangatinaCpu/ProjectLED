#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <time.h>

// =====================================================
// VERSION INFORMATION
// =====================================================

const char* VERSION = "1.0.0";
const int BUILD_NUMBER = 1;

// =====================================================
// WIFI
// =====================================================

const char* ssid = "PLDT FIBR 5G";
const char* password = "cheese_91125";

// =====================================================
// LED
// =====================================================

const int LED_PIN = 2;

const unsigned long BLINK_INTERVAL = 1000;

// =====================================================
// WEB SERVER
// =====================================================

WebServer server(80);

// =====================================================
// PERMANENT LED COUNTER
// =====================================================

Preferences preferences;

unsigned long ledOnCount = 0;

// =====================================================
// LED STATE
// =====================================================

bool ledState = false;

unsigned long lastBlinkTime = 0;

// =====================================================
// PHILIPPINE TIME
// UTC +8
// =====================================================

const long GMT_OFFSET_SEC = 8 * 3600;
const int DAYLIGHT_OFFSET_SEC = 0;

// =====================================================
// GET PHILIPPINE DATE AND TIME
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

  html += "body{";
  html += "font-family:Arial,sans-serif;";
  html += "text-align:center;";
  html += "background:#f4f4f9;";
  html += "margin:0;";
  html += "padding:30px;";
  html += "}";

  html += ".card{";
  html += "background:white;";
  html += "padding:30px;";
  html += "border-radius:15px;";
  html += "box-shadow:0 4px 15px rgba(0,0,0,.1);";
  html += "max-width:800px;";
  html += "margin:auto;";
  html += "}";

  html += ".counter{";
  html += "font-size:60px;";
  html += "font-weight:bold;";
  html += "margin:20px;";
  html += "}";

  html += "table{";
  html += "border-collapse:collapse;";
  html += "width:100%;";
  html += "margin-top:20px;";
  html += "}";

  html += "th,td{";
  html += "border:1px solid #ccc;";
  html += "padding:10px;";
  html += "}";

  html += "th{";
  html += "background:#eeeeee;";
  html += "}";

  html += ".reset{";
  html += "background:#dc3545;";
  html += "color:white;";
  html += "padding:10px 20px;";
  html += "border:none;";
  html += "border-radius:5px;";
  html += "font-size:16px;";
  html += "}";

  html += "</style>";

  html += "</head>";

  html += "<body>";

  html += "<div class='card'>";

  // ===================================================
  // TITLE
  // ===================================================

  html += "<h1>ESP32 LED Dashboard</h1>";

  // ===================================================
  // CURRENT VERSION
  // ===================================================

  html += "<p><b>Current Version:</b> ";
  html += VERSION;
  html += "</p>";

  html += "<p><b>Build:</b> ";
  html += String(BUILD_NUMBER);
  html += "</p>";

  // ===================================================
  // LED INFORMATION
  // ===================================================

  html += "<p><b>LED Count:</b> 1</p>";

  html += "<p><b>LED Pin:</b> GPIO 2</p>";

  // ===================================================
  // CURRENT TIME
  // ===================================================

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
  // VERSION HISTORY
  // ===================================================

  html += "<h2>Version History</h2>";

  html += "<table>";

  html += "<tr>";
  html += "<th>Version</th>";
  html += "<th>Build</th>";
  html += "<th>Date / Time (PHT)</th>";
  html += "<th>Changes</th>";
  html += "</tr>";

  // ---------------------------------------------------
  // VERSION 1
  // ---------------------------------------------------

  html += "<tr>";

  html += "<td>1.0.0</td>";

  html += "<td>1</td>";

  html += "<td>2026-10-05 17:48:09 PHT</td>";

  html += "<td>";
  html += "Initial version - single LED on GPIO 2";
  html += "</td>";

  html += "</tr>";

  html += "</table>";

  // ===================================================
  // RESET COUNTER
  // ===================================================

  html += "<br><br>";

  html += "<a href='/reset'>";

  html += "<button class='reset'>";
  html += "Reset Counter";
  html += "</button>";

  html += "</a>";

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
// RESET COUNTER
// =====================================================

void handleReset() {

  ledOnCount = 0;

  preferences.putULong(
    "counter",
    0
  );

  Serial.println(
    "Counter reset to 0."
  );

  server.sendHeader(
    "Location",
    "/"
  );

  server.send(
    303
  );
}

// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  // ===================================================
  // LED
  // ===================================================

  pinMode(
    LED_PIN,
    OUTPUT
  );

  digitalWrite(
    LED_PIN,
    LOW
  );

  // ===================================================
  // PERMANENT STORAGE
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

  Serial.print(
    "Saved LED activation count: "
  );

  Serial.println(
    ledOnCount
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

  Serial.println(
    "Wi-Fi connected."
  );

  Serial.print(
    "IP Address: "
  );

  Serial.println(
    WiFi.localIP()
  );

  // ===================================================
  // PHILIPPINE TIME
  // ===================================================

  configTime(
    GMT_OFFSET_SEC,
    DAYLIGHT_OFFSET_SEC,
    "pool.ntp.org",
    "time.nist.gov"
  );

  Serial.println(
    "Synchronizing Philippine time..."
  );

  delay(2000);

  Serial.print(
    "Current PHT: "
  );

  Serial.println(
    getDateTime()
  );

  // ===================================================
  // WEB SERVER
  // ===================================================

  server.on(
    "/",
    handleRoot
  );

  server.on(
    "/reset",
    handleReset
  );

  server.begin();

  Serial.println(
    "Web server started."
  );

  Serial.println(
    "================================"
  );

  Serial.println(
    "VERSION 1 READY"
  );

  Serial.println(
    "================================"
  );
}

// =====================================================
// LOOP
// =====================================================

void loop() {

  server.handleClient();

  // ===================================================
  // BLINK GPIO 2
  // ===================================================

  if (
    millis() - lastBlinkTime >=
    BLINK_INTERVAL
  ) {

    lastBlinkTime = millis();

    ledState = !ledState;

    digitalWrite(
      LED_PIN,
      ledState
    );

    // =================================================
    // COUNT WHEN LED TURNS ON
    // =================================================

    if (ledState == true) {

      ledOnCount++;

      preferences.putULong(
        "counter",
        ledOnCount
      );

      Serial.print(
        "LED ON | Total activations: "
      );

      Serial.println(
        ledOnCount
      );
    }
  }
}