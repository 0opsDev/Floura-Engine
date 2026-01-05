#version 460 core

// Outputs colors in RGBA
out vec4 FragColor;
// Imports the color from the Vertex Shader
in vec3 Colour;
// Imports the texture coordinates from the Vertex Shader
in vec2 texCoord;

void main()
{
	FragColor = vec4(Colour, 1.0f);
}