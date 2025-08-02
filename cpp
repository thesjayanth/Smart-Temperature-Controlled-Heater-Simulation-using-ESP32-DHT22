#include <Arduino.h>
#include <DHT.h>

#define DHTPIN 15
#define DHTTYPE DHT22
#define HEATER_PIN 18
#define BUZZER_PIN 19
#define TARGET_TEMP 48.0
#define OVERHEAT_TEMP 50.0

DHT dht(DHTPIN, DHTTYPE);

enum SystemState {
  IDLE,
  HEATING,
  STABILIZING,
  TARGET_REACHED,
  OVERHEAT
};

SystemState state = IDLE;

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(HEATER_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
}

void loop() {
  float temp = dht.readTemperature();

  if (isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(1000);
    return;
  }

  if (temp > OVERHEAT_TEMP)
    state = OVERHEAT;
  else if (temp >= TARGET_TEMP)
    state = TARGET_REACHED;
  else if (temp >= TARGET_TEMP - 3)
    state = STABILIZING;
  else
    state = HEATING;

  switch (state) {
    case HEATING:
      digitalWrite(HEATER_PIN, HIGH);
      digitalWrite(BUZZER_PIN, LOW);
      break;
    case OVERHEAT:
      digitalWrite(HEATER_PIN, LOW);
      digitalWrite(BUZZER_PIN, HIGH);
      break;
    default:
      digitalWrite(HEATER_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);
  }

  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.print(" °C | State: ");

  switch (state) {
    case HEATING: Serial.print("HEATING"); break;
    case STABILIZING: Serial.print("STABILIZING"); break;
    case TARGET_REACHED: Serial.print("TARGET REACHED"); break;
    case OVERHEAT: Serial.print("OVERHEAT"); break;
    default: Serial.print("IDLE");
  }

  Serial.print(" | Heater: ");
  Serial.print((state == HEATING) ? "ON" : "OFF");
  Serial.print(" | Buzzer: ");
  Serial.println((state == OVERHEAT) ? "ON" : "OFF");

  delay(1000);
}
