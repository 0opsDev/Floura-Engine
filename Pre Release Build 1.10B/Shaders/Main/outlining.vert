#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 camMatrix;
uniform mat4 model;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;
uniform float outlining;

void main()
{
    //vec3 crntPos = vec3(model * translation * rotation * scale * outlining * vec4(aPos, 1.0f) );
    vec3 crntPos = vec3(model * translation * rotation * scale * vec4(aPos + aNormal * outlining, 1.0f) );
    gl_Position = camMatrix * vec4(crntPos, 1.0);
}