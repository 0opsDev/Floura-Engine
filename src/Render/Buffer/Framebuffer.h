#ifndef FRAMEBUFFER_CLASS_H
#define FRAMEBUFFER_CLASS_H
#include <vector>

#include"string.h"
#include "glm/glm.hpp"

class Framebuffer
{
public:
    
    GLuint FBO;
    
    int uploadAttachment(GLenum internalFormat, GLenum format, GLenum type, GLenum minFilter, GLenum magFilter, GLenum clamp);
    
    void createBuffers(unsigned int  width, unsigned int  height);
    
    void resizeBuffers(unsigned int  width, unsigned int  height);
    
    void deleteBuffers();
    
private:
    
    struct attachment{
        GLuint ID;
        GLenum internalFormat = GL_RGBA8;
        GLenum format = GL_RGBA;
        GLenum type = GL_UNSIGNED_BYTE;
        
        GLenum minFilter = GL_NEAREST;
        GLenum magFilter = GL_NEAREST;
        
        GLenum clamp = GL_CLAMP_TO_EDGE;
    };
    
    std::vector<attachment> attachments;
public:
    
    GLuint fetchID(size_t index){
        return attachments[index].ID;
    }
};

#endif
