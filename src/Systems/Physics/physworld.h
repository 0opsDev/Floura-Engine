#ifndef PHYS_WORLD_H
#define PHYS_WORLD_H

#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include "Systems/Physics/Collision.h"

class physworld
{
public:
    
    struct object
    {
        uint64_t UUID;
        float mass = 1.0f;
        // current motion
        glm::vec3 velocity = glm::vec3(0.0f);
        // applied motion
        glm::vec3 force = glm::vec3(0.0f);

        // flags
        bool affectedByGravity = false;
        bool hasRigidbody = true;
    };
    
    struct particle
    {
        object physicsObject;
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 scale = glm::vec3(0.1f);
        float age = 0.0f;
        bool alive = false;
    };
    
    struct emitter
    {
        object templatePhysicsObject; // for the particles
        std::vector<particle> particles;
        uint64_t UUID = 0;
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 gravity = glm::vec3(0.0f, -9.81f, 0.0f);
        float lifespan = 10.0f;
        float spawnTickrate = 1.0f;
        float spawnAcummulator = 1.0f;
        float maxDistance = 20.0f;
        int amount = 0;
        int limit = 32;
        bool killOnLimit = true;
        bool killOutOfDistance = true;
        bool enabled = false;
    };
    
private:
    
    struct objectPosBundle
    {
        object* physicsObject;
        glm::vec3* position;
        bool* dirtyFlag;
    };
    
public:
    
    static std::vector<objectPosBundle> pObjects;
    
    static std::vector<emitter> emitters;
    static float tickrate;
    
    static glm::vec3 worldGravity; // = glm::vec3(0.0f, -9.81f, 0.0f);
    
    static void uploadEmitter(emitter emitter);
    
    static void fillEmitterParticleBuffer(int index);
    
    static void killParticle(emitter& emitter, int index);
    
    static void update(float deltatime);
    
    static void debugDraw();
    
    static void stimulateObject(object& physicsObject, glm::vec3& position,  glm::vec3 gravity, float deltatime);

    static void addPhysicsObjectToArray(object* physicsObject, glm::vec3* position, bool* dirtyFlag);
    
    static void physicsArrayDynamicUpdateLoop(glm::vec3 gravity, float deltatime);
    
    static void bundleArrayDeleteWithUUID(uint64_t UUID);
    
    // collisions section
    
    static void collisionUpdate();
    
private:
    static float accumulatedTime;
    static void stimulateP_Objects(object* physicsObject, glm::vec3& position, bool &dirtyFlag, glm::vec3 gravity, float deltatime);
};

#endif