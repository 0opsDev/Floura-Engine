#version 460 core

#extension GL_ARB_gpu_shader_int64 : enable
#extension GL_ARB_bindless_texture : require

// Outputs colors in RGBA
out vec4 FragColor;

// Imports the current position from the Vertex Shader
in vec3 crntPos;
// Imports the normal from the Vertex Shader
in vec3 Normal;
// Imports the color from the Vertex Shader
in vec3 color;
// Imports the texture coordinates from the Vertex Shader
in vec2 texCoord;

in vec3 weightColour;

in vec3 camPositon;

//TBN
in vec3 Normal0;
in vec3 Tangent0;
in vec3 Bitangent0;


uniform float deltatime;
uniform float time;
uniform int frame;


void main()
{
	FragColor = vec4(weightColour,1.0);
}