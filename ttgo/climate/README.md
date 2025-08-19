# LILYGO T-Display SHTC3 Climate Monitor

A simple temperature and humidity monitoring project for the LILYGO T-Display ESP32 board with an SHTC3 sensor.

## Features

- Real-time temperature and humidity readings from SHTC3 sensor
- Large, easy-to-read display optimized for vertical orientation
- Color-coded display (yellow for temperature, green for humidity)
- Serial monitoring for data logging
- Automatic sensor initialization and error handling

## Hardware Requirements

- **LILYGO T-Display ESP32** development board
- **SHTC3** temperature and humidity sensor
- Jumper wires for connections

## Wiring

Connect the SHTC3 sensor to the T-Display as follows:

| SHTC3 Pin | T-Display Pin | Description |
|-----------|---------------|-------------|
| VCC       | 3.3V          | Power supply |
| GND       | GND           | Ground |
| SDA       | GPIO 21       | I2C Data line |
| SCL       | GPIO 22       | I2C Clock line |

## Software Dependencies

### Arduino Libraries Required:
- **TFT_eSPI** - Display driver for T-Display
- **Adafruit_SHTC3** - SHTC3 sensor library

### Installation:
1. Install the Arduino IDE or PlatformIO
2. Add ESP32 board support
3. Install required libraries via Library Manager:
   - Search and install "TFT_eSPI" 
   - Search and install "Adafruit SHTC3"

## Display Layout

The project uses vertical orientation (128×160 pixels) for optimal readability:

```
┌─────────────────┐
│    Climate      │ ← Size 2, Cyan header
│                 │
│    Temp         │ ← Size 2, Yellow label  
│    23.4  C      │ ← Size 3 number + Size 2 unit
│                 │
│    Humid        │ ← Size 2, Green label
│     45   %      │ ← Size 3 number + Size 2 unit
│                 │
└─────────────────┘
```

### Display Features:
- **Vertical orientation** - Better use of T-Display shape  
- **Large numbers** - Size 3 font for temperature/humidity values  
- **Clear labels** - "Temp" and "Humid" with size 2 font  
- **Good spacing** - More room between sections  
- **Integer humidity** - No decimal places for cleaner look  
- **Color coding** - Yellow temperature, Green humidity

## Usage

1. **Upload the code** to your LILYGO T-Display
2. **Connect the SHTC3 sensor** according to the wiring diagram
3. **Power on** - The display will show "Climate Monitor" and "Initializing..."
4. **If successful** - Display shows "SHTC3 Ready!" then starts showing readings
5. **If failed** - Display shows "SHTC3 Error!" - check wiring

## Serial Monitor Output

The project provides basic serial logging at 115200 baud:

```
LILYGO T-Display Climate Monitor
SHTC3 sensor initialized
Temperature: 23.4°C, Humidity: 45.2%
Temperature: 23.5°C, Humidity: 45.1%
...
```

## Code Structure

- `setup()` - Initializes display, I2C bus, and SHTC3 sensor
- `loop()` - Reads sensor data every 2 seconds and updates display
- `readSHTC3Data()` - Handles sensor communication and data retrieval

## Troubleshooting

### "SHTC3 Error!" on display:
1. Check all wiring connections
2. Verify 3.3V power to sensor
3. Ensure proper I2C connections (SDA→21, SCL→22)
4. Try different jumper wires

### "Sensor Error" during operation:
- Temporary communication issue
- Usually resolves automatically
- Check Serial Monitor for detailed error messages

### No display output:
- Verify TFT_eSPI library is installed
- Check T-Display board selection in Arduino IDE
- Ensure proper USB connection

## Technical Specifications

- **Sensor**: SHTC3 (I2C address 0x70)
- **Update Rate**: 2 seconds
- **Temperature Range**: -40°C to +80°C display range
- **Humidity Range**: 0% to 100% RH
- **Display**: 160×128 pixel color TFT
- **Communication**: I2C at 100kHz

## License

This project is open source and available for personal and educational use.

## Contributing

Feel free to submit issues, feature requests, or pull requests to improve this project.