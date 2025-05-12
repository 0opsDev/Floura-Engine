#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;

out vec3 crntPos;
out vec3 Normal;
out vec3 color;
out vec2 texCoord;

uniform mat4 camMatrix;  // Camera view-projection matrix
uniform mat4 model;      // Final model matrix (computed in C++)
uniform mat4 normalMatrix; // To properly transform normals

void main()
{
    // Apply model transformation to vertex position
    crntPos = vec3(model * vec4(aPos, 1.0));

    // Transform normal using normal matrix (to handle scaling properly)
    Normal = mat3(normalMatrix) * aNormal;

    // Pass through color and texture coordinates
    color = aColor;
    texCoord = aTex;

    // Compute final vertex position in clip space
    gl_Position = camMatrix * vec4(crntPos, 1.0);
}