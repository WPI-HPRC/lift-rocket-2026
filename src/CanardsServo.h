#include <Servo.h>

struct CanardServo {
    Servo servo;
    double setpoint;
    double Kp;
    double Ki;
    double Kd;
};


