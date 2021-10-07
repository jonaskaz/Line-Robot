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

int kP = 1;
int kI = 1;
int kD = 1;
int P = 0;
int I = 0;
int D = 0;
int lastError = 0;
float pidSpeed = 0;


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

int getReadings(int sensorPin) {
    int senseTotal = 0;
    for (int i=0; i < NUMREADINGS; i++) {
        senseTotal+= analogRead(sensorPin);
    }
    return int(senseTotal/NUMREADINGS);
}

int getError() {
    return getReadings(SENSORPIN1) - getReadings(SENSORPIN2);
}

void updatePID(int error) {
    P = error;
    I += error;
    D = error-lastError;
    lastError=error;
}

void updateMotorSpeed() {
    pidSpeed = int(kP*P + kI*I + kD*D);
    motorRight->setSpeed(motorSpeed + pidSpeed);
}

void loop() {
    updatePID(getError());
    updateMotorSpeed();
}