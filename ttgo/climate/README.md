# LILYGO T-Display SHTC3 Climate Monitor

A simple temperature and humidity monitoring project for the LILYGO T-Display ESP32 board with an SHTC3 sensor.

## Features

- Real-time temperature and humidity readings from SHTC3 sensor
- Dual display modes: textual (vertical) and graphical (horizontal) views
- Button toggle between display modes with immediate response
- Large, easy-to-read textual display optimized for vertical orientation
- Graphical trend view with line charts showing last 10 readings
- Color-coded display (yellow for temperature, green for humidity)
- 30-second sensor refresh with 2-second display updates and countdown timer
- Immediate first reading on startup, then 30-second intervals
- Serial monitoring for data logging and Arduino IDE Serial Plotter
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

The project has two display modes:

### Textual View (Vertical - 128×160 pixels):

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
│ Next Refresh    │ ← Size 2, Magenta label
│     23   s      │ ← Size 3 number + Size 2 unit
└─────────────────┘
```

### Graphical View (Horizontal - 160×128 pixels):
```
┌─────────────────────────────────────────────────────────────┐
│ Climate Graph    T:23.4C  H:45%  Next: 23s                 │
│ ┌─────────────────────────────────────────────────────────┐ │
│ │                Temperature Trend (Yellow)               │ │
│ │     /\    /\                                            │ │
│ │    /  \  /  \                                           │ │
│ │   /    \/    \                                          │ │
│ │ ─────────────────────────────────────────────────────── │ │
│ │                Humidity Trend (Green)                   │ │
│ │      /\      /\                                         │ │
│ │     /  \    /  \                                        │ │
│ │    /    \  /    \                                       │ │
│ └─────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

### Display Features:
- **Dual view modes** - Toggle between textual and graphical displays
- **Right button toggle** - Press right button (GPIO 35) to switch views instantly
- **Textual view** - Large numbers with size 3 font for easy reading
- **Graphical view** - Line charts showing trends over last 10 readings
- **Auto-orientation** - Vertical for text, horizontal for graphs
- **History tracking** - Stores last 10 sensor readings for trend analysis
- **Clear labels** - Consistent size 2 font for all labels
- **Integer humidity** - No decimal places for cleaner look  
- **Color coding** - Yellow temperature, Green humidity, Magenta countdown
- **Immediate response** - Button presses change view instantly
- **Countdown timer** - Shows seconds until next sensor reading

## Usage

1. **Upload the code** to your LILYGO T-Display
2. **Connect the SHTC3 sensor** according to the wiring diagram
3. **Power on** - The display will show "Climate Monitor" and "Initializing..."
4. **If successful** - Display shows "SHTC3 Ready!" then immediately shows first reading
5. **If failed** - Display shows "SHTC3 Error!" - check wiring
6. **Toggle views** - Press the right button to switch between textual and graphical displays
7. **View modes**:
   - **Textual**: Large numbers in vertical orientation
   - **Graphical**: Trend charts in horizontal orientation showing last 10 readings

## Serial Monitor Output

The project provides serial output at 115200 baud in two modes:

### Standard Logging:
```
LILYGO T-Display Climate Monitor
SHTC3 sensor initialized
```

### Arduino IDE Serial Plotter Format:
```
Temperature:23.4,Humidity:45.2
Temperature:23.5,Humidity:45.1
...
```

The Serial Plotter format allows real-time graphing of both temperature and humidity values in the Arduino IDE Serial Plotter (Tools → Serial Plotter).

## Code Structure

- `setup()` - Initializes display, I2C bus, button, and SHTC3 sensor
- `loop()` - Main control loop with immediate first reading, then 30-second intervals
- `readSHTC3Data()` - Handles sensor communication and data retrieval
- `drawTextualView()` - Renders large-text vertical display
- `drawGraphicalView()` - Renders trend charts in horizontal orientation
- Button handling with debouncing and immediate view switching

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
- **First Reading**: Immediate on startup
- **Sensor Reading**: 30 seconds after first successful reading
- **Display Update**: 2 seconds for smooth countdown animation
- **Button Response**: Immediate view toggle (50ms polling)
- **History Storage**: Last 10 readings for graphical trends
- **Temperature Range**: -40°C to +80°C display range
- **Humidity Range**: 0% to 100% RH
- **Display**: 160×128 pixel color TFT
- **Communication**: I2C at 100kHz

## License

This project is open source and available for personal and educational use.

## Contributing

Feel free to submit issues, feature requests, or pull requests to improve this project.