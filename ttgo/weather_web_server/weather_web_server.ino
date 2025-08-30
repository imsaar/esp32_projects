/* ESP32 HTTP IoT Server Example for Wokwi.com

  https://wokwi.com/projects/320964045035274834

  To test, you need the Wokwi IoT Gateway, as explained here:

  https://docs.wokwi.com/guides/esp32-wifi#the-private-gateway

  Then start the simulation, and open http://localhost:9080
  in another browser tab.

  Note that the IoT Gateway requires a Wokwi Club subscription.
  To purchase a Wokwi Club subscription, go to https://wokwi.com/club
*/

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <uri/UriBraces.h>
#include <DHT22.h>
#include <time.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

#define WIFI_SSID ""
#define WIFI_PASSWORD ""

#include <home_wifi.h>

// Defining the WiFi channel speeds up the connection:
#define WIFI_CHANNEL 6

WebServer server(80);

const int LED1 = 12;
const int LED2 = 13;
const int DHT_PIN = 17;

bool led1State = false;
bool led2State = false;

DHT22 dht22(DHT_PIN);

// NTP Time setup
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000); // UTC time, we'll handle timezone conversion manually

// Historical data storage (12 hours of data with 15min intervals)
const int MAX_READINGS = 48; // 12 hours * 4 readings per hour
float temperatureHistory[MAX_READINGS];
float humidityHistory[MAX_READINGS];
unsigned long timeHistory[MAX_READINGS];
int currentIndex = 0;
bool dataFull = false;
unsigned long lastReadingTime = 0;
const unsigned long READING_INTERVAL = 900000; // 15 minutes in milliseconds

float getTemperatureCelsius() {
  float temp = dht22.getTemperature();
  if (isnan(temp)) {
    return -999; // Error value
  }
  return temp;
}

float getTemperatureFahrenheit() {
  float tempC = getTemperatureCelsius();
  if (tempC == -999) {
    return -999; // Error value
  }
  return (tempC * 9.0 / 5.0) + 32.0;
}

float getHumidity() {
  float humidity = dht22.getHumidity();
  if (isnan(humidity)) {
    return -999; // Error value
  }
  return humidity;
}

bool isDST(int year, int month, int day) {
  // Daylight Saving Time in US: Second Sunday in March to First Sunday in November
  
  if (month < 3 || month > 11) return false; // January, February, December
  if (month > 3 && month < 11) return true;  // April through October
  
  // Calculate the second Sunday of March and first Sunday of November
  int secondSundayMarch = 14 - ((1 + year * 5 / 4) % 7);
  int firstSundayNov = 7 - ((1 + year * 5 / 4) % 7);
  
  if (month == 3) {
    return day >= secondSundayMarch;
  } else { // month == 11
    return day < firstSundayNov;
  }
}

unsigned long getUSPacificTime() {
  unsigned long utcTime = timeClient.getEpochTime();
  
  // Get current date components to check for DST
  time_t rawtime = utcTime;
  struct tm * timeinfo = gmtime(&rawtime);
  
  int year = timeinfo->tm_year + 1900;
  int month = timeinfo->tm_mon + 1;
  int day = timeinfo->tm_mday;
  
  // Pacific Standard Time is UTC-8, Pacific Daylight Time is UTC-7
  int offsetHours = isDST(year, month, day) ? -7 : -8;
  
  return utcTime + (offsetHours * 3600);
}

String getPacificTimeString(unsigned long epochTime) {
  // Convert epoch time to US Pacific Time string with DST handling
  time_t rawtime = epochTime;
  struct tm * timeinfo = gmtime(&rawtime);
  
  char timeStr[20];
  strftime(timeStr, sizeof(timeStr), "%H:%M", timeinfo);
  return String(timeStr);
}

String getCurrentPacificTimeString() {
  unsigned long seattleTime = getUSPacificTime();
  return getPacificTimeString(seattleTime);
}

String getLastReadingTime() {
  if (currentIndex == 0 && !dataFull) {
    return "No data";
  }
  
  int lastIndex = currentIndex == 0 ? MAX_READINGS - 1 : currentIndex - 1;
  unsigned long lastTime = timeHistory[lastIndex];
  
  if (lastTime == 0) {
    return "No data";
  }
  
  return getPacificTimeString(lastTime);
}

