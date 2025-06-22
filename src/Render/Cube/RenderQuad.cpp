#include"RenderQuad.h"
#include <utils/SettingsUtil.h>
#include "render/Shader/Cubemap.h"
#include "utils/timeUtil.h"
#include <glm/gtx/string_cast.hpp>

float s_Plane_Verticez[] = {
	// Positions       // Texture Coordinates
	-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,  // Bottom-left
	 1.0f, -1.0f, 0.0f,  1.0f, 0.0f,  // Bottom-right
	 1.0f,  1.0f, 0.0f,  1.0f, 1.0f,  // Top-right
	-1.0f,  1.0f, 0.0f,  0.0f, 1.0f   // Top-left
};

unsigned int s_Plane_Indicez[6] =
{
	0, 1, 2, // First triangle
	2, 3, 0  // Second triangle
};

void RenderQuad::init() {
	skyboxBuffer(); // create buffer in memory for skybox

	//LoadBillBoardTexture(path);
}

void RenderQuad::skyboxBuffer() {
	// Create VAO, VBO, and EBO for the billboard
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glGenBuffers(1, &cubeEBO);

	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(s_Plane_Verticez), &s_Plane_Verticez, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_Plane_Indicez), &s_Plane_Indicez, GL_STATIC_DRAW);

	// **Position attribute**
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// **Texture coordinate attribute**
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void RenderQuad::draw(Shader shader) {
		// Enable depth testing
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		// Activate shader
		shader.Activate();

		// Apply transformations: translation -> rotation -> scale
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0, 2, 0));
		model = model;
		model = glm::scale(model, glm::vec3(1));

		shader.Activate();
		// Pass transformations to shader
		shader.setMat4("model", model);

		shader.setMat4("camMatrix", Camera::cameraMatrix);

		//glBindTexture(GL_TEXTURE_2D, 0); // unbind tex
		// Render the billboard
		glBindVertexArray(cubeVAO);
		//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
}
