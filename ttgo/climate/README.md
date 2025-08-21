# LILYGO T-Display SHTC3 Climate Monitor

A simple temperature and humidity monitoring project for the LILYGO T-Display ESP32 board with an SHTC3 sensor.

## Features

- Real-time temperature and humidity readings from SHTC3 sensor
- Dual display modes: textual (vertical) and graphical (horizontal) views
- Button toggle between display modes with immediate response
- Large, easy-to-read textual display optimized for vertical orientation
- Graphical trend view with hourly averages over 24-hour period
- Long-term trend analysis using hourly data averaging
- Intelligent trend triangles showing hourly-based direction indicators
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
   - Search and install "TFT_eSPI" by Bodmer
   - Search and install "Adafruit SHTC3"

### TFT_eSPI Library Configuration:
**IMPORTANT**: After installing TFT_eSPI library, you must configure it for the LILYGO T-Display:

1. Navigate to your Arduino libraries folder:
   - **Windows**: `Documents\Arduino\libraries\TFT_eSPI\`
   - **Mac**: `~/Documents/Arduino/libraries/TFT_eSPI/`
   - **Linux**: `~/Arduino/libraries/TFT_eSPI/`

2. Open the file `User_Setup_Select.h` in a text editor

3. Find this line and comment it out by adding `//` at the beginning:
   ```cpp
   // #include <User_Setup.h>           // Default setup is root library folder
   ```

4. Find this line and uncomment it by removing the `//`:
   ```cpp
   #include <User_Setups/Setup25_TTGO_T_Display.h>    // Setup file for ESP32 and TTGO T-Display ST7789V SPI bus TFT
   ```

5. Save the file and restart Arduino IDE

**Note**: This configuration tells the TFT_eSPI library to use the correct pins and settings for the LILYGO T-Display board.

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

### Graphical View (Horizontal - 240×135 pixels):
```
┌─────────────────────────────────────────────────────────────────────┐
│ Climate Graph  T:23.4C  H:45%  Hourly Avg  Next: 23s              │
│ ┌─────────────────────────────────────────────────────────────────┐ │
│ │            24-Hour Temperature Averages (Yellow)               │ │
│ │     ●───●    ●─●                                               │ │
│ │    /     \  /   \                                              │ │
│ │   ●       \/     ●                                             │ │
│ │ ─────────────────────────────────────────────────────────────── │ │
│ │            24-Hour Humidity Averages (Green)                   │ │
│ │      ●──●      ●─●                                             │ │
│ │     /    \    /   \                                            │ │
│ │    ●      ●──●     ●                                           │ │
│ └─────────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────────┘
```

### Display Features:
- **Dual view modes** - Toggle between textual and graphical displays
- **Right button toggle** - Press right button (GPIO 35) to switch views instantly
- **Textual view** - Large numbers with size 4 font for easy reading on 135×240 display
- **Graphical view** - Line charts showing hourly averages over 24-hour period
- **Auto-orientation** - Vertical for text, horizontal for graphs
- **Hourly averaging** - Calculates and stores true hourly averages over 24-hour period
- **Long-term trends** - Intelligent trend analysis using hourly data instead of recent readings
- **Trend triangles** - Simple up/down triangles with green (increasing) or red (decreasing) colors
- **Stable indicators** - Trend triangles based on hourly averages for more reliable direction indication
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
   - **Textual**: Large numbers in vertical orientation with hourly-based trend triangles
   - **Graphical**: 24-hour trend charts in horizontal orientation showing hourly averages

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
- `drawTextualView()` - Renders large-text vertical display with trend triangles
- `drawGraphicalView()` - Renders 24-hour hourly average charts in horizontal orientation
- `calculateHourlyTrend()` - Analyzes hourly averages for long-term trend direction
- Hourly data management with rolling 24-hour averages
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
- Verify TFT_eSPI library is installed and configured correctly
- Check that `Setup25_TTGO_T_Display.h` is enabled in `User_Setup_Select.h`
- Check T-Display board selection in Arduino IDE
- Ensure proper USB connection

### Display shows garbled text or wrong colors:
- Incorrect TFT_eSPI configuration
- Make sure you've commented out `User_Setup.h` and uncommented `Setup25_TTGO_T_Display.h`
- Restart Arduino IDE after making configuration changes

## Technical Specifications

- **Sensor**: SHTC3 (I2C address 0x70)
- **First Reading**: Immediate on startup
- **Sensor Reading**: 30 seconds after first successful reading
- **Display Update**: 2 seconds for smooth countdown animation
- **Button Response**: Immediate view toggle (50ms polling)
- **History Storage**: 24-hour rolling hourly averages for long-term trend analysis
- **Temperature Range**: -40°C to +80°C display range
- **Humidity Range**: 0% to 100% RH
- **Display**: 135×240 pixel color TFT (updated for current T-Display models)
- **Communication**: I2C at 100kHz

## License

This project is open source and available for personal and educational use.

## Contributing

Feel free to submit issues, feature requests, or pull requests to improve this project.