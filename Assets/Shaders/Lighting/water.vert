#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex; // UV
layout (location = 4) in vec3 aTangent;
layout (location = 5) in vec3 aBitangent;
//out mat3 TBN;

uniform mat4 camMatrix;
uniform mat4 model; // Final model matrix combining all transformations
uniform mat4 lightProjection;
uniform mat3 normalMatrix;
uniform vec2 uvScale; 
uniform vec3 camPos;

out vec3 crntPos;
out vec3 Normal;
out vec3 color;
out vec2 texCoord;
out vec4 fragPosLight;

//TBN
out vec3 Normal0;
out vec3 Tangent0;
out vec3 Bitangent0;

// cubemap
out vec3 reflectedVector;
out vec3 camPositon;
out vec3 NviewVector;


uniform float deltatime;
uniform float time;

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
    crntPos = vec3(model * vec4(aPos, 1.0f));

    float Cwave_offsetx = sin(crntPos.x * frequency + time * speed) * amplitude;
    float Cwave_offsetz = sin(crntPos.z * frequency + time * speed) * amplitude;

    float SCwave_offsetx = sin(crntPos.x * smallfrequency + time * smallspeed) * smallamplitude;
    float SCwave_offsetz = sin(crntPos.z * smallfrequency + time * smallspeed) * smallamplitude;

    float Cwave_offsetT = Cwave_offsetx+ Cwave_offsetz;

    float SCwave_offsetT = -SCwave_offsetx+ -SCwave_offsetz;

    vec3 cRandomoffset = vec3(random(crntPos.xyz), random(crntPos.xyz + 1), random(crntPos.xyz + 2)) * 0.5;

    vec3 scRandomoffset = vec3(random(crntPos.xyz + 3), random(crntPos.xyz  + 4), random(crntPos.xyz  + 5)) * 1.0;

    vec3 M = vec3(0.5, 1.5, 0.5);
    vec3 S =  vec3(0.5, 2.0, 0.5);

    vec3 Cfinal_offset = vec3(Cwave_offsetT * M) * cRandomoffset + vec3(SCwave_offsetT * S) * scRandomoffset;

    float scaler = 0.3;

    crntPos += vec3(Cfinal_offset) * scaler;

    //crntPos += vec3(wave_offset, 0.0, -wave_offset);
    
    Normal0 = normalMatrix * aNormal;
    Tangent0 = normalMatrix * aTangent;
    Bitangent0 = normalMatrix * aBitangent;


    vec3 Norm = normalize(normalMatrix * aNormal);
    Normal = Norm;
    color = aColor;
    texCoord = vec2(aTex.x  * uvScale.x, aTex.y * uvScale.y);
    fragPosLight = lightProjection * vec4(crntPos, 1.0f);

    vec3 viewVector = crntPos.xyz - camPos;
    NviewVector = viewVector;
    reflectedVector = reflect(viewVector, Norm);

    camPositon = camPos;

    gl_Position = camMatrix * vec4(crntPos, 1.0f);
}