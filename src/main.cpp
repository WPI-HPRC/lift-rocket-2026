#include <Arduino.h>
#include "Context.h"
#include "State.h"
#include "States.h"

#include "boilerplate/Sensors/SensorManager/SensorManager.h"
#include "config.h"

#include "logging.h"

#define SPI_MOSI PD_7
#define SPI_MISO PG_9
#define SPI_SCLK PG_11

#define ASM_INT_1 PF4_ALT0

SPIClass dev_spi(SPI_MOSI, SPI_MISO, SPI_SCLK);

Context *ctx;

using MillisFn = uint32_t (*)();   // or unsigned long (*)() depending on your millis
constexpr size_t N = 4;

using MgrT = SensorManager<MillisFn, N>;

MgrT* mgr = nullptr;   // declare now

uint32_t millisSource() { return millis(); }

StateID currentState;
StateData data;

StateInitFunc initFuncs[NUM_STATES] = {};
StateLoopFunc loopFuncs[NUM_STATES] = {};

void initStateData(StateData *data) {
    data->startTime = millis();
    data->currentTime = 0;
    data->deltaTime = 0;
    data->lastLoopTime = 0;
    data->loopCount = 0;
};

void updateStateData(StateData *data) {
    long long now = millis();
    data->currentTime = now - data->startTime;
    data->deltaTime = now - data->lastLoopTime;
    data->lastLoopTime = now;
    data->loopCount++;
}

void sensorsSetup() {
    Serial.println("Starting MARS board initialization...");

    /*
    Wire.setSDA(SENSOR_SDA);
    Wire.setSCL(SENSOR_SCL);
    Wire.begin();

    Serial.print("I2C initialized on SDA: ");
    Serial.print(SENSOR_SDA);
    Serial.print(", SCL: ");
    Serial.println(SENSOR_SCL);
    */

    mgr->sensorInit();

    Wire.setClock(400000);

    Serial.println("\n=== Sensor Initialization Summary ===");
    Serial.print("Total sensors: ");
    Serial.println(mgr->count);
    Serial.println("=== Starting main loop ===\n");
}

void sensorLoop() {
    static unsigned long last_print = 0;
    static int loop_count = 0;

    // Update all sensors through manager
    mgr->loop();

    /*
    if (currentState >= PRELAUNCH) {
        return;
    }
    */

    // manager is not being used here to get data
    if (millis() - last_print > 200)
    {
        last_print = millis();
        loop_count++;

        Serial.print("\n=== Loop ");
        Serial.print(loop_count);
        Serial.println(" ===");

        // DIRECT ACCESS to sensor data - this is guaranteed to work
        auto &accel_desc = ctx->accel;
        auto &baro_desc = ctx->baro;
        auto &mag_desc = ctx->mag;
        auto &gps_desc = ctx->gps;
        // auto &curr_desc = ctx->curr;

        bool has_data = false;
        // Print ASM330 data
        if (accel_desc.dataUpdatedAt() > 0)
        {
            Serial.print("ASM330 - Accel: ");
            Serial.print(accel_desc.getData()->accelX, 4);
            Serial.print(", ");
            Serial.print(accel_desc.getData()->accelY, 4);
            Serial.print(", ");
            Serial.print(accel_desc.getData()->accelZ, 4);
            Serial.print(" | Gyro: ");
            Serial.print(accel_desc.getData()->gyrX, 4);
            Serial.print(", ");
            Serial.print(accel_desc.getData()->gyrY, 4);
            Serial.print(", ");
            Serial.print(accel_desc.getData()->gyrZ, 4);
            Serial.println();
            has_data = true;
        }
        else
        {
            Serial.println("ASM330: No data (timestamp = 0)");
        }

        // Print LPS22 data
        if (baro_desc.dataUpdatedAt() > 0)
        {
            Serial.print("LPS22 - Pressure: ");
            Serial.print(baro_desc.getData()->pressure, 4);
            Serial.print(" hPa, Temp: ");
            Serial.print(baro_desc.getData()->temperature, 4);
            Serial.println(" C");
            has_data = true;
        }
        else
        {
            Serial.println("LPS22: No data (timestamp = 0)");
        }

        /*
        if(mag_desc.getLastUpdated()  > 0)
        {
            Serial.print("ICM20948 - Accel: ");
            Serial.print(mag_desc.getData().accel0, 4);
            Serial.print(", ");
            Serial.print(mag_desc.getData().accel1, 4);
            Serial.print(", ");
            Serial.print(mag_desc.getData().accel2, 4);
            Serial.print(" | Gyro: ");
            Serial.print(mag_desc.getData().gyr0, 4);
            Serial.print(", ");
            Serial.print(mag_desc.getData().gyr1, 4);
            Serial.print(", ");
            Serial.print(mag_desc.getData().gyr2, 4);
            Serial.println();
            has_data = true;
        }
        else
        {
            Serial.println("ICM20948: No data (timestamp = 0)");
        }
            */

        /*
        if(gps_desc.getLastUpdated() > 0)
        {
            Serial.print("MAX10S - Lat, Lon, AltMSL, AltElipsoid: ");
            Serial.print(gps_desc.getData().lat, 4);
            Serial.print(", ");
            Serial.print(gps_desc.getData().lon, 4);
            Serial.print(", ");
            Serial.print(gps_desc.getData().altMSL, 4);
            Serial.print(", ");
            Serial.print(gps_desc.getData().altEllipsoid, 4);
            Serial.print("| GPS Lock Type - ");
            Serial.print(gps_desc.getData().gpsLockType);
            Serial.println();
            has_data = true;
        }
        else
        {
            Serial.println("MAX10S: No data (timestamp = 0)");
        }
        */

        if (!has_data)
        {
            Serial.println("No sensor data received yet...");
        }

        Serial.println("======================");
    }
}

void setup() {

    Serial.begin(115200);
    while(!Serial.available()) {
        delay(10);
    }

    ctx = new Context(&dev_spi);

    Sensor* sensor_list[] = {
        &ctx->accel,
        &ctx->baro,
        &ctx->mag,
        &ctx->gps,
    };

    MgrT mgr_storage(sensor_list, millisSource);
    mgr = &mgr_storage;

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

    dev_spi.setMISO(SPI_MISO);
    dev_spi.setMOSI(SPI_MOSI);
    dev_spi.setSCLK(SPI_SCLK);
    dev_spi.begin();

    pinMode(ASM_INT_1, INPUT_PULLDOWN);
    
    // NOTE: Run initialization on the first state
    initStateData(&data);
    (*initFuncs[currentState])(&data);
    sensorsSetup();
    // ctx->sdInitialized = initializeLogging(ctx);

    /*
    ctx->airBrakes.attach(SERVO_PIN);
    ctx->airBrakes.writeMicroseconds(SERVO_MIN);
    */
}

void loop() {

    updateStateData(&data);
    StateID newState = (*loopFuncs[currentState])(&data, ctx);

    if(currentState != newState) {
        initStateData(&data);
        (*initFuncs[newState])(&data);
        currentState = newState;
        ctx->errorLogFile.printf("%d %d\n", newState, millis());
    }

    sensorLoop();
    loggingLoop(ctx);
}
