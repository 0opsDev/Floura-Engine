#include "noise.h"
#define STB_PERLIN_IMPLEMENTATION
#include <stb/stb_perlin.h>

void NoiseH::generateNoise(GLuint& texture, int width, int height, float scale, int seed) {
    std::vector<float> noiseData(width * height * 3);
    glDeleteTextures(1, &texture); // Delete the texture if it already exists

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = (y * width + x) * 3;
            float fx = static_cast<float>(x) * scale;
            float fy = static_cast<float>(y) * scale;

            // seed-based noise function
            noiseData[index] = stb_perlin_noise3_seed(fx, fy, 0.0f, 256, 256, 256, seed);
            noiseData[index + 1] = stb_perlin_noise3_seed(fx * 1.5f, fy * 1.5f, 0.0f, 256, 256, 256, seed);
            noiseData[index + 2] = stb_perlin_noise3_seed(fx * 2.0f, fy * 2.0f, 0.0f, 256, 256, 256, seed);
        }
    }

    // Generate and bind the texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, noiseData.data());

    // Set texture parameters for wrapping and filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	noiseData.clear(); // Clear the vector to free memory
}