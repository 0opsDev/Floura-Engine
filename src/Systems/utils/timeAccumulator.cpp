#include "Systems/utils/timeAccumulator.h"

void TimeAccumulator::update() {
    Counter += TimeUtil::s_DeltaTime; // Increase timer
}

void TimeAccumulator::reset() {
    Counter = 0;
}