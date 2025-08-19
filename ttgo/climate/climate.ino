#include <TFT_eSPI.h>
#include <Adafruit_SHTC3.h>

Adafruit_SHTC3 shtc3 = Adafruit_SHTC3();
TFT_eSPI tft = TFT_eSPI();
bool sensorFound = false;
unsigned long lastReadTime = 0;

bool readSHTC3Data(float* temperature, float* humidity) {
  sensors_event_t humidityEvent, tempEvent;
  
  if (shtc3.getEvent(&humidityEvent, &tempEvent)) {
    *temperature = tempEvent.temperature;
    *humidity = humidityEvent.relative_humidity;
    return true;
  }
  return false;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("LILYGO T-Display Climate Monitor");
  
  tft.init();
  tft.setRotation(0);  // Vertical orientation (128x160)
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  
  tft.setTextSize(2);
  tft.drawString("Climate", 10, 10);
  tft.drawString("Monitor", 10, 35);
  tft.setTextSize(1);
  tft.drawString("Initializing...", 10, 70);
  
  // Initialize I2C and SHTC3 sensor
  Wire.begin(21, 22);
  lastReadTime = millis();  // Record initialization time
  
  if (shtc3.begin()) {
    Serial.println("SHTC3 sensor initialized");
    sensorFound = true;
    
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextColor(TFT_GREEN);
    tft.drawString("SHTC3", 20, 20);
    tft.drawString("Ready!", 20, 45);
    delay(1000);
  } else {
    Serial.println("SHTC3 sensor not found");
    sensorFound = false;
    
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextColor(TFT_RED);
    tft.drawString("SHTC3", 15, 20);
    tft.drawString("Error!", 15, 45);
    while(1) delay(1000);
  }
}

void loop() {
  // Calculate countdown to next refresh (10 second intervals)
  unsigned long currentTime = millis();
  unsigned long timeSinceLastRead = currentTime - lastReadTime;
  int countdown = 10 - (timeSinceLastRead / 1000);
  if (countdown <= 0) countdown = 10;
  
  float temperature, humidity;
  static float lastTemp = 0, lastHum = 0;  // Store last good readings
  
  // Try to read sensor data every 10 seconds
  if (timeSinceLastRead >= 10000) {
    lastReadTime = currentTime;
    
    if (readSHTC3Data(&temperature, &humidity)) {
      lastTemp = temperature;
      lastHum = humidity;
      
      // Arduino Serial Plotter format: Temperature:value,Humidity:value
      Serial.print("Temperature:");
      Serial.print(temperature, 1);
      Serial.print(",Humidity:");
      Serial.println(humidity, 1);
    } else {
      Serial.println("Failed to read SHTC3 sensor - using last values");
      temperature = lastTemp;
      humidity = lastHum;
    }
  } else {
    // Use last readings between updates
    temperature = lastTemp;
    humidity = lastHum;
  }
  
  tft.fillScreen(TFT_BLACK);
  
  // Header
  tft.setTextSize(2);
  tft.setTextColor(TFT_CYAN);
  tft.drawString("Climate", 15, 10);
  
  // Temperature section
  tft.setTextSize(2);
  tft.setTextColor(TFT_YELLOW);
  tft.drawString("Temp", 10, 45);
  
  tft.setTextSize(3);
  String tempStr = String(temperature, 1);
  tft.drawString(tempStr, 15, 65);
  
  tft.setTextSize(2);
  tft.drawString("C", 95, 75);
  
  // Humidity section  
  tft.setTextSize(2);
  tft.setTextColor(TFT_GREEN);
  tft.drawString("Humid", 10, 95);
  
  tft.setTextSize(3);
  String humStr = String(humidity, 0);  // No decimal for humidity
  tft.drawString(humStr, 15, 115);
  
  tft.setTextSize(2);
  tft.drawString("%", 85, 125);
  
  // Countdown to next refresh
  tft.setTextSize(2);
  tft.setTextColor(TFT_MAGENTA);
  tft.drawString("Next:", 10, 140);
  tft.drawString(String(countdown) + "s", 70, 140);
  
  delay(100);  // Faster updates for smooth countdown
}