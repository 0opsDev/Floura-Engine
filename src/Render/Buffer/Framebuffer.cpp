#include <glad/gl.h>
#include "Framebuffer.h"
#include <iostream>

int Framebuffer::uploadAttachment(GLenum internalFormat, GLenum format, GLenum type, GLenum minFilter, GLenum magFilter, GLenum clamp){
    attachment nAttach;
    nAttach.internalFormat = internalFormat;
    nAttach.format = format;
    nAttach.type = type;
    nAttach.minFilter = minFilter;
    nAttach.magFilter = magFilter;
    nAttach.clamp = clamp;
    attachments.push_back(nAttach);
    
    // incase you need the index idk 
    return static_cast<int>(attachments.size());
}

void Framebuffer::createBuffers(unsigned int  width, unsigned int  height){
    // only supports colour buffers rn
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    
    std::vector<GLuint> nAttachments;
    nAttachments.reserve(attachments.size());
    
    for (int i = 0; i < attachments.size(); ++i){
        glGenTextures(1, &attachments[i].ID);
        glBindTexture(GL_TEXTURE_2D, attachments[i].ID);
        glTexImage2D(GL_TEXTURE_2D, 0, attachments[i].internalFormat, width, height, 0, attachments[i].format, attachments[i].type, NULL);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, attachments[i].minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, attachments[i].magFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, attachments[i].clamp);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,  attachments[i].clamp);
        
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, attachments[i].ID, 0);
        
        nAttachments.push_back(GL_COLOR_ATTACHMENT0 + i);
    }
    
    glDrawBuffers(nAttachments.size(), nAttachments.data());
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::resizeBuffers(unsigned int  width, unsigned int  height){
    if (width <= 0 || height <=0)return;
    
    for (int i = 0; i < attachments.size(); ++i){
        glBindTexture(GL_TEXTURE_2D, attachments[i].ID);
        glTexImage2D(GL_TEXTURE_2D, 0, attachments[i].internalFormat, width, height, 0, attachments[i].format, attachments[i].type, NULL);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Framebuffer::deleteBuffers(){
    glDeleteFramebuffers(1, &FBO);
    
    if (attachments.empty()) return;
    
    std::vector<GLuint> IDs;
    IDs.reserve(attachments.size());

    for (int i = 0; i < attachments.size(); ++i){
        IDs.push_back(attachments[i].ID);
    }
    
    glDeleteTextures(static_cast<GLsizei>(IDs.size()), IDs.data());
    
    attachments.clear();
}

