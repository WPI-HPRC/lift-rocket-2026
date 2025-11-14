#pragma once

#include "Adafruit_INA219.h"
#include "SensorManager.h"
#include <Arduino.h>
#include <Wire.h>

struct INA219Data {
  float curr;
};

class INA219 : public SensorBase<INA219, INA219Data>, public ISensor {
public:
  using DataType = INA219Data;
  static constexpr SensorDataType TYPE = SensorDataType::CURR;

  INA219()
      : SensorBase<INA219, INA219Data>({TYPE, "INA219", 100}), ina219(),
        last_update_ms_(0), poll_interval_ms_(1000 / info_.poll_rate_hz) {}

  void init_impl() {
    Serial.print("Initializing for INA219... ");

    if (!ina219.begin()) {
      Serial.println("FAILED");
      return;
    }
    Serial.println("OK");
    return;
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

    float current_mA = ina219.getCurrent_mA();

    desc.data.curr = current_mA;
    desc.timestamp = now;
  }

  // ISensor interface implementation
  void init() override { init_impl(); }
  void update() override { update_impl(descriptor_); }
  SensorDataType type() const override { return TYPE; }
  const char *name() const override { return info_.name; }
  const void* get_descriptor_ptr() const override { return &descriptor_; }


private:
  Adafruit_INA219 ina219;
  unsigned long last_update_ms_;
  unsigned long poll_interval_ms_;
};
