#include "Cubemap.h"

std::array<std::string, 6> facesCubemap;

void Cubemap::LoadCubeMapTexture(std::string PathName) {

	// loading

	std::ifstream SkyboxJson(PathName);
	if (SkyboxJson.is_open()) {
		json SkyboxJsonData;
		SkyboxJson >> SkyboxJsonData;
		SkyboxJson.close();

		std::string Path = SkyboxJsonData[0]["Path"].get<std::string>() + "/";

		if (init::LogALL || init::LogModel) std::cout << "Skybox Path: " << Path << std::endl;

		for (int i = 0; i < 6; i++)
		{
			facesCubemap[i] = Path + SkyboxJsonData[0]["Faces"][i].get<std::string>();
			if (init::LogALL || init::LogModel) std::cout << "Skybox Face: " << facesCubemap[i] << std::endl;
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
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// These are very important to prevent seams
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Cycles through all the textures and attaches them to the cubemap object
	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, numColCh;
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
			if (init::LogALL || init::LogModel) std::cout << "Failed to load texture: " << facesCubemap[i] << std::endl;
			stbi_image_free(data);
		}
	}
}