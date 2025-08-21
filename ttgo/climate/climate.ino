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

// History for graphical view and trend calculation (last 10 readings)
float tempHistory[10] = {0};
float humHistory[10] = {0};
int historyIndex = 0;
int historyCount = 0;

// Hourly averages (last 24 hours)
float hourlyTempAvg[24] = {0};
float hourlyHumAvg[24] = {0};
int hourlyReadingCount[24] = {0};
int currentHourIndex = 0;
unsigned long lastHourUpdate = 0;

// Calculate trend direction and magnitude
struct TrendInfo {
  float slope;
  bool isIncreasing;
  float magnitude; // 0-1 scale for arrow angle
};

TrendInfo calculateTrend(float history[], int count) {
  TrendInfo trend = {0.0, true, 0.0};
  
  if (count < 2) return trend;
  
  // Simple linear regression for trend
  float sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
  for (int i = 0; i < count; i++) {
    sumX += i;
    sumY += history[i];
    sumXY += i * history[i];
    sumX2 += i * i;
  }
  
  trend.slope = (count * sumXY - sumX * sumY) / (count * sumX2 - sumX * sumX);
  trend.isIncreasing = trend.slope > 0;
  
  // Calculate magnitude (0-1) based on slope
  trend.magnitude = min(1.0f, abs(trend.slope) / 2.0f); // Scale factor for visibility
  
  return trend;
}

TrendInfo calculateHourlyTrend(float hourlyAvg[], int hourlyCount[], bool isTemperature) {
  TrendInfo trend = {0.0, true, 0.0};
  
  // Collect valid hourly data points
  float validData[24];
  int validCount = 0;
  
  for (int i = 0; i < 24; i++) {
    if (hourlyCount[i] > 0) {
      validData[validCount] = hourlyAvg[i];
      validCount++;
    }
  }
  
  if (validCount < 2) return trend;
  
  // Simple linear regression for hourly trend
  float sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
  for (int i = 0; i < validCount; i++) {
    sumX += i;
    sumY += validData[i];
    sumXY += i * validData[i];
    sumX2 += i * i;
  }
  
  trend.slope = (validCount * sumXY - sumX * sumY) / (validCount * sumX2 - sumX * sumX);
  trend.isIncreasing = trend.slope > 0;
  
  // Calculate magnitude (0-1) based on slope - scale differently for temp vs humidity
  float scaleFactor = isTemperature ? 1.0f : 0.1f; // Humidity changes are typically smaller
  trend.magnitude = min(1.0f, abs(trend.slope) / scaleFactor);
  
  return trend;
}

