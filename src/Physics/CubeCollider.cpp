#include "CubeCollider.h"
#include <Gameplay/Player.h>

bool CubeCollider::isCollide = false;

bool CubeCollider::checkcollide(glm::vec3 victimXYZ, glm::vec3 victimScale) {
    glm::vec3 colliderMin = colliderXYZ - colliderScale;
    glm::vec3 colliderMax = colliderXYZ + colliderScale;
    glm::vec3 victimMin = victimXYZ - victimScale * 0.5f;
    glm::vec3 victimMax = victimXYZ + victimScale * 0.5f;

    bool overlapX = (victimMax.x >= colliderMin.x) && (victimMin.x <= colliderMax.x);
    bool overlapY = (victimMax.y >= colliderMin.y) && (victimMin.y <= colliderMax.y);
    bool overlapZ = (victimMax.z >= colliderMin.z) && (victimMin.z <= colliderMax.z);

    if (overlapX && overlapY && overlapZ) {
        // Determine the closest face and push the victim fully outside
        float leftDist = abs(victimMax.x - colliderMin.x);
        float rightDist = abs(victimMin.x - colliderMax.x);
        float bottomDist = abs(victimMax.y - colliderMin.y);
        float topDist = abs(victimMin.y - colliderMax.y);
        float frontDist = abs(victimMax.z - colliderMin.z);
        float backDist = abs(victimMin.z - colliderMax.z);

        float minDist = std::min({ leftDist, rightDist, bottomDist, topDist, frontDist, backDist });

        if (minDist == leftDist) lastHit = glm::vec3(colliderMin.x - victimScale.x * 0.5f, victimXYZ.y, victimXYZ.z);
        else if (minDist == rightDist) lastHit = glm::vec3(colliderMax.x + victimScale.x * 0.5f, victimXYZ.y, victimXYZ.z);
        else if (minDist == bottomDist) lastHit = glm::vec3(victimXYZ.x, colliderMin.y - victimScale.y * 0.5f, victimXYZ.z);
        else if (minDist == topDist) lastHit = glm::vec3(victimXYZ.x, colliderMax.y + victimScale.y * 0.5f, victimXYZ.z);
        else if (minDist == frontDist) lastHit = glm::vec3(victimXYZ.x, victimXYZ.y, colliderMin.z - victimScale.z * 0.5f);
        else if (minDist == backDist) lastHit = glm::vec3(victimXYZ.x, victimXYZ.y, colliderMax.z + victimScale.z * 0.5f);
        isCollide = true;
        return true;
    }
    isCollide = false;
    return false;
}

bool CubeCollider::CollideWithCamera = true;
bool CubeCollider::showBoxCollider = false;

void CubeCollider::update() {
    if (CollideWithCamera) {
        if (checkcollide((glm::vec3(Camera::Position.x, (Camera::Position.y - (Camera::PlayerHeightCurrent / 2.0f)), Camera::Position.z)), glm::vec3(1, (Camera::PlayerHeightCurrent), 1))) {
			Player::isColliding = true; // Set collision state
            Camera::Position = glm::vec3(lastHit.x, (lastHit.y + (Camera::PlayerHeightCurrent / 2.0f)), lastHit.z);
        }
    }
}

void CubeCollider::draw() {
    if (showBoxCollider) {
        if (isCollide) {// was 1.0, 0.412, 0.0 
            CubeVisualizerRenderObject.draw(colliderXYZ.x, colliderXYZ.y, colliderXYZ.z, colliderScale.x, colliderScale.y, colliderScale.z, glm::vec3(1.0, 0.0, 0.0));}
        else {CubeVisualizerRenderObject.draw(colliderXYZ.x, colliderXYZ.y, colliderXYZ.z, colliderScale.x, colliderScale.y, colliderScale.z, glm::vec3(1.0, 1.0, 1.0));}   
    }
}

void CubeCollider::Delete() {
    CubeVisualizerRenderObject.Delete();
}

void CubeCollider::init() {
    CubeVisualizerRenderObject.init();
}