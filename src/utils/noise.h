#ifndef NOISE_H
#define NOISE_H
#include <glad/glad.h>
#include <vector>

class NoiseH
{
public:

	static void generateNoise(GLuint& texture, int width, int height, float scale, int seed);

};

#endif