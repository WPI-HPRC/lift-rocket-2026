#pragma once

constexpr float LAUNCH_TRHESHOLD = -5; // in ned frame vertical dir up is negative

constexpr float COAST_THRESHOLD = -0.3f; // in ned frame vertical dir up is negative

constexpr float MAIN_MIN_VEL = 5; // in ned frame, going dowm 5m/s = 5

constexpr float MAIN_MAX_VEL = 9; // in ned frame, going down 9m/s = 9

constexpr float BOOST_TIMEOUT = 0.0f;

constexpr float COAST_TIMEOUT = 0.0f;

constexpr float DROGUE_DESCENT_TIMEOUT = 0.0f;

constexpr float MAIN_DESCENT_TIMEOUT = 0.0f;
