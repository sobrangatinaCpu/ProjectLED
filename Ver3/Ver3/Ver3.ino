#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <time.h>

// =====================================================
// VERSION INFORMATION
// =====================================================

const char* VERSION = "3.0.0";
const int BUILD_NUMBER = 3;

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
// WEB SERVER
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

unsigned long previousMillis = 0;

// =====================================================
// PHILIPPINE TIME
// =====================================================

const long GMT_OFFSET_SEC = 8 * 3600;
const int DAYLIGHT_OFFSET_SEC = 0;

// =====================================================
// GET PHT
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
  html += "border-radius:15px;max-width:850px;";
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

  // ===================================================
  // CURRENT VERSION
  // ===================================================

  html += "<p><b>Current Version:</b> ";
  html += VERSION;
  html += "</p>";

  html += "<p><b>Build:</b> ";
  html += String(BUILD_NUMBER);
  html += "</p>";

  html += "<p><b>LED Count:</b> 2</p>";

  html += "<p><b>LED Pins:</b> GPIO 2, GPIO 4</p>";

  // ===================================================
  // TIME
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

  // ===================================================
  // VERSION 1
  // ===================================================

  html += "<tr>";

  html += "<td>1.0.0</td>";

  html += "<td>1</td>";

  html += "<td>2026-10-05 17:48:09 PHT</td>";

  html += "<td>";
  html += "Initial version - single LED on GPIO 2";
  html += "</td>";

  html += "</tr>";

  // ===================================================
  // VERSION 2
  // ===================================================

  html += "<tr>";

  html += "<td>2.0.0</td>";

  html += "<td>2</td>";

  html += "<td>2026-10-05 PHT</td>";

  html += "<td>";
  html += "Two LEDs on GPIO 2 and GPIO 4 - simultaneous 500 ms blink";
  html += "</td>";

  html += "</tr>";

  // ===================================================
  // VERSION 3
  // ===================================================

  html += "<tr>";

  html += "<td>3.0.0</td>";

  html += "<td>3</td>";

  html += "<td>2026-10-05 PHT</td>";

  html += "<td>";
  html += "Bug fix - synchronized LED timing using stable millis() timing";
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

  // ===================================================
  // LED SETUP
  // ===================================================

  pinMode(
    LED1_PIN,
    OUTPUT
  );

  pinMode(
    LED2_PIN,
    OUTPUT
  );

  digitalWrite(
    LED1_PIN,
    LOW
  );

  digitalWrite(
    LED2_PIN,
    LOW
  );

  // ===================================================
  // LOAD PERSISTENT COUNTER
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
  // WEB SERVER
  // ===================================================

  server.on(
    "/",
    handleRoot
  );

  server.begin();

  Serial.println(
    "Web server started."
  );

  Serial.println(
    "Version 3 ready."
  );
}

// =====================================================
// LOOP
// =====================================================

void loop() {

  server.handleClient();

  // ===================================================
  // BUG-FIXED TIMING
  // ===================================================

  unsigned long currentMillis =
    millis();

  if (
    currentMillis - previousMillis >=
    BLINK_INTERVAL
  ) {

    // Maintain stable timing instead of resetting
    // the timer to currentMillis.

    previousMillis +=
      BLINK_INTERVAL;

    // =================================================
    // TOGGLE BOTH LEDS TOGETHER
    // =================================================

    ledState = !ledState;

    digitalWrite(
      LED1_PIN,
      ledState
    );

    digitalWrite(
      LED2_PIN,
      ledState
    );

    // =================================================
    // COUNT LED ACTIVATION
    // =================================================

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