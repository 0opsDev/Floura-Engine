#include"CubeVisualizer.h"
#include <utils/SettingsUtil.h>
#include "render/Shader/Cubemap.h"
#include "utils/timeUtil.h"
#include <glm/gtx/string_cast.hpp>

Shader boxShader;

float s_Cube_Vertices[24] =
{
	//   Coordinates
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f
};

unsigned int s_Cube_Indices[36] =
{
	// Right
	1, 2, 6,
	6, 5, 1,
	// Left
	0, 4, 7,
	7, 3, 0,
	// Top
	4, 5, 6,
	6, 7, 4,
	// Bottom
	0, 3, 2,
	2, 1, 0,
	// Back
	0, 1, 5,
	5, 4, 0,
	// Front
	3, 7, 6,
	6, 2, 3
};

void CubeVisualizer::init() {
	boxShader.LoadShader("Shaders/Lighting/Default.vert", "Shaders/Db/OrangeHitbox.frag");
	skyboxBuffer(); // create buffer in memory for skybox
}

void CubeVisualizer::skyboxBuffer() {
	// Create VAO, VBO, and EBO for the skybox
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glGenBuffers(1, &cubeEBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(s_Cube_Vertices), &s_Cube_Vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_Cube_Indices), &s_Cube_Indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void CubeVisualizer::draw(float x, float y, float z,
	float ScaleX, float ScaleY, float ScaleZ) {
	// Since the cubemap will always have a depth of 1.0, we need that equal sign so it doesn't get discarded
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);
	glLineWidth(5.0f); // Adjust the width as needed
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Enable wireframe mode
	//std::cout << "height" << height << std::endl;
	boxShader.Activate();

	glm::mat4 model = glm::mat4(1.0f);
	// Apply translation
	model = glm::translate(model, glm::vec3(x, y, z));
	// Apply scaling
	model = glm::scale(model, glm::vec3(ScaleX, ScaleY, ScaleZ));
	boxShader.setMat4("model",model);
	//feed model matrix known as inside the shader "model"
	boxShader.setMat4("camMatrix", Camera::cameraMatrix);
	glUniform3f(glGetUniformLocation(boxShader.ID, "camPos"), Camera::Position.x, Camera::Position.y, Camera::Position.z);

	glBindVertexArray(cubeVAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glLineWidth(1.0f); // Adjust the width as needed
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Restore normal rendering < wireframe
	glDepthFunc(GL_LESS);
}

void CubeVisualizer::Delete() {
	boxShader.Delete();
}
