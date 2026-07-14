#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>

// Built-in touchscreen connections.
constexpr int TOUCH_IRQ  = 36;
constexpr int TOUCH_MOSI = 32;
constexpr int TOUCH_MISO = 39;
constexpr int TOUCH_CLK  = 25;
constexpr int TOUCH_CS   = 33;

TFT_eSPI display = TFT_eSPI();

// Use a separate SPI controller for the touchscreen.
SPIClass touchSPI = SPIClass(VSPI);
XPT2046_Touchscreen touch(TOUCH_CS, TOUCH_IRQ);

void drawInterface() {
  display.fillScreen(TFT_NAVY);
  display.setTextColor(TFT_WHITE, TFT_NAVY);
  display.setTextDatum(MC_DATUM);

  display.drawString("TOUCH TEST", 160, 35, 4);
  display.drawString("Press anywhere", 160, 75, 2);

  display.drawRoundRect(40, 105, 240, 90, 8, TFT_CYAN);
}

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, TFT_BACKLIGHT_ON);

  display.init();
  display.setRotation(1);
  drawInterface();

  touchSPI.begin(
    TOUCH_CLK,
    TOUCH_MISO,
    TOUCH_MOSI,
    TOUCH_CS
  );

  touch.begin(touchSPI);
  touch.setRotation(1);

  Serial.println("Touch diagnostic started");
}

void loop() {
  if (!touch.tirqTouched() || !touch.touched()) {
    return;
  }

  TS_Point point = touch.getPoint();

  Serial.printf(
    "Raw X: %d, Raw Y: %d, Pressure: %d\n",
    point.x,
    point.y,
    point.z
  );

  // Clear the previous readings.
  display.fillRect(45, 110, 230, 80, TFT_NAVY);

  display.setTextColor(TFT_YELLOW, TFT_NAVY);
  display.setTextDatum(TL_DATUM);

  display.drawString("Raw X: " + String(point.x), 65, 120, 2);
  display.drawString("Raw Y: " + String(point.y), 65, 145, 2);
  display.drawString("Pressure: " + String(point.z), 65, 170, 2);

  delay(80);
}