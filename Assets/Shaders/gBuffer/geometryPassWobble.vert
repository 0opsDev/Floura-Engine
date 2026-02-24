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
out vec3 color;
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

uniform float deltatime;
uniform float time;
uniform float priorTime;
float speed = 3f;
float frequency = 2.0f; // Frequency of the wave
float amplitude = 0.01f; // Amplitude of the wave

// priorTime

void main()
{
    highp vec4 worldPos = model * vec4(aPos, 1.0f);
    highp vec4 prevWorldPos = previousModel * vec4(aPos, 1.0f);

    float Cwave_offset = sin(worldPos.x * frequency + time * speed) * amplitude;
    float Pwave_offset = sin(prevWorldPos.x * frequency + priorTime * speed) * amplitude; // need previous time

    worldPos += vec4(vec3(Cwave_offset, 0.0, -Cwave_offset), 0.0);
    prevWorldPos += vec4(vec3(Pwave_offset, 0.0, -Pwave_offset), 0.0);
    
    currentPos = camMatrix * worldPos;
    previousPos = previousCamMatrix * prevWorldPos;
    
    crntPos = vec3(worldPos);

    
    gNormals = normalMatrix * aNormal;

    Normal0 = normalMatrix * aNormal;
    Tangent0 = normalMatrix * aTangent;
    Bitangent0 = normalMatrix * aBitangent;

    vec3 Norm = normalize(normalMatrix * aNormal);
    Normal = Norm;

    color = aColor;
    texCoord = vec2(aTex.x  * uvScale.x, aTex.y * uvScale.y);

    gl_Position = camMatrix * vec4(crntPos, 1.0f);
}