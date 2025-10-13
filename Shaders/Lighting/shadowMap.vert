#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 3) in vec2 aTex;

out vec2 texCoord;

uniform mat4 lightProjection;
uniform mat4 model;

void main()
{
    texCoord = aTex;
    gl_Position = lightProjection * model * vec4(aPos, 1.0);
}