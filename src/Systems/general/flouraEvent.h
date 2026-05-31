#ifndef FLOURA_EVENT_H
#define FLOURA_EVENT_H

class flouraEvent
{
public:

    static void onBeginningOfGameThread();
    static void onBeginningOfPhysicsThread();
    static void onBeginningOfWorkThread();
    static void onBeginningOfMainThread();
    
    
private:

};

#endif