#include <Arduino.h>
#include "Context.h"
#include "State.h"
#include "States.h"

#include "SensorManager.h"
#include "Sensors/LSP22.h"
#include "Sensors/ASM330.h"
#include "Sensors/ICM20948.h"
#include "Sensors/MAX10S.h"
#include "Sensors/INA219.h"
#include "config.h"

#include "logging.h"

// Create Sensor Objects
ASM330   asm330;
LPS22    lps22;
ICM20948 icm20948;
MAX10S   max10s;
INA219   ina219;

uint32_t millisSource() { return millis(); }
SensorManager mgr{ millisSource, asm330, lps22, icm20948, max10s, ina219 };


StateID currentState;
StateData data;

StateInitFunc initFuncs[NUM_STATES] = {};
StateLoopFunc loopFuncs[NUM_STATES] = {};

void sensorsSetup() {
    Serial.begin(115200);

    Serial.println("Starting MARS board initialization...");

    Wire.setSDA(SENSOR_SDA);
    Wire.setSCL(SENSOR_SCL);
    Wire.begin();

    Serial.print("I2C initialized on SDA: ");
    Serial.print(SENSOR_SDA);
    Serial.print(", SCL: ");
    Serial.println(SENSOR_SCL);

    mgr.sensorInit();

    Wire.setClock(400000);

    Serial.println("\n=== Sensor Initialization Summary ===");
    Serial.print("Total sensors: ");
    Serial.println(mgr.count());
    Serial.println("=== Starting main loop ===\n");
}

void sensorLoop() {
    static unsigned long last_print = 0;
    static int loop_count = 0;

    // Update all sensors through manager
    manager.loop();

    if (currentState >= PRELAUNCH) {
        return;
    }

    // manager is not being used here to get data
    if (millis() - last_print > 200)
    {
        last_print = millis();
        loop_count++;

        Serial.print("\n=== Loop ");
        Serial.print(loop_count);
        Serial.println(" ===");

        // DIRECT ACCESS to sensor data - this is guaranteed to work
        const auto &accel_desc = asm330.get_descriptor();
        const auto &baro_desc = lps22.get_descriptor();
        const auto &mag_desc = icm20948.get_descriptor();
        const auto &gps_desc = max10s.get_descriptor();
        const auto &curr_desc = ina219.get_descriptor();

        bool has_data = false;
        // Print ASM330 data
        if (accel_desc.timestamp > 0)
        {
            Serial.print("ASM330 - Accel: ");
            Serial.print(accel_desc.data.accelX, 4);
            Serial.print(", ");
            Serial.print(accel_desc.data.accelY, 4);
            Serial.print(", ");
            Serial.print(accel_desc.data.accelZ, 4);
            Serial.print(" | Gyro: ");
            Serial.print(accel_desc.data.gyrX, 4);
            Serial.print(", ");
            Serial.print(accel_desc.data.gyrY, 4);
            Serial.print(", ");
            Serial.print(accel_desc.data.gyrZ, 4);
            Serial.println();
            has_data = true;
        }
        else
        {
            Serial.println("ASM330: No data (timestamp = 0)");
        }

        // Print LPS22 data
        if (baro_desc.timestamp > 0)
        {
            Serial.print("LPS22 - Pressure: ");
            Serial.print(baro_desc.data.pressure, 4);
            Serial.print(" hPa, Temp: ");
            Serial.print(baro_desc.data.temperature, 4);
            Serial.println(" C");
            has_data = true;
        }
        else
        {
            Serial.println("LPS22: No data (timestamp = 0)");
        }

        if(mag_desc.timestamp > 0)
        {
            Serial.print("ICM20948 - Accel: ");
            Serial.print(mag_desc.data.accelX, 4);
            Serial.print(", ");
            Serial.print(mag_desc.data.accelY, 4);
            Serial.print(", ");
            Serial.print(mag_desc.data.accelZ, 4);
            Serial.print(" | Gyro: ");
            Serial.print(mag_desc.data.gyrX, 4);
            Serial.print(", ");
            Serial.print(mag_desc.data.gyrY, 4);
            Serial.print(", ");
            Serial.print(mag_desc.data.gyrZ, 4);
            Serial.println();
            has_data = true;
        }
        else
        {
            Serial.println("ICM20948: No data (timestamp = 0)");
        }

        if(gps_desc.timestamp > 0)
        {
            Serial.print("MAX10S - Lat, Lon, AltMSL, AltElipsoid: ");
            Serial.print(gps_desc.data.lat, 4);
            Serial.print(", ");
            Serial.print(gps_desc.data.lon, 4);
            Serial.print(", ");
            Serial.print(gps_desc.data.altMSL, 4);
            Serial.print(", ");
            Serial.print(gps_desc.data.altEllipsoid, 4);
            Serial.print("| GPS Lock Type - ");
            Serial.print(gps_desc.data.gpsLockType);
            Serial.println();
            has_data = true;
        }
        else
        {
            Serial.println("MAX10S: No data (timestamp = 0)");
        }

        if(curr_desc.timestamp > 0) {
            Serial.print("INA219 - Curr: ");
            Serial.println(curr_desc.data.curr, 4);
            has_data = true;
        } else {
            Serial.println("INA219: No data (timestamp = 0)");
        }

        if (!has_data)
        {
            Serial.println("No sensor data received yet...");
        }

        Serial.println("======================");
    }
}

void setup() {
    currentState = PRELAUNCH;
    data = {};

    initFuncs[PRELAUNCH] = &prelaunchInit;
    initFuncs[BOOST] = &boostInit;
    initFuncs[COAST] = &coastInit;
    initFuncs[DROGUE_DESCENT] = &drogueDescentInit;
    initFuncs[MAIN_DESCENT] = &mainDescentInit;
    initFuncs[RECOVERY] = &recoveryInit;
    initFuncs[ABORT] = &abortInit;

    loopFuncs[PRELAUNCH] = &prelaunchLoop;
    loopFuncs[BOOST] = &boostLoop;
    loopFuncs[COAST] = &coastLoop;
    loopFuncs[DROGUE_DESCENT] = &drogueDescentLoop;
    loopFuncs[MAIN_DESCENT] = &mainDescentLoop;
    loopFuncs[RECOVERY] = &recoveryLoop;
    loopFuncs[ABORT] = &abortLoop;

    pinMode(BLUE_LED_PIN, OUTPUT);
    pinMode(GREEN_LED_PIN, OUTPUT);
    pinMode(RED_LED_PIN, OUTPUT);
    pinMode(RED_LED_PIN1, OUTPUT);
    pinMode(RED_LED_PIN2, OUTPUT);

    
    // NOTE: Run initialization on the first state
    (*initFuncs[currentState])(&data);
    sensorsSetup();
    ctx.sdInitialized = initializeLogging(&ctx);

    ctx.airBrakes.attach(SERVO_PIN);
    ctx.airBrakes.writeMicroseconds(SERVO_MIN);
}

void loop() {
    StateID newState = (*loopFuncs[currentState])(&data, &ctx);

    if(currentState != newState) {
        (*initFuncs[newState])(&data);
        currentState = newState;
        ctx.errorLogFile.printf("%d %d\n", newState, millis());
    }

    sensorLoop();
    loggingLoop(&ctx);
}
