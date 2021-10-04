#include <ArduinoJson.h>
#include <AFMotor.h>


#define SENSORPIN1 A0 // Left
#define SENSORPIN2 A1 // Right
#define NUMREADINGS 3 // Number of readings to average

int maxFloor = 100; // Likely not needed
int minTape = 400; // Determined experimentally
int motorSpeed = 100;
int turnSpeed = 50;

AF_DCMotor motorRight(1);
AF_DCMotor motorLeft(2);

void driveForward() {
    motorRight.setSpeed(motorSpeed);
    motorLeft.setSpeed(motorSpeed);
    motorRight.run(FORWARD);
    motorLeft.run(FORWARD);
    motorRight.run(RELEASE);
    motorLeft.run(RELEASE);
}

void setup() {
    Serial.begin(9600);
    driveForward();
}

int getReadings(int numReadings, int sensorPin) {
    int senseTotal = 0;
    for (int i=0; i < numReadings; i++) {
        senseTotal+= analogRead(sensorPin);
    }
    return int(senseTotal/numReadings);
}

bool isOffTape(int sensorPin) {
    int sensorRead = getReadings(NUMREADINGS, sensorPin);
    return sensorRead <= minTape;
}

void turnLeft() {
    motorRight.setSpeed(turnSpeed);
    motorLeft.setSpeed(turnSpeed);
    motorRight.run(FORWARD);
    motorLeft.run(BACKWARD);
    motorRight.run(RELEASE);
    motorLeft.run(RELEASE);
}

void turnRight() {
    motorRight.setSpeed(turnSpeed);
    motorLeft.setSpeed(turnSpeed);
    motorRight.run(BACKWARD);
    motorLeft.run(FORWARD);
    motorRight.run(RELEASE);
    motorLeft.run(RELEASE);
}

void loop() {;
    if (isOffTape(SENSORPIN1)) {
        driveForward();
    }
    else {
        turnLeft();
    }
    if (isOffTape(SENSORPIN2)) {
        driveForward();
    }
    else {
        turnRight();
    }
}