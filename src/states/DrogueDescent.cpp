#include "../State.h"

void drogueDescentInit(StateData *data) {
    
}

StateID drogueDescentLoop (StateData* data, Context* ctx) {
    // TODO: Startup payload activities here or something

    if(/* TODO: Get velocity from GNC and check less than drogue descent velocity threshold*/ false) {
        return MAIN_DESCENT;
    }

    if(data->currentTime >= DROGUE_DESCENT_TIMEOUT) {
        return MAIN_DESCENT;
    }

    return MAIN_DESCENT;
}