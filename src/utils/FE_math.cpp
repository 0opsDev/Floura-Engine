#include "FE_math.h"
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

bool FE_Math::isInRange(glm::vec3 victimPosition, glm::vec3 viewPosition, float range) {
	float distance = glm::distance(victimPosition, viewPosition);

	return distance <= range;
}

// goes towards min and max depending on distance from range
float FE_Math::GetProximityValueLinear(const glm::vec3& object_position, const glm::vec3& camera_position, float radius, float min, float max) {
    if (radius <= 0.0f) {
        return max;
    }

    float current_distance = glm::distance(object_position, camera_position);

    current_distance = std::clamp(current_distance, 0.0f, radius);

    float normalized_t = current_distance / radius;

    float linear_interpolator = 1.0f - normalized_t;

    float result = min + linear_interpolator * (max - min);

    return std::clamp(result, min, max);
}

glm::mat4 FE_Math::composeMatrix(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation) {// takes rads for degrees
    glm::mat4 mTransform = glm::mat4(1.0f);
    glm::mat4 mRotation = glm::mat4(1.0f);
    glm::mat4 mScale = glm::mat4(1.0f);

    mTransform = glm::translate(mTransform, position);
    
    /*
        mRotation = glm::rotate(mRotation, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    mRotation = glm::rotate(mRotation, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    mRotation = glm::rotate(mRotation, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    
    */


    glm::quat q = glm::quat(glm::radians(rotation));
    mRotation = glm::toMat4(q);

    mScale = glm::scale(mScale, scale);

    return (mTransform * mRotation * mScale);
}

float FE_Math::calculateCenter1D(glm::vec2 input){
    return (input.x + input.y) / 2.0f;
}

float FE_Math::distanceFromTwoPoints1D(glm::vec2 input){
    return glm::abs(input.x - input.y);
}

float FE_Math::furthestPoint(glm::vec2 input){
    return glm::max(input.x, input.y);
}

float FE_Math::magnitude(glm::vec3 v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}


float FE_Math::AreaOfTriangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3){
    // 2 vectors from points
    glm::vec3 u = p2 - p1;
    glm::vec3 v = p3 - p1;

    // cross product of u and v
    glm::vec3 cp = glm::cross(u, v);

    // half of magnitude of the vector cp
    return glm::length(cp) * 0.5f;
}


// pretty sure i have something like this in collision, but this is for uv for its fine, its that barycentric triangle collision functuion, but it pulls uv
glm::vec2 FE_Math::uvPosFromVertexAndPoint(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c, const glm::vec2 &aUV, const glm::vec2 &bUV, const glm::vec2 &cUV, const glm::vec3 &p){
    // calc areas
    float totalArea = AreaOfTriangle(a, b, c);
    float areaPAB = AreaOfTriangle(p, a, b);
    float areaPBC = AreaOfTriangle(p, b, c);
    float areaPCA = AreaOfTriangle(p, c, a);
    
    // sum of each area
    float areasum = areaPAB + areaPBC + areaPCA;
    float elipson = 0.0001f * totalArea;
    
    // if point inside elipson
    if (glm::abs(totalArea - areasum) < elipson){
        // calc weight
        float weightA = areaPBC / totalArea;
        float weightB = areaPCA / totalArea;
        float weightC = areaPAB / totalArea;
        
        return glm::vec2( (weightA * aUV) + (weightB * bUV) + (weightC * cUV) );
    }
    return glm::vec2(0.0f); 
}

glm::vec3 FE_Math::RadiansToNormal(float yawRad, float pitchRad){
    glm::vec3 normal;
    normal.x = cos(pitchRad) * sin(yawRad);
    normal.y = sin(pitchRad);
    normal.z = cos(pitchRad) * cos(yawRad);
    return glm::normalize(normal);
}

void FE_Math::NormalToRadians(glm::vec3 normal, float& yawRad, float& pitchRad){
    normal = glm::normalize(normal);

    pitchRad = asin(normal.y);
    yawRad = atan2(normal.x, normal.z);
}

glm::vec3 FE_Math::getForwardFromMatrix(const glm::mat4& matrix) {
    return glm::normalize(glm::vec3(-matrix[2][0], -matrix[2][1], -matrix[2][2]));
}

glm::vec3 FE_Math::getForwardFromViewMatrix(const glm::mat4& viewMatrix) {
    return glm::normalize(glm::vec3(-viewMatrix[0][2], -viewMatrix[1][2], -viewMatrix[2][2]));
}

glm::vec2 FE_Math::findTwoFurthestPointsX(std::vector<glm::vec3> points){
    // max and min points
    glm::vec3 minX = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 maxX = glm::vec3(std::numeric_limits<float>::lowest());


    for (size_t x = 0; x < points.size(); x++){
        minX = (points[x].x < minX.x) ? points[x] : minX;
        maxX = (points[x].x > maxX.x) ? points[x] : maxX;
    }
    return glm::vec2(minX.x, maxX.x);
}

