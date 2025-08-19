#include <TFT_eSPI.h>
#include <Adafruit_SHTC3.h>

Adafruit_SHTC3 shtc3 = Adafruit_SHTC3();
TFT_eSPI tft = TFT_eSPI();
bool sensorFound = false;
unsigned long lastReadTime = 0;
bool showGraphicalView = false;
unsigned long lastButtonPress = 0;
const int BUTTON_PIN = 35;  // Right button on T-Display
bool firstReadingDone = false;  // Track if we've had our first successful reading

// History for graphical view (last 10 readings)
float tempHistory[10] = {0};
float humHistory[10] = {0};
int historyIndex = 0;
int historyCount = 0;

bool readSHTC3Data(float* temperature, float* humidity) {
  sensors_event_t humidityEvent, tempEvent;
  
  if (shtc3.getEvent(&humidityEvent, &tempEvent)) {
    *temperature = tempEvent.temperature;
    *humidity = humidityEvent.relative_humidity;
    return true;
  }
  return false;
}

void drawGraphicalView(float temperature, float humidity, int countdown) {
  // Switch to horizontal orientation for graphical view
  tft.setRotation(1);  // Horizontal orientation (160x128)
  tft.fillScreen(TFT_BLACK);
  
  // Header
  tft.setTextSize(1);
  tft.setTextColor(TFT_CYAN);
  tft.drawString("Climate Graph", 50, 5);
  
  // Current values in small text
  tft.setTextColor(TFT_YELLOW);
  tft.drawString("T:" + String(temperature, 1) + "C", 5, 20);
  tft.setTextColor(TFT_GREEN);
  tft.drawString("H:" + String(humidity, 0) + "%", 80, 20);
  tft.setTextColor(TFT_MAGENTA);
  tft.drawString("Next: " + String(countdown) + "s", 120, 20);
  
  // Graph area: maximized with proper clearance from text
  int graphX = 2;
  int graphY = 32;  // Clear space below text
  int graphWidth = 158;  // Maximum width (160-2 margin on left only)
  int graphHeight = 94;  // Fill remaining height (128-32-2 for bottom margin)
  
  // Draw graph border
  tft.drawRect(graphX, graphY, graphWidth, graphHeight, TFT_WHITE);
  
  // Draw temperature graph (top half)
  if (historyCount > 1) {
    // Find min/max for scaling
    float minTemp = tempHistory[0], maxTemp = tempHistory[0];
    for (int i = 1; i < historyCount; i++) {
      if (tempHistory[i] < minTemp) minTemp = tempHistory[i];
      if (tempHistory[i] > maxTemp) maxTemp = tempHistory[i];
    }
    
    // Add some padding to range
    float tempRange = max(5.0f, maxTemp - minTemp + 2.0f);
    minTemp -= 1;
    
    for (int i = 1; i < historyCount; i++) {
      int x1 = graphX + 2 + ((i-1) * (graphWidth-4)) / 9;
      int y1 = graphY + 2 + (graphHeight/2 - 4) - ((tempHistory[i-1] - minTemp) * (graphHeight/2 - 4)) / tempRange;
      int x2 = graphX + 2 + (i * (graphWidth-4)) / 9;
      int y2 = graphY + 2 + (graphHeight/2 - 4) - ((tempHistory[i] - minTemp) * (graphHeight/2 - 4)) / tempRange;
      tft.drawLine(x1, y1, x2, y2, TFT_YELLOW);
    }
  }
  
  // Draw humidity graph (bottom half)
  if (historyCount > 1) {
    for (int i = 1; i < historyCount; i++) {
      int x1 = graphX + 2 + ((i-1) * (graphWidth-4)) / 9;
      int y1 = graphY + graphHeight/2 + 2 + (graphHeight/2 - 4) - (humHistory[i-1] * (graphHeight/2 - 4)) / 100;
      int x2 = graphX + 2 + (i * (graphWidth-4)) / 9;
      int y2 = graphY + graphHeight/2 + 2 + (graphHeight/2 - 4) - (humHistory[i] * (graphHeight/2 - 4)) / 100;
      tft.drawLine(x1, y1, x2, y2, TFT_GREEN);
    }
  }
  
  // Draw center line
  tft.drawLine(graphX, graphY + graphHeight/2, graphX + graphWidth, graphY + graphHeight/2, TFT_DARKGREY);
  
  // Note: Countdown moved to top status line
}

void drawTextualView(float temperature, float humidity, int countdown) {
  // Switch back to vertical orientation for textual view
  tft.setRotation(0);  // Vertical orientation (128x160)
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
  tft.drawString("Next Refresh", 10, 140);
  
  tft.setTextSize(3);
  tft.drawString(String(countdown), 15, 160);
  
  tft.setTextSize(2);
  tft.drawString("s", 85, 170);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("LILYGO T-Display Climate Monitor");
  
  // Initialize button
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
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
  float temperature, humidity;
  static float lastTemp = 0, lastHum = 0;  // Store last good readings
  
  unsigned long currentTime = millis();
  
  // Handle first reading or 30-second intervals after first reading
  bool shouldRead = false;
  int countdown = 0;
  
  if (!firstReadingDone) {
    // Always try to read on first attempt
    shouldRead = true;
    countdown = 0;
  } else {
    // Calculate countdown after first successful reading
    unsigned long timeSinceLastRead = currentTime - lastReadTime;
    countdown = 30 - (timeSinceLastRead / 1000);
    if (countdown <= 0) countdown = 30;
    
    // Read every 30 seconds after first reading
    if (timeSinceLastRead >= 30000) {
      shouldRead = true;
    }
  }
  
  // Try to read sensor data
  if (shouldRead) {
    if (readSHTC3Data(&temperature, &humidity)) {
      lastTemp = temperature;
      lastHum = humidity;
      
      // Mark first reading as done and start timer
      if (!firstReadingDone) {
        firstReadingDone = true;
        lastReadTime = currentTime;
        Serial.println("First reading completed - starting 30-second timer");
      } else {
        lastReadTime = currentTime;
      }
      
      // Store in history for graphical view
      tempHistory[historyIndex] = temperature;
      humHistory[historyIndex] = humidity;
      historyIndex = (historyIndex + 1) % 10;
      if (historyCount < 10) historyCount++;
      
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
  
  // Main display loop with button checking
  unsigned long displayStart = millis();
  while (millis() - displayStart < 2000) {  // 2-second display cycle
    // Check button press for immediate view toggle
    if (digitalRead(BUTTON_PIN) == LOW && (millis() - lastButtonPress) > 500) {
      showGraphicalView = !showGraphicalView;
      lastButtonPress = millis();
      Serial.println(showGraphicalView ? "Switched to Graphical View" : "Switched to Textual View");
      delay(200);  // Debounce
      
      // Immediately update display with new view
      if (showGraphicalView) {
        drawGraphicalView(temperature, humidity, countdown);
      } else {
        drawTextualView(temperature, humidity, countdown);
      }
    }
    delay(50);  // Short delay for button checking
  }
  
  // Display based on current view mode
  if (showGraphicalView) {
    drawGraphicalView(temperature, humidity, countdown);
  } else {
    drawTextualView(temperature, humidity, countdown);
  }
}