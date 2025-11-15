#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 lineMatrix;
uniform vec3 aColour;
uniform mat4 camMatrix;


out vec3 colour;
out vec3 crntPos;


void main()
{
    crntPos = vec3(lineMatrix * vec4(aPos, 1.0f));

    //colour = vec3(1.0f);
    colour = aColour;

    gl_Position = camMatrix * vec4(crntPos, 1.0f);
}