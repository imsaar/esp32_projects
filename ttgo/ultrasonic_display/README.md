# Ultrasonic Distance Meter with Servo Sweep

An ESP32-based distance meter using a TTGO T-Display, HC-SR04 ultrasonic sensor, and a servo motor that continuously sweeps 180°.

## Hardware

| Component | Pin |
|-----------|-----|
| HC-SR04 Trig | GPIO 2 |
| HC-SR04 Echo | GPIO 15 |
| Servo signal | GPIO 27 |
| Servo power | 5V |
| Servo ground | GND |
| HC-SR04 VCC | 3.3V |
| HC-SR04 GND | GND |

## Wiring Diagram

```
TTGO T-Display          HC-SR04
─────────────           ───────
3.3V       ────────────  VCC
GND        ────────────  GND
GPIO 2     ────────────  TRIG
GPIO 15    ────────────  ECHO


TTGO T-Display          Servo
─────────────           ─────
5V         ────────────  Red   (power)
GND        ────────────  Brown (ground)
GPIO 27    ────────────  Orange (signal)
```

## Libraries Required

- **TFT_eSPI** — display driver (configure for TTGO T-Display)
- **ESP32Servo** by Kevin Harrington — servo control via LEDC PWM

Install both via Arduino IDE Library Manager.

## How It Works

- The servo sweeps continuously from 0° to 180° and back at ~10ms per degree.
- Distance is measured every 100ms on **Core 0** using a dedicated FreeRTOS task, completely isolated from the servo timer running on Core 1. This is necessary because the ESP32Servo library's hardware timer interrupts block `pulseIn` when both run on the same core.
- The display updates every 10 servo steps (~100ms), showing distance in cm. Objects beyond 400cm or out of sensor range show "Out of Range".

## Display

- Boot: red/green/blue flash sequence
- Header: "Ultrasonic Sensor / Distance Meter"
- Live reading: cyan distance in cm, or red "Out of Range"
