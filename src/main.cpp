#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <TFT_eSPI.h>
#include <WiFi.h>

#include "secrets.h"

constexpr char STATUS_URL[] = "http://tarq-pi.local:8000/status";
constexpr unsigned long REFRESH_INTERVAL_MS = 5000;

TFT_eSPI display = TFT_eSPI();

unsigned long lastRefresh = 0;

void drawHeading() {
  display.fillScreen(TFT_NAVY);
  display.setTextDatum(MC_DATUM);

  display.setTextColor(TFT_CYAN, TFT_NAVY);
  display.drawString("TARQ PI MONITOR", 160, 25, 4);

  display.drawFastHLine(20, 48, 280, TFT_CYAN);
}

void drawMessage(const String &message, uint16_t colour) {
  display.fillRect(10, 65, 300, 155, TFT_NAVY);
  display.setTextDatum(MC_DATUM);
  display.setTextColor(colour, TFT_NAVY);
  display.drawString(message, 160, 130, 2);
}

void drawStatus(
  float temperature,
  float memory,
  float disk,
  unsigned long uptime
) {
  display.fillRect(10, 58, 300, 172, TFT_NAVY);
  display.setTextDatum(TL_DATUM);

  display.setTextColor(TFT_WHITE, TFT_NAVY);
  display.drawString("Temperature", 30, 65, 2);
  display.drawString("Memory", 30, 100, 2);
  display.drawString("Storage", 30, 135, 2);
  display.drawString("Uptime", 30, 170, 2);

  display.setTextColor(TFT_YELLOW, TFT_NAVY);
  display.drawString(String(temperature, 1) + " C", 205, 65, 2);
  display.drawString(String(memory, 1) + " %", 205, 100, 2);
  display.drawString(String(disk, 1) + " %", 205, 135, 2);

  unsigned long hours = uptime / 3600;
  unsigned long minutes = (uptime % 3600) / 60;

  display.drawString(
    String(hours) + "h " + String(minutes) + "m",
    205,
    170,
    2
  );

  display.setTextDatum(MC_DATUM);
  display.setTextColor(TFT_GREEN, TFT_NAVY);
  display.drawString("PI ONLINE", 160, 215, 2);
}

void connectToWiFi() {
  drawMessage("Connecting to Wi-Fi...", TFT_WHITE);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_NAME, WIFI_PASSWORD);

  unsigned long startTime = millis();

  while (
    WiFi.status() != WL_CONNECTED &&
    millis() - startTime < 20000
  ) {
    delay(250);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Wi-Fi connected. ESP32 address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Wi-Fi connection timed out");
    drawMessage("Wi-Fi connection failed", TFT_RED);
  }
}

void requestStatus() {
  if (WiFi.status() != WL_CONNECTED) {
    drawMessage("Wi-Fi offline - reconnecting", TFT_RED);
    connectToWiFi();
    return;
  }

  HTTPClient request;
  request.begin(STATUS_URL);

  int responseCode = request.GET();

  if (responseCode != HTTP_CODE_OK) {
    Serial.printf("HTTP request failed: %d\n", responseCode);
    drawMessage("Pi server unavailable", TFT_RED);
    request.end();
    return;
  }

  JsonDocument document;
  DeserializationError error =
    deserializeJson(document, request.getString());

  request.end();

  if (error) {
    Serial.print("JSON error: ");
    Serial.println(error.c_str());
    drawMessage("Invalid server response", TFT_RED);
    return;
  }

  drawStatus(
    document["temperature_c"] | 0.0,
    document["memory_percent"] | 0.0,
    document["disk_percent"] | 0.0,
    document["uptime_seconds"] | 0UL
  );
}

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, TFT_BACKLIGHT_ON);

  display.init();
  display.setRotation(1);

  drawHeading();
  connectToWiFi();
  requestStatus();

  lastRefresh = millis();
}

void loop() {
  if (millis() - lastRefresh >= REFRESH_INTERVAL_MS) {
    lastRefresh = millis();
    requestStatus();
  }

  delay(20);
}
