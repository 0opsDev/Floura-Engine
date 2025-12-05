#include"SkyBox.h"
#include <glm/gtx/string_cast.hpp>
#include <Core/Render.h>
#include <Render/passes/geometry/geometryPass.h>

unsigned int Skybox::VAO;
unsigned int Skybox::VBO;
unsigned int Skybox::EBO;
bool Skybox::DoSbRGBA = true;
Shader skyboxShader;
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
	skyboxShader.Activate();

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
	SkyboxCubemap = new Cubemap(PathName); // update it to parse in string which is a path,
}

void Skybox::draw() {

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_CULL_FACE);
	glm::mat4 view = glm::mat4(1.0f);
	// We make the mat4 into a mat3 and then a mat4 again in order to get rid of the last row and column
	// The last row and column affect the translation of the skybox (which we don't want to affect)
	view = glm::mat4(glm::mat3(Camera::view));
	//std::cout << "Projection matrix: " << glm::to_string(projection) << std::endl;


	glm::mat4 rot = glm::mat4(1.0f);

	rot = glm::rotate(rot, glm::radians(rotation.x), glm::vec3(1, 0, 0));
	rot = glm::rotate(rot, glm::radians(rotation.y), glm::vec3(0, 1, 0));
	rot = glm::rotate(rot, glm::radians(rotation.z), glm::vec3(0, 0, 1));


	if (RenderClass::DoForwardLightingPass) {

		// Since the cubemap will always have a depth of 1.0, we need that equal sign so it doesn't get discarded
		glDepthFunc(GL_LEQUAL);
		//std::cout << "height" << height << std::endl;
		skyboxShader.Activate();
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", Camera::projection);
		skyboxShader.setInt("skybox", 0);
		skyboxShader.setFloat3("skyRGBA", RenderClass::gammaCorrect3(RenderClass::skyRGBA));
		skyboxShader.setBool("DoSbRGBA", DoSbRGBA);
		skyboxShader.setMat4("rotation", rot);

		// Draws the cubemap as the last object so we can save a bit of performance by discarding all fragments
		// where an object is present (a depth of 1.0f will always fail against any object's depth value)

		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, SkyboxCubemap->ID);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

}

void Skybox::Delete() {
	skyboxShader.Delete();
	SkyboxCubemap->~Cubemap();
}