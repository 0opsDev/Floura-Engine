#include"CubeVisualizer.h"
#include <utils/SettingsUtil.h>
#include <utils/UF.h>
#include "render/Shader/Cubemap.h"
#include "utils/timeUtil.h"
#include <glm/gtx/string_cast.hpp>

Shader boxShader("skip", "");

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
	boxShader = Shader("Shaders/Lighting/Default.vert", "Shaders/Db/OrangeHitbox.frag");
	boxShader.Activate();
	glUniform1i(glGetUniformLocation(boxShader.ID, "skybox"), 0);
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

void CubeVisualizer::draw(Camera& camera, GLfloat skyRGBA[], float x, float y, float z,
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
	glUniformMatrix4fv(glGetUniformLocation(boxShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
	//feed model matrix known as inside the shader "model"

	glUniformMatrix4fv(glGetUniformLocation(boxShader.ID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(camera.cameraMatrix));
	glUniform3f(glGetUniformLocation(boxShader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);

	glBindVertexArray(cubeVAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glLineWidth(1.0f); // Adjust the width as needed
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Restore normal rendering < wireframe
}

void CubeVisualizer::Delete() {
	boxShader.Delete();
}
