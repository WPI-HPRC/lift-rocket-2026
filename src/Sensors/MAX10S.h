#pragma once

#include <stdint.h>
#include "SensorManager.h"
#include <Arduino.h>
#include <SparkFun_u-blox_GNSS_v3.h>

struct MAX10SData {
    float lat;
    float lon;
    float altMSL;
    float altEllipsoid;
    int32_t velN;
    int32_t velE;
    int32_t velD;
    uint32_t epochTime;
    uint8_t satellites;
    uint8_t gpsLockType;
};

class MAX10S: public SensorBase<MAX10S, MAX10SData>, public ISensor {
public:
    using DataType = MAX10SData;
    static constexpr SensorDataType TYPE = SensorDataType::GPS;

    MAX10S()
        : SensorBase<MAX10S, MAX10SData>({TYPE, "MAX10S", 40}),
          GPS(), last_update_ms_(0), poll_interval_ms_(1000 / info_.poll_rate_hz) {}

    void init_impl() {
        Serial.print("Initializing MAX10S...");
        if (GPS.begin()) {
            GPS.setNavigationFrequency(25);
            GPS.setAutoPVT(true);
            Serial.println("OK");
        } else {
            Serial.println("FAILED");
        }
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
        desc.data.gpsLockType = GPS.getFixType();
        desc.data.lat = (float)GPS.getLatitude() / 1e7;
        desc.data.lon = (float)GPS.getLongitude() / 1e7;
        desc.data.altMSL = (float)GPS.getAltitudeMSL() / 1000.0;
        desc.data.altEllipsoid = (float)GPS.getAltitude() / 1000.0;
        desc.data.velN = GPS.getNedNorthVel();
        desc.data.velE = GPS.getNedEastVel();
        desc.data.velD = GPS.getNedDownVel();
        desc.data.epochTime = GPS.getUnixEpoch();
        desc.data.satellites = GPS.getSIV(); // Satellites In View
        desc.timestamp = now;
    }

    // ISensor interface implementation
    void init() override { init_impl(); }
    void update() override { update_impl(descriptor_); }
    SensorDataType type() const override { return TYPE; }
    const char *name() const override { return info_.name; }
    const void *get_descriptor_ptr() const override { return &descriptor_; }

private:
    SFE_UBLOX_GNSS GPS;
    unsigned long last_update_ms_;
    unsigned long poll_interval_ms_;
};
