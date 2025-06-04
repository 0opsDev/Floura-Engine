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
    static glm::vec3 Position;
    static bool DoJump;
    static glm::vec3 Orientation;
    static glm::vec3 Up;
    static bool s_DoGravity;
    static glm::mat4 cameraMatrix;
    static float s_scrollSpeed;
    static float s_sensitivityX;
    static float s_sensitivityY;
    static float PlayerHeightCurrent;
    static glm::mat4 view;
    static glm::mat4 projection;
    static float fov;

    // Prevents the camera from jumping around when first clicking left click
    static bool firstClick;
    // Camera resolution
    static int width;
    static int height;
    static float speed;
    

    // Camera constructor
    void static InitCamera(int width, int height, glm::vec3 position);
    void static SetViewportSize(int newWidth, int newHeight);
    // Updates and exports the camera matrix to the Vertex Shader
    void static updateMatrix(float FOVdeg, float nearPlane, float farPlane);
    void static Matrix(Shader& shader, const char* uniform);

    // Handles camera inputs
    void static Inputs(GLFWwindow* window);

    static bool isPointInFrustum(const glm::vec3& worldPos);

    static bool isRadiusInFrustum(const glm::vec3& worldPos, const float radius);

    static bool isBoxInFrustum(const glm::vec3& worldPos, const glm::vec3& Scale);

};

#endif