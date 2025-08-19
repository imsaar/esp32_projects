# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a LILYGO T-Display ESP32 climate monitoring project that displays temperature and humidity readings from an SHTC3 sensor on the built-in TFT display.

## Hardware Architecture

**Target Hardware**: LILYGO T-Display ESP32 development board
- **Display**: 160×128 pixel color TFT (ST7789 controller via TFT_eSPI library)
- **Orientation**: Vertical (setRotation(0)) for better readability
- **Sensor**: SHTC3 temperature/humidity sensor via I2C
- **I2C Configuration**: SDA=GPIO21, SCL=GPIO22, 100kHz clock

## Development Environment

**Arduino IDE Setup**:
- Board: "ESP32 Dev Module" or "LILYGO T-Display" 
- Required Libraries:
  - TFT_eSPI (display driver)
  - Adafruit_SHTC3 (sensor library)

**Build Command**: Use Arduino IDE "Upload" button or CLI equivalent
**Serial Monitor**: 115200 baud for debugging output

## Code Architecture

**Single-file Arduino sketch** (`climate.ino`) with three main components:

1. **Sensor Interface** (`readSHTC3Data()`):
   - Uses Adafruit_SHTC3 library's event-based API
   - Returns temperature and humidity via pointer parameters
   - Handles sensor communication errors gracefully

2. **Display Management** (in `setup()` and `loop()`):
   - Vertical layout optimized for T-Display dimensions
   - Color-coded sections: Cyan header, Yellow temperature, Green humidity
   - Large fonts (size 3) for primary values, size 2 for labels/units
   - Error states display red text and halt execution

3. **Main Control Flow**:
   - `setup()`: One-time initialization of display, I2C bus, and sensor
   - `loop()`: 2-second cycle of sensor reading and display updates
   - Sensor failures in setup() are fatal (infinite loop)
   - Sensor failures in loop() show error state but continue running

## Hardware Integration Notes

**I2C Sensor Communication**:
- SHTC3 uses I2C address 0x70 (fixed)
- Sensor initialization failure in setup() stops execution
- Runtime sensor failures display "Sensor Error" but don't halt

**Display Layout Strategy**:
- Designed for vertical viewing of the T-Display
- Temperature shows 1 decimal place, humidity shows integers
- Strategic positioning to avoid text overflow on 128-pixel width

## Common Development Tasks

**Testing Sensor Communication**: Check Serial Monitor output at 115200 baud
**Display Layout Changes**: Modify positioning values in loop() display sections
**Sensor Error Handling**: All I2C failures are logged to serial with descriptive messages