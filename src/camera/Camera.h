#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "Render/shader/shaderClass.h"
#include "Core/Main.h"
#include "utils/timeUtil.h"

class Camera
{
public:
    // Stores camera transform
    float s_scrollSpeed = 5.0f;
    glm::vec2 sensitivity;
    glm::mat4 cameraMatrix = glm::mat4(1.0f);
    glm::vec3 Position = glm::vec3(0, 0, 0);
    glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
    bool firstClick = false;
    int width;
    int height;
    float speed = 0.1f;
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    float fov = 60;
    float gamma = 2.2f;
    

    // Camera constructor
    void InitCamera(int width, int height, glm::vec3 position);
    void SetViewportSize(int newWidth, int newHeight);
    // Updates and exports the camera matrix to the Vertex Shader
    void updateMatrix(float FOVdeg, float nearPlane, float farPlane);
    void Matrix(Shader& shader, const char* uniform);

    // Handles camera inputs
    void Inputs(GLFWwindow* window);

    bool isPointInFrustum(const glm::vec3& worldPos);

    bool isRadiusInFrustum(const glm::vec3& worldPos, const float radius);

    bool isBoxInFrustum(const glm::vec3& worldPos, const glm::vec3& Scale);

};

#endif