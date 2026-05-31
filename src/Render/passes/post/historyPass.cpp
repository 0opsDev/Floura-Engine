#include  "historyPass.h"
#include <Render/Shader/Framebuffer.h>
#include "Scene/scene.h"
#include <Render/passes/geometry/geometryPass.h>

unsigned int HistoryPass::hDepthTexture;
unsigned int HistoryPass::hBuffer; 
unsigned int HistoryPass::hColour; 
unsigned int HistoryPass::hNormal; 
unsigned int HistoryPass::hDBO;
Shader HistoryPass::hPassShader;
RenderQuad HistoryPass::hpRenderQuad;

void HistoryPass::init()
{
	hpRenderQuad.init();
}

void HistoryPass::updateHbufferResolution(unsigned int width, unsigned int height)
{
    // Update H-buffer textures
	// colour
	glBindTexture(GL_TEXTURE_2D, hColour);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
    // hNormal
    glBindTexture(GL_TEXTURE_2D, hNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
	// Update depth textures
	glBindTexture(GL_TEXTURE_2D, hDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void HistoryPass::setupHbuffers(unsigned int width, unsigned int height)
{
    	//generate buffer in memory and bind
	glGenFramebuffers(1, &hBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, hBuffer);

	glGenTextures(1, &hColour);
	glBindTexture(GL_TEXTURE_2D, hColour);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hColour, 0);
	
	
	// - normal color buffer
	glGenTextures(1, &hNormal);
	glBindTexture(GL_TEXTURE_2D, hNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, hNormal, 0);
	
	// Create Depth Texture
	glGenTextures(1, &hDepthTexture);
	glBindTexture(GL_TEXTURE_2D, hDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, hDepthTexture, 0);
	

	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2}; // colour normal 
	glDrawBuffers(3, attachments);

	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void HistoryPass::hPassDraw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	hPassShader.Activate();
	hPassShader.setFloat("gamma", Scene::maincamera.gamma);
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, Framebuffer::screentexture);
	hPassShader.setInt("screentexture", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::depthTexture);
	hPassShader.setInt("currentDepth", 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gNormal);
	hPassShader.setInt("currentNormal", 3);
	
	glBindFramebuffer(GL_FRAMEBUFFER, hBuffer);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	hpRenderQuad.draw();
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//FrameBuffer
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);
}
