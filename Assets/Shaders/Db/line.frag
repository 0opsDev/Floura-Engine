#version 460 core

// Outputs colors in RGBA
out vec4 FragColor;

in vec3 colour;

void main()
{
	FragColor = vec4(colour.x, colour.y, colour.z, 1.0f);
}