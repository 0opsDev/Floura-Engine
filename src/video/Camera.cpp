#include "Camera.h"

bool MouseState = true;
float timeAccumulator = 0;
bool toggleESC = true;

Camera::Camera(int width, int height, glm::vec3 position)
{
    Camera::width = width;
    Camera::height = height;
    Position = position;
}

void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane)
{
    // Initializes matrices
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    // Makes camera look in the right direction from the right position
    view = glm::lookAt(Position, Position + Orientation, Up);
    // Adds perspective to the scene
    projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);

    cameraMatrix = projection * view;
}

void Camera::Matrix(Shader& shader, const char* uniform)
{
    // Exports the camera matrix to the Vertex Shader
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}
void Camera::Inputs(GLFWwindow* window, float deltaTime)
{
    float adjustedSpeed = speed * deltaTime;

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
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        Position += adjustedSpeed * Up;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        Position += adjustedSpeed * -Up;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        speed = 5.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
    {
        speed = 2.0f;
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
        float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
        float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

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
