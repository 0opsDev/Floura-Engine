#include "Texture.h"

Texture::Texture(const char* image, const char* texType, GLuint slot)
{
    std::cout << "Texture loading 1: " << image << std::endl;
    // Assigns the type of the texture to the texture object
    type = texType;

    // Stores the width, height, and the number of color channels of the image
    int widthImg, heightImg, numColCh;
    // Flips the image so it appears right side up
    stbi_set_flip_vertically_on_load(true);
    // Reads the image from a file and stores it in bytes
    unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);

    if (!bytes)
    {
        std::cerr << "Failed to load texture: " << image << std::endl;
        throw std::runtime_error("Failed to load texture: " + std::string(image));
    }

    // Generates an OpenGL texture object
    glGenTextures(1, &ID);
    // Assigns the texture to a Texture Unit
    glActiveTexture(GL_TEXTURE0 + slot);
    unit = slot;
    glBindTexture(GL_TEXTURE_2D, ID);

    // Configures the type of algorithm that is used to make the image smaller or bigger
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Configures the way the texture repeats (if it does at all)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if (numColCh == 4)
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
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
            GL_RGB,
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
    else
        throw std::invalid_argument("Automatic Texture type recognition failed");

    // Generates MipMaps
    glGenerateMipmap(GL_TEXTURE_2D);

    // Deletes the image data as it is already in the OpenGL Texture object
    stbi_image_free(bytes);

    // Unbinds the OpenGL Texture object so that it can't accidentally be modified
    glBindTexture(GL_TEXTURE_2D, 0);

    std::cout << "Texture loaded 2: " << image << std::endl;
}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)
{
    // Gets the location of the uniform
    GLuint texUni = glGetUniformLocation(shader.ID, uniform);
    if (texUni == -1)
    {
        std::cerr << "Failed to find uniform: " << uniform << std::endl;
    }
    // Shader needs to be activated before changing the value of a uniform
    shader.Activate();
    // Sets the value of the uniform
    glUniform1i(texUni, unit);
}

void Texture::Bind()
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, ID);
    std::cout << "Texture bound: " << unit << std::endl;
}

void Texture::Unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
    std::cout << "Texture unbound: " << unit << std::endl;
}

void Texture::Delete()
{
    glDeleteTextures(1, &ID);
    std::cout << "Texture deleted: " << ID << std::endl;
}