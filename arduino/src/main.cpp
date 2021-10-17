#include <ArduinoJson.h>
#include <Adafruit_MotorShield.h>
#include <SPI.h>


#define SENSORPIN1 A0 // Left
#define SENSORPIN2 A1 // Right
#define SENSORPIN3 A2 // Middle
#define NUMREADINGS 3 // Number of readings to average

float motorSpeed = 25;
float errorOffset = 25;
float middleSensorThresh = 361;
float kP = 0.05;
float kI = 0.0;
float kD = 0.0;

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
    Serial.println(val);
    if (val<middleSensorThresh){
        return 0;
    }
    return val*4;
}

int getError() {
    int read = getReadings(SENSORPIN1) - getReadings(SENSORPIN2);
    
    if (read >= 0) {
        read -= getMidSensorReading();
    }
    else {
        read += getMidSensorReading();
    }
    if (abs(read)<errorOffset) {
        return 0;
    }
    return read;
}

void updatePID(int error) {
    P = error;
    I += error;
    D = error-lastError;
    lastError=error;
}

void updateMotorSpeed() {
    pidSpeed = kP*P + kI*I + kD*D;
    pidSpeed = constrain(pidSpeed, 0-motorSpeed-pidSpeed, motorSpeed-pidSpeed);
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