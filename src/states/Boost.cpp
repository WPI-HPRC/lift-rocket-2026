#include "../State.h"
#include "StateMachineConstants.h"

void boostInit (StateData* data) {
    data->startTime = millis();
    data->currentTime = 0;
    data->deltaTime = 0;
    data->lastLoopTime = 0;
    data->loopCount = 0;
}

StateID boostLoop (StateData* data, Context* ctx) {
    long long now = millis();
    // These values may be used in the state code
    data->currentTime = now - data->startTime;
    data->deltaTime = now - data->lastLoopTime;
    data->lastLoopTime = now;
    data->loopCount++;
    /*
    - Poll acceleration data from ctx
    - Check acceleration to detect coast stage
    - Check if maximum boost time is exceeded
    - Check if need to abort
    - Update sensor data and ctx for next iteration?
    */
    const auto &accel_desc = ctx->accel.get_descriptor();
    if (accel_desc.getLastUpdated() != data->lastAccelReadingTime && data->currentTime > 2000) {
        data->lastAccelReadingTime = accel_desc.getLastUpdated();
        if(data->accelDebouncer.update(accel_desc.data.accelZ < COAST_THRESHOLD , millis())) {
            return COAST;
        }
    }

    return BOOST;
}
