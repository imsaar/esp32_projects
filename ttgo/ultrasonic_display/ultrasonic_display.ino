#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

const int trigPin = 2;
const int echoPin = 15;

long duration;
float distance;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting T-Display test...");
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  Serial.println("Backlight enabled");
  
  tft.init();
  Serial.println("TFT initialized");
  
  tft.setRotation(1);
  tft.fillScreen(TFT_RED);
  delay(1000);
  tft.fillScreen(TFT_GREEN);
  delay(1000);
  tft.fillScreen(TFT_BLUE);
  delay(1000);
  tft.fillScreen(TFT_BLACK);
  
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  
  tft.drawString("Ultrasonic Sensor", 10, 10);
  tft.drawString("Distance Meter", 10, 35);
  tft.drawLine(0, 65, 240, 65, TFT_WHITE);
  
  Serial.println("Display setup complete");
}

void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  
  tft.fillRect(0, 80, 240, 60, TFT_BLACK);
  
  tft.setTextSize(3);
  tft.setTextColor(TFT_CYAN);
  
  if (distance < 400) {
    tft.drawString(String(distance, 1) + " cm", 20, 90);
  } else {
    tft.setTextColor(TFT_RED);
    tft.drawString("Out of Range", 10, 90);
  }
  
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  
  delay(100);
}