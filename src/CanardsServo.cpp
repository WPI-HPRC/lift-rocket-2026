#include <Context.h>
#include <Servo.h>
#include "CanardsServo.h"

void servoSetup(Context *ctx) {
    ctx->canard0.servo.attach(CANARD_0_PIN);
    ctx->canard0.servo.writeMicroseconds(1500);
    pinMode(CANARD_0_FEEDBACK_PIN, INPUT);
}

void servoLoop(Context *ctx) { 
    // PID

}

double getEncoder() {
    unint32_t timeHigh = pulseIn(CANARD_0_FEEDBACK_PIN, HIGH);
    unint32_t timeLow = pulseIn(CANARD_0_FEEDBACK_PIN, LOW);
    double dutyCycle = (double)timeHigh / ((double)timeHigh + (double)timeLow);
    double angle = (dutyCycle * 360.0);
    return angle;
}