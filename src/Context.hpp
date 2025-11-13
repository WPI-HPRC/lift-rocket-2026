#pragma once
#include <SdFat.h>

struct Context {
    File logFile;
    SdFs sd;
};
