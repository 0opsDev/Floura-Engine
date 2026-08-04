#ifndef FLOURA_IMAGE_WRITE_CLASS_H	
#define FLOURA_IMAGE_WRITE_CLASS_H

#include <glad/gl.h>
#include <GLFW/glfw3.h>
//#include <glad/gl.h>

class FlouraImageWrite{
public:

    static bool writeImage2DToDisk(unsigned int ID, int width, int height, const char* filePath, GLenum format, GLenum type, int channels);
    static bool writeImage3DToDiskPNG(unsigned int ID, int width, int height, int depth, const char* filePath, GLenum format, GLenum type, int channels);
    static bool writeImage3DToDiskJSON(unsigned int ID, int width, int height, int depth, const char* filePath, GLenum format, GLenum type, int channels);

private:
};
#endif