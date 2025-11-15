#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;
layout (location = 4) in vec3 aTangent;
layout (location = 5) in vec3 aBitangent;

uniform mat4 camMatrix;
uniform mat4 model; // Final model matrix combining all transformations
uniform float deltatime;
uniform float time;
float speed = 1.0f;
float frequency = 1.0f; // Frequency of the wave
float amplitude = 0.01f; // Amplitude of the wave

uniform mat4 lightProjection;
uniform mat3 normalMatrix; 
uniform vec2 uvScale;   

out vec3 crntPos;
out vec3 Normal;
out vec3 color;
out vec2 texCoord;
out vec4 fragPosLight;

out vec3 Normal0;
out vec3 Tangent0;
out vec3 Bitangent0;

void main()
{
    crntPos = vec3(model * vec4(aPos, 1.0f));

    Normal0 = normalMatrix * aNormal;
    Tangent0 = normalMatrix * aTangent;
    Bitangent0 = normalMatrix * aBitangent;


    vec3 Norm = normalize(normalMatrix * aNormal);
    Normal = Norm;

    color = aColor;

    texCoord = vec2(aTex.x  * uvScale.x, aTex.y * uvScale.y);
    fragPosLight = lightProjection * vec4(crntPos, 1.0f);



    float wave_offset = sin(crntPos.x * frequency + time * speed) * amplitude;

    crntPos.x += wave_offset;
    crntPos.y += wave_offset;
    crntPos.z += wave_offset;

    gl_Position = camMatrix * vec4(crntPos, 1.0f);

}