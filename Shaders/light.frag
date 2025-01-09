#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;

uniform vec4 lightColor;
//vec4(1.0f, 1.0f, 1.0f, 1.0f)
void main()
{
	FragColor = lightColor;
}
