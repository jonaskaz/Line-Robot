#include <ArduinoJson.h>
#include <Adafruit_MotorShield.h>
#include <SPI.h>


#define SENSORPIN1 A0 // Left
#define SENSORPIN2 A1 // Right
#define NUMREADINGS 3 // Number of readings to average

float motorSpeed = 35;
float errorOffset = 15;
float kP = 0.05;
float kI = 0.001;
float kD = 0.05;

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

int getError() {
    int read = getReadings(SENSORPIN1) - getReadings(SENSORPIN2);
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
    pidSpeed = constrain(pidSpeed, 0, motorSpeed-pidSpeed);
    motorRight->setSpeed(motorSpeed + 0.5*pidSpeed);
    motorLeft->setSpeed(motorSpeed - 0.5*pidSpeed);
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
    //updatePID(getError());
    //updateMotorSpeed(); 
    updateFromSerial();
}