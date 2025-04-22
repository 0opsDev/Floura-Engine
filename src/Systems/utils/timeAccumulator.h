#ifndef TIME_ACCUMULATOR
#define TIME_ACCUMULATOR

#include "Systems/utils/timeUtil.h"

class TimeAccumulator {
public:
	float TimeAccumulator::Counter;

	void update();

	void reset();
};

#endif // TIME_ACCUMULATOR