void drawTrendTriangle(int x, int y, TrendInfo trend, uint16_t color) {
  if (historyCount < 2) return; // Need at least 2 points for trend
  
  int triangleSize = 8; // Size of the triangle
  
  if (trend.isIncreasing) {
    // Draw upward pointing triangle (green)
    // Top point
    int topX = x;
    int topY = y - triangleSize/2;
    // Bottom left
    int leftX = x - triangleSize/2;
    int leftY = y + triangleSize/2;
    // Bottom right
    int rightX = x + triangleSize/2;
    int rightY = y + triangleSize/2;
    
    // Draw filled triangle
    for (int row = 0; row <= triangleSize; row++) {
      int currentY = topY + row;
      int width = (row * triangleSize) / triangleSize;
      int startX = topX - width/2;
      int endX = topX + width/2;
      tft.drawLine(startX, currentY, endX, currentY, color);
    }
  } else {
    // Draw downward pointing triangle (red)
    // Bottom point
    int bottomX = x;
    int bottomY = y + triangleSize/2;
    // Top left
    int leftX = x - triangleSize/2;
    int leftY = y - triangleSize/2;
    // Top right
    int rightX = x + triangleSize/2;
    int rightY = y - triangleSize/2;
    
    // Draw filled triangle
    for (int row = 0; row <= triangleSize; row++) {
      int currentY = leftY + row;
      int width = triangleSize - (row * triangleSize) / triangleSize;
      int startX = bottomX - width/2;
      int endX = bottomX + width/2;
      tft.drawLine(startX, currentY, endX, currentY, color);
    }
  }
}

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
  tft.setRotation(1);  // Horizontal orientation (240x135)
  tft.fillScreen(TFT_BLACK);
  
  // Header
  tft.setTextSize(1);
  tft.setTextColor(TFT_CYAN);
  tft.drawString("Climate Graph", 85, 5);
  
  // Current values and hourly average display
  tft.setTextColor(TFT_YELLOW);
  tft.drawString("T:" + String(temperature, 1) + "C", 5, 20);
  tft.setTextColor(TFT_GREEN);
  tft.drawString("H:" + String(humidity, 0) + "%", 70, 20);
  tft.setTextColor(TFT_CYAN);
  tft.drawString("Hourly Avg", 130, 20);
  tft.setTextColor(TFT_MAGENTA);
  tft.drawString("Next: " + String(countdown) + "s", 200, 20);
  
  // Graph area: maximized for 240x135 screen
  int graphX = 2;
  int graphY = 30;  // Closer to text
  int graphWidth = 236;  // Almost full width (240-4 for minimal margins)
  int graphHeight = 103;  // Fill remaining height (135-30-2 for bottom margin)
  
  // Draw graph border
  tft.drawRect(graphX, graphY, graphWidth, graphHeight, TFT_WHITE);
  
  // Count valid hourly data points
  int validHours = 0;
  for (int i = 0; i < 24; i++) {
    if (hourlyReadingCount[i] > 0) validHours++;
  }
  
  // Draw hourly temperature averages (top half)
  if (validHours > 1) {
    // Find min/max for scaling from hourly data
    float minTemp = 100, maxTemp = -100;
    for (int i = 0; i < 24; i++) {
      if (hourlyReadingCount[i] > 0) {
        if (hourlyTempAvg[i] < minTemp) minTemp = hourlyTempAvg[i];
        if (hourlyTempAvg[i] > maxTemp) maxTemp = hourlyTempAvg[i];
      }
    }
    
    // Add some padding to range
    float tempRange = max(5.0f, maxTemp - minTemp + 2.0f);
    minTemp -= 1;
    
    // Draw lines connecting hourly averages
    int lastValidX = -1, lastValidY = -1;
    for (int i = 0; i < 24; i++) {
      if (hourlyReadingCount[i] > 0) {
        int x = graphX + 2 + (i * (graphWidth-4)) / 23;
        int y = graphY + 2 + (graphHeight/2 - 4) - ((hourlyTempAvg[i] - minTemp) * (graphHeight/2 - 4)) / tempRange;
        
        if (lastValidX >= 0) {
          tft.drawLine(lastValidX, lastValidY, x, y, TFT_YELLOW);
        }
        // Draw data point
        tft.fillCircle(x, y, 1, TFT_YELLOW);
        
        lastValidX = x;
        lastValidY = y;
      }
    }
  }
  
  // Draw hourly humidity averages (bottom half)
  if (validHours > 1) {
    int lastValidX = -1, lastValidY = -1;
    for (int i = 0; i < 24; i++) {
      if (hourlyReadingCount[i] > 0) {
        int x = graphX + 2 + (i * (graphWidth-4)) / 23;
        int y = graphY + graphHeight/2 + 2 + (graphHeight/2 - 4) - (hourlyHumAvg[i] * (graphHeight/2 - 4)) / 100;
        
        if (lastValidX >= 0) {
          tft.drawLine(lastValidX, lastValidY, x, y, TFT_GREEN);
        }
        // Draw data point
        tft.fillCircle(x, y, 1, TFT_GREEN);
        
        lastValidX = x;
        lastValidY = y;
      }
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
  
  // Temperature trend triangle (based on hourly averages)
  TrendInfo tempTrend = calculateHourlyTrend(hourlyTempAvg, hourlyReadingCount, true);
  if (tempTrend.magnitude > 0) {
    uint16_t tempTrendColor = tempTrend.isIncreasing ? TFT_GREEN : TFT_RED;
    drawTrendTriangle(110, 75, tempTrend, tempTrendColor);
  }
  
  // Humidity section  
  tft.setTextSize(2);
  tft.setTextColor(TFT_GREEN);
  tft.drawString("Humid", 10, 95);
  
  tft.setTextSize(3);
  String humStr = String(humidity, 0);  // No decimal for humidity
  tft.drawString(humStr, 15, 115);
  
  tft.setTextSize(2);
  tft.drawString("%", 85, 125);
  
  // Humidity trend triangle (based on hourly averages)
  TrendInfo humTrend = calculateHourlyTrend(hourlyHumAvg, hourlyReadingCount, false);
  if (humTrend.magnitude > 0) {
    uint16_t humTrendColor = humTrend.isIncreasing ? TFT_GREEN : TFT_RED;
    drawTrendTriangle(110, 145, humTrend, humTrendColor);
  }
  
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
  tft.setRotation(0);  // Vertical orientation (135x240)
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  
  tft.setTextSize(2);
  tft.drawString("Climate", 10, 10);
  tft.drawString("Monitor", 10, 35);
  tft.setTextSize(1);
  tft.drawString("Initializing...", 10, 70);
  
  // Initialize I2C and SHTC3 sensor
  Wire.begin(21, 22);
  
  // Clear screen for new dimensions
  tft.fillScreen(TFT_BLACK);
  
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
      
      // Update hourly averages (every hour = 3600000ms)
      if (currentTime - lastHourUpdate >= 3600000 || lastHourUpdate == 0) {
        if (lastHourUpdate != 0) {
          currentHourIndex = (currentHourIndex + 1) % 24;
          hourlyReadingCount[currentHourIndex] = 0;
          hourlyTempAvg[currentHourIndex] = 0;
          hourlyHumAvg[currentHourIndex] = 0;
        }
        lastHourUpdate = currentTime;
      }
      
      // Add current reading to hourly average
      if (hourlyReadingCount[currentHourIndex] == 0) {
        hourlyTempAvg[currentHourIndex] = temperature;
        hourlyHumAvg[currentHourIndex] = humidity;
      } else {
        hourlyTempAvg[currentHourIndex] = (hourlyTempAvg[currentHourIndex] * hourlyReadingCount[currentHourIndex] + temperature) / (hourlyReadingCount[currentHourIndex] + 1);
        hourlyHumAvg[currentHourIndex] = (hourlyHumAvg[currentHourIndex] * hourlyReadingCount[currentHourIndex] + humidity) / (hourlyReadingCount[currentHourIndex] + 1);
      }
      hourlyReadingCount[currentHourIndex]++;
      
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