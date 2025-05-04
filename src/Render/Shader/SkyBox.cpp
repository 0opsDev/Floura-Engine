#include"SkyBox.h"
#include <utils/SettingsUtil.h>
#include <utils/UF.h>
#include "Cubemap.h"
#include "utils/timeUtil.h"

unsigned int Skybox::cubemapTexture;
unsigned int Skybox::skyboxVAO;
unsigned int Skybox::skyboxVBO;
unsigned int Skybox::skyboxEBO;
Shader skyboxShader("skip", "");

void Skybox::init(std::string PathName) {
	skyboxShader = Shader("Shaders/Skybox/skybox.vert", "Shaders/Skybox/skybox.frag");
	skyboxShader.Activate();
	glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);
	skyboxBuffer(); // create buffer in memory for skybox
	LoadSkyBoxTexture(PathName); // load skybox texture into buffer
}

void Skybox::LoadSkyBoxTexture(std::string PathName) {
	Cubemap SkyboxCubemap;

	SkyboxCubemap.LoadCubeMapTexture(PathName); // update it to parse in string which is a path,
	// Create and load a cubemap texture
	SkyboxCubemap.cubeboxTexture(cubemapTexture);
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
float yaw = 1;

void Skybox::draw(Camera camera, GLfloat skyRGBA[], unsigned int width, unsigned int height) {

	// Ensure depth function allows rendering skybox
	glDepthFunc(GL_LEQUAL);

	skyboxShader.Activate();
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	// Base view matrix without translation
	view = glm::mat4(glm::mat3(glm::lookAt(camera.Position, camera.Position + camera.Orientation, camera.Up)));
	/*
	if (yaw >= 360) {
		yaw = 0;
	}
	else {
		yaw += (0.5f * TimeUtil::s_DeltaTime);
	}
	std::cout << yaw << std::endl;
	// Apply rotation around Y-axis using yaw
	view = glm::rotate(view, glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
	*/

	// Perspective projection settings
	projection = glm::perspective(glm::radians(Main::cameraSettings[0]), (float)width / height, Main::cameraSettings[1], Main::cameraSettings[2]);

	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));


	// Activate shader and set skybox color
	skyboxShader.Activate();
	UF::Float3(skyboxShader.ID, "skyRGBA", skyRGBA[0], skyRGBA[1], skyRGBA[2]);

	// Draw cubemap as last object to optimize performance
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
void Skybox::Delete() {
	skyboxShader.Delete();
}
float Skybox::s_skyboxVertices[24] =
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

unsigned int Skybox::s_skyboxIndices[36] =
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