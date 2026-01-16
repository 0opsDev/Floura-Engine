#include "ssao.h"
#include "Render/Object/RenderQuad.h"

GLuint ssao::ssaoTexture;
int ssao::currentWidth;
int ssao::currentHeight;
Shader ssao::ssaoCompute;

void ssao::init(int width, int height)
{
	currentWidth = width;
	currentHeight = height;

	// im gonna start the screenspace effects off at index 20
	glCreateTextures(GL_TEXTURE_2D, 1, &ssaoTexture); // 6
	glTextureParameteri(ssaoTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(ssaoTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(ssaoTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(ssaoTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(ssaoTexture, 1, GL_RGBA32F, width, height);
	glBindImageTexture(20, ssaoTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	ssaoCompute.LoadComputeShader("Assets/Shaders/compute/ssao.comp");

	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		std::cerr << "OpenGL error: " << err << std::endl;
	}
}

void ssao::resize(int width, int height)
{
	currentWidth = width;
	currentHeight = height;
	glDeleteTextures(1, &ssaoTexture); // Delete old texture

	glCreateTextures(GL_TEXTURE_2D, 1, &ssaoTexture);
	glTextureParameteri(ssaoTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(ssaoTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(ssaoTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(ssaoTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(ssaoTexture, 1, GL_RGBA32F, width, height);
	glBindImageTexture(20, ssaoTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}

void ssao::draw(int& depth)
{
	ssaoCompute.Activate();
	ssaoCompute.ActivateCompute((currentWidth + 7) / 8, (currentHeight + 3) / 4, 1);
}