#include "physworld.h"
#include "utils/timeUtil.h"
#include "Core/Render.h"
#include "Render/Handler/RenderHandler.h"
#include <random>

    
std::vector<physworld::objectPosBundle> physworld::pObjects;
std::vector<physworld::emitter> physworld::emitters;
float physworld::tickrate = 30.0f;
glm::vec3 physworld::worldGravity = glm::vec3(0.0f, -9.81f, 0.0f);

void physworld::uploadEmitter(emitter emitter)
{
    emitters.push_back(emitter);
    fillEmitterParticleBuffer(emitters.size() - 1);
}

void physworld::fillEmitterParticleBuffer(int index)
{
    emitters[index].particles.clear();
    for (int i = 0; i < emitters[index].limit; ++i)
    {
        particle nP;
        emitters[index].particles.push_back(nP);
    }
}

void physworld::killParticle(emitter& emitter, int index)
{
    if (!emitter.particles[index].alive) return; // if not alive return
    emitter.particles[index].alive = false;
    emitter.particles[index].position = emitter.position;
    emitter.particles[index].age = 0.0f;
    emitter.particles[index].physicsObject = emitter.templatePhysicsObject;
    emitter.amount = emitter.amount - 1;
}

float physworld::accumulatedTime = 0.0f;

void physworld::update(float deltatime)
{
    for (auto& emitter : emitters)
    {
        // check if enabled
        if (!emitter.enabled) continue;

        emitter.amount = 0; // reset amount;
        // kill if out of distance
        for (int i = 0; i < emitter.particles.size(); ++i)
        {
            // particles
            if (emitter.particles[i].alive)
                stimulateObject(emitter.particles[i].physicsObject, emitter.particles[i].position, emitter.gravity, deltatime);
        }
    }
    
    int index = 0;
    
    accumulatedTime+= TimeUtil::deltatime;
    if (accumulatedTime >= 1.0f / tickrate || true)
    {
        for (auto& emitter : emitters)
        {
            index++;
            // check if enabled
            if (!emitter.enabled) continue;

            emitter.amount = 0; // reset amount;
            // kill if out of distance
            for (int i = 0; i < emitter.particles.size(); ++i)
            {
                
                // particles
                if (emitter.particles[i].alive)
                {
                    
                    // out of distance kill
                    if (emitter.killOutOfDistance &&  glm::distance(emitter.position, emitter.particles[i].position) > emitter.maxDistance) {killParticle(emitter, i);}
                    // over limit kill
                    //if (emitter.killOnLimit && emitter.amount >= emitter.limit) {killParticle(emitter, i);}
                    // kill when over lifespan
                    //if (emitter.lifespan >= emitter.particles[i].age) {killParticle(emitter, i);}
                
                    // check if alive again
                    if (emitter.particles[i].alive)
                    {
                        // accumulate age
                        emitter.particles[i].age += 0.1f * TimeUtil::deltatime;
                
                        // update amont
                        emitter.amount++;
                    
                        // run particle phys here would be nice
                        //stimulateObject(emitter.particles[i].physicsObject, emitter.particles[i].position, emitter.gravity);
                    }

                }
            }
            
            
            // spawn logic
            emitter.spawnAcummulator += TimeUtil::deltatime;
            if (emitter.spawnAcummulator >= 1.0f / emitter.spawnTickrate)
            {
                
                int count = emitter.amount = emitter.limit;
                
                if (count > 0)
                {
                    
                    //std::cout << emitter.particles.size() << std::endl;
                    
                    for (int i = 0; i < emitter.particles.size(); ++i)
                    {
                        
                        if (count <= 0) {
                        }
                        
                        if (!emitter.particles[i].alive)
                        {
                            emitter.particles[i].alive = true;
                            
                            std::mt19937 genX(count + index * TimeUtil::frame +1);
                            std::uniform_real_distribution<float> distX(-1.0f, 1.0f);
                            float x = distX(genX);
                            //std::mt19937 genY(count + index * TimeUtil::frame + 2);
                            //std::uniform_real_distribution<float> distY(-1.0f, 1.0f);
                            //float y = distY(genY);
                            std::mt19937 genZ( count + index * TimeUtil::frame +3);
                            std::uniform_real_distribution<float> distZ(0.0f, 1.0f);
                            float z = distZ(genZ);
                            
                            std::mt19937 genD( count + index * TimeUtil::frame +4);
                            std::uniform_real_distribution<float> distD(1250.0f, 5979.0f);
                            float D = distD(genD);
                            
                            emitter.particles[i].physicsObject.force = glm::vec3(x, 1.0, z) * D * 0.1f;
                            count--; // count
                        }
                    }
                }
                emitter.spawnAcummulator = 0.0f;
            }
            
        }
        
        accumulatedTime = 0.0f;
        
    }
    
    
}

