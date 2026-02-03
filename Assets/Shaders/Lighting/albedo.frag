#version 460 core
#extension GL_ARB_gpu_shader_int64 : enable
#extension GL_ARB_bindless_texture : require

// Outputs colors in RGBA
out vec4 FragColor;
// Imports the normal from the Vertex Shader
in vec3 Normal;
// Imports the color from the Vertex Shader
in vec3 color;
// Imports the texture coordinates from the Vertex Shader
in vec2 texCoord;

// Gets the Texture Units from the main function
uniform sampler2D texture_diffuse;


vec4 lights(){
	vec4 diffuseTex = texture(texture_diffuse, texCoord);
	vec4 finalColour = vec4(0.0);
		return (diffuseTex) + finalColour;
} 

void main()
{
	vec4 light = lights();
	FragColor = vec4(light.xyz, 1.0f);
}