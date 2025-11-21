#pragma once

#include "SensorManager.h"
#include "config.h"
#include <Adafruit_ICM20948.h>
#include <Arduino.h>

struct ICMData {
  float accelX, accelY, accelZ;
  float gyrX, gyrY, gyrZ;
  float magX, magY, magZ;
  float temp;
};

#define ODR 40

class ICM20948 : public SensorBase<ICM20948, ICMData>, public ISensor {
public:
  using DataType = ICMData;
  static constexpr SensorDataType TYPE = SensorDataType::ACCEL;

  ICM20948()
      : SensorBase<ICM20948, ICMData>({TYPE, "ICM20948", ODR}), icm(),
        last_update_ms_(0), poll_interval_ms_(1000 / info_.poll_rate_hz) {}

  void init_impl() {
    Serial.print("Initializing ICM20948...");
    if (!icm.begin_I2C(0x68)) {
      icm.setAccelRange(ICM20948_ACCEL_RANGE_16_G);
      icm.setGyroRange(ICM20948_GYRO_RANGE_2000_DPS);
      uint16_t accelRateDiv = 1125 / ODR - 1;
        // Per datasheet: ODR = 1125 / (1 + div)
      uint8_t gyrRateDiv = 1100 / ODR - 1;
        // Per datasheet: ODR = 1100 / (1 + div)
      icm.setAccelRateDivisor(accelRateDiv);
      icm.setGyroRateDivisor(gyrRateDiv);
      Serial.println("FAILED");
    }
    Serial.println("OK");
  }

  void update_impl(SensorDataDescriptor<DataType> &desc) {
    unsigned long now = millis();

    if (last_update_ms_ == 0) {
      last_update_ms_ = now;
    }

    if (now - last_update_ms_ < poll_interval_ms_) {
      return;
    }
    last_update_ms_ = now;

    sensors_event_t accel, gyr, mag, temp;

    icm.getEvent(&accel, &gyr, &temp, &mag);

    float aX = accel.acceleration.x / g;
    float aY = accel.acceleration.y / g;
    float aZ = accel.acceleration.z / g;

    float gX = gyr.gyro.x;
    float gY = gyr.gyro.y;
    float gZ = gyr.gyro.z;

    float mX = mag.magnetic.x;
    float mY = mag.magnetic.y;
    float mZ = mag.magnetic.z;

    desc.data.accelX = aZ;
    desc.data.accelY = -aY;
    desc.data.accelZ = aX;

    desc.data.gyrX = gZ;
    desc.data.gyrY = -gY;
    desc.data.gyrZ = gX;

    desc.data.magX = mZ;
    desc.data.magY = mY;
    desc.data.magZ = -mX;

    desc.data.temp = temp.temperature;
    desc.timestamp = now;
  }

  // ISensor interface implementation
  void init() override { init_impl(); }
  void update() override { update_impl(descriptor_); }
  SensorDataType type() const override { return TYPE; }
  const char *name() const override { return info_.name; }
  const void *get_descriptor_ptr() const override { return &descriptor_; }

private:
  Adafruit_ICM20948 icm;
  unsigned long last_update_ms_;
  unsigned long poll_interval_ms_;
};
