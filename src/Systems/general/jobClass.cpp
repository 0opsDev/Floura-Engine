#include "jobClass.h"
#include "Scene/scene.h"

void getAjob::onBeginningOfGameThread(){
    // queue deletion
    if (Scene::entityDeletionUnderGoing){
        Scene::queuedDeletionLoop();
    }
    
}

void getAjob::onBeginningOfPhysicsThread(){
    
}

void getAjob::onBeginningOfWorkThread(){
    
}

void getAjob::onBeginningOfMainThread(){
    
}
