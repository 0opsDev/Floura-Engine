#include "Cubemap.h"
#include <utils/logConsole.h>
#include <Render/Shader/Framebuffer.h>
#include <Render/Object/ModelAssimp.h>

std::array<std::string, 6> facesCubemap;

glm::vec3 targets[] = {
glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(-1.0f,  0.0f,  0.0f),
glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f),
glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f,  0.0f, -1.0f)
};

glm::vec3 ups[] = {
	glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f),
	glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f,  0.0f, -1.0f),
	glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)
};


Cubemap::Cubemap() {
}

void Cubemap::loadCubeMap(std::string path)
{
	Cubemap::path = path;
	// loading

	std::ifstream SkyboxJson(path);
	if (SkyboxJson.is_open()) {
		json SkyboxJsonData;
		SkyboxJson >> SkyboxJsonData;
		SkyboxJson.close();

		std::string nPath = SkyboxJsonData[0]["Path"].get<std::string>() + "/";

		//LogConsole::print("Skybox Path: " + nPath);

		for (int i = 0; i < 6; i++)
		{
			facesCubemap[i] = nPath + SkyboxJsonData[0]["Faces"][i].get<std::string>();
			//LogConsole::print("Skybox Face: " + facesCubemap[i]);
		}
	}
	else {
		std::cerr << "Failed to open Skybox.json" << std::endl;
	}

	// creation

	glDeleteTextures(1, &ID);

	// Creates the cubemap texture object
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// These are very important to prevent seams
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


	//res
	 
	int width, height, numColCh;

	// Cycles through all the textures and attaches them to the cubemap object
	for (unsigned int i = 0; i < 6; i++)
	{
		unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &numColCh, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D
			(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGBA,
				width,
				height,
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);


		}
		else
		{
			LogConsole::print("Failed to load texture: " + facesCubemap[i]);
			stbi_image_free(data);
		}
	}

	res = glm::vec2(width, height);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	if (GLAD_GL_ARB_bindless_texture) {
		handle = glGetTextureHandleARB(ID);
		glMakeTextureHandleResidentARB(handle);
	}

}

void Cubemap::bind(int unit)
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
}

void Cubemap::unbind()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Cubemap::cubemapToShader(Shader& shader, int unit)
{
	shader.Activate();
	shader.setInt("skybox", unit);
}

void Cubemap::cubemapToUUIDShader(const char* uniform, Shader & shader)
{
	shader.Activate();
	shader.setHandleui64ARB(uniform, handle); // guess im keeping this weird capital underscore naming convension
}

void Cubemap::drawCubeMap(Shader shader, std::vector<Model*>& Models, glm::vec2 resolution)
{
	// creation

	glDeleteTextures(1, &ID);

	// Creates the cubemap texture object
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// These are very important to prevent seams
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::cmFBO);

	int width = (int)resolution.x;
	int height = (int)resolution.y;
	glViewport(0, 0, width, height);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	Framebuffer::smUpdateResolution(resolution);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	Framebuffer::clearsmbuffer();
	// Cycles through all the textures and attaches them to the cubemap object
	for (unsigned int i = 0; i < 6; i++)
	{
		Camera nCamera;
		nCamera.InitCamera(resolution.x, resolution.y, glm::vec3(0, 5, 0)); // Matching your Position
		nCamera.fov = 90.0f;
		nCamera.Orientation = targets[i];
		nCamera.Up = ups[i];
		nCamera.updateMatrix();

		
		glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::cmFBO);

		for (size_t x = 0; x < Models.size(); x++)
		{
			shader.Activate();
			Models[x]->draw(shader, nCamera);
		}


		unsigned char* data = new unsigned char[width * height * 4];

		glBindFramebuffer(GL_READ_BUFFER, Framebuffer::cmFBO);
		//glReadBuffer(GL_COLOR_ATTACHMENT0);
		glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0,
			GL_RGB16F,
			width,
			height,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			data
		);

		delete[] data;
	}


	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	if (GLAD_GL_ARB_bindless_texture) {
		handle = glGetTextureHandleARB(ID);
		glMakeTextureHandleResidentARB(handle);
	}
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	width = viewport[2];
	height = viewport[3];
	glViewport(0, 0, width, height);
}

void Cubemap::resizeCubeMap(glm::vec2 resolution)
{
	// skip if new res is same as old
	if (res == resolution) return;


	if (GLAD_GL_ARB_bindless_texture && handle != 0) {
		glMakeTextureHandleNonResidentARB(handle);
	}

	if (ID == 0 || true) glGenTextures(1, &ID);

	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

	int width = (int)resolution.x;
	int height = (int)resolution.y;
	res = glm::vec2(width, height);

	for (unsigned int i = 0; i < 6; i++)
	{
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0,
			GL_RGB16F,
			width,
			height,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			NULL
		);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	if (GLAD_GL_ARB_bindless_texture) {
		handle = glGetTextureHandleARB(ID);
		glMakeTextureHandleResidentARB(handle);
	}
}


Cubemap::~Cubemap()
{
	// delete the handle first
	if (GLAD_GL_ARB_bindless_texture) {
		glMakeTextureHandleNonResidentARB(handle);
	}
	glDeleteTextures(1, &ID);	
}