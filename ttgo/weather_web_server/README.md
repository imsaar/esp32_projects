# Shelby Weather Station

A modern ESP32-based weather monitoring system with real-time temperature and humidity tracking, featuring a responsive web interface and 12-hour historical data visualization.

## Features

### 🌡️ **Real-Time Monitoring**
- Temperature readings in both Fahrenheit and Celsius
- Humidity percentage monitoring
- Automatic data collection every 15 minutes
- DHT22 sensor for accurate measurements

### 📊 **Data Visualization**
- Interactive 12-hour historical chart
- Dual-axis display (temperature & humidity)
- Real-time graph updates
- Responsive Chart.js implementation

### ⏰ **Smart Time Management**
- NTP time synchronization
- Automatic daylight saving time handling
- Seattle/Pacific timezone display
- Accurate historical timestamps

### 🎨 **Modern Web Interface**
- Responsive design for mobile and desktop
- Glass morphism UI effects
- Professional weather station aesthetics
- Full-width chart visualization

### 🔧 **Hardware Controls**
- LED control interface
- Real-time status updates
- Touch-friendly mobile controls

## Hardware Requirements

### Components
- **ESP32 Development Board** (TTGO or similar)
- **DHT22 Temperature/Humidity Sensor**
- **2x LEDs** (optional, for status indication)
- **Resistors** (220Ω for LEDs)
- **Breadboard and jumper wires**

### Pin Connections
```
DHT22 Sensor:
- VCC → 3.3V
- GND → GND  
- DATA → GPIO 17

LEDs (Optional):
- LED 1 → GPIO 12 (with 220Ω resistor)
- LED 2 → GPIO 13 (with 220Ω resistor)
```

## Software Requirements

### Arduino IDE Libraries
Install these libraries through the Arduino IDE Library Manager:

1. **DHT22** - For temperature/humidity sensor
2. **NTPClient** - For network time synchronization
3. **WiFiUdp** - For UDP communication (usually included)

### Installation Steps

1. **Clone or download** this project
2. **Open** `web_server.ino` in Arduino IDE
3. **Update WiFi credentials** in the code:
   ```cpp
   #define WIFI_SSID "YourWiFiName"
   #define WIFI_PASSWORD "YourWiFiPassword"
   ```
4. **Install required libraries** (see above)
5. **Select your ESP32 board** in Tools → Board
6. **Upload** the code to your ESP32

## Configuration

### WiFi Setup
Update these lines in the code with your network details:
```cpp
#define WIFI_SSID "YourWiFiSSID"        // Replace with your WiFi name
#define WIFI_PASSWORD "YourWiFiPassword"  // Replace with your WiFi password
#define WIFI_CHANNEL 6              // Optional: specify WiFi channel
```

### Sensor Configuration
The DHT22 sensor is configured on **GPIO 17**. To change this:
```cpp
const int DHT_PIN = 17;  // Change to your preferred pin
```

### Time Zone
Currently set to Seattle/Pacific Time with automatic DST. To change timezone, modify:
```cpp
// In isDST() and getSeattleTime() functions
int offsetHours = isDST(year, month, day) ? -7 : -8;  // Adjust for your timezone
```

## Usage

1. **Power on** your ESP32
2. **Wait for WiFi connection** (check Serial Monitor)
3. **Find the IP address** in Serial Monitor output
4. **Open web browser** and navigate to the ESP32's IP address
5. **View real-time data** and historical charts
6. **Control LEDs** using the web interface

### Web Interface Features

- **Current Conditions**: Real-time temperature and humidity
- **Historical Chart**: 12 hours of data with 15-minute intervals
- **LED Controls**: Toggle LEDs on/off
- **Last Reading Time**: Shows when data was last collected
- **Responsive Design**: Works on phones, tablets, and computers

## API Endpoints

The system provides JSON data endpoints:

- `GET /` - Main web interface
- `GET /data` - Historical data in JSON format
- `GET /toggle/1` - Toggle LED 1
- `GET /toggle/2` - Toggle LED 2

### JSON Data Format
```json
{
  "labels": ["14:30", "14:45", "15:00", ...],
  "temperature": [72.5, 73.1, 72.8, ...],
  "humidity": [45.2, 44.8, 46.1, ...]
}
```

## Technical Specifications

### Data Collection
- **Frequency**: Every 15 minutes
- **Storage**: 48 readings (12 hours)
- **Memory**: Circular buffer for efficient storage
- **Accuracy**: DHT22 sensor specifications

### Web Server
- **Port**: 80 (HTTP)
- **Framework**: ESP32 WebServer library
- **Chart Library**: Chart.js via CDN
- **Responsive**: CSS Grid and Flexbox

### Time Synchronization
- **Protocol**: NTP (Network Time Protocol)
- **Server**: pool.ntp.org
- **Timezone**: Pacific Time (PST/PDT)
- **DST**: Automatic daylight saving time handling

## Troubleshooting

### Common Issues

**WiFi Connection Problems**
- Verify SSID and password
- Check WiFi channel setting
- Ensure ESP32 is in range

**Sensor Not Reading**
- Check DHT22 wiring
- Verify GPIO pin number
- Test sensor with simple sketch

**Time Issues**
- Check internet connection
- Verify NTP server accessibility
- Review timezone calculations

**Web Interface Not Loading**
- Find correct IP address in Serial Monitor
- Check firewall settings
- Try different browser

### Serial Monitor Output
Enable Serial Monitor (115200 baud) to see:
- WiFi connection status
- IP address assignment
- Sensor readings
- NTP synchronization
- Error messages

## Customization

### Styling
Modify CSS in the `sendHtml()` function to change:
- Colors and gradients
- Layout and spacing  
- Typography and fonts
- Chart appearance

### Data Collection
Adjust collection frequency:
```cpp
const unsigned long READING_INTERVAL = 900000; // 15 minutes in milliseconds
```

### Chart Configuration
Modify chart settings in the JavaScript section for:
- Display duration
- Axis scaling
- Color schemes
- Animation effects

## Contributing

Feel free to submit issues and enhancement requests! Areas for improvement:
- Additional sensor support
- Data export features
- Weather alerts/notifications
- Historical data persistence
- Mobile app integration

## License

This project is open source and available under the [MIT License](LICENSE).

## Author

Created for home weather monitoring with modern web technologies and responsive design.

---

**Shelby Weather Station** - Real-time environmental monitoring with ESP32