#version 460 core

in vec2 texCoord;

uniform sampler2D diffuse0;
void main()
{

	vec4 diffuseTex = texture(diffuse0, texCoord);

	if (diffuseTex.a < 0.1)
	discard;

}