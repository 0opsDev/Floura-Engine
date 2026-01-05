#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 3) in vec2 aTex; // UV
//out mat3 TBN;

uniform mat4 camMatrix;
uniform mat4 model; // Final model matrix combining all transformations
uniform vec3 camPos;

out vec3 crntPos;
out vec3 Colour;
out vec2 texCoord;

float random(vec3 seed) {
	vec4 seed4 = vec4(seed, 1.0);
	float dot_product = dot(seed4, vec4(12.9898, 78.233, 45.164, 94.673));
	return fract(sin(dot_product) * 43758.5453);
}

void main()
{
    crntPos = vec3(model * vec4(aPos, 1.0f));

    Colour = vec3( random(vec3(aPos.x, 1.0f, 1.0f ) ), random(vec3(1.0f, aPos.y, 1.0f) ) , random(vec3(1.0f, 1.0f, aPos.z) ) );
    texCoord = aTex;

    gl_Position = camMatrix * vec4(crntPos, 1.0f);
}