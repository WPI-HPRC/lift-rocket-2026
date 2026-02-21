#include "../State.h"
#include "StateMachineConstants.h"

void boostInit (StateData* data) {}

StateID boostLoop (StateData* data, Context* ctx) {
    /*
    - Poll acceleration data from ctx
    - Check acceleration to detect coast stage
    - Check if maximum boost time is exceeded
    - Check if need to abort
    - Update sensor data and ctx for next iteration?
    */
    auto &accel_desc = ctx->accel;
    if (accel_desc.dataUpdatedAt() != data->lastAccelReadingTime && data->currentTime > 2000) {
        data->lastAccelReadingTime = accel_desc.dataUpdatedAt();
        if(data->accelDebouncer.update(accel_desc.getData()->accelX < COAST_THRESHOLD , millis())) {
            return COAST;
        }
    }

    return BOOST;
}
