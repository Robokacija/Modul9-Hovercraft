#include <Arduino.h>
#include <motors.h>

#define MOTOR1_A 18 // LIFT
#define MOTOR1_B 19

#define MOTOR2_A 32 // LIFT
#define MOTOR2_B 33

#define MOTOR3_A 13 // LEFT
#define MOTOR3_B 27

#define MOTOR4_A 26 // RIGHT
#define MOTOR4_B 25

void setupMotors(){
    pinMode(MOTOR1_A, OUTPUT);
    pinMode(MOTOR1_B, OUTPUT);
    pinMode(MOTOR2_A, OUTPUT);
    pinMode(MOTOR2_B, OUTPUT);
    pinMode(MOTOR3_A, OUTPUT);
    pinMode(MOTOR3_B, OUTPUT);
    pinMode(MOTOR4_A, OUTPUT);
    pinMode(MOTOR4_B, OUTPUT);

    analogWriteFrequency(20000);

    stop();
}

void stop(){
    analogWrite(MOTOR1_A, 0);
    analogWrite(MOTOR2_A, 0);
    analogWrite(MOTOR3_A, 0);
    analogWrite(MOTOR4_A, 0);
}

void setLift(int motor1, int motor2)
{
    analogWrite(MOTOR1_A, motor1);
    analogWrite(MOTOR2_A, motor2);
}

void drive(int left, int right){
    analogWrite(MOTOR3_A, left);
    analogWrite(MOTOR4_A, right);
}