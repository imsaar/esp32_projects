#include <TFT_eSPI.h>
#include <ESP32Servo.h>

TFT_eSPI tft = TFT_eSPI();
Servo myServo;

const int trigPin = 2;
const int echoPin = 15;
const int servoPin = 27;

volatile float sharedDistance = -1;
portMUX_TYPE distMux = portMUX_INITIALIZER_UNLOCKED;

// Runs on Core 0 — completely separate from servo/display on Core 1
void distanceTask(void* parameter) {
  while (true) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH, 30000);
    float dist = (duration > 0) ? duration * 0.034 / 2 : -1;

    portENTER_CRITICAL(&distMux);
    sharedDistance = dist;
    portEXIT_CRITICAL(&distMux);

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void updateDisplay(float dist) {
  tft.fillRect(0, 80, 240, 60, TFT_BLACK);
  tft.setTextSize(3);
  if (dist > 0 && dist < 400) {
    tft.setTextColor(TFT_CYAN);
    tft.drawString(String(dist, 1) + " cm", 20, 90);
  } else {
    tft.setTextColor(TFT_RED);
    tft.drawString("Out of Range", 10, 90);
  }
}

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_RED);   delay(1000);
  tft.fillScreen(TFT_GREEN); delay(1000);
  tft.fillScreen(TFT_BLUE);  delay(1000);
  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.drawString("Ultrasonic Sensor", 10, 10);
  tft.drawString("Distance Meter", 10, 35);
  tft.drawLine(0, 65, 240, 65, TFT_WHITE);

  xTaskCreatePinnedToCore(distanceTask, "distanceTask", 2048, NULL, 1, NULL, 0);

  myServo.attach(servoPin);
  myServo.write(90);
  delay(500);
}

void loop() {
  for (int pos = 0; pos <= 180; pos++) {
    myServo.write(pos);
    if (pos % 10 == 0) {
      float dist;
      portENTER_CRITICAL(&distMux);
      dist = sharedDistance;
      portEXIT_CRITICAL(&distMux);
      updateDisplay(dist);
    }
    delay(10);
  }

  for (int pos = 180; pos >= 0; pos--) {
    myServo.write(pos);
    if (pos % 10 == 0) {
      float dist;
      portENTER_CRITICAL(&distMux);
      dist = sharedDistance;
      portEXIT_CRITICAL(&distMux);
      updateDisplay(dist);
    }
    delay(10);
  }
}