void collectData() {
  unsigned long currentTime = millis();
  
  if (currentTime - lastReadingTime >= READING_INTERVAL || lastReadingTime == 0) {
    timeClient.update(); // Update NTP time
    
    float tempF = getTemperatureFahrenheit();
    float humidity = getHumidity();
    
    if (tempF != -999 && humidity != -999) {
      temperatureHistory[currentIndex] = tempF;
      humidityHistory[currentIndex] = humidity;
      timeHistory[currentIndex] = getUSPacificTime(); // Store Pacific time with DST
      
      currentIndex = (currentIndex + 1) % MAX_READINGS;
      if (currentIndex == 0) dataFull = true;
      
      lastReadingTime = currentTime;
    }
  }
}

void sendHtml() {
  String response = R"(
    <!DOCTYPE html><html>
      <head>
        <title>Shelby Weather Station</title>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
        <style>
          * { box-sizing: border-box; }
          html { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; }
          body { 
            margin: 0; 
            padding: 1rem; 
            background-color: #f5f5f5; 
            min-height: 100vh;
            max-width: 1200px;
            margin: 0 auto;
          }
          .container { 
            display: flex; 
            flex-direction: column; 
            gap: 1.5rem; 
            width: 100%;
          }
          h1 { 
            text-align: center; 
            margin: 0 0 1rem 0; 
            font-size: clamp(1.5rem, 4vw, 2.5rem);
            color: #333;
          }
          h2 { 
            margin: 0 0 1rem 0; 
            font-size: clamp(1.1rem, 3vw, 1.5rem);
            color: #555;
          }
          .sensor-data { 
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 1.5rem 1.5rem 1.2rem; 
            border-radius: 12px; 
            box-shadow: 0 6px 20px rgba(102, 126, 234, 0.25);
            border: 1px solid rgba(255, 255, 255, 0.1);
            position: relative;
            overflow: hidden;
          }
          .sensor-data::before {
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            right: 0;
            height: 1px;
            background: linear-gradient(90deg, transparent, rgba(255,255,255,0.3), transparent);
          }
          .sensor-data h2 { 
            color: white; 
            text-align: center;
            margin-bottom: 1.2rem;
            font-weight: 300;
            letter-spacing: 1px;
            text-transform: uppercase;
            font-size: clamp(0.9rem, 2.5vw, 1.1rem);
          }
          .sensor-readings { 
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(180px, 1fr));
            gap: 1.5rem; 
            margin: 1rem 0;
          }
          .sensor-reading { 
            text-align: center; 
            background: rgba(255, 255, 255, 0.1);
            padding: 1.2rem 0.8rem;
            border-radius: 10px;
            backdrop-filter: blur(10px);
            border: 1px solid rgba(255, 255, 255, 0.2);
            transition: transform 0.3s ease, box-shadow 0.3s ease;
          }
          .sensor-reading:hover {
            transform: translateY(-2px);
            box-shadow: 0 8px 25px rgba(0, 0, 0, 0.15);
          }
          .sensor-reading > div {
            font-size: 0.8rem;
            opacity: 0.8;
            margin-bottom: 0.6rem;
            text-transform: uppercase;
            letter-spacing: 0.5px;
            font-weight: 500;
          }
          .sensor-value { 
            font-size: clamp(1.1rem, 4vw, 1.8rem); 
            font-weight: 700; 
            display: block;
            margin-top: 0.3rem;
            text-shadow: 0 2px 4px rgba(0, 0, 0, 0.3);
            line-height: 1.2;
          }
          .temperature-dual {
            display: flex;
            flex-direction: column;
            gap: 0.2rem;
          }
          .temp-primary {
            font-size: clamp(1.2rem, 4vw, 2rem);
            font-weight: 800;
          }
          .temp-secondary {
            font-size: clamp(0.8rem, 2.5vw, 1rem);
            opacity: 0.8;
            font-weight: 500;
          }
          .chart-container { 
            background-color: #fff; 
            padding: 1.5rem 0.5rem; 
            border-radius: 12px; 
            box-shadow: 0 4px 6px rgba(0,0,0,0.1); 
            width: 100%;
            margin: 0 -1rem;
          }
          .chart-wrapper {
            position: relative;
            height: 300px;
            width: 100%;
          }
          #sensorChart { 
            width: 100% !important; 
            height: 300px !important; 
            max-height: 300px !important; 
          }
          .controls-container { 
            background-color: #fff; 
            padding: 1.5rem; 
            border-radius: 12px; 
            box-shadow: 0 4px 6px rgba(0,0,0,0.1);
          }
          .controls-grid { 
            display: grid; 
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); 
            gap: 1rem; 
            align-items: center;
          }
          .control-item {
            display: flex;
            flex-direction: column;
            align-items: center;
            gap: 0.5rem;
          }
          .btn { 
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            border: none; 
            color: #fff; 
            padding: 0.8rem 1.5rem;
            font-size: clamp(1rem, 3vw, 1.2rem);
            text-decoration: none;
            border-radius: 8px;
            transition: transform 0.2s, box-shadow 0.2s;
            min-width: 100px;
            text-align: center;
          }
          .btn:hover {
            transform: translateY(-2px);
            box-shadow: 0 4px 8px rgba(0,0,0,0.2);
          }
          .btn.OFF { 
            background: linear-gradient(135deg, #333 0%, #555 100%);
          }
          .last-reading {
            text-align: center;
            margin-top: 1.2rem;
            opacity: 0.8;
            font-size: 0.75rem;
            padding: 0.6rem 1.2rem;
            background: rgba(255, 255, 255, 0.1);
            border-radius: 16px;
            backdrop-filter: blur(5px);
            border: 1px solid rgba(255, 255, 255, 0.15);
            display: inline-block;
            letter-spacing: 0.5px;
          }
          
          /* Mobile optimizations */
          @media (max-width: 768px) {
            body { padding: 0.5rem; }
            .sensor-readings { 
              flex-direction: column; 
              text-align: center;
            }
            .sensor-reading { 
              min-width: 100%; 
              margin-bottom: 1rem;
            }
            .controls-grid { 
              grid-template-columns: 1fr; 
              text-align: center;
            }
            .chart-container {
              margin: 0 -0.5rem;
              padding: 1rem 0.25rem;
            }
            .chart-wrapper {
              height: 250px;
            }
            #sensorChart { 
              height: 250px !important; 
              max-height: 250px !important; 
            }
          }
          
          /* Tablet optimizations */
          @media (min-width: 769px) and (max-width: 1024px) {
            .controls-grid { 
              grid-template-columns: repeat(2, 1fr); 
            }
          }
        </style>
      </head>
            
      <body>
        <div class="container">
          <h1>Shelby Weather Station</h1>

          <div class="sensor-data">
            <h2>Current Weather Conditions</h2>
            <div class="sensor-readings">
              <div class="sensor-reading">
                <div>Temperature</div>
                <div class="sensor-value temperature-dual">
                  <span class="temp-primary">TEMP_F_VALUE&deg;F</span>
                  <span class="temp-secondary">TEMP_C_VALUE&deg;C</span>
                </div>
              </div>
              <div class="sensor-reading">
                <div>Humidity</div>
                <span class="sensor-value">HUMIDITY_VALUE%</span>
              </div>
            </div>
            <div class="last-reading">Last reading: LAST_READING_TIME US Pacific time</div>
          </div>

          <div class="chart-container">
            <h2>12-Hour History (15min intervals)</h2>
            <div class="chart-wrapper">
              <canvas id="sensorChart"></canvas>
            </div>
          </div>

          <div class="controls-container">
            <h2>LED Controls</h2>
            <div class="controls-grid">
              <div class="control-item">
                <label>LED 1</label>
                <a href="/toggle/1" class="btn LED1_TEXT">LED1_TEXT</a>
              </div>
              <div class="control-item">
                <label>LED 2</label>
                <a href="/toggle/2" class="btn LED2_TEXT">LED2_TEXT</a>
              </div>
            </div>
          </div>
        </div>

        <script>
        let myChart = null;
        
        async function loadChart() {
          try {
            const response = await fetch('/data');
            const data = await response.json();
            
            const ctx = document.getElementById('sensorChart').getContext('2d');
            
            // Destroy existing chart if it exists
            if (myChart) {
              myChart.destroy();
            }
            
            myChart = new Chart(ctx, {
              type: 'line',
              data: {
                labels: data.labels,
                datasets: [{
                  label: 'Temperature (F)',
                  data: data.temperature,
                  borderColor: 'rgb(255, 99, 132)',
                  backgroundColor: 'rgba(255, 99, 132, 0.1)',
                  yAxisID: 'y'
                }, {
                  label: 'Humidity (%)',
                  data: data.humidity,
                  borderColor: 'rgb(54, 162, 235)',
                  backgroundColor: 'rgba(54, 162, 235, 0.1)',
                  yAxisID: 'y1'
                }]
              },
              options: {
                responsive: true,
                maintainAspectRatio: false,
                plugins: {
                  legend: {
                    position: 'top',
                  }
                },
                interaction: {
                  mode: 'index',
                  intersect: false,
                },
                scales: {
                  x: {
                    display: true,
                    title: {
                      display: true,
                      text: 'Time'
                    }
                  },
                  y: {
                    type: 'linear',
                    display: true,
                    position: 'left',
                    title: {
                      display: true,
                      text: 'Temperature (F)'
                    },
                    beginAtZero: false,
                    min: function(context) {
                      const values = context.chart.data.datasets[0].data;
                      return Math.min(...values) - 2;
                    },
                    max: function(context) {
                      const values = context.chart.data.datasets[0].data;
                      return Math.max(...values) + 2;
                    }
                  },
                  y1: {
                    type: 'linear',
                    display: true,
                    position: 'right',
                    title: {
                      display: true,
                      text: 'Humidity (%)'
                    },
                    beginAtZero: false,
                    min: 0,
                    max: 100,
                    grid: {
                      drawOnChartArea: false,
                    },
                  }
                }
              }
            });
          } catch (error) {
            console.error('Error loading chart:', error);
            document.getElementById('sensorChart').style.display = 'none';
          }
        }
        
        // Load chart when page loads
        window.addEventListener('load', loadChart);
        
        // Refresh chart every 5 minutes
        setInterval(loadChart, 300000);
        </script>
      </body>
    </html>
  )";
  float temperatureC = getTemperatureCelsius();
  float temperatureF = getTemperatureFahrenheit();
  float humidity = getHumidity();
  
  response.replace("LED1_TEXT", led1State ? "ON" : "OFF");
  response.replace("LED2_TEXT", led2State ? "ON" : "OFF");
  
  if (temperatureC == -999) {
    response.replace("TEMP_C_VALUE", "Error");
    response.replace("TEMP_F_VALUE", "Error");
  } else {
    response.replace("TEMP_C_VALUE", String(temperatureC, 1));
    response.replace("TEMP_F_VALUE", String(temperatureF, 1));
  }
  
  if (humidity == -999) {
    response.replace("HUMIDITY_VALUE", "Error");
  } else {
    response.replace("HUMIDITY_VALUE", String(humidity, 1));
  }
  
  String lastReading = getLastReadingTime();
  response.replace("LAST_READING_TIME", lastReading);
  
  server.send(200, "text/html", response);
}

