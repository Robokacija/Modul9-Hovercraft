#define MOTORS_H

void setupMotors();
void setLift(int motor1, int motor2);
void stop();
void drive(int left, int right);