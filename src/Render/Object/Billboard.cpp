#include"Billboard.h"
#include "render/Shader/Cubemap.h"
#include "utils/timeUtil.h"
#include <glm/gtx/string_cast.hpp>
#include <Render/Shader/Framebuffer.h>
#include <Core/Render.h>
#include <Render/passes/geometry/geometryPass.h>

float s_Plane_Vertices[] = {
	// Positions       // Texture Coordinates
	-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,  // Bottom-left
	 1.0f, -1.0f, 0.0f,  1.0f, 0.0f,  // Bottom-right
	 1.0f,  1.0f, 0.0f,  1.0f, 1.0f,  // Top-right
	-1.0f,  1.0f, 0.0f,  0.0f, 1.0f   // Top-left
};

unsigned int s_Plane_Indices[6] =
{
	0, 1, 2, // First triangle
	2, 3, 0  // Second triangle
};

BillBoard::BillBoard(std::string path) {
	buffer(); // create buffer in memory for skybox

	//LoadBillBoardTexture(path);
	Tex.flipVertical = true;
	Tex.createTexture((path).c_str(), "diffuse", 0);
}

BillBoard::~BillBoard() {
	glDeleteBuffers(1, &cubeVBO);
	glDeleteBuffers(1, &cubeEBO);
	//glDeleteTextures(1, &BBTexture);
	glDeleteVertexArrays(1, &cubeVAO);
}

void BillBoard::buffer() {
	// Create VAO, VBO, and EBO for the billboard
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glGenBuffers(1, &cubeEBO);

	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(s_Plane_Vertices), &s_Plane_Vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_Plane_Indices), &s_Plane_Indices, GL_STATIC_DRAW);

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

void BillBoard::updatePosition(glm::vec3 Position)
{
	globalTransformation.position = Position;
}
void BillBoard::updateScale(glm::vec3 Scale)
{
	globalTransformation.scale = Scale;
}

void BillBoard::drawF(glm::mat4 modelMatrix, Shader shader, glm::mat4 camMatrix)
{
	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	shader.Activate(); // s
	// Pass transformations to shader
	shader.setMat4("model", modelMatrix);
	shader.setMat4("camMatrix", camMatrix);

	// Render the billboard
	glBindVertexArray(cubeVAO);
	Tex.Bind();
	shader.setInt("texture0", 0);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0); // E
}

void BillBoard::draw() {
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	if (!RenderClass::DoForwardLightingPass && !RenderClass::DoDeferredLightingPass) {
		return; // Skip rendering if not in regular or lighting pass
	}
	// Compute the forward vector towards the camera
	glm::vec3 camForward = glm::normalize(Camera::Position - globalTransformation.position);

	// Lock pitch if `doPitch == false`
	if (!doPitch) {
		camForward.y = 0.0f;
		camForward = glm::normalize(camForward);
	}

	// Compute right & up vectors
	glm::vec3 camRight = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), camForward));
	glm::vec3 camUp = glm::normalize(glm::cross(camForward, camRight));

	// Construct billboard rotation matrix
	glm::mat4 billboardRotation = glm::mat4(glm::mat3(camRight, camUp, camForward));

	// Apply transformations: translation -> rotation -> scale
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, globalTransformation.position);
	model = model * billboardRotation;  // Ensure billboard rotation before scaling
	model = glm::scale(model, globalTransformation.scale);

	if (RenderClass::DoForwardLightingPass) {
		glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Restore normal rendering < wireframe

		

		drawF(model, RenderClass::billBoardShader, Camera::cameraMatrix);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

		//
		//
		// Gpass
		glBindFramebuffer(GL_FRAMEBUFFER, 0); 
		glBindFramebuffer(GL_FRAMEBUFFER, GeometryPass::gBuffer);

		drawF(model, RenderClass::gPassShaderBillBoard, Camera::cameraMatrix);

		glDisable(GL_CULL_FACE); 
		glBindFramebuffer(GL_FRAMEBUFFER, 0); 
		//FrameBuffer

	// got just to reset the framebuffer to default
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);
}