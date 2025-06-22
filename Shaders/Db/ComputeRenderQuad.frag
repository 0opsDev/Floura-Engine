#version 460 core

out vec4 FragColor;
uniform sampler2D screen;
in vec2 texCoord;
void main()
{
	FragColor = texture(screen, texCoord);
}