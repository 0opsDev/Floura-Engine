#version 460 core

// vertex
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;
//layout (location = 4) in vec3 aTangent;

//out vec4 fragPosLight;

out DATA
{
	vec3 Normal;
	vec3 color;
	vec2 texCoord;
	mat4 projection;
    mat4 model;
    vec3 WorldPos;
    vec4 fragPosLight;
    //mat3 normalMatrix;
   //mat3 TBN;
    //vec3 camPos;
//   vec3 directLightPos;
} data_out;

uniform mat4 camMatrix;
uniform mat4 model;
uniform mat4 lightProjection;
// Gets the position of the camera from the main function
//uniform vec3 camPos;
//uniform vec3 directLightPos; //1.0f, 1.0f, 0.0f
uniform mat3 normalMatrix;

void main()
{    


    vec4 worldPoS = model * vec4(aPos, 1.0f);
    data_out.WorldPos = vec3(worldPoS);

    data_out.fragPosLight = lightProjection * vec4(vec3(model * vec4(aPos, 1.0f)), 1.0f);

    gl_Position = worldPoS;

    //mat3 normalMatrix = transpose(inverse(mat3(model)));

    //data_out.Normal = normalize(normalMatrix * aNormal);

    //vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);

    //vec3 B = normalize(cross(N, T));
    //data_out.TBN = mat3(T, B, N);
    data_out.Normal = N;
    data_out.color = aColor;
    data_out.texCoord = aTex; //  mat2(0.0, -1.0, 1.0, 0.0) * aTex;
    data_out.projection = camMatrix;
    data_out.model = model;
    //data_out.camPos = camPos;
    //data_out.directLightPos = directLightPos;
}