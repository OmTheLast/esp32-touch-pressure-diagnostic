# ESP32 Touch Pressure Diagnostic

A small PlatformIO project for learning how the ELEGOO ESP32-32E 2.8-inch
resistive touchscreen works. It displays the raw touch coordinates and pressure
reported by the XPT2046 touch controller, both on screen and over serial.

## Hardware

- ELEGOO ESP32-32E 2.8-inch touchscreen board
- 240 x 320 ILI9341-compatible TFT display
- XPT2046 resistive touch controller
- USB data cable
- Stylus (recommended for resistive touch)

The board contains its own ESP32 and all display/touch wiring is internal; no
external jumper wires are required.

## What it demonstrates

- Initialising the TFT with `TFT_eSPI`
- Running the display and touch controller on separate SPI peripherals
- Reading raw X, Y, and pressure values from the XPT2046
- Showing diagnostic readings on a 320 x 240 landscape interface

Raw values are sensor measurements rather than calibrated screen pixels. A
future calibration step can map them to the display's 320 x 240 coordinates.

## Build and upload

1. Install Visual Studio Code and the PlatformIO extension.
2. Open this repository as a PlatformIO project.
3. Connect the board over USB.
4. Run **Build**, then **Upload**.
5. Open the serial monitor at 115200 baud if you also want terminal output.

PlatformIO normally detects the upload port automatically. If an upload stalls
at `Connecting...`, hold **BOOT**, tap **RESET**, then release **BOOT** when the
write begins.

## Expected output

The display shows a `TOUCH TEST` screen. Pressing it updates:

```text
Raw X: ...
Raw Y: ...
Pressure: ...
```

## Known limitation

The original test unit has a damaged or inconsistent resistive touch layer, so
some presses may not register reliably. The diagnostic is still useful for
identifying dead areas and comparing pressure readings.

## Private local files

The original factory firmware backup is intentionally excluded from Git. Build
artifacts, editor metadata, and machine-specific serial-port settings are also
ignored.
