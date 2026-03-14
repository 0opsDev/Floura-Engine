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


//float random(vec3 seed) {
//    return fract(sin(dot(seed, vec3(12.9898, 78.233, 45.164))) * 43758.5453);
//}


//float frequency = 50.0;
//float amplitude = 2.0;

void main()
{
    vec4 WorldPos = model * vec4(aPos, 1.0f);

    //float noiseCoordX = WorldPos.x * frequency;
    //float noiseCoordZ = WorldPos.z * frequency;

    //float offset = random(vec3(noiseCoordX, WorldPos.y, noiseCoordZ)) * amplitude;
    //WorldPos.y += offset;
    data_out.WorldPos = vec3(WorldPos);
    
    gl_Position = WorldPos;
    data_out.texCoord = aTex;
    data_out.projection = camMatrix;
    data_out.model = model;
}