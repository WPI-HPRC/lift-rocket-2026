#pragma once

#include <SdFat.h>
#include "boilerplate/Sensors/Impl/ASM330.h"
#include "boilerplate/Sensors/Impl/ICM20948.h"
#include "boilerplate/Sensors/Impl/INA219.h"
#include "boilerplate/Sensors/Impl/LSP22.h"
#include "boilerplate/Sensors/Impl/MAX10S.h"
#include "Servo.h"
#include "boilerplate/Sensors/SensorManager/SensorBase.h"

#define ASM_POLLING_RATE 26
#define LPS_POLLING_RATE 50

#define ASM_CS PD_5
#define LSM_CS PB_4
#define LIS_CS PA_15
#define LPS_CS PD_0

struct ASM330Data;
struct LPS22Data;
struct ICMData;
struct MAX10SData;
struct INA219Data;

struct Context {
    Context(SPIClass *dev_spi): accel(dev_spi, ASM_CS, ASM_POLLING_RATE), baro(dev_spi, LPS_CS, LPS_POLLING_RATE) {}

    File logFile;
    File errorLogFile;
    SdFs sd;
    bool sdInitialized;

    ASM330 accel;
    LPS22 baro;
    ICM20948 mag;
    MAX10S gps;
    INA219 curr;
    
    Servo airBrakes;
};
