#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);
  Serial.println("Starting minimal display test...");
  
  // Try different backlight pins
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  delay(100);
  
  tft.init();
  Serial.println("TFT init done");
  
  // Try all rotations
  for(int rot = 0; rot < 4; rot++) {
    tft.setRotation(rot);
    tft.fillScreen(TFT_RED);
    delay(1000);
    tft.fillScreen(TFT_GREEN);
    delay(1000);
    tft.fillScreen(TFT_BLUE);
    delay(1000);
    tft.fillScreen(TFT_BLACK);
    delay(1000);
  }
  
  tft.setRotation(1);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.drawString("Hello", 10, 10);
  
  Serial.println("Test complete");
}

void loop() {
  // Empty
}