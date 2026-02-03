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


uniform uint64_t texture_roughness_Handle;


void main()
{
	sampler2D Sampler = sampler2D(texture_roughness_Handle);
	FragColor = texture(Sampler, texCoord);
}