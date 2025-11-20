#pragma once

#include "SensorBase.h"
#include <Arduino.h>
#include <Wire.h>

struct ASM330Data {
  float accelX, accelY, accelZ, gyrX, gyrY, gyrZ;
};

class ASM330 : public SensorBase<ASM330, ASM330Data>{
public:
  ASM330()
      : SensorBase<ASM330, ASM330Data>(26),
        AccGyr(&Wire, ASM330LHH_I2C_ADD_H) {}

  void init_impl() {
    Serial.print("Initializing ASM330... ");

    if (AccGyr.begin() != 0) {
      Serial.println("FAILED");
      return;
    }

    AccGyr.Set_X_ODR(26.0f);
    AccGyr.Set_G_ODR(26.0f);
    AccGyr.Set_X_FS(16);
    AccGyr.Set_G_FS(2000);
    AccGyr.Enable_X();
    AccGyr.Enable_G();

    float odr = 0.0f;
    AccGyr.Get_X_ODR(&odr);
    poll_interval_ms_ = (odr > 0.0f) ? 1000.0f / odr : 40.0f;
    Serial.println("OK");
  }

  void poll_impl(uint32_t now_ms, ASM330Data& out) {
    unsigned long now = millis();

    int32_t accel[3] = {0};
        int32_t gyro[3]  = {0};

        AccGyr.Get_X_Axes(accel);
        AccGyr.Get_G_Axes(gyro);

        out.accelX =  (float)accel[2] / 1000.0f;
        out.accelY = -(float)accel[0] / 1000.0f;
        out.accelZ = -(float)accel[1] / 1000.0f;
        out.gyrX   =  (float)gyro[2]  / 1000.0f;
        out.gyrY   = -(float)gyro[0]  / 1000.0f;
        out.gyrZ   = -(float)gyro[1]  / 1000.0f;
  }

private:
  ASM330LHHSensor AccGyr;
};