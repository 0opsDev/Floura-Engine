#include"CubeVisualizer.h"
#include "render/Shader/Cubemap.h"
#include "utils/timeUtil.h"
#include <glm/gtx/string_cast.hpp>
#include <Core/Render.h>

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
	float ScaleX, float ScaleY, float ScaleZ, glm::vec3 colour) {

	if (!RenderClass::DoForwardLightingPass && !RenderClass::DoDeferredLightingPass) {
		return; // Skip rendering if not in regular or lighting pass
	}

	if (Camera::isBoxInFrustum(glm::vec3(x,y,z), glm::vec3(ScaleX, ScaleY, ScaleZ) ) || ScaleX + ScaleY + ScaleZ >= 30) {
		if (RenderClass::DoForwardLightingPass) {
			glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);
			// Since the cubemap will always have a depth of 1.0, we need that equal sign so it doesn't get discarded
			glEnable(GL_DEPTH_TEST);
			glBindVertexArray(0);
			glLineWidth(1.0f); // Adjust the width as needed
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Enable wireframe mode
			//std::cout << "height" << height << std::endl;
			RenderClass::boxShader.Activate();

			glm::mat4 model = glm::mat4(1.0f);
			// Apply translation
			model = glm::translate(model, glm::vec3(x, y, z));
			// Apply scaling
			model = glm::scale(model, glm::vec3(ScaleX, ScaleY, ScaleZ));
			RenderClass::boxShader.setMat4("model", model);
			//feed model matrix known as inside the shader "model"
			RenderClass::boxShader.setMat4("camMatrix", Camera::cameraMatrix);

			glUniform3f(glGetUniformLocation(RenderClass::boxShader.ID, "camPos"), Camera::Position.x, Camera::Position.y, Camera::Position.z);

			glUniform3f(glGetUniformLocation(RenderClass::boxShader.ID, "colour"), colour.x, colour.y, colour.z);
			glBindVertexArray(cubeVAO);
			glDepthFunc(GL_ALWAYS);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			glLineWidth(1.0f); // Adjust the width as needed
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Restore normal rendering < wireframe
			glDepthFunc(GL_LESS);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);
			return;
		}

		// debug buffer would be cool, actually 3 debug buffers one for wireframe, another for hitboxes and the other for a polygon view simular to unreals
	}
}

void CubeVisualizer::Delete() {
	
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);
	glDeleteBuffers(1, &cubeEBO);
}
