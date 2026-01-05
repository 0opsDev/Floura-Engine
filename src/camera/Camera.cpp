#include "Camera.h"
#include "Core/Main.h"
#include <glm/gtx/string_cast.hpp>

// Global Variables
bool MouseState = true, toggleESC = true;
float timeAccumulator = 0;

void Camera::InitCamera(int width, int height, glm::vec3 position)
{
    Camera::width = width;
    Camera::height = height;
    Position = position;
}

void Camera::SetViewportSize(int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;
}

void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane)
{
    //std::cout << "Internal camera instance address: " << this << std::endl;
    // Initializes matrices

    // Makes camera look in the right direction from the right position
    view = glm::lookAt(Position, Position + Orientation, Up);
    fov = FOVdeg;
    // Adds perspective to the scene
    projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);

    //std::cout << "Projection matrix: " << glm::to_string(projection) << std::endl;
    cameraMatrix = projection * view;
}

void Camera::Matrix(Shader& shader, const char* uniform)
{
    // Exports the camera matrix to the Vertex Shader
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}
void Camera::Inputs(GLFWwindow* window)
{
    float adjustedSpeed = speed * TimeUtil::deltatime;
	if (true)
	{
        // Handles inputs
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            Position += adjustedSpeed * Orientation;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            Position += adjustedSpeed * -glm::normalize(glm::cross(Orientation, Up));
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            Position += adjustedSpeed * -Orientation;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            Position += adjustedSpeed * glm::normalize(glm::cross(Orientation, Up));
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) //jump
        {
            Position += adjustedSpeed * Up;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        {
            Position += adjustedSpeed * -Up;
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            s_scrollSpeed += 10.0f * TimeUtil::deltatime;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            s_scrollSpeed -= 10.0f * TimeUtil::deltatime;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            speed = (10.0f + s_scrollSpeed);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        {
            speed = (5.0f + s_scrollSpeed);
        }
	}

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        if (toggleESC) {
            MouseState = !MouseState;
            toggleESC = false;
        }

    }
    else
    {
        toggleESC = true;
    }
    // Handles mouse inputs
    if (MouseState)
    {
        // Disables mouse cursor and prevents it from leaving the window
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // Prevents camera from jumping on the first click
        if (firstClick)
        {
            glfwSetCursorPos(window, (width / 2), (height / 2));
            firstClick = false;
        }

        // Stores the coordinates of the cursor
        double mouseX;
        double mouseY;
        // Fetches the coordinates of the cursor
        glfwGetCursorPos(window, &mouseX, &mouseY);

        // Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
        // and then "transforms" them into degrees 
        float rotX = Camera::sensitivity.y * (float)(mouseY - (height / 2)) / height;
        float rotY = Camera::sensitivity.x * (float)(mouseX - (width / 2)) / width;
        //float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
        //float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;
       
        // Calculates upcoming vertical change in the Orientation
        glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));


        // Decides whether or not the next vertical Orientation is legal or not
        if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
        {
            Orientation = newOrientation;
        }

        // Rotates the Orientation left and right
        Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);

        // Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
        glfwSetCursorPos(window, (width / 2), (height / 2));
    }
    else
    {
        // Unhides cursor since camera is not looking around anymore
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        // Makes sure the next time the camera looks around it doesn't jump
        firstClick = true;
    }
}