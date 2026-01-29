#include "../State.h"

void mainDescentInit(StateData *data) {}

StateID mainDescentLoop (StateData* data, Context* ctx) {
    if(/* TODO: Get velocity from GNC < Landing Velocity Threshold*/ false) {
        return RECOVERY;
    }

    if(data->currentTime >= MAIN_DESCENT_TIMEOUT) {
        return RECOVERY;
    }
    
    return MAIN_DESCENT;
}