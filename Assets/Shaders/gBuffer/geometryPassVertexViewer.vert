#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;
layout (location = 4) in vec3 aTangent;
layout (location = 5) in vec3 aBitangent;

out highp vec3 crntPos;
out vec3 Normal;
out vec3 gNormals;
out vec3 colour;
out vec2 texCoord;

uniform mat4 camMatrix;
uniform mat4 previousCamMatrix;
uniform mat4 model;
uniform mat4 previousModel;
uniform mat3 normalMatrix;
uniform vec2 uvScale;

//TBN
out vec3 Normal0;
out vec3 Tangent0;
out vec3 Bitangent0;

out highp vec4 currentPos;
out highp vec4 previousPos;

float random(vec3 seed) {
    vec4 seed4 = vec4(seed, 1.0);
    float dot_product = dot(seed4, vec4(12.9898, 78.233, 45.164, 94.673));
    return fract(sin(dot_product) * 43758.5453);
}

void main()
{
    highp vec4 worldPos = model * vec4(aPos, 1.0f);
    gl_Position = camMatrix * worldPos;
    currentPos = gl_Position;

    highp vec4 prevWorldPos = previousModel * vec4(aPos, 1.0f);
    previousPos = previousCamMatrix * prevWorldPos;
    
    crntPos = vec3(worldPos);
    
    gNormals = normalMatrix * aNormal;

    Normal0 = normalMatrix * aNormal;
    Tangent0 = normalMatrix * aTangent;
    Bitangent0 = normalMatrix * aBitangent;

    vec3 Norm = normalize(normalMatrix * aNormal);
    Normal = Norm;

    colour = vec3( random(vec3(aPos.x, 1.0f, 1.0f ) ), random(vec3(1.0f, aPos.y, 1.0f) ) , random(vec3(1.0f, 1.0f, aPos.z) ) );
    
    texCoord = vec2(aTex.x  * uvScale.x, aTex.y * uvScale.y);
}