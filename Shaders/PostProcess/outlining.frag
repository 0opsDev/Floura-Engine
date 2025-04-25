#version 460 core

uniform vec4 stencilColor;

out vec4 FragColor;

void main(){
// RETURN BASIC COLOUR
	FragColor = vec4(stencilColor);
}