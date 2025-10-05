#include"SkyBox.h"
#include "Cubemap.h"
#include "utils/timeUtil.h"
#include <glm/gtx/string_cast.hpp>
#include <Core/Render.h>
#include <Render/passes/geometry/geometryPass.h>

unsigned int Skybox::skyboxVAO;
unsigned int Skybox::skyboxVBO;
unsigned int Skybox::skyboxEBO;
bool Skybox::DoSbRGBA = true;
Shader skyboxShader;
Shader skyboxgPassShader;
std::string Skybox::DefaultSkyboxPath;
Cubemap SkyboxCubemap;

float s_skyboxVertices[24] =
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

unsigned int s_skyboxIndices[36] =
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

void Skybox::init(std::string PathName) {
	skyboxShader.LoadShader("Shaders/Skybox/skybox.vert", "Shaders/Skybox/skybox.frag");
	skyboxShader.Activate();
	glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);
	skyboxgPassShader.LoadShader("Shaders/gBuffer/geometryPassSkybox.vert", "Shaders/gBuffer/geometryPassSkybox.frag");
	skyboxgPassShader.Activate();
	glUniform1i(glGetUniformLocation(skyboxgPassShader.ID, "skybox"), 0);
	skyboxBuffer(); // create buffer in memory for skybox
	LoadSkyBoxTexture(PathName); // load skybox texture into buffer
}

void Skybox::LoadSkyBoxTexture(std::string PathName) {
	SkyboxCubemap.LoadCubeMapTexture(PathName); // update it to parse in string which is a path,
}

void Skybox::skyboxBuffer() {
	// Create VAO, VBO, and EBO for the skybox
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxEBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(s_skyboxVertices), &s_skyboxVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_skyboxIndices), &s_skyboxIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Skybox::draw(unsigned int width, unsigned int height) {

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_CULL_FACE);
	glm::mat4 view = glm::mat4(1.0f);
	// We make the mat4 into a mat3 and then a mat4 again in order to get rid of the last row and column
	// The last row and column affect the translation of the skybox (which we don't want to affect)
	view = glm::mat4(glm::mat3(glm::lookAt(Camera::Position, Camera::Position + Camera::Orientation, Camera::Up)));
	//std::cout << "Projection matrix: " << glm::to_string(projection) << std::endl;


	if (RenderClass::DoForwardLightingPass) {

		// Since the cubemap will always have a depth of 1.0, we need that equal sign so it doesn't get discarded
		glDepthFunc(GL_LEQUAL);
		//std::cout << "height" << height << std::endl;
		skyboxShader.Activate();
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", Camera::projection);

		skyboxShader.Activate();
		skyboxShader.setInt("skybox", 0);
		skyboxShader.setFloat3("skyRGBA", RenderClass::skyRGBA.r, RenderClass::skyRGBA.g, RenderClass::skyRGBA.b);
		skyboxShader.setBool("DoSbRGBA", DoSbRGBA);

		// Draws the cubemap as the last object so we can save a bit of performance by discarding all fragments
		// where an object is present (a depth of 1.0f will always fail against any object's depth value)

		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, SkyboxCubemap.ID);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		skyboxgPassShader.Activate();
		skyboxgPassShader.setFloat("gamma", RenderClass::gamma);
		glBindFramebuffer(GL_FRAMEBUFFER, GeometryPass::gBuffer);
		glEnable(GL_DEPTH_TEST);

		// Since the cubemap will always have a depth of 1.0, we need that equal sign so it doesn't get discarded
		glDepthFunc(GL_LEQUAL);
		//std::cout << "height" << height << std::endl;
		skyboxgPassShader.Activate();
		skyboxgPassShader.setMat4("view", view);
		skyboxgPassShader.setMat4("projection", Camera::projection);

		skyboxgPassShader.Activate();
		skyboxgPassShader.setInt("skybox", 0);
		skyboxShader.setFloat3("skyRGBA", RenderClass::skyRGBA.r, RenderClass::skyRGBA.g, RenderClass::skyRGBA.b);

		// Draws the cubemap as the last object so we can save a bit of performance by discarding all fragments
		// where an object is present (a depth of 1.0f will always fail against any object's depth value)

		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, SkyboxCubemap.ID);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glDisable(GL_CULL_FACE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Skybox::Delete() {
	skyboxShader.Delete();
	skyboxgPassShader.Delete();
}