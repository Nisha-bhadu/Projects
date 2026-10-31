/*
  Smart Home Environment Monitoring System
  Board  : ESP32 DevKit C
  Sensors: DHT22 (temperature + humidity), LDR module (light)
  Output : Serial Monitor, Alert LED, Buzzer
*/

#include "DHT.h"

// ---------- Pin definitions ----------
#define DHTPIN      4
#define DHTTYPE     DHT22
#define LDR_PIN     34
#define LED_PIN     2
#define BUZZER_PIN  25

// ---------- Alert limits (apne hisaab se change karo) ----------
const float TEMP_LIMIT = 35.0;   // degree C
const float HUM_LIMIT  = 70.0;   // percent

// ---------- Light status limits (percent) ----------
const int DARK_LIMIT   = 30;
const int BRIGHT_LIMIT = 70;

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  analogReadResolution(12);   // ADC range 0 - 4095

  Serial.println("====================================");
  Serial.println(" Smart Home Environment Monitor");
  Serial.println("====================================");
  delay(2000);                // sensor ko stable hone do
}

void loop() {
  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();

  int ldrRaw       = analogRead(LDR_PIN);
  int lightPercent = map(ldrRaw, 0, 4095, 0, 100);

  // DHT read check
  if (isnan(temp) || isnan(hum)) {
    Serial.println("DHT22 read failed! Wiring check karo.");
    delay(2000);
    return;
  }

  // ---------- Readings ----------
  Serial.println("------------------------------------");
  Serial.print("Temperature : "); Serial.print(temp, 1); Serial.println(" C");
  Serial.print("Humidity    : "); Serial.print(hum, 1);  Serial.println(" %");
  Serial.print("Light       : "); Serial.print(lightPercent);
  Serial.print(" %  (raw: "); Serial.print(ldrRaw); Serial.println(")");

  // ---------- Light status ----------
  if (lightPercent < DARK_LIMIT) {
    Serial.println("Light Status: DARK");
  } else if (lightPercent < BRIGHT_LIMIT) {
    Serial.println("Light Status: NORMAL");
  } else {
    Serial.println("Light Status: BRIGHT");
  }

  // ---------- Alerts ----------
  bool alert = false;

  if (temp > TEMP_LIMIT) {
    Serial.println("!! ALERT: Temperature zyada hai !!");
    alert = true;
  }
  if (hum > HUM_LIMIT) {
    Serial.println("!! ALERT: Humidity zyada hai !!");
    alert = true;
  }

  if (alert) {
    digitalWrite(LED_PIN, HIGH);
    tone(BUZZER_PIN, 1000, 500);   // 1 kHz beep, 0.5 sec
  } else {
    digitalWrite(LED_PIN, LOW);
    noTone(BUZZER_PIN);
    Serial.println("Status: All OK");
  }

  delay(2000);   // DHT22 ko minimum 2 sec chahiye
}