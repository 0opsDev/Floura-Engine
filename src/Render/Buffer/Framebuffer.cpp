#include <glad/gl.h>
#include "Framebuffer.h"
#include <iostream>

void Framebuffer::setup(FBOparameters iFBOparameters, RBOparameters iRBOparameters)
{
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glGenTextures(1, &texture); 
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, iFBOparameters.internalFormat, width, height, 0, iFBOparameters.format, iFBOparameters.format, NULL);
    glTexParameteri(GL_TEXTURE_2D, iFBOparameters.minFilter, iFBOparameters.MinSamplingFilter); 
    glTexParameteri(GL_TEXTURE_2D, iFBOparameters.MagFilter, iFBOparameters.MagSamplingFilter);
    glTexParameteri(GL_TEXTURE_2D, iFBOparameters.wrap1, iFBOparameters.clamp1);
    glTexParameteri(GL_TEXTURE_2D, iFBOparameters.wrap2, iFBOparameters.clamp2);
    glFramebufferTexture2D(GL_FRAMEBUFFER, iFBOparameters.colourAttachment, GL_TEXTURE_2D, texture, 0);

    if (iRBOparameters.doRBO){
        glGenRenderbuffers(1, &RBO);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
        glRenderbufferStorage(GL_RENDERBUFFER, iRBOparameters.internalFormat, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, iRBOparameters.attachment, GL_RENDERBUFFER, RBO);
    }
    
    nFBOparameters = iFBOparameters;
    nRBOparameters = iRBOparameters;
    
    // Error checking
    auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer error: " << fboStatus << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::setupMulti(std::vector<FBOparameters> iFBOparametersMulti, RBOparameters iRBOparameters)
{
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    
    std::vector<unsigned int> attachments;

    for (int i = 0; i < iFBOparametersMulti.size(); ++i)
    {
        unsigned int ntexture;
        
        glGenTextures(1, &ntexture); 
        glBindTexture(GL_TEXTURE_2D, ntexture);
        glTexImage2D(GL_TEXTURE_2D, 0, iFBOparametersMulti[i].internalFormat, width, height, 0, iFBOparametersMulti[i].format, iFBOparametersMulti[i].type, NULL);
        glTexParameteri(GL_TEXTURE_2D, iFBOparametersMulti[i].minFilter, iFBOparametersMulti[i].MinSamplingFilter); 
        glTexParameteri(GL_TEXTURE_2D, iFBOparametersMulti[i].MagFilter, iFBOparametersMulti[i].MagSamplingFilter);
        glTexParameteri(GL_TEXTURE_2D, iFBOparametersMulti[i].wrap1, iFBOparametersMulti[i].clamp1);
        glTexParameteri(GL_TEXTURE_2D, iFBOparametersMulti[i].wrap2, iFBOparametersMulti[i].clamp2);
        glFramebufferTexture2D(GL_FRAMEBUFFER, iFBOparametersMulti[i].colourAttachment, GL_TEXTURE_2D, ntexture, 0);
        
        textures.push_back(ntexture);
        
        attachments.push_back(iFBOparametersMulti[i].colourAttachment);
    }
    
    glDrawBuffers(attachments.size(), attachments.data());

    if (iRBOparameters.doRBO){
        glGenRenderbuffers(1, &RBO);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
        glRenderbufferStorage(GL_RENDERBUFFER, iRBOparameters.internalFormat, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, iRBOparameters.attachment, GL_RENDERBUFFER, RBO);
    }
    
    FBOparametersMulti = iFBOparametersMulti;
    nRBOparameters = iRBOparameters;
    
    // Error checking
    auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer error: " << fboStatus << std::endl;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::resize(int width, int height)
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, nFBOparameters.internalFormat, width, height, 0, nFBOparameters.format, nFBOparameters.type, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    
	if (!nRBOparameters.doRBO) return;
    // update renderbuffer texture
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, nRBOparameters.internalFormat, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Framebuffer::resizeMulti(int newWidth, int newHeight)
{
    width = newWidth;
    height = newHeight;

    for (int i = 0; i < FBOparametersMulti.size(); ++i)
    {
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, FBOparametersMulti[i].internalFormat, width, height, 0, FBOparametersMulti[i].format, FBOparametersMulti[i].type, NULL);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    
    if (!nRBOparameters.doRBO) return;
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, nRBOparameters.internalFormat, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Framebuffer::Delete(){
    glDeleteFramebuffers(1, &FBO);
    glDeleteRenderbuffers(1, &RBO);
    glDeleteTextures(1, &texture);
}
