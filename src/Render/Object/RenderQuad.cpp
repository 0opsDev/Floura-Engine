#include"RenderQuad.h"
#include "utils/timeUtil.h"
#include <glm/gtx/string_cast.hpp>

const float s_Plane_Verticez[] = {
	// Positions       // Texture Coordinates
	-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,  // Bottom-left
	 1.0f, -1.0f, 0.0f,  1.0f, 0.0f,  // Bottom-right
	 1.0f,  1.0f, 0.0f,  1.0f, 1.0f,  // Top-right
	-1.0f,  1.0f, 0.0f,  0.0f, 1.0f   // Top-left
};

const unsigned int s_Plane_Indicez[6] =
{
	0, 1, 2, // First triangle
	2, 3, 0  // Second triangle
};

void RenderQuad::init() {
	buffer(); // create buffer in memory for skybox
}

void RenderQuad::buffer() {
	// Create VAO, VBO, and EBO for the billboard
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(s_Plane_Verticez), &s_Plane_Verticez, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
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
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
}
