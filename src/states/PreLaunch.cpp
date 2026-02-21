#include "../State.h"
#include "logging.h"
#include "StateMachineConstants.h"
#include "Arduino.h"
#include "config.h"

void prelaunchInit (StateData* data) { }

StateID prelaunchLoop(StateData *data, Context *ctx) {
    //Serial.println("looping prelaunch");

    static bool BlueLedState = false;
    static bool GreenLedState = false;
    static uint32_t lastBlueToggleTime = 0;
    static uint32_t lastGreenToggleTime = 0;

    /*
    - Poll acceleration data from ctx
    - Check acceleration to detect launch
    - Check if need to abort
    - Update sensor data and ctx for next iteration?
    */
    auto &accel_desc = ctx->accel;
    if (accel_desc.dataUpdatedAt() != data->lastAccelReadingTime) {
        data->lastAccelReadingTime = accel_desc.dataUpdatedAt();
        if(data->accelDebouncer.update(accel_desc.getData()->accelX > LAUNCH_TRHESHOLD, millis())) {
            return BOOST;
        }
    }

    if(ctx->sdInitialized && ctx->logFile != NULL)  {
        // blink
        if((millis() - lastBlueToggleTime) > 250) {
            lastBlueToggleTime = millis();
            BlueLedState = !BlueLedState;
            digitalWrite(BLUE_LED_PIN, BlueLedState);
        }
    }

    /*
    const auto &gps_desc = ctx->gps.get_descriptor();

    if(gps_desc.data.gpsLockType == 3) {
        if((millis() - lastGreenToggleTime) > 250) {
            lastGreenToggleTime = millis();
            GreenLedState = !GreenLedState;
            digitalWrite(GREEN_LED_PIN, GreenLedState);
        }
    }
        */


    return PRELAUNCH;
}
