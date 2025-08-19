#include <TFT_eSPI.h>
#include <Adafruit_SHTC3.h>

Adafruit_SHTC3 shtc3 = Adafruit_SHTC3();
TFT_eSPI tft = TFT_eSPI();
bool sensorFound = false;

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
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  
  tft.setTextSize(1);
  tft.drawString("Climate Monitor", 5, 5);
  tft.drawString("Initializing...", 5, 25);
  
  // Initialize I2C and SHTC3 sensor
  Wire.begin(21, 22);
  
  if (shtc3.begin()) {
    Serial.println("SHTC3 sensor initialized");
    sensorFound = true;
    
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN);
    tft.drawString("SHTC3 Ready!", 5, 10);
    delay(1000);
  } else {
    Serial.println("SHTC3 sensor not found");
    sensorFound = false;
    
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_RED);
    tft.drawString("SHTC3 Not Found!", 5, 10);
    while(1) delay(1000);
  }
}

void loop() {
  float temperature, humidity;
  
  if (readSHTC3Data(&temperature, &humidity)) {
    Serial.print("Temperature: ");
    Serial.print(temperature, 1);
    Serial.print("°C, Humidity: ");
    Serial.print(humidity, 1);
    Serial.println("%");
    
    tft.fillScreen(TFT_BLACK);
    
    tft.setTextSize(1);
    tft.setTextColor(TFT_CYAN);
    tft.drawString("Climate Monitor", 5, 5);
    
    tft.setTextSize(2);
    tft.setTextColor(TFT_YELLOW);
    tft.drawString("Temp:", 5, 30);
    tft.drawString(String(temperature, 1) + "C", 5, 50);
    
    tft.setTextColor(TFT_GREEN);
    tft.drawString("Humidity:", 5, 80);
    tft.drawString(String(humidity, 1) + "%", 5, 100);
  } else {
    Serial.println("Failed to read SHTC3 sensor");
    
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(1);
    tft.setTextColor(TFT_RED);
    tft.drawString("Sensor Error", 5, 5);
  }
  
  delay(2000);
}