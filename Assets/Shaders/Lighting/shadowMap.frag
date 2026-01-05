#version 460 core

in vec2 texCoord;

uniform sampler2D diffuse0;
void main()
{

	float diffuseTex = texture(diffuse0, texCoord).a;

	if (diffuseTex < 0.1)
	discard;

}