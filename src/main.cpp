#include <ArduinoJson.h>
#include <Adafruit_MotorShield.h>
#include <SPI.h>


#define SENSORPIN1 A0 // Left
#define SENSORPIN2 A1 // Right
#define NUMREADINGS 3 // Number of readings to average

const int maxFloor = 100; // Likely not needed
const int minTape = 400; // Determined experimentally
const int turnAdd = 20;
int motorSpeed = 100;
int turnSpeed = 100;
int integralGain = 1;
int integral = 0;
unsigned long errorTime = -1;
unsigned long lastTime;


Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_DCMotor *motorRight = AFMS.getMotor(1);
Adafruit_DCMotor *motorLeft = AFMS.getMotor(1);


void driveForward() {
    motorRight->setSpeed(motorSpeed);
    motorLeft->setSpeed(motorSpeed);
    motorRight->run(FORWARD);
    motorLeft->run(FORWARD);
    motorRight->run(RELEASE);
    motorLeft->run(RELEASE);
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
    motorRight->setSpeed(turnSpeed+integral+turnAdd);
    motorLeft->setSpeed(turnSpeed);
    motorRight->run(RELEASE);
    motorLeft->run(RELEASE);
}

void turnRight() {
    motorRight->setSpeed(turnSpeed);
    motorLeft->setSpeed(turnSpeed+integral+turnAdd);
    motorRight->run(RELEASE);
    motorLeft->run(RELEASE);
}

void updateIntegral() {
    if (int(errorTime) != -1) {
            errorTime += millis()-lastTime;
            lastTime = millis();
            integral = errorTime * integralGain;
        }
    else {
        errorTime = 0;
    }
}

void resetIntegral() {
    if (isOffTape(SENSORPIN1) && isOffTape(SENSORPIN2)) {
        errorTime = -1;
    }
}

void loop() {
    while (true) {
        resetIntegral();
        if (isOffTape(SENSORPIN1)) {
            driveForward();
        }
        else {
            updateIntegral();
            turnLeft();
            continue;
        }
        if (isOffTape(SENSORPIN2)) {
            driveForward();
        }
        else {
            updateIntegral();
            turnRight();
        }
    }
    
}