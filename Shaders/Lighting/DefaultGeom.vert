#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;

//out vec4 fragPosLight;

out DATA
{
	vec3 Normal;
	vec3 color;
	vec2 texCoord;
	mat4 projection;
    vec3 WorldPos;
    vec4 fragPosLight;
} data_out;

uniform mat4 camMatrix;
uniform mat4 model;
uniform mat4 lightProjection;

void main()
{    

    vec4 worldPoS = model * vec4(aPos, 1.0f);
    data_out.WorldPos = vec3(worldPoS);

    data_out.fragPosLight = lightProjection * vec4(vec3(model * vec4(aPos, 1.0f)), 1.0f);

    gl_Position = worldPoS;
    data_out.Normal = aNormal;
    data_out.color = aColor;
    data_out.texCoord = aTex; //  mat2(0.0, -1.0, 1.0, 0.0) * aTex;
    data_out.projection = camMatrix;
}