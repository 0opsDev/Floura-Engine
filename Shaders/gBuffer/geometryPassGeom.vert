#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;

out DATA
{
	vec3 Normal;
    vec3 gNormals;
	vec3 color;
	vec2 texCoord;
	mat4 projection;
    vec3 WorldPos;
} data_out;

uniform mat4 camMatrix;
uniform mat4 model;
uniform mat3 normalMatrix;

void main()
{
    vec4 worldPoS = model * vec4(aPos, 1.0f);
    data_out.WorldPos = vec3(worldPoS); 

    gl_Position = worldPoS;

    vec3 norm = normalize(normalMatrix * aNormal);
    data_out.gNormals = norm;
    data_out.Normal = norm;
    data_out.color = aColor;
    data_out.texCoord = aTex;
    data_out.projection = camMatrix;

}