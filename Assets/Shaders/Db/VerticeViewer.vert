#version 460 core

// vertex
layout (location = 0) in vec3 aPos;
layout (location = 3) in vec2 aTex;


uniform mat4 camMatrix;
uniform mat4 model;

out DATA
{
	vec2 texCoord;
	mat4 projection;
    mat4 model;
    vec3 WorldPos;
} data_out;

void main()
{    


    vec4 worldPoS = model * vec4(aPos, 1.0f);
    data_out.WorldPos = vec3(worldPoS);

    gl_Position = worldPoS;
    data_out.texCoord = aTex;
    data_out.projection = camMatrix;
    data_out.model = model;
}