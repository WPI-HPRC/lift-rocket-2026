#pragma once

#include "../SensorManager/SensorBase.h"
#include <Arduino.h>
#include <Adafruit_ICM20948.h>
#include <Wire.h>
#include "config.h"

// #define ICM20948_POLLING_RATE
#define ODR 40
// is this the polling rate but named different

struct ICMData {
  float accelX, accelY, accelZ;
  float gyrX, gyrY, gyrZ;
  float magX, magY, magZ;
};

template<class SensorBaseType>
class ICM20948 : public SensorBaseType {
public:
  ICM20948()
      : SensorBaseType(ODR), // assuming this is the polling rate
        icm() {}
  // this doesnt seem done but whatever

  bool init_impl() {
    Serial.print("Initializing ICM20948...");
    if (!icm.begin_I2C(0x68)) {
      icm.setAccelRange(ICM20948_ACCEL_RANGE_16_G);
      icm.setGyroRange(ICM20948_GYRO_RANGE_2000_DPS);
      uint16_t accelRateDiv =
          1125 / ODR - 1; // Per datasheet: ODR = 1125 / (1 + div)
      uint8_t gyrRateDiv =
          1100 / ODR - 1; // Per datasheet: ODR = 1100 / (1 + div)
      icm.setAccelRateDivisor(accelRateDiv);
      icm.setGyroRateDivisor(gyrRateDiv);
      Serial.println("FAILED");

      return false;
    }
    Serial.println("OK");

    // init seems weird, will look at later
    return true;
  }

  void poll_impl(uint32_t now_ms, ICMData &out) {
    sensors_event_t accel, gyr, mag;
    
    out.accelX = accel.acceleration.x / g;
    out.accelY = -accel.acceleration.y / g;
    out.accelZ = accel.acceleration.z / g;
    
    out.gyrX = gyr.gyro.x;
    out.gyrY = -gyr.gyro.y;
    out.gyrZ = gyr.gyro.z;

    out.magX = mag.magnetic.x;
    out.magY = mag.magnetic.y;
    out.magZ = -mag.magnetic.z;  

  }
  
private:
  Adafruit_ICM20948 icm;
};
