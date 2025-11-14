#pragma once

#include <stdint.h>
#include "Sensors/ASM330.h"
#include "Sensors/LSP22.h"

enum SensorType {
    ASM330_TAG = 0,
    LPS22_TAG = 1,
};

union SensorData {
    ASM330Data asm330;
    LPS22Data lps22;
};


#pragma pack(1)
struct Packet {
    uint8_t id; // NOTE: should identify the sensor and the length of this packet
    uint32_t timeStamp;
    SensorData data;
};

void initializeLogging(Context *ctx);

void writePacket(Context *ctx, SensorData data, SensorType type);