glm::vec2 FE_Math::findTwoFurthestPointsY(std::vector<glm::vec3> points){
    glm::vec3 minY = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 maxY = glm::vec3(std::numeric_limits<float>::lowest());
    for (size_t x = 0; x < points.size(); x++){
        minY = (points[x].y < minY.y) ? points[x] : minY;
        maxY = (points[x].y > maxY.y) ? points[x] : maxY;
    }
    return glm::vec2(minY.y, maxY.y);
}
glm::vec2 FE_Math::findTwoFurthestPointsZ(std::vector<glm::vec3> points){
    glm::vec3 minZ = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 maxZ = glm::vec3(std::numeric_limits<float>::lowest());
    for (size_t x = 0; x < points.size(); x++)
    {
        minZ = (points[x].z < minZ.z) ? points[x] : minZ;
        maxZ = (points[x].z > maxZ.z) ? points[x] : maxZ;
    }
    return glm::vec2(minZ.z, maxZ.z);
}


void FE_Math::transformPoint(glm::vec3& point, glm::mat4 matrix){
    glm::vec4 transformed = matrix * glm::vec4(point, 1.0f);
    point = glm::vec3(transformed);
}
// version that doesnt use &
glm::vec3 FE_Math::transformPointReturn(glm::vec3 point, glm::mat4 matrix){
    glm::vec4 transformed = matrix * glm::vec4(point, 1.0f);
    return glm::vec3(transformed);
}



glm::vec3 FE_Math::pad(glm::vec3 value, float padding){
    glm::vec3 newPad = value;
    if (value.x < padding)
        newPad.x = padding;
    if (value.y < padding)
        newPad.y = padding;
    if (value.z < padding)
        newPad.z = padding;

    return newPad;
}

glm::vec3 FE_Math::normalFrom2Points(glm::vec3 p1, glm::vec3 p2){
    // direction vector
    glm::vec3 v = glm::normalize(p2 - p1);

    // vec that isnt parallel to v
    glm::vec3 up = (std::abs(v.y) > 0.9f) ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f);

    // right vector
    glm::vec3 r = glm::normalize(glm::cross(v, up));
    
    // normal vector
    return glm::normalize(glm::cross(r, v));
}

glm::mat4 FE_Math::composeMatrixWDegrees(glm::vec3 p, glm::vec3 s, glm::vec3 r){
    glm::mat4 np = glm::translate(glm::mat4(1.0f), p);
    glm::mat4 nr = glm::mat4(1.0f);
    nr = glm::rotate(nr, glm::radians(r.x), glm::vec3(1, 0, 0));
    nr = glm::rotate(nr, glm::radians(r.y), glm::vec3(0, 1, 0));
    nr = glm::rotate(nr, glm::radians(r.z), glm::vec3(0, 0, 1));
    glm::mat4 ns = glm::scale(glm::mat4(1.0f), s);

    return np * nr * ns;
}

glm::mat4 FE_Math::composeMatrixwQuaternion(glm::vec3 p, glm::vec3 s, glm::quat r){
    glm::mat4 np = glm::translate(glm::mat4(1.0f), p);
    glm::mat4 nr = glm::mat4_cast(glm::normalize(r));
    glm::mat4 ns = glm::scale(glm::mat4(1.0f), s);
	return np * nr * ns;
}

int FE_Math::floatToByteRGB(float input){
    return (input) >= 1.0 ? 255 : (int)((input) * 256.0);
}

float FE_Math::ByteToFloatRGB(int input){
    return input / 255.0f;
}

glm::mat4 FE_Math::createHaltonJitterProjectionMatrix(glm::mat4 matrix, glm::vec2 jitter, int height, int width){
    float nJitterX = (jitter.x * 2.0f) / static_cast<float>(width);
    float nJitterY = (jitter.y * 2.0f) / static_cast<float>(height);
    
    matrix[2][0] += nJitterX;
    matrix[2][1] += nJitterY;
    
    return matrix;
}

int FE_Math::calculateLODLevel(glm::vec3 vPosition, glm::vec3 cameraPosition, float transitionDistance, int maxLOD){
    float distance = glm::distance(vPosition, cameraPosition);
    int targetLOD = static_cast<int>(distance / transitionDistance);
    targetLOD = std::min(targetLOD, maxLOD);
    
    return targetLOD;
}

float FE_Math::normalizeScale(glm::vec3 size, float target){
    float longestAxis = glm::max(size.x, glm::max(size.y, size.z));
    return target / glm::max(longestAxis, 0.00001f); // prevent div by zero
}

float FE_Math::normalizeFloat(float v, float min, float max){
    if (min == max) return 0.0;
    return glm::clamp((v -min) / (max - min), 0.0f, 1.0f);
}

glm::quat FE_Math::vec3DegreesToQuat(glm::vec3 degrees){
    glm::vec3 r = glm::radians(degrees);
    glm::quat q = glm::quat(r);
    return q;
}