void sendGraphData() {
  String json = "{\"labels\":[";
  String tempData = "\"temperature\":[";
  String humidData = "\"humidity\":[";
  
  int dataCount = dataFull ? MAX_READINGS : currentIndex;
  int startIndex = dataFull ? currentIndex : 0;
  
  for (int i = 0; i < dataCount; i++) {
    int index = (startIndex + i) % MAX_READINGS;
    
    // Use actual Pacific Time for labels
    String timeLabel = getPacificTimeString(timeHistory[index]);
    
    json += "\"" + timeLabel + "\"";
    tempData += String(temperatureHistory[index], 1);
    humidData += String(humidityHistory[index], 1);
    
    if (i < dataCount - 1) {
      json += ",";
      tempData += ",";
      humidData += ",";
    }
  }
  
  json += "]," + tempData + "]," + humidData + "]}";
  
  server.send(200, "application/json", json);
}

void setup(void) {
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
  Serial.print("Connecting to WiFi ");
  Serial.print(WIFI_SSID);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println(" Connected!");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Initialize NTP client
  timeClient.begin();
  timeClient.update();
  
  Serial.println("NTP time synchronized");

  server.on("/", sendHtml);
  server.on("/data", sendGraphData);

  server.on(UriBraces("/toggle/{}"), []() {
    String led = server.pathArg(0);
    Serial.print("Toggle LED #");
    Serial.println(led);

    switch (led.toInt()) {
      case 1:
        led1State = !led1State;
        digitalWrite(LED1, led1State);
        break;
      case 2:
        led2State = !led2State;
        digitalWrite(LED2, led2State);
        break;
    }

    sendHtml();
  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  collectData();
  delay(2);
}

