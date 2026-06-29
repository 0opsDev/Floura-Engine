#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;
layout (location = 4) in vec3 aTangent;
layout (location = 5) in vec3 aBitangent;

out DATA
{
	vec3 Normal;
    vec3 gNormals;
	vec3 colour;
	vec2 texCoord;
    vec3 Normal0;
    vec3 Tangent0;
    vec3 Bitangent0;
    highp vec3 crntPos;
    highp vec4 currentPos;
    highp vec4 previousPos;
    
} data_out;

uniform mat4 camMatrix;
uniform mat4 previousCamMatrix;
uniform mat4 model;
uniform mat4 previousModel;
uniform mat3 normalMatrix;
uniform vec2 uvScale;

void main()
{
    highp vec4 worldPos = model * vec4(aPos, 1.0f);
    gl_Position = camMatrix * worldPos;
    data_out.currentPos = gl_Position;

    highp vec4 prevWorldPos = previousModel * vec4(aPos, 1.0f);
    data_out.previousPos = previousCamMatrix * prevWorldPos;

    data_out.crntPos = vec3(worldPos);

    data_out.gNormals = normalMatrix * aNormal;

    data_out.Normal0 = normalMatrix * aNormal;
    data_out.Tangent0 = normalMatrix * aTangent;
    data_out.Bitangent0 = normalMatrix * aBitangent;

    vec3 Norm = normalize(normalMatrix * aNormal);
    data_out.Normal = Norm;

    data_out.colour = aColor;
    data_out.texCoord = vec2(aTex.x  * uvScale.x, aTex.y * uvScale.y);
}
/*
vec4 worldPoS = model * vec4(aPos, 1.0f);
//data_out.WorldPos = vec3(worldPoS); 

gl_Position = worldPoS;

vec3 norm = normalize(normalMatrix * aNormal);
data_out.gNormals = norm;
data_out.Normal = norm;
data_out.color = aColor;
data_out.texCoord = vec2(aTex.x  * uvScale.x, aTex.y * uvScale.y);
*/