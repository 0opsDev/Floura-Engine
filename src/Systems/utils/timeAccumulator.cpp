#include "Systems/utils/timeAccumulator.h"
#include <iostream>

void TimeAccumulator::update() {
    Counter += TimeUtil::s_DeltaTime;
    //std::cout << Counter << std::endl;
}

void TimeAccumulator::reset() {
    Counter = 0;
    //std::cout << "reset" << std::endl;
}