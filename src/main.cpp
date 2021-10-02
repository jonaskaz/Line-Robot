#include <ArduinoJson.h>

int sensorPin = A0;

void setup() {
    Serial.begin(9600);
}

void loop() {
    float r = analogRead(sensorPin);
    Serial.println(r);
}