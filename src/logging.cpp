#include <SdFat.h>
#include "Context.hpp"
#include <stdint.h>

typedef struct Sensor1 {
    int dataorsomething;
} Sensor1;

typedef struct Sensor2 {
    int dataorsomething;
} Sensor2;

enum SensorType {
    s1 = 0,
    s2 = 1,
};

union SensorData {
    Sensor1 s1;
    Sensor1 s2;
    // TODO: make these all the real sensors
    // TODO: make sure all contained types are packed structs
};

size_t dataLengths = {
    5, // Length of s1 data in Bytes
    10 // Length of s2 data in Bytes
    // ...
}

#pragma pack(1)
typedef struct Packet {
    uint8_t id; // NOTE: should identify the sensor and the length of this packet
    uint32_t timeStamp;
    SensorData data;
} Packet;

void initializeLogging(Context ctx) {
    int SD_CS = 0;
    int SD_SPI_SPEED = 0;
    if(ctx.sd.begin(SD_CS, SD_SPI_SPEED)) { // TODO: Define these values
        ctx.logFile = ctx.sd.open("LoogingFile.bin", O_RDWR | O_CREAT | O_TRUNC);
    } else {
        // NOTE: SD initialization failed
        // Do something about that probably
    }
}

void writePacket(Context ctx, SensorData data, SensorType type) {
    Packet packetToWrite{
        .id = type;
        .timeStamp = millis();
        .data = data;
    };

    size_t length = sizeof(uint8_t) + sizeof(uint32_t) + dataLengths[type];

    ctx.logFile.write((void *) packet, length);
}

