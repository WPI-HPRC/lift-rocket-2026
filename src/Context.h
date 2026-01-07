#pragma once

#include <SdFat.h>
#include "Sensors/ASM330.h"
#include "Sensors/ICM20948.h"
#include "Sensors/INA219.h"
#include "Sensors/LSP22.h"
#include "Sensors/MAX10S.h"
#include "Servo.h"
#include "SensorManager/SensorBase.h"

struct ASM330Data;
struct LPS22Data;
struct ICMData;
struct MAX10SData;
struct INA219Data;

struct Context {
    File logFile;
    File errorLogFile;
    SdFs sd;
    bool sdInitialized;

    ASM330<Sensor<ASM330<Sensor<ASM330Data, ASM330Data>>, ASM330Data>> accel;
    LPS22<Sensor<LPS22<Sensor<LPS22Data, LPS22Data>>, LPS22Data>> baro;
    ICM20948<Sensor<ICM20948<Sensor<ICMData, ICMData>>, ICMData>> mag;
    MAX10S<Sensor<MAX10S<Sensor<MAX10SData, MAX10SData>>, MAX10SData>> gps;
    INA219<Sensor<INA219<Sensor<INA219Data, INA219Data>>, INA219Data>> curr;
    
    Servo airBrakes;
};
