#include <SdFat.h>
#include "Context.h"
#include <stdint.h>
#include "logging.h"
#include "config.h"

size_t dataLengths[] = {
    sizeof(ASM330Data),
    sizeof(LPS22Data), // Length of s2 data in Bytes
    sizeof(ICMData),
    sizeof(MAX10SData)
};

void initializeLogging(Context *ctx) {
    SPI.setSCLK(SD_SCLK);
    SPI.setMISO(SD_MISO);
    SPI.setMOSI(SD_MOSI);
    SPI.begin();

    if(ctx->sd.begin(SD_CS, SD_SPI_SPEED)) { // TODO: Define these values
        ctx->logFile = ctx->sd.open("LogingFile.bin", O_RDWR | O_CREAT | O_TRUNC);
    } else {
        // NOTE: SD initialization failed
        // Do something about that probably
    }
}

void writePacket(Context *ctx, SensorData data, SensorType type) {
    Packet packetToWrite = { type, millis(), data};

    size_t length = sizeof(uint8_t) + sizeof(uint32_t) + dataLengths[type];

    ctx->logFile.write(&packetToWrite, length);
    ctx->logFile.flush();
}

