#include <ArduinoJson.h>
#include <Adafruit_MotorShield.h>
#include <SPI.h>


#define SENSORPIN1 A0 // Left
#define SENSORPIN2 A1 // Right
#define SENSORPIN3 A2 // Middle
#define NUMREADINGS 3 // Number of readings to average

float motorSpeed = 30;
float errorOffset = 30;
float middleSensorThresh = 100;
int errorDirection = 1;
float kP = 0.07;
float kI = 0.0000;
float kD = 0.03;

float consts[] = {motorSpeed, errorOffset, kP, kI, kD};

float P = 0;
float I = 0;
float D = 0;
float lastError = 0;
float pidSpeed = 0;


Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_DCMotor *motorRight = AFMS.getMotor(3);
Adafruit_DCMotor *motorLeft = AFMS.getMotor(4);


void driveForward() {
    motorRight->setSpeed(motorSpeed);
    motorLeft->setSpeed(motorSpeed);
    motorRight->run(BACKWARD);
    motorLeft->run(FORWARD);
}

void setup() {
    Serial.begin(9600);
    AFMS.begin();
    driveForward();
}

int getReadings(int sensorPin) {
    int senseTotal = 0;
    for (int i=0; i < NUMREADINGS; i++) {
        senseTotal+= analogRead(sensorPin);
    }
    return int(senseTotal/NUMREADINGS);
}

int getMidSensorReading() {
    int val = getReadings(SENSORPIN3);
    if (val>middleSensorThresh){
        return 0;
    }
    return val;
}

void setErrorDirection(int outsideError) {
    if (outsideError >= errorOffset) {
        errorDirection = 1;
    }
    else if (outsideError <= -1*errorOffset) {
        errorDirection = -1;
    }
}

int getError() {
    int outsideError = getReadings(SENSORPIN1) - getReadings(SENSORPIN2);
    setErrorDirection(outsideError);
    int error = (abs(outsideError) + getMidSensorReading()) * errorDirection;
    if (abs(error)<errorOffset) {
        return 0;
    }
    return error;
}

void updatePID(int error) {
    P = error;
    I += error;
    D = error-lastError;
    lastError=error;
}

void updateMotorSpeed() {
    pidSpeed = kP*P + kI*I + kD*D;
    pidSpeed = constrain(pidSpeed, -200+motorSpeed, 200-motorSpeed);
    motorRight->setSpeed(motorSpeed + pidSpeed);
    motorLeft->setSpeed(motorSpeed - pidSpeed);
}

void setConsts() {
    motorSpeed = consts[0];
    errorOffset = consts[1];
    kP = consts[2];
    kI = consts[3];
    kD = consts[4];
}

void updateFromSerial() {
    if (Serial.available() > 3) {
        int key = Serial.parseInt();
        float value = Serial.parseFloat();
        consts[key] = value;
    }
    setConsts();
}

void loop() {
    updatePID(getError());
    updateMotorSpeed();
    updateFromSerial();
}