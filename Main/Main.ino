#include <Adafruit_LSM303_Accel.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#include "audio.h"   // заголовок, где объявлены audioSetup() и audioPlayOnce()

#include <ESP32Servo.h>

Servo servo;

const int SERVO_PIN = 1; 


Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);
#define SDA_PIN 8
#define SCL_PIN 9

// порог по изменению модуля ускорения, м/с^2
const float ACC_THRESHOLD = 1.5;   // подберёшь опытно
// минимальный интервал между срабатываниями, мс
const unsigned long TRIGGER_DEBOUNCE_MS = 5000;

float lastMag = 0.0;
unsigned long lastTriggerMs = 0;

void displaySensorDetails(void) {
  sensor_t sensor;
  accel.getSensor(&sensor);
  // … как у тебя было …
}

void setup() {
  Serial.begin(115200);
  delay(500);

  Wire.begin(SDA_PIN, SCL_PIN);

  Serial.println("Accelerometer Test");
  Serial.println("");

  if (!accel.begin()) {
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while (1);
  }

  displaySensorDetails();

  accel.setRange(LSM303_RANGE_4G);
  accel.setMode(LSM303_MODE_NORMAL);

  audioSetup();          // инициализация аудио

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  servo.setPeriodHertz(50);          // стандартные 50 Гц для серв
  servo.attach(SERVO_PIN, 500, 2500); // минимальный и максимальный импульс, мкс

  // начальное значение модуля
  sensors_event_t event;
  accel.getEvent(&event);
  lastMag = sqrt(event.acceleration.x * event.acceleration.x +
                 event.acceleration.y * event.acceleration.y +
                 event.acceleration.z * event.acceleration.z);
}

void loop(void) {
  sensors_event_t event;
  accel.getEvent(&event);

  float x = event.acceleration.x;
  float y = event.acceleration.y;
  float z = event.acceleration.z;

  float mag = sqrt(x*x + y*y + z*z);      // модуль ускорения
  float diff = fabs(mag - lastMag);
  lastMag = mag;

  Serial.print("X: "); Serial.print(x);
  Serial.print("  Y: "); Serial.print(y);
  Serial.print("  Z: "); Serial.print(z);
  Serial.print("  |mag|: "); Serial.print(mag);
  Serial.print("  diff: "); Serial.println(diff);

  unsigned long now = millis();
  if (diff > ACC_THRESHOLD && (now - lastTriggerMs) > TRIGGER_DEBOUNCE_MS) {
    Serial.println("SW TRIGGER -> play audio");
    audioPlayOnce(); 
    
  servo.write(0);     // 0°
  delay(100);
  servo.write(180);             // запуск звука
    lastTriggerMs = now;
  }

  delay(50);   // частота опроса акселя
}
