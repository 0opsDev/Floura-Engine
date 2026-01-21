#include "Texture.h"
#include <utils/logConsole.h>
#include "utils/FE_math.h"

void Texture::createColour(glm::vec4 colour, const char* texType, GLuint slot)
{
    path = "null";
    LogConsole::print("Texture loading started");
    // Assigns the type of the texture to the texture object
    type = texType;
    //std::cout << type << " U " << slot << std::endl;

    // Flips the image so it appears right side up
    //stbi_set_flip_vertically_on_load(true);
    // Reads the image from a file and stores it in bytes
    unsigned char* bytes = 0;

    unsigned char fallbackPixel[] = { FE_Math::floatToByteRGB(colour.x) , FE_Math::floatToByteRGB(colour.y),  FE_Math::floatToByteRGB(colour.z), FE_Math::floatToByteRGB(colour.w)};
    bytes = fallbackPixel;
    skipstbi = true;

    // Generates an OpenGL texture object
    glGenTextures(1, &ID);
    // Assigns the texture to a Texture Unit
    glActiveTexture(GL_TEXTURE0 + slot);
    unit = slot;
    glBindTexture(GL_TEXTURE_2D, ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        1,
        1,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        bytes
    );

    if (GLAD_GL_ARB_bindless_texture) {
        handle = glGetTextureHandleARB(ID);
        glMakeTextureHandleResidentARB(handle);
    }

    // Unbinds the OpenGL Texture object so that it can't accidentally be modified
    glBindTexture(GL_TEXTURE_2D, 0);
    LogConsole::print("Texture loading finished");

}

void Texture::createTexture(const char* image, const char* texType, GLuint slot)
{
    path = image;
    LogConsole::print("Texture loading started");
    // Assigns the type of the texture to the texture object
    type = texType;
    //std::cout << type << " U " << slot << std::endl;

    // Stores the width, height, and the number of color channels of the image
    int widthImg, heightImg, numColCh;
    // Flips the image so it appears right side up
    //stbi_set_flip_vertically_on_load(true);
    // Reads the image from a file and stores it in bytes
    stbi_set_flip_vertically_on_load(flipVertical);
    //unsigned char* bytes;
    unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);
    //std::cout << "\n" << image << " < image \n" << widthImg << " < widthImg \n" << heightImg << " < heightImg \n" << numColCh << " < numColCh \n" << std::endl;

    if (!bytes) {
        std::cerr << "\nFailed to load texture: " << image << "\n" << std::endl;
        static unsigned char fallbackPixel[] = { 255, 0, 255 };
        widthImg = 1; heightImg = 1; numColCh = 3;
        bytes = fallbackPixel; 
        skipstbi = true;
    }

    // Generates an OpenGL texture object
    glGenTextures(1, &ID);
    // Assigns the texture to a Texture Unit
    glActiveTexture(GL_TEXTURE0 + slot);
    unit = slot;
    glBindTexture(GL_TEXTURE_2D, ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Configures the way the texture repeats (if it does at all)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if (numColCh == 4)
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA, // GL_RGBA
            widthImg,
            heightImg,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            bytes
        );
    else if (numColCh == 3)
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGB, //was GL_RGB
            widthImg,
            heightImg,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            bytes
        );
    else if (numColCh == 1)
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            widthImg,
            heightImg,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            bytes
        );
    // Generates MipMaps
    glGenerateMipmap(GL_TEXTURE_2D);

    if (!skipstbi)
    // Deletes the image data as it is already in the OpenGL Texture object
    stbi_image_free(bytes);

    if (GLAD_GL_ARB_bindless_texture) {
        handle = glGetTextureHandleARB(ID);
		glMakeTextureHandleResidentARB(handle);
    }

    // Unbinds the OpenGL Texture object so that it can't accidentally be modified
    glBindTexture(GL_TEXTURE_2D, 0);
    LogConsole::print("Texture loading finished");
}

void Texture::reload(GLuint slot)
{
    // Assigns the texture to a Texture Unit
    glActiveTexture(GL_TEXTURE0 + slot);
    unit = slot;
    glBindTexture(GL_TEXTURE_2D, ID);

    // Configures the type of algorithm that is used to make the image smaller or bigger
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGenerateMipmap(GL_TEXTURE_2D);

    // remake the handle
	if (GLAD_GL_ARB_bindless_texture) {
        handle = glGetTextureHandleARB(ID);
        glMakeTextureHandleResidentARB(handle);
    }
    // Unbinds the OpenGL Texture object so that it can't accidentally be modified
	glBindTexture(GL_TEXTURE_2D, 0);
}


void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)
{
    // Gets the location of the uniform
    GLuint texUni = glGetUniformLocation(shader.ID, uniform);
    if (texUni == -1)
    {
        // comes from mesh::Draw
       //std::cerr << "Texture.cpp Failed to find uniform: " << uniform << std::endl;
    }
    // Shader needs to be activated before changing the value of a uniform
    
    shader.Activate();
    // Sets the value of the uniform
	//std::cout << "Texture unit: " << unit << std::endl;
    glUniform1i(texUni, unit);
}

void Texture::handleToShader(Shader& shader, const char* uniform)
{
    shader.Activate();
	shader.setHandleui64ARB(uniform, handle);
}

void Texture::Bind()
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, ID);
    //std::cout << "Texture bound: " << unit << std::endl;
}

void Texture::Unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
    //std::cout << "Texture unbound: " << unit << std::endl;
}

void Texture::Delete()
{
	// delete the handle first
    if (GLAD_GL_ARB_bindless_texture) {
        glMakeTextureHandleNonResidentARB(handle);
	}
    glDeleteTextures(1, &ID);
    //std::cout << "Texture deleted: " << ID << std::endl;
}