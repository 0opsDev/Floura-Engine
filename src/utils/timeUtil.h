#ifndef TIMEUTIL_H
#define TIMEUTIL_H

class TimeUtil {
public:
    static float deltaTime;
    static float lastFrameTime;

    static void updateDeltaTime();
};

#endif // TIMEUTIL_H
