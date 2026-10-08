#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  Serial.println("TP02 iniciado correctamente");
}

void loop() {
  Serial.println("ESP32 funcionando...");
  delay(1000);
}