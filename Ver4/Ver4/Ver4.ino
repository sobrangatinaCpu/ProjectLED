#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <time.h>

// =====================================================
// VERSION INFORMATION
// =====================================================

const char* VERSION = "4.0.0";
const int BUILD_NUMBER = 4;

// =====================================================
// WIFI
// =====================================================

const char* ssid = "PLDT FIBR 5G";
const char* password = "cheese_91125";

// =====================================================
// 5 LEDS
// =====================================================

const int LED_COUNT = 5;

const int LED_PINS[LED_COUNT] = {
  2,
  4,
  18,
  19,
  22
};

// =====================================================
// LED CHASER SPEED
// =====================================================

const unsigned long CHASE_INTERVAL = 200;

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
// CHASER VARIABLES
// =====================================================

int currentLED = 0;

int direction = 1;

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
// TURN OFF ALL LEDS
// =====================================================

void turnOffAllLEDs() {

  for (int i = 0; i < LED_COUNT; i++) {

    digitalWrite(
      LED_PINS[i],
      LOW
    );
  }
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
  html += "font-family:Arial;";
  html += "text-align:center;";
  html += "background:#f4f4f9;";
  html += "padding:30px;";
  html += "}";

  html += ".card{";
  html += "background:white;";
  html += "padding:30px;";
  html += "border-radius:15px;";
  html += "max-width:900px;";
  html += "margin:auto;";
  html += "box-shadow:0 4px 15px rgba(0,0,0,.1);";
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
  html += "background:#eee;";
  html += "}";

  html += "</style>";

  html += "</head>";

  html += "<body>";

  html += "<div class='card'>";

  // ===================================================
  // CURRENT VERSION
  // ===================================================

  html += "<h1>ESP32 LED Dashboard</h1>";

  html += "<p><b>Current Version:</b> ";
  html += VERSION;
  html += "</p>";

  html += "<p><b>Build:</b> ";
  html += String(BUILD_NUMBER);
  html += "</p>";

  // ===================================================
  // LED INFORMATION
  // ===================================================

  html += "<p><b>LED Count:</b> ";
  html += String(LED_COUNT);
  html += "</p>";

  html += "<p><b>LED Pins:</b> ";
  html += "GPIO 2, 4, 18, 19, 22";
  html += "</p>";

  html += "<p><b>Pattern:</b> ";
  html += "Left → Right → Left";
  html += "</p>";

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

  // ===================================================
  // VERSION 4
  // ===================================================

  html += "<tr>";

  html += "<td>4.0.0</td>";

  html += "<td>4</td>";

  html += "<td>2026-10-05 PHT</td>";

  html += "<td>";
  html += "Added 5-LED chaser using GPIO 2, 4, 18, 19, and 22";
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
  // INITIALIZE 5 LEDS
  // ===================================================

  for (int i = 0; i < LED_COUNT; i++) {

    pinMode(
      LED_PINS[i],
      OUTPUT
    );

    digitalWrite(
      LED_PINS[i],
      LOW
    );
  }

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
  // PHILIPPINE TIME
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
    "================================"
  );

  Serial.println(
    "VERSION 4 READY"
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
  // LED CHASER
  // ===================================================

  if (
    millis() - previousMillis >=
    CHASE_INTERVAL
  ) {

    previousMillis +=
      CHASE_INTERVAL;

    // Turn everything off first
    turnOffAllLEDs();

    // Turn on current LED
    digitalWrite(
      LED_PINS[currentLED],
      HIGH
    );

    // =================================================
    // INCREMENT COUNTER
    // Every LED activation counts
    // =================================================

    ledOnCount++;

    preferences.putULong(
      "counter",
      ledOnCount
    );

    Serial.print(
      "LED "
    );

    Serial.print(
      currentLED + 1
    );

    Serial.print(
      " ON | Total activations: "
    );

    Serial.println(
      ledOnCount
    );

    // =================================================
    // MOVE CHASER
    // =================================================

    currentLED += direction;

    // Right edge
    if (
      currentLED >= LED_COUNT
    ) {

      direction = -1;

      currentLED =
        LED_COUNT - 2;
    }

    // Left edge
    if (
      currentLED < 0
    ) {

      direction = 1;

      currentLED = 1;
    }
  }
}