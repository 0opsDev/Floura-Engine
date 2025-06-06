#ifndef TIME_ACCUMULATOR
#define TIME_ACCUMULATOR

#include "utils/timeUtil.h"

class TimeAccumulator {
public:

	// multiply this by 1/framerate
	float Counter;

	// Increase timer
	void update();

	// reset the accumulator
	void reset();
};

#endif // TIME_ACCUMULATOR