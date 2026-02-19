#pragma once

#include "Context.h"
#include "CanardServoTypes.h"

void canardsSetup(Context *ctx);
void canardsLoop(Context *ctx);

void servoSetpoint(CanardServo *servo, double setpoint);
