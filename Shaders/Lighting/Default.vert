#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;

out vec3 crntPos;
out vec3 Normal;
out vec3 color;
out vec2 texCoord;
out vec4 fragPosLight;

uniform mat4 camMatrix;
uniform mat4 model; // Final model matrix combining all transformations
uniform mat4 lightProjection;

void main()
{
    crntPos = vec3(model * vec4(aPos, 1.0f));
    Normal = aNormal;
    color = aColor;
    texCoord = aTex;
    fragPosLight = lightProjection * vec4(crntPos, 1.0f);

    gl_Position = camMatrix * vec4(crntPos, 1.0f);
}