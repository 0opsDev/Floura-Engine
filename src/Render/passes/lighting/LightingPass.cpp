#include "LightingPass.h"
#include <Render/Shader/Framebuffer.h>
#include "Render/Cube/RenderQuad.h"
#include <Render/passes/geometry/geometryPass.h>

GLuint LightingPass::computeTexture;
RenderQuad ComputeQuad;
Shader ComputeQuadShader;
Shader testCompute;
unsigned int CurrentWidth;
unsigned int CurrentHeight;

void LightingPass::init() {

}

void LightingPass::initcomputeShader(unsigned int width, unsigned int height) {


	ComputeQuad.init();

	ComputeQuadShader.LoadShader("Shaders/Db/RenderQuad.vert", "Shaders/Db/ComputeRenderQuad.frag");

	glCreateTextures(GL_TEXTURE_2D, 1, &computeTexture);
	glTextureParameteri(computeTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(computeTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(computeTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(computeTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(computeTexture, 1, GL_RGBA32F, width, height);
	glBindImageTexture(0, computeTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	CurrentWidth = width;
	CurrentHeight = height;

	testCompute.LoadComputeShader("Shaders/Db/computeShader.comp");

//	glEnable(GL_DEBUG_OUTPUT);
//	glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity,
//		GLsizei length, const GLchar* message, const void* userParam) {
//			std::cerr << "GL DEBUG: " << message << std::endl;
//		}, nullptr);

	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		std::cerr << "OpenGL error: " << err << std::endl;
	}

}

void LightingPass::resizeTexture(unsigned int width, unsigned int height) {
    glDeleteTextures(1, &computeTexture); // Delete old texture

	glCreateTextures(GL_TEXTURE_2D, 1, &computeTexture);
	glTextureParameteri(computeTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(computeTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(computeTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(computeTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(computeTexture, 1, GL_RGBA32F, width, height);
	glBindImageTexture(0, computeTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	CurrentWidth = width;
	CurrentHeight = height;
}

void RenderComputeToQuad() {
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);
	glDisable(GL_CULL_FACE);

	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, LightingPass::computeTexture); // using gpos as temp
	//ComputeQuadShader.setInt("computeTexture", 1);

	ComputeQuadShader.Activate();
	glBindTextureUnit(0, LightingPass::computeTexture);

	glUniform1i(glGetUniformLocation(ComputeQuadShader.ID, "screen"), 0);


	ComputeQuad.draw(ComputeQuadShader);

	glEnable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);

}

void LightingPass::computeRender() {

//	GLuint groupCountX = (GLuint)ceil((float)width / 8.0f);
//	GLuint groupCountY = (GLuint)ceil((float)height / 4.0f);
	glBindImageTexture(0, computeTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glUseProgram(testCompute.ID); 
	glDispatchCompute(ceil(CurrentWidth / 8), ceil(CurrentHeight / 4), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, LightingPass::computeTexture); // using gpos as temp
	//glBindTextureUnit(0, LightingPass::computeTexture);
	//testCompute.setInt("screen", 1);

	RenderComputeToQuad();

}