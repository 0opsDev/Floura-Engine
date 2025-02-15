#version 330 core

// Positions/Coordinates
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 camMatrix;
void main()
{
	// Outputs the positions/coordinates of all vertices
	gl_Position = camMatrix * model * vec4(aPos, 1.0f);
}