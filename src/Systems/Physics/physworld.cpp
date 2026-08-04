#include "physworld.h"
#include "utils/timeUtil.h"
#include <Render/Handler/RenderClass.h>
#include "Render/Handler/RenderHandler.h"
#include <random>

#include "Gameplay/Player.h"
#include "Render/Handler/CubeVisualizer.h"
#include "Scene/scene.h"
#include "utils/FE_math.h"

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
    for (auto& emitter : emitters){
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

void physworld::debugDraw(){
    for (const auto& emitter : emitters)
    {
        // draw emitter
        if (!emitter.enabled){
            CubeVisualizer::draw(emitter.position, glm::vec3(0.3f), glm::vec3(0.5, 0.0, 0.0),0.5f, false, false);
            return;
        }
        CubeVisualizer::draw(emitter.position, glm::vec3(0.3f), glm::vec3(0.0, 0.5, 0.0),0.5f, false, false);       
        
        
        for (int i = 0; i < emitter.particles.size(); ++i){
            if (emitter.particles[i].alive){
                CubeVisualizer::draw(emitter.particles[i].position, emitter.particles[i].scale, glm::vec3(1.0, 1.0, 0.0),0.5f, false, false);
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
    for (int i = 0; i < pObjects.size(); ++i){
        physworld::stimulateP_Objects(pObjects[i].physicsObject, *pObjects[i].position,  *pObjects[i].dirtyFlag, gravity, deltatime);
    }
    
}

void physworld::bundleArrayDeleteWithUUID(uint64_t UUID){
    for (int i = 0; i < pObjects.size(); ++i){
        if (pObjects[i].physicsObject->UUID == UUID){
            //std::cout << "match: " <<UUID << std::endl;
            pObjects.erase(pObjects.begin() + i);
            return; // erase and exit
        }
    }
}

/*
for (int a = 0; a < pObjects.size(); ++a){
    if (!pObjects[a].physicsObject->collisionObject.isCollider || pObjects[a].physicsObject->collisionObject.type != Collision::typeAABB) continue; // neither is collider
   
    Collision::HitResult rc = Collision::AABBvsAABB(pObjects[a].physicsObject->collisionObject.aabb.position, pObjects[a].physicsObject->collisionObject.aabb.size, 
                                                                            Scene::maincamera.Position, Player::cameraColliderScale);
    if (!rc.isColliding) continue;
    std::cout << "agg" <<  std::endl; 
}
*/
void physworld::collisionResolveCamera(){
    for (int a = 0; a < Scene::entityObjects.size(); ++a){
        
        //if (!Scene::entityObjects[a]->component.flags.hasCollider) continue; // neither is collider
        
        glm::mat4 gModelMatrix = FE_Math::composeMatrixWDegrees(Scene::entityObjects[a]->component.systems.transformation.position, Scene::entityObjects[a]->component.systems.transformation.scale, Scene::entityObjects[a]->component.systems.transformation.rotation);
        
        int index = RenderHandler::fetchModelIndex(Scene::entityObjects[a]->component.render.renderID);
        if (index <= -1) continue;

        //Scene::entityObjects[a]->component.collider.rootnodes
        for (int i = 0; i < Scene::entityObjects[a]->component.collider.rootnodes.size(); ++i){
            Collision::HitResult rc = Collision::AABBvsAABB(Scene::entityObjects[a]->component.collider.rootnodes[i].position, FE_Math::pad(Scene::entityObjects[a]->component.collider.rootnodes[i].size, 0.5f), Scene::maincamera.Position, Player::cameraColliderScale);
            if (!rc.isColliding) continue;
            glm::mat4 tMatrix = gModelMatrix * RenderHandler::models[index].model->lModelMatrix[i];
            for (int j = 0; j < RenderHandler::models[index].model->meshes[i].blas.back().prims.size(); ++j){
                Collision::AABB nprimAABB = Collision::rootNodeFromRubixPointsNoPadding(Collision::aabbToRubixCubePoints(
                    RenderHandler::models[index].model->meshes[i].blas.back().prims[j].extents.position, RenderHandler::models[index].model->meshes[i].blas.back().prims[j].extents.size), tMatrix);
                
                Collision::HitResult rc2 = Collision::AABBvsAABB(nprimAABB.position, FE_Math::pad(nprimAABB.size, 0.1f), Scene::maincamera.Position, Player::cameraColliderScale);
                if (!rc2.isColliding) continue;

                Scene::maincamera.Position += rc2.collisionNormal * rc2.depth; // Scene::maincamera.Position + 
                Player::velocity = glm::vec3(0.0f);
                /*
                const unsigned int &i0 =RenderHandler::models[index].model->meshes[i].blas.back().prims[j].i0;
                const unsigned int &i1 = RenderHandler::models[index].model->meshes[i].blas.back().prims[j].i1;
                const unsigned int &i2 = RenderHandler::models[index].model->meshes[i].blas.back().prims[j].i2;
                
                std::vector<Vertex> &nVertices = RenderHandler::models[index].model->meshes[i].vertices;
				
                if (i0 >= nVertices.size() ||
                    i1 >= nVertices.size() ||
                    i2 >= nVertices.size())
                    continue;
				
                const Vertex* a = &nVertices[i0];
                const Vertex* b = &nVertices[i1];
                const Vertex* c = &nVertices[i2];
                //tMatrix
                Collision::HitResult trihit = Collision::SATTriangleVSAABB(FE_Math::transformPointReturn(a->position, tMatrix), FE_Math::transformPointReturn(b->position, tMatrix), FE_Math::transformPointReturn(c->position, tMatrix),
                    Scene::maincamera.Position, Player::cameraColliderScale);
                //Collision::HitResult trihit = Collision::NearestPointTriangleVSAABB(FE_Math::transformPointReturn(a->position, tMatrix), FE_Math::transformPointReturn(b->position, tMatrix), FE_Math::transformPointReturn(c->position, tMatrix),
                //   Scene::maincamera.Position, Player::cameraColliderScale);
                if (!trihit.isColliding) return;
                
                    Scene::maincamera.Position += trihit.collisionNormal * trihit.depth; // Scene::maincamera.Position + 
                    Player::velocity = glm::vec3(0.0f);
                
                std::cout << "agg" <<  std::endl;
                //Scene::maincamera.Position += rc2.collisionNormal * rc2.depth; // Scene::maincamera.Position + 
                //Player::velocity = glm::vec3(0.0f);
                */
            }
        }
    }
}

void physworld::collisionResolve(){
    return; // disabled
    // some sort of collision objects, or collision types that all test agaisnt eachother and return data in some kind of format for a response 
    for (int a = 0; a < pObjects.size(); ++a)
        for (int b = 0; b < pObjects.size(); ++b)
        {
            // if same object we break
            if (pObjects[a].physicsObject->UUID == pObjects[b].physicsObject->UUID) break;
            
            if (!pObjects[a].physicsObject->collisionObject.isCollider || !pObjects[b].physicsObject->collisionObject.isCollider) continue; // neither is collider
            
            // id like to do col test here, i guess i should store collider obj on the phys obj

            //Collision::HitResult rc = Collision::resolveCollisionW_NewPositons(pObjects[a].physicsObject->collisionObject, pObjects[b].physicsObject->collisionObject,  *pObjects[a].position, *pObjects[b].position);
            Collision::HitResult rc = Collision::resolveCollision(pObjects[a].physicsObject->collisionObject, pObjects[b].physicsObject->collisionObject);
            
            // plug into solver and do some stuff idk
            if (!rc.isColliding) continue; // if we're not colliding there is nothing to solve
            //std::cout << "agg" <<  std::endl;
            
            // debug
            //std::cout << "A index: " << a << " + b index: " << b << " COLLIDING" << std::endl;
            // should plug in a
            solve(rc, *pObjects[a].physicsObject, *pObjects[a].position, *pObjects[b].physicsObject, *pObjects[b].position, *pObjects[b].dirtyFlag, *pObjects[a].dirtyFlag);
            
        }
    
}

void physworld::solve(Collision::HitResult& hr, object& objA, glm::vec3& posA, object& objB, glm::vec3& posB, bool& dirtyFlagb, bool& dirtyFlaga )
{
}
