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
float speed = 2.0f;
float frequency = 1.0f; // Frequency of the wave
float amplitude = 0.5f; // Amplitude of the wave

float smallspeed = 5.0f;
float smallfrequency = 5.0f; // Frequency of the wave
float smallamplitude = 0.05f; // Amplitude of the wave

// priorTime

float random(vec3 seed) {

    vec4 seed4 = vec4(seed, 1.0);
    float dot_product = dot(seed4, vec4(12.9898, 78.233, 45.164, 94.673));
    return fract(sin(dot_product) * 43758.5453);
}

void main()
{
    highp vec4 worldPos = model * vec4(aPos, 1.0f);
    highp vec4 prevWorldPos = previousModel * vec4(aPos, 1.0f);

    float Cwave_offsetx = sin(worldPos.x * frequency + time * speed) * amplitude;
    float Pwave_offsetx = sin(prevWorldPos.x * frequency + priorTime * speed) * amplitude; // need previous time
    float Cwave_offsetz = sin(worldPos.z * frequency + time * speed) * amplitude;
    float Pwave_offsetz = sin(prevWorldPos.z * frequency + priorTime * speed) * amplitude; // need previous time

    float SCwave_offsetx = sin(worldPos.x * smallfrequency + time * smallspeed) * smallamplitude;
    float SPwave_offsetx = sin(prevWorldPos.x * smallfrequency + priorTime * smallspeed) * smallamplitude; // need previous time
    float SCwave_offsetz = sin(worldPos.z * smallfrequency + time * smallspeed) * smallamplitude;
    float SPwave_offsetz = sin(prevWorldPos.z * smallfrequency + priorTime * smallspeed) * smallamplitude; // need previous time

    float Cwave_offsetT = Cwave_offsetx+ Cwave_offsetz;
    float Pwave_offsetT = Pwave_offsetx+ Pwave_offsetz;

    float SCwave_offsetT = -SCwave_offsetx+ -SCwave_offsetz;
    float SPwave_offsetT = -SPwave_offsetx+ -SPwave_offsetz;

    vec3 cRandomoffset = vec3(random(worldPos.xyz), random(worldPos.xyz + 1), random(worldPos.xyz + 2)) * 0.5;
    vec3 pRandomoffset = vec3(random(prevWorldPos.xyz), random(prevWorldPos.xyz + 1), random(prevWorldPos.xyz + 2)) * 0.5;

    vec3 scRandomoffset = vec3(random(worldPos.xyz + 3), random(worldPos.xyz  + 4), random(worldPos.xyz  + 5)) * 1.0;
    vec3 spRandomoffset = vec3(random(prevWorldPos.xyz +3), random(prevWorldPos.xyz + 4), random(prevWorldPos.xyz + 5)) * 1.0;
    
    vec3 M = vec3(0.5, 1.5, 0.5);
    vec3 S =  vec3(0.5, 2.0, 0.5);
    
    vec3 Cfinal_offset = vec3(Cwave_offsetT * M) * cRandomoffset + vec3(SCwave_offsetT * S) * scRandomoffset;
    vec3 Pfinal_offset = vec3(Pwave_offsetT * M) * pRandomoffset + vec3(SPwave_offsetT * S) * spRandomoffset;
    
    float scaler = 0.3;
    
    worldPos += vec4(Cfinal_offset, 0.0) * scaler;
    prevWorldPos += vec4(Pfinal_offset, 0.0) * scaler;
    
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