#pragma once

#include "SensorManager/SensorBase.h"

#include "Sensors/ASM330.h"
#include "Sensors/LSP22.h"
#include "Sensors/ICM20948.h"
#include "Sensors/INA219.h"
#include "Sensors/MAX10S.h"

using ASM330Sensor = ASM330<Sensor<ASM330<Sensor<ASM330Data>>, ASM330Data>>;
using LPS22Sensor = LPS22<Sensor<LPS22<Sensor<LPS22Data>>, LPS22Data>>;
using ICM20948Sensor = ICM20948<Sensor<ICM20948<Sensor<ICMData>>, ICMData>>;
using MAX10SSensor = MAX10S<Sensor<MAX10S<Sensor<MAX10SData>>, MAX10SData>>;
using INA219Sensor = INA219<Sensor<INA219<Sensor<INA219Data>>, INA219Data>>;
