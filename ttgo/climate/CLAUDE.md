# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

LILYGO T-Display ESP32 climate monitoring project that displays real-time temperature and humidity readings from an SHTC3 sensor with a countdown timer showing time until next sensor refresh.

## Hardware Architecture

**Target Hardware**: LILYGO T-Display ESP32 development board
- **Display**: 160×128 pixel color TFT (ST7789 controller via TFT_eSPI library)
- **Orientation**: Vertical (setRotation(0)) for better use of display shape
- **Sensor**: SHTC3 temperature/humidity sensor via I2C (address 0x70)
- **I2C Configuration**: SDA=GPIO21, SCL=GPIO22

## Development Environment

**Arduino IDE Setup**:
- Board: "ESP32 Dev Module" or "LILYGO T-Display"
- Required Libraries: TFT_eSPI (display), Adafruit_SHTC3 (sensor)
- Serial Monitor: 115200 baud

**Build/Upload**: Use Arduino IDE "Upload" button or arduino-cli equivalent

## Code Architecture

**Single-file Arduino sketch** (`climate.ino`) with timing-based sensor management:

1. **Dual Refresh System**:
   - Sensor readings: Every 30 seconds (reduces I2C traffic, extends sensor life)
   - Display updates: Every 2 seconds (smooth countdown animation)
   - Uses `lastReadTime` and `millis()` for non-blocking timing

2. **Sensor Interface** (`readSHTC3Data()`):
   - Event-based API using `sensors_event_t` structures
   - Graceful error handling with fallback to last known values
   - Serial output in Arduino Serial Plotter format: "Temperature:value,Humidity:value"

3. **Display Layout** (optimized for vertical 128×160):
   - Header: "Climate" (Cyan, size 2)
   - Temperature: "Temp" + large number + "C" (Yellow, size 3 for number)
   - Humidity: "Humid" + large number + "%" (Green, size 3 for number)
   - Countdown: "Next Refresh" + large number + "s" (Magenta, size 3 for number)

4. **Error Handling Strategy**:
   - Setup failures: Fatal error display with infinite loop
   - Runtime failures: Continue with last good readings, log to serial

## Serial Output Modes

**Arduino Serial Plotter Compatible**: 
- Format: `Temperature:23.4,Humidity:45.2`
- Access via Tools → Serial Plotter in Arduino IDE
- Enables real-time graphing of both sensors simultaneously

**Standard Logging**: Basic initialization and error messages

## Display Layout Strategy

**Vertical Orientation Benefits**:
- Better fit for T-Display aspect ratio (taller than wide)
- Larger fonts possible with more vertical space
- Multi-line countdown display with consistent font sizing

**Font Hierarchy**:
- Size 3: All numeric values (temperature, humidity, countdown)
- Size 2: All labels and units
- Consistent visual hierarchy across all readings

## Timing Architecture

**30-Second Sensor Cycle**: Reduces I2C communication frequency for sensor longevity
**2-Second Display Cycle**: Provides smooth countdown experience
**Non-blocking Design**: Uses `millis()` comparison instead of `delay()` blocking