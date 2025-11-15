#include "CubeCollider.h"
#include <Gameplay/Player.h>
#include <Scene/scene.h>
#include <utils/logConsole.h>
#include "Collision.h"
bool CubeCollider::isCollide = false;

bool CubeCollider::CollideWithCamera = true;
bool CubeCollider::showBoxCollider = false;

void CubeCollider::update() {
    if (CollideWithCamera && enabled) {

        Collision::collisionData newData = Collision::AABBvsAABB(colliderXYZ, colliderScale, (glm::vec3(Camera::Position.x, (Camera::Position.y - (Camera::cameraColliderScale.y / 2.0f)), Camera::Position.z)), Camera::cameraColliderScale); // b is victim
        if (newData.isColliding)
        {
            Player::isColliding = true; // Set collision state
            Camera::Position = glm::vec3(newData.lastHit.x, (newData.lastHit.y + (Camera::cameraColliderScale.y / 2.0f)), newData.lastHit.z);
        }
    }
}

void CubeCollider::draw() {
    if (showBoxCollider) {
        if (isCollide) {// was 1.0, 0.412, 0.0 
            CubeVisualizerRenderObject->draw(colliderXYZ, colliderScale, glm::vec3(1.0, 0.0, 0.0));}
        else {CubeVisualizerRenderObject->draw(colliderXYZ, colliderScale, glm::vec3(1.0, 1.0, 1.0));}
    }
}

void CubeCollider::Delete() {
    delete CubeVisualizerRenderObject;
    CubeVisualizerRenderObject = nullptr;

    //update lowest free index
    if (ID.index < IdManager::lowestDeletedIndex.Collider || IdManager::lowestDeletedIndex.Collider == -1) {
        IdManager::lowestDeletedIndex.Collider = ID.index;
        LogConsole::print("Lowest Deleted Collider Index is now: " + std::to_string(IdManager::lowestDeletedIndex.Collider));
    }

    IdManager::RemoveID(ID);

    //IdManager::lowestColliderIndexSync(); // sync up the index after deletion because the array has now changed
}

void CubeCollider::init() {
    ID.ObjType = 'c';
    ID.index = Scene::CubeColliderObject.size();
    IdManager::AddID(ID);

    CubeVisualizerRenderObject = new CubeVisualizer;
}