void physworld::debugDraw()
{
    for (const auto& emitter : emitters)
    {
        // draw emitter
        if (!emitter.enabled)
        {
            RenderClass::WhiteCube->draw(emitter.position, glm::vec3(0.3f), glm::vec3(0.5, 0.0, 0.0),0.5f, false, false);
            return;
        }
        RenderClass::WhiteCube->draw(emitter.position, glm::vec3(0.3f), glm::vec3(0.0, 0.5, 0.0),0.5f, false, false);       
        
        
        for (int i = 0; i < emitter.particles.size(); ++i)
        {
            if (emitter.particles[i].alive)
            {
                RenderClass::WhiteCube->draw(emitter.particles[i].position, emitter.particles[i].scale, glm::vec3(1.0, 1.0, 0.0),0.5f, false, false);
            }
        }
    }
}

void physworld::stimulateObject(object& physicsObject, glm::vec3& position, glm::vec3 gravity, float deltatime)
{
    if (physicsObject.hasRigidbody) // change name to has dynamics
    {

        if (physicsObject.affectedByGravity)
        {

            physicsObject.force += physicsObject.mass * gravity; // applying foce
        }

        physicsObject.velocity += physicsObject.force / physicsObject.mass * deltatime;
        position += physicsObject.velocity * deltatime;
        
        //std::cout << position.x << " " << position.y << " " << position.z << std::endl;

        physicsObject.force = glm::vec3(0.0f); // reset force at end
    }
}

void physworld::stimulateP_Objects(object* physicsObject, glm::vec3 &position, bool &dirtyFlag, glm::vec3 gravity, float deltatime)
{
    if (physicsObject->hasRigidbody) // change name to has dynamics
    {

        if (physicsObject->affectedByGravity)
        {

            physicsObject->force += physicsObject->mass * gravity; // applying foce
        }

        physicsObject->velocity += physicsObject->force / physicsObject->mass * deltatime;
        
        position += physicsObject->velocity * deltatime; // sync up position on entity end after

        physicsObject->force = glm::vec3(0.0f); // reset force at end
        
        
        dirtyFlag = true; // so the model will sync up position
    }
}

void physworld::addPhysicsObjectToArray(object* physicsObject, glm::vec3* position, bool* dirtyFlag)
{
    objectPosBundle nBundle;
    nBundle.physicsObject = physicsObject;
    nBundle.position = position;
    nBundle.dirtyFlag = dirtyFlag;
    
    pObjects.push_back(nBundle);
}

void physworld::physicsArrayDynamicUpdateLoop(glm::vec3 gravity, float deltatime)
{
    for (int i = 0; i < pObjects.size(); ++i)
    {
        physworld::stimulateP_Objects(pObjects[i].physicsObject, *pObjects[i].position,  *pObjects[i].dirtyFlag, gravity, deltatime);
    }
    
}

void physworld::bundleArrayDeleteWithUUID(uint64_t UUID)
{
    for (int i = 0; i < pObjects.size(); ++i)
    {
        if (pObjects[i].physicsObject->UUID = UUID)
        {
            pObjects.erase(pObjects.begin() + i);
            return; // erase and exit
        }
    }
}

void physworld::collisionUpdate()
{
    // some sort of collision objects, or collision types that all test agaisnt eachother and return data in some kind of format for a response 
}
