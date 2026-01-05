#version 460 core

// vertex
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;
layout (location = 4) in vec3 aTangent;
layout (location = 5) in vec3 aBitangent;

//out vec4 fragPosLight;

uniform mat4 camMatrix;
uniform mat4 model;
uniform mat4 lightProjection;
// Gets the position of the camera from the main function
//uniform vec3 camPos;
//uniform vec3 directLightPos; //1.0f, 1.0f, 0.0f
uniform mat3 normalMatrix;
uniform vec2 uvScale; 

out DATA
{
	vec3 Normal;
	vec3 color;
	vec2 texCoord;
	mat4 projection;
    mat4 model;
    vec3 WorldPos;
    vec4 fragPosLight;
    vec3 Normal0;
    vec3 Tangent0;
    vec3 Bitangent0;
    //mat3 normalMatrix;
   //mat3 TBN;
    //vec3 camPos;
//   vec3 directLightPos;
} data_out;

void main()
{    


    vec4 worldPoS = model * vec4(aPos, 1.0f);
    data_out.WorldPos = vec3(worldPoS);

    data_out.fragPosLight = lightProjection * vec4(vec3(model * vec4(aPos, 1.0f)), 1.0f);

    gl_Position = worldPoS;

    data_out.Normal0 = normalMatrix * aNormal;
    data_out.Tangent0 = normalMatrix * aTangent;
    data_out.Bitangent0 = normalMatrix * aBitangent;


    vec3 Norm = normalize(normalMatrix * aNormal);
    data_out.Normal = Norm;
    data_out.color = aColor;
    data_out.texCoord = vec2(aTex.x  * uvScale.x, aTex.y * uvScale.y);
    data_out.projection = camMatrix;
    data_out.model = model;
    //data_out.camPos = camPos;
    //data_out.directLightPos = directLightPos;
}