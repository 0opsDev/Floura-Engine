#version 460 core

// Outputs colors in RGBA
out vec4 FragColor;
// Imports the normal from the Vertex Shader
in vec3 Normal;
// Imports the color from the Vertex Shader
in vec3 color;
// Imports the texture coordinates from the Vertex Shader
in vec2 texCoord;

// Gets the Texture Units from the main function
uniform sampler2D diffuse0;

void main()
{
	vec4 diffuseTex = texture(diffuse0, texCoord);
	FragColor = vec4(diffuseTex.xyz, 1.0f);
}