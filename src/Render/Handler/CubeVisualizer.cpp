#include"CubeVisualizer.h"
#include <glm/gtx/string_cast.hpp>
#include <Render/Handler/RenderClass.h>
#include "Scene/scene.h"
#include  "Render/pipeline/prebuilt_pipelines/dbgPass.h"


const float CubeVisualizer::s_Cube_Vertices[24] =
{	//   Coordinates
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f
};

const unsigned int CubeVisualizer::s_Cube_Indices[36] =
{	// Right
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

unsigned int CubeVisualizer::cubeVAO; 
unsigned int CubeVisualizer::cubeVBO;
unsigned int CubeVisualizer::cubeEBO;
unsigned int CubeVisualizer::instanceVBO;

std::vector<CubeVisualizer::drawCommand> CubeVisualizer::drawsList;
std::vector<CubeVisualizer::drawCommand> CubeVisualizer::wireframeDrawList;

Shader CubeVisualizer::boxShader;

void CubeVisualizer::init() {
	
	boxShader.LoadShader("Assets/Shaders/Lighting/Default.vert", "Assets/Shaders/Db/OrangeHitbox.frag");
	
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glGenBuffers(1, &cubeEBO);
	glGenBuffers(1, &instanceVBO);
	
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

void CubeVisualizer::cleanup()
{
	boxShader.Delete();
	
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);
	glDeleteBuffers(1, &cubeEBO);
	
	drawsList.clear();
	wireframeDrawList.clear();
}

void CubeVisualizer::uploadDraws(glm::mat4 transformation, glm::vec3 colour, bool hasWireframe){
	drawCommand nDC;
	nDC.transformation = transformation;
	nDC.colour =colour;
	
	if (!hasWireframe){
		drawsList.push_back(nDC);
		return;
	}
	wireframeDrawList.push_back(nDC);
}

void CubeVisualizer::queuedDraws(float thickness, bool fboveride){
	
	if (drawsList.empty() && wireframeDrawList.empty()) return;
	
	if (dbgPass::overlayDebug){
	
		// seperated for when instaced draw comes in
		for (int i = 0; i < drawsList.size(); ++i)
			draw(drawsList[i].transformation, drawsList[i].colour, 0.0f, false, fboveride);
		for (int i = 0; i < wireframeDrawList.size(); ++i)
			draw(wireframeDrawList[i].transformation, wireframeDrawList[i].colour, thickness, true, fboveride);
	
	}
	drawsList.clear();
	wireframeDrawList.clear();
}

void CubeVisualizer::draw(glm::vec3 position,
                          glm::vec3 scale, glm::vec3 colour, float thickness, bool hasWireframe, bool fboveride) {
	
	if (dbgPass::overlayDebug)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, dbgPass::dbgBuffer);
		//glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);
		// Since the cubemap will always have a depth of 1.0, we need that equal sign so it doesn't get discarded
		glEnable(GL_DEPTH_TEST);
		glBindVertexArray(0);
		glDisable(GL_CULL_FACE);
		glDepthFunc(GL_LESS);
		
		if (hasWireframe) {glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); glLineWidth(thickness); } // Enable wireframe mode
		//std::cout << "height" << height << std::endl;
		boxShader.Activate();

		glm::mat4 model = glm::mat4(1.0f);
		// Apply translation
		model = glm::translate(model, position);
		// Apply scaling
		model = glm::scale(model, scale);
		boxShader.setMat4("model", model);
		//feed model matrix known as inside the shader "model"
		boxShader.setMat4("camMatrix", Scene::maincamera.cameraMatrixAlwaysUnjittered);
		
		glUniform3f(glGetUniformLocation(boxShader.ID, "camPos"), Scene::maincamera.Position.x, Scene::maincamera.Position.y, Scene::maincamera.Position.z);

		glUniform3f(glGetUniformLocation(boxShader.ID, "colour"), colour.x, colour.y, colour.z);
		glBindVertexArray(cubeVAO);
		//glDepthFunc(GL_ALWAYS);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		// Adjust the width as needed
		if (hasWireframe) { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);glLineWidth(1.0f);} // Restore normal rendering < wireframe
		//glDepthFunc(GL_LESS);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, renderTarget::FBO);
		
		glEnable(GL_CULL_FACE);
	}
	// debug buffer would be cool, actually 3 debug buffers one for wireframe, another for hitboxes and the other for a polygon view simular to unreals
	//}
}

void CubeVisualizer::draw(glm::mat4 transformation, glm::vec3 colour, float thickness, bool hasWireframe,
	bool fboveride)
{
		
	if (dbgPass::overlayDebug)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, dbgPass::dbgBuffer);
		//glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);
		// Since the cubemap will always have a depth of 1.0, we need that equal sign so it doesn't get discarded
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glBindVertexArray(0);
		glDisable(GL_CULL_FACE);
		
		if (hasWireframe) {glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); glLineWidth(thickness); } // Enable wireframe mode
		//std::cout << "height" << height << std::endl;
		boxShader.Activate();
		
		boxShader.setMat4("model", transformation);
		//feed model matrix known as inside the shader "model"
		boxShader.setMat4("camMatrix", Scene::maincamera.cameraMatrixAlwaysUnjittered);
		
		glUniform3f(glGetUniformLocation(boxShader.ID, "camPos"), Scene::maincamera.Position.x, Scene::maincamera.Position.y, Scene::maincamera.Position.z);

		glUniform3f(glGetUniformLocation(boxShader.ID, "colour"), colour.x, colour.y, colour.z);
		glBindVertexArray(cubeVAO);
		//glDepthFunc(GL_ALWAYS);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		// Adjust the width as needed
		if (hasWireframe) { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);glLineWidth(1.0f);} // Restore normal rendering < wireframe
		//glDepthFunc(GL_LESS);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, renderTarget::FBO);
		
		glEnable(GL_CULL_FACE);
	}
}
