#include <SdFat.h>
#include "Context.h"
#include <stdint.h>
#include "logging.h"
#include "config.h"

size_t dataLengths[] = {
    sizeof(ASM330Data),
    sizeof(LPS22Data), // Length of s2 data in Bytes
    sizeof(ICMData),
    sizeof(MAX10SData),
    sizeof(INA219Data)
};

bool initializeLogging(Context *ctx) {
    Serial.print("Initailizing SD... ");
    SPI.setSCLK(SD_SCLK);
    SPI.setMISO(SD_MISO);
    SPI.setMOSI(SD_MOSI);
    SPI.begin();

    if (ctx->sd.begin(SD_CS, SD_SPI_SPEED)) { 
        // TODO: Define these values
        int fileIdx = 0;
        char filename[100];
        char errorFilename[100];
        while (fileIdx < 100)
        {
            sprintf(filename, "flightData%d.csv", fileIdx);
            sprintf(errorFilename, "errorLog%d.txt", fileIdx++);

            Serial.printf("Trying files `%s/%s`\n", filename, errorFilename);
            if (!ctx->sd.exists(filename))
            {
                ctx->logFile = ctx->sd.open(filename, O_RDWR | O_CREAT | O_TRUNC);
                ctx->errorLogFile = ctx->sd.open(errorFilename, O_RDWR | O_CREAT | O_TRUNC);
                break;
            }
        }
        return true;
    }
    else
    {
        // NOTE: SD initialization failed
        // Do something about that probably
        Serial.println("FAILED");
        return false;
    }
}

void writePacket(Context *ctx, SensorData data, SensorType type) {
    Packet packetToWrite = { type, millis(), data};

    size_t length = sizeof(uint8_t) + sizeof(uint32_t) + dataLengths[type];

    ctx->logFile.write(&packetToWrite, length);
    ctx->logFile.flush();
}

