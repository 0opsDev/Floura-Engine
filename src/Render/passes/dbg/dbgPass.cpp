#include "dbgPass.h"

bool dbgPass::overlayDebug = false;
unsigned int dbgPass::dbgBuffer;
unsigned int dbgPass::dbgColour;
unsigned int dbgPass::hDBO;

void dbgPass::setupDBGbuffers(unsigned int width, unsigned int height)
{
	glGenFramebuffers(1, &dbgBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, dbgBuffer);
	// GEN TEX and bind tex to fbo
	glGenTextures(1, &dbgColour); // GL_LINEAR
	glBindTexture(GL_TEXTURE_2D, dbgColour); // GL_UNSIGNED_BYTE
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_LINEAR_MIPMAP_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dbgColour, 0);
	

	// Error checking
	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer error: " << fboStatus << std::endl;
	}

}

void dbgPass::updateDBGResolution(unsigned int width, unsigned int height)
{
	glBindTexture(GL_TEXTURE_2D, dbgColour);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

}
