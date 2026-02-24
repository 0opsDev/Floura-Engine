#include"SkyBox.h"
#include <glm/gtx/string_cast.hpp>
#include <Core/Render.h>
#include <Render/passes/geometry/geometryPass.h>
#include "Scene/scene.h"
#include <Render/Shader/Framebuffer.h>

glm::mat4 Skybox::currentview = glm::mat4(1.0);

glm::mat4 Skybox::previousview = glm::mat4(1.0);
glm::mat4 Skybox::previousprojection = glm::mat4(1.0);

unsigned int Skybox::VAO;
unsigned int Skybox::VBO;
unsigned int Skybox::EBO;

bool Skybox::DoSbRGBA = true;
Shader skyboxShader;
Shader gSkyboxShader;
std::string Skybox::DefaultSkyboxPath;
Cubemap* Skybox::SkyboxCubemap;
glm::vec3 Skybox::rotation = glm::vec3(0.0f, 0.0f, 0.0f);

const float s_skyboxVertices[24] =
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

const unsigned int s_skyboxIndices[36] =
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

void Skybox::init() {
	skyboxShader.LoadShader("Assets/Shaders/Skybox/skybox.vert", "Assets/Shaders/Skybox/skybox.frag");
	gSkyboxShader.LoadShader("Assets/Shaders/Skybox/skybox.vert", "Assets/Shaders/gBuffer/gSkybox.frag");

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(s_skyboxVertices), &s_skyboxVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_skyboxIndices), &s_skyboxIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void Skybox::LoadSkyBoxTexture(std::string PathName) {
	if (SkyboxCubemap != nullptr) {
		SkyboxCubemap->~Cubemap();
	}
	SkyboxCubemap = new Cubemap(); // update it to parse in string which is a path,
	SkyboxCubemap->loadCubeMap(PathName);
}

void Skybox::setPreviousMats(Camera& camera)
{
	previousview = currentview;
	previousprojection = camera.projection;
}

void Skybox::draw(Camera& camera, const unsigned int framebuffer, bool gPassEnabled) {

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	//glBindTexture(GL_TEXTURE_2D, 0);
	// We make the mat4 into a mat3 and then a mat4 again in order to get rid of the last row and column
	// The last row and column affect the translation of the skybox (which we don't want to affect)
	currentview = glm::mat4(glm::mat3(camera.view));
	//std::cout << "Projection matrix: " << glm::to_string(projection) << std::endl;


	glm::mat4 rot = glm::mat4(1.0f);

	rot = glm::rotate(rot, glm::radians(rotation.x), glm::vec3(1, 0, 0));
	rot = glm::rotate(rot, glm::radians(rotation.y), glm::vec3(0, 1, 0));
	rot = glm::rotate(rot, glm::radians(rotation.z), glm::vec3(0, 0, 1));
	
	glDisable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);
	
	// should allow take shader in, not make these two seperated shaders inside the skybox class with the arguments
	if (gPassEnabled)
	{
		
		glBindFramebuffer(GL_FRAMEBUFFER, GeometryPass::gBuffer);
		
		// gpass
		gSkyboxShader.Activate();
		
		//     scaledPreviousJitter = scaledCurrentJitter;
		// current and previous jitter matrix
		if (Scene::maincamera.applyJitter){
			gSkyboxShader.setFloat2("currentJitter",  Scene::maincamera.currentJitter);
			gSkyboxShader.setFloat2("previousJitter",  Scene::maincamera.previousJitter);
			gSkyboxShader.setFloat2("scaledCurrentJitter",  Scene::maincamera.scaledCurrentJitter);
			gSkyboxShader.setFloat2("scaledPreviousJitter",  Scene::maincamera.scaledPreviousJitter);
		}
		else{
			gSkyboxShader.setFloat2("currentJitter",  glm::vec2(0.0));
			gSkyboxShader.setFloat2("previousJitter",  glm::vec2(0.0));
			gSkyboxShader.setFloat2("scaledCurrentJitter",  glm::vec2(0.0));
			gSkyboxShader.setFloat2("scaledPreviousJitter",  glm::vec2(0.0));
		}
		
		gSkyboxShader.setMat4("view", currentview);
		gSkyboxShader.setMat4("projection", camera.projection);
		
		gSkyboxShader.setMat4("previousView", previousview);
		gSkyboxShader.setMat4("previousProjection", previousprojection);
		
		//skyboxShader.setInt("skybox", 0);
		gSkyboxShader.setFloat3("skyRGBA", RenderClass::gammaCorrect3(RenderClass::skyRGBA));
		gSkyboxShader.setBool("DoSbRGBA", DoSbRGBA);
		gSkyboxShader.setMat4("rotation", rot);
		SkyboxCubemap->cubemapToUUIDShader("skyboxHandle", gSkyboxShader);
		
		gSkyboxShader.Activate();
		
		glDepthMask(GL_FALSE);
		// Since the cubemap will always have a depth of 1.0, we need that equal sign so it doesn't get discarded
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
		
	//if (RenderClass::DoForwardLightingPass) {

	
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glDepthMask(GL_TRUE);
	
		skyboxShader.Activate();
	
		skyboxShader.setMat4("view", currentview);
		skyboxShader.setMat4("projection", camera.projection);
	    skyboxShader.setMat4("previousView", previousview);
        skyboxShader.setMat4("previousProjection", previousprojection);
		//skyboxShader.setInt("skybox", 0);
		skyboxShader.setFloat3("skyRGBA", RenderClass::gammaCorrect3(RenderClass::skyRGBA));
		skyboxShader.setBool("DoSbRGBA", DoSbRGBA);
		skyboxShader.setMat4("rotation", rot);
		SkyboxCubemap->cubemapToUUIDShader("skyboxHandle", skyboxShader);
		
		
	skyboxShader.Activate();
		// Since the cubemap will always have a depth of 1.0, we need that equal sign so it doesn't get discarded
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	//}
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(0);
}

void Skybox::unbind()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Skybox::Delete() {
	SkyboxCubemap->~Cubemap();
}

void Skybox::cleanup()
{
	skyboxShader.Delete();
	gSkyboxShader.Delete();
}