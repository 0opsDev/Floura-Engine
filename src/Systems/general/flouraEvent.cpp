#include "flouraEvent.h"
#include "Scene/scene.h"

void flouraEvent::onBeginningOfGameThread()
{
    // queue deletion
    if (Scene::entityDeletionUnderGoing)
    {
        Scene::queuedDeletionLoop();
    }
    
}

void flouraEvent::onBeginningOfPhysicsThread()
{
    
}

void flouraEvent::onBeginningOfWorkThread()
{
    
}

void flouraEvent::onBeginningOfMainThread()
{
    
}
